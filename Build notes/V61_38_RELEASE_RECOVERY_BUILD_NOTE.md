# V61.38 Release Recovery — build note

This source finalizes the handoff WIP with the smallest recovery edits requested.

## Release-recovery baseline
- V61.12 gameplay-only culling preserved.
- 1920x1800 KCD2 source baseline preserved.
- FPS is not forced on startup. The FPS slider is seeded from current `sys_MaxFPS`.
- Game FOV and Source FOV start matched; packaged default is 95/95. They remain independently adjustable after startup.
- Presentation modes remain: OLD MONO, CORRECTED MONO, PSEUDO .90, DEPTH PSEUDO .70..1.00.
- Depth candidate/scoring/use is enabled only while DEPTH PSEUDO is selected and disabled immediately when leaving it.
- Dialogue camera modes remain Native / Force POV / No cameras. Force POV is mode 1.
- Restored transient dialogue/inventory zoom reducer defaults to retain 0.30 and is now exposed as a live session slider 0.00..1.00.

## UI recovery
- HUD scale menu range is now consistently 0.50..1.20 in menu, state, and runtime clamp.
- Dialogue/subtitle scale remains 0.50..1.50.
- Both semantic UI commands use instance `-1` and require a recently observed live HUD.
- These semantic bridges remain runtime candidates, not proven visible scaling. Logs distinguish WAIT / REQUESTED / FAILED.
- HUD.xml clipping is not used as live scale.

## Cleanup
- Hidden action-recorder hit targets are disabled in this release-recovery menu so accidental clicks cannot arm old recorder tests.
- No in-game INI save is reintroduced.
- No retired stereo/AFR/OperatorQueue paths are re-enabled.

## Verification status
Static/source checked in this environment. Windows/MSVC and headset runtime verification remain required.
