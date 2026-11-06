# Smithing: reviewed D83 changes

The merge preserves the reviewed D82 fix for hammering after reheating and adds
the useful parts of Claude's D83 work. It repairs two different ways the input
controller could lose readiness, keeps the captured tool grip through the
observed post-fall transition, and makes held-LT movement follow the game's
actual workpiece placement.

These are source repairs verified through the production controller and
placement adapters on a Linux host. This review did not build an MSVC DLL or
run the game/headset. Native strike acceptance, heat, quality and scoring still
need runtime validation.

## Why the two D82 branches had to be combined

Claude's archive says it includes D82. That refers to Claude's independent D82
implementation, not the reviewed D82 repair used as this merge's starting
point. Replacing the reviewed `semi_smith46.h` wholesale would remove its
moving-hand rearming change.

The original controller waited for two samples with at most 6 mm of movement
before allowing a fresh hammer lift. Returning to the anvil while continuously
moving could prevent that gate from ever opening. Reviewed D82 retained the
quiet-entry route and also accepted a fresh 18 mm lowering followed by a
separate 30 mm lift, both measured in native anvil idle. D83R1 keeps this path.
Heating, transfer, stale tracking and unrelated tool motion cannot supply it.

The production final-pad test still passes all 1,513 D82 checks. Against
Claude's full D83 overlay, that same moving-return scenario compiles and fails
at the expected assertion: the fresh lift never reaches native RT.

## What the older recording actually establishes

The latest supplied D81R1 log contains no active smithing capture. Claude cited
an older D80 session, so the review read that exact diagnostics archive rather
than treating the README as runtime evidence.

The member `KCD2VR-diagnostics.log` in `KCD2VR-diagnostics(8).zip` contains
1,419,685 lines and 649,073,628 bytes. Its SHA-256 is
`84ddd2b408293d580c8939167200e4b97663a57c8f6f8730138d0e58f0a93fa0`.
The archive SHA-256 is
`3826e96e5ae7464d7b378e3c3cb0d9f888fe6312cfcba6976ca367fc5b3b4056`.
The compact provenance and original rows are retained in
`archive_provenance.json` and `phase15_capture_excerpt.txt` in the review's
smithing evidence.

All four observations below belong to native instance `2A0E60832E0`, lifetime
3, capture session 3 on 1 November 2026. Earlier recorded rows show native
phase 14 followed by phase 16.

| Phase 15 observation | Original line | Nearby controller output | What it supports |
| --- | ---: | --- | --- |
| 21:56:13.889 | 259887 | `INACTIVE` at line 259889, then `REBASED` at 21:56:13.900 | The active controller lost its state at the observed post-fall transition. |
| 21:56:16.478 | 263239 | `RAISE_REQUEST` at line 263241, then `RAISE_WORK` | A successful path also occurred near a phase 15 observation. |
| 21:56:21.859 | 270198 | `INACTIVE` at line 270200, then `REBASED` at 21:56:21.868 | The same retirement pattern occurred again. |
| 21:56:31.533 | 282658 | `INACTIVE` at line 282660, then `REBASED` at 21:56:31.543 | A third observed retirement pattern. |

These records establish that phase 15 occurs in the native post-fall sequence
and that three nearby input outputs retired the controller. The source then
explains the mechanism: phase 15 was outside the admitted stroke phases.
The observers sample different points and coalesce some rows, so this is not
a precise measurement of how many frames phase 15 lasts. It also does not
prove that every phase 15 transition loses a stroke, or that this accounts for
a particular percentage of missed hammer swings. The fourth observation is
included specifically because it does not show the same failure.

## Phase 15 now continues an existing stroke

For saved tracked smithing modes, phase 15 preserves an existing acknowledged
stroke and its measured recovery lift. It cannot start a new hammer press or
synthesize workpiece axes. A new press still requires native phase 11. Merely
entering phase 15 without an owned stroke grants no readiness.

The native fall-speed permission remains restricted to its existing phases;
phase 15 does not acquire it. The legacy displacement-clock route also keeps
its previous admission contract. Real manual RT remains the native input.

The same passive transition is admitted by held-LT assistance, the already
captured anvil floor, and canonical grip continuity. A floor can still only be
acquired from native idle. A grip can only settle at its existing settled
phases. Tool identity, owner, model, object, attachment, source and mode guards
remain in force, so a different tool or station cannot inherit the old state.

