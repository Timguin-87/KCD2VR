# KCD2VR D85R5R1 — regression review and source patch

3 November 2026

Historical R1 report, amended after the user's corrections. The current source
and readiness status are in [D85R5R2_CORRECTIVE_REVIEW.md](D85R5R2_CORRECTIVE_REVIEW.md).
R1 verification counts below describe that revision unless explicitly labelled
R2; they do not certify the final R2 source or headset behavior.

Baseline: `KCD2VR-D85R5-KNIFE-POUCH-20261102`  
Patch: `KCD2VR-D85R5R1-REGRESSION-PATCH-20261103`

## Result and scope

This patch repairs demonstrated source defects in elbow control, activity
reference lifetime, supported-weapon response, temporal output publication,
alchemy timing, pickup preparation, crossbow mechanism observation and native
kick selection. It applies the requested open-palm HUD behavior and prompt size.
The original movement-option override was removed in R2.
It includes targeted diagnostics for the remaining
native joins instead of presenting an enabled setting as a completed feature.

**No DLL or ASI was built. No new game, GPU or headset run was performed.** The
host checks exercise production functions with explicitly substituted native,
Windows and GPU leaves. They establish particular decisions and invariants;
they cannot certify a displayed eye, finished reload, forged item or natural
feeling arm in the headset.

The latest animation instruction governs this patch: **only Henry's part of
taking, turning, emptying and returning an object should become faster. Actual
pouring/work, particles and sound must follow the gesture. Book changes, station
transitions, camera moves and distillation must not receive a blanket 8x rate.**
The implementation and remaining unsupported variants are separated below.

## What was actually reviewed

The uploaded `source(4).zip` and all three logs identify D85R5. The README still
described D84, so reading that README alone would have selected the wrong
baseline. The three current logs contain 1,912,340 lines in total. They were
scanned for the relevant subsystems and the selected original records retain
their log name, line number and timestamp in the evidence package.

Input SHA-256:

- Source: `fbae3bf0c5788528d33316cb4ea775a7eb105db22fa405cdb7eeb8dbc32fff3c`
- Logs: `e0042067389687466c3eef821a03b874e98562153b661da8da27f00471ce9eb6`

Historical review included the saved D44, D45, D66 and D67R1 reports; actual D67
and D67R1 patch/replay material; local D83/D84/D84R1 source; and the earlier
performance comparison. The D66 report contains a 20-log analysis, and the
retained D84 material documents a prior 50-distinct-log audit. **Those historical
reports were consulted; this request did not reopen and freshly reanalyse 50 raw
recordings.** That distinction prevents an old corpus result being presented as
a new test of this source.

The native evidence uses the previously supplied `WHGame(8).dll`, SHA-256
`bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7`.
Reading and replaying selected read-only functions from that binary did not
build or modify the game DLL.

Library files and saved handoffs were available for this review. This does not
mean access to a separate agent's private memory or every unexported chat.

## 1. Alchemy: repair the owner and the phase

### What went wrong

The general presentation multiplier included book/resource/work paths that the
user did not want accelerated. The current log shows a book child requesting
8x and actually running around 6.5–7.25x while native camera FOV changes. That
is consistent with the reported abrupt station/book movement; faster is the
wrong policy for that transition.

The carafe has a different failure. After Attach, the C46 owner takes over from
the generic presentation controller. The current trace spends roughly 1.62
seconds earning physical pouring credit before Henry's turn reaches Use. A
change to the generic rate did not change that active owner.

There is also a work/presentation split: the native Use event can happen before
the later procedural pour particles. C46 marked the action complete and stopped
its held clock at Use. Continued tilting could then leave the particle phase
unreached. This is distinct from whether the actual emitter is attached to the
right vessel mouth.

### Source changes

The generic presentation dispatcher now admits only the exact qualified
pickup/return/ingredient-transfer parents. Book and general Work classes no
longer acquire that speed lease. The saved 8x value is a ceiling, with a bounded
native update, rather than a speed for the whole alchemy station.

For the active C46 carafe, fast turning requires a current positive pouring
gesture, the same admitted action/instance/token and live animation clock, and
proof that only Henry alignment is active. The retail procedural updater and
two Henry alignment classes are verified against the supplied native binary.
An active particle clip or unknown procedural class receives no new fast rate.

