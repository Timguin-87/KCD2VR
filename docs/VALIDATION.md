# Current validation: R94R1

See [R94R1_REVIEW.md](R94R1_REVIEW.md) and the accompanying delivery manifest
for current changes, source hashes and executed Windows/host results. Older
validation sections below describe their own revision only.

# Current validation: D83R1

See [VALIDATION_D83R1.md](VALIDATION_D83R1.md) and
[CHANGES_D83R1.md](CHANGES_D83R1.md) for the reviewed merge of the two distinct
D82 branches and Claude's D83 additions. The newest changes have focused
production-connected host checks and comparison controls. No D83R1 Windows
release link, native hook execution, GPU rendering or headset run occurred here.

## Reviewed D82 foundation

See [VALIDATION_D82.md](VALIDATION_D82.md) for the retained source repair and
[CHANGES_D82.md](CHANGES_D82.md) for its scope. The latest supplied runtime
recording is D81R1. D82 has production-connected host checks and exact native
binary inspection, with no Windows release link or headset test performed here.

The following section preserves the earlier D81 validation record. Its baseline,
counts, INI hashes and statements about the then-latest D80 run describe that
revision; they do not supersede the newer D81R1 recording or validate a D82/D83R1
game run.

# D81 validation — historical record

## Status

D81 is an edited copy of the supplied D80 source, reviewed against the supplied
November 1 logs and active INI. The latest headset observations are from D80.
**No D81 Windows DLL link or headset run was performed.** The Linux host has GCC
and Lua 5.1 test support, but no Windows SDK/MSVC toolchain. No generated DLL is
included or presented as tested.

The original full-source ZIP has SHA256:
`d5382dbddf03163d7da2202735cb330f6ff649c2a32aff7f4e962fd6a8818a0f`.
The build marker is `KCD2VR-D81-REPAIRS-20261101`. This working copy has no source
commit; the archive manifest hashes its actual file closure instead.

## Executed checks

These fixtures execute current production code or freshly extracted production
functions with explicitly substituted native memory, clock, geometry and other
engine calls. Counts are assertions, not different headset scenarios. Repeated
optimization/sanitizer configurations are not counted as new gameplay coverage.

| Area | Executed evidence |
| --- | --- |
| Collision, mounted and dice | 821 assertions across eight suites: contact 549, unavailable coverage 31, continuity 96, locality 5, full mounted filter 22, shared dice lease 21, actual camera adapter 25 and activity-eye 72. |
| Crossbow | 40 D81 ownership checks, 596 mechanism checks and 80 socket checks in debug, optimized and ASan/UBSan builds; retained native adapter, D72 and D80 transition suites passed. |
| Crafting clock | 50 checks of the complete action-owner TU, including pre-native Update ordering, exact restoration, resource handoff, freshness/identity loss and heat time preservation. |
| Smithing | 176 checks of the actual tracked raise/reversal and native fall permission producer. |
| Alchemy attachments | 478 expanded flask/special-ingredient checks plus 78 retained mortar checks against a fresh current adapter extraction. |
| Pickup | 26 native small-item admission checks, actual requested-wrist acquisition builder, 33 instant native adapter checks, and 176 Lua 5.1 checks of packaged pickup/lab scripts. |
| Alchemy sound |Actual C++ cue selection/queue tests; 83 Lua 5.1 dispatch checks; all 16 names matched the shipped authored animation-event input. No audio was rendered. |
| NPC | 64 checks of the complete startup/HMD/subtitle/Clear/service TU, 16 exact-member checks, speaker policy, existing Lua lifecycle and 20 Lua 5.1 receipt regressions. |
| Map CPU paths | 107 exact-ID cache checks; 10 maximum-list Reader/Capture checks in debug and ASan/UBSan; 13 quest snapshot/encoder and 11 service checks. |
| Map presentation | 35 current model/ACK/presenter checks; 61 Lua 5.1 player/quest lifecycle checks; 702 geometry checks across 26 sheets. 107 paper/player asset files are byte-identical to D80. |
| Controls/profile |Walking/dodge regression and 45% prompt policy pass. 109 live controls exercised; NPC life/listening and smith assistance pass all eight independent ON/OFF persistence combinations. Uploaded profile comparison preserves all unrelated effective values. |
| Logging | 10 checks execute the production initialization, worker output and recorder toggling with explicit process-logger/Windows leaves. Actual DMK API and callback constraints were separately reviewed. |
| Source layout |check_subsystems.py passes: 54 original production TUs retained exactly once, 17 static targets, disjoint frozen ownership and optional proxy retained. |
| Companion |Current gameplay PAK generated successfully; 299 source overlay members verified byte-for-byte, archive CRC passed, optional gore members 0. |

