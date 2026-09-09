# KCD2VR Research Retrospective — 2026-09-07

## Purpose

This is the durable project-state document for the KCD2VR investigation after reconciling:

- the older `KCD2VR-scaffold` archive,
- the corrected Claude `KCD2VR-HANDOFF-PACKAGE`,
- the V61.x source/history carried into the current branch,
- the cumulative debug logs available in the conversation Files surface,
- the latest user-observed runtime results.

The purpose is to prevent repeated experiments, distinguish genuine dead ends from wrong-layer tests, and preserve the exact evidence needed for the next native-stereo, VRIK, projection and HUD work.

This file supersedes old point-in-time "current state" claims when later runtime evidence contradicts them. Old files remain useful for provenance.

---

# 1. Known-good baseline — preserve first

## Camera / culling

- The stable gameplay camera hook is the `CCamera::UpdateFrustumPlanes` / frustum-builder path.
- V61.12 is the current ownership rule:
  - identify the exact gameplay `CView` camera,
  - temporarily inject current HMD matrix only while the gameplay frustum builder runs,
  - call the original,
  - restore the camera matrix immediately,
  - leave auxiliary/non-gameplay cameras untouched,
  - do **not** alter FOV in the culling path.
- Expected marker: `V61.12-CULL-SYNC gameplay CView only; matrix restored; FOV untouched`.
- `V61.12-CULL-SKIP` on auxiliary cameras is expected and desirable.

## OpenXR submission / GOOD MONO

- The current stable bridge may submit the same KCD2 source image to both eyes.
- OpenXR projection views must still keep the runtime-provided per-eye poses and FOVs.
- Never replace both submitted eye poses with a cyclopean center pose.
- Current Quest 3 / VirtualDesktopXR FOV has been stable across logs:
  - left eye approximately `(-0.94248, +0.69813, +0.76794, -0.95993)`
  - right eye approximately `(-0.69813, +0.94248, +0.76794, -0.95993)`
- Preferred KCD2 source/PRESENT resolution is now **1920x1800** for performance.
- XR eye swapchains may remain runtime-recommended **2496x2688**.
- 90 FPS remains the preferred cap.

## Dialogue / input

- Dialogue baseline: `ForcePOV=1`, `NoCameras=0`.
- A -> E Windows key injection is proven for interaction/select.
- F8 is retired in the current branch.
- Rotation-order experiments and the old world/head translation-scale workaround are retired.

---

# 2. Safety lessons — permanent rules

These are not candidates for another test unless the architecture changes fundamentally.

1. **No hardware data breakpoints.** They froze the camera.
2. **No second D3D12 device in-process on the same adapter.** Historical tests produced clean device removal.
3. **Do not revive `r_CustomRes*` runtime mutation.** It correlated with GPU/resource reallocation failures/device removal.
4. **Do not hook overlay-owned COM/API entry points when a game-internal owner is available.** Steam/Discord/NVIDIA layering produced crashes.
5. **Never guess a native ABI from partial disassembly.** Historical 6-as-4 and 5-as-4 argument mistakes caused real crashes.
6. **Every renderer mode switch must reset all derived state.** Past bugs include swap-eye double-negation, pseudo/mono UV leakage and source-FOV desynchronization.
7. **Keep runtime logging sparse.** High-frequency diagnostics caused measurable head-motion jitter/choppiness.
8. **No speculative FlashUI vtable writes.** Read-only/static proof first.

---

# 3. Experiment classification matrix

