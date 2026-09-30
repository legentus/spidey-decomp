@echo off
setlocal EnableExtensions
cd /d "%~dp0"

echo ============================================================
echo   Spider-Man 2000 Dev - Update + Build/Test
echo ============================================================
echo.
echo [1/2] Updating project...
echo.

call "%~dp0UPDATE_SPIDEY_PROJECT.bat"
if errorlevel 1 (
    echo.
    echo ============================================================
    echo   UPDATE FAILED
    echo ============================================================
    echo.
    echo [ERROR] Project update failed. Build/test was not started.
    echo.
    pause
    exit /b 1
)

cd /d "%~dp0"

echo.
echo [OK] Project update completed.
echo.
echo [2/2] Building and launching latest test...
echo.

call "%~dp0TEST_LATEST_BUILD.bat"
set "SPIDEY_RESULT=%ERRORLEVEL%"

if not "%SPIDEY_RESULT%"=="0" (
    echo.
    echo ============================================================
    echo   BUILD/TEST FAILED
    echo ============================================================
    echo.
    echo [ERROR] TEST_LATEST_BUILD.bat exited with code %SPIDEY_RESULT%.
    echo.
    pause
    exit /b %SPIDEY_RESULT%
)

exit /b 0
