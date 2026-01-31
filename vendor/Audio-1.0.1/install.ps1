# PowerShell install script for Audio library
# Usage: .\install.ps1 [-InstallDir <path>] [-BuildType <Debug|Release>]

param(
    [string]$InstallDir = (Join-Path $PSScriptRoot "install"),
    [ValidateSet("Debug", "Release")]
    [string]$BuildType = "Release"
)

Write-Host "Installing Audio library to: $InstallDir" -ForegroundColor Green
Write-Host "Build type: $BuildType" -ForegroundColor Green

try {
    # Configure
    Write-Host "Configuring project..." -ForegroundColor Yellow
    & cmake --preset=ninja "-DCMAKE_INSTALL_PREFIX=$InstallDir"
    if ($LASTEXITCODE -ne 0) { throw "Configuration failed" }

    # Build
    $buildPreset = if ($BuildType -eq "Debug") { "ninja-debug" } else { "ninja-release" }
    Write-Host "Building project ($buildPreset)..." -ForegroundColor Yellow
    & cmake --build --preset=$buildPreset
    if ($LASTEXITCODE -ne 0) { throw "Build failed" }

    # Install
    Write-Host "Installing..." -ForegroundColor Yellow
    & cmake --install "out/build/ninja" --config $BuildType
    if ($LASTEXITCODE -ne 0) { throw "Installation failed" }

    Write-Host "Installation completed successfully!" -ForegroundColor Green
    Write-Host "Library installed to: $InstallDir" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "To use this library in your CMake project, add:" -ForegroundColor Yellow
    Write-Host "  find_package(Audio REQUIRED)" -ForegroundColor White
    Write-Host "  target_link_libraries(your_target Audio::Audio)" -ForegroundColor White
    Write-Host ""
    Write-Host "Make sure to add the install directory to CMAKE_PREFIX_PATH:" -ForegroundColor Yellow
    Write-Host "  -DCMAKE_PREFIX_PATH=`"$InstallDir`"" -ForegroundColor White
}
catch {
    Write-Host "Error: $_" -ForegroundColor Red
    exit 1
}