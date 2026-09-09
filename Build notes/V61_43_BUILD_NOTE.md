# V61.43 — PERF + SEMANTIC VRIK OWNERSHIP + LIVE UI HARDENING

This source branch is based on the supplied V61.41 depth/UV-fix lineage and the 2026-09-08 VRIK handoff.

## Normal play

`[Experimental] EnableVrikSemanticOwnershipTest=false` is the default. With it false, the semantic VRIK hooks are not installed.

## Performance

The remaining per-main-loop `GetRequiredMonoSourceVFovDeg()` calculation was removed. The no-shift XR coverage VFOV is now computed in the existing 200 ms auto-projection cadence and cached for menu/status use.

## VRIK experiment

The active experiment now installs only:

- `WeaponSolver::SetIKTargets @ WHGame+0xA7F9CC` for CPoseData/player correlation;
- `SkinningTransformCore @ WHGame+0x5FE2F0` for semantic ownership proof.

F9 cycles OFF -> READ -> semantic final-output HAND_R -> semantic buffer1 PREBUILD HAND_R -> OFF. OperatorQueue, outer +0x5FE260, WeaponSolver joint writes and the bad +0x53D7C4 interpretation are not used.

Writes are allowed only for the same exact joint owner that previously passed READ-mode semantic-index and final-output structural validation.

## Live UI

The existing VR-menu HUD and subtitle sliders remain. Their hard-coded WHGame/FlashUI path is now gated to PE timestamp `0x6A350E20`, matching the supplied retail WHGame.dll. The menu shows WHGame gate/HUD-observer status.

This does not recreate the missing V61.42 HUD livebridge package; it keeps the current guarded backend until that source is available.
