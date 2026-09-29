@echo off
setlocal EnableExtensions
cd /d "%~dp0"
title Spider-Man 2000 Dev - Build

echo ============================================================
echo   Spider-Man 2000 Dev - Build Matching Proxy
echo ============================================================
echo.

where powershell.exe >nul 2>&1
if errorlevel 1 (
    echo [ERROR] Windows PowerShell was not found.
    goto :FAIL
)

powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0scripts\build_matching.ps1"
if errorlevel 1 goto :FAIL

echo.
echo [OK] Build completed.
echo Output:
echo   out\matching\binkw32.dll
echo.

if not defined SPIDEY_NO_PAUSE pause
exit /b 0

:FAIL
echo.
echo [ERROR] Build failed.
if not defined SPIDEY_NO_PAUSE pause
exit /b 1
