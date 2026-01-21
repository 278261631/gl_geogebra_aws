@echo off
setlocal enabledelayedexpansion

echo ========================================
echo GeoGebra 3D - Complete Setup
echo ========================================
echo.

if not exist external mkdir external
cd external

echo [1/4] Setting up GLFW...
if not exist glfw (
    git clone --depth 1 --branch 3.3.8 https://github.com/glfw/glfw.git
    if !errorlevel! neq 0 (
        echo Failed to clone GLFW
        cd ..
        exit /b 1
    )
    echo GLFW cloned successfully
) else (
    echo GLFW already exists
)

echo.
echo [2/4] Setting up GLM...
if not exist glm (
    git clone --depth 1 --branch 0.9.9.8 https://github.com/g-truc/glm.git
    if !errorlevel! neq 0 (
        echo Failed to clone GLM
        cd ..
        exit /b 1
    )
    echo GLM cloned successfully
) else (
    echo GLM already exists
)

echo.
echo [3/4] Setting up ImGui...
if not exist imgui (
    git clone --depth 1 --branch v1.89.9 https://github.com/ocornut/imgui.git
    if !errorlevel! neq 0 (
        echo Failed to clone ImGui
        cd ..
        exit /b 1
    )
    echo ImGui cloned successfully
) else (
    echo ImGui already exists
)

echo.
echo [4/4] Setting up GLAD...
if not exist glad\include\glad\glad.h (
    echo GLAD needs to be set up manually.
    echo Please run: download_glad.bat
    echo Or visit: https://glad.dav1d.de/
) else (
    echo GLAD already exists
)

cd ..

echo.
echo ========================================
echo Setup Complete!
echo ========================================
echo.
echo Next steps:
echo 1. If GLAD is not set up, run: download_glad.bat
echo 2. Build the project: build.bat
echo 3. Run the application: run.bat
echo.

