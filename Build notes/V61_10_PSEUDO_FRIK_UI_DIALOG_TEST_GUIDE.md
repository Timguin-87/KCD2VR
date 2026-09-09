# KCD2VR V61.10 — pseudo / FRIK / UI / dialogue test

SOURCE-ONLY test branch. No Windows ASI was compiled in the Linux packaging environment.

## Locked known-good
- V61.8 reversible culling pose-sync is retained. It temporarily applies the XR pose only around `UpdateFrustumPlanes`, then restores the camera matrix. It does not widen FOV/frustum.
- Legacy-good mono remains available.

## Pseudo workflow
- Default: pseudo **0.95**.
- F8 cycle: **0.80 -> 0.85 -> 0.95 -> MONO**.
- Removed from the useful cycle: 0.875 and 1.00.
- Clean projection defaults: `VrPresentationZoom=1.00`, `VrSourceVerticalFovDegrees=95`.

## Performance / graphics
- VR balanced profile is back to the practical 120 / 120 / 120 floor for view / vegetation / merged meshes (never lowers higher user values).
- `sys_MaxFPS` is set to 90 once after the CryConsole is available. The VR menu FPS slider can override it.
- The VR menu has `e_MergedMeshesViewDistRatio` live test controls: slider 30..250 + APPLY + ORIGINAL 30 + BALANCED 120.
- Test 30 / 60 / 90 / 120 first. If culling stays fixed at 30, the V61.8 pose-sync was the real fix and merged-mesh distance can stay low for performance.

## First-person dialogue
- Uses the current-game CVars only: `wh_dlg_ForcePOV=1`, `wh_dlg_NoCameras=0`.
- V61.10 does **not** change `cl_fov` or the VR source-calibration FOV when applying first-person dialogue mode.
- Old first-person ADB/Mannequin PAKs are intentionally not included because they crashed on the current KCD2 build.
- Automatic dialogue F6 is not claimed yet: no current-version dialogue-active hook has been verified safely. F6 remains manual during dialogue in this test.

## F6 theater auto behavior
- Startup/title: startup F6 now waits for a published gameplay camera >= 80 degrees before auto-exit, instead of exiting on the 75-degree title/menu camera.
- Inventory opened by the Touch left-Menu short action toggles theater.
- Pause opened by the Touch left-Menu hold / Escape action toggles theater.
- Keyboard-opened inventory/pause are not yet guaranteed because we do not have a verified current UI lifecycle hook for every menu.
- Hold RIGHT GRIP + RIGHT STICK for theater calibration (Y distance, X size/framing).

## Letterbox experiment
`ui_test_paks/KCD2VR_UI_NOLETTERBOX_CENTERED_EXPERIMENT.pak` is built from the centered HUD file supplied for this private test. Its current KCD2 `hud.gfx` has `fc_setRatioPreset` patched to return immediately, so the `ratioStrips` preset logic should no longer activate. This is experimental and must be tested in-game. It does not use the old KCD1 KCSE DLL or Address Library.

## HUD safe-area experiment
Install **one** safe-area PAK matching the current KCD2 source resolution, plus the no-letterbox centered pack if desired.

Available presets:
- 1248x1344: 90% and 85%
- 1872x2016: 90% and 85%
- 2496x2688: 90% and 85%
- `RESTORE_FULLSCREEN` restores the original current KCD2 `HUD.xml`.

The XML changes only the HUD Scaleform constraint rectangle. Cursor/input mapping may need a later companion transform; test whether the visual action cursor and actual selection point still agree.

## FRIK/action recording
Action recorder buttons keep the V61.9 read-only FRIK sampler. Record useful sequences such as pickup/use, door, crouch/jump, combat, bow, alchemy, smithing, dialogue, horse. Look for `V61.9-FRIK-SAMPLE` in the debug log. No skeleton/IK writes are performed yet.

## Suggested first run
1. Use pseudo .95 and 90 FPS.
2. Set merged meshes to 30 -> APPLY and reproduce the location that previously culled. Raise only if actual distance pop-in returns.
3. Install 1872x2016 90% safe-area if that is your source resolution. Check action prompts, compass/cursors and screen edges.
4. Add no-letterbox centered experiment and start a dialogue. Check bars, dialogue FOV and stability.
5. Record one or two FRIK action sequences and send the debug log.
