# R105: HUD, OpenXR recovery and native spanner grip

Build marker: `KCD2VR-R105-HUD-STEAMVR-SPANNER-20261105`

This repair starts from the supplied R104 source and preserves its working physics,
combat and reload state machine. The main changes address concrete controller,
frame-submission, session-recovery and HUD-visibility defects. The medium
crossbow spanner now derives its grip from the native rig reference rather than
freezing an animated slot offset.

This is a source delivery. Host tests execute the relevant production code with
explicit substitutes for the unavailable runtime and engine calls. A Windows DLL,
the game, SteamVR, Meta/Oculus OpenXR, VDXR and a headset were not run here.

## Source and recording provenance

The authoritative new input is `source(6).zip`, 145,826,068 bytes, SHA-256:

```text
7e06a31156fddeeff0be2eb5a507bedf970ceee78e3aba9da2c360c24dc5f8cf
```

Its marker is `KCD2VR-R104-STEAMVR-LAYERS-20261105`. It was imported as
`5cead364a424ac6abefd2d556e39ac61c9c6e907`; the archive's foreign Git pointer was
excluded. The delivery manifest records the final source commit and hashes.

The supplied recordings identify different, earlier builds:

| Recording | Build marker | Recorded `xrEndFrame` result receipts |
| --- | --- | ---: |
| Main `KCD2VR.log` | R97 merged Claude/Codex | 50,883, all code 0 |
| Extra `KCD2VR.log` | R101 corpse-follow | 32,293, all code 0 |
| Main `KCD2VR1.log` | R94R1 | 7,916, all code 0 |
| Main `KCD2VR2.log` | R94R1 | 15,413, all code 0 |

These files have no R102-or-later runtime/input/layer diagnostic rows. They provide
historical context, not a reproduction of the supplied R104 layer retry or the
new R105 behavior. A successful end-frame receipt also does not prove which
pixels the headset displayed. The intermittent upside-down startup has not been
causally reproduced from these recordings.

## OpenXR and SteamVR repairs

### One submission per begun frame

R104 called `xrEndFrame` again with fewer layers after the first call failed.
It then treated a second-call `XR_ERROR_CALL_ORDER_INVALID` as evidence against
the overlays. This could suppress healthy HUD and laser layers for seconds,
including increasingly long delays attributed to whichever layer returned most
recently. A generic runtime error cannot establish that attribution.

R105 submits once. Recovery proceeds through the next frame's ordinary
`xrWaitFrame` / `xrBeginFrame` sequence. The timed overlay quarantine is removed.
Every current overlay still receives local pose, rectangle, space and size
validation; an invalid current overlay does not disable that kind in later frames.
The runtime layer budget is enforced while adding layers, so the receipt mask
describes only layers actually in the submission.

Two numeric validation defects were also corrected: finite large quaternion
components no longer overflow a float squared norm into a zero normalized
quaternion, and rectangle extent clamping no longer depends on signed addition
that can overflow. A null layer reference space is rejected locally.

