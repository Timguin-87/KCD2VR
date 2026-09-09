# MIS V46.6 — Mono Calibration / Safe Coverage

This build focuses on making mono a viable performance mode.

## What changed
- Startup baseline: Mono + OpenXR-FOV, capture FOV 115°, framing 0.95, swap off.
- Older saved V46.5 capture FOV values are overridden at startup for this test build because values around 75° cannot cover the Quest 3 asymmetric vertical eye frustum seen in logs.
- `Zoom` is now `Mono framing` and is constrained to 0.90–1.10.
- FOV is now labeled `Mono capture FOV / projection coverage` and constrained to 110–125°.
- OpenXR-FOV blits clamp requested UV scale to the largest uniform scale that stays inside the source texture, preventing the bottom black bar caused by source under-coverage.
- Mouse/keyboard menu input from V46.5 remains unchanged.
- F8 no longer runs AFR. It is a 25-second mono-only calibration ladder.

## F8 phases — 5 seconds each
1. 112° / framing 0.97
2. 115° / framing 0.97
3. 118° / framing 0.97
4. 115° / framing 0.94 (widest framing test)
5. 115° / framing 1.00 (tighter reference)

After phase 5 the pre-test mono FOV/framing is restored.

Report the phase with the best scale, least warping, and whether the bottom black bar is gone.
