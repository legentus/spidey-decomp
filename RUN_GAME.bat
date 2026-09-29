@echo off
setlocal EnableExtensions
cd /d "%~dp0"
title Spider-Man 2000 Dev - Run Game

if not exist "spidey_local_config.bat" (
    echo [ERROR] No local game path is configured.
    echo Run SETUP_FIRST_TIME.bat first.
    pause
    exit /b 1
)

call "spidey_local_config.bat"

if not defined SPIDEY_GAME_DIR (
    echo [ERROR] SPIDEY_GAME_DIR is missing from the local config.
    pause
    exit /b 1
)

if not exist "%SPIDEY_GAME_DIR%\SpideyPC.exe" (
    echo [ERROR] SpideyPC.exe was not found in:
    echo   %SPIDEY_GAME_DIR%
    echo Run SETUP_FIRST_TIME.bat again.
    pause
    exit /b 1
)

echo ============================================================
echo   Spider-Man 2000 Dev - Launching
echo ============================================================
echo Game:
echo   %SPIDEY_GAME_DIR%\SpideyPC.exe
echo.
echo Keep the spidey-decomp console open if one appears.
echo.

start "" /D "%SPIDEY_GAME_DIR%" "%SPIDEY_GAME_DIR%\SpideyPC.exe"
exit /b 0
