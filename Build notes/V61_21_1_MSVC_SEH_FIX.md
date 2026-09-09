# V61.21.1 MSVC SEH build fix

Source-only repair of V61.21.

Changes:
- Moved the `__try/__except` read of `WHGame+0x492D938` out of `flashui_probe::Install()` into a trivial helper with no C++ objects requiring unwinding. This fixes MSVC C2712.
- Explicitly discard the `[[nodiscard]]` return from hook `disable()` in FlashUI and FRIK uninstall paths, fixing the C4834 warnings shown by the test build.
- No runtime logic, offsets, HUD behavior, FRIK behavior, stereo behavior, camera behavior, culling, input, or DOF behavior changed.

Expected result: the previous `flashui_probe.cpp(49,6): error C2712` is gone.
