# R96R1: additional recovery and pending impale repairs

Date: 2026-11-04. Build marker: `KCD2VR-R96R1-RECOVERY-IMPALE-20261104`.

This complete source revision includes R96 and repairs four further failure paths found while continuing the review. Its baseline is local source commit `b2264262e5bbdbcaae362b69ae0cd4a84796f606`. The previously delivered R96 archive remains a separate revision.

The new work concerns native pickup speed, door readiness after a Lua VM change, exact pose restoration before buffer rotation, and two ways of losing a pending impale request. The focused host tests execute production functions or literal production boundaries with declared native leaves. There is no new Windows DLL, retail game run or headset result in this delivery.

## 1. Native pickup speed must recover before release can finish

The quarantine path required the native hand to stop holding an item before it restored the mod's saved animation speed. A real native Attach could pause the current Picking action; a later missing item-link observation could put that pickup into quarantine. The paused action could then be the reason the hand never reached its release. This was a circular wait in the actual UpdateDetour path.

R96R1 settles the mod's speed write independently when the exact native action lifetime is still qualified. It still requires the current Henry controller, root scope, action token and character, readable finite speed, a value matching the mod's own write, and readback of the restored value. An active, unobserved or mismatched native outer callback remains insufficient authority. A positive completed native cleanup retires the old speed lease without reading the destroyed action.

If another writer has changed the speed, the mod retires its speed lease. It must not later reclaim an equal-valued write from that other owner. Unreadable speed or a failed write remains unresolved.

The hand/item obligation is separate. Restoring speed does not clear the held item, replay Drop, force a native cleanup, or admit another item owner. The quarantine clears only after the current native hand is observed to have released the item and the speed obligation is settled. The regression reaches the failure through actual Attach and UpdateDetour calls and continues through real release observation, a fresh Attach and one Drop without resetting mod state.

Primary implementation: `src/native_grasp59c13.inl`.

## 2. A pending door press needs readiness from the current Lua VM

H40 remembered which hand tokens had already probed script readiness. A VM change cleared the overall ready flag but retained those probe tokens. A press that had been reserved, successfully probed, and not yet entered a native action could therefore skip readiness in a replacement VM. If that VM had not installed the callback, a feed attempt created uncertain entered state. A subsequent VM change then quarantined that state.

The fix clears only the cached readiness probe tokens when the VM generation changes. The original press token and timestamp remain unchanged. A replacement VM must supply its own successful probe and protocol reply before a feed can enter. A failed probe remains unentered; it does not become invented native ownership. The same still-fresh press can recover in a later qualified VM, while an expired or retired press still requires a new squeeze.

This applies equally to the left and right hands. Same-VM readiness is not polled every frame. A door action that really entered the previous VM, or an outstanding detached rest operation, retains its existing ownership obligation; a new VM cannot acknowledge the old native operation.

The actual H40 header passes 52 checks in each of the unoptimized and optimized builds. The retained R96 admission/currentness/service regression passes 28 checks. The exact R96 header compiles with the new test and fails 12 assertions, reproducing the defect on both hands.

Primary implementation: `src/subsystems/doors/hand_only_h40.h`.

## 3. Observe exact pose restoration before the buffer rotates

A native setter may apply the original joint value and then report failure. The existing transaction correctly kept its restore fault and rejected publication. R96 checked recovery at entry to a later callback, however. If native skinning rotated to a different pose buffer first, the restored original buffer might never be observed again and ordinary VRIK could remain disabled.

R96R1 adds a readback-only observation after Restore, committed-pose rejection and the final palette/finger consumers. It qualifies the current descriptor, player and skinning character, skeleton owner, joint count, player lifetime and pinned pose getter before and after copying the native values. It accepts only the exact indebted buffer and exact saved originals. It neither writes another joint nor uses a replacement buffer or an observed load to forgive the old buffer at this boundary.

The callback that experienced the restore failure stays invalid and unpublished. Only future pose work can reopen. Missing or different originals, an unreadable buffer, changed identity, a changed getter or a changed descriptor retain the fault. Existing R96 entry-time recovery and its separately qualified save/load restoration path remain in place.

Primary implementation: `src/vrik/coherent_pose.inl` and the final recovery call in `src/frik_pose_probe.cpp`.

## 4. Preserve a qualified pending impale request through publication timing

Two failures were reproduced in the actual native impale coordinator.

First, its entry clock was sampled before owner observation and the completed physics-frame copy. A newly published lethal request could pass a later currentness check but fail the original ragdoll deadline check as a clock rewind. Its serial had already been consumed. R96R1 refreshes the validation time after copying the publication.

Second, a nonzero ready-sample/mailbox mismatch immediately discarded a pending victim wait while retaining the consumed serial. The same live weapon request could not return. R96R1 permits only an already admitted, unanchored, never-pinned victim wait to survive that publication gap. The immutable request serial, target, hand, weapon identity, ownership epochs, tracking, context, grip, scale and withdrawal checks still have to agree. Both ready observations must remain nonzero and forward. A zero ready marker is not treated as a harmless publication gap.

The original native victim deadline is not extended, the pending timestamp is not renewed, and the incomplete publication grants no body query, first pin, constraint or velocity drive. The later completed frame must qualify normally before acquisition. An active anchor or a previously published first pin cannot use this pending-only path. Context loss, actual weapon release, source replacement, tracking loss, blade withdrawal and expiry still retire the request.

The earlier R96 fix to the separate weapon-side pending/rejoin limit remains included. That 4,700 ms weapon allowance and the original native victim wait are distinct boundaries; R96R1 does not change either duration.

Primary implementation: `src/subsystems/gore/native_impale_part.inl`.

## Retained R96 behavior

