# KCD2VR D85R5R2 corrective review and source patch

3 November 2026. Baseline tested by the user: `KCD2VR-D85R5-KNIFE-POUCH-20261102`.
Source revision: `KCD2VR-D85R5R2-CORRECTIVE-PATCH-20261103`.

## What changed after the user's corrections

The six corrections change the review, not merely its wording. Corpse dragging
was not attempted: enabling it caused an unacceptable performance penalty.
Static tankards and small scenery were deliberate targets, and ordinary pickups
also remained unreliable. The 0.95 zoom change was an attempted remedy after
leaving dice; it was followed by a static old dice picture and a restart. No
previous elbow implementation is a known good baseline. The existing crossbow
recordings need to be followed through the whole reload. The INI is saved on
exit and cannot establish settings during an earlier attempt.

This revision includes the retained R1 source repairs, additional supported
repairs described below, and corrections to the R1 report. **No DLL or ASI was
built.** Host compilation and controlled native-call substitutes were used for
specific checks; no Windows game, physical controller, GPU image or headset run
was performed. Those limits apply to every readiness claim in this report.

The patch also removes R1's one-time movement-option override. It was not a
valid repair for the reported 6DOF failure. The starter profile preserves the
uploaded exit profile's movement choices and calibration. The only changed
setting value is the expressly requested station prompt scale, 0.45 to 0.90,
plus the retained migration marker. This is default preservation, not an
inference that those were the options active during the test.

| Area | Correct interpretation | Source result | Gameplay status |
| --- | --- | --- | --- |
| Corpse interaction | Enabling caused a cost spike; no drag trial occurred. | Earlier idle budgeting, body-bound rejection and bounded sharing of discovery. | Lower operation counts in fixtures; real FPS and dragging remain unverified. |
| Ordinary pickups | Delayed/flickering pickup remains a real failure despite some inventory completions. | Retained setup repair, requesting-hand selection and preparation in both existing hold modes. | Complete instant, stable both-hand pickup is not established. |
| Static scenery | Tankards/small geometry were intentional targets. | Separate acquisition gap identified; no guessed geometry conversion. | Scenery grasp remains unfinished. |
| Dice | A frozen old picture followed a post-exit 0.95 presentation-zoom change. | Retained publication recovery and activity-reference repairs; actual zoom/publication receipt added. | Exact edit timing and final displayed recovery are not measured. |
| Elbows | Every old version remained unsatisfactory. | Historical failure ledger, current arm-step measurement and replayable basis/rotation receipt. | R1's proposed destination is not proven natural anatomy. |
| Crossbow | Existing recordings show several distinct failures through a reload. | Inert duplicate-clip, UpperBody route and same-clock pose-continuity repairs. | Complete light/medium/heavy hand reload is not certified. |
| Settings evidence | Exit-saved values are not a runtime timeline. | Movement override removed; live evidence used for conclusions. | No settings-blame explanation for unresolved 6DOF. |

## A. Corpse interaction: the cost exists before a grab

### What the code and recordings establish

Two discovery paths can run merely because corpse interaction is enabled. The
native articulated-body path searches its known, game-killed body candidates;
the saved-body fallback enters the native script service and calls
`System.GetEntitiesInSphere`. That native sphere query traverses entities. The
saved-body query can then try 31 bone-name aliases on as many as 32 dead bodies
per hand. At ten queries per second per hand, that is a possible 19,840 bone
lookups per second before considering the other work. This is a source-derived
upper bound, not a measurement that every recorded second hit that bound.

Missing aliases can also enter the game's own missing-joint logging. Turning
F9 off does not make those gameplay queries or their native logging free. The
older 100 ms search guard was too late to avoid per-callback VM/owner/hand
validation. The native path also performed hand/pose checks before reaching
its search cadence guard.

The recorded `corpseDrag=1`, provider-installed and no-candidate rows establish
enabled background activity. The main recording's 645 hand-status receipts and
656 provider receipts do not establish deliberate attempts to grab a body.
There is no measured before/after enable FPS edge in the selected diagnostic
receipts, and the user's account remains the evidence for the visible cost
spike. It is wrong to infer failed dragging technique from these rows.
The main log first samples this option on at 00:18:56.996 and still on at
00:40:09.079; those rows do not supply its enable edge. An off readback in
KCD2VR1.log belongs to another run and cannot be joined into a same-scene
before/after comparison.

