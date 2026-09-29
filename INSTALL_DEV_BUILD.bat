@echo off
setlocal EnableExtensions
cd /d "%~dp0"
title Spider-Man 2000 Dev - Install Dev Build

echo ============================================================
echo   Spider-Man 2000 Dev - Install Dev Build
echo ============================================================
echo.

if not exist "spidey_local_config.bat" (
    echo [..] No local game path is configured yet.
    set "SPIDEY_NO_PAUSE=1"
    call "%~dp0SETUP_FIRST_TIME.bat"
    set "SPIDEY_NO_PAUSE="
    if errorlevel 1 goto :FAIL
)

call "spidey_local_config.bat"

if not defined SPIDEY_GAME_DIR (
    echo [ERROR] SPIDEY_GAME_DIR is missing from spidey_local_config.bat.
    goto :FAIL
)

if not exist "%SPIDEY_GAME_DIR%\SpideyPC.exe" (
    echo [ERROR] SpideyPC.exe was not found in:
    echo   %SPIDEY_GAME_DIR%
    echo Run SETUP_FIRST_TIME.bat again.
    goto :FAIL
)

if not exist "%~dp0out\matching\binkw32.dll" (
    echo [ERROR] No built proxy was found.
    echo Run BUILD_DEV.bat first.
    goto :FAIL
)

powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0scripts\install_dev_proxy.ps1" -GameDir "%SPIDEY_GAME_DIR%"
if errorlevel 1 goto :FAIL

echo.
echo [OK] Dev build installed.
echo Next: run RUN_GAME.bat.
echo.

if not defined SPIDEY_NO_PAUSE pause
exit /b 0

:FAIL
echo.
echo [ERROR] Install failed.
if not defined SPIDEY_NO_PAUSE pause
exit /b 1
