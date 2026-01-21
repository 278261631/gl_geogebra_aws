@echo off
echo Running GeoGebra 3D...

if exist build\bin\Release\GeoGebra3D.exe (
    cd build\bin\Release
    GeoGebra3D.exe
    cd ..\..\..
) else if exist build\bin\Debug\GeoGebra3D.exe (
    cd build\bin\Debug
    GeoGebra3D.exe
    cd ..\..\..
) else (
    echo Executable not found! Please build the project first.
    echo Run: build.bat
)

