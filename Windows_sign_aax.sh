#!/bin/bash

# Developer: Gaël Lane Lépine, Samuel Béland

#==============================================================================
export USAGE="usage:\n\tWindows_regroup_and_sign.sh --cert <certificate-path> --pass <certificate-password>"

function print_usage_and_exit()
{
	echo -e "$USAGE"
	exit 1
}

export CERT=""
export PASS=""

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
# sign the aax plugin
export PLUGIN_PATH="Builds/VisualStudio2026/x64/Release/AAX/ControlGRIS2.aaxplugin"
export PATH="$PATH:/c/Program Files/PACEAntiPiracy/Eden/Fusion/Versions/6/bin"
wraptool.exe sign \
		 --verbose \
		 --account grisresearch \
		 --keyfile "$CERT" \
		 --keypassword "$PASS" \
		 --wcguid A4B35290-7C9C-11EB-8B4D-00505692C25A \
		 --in "$PLUGIN_PATH" \
		 --out "$PLUGIN_PATH" \
		 || exit 1

echo "Done"
