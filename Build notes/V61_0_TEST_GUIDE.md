# KCD2VR V61.0 — AFR + DUAL + TPV menu test

Build ID: `AFR-DUAL-TPV-MENU-20260906-N`

This is source-only. Compile with the same Windows/MSVC workflow used for V60.9.

## Main changes

- TPV-style local camera offsets exposed in the VR menu:
  - X / side: -0.30 .. +0.30 m, positive = right
  - Y / distance: -0.30 .. +0.30 m, positive = backward
  - Z / vertical: -0.30 .. +0.30 m, positive = up
- Dedicated RECENTER button requests a full mod-local tracking-origin recapture (position + full orientation).
- Manual stereo section in the VR menu:
  - MONO
  - AFR
  - DUAL renderer diagnostic
  - IPD scale 0.00 .. 1.25 x live Quest IPD
- Either left or right stick can move the 2D menu cursor; the stick with the larger deflection wins.
- Action recording buttons are visible again. The recorder now focuses on game/keyboard/mouse input + frame/camera correlation, not raw XR controller poses.
- Existing KCD2 `cl_fov`, `cl_hfov`, KCD2Tools six-field cull-sync, F6, dialogue controls and pseudo presets remain available.

## F8 stereo ladder

Press F8 once in ordinary gameplay. F6 is forced off during the 48-second test and restored afterward.

The current game/source FOV and current source resolution are preserved. All phases use the V60.7-B matched tangent mapping with pseudo UV disparity and temporal reprojection disabled.

1. A — AFR, 0.25 x live Quest IPD
2. B — AFR, 0.50 x live Quest IPD
3. C — AFR, 0.75 x live Quest IPD
4. D — AFR, 1.00 x live Quest IPD
5. E — DUAL renderer diagnostic, 0.50 x live Quest IPD
6. F — DUAL renderer diagnostic, 1.00 x live Quest IPD

AFR uses the existing frame-locked ViewCamera epoch, alternating camera-local +/- eye translation and caching the last L/R PRESENT images for joint OpenXR submission.

DUAL enables the recovered CryEngine stereo CVars and the Normal CRenderView selector-0/1 dual-pass diagnostic with +/- eye translation. Its PRESENT bridge remains mono deliberately; the important evidence is whether KCD2 emits distinct scene resources/passes and whether the headset/monitor shows native renderer separation.

## What to report

For each A-F, note:

- real depth/parallax or flat
- double image / eye strain
- world stability during head rotation
- shadows or lighting moving incorrectly
- whether Henry/weapon scale feels better/worse
- image sharpness

For E/F also note any monitor-side split/dual-view behavior.

Useful log markers:

- `V61.0-F8-BEGIN`
- `V46.5-AFR-BLIT ... cacheValid=11`
- `V61.0-DUAL-PROBE`
- `V58.8-NATIVE-EYE-PASS`
- `V59.6-SCENE-SUMMARY`
- `V56-REENTRY-DIAG`
- `V61.0-F8-COMPLETE`

## Manual camera-distance test

Keep FOV fixed and try local Y/back at 0.00, +0.05, +0.10, +0.15, +0.20 m. If Henry/body/weapon apparent size changes correctly while FOV stays fixed, that supports the camera-distance theory.

## Safety

`SAFE MONO` turns AFR/native dual off. DUAL is still a diagnostic and can cause wrong shadows/state; do not treat it as production stereo yet.
