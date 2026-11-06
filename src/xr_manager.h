// xr_manager.h
#pragma once
#include "xr_layer_guard_r104.h"
#include "tracking_sample59c6.h"
#include "vrik/hand_present_d61.h"
#include "vrik/fresh_hand_target_r94.h"
#include "ui_surface_geometry.h"
#include "ui_menu_transition59c2.h"
#include "comfort_vignette51.h"
#include "core/publication_wait_d67.h"
#include "native_upscaler_stereo_d82.h"

#include <windows.h>
#include <d3d12.h>
#include <dxgi.h>
#define XR_USE_GRAPHICS_API_D3D12
#define XR_USE_PLATFORM_WIN32
#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>

#include <vector>
#include <cstdint>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <algorithm>
#include <cmath>
#include "camera_hook.h"
#include "xr_input.h"

namespace kcd2vr {

class XrManager
{
public:
    // V66.107: headset eye height above the floor (STAGE space). false when unsupported.
    bool GetEyeHeightAboveFloor(float& meters) const noexcept { meters=eye_height_floor_.load(std::memory_order_acquire); return eye_height_floor_valid_.load(std::memory_order_acquire); }
    struct StereoViewSnapshot
    {
        XrTime predicted_display_time = 0;
        XrPosef eye_pose[2]{};
        XrFovf eye_fov[2]{};
        bool valid = false;              // plausible IPD pair for injection
        bool locate_succeeded = false;
        std::uint32_t view_count = 0;
        XrViewStateFlags view_state_flags = 0;
        XrResult locate_result = XR_ERROR_RUNTIME_FAILURE;
        float raw_ipd = 0.0f;
        std::uint64_t captured_tick_ms = 0;
    };
    // V46: immutable-per-XR-frame snapshot. xrWaitFrame/xrBeginFrame/xrLocateViews
    // are performed once in RunFrame(), then every downstream consumer uses
    // this cached frame identity rather than independently locating views.
    struct RenderFrameState
    {
        std::uint64_t frame_index = 0;
        XrTime predicted_display_time = 0;
        bool should_render = false;
        bool head_pose_valid = false;
        XrPosef head_pose{};
        XrPosef eye_pose[2]{};
        XrFovf eye_fov[2]{};
        bool stereo_views_valid = false;
        bool injection_views_valid = false;
        std::uint32_t view_count = 0;
        XrViewStateFlags view_state_flags = 0;
        XrResult locate_result = XR_ERROR_RUNTIME_FAILURE;
        float raw_ipd = 0.0f;
    };

    struct TestCounters
    {
        std::uint64_t xr_frames = 0;
        std::uint64_t valid_frame_snapshots = 0;
        std::uint64_t mono_blits = 0;
        std::uint64_t afr_blits = 0;
        std::uint64_t fov_blits = 0;
        std::uint64_t marker_blits = 0;
        std::uint64_t swapped_compositions = 0;
    };
    XrManager() = default;
    ~XrManager() { Shutdown(); }

    XrManager(const XrManager&) = delete;
    XrManager& operator=(const XrManager&) = delete;

    static XrManager& Instance();

    bool EnsureInitialized(ID3D12Device* game_device, ID3D12CommandQueue* game_queue, const std::atomic<bool>* keep_running = nullptr);
    void Shutdown();
    bool RunFrame(XrPose& out_head_pose, float head_pose_scale = 1.0f);
    // V48: sample the HMD at current QPC time, close to KCD2 camera consumption.
    bool SampleLateHeadPose(XrPose& out_pose);
    bool CopyInputSampleStamp59c6(tracking59c6::Stamp& out) const {return input_stamp59c6_.read(out);}
    // V53: synchronous game-thread pose lookup at an explicit OpenXR time.
    // xrWaitFrame remains owned by the XR thread; xrLocateSpace is serialized by pose_locate_mutex_.
    struct FreshPoseUncappedReceipt {
        enum Result : unsigned { SourceCached, SourceOffset, TrackingDisabled, SessionUnavailable,
            ClockUnavailable, PoseBusy, GripBusy, Published, PublicationRejected, StalePrediction };
        Result result=SessionUnavailable;
        int source=-1;
        std::uint64_t sequence=0,sourceNs=0,sourceTick=0;
        XrTime target=0,requestedTarget=0;
        unsigned validMask=0,trackedMask=0;
        bool published=false;
        const char* Effective() const noexcept {return published?"INDEPENDENT_HAND_FAMILY":sequence?"RETAINED_SOURCE_AGE":"UNAVAILABLE";}
        const char* Reason() const noexcept {
            switch(result){
                case SourceCached:return "CACHED_CAMERA_SOURCE";case SourceOffset:return "OFFSET_CAMERA_SOURCE";
                case TrackingDisabled:return "TRACKING_DISABLED_OR_CHANGED";case SessionUnavailable:return "SESSION_UNAVAILABLE";
                case ClockUnavailable:return "CLOCK_UNAVAILABLE";case PoseBusy:return "POSE_BUSY";
                case GripBusy:return "GRIP_BUSY";case Published:return "PUBLISHED";
                case PublicationRejected:return "PUBLICATION_REJECTED";case StalePrediction:return "STALE_PREDICTION";
            }
            return "UNKNOWN";
        }
    };
    // Pose-only refresh at the existing pre-update boundary. No token/action wait,
    // no camera/origin mutation. Failed attempts retain the mailbox's source age.
    FreshPoseUncappedReceipt RefreshUncappedPoseFamily() noexcept;
    bool SampleHeadPoseAtTime(XrTime target_time, XrPose& out_pose, hj1::Sample* out_family = nullptr,
                              FreshPoseUncappedReceipt* out_refresh = nullptr);
    bool SampleHeadPoseNow(XrPose& out_pose, XrTime* out_time = nullptr, hj1::Sample* out_family = nullptr);
    XrTime GetLatestPredictedDisplayTime() const { return static_cast<XrTime>(latest_predicted_display_time_.load(std::memory_order_acquire)); }
    // HJ1 (vrik/hand_same_frame_hj1.h): game-thread controller locate at the predictedDisplayTime of the XR frame whose
    // head the render camera will use. try-lock only, never stalls the game thread. head path untouched.
    bool LocateFamilyForGameFrameHJ1(XrPosef (&poses)[3], XrSpaceLocationFlags (&flags)[3], XrTime& target, int& ahead, XrTime& now, hj1::Sample* extraMetadataD69=nullptr, diagnostics::motion_d68::Event* predictionTraceD69=nullptr);
    void NoteCameraApplyHJ1(XrTime camera_target, std::uint64_t camera_frame = 0);
    std::int64_t Hj1PeriodNs() const { return hj1_period_ns_.load(std::memory_order_acquire); } // W3 smooth lab pacing
    void SetLatePoseEnabled(bool enabled) { late_pose_enabled_.store(enabled,std::memory_order_release); }
    bool GetLatePoseEnabled() const { return late_pose_enabled_.load(std::memory_order_acquire); }
    void SetPresentPacingMode(int mode) { present_pacing_mode_.store(std::clamp(mode,0,3), std::memory_order_release); }
    // V64.48: Crysis-style XR-driven game-frame gate. xrWaitFrame remains on
    // the XR worker; the gameplay camera waits for a newly published immutable
    // XR frame token before KCD2 publishes/renders that camera.
    void SetXrRenderGateMode(int mode) {
        publication_wait_d67::notifyChange(frame_gate_mutex_,frame_gate_cv_,[&]{
            xr_render_gate_mode_.store(std::clamp(mode,0,3),std::memory_order_release);
        });
    }
    int GetXrRenderGateMode() const { return xr_render_gate_mode_.load(std::memory_order_acquire); }
    bool WaitForNextRenderFrameToken(std::uint64_t& inout_frame, std::uint32_t timeout_ms = 20, bool force = false, bool fresh_after_arrival_d67 = false); // force = W3 smooth lab pacing with gate mode 0
    void SetExpectedPresentCameraFrame(std::uint64_t frame) { expected_present_camera_frame_.store(frame, std::memory_order_release); }