| Branch | Historical evidence | Later correction / interpretation | Verdict now |
|---|---|---|---|
| CRenderView mode 2 as second eye | V7-V27 changed derived camera state and could visibly affect output | V45 static mapping proved mode 2 = **Shadow View** | **Closed as eye path**; retain only as shadow/camera-lineage evidence |
| CRenderView type 1 Recursive | Static proof: `GetOrCreateRenderView(1)`, `WHGame+0x2525414`, `FX_DrawToRenderTarget`, water-reflection marker | Runtime logs searched so far contain no natural `requested=1(Recursive)` / recursive-pass execution in tested scenes | **Open only as engine-owned secondary-view producer**, not as a ready eye |
| vcam / second camera imitation | User saw right-eye moving shadows and a zero/origin camera | Consistent with hitting incomplete secondary/shadow/RTT lineage without valid gameplay-camera/full scene graph | **Do not repeat unchanged**; trace producer -> camera -> pass flags -> scene color first |
| AFR | Real parallax and depth; rotational reprojection eventually worked | Moving NPCs/translation remained double-image/2-frame; head motion unplayable | **Closed permanently as final stereo solution** |
| CSystem+0x598 gate | Millions of calls; always false; generic getter | Not stereo-specific | **Closed** |
| `$VolCloudLeftEye` | Looked eye-like | Static context proves volumetric-cloud resource name | **Closed as eye selector** |
| `CStandardGraphicsPipeline +0x818/+0x828` runtime probe | Small alternating fields; same CRenderView showed different values; no stable eye identity | Probe caused user-visible choppiness; `crvMode` unresolved | **Runtime path closed**; static-only if needed |
| Legacy stereo CVars | Real registrations and values | Normal gameplay path not shown to consume them as a scene-eye selector | **Do not blindly enable**; use only as xref anchors |
| `g_stereoFrameworkEnable` | Exists and defaults/live=1 | Enabling was never the missing gate | **Not a solution by itself** |
| UV reprojection / crop ladder V59.1 | Attempted image correction | User saw worse head-turn warping | **Closed** |
| FOV/source-FOV live slider | Higher game FOV paradoxically looked more zoomed | V59.8 found source-FOV variable was not kept in sync | **Bug understood; branch useful again with correct identity/calibration** |
| generic published-camera FOV as live source | Published camera value varied in dialogue/inventory/cinematics | V60.1 correctly stopped trusting arbitrary publishes | **Old generic route closed**; **reopen via V61.12 exact gameplay CView identity** |
| V60.2 deferred REALCAL | Implemented/documented marker `V60.2-FOV-REALCAL` | No marker found in the reconciled handoff logs searched; live verification is unclear | **Reopened**, but implement at gameplay-camera-owned point instead of generic delayed sample |
| camera dolly-back / height | Older handoff records 0.7m back / 0.6m up as subjectively useful | Current stable branch later returned offsets to zero | **Historical optional comfort tool**, not baseline and not projection fix |
| HUD XML width/height as scale | V61.20+ tuner and recent 0.70 test | 0.70 clipped bottom HUD/dialogue; dimensions are viewport/safe-area-like, not object scale | **Closed as true scale solution** |
| HUD XML X/Y | True origin semantics fixed | In-game PAK often locked; writes fail live | **Useful offline/startup positioning experiment only** |
| FlashUI live `CombatCursorCross` +0x108 | V61.25 F12 visibly changed real cursor state | Robust live HUD object lookup after delayed retry | **Proven safe live manipulation path** |
| guessed Flash child `+0x168` | Argument type unresolved | No semantic proof of transform | **Do not call** |
| WeaponSolver/CPoseData read path | `WHGame+0xA7F9CC`, live CPoseData and hand joint IDs | Good player/animation correlation source | **Keep as read/correlation anchor** |
| OperatorQueue-after CPoseData write | V61.25 real buffer1 writes executed through verified getter/setter | User saw no visible arm movement | **Closed as final rendered ownership point** |
| old analytic two-bone arm IK | V49 already designed native shoulder root -> elbow -> controller grip | Missing piece was final rendered write boundary, not IK math | **Reopened** once final skeleton owner is proven |
| final skinning / CCharInstance | Recent static trace finds job thunk `WHGame+0xDE7A18` and computation around `WHGame+0x5FE260` | Much later than OperatorQueue; analogous to successful Crysis/Cyberpunk final-pose strategy | **Highest-priority VRIK path**, read-only first |

---

# 4. Native stereo — what the five-day history actually tells us

## 4.1 The mode-2 history was real runtime behavior, but the interpretation was wrong

Early V7/V10/V15/V17 work was not imaginary. It successfully found engine-managed derived CRenderViews and temporarily altered camera-pair data. Later logs even proved native writes could run safely and that an earlier OpenXR singleton split was a genuine bug.

However, V45 later resolved the constructor view-type enum:

- `0 = Normal`
- `1 = Recursive`
- `2 = Shadow`
- `3 = BillboardGen`

