@echo off
setlocal EnableExtensions

set "VCPKG=C:\Users\dsaut\vcpkg"

set "TOOLCHAIN=%VCPKG%\scripts\buildsystems\vcpkg.cmake"

if exist "%TOOLCHAIN%" (
    echo Using vcpkg toolchain: "%TOOLCHAIN%"
    cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE="%TOOLCHAIN%"
) else (
    echo Toolchain not found at "%TOOLCHAIN%"
    echo Configuring without toolchain (FetchContent will be used if needed)
    cmake -B build -S .
)

if errorlevel 1 (
    echo CMake configuration failed.
    endlocal
    exit /b 1
)

cmake --build build --config Release
if errorlevel 1 (
    echo Build failed.
    endlocal
    exit /b 1
)

echo Build finished.
endlocal
exit /b 0