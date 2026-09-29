@echo off
setlocal EnableExtensions
cd /d "%~dp0"
title Spider-Man 2000 Dev - Update Project

echo ============================================================
echo   Spider-Man 2000 Dev - Update Project
echo ============================================================
echo.
echo Updating directly from the GitHub dev branch archive.
echo No Git, PowerShell, or curl installation is required.
echo.

if not exist "%~dp0scripts\update_project_worker.bat" (
    echo [ERROR] scripts\update_project_worker.bat is missing.
    echo Re-run the latest standalone bootstrap once.
    goto :FAIL
)
if not exist "%~dp0scripts\download_file.vbs" (
    echo [ERROR] scripts\download_file.vbs is missing.
    echo Re-run the latest standalone bootstrap once.
    goto :FAIL
)
if not exist "%~dp0scripts\extract_zip.vbs" (
    echo [ERROR] scripts\extract_zip.vbs is missing.
    echo Re-run the latest standalone bootstrap once.
    goto :FAIL
)

set "WORK=%TEMP%\Spidey2000Update-%RANDOM%-%RANDOM%"
mkdir "%WORK%" >nul 2>&1
if errorlevel 1 (
    echo [ERROR] Could not create temporary update folder.
    goto :FAIL
)

copy /y "%~dp0scripts\update_project_worker.bat" "%WORK%\update_project_worker.bat" >nul
copy /y "%~dp0scripts\download_file.vbs" "%WORK%\download_file.vbs" >nul
copy /y "%~dp0scripts\extract_zip.vbs" "%WORK%\extract_zip.vbs" >nul
if exist "%~dp0scripts\get_remote_sha.vbs" copy /y "%~dp0scripts\get_remote_sha.vbs" "%WORK%\get_remote_sha.vbs" >nul

call "%WORK%\update_project_worker.bat" "%CD%"
set "RESULT=%ERRORLEVEL%"

rmdir /s /q "%WORK%" >nul 2>&1

if not "%RESULT%"=="0" goto :FAIL

echo.
echo ============================================================
echo   UPDATE COMPLETE
echo ============================================================
echo.
echo Next: run BUILD_AND_INSTALL.bat.
echo.
if not defined SPIDEY_NO_PAUSE pause
exit /b 0

:FAIL
echo.
echo [ERROR] Project update did not complete.
if not defined SPIDEY_NO_PAUSE pause
exit /b 1
