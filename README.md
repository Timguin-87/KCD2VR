# KCD2VR

Tracked hands, weapons and room movement for Kingdom Come: Deliverance II.
The game owns combat results, inventory, recipes and interaction legality.
The VR side supplies tracked movement and the presentation of those actions.

## Current source: R105 — HUD, OpenXR recovery and native spanner grip

R105 continues from the supplied R104 source. It repairs stale controller input,
OpenXR frame/session recovery, the weapon/potion selector visibility bypass and
the medium crossbow's animated spanner grip capture. Ordinary HUD content uses a
farther reading plane with a more central binocular safe area.

The requested defaults reveal health/stamina/status on an open right-palm glance
and the compass on an open left-palm glance. Weapon/potion selectors, the
crosshair, ordinary interaction hints and spoken subtitle text start hidden.
Dialogue response choices, native menu controls and transient event notices stay
available. This is hand-triggered reveal of native HUD elements on the existing
reading plane; it is not a new wrist-mounted rendering system.

Extract this complete package into a new source directory and run
`build_release.bat` on Windows. No earlier patch overlay is required. This package
contains source, tests and build inputs; a Windows DLL has not been built here,
and the changes have not been tested in the game or a headset. Build the matching
main companion PAK with the binary. Keep your installed INI and calibration.

A one-time migration changes only the eight HUD preferences requested for this
repair. It records completion after all writes succeed, and later saved choices
remain yours. Physics, combat, body calibration, head-follow timing and reload
settings are preserved.

See [the R105 repair review](docs/R105_REPAIR_REVIEW.md) and
[validation evidence](docs/R105_VALIDATION.json) for exact scope, reproducible
checks, old-log provenance and the remaining visual/runtime checks. Historical
R96/R96R1 and earlier reviews describe their own revisions and are not new R105
headset evidence. The historical tests omitted from the supplied R104 archive
were restored without changing current production files; their provenance is in
[R105_RESTORED_TESTS.json](docs/R105_RESTORED_TESTS.json).

The default hand timing remains the accepted fresh-pose mode. Fresh Pose
Uncapped is available as a separate saved choice below. The supplied INI is used
only when no installed INI exists; the small R105 HUD migration also applies to
existing installations. Optional gore remains a separate package.

### The new timing choice

Open **Home > VRIK EXPERIMENTS > Fresh Pose Uncapped**. The same choice is on
**Home > EXPERIMENTS / LIVE SETTINGS > VRIK+ > Motion timing**. These retain the
existing developer-mode visibility. Selecting it applies it live and saves
`VrMotionTrialD67=4`; returning to **FRESH POSE timing** restores the existing
paced behavior. The saved/missing-key default remains 2.

Uncapped skips the game-thread XR-token waits. Once per native frame, before
the ordinary update, it attempts a fresh hand family using try-locks. Predicted
head modes use a bounded, coherent runtime prediction reference; current-time
mode uses the current runtime time. The head in this family is only the hands'
coordinate reference. It does not replace the camera head or its publication.
Failed attempts retain the completed pose and its actual age. This is an added
tracking sample at an existing supported boundary, not a new GPU palette writer
or a promise of a particular FPS. Manual caps, runtime cadence and GPU cost are
separate. F2/F7 can record the new route without re-enabling its token wait.

The `.cmd` opens the setup export in the installed mod binary. **Native (no
upscaling) + Save & start** explicitly writes `r_SuperResolution_Mode = 0`;
the save now verifies the staged and final file before reporting success.
**Keep settings & start** retains its existing discard-edits behavior.

The retained R1 changes are:

- One anatomical elbow destination for ordinary and bow arms, with fixed arm
  lengths and the admitted wrist preserved. Bow admission retains its shoulder
  basis and valid bend history. This addresses implementation defects instead
  of asking for another elbow-settings sweep.
- Separate dice/activity reference lifetimes, preventing an activity pose from
  becoming the ordinary body reference. Leaning during dice remains available.
- Less exaggerated lag and critically damped recoil for supported polearms.
- A completed-eye capture transaction for temporal upscalers, recoverable DLSS
  reset mismatches, and live-switch success based on a new stereo publication.
- Narrow alchemy acceleration: Henry's qualified pickup/return and pre-work
  turn can advance quickly; book/station changes and distillation keep native
  timing. The actual work/effect boundaries remain owned by real gestures.
- Pickup preparation installed before the first eligible Lua Use, with bounded
  F5 reasons for refused preparation. Existing rigid-body and inventory checks
  remain in place.
- A current-pose observer repair for crossbow string/tool geometry, an exact
  native no-opponent kick-query correction, and cooldown only for an observed
  kick request. Complete live reloads and all kick contexts remain unverified.
- Open, face-facing, looked-at palm HUD with a three-second maximum and twice
  the tested station prompt size.

The R2 corrections are:

- Corpse search receipts describe background work, not attempted grabs. Idle
  service work is throttled before native/script setup, distant current body
  bounds can avoid joint scans, and closely timed nearby hands can share a
  bounded discovery list. Remaining full entity traversals and unmeasured game
  frame cost are explicit; this is not a proven drag or FPS result.
