# V58.8 Build Note

Base: user-supplied `KCD2VR-scaffold (11).zip` (V58.7 + Claude A-D F8 ladder).

Changes:

1. Added F8 phase E `CRYSIS_E_NATIVE_DUAL_EXEC` using existing V54 same-frame Normal graphics-pipeline re-entry (`mode=1`).
2. Added F8 phase F `CRYSIS_F_NATIVE_SELECTOR_TAGGED` using the same re-entry with `CRenderView+0x8EE0` selector 0/1 (`mode=2`).
3. During each re-entry pass, `FrameSync::CurrentEye()` is explicitly set L/R so output-side PRESENT telemetry can attribute transitions to the active pass.
4. Phase F enables PRESENT eye tagging and the existing AFR two-cache bridge, but disables CameraHook's AFR offset so the camera is not shifted twice.
5. Added `V58.8-NATIVE-EYE-PASS` and `V58.8-NATIVE-TAGGED-BRIDGE` telemetry.
6. Fixed a duplicate `SetAngularCorrectMono` / `GetAngularCorrectMono` declaration present in the supplied scaffold header, which would otherwise be an MSVC redefinition error.

Important limitation: the final KCD2 PRESENT may still be a single resource written after both engine passes. If so, phase F cannot manufacture true stereo; the test is designed to make that failure explicit in logs.
