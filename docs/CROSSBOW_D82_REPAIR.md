# D82 crossbow repair: let the real reload reach the tracked hands

## What the latest recording actually shows

The tested build is D81R1. Its three recorded light-crossbow reload occurrences were admitted, but none reached a valid first tracked reload pose. They returned control to the native animation after **94 ms, 63 ms and 16 ms**. All 22 recorded D77 reload-binding rows say `UNAVAILABLE`. The controller/body stream continued producing fresh poses during the first failure. Changing the pull distance or grip threshold cannot repair this admission failure.

The source now repairs the production clock and passenger gates that prevented the hand-owned reload from starting. It also removes two kinds of unrelated animation traffic that could cancel that ownership. This is a source repair with host verification. It has not been compiled into a Windows game build or validated in a headset here.

## The main defect was a wrong native scope assumption

Both `XR34NativeClock` and `ReloadPassengerCurrentXR34` required the native root scope to contain exactly one animation-layer slot. That is not how the game's crossbow reload scopes are authored:

| Native scope | First layer | Number of slots | Crossbow use |
| --- | ---: | ---: | --- |
| `UpperBody` | 6 | 2 | Standing player light/medium cocking, cocked bolt placement and ordinary player transitions |
| `FullBody` | 0 | 3 | Heavy lever cocking and stealth variants |

These are the values in the supplied `kcd_male_controllerdefs.xml`, including its `Load` overrides. The recorded light-crossbow charging CAF is on layer 6. All 20 root crossbow `Load` fragments in the supplied `kcd_male_database.adb` contain one `AnimLayer`; some have two sequential clips within that layer. The scope's additional slots are reserved capacity, not additional reload CAFs.

The old count check rejected the native clock before the first-pose admission could be published. Fixing only the clock producer would still leave the identical rejection in the VRIK passenger consumer.

`selectedScopeClockEntryD82` is now shared by both gates. It admits the original single-layer contract and the two exact native profiles above. It selects the exact admitted CAF from the existing bounded FIFO and requires every companion slot to be readable, nonnull and empty, then checks those pointers and counts again. A populated companion slot, an unknown multi-layer profile, a missing CAF or duplicate CAF identity remains unsupported.

The original source freshness, child token, current native owner, event markers, FIFO identity, native timing and first-pose deadline checks remain in place.

## Unrelated animation traffic must not cancel a reload

### Foreign CAF scans

The animation event scanner runs for idle, look and other Henry CAFs as well as the reload. Previously, a scan for a different CAF could increment the reload publication even though it had not changed the reload. That invalidated a just-issued pose packet. A scan arriving while the owner lock was busy could also revoke the reload merely because its skeleton belonged to Henry.

The normal path now leaves a different CAF's scan alone. For a busy callback, an immutable clip identity can prove that the scan belongs elsewhere. That snapshot is tied to the current publication, lease serial and character; it grants no event or clock credit. Missing, stale or ambiguous identity still revokes an unobserved owned scan. The inactive hook returns after one atomic check and performs no native reads.

### Native-inert events

The exact supplied WHGame DLL's reload event handler at RVA `0x289CCD0` has 12 effectful CRC cases. All unmatched cases return without changing the reload. Our registered observer previously treated an unproven inert audio/foley event as a reload failure, including when the owner lock was busy.

The observer now ignores only CRCs that the native handler itself ignores. All 12 native effects remain protected, including the seven flag/cleanup cases beyond the five visible reload milestones. An unreadable event, an unobserved effectful event or an effectful event arriving through a busy boundary still withdraws ownership. The heavy animation's `DespawLever` typo is native-inert and is not promoted into a tool-removal event.

The disassembled native CRC set was compared with the source classifier and matched exactly.

## Resulting control path

The native game still allocates the reload child, tool and bolt. After native metadata and the first original controller update, the admitted clock can now produce the first-pose packet for the real `6+2` or `0+3` scope. The actual passenger consumer accepts that packet, and the actual completed-source producer joins fresh tracked wrist goals to the current reload clock. The existing hand-driven controller can then retain ownership through the string/tool stroke and bolt placement.

