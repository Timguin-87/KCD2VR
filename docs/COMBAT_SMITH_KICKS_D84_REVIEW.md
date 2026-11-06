# D84: smithing, native kicks and paired combat

This review found three concrete defects in the retained source: a smithing acknowledgement was lost when it arrived together with the player's release; a free-space kick treated the native block modifier as acknowledgement of a different guard state; and the masterstrike/combo pair reader skipped a real native callback wrapper. The source repairs and independent negative controls are described below.

**This is not a claim that all smithing strokes, all armed kicks, or blade-driven masterstrike damage now work in the headset.** The first two changes repair specific ownership/timing failures. The third establishes the correct relationship between a native attacker and victim. That relationship alone does not authorize an enemy to take damage before the tracked blade arrives. D84 therefore removes the unsupported automatic 64× paired acceleration and preserves native progression at 1× while the damage/contact bridge remains unfinished.

## What was reviewed

The parent audit selected the latest 50 recordings with distinct full-content SHA-256 hashes, ordered by observed edge timestamps. They span October 1–November 1, 2026. Byte-identical aliases count once; these are 50 recordings, not necessarily 50 independent play sessions. All selected contents were available. The canonical provenance is `evidence/log_corpus/INDEX.json`; the source archive, original member, original line number and hash are retained.

This lane examined the kick, paired-combat and smithing extracts for all 50 recordings, followed the relevant original rows, and compared the retained D83R1 source with historical D63R2, D72 and D79 source and review notes. The newest D81 unified main log is supplementary; it does not contain an R3 transaction that validates the new free-space repair. None of the supplied recordings is a headset test of D84.

Native research used the independently supplied retail `WHGame(8).dll`, SHA-256 `bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7`, the native tables and the combat animation database. The tests distinguish copied retail instructions from controlled engine leaves. No game process, headset, native animation playback or native damage result was executed in this environment.

## Status of the repairs

| Area | Concrete D84 result | What remains open |
| --- | --- | --- |
| Smithing | Keep a valid fall receipt when native phase 14 and the physical release arrive in the same packet. Preserve D83R1 moving-hand rearm, phase-15 continuation and native LT workpiece alignment. | Every physical stroke registering, native quality/scoring, and real hammer-head contact at the displayed location need headset/native validation. |
| Free-space R3 | Wait for current native FreeGuard acknowledgement, without replaying the press or extending the original request lifetime. Clean up only the modifier owned by this request. | Whether each actual loadout reaches this guard within the existing deadline; actual native kick animation, visible leg placement and contact/damage. |
| Masterstrike/combo pair | Read the real nested completion callback and its exact semantic attacker/victim links. | Native authored damage and reaction timing following the real blade edge and struck body part. |
| Paired acceleration | Remove identity-only 64× acceleration. A newly proved pair keeps native 1× progression. | A current, exact native event/contact owner that could safely retime or replace the authored event. |
| Ordinary combo final request | Retain the existing actual-contact endpoint policy and native recipe order/eligibility. | Headset completion of individual recipes and native paired final reactions. This is a separate stage from pair identity. |

## 1. Smithing: the acknowledgement and release can be the same packet

The native-height gesture producer lifts through the existing native RT path. A legitimate stroke can have this order:

1. The player makes the qualifying upward motion and the mod requests the native lift.
2. The native phase first becomes 14 while the player has already begun the deliberate downward reversal.
3. The gesture producer observes both facts in the same input packet and releases RT.
4. The fall tracker used to require `held == true` to acknowledge phase 14. In this packet RT was already released, so it discarded the valid stroke's fall receipt.

The repair accepts phase 14 when either the producer is still holding its requested lift or that same owned producer is releasing it. It does not accept a release before native phase 14, borrow a later native fall, synthesize a hit, or write charge/quality. It also does not extend the original timing bounds.

The production change is deliberately small in `src/subsystems/smithing/fall_timing_d81.h`. `tests/smith_ack_release_d84_test.cpp` uses the actual native-height gesture producer. The same test passes 92 checks in D84 and fails against immutable D83R1 at the assertion that the same-packet native acknowledgement and physical reversal retain the native fall receipt.

