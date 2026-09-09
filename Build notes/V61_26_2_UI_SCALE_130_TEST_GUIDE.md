# KCD2VR V61.26.2 — UI viewport 1.30 inverse proof

## Purpose
V61.26.1 proved that `0.70` shrinks the HUD XML Position rectangle and clips/reframes the lower HUD/dialogue. This test checks the inverse direction without mixing in FRIK or stereo experiments.

## Test values
- HUD XML factor: **1.30**
- X: **0**
- Y: **0**
- Base rectangle: 1872 x 2016
- Test rectangle: approximately **2434 x 2621**

## Expected interpretation
If this XML field is a viewport/canvas extent rather than a true Scaleform scale, 1.30 should make HUD content occupy a smaller apparent portion of the view and should restore bottom HUD/dialogue instead of clipping them.

## Procedure
1. Build/install this source.
2. Launch KCD2 once.
3. Confirm the debug log contains:
   `V61.26.2-UI-SCALE-TEST staged scale=1.30 x=0 y=0 -> 2434x2621; RESTART GAME TO APPLY`
4. Exit the game completely.
5. Relaunch.
6. Check whether the whole HUD is smaller and whether lower HUD + dialogue remain visible.

## Kept unchanged
- KCD2 source resolution is not overridden; 1920x1800 remains the preferred performance baseline.
- Known-good V61.12 gameplay-only culling path is untouched.
- GOOD MONO behavior is untouched.
- Stereo remains static-only.
- FRIK native pose writes remain retired.

This package is source-only and has not been Windows/MSVC compiled in this environment.