Acceleration ends before the earliest real post-Attach event of any kind or the
actual pending procedural transition, whichever is earlier. It leaves an
80 ms native-time guard before that boundary and caps each accelerated update
at 80 ms. The calculation includes the controller scale. The eight inspected
vial asset variants explain the different particle schedules, but no fixed
asset time is used as permission for an unsupported current fragment.

After the original Use, the same held carafe can continue earning ordinary
pouring time from current tilt. Use remains completed once. Stopping, losing
tracking/ownership or a newer revoking input withdraws that permission. Current
upright-home intent or explicit release permits return. The real native Detach
hands the clock back through the existing identity/ownership checks. No Use,
Detach, particle callback, timeline seek or rewind is synthesized.

An independent review caught and closed three additional cases: direct Attach
had to populate the same live clock proof as deferred Attach; optional fast
proof could not be required for ordinary post-Use work; and the admitted header
and FIFO durations had to retain their original one-millisecond relationship
instead of being wrongly required to match exactly afterward.

The review also caught the old fallback's Home/Finish path selecting and
latching 8x after Use. That fallback now stays **1x for every post-Use Home,
Finish and Detach continuation**. It does not assume that Use or Detach proves
all effects are finished. The qualified C46 path retains the narrower fast
Henry interval and ordinary gesture-controlled tail.

The final owner/clock replay passes 170 checks, the actual input adapter 12,
and the general presentation/fallback fixture 38. The native audit verifies
three byte spans, the updater virtual slot, two exact procedural class identities
and eight authored vial profiles. The original slow turn and completed-held
stop fail the corresponding negative controls.

These checks establish the intended timeline/owner decisions. They do not prove
that an already-running particle emitter stops when the animation clock pauses,
that its origin follows the vessel mouth, or that the independent kettle clock
follows the same hand. Those native visual joins remain explicitly unresolved.

The existing successful mortar route is preserved. PlaceBowl is removed from
the semantic-only shortcut that skipped the authored child: the current log at
00:46:47.611 records that child as omitted. Retaining the native child preserves
its ordinary Start/Use/Detach/FX route instead of fabricating an effect.

This is a limited repair, not a claim that every vessel is continuously driven
by the hand. The current bowl controller admits one tilt/release intent and
then uses an authored returning passenger; PlaceBowl is not a C46 working owner.
MovePot also remains native. The exact mortar placement path has its existing
earned gesture clock. **Bowl/pot continuous hand-clock ownership remains
unimplemented here.** Restoring their native effects does not prove that join.

The captured bowl attachment readback is zero after about 140 ms against
a 125 ms expectation. That one observation does not prove that extending every
timeout is correct. No speculative global delay or attachment identity bypass
was added. Audible pickup feedback, every liquid variant and correct emitter
attachment remain game results to establish.

Files: `presentation_speed_d81.*`, `carafe_pour_d85.h`, `instant_native_d72.*`,
`resource_turn_d86.h`, `alchemy_resource46.inl`, `alchemy_semi_native46.inl` and
the localized include in `action_time_owner59c11.cpp`.

## 2. Elbows: why settings did not repair the implementation

The historical evidence does not identify a universally correct old elbow
implementation to restore. It identifies specific improvements that did not
reach every path.

D67 made the ordinary torso destination continuous. The bow still used an
older discrete candidate search and blended independently normalized poles.
Switching into bow ownership could discard valid bend history and remove the
ordinary hand-dependent chest bias, changing the shoulder foundation even when
the controllers barely moved. D85 also normalized a projected outward cue near
zero; close to that singular direction, tiny input differences could become a
large pole reversal. No slider can reliably compensate for all those branches.

The current main recording has 30,494 elbow-input rows. At 00:08:04.417, line
566735, a fresh valid-history FRIK step rotates the actual pole **43.867 degrees
in 11.047 ms**. A larger-looking example separated by 26.7 seconds was excluded.
The bow's own captured steps are smaller; the fresh FRIK jump alone is not an
explanation for every unnatural bow posture.

### The new destination

Ordinary tracked arms and bow arms now share `elbow_destination_d86.h`. For the
already admitted shoulder and wrist, it searches the exact elbow circle that
preserves both native arm lengths. It cannot move the wrist to hide an elbow
problem, and it cannot stretch the arm.

