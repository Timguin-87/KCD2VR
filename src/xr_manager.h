// xr_manager.h
#pragma once

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
#include "camera_hook.h"
#include "xr_input.h"

namespace kcd2vr {

class XrManager
{
public:
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

    bool EnsureInitialized(ID3D12Device* game_device, ID3D12CommandQueue* game_queue);
    void Shutdown();
    bool RunFrame(XrPose& out_head_pose, float head_pose_scale = 1.0f);
    // V48: sample the HMD at current QPC time, close to KCD2 camera consumption.
    bool SampleLateHeadPose(XrPose& out_pose);
    // V53: synchronous game-thread pose lookup at an explicit OpenXR time.
    // xrWaitFrame remains owned by the XR thread; xrLocateSpace is serialized by pose_locate_mutex_.
    bool SampleHeadPoseAtTime(XrTime target_time, XrPose& out_pose);
    bool SampleHeadPoseNow(XrPose& out_pose, XrTime* out_time = nullptr);
    XrTime GetLatestPredictedDisplayTime() const { return static_cast<XrTime>(latest_predicted_display_time_.load(std::memory_order_acquire)); }
    void SetLatePoseEnabled(bool enabled) { late_pose_enabled_.store(enabled, std::memory_order_release); }
    bool GetLatePoseEnabled() const { return late_pose_enabled_.load(std::memory_order_acquire); }
    float GetEstimatedRefreshHz() const { return estimated_refresh_hz_.load(std::memory_order_acquire); }

