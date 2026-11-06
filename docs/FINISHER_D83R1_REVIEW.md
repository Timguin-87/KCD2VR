# D83R1: head turns during native finishers

## Decision

Keep Claude's useful finisher feature, with corrected ownership and restoration guards. It lets Henry's head, neck and part of the chest turn with the headset during qualifying paired combat animations. The menu now calls it **NATIVE + HEAD TURNS**.

This is an orientation overlay. It does not add tracked head translation, tracked arms or hand control during the paired move. The native animation still controls the arms, hands, weapons, pelvis, legs and pair alignment. The implementation changes the pose consumed by the existing skinning call, then restores the exact native joint values. It does not replace the combat event or hit result paths.

Both existing D83 preferences retain Claude's ON default when their keys are absent:

| Preference | Menu location | Meaning |
|---|---|---|
| `VrFinisherUpperBodyPlayerD83` | Experiments → Combat, page 2 → Your finishers / masterstrikes | Head turns while Henry performs a qualifying paired move |
| `VrFinisherUpperBodyNpcD83` | Experiments → Combat, page 2 → NPC finishers / combos on you | Head turns while Henry receives a qualifying nonfatal paired move |

Saved OFF values are preserved. This patch does not rewrite the user's INI or calibration. Clinches, unrelated protected actions, mixed player/victim ownership and Henry's death remain native.

## What was repaired

The original D83 idea was usable, but its safety checks were weaker than those around the existing VRIK pose writer. These are concrete source differences, reproduced against the unchanged Claude implementation by the same host fixture:

| Problem in supplied D83 | D83R1 behavior |
|---|---|
| A tracking-source reset could keep the pose epoch unchanged and reuse an old headset reference. | The reference carries the tracking source, original pose epoch and pose-policy generation. A mismatch refuses the overlay. |
| The last pose could be stamped with current generations after the transaction that produced it had already become obsolete. | Latching happens after successful publication and uses that transaction's original generations and the original sample time. |
| Current menu, dialogue, cutscene, scripted state and pose permission were not checked. | The overlay checks the existing context publications and snapshot permission, then checks the same original generations again before and after pose writes. |
| A failed native setter could have changed memory before reporting failure. | Every joint is marked attempted before the call. Rollback restores all attempted joints, including the failed one. |
| A failed restore only disabled the finisher feature. Other pose consumers could keep using a contaminated pose. | A failed restore also raises the shared pose restore fault and invalidates the committed pose publication. |
| The temporary write had no destructor cleanup on a C++ exception leaving the scope. | A scoped, noncopyable write owner restores on normal completion and C++ unwind. This does not claim arbitrary Windows SEH recovery. |
| The mask included every non-arm descendant of the upper spine. Back weapon and quiver sockets can be siblings of the anatomical chain. | Only the connecting spine2 → spine3 → neck → head paths and neck/head descendants are selected. Known arm/hand/item branches and off-chain torso sockets remain native. |
| A second clip, character or source could inherit the previous blend. OFF followed by ON could also reuse a fully weighted correction. | Those changes restart the blend. An unavailable context clears the correction instead of retaining it for a later owner. |

The existing callback already holds the coherent-pose mutex, restricts the operation to Henry's current character and prevents recursive pose writes. The overlay is admitted only when ordinary committed VRIK, the crossbow reload passenger and the paired-hands owner are not taking the same pose. These existing ownership boundaries are retained.

## Rotation and visible scope

The code uses the same XR-to-game quaternion conversion as ordinary pose tracking. It remembers the headset orientation and calibrated head orientation from a successfully published VRIK pose. A subsequent headset rotation produces a correction relative to the current native head pose.

The correction is limited to 45° of yaw and 30° of the remaining swing rotation. Its cumulative shares are 10% at spine2, 25% at spine3, 60% at the neck and 100% at the head. These are shares of the bounded correction, not percentages of full tracked body control. The reference is limited to 30 seconds; current tracking samples must be no older than 100 ms. The blend approaches its target over 150 ms and fades toward the native pose when current head tracking is unavailable.

Native arms remain at their original model-space transforms while part of the chest turns. Some shoulder or neck mesh stretching is therefore still possible. Whether the chosen limits look good for every native paired animation remains a headset check. Camera placement and full head translation are outside this feature's implementation, so the documentation does not promise that they have been repaired by it.

## Verification

`tools/check_finisher_upper_d83.py` includes the actual production header and implementation. It extracts the production quaternion and pose conversion functions without changing them. Native memory access, the clock, diagnostic output and context publications are controlled leaves; the skeleton used by the fixture is synthetic and deliberately includes an off-chain torso attachment.

| Run | Outcome |
|---|---|
| D83R1, debug | 42 / 42 cases pass |
| D83R1, optimized | 42 / 42 cases pass |
| D83R1, AddressSanitizer + UndefinedBehaviorSanitizer | 42 / 42 cases pass |
| Unmodified supplied Claude D83, same fixture | Compiles; 10 pass and 32 fail |

The failing original cases include source reset, stale publication, current context blocking, a generation change during writes, a failed restore, C++ scope unwind, clip/character blend reuse and the off-chain torso attachment. This is a negative control showing that the new checks execute and distinguish the old implementation, rather than a set of tests that pass either version.

`tools/check_finisher_settings_d83.py` executes the two actual menu reducers and eight literal production read/apply/save/load statements. Its 13 checks pass, including independent switches and preservation of saved OFF values. The complete portable menu reducer test also passes: 111 controls, including the two new rows, 109 persistent controls, two session controls and four read-only rows.

The older `tools/run_live_runtime_d40.py` is not a whole-runtime validation result for this patch. It fails against current dependencies because its host is incomplete and still requests C++17 despite newer C++20 expressions. The finisher settings check above exercises the added wiring directly without pretending to repair that unrelated historical fixture.

Reproduce from the source root:

```text
python tools/check_finisher_upper_d83.py --out ../finisher-tests
python tools/check_finisher_settings_d83.py --out ../finisher-settings
```

To repeat the negative control, add `--original` pointing to the full D81R1 tree with the supplied Claude D83 overlay applied. The result files record hashes of the actual production files and extracted helpers. The independent crossbow-lane review checked those hashes, callback serialization, the narrowed joint mask, attempted-write rollback and shared fault propagation; it found no remaining concrete ownership or rollback blocker in this bounded feature.

These results do not establish a Windows DLL build, native setter ABI execution, correct in-headset stereo, appearance or frame time. They establish the tested source-level ownership, math and restoration behavior. No new standalone runtime log file is created; the existing diagnostic sink is used.

## Source and evidence

Production: `src/vrik/finisher_upper_d83.h`, `src/vrik/finisher_upper_d83.inl`, `src/frik_pose_probe.cpp`, `src/animation_filter.cpp/.h`, and the two existing experimental settings contributions. `mod.cpp` needs only the new finisher header include for those runtime owners.

Evidence accompanying the patch: `finisher/tests/RESULT.json`, the per-mode `run.txt` files, `finisher/settings/RESULT.json`, `finisher/menu_result.json`, and `finisher/manifest.json`. These are host/source receipts, not runtime headset recordings.