The retained D83R1 repairs are still required. Phase 15 must not reset an owned released stroke into unrelated activity; a player who keeps moving at the anvil must not need an artificial stationary pause to rearm; and LT alignment must preserve the native workpiece movement used by the native hammer evaluator. These paths were not reverted or replaced with new hit thresholds. The full retained smith runner passes all 19 cases after this change, including repeated reheat, actual pad/controller transitions, native LT placement and recovery-lift lifetime.

### What the older logs actually show

Eleven of the selected recordings contain `SEMI_SMITH_INPUT` records. Across the selected set there are 374 native `StrokeHit` point callbacks. That is evidence of native callbacks, not 374 proven good physical strikes or scoring successes.

The newest D80 recording, `KCD2VR-diagnostics(8).zip`, member hash `84ddd2b408293d580c8939167200e4b97663a57c8f6f8730138d0e58f0a93fa0`, contains 45 native point callbacks. Forty-three have a correlated VR-stroke receipt and record 328–344 ms from input release to native point callback; two have no VR-stroke receipt. These intervals are **not** a measurement from actual hammer-head collision to accepted native damage/quality. The log explicitly says `contact3D=UNPROVEN`, `cursorInverse=UNPROVEN` and `acceptance=UNOBSERVED`.

The same D80 recording returns from native phase 6 to phase 11 at original line 252338, then records additional native callbacks from line 253586 onward. The return to the anvil did not abolish every native stroke. This does not contradict the user's report of poor response: gesture recognition, displayed hammer motion, native impact timing and scoring are different stages. It prevents another incorrect repair based on the assumption that the native evaluator stopped altogether.

An older D63R2 review also explains why `NO_PLACED_HAMMER` was not proof that the hammer was absent: the diagnostic producer required mode 1 while the consumer accepted the recorded mode 3. That diagnostic mismatch was already repaired. D84 does not reintroduce it or interpret an old missing join as a new gameplay failure.

The F5 output now includes the earned fall receipt, native phase, release tick, sample, freshness and whether LT native alignment is active. These fields are appended inside the existing finite F5 capture. They distinguish a missing physical release receipt from a later native consumer problem without creating another log file.

Evidence: `smith_corpus_findings.json`, `smith_D80_input.txt`, `smith_ack/RESULT.json`, and `smith_retained_final/RESULT.json` under `evidence/combat_smith_d84`.

## 2. R3: a modifier is not current FreeGuard acknowledgement

The exact native free-space kick row requires FreeGuard, native guard type 23. The block modifier's maximum at native state `+0x64D` is a different value. The original block press updates that modifier synchronously; it does not prove that the current guard-type property at state `+0x60` already returns 23.

This distinction was missing from the production dispatch. The old path pressed the modifier, observed its maximum above zero, immediately prepared/released the native kick input, and cleaned up. Its native selector fixture pre-filled the current guard with 23, so the fixture concealed the production timing gap.

The corrected native selector test makes the distinction explicit. With the same armed loadout, owned R3 request and modifier maximum 1, current guard 0 or 1 produces no free-kick row. Current guard 23 admits the real kick row. This test executes the reviewed native selector instructions; it does not substitute a C++ imitation of that selector.

### D84 dispatch behavior

`NativeFreeKickDispatchR24` sends at most one original modifier press. `NativeFreeKickContinueD84` reads the actual guard property through a validated native getter layout. If the current guard is not yet FreeGuard, the existing native input pump may continue that same request until its original 120 ms deadline. It does not issue another modifier press, retain a stale request indefinitely, or replay an expired R3 when a late acknowledgement arrives.

When FreeGuard is observed, the adapter uses the existing original native prepare/release path. The selected native row, native action-manager admission, animation, stamina/RPG refusal and eventual hit remain native. The repair does not write the current guard, weapon class, attack outcome, loaded state or native damage. It does not create a dummy enemy or start a stand-alone kick CAF.

The wait rechecks the player/actor/state, body epoch, loadout and generations, input ownership, tracking, activity context and original request identity. Manual guard or a later physical guard lease takes over its own cleanup; the old request never sends LT-up into that later owner.

Cleanup distinguishes a positive actor/state replacement from a temporary failed read. A temporary failure revokes the kick and retains only its owed modifier cleanup. If the same actor becomes readable again, cleanup may send the original LT-up, but the kick remains revoked. A replacement actor receives no old cleanup. A returned LT-up handler is not treated as native acknowledgement: the adapter waits for the native release state instead of replaying the release.