Therefore the entire "mode 2 could be a native second eye" theory is superseded. The visual moving-shadow behavior is exactly what should be expected from that correction.

**Preserve the old work only for:**
- CRenderView lifecycle/manager knowledge,
- camera bank layout/selectors,
- parent/derived-view relationships,
- proof that temporary camera mutation can propagate into generated renderer state.

Do not re-enable it as an eye path.

## 4.2 AFR is genuinely closed

AFR got farther than most branches:

- distinct cached left/right frames,
- real IPD after the singleton fix,
- rotational stale-eye reprojection genuinely active,
- real binocular depth.

But it is one simulation frame apart. Rotational reprojection cannot correct moving characters, world translation or disocclusion without depth/motion reconstruction. The user observed moving NPC double images and unplayable motion.

This is a structural ceiling, not another tuning task.

## 4.3 Recursive View is not disproven; forced-vcam reuse is

Static KCD2 evidence remains valuable:

- `WHGame+0x43EB60` -> `GetOrCreateRenderView(EViewType)` behavior.
- `WHGame+0x2525414` -> recursive rendering-pass construction, requests view type 1.
- direct owner in `CD3D9Renderer::FX_DrawToRenderTarget`.
- debug context includes water reflections.

But the reconciled archived logs searched so far contain **zero** `requested=1(Recursive)` / `V45-RECURSIVE-PASS` events in those tested sessions. So the branch should not be described as a live second view already waiting to be repurposed.

The correct future question is narrower:

> What complete engine-owned producer supplies camera state, render flags, scene graph and color/depth targets to a type-1 Recursive view, and can that producer be instantiated or reused safely for an eye?

No guessed manual call to the builder until this is answered.

## 4.4 Best newly-ranked stereo path: exact shipped stereo resources

Static inspection of the exact shipped shader PAK found:

- `Shaders/HWScripts/CryFX/Stereo.cfx`
- resource names/concepts including `StereoL`, `StereoR`
- `QuadLayer0`
- `QuadLayerHeadLocked0`
- `QuadLayerHeadLocked1`
- `PostStereo`-style L/R output in post effects.

The depth/parallax `PostStereo` shader by itself is likely CryEngine pseudo/depth stereo and should not be confused with genuine same-frame geometry stereo.

But the **resource ownership is highly valuable**. The current highest-value static stereo work is:

1. xref the exact `StereoL` / `StereoR` resource names into `WHGame.dll` / renderer setup;
2. identify who allocates/binds them;
3. identify whether they are filled by one post-process or two scene passes;
4. trace eye/view index and camera/projection data into that producer;
5. correlate the final resources with D3D12 RTV/depth targets only after the C++ ownership is understood.

This is preferable to another hot `CStandardGraphicsPipeline` field probe.

## 4.5 `r_3MonHack` / HUD FOV path deserves static re-evaluation

Older work treated multi-monitor CVars mainly as leftovers. Fresh static inspection found live renderer code around `WHGame+0x254DF7F` reading `HUDFOVX` / `HUDFOVY` and deriving renderer state.

This is **not permission to enable `r_3MonHack` blindly**. It is useful because multi-monitor rendering solves the same class of problem as VR: off-center projections and HUD placement across non-standard view regions.

Static goals:

- map backing globals for `HUDFOVX/HUDFOVY` and any left/right/center projection offsets;
- identify the exact projection-matrix terms they feed;
- compare those terms with OpenXR asymmetric-frustum `m[8]/m[9]`-style center offsets;
- identify whether HUD is transformed before or after scene projection.

This branch may help both view centering and HUD placement even if it never becomes native stereo itself.

---

# 5. Projection / FOV / optical center — reopened with new context

## 5.1 Why bottom black appears first

The Quest/VDXR eye FOV is not vertically centered. Repeated runtime values are approximately:

- up `+0.76794 rad`
- down `-0.95993 rad`

In tangent space that is roughly:

- top `+0.966`
- bottom `-1.428`
- center approximately `-0.231`

A symmetric source frustum therefore runs out of image at the bottom before the top unless it renders enough extra vertical coverage and/or the compositor mapping accounts for the shifted optical center.

That matches the user's observation: reducing FOV first creates a bottom black band, with other edges appearing only at more extreme settings.

