# V46.3 — MIS Balanced VR Test Build

This is the recommended next test build. It retains the complete V46.1 10×5-second matrix and V46.2 in-HMD control menu, but adds only **low-risk, reversible VR quality settings**.

## First launch

1. Build from a **fresh CMake build directory** with the same Windows/MSVC + DetourModKit/OpenXR setup used for previous versions.
2. Do **not** add `autoexec.cfg` or `user.cfg` overrides for this test.
3. Load normal gameplay.
4. The default `MIS VR Balanced graphics` profile is enabled automatically when CryEngine's console becomes available.
5. Press **F7** for the *Medieval Immersion Simulator* panel.
6. Press **F8** once (or use `Run 50 s matrix`) for the existing mono/FOV/CustomRes/eye-routing/Recursive View matrix.
7. Send the resulting `KCD2VR-debug*.log`.

## Safe quality profile applied by default

The profile snapshots the current game values first, then uses:

- `e_ViewDistRatio = max(original, 120)`
- `e_ViewDistRatioVegetation = max(original, 120)`
- `e_MergedMeshesViewDistRatio = max(original, 120)`
- `r_MotionBlur = 0`
- `r_Sharpening = max(original, VrEngineClarity)`; default `VrEngineClarity=0.18`

The 120 distance floor is deliberately conservative: Warhorse ships High values around 140 and higher presets up to 220/280 in `Engine.pak`. If the user already runs a higher preset, MIS never lowers it.

## Deliberately NOT enabled yet

V46.3 does **not** automatically change:

- `e_SkipInFrustumOptimizationWH`
- occlusion/coverage-buffer behavior
- shadow cascades or shadow reprojection
- TAA/TSAA history or jitter
- SSR / screen-space shadows
- HLOD internals / LOD ratio
- volumetric-cloud stereo flags
- custom CAS shader

Those can materially affect performance or hide a stereo-renderer bug, so they remain later diagnosis phases after the view path is established.

## Menu additions

The F7 menu now exposes:

- **MIS VR Balanced graphics** — live on/off, with runtime restoration
- **OpenXR render scale** — saved in `KCD2VR.ini`; restart required because XR swapchains are created at session initialization
- **Engine clarity** — live `r_Sharpening` target, 0.00–0.75

Internal DLL/project names stay `KCD2VR` for compatibility; the visible UI uses **Medieval Immersion Simulator**.