    // Invoked only from DeviceHook's ExecuteCommandLists detour, i.e. on the
    // thread currently submitting the game's DIRECT-queue work.
    void OnRenderThreadTick(ID3D12CommandQueue* queue);
    void SetDiagnosticRendering(bool enabled) { diagnostic_rendering_.store(enabled, std::memory_order_release); }
    // Controls page, VrComfortVignette: off/low/medium/strong. Presentation only.
    void SetComfortVignette(int level) noexcept { comfort_vignette_level_.store(comfort_vignette51::ClampLevel(level),std::memory_order_release); }
    void SetComfortGameplay51(bool gameplay,bool smoothTurn) noexcept {
        comfort_context51_.store((gameplay?1u:0u)|(smoothTurn?2u:0u),std::memory_order_release);
    }
    // F8 ladder additions - see xr_manager.h member comments for each.
    void SetAngularCorrectMono(bool enabled) { angular_correct_mono_.store(enabled, std::memory_order_release); }
    void SetDepthPushEnabled(bool enabled) { depth_push_enabled_.store(enabled, std::memory_order_release); }
    void SetDepthPushWeight(float w) { depth_push_weight_signed_.store(std::clamp(w, 0.0f, 1.0f), std::memory_order_release); }
    void SetPoseHonestyOverride(int mode) { pose_honesty_override_.store(mode, std::memory_order_release); }
    // V59.0: scales the horizontal OpenXR eye-FOV asymmetry around a centered
    // cyclopean frustum while both eyes share one submitted camera origin.
    // +1 = runtime/native asymmetry (V58.9 phase D), 0 = centered mono,
    // -1 = same magnitude with L/R asymmetry reversed. This is a presentation
    // diagnostic only; it does not claim two independently rendered scene eyes.
    void SetMonoFovAsymmetryScale(float scale) { mono_fov_asymmetry_scale_.store(scale, std::memory_order_release); }
    // V59.1 mono UV reprojection diagnostics. mode 0=off, 1=rotation-only
    // current-output ray transformed back into the render-paired camera space. mode 2=inverse-sign diagnostic.
    void SetMonoUvReprojectionMode(int mode) { mono_uv_reprojection_mode_.store(mode, std::memory_order_release); }
    void SetMonoUvBinocularOffset(float offset) { mono_uv_binocular_offset_.store(offset, std::memory_order_release); }
    // V61.27: common source-UV Y offset for projection-centering diagnosis. This is compositor/presentation
    // mapping only: CView, culling and OpenXR projection-view poses/FOV remain untouched.
    void SetPresentationCenterX(float offset) { presentation_center_x_.store(std::clamp(offset, -0.20f, 0.20f), std::memory_order_release); }
    void SetPresentationCenterY(float offset) { presentation_center_y_.store(std::clamp(offset, -0.20f, 0.20f), std::memory_order_release); }
    void SetHudLag(float v) noexcept { hud_lag_.store(std::isfinite(v)?std::clamp(v,0.0f,1.0f):0.0f,std::memory_order_release); } // C22b (C20 port)
    void SetHudLagGameplay(bool g) noexcept { hud_lag_gameplay_.store(g,std::memory_order_release); }
    void SetPitchCorrectionDeg(float deg) noexcept { pitch_correction_rad_.store(std::clamp(deg,-15.0f,15.0f)*0.01745329251994329577f,std::memory_order_release); }
    void SetAutoProjectionCenter(bool enabled) noexcept { auto_projection_center_.store(enabled,std::memory_order_release); }
    float GetEffectivePitchCorrectionDeg() const noexcept { return effective_pitch_correction_deg_.load(std::memory_order_acquire); }
    // V59.2: bottom-edge-only treatment for the good game-FOV-110 mono mapping.
    // 0=black reference, 1=vertical shift, 2=Y-only mild compression,
    // 3/4=lower-tail remap preserving upper 80/90%, 5=bottom edge hold.
    void SetMonoBottomEdgeMode(int mode) { mono_bottom_edge_mode_.store(mode, std::memory_order_release); }
    void ResetClampStats() { clamp_active_count_.store(0, std::memory_order_relaxed); clamp_total_count_.store(0, std::memory_order_relaxed); }
    void GetClampStats(std::uint64_t& active, std::uint64_t& total) const {
        active = clamp_active_count_.load(std::memory_order_relaxed);
        total = clamp_total_count_.load(std::memory_order_relaxed);
    }
    void SetActiveVrBridge(bool enabled) { active_vr_bridge_.store(enabled, std::memory_order_release); }
    // V65.93 F5 bridge modes (rungs 2-5): 17=A->L/B->R, 18=swapped,
    // 19=A->both, 20=B->both. Rung 1 is read-only; all mappings fail closed until the post-submit pair is ready.
    void SetStereoExperiment(int mode, float ipd_scale) { stereo_experiment_.store(mode, std::memory_order_release); afr_ipd_scale_.store(ipd_scale, std::memory_order_release); }
    int StereoExperimentR89() const noexcept { return stereo_experiment_.load(std::memory_order_acquire); } // R89 dice
    // V65.89: gate capture of the final COUNT2 PRESENT raster used by the resurrected
    // V65.76-79 packed-eye presentation path. Disabling capture freezes the last
    // owned frame; stereo_experiment mode 12 can continue presenting that snapshot.
    void SetV6589PackedCaptureEnabled(bool enabled) { v6589_packed_capture_enabled_.store(enabled, std::memory_order_release); }
    std::uint64_t V6589PackedFrameSerial() const { return v6589_packed_frame_serial_.load(std::memory_order_acquire); }
    // V65.90 one-shot Present-ghost capture. Ready is cleared before arming so an
    // old V65.89 snapshot can never masquerade as the newly requested reproduction.
    void SetSwapEyes(bool enabled) { swap_eyes_.store(enabled, std::memory_order_release); }
    // V58.4/V58.7 mono angular mapping is controlled by the F8-safe setters above.
    void SetPresentationSettings(float render_scale, int mode, float zoom,
                                 float left_x, float left_y, float right_x, float right_y, float source_vfov_deg) {
        render_scale_.store(render_scale, std::memory_order_release);
        presentation_mode_.store(mode, std::memory_order_release);
        presentation_zoom_.store(zoom, std::memory_order_release);
        source_vertical_fov_deg_.store(source_vfov_deg, std::memory_order_release);
        eye_offset_x_[0].store(left_x, std::memory_order_release);
        eye_offset_y_[0].store(left_y, std::memory_order_release);
        eye_offset_x_[1].store(right_x, std::memory_order_release);
        eye_offset_y_[1].store(right_y, std::memory_order_release);
    }
    // Diagnostic copies only; no remap, reset, queue access or XR call. The
    // individual atomics are intentionally labelled NONATOMIC in the receipt.
    struct PresentationObservationD86 {
        float zoom=0,dialogueMultiplier=0,sourceFov=0;
        int presentation=-1,mapping=-1;
    };
    PresentationObservationD86 ObservePresentationD86() const noexcept {
        return {presentation_zoom_.load(std::memory_order_acquire),dialogue_zoom_multiplier_.load(std::memory_order_acquire),
            source_vertical_fov_deg_.load(std::memory_order_acquire),presentation_mode_.load(std::memory_order_acquire),
            mono_uv_reprojection_mode_.load(std::memory_order_acquire)};
    }
    const ControllerState& LeftController() const { return input_.Left(); }
    const ControllerState& RightController() const { return input_.Right(); }
    bool GetMenuPointer(int& x, int& y) const { x=menu_pointer_x_.load(std::memory_order_acquire); y=menu_pointer_y_.load(std::memory_order_acquire); return menu_pointer_active_.load(std::memory_order_acquire); }
    // V63.0: normalized hit point on the F6 LOCAL theater quad. Used only for
    // UI pointer injection; never changes the game camera or interaction ray.
    bool GetFlatUiPointer(float& u, float& v) const {
        u=flat_pointer_u_.load(std::memory_order_acquire); v=flat_pointer_v_.load(std::memory_order_acquire);
        return flat_pointer_active_.load(std::memory_order_acquire);
    }
    // Input owner chosen by the UI arbiter; -1 means no native menu pointer.
    void SetImmersiveMenuPointer(bool enabled) { immersive_menu_pointer_.store(enabled,std::memory_order_release); }
    void SetNativeMenuPointerHand(int hand) { native_menu_pointer_hand_.store(hand,std::memory_order_release); }
    int FlatUiPointerHand() const { return flat_pointer_hand_.load(std::memory_order_acquire); }
    void SetFullMenuFit(bool enabled) { full_menu_fit_.store(enabled,std::memory_order_release); }
    void SetPassiveFullFrameScale(float scale) { passive_full_frame_scale_.store(std::clamp(scale,1.0f,2.0f),std::memory_order_release); } // R107: up to 2x
    // R107: show only the central band of this aspect (16:9) on the Theater screen; 0 = whole frame.
    void SetFlatCropAspectR107(float aspect) { flat_crop_aspect_r107_.store(aspect,std::memory_order_release); }
    bool GetHudSurfaceGeometry(ui_surface_geometry::Hud& geometry,std::uint64_t& epoch) {
        std::lock_guard<std::mutex> lock(hud_geometry_mutex_);geometry=hud_geometry_;epoch=hud_geometry_epoch_;return geometry.surface.valid;
    }
    // V63.0: publish the Win32/right-stick fallback cursor back to the XR thread so
    // F6 always has a visible hit dot even when the controller ray misses the quad.
    void SetUiLaserVisible(bool visible) { ui_laser_visible_.store(visible, std::memory_order_release); }
    void SetUiLayerTest(bool enabled,bool menu_active) { ui_layer_test_enabled_.store(enabled,std::memory_order_release); ui_layer_test_menu_active_.store(menu_active,std::memory_order_release); }
    void SetFlatUiCursorVisual(float u, float v, bool active) {
        flat_cursor_visual_u_.store(std::clamp(u,0.0f,1.0f), std::memory_order_release);
        flat_cursor_visual_v_.store(std::clamp(v,0.0f,1.0f), std::memory_order_release);
        flat_cursor_visual_active_.store(active, std::memory_order_release);
    }
    bool IsRunning() const { return session_running_.load(std::memory_order_relaxed); }
    // R109: the VR menu / lasers draw from the menu swapchain; without it an opened menu is invisible but still owns the controls.
    bool MenuSurfaceReadyR109() const { return menu_ui_.swapchain != XR_NULL_HANDLE; }
    bool CopyLatestStereoViews(StereoViewSnapshot& out);
    bool CopyLastGoodStereoViews(StereoViewSnapshot& out);
    bool CopyActiveRenderFrameState(RenderFrameState& out);
    bool CopyRenderFrameStateByIndex(std::uint64_t frame_index, RenderFrameState& out);
    void SetPresentPosePairingMode(int mode) { present_pose_pairing_mode_.store(mode, std::memory_order_release); }
    // V66.27 F2 experiment: OFF preserves V66.26 blocking native-stereo presentation.
    // ON binds certified L/R pixels to their renderer-owned pose token and uses the
    // nonblocking XR submission ring/stable-cache-only fallback.
    void SetStereoStabilityExperiment(bool enabled) { stereo_stability_experiment_.store(enabled, std::memory_order_release); }
    bool StereoStabilityExperimentEnabled() const { return stereo_stability_experiment_.load(std::memory_order_acquire); }
    // V66.34: non-blocking stereo pair submit (V66.27 allocator/list/SRV ring ONLY).
    // Removes the CPU post-wait that made every XR frame wait for the game's GPU frame
    // on the shared DIRECT queue. Independent of the retired V66.27 stale-cache fallback,
    // which stays keyed to StereoStabilityExperiment (OFF). F2 toggles this for A/B.
    // V52: 0=dynamic render pose (current behavior), 1=frozen render pose captured on next XR frame.
    void SetProjectionPoseMode(int mode) { projection_pose_mode_.store(mode, std::memory_order_release); frozen_projection_valid_.store(false,std::memory_order_release); }
    TestCounters GetTestCounters() const;
    void GetEyeSize(int& out_width, int& out_height) const { out_width = eyes_[0].width; out_height = eyes_[0].height; }
    void SetPresentationTestMarker(bool enabled) { presentation_test_marker_.store(enabled, std::memory_order_release); }
    // Legacy/manual flat-UI mode. Keeps the panel world-level (yaw only).
    void SetFlatUiMode(bool enabled);
    // V66.20 automatic large-menu presentation. The game still renders its native
    // fullscreen UI normally, but XR presents the completed flat output on a LOCAL-space
    // plane captured in front of the current gaze (yaw+pitch, roll removed). The plane
    // stays fixed while the player looks around the black surrounding space.
    void SetNativeMenuPlaneMode(bool enabled);
    void SetProductionPresentation59c2(bool enabled,bool theater) noexcept {
        production_presentation59c2_.Publish(enabled,theater);
    }
    // V66.32/V66.33: theater presentation keeps native stereo production alive. While
    // the flag is set and XR is in flat mode 0, the XR RENDER THREAD consumes certified
    // scene pairs without displaying them so producer backpressure cannot wedge.
    // V66.33 moved the drain onto the render thread: bridge_last_consumed_pair_serial_
    // is render-thread-only state and the V66.32 call from Mod::Run raced with it.
    void SetTheaterDrain(bool enabled) { theater_drain_active_.store(enabled, std::memory_order_release); }
    void SetFlatUiCurved(bool enabled) { flat_ui_curved_.store(enabled, std::memory_order_release); }
    // V66.69 F4 physical-map proof: present the game/native Map surface on a hand-held LOCAL-space quad.
    void SetPhysicalMapMode(bool enabled) { physical_map_mode_.store(enabled, std::memory_order_release); }
    // V66.69 F4 immersive interaction beam. Pose is LOCAL-space and visual-only;
    // the authoritative KCD2 interaction ray is published separately through interaction_alignment.
    void SetHandInteractionLaserLocal(bool active, float ox, float oy, float oz, float dx, float dy, float dz, float length_m = 3.0f) {
        if (active) {
            const float len = std::sqrt(dx*dx + dy*dy + dz*dz);
            if (!(len > 1.0e-4f) || !std::isfinite(len)) { hand_interaction_laser_active_.store(false, std::memory_order_release); return; }
            dx/=len; dy/=len; dz/=len;
            const float l=std::clamp(length_m,0.10f,8.0f);
            hand_interaction_laser_ox_.store(ox,std::memory_order_relaxed);
            hand_interaction_laser_oy_.store(oy,std::memory_order_relaxed);
            hand_interaction_laser_oz_.store(oz,std::memory_order_relaxed);
            hand_interaction_laser_ex_.store(ox+dx*l,std::memory_order_relaxed);
            hand_interaction_laser_ey_.store(oy+dy*l,std::memory_order_relaxed);
            hand_interaction_laser_ez_.store(oz+dz*l,std::memory_order_relaxed);
        }
        hand_interaction_laser_active_.store(active,std::memory_order_release);
    }
    void SetDialogueZoomMultiplier(float multiplier) { dialogue_zoom_multiplier_.store(std::clamp(multiplier,0.05f,2.50f), std::memory_order_release); }
    void SetFlatUiScale(float scale) { flat_ui_scale_.store(scale, std::memory_order_release); }
    void SetFlatUiDistance(float meters) { flat_ui_distance_.store(std::clamp(meters,0.80f,3.20f), std::memory_order_release); }
    void SetFlatUiIpdMm(float mm) { flat_ui_ipd_mm_.store(mm, std::memory_order_release); }
    void SetMenuQuadSource(ID3D12Resource* texture, bool visible, bool diagnosticOnly=false) { menu_quad_source_.store(texture, std::memory_order_release); menu_quad_diagnostic_h24_.store(visible&&diagnosticOnly,std::memory_order_release); menu_quad_visible_.store(visible&&!diagnosticOnly, std::memory_order_release); }
    void SetSharpening(float strength) { sharpening_strength_.store(std::clamp(strength, 0.0f, 0.75f), std::memory_order_release); }
    void SetSourceVerticalFov(float vfov_deg) { source_vertical_fov_deg_.store(std::clamp(vfov_deg,70.0f,140.0f), std::memory_order_release); }
    float GetSourceVerticalFov() const { return source_vertical_fov_deg_.load(std::memory_order_acquire); }
    // V65.08: nominal corrected-view FOV is metadata only; exact mapping uses OpenXR rays and the hard 120-degree source.
    // V65.07 originally separated the actual KCD2 source frustum from
    // the desired headset framing FOV. This value never writes a game CVar.
    void SetCorrectedViewVerticalFov(float vfov_deg) { corrected_view_vertical_fov_deg_.store(std::clamp(vfov_deg,60.0f,140.0f), std::memory_order_release); }
    float GetCorrectedViewVerticalFov() const { return corrected_view_vertical_fov_deg_.load(std::memory_order_acquire); }
    // V62.6: compositor-side color correction. Never writes KCD2 renderer CVars.
    void SetColorAdjustment(float gamma, float brightness, float contrast) {
        output_gamma_.store(std::clamp(gamma,0.50f,2.00f), std::memory_order_release);
        output_brightness_.store(std::clamp(brightness,-0.50f,0.50f), std::memory_order_release);
        output_contrast_.store(std::clamp(contrast,0.50f,2.00f), std::memory_order_release);
    }
    void RequestSwapchainRecreate(float render_scale) { requested_render_scale_.store(std::clamp(render_scale, 0.50f, 2.0f), std::memory_order_release); }
    bool GetFlatUiMode() const { return flat_ui_mode_.load(std::memory_order_acquire); }
    // V58.7: symmetric source VFOV required to cover the current asymmetric OpenXR views.
    // Used by the startup F6 guard so immersive mono is enabled only when the source can cover the HMD without a global zoom clamp.
    float GetRequiredMonoSourceVFovDeg(float source_aspect = 16.0f / 9.0f);
    // Returns the REAL, currently-captured source resolution (updated every
    // blit call - see last_source_width_/last_source_height_), not a static
    // assumption. Found during review: GetRequiredMonoSourceVFovDeg's only
    // call site was passing a hardcoded 2560/1410 (the old resolution),
    // computing the "required FOV" diagnostic against the WRONG aspect
    // ratio once the user moved to a different resolution (e.g. 1920x1800).
    void GetLastCapturedSourceResolution(std::uint32_t& out_width, std::uint32_t& out_height) const {
        out_width = last_source_width_.load(std::memory_order_acquire);
        out_height = last_source_height_.load(std::memory_order_acquire);
    }

private:
    struct EyeSwapchain
    {
        XrSwapchain swapchain = XR_NULL_HANDLE;
        int32_t width = 0;
        int32_t height = 0;
        std::vector<ID3D12Resource*> d3d12_images;
    };