Its soft anatomical cues are the relaxed reference arm, current chest-relative
hand geometry, a calibrated forearm direction carried by the current hand
orientation, and the actual drawing-hand relationship to the bow. The hand cue
is derived from the validated reference skeleton, not a guessed controller axis.
Pure rotation around the forearm direction leaves that cue unchanged. Invalid
reference geometry supplies no invented cue.

Projected cues fade when they become parallel to the reach direction; they are
not normalized back into a strong, unstable pole. Head and torso proxies add
clearance costs. The previous bend starts a bounded local search on the same
continuous branch instead of becoming its own self-following destination.

Bow admission retains its chest basis and valid bend history. Actual policy,
pose-epoch and freshness changes still retire that history. The optional FRIK
response retains its small-motion behavior but caps the large angular step.
That cap is one correction; the common destination and owner transition repairs
are necessary even when the optional FRIK setting is off.

### What the tests establish

The anatomy fixture passes 2,342 checks covering both arms, face-to-overhead
motion, pronation, mirror behavior, singular crossing, near-folded/straight
reachable geometry, repeated samples and 60/90/120 Hz trajectories. The largest
tested wrist error is 2.78e-17 m and segment-length error 2.22e-16 m. These are
arithmetic invariants, not measured headset accuracy. The sampled trajectory
differs by at most 0.193 degrees across those frame rates.

The recorded fresh FRIK case becomes **2.532 degrees over the same 11.047 ms**.
The old production implementation fails that same assertion. Actual extracted
production call sites verify that both ordinary and bow arms use the new helper,
the native passenger fallback remains, and the shoulder/history policies connect.

Three tracked devices cannot uniquely measure the user's real elbows. The new
calibrated hand/chest relationship is an anatomical inference. Proxy clearance
does not prove clearance against every helmet, armor or mesh, and impossible
poses can retain filtered intrusion. F7 now includes a bounded `ANATOMY_D86`
row with the cues and destination/filtered intrusion needed to identify such a
remaining case precisely.

### Cost and smoothness

A small host benchmark measured about 1.27–1.32 microseconds per warm arm solve,
versus 0.77–0.80 for the older continuous torso-only helper. Cold solves cost
3.25–3.43 microseconds. The work is bounded: at most 41 warm or 52 cold objective
evaluations, plus bounded midpoint checks. There is no new scene query or heap
allocation in this geometry helper. These are Linux host timings, not Windows
or headset FPS.

The detailed recorded CPU tracking joins agree at the examined source/staged
points. They do not observe final palette submission or headset scanout. The
patch therefore preserves accepted Fresh Pose publication and saved hand
smoothing instead of adding another controller filter to mask an unproved
presentation defect. H50 and D52 remain the user's same-scene comparison; the
reported performance regression predates Fresh Pose. No return to C71 FPS is
claimed.

## 3. Dice and two-handed weapons

### Dice reference lifetime

The source could classify dice as an ordinary tracked frame, learn a neutral
eye/body reference from it, and retain its native-height smoothing state after
leaving. Activity identity was absent from the cache lifetime.

The shared tracked frame now distinguishes ordinary, dice, mounted, workstation
and other reference domains. Activity entry, exit or a new instance retires the
relevant presentation/reference state. Dice remains a valid VRIK frame, but
cannot train ordinary neutral anatomy or donate an ordinary pickup reference.
Saved calibration and physical HMD movement stay intact.

The actual header passes 31 lifecycle checks; removing just its new domain
comparison makes four checks fail, including the retained-height exit case.
Correction after the user's clarification: the user changed presentation zoom
to **0.95 after leaving dice**, then saw a static image of the old dice game and
had to restart. That control is separate from native Game FOV at **95 degrees**.
The latter readback cannot establish or rebut the former setting. This recording
has no timestamped receipt for the 0.95 edit. It does show a terminal DLSS reset
fault followed by zero fresh pairs and 100% old-pair reuse; that is a concrete
freeze path, not proof that changing zoom caused it. The activity-reference
repair above addresses a different source defect and does not by itself repair
or explain a frozen rendered image. No arbitrary HMD recenter or global FOV
reset was added. Physical cup take/shake/place is still unfinished.

### Supported polearms

