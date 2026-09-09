# KCD2VR V61.36 CLEAN — runtime modes

This branch is intended for normal headset play/calibration rather than archaeology.

## Presentation modes in the old F7 menu

1. **OLD MONO** — legacy/pre-F8 presentation. No automatic Center-Y correction and no pseudo disparity.
2. **CORRECTED MONO** — current XR-derived cyclopean projection with automatic Center-Y and the established `+0.080` trim. Default.
3. **PSEUDO .90** — corrected projection plus fixed pseudo stereo strength `0.90`.
4. **PSEUDO .90 + DEPTH** — same pseudo stereo plus a small depth-weighted modulation of the existing pseudo horizontal shift.

`Depth stereo strength` is live and ranges from `0.00` to `0.20`; default `0.05`. `0.00` is identical to ordinary PSEUDO .90. This is deliberately not full depth-image reprojection: it only changes how strongly the existing pseudo disparity responds to near/far depth.

The depth mode uses the already-proven screen-sized DSV selection rule. V61.36 does **not** install the V61.34 per-draw scoring hooks. It tracks DSV creation/binding and accepts only a repeatedly-bound depth target matching the captured source resolution and aspect.

## Performance cleanup

- synchronous fallback debug-file writes are compiled out;
- normal logger default is `WARN`;
- no F4/F5/F8/F9/F10/F11/F12 diagnostic hotkeys;
- no final-pose/FRIK diagnostic hook installation;
- no per-draw depth hooks;
- in-game INI save disabled;
- failed HUD scaling path disabled/hidden;
- old transient dialogue/inventory `cl_fov` writer removed.

F6 remains the optional virtual screen. F7 opens the old menu.

## Config keys

The new presentation selector intentionally uses a new key so an older `VrCyclopeanMode=3` cannot silently become depth mode:

```ini
VrCleanPresentationMode=1
VrDepthStereoStrength=0.05
```

`VrCleanPresentationMode`: 0 old mono, 1 corrected mono, 2 pseudo .90, 3 pseudo .90 + depth.

## Dialogue / inventory

This clean branch does not write or restore `cl_fov` when a dialogue/inventory camera appears. Game FOV and source calibration remain user-owned. The V61.33 transient-FOV compensation loop is removed.

## Safety baseline

V61.12 gameplay-only culling files are unchanged from the V61.35 compile-fixed source baseline. The corrected `dxguid` CMake link remains present.

Source/static validation only in this environment; Windows/MSVC and headset runtime are not claimed.