    struct RtvEntry
    {
        ID3D12Resource* resource = nullptr;
        D3D12_CPU_DESCRIPTOR_HANDLE handle{};
    };

    XrInstance instance_ = XR_NULL_HANDLE;
    XrSystemId system_id_ = XR_NULL_SYSTEM_ID;
    XrSession session_ = XR_NULL_HANDLE;
    XrSpace local_space_ = XR_NULL_HANDLE;
    XrSpace view_space_ = XR_NULL_HANDLE;
    XrSpace stage_space_ = XR_NULL_HANDLE; // V66.107: floor reference for head height (optional)
    std::atomic<float> eye_height_floor_{0.0f};std::atomic<bool> eye_height_floor_valid_{false};

    ID3D12Device* d3d12_device_ = nullptr;
    ID3D12CommandQueue* d3d12_queue_ = nullptr;

    ID3D12CommandAllocator* command_allocator_ = nullptr;
    ID3D12GraphicsCommandList* command_list_ = nullptr;
    ID3D12Fence* fence_ = nullptr;
    HANDLE fence_event_ = nullptr;
    UINT64 fence_value_ = 0;

    ID3D12DescriptorHeap* rtv_heap_ = nullptr;
    ID3D12DescriptorHeap* srv_heap_ = nullptr;
    ID3D12RootSignature* blit_root_signature_ = nullptr;
    ID3D12PipelineState* blit_pipeline_state_ = nullptr;
    UINT rtv_descriptor_size_ = 0;
    UINT srv_descriptor_size_ = 0;
    std::vector<RtvEntry> rtv_entries_;
    DXGI_FORMAT swapchain_format_ = DXGI_FORMAT_UNKNOWN;

