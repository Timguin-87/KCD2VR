# KCD2VR V61.30 — All-Hypotheses One-Session Test

V61.30 is designed to collect the highest-value projection, pseudo-depth, native-stereo, menu-input, and VRIK evidence in **one KCD2 launch**. The experimental hotkeys start OFF and are intended to be tested sequentially so one log remains easy to interpret.

## Stable baseline preserved

- KCD2 source-resolution baseline is **1920x1800**; this build does not set `r_Width`, `r_Height`, or force the desktop/source render size.
- OpenXR eye swapchains stay runtime-selected.
- V61.12 exact-gameplay-`CView` culling synchronization is retained: temporary HMD matrix injection only during the gameplay frustum-builder call, followed by immediate matrix restore; cull FOV remains untouched.
- Existing per-eye OpenXR poses/FOV submission is retained.
- `EnableLegacyStereoProbes=false`, `EnableStereoPipelineOffsetProbe=false`, and `EnableNativeEyeWrite=false` remain the distributed defaults.
- Physical Insert/Home AFR/native-eye controls remain retired in this branch.
- Fixed **8 ms** rotation filtering remains the preferred baseline. The old F4 adaptive-rotation diagnostic is retired.
- F4, F6, F8, and F9 start OFF.

## F4 — KCD2 DOF-focus pseudo stereo + real G-buffer depth-resource archaeology

**Press F4 once: ON. Press F4 again: OFF + summary.**

This deliberately combines two related but distinct questions:

1. Can KCD2 expose a useful **real engine focus-distance scalar** (`Dof_User_FocusDistance`, `Dof_FocusDistance`, or `FocusDistance`) that can drive pseudo-stereo strength?
2. Can we resolve KCD2's actual **Normal-view G-buffer depth D3D12 resource** at the already-reversed `CSceneGBufferStage` / MRT setup path?

F4 ON:

- saves the prior pseudo/DOF state;
- disables the simulated/fake DOF sweep;
- enables the read-only engine-focus reader;
- uses the existing PS095 matched-FOV pseudo mapper;
- lazily installs the V61.2-derived G-buffer hooks only on the **first** F4 press;
- scans only the first 0x100 bytes of the Normal-view depth wrapper for D3D12 COM resources;
- performs no resource-state transition, AddRef, copy, clear, shader replacement, camera write, or CRenderView write.

The depth hook is intentionally lazy so F6/F8/F9 baseline tests do not pay its hot-path cost before F4 is used. After F4 OFF the hooks remain installed only as a cheap disabled pass-through until normal shutdown; live renderer unhooking is avoided.

Useful markers:

- `V61.30-F4 ON ...`
- `V61.30-F4 SOURCE-VALID focus=... target=... smooth=...`
- `V61.30-F4 SOURCE-MISSING ...`
- `V61.30-F4 DEPTH-RESOURCE-FOUND ...`
- `V61.30-F4 DEPTH-CANDIDATE ...`
- `V61.30-F4 DEPTH-RESOURCE-WAIT ...`
- `V61.30-F4 OFF ... depthSummary={stage:... mrt:... candidates:... depthHits:...}`

Interpretation:

- **SOURCE-VALID + DEPTH-RESOURCE-FOUND:** both ingredients exist. A later build can test genuine per-pixel depth reprojection instead of only global pseudo strength.
- **SOURCE-VALID only:** focus-driven global pseudo is viable; actual depth texture still needs deeper resource tracing.
- **DEPTH-RESOURCE-FOUND only:** better result for long-term pseudo stereo; we have a real depth source even if KCD's DOF scalar is unavailable.
- neither: retire this runtime branch and use the static Stereo.cfx/native-stereo investigation instead.

This is **not** claimed to be per-pixel depth stereo yet.

## F6 — corrected gameplay source FOV + optical-center test

**F6 once: ON. F6 again: restore.**

F6 uses the FOV sampled only when V61.12 positively identifies the exact gameplay `CView`. It then:

1. temporarily adjusts ordinary `cl_fov` toward enough source coverage;
2. measures the **actual** gameplay camera VFOV at `CCamera+0x30`;
3. feeds that actual source VFOV to presentation mapping;
4. computes the vertical principal-point/Center-Y correction from the live asymmetric OpenXR eye tangents and the real captured source aspect;
5. restores game FOV, source calibration and Center-Y on F6 OFF.

It does **not** write the culling FOV or replace OpenXR's submitted eye FOVs.

Useful markers:

- `V61.30-F6 GAME-FOV ...`
- `V61.30-F6 STATUS actualGameplayVFOV=... requiredCentered=... requiredNoShift=... centerY=... coverage=YES/NO`
- `V61.30-F6 OFF restored ...`

Observe vertical centering, especially the bottom-first black band, apparent world scale, and whether the view feels optically centered.

## F7 — 20% smaller VR menu + repaired Touch ray cursor

F7 opens/closes the menu.

- physical quad width is **1.64 m** instead of 2.05 m;
- texture remains 1100x680;
- valid right-Touch ray X/Y now directly drives the software cursor;
- thumbstick navigation is only the fallback when the ray does not hit;
- trigger/A select and B closes.

