# V55.1 build fix

Fixes the MSVC errors in `src/camera_hook.cpp` around line 394.

Cause: `ApplyTrackedPose()` is a free function in the anonymous namespace, so it cannot access the private `CameraHook::s_rotation_filter_mode` and `CameraHook::s_rotation_filter_generation` members by unqualified name.

Fix: `FrustumDetour()` now passes the current rotation-filter mode and generation explicitly into `ApplyTrackedPose()`. The publish-camera path remains unchanged and both injection paths continue to use the same V55 filter configuration.

No intended runtime behavior, F8 phase ordering, stereo experiment, menu, VRIK, or recorder behavior changed.
