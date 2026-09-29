@echo off
setlocal EnableExtensions EnableDelayedExpansion

if "%~1"=="" (
    echo [ERROR] update_project_worker.bat requires the project root path.
    exit /b 2
)

set "TARGET=%~1"
set "WORK=%~dp0"
set "ZIP=%WORK%dev.zip"
set "EXTRACT=%WORK%extract"
set "URL=https://github.com/legentus/spidey-decomp/archive/refs/heads/dev.zip"

set "CSCRIPT_EXE="
if exist "%SystemRoot%\System32\cscript.exe" set "CSCRIPT_EXE=%SystemRoot%\System32\cscript.exe"
if not defined CSCRIPT_EXE if exist "%SystemRoot%\Sysnative\cscript.exe" set "CSCRIPT_EXE=%SystemRoot%\Sysnative\cscript.exe"
if not defined CSCRIPT_EXE (
    echo [ERROR] Windows Script Host cscript.exe could not be found.
    exit /b 3
)

set "ROBOCOPY_EXE="
if exist "%SystemRoot%\System32\robocopy.exe" set "ROBOCOPY_EXE=%SystemRoot%\System32\robocopy.exe"
if not defined ROBOCOPY_EXE if exist "%SystemRoot%\Sysnative\robocopy.exe" set "ROBOCOPY_EXE=%SystemRoot%\Sysnative\robocopy.exe"
if not defined ROBOCOPY_EXE (
    echo [ERROR] Windows robocopy.exe could not be found.
    exit /b 4
)

if exist "%ZIP%" del /q "%ZIP%" >nul 2>&1
if exist "%EXTRACT%" rmdir /s /q "%EXTRACT%"
mkdir "%EXTRACT%" >nul 2>&1

echo [..] Downloading latest dev branch archive...
"%CSCRIPT_EXE%" //nologo "%WORK%download_file.vbs" "%URL%" "%ZIP%"
if errorlevel 1 (
    echo [ERROR] Project archive download failed.
    exit /b 5
)

echo [..] Extracting project archive...
set "TAR_EXE="
if exist "%SystemRoot%\System32\tar.exe" set "TAR_EXE=%SystemRoot%\System32\tar.exe"
if not defined TAR_EXE if exist "%SystemRoot%\Sysnative\tar.exe" set "TAR_EXE=%SystemRoot%\Sysnative\tar.exe"
if defined TAR_EXE (
    "%TAR_EXE%" -xf "%ZIP%" -C "%EXTRACT%"
    if errorlevel 1 set "TAR_EXE="
)
if not defined TAR_EXE (
    "%CSCRIPT_EXE%" //nologo "%WORK%extract_zip.vbs" "%ZIP%" "%EXTRACT%" "spidey-decomp-dev\README.md"
    if errorlevel 1 (
        echo [ERROR] Project archive extraction failed.
        exit /b 6
    )
)

set "SOURCE=%EXTRACT%\spidey-decomp-dev"
if not exist "%SOURCE%\README.md" (
    echo [ERROR] Extracted project root was not found:
    echo   %SOURCE%
    exit /b 7
)

echo [..] Refreshing local project files...
"%ROBOCOPY_EXE%" "%SOURCE%" "%TARGET%" /MIR /R:2 /W:1 /NFL /NDL /NJH /NJS /NP /XD ".git" "out" "Release" "Debug" /XF "UPDATE_PROJECT.bat" "spidey_local_config.bat" "LOCAL_DEV_REVISION.txt"
set "RC=!ERRORLEVEL!"
if !RC! GEQ 8 (
    echo [ERROR] Project refresh failed. Robocopy code: !RC!
    exit /b 8
)

if exist "%WORK%get_remote_sha.vbs" (
    "%CSCRIPT_EXE%" //nologo "%WORK%get_remote_sha.vbs" "%TARGET%\LOCAL_DEV_REVISION.txt" >nul 2>&1
)

if not exist "%TARGET%\LOCAL_DEV_REVISION.txt" (
    >"%TARGET%\LOCAL_DEV_REVISION.txt" echo dev
)

echo.
echo [OK] Local project refreshed from GitHub dev branch.
echo Revision:
set /p "REV="<"%TARGET%\LOCAL_DEV_REVISION.txt"
echo   !REV!
exit /b 0
