@echo off
if not exist "game.exe" (
    echo game.exe not found! Building first...
    call build.bat --run
) else (
    start "" "game.exe"
)
