# MIS V47.0 — Pose Pairing + Safe UI Fit + Combat Capture

Purpose of this build: attack the high-FPS head judder found in V46.9, remove the anchored flat-UI path that produced diplopia, and keep the targeted unarmed-combat capture.

## Runtime defaults
- Mono Performance
- capture/game FOV 115 degrees
- framing 0.95 at startup; F8 uses 0.97 for its controlled A/B phases
- OpenXR FOV remap
- PRESENT-paired render-pose submission ON by default
- no AFR/native pseudo-stereo phase in F8

## F6 / F7
- F6: safe 2D aspect-fit prototype only. No head anchor, no per-eye counter-shift. Press again to restore gameplay projection.
- F7: MIS parchment menu automatically uses the same conservative aspect-fit override while open, then restores gameplay projection when closed.
- This is deliberately NOT the final OpenXR quad-layer UI yet.

## F8 — 27 seconds
1. 5 s LEGACY_CURRENT_XR_POSE — old submission behavior.
2. 5 s PRESENT_PAIRED_RENDER_POSE — submit the projection pose associated with the KCD2 frame actually copied.
3. 5 s PAIRED_POSE_STABILITY — same paired route for a second confirmation window.
4. 2 s COMBAT_COUNTDOWN — parchment prompt.
5. 10 s COMBAT_CAPTURE — box and kick normally with mouse/keyboard.

After F8, V47.0 restores Mono Performance and leaves PRESENT-paired pose submission ON.

## What to report
Most useful report:
- F7 menu size: too close / good / too small
- F6: single image or double image
- head motion phase 1 vs phase 2 vs phase 3: which is smoothest
- combat prompt visible: yes/no
- combat completed: yes/no

The log markers are `V47.0-FRAME-ID` and `V47.0-POSE-PAIR`. A paired phase should show `paired=true` and a non-zero `renderPoseFrame` near, but not necessarily equal to, `currentXr`.
