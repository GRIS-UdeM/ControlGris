#!/bin/sh

# Developer: Samuel Béland

export IDENTIFIER="ca.umontreal.musique.gris.controlgris.pkg"
export PACKAGE_NAME="ControlGRIS_$VERSION.pkg"
export APP_SIGNATURE="Developer ID Application: Samuel Beland (Q2A837SX87)"
export INSTALLER_SIGNATURE="Developer ID Installer: Samuel Beland (Q2A837SX87)"

#==============================================================================
export USAGE="usage:\n\tosx_package --path <plugins-path> --pass <PACE-pass>"

export PATH=""
export PASS=""

#==============================================================================
# Parse args

POSITIONAL=()
while [[ $# -gt 0 ]]
do
key="$1"

case $key in
	--path)
	PATH="$2"
	shift
	shift
	;;
	--pass)
	PASS="$2"
	shift
	shift
	;;
    *)    # unknown option
    POSITIONAL+=("$1") # save it in an array for later
    shift # past argument
    ;;
esac
done
set -- "${POSITIONAL[@]}" # restore positional parameters

if [[ "$PATH" == "" ]];then
	echo "Missing param --path"
	echo -e "$USAGE"
elif [[ "$PASS" == "" ]];then
	echo "Missing param --pass"
	echo -e "$USAGE"
fi

#==============================================================================
# get app version

PROJECT_FILE="../ControlGris.jucer"
VERSION=`Projucer --get-version "$PROJECT_FILE"`
echo "Version is $VERSION"

#==============================================================================
function sign_aax() {
	echo "do this please" && exit 1

	#pluginPath=`echo *.aaxplugin`
	# wraptool sign --verbose --signid "$signUser" --account grisresearch --wcguid A4B35290-7C9C-11EB-8B4D-00505692C25A --in "$pluginPath" --autoinstall on --extrasigningoptions "--timestamp --options runtime" || exit 1
	wraptool sign \
				\ #--signid "$APP_SIGNATURE" \
				--account grisresearch \
				--wcguid A4B35290-7C9C-11EB-8B4D-00505692C25A \
				--in "$AAX_PATH" \
				--autoinstall on \
				--notarize-username "$APP_SIGNATURE" \
				--notarize-password "$PASS" \
				--extrasigningoptions "--timestamp --options runtime" \
				--verbose \
					|| exit 1
}

#==============================================================================
function build_package() {
	echo "building $PACKAGE_NAME"
	pkgbuild    --identifier "$IDENTIFIER" \
	            --root "$PATH" \
	            --version "$VERSION" \
	            --component-plist "Application.plist" \
	            --sign "$INSTALLER_SIGNATURE" \
	            --timestamp \
	            "$PACKAGE_NAME" || exit 1
}



#==============================================================================
sign_aax
build_package

echo "Done !"

# xcrun altool --notarization-info "f82e93e4-aaaf-4990-ad04-fdb222ececc2" -u "samuel.beland@gmail.com"