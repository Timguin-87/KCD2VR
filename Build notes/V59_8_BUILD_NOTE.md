# KCD2VR V59.8 — FOV Sync + Cyclopean Correction

Build ID: `FOV-SYNC-CYCLO-20260905-B`

## Purpose

V59.8 fixes the live FOV calibration bug: applying `cl_fov` from the MIS menu now also synchronizes the mono remap's `VrSourceVerticalFovDegrees` assumption to the confirmed CVar readback. Startup also performs a read-only sync from the current native `cl_fov`; it does not pulse or rewrite the game FOV.

The F8 ladder is replaced with a single-variable cyclopean calibration. It does **not** claim geometric stereo. The captured KCD2 PRESENT remains mono; only the submitted per-eye `XrFovf` asymmetry is varied around a common cyclopean pose.

## F8 phases

Each phase lasts 8 seconds. World/presentation settings are otherwise fixed: tangent remap, zoom 0.95, fixed 8 ms rotation filter, render-paired common pose, no UV binocular offset, no rotation reprojection, no dual pipeline Execute.

1. `C98_0_COMMON` — asymmetry 0.00, centered reference
2. `C98_P025` — +0.25
3. `C98_P050` — +0.50
4. `C98_P075` — +0.75
5. `C98_P100` — +1.00
6. `C98_N025` — -0.25 polarity check
7. `C98_N050` — -0.50 polarity check

Every phase logs `GAME_FOV`, `SOURCE_FOV`, and `MATCHED`.

## Recommended baseline

Keep the known-good source resolution external:

```ini
r_Width = 1920
r_Height = 1800
```

For this build, `cl_fov` does not need to be in `user.cfg`. Use the MIS FOV slider and press APPLY after the game reaches gameplay. The applied/read-back game FOV is synchronized to the remap source FOV. `sys_MaxFPS` can likewise be applied from the menu; 90 is the current test candidate.

If you prefer a deterministic startup before opening the MIS menu, leaving `cl_fov=95` in `user.cfg` is still harmless; startup reads it and synchronizes the remap assumption without writing it.
