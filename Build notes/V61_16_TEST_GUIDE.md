# KCD2VR V61.16 test guide

## What changed
- Removed the old `MIS_MENU_BOOK.bmp` from rendering. That bitmap has legacy menu labels baked into the artwork, which caused the giant/duplicate text and the visible `F8 STEREO TEST` even after the actual F8 control was removed.
- Menu now uses the procedural clean parchment background only.
- Slightly reduced live menu fonts and simplified headings.
- `F6 UI scale` renamed to `F6 theater scale` so it is not confused with the native KCD2 HUD.
- Added an explicit note: native KCD2 HUD scale/Y is not hooked yet.
- F8 remains retired.
- ENGINE DOF remains the startup mode.
- FAKE DOF remains manual, exaggerated to 0.70 -> 0.85 -> 1.00 and logs `V61.16-FAKE-DOF ... pseudoScale=...`.

## Test 1 — menu
Open F7/Left X. The parchment should contain ONLY the live KCD2VR controls. There should be no giant 'Medieval Immersion Simulator', no baked `VR Mode`, `Start Stereo Test`, or `F8 STEREO TEST` underneath them.

## Test 2 — fake DOF
Click `FAKE DOF`. Leave the menu open for a few seconds, then close it and look at a near object and distant background for at least 26 seconds. The log should contain multiple lines like:

`V61.16-FAKE-DOF t=... pseudoScale=0.700 ...`
`V61.16-FAKE-DOF t=... pseudoScale=0.850 ...`
`V61.16-FAKE-DOF t=... pseudoScale=1.000 ...`

If those lines appear, the click definitely registered and the pseudo strength was being changed.

## HUD
The `F6 theater scale` control is NOT the native KCD2 HUD scale. Native HUD scale/Y offset are intentionally not exposed until the Scaleform/HUD transform is actually hooked.
