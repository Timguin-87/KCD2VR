@echo off
cd /d "%~dp0"
call "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat" >nul
cl /nologo /std:c++17 /EHsc /Od /Zi /I ../src /I ../src/vrik grab_backend60_test.cpp /Fe:grab_backend60.exe >grab_backend60_compile.log 2>&1
if errorlevel 1 exit /b 1
grab_backend60.exe >grab_backend60.log 2>&1
if errorlevel 1 exit /b 1
cl /nologo /std:c++17 /EHsc /Od /Zi /fsanitize=address /I ../src /I ../src/vrik grab_backend60_test.cpp /Fe:grab_backend60_asan.exe /link /INCREMENTAL:NO >grab_backend60_asan_compile.log 2>&1
if errorlevel 1 exit /b 1
grab_backend60_asan.exe >grab_backend60_asan.log 2>&1
exit /b %errorlevel%
