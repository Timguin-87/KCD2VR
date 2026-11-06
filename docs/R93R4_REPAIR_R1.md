# Incoming proposal — superseded by the R94R1 integration review

This is the repair author's original handoff, retained for provenance. Its
defaults, door behavior, test totals and unbuilt status describe that proposal,
not the final integration. Read `R94R1_REVIEW.md` for the delivered behavior.

# R93R4 Repair R1 — source patch and integration handoff

Date: 4 November 2026. This is a source-only repair based on the exact supplied
R93R4 archive. It is not a compiled or headset-tested release.

## Outcome and scope

The six concrete defects in the R93R4 review are repaired in source: CR1, CR2,
PU1, AL1, DI1 and SP1. The patch also implements independent Fresh Pose Uncapped
hands under retained head-source modes 0–2, makes reload hand ownership
side-specific, records the actual selected R89 elbow inputs, replaces small
scenery teleport carry with native rigid-body driving, and refuses locked-door
hand acquisition. Existing stealth-overlay parent fitting was verified; its
scale was not speculatively retuned.

Two requested features remain incomplete: **CB1 tracked-blade control of native
paired damage**, and **DC1 physical dice-cup take/shake/place**. Their missing
native event/ownership contracts are identified below. This patch does not
claim either feature is finished or turn on an unsupported timing shortcut.

The acceptance boundary is source-connected host verification. No Windows game
DLL, ASI, retail engine run, GPU submission measurement, native inventory run,
controller actuator measurement or headset validation was performed. Native/OS
services used in host fixtures are explicitly substituted. Portable SEH syntax
leaves do not verify Windows exception behavior.

## Exact baseline and identity

- Input: `KCD2VR_R93R4_FULL_SOURCE_20261104.zip`.
- Archive SHA-256: `73e67b6578ac9648c8204e98fe0659c56ff6a8e39e5ce59d2a12dc5360b5e4ed`.
- Supplied source marker: `KCD2VR-R93R4-FINAL-RELOAD-GORE-20261104`.
- Patched source marker: `KCD2VR-R93R4-REPAIR-R1-20261104`.
- The supplied source manifest names revision `1b3f4e4cd5fc8e924de256a1c58945544bbe1013`.
- The local baseline commit used solely to generate this diff is
  `7c2c2ea92541c55bde386cba18a1c1dce7b54d5e`; it is not a new upstream release.

`MANIFEST.json` in the delivery lists every changed/new file with before/after
SHA-256 values. No user INI, calibration file, original game archive, DLL, ASI,
PAK, raw gameplay log, private repository history or complete source tree is in
the delivery. Internal R94/R95 suffixes identify repair helpers, not separate
published releases.

The latest substantive recordings supplied with the review are R92R1, not
R93R4. Ten log files include main/diagnostic pairs. Historical observations are
not treated as a current build test; the exit-saved INI is not live settings
evidence. The new build marker permits a subsequent real run to be identified.

## Repairs

### CR1 / CR2 — crossbow source gaps and publication

The source producer distinguishes auxiliary reload data being unavailable from
positive ownership loss. A valid completed ordinary body may temporarily lack
its reload socket. That condition retains the original last complete reload
source and its original sample, tick and body-publication serial; repeated gaps
cannot extend the 80 ms deadline. The client pauses only its exact owned native
clock, clears unproved constraints and returns before awarding any new input,
progress, event or ammunition credit. A source at 80 ms may pause; 81 ms expires.
True native owner/action, weapon, body/request/tracking, setting, context or hand
claim replacement still revokes immediately.

The target, source and wrists now travel in one `CommittedFrame6695` packet and
share its exact publication serial. Staging a target cannot expose it before
its body. Pending publication can retain only an unchanged, previously achieved
placement of the same binding, with its original age and no new destination
write. A missing placement receipt can pause an already moving pull, rather
than requiring the clock to have been zero before the gap. Geometry failure
and real identity change remain refusals. A partial placement consumes the
callback and denies an achieved-placement receipt; it cannot fall through to a
second native writer.

Native torso ownership, clip/event tables, crossbow class 10/14/15 rules and
paid bolt/load accounting are retained. Successful host callbacks do not prove
retail stock was consumed or a visible reload completed.

### RP1 — each reload hand has an explicit owner

The primary crossbow grip remains tracked with native fingers. A held tool,
bolt, string or mounted mechanism retains its own reload constraints and native
finger pose. A verified free working hand can use the ordinary bounded contact
and free-finger path. Tracked hands can reach the ordinary elbow path when its
other ownership guards permit it. The ordinary weapon/holster executor does not
run a competing reload writer.

