# KCD2VR V59.5 — Square Mono + Zoom + Smoothing

Build ID: `SQUARE-MONO-20260905-A`

## Required test baseline
Set KCD2 in `user.cfg` to:
```
r_Width = 1920
r_Height = 1920
```
Keep the in-game FOV at **110**. Do not change game FOV while VR is running.

## Changes
- Gameplay framing/zoom slider range is now **0.80x .. 2.00x** (was 0.95x .. 2.00x).
- F8 is now a 50-second tracking-only comparison:
  1. RAW
  2. 4 ms smoothing
  3. 6 ms smoothing
  4. 8 ms smoothing
  5. adaptive smoothing
- F8 does not change presentation mode, source FOV, zoom, UV mapping, stereo mode or resolution.
- V59.3 low-FOV safety, F6 comfort UI, recenter, right-stick-up jump and Left-Y sheath are retained.

## Crash finding
The 2026-09-05 crash was preceded by the captured PRESENT changing live from 1920x1920 to 1920x1440 while published VFOV dropped to ~83.6 degrees. Immediately afterward OpenXR frame begin calls failed; BugSplat reported Device Removed=Yes.
This build therefore does not attempt live game-FOV or resolution changes. Treat FOV <110 as unsafe for now.

## Runtime verification
The debug log must contain:
`V59.5 BUILD ID SQUARE-MONO-20260905-A`

F8 must contain:
`V59.5-F8-START BUILD=SQUARE-MONO-20260905-A`