This specifically fixes the regression where the ray intersection was calculated but its `pointer_x/pointer_y` values were discarded.

## F8 — passive native `StereoL` / `StereoR` producer test

**F8 once: ON. Move/look around for about 10–20 seconds. F8 again: OFF + summary.**

This does **not** retry the failed Shadow/mode-2, AFR, `+0x818/+0x828`, double-pipeline, or zero-camera vcam experiments.

F8:

- temporarily sets `g_stereoFrameworkEnable` to the existing CryEngine framework diagnostic value used by this test;
- watches exact source-sized D3D12 render-target binds;
- tracks unique resources and distinct resources appearing in the same PRESENT epoch;
- records D3D debug names where available;
- makes no game-camera, CRenderView, renderer re-entry, or render-target-content write;
- restores `g_stereoFrameworkEnable` on OFF.

This is motivated by KCD2's shipped `Stereo.cfx`, whose compositor still refers to distinct `StereoL` and `StereoR` resources plus quad/head-locked layers.

Useful markers:

- `V61.30-F8 ON ...`
- `V61.30-STEREO-PRODUCER UNIQUE ...`
- `V61.30-STEREO-PRODUCER PAIR ...`
- `V61.30-F8 OFF summary ... sameEpochPairs=...`

A pair is a candidate, not proof that the resources are the two scene eyes.

## F9 — final-skinning VRIK ownership ladder

F9 now has **three states**:

1. **OFF**
2. **HAND**
3. **FULLBODY**
4. next press returns to **OFF**

Each transition has a different beep and a `V61.30-F9 STATE ...` log marker.

### F9 HAND

This is the V61.28 final-skinning proof, not the retired OperatorQueue pulse.

Only after final-skin arguments correlate to Henry's already-proven CPoseData relation and gate-2 getter/setter validation succeeds, the two captured joints are temporarily shifted +/-10 cm in local X immediately before `WHGame+0x5FE260`. The real skinning function runs, then the original QuatTs are immediately restored.

Markers:

- `V61.30-FINAL-SKIN MATCH ... mode=1`
- `V61.30-FINAL-SKIN HAND-APPLY ...`
- `V61.30-FINAL-SKIN HAND-RESTORE ...`

### F9 FULLBODY

This adds a **semantic-body ownership proof without guessing joint indices or calling an unproven LimbIK ABI**.

Static WHGame proof used by the test:

- `WHGame+0xB7EBFC` parses semantic `BONEID_*` names;
- `WHGame+0xB7EB04` stores each resolved `uint16 jointId` at `object + 0x17C + 2*BoneID`;
- confirmed IDs used here include `PELVIS=2`, `SPINE2=5`, `HEAD=10`, `HAND_L/R=18/19`, `FOOT_L/R=22/23`, `CLAVICLE_L/R=32/33`.

The test searches only the known final-skin owners and **one bounded pointer hop**. A table is accepted only if its semantic `HAND_L/R` entries match the two live Henry joints independently captured from WeaponSolver. If that validation fails, the result is:

`V61.30-FULLBODY MAP-WAIT ...`

and **nothing is written**.

When validation succeeds, it temporarily pulses pelvis, spine2, head, both clavicles, both hands, and both feet immediately before the same final-skin consumer, then restores every QuatT afterward.

Markers:

- `V61.30-FULLBODY MAP-FOUND ... validation=matches-live-Henry-joints`
- `V61.30-FULLBODY APPLY ...`
- `V61.30-FULLBODY RESTORE ...`

Interpretation:

- **HAND-APPLY + visible hands:** final ownership boundary found for arms.
- **FULLBODY MAP-FOUND + APPLY + visible body deformation:** major breakthrough; we can build HMD-driven head/spine/pelvis and controller arms at this boundary, then pursue native LookIK/LimbIK/PoseAligner as production APIs.
- **APPLY but nothing visible:** don't repeat the same CPoseData stage. Move inside the final skinning/post-process buffer or to the surviving native `CLimbIk` / `CPoseBlenderLook` / `CPoseAlignerWH` stack.
- **MAP-WAIT:** no risky fallback is attempted; use the log to locate the real semantic-map owner.

## F12 / recordings

- F10/F11 retain the animation/FRIK recording workflow.
- F12 retains the proven live `CombatCursorCross` FlashUI state test.

## Recommended order for one session

For the cleanest evidence and lowest renderer-hook overhead:

1. Verify baseline first.
2. **F6 ON/OFF** — projection/FOV/Center-Y.
3. **F8 ON for 10–20 s/OFF** — native stereo producer summary.
4. Make Henry's body/arms clearly visible. Press **F9 → HAND**, observe; press **F9 → FULLBODY**, observe; press **F9 → OFF**.
5. **F7** — verify smaller menu and Touch-ray cursor.
6. Test **F4 last** — DOF focus + G-buffer depth resource; move between near/far objects for several seconds, then F4 OFF for summary.
7. Exit normally and upload the single `KCD2VR-debug*.log` plus brief visual notes for F4/F6/F9/F7.

## Build status

This package is **source only**. Static/source audits are performed in the current Linux environment, but the ASI has **not been compiled or runtime-verified under Windows/MSVC here**.