The log confirms that the second-hand support grip really engaged. Existing
class-7 logic then multiplied support authority by 0.8/1.7 and expanded lag by
1.6, on top of existing length/inertia, skill and fatigue effects. That partly
cancelled the second hand's contribution and exaggerated the long-pole feel.

The patch removes that extra penalty only while a class-7 weapon is supported.
Its ordinary supported lag envelope becomes 6 degrees instead of 9.6 in the
neutral host profile. Supported recoil uses critical damping for the existing
900 spring: damping 60 instead of 27. One-hand response retains its existing
27 damping. Mass/inertia, skill, stamina, support reach, primary grip, collision
permissions and combat results retain their existing owners.

The real integration kernel passes 134 checks; the baseline fails seven relevant
supported-response assertions. These establish bounded response and grip
invariants, not the user's final perception of weight.

## 4. DLSS/FSR: histories existed; final output still failed

The current DLSS log creates two distinct eye handles. Repeating the original
"add separate histories" explanation would miss the present failure.

At 23:49:20.609 and again 23:50:07.811, the watchdog has made 86 attempts with
zero certified temporal pairs and `compose-qualification` failure. D85 responds
by falling back to the first RIGHT main writer. A successful SDK evaluation at
that point is not proof that the final processed eye image is complete.

At 23:50:36.819, `right-reset-differs-from-left` permanently latches the DLSS
backend fault. Later metrics show zero fresh pairs and 100% reuse. That is a
specific terminal publication failure, separate from the reported short startup
flicker.

### Completed output and lifetime

The temporal route now preserves the completed RIGHT main output immediately
before the observed engine-main render-target-to-present transition and before
the desktop HUD mirror can alter it. It requires the completed native RIGHT
pass and matching per-eye SDK receipts for the same attempt.

Recording that copy does not publish it. Publication waits for the exact command
list recording to pass through the actual game queue. Frame, epoch, source,
capture resources and per-eye rendered-pose tokens remain bound to that attempt.
Resources remain referenced until a real fence completes. A failed Reset is not
treated as evidence that pending commands disappeared; cancellation cannot
release still-recorded or submitted resources early.

LEFT is preserved with its immutable end-of-LEFT source/state snapshot. The
global CPU state tracker may already describe later RIGHT commands, which is
the wrong state for an earlier queued LEFT copy. Outstanding LEFT writers and
ambiguous mixed LEFT/RIGHT batches withhold that pair. Native batches are not
split, replayed or reordered. The timer is diagnostic only and can no longer
bypass output-completion requirements.

This implements the D3D12 distinction between recorded work, queue execution
and fenced resource lifetime. It preserves the native eye projection/IPD,
existing native view identity, OpenXR composition and accepted Fresh Pose feed.
Native0 retains its established capture decision. The FSR runtime itself is
unchanged; the shared final-output repair also applies to its presentation path.

### Reset recovery and truthful live switching

A RIGHT-only reset invalidates that pair and resets both histories for the next
pair. It no longer permanently disables the backend. Actual ABI, SDK evaluation
and restoration failures retain their existing hard-failure handling.

Live switching distinguishes saved settings, ready contexts and a new stereo
publication. Readiness permits rendering to start; only a publication from the
requested technology in the new current epoch confirms stereo output. If none
appears within eight seconds, the UI says `SETTING APPLIED: STEREO OUTPUT
UNCONFIRMED`. The Apply label says `SAVED / REQUESTED` while pending.

AFR remains inactive; no fake live AFR switch was added. Current XeSS support is
retained as a candidate. The actual startup help now agrees with the available
choices instead of simultaneously offering XeSS and saying it is unsupported.

### Blur is not proved solved

The short recording reads the live NVIDIA quality CVar as 2 at 23:56:58.973 and
23:57:01.021. The current launcher labels value 2 **Balanced**, value 1 Quality,
and value 5 DLAA. That excerpt does not establish a Quality-mode run. It also
does not establish the active SDK input subrectangle or the Oculus setting.

F2 now records the missing per-eye input allocation/render dimensions where the
public/native contract exposes them, jitter, motion-vector scale, resets and
publication identity. DLSS allocation dimensions are explicitly not presented
as the active render subrectangle. No invented projection or motion-vector
correction was applied. Output ownership, reset recovery and UI confirmation
are repaired source paths; driver output, residual softness, stereo pixels,
VRAM cost and frame rate remain unmeasured.