    HRESULT hr_check_feature_level_ = E_FAIL;
    HRESULT hr_check_adapter_ = E_FAIL;

    EyeSwapchain eyes_[2];
    EyeSwapchain menu_ui_{};
    DXGI_FORMAT menu_ui_format_ = DXGI_FORMAT_UNKNOWN;
    std::vector<bool> menu_ui_image_initialized_{}; // V63.0: one-time F6 gold-texel initialization per XR image
    std::atomic<ID3D12Resource*> menu_quad_source_{nullptr};
    std::atomic<bool> menu_quad_visible_{false};
    std::atomic<bool> menu_quad_diagnostic_h24_{false};
    std::atomic<int> menu_pointer_x_{325};
    std::atomic<int> menu_pointer_y_{138};
    std::atomic<bool> menu_pointer_active_{false};
    std::atomic<bool> immersive_menu_pointer_{false};
    std::atomic<int> native_menu_pointer_hand_{-1},flat_pointer_hand_{-1};
    std::atomic<bool> full_menu_fit_{false};
    std::atomic<float> passive_full_frame_scale_{1.0f};
    std::atomic<float> flat_crop_aspect_r107_{0.0f};
    std::mutex hud_geometry_mutex_;
    ui_surface_geometry::HudLatch hud_geometry_latch_;
    ui_surface_geometry::Hud hud_geometry_{};
    std::uint64_t hud_geometry_epoch_=0;
    std::atomic<float> flat_pointer_u_{0.5f};
    std::atomic<float> flat_pointer_v_{0.5f};
    std::atomic<bool> flat_pointer_active_{false};
    std::atomic<float> flat_cursor_visual_u_{0.5f};
    std::atomic<float> flat_cursor_visual_v_{0.5f};
    std::atomic<bool> flat_cursor_visual_active_{false};
    std::atomic<bool> ui_laser_visible_{true};
    std::atomic<bool> ui_layer_test_enabled_{false};
    std::atomic<bool> ui_layer_test_menu_active_{false};
    std::atomic<bool> ui_layer_test_pointer_active_{false};
    std::atomic<float> ui_layer_test_pointer_u_{0.5f},ui_layer_test_pointer_v_{0.5f};
    bool ui_layer_test_anchor_valid_=false;
    std::atomic<float> hud_lag_{0.4f};std::atomic<bool> hud_lag_gameplay_{false}; // C22b lagging HUD (0 = head-locked)
    // R104/R106 overlay layer guard (XR thread only), xr_layer_guard_r104.h
    xr_layer_guard_r104::Quarantine layer_guard_r104_{};
    std::uint32_t max_layer_count_r104_=0;
    std::uint64_t layer_sent_r104_[xr_layer_guard_r104::Count]{},layer_held_r104_[xr_layer_guard_r104::Count]{};
    std::uint64_t layer_drop_r104_[xr_layer_guard_r104::Count]{},layer_fix_r104_[xr_layer_guard_r104::Count]{};
    std::uint64_t end_ok_r104_=0,end_fail_r104_=0;
    std::uint64_t layer_fail_next_r104_=0,layer_fail_rows_r104_=0,layer_kind_next_r104_=0,layer_kind_rows_r104_=0;
    std::uint64_t layer_status_next_r104_=0,layer_status_sig_r104_=0;unsigned last_kinds_r104_=~0u;
    void NoteLayerDropR104(unsigned kind,unsigned drop) noexcept;
    void NoteLayerFixR104(unsigned kind,unsigned fixes) noexcept;
    void NoteEndFrameR105(unsigned kinds,int result,std::uint32_t sitOutMs,std::uint64_t now) noexcept;
    // R111 menu layer (XR thread only): the menu swapchain keeps its last RELEASED image; a frame that gets no new
    // image (acquire/wait/copy failed, texture not ready) re-submits that image instead of dropping menu + laser.
    // Counters per reason feed the always-on "R111 XR_UI_LAYERS" 5 s row (written only when they changed).
    enum MenuMissR111 : unsigned { MenuRequested=0, MenuSent, MenuFresh, MenuReused, MenuNotSent, MissNoScene, MissEyes, MissNoSwapchain,
        MissNoSource, MissAcquire, MissWait, MissCopy, MissRelease, MissSceneCancelled, MissGuard, MenuLaserSent, MenuSourceChanges, MenuR111Count };
    std::uint64_t menu_r111_[MenuR111Count]{};
    std::uint64_t menu_r111_next_ms_=0,menu_r111_sig_=0;
    bool menu_ui_has_released_r111_=false;
    ID3D12Resource* menu_source_seen_r111_=nullptr; // identity only, never dereferenced
    void NoteMenuFrameR111(std::uint64_t now) noexcept;
    XrQuaternionf hud_lag_q_{0,0,0,1};bool hud_lag_valid_=false;long long hud_lag_last_ns_=0; // render thread only
    XrPosef ui_layer_test_anchor_pose_{{0,0,0,1},{0,0,0}};
    mutable std::mutex ui_layer_test_mutex_;
    XrPosef ui_layer_test_plane_pose_{{0,0,0,1},{0,0,0}};
    float ui_layer_test_plane_width_=1.82f,ui_layer_test_plane_height_=1.02f;
    std::atomic<bool> session_running_{false};
    bool init_attempted_ = false;
    bool head_receipt_pending_r105_=true;
    void PollSessionEventsR105();
    void ResetUiAnchorsR105() noexcept;
    std::atomic<bool> diagnostic_rendering_{false};
    std::atomic<bool> active_vr_bridge_{true};
    std::atomic<float> render_scale_{1.0f};
    std::atomic<float> requested_render_scale_{0.0f};
    std::atomic<float> active_swapchain_scale_{1.0f};
    std::atomic<float> sharpening_strength_{0.18f};
    std::atomic<float> output_gamma_{1.0f};
    std::atomic<int> comfort_vignette_level_{0};
    std::atomic<unsigned> comfort_context51_{0}; // gameplay + smooth turn, one publication
    comfort_vignette51::Envelope comfort_envelope51_; // XR thread only; value travels with the presentation frame
    std::atomic<float> output_brightness_{0.0f};
    std::atomic<float> output_contrast_{1.0f};
    bool cylinder_layer_supported_ = false;
    std::atomic<int> presentation_mode_{1};
    std::atomic<float> presentation_zoom_{1.0f};
    std::atomic<float> dialogue_zoom_multiplier_{1.0f}; // V63.4 presentation-only forced-1P dialogue framing; 1.0 outside dialogue
    std::atomic<float> source_vertical_fov_deg_{95.0f};
    std::atomic<float> corrected_view_vertical_fov_deg_{105.0f}; // V65.27 user-selected VR FOV calibration for Correct FOV
    std::atomic<bool> angular_correct_mono_{false};
    // V61.37: depth-conditioned asymmetry mapping. In clean depth mode the
    // shader keeps far/background at 1.00 eye asymmetry and pulls near pixels
    // toward 0.70 according to this 0..1 influence. No draw-count probe.
    std::atomic<bool> depth_push_enabled_{false};
    std::atomic<float> depth_push_weight_signed_{0.30f};
    // F8 ladder addition: independent override for honest-mono-pose,
    // separate from the stereo_experiment_==0 gating below. -1 = use the
    // default gating (unchanged behavior for normal play); 0 = force the
    // OLD fake-separated-eye pose; 1 = force the NEW honest common-origin
    // pose. Lets the F8 ladder isolate crop-mode and pose-honesty
    // independently instead of only testing them bundled together.
    std::atomic<int> pose_honesty_override_{-1};
    std::atomic<float> mono_fov_asymmetry_scale_{1.0f};
    std::atomic<int> mono_uv_reprojection_mode_{0};
    std::atomic<float> mono_uv_binocular_offset_{0.0f};
    std::atomic<float> presentation_center_x_{0.0f};
    std::atomic<float> presentation_center_y_{0.0f};
    std::atomic<float> pitch_correction_rad_{0.0f};
    std::atomic<bool> auto_projection_center_{true};
    std::atomic<float> effective_pitch_correction_deg_{0.0f};
    std::atomic<int> mono_bottom_edge_mode_{0};
    // F8 ladder addition: aggregates the ALREADY-COMPUTED coverage_clamped_eye[]
    // flag (see BlitAfrStereoToEyes/BlitGameImageToEyes - this is the exact
    // boolean the musings doc's 0.697-vs-0.995 finding was based on) across a
    // whole phase, so a phase-end report can state directly "clamp was active
    // in N% of blits" instead of inferring it from scale numbers alone. CPU-
    // side only - no shader/GPU-readback changes needed, since this value
    // already exists per blit call.
    std::atomic<std::uint64_t> clamp_active_count_{0};
    std::atomic<std::uint64_t> clamp_total_count_{0};
    std::atomic<float> eye_offset_x_[2] = {0.0f, 0.0f};
    std::atomic<float> eye_offset_y_[2] = {0.0f, 0.0f};
    std::atomic<int> stereo_experiment_{1}; // 0=mono,1=AFR,2=certified pair,3=COUNT2 raw candidate regions,4=COUNT2 per-eye live-source projective intrinsics,5=V65.85 candidate browser,11=V65.88 A/B pair,12=V65.89 packed COUNT2 Present,13..16=V65.90 deterministic first-F9 Present-ghost reproduction/controls
    std::atomic<int> v6585_candidate_index_{-1};
    std::atomic<float> afr_ipd_scale_{1.0f};
    // V65.89 packed COUNT2 final-raster snapshot. Owned by the render-thread bridge.
    std::atomic<bool> v6589_packed_capture_enabled_{false};
    std::atomic<std::uint64_t> v6589_packed_frame_serial_{0};
    std::uint64_t v6589_packed_source_present_serial_ = 0;
    ID3D12Resource* v6589_packed_snapshot_ = nullptr;
    D3D12_RESOURCE_DESC v6589_packed_desc_{};
    std::atomic<bool> v6590_ghost_ready_{false};
    std::atomic<bool> swap_eyes_{false};
    std::atomic<bool> presentation_test_marker_{false};
    std::atomic<bool> flat_ui_mode_{false};
    std::atomic<bool> native_menu_plane_mode_{false}; // V66.20: gaze-anchored LOCAL menu plane, not head locked
    std::atomic<bool> flat_ui_curved_{false}; // V64.12 shallow concave theater when non-interactive
    std::atomic<bool> physical_map_mode_{false}; // V66.69: controller-midpoint hand-held native map quad
    std::atomic<bool> hand_interaction_laser_active_{false};
    std::atomic<float> hand_interaction_laser_ox_{0.0f}, hand_interaction_laser_oy_{0.0f}, hand_interaction_laser_oz_{0.0f};
    std::atomic<float> hand_interaction_laser_ex_{0.0f}, hand_interaction_laser_ey_{0.0f}, hand_interaction_laser_ez_{0.0f};
    std::atomic<float> flat_ui_ipd_mm_{0.0f};
    std::atomic<float> flat_ui_scale_{1.58f};
    std::atomic<float> flat_ui_distance_{1.40f};
    std::atomic<std::uint32_t> last_source_width_{1920};
    std::atomic<std::uint32_t> last_source_height_{1800};
    std::atomic<bool> late_pose_enabled_{false};
    bool qpc_time_extension_enabled_ = false;
    PFN_xrConvertWin32PerformanceCounterToTimeKHR qpc_to_xr_time_ = nullptr;
    std::mutex pose_locate_mutex_;
    core::SnapshotMailbox<tracking59c6::Stamp> input_stamp59c6_;
    std::atomic<std::uint64_t> late_pose_samples_{0};
    std::atomic<std::uint64_t> late_pose_failures_{0};
    std::atomic<float> estimated_refresh_hz_{0.0f};
    std::atomic<int> present_pacing_mode_{0}; // V65.03b F2-OFF = V65.02b adaptive35 trusted baseline; F2 active = immediate
    std::atomic<std::uint64_t> present_pacing_fresh_count_{0};
    std::atomic<std::uint64_t> present_pacing_reuse_count_{0};
    std::atomic<std::uint64_t> present_pacing_last_wait_ms_{0};
    std::atomic<std::uint64_t> present_pacing_last_serial_{0};
    std::vector<XrTime> pending_reference_changes125_; // render-thread event/locate timeline
    XrTime previous_predicted_display_time_ = 0;
    std::atomic<std::int64_t> latest_predicted_display_time_{0};
    // HJ1: XR time when the latest head (mode 0) / latest predicted time (mode 1) became visible to the camera.
    std::atomic<std::int64_t> hj1_head_target_{0},hj1_head_now_{0},hj1_predicted_now_{0},hj1_period_ns_{0};
    hand_present_d61::Lead hand_present_lead_d61_{}; // XR RunFrame owner only; optional visual estimate
    bool hand_present_was_enabled_d61_=false;
    std::atomic<int> present_pose_pairing_mode_{1}; // 0=legacy current XR pose, 1=PRESENT-paired render pose
    std::atomic<bool> stereo_stability_experiment_{false}; // V66.27 F2 A/B experiment
    std::atomic<int> projection_pose_mode_{0};
    std::atomic<bool> frozen_projection_valid_{false};
    XrPosef frozen_projection_eye_pose_[2]{};
    std::atomic<std::uint64_t> last_submitted_present_serial_{0}; // renderer/source token: may be a PAIR token
    std::atomic<std::uint64_t> last_submitted_desktop_serial59c6_{0}; // desktop counter ONLY
    std::atomic<std::uint64_t> last_submitted_camera_frame_{0};
    // V65.33 AFR: exact historical XR frame identity for the pixels cached for each eye.
    // This is separate from the shared mono token so ordinary submission behavior is untouched.
    std::atomic<std::uint64_t> last_submitted_camera_frame_by_eye_[2]{0,0};
    std::atomic<bool> afr_per_eye_historical_pose_{false};
    static constexpr std::size_t kRenderFrameHistoryCount = 512;
    RenderFrameState render_frame_history_[kRenderFrameHistoryCount]{};
    std::mutex flat_ui_mutex_;
    XrPosef flat_ui_anchor_pose_{};
    bool flat_ui_anchor_valid_ = false;
    ID3D12Resource* afr_cache_[2] = { nullptr, nullptr };
    bool afr_cache_valid_[2] = { false, false };
    D3D12_RESOURCE_DESC afr_cache_desc_{};
    std::uint64_t afr_last_present_serial_ = 0;
    // V66.22 production native-stereo presentation cache. Unlike the historical
    // AFR cache, both eyes are refreshed atomically from one certified same-frame
    // pair before either is exposed to XR. Once refreshed, this pair remains stable
    // while DeviceHook reuses/overwrites its own bridge capture textures.
    ID3D12Resource* bridge_present_cache_[2] = { nullptr, nullptr };
    D3D12_RESOURCE_DESC bridge_present_cache_desc_{};
    bool bridge_present_cache_valid_ = false;
    std::uint64_t bridge_present_cache_serial_ = 0;
    native_upscaler_stereo_d82::PairOwner bridge_present_cache_owner_d85_{};
    std::uint64_t bridge_present_cache_camera_frame_ = 0; // V66.27 exact F2 pose owner for cached L/R pixels
    std::uint64_t bridge_last_consumed_pair_serial_ = 0; // V65.47 render-thread only; consume each transaction pair once
    std::atomic<bool> theater_drain_active_{false};
    std::atomic<bool> nonblocking_pair_submit_{true}; // V66.34 production default ON // V66.33: set by Mod::Run, acted on by render thread only
    bool DrainBridgePairForTheater(); // V66.33: render thread only

