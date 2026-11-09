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
#include "core/trigger_ownership.h"

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
    bool RecalculateCorrectedProjection();
    bool CenterCorrectedProjection();
    bool SaveMenuSettings() const;
    void RequestMenuSave(bool beep) const; // C25: coalesced background SaveMenuSettings()
    void FlushMenuSave(const char* where) const; // C25: bounded shutdown wait

    std::unique_ptr<CameraHook> camera_hook_;
    std::unique_ptr<DeviceHook> device_hook_;
    std::unique_ptr<VrMenu> vr_menu_;
    TouchBindings touch_bindings_;
    core::TriggerOwnership gameplay_trigger_[2]{};
    bool trigger_vrik_was_live_=false,trigger_calibration_was_ready_=false;
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
    std::atomic<float> camera_height_offset_m_{0.035f};
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
    std::atomic<bool> menu_fps_cap_enabled_{false};
    std::atomic<float> menu_fps_cap_target_{90.0f};
    std::atomic<float> startup_original_fps_cap_{0.0f};
    std::atomic<float> menu_merged_mesh_target_{120.0f}; // V61.32 unified view/vegetation/merged distance target
    std::atomic<bool> vr_auto_projection_active_{false};
    std::atomic<float> vr_auto_fov_trim_deg_{0.0f};
    std::atomic<float> vr_auto_center_x_trim_{0.00f};
    std::atomic<float> vr_auto_center_y_trim_{0.00f};
    std::atomic<bool> vr_auto_projection_center_{true};
    std::atomic<float> vr_pitch_correction_deg_{0.0f};
#define KCD2VR_M2_HOST_SLOT 220 // M2_SOURCE_SELECTOR
#include "subsystems/ranged_reload/host.inl" // M2_OWNED_SOURCE slot=220
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    std::atomic<bool> apply_head_rotation_{true};
    std::atomic<bool> apply_head_position_{true};
    // V61.45: expose the already-implemented pose-honesty source ladder to normal gameplay.
    // 0=cached XR, 1=sync predicted, 2=sync now, 3=sync predicted + offset.
    std::atomic<int> vr_pose_source_mode_{0}; // C22b: Head smooth OFF by default (Miki)
    // V62.4: native semantic HUD controls. No Lua/PAK/SWF replacement is required.
    // MovieClip names and vanilla design-space bases were verified against current KCD2 HUD.xml/hud.gfx.
#define KCD2VR_M2_HOST_SLOT 221 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=221
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    std::atomic<bool> f2_ownership_fix_{true};          // V65.04 renderer-matched pose -> PRESENT pairing A/B
#define KCD2VR_M2_HOST_SLOT 222 // M2_SOURCE_SELECTOR
#include "subsystems/interactions/host.inl" // M2_OWNED_SOURCE slot=222
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 223 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=223
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 224 // M2_SOURCE_SELECTOR
#include "subsystems/dialogues/host.inl" // M2_OWNED_SOURCE slot=224
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 225 // M2_SOURCE_SELECTOR
#include "subsystems/physics/host.inl" // M2_OWNED_SOURCE slot=225
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 226 // M2_SOURCE_SELECTOR
#include "subsystems/dialogues/host.inl" // M2_OWNED_SOURCE slot=226
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 227 // M2_SOURCE_SELECTOR
#include "subsystems/interactions/host.inl" // M2_OWNED_SOURCE slot=227
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    // V64.22: ItemSelection + ItemTransfer share one tuning group. The XML
    // fixdyntexsize A/B remains optional; these runtime values are best-effort
    // Scaleform root/semantic transforms and fail closed when no known root is exposed.
