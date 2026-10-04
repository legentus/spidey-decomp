@echo off
setlocal EnableExtensions
cd /d "%~dp0"
title Spider-Man 2000 - Play Current Installed Build

echo ============================================================
echo   Spider-Man 2000 Dev - Play Current Installed Build
echo ============================================================
echo.
echo [INFO] This launcher does NOT:
echo        - git pull / fetch
echo        - download anything
echo        - rebuild the project
echo        - replace installed DLLs
echo.
echo [INFO] It launches only the build already installed in the
echo        Spider-Man game folder, but starts a fresh diagnostic log.
echo.

if exist "spidey_local_config.bat" (
    call "spidey_local_config.bat"
)

if not defined SPIDEY_GAME_DIR (
    set "SPIDEY_GAME_DIR=C:\Program Files (x86)\Activision\Spider-Man"
)

if not exist "%SPIDEY_GAME_DIR%\SpideyPC.exe" (
    echo [INFO] SpideyPC.exe was not found at:
    echo   %SPIDEY_GAME_DIR%
    echo.
    set "SPIDEY_GAME_DIR="
    set /p "SPIDEY_GAME_DIR=Enter the folder containing SpideyPC.exe: "
    set "SPIDEY_GAME_DIR=%SPIDEY_GAME_DIR:"=%"
)

if not defined SPIDEY_GAME_DIR (
    echo [ERROR] No game folder was provided.
    pause
    exit /b 1
)

if not exist "%SPIDEY_GAME_DIR%\SpideyPC.exe" (
    echo [ERROR] SpideyPC.exe was not found in:
    echo   %SPIDEY_GAME_DIR%
    pause
    exit /b 1
)

> "spidey_local_config.bat" echo @echo off
>>"spidey_local_config.bat" echo set "SPIDEY_GAME_DIR=%SPIDEY_GAME_DIR%"

set "SPIDEY_LOG=%SPIDEY_GAME_DIR%\spidey-decomp.log"

echo [INFO] Clearing stale diagnostic logs...
del /q "%SPIDEY_LOG%" 2>nul
for %%L in (
    spidey-decomp-crash.log
    spidey-decomp-dxerror.log
    spidey-decomp-compat.log
    spidey-decomp-present.log
    spidey-decomp-texture.log
    spidey-decomp-draw.log
    spidey-decomp-input.log
    spidey-renderer11.log
    spidey-input11.log
    spidey-decomp-camera.log
    spidey-decomp-audio.log
    spidey-decomp-timing.log
    spidey-decomp-runtime.log
) do del /q "%SPIDEY_GAME_DIR%\%%L" 2>nul

> "%SPIDEY_LOG%" echo [SESSION] launch_mode=current_installed_build
>>"%SPIDEY_LOG%" echo [SESSION] started=%date% %time%

echo.
echo [RUN] %SPIDEY_GAME_DIR%\SpideyPC.exe
echo [LOG] %SPIDEY_LOG%
echo.
start "" /wait /D "%SPIDEY_GAME_DIR%" "%SPIDEY_GAME_DIR%\SpideyPC.exe"
set "SPIDEY_EXIT=%ERRORLEVEL%"

>>"%SPIDEY_LOG%" echo [SESSION] exit_code=%SPIDEY_EXIT%
>>"%SPIDEY_LOG%" echo [SESSION] ended=%date% %time%

echo.
echo [INFO] Spider-Man exited with code %SPIDEY_EXIT%.
echo [LOG] Fresh consolidated log:
echo       %SPIDEY_LOG%
echo.
echo [UPLOAD] If something went wrong, send only spidey-decomp.log.
echo.
pause
exit /b %SPIDEY_EXIT%
