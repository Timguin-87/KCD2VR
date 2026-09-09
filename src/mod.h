// mod.h

#pragma once

#include <memory>
#include <atomic>
#include <string>
#include "camera_hook.h"
#include "device_hook.h"
#include "xr_manager.h"
#include "vr_menu.h"
#include "touch_bindings.h"

namespace kcd2vr {

class Mod
{
public:
    static Mod& Instance();

    // Prevent copying or moving the singleton instance
    Mod(const Mod&) = delete;
    Mod& operator=(const Mod&) = delete;
    Mod(Mod&&) = delete;
    Mod& operator=(Mod&&) = delete;

    void Run();
    void Shutdown();

private:
    Mod() = default;

    bool LoadConfig();
    static void OnRenderTick(ID3D12CommandQueue* queue);
    void FillMenuState(VrMenuState& out) const;
    bool ApplyMenuState(const VrMenuState& state);
    bool SaveMenuSettings() const;

    std::unique_ptr<CameraHook> camera_hook_;
    std::unique_ptr<DeviceHook> device_hook_;
    std::unique_ptr<VrMenu> vr_menu_;
    TouchBindings touch_bindings_;
    // Points at XrManager::Instance() - the process-wide singleton, NOT a
    // separately-owned object. This was found to be a real bug during V17
    // review: GetOpenXrStereoInjection() (stereo_archaeology.h) reads from
    // XrManager::Instance(), but this member used to be its own separate
    // std::make_unique<XrManager>() - meaning the native-write code was
    // reading from a completely different, never-initialized XrManager the
    // whole time (locate_result stuck at its default XR_ERROR_RUNTIME_FAILURE,
    // predicted=0, in 100% of logged applies). Now both consumers share the
    // one real, running instance. We do NOT own this pointer - never delete
    // it, only Shutdown() it and clear the pointer.
    XrManager* xr_manager_ = nullptr;

    std::atomic<bool> running_{false};
    std::atomic<bool> matrix_test_requested_{false};
    std::atomic<bool> safe_mono_requested_{false};
    std::atomic<bool> matrix_running_for_ui_{false};
    std::atomic<bool> graphics_profile_refresh_requested_{false};
    std::atomic<bool> graphics_profile_applied_for_ui_{false};
    std::atomic<int> record_request_{0}; // V56 F7 action-recorder request

