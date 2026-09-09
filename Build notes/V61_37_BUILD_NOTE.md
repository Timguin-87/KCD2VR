# KCD2VR V61.37 CLEAN DEPTH .70-1.00 + LIVE UI TEST

This source is a focused clean follow-up to V61.36.

## Presentation modes

0. OLD MONO — pre-F8/correction presentation.
1. CORRECTED MONO — F8-derived projection/Center-Y foundation, default trim +0.080.
2. PSEUDO .90 — fixed 0.90 eye-asymmetry pseudo stereo.
3. DEPTH PSEUDO .70-1.00 — per-pixel depth-conditioned eye asymmetry. Far/background remains 1.00. Near pixels can move toward 0.70 according to the Depth Influence slider.

Depth Influence is 0.00..1.00, default 0.30:
- 0.00: all pixels use 1.00 asymmetry.
- 0.50: nearest pixels can reach about 0.85; far remains 1.00.
- 1.00: nearest pixels can reach 0.70; far remains 1.00.

The depth path does not perform full DIBR/reprojection. It samples the already-proven screen-sized KCD2 DSV and only changes the presentation's eye-asymmetry interpolation. Generic per-draw depth scoring remains disabled.

## Dialogue/FOV rule

V61.37 does not write cl_fov in response to dialogue/inventory/bow transient framing. Game FOV, source calibration FOV, V61.12 culling FOV and OpenXR submitted eye FOV ownership remain separate. The retired V61.33 transient FOV pump is not re-enabled.

## Live UI session controls

Two bounded session-only controls are exposed in the old F7 menu:
- LIVE HUD scale: 50%..100%, default 70%.
- Dialogue/subtitle text: 50%..150%, default 100%.

They use CryEngine FlashUI semantic calls through the existing guarded console bridge. HUD scale calls UIAction.SetScale on HUD / MasterSwitch instance 0. Dialogue text calls HUD's XML-declared SetSubtitleSize function through UIAction.CallFunction. No HUD XML viewport rewrite and no runtime INI save are performed.

These UI calls are still runtime experiments: live HUD lookup/child control is proven in earlier builds, but this exact root scale call/units are not headset-proven yet.

## Clean runtime preserved

- High-frequency debug_trace remains compiled to a no-op.
- Structured logger default remains WARN.
- No FRIK/final-pose diagnostic hook installation.
- No F4/F8/F9 diagnostic hotkey suite.
- No per-draw depth hooks.
- Runtime Save Settings remains disabled.
- V61.12 gameplay-only culling path is untouched.