## 5.2 Historical FOV evidence becomes useful again

The older roadmap records two crucial facts:

- raising the game/source FOV toward the 100-110 region improved border/coverage behavior;
- `cl_fov` did not map 1:1 to the actual published camera FOV in the old measurement path (`cl_fov=110` was documented as about 81.69 degrees published camera FOV).

V60.1 correctly rejected continuous generic camera-publish FOV because dialogue/inventory/cinematic cameras polluted the signal.

**V61.12 changes the situation.** We now have an exact gameplay-camera identity gate. Therefore the old real-FOV idea can be reopened safely by publishing FOV only from the validated gameplay `CView`/camera context, without trusting auxiliary camera publishes.

## 5.3 Recommended projection architecture

Keep three quantities separate:

1. **Game-rendered source FOV** — what the exact gameplay camera actually renders.
2. **Source optical-center offset** — how the symmetric game image is mapped into the asymmetric Quest frustum.
3. **OpenXR submitted per-eye FOV/pose** — always the runtime-provided eye data.

Do not use presentation zoom as a substitute for these. Old notes correctly found flat UV zoom reintroduces edge warping.

Next combined build should expose:

- baseline 95 source mode,
- safe wider presets around 100 / 102 / 110 for rapid comparison,
- a live **presentation Center-Y** control (rough expected useful region around -0.09 to -0.10 in normalized sampling space; exact sign must be verified visually),
- a reset-to-zero action,
- logging of exact gameplay-camera FOV and calculated full-cover requirement.

All of this belongs in presentation/remap math, **not** in the V61.12 culling camera write.

---

# 6. VRIK — five days of work condensed into the real remaining problem

## 6.1 The arm solution was already conceptually correct in V49

The old V49 plan already described the architecture that successful VR body mods use:

- native pelvis/legs/locomotion remain game-driven;
- shoulder/clavicle roots come from the native skeleton;
- solve shoulder -> elbow -> wrist analytically to the OpenXR grip target;
- use a stable chest-relative elbow pole;
- preserve controller wrist orientation at the hand;
- keep game weapon/combat systems as much as possible;
- suppress authored animation only when it directly fights tracked upper-body pose.

The missing problem was never "how to do two-bone IK." It was **where to write it so the renderer actually uses it**.

## 6.2 WeaponSolver and CPoseData remain useful, but not as final owner

Confirmed native anchors:

- `WeaponSolver::SetIKTargets` = `WHGame+0xA7F9CC`.
- `solver+0xC` / `solver+0xE` are 16-bit joint IDs.
- gate 2 uses CPoseData buffer1 API.
- CPoseData vtable = `WHGame+0x3A71510`.
- buffer1 full getter: vtable +0x50 -> `WHGame+0x53D830`.
- buffer1 setter: vtable +0x70 -> `WHGame+0x53F4D0`.
- buffer0 setter: vtable +0x30 -> `WHGame+0x53F510`.
- `WHGame+0x53D7C4` is **not** a full `QuatT[0x1C]` getter; do not reuse the old mistaken interpretation.
- `QuatT` size is 0x1C (quat + Vec3).

These are valuable for joint identity, live character correlation and animation semantics.

## 6.3 V61.25 is the decisive ownership experiment

V61.25 finally performed real, verified buffer1 writes after `COperatorQueue::Execute`. The values changed by approximately the intended 8 cm pulse and API validation was correct.

The user saw **no visual skeletal movement**.

Therefore:

- do not keep modifying OperatorQueue-after CPoseData;
- do not describe earlier zero-apply builds as "overwritten writes" — they did not write;
- V61.25 is the first clean proof that this stage can be written yet does not own the final visible arm pose in the way needed.

Possible explanations remain:

- later pose rebuild/overwrite,
- buffer not used by final skinning,
- WeaponSolver joint IDs are not the visible rendered arm joints at that stage.

All lead to the same practical conclusion: move downstream.

## 6.4 Highest-priority target: CCharInstance final skinning/post-process

Recent static work found a much stronger late boundary:

- profile context: `CCharInstance::SkinningTransformationsComputation(...)`
- job/worker thunk around `WHGame+0xDE7A18`
- final computation path around `WHGame+0x5FE260`
- preliminary static analysis indicates a seven-argument job ABI.

