# D84 review: physical walking, dodges, corpse handling and crossbows

Review date: 2 November 2026. Source baseline: the supplied D83R1 tree. This report describes the motion/physics lane of the D84 repair and distinguishes source defects, historical headset evidence and behavior that still needs a current headset run.

## What changed

Five concrete problems were repaired:

1. Native pad dispatch could combine the previous frame's forward direction with the next frame's dodge button. The pad publisher and dispatcher now exchange one coherent snapshot. The existing side/back dodge recognizer, native acceptance rules and input order remain intact.
2. The visible physical-walking controls selected the old offset-to-stick follow path, while finite native displacement had a separate switch. An explicit physical-walking choice now selects the existing D76 finite-step path. The controls agree with each other, and F1 controls torso following without silently enabling continuous walking.
3. Corpse handling depended on physics/combat modes and diagnostic keys in several places. Selected corpse gameplay now runs independently of F9 recording and physical combat. F9 can report physics status on its own. The native identity, death, hand, thread and VM checks remain mandatory.
4. Crossbow diagnostic producers were enrolled only in F11. Their existing owner/clock/physical-binding evidence is now available through F5, F7 or F11, with the original recorder and session retained through queued delivery. This change does not replace the physical reload state machine with gesture-triggered animation.
5. F9 alone did not enroll the existing native-held stage, passive hand-contact or raw-ray queues. Those producers now accept the independent Physics recorder and preserve the originating session through delivery. Recorded data stays separate from collision and haptic gameplay.

These are source repairs with focused production-path tests. They do not establish that a dead NPC moved in the game, that every crossbow asset binds correctly, or that every physical dodge is accepted by the native combat system.

## Evidence and provenance

The audit retrieved all 90 diagnostic candidates, identified 69 distinct full log contents by SHA-256 and selected the newest 50 distinct contents by their recorded timestamps. The selected recordings cover 1 October through 1 November 2026 and contain 8,135,923,472 uncompressed bytes. A same-byte alias counts once; distinct contents can still overlap or belong to one play session. Counts below describe matching records or snapshots, not automatically distinct player actions.

The provenance index is `evidence/log_corpus/INDEX.json`; the completed scan summary is `evidence/log_corpus/SUMMARY.json`. Each selected log retains its original file/container name, member name, timestamp range, full-content hash and local path. This lane's summaries are in `evidence/motion_physics_d84/`:

- `motion_physics_capture_summary.json`: physics modes, corpse refusal records and motion families.
- `motion_directions_and_free_view.json`: actual reported dodge directions and free-targeting counters.
- `crossbow_capture_summary.json`: 195 precise crossbow lifecycle/refusal/binding rows in eight selected captures, with first/last examples and original line numbers.
- `native_script_api_usage.json`: relevant references in the supplied game scripts.
- `pad_dispatch/`, `roomscale_menu/`, `corpse_mode/`, `reload_capture/`, `crossbow_d82/`, `physics_capture/`: test commands, extracted source, source hashes and complete results.

The latest selected diagnostic archive, `KCD2VR-diagnostics(8).zip`, contains a D80 recording from 1 November, 21:51:14–22:16:53. Its extracted log SHA-256 is `84ddd2b408293d580c8939167200e4b97663a57c8f6f8730138d0e58f0a93fa0`. The D79 archive is also heavily represented in the relevant evidence. A separate newer main log, `KCD2VR(20261102-000003).log`, identifies a D81R1 run; its hash is `a644f4d0893b855a0962fd11805eeb7acc9da61305cc1cc706d99562b658d938`.

The 1 November 16:53 recording is historically labelled D79 in the handoff context, but the canonical index detects no in-file build marker for it. References to D79 below retain that historical association; they do not assert a detected marker.

Those recordings establish what happened in those builds. They are not headset validation of D83R1 or D84. The D83R1 source already includes later crossbow corrections, so an old refusal cannot justify reverting a newer working ownership repair.

## 1. Wrong-way or unreliable dodges: a real input publication race

### Defect

The old native pad publisher stored axes, combat ownership and buttons in separate atomic values. The dispatcher read axes first, emitted events, read axes again for directional pseudo-keys and read the dodge button later. A producer publication between those reads could yield:

