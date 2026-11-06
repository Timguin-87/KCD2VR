# D83R1: reviewed merge of Claude D83 and the reviewed D82 repair

## Current result

The supplied Claude D83 archive contains useful smithing and paired-action work,
but it cannot be copied over the reviewed D82 repair as a compatible update.
There are two independent implementations named D82. D83R1 merges the qualified
improvements, preserves the reviewed D82 foundation, and repairs demonstrated
problems in the added code.

Target marker: `KCD2VR-D83R1-REVIEWED-MERGE-20261102`.

This is source with production-connected host verification. No D83R1 MSVC DLL
build, native hook execution, game session, vendor GPU rendering or headset run
has been performed here. No generated DLL is presented as a validated release.

## What was supplied and compared

`KCD2VR_D83_SOURCE_20261102.zip` has SHA-256
`a822598639d9e6539af14ce2e859334808a7ec126fad962f3167b7d88b533eeb`.
Despite its name, it is an overlay: 24 source replacement/addition files and a
README, not a complete source tree. Its installation instructions target the
supplied D81R1 source. It does not update that source's build marker or export
manifest, so applying it literally still identifies the build as D81R1.

The review reconstructed the exact D81R1, D81R2 and reviewed D82 sources from
the previously supplied archive and saved patches. It also reconstructed
Claude's overlay on D81R1 and on reviewed D82. The cumulative patch can recognise
these complete trees by affected-file preimages, including the cases sharing
an older marker. It refuses a partial or locally modified mixture.

## Crossbow: retain continuous hand ownership and repair shoulder arbitration

The reviewed D82 implementation already contains the native hand-bolt path. It
joins the real bolt entity, native hand attachment and committed wrist pose,
then feeds placement/orientation back to the native reload. Its admission fix
accepts the actual 6+2 / 0+3 animation-scope profiles and protects reload
ownership from unrelated animation/event scans.

Claude's replacement creates a second light-crossbow owner that bypasses that
clock path. Its cocking threshold takes the absolute value of projected hand
travel, so motion in either direction calls native cocking. It does not apply
continuous string progress, and its README deliberately leaves tool cocking
to native animation. That does not fulfil the requested hand-owned string and
lever reload.

The replacement also appends a `//` comment before `m.nockRelease56=...` on the
same line. The assignment is commented out. Literal-body tests show a native
nock can succeed while both bow and crossbow release acknowledgement stay false.
The existing working bow path must retain that acknowledgement.

D83R1 keeps the reviewed hand-owned clock, event and attachment implementation.
It merges the useful shoulder arbitration: a held crossbow with an enabled
physical reload path reserves the right-shoulder quiver reach, preventing that
same reach from issuing a conflicting weapon/holster request. All existing
enabled-entry modes are covered. The working bow and its nock handoff remain
unchanged.

See `CROSSBOW_D83_REVIEW.md`. Rendered string/tool/bolt behaviour and inventory
results still require a real reload with each class in the game.

## Smithing: combine both post-reheat repairs

Reviewed D82 fixed the continuously moving return to the anvil: fresh lowering
and a separate lift can arm the hammer without requiring two stationary samples.
Claude's replacement removed that path. D83R1 retains it.

Claude also identified a real additional transition. The older D80 diagnostics
contains phase15 between native fall and return to idle. Three sampled
observations coincide with `INACTIVE` followed by `REBASED`; a fourth observation
coincides with a successful raise request and is included in the evidence.
The samples do not establish a missed-stroke percentage or exact phase duration.

D83R1 treats phase15 as passive continuation of an existing owned stroke. It
preserves recovery, a previously learned floor and the canonical grip. It cannot
start a new hammer press, synthesize workpiece axes or acquire fall-speed control.
Fresh hammer presses still require native idle.

A finite spatial jump during an already released native fall can rebase without
discarding observed completion. This exception is bounded by current tracking,
ownership and coherent timing within 80 ms. A long gap, backwards sample/clock,
changed source or tool loss still retires the state. No old lift is carried over.

Held LT now leaves the existing fore/aft hand-input route active. Final wrist
placement follows the current native tool transform with the captured grip, so
visible iron and native hammer evaluation agree. Claude's separate visual slide
could move the mesh up to 20 cm away from that placement and was excluded.
This merge does not claim unrestricted 1:1 lateral movement. The current hand
policy drops X; that is not proof that the engine has no lateral input API.

See `SMITH_D83R1_REVIEW.md`. Native heat, impact, quality and strike acceptance
remain game-owned and still require in-game verification.

## Paired actions: retain bounded tracked head turns with correct ownership

