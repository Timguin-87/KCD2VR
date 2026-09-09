# KCD2VR V53 — synchronous pose + rotation-order audit

Purpose: isolate whether HMD rotational jitter comes from (a) quaternion composition order or (b) consuming an XR-thread cached pose on an independently clocked KCD2 camera thread.

## F8 — six phases, exactly 10 seconds each
All phases keep presentation mode, FOV, desktop resolution, stereo mode, head-position tracking, and gameplay rendering unchanged.

1. **CACHED_CURRENT_ROT_ORDER** — current cached XR pose; current `base * HMD_local` composition.
2. **CACHED_OPPOSITE_ROT_ORDER** — same cached pose; diagnostic opposite `HMD * base` composition.
3. **SYNC_LATEST_PREDICTED** — camera thread calls `xrLocateSpace` synchronously at the latest XR `predictedDisplayTime`; current rotation order.
4. **SYNC_QPC_NOW** — camera thread calls `xrLocateSpace` synchronously at QPC-now; current rotation order. This intentionally reproduces the risky "sample now" idea as a controlled comparison.
5. **SYNC_PREDICTED_PLUS_5MS** — synchronous locate at latest `predictedDisplayTime + 5 ms`.
6. **SYNC_PREDICTED_PLUS_15MS** — synchronous locate at latest `predictedDisplayTime + 15 ms`.

Move the headset continuously through slow yaw, fast left/right reversals, pitch, roll, lean, and small positional motion. Do not change game resolution during F8.

## New logging
`V53-F8-BEGIN/END` identifies each phase.

`V53-ANGULAR` logs:
- `hmdDeg` — raw HMD angular change between camera applications
- `baseDeg` — KCD2 native/base camera angular change (mouse/game camera)
- `finalDeg` — final camera angular change after our HMD transform
- `source` — cached / sync predicted / sync now / predicted+offset
- `order` — rotation composition order

`V53-SYNC-POSE-FAIL` means a synchronous lookup failed and the phase safely fell back to the cached pose.

## F7
V52 F7 interaction is retained: additive menu-only OpenXR quad, right Touch ray pointer, trigger/A activate, B close, thumbstick fallback, and software cursor in the menu texture.

## Important
This is a source package. The local environment does not include DetourModKit headers, so Windows compilation could not be completed here. Build with the same MSVC + DetourModKit environment used for prior versions.
