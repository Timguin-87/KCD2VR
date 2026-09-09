# KCD2VR — Technical Roadmap

Status: experimental research scaffold, September 2026.

This project is deliberately split into small milestones. A purple OpenXR image is **not** treated as "VR working"; it only proves that a compositor layer was submitted. Stable 6DOF camera tracking has now been confirmed in-game on the desktop view. The next renderer milestone is a render-thread-synchronized stereo foundation, followed by identifying KCD2's main-view dispatch so the engine can render true left/right eyes.

## Reference projects

### KCD2Tools / TPVCamera

Source: https://github.com/tkhquang/KCD2Tools

The current TPVCamera implementation gives us the most valuable KCD2-specific camera information:

* `CCamera::UpdateFrustumPlanes` is a verified hook point.
* The camera passed to that function is the embedded `CCamera` at `CView + 0xE8`.
* The untouched CView eye position is at `CView + 0x14`.
* The untouched CView eye quaternion is at `CView + 0x20`.
* TPVCamera modifies the render camera **before** the original frustum builder is called, so culling is computed from the modified camera.
* The function is also used by shadow/reflection/portal cameras, so production-quality code ultimately needs the same CView RTTI gate used by TPVCamera (`.?AVCView@@`).

The offsets above are documented in the current public TPVCamera source (`src/constants.hpp`) and the pre-frustum modification order is documented in `src/hooks/camera_hook.cpp`.

### Crysis VR

Source: https://github.com/fholger/crysis_vrmod

The reusable architecture is:

1. OpenXR session / session state handling.
2. `xrWaitFrame` and predicted display time.
3. Locate HMD pose for that predicted display time.
4. Feed the tracked pose into the game camera separately from the renderer.
5. Add OpenXR controller actions and action spaces.
6. Add stereo submission only after tracking/camera behavior is stable.
7. Add roomscale and weapon/controller transforms as separate systems.

The renderer details are engine-specific; the tracking/input separation is the part being reused here.

---

# Current implementation

## Phase 0 — ASI / KCD2 foundation

- [x] `.asi` plugin architecture compatible with the existing `dinput8.dll` ASI-loader setup.
- [x] DetourModKit logging/config/hooking.
- [x] Patch-resilient frustum-builder AOB candidate ladder derived from TPVCamera.
- [x] No hardcoded absolute WHGame address for the frustum hook.

## Phase 1 — OpenXR tracking foundation

- [x] OpenXR instance and HMD system discovery.
- [x] `XR_KHR_D3D12_enable` graphics binding using the game's captured D3D12 device/queue.
- [x] OpenXR LOCAL and VIEW reference spaces.
- [x] Session-state wait for `XR_SESSION_STATE_READY` before `xrBeginSession`.
- [x] `xrWaitFrame` -> predicted display time -> HMD pose acquisition.
- [x] OpenXR left/right grip-pose action spaces.
- [x] Trigger, squeeze/grip and thumbstick actions.
- [x] Oculus Touch suggested bindings for Quest/Virtual Desktop usage.
- [x] Recenter input action (X/A) wired to the camera tracking origin and edge-triggered on button press.

### New safety mode

- [x] **Tracking-only mode is now the default.**

`DiagnosticRendering = false` means the mod creates the OpenXR session and reads tracking/input, but creates **no OpenXR eye swapchains, no D3D12 command allocator/list, and submits no purple diagnostic texture**.

This is intentional. It lets us answer the first question independently:

> Can KCD2 + VDXR run a stable OpenXR session and drive the KCD2 camera from 6DOF tracking?

Only after that succeeds should `DiagnosticRendering = true` be used to exercise the custom D3D12 eye-swapchain path.

## Phase 2 — Correct mono 6DOF camera integration

- [x] HMD pose is recentered against the first valid pose instead of applying the runtime's absolute LOCAL-space orientation directly to the game.
- [x] HMD translation is treated as a **delta from recenter origin**.
- [x] OpenXR axes are explicitly remapped (`+X right, +Y up, -Z forward` -> KCD2/Cry camera `+X right, +Y forward, +Z up`).
- [x] Head translation is transformed through the current KCD2 eye basis, rather than added directly to world XYZ.
- [x] Head rotation is composed on top of KCD2's untouched eye orientation instead of replacing the game's orientation.
- [x] Camera modification now occurs **before** `CCamera::UpdateFrustumPlanes`, matching TPVCamera's proven ordering.
- [x] Repeated same-frame frustum rebuilds are idempotent: each application starts from CView's untouched position/quaternion instead of adding onto our previous modified matrix.
- [x] Rotation and positional tracking can be independently disabled in `KCD2VR.ini` for debugging.
- [~] Basic game-view filter: candidate CView must have a vtable pointer inside `WHGame.dll`.
- [ ] Replace the basic filter with TPVCamera-equivalent **MSVC RTTI CView validation**. This is required before calling the camera gate production-safe, because the frustum builder is shared by non-game cameras.

