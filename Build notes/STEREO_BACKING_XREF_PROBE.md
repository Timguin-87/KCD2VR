# Stereo backing/xref probe

Read-only probe for KCD2 build `release_1_5_1308617_856`.

It narrows the renderer CVar registration block to the confirmed stereo names, infers the nearby non-string backing-storage address used by each registration, logs its raw value, then scans executable WHGame pages for runtime reads/writes outside the registration block.

Targets:
- `r_stereoScaleCoefficient`
- `r_StereoGammaAdjustment`

Look for `STEREO-BACKING registration`, `value`, `runtime-xref`, and `runtime-scan-complete` in `KCD2VR-debug.log`.

This build does not change stereo CVars or rendering. Keep `DiagnosticRendering=false` while probing.
