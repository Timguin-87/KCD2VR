# D84: repairs from the latest 50 diagnostic files

**Source revision:** `KCD2VR-D84-REGRESSION-REPAIR-20261102`  
**Review date:** 2 November 2026  
**Starting point:** reviewed D83R1, including its retained D81/D82 repairs.

D84 fixes demonstrated failures in pickup feedback, smithing stroke ownership, native kick preparation, dodge input delivery, physical-walking selection, corpse service and HUD routing. It also closes specific recording gaps that hid the actual native result. This is a cumulative source patch. It contains no compiled mod or new gameplay PAK.

The source and portable production-connected tests are the completed result. A Windows/MSVC build and a game/headset run have not been performed here. Physical corpse movement, every crossbow variant, audible alchemy feedback, universal collision reliability and blade-driven paired finishers are not established runtime results.

## Evidence used

All 90 retrieved diagnostic candidates were available. SHA-256 comparison found 69 distinct complete contents; the newest 50 were selected using their recorded timestamp edges, not upload filenames. Every selected file was scanned in full: **18,203,035 lines**, spanning recorded times from **1 October to 1 November 2026**. ZIP members were decompressed and checked. Same-byte aliases count once; distinct files can still overlap or belong to the same play session.

`evidence/log_corpus/INDEX.json` records the selection and hashes. `SUMMARY.json` records complete line counts and discovery categories. Keyword counts are not gameplay success rates. The newest selected diagnostic archive explicitly identifies D80. A separate newer unified log, `KCD2VR(20261102-000003).log`, identifies D81R1 and remains part of the retained D83 review evidence. Its SHA-256 is `a644f4d0893b855a0962fd11805eeb7acc9da61305cc1cc706d99562b658d938`.

Historical source and handoffs were compared, including D43, D46, D63R2, D72, D76, D77 and D79, the supplied Claude overlay, and complete D81R1/D81R2/D82/D83R1 trees. An archive's historical nickname is not automatically its runtime build marker. In particular, the 1 November 16:53 recording is historically labelled D79 but has no detected build marker in the canonical index. Native layout findings are qualified against the supplied `WHGame(8).dll`, SHA-256 `bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7`.

## What changes for each requested area

| Area | D84 result | Remaining boundary |
| --- | --- | --- |
| Both-hand pickup, chest storage, release | Confirmation now follows verified native storage on the acquiring hand in both instant and fallback paths. Staged chest intent follows the requested wrist before collision correction. | Either hand can acquire sequentially; there is one ordinary prop owner. A refused instant pickup can still use an authored native fallback. Two unrelated props simultaneously and arbitrary scenery are not implemented. |
| Collisions | Retains mandatory sweeps, bounded station correction and requested/displayed wrist separation; F9 now reaches the identified contact and native-held recorders independently. | Sparse palm/finger probes are not complete articulated hand-mesh physics. No claim of flicker-free behavior in every mode without a new runtime result. |
| Alchemy | Retains the repaired pre-Update native action clock and take/return semantics. F5 reports requested versus actually applied presentation speed. | Sound/particles need actual playback. Mortar and pestle have separate native attachments within one action lifecycle, not unrestricted independent world grabs. |
| Smithing | A native phase-14 acknowledgement arriving with the same owned downward release now retains that stroke's fall receipt. Existing phase-15 continuation, moving rearm and LT native workpiece positioning remain. | Native impact/quality acceptance still needs a current in-game result. No hit or quality is fabricated. |
| Combat and combos | Repairs the native paired-action callback wrapper join and adds precise F3 refusal evidence. Existing contact-gated final-combo requests remain. | Pair identity does not give the VR blade authority over native paired damage. Unsupported paired acceleration remains disabled; full edge-driven masterstrikes/finishers remain open. |
| R3 kicks | Armed free-space preparation waits for actual native FreeGuard within the existing 120 ms request. Modifier maximum alone no longer counts as that acknowledgement. | Native legality still decides whether a kick is accepted. Engaged, protected-action and every weapon case are not certified. |
| Small objects | Preserves the qualified inventory-item preparation and either-hand pickup path. | Decorative cups/plates/tankards without the required inventory/entity/body contract need a separate general prop owner. |
| Dead NPCs | Removes diagnostic/lab/physical-combat dependencies, fixes the outer service path and preserves the chosen corpse setting on Save/load. F9 exposes exact refusal stages. | Exact dead/body/thread/hand ownership remains required. Provider READY is not measured movement; the current whole-body servo is not a proven limb constraint. |
| Immersive HUD | **Left open palm: compass. Right open palm: health/vitals/buffs.** Semantic visibility and actual FlashUI alpha/visibility consumers agree. | A new game run is still required for visual acceptance. |
| Free targeting | Confirms that the existing native no-turn hook actually receives calls and vetoes in historical captures; preserves its ownership checks. | Paired/native camera ownership is distinct from blade contact. No unsupported blanket removal of native constraints. |
| Physical walking and dodges | Explicit walking controls now select finite D76 displacement; F1 no longer silently enables the legacy servo. Native pad dispatch consumes one coherent axes/buttons snapshot. | Existing explicit OFF remains OFF. Head-only tracking cannot perfectly distinguish a level lean from a step. Native collision and combat acceptance still apply. |
| Dice | Adds a qualified, read-only completed-frame F5 snapshot of native action map and cursor state; defines the real cup/selection work still needed. | `dice_hold` toggles keeping a die. It is not cup grip. Physical cup shake/place and visible-die-to-native-index ownership remain unqualified. |
| Physical crossbow reload | Retains the continuous hand/string/tool/bolt state machine and D82/D83 ownership repairs. F5/F7 now receive the same native owner/clock/binding evidence previously restricted to F11. | Existing logs predate the current source. Native tool/bolt binding and full completion for each asset/class need a new result; no gesture-triggered animation is labelled physical success. |

