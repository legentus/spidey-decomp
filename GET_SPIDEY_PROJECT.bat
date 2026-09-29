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
    echo   Spider-Man 2000 Developer Project - First Setup
    echo ============================================================
    echo.
    echo [ERROR] Windows PowerShell could not be found at its normal Windows locations.
    echo.
    echo Checked:
    echo   %SystemRoot%\System32\WindowsPowerShell\v1.0\powershell.exe
    echo   %SystemRoot%\Sysnative\WindowsPowerShell\v1.0\powershell.exe
    echo   %SystemRoot%\SysWOW64\WindowsPowerShell\v1.0\powershell.exe
    echo.
    pause
    exit /b 1
)

if not exist "%~dp0tools\BOOTSTRAP_SPIDEY_PROJECT.ps1" (
    echo [ERROR] tools\BOOTSTRAP_SPIDEY_PROJECT.ps1 is missing.
    echo Extract the full bootstrap ZIP before running this BAT.
    pause
    exit /b 1
)

"%SPIDEY_POWERSHELL%" -NoProfile -ExecutionPolicy Bypass -File "%~dp0tools\BOOTSTRAP_SPIDEY_PROJECT.ps1"
exit /b %ERRORLEVEL%
