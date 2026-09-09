# KCD2VR V60.8 — Camera Offset / UI Research Test

## Purpose
Test the hypothesis that changing KCD2 FOV mostly changes world coverage while Henry / near-body scale remains visually too large, and that a physical local-space camera translation backward is the missing control.

## New menu sliders
- **Camera back / forward (+ = backward): -0.30 m .. +0.30 m**
- **Camera vertical (+ = up): -0.30 m .. +0.30 m**

The offsets are applied in the game's camera-local basis at the same reversible published-camera injection point used for HMD tracking. They do not change FOV or compositor zoom. They remain active even if tracked head-position translation is disabled.

## Suggested test
1. Keep the best current FOV/pseudo baseline unchanged.
2. Set vertical = 0.00.
3. Move Camera Back slowly: +0.05, +0.10, +0.15, +0.20 m.
4. Stop if body/weapon clipping becomes objectionable.
5. Then adjust vertical only if the eye-height/body alignment needs correction.

Report: best Camera Back value, best Camera Vertical value, whether Henry/arms/weapon became more naturally scaled, and whether near clipping appeared.

## UI research included
See `V60_8_UI_ENGINE_FINDINGS.md`. No risky live Scaleform memory write is enabled in this build.