#define KCD2VR_M2_HOST_SLOT 228 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=228
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    // Extra HUD element groups retained in config; Lua path is retired in V63.0.
#define KCD2VR_M2_HOST_SLOT 229 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=229
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    std::atomic<float> vr_pose_prediction_offset_ms_{0.0f};
    std::atomic<bool> diagnostic_rendering_{false};
    std::atomic<bool> enable_vr_output_bridge_{true};
    std::atomic<float> vr_render_scale_{1.0f};
    // V63.1: source/backbuffer resolution selection is mirrored in KCD2VR.ini, but renderer ownership stays with KCD2.
    // Graphics SAVE RESOLUTION writes r_Width/r_Height to game-root user.cfg for the next launch; no live/startup CVar mutation.
    std::atomic<int> vr_source_width_{1920};
    std::atomic<int> vr_source_height_{1800};
    // V46.3 conservative VR quality profile. Runtime-only CryEngine CVar edits
    // are snapshotted and restored on disable/shutdown. No culling/shadow/TAA
    // compatibility knobs are forced by this profile.
    std::atomic<bool> enable_vr_balanced_graphics_{true};
    std::atomic<float> vr_engine_clarity_{0.18f};
    std::atomic<float> vr_gamma_{1.0f};
    std::atomic<float> vr_brightness_{0.0f};
    std::atomic<float> vr_contrast_{1.0f};
#define KCD2VR_M2_HOST_SLOT 230 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=230
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 231 // M2_SOURCE_SELECTOR
#include "subsystems/dialogues/host.inl" // M2_OWNED_SOURCE slot=231
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 232 // M2_SOURCE_SELECTOR
#include "subsystems/interactions/host.inl" // M2_OWNED_SOURCE slot=232
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 233 // M2_SOURCE_SELECTOR
#include "subsystems/dialogues/host.inl" // M2_OWNED_SOURCE slot=233
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 234 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=234
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    // V66.150: ranged-only native CameraInputDispatch head aim; ordinary gameplay stays OpenXR free-look.
    // Ordinary immersive view retains OpenXR/HMD rotation ownership.
#define KCD2VR_M2_HOST_SLOT 235 // M2_SOURCE_SELECTOR
#include "subsystems/interactions/host.inl" // M2_OWNED_SOURCE slot=235
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    std::atomic<bool> native_activity_controls163_{false}; // session-only workstation override
#define KCD2VR_M2_HOST_SLOT 236 // M2_SOURCE_SELECTOR
#include "subsystems/direct/host.inl" // M2_OWNED_SOURCE slot=236
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    std::atomic<int> vr_turn_mode_{1}; // V64.17 0=snap, 1=smooth
#define KCD2VR_M2_HOST_SLOT 237 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=237
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 238 // M2_SOURCE_SELECTOR
#include "subsystems/vrik/host.inl" // M2_OWNED_SOURCE slot=238
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 239 // M2_SOURCE_SELECTOR
#include "subsystems/ranged_reload/host.inl" // M2_OWNED_SOURCE slot=239
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 240 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=240
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    std::atomic<int> vr_presentation_mode_{3}; // 0=stretch, 1=fit, 2=crop, 3=OpenXR FOV remap
    std::atomic<float> vr_zoom_{0.95f};
    std::atomic<float> vr_source_vertical_fov_deg_{95.0f}; // Normal/Pseudo source-FOV calibration only
    // V65.07 explicit Corrected Vision map commit. Slider edits do not silently
    // rebuild the projection until RECALCULATE is pressed.
    std::atomic<float> vr_corrected_source_vfov_deg_{95.0f}; // legacy field: last projective REMAP source FOV
    std::atomic<float> vr_corrected_view_vfov_deg_{95.0f};  // V66.30 requested VR view FOV for map-5 Correct FOV
    std::atomic<float> vr_corrected_zoom_{1.00f};            // V65.08 exact tangent mapping has no post-projection zoom
    std::atomic<float> vr_required_source_vfov_deg_{0.0f}; // V61.43 cached 5 Hz XR coverage diagnostic
    // V61.27 presentation-only optical-center diagnostic. Applied in the source->XR blit only;
    // never changes the gameplay CView, culling frustum, or submitted OpenXR eye poses/FOV.
    std::atomic<float> vr_presentation_center_x_{0.0f};
    std::atomic<float> vr_presentation_center_y_{0.0f};
    std::atomic<float> vr_cull_frustum_scale_{1.60f}; // V60.4 culling-only tangent expansion in frustum builder
