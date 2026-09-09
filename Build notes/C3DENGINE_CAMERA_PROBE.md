# C3DEngine camera handoff probe — fixed installer

The previous ZIP contained the intended probe documentation but the code patch targeted an obsolete installer name, so the live hook was never registered.

This fixed build installs the probe through the same `ScanLoadedModules()` / `Install(...)` path already proven by the CViewSystem, CSystem and CView hooks.

Target:
- `C3DEngine` RTTI confirmed at runtime.
- vtable slot `+0x40`.
- exact WHGame RVA `0x4E46AC`.

Expected log:
- `MULTIHOP hook enabled: C3DEngine camera handoff`
- `C3D-CAMERA ...`
- `C3D-CAMERA-OUT ...`

Observer-only; keep `DiagnosticRendering=false`.
