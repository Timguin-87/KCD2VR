#pragma once
#include <cstdint>
#include <cstddef>

namespace kcd2vr::native_stereo_outer_lab {

struct Status {
    int mode{};
    bool supported_build{};
    bool frame_nexus_hook_installed{};
    bool camera_handoff_hook_installed{};
    bool get_or_create_hook_installed{};
    bool phase_hooks_installed{};
    bool csystem_render_hook_installed{};

    std::uintptr_t frame_nexus_rva{};
    std::uintptr_t camera_handoff_target_rva{};
    std::uintptr_t phase7e8_target_rva{};
    std::uintptr_t phase7f8_target_rva{};
    std::uintptr_t csystem_render_rva{};
    std::uintptr_t csystem_render_last_caller_rva{};
    std::uintptr_t csystem_render_begin_target_rva{};
    std::uintptr_t csystem_render_end_target_rva{};

    std::uint64_t nexus_calls{};
    std::uint64_t nexus_present_advances{};
    std::uint64_t camera_handoff_calls{};
    std::uint64_t camera_handoff_in_nexus{};
    std::uint64_t get_or_create_calls{};
    std::uint64_t get_or_create_in_nexus{};
    std::uint64_t pipeline_calls{};
    std::uint64_t pipeline_in_nexus{};
    std::uint64_t normal_pipeline_in_nexus{};
    std::uint64_t handoff_matches{};
    std::uint64_t handoff_misses{};
    std::uint64_t last_handoff_nexus_serial{};
    std::uint64_t render_view_token_epochs{};
    std::uint64_t render_view_token_reuses{};
    std::uintptr_t last_render_view_token{};
    std::uint64_t phase7e8_calls{};
    std::uint64_t phase7f8_calls{};
    std::uint64_t csystem_render_calls{};
    std::uint64_t csystem_render_in_nexus{};
    std::uint64_t csystem_render_pipeline_matches{};
    std::uint64_t csystem_render_present_advances{};