## 5. Pickups: a bootstrap-order defect and four real refusals

Four current F5 inventory-item requests fall back because the observed object is a static
`CPhysicalEntity`, not the verified rigid-body type. Their times are
00:19:19.684, 00:20:30.183, 00:20:33.026 and 00:20:35.872. Native Use is accepted
after the fallback. The subsequent native Attach starts 386, 400, 420 and 388 ms
after the respective Use pulse; the handover to a held item finishes later.
Those are delayed inventory-pickup transactions, not proof of instant or
flicker-free presentation. They do not identify the model as the user's milk
solely from its WUID. The user's separate deliberate attempts to grab scenery
tankards/small geometry must not be conflated with these PickableItem records.
Ordinary pickups also remain reported unreliable by the user.

There is a real ordering defect: Lua preparation can occur before the native
pickup admission reader is installed. Installation used to be triggered later
from OnUsed. A first eligible static item therefore had no reader available
when the preparation step needed it. A failed reader registration could also
be marked complete before it had actually succeeded.

The current native-frame path now installs the qualified pickup bridge before
Lua Use. Registration commits only on success, and a busy failure gets at most
four spaced retries. Shutdown and failed installation remain blocked; the
stopping flag is rearmed only at the successful installation tail.

The rigid-body contract is not broadened. Static and rigid vtables do not share
the required velocity ABI. Stealing, living/dead actor, quest/item identity,
mass, empty-hand, reach and current-owner conditions remain intact.

F5 now receives fixed-code Lua preparation reasons and the precise native
admission phase. A refusal can identify unavailable helper, identity change,
CanUse/steal/dead-body exclusion, unsupported mass/slot/physics, native admission
or failed positive-mass readback. The original OnUsed still runs exactly once.
The new notes are bounded and use the existing log; the Lua path does not add
System.LogAlways or another file.

The companion's old BASE_GAMEPLAY PAK is intentional donor content. The actual
release helper overlays current Lua source. Its final preparation was exercised: all 299 current-source overlays and all
391 member hashes verified, including the final lab, PickableItem and corpse
scripts; the donor's age
alone was not treated as evidence that the user installed the wrong package.
The generated test PAK was removed and is not shipped in this source patch.

This fixes a preparation-order failure. It does not certify every small object,
both-hand hold/throw behavior, chest storage haptic or every native fallback's
camera presentation. Accepted collision and unsheathing improvements are retained.

## 6. Crossbow reload: mechanism observation had the wrong dependency

The rendered weapon's string/tool observer used the same current-publication
dependency as a hand/prop writer. A legal sequence is: current passenger/native
clock, new Henry publication, then weapon skinning. The observer could reject
that sequence even though its own current native clock and weapon pose were
valid. That removes the geometry the hand-controlled string/lever path needs.

A dedicated read-only mechanism witness now validates the actual current
passenger lease, identity, native clock and original 80 ms freshness bound. It
does not require the later Henry writer publication merely to read string/tool
geometry. The stricter hand/prop write gate is unchanged. No native event,
loaded flag, socket or ammunition result is fabricated.

The production socket/observer replay passes 116 checks in debug, optimized and
ASan/UBSan configurations; reinstating the old dependency fails the same case.

**Other captured `UNAPPROVED_INTERVAL` and initial `NATIVE_CLOCK_NOT_QUALIFIED`
failures remain.** The patch therefore cannot honestly be called completed
light/medium/heavy crossbow reload. F5/F7 and the retained F11 recorder now
distinguish missing native pose, clock/owner refusal, pose/socket identity,
string/tool geometry and final ownership. This is one shared bounded observation,
not additional native polling for each key.

Visible bolt retrieval in the hand, spanner attachment and lever travel, native
cock/load state, inventory debit and the completed live reload remain the
integration outcomes. The current bow aim and unholster path are preserved.

## 7. Smithing, kicks, combos and knives

### Smithing

The current recording contains 40 native StrokeHit callbacks after its last
forge-to-anvil transition. That rules out describing this recording as an anvil
path that universally stops receiving all strokes after reheat. It does not
measure the fraction of the user's attempted strokes that were lost, accepted
quality, or every LT attempt. The current re-entry, same-release acknowledgement
and LT native-placement improvements are preserved. No new sensitivity threshold
or guessed post-reheat timer was introduced without a reproduced defect.

