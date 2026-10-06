@echo off
setlocal EnableExtensions
cd /d "%~dp0"

set "SPIDEY_POWERSHELL="
if exist "%SystemRoot%\System32\WindowsPowerShell\v1.0\powershell.exe" set "SPIDEY_POWERSHELL=%SystemRoot%\System32\WindowsPowerShell\v1.0\powershell.exe"
if not defined SPIDEY_POWERSHELL if exist "%SystemRoot%\Sysnative\WindowsPowerShell\v1.0\powershell.exe" set "SPIDEY_POWERSHELL=%SystemRoot%\Sysnative\WindowsPowerShell\v1.0\powershell.exe"
if not defined SPIDEY_POWERSHELL if exist "%SystemRoot%\SysWOW64\WindowsPowerShell\v1.0\powershell.exe" set "SPIDEY_POWERSHELL=%SystemRoot%\SysWOW64\WindowsPowerShell\v1.0\powershell.exe"

if not defined SPIDEY_POWERSHELL (
    echo [ERROR] Windows PowerShell could not be found.
    pause
    exit /b 1
)

"%SPIDEY_POWERSHELL%" -NoProfile -ExecutionPolicy Bypass -File "%~dp0tools\SYNC_CLAUDE_DECOMP.ps1"
exit /b %ERRORLEVEL%
