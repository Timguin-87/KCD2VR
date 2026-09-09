# KCD2VR V60.1 test guide

Build ID: `OVERSCAN-PSEUDO-UI-FOV-GUARD-20260905-E`

## What changed

- Removed the Stretch/Fit/Crop/OpenXR presentation selector from the VR book UI.
- The three large preset buttons are now the promising V59.9 phases 6/7/8:
  - **P6 DIRECT** = Cyberpunk direct + cyclopean 0.75, no UV shift.
  - **P7 UV .002** = tangent projection + cyclopean 0.75 + UV shift -0.002.
  - **P8 UV .004** = tangent projection + cyclopean 0.75 + UV shift -0.004.
- VR book is physically larger and the preset buttons are taller.
- KCD2 FOV slider range is 70-130.
- Production presentation no longer follows transient published camera FOV during dialogue/inventory. It stays locked to the confirmed render/source FOV. This is the **UI/DIALOG FOV GUARD** experiment.
- F8 is now only a 42-second source-frustum overscan ladder: 100, 105, 110, 115, 120, 125 degrees. Quest/OpenXR eye FOV stays unchanged.
- Removed the dangerous double-pipeline/shadow-smear phases from F8.

## Test 1 — live pseudo stereo

Try P6, P7, P8 in normal gameplay. Pick the most comfortable one. P7 is the default comparison target because it was the middle promising V59.9 phase.

## Test 2 — FOV overscan

Press F8 once. Six phases, 7 seconds each:

1. 100
2. 105
3. 110
4. 115
5. 120
6. 125

Look for this specific result: peripheral visibility should increase with higher source FOV while the *center apparent scale* should stay approximately unchanged. Image softness is expected at wider FOV because the same 1920x1800 source pixels cover more angle.

The log should contain `V60.1-OVERSCAN ... XR_FOV=UNCHANGED` for each phase.

## Test 3 — dialogue / inventory FOV guard

After F8, enter dialogue and inventory normally. Compare against V60.0. The compositor should no longer chase the temporary camera FOV published by those states. If the old "tiny window inside the view" disappears or is reduced, that identifies the transient camera-FOV sync as a major part of the problem.

If the tiny-window effect remains exactly the same, the next target is the Scaleform HUD ratio/viewport path rather than camera FOV.
