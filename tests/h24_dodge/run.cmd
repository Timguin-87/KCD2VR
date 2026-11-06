@echo off
setlocal
if "%~1"=="" exit /b 2
call "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat" >nul
if not exist "%~1" mkdir "%~1"
pushd "%~1"
cl /nologo /std:c++17 /EHsc /UNDEBUG /Od /Zi /MD /Fd:dodge_debug.pdb /Fo:dodge_debug.obj /Fe:dodge_debug.exe "%~dp0context_test.cpp" /link /INCREMENTAL:NO >debug_compile.log 2>&1
if errorlevel 1 exit /b 1
dodge_debug.exe >debug_result.log 2>&1
if errorlevel 1 exit /b 1
cl /nologo /std:c++17 /EHsc /UNDEBUG /Od /Zi /MD /fsanitize=address /Fd:dodge_asan.pdb /Fo:dodge_asan.obj /Fe:dodge_asan.exe "%~dp0context_test.cpp" /link /INCREMENTAL:NO >asan_compile.log 2>&1
if errorlevel 1 exit /b 1
dodge_asan.exe >asan_result.log 2>&1
set result=%errorlevel%
popd
exit /b %result%
