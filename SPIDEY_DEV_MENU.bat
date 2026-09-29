@echo off
setlocal EnableExtensions
cd /d "%~dp0"

:MENU
cls
echo ============================================================
echo   Spider-Man 2000 Developer Project
echo ============================================================
echo.
echo   1. First-time setup / change game folder
echo   2. Update project from GitHub (DAH-style)
echo   3. Build dev proxy
echo   4. Update, build, install, and run latest
echo   5. Run game
echo   6. Restore stock game DLL
echo   7. Exit
echo.
set "CHOICE="
set /p "CHOICE=Choose an option: "

if "%CHOICE%"=="1" call "%~dp0SETUP_FIRST_TIME.bat"
if "%CHOICE%"=="2" call "%~dp0UPDATE_SPIDEY_PROJECT.bat"
if "%CHOICE%"=="3" call "%~dp0BUILD_DEV.bat"
if "%CHOICE%"=="4" call "%~dp0TEST_LATEST_BUILD.bat"
if "%CHOICE%"=="5" call "%~dp0RUN_GAME.bat"
if "%CHOICE%"=="6" call "%~dp0RESTORE_STOCK_GAME.bat"
if "%CHOICE%"=="7" exit /b 0

goto :MENU
