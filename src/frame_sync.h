#pragma once

#include <DetourModKit/hook.hpp>
#include <atomic>
#include <cstdint>
#include <optional>

namespace kcd2vr {

// V46: retained renderer correlation helper. The old V41 assumption that
// ViewCamera publication is a unique game-frame epoch was disproven at runtime;
// automatic V46 tests keep stereo mode 0 and do NOT use that publication as eye
// identity. The useful live role here is CStandardGraphicsPipeline correlation
// and per-EViewType counters (especially type 1 Recursive).
class FrameSync {
public:
    static bool Install();
    static void Uninstall();
    static void SetStereoMode(int mode);
    static std::uint64_t CurrentFrameSerial();
    static int CurrentEye();
    static int CurrentPipelineViewType(); // thread-local: -1 outside pipeline execute
    static std::uint64_t PipelineCount(int view_type);
    // V54 aggressive native-stereo diagnostics. 0=off, 1=dual execute +/-IPD/2, 2=dual execute + selector 0/1.
    static void SetNativeStereoReentryMode(int mode, float ipd_meters);
    static int NativeStereoReentryMode();
    static std::uint64_t NativeStereoReentryPasses();
    // V59.6: thread-local eye identity while inside an explicit native re-entry pass.
    // -1 outside a re-entry pass, 0 left, 1 right. Used only by D3D12 scene-color archaeology.
    static int CurrentNativeStereoPassEye();
    static float LatestPublishedFovRadians();
    static std::uint64_t PublishedFovSampleCount();
    static void SetDenseRotationProbe(bool enabled);

private:
    using PublishViewCameraFunc = void(__fastcall*)(void* system, void* camera);
    using GraphicsPipelineExecuteFunc = void(__fastcall*)(void* self);

    static void __fastcall PublishViewCameraDetour(void* system, void* camera) noexcept;
    static void __fastcall GraphicsPipelineExecuteDetour(void* self) noexcept;

    static PublishViewCameraFunc s_publish_original;
    static GraphicsPipelineExecuteFunc s_pipeline_original;
    static std::optional<DetourModKit::hook::Hook> s_publish_hook;
    static std::optional<DetourModKit::hook::Hook> s_pipeline_hook;
    static std::atomic<std::uint64_t> s_frame_serial;
    static std::atomic<int> s_eye;
    static std::atomic<int> s_mode;
    static std::atomic<std::uint64_t> s_pipeline_counts[4];
    static std::atomic<int> s_native_stereo_reentry_mode;
    static std::atomic<float> s_native_stereo_ipd;
    static std::atomic<std::uint64_t> s_native_stereo_reentry_passes;
    static thread_local bool s_native_stereo_reentry_active;
    static thread_local int s_native_stereo_pass_eye;
    static std::atomic<float> s_latest_published_fov_rad;
    static std::atomic<std::uint64_t> s_published_fov_samples;
    static std::atomic<bool> s_dense_rotation_probe;
    static thread_local int s_pipeline_view_type;
};

} // namespace kcd2vr