- analog direction from the previous forward movement;
- directional key state from the next side/back request;
- B from that new dodge request.

The recognizer can correctly request `ly <= 0` while the native consumer receives B with an older positive Y. This is a concrete source-level way to request an unintended forward dodge. It is not proof that this race caused every forward dodge the user saw.

### Repair

`src/subsystems/hud_ui_controls/runtime/pad_dispatch_d84.h` defines a trivial pad frame containing the four axes, the existing packed combat/trigger ownership record and the remaining buttons. The real publisher in `native_input_look_lab.cpp` publishes that frame through the existing bounded `SnapshotMailbox`. The native flush reads one frame before emitting any event and uses it throughout that dispatch.

The existing 8 ms dispatch throttle, analog/digital emission order, trigger ownership encoding, button hysteresis and held/release semantics remain unchanged. No pose prediction, stereo timing or global input delay was added. If no coherent snapshot is available, the flush waits for the next normal dispatch rather than synthesizing mixed input.

### Evidence and verification

The corpus contains 23 dodge request rows. Seven recent rows use `attemptedDirection`; all seven are side/back requests. Sixteen older rows use `dir`; four contain a positive forward component in 18–19 October recordings. The current recognizer's no-forward clamp and finite gesture retirement already address that older recognizer behavior and were preserved. Recent rows explicitly say native acceptance is unobserved; an emitted request alone is insufficient to call the native dodge successful.

The new test extracts the literal publisher, pad storage and native dispatcher. It publishes a new frame at every emission boundary in forward-to-side-B, forward-to-back-B, held-to-neutral and neutral-to-held scenarios. All 246 checks pass in debug, optimized and ASan/UBSan configurations. The immutable D83R1 preimage compiles and fails the same coherence assertions: 116 failures across those forced interleaves. See `pad_dispatch/results.json`.

## 2. Physical walking: keep the finite path and connect the controls

### Existing source behavior

The D76 implementation is already a displacement path. It does not convert the headset's distance from the origin into a continuous joystick speed. `pose_producer_d76.inl` observes distinct tracked HMD samples, derives their horizontal difference, and submits that finite difference to the qualified native living-physics collision step. It retires stale, duplicate, rejected or ownership-invalid intent rather than repeatedly replaying a remaining offset.

The retained mapping is:

`worldDelta = worldRight * rawXDelta - worldForward * rawZDelta`

The horizontal forward vector comes from the calibrated tracking-to-body frame combined with the current native body/world transform. Right is its horizontal perpendicular. Model scale and pitch do not turn a 0.20 m horizontal physical displacement into a different number of metres.

The native step adds the requested delta once. Separate entity/character adoption receipts identify the corresponding native occurrence. The camera removes consumed raw tracking displacement only after native adoption. A requested delta is not presented as measured body movement, and a blocked or cancelled request is not stored as a persistent walk debt. These ownership and coordinate rules were preserved.

### Actual blocker in selection

The reviewed INI contains `VrikBodyFollow6DOF=true`, `VrRoomscaleFollow=false`, `VrRoomscaleDodge=true` and `VrNativeRoomscale1to1D76=false`. Torso following and dodges were selected; finite physical walking was not. The old visible walking controls could switch `VrRoomscaleFollow` on while leaving D76 off, choosing the offset-to-stick behavior the user explicitly dislikes.

An additional inconsistency remained in F1: the D81 `bodySelection` helper already preserved independent walking and dodge choices, but the direct F1 hotkey still wrote legacy follow `true` whenever the torso was enabled. That hotkey side effect is removed by the shared Mod integration.

### D84 control behavior

The Mod getter now reports walking as native D76 **or** a previously selected legacy follow. The live roomscale cycle and the duplicate physical 1:1 row update the same requested walking state and preserve the dodge choice. Classic walking and four-state controls also select D76 when their requested walking bit changes. Selecting walking off disables that owner and does not fall back to the old servo.

Because D40 live settings apply before host slot 75, Mod captures the previous native preference before that apply. Slot 75 compares the requested walking bit and explicit native preference against the previous state. This prevents a later unrelated menu change from silently migrating an old legacy preference or turning an explicit OFF into ON. Existing native selection suppresses the legacy servo. BODY and HMD dodge remain separate settings; physical walking still needs the existing body/context admission.

