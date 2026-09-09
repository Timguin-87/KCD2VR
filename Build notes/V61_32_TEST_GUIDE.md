# KCD2VR V61.32 — Combined headset test guide

This is a **source-only, not-yet-Windows/MSVC-verified** combined build. Test from a clean known-good V61.12/GOOD MONO baseline and keep the debug log for this first pass.

## 0. Baseline

Before hotkeys:
- confirm normal GOOD MONO behaves exactly as before;
- confirm gameplay head tracking and V61.12 culling remain correct;
- do not use retired Insert/Home/legacy stereo experiments.

Recommended first settings:
- source resolution: existing 1920x1800 baseline unless testing Resolution tab separately;
- F8 trims: `0.0 deg`, Center-Y `0.000`;
- Presentation Zoom: `1.00`;
- Render: Mono for the first projection test.

## 1. F6 — restored virtual screen

Press **F6**.

Expected:
- game moves to the LOCAL/world-locked virtual screen;
- head rotation is independent of the screen;
- no corrected-projection test should start.

While F6 is ON, hold **Right Grip**:
- stick Y = screen distance;
- stick X = screen scale.

Check the new starting scale around **1.42**. If inventory/dialogue feels too small/large, tune it once in the UI tab or with Right Grip + Stick; it persists via Save Settings.

Press F6 again and verify immersive mode returns cleanly.

## 2. F8 — automatic XR-derived projection

Press **F8 once** in ordinary gameplay.

Do **not** manually change anything for the first 10–15 seconds. Look for:

`V61.32-F8 AUTO STATUS ... coverage=YES`

The important fields are:
- `actualGameplayVFOV`
- `target`
- `requiredCentered`
- `requiredNoShift`
- `sourceMapper`
- `centerY={auto,trim,effective}`
- `zoom`

Visual questions:
- are straight vertical/horizontal structures less warped than V61.30 F6?
- does the world scale feel natural?
- is the bottom/peripheral coverage adequate?
- does head rotation remain geometrically stable?

### Manual correction, only after the automatic baseline

Open the menu -> **FOV / Projection**.

Change one thing at a time:
1. F8 Auto FOV trim: start with +/-0.5 or 1.0 degrees.
2. F8 optical-center trim: start with +/-0.005 or 0.010.
3. Presentation Zoom: use small steps around 1.00; the release-facing range is deliberately narrow at 0.95–1.05.

The intent is **automatic calibration plus personal correction**, not a required manual calibration process.

Press F8 again. Verify the pre-F8 game FOV, source FOV and Center-Y are restored.

## 3. Mono / Fake Stereo on the same F8 base

Turn F8 ON again.

Compare:
- Render -> Mono
- Render -> Fake Stereo

F8 should keep deriving the source VFOV/Center-Y in both modes; only the safe pseudo strength differs.

Native Stereo remains locked and should not be tested from this menu.

## 4. F4 — focus-driven pseudo

Prefer testing this **after F8 is already visually acceptable**.

Press **F4 once**. Move/look alternately at:
- a very near object;
- a mid-distance object;
- distant scenery.

Useful log marker:

`V61.32-F4 SOURCE-VALID focus=... target=... smooth=...`

If the engine focus source is not usable:

`V61.32-F4 SOURCE-MISSING ...`

There is deliberately **no depth-resource probe** in this build. Do not expect G-buffer markers.

Judge whether near/far pseudo strength changes feel more natural than a single fixed `.95` value and whether it adds discomfort/scale pumping.

Press F4 again to restore the previous pseudo state.

## 5. F9 — READ, then READ+WRITE

Make Henry's arms/hands clearly visible.

Press **F9 once**.

Expected beep/state:
- READ
- log: `V61.32-F9 STATE READ (no writes)`
- final-skin `MATCH` lines may appear
- there must be **no `HAND-APPLY` in READ mode**

Wait several seconds.

Press **F9 a second time**.

Expected:
- READ+WRITE
- log: `V61.32-F9 STATE READ+WRITE ...`
- when the exact validated gate-2 path occurs, `V61.32-FINAL-SKIN HAND-APPLY` / `HAND-RESTORE` may appear.

Observe hands and held sword. The previous V61.30 runtime already produced real hand writes without visible hand movement; this second stage is retained for clean read-vs-write timing comparison, not as a new VRIK ownership claim.

Press F9 a third time -> OFF.

## 6. Native control pass

Test ordinary walking first, then combat.

Expected mappings:
- Left Stick = move/strafe
- LT = block
- RT = attack; hold RT for KCD2's native held/heavy behavior
- Right Grip + Right Stick = directional weapon control / unchanged bow aim rate
- Right Stick inner range = slower turn
- hard Right Stick left/right = one latched opponent-switch gesture per deflection
- B short = weapon swap; B hold = draw/holster fallback
- Y short/hold = horse/companion family

### Bow

Bow aim rate should feel the same as the prior working build.

During a draw/aim, check whether the native zoom/warp is suppressed. Useful markers:
- `V61.32-BOW-ZOOM-GUARD armed`
- `... suppress ...` only if actual gameplay FOV really collapses
- `... release restoreGame=...`

The guard is heuristic until a real bow-state/action owner is found. If it triggers incorrectly during melee Right-Grip use, report the exact action and log timestamp; it can be disabled with `EnableBowZoomGuard=false`.

## 7. VR menu

Open F7 / Left X and verify:
- right-controller laser/reticle follows the book;
- all eight accordion tabs open/close smoothly;
- no old dense right-page diagnostic text returns.

Test the new tabs specifically:

### FOV / Projection
- four sliders work with the Touch ray;
- F8 status shows ON/OFF correctly;
- trims persist after Save Settings.

### Optimized Defaults
- Default / VR Optimized selection is clickable;
- FPS slider covers 60–120;
- View Distance covers 30–300;
- Apply should log one unified view-distance write and readback for all three CVars.

Suggested VR starting point: **90 FPS / 120 view distance**.

### UI
This is virtual-screen size, not native HUD scaling. Confirm the new 1.42 starting point is closer to the desired inventory/dialogue framing.

## 8. What to send back

Please send:
- `KCD2VR.log`
- `KCD2VR-debug.log`
- brief visual notes for F8 Mono, F8 Fake Stereo, F4, F9 READ/WRITE, bow zoom, and virtual-screen scale.

Especially useful answers:
- best F8 trims if zero is not correct;
- whether F8 reduces warp compared with V61.30 F6;
- whether F4 focus changes are perceptually useful;
- whether READ mode truly has zero HAND-APPLY lines;
- whether the bow zoom guard fires only during bow zoom;
- whether 1.42 is a better inventory/dialogue screen size.