### Kicks

For the exact standard retail combat object observed in the capture, the native
query builder selects FreeAttack with no opponent even when guard is 0, 1 or 2.
D85 guessed CombatAttack from those guards plus a non-null combat object. That
released the modifier the free request needed and could select a punch/weapon
attack instead of the intended kick.

The patch recognizes the exact pinned native query/delegate/object chain. A
qualified no-opponent object keeps the free route; a present opponent and unknown
objects retain their existing policies. It does not call a guessed vtable,
spoof a target, write equipment class or synthesize damage. Existing native
FreeGuard acknowledgement and protected-action rules remain.

At 00:30:52.757 and 00:30:55.495, captured requests return but immediately observe
attack type 0. The old code still imposed a two-second kick cooldown. Cooldown
now requires a returned release and observed type 4. Those two type-0 cases had
an opponent, so the no-opponent routing fix does not explain them by itself.

The four copied original native read-only query functions pass 26 host checks;
the exact production route/cooldown passes 27. This establishes the particular
selection and cooldown corrections, not kicks in every armed/unarmed context
or confirmed contact.

### Combos and blade-edge reactions

The native combo extension actually rearms: current records show
`REARMED_ONCE nativeSeconds=0.500 wantedSeconds=1.500`, followed in one case by
expiry after roughly 1.58 seconds. The unchanged native base duration is not
proof that the configured extension failed. A full recipe/finisher remains
unproved, and arbitrary further window widening would not establish it.

The F3 combo notes now receive a fresh finite budget per recording session;
ordinary play cannot silently consume the useful first rows before F3 starts.
No combo recipe, hit, eligibility, masterstrike target or native paired damage
authority is changed. Making every native paired reaction/damage event follow
the actual VR blade edge remains unfinished and is not claimed by this patch.

### Throwing knives

The uploaded source has an active **left-hip, left-hand** acquisition service,
despite the historical `VrLeftChestKnives` name. The current setting is ON. The
log has 26 preflight/readiness observations with an accepted current-owner
refresh, service ready, and **zero recognized throwing stock**. No acquired or
launched knife is recorded.

The supported stock includes the ordinary common dagger and finite existing
legacy/recovered throwing stock. Some named/reward/unique dagger types are not
safe substitutes in the current conversion pipeline, which would recover a
generic throwing item instead of preserving the original identity. This patch
does not silently expand that conversion or grant free inventory.

F3/F5 now distinguish unknown refresh, refused refresh, no supported stock and
available stock, with a fresh bounded session budget. Misleading retired/inert
comments are corrected. This is an evidence/clarity repair, **not a successful
throwing-knife gameplay result**.

## 8. Corpse dragging: enable-time cost; acquisition was not tested

Correction: the user did **not** attempt dragging. Enabling the option caused a
large performance penalty, so the test was abandoned. The search receipts below
are background feature activity and cannot be described as failed grab attempts.
The R1 change added observation, not a repair to this enable-time cost. The
corrective R2 review audits and bounds that background work separately.

The main log has 645 hand receipts: 270 `NO_NEAR_DEAD_BONE`, 194
`GAME_CONTEXT_OWNS_HANDS`, 179 `HAND_OCCUPIED`, and two
`NATIVE_BODY_OWNS_HANDS`. All 656 provider receipts show selection/installation
but zero candidate or held body. Most callback ages are 0–16 ms; one reaches
594 ms. The evidence establishes no acquisition, so it cannot isolate a failure
of the subsequent velocity/drag servo.

The exact supplied native DLL proves that `Entity:GetBonePos(boneName)` takes
the string as its first argument, selects character slot zero internally and
returns the joint after its world transform. Adding a slot argument or applying
another world conversion would introduce a bug. The sphere query is based on
an entity reference position, not proven closest-ragdoll geometry; the current
receipt does not establish which nomination stage failed.

The new F9-only `F9_CORPSE_QUERY` receipt distinguishes sphere contents, required
APIs, current identity, death qualification, usable numeric IDs, examined dead
bodies, finite joint results and nearest finite joint distance. It reuses the
existing query, at most once per hand per two seconds, and adds no physics write
or second search. Existing candidate selection and acquisition limits remain.
Malformed diagnostic suffixes cannot change an otherwise valid gameplay reply.

