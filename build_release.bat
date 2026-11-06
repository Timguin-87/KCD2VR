@echo off
setlocal EnableExtensions
cd /d "%~dp0"
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0tools\build_release.ps1" %*
set "BUILD_EXIT=%errorlevel%"
if not "%BUILD_EXIT%"=="0" (
  echo Build failed. See build_release.log.
  if not defined KCD2VR_NO_PAUSE pause
)
exit /b %BUILD_EXIT%
