@echo off
setlocal EnableExtensions
cd /d "%~dp0"
title Spider-Man 2000 Dev - First Time Setup

echo ============================================================
echo   Spider-Man 2000 Dev - First Time Setup
echo ============================================================
echo.

if exist "spidey_local_config.bat" (
    call "spidey_local_config.bat"
    if defined SPIDEY_GAME_DIR (
        echo Current configured game folder:
        echo   %SPIDEY_GAME_DIR%
        echo.
    )
)

:ASK_GAME_DIR
set "GAME_DIR="
set /p "GAME_DIR=Enter the folder containing SpideyPC.exe: "
if not defined GAME_DIR goto ASK_GAME_DIR
set "GAME_DIR=%GAME_DIR:"=%"

if not exist "%GAME_DIR%\SpideyPC.exe" (
    echo.
    echo [ERROR] SpideyPC.exe was not found here:
    echo   %GAME_DIR%
    echo.
    goto ASK_GAME_DIR
)

> "spidey_local_config.bat" echo @echo off
>>"spidey_local_config.bat" echo set "SPIDEY_GAME_DIR=%GAME_DIR%"

echo.
echo [OK] Saved local game path:
echo   %GAME_DIR%
echo.
echo This file is ignored by Git and will stay local to this PC:
echo   spidey_local_config.bat
echo.

echo.
echo Setup complete.
echo Next: run UPDATE_PROJECT.bat, then BUILD_AND_INSTALL.bat.
echo.

if not defined SPIDEY_NO_PAUSE pause
exit /b 0
