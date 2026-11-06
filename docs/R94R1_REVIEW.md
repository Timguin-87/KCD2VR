# R94R1 reviewed repairs — 4 November 2026

This is a cumulative source successor to R93R4, commit
`1b3f4e4cd5fc8e924de256a1c58945544bbe1013`. Extract the full source into a new
directory. No earlier patches are needed. The export manifest records this
revision's exact commit and every exported file hash.

The incoming Repair R1 was not merged unchanged. I compared its ownership
transitions with the earlier independent review, then corrected the remaining
holes below. The separate review archive contains the input hashes, focused
results, independent reviews and actual Windows build receipt. The older
`R93R4_REPAIR_R1.md` is the original proposal, not the final behavior.

## Crossbow reload ownership

Two different gaps were returning the reload to native presentation:

1. An otherwise complete body packet could lack its auxiliary reload payload.
   The old path treated that as input loss. The repaired path distinguishes a
   missing payload from a real owner/tracking change and pauses only the exact
   native reload clock it already owns. It keeps the original sample/time and
   original 80 ms limit. It earns no pull, acquisition, nock or ammunition credit.
2. The next tool/bolt target was published before its body transaction completed.
   The attachment callback could pair it with the previous wrists and withdraw
   placement. Source, target and wrists now travel in the same committed body
   packet, with the same publication serial.

The incoming patch missed a release during that temporary gap. A later packet
could interpret the old-to-new Grip change as a valid bolt release. Current
qualified input now records only the observed release, without refreshing the
old pose. Recovery requires a fresh regrip before it can credit another placement.

The primary crossbow hand and the hand actually holding a string, tool or bolt
keep their own attachment/finger owner. Only a verified free working hand uses
ordinary free-hand contact and curl. The actual native bolt remains the object
being placed, not a visual substitute.

The source and fixtures cover native classes 10, 14 and 15, including a second
reload. Light uses the string hand; the medium and heavy paths use their native
spanning objects. The heavy path still uses the existing lever/rail gesture,
not a newly proven rotary crank. Native spawning, clips, events, inventory cost
and reload completion remain authoritative. Missing first placement, a replaced
native action, foreign clock changes or genuine input loss still relinquish
control. This is not a claim that every crossbow now completes in a headset.

## Pickups and station handovers

The incoming pickup repair covered the attachment callback but not the following
tracking/body update. After native Picking cleaned up and the item became Held,
the generic gameplay-policy cache could still say false. That next update then
invalidated the body used to place the object.

An exact adopted-held receipt now connects the tracking producer, body admission,
body foundation and attachment writer. It requires the same item, character,
body/tracking epochs and fresh physical Grip. It does not resurrect the completed
Picking action or grant gameplay permissions. Only the policy-gap body needs this
exception; ordinary valid body updates remain publishable when Grip is released.
Menus, foreign actions, item replacement, release and lost tracking still revoke
the exceptional presentation. The receipt clock is taken after the fixed input
snapshots so a concurrently newer sample is not rejected as being in the future.
The existing native crouch getter also accepts that same typed held-item context
while the policy cache catches up. It rechecks ownership after the read and
accepts genuine ordinary-policy recovery; it does not freeze an old stance or
change camera code. This prevents a new body-height jump for controller crouch.

Alchemy had the same clock-order error: validation sampled time before reading
newer control/tool snapshots. Those snapshots are now read first and validated
against the later clock. This removes that proven false rejection; it does not
claim universal instant resource attachment, sound or particles.

Dice exit now distinguishes an unavailable policy sample from explicit OFF or a
confirmed context change. Missing data cannot authorize another edit, but it no
longer destroys the visit identity needed for the exact restoration. The dice
camera, table layout and stereo view were not retuned. Physical cup handling is
still incomplete.

## Hand timing and elbow evidence

The accepted paced fresh-pose mode remains the missing-key default (2).
Fresh Pose Uncapped (4) stays a separate saved choice. It now acquires a separate
head-reference/left/right family before the native update under supported source
modes 0, 1 and 2. It uses nonblocking locks and a coherent, bounded runtime
prediction reference. That head is only the coordinate reference for the hands;
it is never published as the camera head. Source mode 3 is unchanged and unsupported
for this family. No controller low-pass delay was added.

The worker can no longer overwrite the independent family with a differently
timed packet. I also fixed a narrower race found during independent review:
a contended read after that family has taken ownership cannot fall back to the
worker lane. Original timestamps and the existing tracking grace remain intact.
Unavailability is reported honestly rather than relabeling an old pose as fresh.

F7 now records which elbow solver actually ran, its calibrated hand/body cues,
native seed, prior bend, target, confidence and clamps. The exact production
serializer and replay have synthetic tests. The numerical elbow solver is
unchanged. Neither these records nor the added sampling establish final rendered
hand smoothness or a measured FPS improvement.

## Combat and optional gore