    struct StereoSubmitSlot {
        ID3D12CommandAllocator* allocator = nullptr;
        ID3D12GraphicsCommandList* list = nullptr;
        ID3D12DescriptorHeap* srv_heap = nullptr;
        UINT64 fence_value = 0;
        // UI copy leases outlive VrMenu recreation/shutdown until this slot's fence.
        ID3D12Resource* ui_source = nullptr;
        ID3D12Resource* ui_target = nullptr;
    };
    static constexpr std::size_t kStereoSubmitRingCount = 16;
    StereoSubmitSlot stereo_submit_ring_[kStereoSubmitRingCount]{};
    std::size_t stereo_submit_ring_cursor_ = 0;
    bool stereo_submit_ring_ready_ = false;
    bool stereo_submit_ring_disabled_ = false;

    XrInput input_{};

    struct FramePresentation59c2 {
        ui_menu_transition59c2::FramePolicy policy{};
        std::uint64_t ticket=0;
        XrPosef anchor{};
        bool anchor_valid=false;
        float comfort_vignette51=0.0f; // immutable across the complete eye pair
    };
    ui_menu_transition59c2::Production production_presentation59c2_;
    FramePresentation59c2 CapturePresentation59c2();
    FramePresentation59c2 render_request_presentation59c2_{}; // render_mutex_
    FramePresentation59c2 render_presentation59c2_{}; // only the render callback

