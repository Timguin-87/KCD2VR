@echo off
call "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat" >nul
cd /d "D:\KCD2VR-review-builds\H19_COMBAT_RECOVERY_20261020\source\tests\h19_combat"
cl /nologo /std:c++17 /EHsc /UNDEBUG /Od /Zi /MDd /D_DEBUG /I"D:\KCD2VR-review-builds\H19_COMBAT_RECOVERY_20261020\source/src" /Fd:slot_debug.pdb /Fo:slot_debug.obj /Fe:slot_debug.exe "native_slot_h19_test.cpp" /link /INCREMENTAL:NO
if errorlevel 1 exit /b 1
slot_debug.exe
if errorlevel 1 exit /b 1
cl /nologo /std:c++17 /EHsc /UNDEBUG /Od /Zi /MDd /D_DEBUG /I"D:\KCD2VR-review-builds\H19_COMBAT_RECOVERY_20261020\source/src" /Fd:outer_debug.pdb /Fo:outer_debug.obj /Fe:outer_debug.exe "actual_outer_test.cpp" /link /INCREMENTAL:NO
if errorlevel 1 exit /b 1
outer_debug.exe
if errorlevel 1 exit /b 1
cl /nologo /std:c++17 /EHsc /UNDEBUG /Od /Zi /MDd /D_DEBUG /I"D:\KCD2VR-review-builds\H19_COMBAT_RECOVERY_20261020\source/src" /Fd:policy_debug.pdb /Fo:policy_debug.obj /Fe:policy_debug.exe "../../tests/counter_stab40_test.cpp" /link /INCREMENTAL:NO
if errorlevel 1 exit /b 1
policy_debug.exe
if errorlevel 1 exit /b 1
