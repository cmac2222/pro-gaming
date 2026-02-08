@echo off
echo ========================================
echo Rust ESP Build Script
echo ========================================
echo.

REM Check if ImGui files exist
if not exist "imgui.cpp" (
    echo [ERROR] ImGui files not found!
    echo Please download ImGui from: https://github.com/ocornut/imgui
    echo.
    echo Required files:
    echo - imgui.cpp, imgui.h
    echo - imgui_demo.cpp
    echo - imgui_draw.cpp
    echo - imgui_tables.cpp
    echo - imgui_widgets.cpp
    echo - imgui_impl_win32.cpp/h
    echo - imgui_impl_dx9.cpp/h
    echo - imgui_internal.h
    echo - imconfig.h
    echo.
    pause
    exit /b 1
)

echo [+] ImGui files found
echo.

REM Create build directory
if not exist "build" mkdir build
cd build

echo [*] Running CMake...
cmake .. -G "Visual Studio 16 2019" -A x64
if errorlevel 1 (
    echo [ERROR] CMake configuration failed!
    cd ..
    pause
    exit /b 1
)

echo [+] CMake configuration successful
echo.

echo [*] Building project (Release)...
cmake --build . --config Release
if errorlevel 1 (
    echo [ERROR] Build failed!
    cd ..
    pause
    exit /b 1
)

echo.
echo ========================================
echo [+] Build successful!
echo ========================================
echo.
echo Executable location: build\Release\RustESP.exe
echo.
pause