This follows the [Khronos xrEndFrame contract](https://registry.khronos.org/OpenXR/specs/1.0/man/html/xrEndFrame.html),
including its recommendation to discard recoverable-error frames rather than
resubmit different parameters. The actual production assembly and submission
code is compiled in the regression fixture; the OpenXR callback is substituted.

### Current controller state, independent recovery

R104's float, vector and button action readers could return failure while leaving
the previous values or edges in place. A partially successful sync could therefore
publish old controls alongside a current pose. Focus loss also retained levels.

R105 begins each sync from neutral outputs and repopulates the actions that
succeed. A failed action does not remove another healthy action or hand. Focus
loss and sync failure clear old input, and the next successful sync restores
available controls without recreating or reattaching the action set.

Manager paths that return before input sync now invalidate local controller
outputs. The pose-only grip lookup initializes its output flags, poses and
velocities even when it exits early. Session-stop cleanup retires local haptic
receipts so shutdown does not send a haptic-stop call after the session has ended.

The existing Touch, Index, Vive, WMR and simple-controller suggestions are retained.
No runtime name selects a different gameplay algorithm or guessed axis flip.

### Startup, pause/resume and reference-space changes

The old startup path allowed only 500 ms for `READY`, then tore down VR. R105
continues polling while the caller remains active, with 25 ms sleeps and sparse
status messages. It responds to cancellation and terminal events/errors. It adds
no delay after a healthy `READY` event and does not change live pose timing.

The old frame function stopped polling events as soon as its running flag became
false. R105 polls before that admission check. `STOPPING` ends the native session
and clears its input; a subsequent `READY` begins the same session with its
existing action set and OpenXR clock conversion. Ordinary focus changes do not
restart the session. The behavior follows the
[Khronos session lifecycle](https://registry.khronos.org/OpenXR/specs/1.1/html/xrspec.html#session-lifecycle).

At a scheduled LOCAL reference-space change, the existing recenter request still
occurs at the appropriate predicted display time. R105 also discards the old flat
UI, debug HUD and lag-HUD anchors so they can be captured in the new space.

Automatic recreation after actual session/instance loss is outside this repair.
Terminal loss clears input; it is distinct from an ordinary stopped session
resuming. Initial action creation/attachment failures are also distinct from the
repaired transient read/focus failures.

### Upside-down startup and diagnostics

R105 records the first valid head pose after startup or a reference change,
including flags, quaternion, position and the orientation's up-axis component.
It also reports session recovery and exact HUD swapchain creation failure codes.
These use the existing log without requiring a diagnostic capture.

The head quaternion is not flipped or reinterpreted. No stricter TRACKED-bit gate
was introduced: valid inferred poses are permitted by OpenXR. The accepted head
sampling, calibration math, fresh-hand acquisition and camera publication remain
unchanged. These changes improve recovery and evidence; they do not prove the
reported inverted startup is eliminated.

## HUD behavior and defaults

| Element | R105 default behavior |
| --- | --- |
| Health/stamina and status | Reveal on an open right-palm glance |
| Compass | Reveal on an open left-palm glance |
| Weapon/potion selectors | Hidden by their own permission |
| Crosshair | Hidden |
| Ordinary interaction hints | Hidden |
| Spoken subtitle/dialogue text | Hidden |
| Dialogue response choices | Retain native behavior |
| Native menu/minigame controls | Retain their own presentation owner |
| Transient event/quest notices | Retain native behavior |

### Selector and native-writer ownership

R104's corrected-HUD group resolver explicitly excluded `QAMWeapon` and
`QAMFood` from visibility ownership. R105 gives these selectors an independent,
reversible permission instead of coupling them to the health/stamina group.
This hides their presentation without disabling weapon selection, physical
holsters, inventory operations or the native quick-selection action map.

The existing verified Flash child boundaries now recognize the exact selector,
subtitle and hint paths as well as their known names. The native display-info
visibility bit is handled for these explicit permissions while preserving the
latest native visibility intent. Nested native visibility calls do not overwrite
that intent with the mod's filtered value. Re-enabling an element restores native
intent; it does not force a closed selector open.

Spoken subtitles use `bc.subtitles`. Dialogue response containers `cl` and `cr`
are not included in this ownership. The existing `wh_ui_ShowHints` lease supplies
the ordinary gameplay hint setting and restores its baseline when a protected
native-control context owns presentation. There is no new global HUD-off command.

### Palm response

Each hand has an independent 150 ms open-palm/gaze dwell. The former one-second
dwell and three-second forced expiry are removed. A revealed group remains
readable while that palm is open and viewed, and hides when the hand closes,
turns away or loses current validity. A saved OFF permission remains OFF even
when a palm is revealed. The policy does not require VRIK to be enabled.

This is hand-triggered visibility on the existing HUD reading plane. It does not
create physically wrist-attached 3D panels. Native quest/event notifications
continue independently of palm reveal.

### Reading distance and peripheral comfort

The ordinary HUD plane moves from 1.55 m to 3 m. The semantic readable region
uses a more central binocular fit, changing its tangent-space comfort factors
from 0.88/0.78 to 0.80/0.70. This is intended to reduce near-plane disparity and
the amount of text at the far edges. It is not a measured cure for all double
images or runtime-specific overlay behavior.

The complete capture is still available for full-field effects. The size sanity
bound and width-search bound scale with distance, preserving the previous
angular coverage limits for wide/canted optics. Without this adjustment, a valid
wide-eye fixture would have been incorrectly rejected after moving the plane.
Drawing and pointer mapping use the same surface dimensions and distance.

Normal menu surfaces retain their own depths. Dice retains its independent
0.45–1.55 m placement and unchanged production geometry. The pre-existing optional
HUD orientation lag remains; full-edge coverage under every lag pose is not
claimed. Headset comfort and the reported peripheral double image remain visual
checks.

### Existing INI migration

Keep the installed INI and calibration. R105 migrates exactly these eight HUD
settings once, then writes `VrMinimalHudDefaultsR105=1` only after all succeed:

```ini
VrContextualHud=true
VrHudCrosshairVisible=false
VrHudLowerIconsVisible=true
VrHudCompassVisible=true
VrHudStatusIconsVisible=true
VrHudQuickSlotsVisible=false
VrHudSubtitlesVisible=false
VrShowInteractionHints=false
```

Later saved choices are retained. The marker also follows all eight keys during
an explicit settings save. Physics, body calibration, reload and other settings
are outside this migration. The supplied main and extra INIs were replayed
through the actual policy/config adapters to verify unrelated settings survive.

`VrTimedInteractionPrompts` retains its existing value; the new ordinary hint
default is controlled by `VrShowInteractionHints` and the scoped visibility
permission. Essential native menu/minigame controls have their own ownership.

## Medium crossbow spanner

R104 captured the hand-to-tool relationship from the animated original palette
after its native spawn-edge/two-body-sample admission. Two samples did not prove
that the slot had returned to its authored reference transform. A clip-specific
slot offset or rotation could become the permanent grip for that binding.

R105 uses the current rig's validated reference hand and socket plus the current
native attachment descriptor, following the reference-basis approach of the
working manual arrow. If the reference hand is H, socket S, attachment A and
current solved wrist W, the grip is `inverse(S*A)*H`, and the item placement
remains `W*inverse(grip)`. The authored grip therefore maps to W.

The existing placement formula and scale handling stay intact. There is no fixed
180-degree flip, guessed mesh offset or replacement tool. Missing reference data
can recover at the next eligible sample without a persistent failure latch.
Native spawn timing, ownership/binding identity, clock/event permissions,
ammunition accounting, bolt paths and the heavy mounted mechanism are unchanged.

The production builder and writers pass tests for animated/reference divergence,
scaled bodies, late reference arrival, identity changes, retained timing and the
heavy mechanism. These tests do not inspect the actual spanner mesh in a headset.
The historical recordings contain no tool-grip transform receipt that would
establish the exact reported angle.

## Validation and preserved scope

| Production path | Current host result | Negative/control evidence |
| --- | --- | --- |
| Layer assembly and `xrEndFrame` | 68 checks pass; optimized and ASan/UBSan | Supplied R104 fails 25 assertions |
| OpenXR input | 54 checks pass; optimized and ASan/UBSan | R104 fails 11 of 39 baseline assertions |
| Startup/session/reference recovery | 36 checks pass; optimized and ASan/UBSan | R104 fails 20 of 33 baseline assertions |
| HUD policy/config/main-loop/CVar adapters | 1,630 checks pass in each O0/O2 run | Supplied main/extra profiles and failure/retry migration cases |
| Full FlashUI TU and production native visibility workers | 463 checks pass in each O0/O2 run | Exact native identity, restoration, nested visibility and context cases |
| HUD cross-version contract | 14 checks pass in each O0/O2 run | Original R104 fails 11 of 14 assertions |
| Spanner builder, caller and native writers | 398 checks pass in each O0/O2/ASan+UBSan run | Original builder fails authored-grip assertion; 57 retained baseline checks pass |
| HUD optics and pointer integration | 451 checks pass in each O0/O2 run; six integration checks | Fixed-30 m cap control fails the wide/canted coverage assertion |

Assertion counts are not counts of independent bugs or headset scenarios.
Address/undefined-behavior instrumentation remained active; only leak detection
was disabled for the host's ptrace restriction. Host exception substitutes do not
verify Windows SEH or SDK ABI behavior.

Of 1,419 existing tracked files under `src`, 1,400 remain byte-identical to R104.
The 19 modified files are the listed HUD/input/XR/spanner surfaces and build
identity. There is one new HUD policy header. The physical combat/physics
subsystems, normal reload client and accepted camera source are retained.
Ten explicit head/tracking function/file comparisons also match the baseline.

The supplied R104 archive omitted its historical tests tree, including seven
files required by its unchanged CMake source-ownership check. Those assets were
recovered byte-for-byte from the previously saved R96R1 source archive; no current
production file or new R105 test was overwritten. The 1,616 restored historical
test assets are recorded in `R105_RESTORED_TESTS.json` and are not represented as
newly executed against R105. The original ownership check now passes unchanged.

The bundled pinned MSVC dependency restores and verifies offline. The matching
main companion prepares successfully for the R105 marker, with the same gameplay
PAK hash as the supplied R104 preparation:

```text
71544b7e3ff64c53fa828c3a21b2ea2a3a539f720aeeb1a1145cae23c578be89
```

Detailed receipts, source hashes, compiler configurations and recording inventory
are in `R105_VALIDATION.json`. The export verifies every archived file against
its source hash. This does not substitute for a Windows DLL link or a game run.

## Building and checking the result

Extract into a fresh source directory and run `build_release.bat` on Windows with
the documented Visual Studio C++, CMake and Python prerequisites. The build
produces the binary and matching main companion in `dist/prerelease`; it does not
install into the game. Keep the installed INI and calibration when updating.

Focused host runners are included and do not need the private Git checkout:

```text
python3 tools/check_subsystems.py
python3 tools/test_xr_composition_r105.py
python3 tools/check_xr_input_r105.py --skip-baseline
python3 tools/check_xr_session_r105.py --skip-baseline
python3 tools/test_hud_r105.py --out ../hud-check --no-baseline
python3 tools/check_spanner_grip_r105.py --out ../spanner-check
python3 tools/check_hud_geometry_r105.py --out ../geometry-check
```

The host runners require GCC/Clang as documented in their arguments; they are
separate from the Windows release build. Exact original-version negative
controls use the explicit baseline options and are not invented when history is
absent.

The remaining game checks should focus on the changed behavior: a cold SteamVR
start and recalibration; dashboard/focus loss followed by return; right/left palm
reveal and immediate hide; repeated weapon/potion selection with selectors off;
subtitle suppression with usable dialogue choices; and medium-spanner grip while
completing the already-working reload. Meta/Oculus OpenXR and VDXR need their own
runtime smoke runs. A current R105 log can distinguish runtime startup poses,
input availability and actual layer submission without treating the old logs as
evidence of the new build.
