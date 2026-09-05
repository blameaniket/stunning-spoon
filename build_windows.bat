@echo off
setlocal enabledelayedexpansion

:: 1. Detect vcpkg executable location
for /f "delims=" %%I in ('where vcpkg 2^>nul') do (
    set "VCPKG_EXE=%%I"
)

if not defined VCPKG_EXE (
    echo [ERROR] vcpkg executable not found in PATH! Run setup_cmdline_windows.bat first.
    exit /b 1
)

:: Get folder containing vcpkg.exe
for %%I in ("%VCPKG_EXE%") do set "VCPKG_DIR=%%~dpI"

:: Strip trailing backslash
if "%VCPKG_DIR:~-1%"=="\" set "VCPKG_DIR=%VCPKG_DIR:~0,-1%"

:: Check for CMake toolchain in Visual Studio bundled location or standard root
if exist "%VCPKG_DIR%\scripts\buildsystems\vcpkg.cmake" (
    set "TOOLCHAIN_FILE=%VCPKG_DIR%\scripts\buildsystems\vcpkg.cmake"
) else if exist "%VCPKG_DIR%\..\scripts\buildsystems\vcpkg.cmake" (
    set "TOOLCHAIN_FILE=%VCPKG_DIR%\..\scripts\buildsystems\vcpkg.cmake"
) else (
    echo [ERROR] Could not locate vcpkg.cmake toolchain!
    exit /b 1
)

echo [INFO] Found vcpkg toolchain at: %TOOLCHAIN_FILE%

:: 2. Create build directory
if not exist build mkdir build
cd build

:: 3. Run CMake with detected toolchain
cmake -G "NMake Makefiles" ^
    -DCMAKE_BUILD_TYPE=Debug ^
    -DCMAKE_TOOLCHAIN_FILE="%TOOLCHAIN_FILE%" ^
    ..

if %errorlevel% neq 0 (
    echo.
    echo [ERROR] CMake configuration failed!
    cd ..
    exit /b %errorlevel%
)

:: 4. Build with NMake
nmake

if %errorlevel% neq 0 (
    echo.
    echo [ERROR] Build failed!
    cd ..
    exit /b %errorlevel%
)

cd ..
echo.
echo [SUCCESS] Build completed successfully!
