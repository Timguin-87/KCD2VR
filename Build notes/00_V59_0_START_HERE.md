# KCD2VR V59.0 — Cyclopean Stereo/IPD Calibration

V58.9 phase D produced a strong stereoscopic percept in-headset even though the log proved there were zero native dual-render passes. This build isolates that result instead of mixing it with more renderer experiments.

## What V58.9 proved
- Phase D used one center source image for both eyes.
- Both projection views used one common/cyclopean camera origin.
- OpenXR still received separate asymmetric left/right `XrFovf` values.
- `dualPassExecutions=0`, so this is NOT native two-render stereo.
- The user nevertheless perceived strong stereo, but with crossed/swapped-eye / bad-IPD feeling.

The likely cause is the horizontal asymmetry of the submitted eye FOVs. V59.0 therefore leaves the source image, render path, camera pose, crop mode, and tracking filter identical in all F8 phases and changes ONLY the eye-FOV asymmetry polarity/strength.

## F8 — six 10-second phases
All phases use V58.9 phase-D direct aspect-fill copy + honest common camera origin + RAW tracking.

1. `CYCLO_A_RUNTIME_ASYM_P1_00` — +1.00, exact V58.9 phase D.
2. `CYCLO_B_REVERSED_M1_00` — -1.00, full L/R asymmetry reversal.
3. `CYCLO_C_REVERSED_M0_75` — -0.75.
4. `CYCLO_D_REVERSED_M0_50` — -0.50.
5. `CYCLO_E_REVERSED_M0_25` — -0.25.
6. `CYCLO_F_CENTERED_0_00` — 0.00, centered mono control.

Please report which phase has:
- correct eye polarity (not crossed/swapped),
- most natural perceived depth,
- comfortable IPD/world scale,
- least jitter,
- no black edge.

This is a binocular presentation calibration, not a claim of true geometric stereo. True native stereo still requires distinct scene renders/resources.

## Dialogue test
If possible, enter a dialogue during one of the direct-copy F8 phases and check whether the response/options are visible. The direct aspect-fill path preserves the full vertical source range, unlike some earlier angular remaps, so this is a useful UI comparison.

## Comfort/UI
- Successful world-fixed startup/main-menu F6 LOCAL quad retained.
- Short Left Menu still toggles vanilla inventory together with the world-fixed comfort quad.

## Controls retained
- Left stick: forward/back + strafe.
- Left-stick click: sprint.
- Right stick X: smooth horizontal camera turn.
- Right-stick click: crouch.
- Left Y: jump.
- Right A: interact/select/dialogue.
- Right B: horse/companion context; hold dog call.
- Right Trigger: attack; right stick becomes attack direction while attacking.
- Left Grip: block.
- Left Trigger + left stick: torch / visor / quick weapon / quick consumable.
- Left X: MIS menu.
- Left Menu short: inventory + comfort quad.
- Left Menu hold: pause/Escape.
- Right grip remains reserved.

## Useful log markers
- `V59.0-F8-BEGIN`
- `V59.0-MONO-POSE`
- `V59.0-CYCLO-FOV`
- `V56-F8-END`
- `V56-ANGULAR-WORST`