This is much closer to the architecture used by successful mods:

- Crysis VR: late `ISkeletonPose::SetPostProcessQuat` style writes.
- Cyberpunk VR: mutate the final animation/bone pose buffer after normal animation layers have done their work.

Recommended sequence:

1. **Passive ABI-safe hook only** at the final-skinning boundary.
2. Correlate calls with Henry's known `CPoseData` / `CCharInstance` chain.
3. Identify final joint count / joint table / local-vs-model transforms.
4. Prove one visible arm joint with a tiny, reversible read/write only after ownership is certain.
5. Then plug the already-designed two-bone arm solve into shoulder/elbow/hand.
6. Only later consider selective animation-layer suppression for combat actions that fight the controller pose.

No full-body procedural rewrite is needed for the first milestone.

---

# 7. HUD / UI — what is actually proven

## 7.1 XML viewport is not HUD scale

Recent user test:

- scale 0.70 wrote a 1310x1411 position rectangle,
- no crash,
- lower HUD disappeared,
- dialogue text disappeared.

This independently reconfirms the older V60.1 conclusion that the XML path is safe-area/viewport-like and clips/reframes content. It is not a general Scaleform root scale.

The later 1.30 wrapper test was initially invalid because the writer still clamped to 1.10; corrected 1.30/1.40/1.50/2.00 standalone packs were created afterward for theory testing.

Do not make XML width/height the main HUD-scaling architecture.

## 7.2 FlashUI live object access is real

Proven live path:

- global interface `WHGame+0x492D938`.
- global vtable +0x70 lookup with `"hud"`.
- HUD vtable +0x238 child lookup.
- `CombatCursorCross` child exists.
- child vtable +0x108 bool/state call visibly affects the real cursor.
- startup global may be null; periodic delayed retry is the correct solution and is proven.

Keep this as the live UI bridge.

Do not guess +0x168 as position/scale until its arguments are statically traced.

## 7.3 Old GFx mod archaeology points to the right next target

The older UI-mod analysis found KCD2-specific Scaleform symbols:

- `g_RatioPreset`
- `fc_setRatioPreset`
- `RATIO_PRESET_GAME`
- `RATIO_PRESET_CUTSCENE`
- `RATIO_PRESET_DIALOG`
- `RATIO_PRESET_DIALOG_EXTENSION`
- `RATIO_PRESET_MAIN_MENU`
- `resolutionScale`
- `SCREEN_WIDTH`, `SCREEN_HEIGHT`
- `updateDialogPosition`
- `cinematicRatio`, `ratioStrips`, `scaleMode`, `noScale`

This is a better semantic route to HUD/dialog positioning than modifying the viewport rectangle.

Best HUD goals:

1. identify root movie/object transform (`_xscale/_yscale` or equivalent) semantically;
2. identify ratio-preset update entry point and dialog-position routine;
3. map root X/Y and scale to VR-menu sliders;
4. keep cursor separately controllable through the proven live child path;
5. longer-term: investigate exact renderer `QuadLayerHeadLocked*` resources as a possible native head-locked HUD surface.

---

# 8. VR menu — known regression and concrete fix

Old V52/V53 design already had the intended behavior:

- right Touch controller grip pose ray intersects the head-locked menu quad,
- hit maps to 1100x680 menu pixels,
- right trigger/A selects,
- thumbstick is fallback.

Later source regressed this. `PumpVrInput()` still receives `pointer_x/pointer_y/pointer_valid` but the visible cursor was advanced from thumbstick input instead of consuming the ray coordinates.

This directly matches the user's current report: Touch pointing does not move the cursor; mouse works but moves the game view.

V61.27 should restore the original intent:

- ray valid -> cursor position = ray hit directly;
- ray invalid -> thumbstick fallback;
- trigger/A click only while menu has focus;
- do not move game camera from mouse/controller while the VR menu owns pointer input.

Physical menu target: **20% smaller**, changing width from about 2.05m to **1.64m**, with intersection geometry scaled identically. Keep the 1100x680 texture resolution.

---

# 9. Do-not-retest list

Unless new static evidence changes the premise, do **not** spend runtime sessions on:

