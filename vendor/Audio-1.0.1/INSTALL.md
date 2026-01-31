# Audio Library Installation and Usage Guide

## Installation

### Option 1: Using PowerShell (Recommended for Windows)
```powershell
# Install Release build to default location (./install)
.\install.ps1

# Install Debug build to custom location
.\install.ps1 -InstallDir "C:\MyLibraries\Audio" -BuildType Debug
```

### Option 2: Using Batch Script
```cmd
# Install to default location
install.bat

# Install to custom location
install.bat "C:\MyLibraries\Audio"
```

### Option 3: Manual Installation
```bash
# Configure
cmake --preset=ninja -DCMAKE_INSTALL_PREFIX="your/install/path"

# Build
cmake --build --preset=ninja-release

# Install
cmake --install out/build/ninja --config Release
```

## Usage in Your CMake Project

After installation, you can use the Audio library in your CMake projects:

```cmake
# Find the package
find_package(Audio REQUIRED)

# Link to your target
target_link_libraries(your_target_name Audio::Audio)
```

If you installed to a custom location, make sure to set `CMAKE_PREFIX_PATH`:

```bash
cmake -DCMAKE_PREFIX_PATH="path/to/your/install/dir" ..
```

## Example CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.22)
project(MyAudioProject)

# Find Audio library
find_package(Audio REQUIRED)

# Create your executable
add_executable(my_app main.cpp)

# Link Audio library
target_link_libraries(my_app Audio::Audio)
```

## Example Usage in Code

```cpp
#include <Audio/Device.hpp>
#include <Audio/Sound.hpp>

int main()
{
    // Initialize audio device
    Audio::Device device;
    
    // Load and play a sound
    Audio::Sound sound("path/to/sound.wav");
    sound.Play();
    
    return 0;
}
```

## Uninstallation

To remove the installed library:

```powershell
# Remove from default location
.\uninstall.ps1

# Remove from custom location
.\uninstall.ps1 -InstallDir "C:\MyLibraries\Audio"
```

## Package Contents

The installation includes:
- **Headers**: `include/Audio/` - All public header files
- **Libraries**: `lib/` - Static/shared libraries (Audio.lib, Audio_d.lib)
- **CMake Config**: `lib/cmake/Audio/` - CMake package configuration files
- **Binaries**: `bin/` - DLL files (if built as shared library)

## Build Options

- `BUILD_SHARED_LIBS=ON/OFF` - Build as shared library (DLL) or static library
- `AUDIO_BUILD_EXAMPLES=ON/OFF` - Include example projects in build
- `CMAKE_BUILD_TYPE=Debug/Release` - Build configuration