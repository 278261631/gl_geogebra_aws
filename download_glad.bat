@echo off
echo GLAD Setup Instructions
echo ========================
echo.
echo GLAD needs to be generated from the web service.
echo.
echo 1. Visit: https://glad.dav1d.de/
echo.
echo 2. Configure with these settings:
echo    - Language: C/C++
echo    - Specification: OpenGL
echo    - API gl: Version 3.3 (or higher)
echo    - Profile: Core
echo    - Options: Check "Generate a loader"
echo.
echo 3. Click "GENERATE" button
echo.
echo 4. Download the ZIP file
echo.
echo 5. Extract and copy files:
echo    - Copy glad/include/glad/ to external/glad/include/glad/
echo    - Copy glad/include/KHR/ to external/glad/include/KHR/
echo    - Copy glad/src/glad.c to external/glad/src/glad.c
echo.
echo Press any key to open GLAD website...
start https://glad.dav1d.de/

