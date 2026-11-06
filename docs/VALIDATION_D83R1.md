# D83R1 validation and evidence boundaries

Target: `KCD2VR-D83R1-REVIEWED-MERGE-20261102`.

The review used the exact supplied Claude D83 overlay, reconstructed reviewed
D82 and D81R1 sources, the latest D81R1 recording, the older D80 smithing
diagnostics and the supplied supported WHGame image. The new tests compile
production code or literal extracted production functions with controlled
native/OS/SDK leaves. They do not simulate a successful native game result.

## Newly executed checks

| Area | Executed result and connection to production |
| --- | --- |
| Crossbow/holster arbitration | 210 checks of the real arbiter and nock-commit bodies in optimized and ASan/UBSan builds. Old arbitration fails 42 relevant assertions; Claude's swallowed release acknowledgement fails three. |
| Crossbow continuity retained | Reviewed D82's actual producer/passenger first-pose fixture passes 606 checks, including its legacy suites and five restored-defect controls. The actual D78 socket/writer suite passes 80 checks in debug, optimized and ASan/UBSan with three causal controls. |
| Claude crossbow comparison | Literal-body probes reproduce missing bow/crossbow release acknowledgement and native cocking in both pull directions. These are source reproductions, not new headset observations. |
| Smith station/controller | 1,998 checks in each of debug, optimized and UBSan configurations. Actual station/pad routing covers moving re-entry, phase15, fall continuity, timing/source retirement and held-LT movement. |
| Smith placement | 102 checks in each of debug, optimized and UBSan configurations. Actual native placement adapter and canonical grip are exercised for current native alignment and immediate release. |
| Smith retained checks | Thirteen existing fixtures pass, including D82 reheat 1,513, native height 369, recovery 370, recovery lifetime 257, retirement 666, fall timing 176, floors 43/76, LT 128/105, grip lifecycle 46/46 and pad wiring 22. |
| Smith comparison controls | Five old-source controls compile successfully and fail the intended behaviour: D82 phase15, D82 held-LT axes, Claude moving-hand rearm, Claude invalid-timing fall continuation and Claude visual/native placement mismatch. |
| Paired head-turn overlay | 42/42 checks in debug, optimized and ASan/UBSan. The actual production header/include and literal pose math exercise source/context freshness, bone masks, ownership, attempted-write rollback, C++ unwind restoration and shared fault publication. Immutable Claude source compiles against the same fixture and fails 32/42. |
| Paired-action settings/menu | 13 production-sourced menu/runtime read/apply/save/load checks, including saved OFF values. The complete portable settings reducer passes for 111 controls. |
| Claude upscaler comparison | 135 checks and 18 synthetic defect sequences across DLSS/FSR/XeSS, in debug, optimized and ASan/UBSan. The complete unchanged Claude TU is compiled with controlled Win32/hook/vendor leaves. Successful NGX evaluations use result 1. |
| Upscaler source interface | The actual reviewed aggregate call surface compiles against reviewed D82 and fails against Claude's replacement header. This is source/API incompatibility, not a native ABI or Windows-link test. |
| Native upscaler pins | Five entry/layout byte pins and four direct-call targets from Claude's source match the exact supported image. This does not qualify every import binding, SDK declaration or GPU resource. |
| Independent reviews | Crossbow reviewer inspected final finisher ownership, mutex scope, equipment exclusion and restore fault handling; a separate review checked the upscaler fixture's production connection and claim limits. The installer change was independently reviewed for preimage selection and clobber risks. |

Assertion counts are not counts of separate play sessions. Repeated compiler
configurations do not multiply gameplay coverage. Where reported, ASan/UBSan
remain enabled; LeakSanitizer alone is disabled for the sandbox process-task
inspection limitation.

The smith receipt preserves hashes from its compiled run and identifies one
subsequent comment-only wording clarification. The compiled behaviour was not
silently re-labelled as a later source version. Final source manifests identify
the delivered bytes separately.

## Retained reviewed D82 evidence

