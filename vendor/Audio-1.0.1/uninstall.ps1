# PowerShell uninstall script for Audio library
# Usage: .\uninstall.ps1 [-InstallDir <path>]

param(
    [string]$InstallDir = (Join-Path $PSScriptRoot "install")
)

Write-Host "Uninstalling Audio library from: $InstallDir" -ForegroundColor Yellow

if (Test-Path $InstallDir) {
    try {
        Remove-Item -Recurse -Force $InstallDir
        Write-Host "Audio library uninstalled successfully!" -ForegroundColor Green
    }
    catch {
        Write-Host "Error removing directory: $_" -ForegroundColor Red
        exit 1
    }
}
else {
    Write-Host "Install directory not found: $InstallDir" -ForegroundColor Yellow
    Write-Host "Nothing to uninstall." -ForegroundColor Green
}