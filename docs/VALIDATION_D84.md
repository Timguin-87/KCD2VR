# D84 validation and delivery boundary

Source marker: `KCD2VR-D84-REGRESSION-REPAIR-20261102`. Review date: 2 November 2026.

## Completed verification

The focused CMake configuration builds and passes **16/16 tests**. The additional runners execute actual headers, literal production functions or complete selected translation units with explicitly substituted native/OS/GFx leaves. Assertions remain enabled in optimized builds. The final root integration runner passes 26 cases, including expected-failing historical controls.

| Area | Result and meaningful boundary |
| --- | --- |
| Staged pickup/storage | 139 production-consumer checks in debug, optimized and ASAN+UBSAN; the previous consumer fails the new regression fixture. |
| Native fallback storage | 64 actual-owner checks, including each acquiring hand, capture OFF/ON, failures, loose drop, cancellation, merged inventory stack and repeated callbacks. Native haptic leaf controlled. |
| Instant adapter/native pins | 37 actual adapter checks; 15 function pins and seven vft targets match the supplied exact retail image. |
| Alchemy presentation | 19 actual pre-Update receipt checks; retained complete crafting clock passes 50 checks. Requested/applied rate and owner handover tested separately from semantic completion. |
| Dice observation | 17 actual completed-frame observer checks; four field-use pins match the supplied retail image. OFF admission, lifecycle refusal and unchanged fixture bytes tested. No physical cup feature is claimed. |
| Smithing | New acknowledgement/release fixture passes 92 checks and fails against D83R1. Retained runner passes 19/19 suites, including reheat/phase continuation and native LT placement. |
| Free-space R3 | Literal dispatch/wait/cleanup passes 61 checks in normal and sanitized runs; the native selector fixture passes 89. Historical preimage fails the ordering control. |
| Native paired reader | Actual retail constructor/clone composition passes 64 checks; D83R1 fails the four legitimate joins/eight expected assertions. Reciprocal reader passes 198; complete adapter cleanup passes 79 normal/sanitized; native static audit passes 61; retained final-combo contact passes 66. |
| Dodge input delivery | Actual publisher/dispatcher interleavings pass 246 checks per debug/optimized/sanitized configuration; D83R1 compiles and fails 116 forced-interleave assertions. |
| Physical-walking controls | Actual menu/Mod/host pipeline passes 121 checks in each configuration. Retained producer 38, native callback/adoption 144 and no-forward dodge 66 pass. D83R1 fails the intended owner/F1 controls. |
| Corpse service | Actual policy, outer service and saved-owner fixtures pass debug/optimized/sanitized configurations. Articulated adapter 56 and lab policy 74 pass. Three historical preimages fail the intended unwanted dependencies. |
| Corpse persistence | Literal menu/apply/Save/load fixtures pass debug/optimized/sanitized configurations; immutable D83R1 fails the expected persistence checks. INI IO is controlled. |
| Crossbow ownership | Actual retained bridge/clock/passenger fixture passes 606 checks in each configuration. Five deliberately broken historical variants each fail their intended check. |
| Reload capture | 23 checks per configuration; F5/F7/F11 coexistence, original session, stop/restart/expiry and bounded drain tested. D57/D77 compatibility fixtures pass. |
| Physics capture queues | 20 new checks per configuration; retained native-stage 28 and passive contact/haptic 140 pass. F9-only preimage control fails as expected. |
| Host/profile diagnostic gates | Three fixtures pass in three configurations: direct ticket test with 10,000 cold calls plus routing checks; 61 literal producer/lifecycle checks; 323 retained F9 status checks. OFF skips optional CVar/phase reads, clocks and formatting; gameplay state and retirement continue. |
| HUD | Actual semantic, reveal, lifetime, graph/native alpha/visibility and full FlashUI writer consumers pass. Right-only palm reveals vitals; left-only reveals compass. Historical semantic preimage fails the new assignment. |
| Map recording | Actual scopes, worker and receipt path pass debug/optimized/sanitized checks. OFF worker/destructor clocks, mixed clock epochs, mid-qualification recorder restart and stale intervals covered. Independent review reproduced the old restart problem and verified rejection after repair. |
| Shared sinks/F9 keys | Literal `Write`, `WriteFor`, `WriteReceiptFor` and Mod F9/Shift+F9 blocks pass. Source has no gameplay setter available to the key fixture. Queued data cannot borrow a new or different recorder. |
| Source registration | `check_subsystems.py` passes the retained 54-original-TU contract and static target registration. This is source registration, not object compilation/linking. |
| Protected source | 72 critical files are byte-identical to reviewed D83R1, including runtime INI, camera/frame/XR/stereo/upscaler, bow/smoothing and core collision files. Complete profile geometry/support/bow math is identical after removing exactly the two changed diagnostic blocks and normalizing line endings. |

