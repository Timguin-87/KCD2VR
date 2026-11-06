@echo off
setlocal EnableExtensions
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0tools\verify_release_payload.ps1" %*
set "VERIFY_EXIT=%errorlevel%"
if not defined KCD2VR_NO_PAUSE pause
exit /b %VERIFY_EXIT%