### Important test target

With `DiagnosticRendering = false`, the headset may remain in the runtime/loading environment because KCD2VR submits no visual layer. That is expected. The useful observation is the **desktop KCD2 camera**:

* turn headset -> KCD2 camera should turn relative to the game's normal facing;
* lean left/right -> KCD2 camera should translate locally;
* move head forward -> camera should move forward relative to the current game view;
* X/A recenter -> current physical pose becomes the new neutral pose.

If this works without a crash, Phase 2 is substantially validated without involving custom D3D12 eye rendering.

## Phase 3 — D3D12 device / queue capture

- [x] Never create a second throwaway D3D12 device.
- [x] Hook exported `D3D12CreateDevice` only to observe the game's real device creation.
- [x] Hook that real device's `CreateCommandQueue` method.
- [x] **Only capture a `D3D12_COMMAND_LIST_TYPE_DIRECT` queue.** Earlier code captured the first queue regardless of type; KCD2 can create COPY/COMPUTE queues too.
- [x] Compare the game's native `ID3D12Device::GetAdapterLuid()` with OpenXR's required adapter LUID (no invalid `IDXGIDevice` query).
- [x] Check the runtime's required D3D feature level against the captured device.

## Phase 4 — Diagnostic OpenXR D3D12 rendering

Enable explicitly with:

```ini
DiagnosticRendering = true
```

Current behavior:

- [x] Create per-eye OpenXR D3D12 swapchains.
- [x] Choose a known DXGI color format instead of blindly taking format 0.
- [x] Acquire/wait/release swapchain images.
- [x] Clear each acquired eye image to the diagnostic purple color.
- [x] Explicit synchronization for KCD2VR's own diagnostic allocator/list/fence.
- [x] Render-thread producer/consumer synchronization restored: XR frame timing/swapchain ownership stay on the XR worker, while KCD2VR D3D12 command recording/submission is handed to the captured DIRECT queue's `ExecuteCommandLists` thread.
- [x] Submit `XrCompositionLayerProjection` using runtime-provided eye poses/FOV.
- [ ] Re-test `DiagnosticRendering=true` with the synchronized submission path. Keep it off by default until that runtime test passes.
- [ ] Add `ID3D12Device::GetDeviceRemovedReason` diagnostics on every D3D12 failure path.
- [ ] Add bounded fence waits / clean failure instead of any permanent GPU wait.

This phase is **diagnostic only**. It does not render KCD2 into the headset.

## Phase 5 — Native stereo rendering

This is the largest remaining renderer-specific problem. The new foundation deliberately separates **XR stereo data** from **KCD2 scene rendering**.

Foundation now present:

- [x] Capture OpenXR `XrView` data at the runtime's predicted display time.
- [x] Publish a thread-safe stereo snapshot containing exact left/right eye poses and each eye's asymmetric `XrFovf`.
- [x] Observe KCD2's real DIRECT-queue `ExecuteCommandLists` activity as a render-thread rendezvous.
- [x] Keep OpenXR frame timing and swapchain API ownership on the XR worker thread; only custom D3D12 GPU work crosses to the game submission thread.
- [x] Shutdown quiescence: disable new render callbacks and wait for in-flight callbacks before destroying XR/D3D12 state.

Still required for **actual native stereo**:

- [ ] Identify the game's final main-view color target and the point at which a full scene view is dispatched.
- [ ] Determine whether KCD2 can render a second main view in the same engine frame without corrupting temporal systems.
- [ ] Feed the already-captured OpenXR per-eye poses and asymmetric FOV tangents into the KCD2 main-view/projection setup.
- [ ] Render/copy the resulting left/right images into OpenXR swapchain images.
- [ ] Handle TAA/DLSS/frame-generation history safely; stereo eyes cannot blindly share temporal jitter/history.
- [ ] Decide whether alternate-eye rendering is needed as an initial prototype before true stereo double-rendering.

The current purple clear must **not** be described as stereo KCD2 rendering.

## Phase 6 — Controller-to-game integration

OpenXR controller data already exists. KCD2 integration does not.

