# V58.1 build fix

Fixes the two MSVC errors reported from V58:

1. `xr_manager.cpp`: removed stray writes to `requested_uv_scale_eye` / `coverage_clamped_eye` from `BlitAfrStereoToEyes`. Those telemetry arrays are local to `BlitGameImageToEyes`; the AFR path still applies the same `safe_scale` clamp.
2. `vrik_bridge`: `VrikBridge::Update` now accepts `XrManager&` instead of `const XrManager&`, matching the existing non-const `CopyActiveRenderFrameState` API. No runtime behavior change is intended.

No native skeleton writes were added.
