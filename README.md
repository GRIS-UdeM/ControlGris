<p align="left">
  <img width="150" src="https://github.com/user-attachments/assets/fbec48ec-1f0c-41f1-9a10-4ce993747f57">
  <img width="450" src="https://github.com/user-attachments/assets/4cd12fd2-1bab-4139-95a2-73bbfadde332">
</p>

# ControlGRIS
Spatialization plugin for [SpatGRIS](https://github.com/GRIS-UdeM/SpatGRIS). ControlGRIS is currently developed at the [Groupe de Recherche en Immersion Spatiale (GRIS)](https://gris.musique.umontreal.ca/) and the [Société des Arts Technologiques (SAT)](https://sat.qc.ca/en/).

## Building
### Clone ControlGRIS sources and submodules
```
git clone git@github.com:GRIS-UdeM/ControlGRIS.git
```
For the submodules:
```
cd ControlGRIS
git submodule update --init --recursive
```

### Build the Projucer
JUCE is included as a submodule. Go to `ControlGris/submodules/StructGRIS/submodules/JUCE/extras/Projucer/Builds/` and build the Projucer for your plateform.
Ensure that the Projucer global paths are set correctly. JUCE path is `ControlGris/submodules/StructGRIS/submodules/JUCE` and modules path is `ControlGris/submodules/StructGRIS/submodules/JUCE/modules`.

##### Additional dependencies on Linux :
From the [JUCE github website](https://github.com/juce-framework/JUCE/blob/develop/docs/Linux%20Dependencies.md) :
```bash
sudo apt install libasound2-dev libjack-jackd2-dev \
    ladspa-sdk \
    libcurl4-openssl-dev  \
    libfreetype6-dev libfontconfig1-dev \
    libx11-dev libxcomposite-dev libxcursor-dev libxext-dev libxinerama-dev libxrandr-dev libxrender-dev \
    libwebkit2gtk-4.1-dev \
    libglu1-mesa-dev mesa-common-dev
```

### Build the plugin
####  Generating project files
```bash
<path/to/Projucer> --resave <path/to/ControlGRIS.jucer>
```

#### Compiling
Go to the generated `Builds/` folder.

On Windows, use the Visual Studio 2022/2026 solution file.

On MacOS, use the Xcode project. You will have to supply your own developer ID to XCode.

On Linux :
Make sure the directory `~/.vst3` exists.
```bash
cd ControlGRIS/Builds/LinuxMakeFile
make CXX=clang++ CONFIG=Release -j8
cp -r build/ControlGRIS2.vst3 ~/.vst3/
```