- [x] Left/right controller tracking poses available.
- [x] Trigger/grip/thumbstick values available.
- [ ] Transform controller poses from XR LOCAL space through the same recentered player/camera origin used by HMD tracking.
- [ ] Map left-stick locomotion into KCD2's existing action system rather than synthesizing raw keyboard input if possible.
- [ ] Map trigger/grip to KCD2 actions.
- [ ] Find weapon/hand attachment transforms.
- [ ] Drive right-hand weapon pose.
- [ ] Add two-handed support target.
- [ ] Add hand skeleton / arm IK only after a stable weapon transform exists.

## Phase 7 — Roomscale / body orientation

- [ ] Separate HMD roomscale displacement from character locomotion.
- [ ] Keep small physical movement as camera offset.
- [ ] Recenter/consume larger offsets into player movement when appropriate.
- [ ] Rotate roomscale offset around snap/smooth-turn yaw.
- [ ] Integrate collision so the head cannot walk through solid world geometry independently of the body.
- [ ] Keep weapon/controller transforms in the same tracking origin.

---

# Test order

Do not enable everything at once.

1. `DiagnosticRendering=false`, `ApplyHeadRotation=true`, `ApplyHeadPosition=false`.
   Test rotation only.
2. If stable, enable `ApplyHeadPosition=true`.
   Test leaning/6DOF translation.
3. Test recenter (X/A controller click).
4. Inspect `KCD2VR.log` for OpenXR/session/device errors.
5. Render-thread synchronization is now present, but keep `DiagnosticRendering=false` for normal testing until the synchronized purple-eye path is explicitly validated on hardware.
6. After that validation, identify KCD2's main-view dispatch and begin true two-eye engine rendering using the published OpenXR stereo snapshot.

# What is safe to claim publicly now

* KCD2's current TPVCamera frustum hook gives a verified game-camera integration point.
* KCD2VR has an OpenXR/VDXR-oriented tracking and controller-input foundation.
* HMD pose is now applied as a recentered 6DOF delta on top of KCD2's native eye pose, before frustum construction.
* The D3D12 capture path uses the game's real device and now explicitly waits for a DIRECT graphics queue.
* Tracking-only mode has now been tested successfully: HMD rotation and positional 6DOF drove the KCD2 desktop camera without the previous purple-screen crash.
* A render-thread rendezvous now exists for custom D3D12 work, and exact per-eye OpenXR pose/FOV data is published for the native-stereo hook.
* Real KCD2 scene stereo rendering, weapon pose injection, IK and roomscale locomotion remain research work.

# What should NOT be claimed yet

* "KCD2 runs in VR" — not yet; real stereo game rendering is not implemented.
* "Hand tracking works" — OpenXR controller poses exist, but KCD2 hands/weapons are not driven yet.
* "D3D12 crashes are solved" — the architecture is safer, but only runtime testing can establish that.
* "Native stereo is working" — not yet. 6DOF tracking is confirmed, but KCD2 is not yet being asked to render two scene views.

## Diagnostic build note — immediate logging

The normal DetourModKit `KCD2VR.log` may not become visible until clean shutdown because of buffering. A second file, `KCD2VR-debug.log`, is now written with Win32 file APIs and `FlushFileBuffers()` on every diagnostic milestone. Use this file for live testing. It reports the first valid HMD pose and first camera-hook pose application, then sparse counters so it does not flood the disk.


### Runtime diagnostic correction (2026-09-01)
Runtime testing showed only the first relative-path trace line despite confirmed 6DOF execution. The fallback trace is now resolved against the loaded ASI module directory, with sequence/thread IDs and debugger mirroring. Validate this before interpreting missing checkpoints as missing execution.

## 2026-09-02: CRenderView manager checkout path
- Direct construction is abandoned after the orphan CRenderView dry-run crash.
- Exact managed checkout identified at WHGame+0x16F78F4.
- Direct callers: WHGame+0x4E6F4B and WHGame+0x43EBBE.
- Current probe observes checkout -> factory -> constructor -> manager registration without modifying lifecycle.
- Next gate: classify the world-render checkout and its mode/provider. Only then attempt a second eye through the normal manager/provider path.


## SAFE-V3 checkout-only isolation
Disabled both renderer hot-path detours at 0x4E6924 and 0x4E7E64 after SAFE-V2 still crashed at WHGame+0x4E6598. Retain only engine-managed CRenderView lifecycle observers plus previously stable camera handoff probes. Next mutation work remains blocked until this baseline is stable.


