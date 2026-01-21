@echo off
echo Building GeoGebra 3D...

if not exist build mkdir build

cd build

echo Running CMake...
cmake .. -G "Visual Studio 16 2019" -A x64

if %errorlevel% neq 0 (
    echo CMake configuration failed!
    cd ..
    exit /b %errorlevel%
)

echo Building project...
cmake --build . --config Release

if %errorlevel% neq 0 (
    echo Build failed!
    cd ..
    exit /b %errorlevel%
)

cd ..

echo Build completed successfully!
echo Executable location: build\bin\Release\GeoGebra3D.exe

echo Copying DLL files...
if exist dll (
    copy /Y dll\*.dll build\bin\Release\ >nul
    echo DLL files copied.
)