An exact native reload occupancy observer supplies fresh hand-item identity
when the ordinary gameplay equipment mailbox is stale. Loss of that owner
returns unknown, not stale empty-hand data. The diagnostic now says
`physicalReload=XR34_SEPARATE_NATIVE_CLOCK_OWNER`.

### PU1 — adopted pickup presentation

The attachment callback can obtain the exact finishing/adopted presentation
witness even when ordinary binding succeeds while the independent gameplay
policy cache still says false. It joins the same held resource and native hand;
it cannot use another pickup's token or object. The 80 ms cleanup-only limit is
removed only after exact semantic adoption. Held-token lifetime, generation,
retirement, source freshness, raw input, native attachment and final writer
checks remain. The original timestamp is never moved forward to hide age.

### AL1 — alchemy snapshot clock ordering

All relevant control/table/tool snapshots, including the other-hand plate
witness, are acquired first. The freshness clock is sampled afterward, and
validation uses those exact copies. A valid publication that arrived after
callback entry is no longer rejected as being from the future. Mode, instance,
resource lifetime, tracking epoch, current attachment and final writer checks
remain. The explicit-clock helper is retained for deterministic tests; live
presentation consumers use the read-then-clock adapter.

### DI1 — dice-exit restoration

Explicit eye-policy OFF is distinct from unreadable or stale policy data.
Temporary unavailability suspends output writes while preserving only the
qualified visit identity. Positive player/lifecycle replacement, other activity,
dialogue or protected camera ownership revokes it. Before an exit FOV write,
the adapter rechecks native dialogue/player/camera, policy and dice lifecycle
after the output read. The retained identity itself grants no lens authority.

This corrects a source restoration defect. It does not establish the cause of
every historical frozen stereo image or certify visible recovery in the headset.

### FP1 — independent uncapped hands, accepted view retained

The early game-update route can acquire a new hand family with cached head
source 0, predicted source 1 or current-time source 2. Source 3 remains explicitly
unavailable. All three poses in the hand family use a common space/time lookup;
the head member is only a coordinate reference. It never publishes the camera
head, changes camera snapshots, synchronizes actions, generates input edges or
submits haptics.

Modes 0/1 use one coherent XR-worker prediction anchor, advanced continuously
between worker frames and bounded to four supplied periods. The route rejects
stale anchors, changed tracking epochs, unavailable conversion and contended
locks without waiting for them. A backward target cannot replace the current
hand history or renew its age. Independent hand and worker mailboxes are
separate, so a later worker publication cannot overwrite the selected hands.

A dedicated hand-source generation changes when entering/leaving Uncapped.
F7 rung, elbow and continuity setting changes no longer invalidate that lane and
briefly select an older worker sample. Existing Fresh Pose capped/default mode
2 and explicit saved choices remain. F7 exposes requested/effective acquisition,
source, target, validity, publication and unavailable reason. No pose acquisition
receipt proves final rendered smoothness or improved FPS.

### EL1 — actual R89 diagnostics and exact replay

The production branch records a selected-solver enum. When R89 is actually
selected, it records calibrated hand-forward/thumb vectors, body cue, native
seed, torso geometry, weights, target, prior state, confidence, clamps and output
at sufficient precision for replay. Records join by session, kind, observer
ordinal, sample and side. No inferred local axes replace those inputs.

`tools/replay_elbow_r94.py` restores each record's exact prior state and executes
the current production solver; it does not treat sparse capture as a complete
motion sequence. It rejects policy mismatches and compares output. Diagnostic
observation does not change numerical solver output. An 18-case synthetic
production-serializer replay has zero error; altered output and weights fail as
expected. Natural anatomy and visible flicker still require a current F7 run.

### SP1 and physical small objects

Exact `RigidBodyLight` admission now precedes the protected `Light` substring.
Generic lights/lamps, actors and native interaction classes retain their guards.
A supported standalone small entity is promoted, when necessary, to a native
finite-mass rigid body before it can be held. Admission verifies the original
contact/body identity, proxy and foreign owner, one valid slot, unchanged render
bounds/scale/pose and post-promotion rigid-body/mass readback; a Lua call merely
returning is insufficient. It does not enumerate the new collision mesh or prove
a successful collision in the game.

The hold uses the existing pinned bounded velocity servo with the raw controller
target. Physics remains enabled for native collision response; the route no longer
teleports the entity or rephysicalizes it on release. Promoted scenery remains
rigid after release. Achieved native linear/angular motion is preserved
on release. The controller wrist stays authoritative; achieved-body presentation
is not fed back into its control target. Hand/entity reservations exclude other
prop, F9 constraint and attachment writers.