    // Bound directly to DetourModKit::config atomics - see config::bind in mod.cpp.
    std::atomic<bool>  enabled_{true};
    std::atomic<float> head_pose_scale_{1.0f};
    std::atomic<float> camera_side_offset_m_{0.0f};
    std::atomic<float> camera_back_offset_m_{0.0f};
    std::atomic<float> camera_height_offset_m_{0.0f};
    std::atomic<float> menu_game_fov_target_{95.0f};
    std::atomic<float> menu_horizontal_fov_target_{95.0f}; // V60.9 cl_hfov diagnostic
    // V60.3: explicit FOV Apply arms a short stability window.  A single
    // 500ms sample proved too early in V60.2 (e.g. target 105 briefly read
    // ~99.97 before settling to 105).  We therefore wait at least 300ms and
    // require a stable published-camera FOV for 300ms, with a 2s timeout.
    // This is still one-shot only: dialogue/inventory/cinematic publications
    // can never continuously drive the VR remap.
    std::atomic<std::uint64_t> pending_fov_calibration_not_before_ms_{0};
    std::atomic<std::uint64_t> pending_fov_calibration_deadline_ms_{0};
    std::atomic<std::uint64_t> fov_calibration_stable_since_ms_{0};
    std::atomic<float> fov_calibration_last_sample_deg_{0.0f};
    std::atomic<float> menu_fps_cap_target_{90.0f};
    std::atomic<float> menu_merged_mesh_target_{120.0f}; // V61.32 unified view/vegetation/merged distance target
    std::atomic<bool> vr_auto_projection_active_{false};
    std::atomic<float> vr_auto_fov_trim_deg_{0.0f};
    std::atomic<float> vr_auto_center_y_trim_{0.08f};
    std::atomic<bool> enable_bow_zoom_guard_{false};
    std::atomic<bool> apply_head_rotation_{true};
    std::atomic<bool> apply_head_position_{true};
    // V61.45: expose the already-implemented pose-honesty source ladder to normal gameplay.
    // 0=cached XR, 1=sync predicted, 2=sync now, 3=sync predicted + offset.
    std::atomic<int> vr_pose_source_mode_{2};
    // V62.4: native semantic HUD controls. No Lua/PAK/SWF replacement is required.
    // MovieClip names and vanilla design-space bases were verified against current KCD2 HUD.xml/hud.gfx.
    std::atomic<bool> enable_vr_ui_layout_{true};
    std::atomic<float> vr_ui_subtitle_scale_{0.55f};
    std::atomic<float> vr_ui_subtitle_y_{0.0f};          // design pixels relative to vanilla bc.subtitles
    std::atomic<float> vr_ui_dialogue_scale_{0.70f};
    std::atomic<float> vr_ui_dialogue_y_{0.0f};          // V62.7: design pixels relative to vanilla cl/cr Y=540
    std::atomic<float> vr_ui_dialogue_inset_pct_{14.0f}; // percent of half-screen (960px)
    std::atomic<float> vr_ui_hints_scale_{1.00f};
    std::atomic<float> vr_ui_hints_inset_pct_{35.0f};    // percent of half-screen, moves br.actionHints left
    std::atomic<float> vr_ui_hints_y_{0.0f};             // design pixels relative to vanilla actionHints
    // V62.9 Lua-persistent HUD element groups.
    std::atomic<float> vr_ui_compass_scale_{1.00f};
    std::atomic<float> vr_ui_compass_y_{0.0f};
    std::atomic<float> vr_ui_stats_scale_{1.00f};
    std::atomic<float> vr_ui_stats_x_{0.0f};
    std::atomic<float> vr_ui_stats_y_{0.0f};
    std::atomic<float> vr_ui_lower_scale_{1.00f};
    std::atomic<float> vr_ui_lower_y_{0.0f};
    std::atomic<float> vr_ui_lower_spread_{0.0f};
    std::atomic<bool> vr_ui_hide_letterbox_{false};
    std::atomic<float> vr_pose_prediction_offset_ms_{0.0f};
    std::atomic<bool> diagnostic_rendering_{false};
    std::atomic<bool> enable_vr_output_bridge_{true};
    std::atomic<float> vr_render_scale_{1.0f};
    // V46.3 conservative VR quality profile. Runtime-only CryEngine CVar edits
    // are snapshotted and restored on disable/shutdown. No culling/shadow/TAA
    // compatibility knobs are forced by this profile.
    std::atomic<bool> enable_vr_balanced_graphics_{true};
    std::atomic<float> vr_engine_clarity_{0.18f};
    std::atomic<float> vr_gamma_{1.0f};
    std::atomic<float> vr_brightness_{0.0f};
    std::atomic<float> vr_contrast_{1.0f};
    std::atomic<bool> hud_crosshair_visible_{true};
    std::atomic<bool> hud_lower_icons_visible_{true};
    std::atomic<bool> hud_compass_visible_{true};
    std::atomic<int> vr_dialogue_camera_mode_{1}; // 0 native, 1 native ForcePOV CVar, 2 native NoCameras CVar
    std::atomic<bool> enable_touch_bindings_{true};
    std::atomic<bool> enable_touch_attack_direction_{true};
    std::atomic<int> vr_presentation_mode_{3}; // 0=stretch, 1=fit, 2=crop, 3=OpenXR FOV remap
    std::atomic<float> vr_zoom_{1.0f};
    std::atomic<float> vr_source_vertical_fov_deg_{95.0f};
    std::atomic<float> vr_required_source_vfov_deg_{0.0f}; // V61.43 cached 5 Hz XR coverage diagnostic
    // V61.27 presentation-only optical-center diagnostic. Applied in the source->XR blit only;
    // never changes the gameplay CView, culling frustum, or submitted OpenXR eye poses/FOV.
    std::atomic<float> vr_presentation_center_y_{0.0f};
    std::atomic<float> vr_cull_frustum_scale_{1.60f}; // V60.4 culling-only tangent expansion in frustum builder
    std::atomic<float> vr_native_ui_scale_{1.00f}; // V61.38 live HUD root scale request, 0.50..1.20; no startup write
    std::atomic<float> vr_dialogue_text_scale_{1.00f}; // V61.37 subtitle size multiplier test, 0.50..1.50
    std::atomic<float> vr_flat_screen_scale_{1.58f}; // F6 virtual-screen size, separate from HUD
    // V61.45 RETIRED: dialogue/inventory zoom is handled by the V60.1 fixed source-FOV guard.
    // Kept only so old config/menu-state serialization code remains source-compatible; it has no runtime effect.
    std::atomic<float> vr_transient_zoom_retain_{0.00f};
    // V61.20 XML HUD rectangle tuner. These values generate a normal KCD2 HUD mod on demand; restart required.
    std::atomic<float> hud_xml_scale_{0.85f};
    std::atomic<int> hud_xml_offset_x_{0};
    std::atomic<int> hud_xml_offset_y_{0};
    std::atomic<float> vr_flat_ui_ipd_mm_{0.0f};
    std::atomic<bool> mirror_mis_menu_to_desktop_{false};
    std::atomic<float> vr_left_offset_x_{0.0f};
    std::atomic<float> vr_left_offset_y_{0.0f};
    std::atomic<float> vr_right_offset_x_{0.0f};
    std::atomic<float> vr_right_offset_y_{0.0f};
    std::atomic<int> vr_stereo_experiment_{0}; // 0=mono, 1=AFR, 2=sync-sequential scaffold, 3=native scaffold
    std::atomic<int> vr_cyclopean_mode_{1}; // V61.37 clean modes: 0=old mono, 1=corrected mono, 2=fixed pseudo .90, 3=depth pseudo .70..1.00
    std::atomic<float> vr_depth_stereo_strength_{0.30f}; // V61.37 depth influence 0..1; near asymmetry lerps 1.00 -> 0.70
    std::atomic<int> vr_rotation_order_{0}; // V61.13 recenter frame test: 0=B*D, 1=D*B
    std::atomic<float> vr_afr_ipd_scale_{0.0f};
    std::atomic<bool> fake_dof_active_{false};
    std::atomic<std::uint64_t> fake_dof_started_ms_{0};
    std::atomic<float> fake_dof_scale_{0.95f};
    std::atomic<bool> engine_dof_active_{false};
    std::atomic<bool> engine_dof_available_{false};
    std::atomic<float> engine_dof_focus_m_{0.0f};
    std::atomic<float> engine_dof_scale_{0.95f};
    std::atomic<bool> vr_swap_eyes_{false};
    std::atomic<bool> enable_legacy_probes_{false};
    // SAFETY GATE added during review of V10: independently opts the actual
    // native-camera-pair memory WRITE in and out, separate from
    // DiagnosticRendering - see stereo_archaeology.h's g_native_eye_write_enabled
    // comment. Defaults false: correlation/targeting logging runs regardless,
    // the write itself does not, until explicitly enabled via KCD2VR.ini.
    std::atomic<bool> enable_native_eye_write_{false};

    // Unified roadmap feature gates. Parsed and logged now, but deliberately
    // inert until each subsystem reaches its runtime phase.
    std::atomic<bool> enable_vrik_ownership_test_{false}; // V61.43 opt-in semantic +0x5FE2F0 ownership ladder
    std::atomic<bool> enable_body_ik_{false};
    std::atomic<bool> enable_virtual_holsters_{false};
    std::atomic<bool> enable_motion_controls_{false};
    std::atomic<bool> enable_native_melee_{false};
    std::atomic<bool> enable_stamina_weapon_handling_{false};
    std::atomic<bool> enable_physical_grabbing_{false};
    std::atomic<bool> enable_npc_physical_reaction_{false};
    std::atomic<bool> enable_physical_bow_{false};
    std::atomic<bool> enable_alchemy_vr_{false};
    std::atomic<bool> enable_smithing_vr_{false};
    std::atomic<bool> disable_target_lock_steering_{false};
    std::atomic<bool> hide_center_cursor_{false};
    std::atomic<bool> enable_wrist_body_ui_{false};
};

} // namespace kcd2vr