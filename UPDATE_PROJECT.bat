@echo off
setlocal EnableExtensions
cd /d "%~dp0"
title Spider-Man 2000 Dev - Update Project

echo ============================================================
echo   Spider-Man 2000 Dev - Update Project
echo ============================================================
echo.

where git.exe >nul 2>&1
if errorlevel 1 (
    echo [ERROR] Git for Windows was not found in PATH.
    goto :FAIL
)

if not exist ".git" (
    echo [ERROR] This folder is not a Git clone.
    echo Use the project bootstrap BAT for the first download.
    goto :FAIL
)

for /f "delims=" %%A in ('git remote get-url origin 2^>nul') do set "ORIGIN_URL=%%A"
echo Repository:
echo   %CD%
echo Origin:
echo   %ORIGIN_URL%
echo.

set "DIRTY="
for /f "delims=" %%A in ('git status --porcelain --untracked-files=no') do set "DIRTY=1"
if defined DIRTY (
    echo [ERROR] Tracked local files have uncommitted changes.
    echo The updater will not overwrite or stash them automatically.
    echo.
    git status --short
    goto :FAIL
)

echo [..] Fetching latest dev branch...
git fetch origin dev
if errorlevel 1 goto :GIT_FAIL

git show-ref --verify --quiet refs/heads/dev
if errorlevel 1 (
    echo [..] Creating local dev branch from origin/dev...
    git checkout -b dev --track origin/dev
    if errorlevel 1 goto :GIT_FAIL
) else (
    git checkout dev
    if errorlevel 1 goto :GIT_FAIL
)

echo [..] Fast-forwarding to origin/dev...
git pull --ff-only origin dev
if errorlevel 1 goto :GIT_FAIL

git submodule update --init --recursive
if errorlevel 1 goto :GIT_FAIL

for /f "delims=" %%A in ('git rev-parse --short HEAD') do set "CURRENT_COMMIT=%%A"

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