The F3 receipt records original request tick/age, current FreeGuard value, modifier maximum and whether both input boundaries were visited. The wait emits an initial wait edge and one terminal outcome, not a line every native frame. The terminal still says `selection=NATIVE_CONTEXTUAL hit=UNCLAIMED`.

### What this does and does not explain in the history

The newest D80 capture contains four `R3_NATIVE` rows with both input edges observed but `observedAttackType=-1`; they use the engaged `DOWN5_HEAVY` route. Their returned handlers are not proof of accepted kicks. They also do not directly exercise this newly repaired free-space guard wait.

Older captures contain actual native `kick_nwpn` clip observations. For example, the October 31 11:40:34 recording has native kick clip/output records around original lines 639086 and 639188, but the latter explicitly reports `requestAssociation=UNPROVEN`. This establishes that native kick animation was possible in an older run. It does not establish a universal R3-to-animation-to-landed-hit chain that can simply be restored by copying an old delay.

The historical long trigger pulses and old special-input attempts must not be promoted to proven native kick acceptance. The present change targets the independently demonstrated FreeGuard ordering error. Universal armed and targetless kicking remains subject to live native acceptance, protected actions and the actual guard transition reaching acknowledgement before expiry.

Validation: 89 native-selector checks; 61 checks executing the extracted production dispatch/wait/cleanup functions, also under ASAN/UBSAN; immutable D83R1 fails the same readiness case. Native readiness, allocation/RPG and surrounding engine leaves are explicitly controlled in these fixtures. Actual native kick animation and damage are not executed.

Evidence: `kick_log_summary.json`, `kick_native_fixture/native_guard_RESULT.json`, `kick_ack/current/RESULT.json`, `kick_ack/sanitize/RESULT.json`, and `kick_ack/d83r1_negative/RESULT.json`.

## 3. Masterstrikes and combo endings: the pair reader skipped a native wrapper

All 326 `MS_PAIR_D62C` records found across seven selected captures have `pair=0`. In the four captures with `instantD72=1`, all 112 such records have zero speed writes. A unique nearby queue candidate did not turn into a proved native pair.

There is a relevant historical exception to the speed-write statement: eight older D62C records report a speed write while still showing `pair=0`, covering two unpaired attack occurrences and their later state/restoration records. This is the attacker-only acceleration defect that the D63 ownership work quarantined. It must not be used as evidence that a complete attacker/victim pair once worked correctly.

The D72 and D79 archived source, and the retained D83R1 source, read this chain:

`fragment + 0x158 -> callback + 0x20 -> supposed semantic action`

The actual native construction at `90CEC8 -> 90CF5C -> 90D04C` stores an additional wrapper. Its vtable is `3E3FA18`. The correct chain is:

`fragment + 0x158 -> wrapper + 0x40 -> inner member callback + 0x20 -> semantic action`

The wrapper also carries `wrapper + 0x48 == fragment`, which D84 checks as a reverse ownership witness. The old reader inspected wrapper padding/inner-capture storage as if it were the action pointer. Its fixtures built a flattened callback containing the desired action directly at `+0x20`, so they passed while omitting the native layout that blocked production.

D84 fixes the chain, checks the exact wrapper identity and reverse fragment link, and retains the existing reciprocal attacker/victim links, semantic vtables, current action-manager membership, entity identity and fragment lifetime checks. A neighboring queue call still cannot grant authority.

The new independent fixture executes the actual retail wrapper constructor, both actual member-functor clone routines and native source-clear routine. The production reader consumes the resulting bytes. Only allocation and temporary functor destruction are controlled leaves. It covers inline and heap inner callbacks, both semantic families, reverse-link mismatch, a foreign inner owner and the old flattened layout. D84 passes 64/64. The same fixture compiles against immutable D83R1 and fails all four legitimate pair-read cases, with eight failed assertions. This is a demonstrated production-layout defect, not a test-only policy adjustment.

F3 now reports the first relationship-read stage, address/value/expected value and readability. It also states `damageTiming=NATIVE_1X contactEventAuthority=UNPROVEN`. Diagnostics can therefore separate a remaining native pair refusal from the separate absent contact/event owner.

