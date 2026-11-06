# D83 combat claims: what the supplied recording establishes

The latest recording confirms native combat results and a native combo flag. It does **not** establish that 19% of the player's intended hits failed, and it does not justify changing combat timing settings on that percentage alone. D83R1 preserves the existing D82 contact-timing repair and the user's settings.

This audit uses `KCD2VR(20261102-000003).log`, 115,104,677 bytes, SHA-256 `a644f4d0893b855a0962fd11805eeb7acc9da61305cc1cc706d99562b658d938`. The log is a D81R1 recording; it cannot prove the later D83 overlay or reload implementation works.

## Corrected counts

| Recorded category | Count | What it means |
|---|---:|---|
| Recognizer onsets, final cumulative totals for the three sessions | 258: 62 + 96 + 100 | Recognizer events. They are not independently observed human swing attempts. |
| Queued contact rows and distinct serials | 243 | Contact-stage events; a single gesture can produce several. |
| Distinct gesture IDs with a queued contact | 79 | Gestures that reached this contact stage. |
| NPC observations before native contact admission | 416 rows, 239 serials, 78 gestures | Includes repeated observations and logger wrappers. The row explicitly says the decision is not inferred. |
| Exact native result rows | 64 serials, 64 gestures | Matched `H29 STRIKE_RESULT` records. |
| Queued gesture IDs without a matching exact native result | 15 | Missing a result in this capture. This alone does not establish whether the hit failed or why. |

The 64 exact results comprise **30 health-damage results, 21 stamina-only results, 11 perfect blocks and two parries**. Calling all 64 “landed hits” obscures those different outcomes. The route split is exactly 32 `DIRECT_NATIVE_CONTINUATION` and 32 `HYBRID_NATIVE_LIVE` results.

Fifteen divided by 79 is 18.99%. That arithmetic is correct, but the denominator is “gestures with a queued contact” and the numerator is “those without an exact recorded result.” It is not a measured miss rate for the player's intended strikes. The capture itself distinguishes stages and warns against interpreting absent source samples as missed hits.

## The fifteen unmatched gesture IDs

The unmatched IDs are 324430, 333618, 339326, 346794, 427098, 436718, 454222, 454322, 455674, 460986, 461566, 469894, 472098, 573066 and 582118. Their first queue-row positions are recorded in `finisher/combat_audit.json` so these joins can be reproduced without guessing from timestamps.

Claude's README assigns these fifteen to seven hit reactions, two NPC blocks with missing buzz, two phase-2 refusals, two uncommitted gestures, one expired command and one stale contact. Nearby state observations can be useful leads, but this audit does not turn them into fifteen proved causes. In particular, observing an NPC block state does not prove a lost haptic event. A diagnostic named `EXISTING_ACTION_REFUSED_OR_NOT_CALLED` also includes the possibility that no native call occurred.

There are 11 such command-return rows in this recording, and every one has `called=0` and does not infer native acceptance. Describing those rows as “the game was called and refused” would contradict the available marker. They are diagnostic rows rather than eleven independently counted human attempts.

## Combos

One exact native result has `combo=1`: gesture 572682, serial 316, at 00:56:49.636, log row 289229. It used `HYBRID_NATIVE_LIVE`, reported 36.88539 health damage and has `comboEnd=0`. This supports **one recorded result carrying the native combo flag**. It does not prove a completed combo finisher; none of the exact result rows has `comboEnd=1`.

The recording's 32/32 route split is confirmed. The broader statement that the direct route “never advances the game's combo chain” is not proved by this recording alone. Absence of a combo flag among those direct results is narrower than a universal implementation claim. That broader conclusion would need a native call-path and combo-state analysis with an appropriate matched recording.

The proposed `VrIntentLiveWaitMs` 60 → 120 and `VrComboWindowSeconds` 1.5 → 2.0 changes are therefore not applied. The supplied evidence is sufficient to identify stages worth investigating, but it does not show that these settings caused the missing results or that widening them would preserve the intended timing. The existing D82 timing fix remains in the reviewed source.

## Why the finisher overlay is relevant

The recording does show the native body taking ownership during a player masterstrike. At approximately 00:50:19.337, row 257834, the protected alias is `combat_rg_sz1_dz2_masterstrikedeath_lngsw_master`. The following pose-validity record at row 257841 reports `nativeBody=1` and `stage=NOT_ATTEMPTED` even though tracked head and hands are available. This supports the observation that ordinary VRIK yielded during that native paired move.

It is useful evidence for adding bounded head turns at that native boundary. It is not evidence that the new overlay has already run. The separate `FINISHER_D83R1_REVIEW.md` describes what was repaired and what the host tests actually verify.

## Reproduce

```text
python tools/audit_combat_log_d83.py path/to/KCD2VR.log --out ../combat-audit.json
```

The script records the input hash, final session counter rows, distinct serial/gesture counts, exact result distribution, combo records, unmatched gesture IDs and relevant evidence-row positions. It keeps event categories separate and never promotes an absent acknowledgment to a proved native rejection.
