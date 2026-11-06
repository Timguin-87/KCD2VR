# D82 validation

## What was and was not run

The supplied runtime recording is D81R1. D82 is the new source repair built on
that source, including the later D81R2 launcher/config protection as revised for
the new temporal bridges. The target marker is:

`KCD2VR-D82-RELOAD-SMITH-STEREO-20261102`

No D82 Windows DLL link, native DetourModKit hook execution, game session, vendor
GPU execution or headset test was performed in this environment. No generated
DLL is supplied as if it had passed those steps. The existing older build log
does not validate D82.

The executed checks below compile production headers/TUs or extract literal
production functions. Native memory, operating-system calls, devices, vendor SDK
calls and tracked input are explicitly controlled fixture leaves. Assertions are
not independent VR play sessions, and repeated sanitizer/optimization builds
are not counted as additional gameplay coverage.

## Executed checks

| Area | Production connection and result |
| --- | --- |
| Crossbow | 606 checks in debug, optimized and ASan/UBSan. Actual clock adapter, passenger consumer, raw-intent producer and completed-source producer begin before first-pose admission and use the real 6+2 / 0+3 scope profiles. Five independent restored-defect controls fail. Existing XR34/D72/D80/D81 suites and prior negative controls also pass. |
| Smithing after reheating | 1,513 checks using the real workstation state and final pad adapter. The untouched D81R1 source compiles and fails the same post-reheat case at check 40. Existing native-height, recovery, pad-wiring, LT and native-fall suites pass. |
| Final-combo contact | 248 checks on the actual queue, plan, continuation, pop and input-entry bodies. The exact old source fails the 109/140 ms cases. Existing D72 input-edge and endpoint/geometry checks pass 51 and 66 assertions. |
| DLSS routing | 268 actual-runtime checks, including a sanitizer build, plus 23 actual native-eye accessor checks. Nine causal controls reject shared histories, missing reset, wrong pipeline/frame, asymmetric resets, missing RIGHT continuity, wrong API entry, a too-short receipt history and a queue/SDK lock cycle. |
| DLSS installation/lifecycle | Independent 141-check suite across 12 fresh processes. A competing native creation waits for complete hook installation and receives a twin. The exact pre-fix source fails that race; discarded-trampoline cleanup also fails. All nine native export contracts were rechecked independently. |
| FSR routing | 911 actual-runtime checks in debug, optimized and ASan/UBSan. Eight controls reject shared history, missing reset, LEFT-only publication, wrong frame, a queue/SDK lock cycle, RIGHT without LEFT, RIGHT-only reset and missing pair continuity. |
| FSR binding and aggregate | 271 FFX binding checks across eight processes, plus 184 checks of the actual aggregate TU. Late module availability, terminal setup failure, retained trampolines, call ordering, current CVar readback, epoch technology binding, restart and unified-log preservation are covered. The actual pre-epoch-pin aggregate fails temporal-to-Native mid-pair acceptance. |
| Final-composition publication | 49 production policy checks and 85 extracted adapter checks, each in debug, optimized and ASan/UBSan. Six causal controls cover reset/reuse, generation, clear-state binding, a later main-output draw, generic barriers and missing SDK receipts. |
| Startup and menu | 525 actual startup/parser/load/commit/save checks, including a sanitizer run, and 635 actual menu/callback/save-expression/activation-gate checks. Seven causal controls are rejected. Missing twins preserve a saved Stereo request for restart while actual activation remains gated. |
| Source wiring | 23 startup/epoch/provider/publication source checks pass. `check_subsystems.py` passes: all 54 original production translation units retained exactly once, 17 static targets and disjoint frozen ownership; the three new backend/aggregate translation units are registered. |
| Preserved source | 479 selected camera, OpenXR, pose/history, bow, finger/contact, alchemy, companion, asset and INI files compare byte-for-byte with D81R1. Five camera-build/restoration functions in the intentionally edited eye-control file also compare unchanged after newline normalization. |

LeakSanitizer alone is disabled because the sandbox cannot inspect the process
task directories it requires. AddressSanitizer and UndefinedBehaviorSanitizer
remain enabled in the reported sanitizer runs. The FSR refusal repro found by
the independent reviewer was rerun after repair: both histories reset on the
next pair. A failed attempt to compile the complete Windows-dependent frik TU
with Linux shims is retained as a limit, not counted as a successful build.

## Native evidence

