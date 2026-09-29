@echo off
setlocal EnableExtensions EnableDelayedExpansion
cd /d "%~dp0"
title Spider-Man 2000 Dev - Build

echo ============================================================
echo   Spider-Man 2000 Dev - Build Matching Proxy
echo ============================================================
echo.

set "TOOLCHAIN_ROOT=%LocalAppData%\Spidey2000Dev\MatchingVS"
set "TOOLCHAIN_ZIP=%TEMP%\spidey-vs-v1.0.zip"
set "TOOLCHAIN_URL=https://github.com/krystalgamer/spidey-decomp-vs/releases/download/v1.0/spidey-vs.zip"

if not exist "%TOOLCHAIN_ROOT%\BIN\nmake.exe" (
    echo [..] Matching compiler toolchain is not installed locally.
    call :REQUIRE_TOOL curl.exe
    if errorlevel 1 goto :FAIL
    call :REQUIRE_TOOL tar.exe
    if errorlevel 1 goto :FAIL

    if exist "%TOOLCHAIN_ROOT%" rmdir /s /q "%TOOLCHAIN_ROOT%"
    mkdir "%TOOLCHAIN_ROOT%" >nul 2>&1

    echo [..] Downloading preserved matching compiler toolchain...
    curl.exe -L --fail --retry 3 --retry-delay 2 -o "%TOOLCHAIN_ZIP%" "%TOOLCHAIN_URL%"
    if errorlevel 1 (
        echo [ERROR] Toolchain download failed.
        goto :FAIL
    )

    echo [..] Extracting matching compiler toolchain...
    tar.exe -xf "%TOOLCHAIN_ZIP%" -C "%TOOLCHAIN_ROOT%"
    if errorlevel 1 (
        echo [ERROR] Toolchain extraction failed.
        goto :FAIL
    )
    del /q "%TOOLCHAIN_ZIP%" >nul 2>&1
)

if not exist "%TOOLCHAIN_ROOT%\BIN\nmake.exe" (
    echo [ERROR] Matching compiler setup finished but BIN\nmake.exe was not found.
    goto :FAIL
)

echo [OK] Matching toolchain:
echo   %TOOLCHAIN_ROOT%
echo.

call :FIND_GIT
set "VERSION=LOCAL"
if defined GIT_EXE (
    for /f "delims=" %%A in ('git.exe rev-parse HEAD 2^>nul') do set "VERSION=%%A"
)

set "RUNTIME_HEADER=%CD%\runtime_version.h"
set "RUNTIME_BACKUP=%TEMP%\spidey-runtime-version-%RANDOM%-%RANDOM%.bak"
set "HAD_RUNTIME=0"
if exist "%RUNTIME_HEADER%" (
    copy /y "%RUNTIME_HEADER%" "%RUNTIME_BACKUP%" >nul
    set "HAD_RUNTIME=1"
)

> "%RUNTIME_HEADER%" echo #define RUNTIME_VERSION "!VERSION!"

echo Version:
echo   !VERSION!
echo.

set "SPIDEY_MSVC_ROOT=%TOOLCHAIN_ROOT%"
call "%CD%\build.bat"
set "BUILD_RESULT=%ERRORLEVEL%"

if "!HAD_RUNTIME!"=="1" (
    copy /y "%RUNTIME_BACKUP%" "%RUNTIME_HEADER%" >nul
    del /q "%RUNTIME_BACKUP%" >nul 2>&1
) else (
    del /q "%RUNTIME_HEADER%" >nul 2>&1
)

if not "%BUILD_RESULT%"=="0" (
    echo.
    echo [ERROR] Matching build failed with exit code %BUILD_RESULT%.
    goto :FAIL
)

if not exist "%CD%\Release\spider.dll" (
    echo [ERROR] Build returned success but Release\spider.dll was not produced.
    goto :FAIL
)

if not exist "%CD%\out\matching" mkdir "%CD%\out\matching" >nul 2>&1
copy /y "%CD%\Release\spider.dll" "%CD%\out\matching\binkw32.dll" >nul
if exist "%CD%\Release\spider.pdb" copy /y "%CD%\Release\spider.pdb" "%CD%\out\matching\spider.pdb" >nul

echo.
echo ============================================================
echo   BUILD SUCCESS
echo ============================================================
echo Artifact:
echo   %CD%\out\matching\binkw32.dll
echo.
where certutil.exe >nul 2>&1
if not errorlevel 1 (
    echo SHA-256:
    certutil.exe -hashfile "%CD%\out\matching\binkw32.dll" SHA256
)
echo.
if not defined SPIDEY_NO_PAUSE pause
exit /b 0

:FIND_GIT
set "GIT_EXE="
for /f "delims=" %%G in ('where git.exe 2^>nul') do if not defined GIT_EXE set "GIT_EXE=%%G"
if not defined GIT_EXE if exist "%ProgramFiles%\Git\cmd\git.exe" set "GIT_EXE=%ProgramFiles%\Git\cmd\git.exe"
if not defined GIT_EXE if exist "%ProgramFiles(x86)%\Git\cmd\git.exe" set "GIT_EXE=%ProgramFiles(x86)%\Git\cmd\git.exe"
if not defined GIT_EXE if exist "%LocalAppData%\Programs\Git\cmd\git.exe" set "GIT_EXE=%LocalAppData%\Programs\Git\cmd\git.exe"
if not defined GIT_EXE if exist "%LocalAppData%\Spidey2000Dev\MinGit\cmd\git.exe" set "GIT_EXE=%LocalAppData%\Spidey2000Dev\MinGit\cmd\git.exe"
if defined GIT_EXE for %%D in ("%GIT_EXE%") do set "PATH=%%~dpD;%PATH%"
exit /b 0

:REQUIRE_TOOL
where %~1 >nul 2>&1
if errorlevel 1 (
    echo [ERROR] Required Windows tool was not found: %~1
    exit /b 1
)
exit /b 0

:FAIL
echo.
echo [ERROR] Build did not complete.
if not defined SPIDEY_NO_PAUSE pause
exit /b 1
