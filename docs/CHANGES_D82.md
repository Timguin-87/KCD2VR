# D82 — crossbow ownership, smithing re-entry and stereo upscalers

## What this revision is based on

The latest supplied recording identifies its running build as
`KCD2VR-D81R1-REVIEWED-MERGE-20261102`. The user reports faster alchemy and better
collisions, unreliable hammering after reheating, some missed combat hits, and
crossbow reloads that still behave like an animation trigger. Those observations
are the starting point for this repair. Earlier test counts are not substituted
for those observations.

D82 also carries forward the launcher/config safeguards from D81R2, then replaces
its Native-only selection policy with actual per-eye DLSS and FSR bridges. The
new build identity is `KCD2VR-D82-RELOAD-SMITH-STEREO-20261102`.

**This is a source patch. No D82 Windows DLL or headset run has been produced
here.** The host checks execute production code or literal production functions
with explicit native/GPU leaves. They establish the repaired connections and
failure handling, not the rendered image, sound, native hit acceptance or FPS.

## Crossbow: repair the native-to-hand connection

The three captured light-crossbow reloads were accepted, then lost ownership
after 94, 63 and 16 ms. The current source had two copies of the same wrong
assumption: a reload root scope must have exactly one layer slot. The supplied
animation assets instead define UpperBody as layer 6 with two slots and FullBody
as layer 0 with three slots. Their crossbow Load fragments have one populated
animation layer; the extra slots are reserved capacity.

Both the clock producer and the actual VRIK passenger consumer now accept those
exact authored profiles, while requiring the companion slots to remain empty
and the selected CAF to remain unambiguous. Unrelated animation scans no longer
invalidate a current reload pose, and animation events that the native reload
handler ignores no longer cancel hand ownership. All 12 native effectful event
cases retain their strict checks.

The existing physical mechanism can therefore reach its intended sequence:
grip and move the string or lever, obtain the bolt from the quiver, retain it in
the tracked hand and place/release it at the rail. The game continues to own tool
and ammunition creation, cocking/loading state and inventory transfer. This
repair does not declare a reload complete merely because a gesture was seen.

The new test starts with no completed reload pose and crosses both real
production gates before it exercises physical movement. That matters because
the older tests supplied a one-slot scope and often supplied an already completed
source, so they could miss the failing first admission. Light, medium and heavy
layouts are covered; the actual capture contains light-crossbow failures only.

See [the crossbow report](CROSSBOW_D82_REPAIR.md) for the exact asset evidence,
test seams and the remaining tool/bolt rendering limits.

## Smithing: allow a moving hand to rearm after reheating

Leaving the anvil clears the preceding stroke. Returning previously required
two consecutive samples with at most 6 mm of hand movement before another lift
could arm. A player continuing to move the hammer could remain unarmed indefinitely.

D82 keeps quiet entry and adds a fresh native-idle down/up sequence. Lowering
at least 18 mm can arm the next lift; the separate 30 mm lift requests the native
trigger, and a subsequent downward reversal releases it. Movement during heating
or transfer cannot prearm that sequence. Native charge, exhaustion, heat, impact
and work quality remain unchanged.

This dead zone is reproduced against the old source and repaired in the actual
final-pad adapter. The supplied recording has no active smithing capture, so the
repair is not presented as a log reconstruction or proof that every missed
hammer strike had this cause. The existing LT assistance setting is preserved.

## Combat: one confirmed combo progression and one timing repair

The capture contains a damaging hit at 00:56:49.636 with `combo=1`, after the
observed candidate advanced to 2/3. It has `comboEnd=0`. That proves a
combo-marked hit/progression; it does not prove a completed finisher.

A later final-step attempt waits for contact but its input expires at 109 ms.
The same gesture's measured contact arrives at 140 ms. D82 separates that
already-captured final intent from the input transaction issued at contact.
The intent may wait only while the original native action/slot, candidate,
target, source generation and recovery epoch remain current, and never beyond
the existing 600 ms physical strike lifetime. Contact then starts one fixed
100 ms transaction. New gestures, cancellation and stale identity retire it;
later contact samples cannot extend it. Prepare/release each enter at most once.

Ordinary input expiry stays at 100 ms. This does not grant damage, invent native
combo acceptance or reinterpret a typed solver command. It is limited to the
qualified existing tracked-ray continuation path.

The recorded native outcomes include health damage, stamina-only damage, perfect
blocks and parries. They are not a count of all attempted human swings. No
sensitivity setting is changed on the assumption that every non-damaging event
was a missed hit. See [smithing and combat evidence](SMITH_COMBAT_D82.md).

## DLSS and FSR: separate the vendor histories for both eyes

The earlier doubled-image incident was consistent with selecting DLSS through
the launcher while the integration still shared the vendor history. Native TAA's
per-eye support did not create separate DLSS/FSR instances. Sequential rendering
of two eyes is compatible with temporal upscaling when each view has its own
instance and correct current inputs.

D82 intercepts the game's actual public NGX and FFX API calls. At a native
creation boundary it creates an independent right-eye instance using the same
live creation parameters. The original instance remains the left-eye instance.
Evaluation follows the current verified native eye. The game still supplies
color, depth, motion vectors, jitter, output resources and quality settings.

A temporal pair is accepted only after both distinct vendor instances have
successfully evaluated the same native epoch/frame. Histories reset together
after gaps, menu use, pipeline changes or a missed/refused eye. A technology
change cannot relabel an incomplete temporal pair as Native. It ends the current
run through the existing stop/rearm path and captures the new choice in a new
epoch. Native selection and camera/projection algorithms remain available.

The temporal publication path additionally identifies the actual right-eye final
composition. An earlier target barrier must not consume the one right-eye copy
or certify unfinished output. See [the upscaler report](TEMPORAL_STEREO_D82.md)
for lifetime, queue ordering, dependency and validation details.

The startup and VR Graphics choices are now Native, DLSS or FSR for Stereo.
The new temporal choices are labelled as D82 candidates requiring a headset
test. XeSS remains unavailable in Stereo. Graphics saves take effect after a
full restart, and no upscaler is silently enabled by applying this patch.

## Scope retained

The patch retains the current runtime INI and calibration, working bow alignment,
accepted hand-pose timing, native TAA/history algorithms, camera/IPD/FOV math,
OpenXR submission, improved alchemy/collision code and current companion assets.
It introduces no new runtime log destination. Relevant status goes to the
existing `KCD2VR.log`; test evidence in the patch is not a runtime recorder.

Throwing knives remain retired. This revision does not claim to finish corpse
dragging, general NPC physics, arbitrary object pickup, map performance, mounted
rein interaction or every combat finisher. Those older limitations are kept
separate from the specific failures repaired here.

## Build and review

Apply the accompanying source patch to the supplied D81R1 source or the D81R2
source derived from it, then run `build_release.bat` with the normal Windows
toolchain. The patch includes an explicit preimage check and source backup; it
does not install itself into the game. Keep the normal matching DLL/companion
build workflow. The optional gore package is not added.

Read [D82 validation](VALIDATION_D82.md) for commands, assertions, negative
controls and the exact boundary between host verification and an in-game result.