### What the corrective source does

The saved script provider now answers whether idle discovery is due before
performing its expensive VM/current-hand work. With no candidate or held owner,
that service is due at the existing 100 ms interval. Active ownership and
release cleanup retain their required cadence. The native no-candidate path
likewise budgets before repeated hand/pose/native reads.

Both discovery paths use a valid current body bounding box to reject a hand
well outside the body before enumerating parts or trying bone aliases. The
margin is 0.65 m: the existing 0.45 m nomination radius plus a 0.20 m tolerance.
This is only an early rejection. Missing, invalid, collapsed or otherwise
unusable bounds fall back to the previous exact search. Bounds do not grant
grip, death status, body identity or movement authority.

The Lua route can also share one copied discovery list between the two hands.
The first query covers 2.4 m; a second hand within 0.8 m and no more than 30 ms
later is geometrically covered. Each hand re-filters the list to its original
1.6 m sphere before spending the 32-body budget. Native evidence verifies that
the position accessor used for this filter reads the same entity translation
as the original sphere callback. A missing clock, bad current position, dense
expansion or ineligible second request uses the original independent query.

Only a discovery list is shared. Current entity identity, death, bone position,
part, grip and motor admission are still checked at use. No cached death answer
or old physics permission becomes a live grab. The cache is short-lived and
one-use across hands; repeated same-hand queries cannot keep it alive.

### What this proves, and what remains

In the controlled case with 32 out-of-reach corpses, the old query makes 992
bone calls per hand. The revised query makes zero bone calls and 32 bounds
calls per hand. For two eligible hands, shared discovery uses one entity
traversal instead of two. F9-on and F9-off variants preserve the same gameplay
query/physics calls. Near-body admission, drive/release and invalid-bounds
fallback are checked separately so the optimization is not just disabling the
feature.

This is a **partial performance repair**. Eligible idle hands can still cause
roughly ten full traversals per second when sharing succeeds, or twenty when
independent queries are needed. The native known-body path has additional
bounded work. There is no measured in-game frame-time result here and no proof
that the user's enable-time penalty is gone.

The old F2 frame scopes did not encompass the saved-body script-service query.
R2 therefore adds elapsed native-query time to the existing bounded F9 query
receipt, alongside sphere-call/reuse and bounds counters. With F9 off, this
additional timing performs no clock reads or formatting. It measures the
remaining work when requested without creating another log or query.

Removing all idle discovery requires a correct input-intent bridge. The current
grip owner expects a candidate before a squeeze. Merely moving the query behind
closed grip would miss fast presses; reserving every empty-hand squeeze would
interfere with pickups, doors or holstering. That remaining interface work is
identified explicitly. This patch does not conceal it behind a “dragging fixed”
label or add an unverified ragdoll motor.

Evidence: `evidence/corpse_static/`, including actual Lua/idle/bounds fixtures,
native binding evidence, runtime-state excerpts and the lane's corrective report.

## B. Static scenery and ordinary pickups are different failures

### Static scenery

The intended feature includes grabbing a tankard or other small scenery,
turning it, placing it and making it react to the hands. A native inventory
item with a static physics body is not the same object category as a scenery
`BasicEntity` or an unselectable piece of level geometry.

The existing loose-prop producer still depends on native selection. An object
which never enters that selector never becomes a grab candidate. The companion
wake route refuses zero-mass static BasicEntity props; contact preparation has
only a narrow existing PickableItem subset. A previously rigid BasicEntity can
react, but this does not implement static-scenery acquisition. The historical
small-object handoff already described that gap as unfinished.

No arbitrary static vtable was accepted as a rigid body. The static and rigid
velocity contracts differ. The source patch does not guess a physicalization
call against arbitrary scene geometry, clone a decorative mesh, or fabricate
inventory ownership. **Static scenery grasp remains open.** The implementation
still needs an actual near-hand scenery selector, verified ownership of a
movable object/slot, qualified rigid creation where permitted, and release
handling that preserves the original entity's lifetime and scene state.

