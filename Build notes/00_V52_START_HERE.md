# KCD2VR V52 — Rotation Ownership + Menu Input

This source build narrows the HMD-rotation problem rather than changing FOV, game resolution, FPS, or startup presentation.

## F8 — six 10-second phases
1. `BOTH_CAMERA_PLUS_DYNAMIC_XR` — V52 normal path: HMD rotation in KCD2 camera, matching dynamic OpenXR render-pose metadata.
2. `CAMERA_ROT_PLUS_FROZEN_XR_META` — intentionally freezes projection render-pose metadata while the KCD2 camera still follows HMD rotation. If this becomes dramatically worse it is evidence that compositor pose metadata materially affects the perceived rotation.
3. `COMPOSITOR_ONLY_STATIC_RENDER_POSE` — disables KCD2 HMD rotation and freezes the render pose at phase start. Physical head rotation is therefore left to OpenXR reprojection as a diagnostic control; positional tracking remains on.
4. `POSITION_ONLY_DYNAMIC_XR_CONTROL` — KCD2 HMD rotation off but dynamic XR pose metadata retained. This is intentionally a mismatched-metadata control against phase 3.
5. `AFR_STEREO_NORMAL_IPD` — sequential diagnostic only, using live OpenXR IPD with positive sign.
6. `AFR_STEREO_REVERSED_IPD` — same sequential diagnostic with reversed IPD sign. Not final stereo.

Do the same head yaw/pitch/roll during phases 1–4. The key comparisons are 1 vs 2 and 3 vs 4.

## F7 input
F7 remains an additive, head-locked menu-only OpenXR quad. V52 adds a software cursor rendered into the 650x990 menu texture. Right Touch grip pose is raycast against the head-locked quad and mapped to the 650x990 menu pixels. Right trigger or A selects under the ray; B closes. Thumbstick navigation/adjustment remains as fallback. Physical mouse position is also rendered as a software cursor in the quad.

## Stereo status
AFR remains diagnostic only. V52 does not claim same-frame native stereo. The final target is still two real eye renders from one simulation/head sample.
