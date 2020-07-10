# ControlGris
Spatialization plugin (SpatGris version 2)

## Building the ControlGris VST plugin on Debian (Ubuntu)

### Install dependencies

```
sudo apt-get install clang git ladspa-sdk freeglut3-dev g++ libasound2-dev libcurl4-openssl-dev libfreetype6-dev libjack-jackd2-dev libx11-dev libxcomposite-dev libxcursor-dev libxinerama-dev libxrandr-dev mesa-common-dev webkit2gtk-4.0 juce-tools
```

You might also need

```
sudo apt-get install libgtk2.0-dev
```

### Download Juce

To build the ControlGris VST plugin, you'll need Juce 5, download it from https://shop.juce.com/get-juce/download page.

### Download the Steinberg VST SDK

You will need a "legacy" SDK (VST2 - compatible). Extract it somewhere and add it the Projucer global paths.

### Clone ControlGris sources

```
git clone https://github.com/GRIS-UdeM/ControlGris.git
```

### Build the plugin

1. Start the Projucer app, open the ControlGris.jucer file and save the project. This step must be done each time the structure of the project changes (new files, new JUCE version, etc.).

```
cd /path/to/JUCE/folder
./Projucer
```

After saving the project, you can quit the Projucer.

2. If the directory *.vst/* doesn't exist in your home folder, create it.

```
cd
mkdir .vst
```

3. Go to the ControlGris Builds folder, compile the plugin and move a copy to the VST directory.

```
cd ControlGris/Builds/LinuxMakeFile
make CONFIG=Release && cp build/*.so ~/.vst/
```

4. Start Reaper and load the plugin!