### Ordinary inventory pickup: the recorded delays are real

Four current F5 inventory requests first fail immediate rigid-body admission,
then use native animated pickup. Their exact request-to-attachment timings are:

| Use pulse | Requesting hand | Native Attach starts | Held observed | Native Picking ends |
| --- | --- | --- | --- | --- |
| 00:19:19.586 | Right | 386 ms later | 411 ms later | 430 ms later |
| 00:20:30.090 | Right | 400 ms later | 421 ms later | 445 ms later |
| 00:20:32.905 | Right | 420 ms later | 435 ms later | 458 ms later |
| 00:20:35.783 | Right | 388 ms later | 409 ms later | 421 ms later |

These contain actual job/controller adoption, socket wait, grip capture and
FinishForHold stages. Their later inventory commits do not turn them into
instant or flicker-free pickups. They also cannot identify the particular
milk/tankard model solely from a target ID or world item key.

A fifth left-hand request at 00:20:40.147 has an accepted input queue receipt
but no matching native pickup/Attach/held receipt in this capture. Selection
rows around it are not a final native-use result. The four right-hand successes
cannot establish both-hand reliability. The missing left-hand join remains
distinctly recorded, not claimed repaired by the new hand-choice test.

The four successful stows verify inventory and hand release and report
`hapticQueued=1`. That proves a queued event, not that the controller physically
vibrated. The user's missing-feedback report is not contradicted by it.

### Retained and additional source repairs

R1 repaired a setup-order failure: static-item preparation could run before its
native admission reader was installed. The bridge is now installed before Lua
Use, registration commits only on success, retry is bounded, and shutdown is
rearmed only after successful installation. The new F5 reasons distinguish
preparation failure from native pickup fallback. Those changes are retained.

R2 additionally fixes two source defects in `staged_pickup596c2.inl`:

1. The instant candidate selected the nearest eligible closed hand, even when a
   fresh transported VR Use identified the other hand. Preparation and candidate
   construction now honor that same-target requesting-hand receipt. If that
   hand does not qualify, the other hand cannot take over this qualified
   request. Older acquisitions without that receipt retain their previous
   policy. This does not add simultaneous two-item holding; the existing held
   world-item owner is still singular.
2. Static inventory-item preparation unconditionally required velocity-drive
   mode and a physics-requested wrist, while the existing kinematic hold also
   needed a rigid body. Preparation now follows the hold mode's actual wrist
   source. It uses the requested wrist for physics drive and the allowed wrist
   for kinematic hold, and rejects a mid-admission mode change. An old physics
   drive fault does not veto kinematic admission. The separate legacy F9
   physical experiment keeps its existing physics requirement.

The actual admission function passes 89 host checks; its actual hand-selection
block passes 12. The unchanged R1 code fails the intended kinematic-admission
and nearer-opposite-hand cases. The tests substitute the existing requesting
hand reader/native/pose leaves; they are caller-routing and admission checks,
not complete native gameplay integration. Both directions of a mid-admission
mode flip are rejected by the new final recheck.

Remaining work includes the left-hand final-use/admission join, final rendered
camera/hand/object continuity on fallback, small-object fit, and consistent
release/throw feedback. The source fixes are not a declaration that the user's
ordinary pickup problem is solved.

Evidence: `evidence/ordinary_pickup/PICKUP_TIMELINES.json`, original numbered
pickup rows, `left_request_window.txt`, admission results and the detailed
`ORDINARY_PICKUP_AND_SETTINGS.txt`.

## C. Dice: 0.95 framing is not 95-degree FOV

The actual “VR zoom / framing” control is `presentation_zoom`, saved as
`VrPresentationZoom`. Its range is 0.85–1.20 in 0.01 steps. The Game FOV control
is a separate value measured in degrees. Reading back 95 degrees does not tell
us whether 0.95 framing was applied, or whether the compositor had a new image
to display.

