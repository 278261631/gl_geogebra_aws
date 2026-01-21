cmake_minimum_required(VERSION 3.15)
project(GeoGebra3D VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Options
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

# Find packages
find_package(OpenGL REQUIRED)

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(external/glfw)

# GLAD
add_subdirectory(external/glad)

# GLM
add_subdirectory(external/glm)

# ImGui
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui)
add_library(imgui STATIC
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC 
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)
target_link_libraries(imgui PUBLIC glfw OpenGL::GL)

# Source files
set(SOURCES
    src/main.cpp
    src/Application.cpp
    src/Window.cpp
    src/Renderer.cpp
    src/Camera.cpp
    src/Shader.cpp
    src/Grid.cpp
    src/Axes.cpp
    src/GeometryObject.cpp
    src/InputHandler.cpp
    src/UI/UIManager.cpp
    src/UI/Toolbar.cpp
    src/UI/Sidebar.cpp
    src/UI/PropertiesPanel.cpp
    src/Math/Vector3.cpp
    src/Math/Matrix4.cpp
    src/Geometry/Point.cpp
    src/Geometry/Line.cpp
    src/Geometry/Plane.cpp
    src/Geometry/Sphere.cpp
    src/Geometry/Cube.cpp
)

set(HEADERS
    include/Application.h
    include/Window.h
    include/Renderer.h
    include/Camera.h
    include/Shader.h
    include/Grid.h
    include/Axes.h
    include/GeometryObject.h
    include/InputHandler.h
    include/UI/UIManager.h
    include/UI/Toolbar.h
    include/UI/Sidebar.h
    include/UI/PropertiesPanel.h
    include/Math/Vector3.h
    include/Math/Matrix4.h
    include/Geometry/Point.h
    include/Geometry/Line.h
    include/Geometry/Plane.h
    include/Geometry/Sphere.h
    include/Geometry/Cube.h
)

# Executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/glfw/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/glm
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    OpenGL::GL
    glfw
    glad
    glm
    imgui
)

# Copy shaders to build directory (currently shaders are embedded in code)
# file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/shaders DESTINATION ${CMAKE_BINARY_DIR})

# Install
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
# install(DIRECTORY shaders DESTINATION bin)

