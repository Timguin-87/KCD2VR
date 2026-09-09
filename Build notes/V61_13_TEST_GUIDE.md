# KCD2VR V61.13.1 — Adaptive startup / big VR menu / recenter order

Source-only test branch. No Windows ASI was compiled in this environment.

## Startup
- ENGINE DOF adaptive pseudo starts automatically. It reads KCD2's exposed DOF focus-distance source and maps near/mid/far to .80/.85/.98 with smoothing.
- If the engine focus source is unavailable, startup stays at fixed .95; it does not auto-run the fake sweep.
- Manual MONO / .80 / .85 / .95 immediately cancels adaptive mode.
- ENGINE DOF is the default startup mode. MONO/.80/.85/.95 switch it off.
- FAKE DOF remains a separate manual diagnostic toggle so you can compare against the proven sweep if needed.

## VR menu
- Presented as a much larger flat OpenXR quad (~2.05 m wide at 1.45 m).
- Flat presentation intentionally replaces the cylinder so the existing right-controller ray can intersect it exactly.
- Right-controller pointer/trigger and existing stick/A navigation should both work.
- Added ROT ORDER button: B*D (default) / D*B (recenter diagnostic).

## Recenter test
Try both orders and recenter while looking level, down, and up. Send logs containing `V61.13.1-ROTORDER` and report whether vertical inversion disappears or flips.

## Retained
- V61.12 gameplay-only cull sync
- 90 FPS target
- merged-mesh performance controls
- FRIK action recorder
- .80/.85/.95/MONO manual pseudo presets