## SAFE-V5 deferred CRenderView state
V4 proved checkout stack splits but sampled +0x38/+0x8D40 too early. V5 defers inspection until the next existing checkout or camera publish hook, after the 0x4E6F50 wrapper has configured the returned mode-2 CRenderView. No new engine hook is added.


## SAFE-V6 full-world derived-view candidate correlation
Static analysis confirmed WHGame+0x4E6F1C derives a managed mode-2 CRenderView from a source CRenderView. V6 classifies the two runtime stack families and correlates each fully configured child/source against the published CSystem+0x288 ViewCamera, without adding renderer hot-path hooks.


## EXPERIMENT-V7 native-eye perturbation
First controlled native-eye test. Select exactly one engine-managed main-4E6C mode-2 child only after its parent is runtime-correlated to the published ViewCamera at CRenderView+0x8EE8. Detour WHGame+0x865550 with its statically proven one-argument signature; apply a reversible +3cm camera-local-right translation only during the camera-build call. No custom CRenderView allocation, no AER, and 0x4E6924/0x4E7E64 stay disabled.


## EXPERIMENT-V10 selected camera-pair boundary
Static proof: 0x865550 selects indexed 0x310 camera slots using (CRenderView+0x8EE0)&1 and feeds the exact pair into 0x866F6C, whose output begins at CRenderView+0x9B30. V10 hooks that narrow boundary, shifts only the exact selected pair for the correlated mode-2 child, and restores translation after the engine builds derived state. This preserves the child's native special camera data unlike V9 whole-block seeding.

## MAJOR UPDATE 14: V10 code reviewed - genuinely well-engineered write, gated with an independent safety flag

Full review of the actual V10 implementation (`CRenderViewCameraPairBuildDetour`
at `WHGame+0x866F6C`), not just its description. Verdict: this is careful,
well-reasoned engineering, not a reckless first write attempt:

- The write only activates for the ONE specific CRenderView `g_native_eye_target`
  locked onto via the deferred-snapshot correlation logic (route==1,
  `source_cam_matrix_off==0x8EE8`, vtable-verified) - not indiscriminately for
  every mode-2 view.
- The detour double-checks owner identity AND vtable match before doing
  anything; any mismatch falls through to an unmodified call.
- `ShiftEmbeddedCameraRight`/`RestoreEmbeddedCameraTranslation` are both
  individually SEH-guarded, and the shift is computed along the camera's own
  local-right basis vector (mathematically correct for a rotated camera, not
  a naive world-space offset).
- The original routine IS called normally with the shifted inputs, and the
  three translation floats are restored immediately after, within the same
  call, before returning - no state left mutated across frames.

**One real gap found and fixed during review**: unlike every other risk
category in this project (`DiagnosticRendering=false` gates GPU submission),
this write had NO independent opt-in - it activated automatically, bundled
with the same `INST()` install as the other eleven purely-observational
hooks, the moment target correlation locked on. Added
`EnableNativeEyeWrite` (defaults `false`, `KCD2VR.ini` `[Settings]`,
threaded through `mod.h`/`mod.cpp` into
`stereo_archaeology::detail::g_native_eye_write_enabled`): the correlation/
targeting logic and its logging (`NATIVE-PAIR-V10 write-disabled` lines)
still run unconditionally and safely, but the actual memory write now
requires this explicit, separate flag before it ever executes. This means
the very first test can validate WHICH object gets targeted and WHICH
offsets get selected purely via logging, with zero live-memory-write risk,
before a second, deliberate test ever turns the write on.

Standard compile-pattern sweep - clean.

**Recommended next test, in order**: (1) build and run with both
`DiagnosticRendering=false` and `EnableNativeEyeWrite=false` (both
defaults - no code change needed), confirm `NATIVE-PAIR-V10 write-disabled`
lines appear and target correlation looks sane; (2) only then, in a
separate deliberate session, set `EnableNativeEyeWrite=true` to test the
actual write, watching closely for any anomaly.

---

# Unified VR feature roadmap — 2026-09-03

The latest scaffold is now the single baseline. Native stereo remains the gate
for every gameplay-interaction feature below.

1. **Native stereo** — prove selected camera-pair path, then inject OpenXR eye
   poses/FOV into KCD2's own multi-view/render path. No AER.
2. **Player body / FRIK** — native pelvis/legs and animation, HMD head/neck plus
   controller-driven arms/hands using KCD2 skeleton/WeaponSolver where possible.
3. **Virtual holsters** — body-relative shoulder/hip/quiver volumes mapped to
   KCD2 `E_HandSlot`, weapon slots and clothing/entity attachments.
