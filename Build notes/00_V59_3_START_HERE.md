# KCD2VR V59.3 — SAFE 110 / F6 COMFORT / RECENTER / CONTROLS

Source-only Windows/MSVC test build based on V59.2.

## Requested changes
- Right-stick Up is jump again.
- Left Y restores the weapon-slot/sheath toggle from V58.7.
- `cl_fov < 110` is treated as unsafe: no V58.3 FOV refresh pulse is allowed and startup remains on the comfort presentation until FOV is restored to >=110.
- Inventory/quest-style comfort presentation continues to use the F6 LOCAL-space quad.
- F6 quad is now yaw-only/level (pitch+roll removed), closer and larger.
- OpenXR `XR_TYPE_EVENT_DATA_REFERENCE_SPACE_CHANGE_PENDING` now triggers KCD2 camera recenter and invalidates the F6 anchor for recapture. This is the app-side response to Quest/runtime recenter.

## Crash-log note
Three supplied BugSplat reports ended at `WHGame!004e59c5`; one ended in `nvwgf2umx!004476d0`. The text reports do not include stacks/minidump contents, so V59.3 does not claim a root cause. The low-FOV guard is based on the reproducible headset/desktop black-output behavior reported by the tester.

## Test
1. Keep game FOV at 110.
2. Verify startup/menu comfort screen is level and larger.
3. Open inventory with short Left Menu and verify it stays on the same comfort presentation.
4. Use Quest recenter; verify game-camera origin and later comfort-panel anchor follow the new runtime origin.
5. Verify left stick locomotion, right-stick X turn, right-stick Up jump, Left Y weapon-slot/sheath.
6. Do not intentionally lower game FOV below 110 during normal testing. If a saved value below 110 is encountered, V59.3 should stay comfort-safe and skip the FOV pulse until it is restored.
