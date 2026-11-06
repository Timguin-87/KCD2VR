# M2: canonical source ownership. Tests/research are never globbed into production.
add_library(KCD2VR_compile_contract INTERFACE)
target_include_directories(KCD2VR_compile_contract INTERFACE "${CMAKE_SOURCE_DIR}/src")
target_compile_definitions(KCD2VR_compile_contract INTERFACE WIN32_LEAN_AND_MEAN NOMINMAX)
target_link_libraries(KCD2VR_compile_contract INTERFACE DetourModKit::DetourModKit OpenXR::openxr_loader)
if(MSVC)
 target_compile_options(KCD2VR_compile_contract INTERFACE /FS)
endif()
function(kcd2vr_module name)
 set(module_sources ${ARGN})
 if(NOT name STREQUAL "base")
  set(source_list "${CMAKE_SOURCE_DIR}/src/subsystems/${name}/sources.json")
  set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${source_list}")
  file(READ "${source_list}" source_json)
  string(JSON source_count LENGTH "${source_json}")
  set(module_sources)
  if(source_count GREATER 0)
   math(EXPR last_source "${source_count} - 1")
   foreach(index RANGE 0 ${last_source})
    string(JSON source_path GET "${source_json}" ${index})
    list(APPEND module_sources "${source_path}")
   endforeach()
  endif()
 endif()
 add_library(KCD2VR_${name} OBJECT ${module_sources})
 target_link_libraries(KCD2VR_${name} PRIVATE KCD2VR_compile_contract)
 set_target_properties(KCD2VR_${name} PROPERTIES COMPILE_PDB_NAME "KCD2VR_${name}")
 target_link_libraries(KCD2VR PRIVATE KCD2VR_${name})
endfunction()
kcd2vr_module(base
 src/dllmain.cpp
 src/companion_loader59c4.cpp
 src/mod.cpp
 src/native_script_t2.cpp
 src/diagnostic_trace.cpp
 src/camera_hook.cpp
 src/immersive_fov59c7g.cpp
 src/frame_sync.cpp
 src/recursive_view_probe.cpp
 src/device_hook.cpp
 src/depth_probe.cpp
 src/xr_manager.cpp
 src/retail_build112.cpp
 src/animation_filter.cpp
 src/action_time_owner59c11.cpp
 src/combat_diag_probe.cpp
 src/native_stereo_outer_lab.cpp
 src/native_stereo_preflight_v6602.cpp
 src/native_dlss_stereo_d82.cpp
 src/native_fsr_stereo_d82.cpp
 src/native_upscaler_stereo_d82.cpp
 src/native_xess_stereo_d85.cpp
 src/native_postaa_repair_v6632.cpp
 src/native_cloud_right_v6641.cpp
 src/native_stage_bisect_v6644.cpp
 src/native_svo_prev_per_eye_v6649.cpp
 src/native_svo_right_v6654.cpp
 src/native_taa_probe_v6657.cpp
 src/native_taa_pair_epoch_v6661.cpp
 src/native_water_reflection_right_v6663.cpp
 src/native_vr_control_plane_lab.cpp
 src/subsystems/subsystem.cpp
 src/subsystems/catalog.cpp
 src/native_physics_phase_t4.cpp
 src/activity_lens59c10r1.cpp
 src/vr_menu.cpp
 src/frik_pose_probe.cpp
)
kcd2vr_module(hybrid)
kcd2vr_module(direct)
kcd2vr_module(masterstrike_combos)
kcd2vr_module(physics)
kcd2vr_module(interactions)
kcd2vr_module(alchemy)
kcd2vr_module(smithing)
kcd2vr_module(doors)
kcd2vr_module(dialogues)
kcd2vr_module(hud_ui_controls)
kcd2vr_module(vrik)
kcd2vr_module(vrmap)
kcd2vr_module(gore)
kcd2vr_module(throw_drop)
kcd2vr_module(ranged_reload)
kcd2vr_module(experimental)
