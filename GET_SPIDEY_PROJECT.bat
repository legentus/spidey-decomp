@echo off
setlocal EnableExtensions
title Spider-Man 2000 Dev - Get / Update Project

echo ============================================================
echo   Spider-Man 2000 Developer Project - Bootstrap
echo ============================================================
echo.
echo This BAT will:
echo   1. Find or install Git for Windows automatically
echo   2. Clone or update legentus/spidey-decomp
echo   3. Switch to the active dev branch
echo   4. Start first-time game-folder setup if needed
echo.

call :FIND_GIT
if defined GIT_EXE goto :HAVE_GIT

echo [..] Git was not found. Installing Git for Windows automatically...
call :INSTALL_GIT
if errorlevel 1 goto :FAIL

call :FIND_GIT
if not defined GIT_EXE (
    echo [ERROR] Git installation completed but git.exe still could not be located.
    goto :FAIL
)

:HAVE_GIT
echo [OK] Git:
echo   %GIT_EXE%
echo.

set "DEFAULT_DIR=%USERPROFILE%\Documents\Spider-Man-2000-Dev"
set "TARGET_DIR="
set /p "TARGET_DIR=Install folder [%DEFAULT_DIR%]: "
if not defined TARGET_DIR set "TARGET_DIR=%DEFAULT_DIR%"
set "TARGET_DIR=%TARGET_DIR:"=%"

echo.
echo Target:
echo   %TARGET_DIR%
echo.

if exist "%TARGET_DIR%\.git" goto :UPDATE_EXISTING

if exist "%TARGET_DIR%" (
    dir /b "%TARGET_DIR%" 2>nul | findstr . >nul
    if not errorlevel 1 (
        echo [ERROR] Target folder already exists and is not empty:
        echo   %TARGET_DIR%
        echo Choose a different folder.
        goto :FAIL
    )
)

echo [..] Cloning development branch...
"%GIT_EXE%" clone --branch dev --single-branch https://github.com/legentus/spidey-decomp.git "%TARGET_DIR%"
if errorlevel 1 goto :GIT_FAIL
goto :PROJECT_READY

:UPDATE_EXISTING
echo [OK] Existing project clone found.
pushd "%TARGET_DIR%"

set "DIRTY="
for /f "delims=" %%A in ('"%GIT_EXE%" status --porcelain --untracked-files=no 2^>nul') do set "DIRTY=1"
if defined DIRTY (
    echo [ERROR] Tracked local files have uncommitted changes.
    echo The updater will not overwrite or stash them automatically.
    echo.
    "%GIT_EXE%" status --short
    popd
    goto :FAIL
)

echo [..] Fetching latest dev branch...
"%GIT_EXE%" fetch origin dev
if errorlevel 1 (
    popd
    goto :GIT_FAIL
)

"%GIT_EXE%" show-ref --verify --quiet refs/heads/dev
if errorlevel 1 (
    echo [..] Creating local dev branch from origin/dev...
    "%GIT_EXE%" checkout -b dev --track origin/dev
    if errorlevel 1 (
        popd
        goto :GIT_FAIL
    )
) else (
    "%GIT_EXE%" checkout dev
    if errorlevel 1 (
        popd
        goto :GIT_FAIL
    )
)

echo [..] Fast-forwarding to origin/dev...
"%GIT_EXE%" pull --ff-only origin dev
if errorlevel 1 (
    popd
    goto :GIT_FAIL
)

"%GIT_EXE%" submodule update --init --recursive
if errorlevel 1 (
    popd
    goto :GIT_FAIL
)

popd

:PROJECT_READY
echo.
echo ============================================================
echo   PROJECT READY
echo ============================================================
echo.
echo Local project:
echo   %TARGET_DIR%
echo.