## A spatial tracking jump is different from a lost input history

Claude's fall recovery change preserved the completed-fall state whenever the
controller encountered a discontinuity. That also covered backwards sample
sequences and long timing gaps.

D83R1 preserves only an already released, owned native fall with a current,
finite hand sample and coherent timing within the existing 80 ms bound. A
spatial jump is rebased and contributes no hammer movement or queued lift.
The following deliberate fresh lift can then use the observed completion.
Clock or sequence regression, a long gap, changed epochs, tool loss and
disabled recovery retire the state. No old lift crosses the rebase.

This does not invent a native cancel action. Releasing an acknowledged native
hammer raise can itself mean a strike; the existing native continuation and
release rules remain authoritative.

## Held LT follows the native workpiece

Claude correctly found that held LT suppressed the synthesized movement axes.
Its new visual slide also added a separate lateral/forward offset to the tool,
up to 20 cm from the current native position. The current anchored hand-input
policy drops the lateral axis, so that visual slide could place the iron
somewhere the native evaluator was not using.

D83R1 keeps the existing fore/aft hand-input route active while LT is held and
uses the current untouched native tool transform for the final placement.
The captured local grip is inverted to obtain the wrist position and
orientation that put the visible tool at that native result. This also
preserves the native height and tilt. Directly held blanks and blanks nested
in the tongs use the same current relationship.

This is not a promise of unrestricted 1:1 sideways sliding. The current mod
policy suppresses lateral hand input; the native binary's older movement pins
describe a bounded Vec2 mover, so the evidence does not establish that the
engine could never support a lateral route. Adding one would require proving
the input-to-workpiece mapping and contact behavior together. This merge does
not add a separate visual offset ahead of that work.

Releasing LT immediately restores the current tracked goal. There is no stale
press anchor or delayed blending tail. Samples consumed while LT is held are
not replayed on release. Explicit LT placement still does not depend on a
collision-bounds query being available.

## Validation and limits

Run from the source root:

```sh
python tools/check_smith_d83r1.py \
  --baseline-d82 /path/to/reviewed/D82 \
  --claude /path/to/D81R1/with/original/Claude/D83 \
  --output /path/to/receipts
```

`RESULT.json` records the compiler commands, exit status, readable assertion
outputs and source hashes. A negative control must compile successfully and
fail the identified behavioral assertion; a compilation error never counts.

| Production coverage | Result |
| --- | --- |
| Final station pad, repeated reheat, phase 15, fall jumps/timing boundaries, held-LT axis behavior | 1,998 checks in each of debug, optimized and UBSan configurations |
| Current native tool alignment, nonidentity grips, direct/nested blanks, rotated/scaled model frames, phase 15, release and floor ownership | 102 checks in each of debug, optimized and UBSan configurations |
| Existing D82 reheat fixture | 1,513 checks |
| Existing native-height, recovery, recovery-lifetime and retirement fixtures | 369, 370, 257 and 666 checks |
| Existing native-fall permission fixture | 176 checks |
| Existing floor and floor-owner fixtures | 43 and 76 checks |
| LT placement and final-input fixtures | 128 and 105 checks |
| Actual support and hammer grip lifecycle fixtures | 46 and 46 checks |
| Actual stroke diagnostic/pad wiring fixture | 22 checks |

All 19 positive build/run configurations pass. The two changed legacy test
expectations are explicit: LT phase admission now includes 15, and the actual
support lifecycle loop now crosses phase 15. Existing D82 reheat scenarios are
unchanged; its main function only gained a guard so the new fixture can reuse
the real driver.

Five comparison controls compile and fail the intended checks: reviewed D82
loses phase-15 recovery and suppresses held-LT movement; Claude's D83 loses the
moving-return repair, preserves fall readiness across an invalid timing gap,
and moves the visible tool away from its current native placement.

The fixtures supply native memory, copied tool witnesses and model transforms
as controlled leaves. They execute the production algorithms, not a
replacement controller, but cannot demonstrate a successful forged hit in
WHGame. A runtime check still needs to confirm repeated anvil/forge/anvil
cycles and fore/aft movement under LT with the actual native hit evaluator.
The user's saved INI settings are unchanged by this lane.
