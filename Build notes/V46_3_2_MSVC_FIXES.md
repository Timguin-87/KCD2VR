# V46.3.2 MSVC compile fixes

This revision changes only two Windows/MSVC portability issues found by the first fresh build of V46.3.1:

1. `recursive_view_probe.cpp`: moved the guarded renderer-global pointer read (`__try/__except`) out of `RecursiveViewProbe::Install()` into a small leaf helper with no C++ object-unwind state. This resolves MSVC C2712 without changing the probed RVA or runtime behavior.
2. `vr_menu.cpp`: changed `LoadCursorW(nullptr, IDC_ARROW)` to explicit wide resource form `LoadCursorW(nullptr, MAKEINTRESOURCEW(32512))`, resolving C2664 in non-UNICODE builds.

No renderer RVAs, hook signatures, D3D12 state transitions, OpenXR behavior, graphics-profile values, or menu controls were changed.
