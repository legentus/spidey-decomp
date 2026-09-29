@echo off
setlocal EnableExtensions
cd /d "%~dp0"
title Spider-Man 2000 Dev - Update Project

echo ============================================================
echo   Spider-Man 2000 Dev - Update Project
echo ============================================================
echo.

set "GIT_EXE="
for /f "delims=" %%G in ('where git.exe 2^>nul') do if not defined GIT_EXE set "GIT_EXE=%%G"
if not defined GIT_EXE if exist "%ProgramFiles%\Git\cmd\git.exe" set "GIT_EXE=%ProgramFiles%\Git\cmd\git.exe"
if not defined GIT_EXE if exist "%ProgramFiles(x86)%\Git\cmd\git.exe" set "GIT_EXE=%ProgramFiles(x86)%\Git\cmd\git.exe"
if not defined GIT_EXE if exist "%LocalAppData%\Programs\Git\cmd\git.exe" set "GIT_EXE=%LocalAppData%\Programs\Git\cmd\git.exe"
if not defined GIT_EXE if exist "%LocalAppData%\Spidey2000Dev\MinGit\cmd\git.exe" set "GIT_EXE=%LocalAppData%\Spidey2000Dev\MinGit\cmd\git.exe"

if not defined GIT_EXE (
    echo [ERROR] Git for Windows was not found.
    echo Run the latest GET_SPIDEY_PROJECT.bat bootstrap to set up portable MinGit automatically.
    goto :FAIL
)

for %%D in ("%GIT_EXE%") do set "PATH=%%~dpD;%PATH%"

if not exist ".git" (
    echo [ERROR] This folder is not a Git clone.
    echo Use the project bootstrap BAT for the first download.
    goto :FAIL
)

for /f "delims=" %%A in ('git.exe remote get-url origin 2^>nul') do set "ORIGIN_URL=%%A"
echo Repository:
echo   %CD%
echo Origin:
echo   %ORIGIN_URL%
echo.

set "DIRTY="
for /f "delims=" %%A in ('git.exe status --porcelain --untracked-files=no') do set "DIRTY=1"
if defined DIRTY (
    echo [ERROR] Tracked local files have uncommitted changes.
    echo The updater will not overwrite or stash them automatically.
    echo.
    git.exe status --short
    goto :FAIL
)

echo [..] Fetching latest dev branch...
git.exe fetch origin dev
if errorlevel 1 goto :GIT_FAIL

git.exe show-ref --verify --quiet refs/heads/dev
if errorlevel 1 (
    echo [..] Creating local dev branch from origin/dev...
    git.exe checkout -b dev --track origin/dev
    if errorlevel 1 goto :GIT_FAIL
) else (
    git.exe checkout dev
    if errorlevel 1 goto :GIT_FAIL
)

echo [..] Fast-forwarding to origin/dev...
git.exe pull --ff-only origin dev
if errorlevel 1 goto :GIT_FAIL

git.exe submodule update --init --recursive
if errorlevel 1 goto :GIT_FAIL

for /f "delims=" %%A in ('git.exe rev-parse --short HEAD') do set "CURRENT_COMMIT=%%A"

echo.
echo [OK] Project is up to date.
echo Commit:
echo   %CURRENT_COMMIT%
echo.
echo Next: run BUILD_AND_INSTALL.bat.
echo.

if not defined SPIDEY_NO_PAUSE pause
exit /b 0

:GIT_FAIL
echo.
echo [ERROR] Git update failed.
goto :FAIL

:FAIL
echo.
if not defined SPIDEY_NO_PAUSE pause
exit /b 1
