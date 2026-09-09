# KCD2VR unified baseline — test order

This ZIP intentionally carries the whole researched VR roadmap while keeping
unproven runtime features disabled. Test one subsystem at a time.

## Phase 0 — current baseline / native stereo

Settings for the first run:
- Enabled = true
- DiagnosticRendering = false
- EnableNativeEyeWrite = false
- every `[FutureFeatures]` option = false

Expected stereo markers:
- `CRV-DOWNSTREAM EXPERIMENT-V10`
- `NATIVE-PAIR-V13 selected`
- `NATIVE-PAIR-V13 write-disabled`

Only after the target/offA/offB/camera values look sane should
`EnableNativeEyeWrite=true` be tested deliberately.

## Phase 1 — native stereo injection

- characterize exact selected camera pair at WHGame+0x866F6C
- fix/confirm full ABI if needed (caller also prepares two stack float args)
- map OpenXR left/right eye pose+FOV onto the native render path
- prove genuine two-eye world rendering

## Phase 2 — body / skeleton / virtual holsters

- read-only local player skeleton census
- head/neck + hand/arm IK overlay
- native legs retained
- observe E_HandSlot / clothing attachment transitions
- first physical proof: draw/holster from shoulder/waist volume

## Phase 3 — controls / UI

- OpenXR input -> native actions
- HMD/body/hand-relative locomotion options
- remove camera-facing target lock while retaining combat opponent state
- suppress permanent cursor
- wrist/body UI visibility by gaze

## Phase 4 — native melee

- physical guard direction -> native combat zone
- physical swing classifier -> native attack intent
- observe WeaponSolver::SetIKTargets
- preserve native blocks/perfect blocks/ripostes/master strikes
- stamina/stat/weight handling controls virtual weapon response, not direct damage

## Phase 5 — grabbing / NPC contact

- physical world-object candidate and hold state
- native CanPickUp/interaction semantics
- shoulder/body inventory transfer gesture
- low-speed NPC bump/push, high-speed contacts gated by native combat state
- preserve KCD2 threat/hostility authority

## Phase 6 — bow

- body-relative quiver
- physical arrow nock
- two-hand bow pose
- physical draw distance
- native bow_primary / bow_primary_release
- retain KCD2 projectile/ammo/damage logic

## Phase 7 — alchemy

- ingredient take/place
- mortar/pestle
- bellows
- hourglass/pot/book
- preserve C_AlchemyActionDirector and native recipe state

## Phase 8 — smithing

- native tool/workpiece attachments
- tracked hammer/tongs pose
- physical hammer contact -> native smithing stroke event/state
- preserve temperature/stamina/quality/completion/XP

Rule for every phase: observe first, write second.


## After stereo: embodiment sequence

See `VR_EMBODIMENT_ROADMAP_V12.md`. Stereo remains the gate. After it is stable,
progress through skeleton/holsters -> fingers/crouch -> target lock/cursor/UI ->
unarmed -> shared collision -> sword/stamina -> grabbing/NPC -> bow -> alchemy ->
smithing. Keep one runtime variable under test at a time.

## V15 immediate stereo test

Use the packaged settings `DiagnosticRendering=false`, `EnableNativeEyeWrite=true`. Confirm `NATIVE-PAIR-V15 selected`, then `NATIVE-PAIR-V15 apply` with a plausible `xr_ipd` (roughly 0.04-0.09 m by safety gate), and inspect `NATIVE-PAIR-V15 post`. Record the desktop/HMD visual result: double/separated view, clear image/depth change, no change, or crash. Do not enable future embodiment feature flags during this test.
