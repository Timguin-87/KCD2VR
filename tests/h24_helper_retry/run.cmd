@echo off
setlocal
if "%~1"=="" exit /b 2
call "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat" >nul
if not exist "%~1" mkdir "%~1"
pushd "%~1"
cl /nologo /std:c++17 /EHsc /I"%~dp0../../src" /UNDEBUG /Od /Zi /MD /Fd:retry_debug.pdb /Fo:retry_debug.obj /Fe:retry_debug.exe "%~dp0retry_test.cpp" /link /INCREMENTAL:NO >debug_compile.log 2>&1
if errorlevel 1 exit /b 1
retry_debug.exe >debug_result.log 2>&1
if errorlevel 1 exit /b 1
cl /nologo /std:c++17 /EHsc /I"%~dp0../../src" /UNDEBUG /Od /Zi /MD /fsanitize=address /Fd:retry_asan.pdb /Fo:retry_asan.obj /Fe:retry_asan.exe "%~dp0retry_test.cpp" /link /INCREMENTAL:NO >asan_compile.log 2>&1
if errorlevel 1 exit /b 1
retry_asan.exe >asan_result.log 2>&1
set result=%errorlevel%
popd
exit /b %result%