- Ordinary inventory pickup respects a qualified requesting hand and prepares
  eligible static inventory items in both existing hold modes. Scenery props
  without native selection/ownership remain a separate unfinished feature.
- The post-dice report concerns a frozen old image after a 0.95 presentation
  zoom edit. That scalar is distinct from 95-degree native FOV. Gated receipts
  now join actual zoom application with publication/fault observations; the R1
  stereo-publication and activity-reference repairs address separate defects.
- Existing crossbow recordings are reviewed through native clock, completed
  pose and mechanism ownership. Finished inert duplicate clips, the UpperBody
  source route and same-clock newer-wrist admission receive bounded repairs.
  Unqualified native event intervals are not bypassed to manufacture success.
  Existing F5/F7/F11 captures now retain exact clock/interval refusal evidence
  with finite per-session budgets and no extra native traversal.
- F7 now records the elbow reference/conversion inputs and exact final-arm
  observation identity. No old elbow preset is treated as correct, and smoother
  numerical output is not called proven natural anatomy.
- The one-time movement-option override is removed. Exit-saved INI values do
  not establish earlier live state, and changing a flag is not a 6DOF repair.

The supplied starter INI preserves the user's uploaded exit profile. Only
station prompt scale (0.45 to 0.90) and the existing R1 migration marker differ
in setting values. Movement flags and calibration are preserved. The normal
release build preserves an existing game INI. The one-time migration applies
only the requested prompt size; later saved prompt changes remain effective.

## Build and integration

Use the complete `KCD2VR_SOURCE` directory from the R105 archive. Build in that
source directory, separate from the game installation. Historical patch and
revert instructions in earlier review notes do not apply to this full archive.

On Windows, install Visual Studio 2022's C++ desktop tools, CMake and Python 3,
then run:

```bat
build_release.bat
```

The output goes to `dist/prerelease`; building does not install it. Use the DLL
and its matching main companion together. The build regenerates that companion
from the verified gameplay donor plus current source overlays. The donor PAK's
older scripts are intentional; their age alone does not identify the installed
companion or prove a packaging failure. The optional gore package stays separate.

The source archive contains no newly compiled DLL/ASI, raw recordings or optional
gore payload. Its required gameplay donor PAK is a build input, and current
source overlays produce the main companion during the build. See [building](docs/BUILDING.md) and
[dependencies](docs/DEPENDENCIES.md) for the normal build requirements.

## Settings and diagnostics

Read [settings](docs/SETTINGS.md), [diagnostics](docs/DIAGNOSTICS.md) and
[known issues](docs/KNOWN_ISSUES.md). Gameplay does not require a diagnostic key.
F2 records bugs/rendering, F3 combat, F5 interactions and minigames, Shift+F7 tracked or
native pose behavior, and F9 physics. F7 opens/closes the VR menu, F6 switches between
Theater (flat screen) and Immersive until you press it again or the next menu/scene change. Existing finite capture sessions can run
together. F11 remains available for the older dedicated reload capture.

Diagnostic receipts use the existing `KCD2VR.log`. Runtime identity, R105
frame/session recovery and the first head pose after startup or a reference
change are visible without an F2 capture; detailed feature captures keep their
existing gates. No fingers, map-performance, companion-startup or hook-library
sidecar is introduced. The existing last-good INI checkpoint is a settings
recovery file.

Knives are active in the uploaded D85R5 left-hip service, despite the retained
`VrLeftChestKnives` key name. The current logs show zero recognized throwing
stock. This patch improves the evidence and corrects the misleading retired
label; it does not grant inventory or prove a completed throw.

## History and constraints

Earlier notes are retained as dated implementation history. They do not certify
later source, and their old defaults must not be read as current settings.

| Revision | Relevant history |
| --- | --- |
| D85R5R3 | [Fresh Pose Uncapped and verified Native launcher save](docs/D85R5R3_FRESH_POSE_UNCAPPED.md) |
| D85R5R2 | [Corrective review](docs/D85R5R2_CORRECTIVE_REVIEW.md) |
| D85R5R1 | [Amended R1 repair status](docs/D85R5R1_REPAIR_STATUS.md) |
| D85 | XeSS/live-switch implementation; the old temporal timeout fallback is superseded by this patch. |
| D84R1 / D84 | [Masterplan](docs/D84R1_MASTERPLAN_STATUS.md), [regression ledger](docs/REGRESSION_LEDGER_D84.md) and the prior 50-log audit. |
| D83R1 | [Reviewed merge](docs/CHANGES_D83R1.md), including smithing re-entry and quiver reservation. |
| D82 | [Changes](docs/CHANGES_D82.md), separate per-eye DLSS/FSR contexts and reload ownership work. |
| D81 | [Changes](docs/CHANGES_D81.md), collision, alchemy, map and log consolidation work. |

Preserve the accepted bow alignment, fresh hand timing, improved contact and
unholstering. H50 and D52 were reported from the same scenes; the performance
regression predates the Fresh Pose comparison. The observed later near-90 XR
token coupling is a separate finding. This patch does not claim a measured
return to C71 frame rate. Existing Fresh Pose is retained alongside the new
explicitly selected uncapped route.