#define KCD2VR_M2_HOST_SLOT 241 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=241
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 242 // M2_SOURCE_SELECTOR
#include "subsystems/dialogues/host.inl" // M2_OWNED_SOURCE slot=242
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    std::atomic<float> vr_flat_screen_scale_{1.70f}; // F6 virtual-screen size, separate from HUD
    std::atomic<float> vr_passive_full_frame_scale_{1.56f}; // R160 UI default (was 1.20) // V66.97 loading/cutscene-only Theater zoom
    std::atomic<float> vr_pause_overlay_size_{1.00f}; // V66.97 readable Pause/Menu content fit; background/DoF excluded
    // V61.45 RETIRED: dialogue/inventory zoom is handled by the V60.1 fixed source-FOV guard.
    // Kept only so old config/menu-state serialization code remains source-compatible; it has no runtime effect.
    std::atomic<float> vr_transient_zoom_retain_{0.00f};
    // V61.20 XML HUD rectangle tuner. These values generate a normal KCD2 HUD mod on demand; restart required.
#define KCD2VR_M2_HOST_SLOT 243 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=243
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    std::atomic<bool> mirror_mis_menu_to_desktop_{false};
    std::atomic<float> vr_left_offset_x_{0.0f};
    std::atomic<float> vr_left_offset_y_{0.0f};
    std::atomic<float> vr_right_offset_x_{0.0f};
    std::atomic<float> vr_right_offset_y_{0.0f};
    std::atomic<int> vr_stereo_experiment_{0}; // 0=mono, 1=AFR, 2=sync-sequential scaffold, 3=native scaffold
    std::atomic<bool> native_stereo_running_r89_{false}; // R89 dice: stage4 producer live (Run's f9_postfx_step==4); menu edits read it
    std::atomic<int> vr_production_stereo_mode_{3}; // 0=mono, 3=native dual-camera Stereo, 4=DLSS alternate-eye
    std::atomic<int> vr_cyclopean_mode_{0}; // clean modes: 0=mono, 1=projective FOV REMAP, 3=depth pseudo
    std::atomic<float> vr_depth_stereo_strength_{0.30f}; // V64.28 slider 0..1 maps near asymmetry 0.70 -> 0.55; far stays 1.00
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
#define KCD2VR_M2_HOST_SLOT 244 // M2_SOURCE_SELECTOR
#include "subsystems/vrik/host.inl" // M2_OWNED_SOURCE slot=244
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    // V64.16: menu requests are handed to the main loop so the shared VRIK
    // probe can stay completely uninstalled until an explicit F9/F10/F11/menu action.
    // -1 = no request, 0/1 = requested V65.09 F10 OFF / corrected-right-arm baseline.
#define KCD2VR_M2_HOST_SLOT 245 // M2_SOURCE_SELECTOR
#include "subsystems/vrik/host.inl" // M2_OWNED_SOURCE slot=245
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 246 // M2_SOURCE_SELECTOR
#include "subsystems/physics/host.inl" // M2_OWNED_SOURCE slot=246
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
#define KCD2VR_M2_HOST_SLOT 247 // M2_SOURCE_SELECTOR
#include "subsystems/ranged_reload/host.inl" // M2_OWNED_SOURCE slot=247
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
    std::atomic<bool> enable_alchemy_vr_{false};
    std::atomic<bool> enable_smithing_vr_{false};
    std::atomic<bool> disable_target_lock_steering_{false};
#define KCD2VR_M2_HOST_SLOT 248 // M2_SOURCE_SELECTOR
#include "subsystems/hud_ui_controls/host.inl" // M2_OWNED_SOURCE slot=248
#undef KCD2VR_M2_HOST_SLOT // M2_SOURCE_SELECTOR
};

} // namespace kcd2vr
