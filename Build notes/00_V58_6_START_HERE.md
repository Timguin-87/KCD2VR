# KCD2VR V58.6 — control swap + fixed 8 ms head smoothing

This candidate keeps the V58.5 calibrated 90-degree mono presentation and source-quality telemetry, while isolating two user-requested input/motion changes.

## Locomotion test layout

- Left stick Y: forward / backward.
- Left stick X: continuous smooth horizontal mouse-turning (time based, not one-shot pulses).
- Left stick click: sprint.
- Right stick X: strafe left / right.
- Right stick UP: jump (one SPACE tap on the direction edge).
- Right stick click: crouch.
- Right trigger: attack as before. While the trigger is engaged, right stick temporarily selects the four attack directions instead of leaking strafe/jump into the attack.
- Left trigger + right stick: provisional quick-action layer.
- Right grip remains reserved/unmapped.

The bridge is still Win32 input emulation; this layout test does not replace the planned CryInput/ViGEm path.

## Head-jitter test

- Startup/default rotation smoothing is now fixed 8 ms instead of adaptive.
- F4 toggles FIXED_8MS <-> ADAPTIVE for direct headset comparison.
- F5 remains CALIBRATED mono <-> raw LIVE FOV comparison.

The adaptive filter itself does not read game FOV. The test is intended to separate a filter/cadence issue from the visual amplification introduced by the mono remap and low-resolution source.

## What to report

1. Whether left-stick horizontal turning is smooth and whether its speed is usable.
2. Whether left forward/back and right strafe/jump feel more natural.
3. Whether FIXED_8MS reduces head jitter at KCD2 FOV 130-140 compared with ADAPTIVE (F4).
4. If possible, keep F5 in whichever mono mode currently looks best while doing the F4 comparison.
5. Send `KCD2VR-debug.log`; useful lines start with `V58.6-ROT`, `V58.6-TURN`, `V58.6-TOUCH-STATE`, `V58.6-INJECT`, and `V58.6-MONO-SYNC`.
