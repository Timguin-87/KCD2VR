# V58.7 build note

V58.7 is based on V58.6 and changes only the active mono presentation / F6 startup path. The V58.6 control swap is retained.

The design is informed by the included Crysis VR source research:
- oversized symmetric source frustum,
- asymmetric per-eye crop,
- projection metadata matching the view that rendered the pixels,
- quad layers for flat presentation.

This package is source-only in this environment. Build with `build_release.bat` on the existing Windows toolchain.
