# Smithing and combat review — D82

## What was actually recorded

The supplied `KCD2VR(20261102-000003).log` identifies the running build as
`KCD2VR-D81R1-REVIEWED-MERGE-20261102`. It contains three F3 combat sessions.
It contains no smithing runtime records: the four smith/anvil matches are hook
installation messages, and the capture summary reports zero F5 sessions.
The user's report of failure after reheating is therefore the symptom driving
the smithing source review, not a result reconstructed from smithing log rows.

## Smithing after reheating

Saved tracked modes use `semi_smith46::Controller` to hold and release the native
hammer trigger. Leaving the anvil retires the preceding stroke and recovery
state. On return, the old code required two consecutive tracking samples with
at most 6 mm total right-hand movement before recognizing a new lift.
Repeated moving strokes can remain above that threshold indefinitely. At a
90 Hz sample rate, 6 mm per sample is approximately 0.54 m/s. This is an arming
condition, not the strike-strength setting.

D82 keeps the quiet-entry path and adds a second way to become ready. Fresh
movement in native anvil idle can lower the hammer at least 18 mm, then lift it
at least 30 mm. Lowering only arms the gesture. The separate lift still requests
the ordinary native trigger, and a downward reversal still releases it. Native
charge, fatigue, heat, impact, quality and hit evaluation remain authoritative.
Transfer or heating motion cannot supply this lowering. Owner, tracking, tool
availability, manual-trigger and native-phase boundaries retire it.

This repairs a source-reproduced post-reheat dead zone. The capture cannot prove
that this is the only cause of the user's missed smithing strokes. It does not
establish a correct native workpiece collision or successful forged strike.

### Validation

`tests/smith_reheat_d82_test.cpp` includes the real `semi_workstations46::State`
and real final pad adapter. Only copied native tool witnesses and trace output
are supplied by the fixture. It covers repeated forge/anvil returns with
continuous moving hands at 8, 9, 11 and 16 ms sample intervals, owner and tool
loss, manual input, duplicate samples, passive entry motion and native fall
ownership. The repaired source passes 1,513 checks. The same test compiled
against the untouched D81R1 headers fails at check 40, where the post-reheat
lift never reaches RT.

Existing checks also pass: native-height cycles 369, recovery 370, actual
diagnostic/pad wiring 22, LT assistance 105, and native-fall permission 176.
These are Linux host C++ tests, not an MSVC DLL build or headset validation.

Reproduce the focused test with:

```sh
python tools/check_smith_reheat_d82.py --baseline /path/to/unmodified/D81R1
```

## Combat and combos in the supplied capture

The exact same-stack `H29 STRIKE_RESULT` records report 64 applied native
outcomes: 30 with health damage, 21 with stamina damage only, 11 perfect blocks,
and two parries. These outcomes are a captured subset, not a denominator for
the player's attempted swings. The 243 queued contact rows include multiple
contacts, while recognizer and logger counters measure different stages.
They cannot substantiate or disprove a 10–20% missed-swing rate.

At 00:56:49.636, line 289229, serial 316 records a cut against target 45102 with
`accepted=1 applied=1 healthDamage=36.88539 combo=1 comboEnd=0`. Immediately
before it, the combo manager advances candidate C01 to 2/3. There was a
combo-marked damaging hit and progression. No complete finisher is established:
all 64 result rows have `comboEnd=0`, and the native combo observer explicitly
does not claim accepted hits from candidate cursors alone.

The 16 `STRIKE` rows labelled `TOO_WEAK` still have `accepted=1`. Their legacy
reason text is not evidence that 16 attacks were dropped. Likewise,
`PUNCH_HEALTH accepted=1112` means accepted diagnostic queue records, not
1,112 landed punches. Free-targeting rotation veto counters are not hit misses.

### The final-contact timing case and D82 repair

Gesture 573066 queues prepare and commit at native tick 249949578. Its exact
final-combo path waits for physical contact under the existing 100 ms command
deadline. Both commands expire at age 109 ms (lines 289443–289444). The first
queued contact for that same gesture arrives at tick 249949718, 140 ms after
the original request (line 289449, serial 317). It reaches `STRIKE_NPC` at
line 289465, but no exact `STRIKE_RESULT` for serial 317 appears.

D82 repairs that timing class by separating the captured final-step intent
from the input transaction created by its eventual measured contact. Ordinary
commands still expire at 100 ms. An already captured final intent can wait
without sending an input only while its original native action, live combo
slot, target, candidate, input source and recovery epoch remain current. The
wait is bounded by the existing 600 ms physical-strike lifetime; the original
native slot keeps its existing deadline as well. A later deliberate gesture
or an enqueued cancellation retires the wait.

A matching measured contact then supplies a new input timestamp and sample.
The original queue entry, request timestamp, gesture, equipment and epochs
remain unchanged as provenance. The new transaction expires 100 ms after the
captured contact. Later contact samples can neither renew it nor change the
selected original recipe. Prepare and release each reserve their edge before
the original native call, so an uncertain return cannot cause a duplicate
entry. If the final route disappears before dispatch, the transaction cannot
fall through to a newly opened master-strike or ordinary-input route.

The delayed bridge is deliberately limited to the existing tracked-ray HYBRID
path with an exact current H19 native continuation. It does not reinterpret a
typed solver command as a legacy command or manufacture a new solver proof
from the current frame. Unsupported methods and absent native continuation
proof keep the existing refusal. The saved instant-combat, combo-assistance,
physical-finisher and 1.5-second combo-window preferences remain unchanged.
The captured wait proves the relevant native final-input path ran; the saved
`VrDirectOwnsHits` option is separate from the comparison-backend selector.

`tools/test_final_contact_d82.py` compiles literal production bodies for the
queue ring, deadline block, snapshot-driven final plan, continuation checks,
queue pop and irreversible input-call seam. Native memory and native action
read/call leaves are controlled; native admission and damage are not simulated.
The repaired path passes 248 checks, including the 109/140 ms sequence,
wrong gesture/target/candidate, completed phase, stale contact, source and
recovery loss, typed solver refusal, cancelled attempts, newer gestures,
original 100/101 ms ordinary boundaries, fixed contact expiry and once-only
calls. The same fixture against untouched D81R1 source compiles and fails
exactly the two intended 109/140 ms checks. Existing actual D72 input-edge
checks pass 51/51 and endpoint/impact-geometry checks pass 66/66.

This establishes the source repair and its rejection behavior. It does not
prove that gesture 573066 would become an accepted finisher in the game: the
capture does not contain the exact native continuation witnesses needed to
replay every admission decision. The game still decides native input,
selection, stamina, damage and combo completion. MSVC/native ABI/headset
validation remains unperformed.

Reproduce the contact regression with:

```sh
python tools/test_final_contact_d82.py --out /path/to/evidence --baseline /path/to/unmodified/D81R1
```

There are also 11 `EXISTING_ACTION_REFUSED_OR_NOT_CALLED` command records,
nine orphan-commit discards, one explicit context abort, and the two expired
commands above. These are commands, not independent human attempts; some share
the same gesture and the capture does not resolve every underlying refusal.
No combat sensitivity setting is changed on the strength of these counters.