if not exist "%TARGET_DIR%\spidey_local_config.bat" (
    echo Starting first-time game-folder setup...
    echo.
    pushd "%TARGET_DIR%"
    call "SETUP_FIRST_TIME.bat"
    set "SETUP_RESULT=%ERRORLEVEL%"
    popd
    if not "%SETUP_RESULT%"=="0" goto :FAIL
) else (
    echo [OK] Game folder is already configured.
)

echo.
echo From now on, use:
echo.
echo   UPDATE_PROJECT.bat
echo   BUILD_AND_INSTALL.bat
echo   RUN_GAME.bat
echo.
echo Or open:
echo   SPIDEY_DEV_MENU.bat
echo.
pause
exit /b 0

:FIND_GIT
set "GIT_EXE="
for /f "delims=" %%G in ('where git.exe 2^>nul') do if not defined GIT_EXE set "GIT_EXE=%%G"
if not defined GIT_EXE if exist "%ProgramFiles%\Git\cmd\git.exe" set "GIT_EXE=%ProgramFiles%\Git\cmd\git.exe"
if not defined GIT_EXE if exist "%ProgramFiles(x86)%\Git\cmd\git.exe" set "GIT_EXE=%ProgramFiles(x86)%\Git\cmd\git.exe"
if not defined GIT_EXE if exist "%LocalAppData%\Programs\Git\cmd\git.exe" set "GIT_EXE=%LocalAppData%\Programs\Git\cmd\git.exe"
exit /b 0

:INSTALL_GIT
where winget.exe >nul 2>&1
if errorlevel 1 goto :GIT_FALLBACK

echo [..] Using winget to install Git for Windows...
winget install --id Git.Git -e --source winget --accept-source-agreements --accept-package-agreements --silent
call :FIND_GIT
if defined GIT_EXE exit /b 0

:GIT_FALLBACK
echo [..] winget did not provide Git. Falling back to the official Git for Windows release...
set "GIT_INSTALL_PS1=%TEMP%\spidey-install-git-%RANDOM%.ps1"

> "%GIT_INSTALL_PS1%" echo $ErrorActionPreference = 'Stop'
>>"%GIT_INSTALL_PS1%" echo [Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12
>>"%GIT_INSTALL_PS1%" echo $release = Invoke-RestMethod -Uri 'https://api.github.com/repos/git-for-windows/git/releases/latest' -Headers @{'User-Agent'='Spider-Man-2000-Dev-Bootstrap'}
>>"%GIT_INSTALL_PS1%" echo $asset = $release.assets ^| Where-Object { $_.name -match '^Git-.*-64-bit\.exe$' } ^| Select-Object -First 1
>>"%GIT_INSTALL_PS1%" echo if (-not $asset) { throw 'Could not find the current 64-bit Git for Windows installer.' }
>>"%GIT_INSTALL_PS1%" echo $installer = Join-Path $env:TEMP $asset.name
>>"%GIT_INSTALL_PS1%" echo Write-Host ('Downloading ' + $asset.name + ' ...')
>>"%GIT_INSTALL_PS1%" echo Invoke-WebRequest -Uri $asset.browser_download_url -OutFile $installer -UseBasicParsing
>>"%GIT_INSTALL_PS1%" echo $p = Start-Process -FilePath $installer -ArgumentList '/VERYSILENT','/NORESTART','/NOCANCEL','/SP-' -Wait -PassThru
>>"%GIT_INSTALL_PS1%" echo Remove-Item $installer -Force -ErrorAction SilentlyContinue
>>"%GIT_INSTALL_PS1%" echo exit $p.ExitCode

powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%GIT_INSTALL_PS1%"
set "PS_RESULT=%ERRORLEVEL%"
del /q "%GIT_INSTALL_PS1%" >nul 2>&1
if not "%PS_RESULT%"=="0" exit /b %PS_RESULT%

call :FIND_GIT
if not defined GIT_EXE exit /b 1
exit /b 0

:GIT_FAIL
echo.
echo [ERROR] Git clone/update failed.
goto :FAIL

:FAIL
echo.
echo Bootstrap did not complete.
echo.
pause
exit /b 1
