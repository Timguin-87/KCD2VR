# V59.4 build note

Build ID: `EDGE-ONLY-20260905-A`

The V59.2 source contained the intended edge table, but the reported headset run logged the V59.1 UV-reprojection ladder. V59.4 makes stale binaries unmistakable and hard-neutralizes the retired cyclopean variables before F8.

Active F8 invariants:
- phase count = 6
- names begin `EDGE_`
- `mono_fov_asym = 1.0`
- `uv_reproj_mode = 0`
- `uv_binocular = 0.0`
- `pairing_mode = 0`
- only `bottom_edge_mode` varies 0..5

This is source-only and still requires the user's Windows/MSVC build environment.