## Why these repairs address different failures

### Chest storage and collisions

The newest D80 diagnostic recording refuses the instant path for target 525213 as an unverified rigid body, then reports successful native storage of the same target. Fixing only the instant path would leave that fallback without feedback. D84 uses each path's actual success contract: inventory readback for the staged owner, and the checked original transfer result plus same-item hand release for the native attachment owner. Failure, cancellation, loose drop and repeated callbacks do not pulse. Haptic dispatch works with F5 off and respects the saved preference and XR availability.

The staged chest gesture uses completed requested hand intent. A collision-corrected display wrist stopped at the torso must not prevent the gesture, and moving only that displayed wrist must not create it. The retained native attachment owner's gesture source is unchanged. Ordinary collision geometry, mandatory query priority, fixed arm lengths and the accepted smoothing foundation are preserved.

### Alchemy and smithing

The current alchemy presentation clock has an 8x requested maximum and an 80 ms authored-step bound. At a 33 ms native frame, the applied rate is about 2.424x. D84 records both values. Physical grinding, bellows and resource work keep their own owner; a general speed multiplier must not advance native work beyond what the hands earned. Semantic transfer, attachment readback, parent retirement, cue dispatch and audible output remain separate observations.

Mortar history contains an important trap: old versions waited for the child to disappear before invoking the Stop operation that retires that child. Restoring those old files would restore the deadlock. Current ready/Stop/contents/return ownership is retained. Mortar and pestle are two native attachments within that lifecycle.

Smithing had another transition error. A legitimate native charge acknowledgement can arrive in the same input packet as the downward release. The old fall tracker required RT still held, so it discarded that owned stroke. D84 keeps the receipt for that exact acknowledgement/release, while refusing a release before acknowledgement. The historical native `StrokeHit` callbacks establish native evaluation, not the physical hammer contact or forging quality. Their continued appearance after reheating rules out the simple claim that the evaluator always stops completely on returning to the anvil.

### Kicks, combos and paired damage

The actual native free-kick selector distinguishes current guard from block modifier maximum. D84 waits for FreeGuard on the same actor and original request; it does not press repeatedly, extend the deadline, write a guessed guard field, or replay after temporary unreadability. Temporary unreadability retains only owned modifier cleanup debt.

The paired reader omitted a genuine heap callback wrapper. D84 follows and validates that wrapper, its inner callback and reverse fragment link before the existing reciprocal owner/lifetime checks. The recorded `pair=0` failures now have a concrete source explanation. Earlier attacker-only speed writes with no pair are a different already-quarantined historical error.

Correctly identifying both actions still does not make their native damage event follow the user's edge. D84 therefore keeps native paired timing and reports `PAIR_DAMAGE_CONTACT_UNPROVEN`. Enabling the old 64x path after fixing the pointer chain would revive unsupported damage timing. Ordinary native recipe order and the separate contact-gated final-combo request are preserved.

### Motion and corpse service

