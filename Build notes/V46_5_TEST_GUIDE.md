# V46.5 Test Guide

## 1. Build fresh
Use the included build script or:

```bat
rmdir /s /q build
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=E:/backup/vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_PREFIX_PATH=E:/backup/DetourModKit
cmake --build build --config RelWithDebInfo
```

## 2. Menu input first
At the KCD2 main menu press F7.

Try mouse clicks before F8. Also try keyboard fallback:
- Up / Down = select
- Left / Right = change slider/selector
- Enter = activate
- F7 or Escape = close

The log should contain `V46.5-MENU-MOUSE down mapped=(x,y)` for mouse presses inside the parchment panel.

## 3. Tune FOV before stereo
Use `VR FOV (game + compositor)` until mono scale feels comfortable. V46.5 attempts to set CryEngine `cl_fov` to the same value while keeping OpenXR FOV remap active.

## 4. F8 stereo diagnostic
F8 runs 30 seconds total:
1. MONO_CURRENT_FOV
2. AFR_QUARTER_IPD
3. AFR_HALF_IPD
4. AFR_HALF_IPD_SWAPPED
5. SEQUENTIAL_REENTRY_OBSERVER (mono fallback)
6. NATIVE_REENTRY_OBSERVER (mono fallback)

Report which phase has the least zoom/warp and whether 0.25x or 0.50x gives more believable depth. AFR ghosting is expected and is not a target to polish.

Important log checks:
- AFR should show both `capturedEye=L` and `capturedEye=R` over time.
- AFR log should show `xrFov=true`.
- Phase-end lines show `pipeline={normal:... recursive:...}`.

After phase 6 the build restores Mono + OpenXR FOV at the currently selected FOV.
