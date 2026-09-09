# KCD2VR Native Stereo Injection V16

V16 fixes the V15 live-eye handoff failure seen in `KCD2VR-debug(20260903-150426).log`.

## Runtime finding from V15

The selected `main-4E6C` mode-2 target was found, but every sampled camera-pair call logged `NATIVE-PAIR-V15 xr-eye-unavailable`. No `apply` marker appeared and post-builder `q2` stayed zero. Therefore the clear/no-change visual result was expected: V15 never performed an eye-offset write.

## V16 change

The OpenXR worker now keeps a separate short-lived injection snapshot whenever `xrLocateViews` succeeds with two views whose finite eye separation is plausible (0.040-0.090 m). Transient OpenXR view-state validity-bit gaps no longer immediately erase the snapshot used by the KCD2 camera-pair hook.

The normal OpenXR composition path is unchanged and remains strict: diagnostic/real projection-layer submission still requires the standard position/orientation validity bits for that frame.

The injection cache expires after 250 ms, so a genuinely stale eye pair is not reused indefinitely.

## Intended first run

```ini
[Settings]
DiagnosticRendering=false
EnableNativeEyeWrite=true
```

Look for:

- `NATIVE-PAIR-V16 selected`
- `NATIVE-PAIR-V16 apply`
- `NATIVE-PAIR-V16 post`

A successful apply should report a plausible `xr_ipd` and `left/right` near +/- half that value. `q2` should no longer remain zero if the engine output responds as it did to the V14 fixed-offset experiment.