The inspected path runs from the slider through `Mod::ApplyMenuState` and
`XrManager::SetPresentationSettings` into the compositor multiplier. Projective
remap mode can force its effective scalar to 1.0. The setter stores values; it
does not itself recover a faulted stereo-publication pipeline. A zoom change can
therefore reframe an already cached image. This is source reasoning, not proof
of the exact causal order in the user's attempt.

The current KCD2VR1.log contains a concrete publication stall:

- 23:50:35.881: last sampled LIVE-PUBLISH row. The row is sampled; it is not
  asserted to be the very last actual publication.
- 23:50:36.819: DLSS `right-reset-differs-from-left`, fault code 14.
- 23:53:24.449: native frames, Present and XR still run around 90/s, while fresh
  pairs are zero and reuse is 100%.

That failure can leave the old dice surface visible while the native game moves
on. The old code latched the mismatch into a permanent fault. R1's recovery
path discards a mismatched pair and requires a later qualified pair, and its
completed-RIGHT capture/publication repair addresses a separate upstream
failure. Both are retained.

The recording has no timestamped MENU-APPLY receipt for the user's 0.95 edit.
Its one sampled `userZoom=1.000` value cannot reconstruct a later edit. Thus the
review cannot claim whether the adjustment caused, exposed or coincided with
the freeze. It also cannot use an incomplete native dice trace to contradict
the user's report that the minigame had already ended.

R1's activity-reference lifetime repair addresses another real source defect:
dice could donate neutral/body-height history to ordinary play. That fix is
retained but is not relabelled as a complete explanation of a frozen picture.
No additional speculative FOV reset, HMD recenter or calibration write was made.

R2 adds a changed-zoom/mode apply receipt only under the finite F2 or F5 capture.
It records requested/stored/actual XR zoom, dialogue multiplier, source FOV,
publication identity, produced/consumed pairs and current DLSS fault. The
snapshot is explicitly non-atomic and queue evidence is not claimed to be
headset scanout. With both captures off, the status reader and formatter do not
run. The observation adds no GPU copy, restart or rendering authority.

Evidence: `evidence/dice_elbow/evidence_rows.json`, the lane's detailed review
and actual menu/getter tests. A mutation substituting 95-degree FOV for the
zoom value fails the expected checks.

## D. Elbows: no old implementation is declared good

The history contains several different defects, not one missing slider value:

- D37/D38 shoulder-relative reach handling could move the wrist when the
  shoulder changed. The later warning-only shoulder-fit behavior must be
  preserved; an elbow preference cannot pull the controller endpoint away.
- D44 recorded an upstream admitted-wrist jump at acquisition. Smoothing an
  elbow cannot repair an already displaced wrist target.
- D45/D66 exposed torso-clearance branch changes. D66's review describes a
  60.90-degree destination switch, a 4.74-degree bend step and 26.72 mm of elbow
  movement in the observed case. Repeating an old narrow conditioner fix would
  not address the later implementation.
- D67/D67R1 improved ordinary-arm continuity but excluded bow/reload ownership
  from that route. A smoother ordinary replay did not establish a natural bow
  draw, and bow admission could change the shoulder basis/history.

The new current-log quaternion audit provides another concrete arm step. At
00:10:02.571–.583, the admitted left wrist position is unchanged at printed
precision; the shoulder moves 2.686 mm, the elbow 45.342 mm, upper-arm rotation
changes 15.012 degrees and forearm rotation 12.814 degrees in 11.0149 ms. The
live input row reports the actual selected solver state. This is a held-hand
interval and a CPU-pose measurement, not a claim about a physical elbow sensor
or the final rendered mesh.

R1's fixed-length shared destination and bounded pole response address certain
discontinuities. **They do not prove realistic anatomy.** The wrist-local cue
is an estimate: wrist flexion can alter it without the user's elbow moving
the same way. A smoothly wrong destination is still wrong. No older preset,
new constant or settings sweep is offered as the solution.