Current bare-hand contact and exact immutable weapon impacts can submit bounded
native point impulses. A first weapon strike on an eligible static PickableItem
can use the existing same-stack native weight admission, promote it, verify the
new rigid body and apply that original contact impulse. No Use, theft action,
inventory transfer or automatic pickup is fabricated. Normal mode no longer
depends on the narrow apple/jug F9 experiment.

`VrStaticPropGrabR89` defaults on only when the key is absent; an explicitly
saved false remains false. A matching rebuilt companion is required.

Admission is bounded to 3 kg, 65 cm transformed bounds diameter and 30 cm grab
reach. The prop motor releases beyond 35 cm target/body separation; that limit
applies to the object motor and does not shorten tracked-hand reach.

Limits: fixed brushes are not detachable entities; a mesh with no physics body
provides no contact witness and is not discovered by this patch. Unsupported,
parented, multi-slot, quest/owned and heavy objects remain excluded. Breakable
classes may use an existing dynamic body but are not blindly rephysicalized
from static state. Generic scenery without a real inventory item cannot become
an invented inventory item. Save/load persistence, authored resets, threading,
friction, throwing feel, fracture behavior and performance require the game.

### Doors, stealth overlays and diagnostic honesty

Locked or unknown-lock-state doors refuse hand acquisition, even if the player
has a key or approaches a manual-unlock side. Native key/lockpick actions remain.
After native unlock, either hand can use the original object-only door operation.
A new lock during a hold stops controller seeking and retires through the exact
already acquired native operation; no unlock, endpoint seek or new action is
invented. Retail Lua plus both VR scripts were exercised in both startup orders.

The current stealth overlay adapter fits the native parent of artwork and cursor
for lockpicking/pickpocketing, uses bounded retries and restores its lease.
Existing fit/readback/restoration/destruction tests pass. No additional scale
constant was guessed; headset sizing/cursor alignment remains a visual check.

F3 candidate receipts now distinguish an observed native candidate callback,
no callback at an installed boundary, and an unavailable hook, including when
the extra VR candidate filter is off. Recipe progression, optional filter and
final-contact permission remain separate. F5 haptic receipts distinguish queue
acceptance/rejection, expiry, policy/focus refusal and XR submission/result,
joined by cue identity and the original finite capture session. The existing
mailbox keeps only the latest cue: an accepted cue overwritten before consumer
observation may have no terminal receipt. Logs say `queueModel=LATEST_ONLY` and
`physicalDelivery=UNVERIFIED`. XR success does not prove a physically felt pulse.
The old aggregate submit log is replaced by cue-correlated receipts explicitly
bound to the original finite F5 capture session and the central log.

## Incomplete native features

### CB1 — tracked-blade control of paired damage

The source has exact reciprocal pair/action identities. Available contact and
applied-hit observations do not join the selected finisher recipe/gesture to
the exact native CAF hit event or CombatAttack procedure occurrence. An applied
hit is an observation after damage; target/time proximity cannot authorize a
new earlier damage event. A pair may contain multiple authored events.

`stepInstantD72` therefore remains at native 1x with
`PAIR_DAMAGE_CONTACT_UNPROVEN`; the unproved 64x path is not re-enabled. The next
implementation needs a verified occurrence-specific bridge from native pair
construction through the authored event/procedure and actual tracked blade
contact to native once-only damage/reaction dispatch. Merely advancing both
paired clocks or enabling tracked arms would not complete this feature.

### DC1 — physical dice cup

The verified implementation supplies a right-hand pointing/cursor experiment
and native A dispatch. It does not supply an exact cup occurrence, current native
cup-hand attachment, permitted manipulation interval, placement owner or
once-only throw completion. Retail scripts/assets and prior captures do not
establish that complete callable bridge. The diagnostic names each missing
join. No guessed offset or generic input macro is presented as cup physics.

## Applying and building

The delivery ZIP contains `changed/` with only changed/new source files, the
unified `R93R4_REPAIR_R1.diff`, before/after `MANIFEST.json`, a read-only manifest
verifier and focused evidence. Apply from the R93R4 source root, not the game
installation. First verify the baseline and check the diff:

```text
python PATH_TO_PATCH/verify_patch.py --source PATH_TO_R93R4_SOURCE --state before
cd PATH_TO_R93R4_SOURCE
git apply --check PATH_TO_PATCH/R93R4_REPAIR_R1.diff
git apply PATH_TO_PATCH/R93R4_REPAIR_R1.diff
python PATH_TO_PATCH/verify_patch.py --source PATH_TO_R93R4_SOURCE --state after
```