The exact supported WHGame image has SHA-256:

`bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7`

Its public NGX exports, actual `EvaluateFeature_C` caller, new FFX loader imports,
creation/dispatch descriptor layout, native FSR jitter/velocity selection and
final-compose call chain were inspected. The crossbow controller XML and
animation database establish the real scope profiles; the exact native reload
event handler establishes the 12 effectful CRC cases.

The composition review also traced the deferred scheduler predicate and its
named CVar. The existing stereo admission already requires the native scheduler
setting disabled; fresh call-time checks preserve that supported synchronous
path. No deferred-job TLS ownership is assumed. Microsoft's public D3D12 header
was used to verify Reset and ClearState vtable positions. The optional standalone
Windows ABI assertion is supplied but was not compiled against a Windows SDK here.

These checks establish API contracts and control/data flow. They do not establish
the GPU contents of a motion-vector, depth, color or output resource.

## Reproduce the focused checks

Run the portable tools from the source root. Use output directories outside that
tree; the source export deliberately excludes generated test artifacts.

```sh
python tools/check_crossbow_d82.py --out /tmp/kcd2vr-crossbow-d82
python tools/check_smith_reheat_d82.py --output /tmp/kcd2vr-smith-d82 --baseline /path/to/D81R1
python tools/test_final_contact_d82.py --out /tmp/kcd2vr-final-contact-d82 --baseline /path/to/D81R1
python tools/check_fsr_stereo_d82.py --out /tmp/kcd2vr-fsr-d82
python tools/check_native_dlss_stereo_d82.py --out /tmp/kcd2vr-dlss-d82 --native /path/to/WHGame.dll
python tools/test_native_dlss_binding_d82.py --out /tmp/kcd2vr-dlss-binding-d82
python tools/test_upscaler_binding_d82.py --out /tmp/kcd2vr-ffx-binding-d82
python tools/check_temporal_compose_d82.py --output /tmp/kcd2vr-compose-d82 --native-dll /path/to/WHGame.dll
python tools/check_stereo_menu_d82.py --out /tmp/kcd2vr-menu-d82
python tools/run_stereo_upscaler_d82.py --out /tmp/kcd2vr-startup-d82
python tools/check_stereo_wiring_d82.py --out /tmp/kcd2vr-wiring-d82
python tools/check_subsystems.py
```

Some evidence tools require the supplied exact game binary or saved baseline
source for their native-contract/old-source controls. Those proprietary binaries
are not redistributed. Use each tool's advertised input arguments; do not
substitute a different retail image and describe the result as the same test.
The compact JSON receipts include exact commands and hashes.

The historical D81R2 startup/menu expectations remain available as history.
Their Native-only assertions are superseded by the D82 suites; they are not
current D82 acceptance criteria. The UI regression report explains that boundary.

## What the latest runtime evidence proves

The three crossbow failures happen before hand-owned reload can obtain a valid
first pose. They are not evidence that a larger pull-distance setting would help.
All captured reloads are light crossbows; medium/heavy coverage here comes from
native assets and host fixtures.

The recording has no active smith runtime capture. The reheat repair is driven
by the user's observation and a source-reproduced arming failure.

The combat log proves a combo-marked damaging hit and progression, not a completed
finisher. Its 64 exact native results are a subset of events, not all attempted
human swings. The 10–20% reported miss rate cannot be calculated from them.

The doubled-image explanation is supported by the earlier selected DLSS config
and the old shared vendor-history implementation. There is no GPU capture of
that exact bad image, and no D82 headset image yet.

## Next acceptance boundary

Build the patch with the normal Windows toolchain and verify the D82 marker.
Use the normal matching DLL/companion installation workflow. Keep current
calibration and runtime settings; the source patch does not include a replacement
runtime INI or automatically select a temporal upscaler.

The useful in-game checks are the actual problem transitions: return from forge
to anvil while continuing to move the hammer; pause a crossbow pull halfway;
retain a retrieved bolt and place it at the rail; then compare Native with one
selected DLSS or FSR quality after a full restart. No new diagnostic sidecar or
new capture ritual is required just to exercise the repaired paths. Existing
unified startup/pair records identify the running revision and vendor routing.

Only that real run can confirm hand/prop appearance, successful native reload and
hammer outcomes, stereo clarity and performance. The source repair does not
promise the earlier C71 frame rate or guarantee a completed combat finisher.
