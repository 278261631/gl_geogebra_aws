@echo off
echo Downloading dependencies...

if not exist external mkdir external

cd external

if not exist glfw (
    echo Cloning GLFW...
    git clone https://github.com/glfw/glfw.git
    cd glfw
    git checkout 3.3.8
    cd ..
)

if not exist glad (
    echo Setting up GLAD...
    mkdir glad
    cd glad
    mkdir include
    mkdir src
    cd ..
)

if not exist glm (
    echo Cloning GLM...
    git clone https://github.com/g-truc/glm.git
    cd glm
    git checkout 0.9.9.8
    cd ..
)

if not exist imgui (
    echo Cloning ImGui...
    git clone https://github.com/ocornut/imgui.git
    cd imgui
    git checkout v1.89.9
    cd ..
)

cd ..

echo Dependencies downloaded successfully!
echo.
echo Please download GLAD from https://glad.dav1d.de/
echo Configuration:
echo - Language: C/C++
echo - Specification: OpenGL
echo - API gl: Version 3.3 or higher
echo - Profile: Core
echo - Generate a loader: checked
echo.
echo Extract the downloaded files:
echo - Copy include/glad and include/KHR to external/glad/include/
echo - Copy src/glad.c to external/glad/src/
echo.

