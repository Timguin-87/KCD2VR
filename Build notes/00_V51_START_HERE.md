# KCD2VR V51 — Tracking Rebuild + Menu/Stereo Audit

This is a **source package** for the same Windows/MSVC + DetourModKit build environment used by prior KCD2VR builds.

## Default tracking changed

V51 no longer uses the V48 2-ms QPC-now late-pose refresh or repeated frustum mutation as its normal path.

Default:
- OpenXR head pose sampled once at `predictedDisplayTime` for the XR frame.
- Full 6DoF transform applied **reversibly at `PublishViewCamera`** immediately before KCD2 copies/publishes the camera.
- The source KCD2 camera matrix is restored immediately after the publish call.
- Pose frame identity is stamped at that publish point, so PRESENT correlation should no longer inherit an arbitrary stale frustum pose ID.

## F8: 60-second test, 10 seconds per phase

1. `V51_PUBLISH_IMMUTABLE_PAIRED` — new tracking, predicted pose, PRESENT-paired submit metadata.
2. `V51_PUBLISH_IMMUTABLE_CURRENT_META` — same camera tracking, current XR submit metadata.
3. `V51_POSITION_ONLY_CONTROL` — HMD translation on, HMD rotation off. Mouse/game rotation remains available.
4. `LEGACY_FRUSTUM_IMMUTABLE` — old frustum injection, but no QPC-now late refresh.
5. `LEGACY_FRUSTUM_LATE_V50` — old V50 behavior including the 2-ms QPC-now refresh.
6. `AFR_STEREO_DIAGNOSTIC` — sequential alternating-eye diagnostic using live IPD. **Not same-frame native stereo and not a final stereo solution.**

Keep doing the same yaw/pitch/roll/lean motion through all six phases and note which phase changes rotational judder. FOV, desktop resolution and presentation mode are intentionally held constant.

## F7 menu

F7 remains a second, additive OpenXR quad over gameplay. V50 failed because the menu source was typed BGRA (`DXGI_FORMAT_B8G8R8A8_UNORM`) while the runtime exposed the UI swapchain resource as the compatible typeless BGRA family. V51 accepts that compatible copy instead of rejecting it. The menu is centered/head-locked at 1.35 m and 1.05 m wide.

## Native KCD2 menus

Startup video/main menu/inventory/map are **not claimed fixed** in V51. We still need a reliable KCD2 UI render-target/state hook to put only native UI into a quad without flattening gameplay.
