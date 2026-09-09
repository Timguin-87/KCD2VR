# KCD2VR V59.1 — UV reprojection ladder

Purpose: isolate the head-motion warp seen in V59.0's promising cyclopean/direct presentation.

## What stays unchanged
- Fixed LOCAL-space F6/startup comfort quad.
- Inventory comfort quad from V58.9.
- Conventional Touch layout: left stick locomotion, right stick X smooth turn, Left Y jump.
- No native stereo render re-entry in F8.
- No skeleton/VRIK writes.

## F8: 8 phases x 8 seconds (~64 s)
A. `UV_A_BASELINE_M0_25` — V59.0-style direct copy, current XR pose, -0.25 FOV asymmetry.
B. `UV_B_BASELINE_CENTER` — direct copy, current XR pose, centered FOV.
C. `UV_C_PAIRED_M0_25` — same pixels as A, but projection layer uses PRESENT/render-paired pose.
D. `UV_D_PAIRED_CENTER` — same as C, centered FOV.
E. `UV_E_ROT_REPROJ_M0_25` — current-pose layer + rotation-only UV ray reprojection into render-pose camera space, -0.25 FOV asymmetry.
F. `UV_F_ROT_REPROJ_CENTER` — same reprojection, centered FOV.
G. `UV_G_ROT_REPROJ_INVERSE` — inverse quaternion sign diagnostic; should tell us immediately if E/F use the wrong relative-rotation convention.
H. `UV_H_ROT_REPROJ_UV_SHIFT` — E/F mapping plus a very small opposite-sign binocular source-UV shift (-0.006) to see whether disparity belongs in UV space rather than XrFovf.

## What to report
For each A-H: stationary image quality, head-turn warp/swim, depth/stereo percept, eye comfort, black edges.

Most important comparison: C/D versus E/F. If C/D fix motion, render-paired projection pose is the answer. If E/F fix motion, rotation ray reprojection is the answer. If G is better than E/F, quaternion polarity is reversed. H is only a small UV-disparity probe.

## Important limitation
Rotation can be reprojected from a color frame. Translation cannot be corrected correctly without depth, so V59.1 deliberately does not fake positional reprojection.