R96's detailed review and validation receipts are retained in `R96_REPAIR_REVIEW.md` and `R96_VALIDATION.json`. They describe the tests run for that earlier revision, not fresh executions of every suite against R96R1.

- Mounting and dismounting use native body animation. A positive seated pose and one-second settling interval lead into full upper-body VRIK. Each mounted visit gets a fresh seat foundation; native gait sway does not shift the common headset/controller reference.
- Physical crossbow reload for light, medium and heavy classes remains independent of the Instant master. Manual string/tool handling and native right-hand bolt placement are the intended normal flow. A failed requested native attachment join has a bounded recovery path that restores the still-current native action rate instead of freezing it indefinitely.
- The bound spanner/lever transition, native spanner grip scale, first bolt release, heavy-tool publication gap and primary placement clock repairs remain included.
- Corpse dragging can use either hand. Load-scoped fault recovery and fresh input qualification remain included, together with the interaction, mortar, shared script transport and native weapon retirement fixes.
- Installed INI values and calibration remain authoritative. R96R1 does not change their settings or add a new gameplay switch.

## Remaining limits and specifically unresolved cases

**Killing-blow contact selection remains a separate issue.** The historical lethal-thrust log selected Blade and was refused because a piercing tip was not proven; its reported contact energy was also below the configured threshold. The sparse sweep can select a zero safe-stop blade hit before querying the tip. A focused reproduction confirms that a nearer tip can be missed in one arrangement, but the reverse arrangement disproves simply preferring Tip. Its stored fraction is a margin-adjusted movement stop rather than raw collision time. This revision does not relabel blade contact or lower the impale threshold. The new fixes prevent loss of an already qualified pending request; they do not establish that every lethal thrust should become an impale.

**Uncertain native obligations remain real.** The pickup repair does not discard an unreadable held-item lifetime. A door action actually entered in an old VM still needs its native completion proof. A partially retired weapon geometry whose native unregister call entered and returned an uncertain failure cannot safely be retried merely because the world-reset counter changed. The API currently does not distinguish a refusal with no side effect from an exception after a side effect. These cases are not declared fixed by the generic load recovery.

**Runtime appearance is unverified.** No current game/HMD run establishes the final horse posture, crossbow flicker or bolt/spanner visibility, all corpse motion outcomes, every holster context, or black-frame behavior. The supplied recordings identify R94R1, not R96 or R96R1. No renderer fallback or arbitrary static-scenery conversion is introduced here. Native inventory eligibility remains required.

## Build and installation

Extract the complete `KCD2VR_SOURCE` directory into a new directory. On Windows, use Visual Studio 2022 C++ desktop tools, CMake and Python 3, then run `build_release.bat`. Pinned dependency acquisition and an optional existing OpenXR SDK tree are documented in `BUILDING.md` and `DEPENDENCIES.md`.

Build output goes to `dist/prerelease`; building does not install it. Use the resulting binary with its matching generated main companion PAK. Optional gore assets remain a separate package. Preserve the installed INI and calibration.

The first new run should identify `KCD2VR-R96R1-RECOVERY-IMPALE-20261104` in the log. The archive's `EXPORT_MANIFEST.json` records the exact source commit and SHA-256 for every exported file.

## Validation receipts

`R96R1_VALIDATION.json` contains this follow-up's commands, source hashes, positive runs, genuine R96 comparisons, declared native leaves and retained failures. It also identifies the earlier R96 evidence as inherited provenance. Compiler errors or arbitrary crashes are never counted as successful regression negatives. A host adapter result does not claim real Windows SEH, complete Windows translation-unit linking, native engine scheduling, retail Lua behavior or headset visibility.

| Area | New positive coverage | Retained coverage | Genuine R96 comparison |
| --- | --- | --- | --- |
| Native grasp | 193 checks in each O0/O2 run | Verbatim C25 cleanup/quarantine block | Compiles, then fails the exact speed-before-release assertion |
| Doors | 52 checks in each O0/O2 run | 28 R96 admission/currentness/service checks | Compiles, then fails 12 assertions across both hands |
| Pose restoration | 28 production-boundary cases | 27 R96 entry/load recovery cases | Compiles; exactly three new recovery cases fail and 25 pass |
| Native pending impale | 164 checks in each Debug/O2/ASan+UBSan run | Existing H18 native coordinator | Compiles, then fails the two exact publication/clock assertions |

The grasp suite also rejects three controlled production mutations: reclaiming a foreign speed write, bypassing native outer lifetime, and clearing item ownership before real release. Repeated optimization, sanitizer and portability runs repeat the same cases; their counts are not distinct gameplay scenarios. ASan/UBSan passed with leak detection disabled for the managed ptrace environment; no LeakSanitizer success is claimed.

The older omnibus native-grasp fixture still fails its unrelated supply-readiness assertion before reaching the pickup cases. The same failure was reproduced on original R96. Its explicit external `Context::Current` leaf reports false where that older scenario expects true. The failed result is retained, and the relevant C25 block is executed unchanged separately. The complete old fixture is not reported as passing. The full coherent-solver fixture and Windows build also remain outside the new focused tests.

To rerun the current host checks from the exported archive with Python 3 and GCC, use separate output directories:

```sh
python3 tools/check_door_vm_probe_r96r1.py --out ../r96r1-doors
python3 tools/check_native_grasp_speed_r96r1.py --out ../r96r1-grasp
python3 tools/check_pose_restore_post_r96r1.py --skip-baseline --output ../r96r1-pose
python3 tools/check_impale_native_pending_r96r1.py --out ../r96r1-impale
```

These commands do not require private Git history. The supplied validation receipts include the original-source comparisons performed during this repair; reproducing those comparisons requires the original R96 source or its Git history, as described by each runner's `--help`.
