# KCD2VR V56.1 — adaptive tracking + FOV 130 + F6 UI-IPD tuning

This is a source build for the next runtime test.

## Important V56 finding fixed

V56's F8 table advertised filter modes 4/5/6/7 for 6 ms / 8 ms / 10 ms / adaptive smoothing, but `CameraHook::SetRotationFilterMode()` still clamped every value above 3 back to mode 3. V56.1 allows modes 0..7, so the ladder now tests the filter it says it is testing.

## Defaults

- Tracking rotation filter: **adaptive** (mode 7).
- Capture FOV: **130 degrees**.
- Gameplay framing: **0.95**.
- F6 UI eye separation/convergence: **0 mm** (identical projection-eye geometry).
- F6 UI fit scale: **1.58x**.

## F7 tuning

- `Capture FOV`: 100–160 degrees. Right is wider. Default/reset is 130.
- `Gameplay framing`: 0.95–2.00. Right moves the source farther / zooms out.
- `RESET VIEW`: immediately restores OpenXR-FOV mode, 130 degrees, framing 0.95, global swap off.
- `F6 UI eye separation / convergence`: -80 mm to +80 mm. 0 mm collapses the projection-eye separation and eye-FOV asymmetry to a common centered view. Negative deliberately reverses it for diagnosis.
- `RESET F6`: UI scale 1.58 and UI IPD 0 mm.

The global Home eye-swap remains available, but it is intentionally separate from F6 UI convergence.

## F8

Six 10-second phases, with N beeps:

1. raw
2. 4 ms smoothing
3. 6 ms smoothing
4. 8 ms smoothing
5. 10 ms smoothing
6. adaptive smoothing

After F8 completes or is cancelled via Good Mono, V56.1 restores **adaptive smoothing**, not raw.

## Suggested test order

1. Launch and first judge the new default adaptive tracking without pressing F8.
2. Open F7. Press RESET VIEW once, then tune capture FOV around 130 and gameplay framing to the right.
3. Open a native KCD2 menu/dialogue, press F6, and tune `F6 UI eye separation / convergence` starting at 0 mm. If the eye geometry still appears reversed, move gradually negative; if it diverges the other way, move positive. Also tune native UI fit scale.
4. Run F8 once and report the best phase.
5. Only after the view is comfortable, use the action recording buttons.

No KCD2 resolution changes are made.
