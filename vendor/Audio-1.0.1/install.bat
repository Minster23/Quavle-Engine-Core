@echo off
REM Install script for Audio library
REM Usage: install.bat [install_directory]

set INSTALL_DIR=%1
if "%INSTALL_DIR%"=="" set INSTALL_DIR=%~dp0install

echo Installing Audio library to: %INSTALL_DIR%

REM Configure and build
cmake --preset=ninja -DCMAKE_INSTALL_PREFIX="%INSTALL_DIR%"
if %ERRORLEVEL% neq 0 (
    echo Failed to configure project
    exit /b 1
)

REM Build Release
cmake --build --preset=ninja-release
if %ERRORLEVEL% neq 0 (
    echo Failed to build project
    exit /b 1
)

REM Install
cmake --install out/build/ninja --config Release
if %ERRORLEVEL% neq 0 (
    echo Failed to install project
    exit /b 1
)

echo Installation completed successfully!
echo Library installed to: %INSTALL_DIR%
echo.
echo To use this library in your CMake project, add:
echo   find_package(Audio REQUIRED)
echo   target_link_libraries(your_target Audio::Audio)
echo.
echo Make sure to add the install directory to CMAKE_PREFIX_PATH:
echo   -DCMAKE_PREFIX_PATH="%INSTALL_DIR%"