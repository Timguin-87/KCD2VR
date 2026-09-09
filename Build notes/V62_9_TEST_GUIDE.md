# KCD2VR V62.9 — Lua UI + VRIK Ladder Test Guide

Build marker:

`KCD2VR-V62.9-LUA-UI-VRIK-LADDER`

This is an experimental source build. The camera/culling/head-sync baseline is intentionally unchanged. The new VRIK write modes are fail-closed and only target the validated retail WHGame build.

## 1. Clean build / files

Set `DETOURMODKIT_ROOT` to the extracted DetourModKit MSVC package and run:

```bat
build_release.bat
```

vcpkg is **not required**. CMake uses an installed OpenXR package when present, otherwise it fetches the Khronos OpenXR loader.

The release build copies `KCD2VR.ini`, `MIS_MENU_BOOK.bmp`, and `KCD2VR_UI_LIVE.pak` beside the new ASI. Keep `KCD2VR.ini` there; V62.9's SAVE/APPLY paths write only KCD2VR's own INI.

## 2. UI persistence / dialogue vertical

The V62.8 UI work is carried forward.

- `Dialogue vertical` now has a full `-720 ... +720 px` range and moves `DialogLeft` and `DialogRight` together.
- `APPLY` on the UI page applies the native FlashUI values, syncs the Lua HUD companion, and saves `KCD2VR.ini`.
- The Lua companion remains responsible for the HUD groups that KCD2 recreates after inventory/menu transitions.
- The VR View `Y-offset` is a signed user correction. `0.000 = centered / no user trim`. The supplied INI now defaults to `0.0`.

After positioning the UI, press the UI page `APPLY` button once, close inventory, reopen it, and verify the position survives. Then restart the game and verify the saved values load again.

## 3. VRIK: start empty-handed

A weapon is **not required for the first test**. Stand in ordinary first-person gameplay with empty hands and keep the VR menu closed.

The release-facing VR menu still has only:

`NATIVE TOUCH` / `VRIK`

Choosing `VRIK` enters the safe READ mode. F9 is the developer ladder and cycles:

1. `READ GEOMETRY` — zero writes.
2. `PREBUILD HAND DELTA` — selector-1 HAND_R gets a bounded controller-relative translation before `WHGame+0x5FE2F0`, then is restored immediately after the builder consumes it.
3. `PREBUILD TWO-BONE POSITIONS` — uses the validated semantic `UPPERARM_R -> FOREARM_R -> HAND_R` positional chain, preserves the current bend plane, writes FOREARM_R / ELBOW_R helper / HAND_R positions before the builder, then restores immediately.
4. `OFF / NATIVE TOUCH`.

Every F9 edge writes an explicit mode line to both logs and requests a non-blocking Windows system sound. The logs are the source of truth if the machine's sound scheme is muted.

## 4. READ mode — do this first

Press F9 once from OFF, or click `VRIK` in the menu.

Wait for a line similar to:

```text
V62.9 VRIK mode=1(READ GEOMETRY) rig=true ... joints=718 handR=408 b1Len=.../... refLen=.../...
```

The important expected semantic chain from the last successful diagnostic is:

- UPPERARM_R = 403
- FOREARM_R = 405
- ELBOW_R helper = 406
- HAND_R = 408
- parents = 395 / 403 / 403 / 405
- selector-1 lengths approximately 0.266 m / 0.262 m

V62.9 also fixes the final-output diagnostic to measure `UPPERARM_R -> FOREARM_R -> HAND_R`; the previous diagnostic accidentally used the separate ELBOW_R helper for the DQ length candidates.

If READ stays `rig=false` and `setIK=0`, do one normal interaction (door/container/etc.) or draw/sheath once, then return to empty hands. This only seeds the existing read-only player-pose correlation. Do not repeatedly toggle F9 while waiting.

Do **not** continue to a write mode unless READ has a coherent 403 -> 405 -> 408 chain and plausible ~0.2-0.4 m segment lengths.

## 5. PREBUILD HAND DELTA

From READ, press F9 once.

Stand still for a moment. The first valid frame calibrates the right-controller position relative to the HMD and performs no write. Wait for `cal=true` / `cal=1`.

Then move the right controller slowly by about 5-15 cm. V62.9 maps relative OpenXR movement using the existing project convention:

`Cry local = { XR.x, -XR.z, XR.y }`

The delta is hard-clamped to 0.30 m.

Expected result: Henry's right hand should visibly respond/deform in the same direction as the controller. This mode is deliberately hand-only; it is an ownership/axis proof, not a finished arm solver.

If the wrong character moves, the mesh explodes, the camera/head becomes unstable, or the game crashes, stop testing write modes and send both logs.

## 6. PREBUILD TWO-BONE POSITIONS

Only test this after HAND DELTA gives a controlled visible response.

Press F9 once more. V62.9 uses the current selector-1 shoulder/forearm/hand positions, keeps their measured lengths, preserves the current elbow bend side, and moves the forearm + hand position toward the controller-relative target. Quaternions/wrist orientation are intentionally not solved yet.

Expected result: a more arm-like positional response than HAND DELTA. Some wrist/forearm twist or deformation is still possible because this test proves positional ownership before orientation work.

Do not test smithing, alchemy, pickpocketing, horse mounting, scripted cinematics, or other authored hand-critical sequences in write modes 2/3.

## 7. Weapon test comes later

After empty-hand mode 2/3 is understood, repeat with a simple weapon drawn. The weapon remains attached through KCD2's native skeleton. Do not infer combat readiness from a visible arm response; native attack/block integration is a later step.

The hook also records read-only `WeaponSolver::SetIKTargets` call count and the observed `solver+0xC/+0xE` values. This is for correlation with the newly discovered native `RgtArm01`/`LftArm01` animation-driven IK route; V62.9 does **not** override that procedural IK target yet.

## 8. Send back

Please send:

- `KCD2VR.log`
- `KCD2VR-diagnostics.log`
- what each F9 mode did visually
- whether UI/dialogue vertical settings survived Inventory and a full game restart
- whether `Y-offset = 0.000` is visually the expected center

Do not rely on the sound alone; confirm the F9 mode from the log.
