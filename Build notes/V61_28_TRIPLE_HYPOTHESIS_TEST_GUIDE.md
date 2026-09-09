# KCD2VR V61.28 — Triple-Hypothesis Test

This source branch is designed to test the three strongest remaining hypotheses in **one KCD2 launch**. No rebuild or restart is required between F6/F8/F9 tests.

## Stable baseline retained

- KCD2 source/render baseline remains **1920x1800**. V61.28 does not override desktop/source resolution.
- OpenXR eye swapchains remain runtime-managed (Quest/VDXR has been recommending 2496x2688 per eye).
- V61.12 gameplay-only cull synchronization is retained. Auxiliary cameras are untouched and the cull hook does not write FOV.
- GOOD MONO remains the fallback/submission baseline: the OpenXR projection views keep their real per-eye poses and FOVs.
- `EnableLegacyStereoProbes=false`, `EnableStereoPipelineOffsetProbe=false`, and `EnableNativeEyeWrite=false` remain the distributed defaults.
- The old physical Insert/Home AFR/native-eye/swap controls are retired in this test branch so they cannot contaminate F8.
- All three hypothesis tests start **OFF**.

## F6 — corrected gameplay FOV + optical-center test

**Press F6 once: ON. Press F6 again: restore baseline.**

F6 does not write a projection matrix or culling field. It:

1. reads the **actual VFOV at `CCamera+0x30` only when V61.12 identifies the exact gameplay CView**;
2. temporarily adjusts the normal `cl_fov` CVar until the actual gameplay source camera approaches 106 degrees (bounded request range 70..150);
3. feeds that measured actual source VFOV into the angular mono presentation mapper;
4. derives a vertical presentation-center correction from the current OpenXR left/right/up/down tangents and the real captured source aspect;
5. restores the pre-test game FOV, source calibration, and Center-Y when F6 is switched OFF.

The current Quest/VDXR frustum is strongly asymmetric vertically, so this tests the hypothesis that the familiar **bottom-first black band / off-center view** is a symmetric-source-camera coverage + principal-point mismatch rather than simply a bad OpenXR FOV.

Useful log markers:

- `V61.28-GAMEPLAY-FOV ... actualVFOV=...`
- `V61.28-F6 GAME-FOV request=... readback=...`
- `V61.28-F6 STATUS actualGameplayVFOV=... requiredCentered=... requiredNoShift=... centerY=... coverage=YES/NO`
- `V61.28-F6 OFF restored ...`

### What to observe for F6

Look primarily at:

- whether the view feels vertically centered;
- whether the bottom black band disappears or becomes balanced with other edges;
- world scale / apparent zoom;
- whether head motion remains natural;
- whether HUD/dialogue framing becomes better or worse.

If `coverage=YES` but a one-sided black band remains, the source-frustum model is incomplete and we should inspect the projection/crop origin deeper in CryEngine. If the image centers correctly and the bands disappear, this gives us a production path with less brute-force FOV than the old 110+ experiments.

## F8 — native StereoL/StereoR producer test

**Press F8 once: ON. Move/look around for roughly 10–20 seconds. Press F8 again: OFF + summary.**

This is intentionally **not another vcam/shadow/AFR/native-eye write**. Those paths were already tested and/or disproven.

F8:

- temporarily changes `g_stereoFrameworkEnable` from its current value to CryEngine's framework debug value `2`;
- passively watches normal-pipeline, source-sized D3D12 render targets;
- records distinct resources and whether multiple distinct full-size RTs are bound in the **same PRESENT epoch**;
- records a D3D debug object name when the resource has one;
- performs **no CRenderView write, no camera write, no renderer re-entry, and no GPU-resource mutation**;
- restores `g_stereoFrameworkEnable` on F8 OFF.

The reason for this test is the exact shipped KCD2 `Stereo.cfx`, whose final stereo compositor consumes distinct `StereoL` and `StereoR` textures plus world/head-locked quad-layer textures. We are testing whether a native stereo producer pair still exists in this executable before trying to force a second eye again.

Useful log markers:

- `V61.28-F8 ON StereoL/R producer test ...`
- `V61.28-STEREO-PRODUCER UNIQUE ...`
- `V61.28-STEREO-PRODUCER PAIR ...`
- `V61.28-F8 OFF summary binds=... uniqueRT=... sameEpochPairs=...`

### How to interpret F8

