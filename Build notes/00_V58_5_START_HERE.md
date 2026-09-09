# KCD2VR V58.5 — Mono 90 calibration + source-quality telemetry

This candidate follows the V58.4 headset result rather than another theoretical FOV guess.

## What changed

- Default mono geometry is the V58.4 **CALIBRATED** path, now at **90° effective source VFOV**.
- F5 toggles between **CALIBRATED** and raw **LIVE** published-camera mapping.
- F7 labels the calibration honestly: **lower = wider**.
- XR resolution is explicitly labeled as target-only; it does not increase KCD2 source detail.
- Runtime emits `V58.5-SOURCE-QUALITY` whenever the captured PRESENT size changes.

## Test

1. Do not change F7 first. Judge startup and gameplay at the new 90° default.
2. Tap F5 once for LIVE, then once again for CALIBRATED. Compare world scale.
3. If 90° is still too zoomed, use F7 and lower the mono width calibration a few degrees.
4. Do not expect XR render scale alone to sharpen the world. Send the debug log so the captured source size can be compared with the XR eye size.

This is still a mono bridge. True source-resolution improvement requires a larger engine-owned source target or native per-eye rendering.
