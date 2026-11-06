# D83 upscaler review: preserve the reviewed D82 bridge

## Decision

D83R1 retains the reviewed D82 DLSS/FSR bridge, startup policy and temporal
composition checks. Claude's replacement upscaler module is not merged. XeSS
Stereo remains unsupported in this reviewed patch. This decision follows
reproducible source defects, not an assumption that sequential-eye upscaling
cannot work.

Claude's archive is an independent 24-file overlay for D81R1. Its statement that
it contains "all of D82" refers to Claude's own earlier implementation. It does
not contain the 86-file D82 patch delivered in this conversation. The two modules
share a filename and namespace but expose different interfaces and ownership
contracts.

## What the literal-source tests show

The new review fixture compiles the complete, unmodified Claude
`native_upscaler_stereo_d82.cpp` with its supplied header. Only the Win32,
hook-library and vendor API leaves are replaced by controlled host definitions.
The actual create, evaluate, reset, lookup and lifetime wrappers are executed.
Successful NGX calls use the documented success value 1; FFX/XeSS fixtures use 0.

All six behaviours below reproduce for each of DLSS, FSR and XeSS: 18 cases,
135 assertions, passing in debug, optimized and ASan/UBSan builds. "Passing" here
means the tests successfully reproduce the defects in the supplied source.

| Synthetic call sequence | Observed behaviour in Claude's actual wrappers | Consequence |
| --- | --- | --- |
| Original creation succeeds; right-eye creation fails; RIGHT evaluates | RIGHT is sent to the original LEFT context | The code knowingly resumes shared history after the exact failure that makes independent history unavailable. |
| Both native eyes request reset; the next pair does not | The next LEFT is reset again while RIGHT is not: `1,0` | The reset handoff creates an additional asymmetric reset. |
| LEFT evaluates and RIGHT is skipped, then another pair runs | The later pair receives resets `0,0` | There is no frame/epoch continuity check to repair the missing RIGHT history. |
| An evaluation occurs outside the eye scope between pairs | It advances LEFT; the following pair receives resets `0,0` | Unscoped/mono work does not invalidate the later stereo histories. |
| RIGHT is evaluated twice without a new qualified frame | The SDK evaluates RIGHT twice | No frame identity prevents duplicate history advancement. |
| An evaluation fails, then a later pair runs | The later pair receives resets `0,0` | Failed work does not invalidate the continuity state. |

These are controlled call sequences, not newly observed events from a D83 game
session. They establish the implementation's behaviour at these boundaries.
They do not show what pixels a vendor shader produced on the user's GPU.

The review also confirms that Claude's module supplies no completed pair receipt
to the existing D82 final-composition publication gate. It has no equivalent
qualified epoch/frame API. Counting calls or printing that two instances exist
does not establish that a complete frame may be presented.

## Overlay compatibility is a concrete source problem

The reviewed D82 callers in `dllmain.cpp`, `native_stereo_preflight_v6602.cpp`,
`native_stereo_eye_control_v6604.inc` and `device_hook.cpp` call the aggregate's
`InstallEarly`, `BeginEpoch`, `PairReady`, `TemporalActive` and related methods.
Claude's header replaces those declarations with `Install`, `Uninstall`,
`Installed` and `LogStats`.

A source-interface probe fails against the Claude header and compiles against
the retained reviewed header. Therefore copying Claude's overlay over reviewed
D82 does not produce a compatible combination. This is a source/API check, not
a Windows ABI or DLL-link test.

When installed exactly as Claude documents, onto D81R1, those newer D82 callers
are absent. In that case the overlay instead omits the reviewed D82 history,
readiness and final-composition checks. Neither installation path includes both
implementations correctly.

Claude also adds its new translation unit to the frozen original-source lists.
The supplied `check_subsystems.py` consequently fails its original 54-TU contract.
D83R1 preserves the original lists and the reviewed D82 registration of its
three new translation units. The current source contract passes. This source
contract failure is separate from the unperformed Windows build.

## Native addresses were checked fairly

The supplied native image has SHA-256:

`bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7`

All five checked entry/layout byte pins and four direct-call targets from
Claude's implementation match that image, timestamp `0x6A350E20`. The review
does not dismiss these as guessed addresses. Correct entry addresses do not,
however, establish correct history continuity, hook lifetime or presentation.
This compact pin check does not independently certify every XeSS declaration
or runtime import-table binding.

## Retained implementation and current limits

The reviewed D82 code retains independent vendor-created LEFT/RIGHT contexts,
paired discontinuity handling, exact eye/frame/epoch qualification, SDK success
receipts and publication tied to the verified final-composition command-list
recording. Mode changes do not reinterpret incomplete old work as a Native pair.
The original native camera, projection, TAA and OpenXR algorithms are preserved.

Startup offers Native, DLSS and FSR; selecting a temporal technology is saved
for a full restart. The source patch does not delete `user.cfg` assignments,
change the selected technology automatically, or replace calibration. Use the
existing explicit selection/save workflow.

No D83R1 Windows build, native detour execution, vendor GPU run, headset fusion
test or performance measurement has been performed here. The earlier reviewed
D82 source remains a tested source candidate; retaining it is not a claim that
the headset acceptance step has already happened.

## Reproduce and read the evidence

From the complete reviewed source root:

```sh
python tools/check_claude_upscaler_d83.py --claude-source /path/to/KCD2VR_D83_SOURCE --out /tmp/kcd2vr-claude-upscaler-review
python tools/check_stereo_wiring_d82.py --out /tmp/kcd2vr-stereo-wiring
python tools/check_subsystems.py
```

The original Claude overlay is an input to the comparison test; it is not the
production implementation in D83R1. The patch's compact evidence contains exact
source hashes, test commands, native pin results and an independent fixture
review. For the retained design, see `TEMPORAL_STEREO_D82.md`,
`TEMPORAL_COMPOSITION_D82.md` and `VALIDATION_D82.md`.
