# KCD2VR V60.5 — Static Projection F8 / Mono Gate

Build ID: `STATIC-PROJECTION-F8-MONO-GATE-20260905-I`

This is the final FOV/projection experiment for now. It intentionally removes the known contaminants from the old V60.1 F8 ladder: no 0.95 zoom and no binocular UV offset.

## F8 ladder

Five phases, 8 seconds each:

1. 100.0°
2. 102.5°
3. 105.0°
4. 107.5°
5. 110.0°

For every phase:
- KCD2 `cl_fov` and source projection model are matched.
- Zoom is exactly 1.00.
- UV binocular offset is exactly 0.
- FOV asymmetry is fixed at 0.75 (PS1-style comfortable pseudo view).
- OpenXR runtime eye FOV is unchanged.
- Culling expansion is temporarily forced to 1.00 so the projection test is isolated.
- `uv_reproj_mode=3` enables an explicit static source-to-XR intrinsics homography. This uses no temporal pose delta and is not the retired V59.1 rotation reprojection.

The explicit homography is mathematically equivalent to ray/tangent remapping for a symmetric source camera, but it is a separate shader branch so the test cannot silently fall through the old zoom/UV path.

## Mono gate

V60.4 reset MONO correctly, then accidentally re-applied the stale persisted stereo mode at the end of `ApplyMenuState`. V60.5 computes an `effective_stereo=0` whenever MONO is selected before updating DeviceHook/FrameSync.

## What this test can prove

A useful phase is the first one with no black edges while straight lines / head rotation remain visually stable. If all five still warp as coverage increases, compositor-only FOV remapping is considered exhausted for now and the next FOV work should move to the actual KCD2 projection/frustum matrix.