The actual R1 call inputs were checked: right/left rig selection is consistent,
rest hand/forearm values are in the same validated model space, and the target
hand rotation is the final owner-resolved rotation. No demonstrated side-sign
or coordinate-space mix-up was found to justify another blind axis change.
That does not establish calibration/shoulder fit for every real pose.

R2 closes a specific replay gap. The F7 basis receipt now includes actual bone
IDs, reference upper/forearm/hand positions, reference and desired hand
quaternions, hand conversion, chest axes and hinge normal. The final ARM row
also carries the same observation identity, so a final arm rotation can be
joined without relying on row order. These are F7-gated transaction copies;
there are no new native queries and no new solver authority. R2 makes no further
change to R1's elbow destination or filter constants.

The previous source-connected geometry/response checks remain useful for their
specific invariants. They cannot certify the requested natural elbow motion.
The outstanding acceptance is the real destination and final skinned pose,
especially bow near the face and raised/overhead hands. Existing history is
used to avoid repeating old errors, not to ask the user to repeat a settings
sweep.

Evidence: `evidence/dice_elbow/CORRECTIVE_DICE_ELBOW_REVIEW_20261103.txt`,
`elbow_quaternion_audit.json`, its executable audit script and receipt tests.

## E. Crossbow: follow the complete reload ownership chain

The corrective history pass inspected 63 diagnostic files: the selected latest
50 standalone logs, three current logs, the D81-era capture and nine extracted
archive members. These contain 48 distinct byte contents, not 63 independent
sessions; duplicates are explicitly counted. The extraction selected 12,417
reload/state record occurrences across those files, or 10,123 records after
deduplicating their complete contents. Earlier review documents were used as pointers and checked
against available source/log evidence.

The D85R5 timeline contains eight admitted native leases, all light class 10
with a left supporting hand and right working hand. Two reach the native
bolt-spawn binding. One fails an unapproved event interval during transition
into loading. Later attempts contain multiple native clips, and another reaches
the dedicated reload pose for one completed frame before 27 consecutive frames
return to ordinary full-body handling while the reload clock remains owned and
paused. A native bolt existing is not the same as a bolt with a valid current
VR hand presentation.

This is a mechanical scan of the stated files plus detailed chronological
inspection of those eight attempts and relevant older intervals. It is not a
claim that every unrelated log line was manually reviewed or that there are
current live positive examples for medium and heavy reloads.

R1's mechanism observer fixed one bootstrap dependency, but it was not a whole
reload repair. R2 traces native request, clock/clip identity, completed pose,
mechanism and bolt/tool ownership rather than stopping at that helper.

### Three bounded corrective changes

**Finished duplicate transition clips.** At 00:07:28.114 and .359, layer 6
contains the same transition animation twice: the previous occurrence at 1.0
and the new one at 0.0, with another reload clip between. The old selector
rejected every duplicate animation ID. R2 can distinguish the old occurrence
only when native evidence proves it inert: previous/current times both 1.0,
matching zero segments, no loop, no sampler, no forced event scan, and no
native reload effects in that transition. Two live duplicates or effectful
ambiguity remain refused. Exact native disassembly supports the event-skip
condition; no event is fabricated or skipped merely to get past the guard.
This is a conditional repair: the old terminal entry must satisfy those native
fields at use. An independently active semantic load predecessor remains a
reason to refuse ownership; recognizing an inert duplicate does not make the
entire recorded seventh attempt valid.

**Light/medium upper-body source routing.** The reload-specific completed-pose
source must remain active for the native UpperBody reload scope even when the
unrelated global full-body choice is off. R2 repairs that route so ownership
does not disappear after its first frame. This is scoped to the active reload;
it does not enable ordinary full-body floor/locomotion behavior as a side effect.
The retained source also withdraws when the effective reload option is switched
off live. That final check clears a previously valid output with either global
full-body setting; preserving a reload source must not keep a disabled action
alive.

**Newer wrists within the same live clock.** Exact previous-sample equality
could make a real newer completed wrist unusable while the same native reload
clock/lease was still current. The revised consumer accepts a qualified newer
completed sample descended from that same live owner. It retains actor,
tracking/body epoch, scope/action/token, clock and freshness qualification.
The older exact-read primitive keeps its existing contract. A different owner,
cancelled lease, stale input or native reference does not inherit this permission.

