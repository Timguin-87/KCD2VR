# V61.50 — VRIK + HEAD-SYNC TEST

This is a focused test branch based on the V61.45 corrected-culling / GOOD-MONO / V60.1 dialogue-inventory FOV-guard source.

## Tracking
- Keeps V61.12 exact gameplay-CView culling synchronization.
- Adds a visible **Fix head sync** checkbox beside **Best VR settings** in the F7 VR menu.
- Checkbox OFF = pose source 0 (`CACHED_XR`).
- Checkbox ON = pose source 2 (`SYNC_NOW`).
- Supplied INI starts ON.
- FPS cap remains manual. The reported headset sweet spot was game `sys_MaxFPS` 84–85 with mode 2 enabled; this build does not fake or hard-code a different cap.
- No V61.48-style extra frame-wait loop is added.

## Dialogue/inventory
The recovered V60.1/V61.36 behavior remains authoritative: no automatic `cl_fov` writes for dialogue/inventory. Transient camera FOV does not drive the compositor source-FOV model.

## VRIK
The supplied INI arms the semantic ownership hooks at startup, but the write state starts OFF.

F9 cycles:
1. OFF -> READ: zero writes. Resolve/validate semantic arm joints from the exact `+0x5FE2F0` job owner.
2. READ -> FINAL HAND_R: after-original semantic right-hand final-output ownership proof.
3. FINAL -> PREBUILD HAND_R: modify exact buffer-1 semantic right-hand QuatT immediately before `+0x5FE2F0`, call original, restore immediately.
4. PREBUILD -> OFF.

Retired paths remain retired: no OperatorQueue writes, no WeaponSolver jointC/jointE write assumption, no `+0x53D7C4` QuatT misuse, no old broad FRIK hooks.

A sparse VRIK status line is emitted from the control thread once per second only while a VRIK test mode is active. There is no per-skin-call file logging.
