@echo off
setlocal EnableExtensions
cd /d "%~dp0"

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

if not exist "%~dp0tools\FAST_UPDATE_AND_TEST_LATEST_BUILD.ps1" (
    echo ============================================================
    echo   Spider-Man 2000 Dev - FAST Update + Test
    echo ============================================================
    echo.
    echo [INFO] Fast workflow is not installed locally yet.
    echo [..] Running the normal updater once to fetch it...
    echo.
    call "%~dp0UPDATE_SPIDEY_PROJECT.bat"
    if errorlevel 1 (
        echo.
        echo [ERROR] Update failed.
        pause
        exit /b 1
    )
)

if not exist "%~dp0tools\FAST_UPDATE_AND_TEST_LATEST_BUILD.ps1" (
    echo.
    echo [ERROR] Fast workflow was not found after updating.
    echo.
    pause
    exit /b 1
)

"%SPIDEY_POWERSHELL%" -NoProfile -ExecutionPolicy Bypass -File "%~dp0tools\FAST_UPDATE_AND_TEST_LATEST_BUILD.ps1"
exit /b %ERRORLEVEL%