    // Producer/consumer bridge. OpenXR frame timing and swapchain ownership
    // remain on the XR worker thread; only D3D12 command recording/submission
    // is handed to the game's render-thread queue callback.
    std::mutex render_mutex_;
    std::condition_variable render_cv_;
    bool render_request_pending_ = false;
    bool render_request_in_progress_ = false;
    bool render_request_done_ = false;
    bool render_request_success_ = false;
    bool render_request_copy_ui_ = false;
    bool render_request_ui_copy_success_ = false;
    ID3D12Resource* render_request_images_[2] = { nullptr, nullptr };
    ID3D12Resource* render_request_ui_image_ = nullptr;
    std::uint64_t render_request_enqueue_ns59c6_ = 0; // original high-resolution handoff deadline anchor
    std::atomic<std::uint64_t> render_tick_count_{0};

    // V64.48 XR-driven render gate. Published after xrLocateViews has populated
    // active_render_frame_; consumed at PublishViewCamera without moving OpenXR
    // frame-loop ownership off the XR worker.
    std::atomic<int> xr_render_gate_mode_{0};
    std::atomic<std::uint64_t> frame_gate_published_frame_{0};
    std::atomic<std::uint64_t> expected_present_camera_frame_{0};
    std::mutex frame_gate_mutex_;
    std::condition_variable frame_gate_cv_;

