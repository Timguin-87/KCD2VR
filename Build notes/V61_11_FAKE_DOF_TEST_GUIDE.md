# KCD2VR V61.11 — Fake DOF / adaptive pseudo comfort test

This is a source-only test branch built on V61.10. The V61.8 culling pose-sync fix, pseudo workflow, 90 FPS target, merged-mesh controls, UI/dialogue work, and FRIK recorder are retained.

## What FAKE DOF does
The VR menu has a new **FAKE DOF** button beside the pseudo presets. It does **not** read KCD2 scene depth or the engine DOF focus distance yet. It intentionally applies one global pseudo-stereo strength to the whole image and sweeps smoothly through the proposed comfort range:

- 0–4 s: 0.80
- 4–8 s: blend 0.80 -> 0.85
- 8–12 s: 0.85
- 12–16 s: blend 0.85 -> 0.98
- 16–22 s: 0.98
- 22–26 s: blend 0.98 -> 0.80
- repeat

The purpose is only to answer: **does a smoothly varying 0.80 / 0.85 / 0.98 pseudo scale feel comfortable enough to justify connecting it to real KCD2 focus/depth later?**

Clicking MONO / PSEUDO .80 / .85 / .95 or using F8 cancels FAKE DOF and returns to manual preset control.

## Log markers
- `V61.11-FAKE-DOF ON ...`
- `V61.11-FAKE-DOF t=... pseudoScale=... source=SIMULATED_GLOBAL noDepth=true`
- `V61.11-FAKE-DOF OFF ...`

## About KCD2's real DOF system
The project already has static evidence tying the Normal GBuffer/depth path to DOF-related rendering, and V61.2 included a read-only depth-wrapper probe. What is still missing is a verified current-game scalar such as focus distance / focal plane, or a confirmed D3D12 depth resource usable by our compositor. Do not treat the FAKE DOF mode as real depth-adaptive stereo.

For the next real-depth step, prefer finding KCD2's existing DOF focus-distance value or camera parameter first. That would be cheaper and safer than sampling/copying the full depth texture merely to drive one global pseudo strength.
