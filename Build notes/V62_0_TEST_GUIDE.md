# V62.0 Test Guide

## 1. Clean install
Do not install the earlier KCD2VR UI viewport/letterbox/Lua test mods for this test. V62.0 does not require Dynamic HUD, Dynamic Detector, Lua, HUD.xml replacement, or `hud.gfx` replacement.

Build the ASI with `build_release.bat`, copy the freshly verified `KCD2VR.asi` and supplied `KCD2VR.ini` to the normal ASI directory.

## 2. Head sync baseline
Open F7.

- Set the game's FPS cap to the headset-proven range first: **84 or 85**.
- Keep **Fix head sync** checked. This selects pose source mode 2 (`SYNC_NOW`).
- If needed, A/B by unchecking it; OFF selects cached mode 0.

The FPS slider and pose-source checkbox are intentionally independent.

## 3. Native UI
Open F7 and press **TUNE UI**.

Defaults:
- subtitle size: 55%
- subtitle vertical: 0 px
- dialogue size: 70%
- dialogue inset: 14%
- interaction icon size: 100%
- interaction icon inset: 35%
- interaction vertical: 0 px

Press **APPLY UI NOW** once during normal gameplay after HUD is loaded. Slider changes are also sent on release.

`RESET VANILLA` restores the recovered retail base transforms and disables the VR UI correction.

If a specific control changes nothing, report which target failed (`Subtitles`, `DialogLeft/Right`, or `Hints`). Do not fall back to whole-HUD viewport scaling in the same test.

## 4. VRIK ownership — READ first
The supplied INI sets:

`[Experimental] EnableVrikSemanticOwnershipTest = true`

The hooks install at startup but the runtime VRIK mode begins OFF.

Press F9 once -> READ.

In F7, wait for:
- rig = OK;
- plausible joint count;
- `handR` >= 0;
- correlated count increasing.

The normal `KCD2VR.log` also emits a sparse V62.0 VRIK status about once per second while a test mode is active.

Do not continue to a write mode if READ never becomes valid.

## 5. FINAL HAND_R
Press F9 again.

Expected proof if the late final-output array is the visible owner: Henry's right hand / attached weapon should show a small obvious displacement.

If it moves: STOP. Do not test PREBUILD. The production VRIK insertion point is proven.

If writes increase but nothing visible moves, continue once to PREBUILD.

## 6. PREBUILD HAND_R
Press F9 again.

This modifies the exact semantic HAND_R in pose buffer 1 immediately before `WHGame+0x5FE2F0` consumes it, calls original, then restores immediately.

If it moves: STOP. Production VRIK should use late buffer-1 `QuatT` writes and let KCD2 build the hierarchy/output.

If both FINAL and PREBUILD report valid writes but nothing visible moves: STOP. Do not increase amplitude and do not return to OperatorQueue. The next RE target is downstream of `desc+0x20` / actual rendered skinning consumer.

## 7. After ownership succeeds
The next build should implement right-arm controller tracking using:
- Crysis-style analytic two-bone arm math with real KCD2 bone lengths;
- Cyberpunk-style late model-space ownership/cache;
- KCD2's proven semantic arm map and proven late insertion point.