    std::mutex stereo_view_mutex_;
    StereoViewSnapshot latest_stereo_views_{};
    StereoViewSnapshot last_good_stereo_views_{};
    RenderFrameState active_render_frame_{};

    std::atomic<std::uint64_t> test_xr_frames_{0};
    std::atomic<std::uint64_t> test_valid_frame_snapshots_{0};
    std::atomic<std::uint64_t> test_mono_blits_{0};
    std::atomic<std::uint64_t> test_afr_blits_{0};
    std::atomic<std::uint64_t> test_fov_blits_{0};
    std::atomic<std::uint64_t> test_marker_blits_{0};
    std::atomic<std::uint64_t> test_swapped_compositions_{0};
    // V65.15: real live depth dimensions for pseudo-stereo texel addressing.
    std::atomic<std::uint32_t> depth_tex_width_{0};
    std::atomic<std::uint32_t> depth_tex_height_{0};

    bool CreateInstance();
    bool GetSystem();
    bool CreateSession(ID3D12Device* game_device, ID3D12CommandQueue* game_queue);
    bool CreateD3D12Objects();
    bool CreateReferenceSpace();
    bool CreateSwapchains();
    bool RecreateSwapchainsOnXrThread(float new_scale);
    bool CreateRtvHeap();
    void DestroySwapchains();
    bool ClearD3D12Image(ID3D12Resource* image);
    bool CreateBlitPipeline();
    // V61.35: writes the color SRV always, and attempts a depth SRV into the
    // adjacent heap slot if depth_push_enabled_ and a live, confirmed depth
    // candidate exist. Falls back to duplicating the color SRV into the
    // depth slot on any failure (never leaves a descriptor table slot
    // uninitialized). Returns the depthPushWeight to write into present[15]
    // (presentPad) - 0.0f whenever depth isn't actually usable this frame.
    float WriteColorAndDepthDescriptors(ID3D12Resource* color_source, DXGI_FORMAT color_format,
        D3D12_CPU_DESCRIPTOR_HANDLE color_cpu, D3D12_CPU_DESCRIPTOR_HANDLE depth_cpu) noexcept;
    bool BlitGameImageToEyes(ID3D12Resource* source, ID3D12Resource* left, ID3D12Resource* right, D3D12_RESOURCE_STATES source_state = D3D12_RESOURCE_STATE_PRESENT);
    bool CaptureV6589PackedPresent(ID3D12Resource* source, std::uint64_t present_serial);
    bool CopyMenuTextureToUiImage(ID3D12Resource* source, ID3D12Resource* target);
    bool EnsureAfrCaches(const D3D12_RESOURCE_DESC& source_desc);
    bool EnsureBridgePresentCaches(const D3D12_RESOURCE_DESC& source_desc);
    void DestroyBridgePresentCaches();
    bool EnsureStereoSubmitRing();
    void DestroyStereoSubmitRing();
    StereoSubmitSlot* AcquireStereoSubmitSlot();
    bool StereoSubmitRingIdle() const;
    bool BlitBridgePairStableToEyes(ID3D12Resource* left_source, ID3D12Resource* right_source,
        std::uint64_t pair_serial, std::uint64_t pair_camera_frame, bool refresh_pair,
        ID3D12Resource* left, ID3D12Resource* right, bool* safe_to_drop = nullptr,
        native_upscaler_stereo_d82::PairOwner pair_owner = {});
    bool BlitAfrStereoToEyes(ID3D12Resource* source, int source_eye, std::uint64_t serial, ID3D12Resource* left, ID3D12Resource* right, D3D12_RESOURCE_STATES source_state = D3D12_RESOURCE_STATE_PRESENT);
    void DestroyAfrCaches();
    bool SubmitDiagnosticImagesOnRenderThread(ID3D12Resource* left, ID3D12Resource* right);
    bool SubmitBridgeImagesOnRenderThread(ID3D12Resource* left, ID3D12Resource* right);
    bool QueueRenderThreadDiagnosticWork(ID3D12Resource* left, ID3D12Resource* right, ID3D12Resource* ui_image,
                                         bool copy_ui, bool* ui_copy_success, const FramePresentation59c2& presentation);
    D3D12_CPU_DESCRIPTOR_HANDLE GetRtvHandle(ID3D12Resource* image);
};

} // namespace kcd2vr
