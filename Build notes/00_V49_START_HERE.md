# KCD2VR V49 — 10-second 6DoF / FOV / refresh audit

This is a source build derived from V48. Build it on the same Windows/MSVC + DetourModKit + vcpkg OpenXR setup as V48.

## What changed

- F8 is now a 50-second audit: **5 phases × exactly 10 seconds**.
- Full 6DoF rotation + translation remains enabled in every phase.
- Headset refresh is measured dynamically from consecutive OpenXR `predictedDisplayTime` values. No 72 Hz constant is hardcoded.
- Optional KCD2 `sys_MaxFPS` is used for the refresh-matched phases. If that CVar is absent, the log says `fpsCVar=unavailable` / `fpsCapOk=false`; it does not pretend the cap occurred.
- F8 logs `r_Vsync` and `r_DuplicateLastFrameWH` when available.
- Recenter translation is now expressed in the inverse recenter orientation basis (`inverse(origin) * delta`) instead of only subtracting LOCAL-space XYZ.

## F8 phase order

1. **CURRENT_V44_FOV_UNLOCKED** — current V44+ OpenXR-FOV compositor remap, game FPS restored/unlocked.
2. **PRE_V44_RAW_FIT_UNLOCKED** — no compositor FOV remap; aspect-fit control using the pre-test KCD2 FOV.
3. **CURRENT_FOV_XR_MATCHED** — current FOV remap + game cap to measured XR Hz (72/80/90/120 etc.).
4. **CURRENT_FOV_XR_2X** — current FOV remap + game cap to 2× measured XR Hz.
5. **RAW_FIT_XR_MATCHED** — pre-V44 raw/fit presentation + game cap to measured XR Hz.

Move your head continuously through every phase: yaw/pitch/roll, lean left/right, lean forward/back, crouch/stand. Note which numbered 10-second block has the least lag/jitter and which has the most natural scale/FOV.

## Important limitation

This build does **not** claim to implement true upstream per-eye KCD2 projection or a proven downstream camera-injection point. Those remain hypotheses until a live projection/late camera commit is identified in WHGame. The F8 ladder isolates the safe hypotheses we can test now: V44 compositor remap vs raw/fit and KCD2 FPS vs runtime refresh cadence.

## F7 / quad UI

V49 retains the V48 F7 implementation for now. It is **not yet** the final additive menu-only quad requested: V48's current quad path can replace the gameplay projection while active. Do not evaluate that as the final UI architecture. The requested target is: gameplay projection always remains present, while only the MIS parchment/menu texture is a centered head-following overlay layer.

## Expected log identity

Look for:
- `V49 ready:`
- `V49-F8-START`
- five `V49-F8-BEGIN` / `V49-F8-END` pairs
- `V49-F8-COMPLETE`
- `V49-FPS-CAP`