The actual Lua passes 228 checks; the production reply parser and extracted F9
gate pass 73. Ordinary and traced queries have the same candidates and the same
native query/physics call counts under controlled native leaves. These prove
that the added observation is bounded and transparent; they do not prove that
a dead body can now be pulled.

No new body servo, arbitrary rigid-body conversion, looser living/dead check or
forced limb movement is introduced. The existing GetBonePos missing-joint branch
can log inside the game itself; a list of absent bone aliases can therefore
produce native log work even when the mod's recorder is off. The added F9 receipt
performs no extra bone or sphere query. Eliminating native alias misses safely
requires validated skeleton/lifetime knowledge; this patch does not pretend
all external/native logging is controlled by KCD2VR's logger.

## 9. HUD, prompts and native physical walking

The immersive HUD now requires current open-hand input, signed palm-facing
orientation and head-directed gaze. The old absolute facing dot product accepted
the back of the hand. Invalid input actions can no longer look like a released
open hand. Known capacitive trigger contact, pressed grip/trigger/buttons or a
known held physical item refuse the open condition. Controllers without a
capacitive binding can use valid analog release; an unavailable touch binding
is not invented as a real touched state.

The original one-second deliberate-look dwell remains. A reveal lasts at most
**three seconds**. Closing, turning away, looking away or losing tracking hides
it immediately. Holding the same glance cannot restart the timer. Left remains
compass; right remains health/vitals. Gaze here means head direction, not an
eye-tracking sensor.

Station icons become **0.90 of native size**, exactly twice the tested 0.45.
That value is also selectable in the existing menu cycle. The independent timed
station-hint window remains ten seconds; it is not the palm HUD's three-second
limit.

The INI is saved on exit. Its final values cannot prove which movement switches
were active during any earlier attempt. The captured D76 counters show zero
submitted/applied walking in those samples; that observation alone does not
identify the enabling choice or the first rejected condition. The patch's
one-time native-walking/body-follow override was removed in corrective R2.
Movement preferences remain as saved, without replacing finite displacement
with continuous stick input or changing accepted Fresh Pose timing. The original
6DOF failure remains an unresolved runtime behavior; an option migration is not
a source repair or proof that the user had left a feature off.

The starter INI is the current uploaded profile, preserving calibration and all
other setting occurrences. In corrective R2, its only value changes are 0.45 to
0.90 prompt scale and the added migration marker. The production migration
checks its existing marker, applies only the requested prompt size even if the
file cannot be written, and respects later prompt choices after a successful
migration. It makes no movement-setting write. The normal release helper preserves an existing game
INI; applying this source patch is not installing settings into the game.

R1's original migration passed its then-current 23 checks. Corrective R2 adds
checks for every combination of the three movement flags, preserving their live
and saved values; 71 checks now pass. This verifies the prompt migration and
setting preservation, not the live state during prior recordings or
directional walking or every native dodge. Native collision/legality and
HMD-only lean-versus-step ambiguity remain.

## 10. Diagnostics, documentation and integration

All new runtime records use the existing KCD2VR log. The capture mapping is:

| Key | Relevant new or repaired evidence |
| --- | --- |
| F2 | Temporal output identity, reset/input metadata, publication and timing. |
| F3 | Actual kick route/type, fresh combo note budget, accepted/zero/unknown knife stock. |
| F5 | Pickup preparation/admission, alchemy owner and clock, minigames and crossbow mechanism. |
| F7 | Anatomical destination/filtered result and current crossbow pose witness. |
| F9 | Existing physics plus exact corpse nomination stages. |
| F11 | Retained dedicated reload capture, shared with F5/F7 without duplicate polling. |

New diagnostic formatting/queue writes are gated by the relevant finite capture
session. Captures can operate together. The existing five-second drain allowance
can deliver previously recorded rows after stopping; that is not a new producer
running forever. No fingers BIN/JSON, map-performance, companion-startup or
DetourModKit sidecar is introduced. A last-good INI checkpoint remains a recovery
file, not a diagnostic stream.

