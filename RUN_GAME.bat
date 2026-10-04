@echo off
setlocal EnableExtensions
cd /d "%~dp0"
title Spider-Man 2000 Dev - Run Current Installed Build

echo ============================================================
echo   Spider-Man 2000 Dev - Run Current Installed Build
echo ============================================================
echo.
echo [INFO] No update, download, rebuild, or DLL replacement is performed.
echo [INFO] This launches whatever dev build is already installed.
echo.

if exist "spidey_local_config.bat" (
    call "spidey_local_config.bat"
)

if not defined SPIDEY_GAME_DIR (
    set "SPIDEY_GAME_DIR=C:\Program Files (x86)\Activision\Spider-Man"
)

if not exist "%SPIDEY_GAME_DIR%\SpideyPC.exe" (
    echo [INFO] SpideyPC.exe was not found at:
    echo   %SPIDEY_GAME_DIR%
    echo.
    set "SPIDEY_GAME_DIR="
    set /p "SPIDEY_GAME_DIR=Enter the folder containing SpideyPC.exe: "
    set "SPIDEY_GAME_DIR=%SPIDEY_GAME_DIR:"=%"
)

if not defined SPIDEY_GAME_DIR (
    echo [ERROR] No game folder was provided.
    pause
    exit /b 1
)

if not exist "%SPIDEY_GAME_DIR%\SpideyPC.exe" (
    echo [ERROR] SpideyPC.exe was not found in:
    echo   %SPIDEY_GAME_DIR%
    pause
    exit /b 1
)

> "spidey_local_config.bat" echo @echo off
>>"spidey_local_config.bat" echo set "SPIDEY_GAME_DIR=%SPIDEY_GAME_DIR%"

echo [RUN] %SPIDEY_GAME_DIR%\SpideyPC.exe
echo.
start "" /D "%SPIDEY_GAME_DIR%" "%SPIDEY_GAME_DIR%\SpideyPC.exe"
exit /b 0
