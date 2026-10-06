@echo off
setlocal EnableExtensions
cd /d "%~dp0"

echo ============================================================
echo   Spider-Man 2000 Dev - LOCAL Fast Test
echo ============================================================
echo.
echo [LOCAL] No GitHub download/update will be performed.
echo [LOCAL] Building and testing the files currently in this checkout.
echo.

call "%~dp0TEST_LATEST_BUILD.bat"
exit /b %ERRORLEVEL%
