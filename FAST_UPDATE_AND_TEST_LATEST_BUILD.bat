@echo off
setlocal EnableExtensions EnableDelayedExpansion

if /I "%~1"=="--spidey-fast-runner" goto :RUNNER

set "SPIDEY_FAST_TEMP=%TEMP%\Spidey2000-Fast-Update-Test-%RANDOM%-%RANDOM%.bat"
copy /y "%~f0" "%SPIDEY_FAST_TEMP%" >nul 2>&1
if errorlevel 1 (
    echo [ERROR] Could not create temporary fast-test runner:
    echo   %SPIDEY_FAST_TEMP%
    pause
    exit /b 1
)

rem IMPORTANT: keep CALL + EXIT on this same physical line. The project updater
rem may replace this BAT while the temporary copy is running. CMD parses this
rem whole line before CALL, so it never resumes by reading the replaced file.
call "%SPIDEY_FAST_TEMP%" --spidey-fast-runner "%~dp0" & exit /b !ERRORLEVEL!

:RUNNER
set "SPIDEY_PROJECT_ROOT=%~2"
if not defined SPIDEY_PROJECT_ROOT (
    echo [ERROR] Fast runner did not receive the project root.
    pause
    exit /b 1
)
cd /d "%SPIDEY_PROJECT_ROOT%"

set "PATH=%SystemRoot%\System32;%SystemRoot%;%SystemRoot%\System32\Wbem;%SystemRoot%\System32\WindowsPowerShell\v1.0;%PATH%"

set "SPIDEY_POWERSHELL="
if exist "%SystemRoot%\System32\WindowsPowerShell\v1.0\powershell.exe" set "SPIDEY_POWERSHELL=%SystemRoot%\System32\WindowsPowerShell\v1.0\powershell.exe"
if not defined SPIDEY_POWERSHELL if exist "%SystemRoot%\Sysnative\WindowsPowerShell\v1.0\powershell.exe" set "SPIDEY_POWERSHELL=%SystemRoot%\Sysnative\WindowsPowerShell\v1.0\powershell.exe"
if not defined SPIDEY_POWERSHELL if exist "%SystemRoot%\SysWOW64\WindowsPowerShell\v1.0\powershell.exe" set "SPIDEY_POWERSHELL=%SystemRoot%\SysWOW64\WindowsPowerShell\v1.0\powershell.exe"

if not defined SPIDEY_POWERSHELL (
    echo ============================================================
    echo   Spider-Man 2000 Dev - FAST Update + Test
    echo ============================================================
    echo.
    echo [ERROR] Windows PowerShell could not be found.
    echo.
    pause
    exit /b 1
)

if not exist "%SPIDEY_PROJECT_ROOT%tools\FAST_UPDATE_AND_TEST_LATEST_BUILD.ps1" (
    echo ============================================================
    echo   Spider-Man 2000 Dev - FAST Update + Test
    echo ============================================================
    echo.
    echo [INFO] Fast workflow is not installed locally yet.
    echo [..] Bootstrapping it from live dev using the temporary runner...
    echo.

    set "SPIDEY_FAST_BOOTSTRAP_BEFORE_REV="
    if exist "%SPIDEY_PROJECT_ROOT%LOCAL_DEV_REVISION.txt" set /p SPIDEY_FAST_BOOTSTRAP_BEFORE_REV=<"%SPIDEY_PROJECT_ROOT%LOCAL_DEV_REVISION.txt"

    "%SPIDEY_POWERSHELL%" -NoProfile -ExecutionPolicy Bypass -File "%SPIDEY_PROJECT_ROOT%tools\UPDATE_SPIDEY_PROJECT.ps1" -NoPause
    if errorlevel 1 (
        echo.
        echo [ERROR] Update failed.
        pause
        exit /b 1
    )
)

if not exist "%SPIDEY_PROJECT_ROOT%tools\FAST_UPDATE_AND_TEST_LATEST_BUILD.ps1" (
    echo.
    echo [ERROR] Fast workflow was not found after updating.
    echo Expected:
    echo   %SPIDEY_PROJECT_ROOT%tools\FAST_UPDATE_AND_TEST_LATEST_BUILD.ps1
    echo.
    pause
    exit /b 1
)

"%SPIDEY_POWERSHELL%" -NoProfile -ExecutionPolicy Bypass -File "%SPIDEY_PROJECT_ROOT%tools\FAST_UPDATE_AND_TEST_LATEST_BUILD.ps1"
exit /b %ERRORLEVEL%
