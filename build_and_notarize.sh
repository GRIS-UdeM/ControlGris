#!/bin/bash

if [ $# -ne 1 ];then
	echo "You must provide the notarizing account's password as an argument."
	exit 1
fi

export LINE=":::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::"

export INSTALLER_SIGNATURE="Developer ID Installer: Samuel Beland (Q2A837SX87)"
export APP_SIGNATURE="Developer ID Application: Samuel Beland (Q2A837SX87)"
export NOTARIZE_USER="samuel.beland@gmail.com"
export PASS="$1"
export IDENTIFIER="ca.umontreal.musique.gris.controlgris.pkg"

export PROJECT_PATH="`pwd`"
export PROJECT_FILE="$PROJECT_PATH/ControlGris.jucer"
export XCODE_PATH="$PROJECT_PATH/Builds/MacOSX"
export BIN_PATH="$XCODE_PATH/build/Release/"
export TEMP_PATH="$PROJECT_PATH/out"
export PLIST_PATH="$PROJECT_PATH/Application.plist"
export ZIP_PATH="$TEMP_PATH/plugins.zip"

#==============================================================================
# get app version
export VERSION=`Projucer --get-version "$PROJECT_FILE"`
echo -e "$LINE\nVersion is $VERSION"
export PKG_PATH="$TEMP_PATH/ControlGris_$VERSION.pkg"

#==============================================================================
function generate_project() {
	echo "Generating project files..."
	Projucer --resave "$PROJECT_FILE" || exit 1
	cd "$PROJECT_PATH"
}

#==============================================================================
function build() {
	cd "$XCODE_PATH"
	echo -e "$LINE\nBuilding ControlGris $VERSION"
	chmod -R 755 .
	xcodebuild -configuration Release || exit 1
}

#==============================================================================
function copy_to_temp() {
	echo -e "$LINE\nCopying plugins..."
	cd "$BIN_PATH" || exit 1
	rm -fr "$TEMP_PATH"
	mkdir "$TEMP_PATH"
	for filename in *.vst; do
		cp -R -H "$filename" "$TEMP_PATH"
	done
	for filename in *.vst3; do
		cp -R -H "$filename" "$TEMP_PATH"
	done
	for filename in *.component; do
		cp -R -H "$filename" "$TEMP_PATH"
	done
}

#==============================================================================
function sign() {
	echo -e "$LINE\nSigning plugins..."
	cd "$TEMP_PATH"
	for filename in *; do
		# wraptool sign \
		# 	 --verbose \
		# 	 --signid "$APP_SIGNATURE" \
		# 	 --account grisresearch \
		# 	 --wcguid A4B35290-7C9C-11EB-8B4D-00505692C25A \
		# 	 --in "$filename" \
		# 	 --out "$filename" \
		# 	 --autoinstall on \
		# 	 --dsigharden \
		# 	 --extrasigningoptions "--timestamp --options runtime" \
		# 	 || exit 1
		codesign \
			-s "$APP_SIGNATURE" \
			-v "$filename" \
			--options=runtime \
			--timestamp \
			|| exit 1
	done
}

#==============================================================================
function sign_aax() {
	echo -e "$LINE\nSigning aax plugin..."
	cd "$BIN_PATH"
	PLUGIN_NAME=`echo *.aaxplugin`
	IN_PATH="$TEMP_PATH/unsigned_$PLUGIN_NAME"
	OUT_PATH="$TEMP_PATH/$PLUGIN_NAME"
	cp -R -H "$PLUGIN_NAME" "$IN_PATH" || exit 1

	wraptool sign \
			 --verbose \
			 --signid "$APP_SIGNATURE" \
			 --account grisresearch \
			 --wcguid A4B35290-7C9C-11EB-8B4D-00505692C25A \
			 --in "$IN_PATH" \
			 --out "$OUT_PATH" \
			 --autoinstall on \
			 --dsigharden \
			 --extrasigningoptions "--timestamp --options runtime" \
			 || exit 1
			 # --notarize-username "$NOTARIZE_USER" \
			 # --notarize-password "$PASS" \
	rm -fr "$IN_PATH" || exit 1
}

#==============================================================================
function build_tree() {
	echo -e "$LINE\nBuilding directory structure..."
	cd $TEMP_PATH
	BASE_PATH="Product/Library/Audio/Plug-Ins"
	AU_PATH="$BASE_PATH/Components"
	VST_PATH="$BASE_PATH/VST"
	VST3_PATH="$BASE_PATH/VST3"
	AAX_PATH="Product/Library/Application Support/Avid/Audio/Plug-Ins"

	mkdir -p "$AU_PATH" || exit 1
	mkdir -p "$VST_PATH" || exit 1
	mkdir -p "$VST3_PATH" || exit 1
	mkdir -p "$AAX_PATH" || exit 1

	cp -R -H *.component "$AU_PATH" || exit 1
	cp -R -H *.vst "$VST_PATH" || exit 1
	cp -R -H *.vst3 "$VST3_PATH" || exit 1
	cp -R -H *.aaxplugin "$AAX_PATH" || exit 1
}

#==============================================================================
function package() {
	echo -e "$LINE\nBuilding package..."
	cd $TEMP_PATH

	export IN_PATH="$PKG_PATH.unsigned"
	export OUT_PATH="$PKG_PATH"

	pkgbuild    --root "Product" \
	            --install-location "/" \
	            --identifier "$IDENTIFIER" \
	            --version "$VERSION" \
	            --sign "$INSTALLER_SIGNATURE" \
	            --timestamp \
	            "$PKG_PATH" || exit 1
}

#==============================================================================
function notarize_pkg()
{
	echo -e "$LINE\nNotarizing pkg..."

	cd "$TEMP_PATH" || exit 1

	# xcrun altool \
	# 	--notarize-app \
	# 	--primary-bundle-id "$IDENTIFIER" \
	# 	-u "$NOTARIZE_USER" \
	# 	-p "$PASS" \
	# 	--file "$PKG_PATH" \
	# 	|| exit 1
	# wait_for_notarization
	# xcrun stapler staple "$PKG_PATH" || exit 1

	/Applications/PACEAntiPiracy/Eden/Fusion/Current/scripts/notarize.py \
		--file "$PKG_PATH" \
		--username "$NOTARIZE_USER" \
		--password "$PASS" \
		--primary-bundle-id "$IDENTIFIER" \
		|| exit 1
}

#==============================================================================

#==============================================================================
function send_for_notarisation() {
	echo -e "$LINE\nSending to notarization authority..."
	cd "$TEMP_PATH"
	# zip -r "$ZIP_PATH" *
	ditto -c -k --sequesterRsrc --keepParent . "$ZIP_PATH"
	xcrun altool --notarize-app --primary-bundle-id "$IDENTIFIER" -u "$NOTARIZE_USER" -p "$PASS" --file "$ZIP_PATH"
	rm "$ZIP_PATH"
}

#==============================================================================
function get_last_request_uuid() {
	history=`xcrun altool --notarization-history 0 -u "$NOTARIZE_USER" -p "$PASS"`
	echo "$history" | head -n 6 | tail -n 1 | cut -d' ' -f 4
}

#==============================================================================
function wait_for_notarization() {
	echo -e "$LINE\nChecking for notarization success..."
	echo "waiting a bit..."
	sleep 30
	WAITING=" in progress"
	SUCCESS=" success"
	uuid=`get_last_request_uuid`
	status="$WAITING"
	while [[ "$status" == "$WAITING" ]];do
		sleep 10
		history=`xcrun altool --notarization-info "$uuid" -u "$NOTARIZE_USER" -p "$PASS"`
		status=`echo "$history" | grep Status | head -n 1 | cut -d: -f 2`
		echo "Status is \"$status\""
	done
	if [[ "$status" != "$SUCCESS" ]];then
		echo -e "Error : notarization was refused, see the report:\n"
		xcrun altool \
			--notarization-info "$uuid" \
			-u "$NOTARIZE_USER" \
			-p "$PASS"
		exit 1
	fi
}

#==============================================================================
function staple() {
	echo -e "$LINE\nRubber stamping plugins..."
	cd "$TEMP_PATH" || exit 1
	for filename in *;do
		xcrun stapler staple "$filename" || exit 1
	done
}

#==============================================================================
# wait_for_notarization
# exit 1
#==============================================================================

# generate_project
# build
copy_to_temp
sign
sign_aax
build_tree
package
notarize_pkg

echo "Done !"

exit 0

# codesign --test-requirement="=notarized" --verify --verbose 