    std::uint64_t last_nexus_serial{};
    std::uint64_t last_camera_nexus_serial{};
    std::uint64_t last_pipeline_nexus_serial{};
    std::uint32_t last_flags{};
    std::uint32_t last_camera_out{};
    bool stereo_build_hook_installed{};
    bool camera_pair_hook_installed{};
    std::uint64_t native_loop_promotions{};
    std::uint64_t stereo_build_calls{};
    std::uint64_t stereo_build_writes{};
    std::uint64_t stereo_build_restores{};
    std::uint64_t stereo_build_rejects{};
    float last_applied_ipd{};
};

// V65.16 focused F8 ladder. Rungs 1/2 reproduce the old V65.10 OpenXR-IPD
// camera path without/with phase observers for the user's shadow-quality A/B.
// Rung 3 observes same-frame RTV resources; rung 4 adds the native two-eye camera
// loop/metadata; rung 5 detours exact CSystem::Render READ-ONLY and correlates its
// frame-nexus/caller with the existing render-view/pipeline handoff. Rung 6 keeps the
// zero-IPD outer duplicate-render experiment LOCKED until rung 5 proves the boundary.
// V66.02: one F11 starts read-only resource/camera preflight during normal rendering.
// No COUNT2/replay/selector mutation. Phase -1=armed, 1=collecting,
// 0=complete/idle, -2=stopped/refused/faulted. A stop does not interrupt native calls.
void StopSingleRun() noexcept;
void PollSingleRun() noexcept;
bool SingleRunBusy() noexcept;
int SingleRunPhase() noexcept;
// V66.12 gated single-session progression. These wrappers expose only the
// verified proof/design state; they do not re-enable historical COUNT2 ladders.
bool Eye1Verified() noexcept;
unsigned Eye1Outcome() noexcept;
bool StartStereoDesign(int stage, float openxr_ipd_meters) noexcept;
// V66.25 production selected-eye sharing policy. The recovered native RIGHT pass
// must not advance shared shadow maps/particles a second time in the same game frame.
// Begin/End save and restore the exact live CVars; no permanent quality override.
int LastRunKind() noexcept;
bool LastRunPassed() noexcept;
int LastDesignStagePassed() noexcept;
std::uint64_t LastDesignPairSerial() noexcept;

bool EnsurePhysicalFramePump6698() noexcept;
// Read-only identity of the currently executing outer gameplay frame on this thread.
std::uint64_t CurrentNativeFrameHJ1() noexcept;
bool Initialize() noexcept;
void Shutdown() noexcept;
void SetMode(int mode, float openxr_ipd_meters) noexcept;
// V65.31 research-only observer ladder. Installs only READ-ONLY hooks; never
// enables the old camera-bank/IPD/native-pair write paths.
// V66.36 F11 cloud-ownership diagnostic. Broad mode0->mode2 forcing is retired.
// 0 baseline, 1 observe, 2 copy LEFT cloud-eye state to RIGHT,
// 3 copy LEFT cloud-shadow state to RIGHT, 4 copy both state blocks.
// V66.26 production read-only census: keeps the exact CSystem RenderBegin and
// secondary-view/mode hooks armed while native stage4 runs. It never replays or
// mutates the frame transaction; it only reports whether LEFT/RIGHT traverse the
// surviving native stereo preparation/gating machinery.
bool SetV6626NativeStageCensus(bool enabled) noexcept;
// V66.39R1 F9 pair-wide proof ladder.
// 0 census only; 1 proof-gated center/main ShadowMap preparation;
// 2 exact SSR source census; 3 center ShadowMap prep + exact SSR source census.
// "ACTIVE" is logged only after exact stage/CRV/frame/selector checks, byte-for-byte
// substitution readback, native return, and byte-for-byte restoration.
bool V6639R1CenterShadowPrepRequested() noexcept;
// V66.44 bisect: skip CShadowMaskStage::Execute for the RIGHT pass only.
std::uint64_t V6644RightShadowMaskSkips() noexcept;
bool PublishV6639R1CenterShadowRecord(void* crv,std::uint32_t frame,const void* record,std::size_t bytes,std::uint64_t source_camera_hash) noexcept;
// V66.38 F9: retired/compatibility-only Crysis-style per-view state reset immediately before the manual RIGHT
// pipeline. Uses the already-captured same-frame CSystem::RenderBegin arguments and
// never performs another top-level CSystem::Render.
bool V6638InterEyeRenderBeginReset() noexcept;
bool RunV6638InterEyeRenderBeginReset() noexcept;
// V66.27 F2 rung2: replay only the exact retail +0x7C2D40 pre-pipeline
// transaction for the current RIGHT CRenderView, using the owner observed on the
// normal frame and requiring same CRV + same render thread. The caller then
// reasserts selector=1 and continues through the already-proven +0x864728 path.
bool SetV6627RightPreparationExperiment(bool enabled) noexcept;
bool V6627RightPreparationEnabled() noexcept;
// V66.28: cumulative per-eye counters (read by mod.cpp every few seconds, logged as deltas).
// mode* = native shader-mode resolutions (particles etc.), gate* = secondary-view gate calls.
struct V6628EyeWork { std::uint64_t modeLeft=0, modeRight=0, gateLeft=0, gateRight=0, prepCalls=0, prepSkips=0; };
void GetV6628EyeWork(V6628EyeWork& out) noexcept;
// V65.56 one-shot native multiview bridge. F8 stage 1 keeps exactly one normal
// CSystem::Render and one normal CStandardGraphicsPipeline::Execute, but asks the
// retail CRenderView camera-build loop to publish two native view records (LEFT/RIGHT)
// by raising the built-view count to 2 after the first native pair-builder iteration.
// No top-level render duplication, pipeline re-entry, RenderBegin injection, or XR pair
// submission occurs in this test.
void SetBridgeStage(int stage, float openxr_ipd_meters) noexcept;
int BridgeStage() noexcept;
const char* BridgeStageName(int stage) noexcept;

// V65.95B F11 diagnostic: 0=off, 1=observe per-CRenderView transaction,
// 2=observer plus exact COUNT2 block-1 substitution at +0x42DA44,
// 3=experimental internal stereo replay: normal COUNT2 pass stays block0, then
// SetCurrentRenderView -> pipeline -> finalize is replayed with block1. This tests
// two eye scene transactions inside one outer render; output separation is not yet claimed.

// V65.81 F5 PSO provenance bridge. The mode-lineage observer fingerprints the
// compiled DXBC blobs owned by MODE0 and MODE2. DeviceHook hashes all graphics-PSO shader stages
// (VS/PS/GS/HS/DS) and asks this classifier whether a PSO belongs to the proven primary
// or secondary-view shader family. 0=unknown, 1=MODE0, 2=MODE2.
int ClassifyModeShaderHash(std::uint64_t hash) noexcept;

// V65.51 F9 retains the certified XR-IPD same-frame dual-render ladder.

// V65.74: exact built-view +0x3B0 extent census + extent-gated COUNT2 same-target SBS split lab.
// 0=off, 1=read-only viewport signature census, 2=guarded split write.
int Count2SbsStage() noexcept;
void Cycle(float openxr_ipd_meters) noexcept;
int Mode() noexcept;
const char* ModeName(int mode) noexcept;
Status GetStatus() noexcept;

// Called by the already-existing CStandardGraphicsPipeline observer so F8 can
// correlate a renderer execution with the enclosing CCryAction frame nexus
// without installing a second hook on the same pipeline function.
void NotifyPipelineExecute(void* pipeline, void* render_view,
                           std::uint32_t view_type,
                           std::uint32_t selector) noexcept;
void NotifyPipelineExecuteReturn(void* pipeline, void* render_view,
                                 std::uint32_t view_type,
                                 std::uint32_t selector) noexcept;

// Preserved compatibility API. V65.04a never performs a second eye render.

} // namespace kcd2vr::native_stereo_outer_lab
