@echo off
setlocal
cd /d "%~dp0"
set MARKER=KCD2VR-V62.9-LUA-UI-VRIK-LADDER

if "%DETOURMODKIT_ROOT%"=="" (
  echo ERROR: Set DETOURMODKIT_ROOT to the extracted DetourModKit MSVC package.
  echo NOTE: vcpkg is NOT required. OpenXR is found locally or fetched by CMake.
  exit /b 1
)

echo [V62.9] FORCE CLEAN build - %MARKER%
if exist build rmdir /s /q build
if exist build ( echo ERROR: build directory could not be removed. & exit /b 1 )

cmake -B build -S . -DCMAKE_PREFIX_PATH="%DETOURMODKIT_ROOT%" %*
if errorlevel 1 exit /b %errorlevel%
cmake --build build --config RelWithDebInfo --clean-first
if errorlevel 1 exit /b %errorlevel%

set ASI=build\release\RelWithDebInfo\KCD2VR.asi
if not exist "%ASI%" set ASI=build\release\KCD2VR.asi
if not exist "%ASI%" ( echo ERROR: freshly built KCD2VR.asi not found. & exit /b 1 )

echo.
echo [V62.9] Fresh ASI: %ASI%
findstr /m /c:"%MARKER%" "%ASI%" >nul
if errorlevel 1 ( echo ERROR: V62.9 build marker NOT found in ASI. Do not copy this binary. & exit /b 1 )
echo [V62.9] VERIFIED marker %MARKER% in fresh ASI.
echo CMake copies MIS_MENU_BOOK.bmp, KCD2VR.ini and KCD2VR_UI_LIVE.pak beside the fresh ASI.
echo KCD2VR-diagnostics.log will be created next to the ASI at startup.