Use the exact extracted patch paths. If before hashes differ, review/merge the
diff against those local changes; do not force-overwrite them. `changed/` is also
provided for manual integration. The verifier never writes a source file.

On the supported Windows build environment, follow `docs/BUILDING.md` and run
`build_release.bat`, optionally with a separate `-BuildDirectory`. Rebuild both
the DLL and main companion so the C++ and new Lua entry points match. The existing
canonical companion mapping includes both modified Lua files. No original game
PAK should be replaced, and no user INI/calibration migration is included.

For reversal before further source edits, first run `git apply --reverse --check`
with the same diff, then `git apply --reverse`. Preserve any subsequent edits by
reviewing conflicts instead of forcing reversal.

## Reproducing focused host checks

Run from the source root with output outside it. The Python runners document
their compiler/runtime requirements. Some are Linux/GCC-focused; they do not
replace the normal MSVC release build. Lua checks here use the system Lua 5.4 VM
with explicit compatibility names; retail uses Lua 5.1.

```text
python tools/test_fresh_pose_uncapped_core.py OUTPUT/fresh_hands
python tools/test_fresh_pose_uncapped_menu.py --output OUTPUT/fresh_menu
python tools/test_elbow_diagnostic_r94.py OUTPUT/elbow_exact
python tools/check_interaction_review_r95.py --out OUTPUT/interactions
python tools/check_crossbow_pipeline_d87.py --out OUTPUT/source_gap --source-gap-only
python tools/test_crossbow_socket_d78.py --out OUTPUT/publication --publication-only
python tools/test_crossbow_socket_d78.py --out OUTPUT/socket
python tools/check_crossbow_pipeline_d87.py --out OUTPUT/body --body-source-only
python tools/check_crossbow_pipeline_d87.py --out OUTPUT/placement --placement-only
python tools/check_crossbow_pipeline_d87.py --out OUTPUT/pipeline --debug-only
python tools/run_contact_occupancy_d52.py --out OUTPUT/contact
python tools/run_static_prop_r94.py
python tools/test_static_prop_lua_r94.py
python tests/test_prop_wake_d40.py
python tools/test_door_lock_r94.py --scripts-pak PATH_TO_RETAIL_SCRIPTS_PAK --out OUTPUT/doors
python tools/verify_r93r4_view_preservation.py PATH_TO_BASELINE_ZIP OUTPUT/view_preservation
```

The package evidence index and individual results preserve exact commands,
source hashes, positive outcomes and deliberate negative controls. A negative
control passes only when the intentionally broken behavior is detected; its
nonzero test exit is expected. Compiler failure is not accepted as that proof.

## Recorded validation results

All results below are host verification of the identified source paths. Counts
are per execution unless stated otherwise; repeated compiler configurations are
not additional unique scenarios. Expected-failure controls pass only when the
fixture successfully compiles and detects the intentionally broken behavior.

| Area | Recorded result | Configuration / evidence |
| --- | --- | --- |
| Fresh hands | 62/62 checks | Actual extracted manager and production mailbox; controlled XR/clock leaves. Ten consecutive executions also pass after removing a fabricated future timestamp from the worker fixture. |
| Hands menu and INI migration | 209 checks, zero failures | Actual extracted branches; two legacy-behavior mutations rejected. |
| Protected view/calibration/bow paths | 60/60 source checks | Exact source comparison to supplied R93R4 ZIP; includes RunFrame with only the three-line read-only hand anchor observation permitted. |
| R89 diagnostic invariance and replay | 18 complete rows, zero output error | Production serializer and solver with exact prior states; altered output and altered policy both detected. |
| Pickup adopted-policy adapter | 94 assertions per execution | Debug, optimized, UBSan. |
| Pickup cleanup/adoption owner | 172 checks per execution | Actual owner implementation; debug, optimized, UBSan. |
| Alchemy snapshot/clock adapter | 71 checks per execution | Debug, optimized, UBSan. |
| Dice-exit adapter | 74 checks per execution | Debug, optimized, UBSan; late owner and dialogue changes covered. |
| XR haptic consumer | 32 checks per execution | Debug, optimized, UBSan; runtime mocked. |
| Interaction suite as a whole | 34/34 entries accepted | Fifteen positive executions above, nine regression executions and ten expected-failure source mutations. |
| Reload source gap and hand policy | 205 checks per execution | Debug and optimized; two source mutations detected. |
| Atomic reload target/source/body | 85 checks per execution | Debug, optimized, ASan/UBSan; three mutations detected. |
| Socket/mechanism regressions | 310 checks per execution | Debug, optimized, ASan/UBSan; twelve mutations detected. |
| Reload body and placement | 78 body checks and 127 placement checks per execution | Both debug and optimized. |
| Reload pipeline and cycles | 273 pipeline checks; 111 cycle checks; transition PASS | Debug; no numerical tally claimed for transition. |
| Reload contact occupancy/rejoin | 54 checks | Actual contact policy with explicit native leaves. |
| Small-object native adapter | 38/38 checks | Existing native services substituted; source motor/admission executed. |
| Small-object companion | 42/42 cases | Actual companion Lua, host Lua 5.4 with engine services substituted. |
| Small-object policy and prior paths | 53/53 policy checks; 12/12 legacy Wake cases | Existing native-grab producer and prop-servo suites also pass. |
| Doors | 64/64 checks | Supplied retail AnimDoor plus both VR scripts, both startup orders, both hands. |
| F3 finisher callbacks | 47/47 checks | Extracted production callbacks with native selector boundary double. |
| Stealth overlay parent | PASS | Actual adapter fit/readback/restoration, bounded retries and destruction guards. |

