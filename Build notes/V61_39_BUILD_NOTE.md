# KCD2VR V61.39 — GOOD MONO presentation rollback

Purpose: recover the proven pre-F8/`mapMode=4` GOOD MONO presentation path while retaining the successful optical-center / Center-Y correction as a separate layer.

## Exact presentation modes

- **OLD MONO (0)**: old GOOD MONO source mapping (`mapMode=0`), Center-Y correction disabled.
- **CORRECTED MONO (1)**: the **same** GOOD MONO source mapping (`mapMode=0`), with the existing Center-Y/OpenXR optical-center correction enabled. No matched-XR-FOV source remap.
- **PSEUDO .90 (2)**: deliberate pseudo mode; retains the matched submitted-FOV remapper (`mapMode=4`) and 0.90 asymmetry.
- **DEPTH PSEUDO (3)**: deliberate depth pseudo mode; retains `mapMode=4` and the existing depth modulation path.

The important rollback is that mode 1 no longer enters `mapMode=4`.

## AFR / DUAL safety

Release menu/config activation of retired AFR/DUAL modes is hard-disabled:

- menu `stereo_mode` is ignored and forced to 0;
- stale `VrStereoExperiment` INI values are ignored;
- stale `VrAfrIpdScale` values are ignored;
- runtime apply keeps AFR capture, stereo frame sync, and native stereo re-entry off for clean modes.

Historical diagnostic code remains in source for archaeology but cannot be selected through the release menu/config path.

## Intentionally unchanged

- V61.12 gameplay-only culling behavior.
- 1920x1800 source baseline.
- OpenXR per-eye runtime poses/FOV submission.
- fixed 8 ms rotation baseline.
- user FPS slider.
- dialogue/inventory recovery work from V61.38.
- F6 virtual screen.
- pseudo/depth modes except for the clean-mode routing helper.

## Not changed in this build

No source-frame capture/presentation timing rewrite was added. The old working builds already used the same general PRESENT capture architecture. This build isolates the known regression boundary first: `mapMode=4` was accidentally applied to corrected mono.

## Expected invariant

For both mono modes the blit log, when diagnostics are enabled, should report `mapMode=0`. Corrected mono should differ from old mono only by the Center-Y correction state.