That existing controller requires held-grip motion to cross the cocking milestone, a quiver acquisition before the native bolt-acquire event, and placement/release before the native load event. A release away from the rail keeps the same bolt available for regripping. Native event handlers still perform the actual ammunition transfer and cock/load changes.

The D82 repair changes the connections that were blocking this path. It does not claim to have observed a rendered bolt, a correctly placed heavy lever or a complete in-game reload after the change.

## Why the earlier tests missed this

The older native fixtures used root layer 0 with a single slot. Most clock tests also supplied an already completed reload source. They could exercise the hand-driven policy while missing both production gates that rejected the real game layout.

`tests/crossbow_scope_d82_test.cpp` starts without a completed reload source. `tools/check_crossbow_d82.py` compiles the current native adapter and extracts the current passenger consumer, raw-intent producer and completed-source producer unchanged. The bridge obtains a real adapter-produced first admission, passes that actual consumer, produces the first completed source, closes bootstrap and exercises subsequent physical stages with the authored scope profiles.

Run from the source root, choosing an output directory outside the source tree:

```sh
python tools/check_crossbow_d82.py --out /absolute/path/to/crossbow-verification
```

Verification completed:

| Check | Result |
| --- | --- |
| D82 production bridge, debug | 606 checks passed |
| D82 production bridge, optimized | 606 checks passed |
| D82 production bridge, AddressSanitizer + UndefinedBehaviorSanitizer | 606 checks passed |
| Restore only the old clock count check | Expected failure: first admission unavailable |
| Restore only the old passenger count check | Expected failure: passenger consumer refuses |
| Restore foreign-CAF publication invalidation | Expected failure: first admission invalidated |
| Restore busy foreign-scan revocation | Expected failure: lease revoked |
| Restore inert-event cancellation | Expected failure: unrelated event cancels lease |
| Existing XR34 native, D72 instant, D80 transition and D81 ownership suites | Passed |
| Existing D81 physical/instant and secondary-scope negative controls | Failed at the expected regression checks |

The D82 cases cover light/medium/heavy first admission, hand motion holding and crossing cocking, cocked bolt phases for all three classes, quiver gating, misplaced release, regrip, exact native-effect classification, stale publication and unknown/populated scope refusal.

The fixtures explicitly substitute retail memory, native event effects, equipment/animation context, tracked poses, mechanism geometry and lock contention. They do not run the full VRIK solver, Windows SEH, the game, inventory, renderer or actual job scheduling. The portable admission cleanup substitutes one Windows `__finally` scope with C++ scope cleanup. These checks establish that the repaired production gates connect and reject the old defects; they cannot establish headset behavior.

## Evidence and scope

The review evidence accompanies this repair under `evidence/crossbow` in the investigation workspace: `capture_summary.json`, `captured_reload_timeline.txt`, `authored_scope_evidence.json`, `native_reload_event_handler.asm`, `native_handler_verification.json` and the D82/D81 test result JSON files.

Input identities:

| Input | SHA-256 |
| --- | --- |
| `KCD2VR(20261102-000003).log` | `a644f4d0893b855a0962fd11805eeb7acc9da61305cc1cc706d99562b658d938` |
| `kcd_male_controllerdefs.xml` | `37b96b1ff15e07060104020c0f9910f3ab38217598708147bbf79ebb3d7b04ee` |
| `kcd_male_database.adb` | `bb93457d0cc9441024a9fb1e3ea772ed0c76cbba79588e6564e4e71ddd6fec6e` |
| `WHGame(8).dll` | `bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7` |

The capture contains light-crossbow failures. Medium and heavy scope coverage comes from primary animation assets and host regressions, not a medium/heavy headset recording. The capture does not label which busy callback caused its two early `NATIVE_FALLBACK` results; those cancellation defects are demonstrated in production source and host controls, rather than assigned to a specific runtime callback without evidence.

No bow placement/aiming code, user settings, renderer, stereo or logging destination is changed by this crossbow repair. There is no new always-on recorder or log file.
