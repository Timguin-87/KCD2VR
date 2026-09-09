# KCD2VR V48 — Quad UI + Late Pose

## What changed
- F6 now switches flat presentation to a real `XrCompositionLayerQuad` in VIEW space.
- F7 MIS menu automatically uses the same quad.
- Quad starts 1.60 m forward and 1.05 m wide, visible to both eyes.
- Gameplay stays on the existing projection layer when the quad is off.
- Camera frustum consumption can refresh HMD pose using `XR_KHR_win32_convert_performance_counter_time` + `xrLocateSpace` near camera use.
- If the runtime does not expose that extension, V48 falls back to the prior frame pose without failing VR.

## Test order
1. Start game and inspect the first menus.
2. F6: verify flat content becomes one comfortable binocular panel, not two separated images.
3. F7: open MIS menu and verify the same; close it and verify world projection returns.
4. Load game, walk and rotate head. Compare jitter with V47.
5. Press F8 only after those observations; diagnostics are retained.

## Logs to look for
- `V48-LATE-POSE QPC->XrTime extension ready`
- `V48-LATE-POSE samples=... failures=...`
- `V48-QUAD-UI ON distance=1.60m width=1.05m eyeVisibility=BOTH`

## Build
Use the same Windows DetourModKit/vcpkg toolchain and `build_release.bat` as V47.
