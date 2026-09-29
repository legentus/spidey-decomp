@echo off
setlocal EnableExtensions EnableDelayedExpansion
title Spider-Man 2000 Dev - Get / Update Project

echo ============================================================
echo   Spider-Man 2000 Developer Project - Bootstrap
echo ============================================================
echo.
echo This BAT will:
echo   1. Find Git or set up a private portable MinGit copy
echo   2. Clone or update legentus/spidey-decomp
echo   3. Switch to the active dev branch
echo   4. Start first-time game-folder setup if needed
echo.

call :FIND_GIT
if defined GIT_EXE goto :HAVE_GIT

echo [..] No usable Git installation found.
echo [..] Setting up private portable MinGit - no admin rights required...
call :INSTALL_PORTABLE_GIT
if errorlevel 1 goto :FAIL

call :FIND_GIT
if not defined GIT_EXE (
    echo [ERROR] Portable MinGit setup completed but git.exe was not found.
    goto :FAIL
)

:HAVE_GIT
for %%D in ("%GIT_EXE%") do set "PATH=%%~dpD;%PATH%"
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
git.exe clone --branch dev --single-branch https://github.com/legentus/spidey-decomp.git "%TARGET_DIR%"
if errorlevel 1 goto :GIT_FAIL
goto :PROJECT_READY

:UPDATE_EXISTING
echo [OK] Existing project clone found.
pushd "%TARGET_DIR%"

set "DIRTY="
for /f "delims=" %%A in ('git.exe status --porcelain --untracked-files=no 2^>nul') do set "DIRTY=1"
if defined DIRTY (
    echo [ERROR] Tracked local files have uncommitted changes.
    echo The updater will not overwrite or stash them automatically.
    echo.
    git.exe status --short
    popd
    goto :FAIL
)

echo [..] Fetching latest dev branch...
git.exe fetch origin dev
if errorlevel 1 (
    popd
    goto :GIT_FAIL
)

git.exe show-ref --verify --quiet refs/heads/dev
if errorlevel 1 (
    echo [..] Creating local dev branch from origin/dev...
    git.exe checkout -b dev --track origin/dev
    if errorlevel 1 (
        popd
        goto :GIT_FAIL
    )
) else (
    git.exe checkout dev
    if errorlevel 1 (
        popd
        goto :GIT_FAIL
    )
)

echo [..] Fast-forwarding to origin/dev...
git.exe pull --ff-only origin dev
if errorlevel 1 (
    popd
    goto :GIT_FAIL
)

git.exe submodule update --init --recursive
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
    if not "!SETUP_RESULT!"=="0" goto :FAIL
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
if not defined GIT_EXE if exist "%LocalAppData%\Spidey2000Dev\MinGit\cmd\git.exe" set "GIT_EXE=%LocalAppData%\Spidey2000Dev\MinGit\cmd\git.exe"
exit /b 0

:INSTALL_PORTABLE_GIT
call :REQUIRE_TOOL curl.exe
if errorlevel 1 exit /b 1
call :REQUIRE_TOOL certutil.exe
if errorlevel 1 exit /b 1
call :REQUIRE_TOOL tar.exe
if errorlevel 1 exit /b 1

set "PORTABLE_ROOT=%LocalAppData%\Spidey2000Dev"
set "PORTABLE_GIT=%PORTABLE_ROOT%\MinGit"
set "PORTABLE_ZIP=%TEMP%\spidey-MinGit-2.56.0-64-bit.zip"
set "PORTABLE_URL=https://github.com/git-for-windows/git/releases/download/v2.56.0.windows.1/MinGit-2.56.0-64-bit.zip"
set "PORTABLE_SHA256=064B440FF870ED5198527E8F3A92CDF5BD2FD0FEDF5E718AF95E3FDADDEFF718"

if exist "%PORTABLE_GIT%\cmd\git.exe" (
    echo [OK] Portable MinGit is already present.
    exit /b 0
)

if not exist "%PORTABLE_ROOT%" mkdir "%PORTABLE_ROOT%" >nul 2>&1

echo [..] Downloading official portable MinGit...
curl.exe -L --fail --retry 3 --retry-delay 2 -o "%PORTABLE_ZIP%" "%PORTABLE_URL%"
if errorlevel 1 (
    echo [ERROR] MinGit download failed.
    exit /b 1
)

echo [..] Verifying SHA-256...
set "ACTUAL_HASH="
for /f "tokens=* delims=" %%H in ('certutil.exe -hashfile "%PORTABLE_ZIP%" SHA256 ^| findstr /R /I "^[0-9A-F][0-9A-F][0-9A-F][0-9A-F][0-9A-F][0-9A-F][0-9A-F][0-9A-F]"') do if not defined ACTUAL_HASH set "ACTUAL_HASH=%%H"
set "ACTUAL_HASH=!ACTUAL_HASH: =!"
if /I not "!ACTUAL_HASH!"=="%PORTABLE_SHA256%" (
    echo [ERROR] MinGit SHA-256 did not match.
    echo Expected:
    echo   %PORTABLE_SHA256%
    echo Actual:
    echo   !ACTUAL_HASH!
    del /q "%PORTABLE_ZIP%" >nul 2>&1
    exit /b 1
)

if exist "%PORTABLE_GIT%" rmdir /s /q "%PORTABLE_GIT%"
mkdir "%PORTABLE_GIT%" >nul 2>&1

echo [..] Extracting portable MinGit...
tar.exe -xf "%PORTABLE_ZIP%" -C "%PORTABLE_GIT%"
if errorlevel 1 (
    echo [ERROR] MinGit extraction failed.
    exit /b 1
)

del /q "%PORTABLE_ZIP%" >nul 2>&1

if not exist "%PORTABLE_GIT%\cmd\git.exe" (
    echo [ERROR] Extraction finished but cmd\git.exe was not found.
    exit /b 1
)

echo [OK] Portable MinGit ready:
echo   %PORTABLE_GIT%
exit /b 0

:REQUIRE_TOOL
where %~1 >nul 2>&1
if errorlevel 1 (
    echo [ERROR] Required Windows tool was not found: %~1
    exit /b 1
)
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