Exact commands, source hashes, outputs and declared substitutions are in the selected JSON receipts. Counts above are assertions or fixture cases, not newly successful player actions. A negative control passes when the old/broken implementation compiles and fails the intended assertion; it is not counted as a functioning historical implementation.

## Source preservation and what was reviewed

`evidence/PRESERVED_SOURCE_D84.json` records the 72 complete-file comparisons. Shared Mod, pose-host and native interaction files contain the documented integration edits; they cannot be called byte-identical as a whole. The profile gate fixture separately verifies that geometry, hand support and bow-axis arithmetic were not changed.

Peer review checked the actual native callback wrapper construction, kick guard/cleanup state machine, full HUD consumer mapping, original-session drains, recorder gates and corpse service branches. Late review findings were repaired: typed F9 stage draining, old F9 fixture expectations, a mismatched legacy F11 session label, automatic map worker reads, clock-domain mixing and recorder-restart acceptance. Production fixtures were rerun after those source changes.

The exact native tests use the user's supplied `WHGame(8).dll`, SHA-256 `bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7`. Some constructor/selector fixtures execute copied retail instructions with controlled external leaves. That is stronger evidence for a layout/selector than a fabricated flat structure, but it is not a running native game. The distribution contains source runners and compact receipts, not the DLL, extracted native byte slices, local executables or game archives.

## Recorded limitations and unsuccessful exploratory checks

- No complete Windows/MSVC DLL build, link, native hook installation, Windows SEH unwinding or live game save was executed.
- No GPU/vendor SDK shaders, eye images, stereo fusion, audio/particles, physical body movement, XR haptic hardware or measured FPS were exercised.
- ASAN and UBSAN ran where labelled. LeakSanitizer was disabled because the executor restricts the required process inspection.
- Native-clock/callback tests explicitly substitute tracking, guarded memory, timing or engine leaves. The roomscale native callback fixture enables the Windows-only callback block in a copied host artifact; it does not install a retail hook.
- A legacy monolithic D40 settings harness did not compile with its supplied C++17/old-stub assumptions against the current source. It is not claimed green. Current relevant Save/load and roomscale menu consumers were instead executed directly under the documented fixtures. Early HUD expectations still described the old hand assignment; those were updated to the user's requested mapping and verified through the actual FlashUI consumer. Compact exploratory-attempt metadata records this distinction.
- Quiet failure status remains published through the existing log mailbox. A session-zero status does not authorize an on-screen popup. The gate fixture verifies publication and ownership, not a visible popup while captures are OFF.
- Source-level access/gating review of the legacy native anatomy sampler is not an independent game or renderer test. Its primary recorded session now matches its typed recorder ticket.

The most important open runtime and implementation questions are in [the regression ledger](REGRESSION_LEDGER_D84.md). Passing these checks does not close physical dice-cup ownership or blade-driven paired damage, and does not establish every crossbow/body/collision asset in a headset.

## Reproducing the principal checks

Use fresh build directories outside the source. From the complete patched source:

```bash
cmake -S tests -B ../d84-tests -DKCD2VR_D84_FOCUSED_ONLY=ON -DCMAKE_BUILD_TYPE=Release
cmake --build ../d84-tests --parallel 2
ctest --test-dir ../d84-tests --output-on-failure
python tools/check_subsystems.py
python tools/check_root_d84.py --out ../d84-root-checks --baseline PATH_TO_REVIEWED_D83R1
python tools/check_physics_gates_d84.py --out ../d84-physics-gates --baseline PATH_TO_REVIEWED_D83R1
python tools/check_corpse_settings_d84.py --out ../d84-corpse-settings
python tools/check_interactions_d84.py --build-dir ../d84-interactions --baseline PATH_TO_REVIEWED_D83R1
```

Lane reports give the native-DLL and specialized runner commands. Their receipts retain the actual invocation rather than requiring a guessed command line. Do not treat an unavailable old fixture or rewritten expected hash as a passed gate. All source runners needing original inputs state those dependencies.

## Package verification

The delivered ZIP is sealed only after the actual delivered installer passes on all six supported complete baselines: D81R1, D81R2, reviewed D82, Claude overlay on D81R1, Claude overlay on reviewed D82, and reviewed D83R1. Additional CRLF copies of Claude-on-D82 and D83R1 exercise newline tolerance. The installer fixtures cover preflight, unrelated edits/tampering, idempotence, exact backup bytes and injected write-failure rollback.

An independent complete Claude-on-D82 tree is then patched using that same installer and compared file by file with the reviewed exported source closure. Final sealing rechecks the installer, manifest, payload, compact evidence and complete source hashes, then validates ZIP CRCs and every member hash. The final results are in `evidence/installer/`, `evidence/fulltree/` and the package contents manifest. Those delivery receipts are distinct from native gameplay tests.

The installer preserves the active INI/calibration and unrelated files. It validates all affected preimages; it does not certify arbitrary unaffected user customizations as the exact reviewed source closure. Use a complete source directory, not a small overlay or the game directory.