OpenXR LOCAL-space positions use +X right, +Y up and -Z forward. The retained finite motion path maps horizontal deltas through the calibrated tracking/body frame, submits each distinct tracked displacement once and removes the camera's consumed raw displacement only after native adoption. It does not keep pushing a stick while the HMD remains offset. D84 fixes which existing path the menu actually selects and the F1 side effect; it does not invent a timed movement impulse.

A separate input race could combine old forward axes with a new side/back dodge button. One mailbox snapshot now supplies the entire native pad dispatch. The no-forward HMD recognizer and the native combat eligibility rules remain separate.

Corpse gameplay had several unwanted dependencies, including an outer early return whenever physical combat was off and a setting forced false during Save/load. Those are repaired. Both native death/identity qualification and clean ownership release remain mandatory. The selected corpus contains provider installation and refusals, but no saved-provider GRAB/DRIVE chain. A qualified API call still needs a measured body movement result before dragging is called working.

## Diagnostics and performance

All new records feed the existing **KCD2VR.log**. No new finger, pre-stowed-bow, map-performance, hook-library or companion-startup sidecar is introduced. Existing files on disk are not deleted by a source patch.

| Key | Current purpose |
| --- | --- |
| F2 | Bugs and lifecycle evidence; existing explicitly named comparison rungs remain. |
| Shift+F2 | Performance capture. |
| F3 | Combat, kick admission, combo and paired-action evidence. |
| F5 | Pickups, storage, alchemy, smithing, dice and physical reload evidence. |
| F7 | VRIK/native pose and physical reload ownership; existing pose-comparison rungs remain. |
| F9 | Independent finite physics recorder; does not enable a gameplay physics experiment. |
| Shift+F9 | Read-only upstream physics detail ladder under the Physics recorder. |
| F11 | Compatible reload recorder and its existing staged comparison. |

The finite recorder lease is 180 seconds. Already captured rows and terminal receipts may drain for up to five seconds under their original session; this does not permit new observations after stop. Independent keys can coexist. Restarting one key cannot relabel its old queued records. Shared reload and passive-contact observations select one originating recorder and carry the active mask instead of duplicating native reads for every selected key.

Map opening no longer starts automatic profiling. The map timing worker, scope clocks and copied counter snapshots are guarded by F2 or Shift+F2. Capture lease time and duration-measurement time use their appropriate clock domains; a restarted capture cannot accept the old map interval. Optional host/profile physics reads and formatting are similarly gated without suspending gameplay retirement, exhaustion state or failure-status publication.

This removes identified recording work outside a requested capture. It is not a measured recovery from 90 to 110 FPS. The user's H50/D52 recordings were the same scenes; the accepted Fresh Pose smoothing change is not being treated as the established cause of that regression. The renderer and current DLSS/FSR implementation are preserved from D83R1. Separate per-eye contexts are implemented there, but current GPU quality/performance and headset fusion are still unverified. XeSS Stereo remains unsupported.

## Applying the cumulative patch

Use the provided installer against a complete supported source directory: original D81R1, D81R2, reviewed D82, either supported Claude overlay on its complete base, or reviewed D83R1. It validates all affected preimages before writing, accepts LF/CRLF-only differences, retains exact backups and rolls back failed writes. It preserves runtime INI and calibration. It does not validate unrelated custom source as the exact reviewed whole tree.

From the extracted patch directory:

```bat
python apply_patch.py "E:\kcd2vr-build\KCD2VR_SOURCE" --check
python apply_patch.py "E:\kcd2vr-build\KCD2VR_SOURCE"
```

Then run `build_release.bat` in the patched source with the normal Windows toolchain. This source root is not the game directory. Build the matching DLL and main companion together; the optional gore payload is excluded. Do not apply an older overlay on top of D84.

For finite physical walking, select **Physical walking ON** once if the saved preference is OFF. For corpse grabbing, select the existing corpse option once if an older build saved it OFF. D84 preserves explicit preferences; it cannot infer that a stored false was intended true. F9 is only the recorder.

Read [validation](VALIDATION_D84.md), [regression ledger](REGRESSION_LEDGER_D84.md), [interactions/alchemy/dice](INTERACTIONS_D84_REVIEW.md), [combat/smithing/kicks](COMBAT_SMITH_KICKS_D84_REVIEW.md), and [motion/physics/crossbows](MOTION_PHYSICS_D84_REVIEW.md) for exact source and evidence boundaries.