- hardware data breakpoints;
- second D3D12 device;
- `r_CustomRes*` live writes;
- AFR as final stereo;
- CRenderView type/mode 2 as an eye;
- generic CSystem+0x598 stereo gate;
- `$VolCloudLeftEye` as scene-eye selector;
- `CStandardGraphicsPipeline +0x818/+0x828` hot probing;
- blind `r_Stereo*` / `g_stereoFrameworkEnable` activation as the missing stereo switch;
- V59.1 UV reprojection ladder;
- arbitrary generic published-camera FOV following;
- OperatorQueue-after CPoseData as final FRIK owner;
- HUD XML width/height as a true scale control;
- blind FlashUI +0x168 calls;
- speculative CRenderView construction / guessed recursive-pass ABI.

---

# 10. Reopened / high-value paths

## Projection

- Exact gameplay-camera FOV publication using V61.12 identity.
- Quest asymmetric-frustum optical-center correction in presentation mapping.
- Safe wider source-FOV presets around 100/102/110 for rapid visual calibration.
- Static `r_3MonHack` / `HUDFOVX/HUDFOVY` projection-center analysis.

## Native stereo

- C++ owners/producers of shipped `StereoL` / `StereoR` textures.
- `QuadLayer0` / `QuadLayerHeadLocked0/1` ownership.
- Complete engine-owned Recursive/RTT producer only if static trace proves a full scene camera/render graph.
- Scene-color lineage and real eye/view index at renderer resource creation/binding stage.

## VRIK

- Henry `IEntity -> CAnimatedCharacter -> CCharInstance` chain.
- final skinning/post-process boundary around `WHGame+0x5FE260` / job thunk `WHGame+0xDE7A18`.
- final visible joint table/name/index mapping.
- then reuse V49 analytic two-bone arm solve with controller grip targets.

## HUD/UI

- live FlashUI root transform / ratio preset.
- `fc_setRatioPreset` / `updateDialogPosition` semantics.
- `QuadLayerHeadLocked*` renderer resources.
- proven live cursor child +0x108 retained.

---

# 11. Proposed V61.27 combined build — one useful runtime session, multiple answers

User preference is explicitly to avoid isolated micro-tests. V61.27 should therefore combine safe independent diagnostics/controls without mixing speculative writes.

## Always-on known-good behavior

- V61.12 gameplay-only culling.
- GOOD MONO with real per-eye OpenXR submission poses/FOV.
- 1920x1800 preferred KCD2 source baseline; no forced resolution.
- FOV baseline stays selectable; do not silently replace user settings.
- dialogue `ForcePOV=1 / NoCameras=0`.
- A -> E interaction.
- stereo hot probes disabled.

## User-facing menu fixes

- 20% smaller menu physical quad.
- right Touch ray directly controls menu cursor.
- thumbstick fallback.
- presentation Center-Y slider with reset.
- source-FOV quick presets 95 / 100 / 102 / 110 (or equivalent slider) with explicit reset.
- do not expose XML viewport width/height as "HUD Scale".

## Passive diagnostics

- publish exact gameplay-camera FOV only when the V61.12 gameplay camera identity is true.
- log calculated source coverage and optical-center target sparsely.
- passive final-skinning observer only if seven-argument ABI audit is complete.
- static-derived stereo resource observer only if it can be attached without sharing the retired hot pipeline address.

## No active VRIK/stereo memory writes yet

The next active write should be chosen only after the passive session proves:

- Henry's final skinning object and one visible arm joint, or
- a genuine stereo scene-color producer with a valid eye/view identity.

---

# 12. Key addresses / anchors to keep with the project

## Camera / renderer

- gameplay frustum/camera ownership: preserve current AOB-resolved V61.12 path.
- `WHGame+0x43EB60` — `GetOrCreateRenderView(EViewType)`-like function.
- `WHGame+0x2525414` — Recursive render-pass builder.
- `WHGame+0x57278C` — CStandardGraphicsPipeline dispatcher; do not layer hot probes here again.
- live `HUDFOVX/HUDFOVY` consumer region: around `WHGame+0x254DF7F` (static follow-up required).

## FlashUI

- `WHGame+0x492D938` — live UI interface/global.
- UI global vslot +0x70 — lookup `"hud"`.
- HUD vslot +0x238 — child lookup.
- `CombatCursorCross` rdata `0x3C33BE0`.
- cursor child +0x108 — proven live bool/state operation.

