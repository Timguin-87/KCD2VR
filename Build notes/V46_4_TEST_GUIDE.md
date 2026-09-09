# MIS V46.4 test guide

## Build
Use a fresh build folder:

```bat
rmdir /s /q build
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=E:/backup/vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_PREFIX_PATH=E:/backup/DetourModKit
cmake --build build --config RelWithDebInfo
```

## Test order
1. Start KCD2 and stay in the main menu first.
2. Confirm the headset view starts directly in the old Phase-7 geometry: mono, correct-looking projection, still likely soft because the captured source is 1920x1080.
3. Press F7 and verify clicks/slider drags line up with the visible cursor.
4. Close F7.
5. Press F8 once. Each phase lasts 5 seconds. Note which phase numbers have convincing depth, wrong/reversed depth, excessive separation, or remain mono.
6. At the end V46.4 automatically restores GOOD_MONO_XRFOV_110.
7. Send the debug log and your phase observations.

If anything becomes uncomfortable or unstable, F7 -> Good Mono 110° cancels the ladder and returns to the known-good mono baseline.
