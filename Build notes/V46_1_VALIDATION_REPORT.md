# V46.1 static validation report

No KCD2/VDXR runtime is available in this Linux environment, so these are source/binary-structure checks rather than a Windows game launch.

- PASS `console_global_rva`
- PASS `get_cvar_vslot_b8`
- PASS `int_setter_vslot_38`
- PASS `float_setter_vslot_40`
- PASS `customres_names`
- PASS `cl_fov_name`
- PASS `matrix_10_phases` — ['BASELINE_FIT', 'BASELINE_STRETCH', 'BASELINE_CROP', 'CUSTOMRES_1248x1344', 'CUSTOMRES_1248x1344_FOV110', 'CUSTOMRES_1872x2016_FOV110', 'CUSTOMRES_1872x2016_XR_FOV', 'EYE_ROUTE_CANONICAL', 'EYE_ROUTE_SWAPPED', 'RECURSIVE_VIEW_OBSERVER']
- PASS `matrix_5_seconds`
- PASS `customres_half_13_14`
- PASS `customres_075_13_14`
- PASS `customres_below_4096`
- PASS `fov110_phase`
- PASS `cvar_restore`
- PASS `shutdown_restore`
- PASS `customres_probe_api`
- PASS `probe_armed_before_setter`
- PASS `customres_resource_marker`
- PASS `customres_rt_check`
- PASS `published_camera_fov_observer`
- PASS `published_fov_counter`
- PASS `camera_fov_change_not_cvar_only`
- PASS `single_final_swap`
- PASS `no_gemini_copyresource_submission` — the only remaining `CopyResource` is the legacy AFR source→same-size cache copy, not a CryEngine→OpenXR swapchain copy
- PASS `single_xrlocateviews_call`
- PASS `matrix_forces_mono`
- PASS `no_manual_crenderview_ctor`
- PASS `no_dangerous_targets`
- PASS `ini_no_autoexec_required`

**Overall: PASS**

## Compile note

A local `clang-cl /Zs` syntax attempt cannot proceed because the DetourModKit SDK headers are not installed in this container. The package remains a source package for the user’s normal Windows/MSVC + DetourModKit + OpenXR build environment.
