# MIS V46.8 — Stereo + Boxing Capture

V46.8 keeps the current provisional Mono Performance reference at 115° / framing 0.97 and turns F8 into one combined reverse-engineering run.

## F8 flow
1. 5 s Mono reference — 115° / 0.97
2. 5 s Sequential renderer observer — IPD hint 0.25
3. 5 s Native renderer observer — IPD hint 0.25
4. 5 s Native renderer observer — IPD hint 0.50
5. 5 s Native renderer observer — IPD hint 1.00
6. 2 s parchment **Record Boxing** countdown
7. 10 s targeted boxing capture — use ordinary mouse + keyboard punches

Then MIS restores the pre-test mono settings automatically.

Important: sequential/native phases are still observer-only mono compositor fallbacks. They do not make an unsafe second scene-render call and do not claim true native stereo yet.

The boxing capture is passive. It records mouse attack edges with millisecond offsets, movement keys, IK-related CVar state, controller state if available, and renderer/PRESENT counters. It performs no skeleton, joint, or IK target writes.
