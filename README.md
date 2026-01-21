# GeoGebra 3D Clone

A 3D geometry visualization software inspired by [GeoGebra 3D](https://www.geogebra.org/3d), built with CMake and OpenGL.

## Features

- 3D geometry visualization with OpenGL 3.3
- Interactive camera controls (orbit, pan, zoom)
- Create and manipulate geometric objects (points, lines, planes, spheres, cubes)
- ImGui-based user interface
- Cross-platform build system with CMake

## Requirements

- CMake 3.15+
- C++17 compiler
- OpenGL 3.3+ support
- Git

## Quick Start

### 1. Setup Dependencies

```bash
setup.bat
```

This will download GLFW, GLM, and ImGui.

### 2. Setup GLAD

GLAD needs to be generated manually. Run:

```bash
download_glad.bat
```

Or manually:
```bash
pip install glad
python -m glad --profile core --out-path external/glad --api gl=3.3 --generator c
```

### 3. Build

```bash
build.bat
```

### 4. Run

```bash
run.bat
```

Or directly:
```bash
build\bin\Release\GeoGebra3D.exe
```

## Controls

- **Right Mouse Button**: Orbit camera
- **Middle Mouse Button**: Pan camera
- **Mouse Wheel**: Zoom in/out
- **ESC**: Exit application

## Project Structure

```
gl_geogebra_aws/
├── include/          # Header files
├── src/             # Source files
├── external/        # External dependencies
├── build/           # Build output (generated)
├── CMakeLists.txt   # CMake configuration
└── *.bat            # Build scripts
```

## Dependencies

- [GLFW](https://www.glfw.org/) - Window and input handling
- [GLAD](https://glad.dav1d.de/) - OpenGL loader
- [GLM](https://github.com/g-truc/glm) - Mathematics library
- [ImGui](https://github.com/ocornut/imgui) - GUI library

## License

MIT License

