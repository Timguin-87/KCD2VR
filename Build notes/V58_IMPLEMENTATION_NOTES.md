# V58 implementation notes

## Projection / sharpening
- KCD2 `cl_fov` is no longer written from MIS mono-coverage controls.
- The bridge retains tangent-space OpenXR-FOV remapping for the temporary mono path and logs requested vs. safe/actual scale plus clamp state.
- A real bridge-side sharpening filter is applied in the copy shader; strength is independent of KCD2's own sharpening CVar.

## XR resolution apply
- UI requests a render scale in the 0.50x..2.00x range.
- The request is consumed at the start of `XrManager::RunFrame`, before new swapchain images are acquired.
- The code waits for the mod's last D3D12 fence, destroys XR eye/UI swapchains, recreates them at the new scale, and rolls back to the prior scale if recreation fails.
- KCD2 desktop/backbuffer resolution is intentionally unchanged.

## MIS menu
- Dedicated UI target is 1100x680 with a two-page/open-book layout.
- When `XR_KHR_composition_layer_cylinder` is advertised and enabled, the UI is submitted as an `XrCompositionLayerCylinderKHR`; otherwise a quad is used.
- Cylinder-mode controller-ray intersection is intentionally not treated as a flat plane; stick/A navigation plus desktop mouse are the safe controls for this build.
- The same UI texture is drawn to the monitor after XR captures the clean KCD2 frame (hybrid presentation).
- Holding a slider for 0.5 s fades the menu to ~20% opacity for live preview.

## Dialogue
- V58 exposes native WHGame dialogue-camera modes using the discovered CVars `wh_dlg_ForcePOV` and `wh_dlg_NoCameras`.
- Original values are captured and restored on shutdown.
- This is intended to test whether KCD2's own dialogue-camera/FOV transition can be disabled, potentially avoiding a separate dialogue presentation mode.

## Touch bindings
- OpenXR actions now include A/B/X/Y, left Menu and both thumbstick clicks for the Oculus Touch profile.
- A reversible `SendInput` compatibility layer maps the corrected user layout to vanilla KCD2 keyboard/mouse actions.
- Right grip is not consumed.
- Four-way attack direction is currently a small mouse-direction pulse, not native combat-state injection.
- Quick actions are provisional keyboard fallbacks until a native KCD2 gamepad/action-map path is resolved.

## Primitive VRIK
- F9 toggles a read-only two-bone diagnostic chain for each active controller.
- Shoulder positions are estimated relative to the HMD; elbow is solved analytically using fixed upper/forearm lengths and a side/down pole vector.
- Positions are logged in OpenXR LOCAL space.
- No `WeaponSolver::SetIKTargets`, skeleton pose, joint, or IK target writes are performed in V58.

## Stereo status
V58 does not promote the previous dual-pipeline/same-target experiments to true stereo. True stereo still requires distinct left/right scene resources, asymmetric per-eye cameras/projections, and correct per-eye renderer state handling.
