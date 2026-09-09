# KCD2VR V56.2 — FOV 100 + Automatic Flat UI

Runtime-tuned source build.

## Defaults
- Adaptive rotation smoothing ON by default.
- Capture FOV default/reset: **100 degrees**.
- FOV slider range: **80–115 degrees**, 1-degree keyboard/button step.
- Gameplay framing default: **0.95**.
- F6 UI eye separation default: **0 mm**.

## F6 behavior
F6 now cycles three states:
1. **AUTO** — conservative camera-FOV heuristic automatically enables flat UI for extreme menu/cinematic camera states.
2. **FORCE ON** — manual flat UI.
3. **FORCE OFF** — manual gameplay projection.
Then back to AUTO.

AUTO is deliberately conservative: it does not claim exact KCD2 menu-state detection. It uses the already-observed published camera FOV and hysteresis. If Inventory/Map/etc. does not trigger automatically, use FORCE ON and record/send the log so a stronger detector can be derived safely.

## Test priority
1. Check 100-degree gameplay FOV and tune 80–115.
2. Enter Inventory/Map/Quest/Codex/dialogue and see if AUTO switches flat UI.
3. If AUTO misses a screen, F6 FORCE ON remains available.
4. Only after view is comfortable, run action recordings.