    // Invoked only from DeviceHook's ExecuteCommandLists detour, i.e. on the
    // thread currently submitting the game's DIRECT-queue work.
    void OnRenderThreadTick(ID3D12CommandQueue* queue);
    void SetDiagnosticRendering(bool enabled) { diagnostic_rendering_.store(enabled, std::memory_order_release); }
    // F8 ladder additions - see xr_manager.h member comments for each.
    void SetAngularCorrectMono(bool enabled) { angular_correct_mono_.store(enabled, std::memory_order_release); }
    void SetDepthPushEnabled(bool enabled) { depth_push_enabled_.store(enabled, std::memory_order_release); }
    void SetDepthPushWeight(float w) { depth_push_weight_signed_.store(std::clamp(w, 0.0f, 1.0f), std::memory_order_release); }
    float GetDepthPushWeight() const { return depth_push_weight_signed_.load(std::memory_order_acquire); }
    bool GetDepthPushEnabled() const { return depth_push_enabled_.load(std::memory_order_acquire); }
    bool GetAngularCorrectMono() const { return angular_correct_mono_.load(std::memory_order_acquire); }
    void SetPoseHonestyOverride(int mode) { pose_honesty_override_.store(mode, std::memory_order_release); }
    // V59.0: scales the horizontal OpenXR eye-FOV asymmetry around a centered
    // cyclopean frustum while both eyes share one submitted camera origin.
    // +1 = runtime/native asymmetry (V58.9 phase D), 0 = centered mono,
    // -1 = same magnitude with L/R asymmetry reversed. This is a presentation
    // diagnostic only; it does not claim two independently rendered scene eyes.
    void SetMonoFovAsymmetryScale(float scale) { mono_fov_asymmetry_scale_.store(scale, std::memory_order_release); }
    float GetMonoFovAsymmetryScale() const { return mono_fov_asymmetry_scale_.load(std::memory_order_acquire); }
    // V59.1 mono UV reprojection diagnostics. mode 0=off, 1=rotation-only
    // current-output ray transformed back into the render-paired camera space. mode 2=inverse-sign diagnostic.
    void SetMonoUvReprojectionMode(int mode) { mono_uv_reprojection_mode_.store(mode, std::memory_order_release); }
    int GetMonoUvReprojectionMode() const { return mono_uv_reprojection_mode_.load(std::memory_order_acquire); }
    void SetMonoUvBinocularOffset(float offset) { mono_uv_binocular_offset_.store(offset, std::memory_order_release); }
    float GetMonoUvBinocularOffset() const { return mono_uv_binocular_offset_.load(std::memory_order_acquire); }
    // V61.27: common source-UV Y offset for projection-centering diagnosis. This is compositor/presentation
    // mapping only: CView, culling and OpenXR projection-view poses/FOV remain untouched.
    void SetPresentationCenterY(float offset) { presentation_center_y_.store(std::clamp(offset, -0.20f, 0.20f), std::memory_order_release); }
    float GetPresentationCenterY() const { return presentation_center_y_.load(std::memory_order_acquire); }
    // V59.2: bottom-edge-only treatment for the good game-FOV-110 mono mapping.
    // 0=black reference, 1=vertical shift, 2=Y-only mild compression,
    // 3/4=lower-tail remap preserving upper 80/90%, 5=bottom edge hold.
    void SetMonoBottomEdgeMode(int mode) { mono_bottom_edge_mode_.store(mode, std::memory_order_release); }
    int GetMonoBottomEdgeMode() const { return mono_bottom_edge_mode_.load(std::memory_order_acquire); }
    void ResetClampStats() { clamp_active_count_.store(0, std::memory_order_relaxed); clamp_total_count_.store(0, std::memory_order_relaxed); }
    void GetClampStats(std::uint64_t& active, std::uint64_t& total) const {
        active = clamp_active_count_.load(std::memory_order_relaxed);
        total = clamp_total_count_.load(std::memory_order_relaxed);
    }
    void SetActiveVrBridge(bool enabled) { active_vr_bridge_.store(enabled, std::memory_order_release); }
    void SetStereoExperiment(int mode, float ipd_scale) { stereo_experiment_.store(mode, std::memory_order_release); afr_ipd_scale_.store(ipd_scale, std::memory_order_release); }
    void SetSwapEyes(bool enabled) { swap_eyes_.store(enabled, std::memory_order_release); }
    // V58.4/V58.7 mono angular mapping is controlled by the F8-safe setters above.
    bool GetSwapEyes() const { return swap_eyes_.load(std::memory_order_acquire); }
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
    bool ConsumeRecenterRequest();
    const ControllerState& LeftController() const { return input_.Left(); }
    const ControllerState& RightController() const { return input_.Right(); }
    bool GetMenuPointer(int& x, int& y) const { x=menu_pointer_x_.load(std::memory_order_acquire); y=menu_pointer_y_.load(std::memory_order_acquire); return menu_pointer_active_.load(std::memory_order_acquire); }
    // V62.9: normalized hit point on the F6 LOCAL theater quad. Used only for
    // UI pointer injection; never changes the game camera or interaction ray.
    bool GetFlatUiPointer(float& u, float& v) const {
        u=flat_pointer_u_.load(std::memory_order_acquire); v=flat_pointer_v_.load(std::memory_order_acquire);
        return flat_pointer_active_.load(std::memory_order_acquire);
    }
    bool IsRunning() const { return session_running_.load(std::memory_order_relaxed); }
    bool CopyLatestStereoViews(StereoViewSnapshot& out);
    bool CopyLastGoodStereoViews(StereoViewSnapshot& out);
    bool CopyActiveRenderFrameState(RenderFrameState& out);
    bool CopyRenderFrameStateByIndex(std::uint64_t frame_index, RenderFrameState& out);
    void SetPresentPosePairingMode(int mode) { present_pose_pairing_mode_.store(mode, std::memory_order_release); }
    int GetPresentPosePairingMode() const { return present_pose_pairing_mode_.load(std::memory_order_acquire); }
    // V52: 0=dynamic render pose (current behavior), 1=frozen render pose captured on next XR frame.
    void SetProjectionPoseMode(int mode) { projection_pose_mode_.store(mode, std::memory_order_release); frozen_projection_valid_.store(false,std::memory_order_release); }
    int GetProjectionPoseMode() const { return projection_pose_mode_.load(std::memory_order_acquire); }
    TestCounters GetTestCounters() const;
    void GetEyeSize(int& out_width, int& out_height) const { out_width = eyes_[0].width; out_height = eyes_[0].height; }
    void SetPresentationTestMarker(bool enabled) { presentation_test_marker_.store(enabled, std::memory_order_release); }
    // V46.9 manual Cyberpunk-style flat-UI prototype. Captures an anchor head orientation
    // when enabled and counter-shifts the virtual screen as the head turns.
    void SetFlatUiMode(bool enabled);
    void SetFlatUiScale(float scale) { flat_ui_scale_.store(scale, std::memory_order_release); }
    float GetFlatUiScale() const { return flat_ui_scale_.load(std::memory_order_acquire); }
    void SetFlatUiDistance(float meters) { flat_ui_distance_.store(std::clamp(meters,0.80f,3.20f), std::memory_order_release); }
    float GetFlatUiDistance() const { return flat_ui_distance_.load(std::memory_order_acquire); }
    void SetFlatUiIpdMm(float mm) { flat_ui_ipd_mm_.store(mm, std::memory_order_release); }
    float GetFlatUiIpdMm() const { return flat_ui_ipd_mm_.load(std::memory_order_acquire); }
    void SetMenuQuadSource(ID3D12Resource* texture, bool visible) { menu_quad_source_.store(texture, std::memory_order_release); menu_quad_visible_.store(visible, std::memory_order_release); }
    void SetSharpening(float strength) { sharpening_strength_.store(std::clamp(strength, 0.0f, 0.75f), std::memory_order_release); }
    void SetSourceVerticalFov(float vfov_deg) { source_vertical_fov_deg_.store(std::clamp(vfov_deg,70.0f,140.0f), std::memory_order_release); }
    float GetSharpening() const { return sharpening_strength_.load(std::memory_order_acquire); }
    // V62.6: compositor-side color correction. Never writes KCD2 renderer CVars.
    void SetColorAdjustment(float gamma, float brightness, float contrast) {
        output_gamma_.store(std::clamp(gamma,0.50f,2.00f), std::memory_order_release);
        output_brightness_.store(std::clamp(brightness,-0.50f,0.50f), std::memory_order_release);
        output_contrast_.store(std::clamp(contrast,0.50f,2.00f), std::memory_order_release);
    }
    void RequestSwapchainRecreate(float render_scale) { requested_render_scale_.store(std::clamp(render_scale, 0.50f, 2.0f), std::memory_order_release); }
    float GetActiveSwapchainScale() const { return active_swapchain_scale_.load(std::memory_order_acquire); }
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
    std::atomic<ID3D12Resource*> menu_quad_source_{nullptr};
    std::atomic<bool> menu_quad_visible_{false};
    std::atomic<int> menu_pointer_x_{325};
    std::atomic<int> menu_pointer_y_{138};
    std::atomic<bool> menu_pointer_active_{false};
    std::atomic<float> flat_pointer_u_{0.5f};
    std::atomic<float> flat_pointer_v_{0.5f};
    std::atomic<bool> flat_pointer_active_{false};
    std::atomic<bool> session_running_{false};
    bool init_attempted_ = false;
    std::atomic<bool> diagnostic_rendering_{false};
    std::atomic<bool> active_vr_bridge_{true};
    std::atomic<float> render_scale_{1.0f};
    std::atomic<float> requested_render_scale_{0.0f};
    std::atomic<float> active_swapchain_scale_{1.0f};
    std::atomic<float> sharpening_strength_{0.18f};
    std::atomic<float> output_gamma_{1.0f};
    std::atomic<float> output_brightness_{0.0f};
    std::atomic<float> output_contrast_{1.0f};
    bool cylinder_layer_supported_ = false;
    std::atomic<int> presentation_mode_{1};
    std::atomic<float> presentation_zoom_{1.0f};
    std::atomic<float> source_vertical_fov_deg_{95.0f};
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
    std::atomic<float> presentation_center_y_{0.0f};
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
    std::atomic<int> stereo_experiment_{1}; // 0=mono bridge, 1=AFR stereo
    std::atomic<float> afr_ipd_scale_{1.0f};
    std::atomic<bool> swap_eyes_{false};
    std::atomic<bool> presentation_test_marker_{false};
    std::atomic<bool> flat_ui_mode_{false};
    std::atomic<float> flat_ui_ipd_mm_{0.0f};
    std::atomic<float> flat_ui_scale_{1.58f};
    std::atomic<float> flat_ui_distance_{1.40f};
    std::atomic<std::uint32_t> last_source_width_{1920};
    std::atomic<std::uint32_t> last_source_height_{1800};
    std::atomic<bool> late_pose_enabled_{false};
    bool qpc_time_extension_enabled_ = false;
    PFN_xrConvertWin32PerformanceCounterToTimeKHR qpc_to_xr_time_ = nullptr;
    std::mutex pose_locate_mutex_;
    std::atomic<std::uint64_t> late_pose_samples_{0};
    std::atomic<std::uint64_t> late_pose_failures_{0};
    std::atomic<float> estimated_refresh_hz_{0.0f};
    XrTime previous_predicted_display_time_ = 0;
    std::atomic<std::int64_t> latest_predicted_display_time_{0};
    std::atomic<int> present_pose_pairing_mode_{1}; // 0=legacy current XR pose, 1=PRESENT-paired render pose
    std::atomic<int> projection_pose_mode_{0};
    std::atomic<bool> frozen_projection_valid_{false};
    XrPosef frozen_projection_eye_pose_[2]{};
    std::atomic<std::uint64_t> last_submitted_present_serial_{0};
    std::atomic<std::uint64_t> last_submitted_camera_frame_{0};
    static constexpr std::size_t kRenderFrameHistoryCount = 512;
    RenderFrameState render_frame_history_[kRenderFrameHistoryCount]{};
    std::mutex flat_ui_mutex_;
    XrPosef flat_ui_anchor_pose_{};
    bool flat_ui_anchor_valid_ = false;
    ID3D12Resource* afr_cache_[2] = { nullptr, nullptr };
    bool afr_cache_valid_[2] = { false, false };
    D3D12_RESOURCE_DESC afr_cache_desc_{};
    std::uint64_t afr_last_present_serial_ = 0;
    XrInput input_{};

