#pragma once

#include <DetourModKit/hook.hpp>
#include <atomic>
#include <cstdint>
#include <optional>
#include "vrik/hand_same_frame_hj1.h"

namespace kcd2vr {

// R24 private metadata only. This receipt is created by an observed setter
// transaction, never by camera pointer/matrix equality alone.
struct CameraPublicationR24 {
    hj1::Sample family{};
    std::uintptr_t system=0,copiedCamera=0,sourceCamera=0;
    std::uint64_t publication=0,nativeFrame=0;
    float matrix[12]{};
};
struct SystemRenderFamilyR24 {
    CameraPublicationR24 camera{};
    std::uintptr_t system=0;
    std::uint64_t serial=0,nativeFrame=0;
    bool active=false,poisoned=false;
};
struct SystemRenderMarkR24 {SystemRenderFamilyR24 previous{};std::uint64_t entered=0;};

struct CameraWriteMarkR24 {std::uint64_t entered=0,previous=0;bool counted=false;};

struct HeadOwnershipSummary {
    int level{0};
    std::uint64_t published_samples{0};
    std::uint64_t renderer_samples{0};
    std::uint64_t renderer_matches{0};
    std::uint64_t renderer_misses{0};
    std::uint64_t last_publish_serial{0};
    std::uint64_t last_publish_pose_frame{0};
    std::uint64_t last_renderer_match_serial{0};
    std::uint64_t last_renderer_match_pose_frame{0};
    float last_renderer_rotation_error_deg{-1.0f};
    float last_renderer_translation_error_m{-1.0f};
};

// V46: retained renderer correlation helper. The old V41 assumption that
// ViewCamera publication is a unique game-frame epoch was disproven at runtime;
// automatic V46 tests keep stereo mode 0 and do NOT use that publication as eye
// identity. The useful live role here is CStandardGraphicsPipeline correlation
// and per-EViewType counters (especially type 1 Recursive).
class FrameSync {
public:
    // F2/2 only, called before the outer verified native gameplay update.
    static void PaceGameplayBeforeUpdateD45() noexcept;
    static bool Install();
    static void Uninstall();
    static bool SetPipelineProbeEnabled(bool enabled);
    static bool PipelineProbeEnabled();
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
    // V65.03b F2: render-ownership diagnostics are read-only. The F2 handler itself
    // A/B-switches only V65.02b fixed8ms/adaptive35 vs raw/immediate baseline.
    static void SetHeadOwnershipProbeLevel(int level);
    static int HeadOwnershipProbeLevel();
    // V65.04: optional write-side pose/pixels ownership fix.  The renderer matcher
    // remains the same rung-3 observation, but PRESENT may now consume its exact
    // matched pose-frame token instead of the later global LastPoseFrameIndex().
    static void SetHeadOwnershipFixEnabled(bool enabled);
    static bool HeadOwnershipFixEnabled();
    static std::uint64_t LatestRendererMatchedPoseFrame(std::uint32_t max_age_ms = 50);
    // V65.55a: the normal PublishViewCamera owner is already detoured by FrameSync.
    // The stereo system-camera bridge needs to distinguish that known self-hook from
    // an unknown patch and must be able to invoke the saved retail setter directly
    // so an already HMD-composed camera snapshot is not composed a second time.
    static bool PublishViewCameraHookInstalled();
    static bool SetViewCameraRetail(void* system, void* camera,CameraPublicationR24* copied_family=nullptr) noexcept;
    // A caller owns the live CSystem/camera during this read. A successful
    // receipt still needs the exact primary/pass/native-copy proof downstream.
    static bool ReadSystemCameraFamilyR24(void* system,const void* actual_camera,CameraPublicationR24& out) noexcept;
    static SystemRenderMarkR24 BeginSystemRenderFamilyR24(void* system,std::uint64_t native_frame) noexcept;
    static void EndSystemRenderFamilyR24(const SystemRenderMarkR24& mark) noexcept;
    // Private native-writer adapter: begin before ANY native camera mutation,
    // end exactly once in native finally. Never restores a previous receipt.

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
    static std::atomic<bool> s_pipeline_probe_enabled;
    static thread_local int s_pipeline_view_type;
};

} // namespace kcd2vr
