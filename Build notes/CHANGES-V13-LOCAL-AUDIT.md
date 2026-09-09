# V13 local audit changes

- Corrected `WHGame+0x866F6C` detour ABI from four to six arguments after proving both stack floats are consumed by the callee.
- Added richer read-only selected-slot diagnostics.
- Added stereo observer cleanup on OpenXR init failure.
- Added local/static audit report.
- `EnableNativeEyeWrite=false` remains default.
- All roadmap feature gates remain inert.