The vendor runtime/binding/aggregate implementation, native eye flow and temporal
composition code remain byte-identical to reviewed D82. Its reset, lifecycle and
publication receipts remain applicable to those files. The new `mod.cpp` change
is the finisher settings header include; it preserves the existing stereo
startup, mode, readiness and publication integration.

The D82 final-combo timing production files are also unchanged. Its 248-case
contact regression and associated edge/geometry checks remain the validation for
that retained repair. No combat tuning change was applied in D83R1.

Final identity checks confirm 479 previously selected protected files remain
byte-identical to reviewed D82, together with 27 separately checked upscaler,
presentation, startup and combat files. `mod.cpp` differs only by the new
finisher settings include. The final stereo-wiring check passes 23 assertions;
the source-layout contract retains all 54 original TUs and 17 static targets.

See `VALIDATION_D82.md` and the retained compact D82 evidence in the cumulative
patch. Source identity establishes retention of those implementations; it does
not turn earlier host tests into a new GPU or headset test.

## Runtime evidence and corrected interpretation

The latest log is D81R1, not D83 or D83R1. It shows the crossbow ownership
failures, one combo-marked damaging result and the final-contact timing case.
Its recognizer onsets and diagnostic contact IDs do not measure all human
swings. The 15/79 unmatched-result fraction is not a validated 19% miss rate.

The older diagnostics archive provides the passive phase15 smithing evidence.
Three nearby controller outputs retire/rebase, while a fourth sampled phase15
observation accompanies successful raising. The compact original rows and
archive/member hashes preserve both kinds of observation.

The native image used for current pin checks has SHA-256:

`bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7`

## Whole-build limits

No Windows SDK/MSVC release build, native detour execution, Windows SEH recovery,
vendor GPU execution or headset rendering occurred here. No FPS improvement,
correct final mesh, successful forged strike, complete native reload or completed
finisher is inferred from host assertions.

The historical whole `run_live_runtime_d40.py` host runner is stale against both
the unmodified reviewed D82 baseline and this merge: it selects C++17 despite
later requires expressions and lacks later runtime-owner fixtures. Both failures
were retained as limitations. The new finisher settings flow is checked through
its actual production contributions, and the full portable menu reducer passes;
neither is presented as a successful compile of all of `Mod`.

## Packaging and reproduction

The cumulative patch has guarded preimages for complete D81R1, D81R2, reviewed
D82, Claude D83 over D81R1 and Claude D83 over reviewed D82. Claude's overlay
retains the older marker, so selection compares every affected preimage and
requires exactly one match. Payloads and preimages are checked before writes.
The final installer receipt is included with the package after actual application,
refusal, backup and rollback fixtures have run on all supported combinations.

The export manifest describes the exact reviewed complete source. The installer
checks affected files and preserves unrelated local files; it does not certify
every unaffected user customization as that complete source closure. Runtime
INI/calibration, game installations, proprietary binaries and optional gore are
outside the patch payload.

Run the focused tools from the source root, using output directories outside it:

```sh
python tools/check_crossbow_merge_d83.py --out /tmp/crossbow-merge --baseline /path/to/reviewed/D82 --claude /path/to/KCD2VR_D83_SOURCE
python tools/check_smith_d83r1.py --output /tmp/smith-merge --baseline-d82 /path/to/reviewed/D82 --claude /path/to/complete/Claude/D83
python tools/check_finisher_upper_d83.py --out /tmp/finisher --original /path/to/complete/Claude/D83
python tools/check_finisher_settings_d83.py --out /tmp/finisher-settings
python tools/check_claude_upscaler_d83.py --claude-source /path/to/KCD2VR_D83_SOURCE --out /tmp/upscaler-review
python tools/check_stereo_wiring_d82.py --out /tmp/stereo-wiring
python tools/check_subsystems.py
```

The lane reports and JSON receipts include the exact commands and old-source
input paths used for the comparison controls. Supplied proprietary game/runtime
binaries and large raw recordings are not redistributed in this source patch.
