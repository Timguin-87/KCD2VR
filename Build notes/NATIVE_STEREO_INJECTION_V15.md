# Native Stereo Injection V15

V15 is the first build in this branch that replaces the fixed +3.2 cm diagnostic write with a live OpenXR stereo split.

## What V14 proved

- `WHGame+0x866F6C` receives the selected mode-2 child camera pair.
- The selected child camera translations are zero before the experiment.
- A temporary +0.032 m camera write is propagated into the builder output (`q2 ~= 0.032`).
- The effect is independent of `DiagnosticRendering`; DR is only the purple OpenXR composition diagnostic.

## V15 injection

For each qualifying selected-camera-pair build:

1. Copy the latest valid OpenXR left/right `XrView` snapshot.
2. Compute live IPD from the distance between the two OpenXR eye positions.
3. Reject the write if IPD is non-finite or outside 40-90 mm.
4. Apply `-IPD/2` to camera A along its local-right basis.
5. Apply `+IPD/2` to camera B along its local-right basis.
6. Call the original `0x866F6C` builder with the proven six-argument ABI.
7. Log the generated output state.
8. Restore both child camera translations exactly.

Absolute HMD translation is deliberately NOT injected here because the existing CView/frustum hook already owns head pose. V15 injects only stereo separation.

## Test configuration

The packaged INI intentionally uses:

```ini
DiagnosticRendering = false
EnableNativeEyeWrite = true
```

## Expected markers

- `NATIVE-PAIR-V15 selected ... injection=OpenXR-live-IPD`
- `NATIVE-PAIR-V15 apply ... xr_ipd=... left=... right=...`
- `NATIVE-PAIR-V15 post ...`

If no current valid stereo snapshot exists, the hook logs `NATIVE-PAIR-V15 xr-eye-unavailable` and calls the engine unchanged.

## Visual outcomes that matter

- Two horizontally separated views/double image: confirms live OpenXR IPD reaches the old V7-style multi-view contribution.
- Clear image with a different depth response: pair semantics differ from a simple side-by-side/double contribution; inspect logs/output state.
- Crash: first inspect whether it occurs before the first `apply`, during builder execution, or after restore.
- No visible difference but `q2`/output changes: next hop is routing/capture of the generated views into OpenXR eye swapchains rather than more camera archaeology.