4. **Controls / locomotion** — controller actions merged into KCD2 native input;
   configurable body/HMD/hand-relative movement and physical crouch.
5. **Target lock / cursor / UI** — remove VR-facing lock steering and permanent
   crosshair while preserving internal combat target state; wrist/body-gaze HUD.
6. **Native physical melee** — controller pose -> combat zone/attack intent ->
   WeaponSolver/native collision -> KCD2 stamina/damage/block/riposte/master strike.
7. **Stamina / weapon weight** — read KCD2 stamina, Strength, Agility, Weight,
   StrReq/AgiReq and turn them into virtual weapon lag/inertia/recovery/haptics;
   never direct custom damage.
8. **Object grabbing / NPC contact** — physical hand candidates/constraints and
   native take/equip semantics; KCD2 remains AI/threat/combat authority.
9. **Physical bow** — quiver, nock, two-hand aim, physical draw and native bow
   action/release/projectile semantics.
10. **Alchemy** — tracked hands/tools drive the native alchemy action director
    and attachment/event system.
11. **Smithing** — tracked hammer/tongs/workpiece trajectories drive native
    blacksmithing actions/events while KCD2 owns quality, heat, stamina and XP.

Safety policy for development: every new subsystem begins as a read-only observer,
then gets a dedicated opt-in write/action gate, then becomes default only after
runtime evidence is stable.


---

# VR embodiment roadmap extension — V12

This section records the post-stereo embodiment target. It is deliberately staged:
**native stereo remains the gating milestone**, and these systems should be enabled
one at a time after stereo is stable.

## Design principle

Preserve KCD2 gameplay authority wherever possible:

**OpenXR physical input -> VR pose/constraint/intent -> native KCD2 action/state**

Do not replace KCD2 damage, stamina, inventory, crafting, AI, or combat semantics
with parallel VR-only systems unless no native bridge exists.

## Controller fingers

Initial Oculus/Quest Touch support should derive finger poses from controller
touch/button/grip state rather than assume full optical five-finger tracking.

Planned pose pipeline:

KCD2 animation -> body/arm IK overlay -> hand/finger bone overlay -> final skeleton

First implementation:
- thumb state from thumb/control touch
- index curl from trigger touch/value
- middle/ring/pinky curl from grip/squeeze
- object-specific grip poses
- fist detection from grip + trigger state

Later: investigate optional optical hand tracking separately.

## Physical crouch

Calibrate standing HMD height. Use relative HMD height plus hysteresis to request
KCD2's **native crouch action/state**, rather than merely lowering the camera.

Goal:
physical squat -> native crouch -> native stealth/movement/collision semantics.

Do not continuously toggle around the threshold; use separate enter/exit heights.

## Physical unarmed combat

A closed fist plus tracked controller velocity/orientation becomes punch intent.
Do not directly subtract NPC health.

Preferred path:
physical punch -> velocity/contact classifier -> native unarmed attack intent ->
KCD2 stamina/skill/armor/damage/reaction.

Forearm/fist guard poses may later feed native unarmed block intent.

## Shared physical constraint layer

Use a common controller-to-virtual-pose constraint/error model for:
- hand collision with world geometry
- NPC touching/pushing
- weapon collision
- heavy-weapon inertia
- stamina exhaustion
- grabbing/throwing
- smithing hammer contact
- mortar/pestle contact
- bow/string constraints

The real controller is never forcibly displaced. The virtual hand/tool can stop,
lag or deflect, and the controller-to-virtual error becomes a resistance/contact
signal and optional haptic source.

## Target lock and cursor

Desired VR behavior:
- no camera/HMD steering from target lock
- no aim magnetism
- no forced body rotation
- no permanent center cursor/crosshair

Keep internal combat-opponent knowledge if KCD2 requires it for AI, ripostes,
paired combat states or directional combat. Decouple that knowledge from camera
control rather than destroying the entire targeting subsystem.

Interaction feedback should migrate toward hand proximity, subtle highlighting
and haptics.

## Body-attached / gaze-revealed UI

Normal gameplay HUD should be nearly invisible.

Primary concept:
- left/non-dominant wrist: health, stamina, status
- weapon-hand gaze: weapon condition / handling information
- belt/pouch: quick inventory context
- shoulder/quiver gaze: arrow/ammo context
- quest/menu information only on intentional invocation

UI fades in only when the relevant body region is raised/oriented toward the HMD,
then fades away when gaze/pose leaves it. Avoid a permanent floating VR HUD.

## Stamina + RPG handling

