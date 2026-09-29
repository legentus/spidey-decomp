@echo off
setlocal
cd /d "%~dp0"
call "%~dp0TEST_LATEST_BUILD.bat"
exit /b %ERRORLEVEL%
