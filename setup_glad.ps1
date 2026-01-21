# PowerShell script to setup GLAD

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "GLAD Setup for GeoGebra 3D" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

$gladDir = "external/glad"
$gladInclude = "$gladDir/include"
$gladSrc = "$gladDir/src"

# Create directories
New-Item -ItemType Directory -Force -Path "$gladInclude/glad" | Out-Null
New-Item -ItemType Directory -Force -Path "$gladInclude/KHR" | Out-Null
New-Item -ItemType Directory -Force -Path $gladSrc | Out-Null

Write-Host "Created GLAD directories" -ForegroundColor Green
Write-Host ""

# Check if GLAD files exist
if (Test-Path "$gladInclude/glad/glad.h") {
    Write-Host "GLAD is already set up!" -ForegroundColor Green
    exit 0
}

Write-Host "GLAD needs to be generated from the web service." -ForegroundColor Yellow
Write-Host ""
Write-Host "Steps:" -ForegroundColor White
Write-Host "1. Opening GLAD web generator..." -ForegroundColor White
Write-Host "2. Configure with these settings:" -ForegroundColor White
Write-Host "   - Language: C/C++" -ForegroundColor Gray
Write-Host "   - Specification: OpenGL" -ForegroundColor Gray
Write-Host "   - API gl: Version 3.3 or higher" -ForegroundColor Gray
Write-Host "   - Profile: Core" -ForegroundColor Gray
Write-Host "   - Options: Check 'Generate a loader'" -ForegroundColor Gray
Write-Host "3. Click GENERATE button" -ForegroundColor White
Write-Host "4. Download the ZIP file" -ForegroundColor White
Write-Host "5. Extract and copy files:" -ForegroundColor White
Write-Host "   - glad/include/glad/ -> $gladInclude/glad/" -ForegroundColor Gray
Write-Host "   - glad/include/KHR/ -> $gladInclude/KHR/" -ForegroundColor Gray
Write-Host "   - glad/src/glad.c -> $gladSrc/glad.c" -ForegroundColor Gray
Write-Host ""

$response = Read-Host "Open GLAD website now? (Y/N)"
if ($response -eq "Y" -or $response -eq "y") {
    Start-Process "https://glad.dav1d.de/"
}

Write-Host ""
Write-Host "After setting up GLAD, run: build.bat" -ForegroundColor Cyan