These changes are intended to preserve the user's hand-driven sequence:
acquire the string or tool, control the pull/lever progress, retrieve the bolt
from the right shoulder into the hand, then insert it. Starting native animation
alone is not that feature. Tests exercise actual clock/admission and pose
consumers across repeated hand-pull frames, including negative controls. They
still substitute native leaves and do not show an in-game bolt in the hand.

### Explicit remaining reload failures

The fourth admitted attempt's `UNAPPROVED_INTERVAL` has no recorded scan
endpoints, and the eighth attempt's remaining clock refusal has no recorded
leaf reason. Those are retained as separate failures. The patch does not bypass native effect
admission, force an ammo count, fabricate cock/load completion or call a
successful helper result a complete reload. Tool/spanner acquisition, mounting,
lever motion, bolt rendering/insertion and native completion must all belong to
one live sequence. The current evidence does not certify that whole sequence
for every light, medium and heavy crossbow.

The missing rejection detail is now captured at the existing decision. The
bounded `D87 RELOAD_NATIVE_DECISION` receipt records the actual clock-refusal
family, native layer/scope, selected and duplicate entries, previous/current
times and segments, sampler/flags, and a mask identifying which values were
actually read. Interval refusals include the observed event list and the
from/to/permitted endpoints before the original restore occurs. A missing
native scan and a mismatched event witness have distinct reasons.

These receipts use the existing finite F5/F7/F11 capture ticket and central
64-entry nonblocking queue. Each domain/session/thread has at most 32 decision
edges; switching capture priority cannot renew an existing session's budget.
The producer copies values already obtained by the original decision, with no
extra native traversal. Formatting occurs on the existing worker only after
the capture is revalidated. OFF and expired captures enqueue nothing. The
original event permission, restore and ownership withdrawal remain unchanged.
This makes the next corresponding capture informative; it does not pretend
those missing fields existed in the older recordings.

The detailed lane review records each admitted attempt and its earliest known
block, plus the earlier D66/D73/D79/D80/D81 changes. This is a deeper supported
patch, not a claim that repeated F11 capture was missing or that the user simply
had the setting off.

Evidence: `evidence/crossbow/CAPTURE_TIMELINES.json`, selected numbered records,
native byte evidence, production fixtures and the crossbow corrective report.

## F. Evidence must describe live state

The older `CLAUDE_D66_LOG_REVIEW_20261031.txt` already records a decisive
counterexample to the previous reasoning. Four crossbow attempts at 11:53
occurred while the trial was enabled. Later receipts show it off by 11:54:08.
The final INI contained false because it represented the end state.

This revision uses timestamped runtime transitions/readbacks for claims about
settings during an action. If that evidence is absent, the state is unknown.
It does not substitute an exit profile, a menu default or a source default.
Likewise, “queued”, “native returned”, “CPU pose written” and “fresh image
displayed” are separate claims and are not collapsed into “working”.

The prompt-only migration passes 71 checks, including all eight combinations
of the three movement flags, later custom prompt values and write-failure retry.
The starter profile comparison preserves every other setting occurrence and
all calibration. If R1 was already applied, R2 does not guess which subsequent
movement choice to restore; it simply stops imposing a new movement override.
The unresolved 6DOF behavior remains a source/runtime investigation, not a
configuration diagnosis from saved data.

## Retained constraints and what this patch does not disturb

The R1 alchemy constraint remains: only qualified Henry pickup/handling/pre-work
turn can accelerate. Book/station transitions, distillation and actual
work/effect boundaries are not globally sped up. Existing supported resource
tail fixes and native particle-bearing child behavior remain. This corrective
pass adds no broader animation multiplier.

Accepted Fresh Pose timing, bow alignment, collision-admitted hand endpoints,
unholstering improvements, native IPD and projection are preserved. The user
identified H50/D52 as the same scenes and located the performance regression
before Fresh Pose; no contrary explanation or claimed return to C71 FPS is made.

