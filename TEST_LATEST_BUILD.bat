@echo off
setlocal EnableExtensions
cd /d "%~dp0"

echo ============================================================
echo   Spider-Man 2000 Dev - LOCAL Checkout Test
echo ============================================================
echo.
echo [LOCAL] F:\Spider-Man 2000 Recomp\project main is authoritative.
echo [LOCAL] GitHub update is intentionally skipped.
echo.

set "SPIDEY_LOCAL_HEAD="
for /f "delims=" %%I in ('git rev-parse HEAD 2^>nul') do set "SPIDEY_LOCAL_HEAD=%%I"
if not defined SPIDEY_LOCAL_HEAD (
    echo [ERROR] This folder is not a valid Git checkout.
    echo.
    pause
    exit /b 1
)

>LOCAL_DEV_REVISION.txt echo %SPIDEY_LOCAL_HEAD%
echo [LOCAL] Revision %SPIDEY_LOCAL_HEAD%

git diff --quiet
if errorlevel 1 (
    echo [WARNING] The local working tree has uncommitted tracked changes.
    echo [WARNING] This test will build those files, while the revision label remains HEAD.
    echo.
)

set "PATH=%SystemRoot%\System32;%SystemRoot%;%SystemRoot%\System32\Wbem;%SystemRoot%\System32\WindowsPowerShell\v1.0;%PATH%"
set "SPIDEY_POWERSHELL="
if exist "%SystemRoot%\System32\WindowsPowerShell\v1.0\powershell.exe" set "SPIDEY_POWERSHELL=%SystemRoot%\System32\WindowsPowerShell\v1.0\powershell.exe"
if not defined SPIDEY_POWERSHELL if exist "%SystemRoot%\Sysnative\WindowsPowerShell\v1.0\powershell.exe" set "SPIDEY_POWERSHELL=%SystemRoot%\Sysnative\WindowsPowerShell\v1.0\powershell.exe"
if not defined SPIDEY_POWERSHELL if exist "%SystemRoot%\SysWOW64\WindowsPowerShell\v1.0\powershell.exe" set "SPIDEY_POWERSHELL=%SystemRoot%\SysWOW64\WindowsPowerShell\v1.0\powershell.exe"

if not defined SPIDEY_POWERSHELL (
    echo [ERROR] Windows PowerShell could not be found.
    pause
    exit /b 1
)

"%SPIDEY_POWERSHELL%" -NoProfile -ExecutionPolicy Bypass -File "%~dp0tools\TEST_LATEST_BUILD.ps1" -PostUpdate
exit /b %ERRORLEVEL%
