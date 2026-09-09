# CView virtual-owner probe changes

- Replaced the inactive `WHGame+0x2525414` render-pass probe with a live hook at exact CView vtable slot 2, `WHGame+0x7F0EC0`.
- Logs runtime caller RVA, exact CView pointer/vtable, frame-time-like scalar, active flag, position/quaternion, and thread ID.
- Keeps the frustum CView census for correlation.
- Replaced permissive camera gating with exact RTTI-backed CView vtable matching (`0x3A7A030` / `0x3A7A048`).
- `camera_pose_applications` now counts only successful exact-CView modifications.
- `DiagnosticRendering=false` remains the probe default.