Read native KCD2 stamina and keep native stamina drain authoritative.

Virtual weapon/arm response can combine:
- current/max stamina
- weapon Weight
- StrReq / AgiReq
- player Strength / Agility
- native attack/block/recovery state

Low stamina should reduce virtual acceleration/follow/recovery and increase
perceived weapon drag. Do not slow the real controller and do not directly
multiply damage; pass the resulting virtual blade/fist motion into native combat.

## Implementation order after stereo

1. Prove player skeleton and hand/head/pelvis transforms.
2. Prove native draw/holster attachment transitions.
3. Add body-relative virtual holster volumes.
4. Add Touch-derived hand/finger poses.
5. Add physical crouch -> native crouch action.
6. Remove camera lock/magnetism and permanent cursor.
7. Add gaze-revealed wrist/body UI.
8. Add fist classifier -> native unarmed attack.
9. Add shared hand/world collision constraint layer.
10. Integrate sword WeaponSolver targets and native directional melee.
11. Feed stamina/Strength/Agility/weight into virtual handling.
12. Add physical grabbing and native inventory transfer.
13. Add NPC touch/threat refinements.
14. Add physical bow/quiver/nocking/release.
15. Add physical alchemy actions.
16. Add physical smithing/tongs/hammer actions.

Each stage: **read-only observer -> gated experiment -> validated implementation**.

## V15 native stereo injection experiment (2026-09-03)

V14 A/B/C logs proved that WHGame+0x866F6C propagates the temporary camera-pair X translation into its generated output state (`q2` changed from 0 to ~0.032), independently of DiagnosticRendering. V15 therefore replaces the fixed +3.2 cm diagnostic with live OpenXR stereo separation: camera A receives -IPD/2 and camera B +IPD/2 using the current valid XrView positions. Absolute HMD translation remains owned by the existing CView/frustum tracking hook. Invalid/non-finite IPD or values outside 40-90 mm suppress the write for that call. Child translations are restored after the original builder returns.

## MAJOR UPDATE 17: V17 log analysis found and fixed a real singleton/instance mismatch - the actual reason "nothing changed"

`KCD2VR-debug.log` from V17 showed `EnableNativeEyeWrite=true` was actually
tested this round (54 `NATIVE-PAIR-V17 apply=` events, zero crashes, zero
errors - genuinely stable). But **every single apply** showed
`beforeA/beforeB=(0.000,0.000,0.000)`, `locate_result=-2`, `predicted=0`,
`source=fixed-64mm` - 100% consistent, not intermittent. This is why
nothing visibly changed: the write executed against a zeroed camera using a
static non-tracked IPD fallback, never real headset data.

