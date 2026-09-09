# V38 — VR Presentation + Resolution Manager

V37 proved the KCD2 backbuffer reaches OpenXR. V38 retains that bridge and adds headset-native sizing plus non-distorting presentation controls.

## New controls
- `VrRenderScale` — multiplies OpenXR recommended per-eye dimensions (0.50..2.00).
- `VrPresentationMode` — 0 Stretch, 1 Fit, 2 Crop/fill.
- `VrPresentationZoom` — 0.25..4.0.
- `VrLeftOffsetX/Y`, `VrRightOffsetX/Y` — per-eye UV alignment offsets.

## Expected markers
- `V38-RESOLUTION eye=... recommended=... swapchain=...`
- `V38-PRESENT fullscreen blit pipeline created with UV fit/crop controls`
- `V38-PRESENT-BLIT ... mode=... zoom=...`

This is still mono-in-both-eyes. Its purpose is to make the known-good V37 image correctly sized and configurable before V39 starts producing distinct left/right source renders.
