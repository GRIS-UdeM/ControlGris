#!/bin/sh

# Developer: Samuel Béland

#==============================================================================
export USAGE="usage:\n\tWindows_regroup_and_sign.sh --cert <certificate-path> --pass <certificate-password>"

function print_usage_and_exit()
{
	echo -e "$USAGE"
	exit 1
}

export CERT_PATH=""
export PASS=""
#==============================================================================
export LINE=":::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::"
# https://stackoverflow.com/questions/59895/how-can-i-get-the-source-directory-of-a-bash-script-from-within-the-script-itsel
export PROJECT_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
export BUILDS_DIR="$PROJECT_DIR/Builds/VisualStudio2019/x64/Release"
export OUT_DIR="$PROJECT_DIR/out"

export VST_IN_PATH="$BUILDS_DIR/VST/ControlGris.dll"
export VST_OUT_PATH="$OUT_DIR/ControlGris.dll"
export VST3_IN_PATH="$BUILDS_DIR/VST3/ControlGris.vst3"
export VST3_OUT_PATH="$OUT_DIR/ControlGris.vst3"
export AAX_IN_PATH="$BUILDS_DIR/AAX/ControlGris.aaxplugin"
export AAX_OUT_PATH="$OUT_DIR/ControlGris.aaxplugin"

#==============================================================================
# Parse args
POSITIONAL=()
while [[ $# -gt 0 ]]
do
key="$1"

case $key in
    --cert)
    CERT="$2"
    shift # past argument
    shift # past value
    ;;
    --pass)
    PASS="$2"
    shift # past argument
    shift # past value
    ;;
    *)    # unknown option
	echo "lol"
    print_usage_and_exit
    ;;
esac
done
set -- "${POSITIONAL[@]}" # restore positional parameters

if [[ "$CERT" == "" ]];then
	print_usage_and_exit
elif [[ "$PASS" == "" ]];then
	print_usage_and_exit
fi

#==============================================================================
# Clean and create output path
rm -fr "$OUT_DIR"
mkdir -p "$OUT_DIR" || exit 1

# copy non-aax plugins
cp -f "$VST_IN_PATH" "$VST_OUT_PATH" || exit 1
cp -f "$VST3_IN_PATH" "$VST3_OUT_PATH" || exit 1

# copy aax plugin
# The bash cp command doesn't copy file properties, so we use a powershell command instead
cd "$PROJECT_DIR"
powershell "Copy-Item Builds\\VisualStudio2019\\x64\\Release\\AAX\\ControlGris.aaxplugin out -Recurse"

# copy and sign aax plugin
wraptool sign \
		 --verbose \
		 --account grisresearch \
		 --keyfile "$CERT" \
		 --keypassword "$PASS" \
		 --wcguid A4B35290-7C9C-11EB-8B4D-00505692C25A \
		 --in "$AAX_OUT_PATH" \
		 --out "$AAX_OUT_PATH" \
		 || exit 1

echo "Done"
