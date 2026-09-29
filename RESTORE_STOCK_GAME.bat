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

powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0scripts\restore_stock_bink.ps1" -GameDir "%SPIDEY_GAME_DIR%"
if errorlevel 1 goto :FAIL

echo.
echo [OK] Retail binkw32.dll restored.
echo.
pause
exit /b 0

:FAIL
echo.
echo [ERROR] Restore failed.
pause
exit /b 1
