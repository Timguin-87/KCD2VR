# KCD2VR V59.6 — Square Mono + Native Stereo Scene-RT Probe

## Required baseline
Keep KCD2 at:

```ini
r_Width = 1920
r_Height = 1920
```

Keep the in-game FOV at **110**. Do not change FOV or resolution while the ASI is active.

## Verify the binary before testing
The runtime log MUST contain:

`V59.6 BUILD ID SQUARE-STEREO-PROBE-20260905-B | ZOOM_MIN=0.80 F8=4MS+ADAPTIVE+SCENE_RT_PROBES`

Pressing F8 MUST contain:

`V59.6-F8-START BUILD=SQUARE-STEREO-PROBE-20260905-B`

If either marker is missing, stop: the wrong ASI is running.

The gameplay framing slider range is **0.80 to 2.00**. Before F8, tune framing as desired; F8 preserves it.

## F8 ladder — 6 x 10 seconds
A. `S59_A_SMOOTH_4MS` — square mono, 4 ms rotation smoothing only.
B. `S59_B_ADAPTIVE` — square mono, adaptive rotation smoothing only.
C. `S59_C_SCENE_RT_MONO` — 4 ms smoothing, read-only 1920x1920 normal-pipeline render-target census. No dual render.
D. `S59_D_DUAL_10MM_NOSELECT` — dual normal-pipeline Execute, total test IPD 10 mm (±5 mm), no selector write; scene RT transitions tagged L/R.
E. `S59_E_DUAL_10MM_SELECTOR` — same ±5 mm camera shift plus selector 0/1; scene RT transitions tagged L/R.
F. `S59_F_DUAL_30MM_SELECTOR` — total test IPD 30 mm (±15 mm) plus selector 0/1; stronger separation proof.

### Important
Phases D-F do **not** submit native stereo to OpenXR. PRESENT remains the normal mono fallback. The goal is to discover whether the renderer writes an exact-size scene-color resource separately during the two re-entry passes.

Look for log lines:

- `V59.6-SCENE-RT ... eye=MONO|L|R ... resource=...`
- `V59.6-SCENE-SUMMARY ... left=... right=...`
- `V59.6-STEREO-PROBE ... PRESENT_BRIDGE=OFF`

A strong result is the **same 1920x1920 resource address** appearing during both `eye=L` and `eye=R` passes with RENDER_TARGET transitions. That would identify a likely place to copy left output before the right pass overwrites it.

## What to report
For A and B: which is less jittery during slow head turns.
For C-F: visual stability/crashes only. The stereo conclusion will come from the log, not headset appearance.

## Clean Windows build
`build_release.bat` deletes the entire build directory before configuring. It also searches the freshly built ASI for the V59.6 build marker. Delete the old game-folder `KCD2VR.asi` before copying the new one.
