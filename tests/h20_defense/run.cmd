@echo off
setlocal
if "%~1"=="" exit /b 2
call "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat" >nul
if not exist "%~1" mkdir "%~1"
pushd "%~1"
cl /nologo /std:c++17 /EHsc /Od /Zi /Fe:context_test.exe "%~dp0context_test.cpp" >context_compile.log 2>&1
if errorlevel 1 exit /b 1
context_test.exe >context_result.log 2>&1
set result=%errorlevel%
popd
exit /b %result%
