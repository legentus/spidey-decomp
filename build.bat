setlocal
if defined SPIDEY_MSVC_ROOT (
    set MSVCDir=%SPIDEY_MSVC_ROOT%
) else (
    set MSVCDir=C:\vs
)

set PATH=%MSDevDir%\BIN;%MSVCDir%\BIN;%PATH%

set INCLUDE=%MSVCDir%\ATL\INCLUDE;%MSVCDir%\INCLUDE;%MSVCDir%\MFC\INCLUDE;%INCLUDE%
set LIB=%MSVCDir%\LIB;%MSVCDir%\MFC\LIB;%LIB%

if defined SPIDEY_FORCE_CLEAN (
    echo [..] Forced clean build requested.
    nmake /f "spider.mak" CFG="spider - Win32 Release" CLEAN
    if errorlevel 1 exit /b %ERRORLEVEL%
)

nmake /f "spider.mak" CFG="spider - Win32 Release"