### Why the repaired reader does not enable the old 64× path

The old active policy started 64× animation progression as soon as both native endpoints were proved current roots. It did not require a current blade contact. Fixing the pointer chain and allowing that policy to run would make authored damage advance rapidly even when the real blade had not reached the enemy.

D84 deliberately removes that unsupported acceleration. Pair identity alone moves the policy to a terminal `PAIR_DAMAGE_CONTACT_UNPROVEN` state, without granting a speed lease. Native animations, recipe progression and authored effects continue at their normal rate. Existing possible lease cleanup remains intact. The instant-combat availability state reports unsupported for this missing event/contact capability; there is no new menu switch that pretends to supply it.

This is a necessary behavioral decision, not a completed blade-driven masterstrike implementation. The remaining bridge must identify the exact admitted pair occurrence and the particular native animation/procedural damage event, join it to the current tracked weapon/gesture/contact and body part, and establish what native cancellation, deferral or replacement is legal. The old entity/tick ring carries none of that ownership. Suppressing arbitrary native hit calls or advancing a combo cursor would risk missed damage, duplicate damage or broken game progression. Neither was added here.

The existing actual-contact final-combo request remains separate and unchanged. For its supported ordinary weapon endpoints, it accepts blade/tip slash or stab at the required region while retaining the selected recipe, prefix order, current target and native eligibility. Explicit pommel/handle/shield/fist requirements still use their own producer. This does not turn the subsequent paired native ending into contact-driven damage. The retained 66-check endpoint/impact-geometry fixture verifies that distinction, not headset recipe completion.

Evidence: `paired_join_gap.json`, `historical_source_observations.json`, `native_callback/RESULT.json`, `native_pair_static.json`, and `pair_retained/RESULT.json`.

## Diagnostic ownership and validation

F3 owns kick and paired-combat detail; F5 owns smithing and its minigame receipts; F7 can run concurrently to identify native versus tracked pose ownership. The changed producers are silent outside their finite capture. The obsolete entity/tick observation rings now require active F3 even when a gameplay pair is live. They remain observations and never grant timing. All output uses the existing diagnostic writer and single `KCD2VR.log`; this lane adds no log file.

| Verification | Result | Scope |
| --- | --- | --- |
| New smith same-packet acknowledgement | 92 pass; old D83R1 negative fails | Actual gesture producer and fall tracker |
| Retained smith suites | 19/19 cases pass | Transition 1,998 and LT placement 102 in debug/release/UBSAN; reheat, recovery, ownership and floor/grip checks |
| Free-kick dispatch/wait/cleanup | 61 pass, also ASAN/UBSAN; old negative fails | Extracted production functions, explicit native leaves |
| Native kick selector | 89/89 pass | Copied retail instructions and actual table rows; controlled surrounding engine leaves |
| Native callback wrapper | 64/64 pass; old negative has 8 expected failures | Actual native constructor/clone/clear bytes composed with production reader |
| Reciprocal pair reader | 198 pass | Production reader, controlled native memory |
| Paired adapter cleanup | 79/79, also ASAN/UBSAN | Complete production adapter with controlled native containers/writes |
| Final combo contact | 66/66 | Retained production endpoint and impact geometry policy |
| Legacy policy/observational gating | 42/42 | Retired timing helper tests plus F3-off observation checks; no claim the old helper is active |
| Exact retail static audit | 61/61 | Production pins, vtables and native call targets |

Reproducible commands are provided by `tools/test_free_kick_ack_d84.py`, `tools/test_native_pair_callback_d84.py`, `tools/audit_combat_pair_d72.py` and `tools/check_smith_d83r1.py`. The callback fixture requires the supplied exact retail DLL. Its generated native byte slices are local research inputs and must not be included in a redistributable source or build package.

A focused live acceptance pass should combine F3+F7 for one empty-hand, one armed and one targetless R3; retain the same request serial through guard acknowledgement, native row/action admission and visible kick. For smithing, F5 should cover repeated strokes before and after reheating, including a quick reversal as native phase 14 first appears, and one held-LT alignment sequence. For paired combat, F3+F7 should first confirm the corrected native pair proof. That proof must not be reported as blade-driven damage until the separate native event/contact bridge is implemented and measured.