The retained R1 HUD requires open, face-facing hands and gaze, with a three-second
maximum. Station prompts remain twice the tested 0.45 scale. The retained R1
render repairs address temporal output/reset ownership; the added R2 zoom note
is observation only. This pass does not claim DLSS/FSR pixel quality, residual
softness, frame rate, all kicks, throwing knives, blade-edge paired damage,
smithing reheat reliability or every alchemy effect newly verified.

## Diagnostics, validation and applying the source

All added runtime diagnostic output uses the existing KCD2VR log and finite
capture gates. F2/F5 cover the changed-zoom/publication receipt; F5 covers
pickup preparation/admission; F7 covers arm basis and final CPU rotation; F9
covers corpse search cost counters. F5/F7/F11 also carry the new exact native
reload-decision receipts. Existing F11 reload capture remains usable alongside
the other relevant domains. No new sidecar log is introduced. Gameplay
discovery and native ownership do not depend on a recorder being enabled.

Validation is reported in the per-domain evidence with actual source hashes,
controlled native leaves and expected negative failures. It includes the actual
ordinary admission/selection and settings blocks, actual menu/getter receipts,
source-connected elbow calls, actual corpse Lua/service scheduling, and actual
crossbow clock/pose consumers. Independent review checked the root changes and
the physics/reload delta; a degenerate native bounding-box fallback gap was
caught and corrected before packaging. These are source checks, not gameplay
acceptance or a Windows build.

| Verification | What passed and what it means |
| --- | --- |
| Ordinary pickup | 89 actual admission checks and 12 actual hand-selection checks; old behavior fails the intended mode/hand cases. Native and pose leaves remain controlled substitutes. |
| Settings | 71 checks, including all eight movement-flag combinations and later prompt changes. This proves preservation, not earlier runtime settings. |
| Presentation and elbow receipts | 17 serialization/gating checks and 8 actual menu/getter checks; FOV-as-zoom mutation rejected. Existing geometry/reference invariants still pass, without natural-motion acceptance. |
| Corpse | Actual Lua/native scheduling, sharing, bounds, ownership and off-gate fixtures; 228 Lua and 73 parser compatibility checks. Operation counts are modeled, not measured FPS. |
| Crossbow | 606 production-bridge checks, 273 repeated-frame/receipt checks and 158 actual tool/bolt-writer checks pass in debug, optimized and address/undefined-behavior-sanitized runs. The 273 include 26 new diagnostic assertions. Each suite rejects five deliberate mutations. Native leaves remain substituted; these are not complete game reloads. |
| Preserved paths | 29 selected render, camera, alchemy and pose files match R1 byte for byte. This prevents an accidental reversal; it is not new GPU or gameplay evidence. |
| Companion selection | The actual read-only packaging selection chooses the revised corpse Lua for Scripts/Startup/zzkcd2vr_corpse_d72.lua. All 18 existing PAKs match the prior source. No PAK is generated in this corrective pass. |

The source package provides two explicit overlays:

| Existing source | Overlay to use |
| --- | --- |
| Original uploaded D85R5 `source(4).zip` | Package-root `apply_patch.py` and manifest: cumulative D85R5 → R2. |
| Exact previously supplied R1 source | `incremental_from_R1/apply_patch.py`: R1 → R2 only. |

Run the matching helper with `--source PATH --check`, then `--apply`. The
default is read-only. Every affected before/after hash is checked before the
first write; local edits are reported as conflicts. The helper supports an
ownership-checked revert to its corresponding baseline. Neither overlay builds
or installs a mod binary, and neither is a ZIP to extract over the game.

The archive carries the complete changed source, readable diffs, before/after
payloads, current detailed evidence and retained R1 evidence. It excludes raw
full logs, compiled host executables, DLL/ASI, generated PAKs and optional gore.
The normal later release build must generate the matching companion from current
source; the corpse Lua change is part of that source, not an already installed
game asset.

The remaining failures are stated above because a release needs actual stable
interaction, natural arm behavior, usable reloads and acceptable performance.
This review corrects specific source defects and the previous unsupported
interpretations. It does not replace the user's failed gameplay observations
with host-test success.
