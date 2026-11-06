@echo off
call "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat" >nul
cd /d "D:\KCD2VR-review-builds\H19_COMBAT_RECOVERY_20261020\source\tests\h19_combat"
cl /nologo /std:c++17 /EHsc /UNDEBUG /Od /Zi /MD /fsanitize=address /I"D:\KCD2VR-review-builds\H19_COMBAT_RECOVERY_20261020\source/src" /Fd:slot_asan.pdb /Fo:slot_asan.obj /Fe:slot_asan.exe "native_slot_h19_test.cpp" /link /INCREMENTAL:NO
if errorlevel 1 exit /b 1
slot_asan.exe
if errorlevel 1 exit /b 1
cl /nologo /std:c++17 /EHsc /UNDEBUG /Od /Zi /MD /fsanitize=address /I"D:\KCD2VR-review-builds\H19_COMBAT_RECOVERY_20261020\source/src" /Fd:outer_asan.pdb /Fo:outer_asan.obj /Fe:outer_asan.exe "actual_outer_test.cpp" /link /INCREMENTAL:NO
if errorlevel 1 exit /b 1
outer_asan.exe
if errorlevel 1 exit /b 1
cl /nologo /std:c++17 /EHsc /UNDEBUG /Od /Zi /MD /fsanitize=address /I"D:\KCD2VR-review-builds\H19_COMBAT_RECOVERY_20261020\source/src" /Fd:policy_asan.pdb /Fo:policy_asan.obj /Fe:policy_asan.exe "../../tests/counter_stab40_test.cpp" /link /INCREMENTAL:NO
if errorlevel 1 exit /b 1
policy_asan.exe
if errorlevel 1 exit /b 1