The README, current known issues, settings and launcher help now describe this
baseline. Historical D85 notes are visibly marked as superseded where they
recommended the timed temporal fallback or equated context readiness with live
success. Older D84/D83/D82 reports remain history. Internal file suffixes such
as D86 name components; they do not identify a different installed build.

### Main verification results

| Area | Passing scope |
| --- | --- |
| Elbow destination | 2,342 checks; recorded 43.867 to 2.532 degree fresh-step replay; actual production wiring. |
| Supported weapon | 134 checks; old source fails seven supported-response assertions. |
| Activity references | 31 checks; removing domain identity fails four lifecycle cases. |
| Temporal capture | 79 assertions plus 141 simulated GPU state/pixel-order checks; optimized and ASan/UBSan; 14 rejected mutations. |
| DLSS | 285 routing plus 23 view-access checks; ten rejected mutations; nine exact native ABI pins. |
| FSR | 911 checks in each of three configurations; eight rejected mutations. |
| Live switch | 91 checks; five rejected mutations. |
| Pickup | Actual bootstrap 17, actual native admission 76, Lua 112 and native getter 17 checks; old failures reproduced. |
| Crossbow | 116 checks in three configurations; four rejected causal mutations. |
| Kicks | 27 production checks plus 26 original native query checks; old route/cooldown rejected. |
| Combat diagnostics | 18 checks, including off silence and renewed finite session budgets. |
| HUD | 40 checks for open/facing/gaze/current-input/timer and requested prompt defaults. |
| Settings | 23 production migration checks and exact current-profile value preservation. |
| Alchemy / corpse | Alchemy: 170 owner/clock + 12 actual input + 38 scope/fallback checks. Corpse diagnostics: 228 Lua + 73 production parser/gate checks. |

Expected baseline/mutation failures are negative controls demonstrating that a
test distinguishes the repaired decision. Counts from repeated configurations
are not added together to manufacture a larger number of independent cases.
Where this host cannot enumerate tasks for LeakSanitizer, that enumeration was
disabled while address and undefined-behavior sanitizers remained active. This
is documented in the individual receipts. No full Windows translation-unit,
native hook-install, GPU pixel or headset acceptance is claimed.

### Applying the source patch

Extract the patch beside the original source, outside that source directory.
From the patch directory:

```text
python apply_patch.py --source "PATH_TO_EXTRACTED_D85R5_SOURCE" --check
python apply_patch.py --source "PATH_TO_EXTRACTED_D85R5_SOURCE" --apply
```

The default is read-only check. Every affected path must match its known before
or after hash; unrelated edits are refused before any write. The package includes
the complete changed/new files, the original affected contents, a unified diff
and a hash manifest. Revert is available with `--check-revert` and `--revert`.
It restores known original files and removes only known new patch files. This
helper never builds a DLL or operates on the game installation.

The resulting source uses the normal Windows build process later. A matching
companion must be generated from that source by the existing release helper;
copying only a later DLL with an older installed companion is not equivalent.
The package contains no generated DLL/ASI, raw full recording, compiled host
test executable or optional gore payload.

The release decision remains separate: complete light/medium/heavy hand reloads,
all kick contexts, blade-edge paired damage, physical dice cup, proven corpse
motion, every pickup/effect and final GPU/headset presentation are still open.
The source repairs and receipts above make those remaining outcomes specific
enough to assess without another indiscriminate settings sweep or a claim that
everything was fixed by compiling.

## Primary design references

Source/log findings above are documented in the accompanying per-domain evidence.
These primary references explain the external contracts; they do not prove the
mod's final in-game behavior:

- Final IK VRIK documentation: reference pose, hand axes, bend goals and shoulders.
  https://www.root-motion.com/finalikdox/html/page16.html
- Microsoft, Creating and recording command lists and bundles: recorded versus
  executed work, Reset and resource lifetime.
  https://learn.microsoft.com/en-us/windows/win32/direct3d12/recording-command-lists-and-bundles
- Microsoft, Resource barriers and resource-state synchronization.
  https://learn.microsoft.com/en-us/windows/win32/direct3d12/using-resource-barriers-to-synchronize-resource-states-in-direct3d-12
- AMD FidelityFX Super Resolution upscaler manual: render/input dimensions,
  camera jitter, motion information and reset at discontinuities.
  https://gpuopen.com/manuals/fidelityfx_sdk/techniques/super-resolution-upscaler/
