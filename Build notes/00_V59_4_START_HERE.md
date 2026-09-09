# KCD2VR V59.4 — EDGE-ONLY-20260905-A

## Purpose
This is the corrected F8 build after the V59.2 runtime was proven to be a stale V59.1 ASI.

## Before testing
- Set KCD2 game FOV to **110**.
- Perform a **clean Windows build** and copy the newly built ASI.
- On startup, verify the debug log contains exactly:
  `V59.4 BUILD ID EDGE-ONLY-20260905-A`
- When F8 starts, verify:
  `V59.4-F8-START BUILD=EDGE-ONLY-20260905-A`
  and
  `V59.4-F8-NEUTRAL ... fovAsym=1.000 uvReproj=0 uvBinoc=0 pairing=0`

If those markers are absent, do not evaluate the headset image: the wrong ASI is running.

## F8 ladder — 6 x 10 seconds
Only the bottom-edge treatment changes. All phases use current published KCD2 source FOV, runtime eye FOV, honest common mono origin, RAW tracking, no UV reprojection, no binocular UV shift and no artificial FOV asymmetry.

1. `EDGE_A_BLACK_REFERENCE` — exact good geometry, black lower uncovered area allowed.
2. `EDGE_B_VERTICAL_SHIFT` — Y-only translation.
3. `EDGE_C_Y_COMPRESS_88` — Y-only compression, X untouched.
4. `EDGE_D_TAIL_FROM_80` — upper 80% unchanged, lower tail compressed.
5. `EDGE_E_TAIL_FROM_90` — upper 90% unchanged, lower tail compressed.
6. `EDGE_F_BOTTOM_HOLD` — exact geometry, extend only the final lower source row.

## Other V59.3 fixes retained
- right-stick Up = jump
- Left Y = weapon slot/sheath
- left stick = forward/back + strafe
- right stick X = smooth turn
- low-FOV safety guard around game FOV <110
- leveled/larger world-fixed F6 comfort panel
- inventory comfort presentation
- OpenXR reference-space recenter handling