Causal controls were also executed. Untouched D80 fails 223 contact and 12 mounted
assertions in the same compiling fixtures. Ten broken crossbow variants, the NPC
permanent-wait restoration, repeated map name lookup and repeated map memory
queries are detected. These controls demonstrate that the tests distinguish the
repaired behavior from the relevant failure; they do not prove engine rendering.

Full frik host syntax was compared with untouched D80: there were no new normalized
errors, but the host compile still has 38 preexisting platform/shim errors versus
39 in D80. Both complete camera TUs stop at missing d3d12.h. This comparison is
**not a successful whole-project build**. The changed camera adapter itself was
compiled and executed in its 25-check fixture. Windows diagnostics fixtures and
link wiring were updated to use the real shared logger, but not executed here.

## Matching companion

The prepared main PAK SHA256 is:
`5ecb749313f8224345d753fe679312aaf66b7c39038b4d4f1201d389872ee611`.
It contains the new alchemy sound Lua, NPC dialogue changes, map runtime and dark
POI geometry. Its size is 108,283,819 bytes. Normal build_release regenerates this
matching companion from the included seed/overlays; do not combine an older DLL
with a separately prepared new PAK as if that were a validated D81 installation.

## Profile preservation

The uploaded INI SHA256 is:
`0945b50a3e6ecc2ac9ac0d4881420fe1324db795a150467db7c3c5f4fa279ada`.
It has 844 effective section/key entries. The D81 profile has 845, adding one
migration marker. Only five preference names change: VrMinigamePromptScale=.45,
VrSmithSemiAnchorsD67=true, VrLeftChestKnives=false, VrLivelyNpcD73=true and
VrNpcListeningGesturesD76=true. The NPC duplicates in VRIK.Presentation agree.
All other effective values, including calibration, match the uploaded file.
VrAlchemyTransferSpeedD65=8, physical crossbow ON/fast transitions OFF, FPS cap OFF,
map backend, walking OFF/dodge ON and accepted motion settings remain as supplied.

## Reproduction and evidence

Focused runners are in tools/check_pose_repairs_d81.py,
check_crossbow_d81.py, check_npc_d81.py, check_map_reader_d81.py and
check_root_repairs_d81.py. Crafting and pickup fixtures are registered in the
focused tests/d80_tests.cmake set; historical names are retained for compatibility.
Source tests include the actual Lua scripts. Lua fixtures require Lua 5.1 through
Lupa or their advertised interpreter option. Generated output belongs outside
the source tree. Some runners require the supplied baseline archive or an exact
retail input; missing inputs must be reported rather than replaced with guesses.

The accompanying patch includes compact evidence reports, exact successful
compiler commands and result JSON. It omits raw logs and test executables.
Historical D80 Windows validation remains in VALIDATION_D80.md and must not be
attributed to D81. A normal Windows release build and in-game behavior are still
separate acceptance steps.

## Remaining limits

Sparse hand contact can still clip where mandatory current coverage is unavailable,
and a 20 mm station display cap cannot guarantee full mesh clearance. Precise-mode
runtime behavior was not established by the supplied CONTACT_LINK rows.
Native hammer quality/hit acceptance, audible alchemy sound, all physical crossbow
variants, final dice/rider/NPC skinning and actual recovered FPS remain unexecuted.
The logged static pickup refusal has no item name; it cannot be called a confirmed
milk refusal. Halberd hit reliability and native dodge acceptance remain unresolved.
D81 does not add physical dice control or reliable general corpse/living-NPC grabs.
