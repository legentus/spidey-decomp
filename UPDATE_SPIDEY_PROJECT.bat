@echo off
setlocal EnableExtensions
cd /d "%~dp0"

echo ============================================================
echo   Spider-Man 2000 Dev - LOCAL AUTHORITATIVE MODE
echo ============================================================
echo.
echo This project is now a real local Git checkout.
echo.
echo UPDATE_SPIDEY_PROJECT.bat is intentionally disabled so a GitHub
echo archive refresh cannot overwrite local ChatGPT development work.
echo.
echo Current authoritative folder:
echo   F:\Spider-Man 2000 Recomp\project main
echo.
echo The legacy GitHub updater still exists at:
echo   tools\UPDATE_SPIDEY_PROJECT.ps1
echo.
echo To return to the old GitHub-authoritative workflow, restore these
echo BAT files from the previous Git commit or ask ChatGPT to switch
echo the project back. No source history is lost.
echo.
pause
exit /b 0