The retained source normalization receipt records only line-ending restoration;
execution-time hashes are preserved instead of being relabeled as delivery-time
hashes. `MANIFEST.json` supplies the final delivered hashes. The final package
also records a clean baseline hash check, `git apply --check`, patch application,
after-hash check and reversal back to the baseline.

## Next real-run acceptance

1. Build the marker above and the matching companion. Preserve current
   calibration, bow alignment, camera/head-source choice and capped settings.
2. Compare capped and uncapped hands while preserving the same head policy.
   Include F7 enable/rung changes, temporary tracking loss and late-session use.
   Check source/body/render cadence and actual hand appearance, not only FPS.
3. Exercise paid class 10/14/15 reloads, interruption during a moving pull,
   tool/bolt handoff, native owner replacement and the free working hand.
4. Exercise pickup adoption with each hand, return-to-gameplay delay, release,
   stow and haptic queue-to-XR receipts; then alchemy tool/plate transitions.
5. Exit dice across a short policy gap; check visible zoom restoration. Physical
   cup behavior is not an acceptance claim for this patch.
6. Touch/grab/release supported small scenery and native items; include a first
   weapon contact, body replacement, real collision, drop/throw and reload/save.
   Check locked/keyed/manual-side doors, then natively unlock and use both hands.
7. Inspect lockpicking and pickpocketing artwork/cursor together at current
   native HUD canvas sizes. Do not infer visual fit from the adapter's return.

Use only the relevant finite F3/F5/F7 capture in the existing central log. The
source success and runtime acceptance columns must remain distinct.

## Primary research references and applicability

OpenXR specifies lookup in a base space at a requested time and distinct valid/
tracked flags. A lookup is not a smoothness guarantee; the new adapter reads pose
components only when their valid flags permit it. The QPC conversion extension
supplies an XR time domain; the hand prediction anchor does not retune the view.

- [OpenXR xrLocateSpace](https://registry.khronos.org/OpenXR/specs/1.0/man/html/xrLocateSpace.html)
- [OpenXR location flags](https://registry.khronos.org/OpenXR/specs/1.1/man/html/XrSpaceLocationFlagBits.html)
- [OpenXR QPC time conversion](https://registry.khronos.org/OpenXR/specs/1.0/man/html/xrConvertWin32PerformanceCounterToTimeKHR.html)
- [CryEngine physical entity types](https://www.cryengine.com/docs/static/engines/cryengine-5/categories/23756813/pages/24282101)
- [CryEngine physical entity interface](https://www.cryengine.com/docs/static/engines/cryengine-5/categories/23756813/pages/24282108)
- [HIGGS project](https://github.com/adamhynek/higgs)
- [PLANCK project](https://github.com/adamhynek/activeragdoll)

The CryEngine documentation supports distinguishing fixed geometry, dynamic
bodies and velocity/impulse behavior. It is not authority for KCD2 retail vtables
or offsets. HIGGS/PLANCK identify Havok/SKSE-specific dependencies; their ABI was
not transplanted. Direct implementation-file retrieval was unavailable, so this
handoff does not claim those implementations were fully inspected. Supplied
retail scripts and the repository's existing pinned native contracts provide
local evidence; no new retail ABI layout was guessed.
