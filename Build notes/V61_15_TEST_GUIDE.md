# KCD2VR V61.15 test guide

Source-only branch. No Windows/MSVC ASI binary was compiled in this environment.

## Changes
- Large 2.05 m VR menu retained, but typography is reduced and labels shortened to stop overlap.
- F8 pseudo selector/button retired for now. Use MONO / .80 / .85 / .95 buttons in the VR menu.
- ENGINE DOF remains the startup mode (.80 near / .85 mid / .98 far) when KCD2 focus can be read.
- FAKE DOF is now an exaggerated manual visibility test: .70 -> .85 -> 1.00 -> .70.
- Gameplay-only V61.8 culling pose sync retained.
- 90 FPS target, merged-mesh controls, recenter rotation-order test, and FRIK recorder retained.

## DOF test
1. Start game and leave ENGINE DOF on.
2. Check log for `V61.12-ENGINE-DOF focus=...`. If absent, the current focus CVar candidate is not producing a live value.
3. Toggle FAKE DOF manually. The .70 <-> 1.00 change should now be unmistakable.
4. Return to ENGINE DOF with its button.

## Menu
Check for no overlapping text at normal Quest 3 viewing distance. Controller ray / RT or A should remain the primary pointer path.

## HUD note
This branch does not pretend to have a live KCD2 Scaleform HUD transform yet. The external HUD XML/PAK is still separate. A true live HUD scale/Y control needs the current Scaleform movie/layout hook, not compositor-wide zoom.