**Root cause found, not just theorized**: `GetOpenXrStereoInjection()` in
`stereo_archaeology.h` reads live view data via
`kcd2vr::XrManager::Instance()` (a Meyer's singleton). `mod.cpp` was
creating its own, entirely separate `XrManager` via
`std::make_unique<XrManager>()` and never touched `Instance()` at all -
`XrManager::Instance()` existed as dead code since much earlier in this
project (noted once before, in an earlier review, as apparently-unused).
The native-write code had been reading from a never-initialized,
never-updated object this whole time - not a timing bug, not a threading
bug, a genuine object-identity split. `locate_result` sitting permanently
at its constructor default (`XR_ERROR_RUNTIME_FAILURE`) and `predicted=0`
in literally every logged sample is exactly what that object-identity bug
predicts.

**Fixed**: `mod.h`'s `xr_manager_` changed from an owning
`std::unique_ptr<XrManager>` to a non-owning `XrManager*` pointing at
`&XrManager::Instance()`. Both shutdown paths in `mod.cpp` now call
`xr_manager_->Shutdown()` then clear the pointer (never delete - the
singleton is owned by its own static local, standard Meyer's singleton
lifetime, destroyed automatically at process teardown). One other call
site (`Mod::OnRenderTick`) updated from `.get()` (unique_ptr-only) to a
plain pointer read. Verified: no other `unique_ptr`-specific calls remained
on `xr_manager_` anywhere in `mod.cpp`. Standard compile-pattern sweep:
clean. Both safety gate defaults confirmed unchanged.

**Not yet tested with this fix** - next test should show `NATIVE-PAIR-V17`
(or later) apply lines with `locate_result=0` (`XR_SUCCESS`), nonzero
`predicted`, and `source=fresh-xr`/`cached-xr` instead of `fixed-64mm`, with
`beforeA`/`beforeB` reflecting real, nonzero camera positions. If those
still don't appear after this fix, the next thing to check is whether
`WaitForFrame` (which populates `latest_stereo_views_`) and
`GetOpenXrStereoInjection` (which reads it) are racing in a way the
existing mutex doesn't fully cover, or whether the native hook fires before
the first `WaitForFrame` call of a session has ever run.

## MAJOR UPDATE 18: singleton fix confirmed working; new always-zero camera position issue found

Fresh `KCD2VR-debug.log` with the MAJOR UPDATE 17 singleton fix applied
confirms the fix worked: 56/110 applies now show genuine live OpenXR data
(`xr_ipd=0.068530` - a real, plausible human IPD from the actual headset,
`locate_result=0`, nonzero `predicted`) where 100% showed the hardcoded
fallback before. Real, confirmed win.

**New, separate issue found**: `beforeA`/`beforeB` are zero in ALL 110
applies, including the 56 fully-successful ones - meaning the GAME-side
camera translation at the correlated `CRenderView`'s
`+0x8EE8`/`+0x9508` slot is genuinely zero every time, independent of
whether the OpenXR data pipeline succeeded. Two hypotheses (wrong/inactive
view targeted vs. deferred population not yet written at hook time, same
shape as the SAFE-V5 bug) with concrete next investigation steps written up
in `V17_POST_FIX_FINDINGS.md` - not yet resolved.

## MAJOR UPDATE 25: F8 crop/pose isolation ladder implemented, verified against real Crysis VR source

Verified the musings doc's specific technical claims directly against the
actual `fholger/crysis_vrmod` source (user-provided): oversized-symmetric-
frustum sizing from max tangent extent (confirmed, `VRManager.cpp` line
333-338), genuine per-eye rendering via a wrapped render-function callback
(confirmed, `VRRenderer::Render` + shadow/particle-skip comments that only
make sense with true dual rendering), and `XrCompositionLayerQuad` usage
for HUD/menus (confirmed, three separate instances). All three checked out
accurately, not exaggerated.

**Honest assessment given to the user**: 2 of 3 proposed structural
changes (Quad-based F6, honest common-origin mono pose) are directly
transferable, low-risk, and don't require solving native stereo. The third
(Crysis-style crop math) is a real improvement to single-image FOV mapping
but does NOT grant genuine stereo depth - the "render twice" part of
Crysis's architecture requires source-level engine access we don't have;
our black-box equivalent would mean the same still-unsolved goal the
recursive-view/CRenderView investigation has been chasing, not a new
shortcut.

**Implemented the proposed F8 isolation ladder** (4 new phases appended to
the existing 6-phase tracking ladder, reusing all its infrastructure):
`CRYSIS_A_BASELINE_OLD` (legacy clamp + fake-separated pose, reproduces
V58.6), `CRYSIS_B_NEWCROP_ONLY`, `CRYSIS_C_HONESTPOSE_ONLY`,
`CRYSIS_D_COMBINED` (the actual V58.7 proposal). Added:

- `XrManager::SetAngularCorrectMono`/`SetPoseHonestyOverride` - the latter
  is a new independent override (-1=default `stereo_experiment_==0`
  gating, 0/1=forced) added specifically because pose-honesty wasn't
  previously toggleable separately from crop mode, which the isolation
  ladder needs.
- `MatrixPhase` struct extended with `crop_mode`/`pose_mode` fields
  (default -1 = "leave unchanged" via C++ aggregate default member
  initializers), so the original 6 tracking phases are completely
  unaffected.
- Clamp-activity aggregation (`clamp_active_count_`/`clamp_total_count_`),
  reusing the ALREADY-COMPUTED `coverage_clamped_eye[]` boolean (the exact
  value the musings doc's 0.697-vs-0.995 finding was based on) rather than
  adding new GPU-side shader/UAV counting - CPU-side only, much lower risk
  for a diagnostic feature. New `V56-CLAMP-STATS` log line reports
  clamp-active percentage per phase.
- Reused V53's existing `CameraHook::ResetWorstCaseAngular()`/
  `GetWorstCaseAngular()` unchanged - confirmed still correctly wired at
  both phase start and end.

**Found and fixed a real gap during implementation**: neither of the two
F8-exit paths (normal completion, and the "Good Mono" mid-test cancel
path) restored the two new overrides back to normal-gameplay state -
without this fix, finishing or cancelling the ladder would leave gameplay
stuck on whatever the last-run phase had configured. Both paths now
restore `angular_correct_mono_` to `crysis_crop_mono` (the user's own
toggled preference, same variable normal gameplay reads) and
`pose_honesty_override_` to `-1`.

Standard compile-pattern sweep: clean. No new hooks (still 11 total). No
raw memory writes. Both original safety gates unchanged. Not yet
live-tested.

## MAJOR UPDATE 27: found the actual cause of "higher FOV = more zoomed in" - a real desync bug, not a math error

User reported raising `cl_fov` toward 110° via the live menu slider made
the zoomed-in feeling WORSE, the opposite of MAJOR UPDATE 26's prediction.
Root cause found: `cl_fov` (the game's real rendering FOV) and
`vr_source_vertical_fov_deg_`/`VrSourceVerticalFovDegrees` (our own remap
math's SEPARATE assumption about the source FOV) are two independent
settings - `Mod::ApplyMenuState` treats them as genuinely separate fields
(likely intentional, for advanced independent tuning), but the live FOV
slider's `apply_game_fov` callback only updated the real `cl_fov` CVar and
the menu's own display value - it never touched
`vr_source_vertical_fov_deg_`. The two silently drifted apart the moment
that slider was used, growing worse the further it moved: with the remap
math still assuming the OLD, narrower FOV while the game now genuinely
renders wider, any given output angle samples a smaller fraction of the
now-wider source image - a real, mechanistic explanation for the
paradoxical "wider FOV, more zoomed" experience, not a flaw in the
underlying FOV-matching theory itself.

**Fixed**: `apply_game_fov` now reads back the CONFIRMED applied `cl_fov`
value (not the raw requested one, in case the CVar didn't take exactly as
requested) and syncs `vr_source_vertical_fov_deg_` to match, keeping both
in lockstep for this specific interaction. The separate, more advanced
`source_vertical_fov_deg` menu field remains independently adjustable for
anyone who wants deliberate fine-grained control later - this fix only
addresses the actual live-slider workflow the user was using.

Standard compile-pattern sweep: clean. No new hooks. Both safety gates
unchanged. Not yet live-tested with this fix.

## MAJOR UPDATE 28: found and used the already-documented ~81.69deg-vs-110deg gap; deferred one-time real-FOV calibration added

Evaluated the collaborator's 4 FOV hypotheses critically rather than
accepting them uncritically: hypothesis #1 (1920x1800 aspect mismatch) does
NOT hold up under the actual tangent math - at cl_fov=110 vertical on that
aspect, horizontal comes out to ~113.4°, while the real Quest 3 tangent
data only requires ~108° horizontal for full coverage - a harmless
surplus, not a deficit, so aspect mismatch alone doesn't explain the
zoom.

Hypothesis #2 (the `cl_fov` scalar doesn't correspond to the actual
rendered FOV) is correct, and turned out to already be CONFIRMED with hard
ground-truth data sitting in the codebase's own comments since V59.2:
"the ~81.69deg published camera produced by game FOV 110." Setting
`cl_fov=110` only produces ~81.69° of REAL internal camera FOV - a ~28°
gap. `FrameSync::LatestPublishedFovRadians()` already reads this real
value via the existing publish-camera hook, but was only ever used for one
startup diagnostic check, never fed into the actual remap math.

**Important nuance found before implementing anything**: V60.1 had already
tried and deliberately rejected using this live published value
continuously (`(void)have_published_fov;`, the "UI/DIALOG FOV GUARD"
comment) - correctly, because the publish hook fires on WHATEVER camera
object triggers it (verified directly - `PublishViewCameraDetour` reads
`float[12]`/`+0x30` from any camera indiscriminately, no type filtering),
so dialogue/inventory/cinematic cameras would cause unwanted rescaling if
trusted continuously. A naive "just use the live value" fix would have
reintroduced that exact, already-identified instability.

**Implemented instead**: a deferred, one-time calibration
(`pending_fov_calibration_until_ms_`), armed only when the user explicitly
presses Apply on the FOV slider (never during ordinary gameplay, so a
stray transient-camera publish can never trigger it), takes one confirmed
reading of the REAL published FOV ~500ms later (long enough for KCD2 to
settle after the CVar change, short enough the user is still in the menu),
then disarms. Logs `V60.2-FOV-REALCAL` with the before/after delta.
`apply_game_fov`'s existing CVar-readback sync (MAJOR UPDATE 27) is kept
as an immediate first approximation while the calibration is pending.

Standard compile-pattern sweep: clean. No new hooks. Both safety gates
unchanged. Not yet live-tested - the first real test of whether using the
ACTUAL measured FOV (rather than the requested CVar value) resolves the
remaining zoom.
