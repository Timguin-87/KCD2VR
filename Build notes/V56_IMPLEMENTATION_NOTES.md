# V56 implementation notes

- `camera_hook.cpp`: adds continuous 4/6/8/10 ms shortest-path quaternion smoothing plus adaptive 10->3 ms smoothing. Raw mode still returns before QPC/thread-local filter state.
- `vr_menu.cpp/.h`: widened/reversed-feeling FOV + zoom test controls, adjustable F6 UI fit scale, nine visible action recording buttons.
- `xr_manager.cpp/.h`: replaces fixed V55.2 `1.58` flat-UI scale with runtime atomic `flat_ui_scale_` clamped 0.60..3.00.
- `mod.cpp/.h`: persists `VrNativeUiScale`; F8 becomes smoothing-only ladder; F7 recorder requests are marshalled through an atomic to the control loop; category-specific records plus periodic IK-CVar observer samples.
- No player skeleton/IK memory writes were added.