The reviewed combat changes are deliberately narrower than the proposed overlay:

- The idle combo-slot experiment defaults OFF when no value was saved. A live
  slot must not outlive its action/listener and swallow the next press. Existing
  explicit true/false values remain respected; recovery-hold settings are unchanged.
- A successfully announced recipe zone survives an ordinary refinement only for
  the same native prepare, gesture, weapon, request/body/source and opponent
  actor/state/entity/ID. The current native slot and zone are rechecked. Target
  changes and deliberate feints resume the ordinary path; they do not cancel a
  valid attack just because the preservation lease no longer applies.
- The existing native hit callback observes a successful, applied Henry kill.
  Attacker, target and their entity/state identities must match before and after
  the original call. Evaluation-only hits, KO-only results and replaced owners
  cannot register a corpse. No damage, stamina or health is manufactured.
- Nonlethal limb contacts cannot replace a lethal severing candidate. Corpse and
  genuinely lethal lanes remain separate. This does not discover every corpse
  already stored in a save or implement a hand-to-corpse constraint.
- The exposed power floor now matches the existing effective 0.9 full-swing floor.
  The sample INI states 0.9; the installed user INI is not edited.
- Optional gore's on-screen Lua messages follow F2. This is a script correction,
  not new hair, cap or helmet art. Gore assets stay separate from the main PAK.

Paired finisher observation now distinguishes a reached native callback from a
missing hook, including when the extra VR filter is OFF. Native paired damage
remains native; there is no invented blade-contact damage or instant 64x finish.

## Small props, doors and diagnostics

The small-prop experiment corrects RigidBodyLight classification and uses the
native finite-mass rigid-body velocity path instead of scenery teleport carry.
It remains OFF by default. Positive native body readback, exact identity, reach,
size/mass bounds and current hand ownership are required. Promotion leaves the
prop physical after release. The actual new collision mesh, breakage and visible
behavior are not proven by a host API double. Ordinary native pickups retain
their own route; this is not a universal pickup guarantee.

The donor door patch wrongly prohibited key/manual-side unlocking. Native
OnUsed retains those cases and the original hand clip. Truly locked/unknown
unusable doors cannot acquire the gesture; relocking revokes the gesture.

Added haptic receipts distinguish queue acceptance from XR submission and from
a pulse actually felt. The latest-value queue can overwrite a cue, so an absent
terminal receipt is not proof of delivery. New motion, elbow, interaction and
finisher details use the existing diagnostic gates and central log. No new
always-on scan or general renderer readback was added.

## Changes intentionally left out

The earlier overlay's renderer fallback could accept a pair before final RIGHT
completion. It is not included. Neither are its shared-reader spin, frozen bow
arrow fallback, blanket same-equipment request swallowing, alternate hand-only
reload replacement, forced saved-option migration, or unqualified SPECIAL kick
selection/table changes. Those would undermine the accepted owners or native
semantics. Current bow aim, camera, stereo/upscaler boundaries and calibration
are protected by exact source/function comparisons.

## Validation and limits

Focused checks execute production functions or current extracted adapters with
explicit native/XR leaves. They include failure and mutation controls, not just
checks that constants exist. Ranged tests cover all three classes, source gaps,
release during gaps, atomic publication, partial writes, expiry and repeated
reloads. Motion checks cover source/mode/epoch changes, contention, original age,
no camera-head write and no wait in uncapped mode. Pickup tests cover adoption,
the next producer, release, source replacement and protected contexts. Additional
checks cover alchemy clock ordering, dice exit, door exceptions, disabled props,
combo leases, the actual hit observer and optional Lua behavior.

The Windows `build_release.bat` RelWithDebInfo x64 link completed successfully.
The final DLL SHA-256 is recorded in the delivery manifest.
Embedded identity, menu art and system-only imports passed verification. The
link retained four existing proxy-export ordinal warnings (LNK4222), with no
compile or link error. All 60 protected source/function comparisons passed.
The delivery receipt records the final focused results and packaged hashes.
The requested legacy shell aggregate cannot configure on this host because its
required clang++ is unavailable; it is not reported as passing. MSVC host tests
and a Windows link do not replace headset validation. Negative controls that
intentionally fail are listed separately from current-source failures.

Installed dinput8.dll was identified as R92R1 (SHA256
`45e6723c206d243b9271d3280fb0a1e149e74f40a978e5473382b5c5ffa6e0db`).
The reviewed recent recordings are historical R92R1 evidence, not a test of
R93R4 or this successor. Retail WHGame.dll matches the qualified hash
`bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7`.
No game files, user settings, saves or calibration were installed or reset.

To build, run `build_release.bat`. Optional gore can be generated separately
with `-OptionalGoreSource` pointing to the reviewed D77 asset package. It is not
embedded in the source archive or main gameplay PAK. Raw historical recordings,
intermediate binaries and private review trees are excluded from full source.