The useful D83 addition is limited head/neck/chest rotation while the game runs
a qualified native paired action. It retains native arms, hands, weapon, pelvis
and legs, including their alignment with the other actor. It does not drive
HMD translation or provide full upper-body VRIK. The menu describes it as
`NATIVE + HEAD TURNS`.

D83R1 repairs the new overlay's ownership boundaries:

- A published head reference must belong to the current character, pose,
  tracking source and pose-policy generation.
- Current gameplay/context checks apply before writing and again after native
  writes; a changed context rolls the writes back.
- Clip, actor, generation and toggle boundaries reset the blend state.
- A joint is recorded before attempting its write, because a failing native
  call may already have changed memory.
- Restoration also runs on C++ unwinding and publishes a shared pose fault if
  restoration fails. Ordinary VRIK cannot continue from a falsely valid pose.
- The affected mask follows the anatomical spine-to-head chain and qualified
  neck/head descendants. Native arm/item branches and off-chain torso equipment
  sockets remain unchanged.

The existing coherent-pose mutex and callback recursion guard cover this state.
The overlay cannot run alongside ordinary committed VRIK, the XR34 reload
passenger or existing paired-hands ownership. Claude's two new ON defaults are
retained; existing saved OFF preferences remain authoritative. Existing INI
values and calibration are not replaced.

The source tests and independent review qualify these boundaries. Retail
skinning, mesh appearance, comfort and Windows SEH behaviour remain untested.
See `FINISHER_D83R1_REVIEW.md`.

## DLSS/FSR: preserve the reviewed history and presentation implementation

Claude's native entry/layout pins and callsites match the supplied game image.
Its surrounding state handling nevertheless has reproducible faults. If the
right-eye context cannot be created, it renders RIGHT through LEFT's context.
The literal implementation also reproduces asymmetric resets, unhandled missing
eyes, unscoped history advancement and duplicate evaluation without frame
qualification. These behaviours occur in controlled source tests for DLSS, FSR
and XeSS; they are not asserted as newly captured D83 game events.

Its header also removes the methods used by the reviewed D82 startup, native
eye flow and final-composition callers. Overlaying it onto reviewed D82 breaks
that source interface; overlaying it onto D81R1 omits the reviewed D82 gates.

D83R1 keeps the reviewed DLSS/FSR instances, reset continuity, exact pair receipts,
mode-change checks and final-composition publication. Native stays available;
XeSS Stereo is not merged. No upscaler is selected automatically, and no
`user.cfg` assignment is deleted by the patch. Use the existing D82 explicit
startup selection and full-restart workflow.

See `UPSCALER_D83_REVIEW.md`. Correct GPU inputs, headset fusion, memory cost and
FPS remain runtime acceptance work. This patch does not claim a C71 performance
recovery or vendor frame generation.

## Combat: correct the evidence labels and retain the timing repair

The capture's final recognizer counters sum to 258 onsets. Those are not a
measured total of human swings. There are 79 gesture IDs with queued contacts,
and 64 have exact native results. The 15/79 figure is an unmatched-result
category; it is not a validated 19% player miss rate. A missing result can have
several causes, and the diagnostic stages have different denominators.

The exact outcomes remain 30 health-damaging results, 21 stamina-only results,
11 perfect blocks and two parries. One result is combo-marked and the candidate
advances to 2/3; no completed finisher is established. Route counts alone also
do not prove that a direct route can never advance native combo state.

D83R1 retains the reviewed D82 final-contact repair for the 109 ms expiration /
140 ms contact sequence, without extending ordinary command deadlines or
changing the user's combat settings. The README's proposed tuning changes are
not applied on the strength of those counters. See `COMBAT_D83_LOG_AUDIT.md`.

## Build and handoff

Apply the cumulative patch to a recognised complete source tree using its
`apply_patch.py`, then run the normal Windows `build_release.bat`. The installer
checks all affected preimages, preserves exact backups and supports a read-only
`--check`. It does not install into the game or replace the runtime INI.

The useful next acceptance checks are the actual transitions: reheat and return
while moving the hammer; hold LT while moving the workpiece fore/aft; pause a
crossbow pull and retain/place its bolt; turn the head during a paired native
action; compare Native with a selected DLSS or FSR quality after a full restart.
The existing unified `KCD2VR.log` identifies the revision and feature routing.
No new sidecar recording is introduced.

`VALIDATION_D83R1.md` separates newly executed tests, retained D82 evidence,
native input evidence and the unperformed Windows/headset boundary. Earlier
changelogs remain historical records rather than claims that later runs passed.
