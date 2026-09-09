# KCD2VR V54 — Tracking + Native Stereo + VRIK Start

## F8 audible test ladder
Every phase begins with N short beeps for test N. Completion is a low+high double tone.

1. TRACK_CACHED_CURRENT_ORDER — current 6DoF baseline.
2. TRACK_CACHED_OPPOSITE_ORDER — quaternion composition order A/B.
3. TRACK_SYNC_PREDICTED — synchronous camera-thread locate at latest XR predictedDisplayTime.
4. NATIVE_STEREO_ENGINE_FRAMEWORK — shipping CryEngine stereo framework CVars driven from live OpenXR IPD. No AFR.
5. NATIVE_STEREO_DUAL_EXECUTE — Normal scene pipeline executes twice in one render epoch with renderer-consumed camera shifted -IPD/2 then +IPD/2. Reversible.
6. NATIVE_STEREO_DUAL_EXEC_SELECTOR — same dual execute plus camera-selector 0/1 per pass. Reversible.

Tests 5/6 are aggressive renderer diagnostics. They are attempts to force two real scene renders in the same simulation epoch. They are NOT claimed to be working OpenXR native stereo until logs and headset output prove distinct same-frame eyes.

## F7
Keeps the working additive menu quad and V52/V53 controller/mouse software cursor input.

## VRIK / hands
F9 toggles `V54-VRIK` experimental hand-target bridge. It records both OpenXR grip poses and trigger/squeeze state, converts positions into KCD2-style X/right, Y/forward, Z/up coordinates, and exposes stable left/right target structures.

This build does NOT yet call `WeaponSolver::SetIKTargets` or write a guessed `LimbIK` object. WHGame proves those systems exist, but their owner/ABI is not yet safe enough for a native write.

## Evidence used
- Engine.pak inspected: no obvious packaged VR/stereo preset; stereo attack remains runtime renderer side.
- WHGame strings include `WeaponSolver::SetIKTargets`, `LimbIK_Definition`, `IKTargetProceduralClip`, `GetBoneLocal`, `player_NoIK`, `player_DrawIK`, `wh_am_DebugHandIK`, `ca_useADIKTargets`.
- Renderer archaeology: active pipeline `CRenderView` at pipeline+0x1978; consumed camera bank observed at CRenderView+0x8EE8; +0x8EE0 treated only as a camera-selector experiment, not as a proven eye field.

Source package only: compile in the normal Windows MSVC + DetourModKit environment.