- `sameEpochPairs > 0`: **candidate**, not proof, of a surviving dual-buffer producer. The next step is to trace those exact resources/callsites backward into the renderer and determine whether they are actually StereoL/R.
- `uniqueRT >= 2` but `sameEpochPairs = 0`: multiple full-size buffers exist, but no same-frame stereo pair was established by this test.
- no pair/unique candidate: the shipped StereoL/R compositor resources are probably not populated by a native dual renderer in normal KCD2 operation; we then need to locate the disabled producer/creation path statically rather than repeat camera hooks.

A visible stereo change is **not required** for F8 to succeed as a diagnostic.

## F9 — final-skinning ownership proof for VRIK

**Best tested while Henry's arms/weapon are clearly visible. Press F9 once: ON; press again: OFF.**

F9 is not the old OperatorQueue write. V61.25 already proved those writes executed without visible arm movement, so that stage is retired.

V61.28 hooks the statically recovered `CCharInstance::SkinningTransformationsComputation` boundary at `WHGame+0x5FE260` (seven-argument ABI recovered from the job thunk at `WHGame+0xDE7A18`). While F9 is ON it:

1. correlates the final-skinning arguments/owners only against Henry's already-proven `CPoseData` pointer or the runtime-proven +/-0x68 companion relation;
2. requires the verified gate-2 buffer-1 getter/setter API;
3. immediately **before the real final-skinning consumer**, temporarily shifts the two captured WeaponSolver joints by +10 cm / -10 cm local X;
4. calls the original engine skinning function;
5. immediately restores the original QuatT values with the verified setter.

Only one mod-generated pulse is allowed at a time. F9 OFF performs no deep final-pose scan and no pose write.

Useful log markers:

- `V61.28-FRIK F9 ON ...`
- `V61.28-FINAL-SKIN MATCH ...`
- `V61.28-FINAL-SKIN APPLY ... temporaryBeforeConsumer=true`
- `V61.28-FINAL-SKIN RESTORE ...`
- `V61.28-FINAL-SKIN TEST-WAIT ...`

### How to interpret F9

- **APPLY log + visible arm/hand movement:** major ownership proof. The next build should replace the 10 cm pulse with the existing V49/Crysis-style shoulder-elbow-hand two-bone solve toward OpenXR controller targets at this same late boundary.
- **MATCH but no APPLY:** final-skin relation was found, but gate/API/joint validation rejected the write. Use the reason/joint fields in the log.
- **APPLY but no visible movement:** do not call it “overwritten.” It proves the CPoseData relation reaches this boundary but not necessarily the final visible joint buffer; the next target is the inner pose/buffer resolver / post-process skeleton data used by skinning.
- **TEST-WAIT only:** this final-skinning ABI does not expose the known CPoseData relation in the scanned owners; trace the inner `+0x5FE2F0`/pose resolver instead.

## F7 — VR menu / Touch regression fix

F7 still opens/closes the VR menu. In this branch:

- the physical menu quad is **20% smaller**: 2.05 m -> **1.64 m**;
- the 1100x680 menu texture resolution is unchanged;
- a valid right-Touch ray intersection now directly drives the software menu cursor;
- thumbstick navigation remains only the fallback when the ray misses;
- right trigger selects/drags, A selects, B closes.

Look for:

`V61.28-MENU Touch ray cursor active; right-controller intersection drives software cursor`

The old `RESET F6` menu label is renamed `RESET UI` because F6 now belongs exclusively to the corrected-projection test.

## Other retained controls

- **F4:** adaptive-vs-fixed rotation diagnostic; fixed 8 ms remains preferred.
- **F5:** CRYSIS_CROP vs LEGACY_CLAMP mono mapping A/B.
- **F10/F11:** existing animation/FRIK recordings.
- **F12:** proven live `CombatCursorCross` FlashUI state test.

## Recommended one-session order

1. Start game and first verify normal GOOD MONO baseline.
2. F6 ON, inspect centering/borders/scale; F6 OFF.
3. F8 ON, play/look around 10–20 seconds; F8 OFF to force its summary log.
4. Get Henry's arms/weapon visible; F9 ON and watch carefully for a 10 cm hand/arm change; F9 OFF.
5. Open F7 and verify menu is smaller and the Touch-ray cursor follows the controller.
6. Exit normally and upload the single `KCD2VR-debug*.log`, plus your visual notes for F6/F9 and whether the Touch cursor worked.

The tests can technically be enabled together, but doing F6/F8/F9 sequentially in the **same launch** gives much cleaner visual attribution while still avoiding rebuild/restart iteration.

## Build status

This package is **source only**. It has been statically audited in the Linux environment, but it has **not been compiled or verified under Windows/MSVC here**.
