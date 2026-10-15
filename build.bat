@echo off
setlocal enabledelayedexpansion

echo ========================================================
echo   2D Monster Hunter Mobile Prototype - Build Script
echo ========================================================

:: 1. Detect Compiler
set "GXX="
if exist "C:\raylib\w64devkit\bin\g++.exe" (
    set "GXX=C:\raylib\w64devkit\bin\g++.exe"
    echo [OK] Using Raylib bundled w64devkit g++: !GXX!
) else (
    where g++ >nul 2>&1
    if !errorlevel! equ 0 (
        set "GXX=g++"
        echo [OK] Using system g++
    ) else (
        echo [ERROR] No g++ compiler found!
        echo Please ensure Raylib is installed at C:\raylib or g++ is in your PATH.
        pause
        exit /b 1
    )
)

:: 2. Setup Include and Library paths
set "RAYLIB_DIR=C:\raylib\raylib\src"
if not exist "!RAYLIB_DIR!\raylib.h" (
    echo [WARNING] !RAYLIB_DIR! not found. Trying local environment...
    set "INC_FLAGS=-Isrc"
    set "LIB_FLAGS=-lraylib -lopengl32 -lgdi32 -lwinmm"
) else (
    set "INC_FLAGS=-I"!RAYLIB_DIR!" -Isrc"
    set "LIB_FLAGS=-L"!RAYLIB_DIR!" -lraylib -lopengl32 -lgdi32 -lwinmm"
)

:: 3. Source files
set "SOURCES=src\main.cpp src\core\ScreenManager.cpp src\rendering\RenderManager.cpp src\input\VirtualJoystick.cpp src\entities\Character.cpp src\entities\DragonBoss.cpp src\world\GameMap.cpp src\assets\AssetManager.cpp"

echo.
echo Compiling source files...
echo Command: "!GXX!" -std=c++17 -O2 -Wall !INC_FLAGS! !SOURCES! !LIB_FLAGS! -o game.exe
echo.

"!GXX!" -std=c++17 -O2 -Wall !INC_FLAGS! !SOURCES! !LIB_FLAGS! -o game.exe

if %ERRORLEVEL% neq 0 (
    echo.
    echo ========================================================
    echo   [FAILED] Build encountered errors.
    echo ========================================================
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo ========================================================
echo   [SUCCESS] Build completed successfully: game.exe
echo ========================================================
echo.

:: 4. Option to launch
if "%1"=="--run" (
    echo Launching game.exe...
    start "" "game.exe"
) else (
    set /p RUN_CHOICE="Do you want to run the game now? (Y/n): "
    if /i "!RUN_CHOICE!"=="" set "RUN_CHOICE=Y"
    if /i "!RUN_CHOICE!"=="Y" (
        echo Launching game.exe...
        start "" "game.exe"
    )
)

exit /b 0