    // Producer/consumer bridge. OpenXR frame timing and swapchain ownership
    // remain on the XR worker thread; only D3D12 command recording/submission
    // is handed to the game's render-thread queue callback.
    std::mutex render_mutex_;
    std::condition_variable render_cv_;
    bool render_request_pending_ = false;
    bool render_request_in_progress_ = false;
    bool render_request_done_ = false;
    bool render_request_success_ = false;
    ID3D12Resource* render_request_images_[2] = { nullptr, nullptr };
    ID3D12Resource* render_request_ui_image_ = nullptr;
    std::atomic<std::uint64_t> render_tick_count_{0};

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
    bool BlitGameImageToEyes(ID3D12Resource* source, ID3D12Resource* left, ID3D12Resource* right);
    bool CopyMenuTextureToUiImage(ID3D12Resource* source, ID3D12Resource* target);
    bool EnsureAfrCaches(const D3D12_RESOURCE_DESC& source_desc);
    bool BlitAfrStereoToEyes(ID3D12Resource* source, int source_eye, std::uint64_t serial, ID3D12Resource* left, ID3D12Resource* right);
    void DestroyAfrCaches();
    bool SubmitDiagnosticImagesOnRenderThread(ID3D12Resource* left, ID3D12Resource* right);
    bool SubmitBridgeImagesOnRenderThread(ID3D12Resource* left, ID3D12Resource* right);
    bool QueueRenderThreadDiagnosticWork(ID3D12Resource* left, ID3D12Resource* right, ID3D12Resource* ui_image = nullptr);
    D3D12_CPU_DESCRIPTOR_HANDLE GetRtvHandle(ID3D12Resource* image);
};

} // namespace kcd2vr
