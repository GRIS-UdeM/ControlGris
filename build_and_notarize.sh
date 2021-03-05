#!/bin/bash

if [ $# -ne 1 ];then
	echo "You must give the notarizing account app-specific password as an argument."
	exit 1
fi

signUser="Developer ID Application: Samuel Beland (Q2A837SX87)"
notarizeUser="samuel.beland@gmail.com"
password="$1"
identifier="ca.umontreal.musique.gris.controlgris"

ZIP_NAME="plugins.zip"

PROJECT_PATH="`pwd`"
PROJECT_FILE="$PROJECT_PATH/ControlGris.jucer"
XCODE_PATH="$PROJECT_PATH/Builds/MacOSX"
BIN_PATH="$XCODE_PATH/build/Release/"
TEMP_PATH="$PROJECT_PATH/out"

function get_app_version() {
	Projucer --get-version "$PROJECT_FILE" || exit 1
}

function run_projucer() {
	echo "Running Projucer..."
	Projucer --resave "$PROJECT_FILE" || exit 1
}

function build() {
	cd "$XCODE_PATH"
	echo "Building ControlGris $VERSION"
	xcodebuild -configuration Release -quiet || exit 1
}

function copy_to_temp() {
	cd "$BIN_PATH"
	mkdir "$TEMP_PATH"
	echo "Copying plugins..."
	for filename in *.vst; do
		cp -r "$filename" "$TEMP_PATH"
	done
	for filename in *.vst3; do
		cp -r "$filename" "$TEMP_PATH"
	done
	for filename in *.component; do
		cp -r "$filename" "$TEMP_PATH"
	done
}

function sign() {
	cd "$TEMP_PATH"
	echo "Signing plugins..."
	for filename in *; do
		codesign -s "$signUser" -v "$filename" --options=runtime --timestamp
	done
}

function send_for_notarisation() {
	cd "$TEMP_PATH"
	zip -r "$ZIP_NAME" *
	echo "Sending to notarization authority..."
	xcrun altool --notarize-app --primary-bundle-id "$identifier" -u "$notarizeUser" -p "$password" --file "$ZIP_NAME"
	rm "$ZIP_NAME"
}

function get_last_request_uuid() {
	history=`xcrun altool --notarization-history 0 -u "$notarizeUser" -p "$password"`
	echo "$history" | head -n 6 | tail -n 1 | cut -d' ' -f 4
}

function wait_a_bit() {
	echo "waiting a bit..."
	sleep 10
}

function wait_for_notarization() {
	WAITING=" in progress"
	SUCCESS=" success"
	uuid=`get_last_request_uuid`
	status="$WAITING"
	while [[ "$status" == "$WAITING" ]];do
		history=`xcrun altool --notarization-info "$uuid" -u "$notarizeUser" -p "$password"`
		status=`echo "$history" | grep Status | head -n 1 | cut -d: -f 2`
	done
	echo "status is $status"
	if [[ "$status" != "$SUCCESS" ]];then
		echo "Error : notarization was refused"
		exit 1
	fi
}

function staple() {
	cd "$TEMP_PATH"
	echo "Rubber stamping plugins..."
	for filename in *;do
		xcrun stapler staple "$filename" || exit 1
	done
}

function sign_aax() {
	cd "$BIN_PATH"
	pluginPath=`echo *.aaxplugin`
	cp -r "$pluginPath" "$TEMP_PATH"
	cd "$TEMP_PATH"
	wraptool sign --verbose --signid "$signUser" --account grisresearch --wcguid A4B35290-7C9C-11EB-8B4D-00505692C25A --in "$pluginPath" --autoinstall on --extrasigningoptions "--timestamp --options runtime" || exit 1
	# wraptool sign --verbose --signid "$signUser" --account grisresearch --wcguid A4B35290-7C9C-11EB-8B4D-00505692C25A --in "$aaxPath" --autoinstall on --notarize-username "$signUser" --notarize-password "$password" --extrasigningoptions "--timestamp --options runtime" || exit 1
}

function bundle() {
	cd "$TEMP_PATH"
	zip -r "../ControlGRIS_MacOS_x64_$VERSION.zip" *
	cd ..
	rm -r "$TEMP_PATH"
}

VERSION=`get_app_version`
run_projucer
build
copy_to_temp
sign
send_for_notarisation
wait_a_bit
wait_for_notarization
staple
sign_aax
bundle

echo "Done !"

# xcrun altool --notarization-info "f82e93e4-aaaf-4990-ad04-fdb222ececc2" -u "samuel.beland@gmail.com"