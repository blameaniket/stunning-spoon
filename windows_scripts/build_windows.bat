@echo off
setlocal

cd /d "%~dp0.."

echo.
echo ========================================
echo   Configuring stunning-spoon
echo ========================================
echo.

cmake -S . -B build ^
    -G "Visual Studio 18 2026" ^
    -A x64 ^
    -DCMAKE_TOOLCHAIN_FILE="C:/Program Files (x86)/Microsoft Visual Studio/18/BuildTools/VC/vcpkg/scripts/buildsystems/vcpkg.cmake"

if errorlevel 1 (
    echo.
    echo [ERROR] CMake configuration failed.
    exit /b 1
)

echo.
echo ========================================
echo   Building stunning-spoon
echo ========================================
echo.

cmake --build build --config Release

if errorlevel 1 (
    echo.
    echo [ERROR] Build failed.
    exit /b 1
)

echo.
echo [SUCCESS] Build completed.
echo Executable: build\Release\app.exe
echo.

endlocal
