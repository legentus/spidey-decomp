@echo off
setlocal
cd /d "%~dp0"
call "%~dp0UPDATE_SPIDEY_PROJECT.bat"
exit /b %ERRORLEVEL%