The supplied explicit OFF is retained. To try finite walking, choose physical walking ON once in the menu; the change is then saved through the existing settings path. F1 only changes torso following. Legacy INIs remain interpretable, and the live row labels a still-preserved old selection as `LEGACY FOLLOW`.

### Tracking semantics and limitations

The official OpenXR LOCAL-space definition uses right-handed coordinates with +X right, +Y up and -Z forward. Reference-space changes take effect for locations at or after the event's change time. A valid position may be inferred while tracking is unavailable; validity alone is not evidence of current tracked translation. The current producer requires tracked, fresh positions and retires/rebases around tracking gaps and origin changes. These rules are consistent with the [Khronos OpenXR LOCAL-space reference](https://registry.khronos.org/OpenXR/specs/1.1/man/html/XR_REFERENCE_SPACE_TYPE_LOCAL.html).

The native Oculus tracking documentation likewise describes +X right, +Y up and +Z backward, distinguishes floor/eye origins, and describes yaw/position recentering. See [Meta's native tracking documentation](https://developers.meta.com/vr/documentation/native/pc/dg-sensor/). A left-handed convention from a different SDK should not be imported into this native OpenXR path.

HMD tracking reports the headset, not an anatomical pelvis or a detected footstep. Head-only data cannot reliably distinguish an upright sideways lean from a short body translation. The retained lowered-head exclusion uses a 10 cm height loss with hysteresis; it is a heuristic, not a complete lean classifier. Changing that threshold or claiming perfect lean/walk separation needs different evidence.

### Verification

The new pipeline fixture compiles the actual live reducer, actual Mod getter/prior-D40 snapshot/F1 block, actual host slots 75/84, real D76 setting transitions and all three classic control handlers. Native install/admission and camera publication are controlled leaves. All 121 checks pass in debug, optimized and ASan/UBSan configurations. The D83R1 preimage compiles and fails the finite-owner and F1-independence assertions.

Existing producer tests pass 38 checks; actual native callback/adoption tests pass 144 checks; the D75 no-forward dodge header tests pass 66 checks; the broader roomscale58 tests report no failures. The native callback test required a copied Linux host artifact to enable the Windows-only callback block and substitute clock/guarded-memory leaves with fixture-owned buffers. Identity checks, callback logic and the adoption ledger were not changed. This is not a native hook-install or Windows game execution result. See `roomscale_menu/results.json` and `native_adapter_source.json`.

## 3. Dead NPC grabbing: remove accidental gates, preserve actual qualification

### What the recordings establish

There are 931 `PHYS_DIAG` snapshots in the focused corpus, 14 lab-mode changes, four legacy `LAB_DRAG` refusals and one saved-provider `READY` record. No saved-provider `GRAB` or `DRIVE` receipt is recorded in the selected 50 logs.

In the latest D80 log:

| Recorded time / line | Evidence | Meaning |
|---|---|---|
| 1 Nov 22:13:58.075 / 1121323 | `f9=1 labRung=0 corpseDrag=1`; both hands current/empty; no held body | The requested preference and selected lab state disagreed. |
| 1 Nov 22:14:15.602 / 1179799 | Provider READY with `gate=F9_LAB_AND_CORPSE_DRAG` | The provider reported installation after the lab was selected. This is not a candidate, a native grip or body motion. |
| Same time / 1179800 | `f9=3 labRung=3 corpseDrag=1`; zero held/qualified candidates | The status still does not establish a usable body. |

Older 28 October rows include an occupied right hand, missing open-then-squeeze intent and `ROOT_STILL_LIVING`. These are distinct failures. A mesh that looks like a corpse is not necessarily a qualified dead NPC with the expected articulated root. The specific original lines and hashes are retained in `motion_physics_capture_summary.json`.

### Source blockers repaired

The previous code required lab rung 3 in `lab58::active(CorpseDrag)`, required F9 rung 3 in saved hand/provider admission, returned before corpse Tick at a different lab rung, and returned from `NativeRhythmTick178` when physical combat was off. Its optional physics observer additionally required F3, so F9 alone could not reveal the failure. Central settings also overwrote the corpse preference with false on save/load; the shared integration corrects that persistence.

The selected corpse preference now admits its existing gameplay service independently of recording. The native outer frame still establishes its stage thread/tick first. With physical combat off, the early branch calls `lab58n::Tick`; with combat on, the original late call remains. There is one call per path, preserving the combat-on ordering. Tick handles service retirement when the feature is turned off.

Plain F9 now selects the independent Physics recorder in the shared capture system. F9 or legacy F3 can enable the optional physics observer. The observer checks that finite recorder before reading hands/candidates or formatting receipts, and typed writes keep a Physics record from being rerouted because an old family name contains `F3`. Recorder expiry does not disable selected gameplay; recording alone does not enable it.

New pre-query refusal reasons identify failures that previously returned silently: current tracking/input, hand occupancy, native hand/body ownership, calibration, committed hand packet/freshness, hand claim, raw intent, retail qualification and script thread/context. `D84 PHYS_CORPSE_PROVIDER` reports installed transport, callback age and candidate/held flags using copied atomics. It explicitly does not infer body motion from installation.

### What remains required

The saved-corpse route still requires the exact native-script callback, original-first ordering, the same owner thread as the native frame, a current VM/context, a current native NPC death result, unchanged entity/character/root identity, no parent transfer, an empty current hand, completed raw hand intent and a consumed grip claim. First acquisition writes zero velocity. Release/retirement relinquishes ownership without writing an arbitrary velocity zero to a replacement body.

The legacy articulated route still needs a recent native death receipt and a qualified current root/part/local anchor. The saved route is a whole-entity `SetVelocity` servo. It is not a proven limb constraint, and an API return is not proof that the body followed the hand. Living-body actuation remains paused.

The supplied scripts contain real `actor:IsDead()` usages, but generic `EntityCommon:IsDead()` is also used for breakable object health and cannot substitute for NPC death qualification. No literal `GetBonePos`, `SetVelocity` or `GetEntitiesInSphere` calls were found in those Lua files. That absence does not prove those native-bound APIs unavailable. Authored `DeadBody_*` and ghost assets also cannot all be treated as current dead NPC physics actors. The audit does not invent an ABI or force a guessed ragdoll/constraint operation to bypass missing proof.

### Verification and remaining runtime question

The local mode/observer test, outer-dispatch test and real saved-service fixture pass in debug, optimized and ASan/UBSan configurations. The outer fixture retains the real early branch and late call, actual native stage/dispatch statements and the nexus depth/preview guard; unrelated combat work is a named controlled leaf. It checks exactly-once service with physical combat on/off, correct owner publication, feature-off retirement entry, F9-only observation and no nested/preview service.

The saved-service fixture exercises both hands with all recorders off, candidate/acquisition/drive/release, native-part priority, zero acquisition write and retirement for feature, retail, tracking, item, VM/thread, body, parent and epoch changes. The articulated adapter passes 56 checks; the lab policy passes 74. All three D83R1 preimages compile and fail the intended independence assertions. See `corpse_mode/results.json`.

Current headset evidence is still required for the first complete chain: provider callback → native dead candidate → consumed grip → same body/anchor → native velocity call → measured position/bone movement → release. The new diagnostics distinguish where that chain stops. The user's existing explicit OFF is not turned into an unqualified default ON.

### F9 also needs the existing contact and native-held records

The final routing review found another concrete coverage gap. `physics_stage_d42.h` accepted only F2/Bug and F7/Vrik, while passive contact and raw-ray capture accepted only F3/Combat and F5/Interaction. F9-only corpse status therefore could not be described as complete physics coverage.

The native-held stage queue now preserves its original F2 and F7 lanes and adds independently bounded F9 and F3 lanes. Each stage record carries its original domain and session to the typed receipt sink. Independent per-key budgets and END receipts remain intact. Scope construction reads no diagnostic clock when all applicable recorders are off, and this change adds no native physics query.

The passive-contact and raw-ray queues select one ticket from F9, F5, F7 and F3, in that priority order. A copied record includes its full active mask, so combined recording does not duplicate ray reads. Changed recorder ownership produces a current contact snapshot without altering the contact state or haptic policy. The service validates the original session's bounded receipt window before formatting a queued record; starting a different recorder cannot relabel an older record. Restarted or expired sessions reject stale queued data. Raw-ray witness bytes are not read when capture is off. A final already-captured receipt may drain within the existing five-second window, after which steady OFF does no capture formatting.

The actual producer headers and literal passive-contact service pass 20 new checks in each of debug, optimized and ASan/UBSan builds. The existing native-stage fixture passes 28 checks and the existing passive-contact/haptic fixture passes 140/140. Coverage includes F9 alone, combined mask `0x123`, independent stop/restart, original-session drains, one END receipt, F7 alone, and an invalid raw-witness pointer that is never dereferenced when all recorders are off. The D83R1 producer header, compiled against the current recorder registry solely to expose the new F9 domain, fails the expected F9-enrollment assertion. This is a producer preimage check, not a claim that the full D83R1 binary contained an F9 recorder. See `physics_capture/results.json`.

These fixes cover the identified native-held stage, passive-contact, raw-ray and corpse-observer routes. They do not assert that every unrelated historical physics experiment has become a supported F9 producer. The shared Mod integration supplies all three typed stage drains and separately gates host/profile formatting.

## 4. Crossbow reload: preserve one physical owner and expose its missing joins

### Historical failure chain

The latest D80 recording contains five `TAKEN` reload starts, three `INPUT_LOST` fallbacks and two `CLIP_UNSUPPORTED` refusals naming `CLIP_SCOPE_CHARACTER_ANIMATION_SET`. Its physical-binding snapshots remain unavailable: no joined tool/bolt mechanism, no demonstrated native cock/load acknowledgement and no demonstrated physical completion. Examples begin at lines 775283–775285, followed by `INPUT_LOST` at 775382; a clip-scope refusal appears at 805901.

The D79 recording contains three starts, two duplicate/ambiguous semantic-marker refusals and one native fallback. Earlier records show native frame-owner mismatches, missing source, and old F11-mode dependence. These explain how a gesture could appear merely to start the native animation in those builds. They do not show that the user's hand completed a physical reload.

The older code/history and the D83R1 source must be read together. D72 preserved the frame-owner boundary, later work added the real physical mechanism path, D81 separated instant nonphysical tails from physical stages, and D82 corrected exact multilayer/FIFO clock matching, first-pose bootstrap, passenger qualification and foreign-callback interference. D84 retains those repairs.

### Current ownership and physical stages

`crossbow_native_xr34.h` admits the supported native class IDs **10, 14 and 15**. One actual native reload child owns its clock and binding. It does not fabricate inventory, cocking or loaded state.

The physical branch retains these relationships:

- A direct string pull begins from current controller intent at the current qualified string. The rendered hand can be constrained to that string, while unmodified intent earns the pull travel.
- Tool retrieval uses the native tool and its read-back placement at the working hand. The native spawn marker controls visibility. Current string/tool geometry must admit engagement; the mounted mechanism then remains bound to the actual socket/handle.
- Progress comes from measured travel along the qualified axis with current mechanism/clock identity. The working clock is held before an unearned cock/load boundary. A self-corrected rendered hand cannot earn stroke credit by following its own destination.
- A grip at the right-shoulder quiver admits the original native ammo-acquire marker. The actual native bolt must bind and have a hand pose before placement proceeds.
- Bolt placement holds that same bolt. Releasing away from the valid placement does not resume automatic loading; a regrip can retry. Native load acknowledgement and nock binding remain required.
- Missing owner/clip/pose/binding evidence releases to the native fallback with a refusal reason. It is not reported as physical success.

This lane changes diagnostic enrollment and receipts, not those mechanics. No second reload owner or binary gesture-to-animation replacement was introduced. The current state machine and native clock/hand binding remain the appropriate implementation to validate against new evidence.

### F5/F7/F11 coverage

`reload_capture_d84.h` selects one live recorder ticket from Interaction/F5, Vrik/F7 or Reload/F11. Priority is F5, then F7, then F11; the record also carries the full active mask. Simultaneous recording yields one copied record with a common sample identity, avoiding repeated expensive native reads. Diagnostic enrollment never selects a gameplay reload mode.

Early native requests, source refusals, admitted starts, state changes and D77 binding packets retain that original domain and session. Source reasons only join the same domain/session, even when two recorder domains happen to have equal session numbers. Stopping one key does not stop another. Already copied records have only the existing bounded receipt-drain window; a replacement session rejects old records. Queued records cannot be relabeled as whichever recorder happens to be active at drain time.

The production drains use the shared `WriteReceiptFor(domain, session, row)` sink. Legacy family labels remain searchable, with explicit `recorder` and `captureMask` added. F5 or F7 alone can now reveal the crossbow failure chain; F11 remains compatible.

### Verification and unresolved native scope

The capture tests pass 23 checks in debug, optimized and ASan/UBSan modes, including all-off behavior, independent keys, simultaneous mask `0x2A`, same-number sessions, stop/restart/expiry and original-session drains. Existing D57 and D77 compatibility fixtures pass; the D83R1 preimage compiles and fails F5-only enrollment.

The targeted actual native bridge/clock/passenger fixture passes **606 checks** in debug, optimized and ASan/UBSan builds. Five deliberate historical mutants—single-layer clock, single-layer passenger, foreign scan publication, foreign busy revocation and inert-event cancellation—each fail their expected assertion. This guards against reintroducing known regressions. It uses controlled native/tracking leaves and does not prove that the user's current crossbow animation assets satisfy all runtime bindings. See `crossbow_d82/RESULT.json`.

## 5. Free targeting: avoid weakening the wrong ownership check

The selected corpus contains 5,450 free-targeting snapshots. In 3,849, selection, context, camera lease and hook are all active. A further 931 show the selection retained while context/lease are paused. Across the snapshots, 4,522 have a nonzero cumulative native-call counter and 4,406 have a nonzero cumulative veto counter. These are observations of counters, not counts of newly intercepted calls.

The native no-turn hook therefore does operate. Initial zero snapshots do not establish that it is unused. `ContactIdentity` in this path refers to the current Henry/NPC ownership relationship; it is not a test for the user's blade touching the enemy. Removing it would broaden which actor commands are suppressed. Protected native actions, uncertain ownership and ordinary motion also have distinct policy boundaries.

This lane found no evidence supporting a blanket removal of those ownership checks. It leaves the free-targeting hook intact and records the remaining distinction: camera freedom and body/paired-action ownership must be evaluated from the same current event. The combat lane's paired-action repairs are separate from this audit's no-turn finding.

## Acceptance evidence for the next run

Keep the chosen gameplay features separate from diagnostic recording. The relevant keys can run together:

| Action | Recorder(s) | Evidence needed |
|---|---|---|
| Physical 0.20 m right step, stop, then return; repeat after turning/recentering | F7, optionally F2 | Distinct tracked delta, native step/adoption, no repeated displacement while stationary, consistent yaw mapping and no accumulated debt. Test physical walking ON explicitly. |
| Left/right/back combat dodge, including a previous forward-stick frame | F3 + F7 | Current tracked translation, qualified request, coherent native pad direction/B and native combat response. A forward head movement must not request a forward dodge. |
| Grab a current dead NPC with either empty hand, pull, release | F9, optionally F7 | Provider age, exact refusal or candidate, consumed grip, same native body, actual observed movement and clean release. READY alone is insufficient. |
| Direct string, tool retrieval/engagement/stroke, shoulder bolt retrieval and placement | F5 + F7; F11 optional | Admitted owner/clip, matching completed pose/clock, actual tool/bolt IDs and pose readbacks, progress from intent, native cock/load/nock acknowledgement. |

If a stage refuses, retain that reason and its owner/sample identities. Do not compensate by adding another animation trigger, forcing a guessed physics class, widening all stale-pose windows or changing the stereo/camera basis without evidence.

## Test limits

All focused current tests and expected-failing preimage/mutant checks listed above pass. Assertions are enabled. ASan bounds/use-after-free and UBSan checks are enabled in the sanitized runs; LeakSanitizer is disabled because this workspace restricts the `/proc` inspection it requires. Windows SEH/hook installation, an in-game save, the headset compositor and live NPC/crossbow behavior were not executed in this lane. Production motion math, native collision-step/adoption logic, stereo and the D83R1 physical crossbow state machine were preserved.
