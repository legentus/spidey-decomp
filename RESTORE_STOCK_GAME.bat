@echo off
setlocal EnableExtensions
cd /d "%~dp0"
title Spider-Man 2000 Dev - Restore Stock Game

echo ============================================================
echo   Spider-Man 2000 Dev - Restore Stock Bink DLL
echo ============================================================
echo.

if not exist "spidey_local_config.bat" (
    echo [ERROR] No local game path is configured.
    goto :FAIL
)

call "spidey_local_config.bat"

if not defined SPIDEY_GAME_DIR (
    echo [ERROR] SPIDEY_GAME_DIR is missing from the local config.
    goto :FAIL
)

if not exist "%SPIDEY_GAME_DIR%\SpideyPC.exe" (
    echo [ERROR] SpideyPC.exe was not found in:
    echo   %SPIDEY_GAME_DIR%
    goto :FAIL
)

if not exist "%SPIDEY_GAME_DIR%\binkw32_.dll" (
    echo [ERROR] binkw32_.dll was not found.
    echo There is no preserved retail Bink DLL to restore.
    goto :FAIL
)

if exist "%SPIDEY_GAME_DIR%\binkw32.dll" del /q "%SPIDEY_GAME_DIR%\binkw32.dll"
if errorlevel 1 goto :FAIL

move /y "%SPIDEY_GAME_DIR%\binkw32_.dll" "%SPIDEY_GAME_DIR%\binkw32.dll" >nul
if errorlevel 1 goto :FAIL

echo [OK] Restored the retail binkw32.dll.
echo.
pause
exit /b 0

:FAIL
echo.
echo [ERROR] Restore failed.
pause
exit /b 1
