# V60.9 quick test

1. Start with the camera offsets you like from V60.8.
2. In the book, note KCD2 game FOV (`cl_fov`) and new `KCD2 horizontal FOV cl_hfov`.
3. Keep `cl_fov` fixed. Apply `cl_hfov` at roughly 90, 100, 110, 120 (stop if the image/resolution becomes unusable).
4. Watch three things separately: world scale/framing, black-edge/render coverage, and the exact point where side geometry culls.
5. Then test culling-frustum expansion 1.00 vs 1.15/1.25. V60.9 now changes the full KCD2Tools CCamera cull tuple, not only +0x30.

Useful logs:
- `V60.9-HFOV APPLY`
- `V60.9-CULL-SYNC`
- `V45-GET-RENDERVIEW ... Normal`
- `V45-PIPELINE ... viewType=0(Normal)`

If cl_hfov changes the desktop/custom resolution, the before/after dimensions are now in the same log line.