## Animation / VRIK

- `WeaponSolver::SetIKTargets` — `WHGame+0xA7F9CC`.
- CPoseData vtable — `WHGame+0x3A71510`.
- CPoseData buffer1 getter — `WHGame+0x53D830`.
- CPoseData buffer1 setter — `WHGame+0x53F4D0`.
- CPoseData buffer0 setter — `WHGame+0x53F510`.
- `COperatorQueue::Execute` candidate — `WHGame+0x6D7654` (retired as final write stage).
- final-skinning job thunk — `WHGame+0xDE7A18`.
- final skinning computation candidate — around `WHGame+0x5FE260` (ABI must remain verified before hook/write).

---

# 13. Evidence/provenance map

Use these files before resurrecting an old branch:

## Older scaffold

- `00_HANDOFF_CURRENT_STATE.md` — old synthesized current state; useful but superseded by later V61.x results.
- `ROADMAP.md` — chronological major updates and reasoning.
- `NATIVE_EYE_EXPERIMENT_V7.md`
- `NATIVE_SELECTED_CAMERA_PAIR_V10.md`
- `NATIVE_STEREO_INJECTION_V15.md`
- `NATIVE_STEREO_INJECTION_V16.md`
- `NATIVE_STEREO_INJECTION_V17.md`
- `PARENT_CAUSALITY_V25.md`
- `PARENT_PAIR_V26.md`
- `PARENT_ISOLATION_V27.md`
- `V45_STATIC_FINDINGS.md`
- `V45_RECURSIVE_VIEW_RTT.md`
- `AFR_STEREO_V39.md`
- `V49_CONTROLS_AND_VRIK_PLAN.md`
- `UI_MOD_FINDINGS_V60_1.md`

## Corrected later handoff

- `KCD2VR-HANDOFF.md` — V61.12 through V61.21-ish summary and later corrections.
- `research_docs/FLASHUI_V61_21_STATIC_FINDINGS.md`
- `research_docs/STEREO_V61_21_STATIC_PROGRESS.md`

## Runtime logs

When docs and logs disagree, prefer the later log for runtime status. Important marker families:

- `V61.12-CULL-*`
- `XR-VIEWS-V21`
- `V58.6-SOURCE-QUALITY`
- `V45-GET-RENDERVIEW`
- `V61.21-STEREO`
- `V61.21-FLASHUI HUD-LOOKUP`
- `V61.24-FRIK-CAPTURE`
- `V61.24.1-LATE-POSE MATCH`
- `V61.25-LATE-POSE APPLY`
- `V61.26-RESOLUTION`

Historical handoff-log grep performed during this retrospective found:

- no `V60.2-FOV-REALCAL` marker in the corrected handoff logs searched;
- no natural `requested=1(Recursive)` / `V45-RECURSIVE-PASS` markers in those archived sessions;
- many `V58.8-NATIVE-EYE-PASS` events on mode 2 (now known to be Shadow view);
- many V61.12 gameplay/auxiliary culling markers;
- many delayed FlashUI HUD lookup hits;
- repeated 1920x1800 source-capture markers.

Later conversation logs supersede the corrected handoff on V61.25: they prove real OperatorQueue/CPoseData writes occurred and were visually ineffective.

---

# 14. Bottom line

The five days were not wasted. The investigation removed several entire wrong layers:

- mode-2/shadow is not an eye,
- AFR cannot become comfortable same-frame stereo,
- generic stereo-looking CVars/gates are not the renderer owner,
- XML viewport resizing is not HUD scale,
- OperatorQueue CPoseData is not the final visible arm owner.

That leaves a much narrower and better-founded problem:

**Stereo:** find the producer/owner of the exact shipped `StereoL/StereoR` scene resources or a complete engine-owned secondary scene pass.

**VRIK:** identify Henry's final `CCharInstance` skinning/post-process data and write the already-designed two-bone arm solution there.

**Projection:** derive source coverage and optical-center correction from the exact gameplay camera plus the asymmetric Quest FOV, without touching the proven culling path.

**HUD:** use live FlashUI/Scaleform semantic transforms and ratio presets; keep XML only as an offline viewport experiment.

This is the state future work should start from.
