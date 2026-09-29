@echo off
setlocal EnableExtensions
cd /d "%~dp0"
title Spider-Man 2000 Dev - Build and Install

echo ============================================================
echo   Spider-Man 2000 Dev - Build and Install
echo ============================================================
echo.

set "SPIDEY_NO_PAUSE=1"

call "%~dp0BUILD_DEV.bat"
if errorlevel 1 goto :FAIL

call "%~dp0INSTALL_DEV_BUILD.bat"
if errorlevel 1 goto :FAIL

set "SPIDEY_NO_PAUSE="

echo.
echo ============================================================
echo   SUCCESS
echo ============================================================
echo Dev build is built and installed.
echo Run RUN_GAME.bat to launch Spider-Man.
echo.
pause
exit /b 0

:FAIL
set "SPIDEY_NO_PAUSE="
echo.
echo ============================================================
echo   FAILED
echo ============================================================
echo Build/install did not complete.
echo.
pause
exit /b 1
