@echo off
cd /d "%~dp0"
call "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat" >nul
cl /nologo /std:c++17 /EHsc /Od /Zi /I ../src /I ../src/vrik grab_connected_fixture_test.cpp /Fe:grab_proxy60.exe >grab_proxy60_compile.log 2>&1
if errorlevel 1 exit /b 1
grab_proxy60.exe >grab_proxy60.log 2>&1
if errorlevel 1 exit /b 1
cl /nologo /std:c++17 /EHsc /Od /Zi /fsanitize=address /I ../src /I ../src/vrik grab_connected_fixture_test.cpp /Fe:grab_proxy60_asan.exe /link /INCREMENTAL:NO >grab_proxy60_asan_compile.log 2>&1
if errorlevel 1 exit /b 1
grab_proxy60_asan.exe >grab_proxy60_asan.log 2>&1
exit /b %errorlevel%
