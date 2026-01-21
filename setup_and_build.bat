@echo off
echo ========================================
echo GeoGebra 3D - One-Click Setup and Build
echo ========================================
echo.

echo Step 1: Setting up dependencies...
call setup.bat
if %errorlevel% neq 0 (
    echo Setup failed!
    exit /b %errorlevel%
)

echo.
echo Step 2: Checking GLAD...
if not exist external\glad\include\glad\glad.h (
    echo.
    echo WARNING: GLAD is not set up!
    echo Please run download_glad.bat and follow instructions.
    echo Then run build.bat to compile.
    exit /b 1
)

echo.
echo Step 3: Building project...
call build.bat
if %errorlevel% neq 0 (
    echo Build failed!
    exit /b %errorlevel%
)

echo.
echo ========================================
echo Setup and Build Complete!
echo ========================================
echo.
echo To run the application: run.bat
echo.

