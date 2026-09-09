# KCD2VR V56 — all-in-one tuning + action probe

This is a SOURCE package. No Windows/MSVC compile was performed in this environment.

## What changed

### F8 tracking ladder — 60 seconds, 10 seconds each
1. RAW cached rotation
2. 4 ms quaternion SLERP smoothing
3. 6 ms smoothing
4. 8 ms smoothing
5. 10 ms smoothing
6. adaptive smoothing (10 ms for tiny jitter, falling toward ~3 ms on fast turns)

The old 45/60 Hz sample-and-hold phases were removed because the runtime test reported them as worse.

### F7 tuning sliders
- Gameplay zoom now has a much larger test range: raw 1.80 .. 0.50. Slider direction is intentionally presented as **RIGHT = zoom out**.
- Gameplay FOV tune range is now raw source VFOV 160 .. 70 degrees. Slider direction is intentionally presented as **RIGHT = wider** to correct the reversed-feeling UI. The raw source value is still shown/logged so the winning value can be made the default later.
- Native menu/dialog fit scale: 0.60x .. 3.00x. Enable F6, then tune this slider to find a comfortable menu/dialog size.

F6 remains a projection-flat DIAGNOSTIC, not the final quad-layer menu solution. It may still show binocular/double-image artifacts. Use it only to determine useful framing/scale.

### F7 action-recording buttons
- Weapon
- Pickup
- Jump/crouch
- Bow
- Alchemy
- Smithing
- Dialogue
- Misc interaction
- Horse/mounted

Click once to begin a category, click the same button again to stop. High beep = start, low beep = stop. F10 remains weapon and F11 remains pickup shortcuts.

Records contain mouse/keyboard inputs, XR/present/camera frame counters, and periodic observer-only IK CVar snapshots. Controller values are still logged but should be ignored when testing with mouse/keyboard only.

### VRIK / IK safety
V56 does **not** write skeleton, hand, elbow, or WeaponSolver targets. It adds action-aware observer logging only. This is intentional until the KCD2 player skeleton/IK owner and ABI are proven from WHGame/runtime correlation.

## Suggested test order
1. Leave the first-person animation overhaul disabled for the baseline run.
2. Open F7 and tune Gameplay FOV + Gameplay zoom. Tell me the exact displayed/raw values that look correct.
3. Open Inventory/Map/Quest/Codex/dialogue, press F6, tune Native menu/dialog fit scale, and report the best scale plus whether double-image remains.
4. Press F8 and continuously turn your head through all six phases. Report the best phase number.
5. Use F7 Record buttons and perform the named actions normally with mouse/keyboard. Stop each category when finished.
6. Send both KCD2VR logs.

## Important
- No live resolution changes.
- Same-backbuffer dual-eye presentation is still mono, not true stereo.
- V56 removes the destructive native-stereo dual-execute phases from F8; stereo render-target separation remains a separate engineering task.
