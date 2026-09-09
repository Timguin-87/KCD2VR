# KCD2VR V62.4 — Book Menu / F6 Dialogue / VRIK Diagnostics Test Guide

## Scope

V62.4 deliberately combines the new uncluttered VR configuration menu with the current evidence-first VRIK diagnostic build. It does **not** add a new skeletal write experiment.

Preserved baselines:
- V61.12 corrected gameplay-only culling pose synchronization.
- Fix head sync: OFF = pose source 0 (`CACHED_XR`), ON = pose source 2 (`SYNC_NOW`).
- V60.1 dialogue/inventory source-FOV guard; no transient `cl_fov` pumping.
- F6 AUTO / FORCE SCREEN / FORCE IMMERSIVE state machine.
- Latest-good automatic Inventory/Map theater behavior.

## F7 book menu

F7 opens the parchment home page. The right page slides during navigation (about 220 ms); the left identity/status page stays fixed.

Home buttons:
- VR CONFIG
- GRAPHICS
- VR VIEW MODES
- VRIK DIAGNOSTICS [F9]
- Best VR settings
- Fix head sync

### VR CONFIG / VIEW
- Head up/down
- Head forward/backward
- Game FOV + dedicated small APPLY button on the right
- VR / Source FOV
- VR zoom/framing
- Y offset / vertical center
- F6 theater zoom
- Recenter / Reset View

Game FOV intentionally does **not** apply just because the slider is released. Use its nearby APPLY button.

### VR CONFIG / HUD
- Enable VR UI correction
- Subtitle size / Y
- Dialogue size / inset
- Interaction icon size / inset / Y
- Crosshair on/off
- Lower icons (`Stats`) on/off
- Compass on/off
- Reset Vanilla / Apply HUD

HUD transform sliders preview while held at a throttled rate (~12 Hz). Logging is separately throttled.

### VR CONFIG / ACTIONS
Dialogue:
- NATIVE CAMERA — automatically enters F6 theater while `iDialogueContextId > 0`.
- 1ST PERSON — stays immersive and keeps the fixed source-FOV guard.

For first-person dialogue V62.4 also attempts to hide the retail `RatioStrips` MovieClip. This is an experimental black-square/letterbox candidate, **not a claimed complete fix**.

Actions:
- Native animation remains active.
- `1ST PERSON [PENDING]` is intentionally disabled. No unproven Mannequin/action-state ABI is called from the ASI.

## Graphics page

Resolution presets:
- Square 1920x1920 / 2304x2304 / 2880x2880 / 3840x3840
- Nearly square 1920x1800 / 2304x2160 / 2880x2700 / 3840x3600
- Portrait 1872x2016 / 2246x2420 / 2808x3024 / 3744x4032

`APPLY + XR REBUILD` first commits KCD2's explicit custom-resolution CVars. Only if readback succeeds does KCD2VR request its existing controlled OpenXR swapchain rebuild at the current XR render scale.

This is **not** described as a full OpenXR session restart; V62.4 uses the safer already-existing swapchain recreate path.

Other graphics controls:
- Best recommended VR graphics
- View distance 30–300 (writes `e_ViewDistRatio`, vegetation distance, and merged-mesh distance together)
- Gamma
- Brightness
- Contrast
- Game FPS cap

These avoid entering KCD2's Graphics menu and pressing its problematic global Apply button.

## VR View Modes
- Mono
- Corrected Mono
- Pseudostereo Dynamic
- Native Stereo [DISABLED]
- Dynamic 3D effect 0.75–0.99

Native Stereo is visible but cannot be activated from the release menu.

## Pointer / stick test

The right-hand menu pointer now prefers the OpenXR `/input/aim/pose`; grip pose is only a fallback. This should allow normal pointing with the controller held farther from the torso.

Horizontal thumbstick input is now page navigation **even when the ray is valid**:
- stick left = previous page
- stick right = next page

The stronger of the left/right stick inputs is accepted, so either controller can page-turn.

## F6 / dialogue test

1. Inventory and Map should behave as in V62.1/V62.3 (automatic theater).
2. Select `VR CONFIG -> ACTIONS -> NATIVE CAMERA` and start a dialogue. It should automatically enter the F6 theater.
3. Leave dialogue: AUTO should return to immersive gameplay.
4. Select `1ST PERSON` and start a dialogue. It should stay immersive; check whether the prior excessive dialogue zoom is gone and whether the black squares/strips disappear.
5. Manual F6 still cycles AUTO -> FORCE SCREEN -> FORCE IMMERSIVE -> AUTO.

## VRIK test

V62.4 is diagnostic-only. F9 toggles low-rate geometry capture. No arm transforms are written.

After 10–15 seconds of F9 ON, collect:
- `KCD2VR.log`
- `KCD2VR-diagnostics.log`

The diagnostic contains semantic UPPERARM/FOREARM/ELBOW/HAND IDs, direct parent IDs, selector-0/selector-1/reference translations, final 0x20 outputs, and candidate final-output DQ arm lengths.

## Build verification

The supplied `build_release.bat` searches the freshly built ASI for:

`KCD2VR-V62.4-BOOKMENU-DIAG-VRIK`

No Windows/MSVC compile is claimed by the source pack created in the Linux analysis environment.
