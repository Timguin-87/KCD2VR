#include "native_upscaler_stereo_d82.h"
#include "temporal_compose_publication_d82.h"
#include "temporal_pair_watchdog_d85.h"
#include "temporal_capture_transaction_d85.h"
#include "vrik/visible_failure.h"
#include "diagnostics/log_sink_d48.h"
#include "diagnostics/capture168.h"
// device_hook.cpp

#include "hud_layer_v6664.h"
#include "device_hook.h"
#include "camera_hook.h"
#include "frame_sync.h"
#include "debug_trace.h"
#include "diagnostic_trace.h"
#include "native_stereo_outer_lab.h"

#include <DetourModKit/logger.hpp>
#include <windows.h>
#include <cstdio>
#include <algorithm>
#include <unordered_set>
#include <utility>
#include <array>
#include <deque>
#include <cmath>
#include <condition_variable>
#include <chrono>
#include <vector>

namespace kcd2vr {

using DetourModKit::log;
namespace hook = DetourModKit::hook;

#include "native_stereo_gpu_v6600.inc"

DeviceHook::D3D12CreateDeviceFunc DeviceHook::s_original_create_device = nullptr;
DeviceHook::CreateCommandQueueFunc DeviceHook::s_original_create_queue = nullptr;
DeviceHook::ExecuteCommandListsFunc DeviceHook::s_original_execute_command_lists = nullptr;
DeviceHook::CreateGraphicsPipelineStateFunc DeviceHook::s_original_create_graphics_pipeline_state = nullptr;
DeviceHook::CreateCommandListFunc DeviceHook::s_original_create_command_list = nullptr;
DeviceHook::SetPipelineStateFunc DeviceHook::s_original_set_pipeline_state = nullptr;
DeviceHook::ResourceBarrierFunc DeviceHook::s_original_resource_barrier = nullptr;
DeviceHook::CreateRenderTargetViewFunc DeviceHook::s_original_create_rtv = nullptr;
DeviceHook::CreateDepthStencilViewFunc DeviceHook::s_original_create_dsv = nullptr;
DeviceHook::CreateDescriptorHeapFunc DeviceHook::s_original_create_descriptor_heap = nullptr;
DeviceHook::CreateShaderResourceViewFunc DeviceHook::s_original_create_srv = nullptr;
DeviceHook::CopyDescriptorsFunc DeviceHook::s_original_copy_descriptors = nullptr;
DeviceHook::CopyDescriptorsSimpleFunc DeviceHook::s_original_copy_descriptors_simple = nullptr;
DeviceHook::SetGraphicsRootDescriptorTableFunc DeviceHook::s_original_set_graphics_root_descriptor_table = nullptr;
std::atomic<DeviceHook::ClearCommandListStateFunc> DeviceHook::s_original_clear_command_list_state_d82{nullptr};
std::atomic<DeviceHook::ResetCommandListFunc> DeviceHook::s_original_reset_command_list_d82{nullptr};
DeviceHook::DrawInstancedFunc DeviceHook::s_original_draw_instanced = nullptr;
DeviceHook::DrawIndexedInstancedFunc DeviceHook::s_original_draw_indexed_instanced = nullptr;
DeviceHook::OMSetRenderTargetsFunc DeviceHook::s_original_om_set_render_targets = nullptr;
DeviceHook::RSSetViewportsFunc DeviceHook::s_original_rs_set_viewports = nullptr;
DeviceHook::RSSetScissorRectsFunc DeviceHook::s_original_rs_set_scissor_rects = nullptr;
std::atomic<ID3D12Device*> DeviceHook::s_captured_device{nullptr};
std::atomic<ID3D12CommandQueue*> DeviceHook::s_captured_queue{nullptr};
std::atomic<bool> DeviceHook::s_captured{false};
std::atomic<bool> DeviceHook::s_queue_hook_installed{false};
std::atomic<bool> DeviceHook::s_execute_hook_installed{false};
std::atomic<bool> DeviceHook::s_create_graphics_pso_hook_installed{false};
std::atomic<bool> DeviceHook::s_command_list_hook_installed{false};
std::atomic<bool> DeviceHook::s_set_pipeline_state_hook_installed{false};
std::atomic<bool> DeviceHook::s_resource_barrier_hook_installed{false};
std::atomic<bool> DeviceHook::s_create_rtv_hook_installed{false};
std::atomic<bool> DeviceHook::s_create_dsv_hook_installed{false};
std::atomic<bool> DeviceHook::s_create_descriptor_heap_hook_installed{false};
std::atomic<bool> DeviceHook::s_create_srv_hook_installed{false};
std::atomic<bool> DeviceHook::s_copy_descriptors_hook_installed{false};
std::atomic<bool> DeviceHook::s_graphics_root_descriptor_table_hook_installed{false};
std::atomic<bool> DeviceHook::s_draw_hooks_installed{false};
std::atomic<bool> DeviceHook::s_om_set_rt_hook_installed{false};
std::atomic<bool> DeviceHook::s_output_layout_hooks_installed{false};
std::atomic<bool> DeviceHook::s_output_layout_probe_enabled{false};
std::atomic<std::uint64_t> DeviceHook::s_output_layout_event_count{0};
ID3D12Resource* DeviceHook::s_latest_present_resource = nullptr;
int DeviceHook::s_latest_present_eye = 0;
std::uint64_t DeviceHook::s_latest_present_serial = 0;
std::uint64_t DeviceHook::s_latest_present_tick_ms = 0;
std::uint64_t DeviceHook::s_latest_present_camera_frame = 0;
ID3D12GraphicsCommandList* DeviceHook::s_latest_present_command_list = nullptr;
thread_local bool DeviceHook::s_current_execute_submitted_present = false;
std::atomic<bool> DeviceHook::s_afr_stereo_capture{false};
std::mutex DeviceHook::s_present_resource_mutex;
std::atomic<ID3D12GraphicsCommandList*> DeviceHook::s_internal_vr_command_list{nullptr};
std::atomic<ID3D12GraphicsCommandList*> DeviceHook::s_internal_menu_command_list{nullptr};
std::atomic<ID3D12GraphicsCommandList*> DeviceHook::s_internal_hud_command_list{nullptr};
std::atomic<DeviceHook::RenderTickCallback> DeviceHook::s_render_tick_callback{nullptr};
std::atomic<unsigned> DeviceHook::s_active_render_callbacks{0};
std::atomic<std::uint64_t> DeviceHook::s_present_capture_count{0};
std::atomic<std::uint64_t> DeviceHook::s_recursive_barrier_count{0};
std::atomic<std::uint64_t> DeviceHook::s_recursive_rt_barrier_count{0};
std::atomic<bool> DeviceHook::s_customres_probe_enabled{false};
std::atomic<std::uint32_t> DeviceHook::s_customres_probe_width{0};
std::atomic<std::uint32_t> DeviceHook::s_customres_probe_height{0};
std::atomic<std::uint64_t> DeviceHook::s_customres_barrier_count{0};
std::atomic<std::uint64_t> DeviceHook::s_customres_rt_barrier_count{0};
std::atomic<bool> DeviceHook::s_scene_color_probe_enabled{false};
std::atomic<std::uint32_t> DeviceHook::s_scene_color_probe_width{0};
std::atomic<std::uint32_t> DeviceHook::s_scene_color_probe_height{0};
std::atomic<std::uint64_t> DeviceHook::s_scene_color_probe_barrier_count{0};
std::atomic<std::uint64_t> DeviceHook::s_scene_color_probe_left_count{0};
std::atomic<std::uint64_t> DeviceHook::s_scene_color_probe_right_count{0};
std::atomic<bool> DeviceHook::s_stereo_producer_probe_enabled{false};
std::atomic<std::uint32_t> DeviceHook::s_stereo_producer_probe_width{0};
std::atomic<std::uint32_t> DeviceHook::s_stereo_producer_probe_height{0};
std::atomic<std::uint64_t> DeviceHook::s_stereo_producer_bind_count{0};
std::atomic<std::uint64_t> DeviceHook::s_stereo_producer_unique_count{0};
std::atomic<std::uint64_t> DeviceHook::s_stereo_producer_pair_count{0};
std::atomic<std::uint64_t> DeviceHook::s_stereo_producer_epoch{0};
std::atomic<ID3D12Resource*> DeviceHook::s_stereo_producer_first_resource{nullptr};
std::atomic<std::uint64_t> DeviceHook::s_stereo_producer_pair_epoch{~0ull};
std::atomic<ID3D12Resource*> DeviceHook::s_stereo_producer_unique0{nullptr};
std::atomic<ID3D12Resource*> DeviceHook::s_stereo_producer_unique1{nullptr};
std::atomic<ID3D12Resource*> DeviceHook::s_stereo_producer_unique2{nullptr};
std::atomic<ID3D12Resource*> DeviceHook::s_stereo_producer_unique3{nullptr};
std::atomic<std::uint64_t> DeviceHook::s_stereo_producer_detailed_bind_count{0};

// V65.61 bounded visual-identity candidate snapshots.  The resources exposed to
// OpenXR are owned copies in COMMON state, never live game render targets.
static std::mutex g_v6561_candidate_mutex;
static ID3D12Resource* g_v6561_candidate_source[2] = {nullptr,nullptr};
static ID3D12Resource* g_v6561_candidate_snapshot[2] = {nullptr,nullptr};
static ID3D12GraphicsCommandList* g_v6561_candidate_list = nullptr;
static ID3D12Resource* g_v6561_prev_fmt28 = nullptr;
static ID3D12GraphicsCommandList* g_v6561_prev_fmt28_list = nullptr;
static D3D12_RESOURCE_DESC g_v6561_candidate_desc{};
static bool g_v6561_pair_selected = false;
static bool g_v6561_snapshot_done[2] = {false,false};
static std::uint64_t g_v6561_pair_serial = 0;

// V65.88 live full-stereo publication. Working V65.61 snapshots are per-frame and
// may be cleared when the next COUNT2 packet arms its route window; these refs hold
// the last pair whose command list has actually reached ExecuteCommandLists.
static std::mutex g_v6588_live_mutex;
// R111: how often the "latest present resource" changed format (any list that moves a texture to COMMON/PRESENT
// counts, SteamVR's own lists included). Read by the R111 XR_UI_LAYERS row; no behaviour depends on it.
static std::atomic<std::uint64_t> g_present_fmt_changes_r111{0};
static std::atomic<std::uint32_t> g_present_fmt_last_r111{0},g_present_fmt_prev_r111{0};
static std::atomic<bool> g_v6588_live_enabled{false};
static ID3D12Resource* g_v6588_live_pair[2] = {nullptr,nullptr};
static std::uint64_t g_v6588_live_serial = 0;
static std::atomic<std::uint64_t> g_v6588_source_pair_serial{0};


// V65.85 COUNT2 output-surface sweep. These are owned snapshots, never live
// game render targets. Candidate sources are selected from the first four
// distinct full-size RTVs observed while the sweep is armed, then refreshed at
// each exact RENDER_TARGET -> next-state barrier.
static std::mutex g_v6585_mutex;
static std::atomic<bool> g_v6585_enabled{false};
static ID3D12Resource* g_v6585_source[4]={nullptr,nullptr,nullptr,nullptr};
static ID3D12Resource* g_v6585_snapshot[4]={nullptr,nullptr,nullptr,nullptr};
static ID3D12GraphicsCommandList* g_v6585_list[4]={nullptr,nullptr,nullptr,nullptr};
static std::uint64_t g_v6585_serial[4]={0,0,0,0};
static bool g_v6585_snapshot_done[4]={false,false,false,false};
static std::atomic<int> g_v6585_selected{-1};
static int g_v6585_count=0;
static std::atomic<std::uint64_t> g_v6585_event{0};

// V66.12 F9 read-only internal output-owner census. This deliberately stores
// pointer identities + copied descriptions only; it never AddRefs or writes game
// resources. The pass label is supplied by the proven selected-eye controller.
struct V6610OutputCandidate {
    ID3D12Resource* resource=nullptr;
    D3D12_RESOURCE_DESC desc{};
    std::uint32_t eye_mask=0;
    std::uint32_t binds[2]{0,0};
    std::uint32_t barriers[2]{0,0};
    // V66.24 read-only post-process history census. A temporal ping-pong surface
    // normally alternates between RT write and shader-resource read. Tracking
    // these transitions by the proven eye phase lets us distinguish shared
    // history from per-eye history without intercepting descriptor tables.
    std::uint32_t to_srv[2]{0,0};
    std::uint32_t from_srv[2]{0,0};
    std::uint32_t to_rt[2]{0,0};
    std::uint32_t last_frame[2]{0,0};
    bool same_main=false;
    // V66.47 read-only history-leak census (per eye pass): first access class in the
    // current pass (0 none, 1 READ, 2 WRITE), counters, and which eye wrote last.
    std::uint8_t pass_first[2]{0,0};
    std::uint32_t passes_seen[2]{0,0};
    std::uint32_t read_first[2]{0,0};
    std::uint32_t write_first[2]{0,0};
    std::uint32_t reads_other_eye[2]{0,0}; // first access READ while last writer was the other eye
    int last_writer=-1;
    bool via_uav=false;
    bool census_only=false; // V66.47: registered by the census only; never fed to live L/R tagging
    std::uint32_t dsv_binds[2]{0,0}; // V66.51: depth-stencil bindings per eye (counted as writes)
};
static std::mutex g_v6610_output_mutex;
static std::atomic<bool> g_v6610_output_enabled{false};
static std::atomic<int> g_v6610_output_eye{-1};
static std::atomic<std::uint32_t> g_v6610_output_frame{0};
static std::atomic<std::uint64_t> g_v6610_output_event{0};
static std::array<V6610OutputCandidate,256> g_v6610_output_candidates{};
static std::unordered_set<ID3D12Resource*> g_v6647_rejected; // non-screen resources seen in barriers
static constexpr bool kV6647HistoryCensus=false; // read-only research, not production rendering
static std::uint64_t g_v6647_last_log_ms=0;
static std::uint64_t g_v6647_pass_serial=0;

// V66.47: record one access for census candidate c in eye pass `eye` (caller holds mutex).
static void V6647NoteAccess(V6610OutputCandidate& c,int eye,bool write) noexcept
{
    if(eye<0||eye>1) return;
    if(c.pass_first[eye]==0) {
        c.pass_first[eye]=write?2:1;
        ++c.passes_seen[eye];
        if(write) ++c.write_first[eye];
        else {
            ++c.read_first[eye];
            if(c.last_writer==(1-eye)) ++c.reads_other_eye[eye];
        }
    }
    if(write) c.last_writer=eye;
}
static int g_v6610_output_count=0;

// V66.24 late-composition window: opens immediately after the native RIGHT
// pipeline returns and closes at the next LEFT phase or PRESENT. Any draw to
// the semantic main output here is downstream of our current eye preservation
// boundary and is therefore a strong HUD/FlashUI/final-compose candidate.
static std::atomic<bool> g_v6624_late_window{false};
static std::atomic<std::uint32_t> g_v6624_late_frame{0};
static std::atomic<std::uint64_t> g_v6624_late_draw_logs{0};
static std::atomic<std::uint64_t> g_v6624_postfx_draw_logs{0};
static std::atomic<std::uint64_t> g_v6624_summary_serial{0};

// V66.12 semantic renderer-output owner, captured from the exact retail
// in-pipeline final-compose boundary (WHGame+0x8645F8). These are copied CPU
// identities/metadata only; no AddRef and no game/D3D state mutation.
struct V6610SemanticOutput {
    bool used=false;
    int eye=-1;
    std::uint32_t frame=0,hits=0;
    std::uintptr_t render_output=0,texture_object=0,device_backing=0,native_rtv_view=0;
    std::uint64_t native_rtv_key=0;
    std::uint16_t width=0,height=0;
    std::uint8_t source_format=0,device_format=0;
    std::uint32_t flags=0;
    char name[96]{};
};
static std::array<V6610SemanticOutput,8> g_v6610_semantic_outputs{};
static int g_v6610_semantic_count=0;

// V66.12 exact final-composition helper trace.  The CPU hook brackets only
// WHGame+0x2543A6C, so any D3D12 OMSetRenderTargets/transition observations
// recorded here originate from KCD2's real final-composition call for that eye.
struct V6611ComposeTransition {
    ID3D12Resource* resource=nullptr;
    D3D12_RESOURCE_STATES before=D3D12_RESOURCE_STATE_COMMON;
    D3D12_RESOURCE_STATES after=D3D12_RESOURCE_STATE_COMMON;
};
struct V6611ComposeRecord {
    bool used=false; int eye=-1; std::uint32_t frame=0;
    std::uintptr_t input_texture=0,input_backing=0,input_rtv_view=0;
    std::uintptr_t output_texture=0,output_backing=0,output_rtv_view=0;
    std::uint32_t om_calls=0,barrier_calls=0;
    ID3D12Resource* rt[8]{};
    std::uint32_t rt_binds[8]{};
    bool rt_same_main[8]{};
    std::uint32_t rt_count=0;
    V6611ComposeTransition transitions[16]{};
    std::uint32_t transition_count=0;
};
static std::array<V6611ComposeRecord,12> g_v6612_compose_records{};
static int g_v6612_compose_count=0;
static thread_local int g_v6612_tls_compose_slot=-1;


// V66.12 F9 command-list ownership/order probe.  This is strictly read-only:
// while the existing L/R census is active, every game command list that binds a
// screen-ish RTV or transitions a tracked color resource is tagged with the eye
// phase that recorded it.  ExecuteCommandLists snapshots those tags in the exact
// submission order, then clears the live generation so pointer reuse cannot merge
// unrelated command-list recordings.
struct V6612ListTag {
    ID3D12GraphicsCommandList* list=nullptr;
    std::uint32_t generation=1;
    temporal_compose_d82::Recording temporal_recording{};
    std::uint32_t eye_mask=0;
    std::uint32_t main_bind_mask=0;
    std::uint32_t main_barrier_mask=0;
    bool present_boundary=false;
    ID3D12Resource* present_resource=nullptr;
    std::uint32_t om_calls[2]{0,0};
    std::uint32_t barrier_calls[2]{0,0};
    std::uint32_t frame[2]{0,0};
    std::uint64_t first_event[2]{0,0};
    std::uint64_t last_event[2]{0,0};
};
struct V6612SubmitItem {
    bool used=false;
    std::uint64_t batch=0;
    std::uint32_t index=0;
    ID3D12GraphicsCommandList* list=nullptr;
    std::uint32_t generation=0;
    std::uint32_t eye_mask=0;
    std::uint32_t main_bind_mask=0;
    std::uint32_t main_barrier_mask=0;
    bool present_boundary=false;
    ID3D12Resource* present_resource=nullptr;
    std::uint32_t om_calls[2]{0,0};
    std::uint32_t barrier_calls[2]{0,0};
    std::uint32_t frame[2]{0,0};
    std::uint64_t first_event[2]{0,0};
    std::uint64_t last_event[2]{0,0};
};
// Native0 pays only the atomic admission read in Reset; it never enters the
// temporal registry path. The SDK/epoch owner pins the selected technology.
static std::atomic<bool> g_d82_temporal_tracking{false};
static std::atomic<bool> g_d82_temporal_hooks_ready{false};
static std::atomic<bool> g_d82_composition_observed{false};
static std::atomic<bool> g_d82_reset_install_attempted{false};
static thread_local temporal_compose_d82::Scope g_d82_compose_scope{};
static thread_local bool g_d82_compose_draw_seen=false;
// Bounded temporal publication diagnostics; never an alternate output owner.
static temporal_pair_d85::Watchdog g_d85_watchdog{};
// Serializes temporal phase/queue/reset/slot ownership. It is never held while
// invoking a vendor SDK. Recursive only for synchronous D3D12 hook re-entry.
static std::recursive_mutex g_temporal_mutex_d85;
static temporal_capture_d85::Owner g_temporal_owner_d85{};
static std::mutex g_v6612_list_mutex;
static constexpr std::size_t kV6613LiveListCap=384;
static constexpr std::size_t kV6613SubmitCap=2048;
static std::array<V6612ListTag,kV6613LiveListCap> g_v6612_live_lists{};
static std::size_t g_v6612_live_count=0;
static std::array<V6612SubmitItem,kV6613SubmitCap> g_v6612_submit_items{};
static std::size_t g_v6612_submit_count=0;
static std::atomic<std::uint64_t> g_v6612_event_seq{0};
static std::atomic<bool> g_v6612_drain_requested{false};
static std::atomic<bool> g_v6612_final_dump_done{false};
// V66.14: F9 may be requested while a renderer call is still unwinding. Do not
// archive PRESENT-only command lists during that wait. Begin a short rolling
// archive on the first design LEFT phase, but DO NOT lock the stereo pair frame
// there: the design intentionally warms up for several LEFT frames. Lock the
// pair frame only when RIGHT begins, then finalize only after a RIGHT submission
// from that locked frame has actually reached ExecuteCommandLists and a later
// submitted PRESENT closes the window.
static std::atomic<bool> g_v6613_pair_capture_active{false};
static std::atomic<std::uint32_t> g_v6613_pair_frame{0};
static std::atomic<bool> g_v6614_right_submitted_seen{false};
// V66.18: continuous native stereo publication. Once the real pair frame
// locks, the first RIGHT main-writer batch is the proven queue boundary. Queue a
// LEFT snapshot immediately before that batch, then queue RIGHT immediately after
// it. Publish the two owned COMMON-state snapshots as one stereo pair and rearm
// the tracker for the next design frame so XR mode 2 can stay live continuously.
static std::atomic<bool> g_v6615_active{false};
static std::atomic<bool> g_v6615_left_copy_queued{false};
static std::atomic<bool> g_v6615_right_copy_queued{false};
static std::atomic<bool> g_v6615_pair_published{false};
static std::atomic<std::uint32_t> g_v6615_last_left_main_frame{0};
static std::atomic<std::uint64_t> g_v6615_last_left_main_batch{0};
// V66.18: after the first proven pair, stop using the one-shot diagnostic
// drain model.  Each consumed XR pair authorizes the next game frame to reuse
// the same owned L/R capture textures.  If XR has not consumed the prior pair,
// native stereo keeps rendering but publication skips that frame rather than
// overwriting one eye under the compositor.
static std::atomic<bool> g_v6618_live_stream_started{false};
static std::atomic<std::uint64_t> g_v6618_live_rearm_count{0};
static std::atomic<std::uint64_t> g_v6618_live_skip_unconsumed{0};

// V66.12R3 compile-order fix: observers below call V6612TagList before its
// implementation later in this translation unit.  The implementation lives in
// this translation unit's anonymous namespace, so the declaration MUST live in
// that same anonymous namespace as well (all namespace { } blocks here denote
// the same unnamed namespace). Defaults live only on this declaration.
namespace {
void V6612BindTemporalOutputD82(ID3D12GraphicsCommandList* list,std::uintptr_t output) noexcept;
void V6612RecordTemporalDrawD82(ID3D12GraphicsCommandList* list,UINT primitives,UINT instances) noexcept;
void V6612DiscardTemporalRecordingD82(ID3D12GraphicsCommandList* list) noexcept;
void V6612DiscardMainWriterD85(ID3D12GraphicsCommandList* list) noexcept;
void V6612ClearTemporalBindingsD82(ID3D12GraphicsCommandList* list) noexcept;
void V6612TagList(ID3D12GraphicsCommandList* list,int eye,std::uint32_t frame,
                  bool om_call,bool barrier_call,bool main_bind,bool main_barrier,
                  bool present_boundary=false,ID3D12Resource* present_resource=nullptr) noexcept;
}

// V65.87 same-frame COUNT2 MRT0/MRT1 pair proof.  Unlike V65.85's browser,
// this latches exactly one full-size MRT pair from one OMSetRenderTargets call
// after COUNT2 has already crossed a PRESENT.  Both snapshots are recorded on
// that same command list and are published only after ExecuteCommandLists sees it.
static std::mutex g_v6587_mutex;
static std::atomic<bool> g_v6587_enabled{false};
static std::uint64_t g_v6587_arm_present_serial=0;
static ID3D12Resource* g_v6587_source[2]={nullptr,nullptr};
static ID3D12Resource* g_v6587_snapshot[2]={nullptr,nullptr};
static ID3D12GraphicsCommandList* g_v6587_list=nullptr;
static bool g_v6587_snapshot_done[2]={false,false};
static bool g_v6587_recorded=false;
static bool g_v6587_submitted=false;
static std::uint64_t g_v6587_pair_serial=0;

// V65.93 F5 COUNT2 scene-color completion bridge.  Unlike V65.91/92,
// this lab NEVER inserts a copy into KCD2's ResourceBarrier stream.  Rung 1 is
// read-only and learns the complete lifetime of the full-resolution FP16 scene-
// color candidates tagged by SECONDARY-APPLY.  Only after a topology is
// certified may rung 2 arm a fresh pair; that pair is copied from a separate
// mod-owned command list appended AFTER the game's submitted-PRESENT batch on
// the same DIRECT queue (Crysis-style ownership handoff).
struct V6593Candidate {
    ID3D12Resource* source=nullptr;
    std::uintptr_t crv=0;
    std::uint64_t frame_serial=0;
    std::uint64_t present_serial=0;
    std::uint64_t tag_tick_ms=0;
    std::uint32_t native_count=0;
    UINT rt_slot=0;
    D3D12_RESOURCE_STATES state=D3D12_RESOURCE_STATE_RENDER_TARGET;
    bool state_known=true;
    bool split_open=false;
    D3D12_RESOURCE_STATES split_before=D3D12_RESOURCE_STATE_COMMON;
    D3D12_RESOURCE_STATES split_after=D3D12_RESOURCE_STATE_COMMON;
    bool state_mismatch=false;
    bool alias_seen=false;
    bool list_overflow=false;
    bool rt_exit_seen=false;
    bool write_after_exit=false;
    std::uint32_t transition_count=0;
    std::uint32_t uav_barrier_count=0;
    std::array<ID3D12GraphicsCommandList*,24> lists{};
    std::array<bool,24> submitted{};
    std::size_t list_count=0;
};

static std::mutex g_v6593_mutex;
static std::atomic<int> g_v6593_mode{0}; // 0 off, 1 census-only, 2 capture-armed/frozen
static V6593Candidate g_v6593_candidate[2];
static ID3D12Resource* g_v6593_snapshot[2]={nullptr,nullptr};
static std::uint64_t g_v6593_pair_serial=0;
static std::uint64_t g_v6593_work_frame=0;
static std::uintptr_t g_v6593_work_crv=0;
static std::uint64_t g_v6593_start_after_frame=0;
static std::uint64_t g_v6593_start_after_present=0;
static bool g_v6593_census_certified=false;
static std::array<std::uint64_t,8> g_v6593_nview_frames{};
static std::size_t g_v6593_nview_cursor=0;
static std::atomic<std::uint32_t> g_v6593_log_count{0};
static thread_local ID3D12Resource* g_v6593_tls_scene=nullptr;
static thread_local ID3D12GraphicsCommandList* g_v6593_tls_list=nullptr;
static thread_local UINT g_v6593_tls_slot=0;
static ID3D12CommandAllocator* g_v6593_copy_allocator=nullptr;
static ID3D12GraphicsCommandList* g_v6593_copy_list=nullptr;
static ID3D12Fence* g_v6593_copy_fence=nullptr;
static HANDLE g_v6593_copy_fence_event=nullptr;
static std::uint64_t g_v6593_copy_fence_value=0;

static void V6593ReleaseCandidate(V6593Candidate& c){
    if(c.source){ c.source->Release(); c.source=nullptr; }
    c=V6593Candidate{};
}

static bool V6593WaitCopyFenceLocked(std::uint32_t timeout_ms=1000){
    if(!g_v6593_copy_fence || !g_v6593_copy_fence_value || g_v6593_copy_fence->GetCompletedValue()>=g_v6593_copy_fence_value) return true;
    if(!g_v6593_copy_fence_event) return false;
    if(FAILED(g_v6593_copy_fence->SetEventOnCompletion(g_v6593_copy_fence_value,g_v6593_copy_fence_event))) return false;
    return WaitForSingleObject(g_v6593_copy_fence_event,timeout_ms)==WAIT_OBJECT_0;
}


static void V6593AddTouchListLocked(V6593Candidate& c, ID3D12GraphicsCommandList* list){
    if(!list) return;
    for(std::size_t i=0;i<c.list_count;++i) if(c.lists[i]==list) return;
    if(c.list_count>=c.lists.size()){ c.list_overflow=true; return; }
    c.lists[c.list_count]=list; c.submitted[c.list_count]=false; ++c.list_count;
}

static bool V6593AllListsSubmittedLocked(const V6593Candidate& c){
    if(!c.list_count) return false;
    for(std::size_t i=0;i<c.list_count;++i) if(!c.submitted[i]) return false;
    return true;
}

static bool V6593FrameSawExactNViewLocked(std::uint64_t frame){
    if(!frame) return false;
    for(const auto f:g_v6593_nview_frames) if(f==frame) return true;
    return false;
}

static bool V6593CandidateSafeLocked(const V6593Candidate& c){
    return c.source && c.state_known && !c.split_open && !c.state_mismatch && !c.alias_seen &&
           !c.list_overflow && c.rt_exit_seen && !c.write_after_exit && V6593AllListsSubmittedLocked(c);
}

static bool V6593EnsureSnapshotLocked(ID3D12Resource* source,int idx){
    if(!source || idx<0 || idx>1) return false;
    ID3D12Device* dev=nullptr; ID3D12CommandQueue* q=nullptr;
    if(!DeviceHook::TryGetDevice(&dev,&q) || !dev) return false;
    const auto d=source->GetDesc();
    if(g_v6593_snapshot[idx]){
        const auto od=g_v6593_snapshot[idx]->GetDesc();
        if(od.Dimension==d.Dimension && od.Width==d.Width && od.Height==d.Height && od.Format==d.Format &&
           od.DepthOrArraySize==d.DepthOrArraySize && od.MipLevels==d.MipLevels && od.SampleDesc.Count==d.SampleDesc.Count) return true;
        g_v6593_snapshot[idx]->Release(); g_v6593_snapshot[idx]=nullptr;
    }
    D3D12_HEAP_PROPERTIES hp{}; hp.Type=D3D12_HEAP_TYPE_DEFAULT; hp.CreationNodeMask=1; hp.VisibleNodeMask=1;
    ID3D12Resource* r=nullptr;
    if(FAILED(dev->CreateCommittedResource(&hp,D3D12_HEAP_FLAG_NONE,&d,D3D12_RESOURCE_STATE_COMMON,nullptr,IID_PPV_ARGS(&r)))) return false;
    g_v6593_snapshot[idx]=r;
    return true;
}

static bool V6593EnsureCopyObjectsLocked(){
    ID3D12Device* dev=nullptr; ID3D12CommandQueue* q=nullptr;
    if(!DeviceHook::TryGetDevice(&dev,&q) || !dev) return false;
    if(!g_v6593_copy_allocator && FAILED(dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,IID_PPV_ARGS(&g_v6593_copy_allocator)))) return false;
    if(!g_v6593_copy_list){
        if(FAILED(dev->CreateCommandList(0,D3D12_COMMAND_LIST_TYPE_DIRECT,g_v6593_copy_allocator,nullptr,IID_PPV_ARGS(&g_v6593_copy_list)))) return false;
        if(FAILED(g_v6593_copy_list->Close())) return false;
    }
    if(!g_v6593_copy_fence){
        if(FAILED(dev->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&g_v6593_copy_fence)))) return false;
        g_v6593_copy_fence_event=CreateEvent(nullptr,FALSE,FALSE,nullptr);
        if(!g_v6593_copy_fence_event) return false;
    }
    return true;
}

static bool V6593QueuePostSubmitCopyLocked(ID3D12CommandQueue* queue,std::uint64_t execute_serial){
    if(!queue || !V6593CandidateSafeLocked(g_v6593_candidate[0]) || !V6593CandidateSafeLocked(g_v6593_candidate[1])) return false;
    if(g_v6593_candidate[0].source==g_v6593_candidate[1].source) return false;
    if(!V6593EnsureSnapshotLocked(g_v6593_candidate[0].source,0) || !V6593EnsureSnapshotLocked(g_v6593_candidate[1].source,1)) return false;
    if(!V6593EnsureCopyObjectsLocked() || !V6593WaitCopyFenceLocked()) return false;
    if(FAILED(g_v6593_copy_allocator->Reset()) || FAILED(g_v6593_copy_list->Reset(g_v6593_copy_allocator,nullptr))) return false;

    D3D12_RESOURCE_BARRIER pre[4]{}; UINT pc=0;
    for(int i=0;i<2;++i){
        auto& c=g_v6593_candidate[i];
        if(c.state!=D3D12_RESOURCE_STATE_COPY_SOURCE){
            auto& b=pre[pc++]; b.Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION; b.Flags=D3D12_RESOURCE_BARRIER_FLAG_NONE;
            b.Transition.pResource=c.source; b.Transition.Subresource=D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
            b.Transition.StateBefore=c.state; b.Transition.StateAfter=D3D12_RESOURCE_STATE_COPY_SOURCE;
        }
        auto& d=pre[pc++]; d.Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION; d.Flags=D3D12_RESOURCE_BARRIER_FLAG_NONE;
        d.Transition.pResource=g_v6593_snapshot[i]; d.Transition.Subresource=D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        d.Transition.StateBefore=D3D12_RESOURCE_STATE_COMMON; d.Transition.StateAfter=D3D12_RESOURCE_STATE_COPY_DEST;
    }
    if(pc) g_v6593_copy_list->ResourceBarrier(pc,pre);
    g_v6593_copy_list->CopyResource(g_v6593_snapshot[0],g_v6593_candidate[0].source);
    g_v6593_copy_list->CopyResource(g_v6593_snapshot[1],g_v6593_candidate[1].source);

    D3D12_RESOURCE_BARRIER post[4]{}; UINT qc=0;
    for(int i=0;i<2;++i){
        auto& d=post[qc++]; d.Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION; d.Flags=D3D12_RESOURCE_BARRIER_FLAG_NONE;
        d.Transition.pResource=g_v6593_snapshot[i]; d.Transition.Subresource=D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        d.Transition.StateBefore=D3D12_RESOURCE_STATE_COPY_DEST; d.Transition.StateAfter=D3D12_RESOURCE_STATE_COMMON;
        auto& c=g_v6593_candidate[i];
        if(c.state!=D3D12_RESOURCE_STATE_COPY_SOURCE){
            auto& b=post[qc++]; b.Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION; b.Flags=D3D12_RESOURCE_BARRIER_FLAG_NONE;
            b.Transition.pResource=c.source; b.Transition.Subresource=D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
            b.Transition.StateBefore=D3D12_RESOURCE_STATE_COPY_SOURCE; b.Transition.StateAfter=c.state;
        }
    }
    if(qc) g_v6593_copy_list->ResourceBarrier(qc,post);
    if(FAILED(g_v6593_copy_list->Close())) return false;
    ID3D12CommandList* lists[]={g_v6593_copy_list};
    if(!DeviceHook::ExecuteCommandListsBypass(queue,1,lists)) return false;
    ++g_v6593_copy_fence_value;
    if(FAILED(queue->Signal(g_v6593_copy_fence,g_v6593_copy_fence_value))) return false;
    g_v6593_pair_serial=execute_serial?execute_serial:g_v6593_copy_fence_value;
    char m[768]{}; std::snprintf(m,sizeof(m),
        "V65.93-BRIDGE COPY-QUEUED serial=%llu frame=%llu crv=%p A=%p stateA=0x%X B=%p stateB=0x%X snapA=%p snapB=%p ordering=AFTER-GAME-SUBMITTED-PRESENT sameDIRECTqueue=1 inlineBarrierMutation=0",
        (unsigned long long)g_v6593_pair_serial,(unsigned long long)g_v6593_work_frame,reinterpret_cast<void*>(g_v6593_work_crv),
        g_v6593_candidate[0].source,(unsigned)g_v6593_candidate[0].state,g_v6593_candidate[1].source,(unsigned)g_v6593_candidate[1].state,
        g_v6593_snapshot[0],g_v6593_snapshot[1]); diagnostic_trace::Write(m);
    return true;
}

static void V6593ObserveBarrierLocked(ID3D12GraphicsCommandList* list,const D3D12_RESOURCE_BARRIER& b){
    if(g_v6593_mode.load(std::memory_order_relaxed)==0) return;
    for(int idx=0;idx<2;++idx){
        auto& c=g_v6593_candidate[idx];
        if(!c.source) continue;
        bool touches=false;
        if(b.Type==D3D12_RESOURCE_BARRIER_TYPE_TRANSITION && b.Transition.pResource==c.source){
            touches=true; V6593AddTouchListLocked(c,list); ++c.transition_count;
            if(b.Transition.Subresource!=D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES) c.state_mismatch=true; // fail closed: per-subresource state not modeled
            const auto before=b.Transition.StateBefore, after=b.Transition.StateAfter;
            const bool had_exit=c.rt_exit_seen;
            const bool leaves_rt=(before==D3D12_RESOURCE_STATE_RENDER_TARGET && after!=D3D12_RESOURCE_STATE_RENDER_TARGET);
            if(leaves_rt) c.rt_exit_seen=true;
            const bool next_is_write=(after&D3D12_RESOURCE_STATE_RENDER_TARGET) || (after&D3D12_RESOURCE_STATE_UNORDERED_ACCESS) ||
                                     (after&D3D12_RESOURCE_STATE_DEPTH_WRITE) || (after&D3D12_RESOURCE_STATE_COPY_DEST) ||
                                     (after&D3D12_RESOURCE_STATE_RESOLVE_DEST);
            if((had_exit || leaves_rt) && next_is_write) c.write_after_exit=true;
            if(b.Flags==D3D12_RESOURCE_BARRIER_FLAG_NONE){
                if(c.split_open) c.state_mismatch=true;
                if(c.state_known && c.state!=before) c.state_mismatch=true;
                c.state=after; c.state_known=true; c.split_open=false;
            } else if(b.Flags==D3D12_RESOURCE_BARRIER_FLAG_BEGIN_ONLY){
                if(c.split_open) c.state_mismatch=true;
                if(c.state_known && c.state!=before) c.state_mismatch=true;
                c.split_open=true; c.split_before=before; c.split_after=after;
            } else if(b.Flags==D3D12_RESOURCE_BARRIER_FLAG_END_ONLY){
                if(!c.split_open || c.split_before!=before || c.split_after!=after) c.state_mismatch=true;
                c.state=after; c.state_known=true; c.split_open=false;
            } else c.state_mismatch=true;
            const auto n=g_v6593_log_count.fetch_add(1,std::memory_order_relaxed)+1;
            if(n<=128){ char m[640]{}; std::snprintf(m,sizeof(m),
                "V65.93-CENSUS BARRIER role=%c frame=%llu crv=%p list=%p source=%p before=0x%X after=0x%X flags=0x%X sub=%u splitOpen=%d mismatch=%d rtExit=%d rewrite=%d transitions=%u",
                idx==0?'A':'B',(unsigned long long)c.frame_serial,reinterpret_cast<void*>(c.crv),list,c.source,(unsigned)before,(unsigned)after,
                (unsigned)b.Flags,b.Transition.Subresource,c.split_open?1:0,c.state_mismatch?1:0,c.rt_exit_seen?1:0,c.write_after_exit?1:0,c.transition_count); diagnostic_trace::Write(m); }
        } else if(b.Type==D3D12_RESOURCE_BARRIER_TYPE_ALIASING &&
                  (b.Aliasing.pResourceBefore==c.source || b.Aliasing.pResourceAfter==c.source)){
            touches=true; c.alias_seen=true; V6593AddTouchListLocked(c,list);
            char m[384]{}; std::snprintf(m,sizeof(m),
                "V65.93-CENSUS ALIAS role=%c frame=%llu source=%p list=%p before=%p after=%p unsafe=1",
                idx==0?'A':'B',(unsigned long long)c.frame_serial,c.source,list,b.Aliasing.pResourceBefore,b.Aliasing.pResourceAfter); diagnostic_trace::Write(m);
        } else if(b.Type==D3D12_RESOURCE_BARRIER_TYPE_UAV && (!b.UAV.pResource || b.UAV.pResource==c.source)){
            touches=true; ++c.uav_barrier_count; V6593AddTouchListLocked(c,list);
        }
        (void)touches;
    }
}



static bool V6587EnsureSnapshotLocked(ID3D12Resource* source,int eye){
    if(!source || eye<0 || eye>1) return false;
    ID3D12Device* dev=nullptr; ID3D12CommandQueue* q=nullptr;
    if(!DeviceHook::TryGetDevice(&dev,&q) || !dev) return false;
    if(q) q->Release();
    const auto d=source->GetDesc();
    if(g_v6587_snapshot[eye]){
        const auto od=g_v6587_snapshot[eye]->GetDesc();
        if(od.Width==d.Width && od.Height==d.Height && od.Format==d.Format && od.DepthOrArraySize==d.DepthOrArraySize){ dev->Release(); return true; }
        g_v6587_snapshot[eye]->Release(); g_v6587_snapshot[eye]=nullptr;
    }
    D3D12_HEAP_PROPERTIES hp{}; hp.Type=D3D12_HEAP_TYPE_DEFAULT;
    ID3D12Resource* r=nullptr;
    const HRESULT hr=dev->CreateCommittedResource(&hp,D3D12_HEAP_FLAG_NONE,&d,D3D12_RESOURCE_STATE_COMMON,nullptr,IID_PPV_ARGS(&r));
    dev->Release();
    if(FAILED(hr)) return false;
    g_v6587_snapshot[eye]=r; return true;
}


static bool V6585EnsureSnapshotLocked(ID3D12Resource* source, int idx) {
    if(!source || idx<0 || idx>=4) return false;
    ID3D12Device* dev=nullptr; ID3D12CommandQueue* q=nullptr;
    if(!DeviceHook::TryGetDevice(&dev,&q) || !dev) return false;
    if(q) q->Release();
    const auto d=source->GetDesc();
    if(g_v6585_snapshot[idx]){
        const auto od=g_v6585_snapshot[idx]->GetDesc();
        if(od.Width==d.Width && od.Height==d.Height && od.Format==d.Format && od.DepthOrArraySize==d.DepthOrArraySize){ dev->Release(); return true; }
        g_v6585_snapshot[idx]->Release(); g_v6585_snapshot[idx]=nullptr;
    }
    D3D12_HEAP_PROPERTIES hp{}; hp.Type=D3D12_HEAP_TYPE_DEFAULT;
    ID3D12Resource* r=nullptr;
    const HRESULT hr=dev->CreateCommittedResource(&hp,D3D12_HEAP_FLAG_NONE,&d,D3D12_RESOURCE_STATE_COMMON,nullptr,IID_PPV_ARGS(&r));
    dev->Release();
    if(FAILED(hr)) return false;
    g_v6585_snapshot[idx]=r; return true;
}

static void V6561ClearCandidatePairLocked() {
    for (int i=0;i<2;++i) {
        if (g_v6561_candidate_source[i]) { g_v6561_candidate_source[i]->Release(); g_v6561_candidate_source[i]=nullptr; }
        if (g_v6561_candidate_snapshot[i]) { g_v6561_candidate_snapshot[i]->Release(); g_v6561_candidate_snapshot[i]=nullptr; }
        g_v6561_snapshot_done[i]=false;
    }
    g_v6561_candidate_list=nullptr; g_v6561_prev_fmt28=nullptr; g_v6561_prev_fmt28_list=nullptr;
    g_v6561_pair_selected=false; g_v6561_candidate_desc={};
}

static bool V6561EnsureSnapshot(ID3D12Resource* source, int eye) {
    if (!source || eye<0 || eye>1) return false;
    ID3D12Device* dev=nullptr; ID3D12CommandQueue* q=nullptr;
    if (!DeviceHook::TryGetDevice(&dev,&q) || !dev) return false;
    if (q) q->Release();
    const auto d=source->GetDesc();
    if (g_v6561_candidate_snapshot[eye]) {
        const auto od=g_v6561_candidate_snapshot[eye]->GetDesc();
        if (od.Width==d.Width && od.Height==d.Height && od.Format==d.Format) return true;
        g_v6561_candidate_snapshot[eye]->Release(); g_v6561_candidate_snapshot[eye]=nullptr;
    }
    D3D12_HEAP_PROPERTIES hp{}; hp.Type=D3D12_HEAP_TYPE_DEFAULT;
    ID3D12Resource* r=nullptr;
    const HRESULT hr=dev->CreateCommittedResource(&hp,D3D12_HEAP_FLAG_NONE,&d,D3D12_RESOURCE_STATE_COMMON,nullptr,IID_PPV_ARGS(&r));
    dev->Release();
    if (FAILED(hr)) return false;
    g_v6561_candidate_snapshot[eye]=r; return true;
}
std::atomic<std::uint64_t> DeviceHook::s_stereo_producer_auto_stop_present_serial{0};
std::atomic<int> DeviceHook::s_stereo_producer_oracle_stage{0};
std::atomic<bool> DeviceHook::s_generic_depth_probe_enabled{false};
DeviceHook::GenericDepthCandidate DeviceHook::s_generic_depth_candidates[DeviceHook::kGenericDepthCandidateCount]{};
thread_local int DeviceHook::s_tls_depth_candidate = -1;
std::mutex DeviceHook::s_dsv_map_mutex;
std::unordered_map<SIZE_T, ID3D12Resource*> DeviceHook::s_dsv_resource_map;

std::mutex DeviceHook::s_rtv_map_mutex;
std::unordered_map<SIZE_T, ID3D12Resource*> DeviceHook::s_rtv_resource_map;
std::unordered_map<SIZE_T, D3D12_RENDER_TARGET_VIEW_DESC> DeviceHook::s_rtv_desc_map;
std::atomic<UINT> DeviceHook::s_rtv_descriptor_increment{0};
std::atomic<std::uint64_t> DeviceHook::s_native_rt_bind_count{0};
std::mutex DeviceHook::s_bridge_capture_mutex;
ID3D12Resource* DeviceHook::s_bridge_current_main_target = nullptr;
ID3D12GraphicsCommandList* DeviceHook::s_bridge_current_main_list = nullptr;
D3D12_RESOURCE_STATES DeviceHook::s_bridge_current_main_state = D3D12_RESOURCE_STATE_COMMON;
DWORD DeviceHook::s_bridge_current_main_tid = 0;
ID3D12Resource* DeviceHook::s_bridge_left_capture = nullptr;
D3D12_RESOURCE_DESC DeviceHook::s_bridge_left_desc{};
ID3D12Resource* DeviceHook::s_bridge_right_capture = nullptr;
D3D12_RESOURCE_DESC DeviceHook::s_bridge_right_desc{};
std::atomic<std::uint64_t> DeviceHook::s_bridge_copy_count{0};
std::atomic<std::uint64_t> DeviceHook::s_execute_submission_count{0};
std::atomic<std::uint64_t> DeviceHook::s_bridge_current_main_last_submit{0};
std::atomic<std::uint64_t> DeviceHook::s_bridge_record_serial{0};
std::atomic<int> DeviceHook::s_bridge_record_eye{-1};
std::atomic<std::uint64_t> DeviceHook::s_bridge_left_recorded_serial{0};
std::atomic<std::uint64_t> DeviceHook::s_bridge_right_recorded_serial{0};
std::atomic<std::uint64_t> DeviceHook::s_bridge_left_submitted_serial{0};
std::atomic<std::uint64_t> DeviceHook::s_bridge_right_submitted_serial{0};
std::atomic<std::uint64_t> DeviceHook::s_bridge_complete_pair_serial{0};
std::atomic<std::uint64_t> DeviceHook::s_bridge_complete_pair_camera_frame{0};
std::atomic<std::uint64_t> DeviceHook::s_bridge_complete_pair_count{0};
std::atomic<std::uint64_t> DeviceHook::s_bridge_last_consumed_pair_serial{0};

std::uint64_t DeviceHook_V6634LastConsumed() noexcept;

// V66.34 two-slot stereo capture. s_bridge_left/right_capture remain the CURRENT WRITE
// slot (all existing capture code writes through them unchanged). A second owned pair
// is kept as the spare, and the PUBLISHED pair is held as its own AddRef'd snapshot.
// At each LEFT pair-begin the write slot is swapped away from the published pair, so the
// game never has to wait for XR to pick up the previous pair. The only remaining skip is
// the rare case where XR is reading (claimed, not yet submitted) the slot we would write.
// All fields below are guarded by s_bridge_capture_mutex.
namespace {
ID3D12Resource* g_v6634_spare_left=nullptr;
ID3D12Resource* g_v6634_spare_right=nullptr;
D3D12_RESOURCE_DESC g_v6634_spare_left_desc{};
D3D12_RESOURCE_DESC g_v6634_spare_right_desc{};
ID3D12Resource* g_v6634_pub_left=nullptr;   // owned reference to the published pair
ID3D12Resource* g_v6634_pub_right=nullptr;
std::uint64_t g_v6634_pub_serial=0;
native_upscaler_stereo_d82::PairOwner g_v6634_pub_owner_d85{};
ID3D12Resource* g_v6634_xr_claim_left=nullptr; // identity only (no reference): slot XR is reading
std::atomic<unsigned long long> g_v6634_swaps{0};
std::atomic<unsigned long long> g_v6634_claim_skips{0};
std::atomic<unsigned long long> g_v6634_superseded{0};
std::atomic<bool> g_v6636_cadence_owner{false};
std::atomic<std::uint64_t> g_v6636_xr_opportunity{0};
std::atomic<std::uint64_t> g_v6636_last_started_opportunity{0};
std::atomic<std::uint64_t> g_v6636_cadence_skips{0};
// V66.38: capture renderer-matched XR pose tokens at the actual LEFT/RIGHT copy
// boundaries. This tests the old hypothesis that one shared post-RIGHT token is
// the wrong owner for both finished eye images. No camera matrix is rewritten here.
std::atomic<int> g_v6638_pair_pose_mode{0};
std::atomic<std::uint64_t> g_v6638_left_pose_frame{0};
std::atomic<std::uint64_t> g_v6638_right_pose_frame{0};
std::atomic<std::uint64_t> g_v6638_shared_pose_frame{0};
} // namespace

// Caller holds s_bridge_capture_mutex.
static void V6634PublishWriteSlotLocked(ID3D12Resource* left,ID3D12Resource* right,std::uint64_t serial,
    native_upscaler_stereo_d82::PairOwner owner) noexcept
{
    if(!left || !right || serial==0) return;
    if(g_v6634_pub_serial!=0 && g_v6634_pub_serial!=serial &&
       DeviceHook_V6634LastConsumed()!=g_v6634_pub_serial)
        g_v6634_superseded.fetch_add(1,std::memory_order_relaxed);
    left->AddRef(); right->AddRef();
    if(g_v6634_pub_left) g_v6634_pub_left->Release();
    if(g_v6634_pub_right) g_v6634_pub_right->Release();
    g_v6634_pub_left=left; g_v6634_pub_right=right; g_v6634_pub_serial=serial;
    g_v6634_pub_owner_d85=owner;
}
std::atomic<bool> DeviceHook::s_v6627_performance_quiet{false};

namespace {
struct BridgeListTagV6547 {
    ID3D12GraphicsCommandList* list{nullptr};
    std::uint64_t serial{0};
    int eye{-1};
};
struct BridgeTicketV6547 {
    std::uint64_t serial{0};
    int eye{-1};
    std::uint64_t enqueue_execute{0};
};
std::array<BridgeListTagV6547,128> g_bridge_list_tags_v6547{};
std::uint64_t g_bridge_list_tag_cursor_v6547=0;
std::deque<BridgeTicketV6547> g_bridge_tickets_v6547{};
struct BridgePairStateV6547 {
    std::uint64_t serial{0};
    native_upscaler_stereo_d82::PairOwner owner{};
    unsigned recorded_mask{0};
    unsigned submitted_mask{0};
    bool complete{false};
};
std::array<BridgePairStateV6547,128> g_bridge_pair_states_v6547{};
std::uint64_t g_bridge_pair_state_cursor_v6547=0;
std::atomic<std::uint64_t> g_bridge_owner_complete_v6547{0};
std::atomic<std::uint64_t> g_bridge_ticket_miss_v6547{0};
constexpr std::size_t kBridgeTicketCapV6547=64;
std::condition_variable g_bridge_cert_cv_v6550;

BridgePairStateV6547& BridgePairStateForV6547(std::uint64_t serial) {
    for(auto& st:g_bridge_pair_states_v6547) if(st.serial==serial) {
        // Native legacy ownership can be invalidated at any recording/submission
        // lookup, but never upgraded again if technology changed mid capture.
        if(native_upscaler_stereo_d82::TemporalActive())st.owner={0,-1};
        return st;
    }
    auto& st=g_bridge_pair_states_v6547[static_cast<std::size_t>(g_bridge_pair_state_cursor_v6547++ % g_bridge_pair_states_v6547.size())];
    st={}; st.serial=serial;
    // Legacy raw-main diagnostics do not carry the completed temporal-output
    // certificate. Never relabel such a capture Native0 after a later switch.
    // The verified temporal production publisher carries its own exact owner.
    st.owner=native_upscaler_stereo_d82::TemporalActive()
        ?native_upscaler_stereo_d82::PairOwner{0,-1}
        :native_upscaler_stereo_d82::PairOwner{};
    return st;
}
}
ID3D12CommandAllocator* DeviceHook::s_bridge_copy_allocator = nullptr;
ID3D12GraphicsCommandList* DeviceHook::s_bridge_copy_list = nullptr;
ID3D12Fence* DeviceHook::s_bridge_copy_fence = nullptr;
HANDLE DeviceHook::s_bridge_copy_fence_event = nullptr;
std::uint64_t DeviceHook::s_bridge_copy_fence_value = 0;
std::atomic<bool> DeviceHook::s_mode_pso_probe_enabled{false};
std::atomic<std::uint64_t> DeviceHook::s_mode_pso_event_count{0};
std::atomic<std::uint64_t> DeviceHook::s_mode0_draw_count{0};
std::atomic<std::uint64_t> DeviceHook::s_mode2_draw_count{0};
std::mutex DeviceHook::s_mode_pso_mutex;
std::unordered_map<ID3D12PipelineState*, DeviceHook::PsoShaderRecord> DeviceHook::s_pso_shader_hashes;
std::unordered_map<ID3D12GraphicsCommandList*, DeviceHook::ModePsoListState> DeviceHook::s_mode_list_state;
std::unordered_map<ID3D12Resource*, std::uint64_t> DeviceHook::s_mode0_rt_draws;
std::unordered_map<ID3D12Resource*, std::uint64_t> DeviceHook::s_mode2_rt_draws;
std::mutex DeviceHook::s_descriptor_trace_mutex;
std::unordered_map<ID3D12DescriptorHeap*, DeviceHook::DescriptorHeapRange> DeviceHook::s_cbv_srv_uav_heaps;
std::unordered_map<SIZE_T, ID3D12Resource*> DeviceHook::s_srv_resource_map;
std::unordered_map<ID3D12PipelineState*, std::uint64_t> DeviceHook::s_v6625_last_ui_table_hash;
std::atomic<std::uint64_t> DeviceHook::s_v6625_ui_descriptor_dumps{0};
std::optional<hook::Hook> DeviceHook::s_hook_create_graphics_pipeline_state;
std::optional<hook::Hook> DeviceHook::s_hook_set_pipeline_state;
std::optional<hook::Hook> DeviceHook::s_hook_create_queue;
std::optional<hook::Hook> DeviceHook::s_hook_execute_command_lists;
std::optional<hook::Hook> DeviceHook::s_hook_create_command_list;
std::optional<hook::Hook> DeviceHook::s_hook_resource_barrier;
std::optional<hook::Hook> DeviceHook::s_hook_create_rtv;
std::optional<hook::Hook> DeviceHook::s_hook_create_dsv;
std::optional<hook::Hook> DeviceHook::s_hook_create_descriptor_heap;
std::optional<hook::Hook> DeviceHook::s_hook_create_srv;
std::optional<hook::Hook> DeviceHook::s_hook_copy_descriptors;
std::optional<hook::Hook> DeviceHook::s_hook_copy_descriptors_simple;
std::optional<hook::Hook> DeviceHook::s_hook_graphics_root_descriptor_table;
std::optional<hook::Hook> DeviceHook::s_hook_clear_command_list_state_d82;
std::optional<hook::Hook> DeviceHook::s_hook_reset_command_list_d82;
std::optional<hook::Hook> DeviceHook::s_hook_draw_instanced;
std::optional<hook::Hook> DeviceHook::s_hook_draw_indexed_instanced;
std::optional<hook::Hook> DeviceHook::s_hook_rs_set_viewports;
std::optional<hook::Hook> DeviceHook::s_hook_rs_set_scissor_rects;
std::optional<hook::Hook> DeviceHook::s_hook_om_set_render_targets;

bool DeviceHook::Install()
{
    // Plain exported function, not a COM vtable slot - overlays don't hook
    // this. BUT: this project's own accumulated testing (see ROADMAP.md,
    // "MAJOR UPDATE 5") found that touching D3D12 too early - even just a
    // GetProcAddress/hook install, without creating any device ourselves -
    // can still race CryEngine's own early adapter/device boot sequence and
    // crash. Poll for d3d12.dll to actually be loaded (same pattern
    // camera_hook.cpp already uses successfully for WHGame.dll) rather than
    // failing immediately or, worse, reintroducing a dummy device just to
    // "wait productively" - we do nothing D3D12-related at all until the
    // module is genuinely present.
    HMODULE d3d12_module = nullptr;
    for (int i = 0; i < 100 && !d3d12_module; ++i) // up to ~10s
    {
        d3d12_module = GetModuleHandleA("d3d12.dll");
        if (!d3d12_module)
            Sleep(100);
    }
    if (!d3d12_module)
    {
        KCD2VR_DIAG_LOG(Bug,error,"DeviceHook: d3d12.dll not loaded in process after polling");
        return false;
    }
    void* target_address = reinterpret_cast<void*>(GetProcAddress(d3d12_module, "D3D12CreateDevice"));
    if (!target_address)
    {
        KCD2VR_DIAG_LOG(Bug,error,"DeviceHook: GetProcAddress(D3D12CreateDevice) failed");
        return false;
    }

    KCD2VR_DIAG_LOG(Bug,info,"DeviceHook: D3D12CreateDevice address = {:#x}", reinterpret_cast<uintptr_t>(target_address));

    hook::InlineRequest request{
        "kcd2vr_device_hook_create_device",
        hook::Target{DetourModKit::Address{reinterpret_cast<uintptr_t>(target_address)}},
        hook::Options{}
    };

    DetourModKit::Result<hook::Hook> result = hook::inline_at(std::move(request), &DeviceHook::DetourD3D12CreateDevice);
    if (!result)
    {
        KCD2VR_DIAG_LOG(Bug,error,"DeviceHook: D3D12CreateDevice inline_at failed");
        return false;
    }

    hook_create_device_ = std::move(*result);
    s_original_create_device = hook_create_device_->original<D3D12CreateDeviceFunc>();

    DetourModKit::Result<void> enable_result = hook_create_device_->enable();
    if (!enable_result)
    {
        KCD2VR_DIAG_LOG(Bug,error,"DeviceHook: D3D12CreateDevice hook failed to enable");
        s_original_create_device = nullptr;
        hook_create_device_.reset();
        return false;
    }

    KCD2VR_DIAG_LOG(Bug,info,"DeviceHook: D3D12CreateDevice hook installed and enabled - waiting for the game to create its device");
    return true;
}

void DeviceHook::Uninstall()
{
    g_d82_temporal_tracking.store(false,std::memory_order_release);
    g_d82_temporal_hooks_ready.store(false,std::memory_order_release);
    // A Reset callback can be in flight independently of render callbacks. Stop
    // admission and disable the hook, but retain its original/trampoline storage
    // until process teardown, including an uncertain partial-enable failure.
    if(s_hook_reset_command_list_d82)(void)s_hook_reset_command_list_d82->disable();
    if(s_hook_clear_command_list_state_d82)(void)s_hook_clear_command_list_state_d82->disable();
    V6600GpuUninstall();
    SetRenderTickCallback(nullptr);
    WaitForRenderCallbacksIdle();
    if (s_hook_om_set_render_targets) { (void)s_hook_om_set_render_targets->disable(); s_hook_om_set_render_targets.reset(); }
    if (s_hook_set_pipeline_state) { (void)s_hook_set_pipeline_state->disable(); s_hook_set_pipeline_state.reset(); }
    if (s_hook_create_graphics_pipeline_state) { (void)s_hook_create_graphics_pipeline_state->disable(); s_hook_create_graphics_pipeline_state.reset(); }
    if (s_hook_draw_indexed_instanced) { (void)s_hook_draw_indexed_instanced->disable(); s_hook_draw_indexed_instanced.reset(); }
    if (s_hook_rs_set_viewports) { (void)s_hook_rs_set_viewports->disable(); s_hook_rs_set_viewports.reset(); }
    if (s_hook_rs_set_scissor_rects) { (void)s_hook_rs_set_scissor_rects->disable(); s_hook_rs_set_scissor_rects.reset(); }
    s_original_rs_set_viewports=nullptr; s_original_rs_set_scissor_rects=nullptr;
    s_output_layout_hooks_installed.store(false,std::memory_order_release); s_output_layout_probe_enabled.store(false,std::memory_order_release);
    if (s_hook_draw_instanced) { (void)s_hook_draw_instanced->disable(); s_hook_draw_instanced.reset(); }
    if (s_hook_graphics_root_descriptor_table) { (void)s_hook_graphics_root_descriptor_table->disable(); s_hook_graphics_root_descriptor_table.reset(); }
    if (s_hook_copy_descriptors_simple) { (void)s_hook_copy_descriptors_simple->disable(); s_hook_copy_descriptors_simple.reset(); }
    if (s_hook_copy_descriptors) { (void)s_hook_copy_descriptors->disable(); s_hook_copy_descriptors.reset(); }
    if (s_hook_create_srv) { (void)s_hook_create_srv->disable(); s_hook_create_srv.reset(); }
    if (s_hook_create_descriptor_heap) { (void)s_hook_create_descriptor_heap->disable(); s_hook_create_descriptor_heap.reset(); }
    if (s_hook_create_dsv) { (void)s_hook_create_dsv->disable(); s_hook_create_dsv.reset(); }
    if (s_hook_create_rtv) { (void)s_hook_create_rtv->disable(); s_hook_create_rtv.reset(); }
    if (s_hook_resource_barrier) { (void)s_hook_resource_barrier->disable(); s_hook_resource_barrier.reset(); }
    if (s_hook_execute_command_lists) { (void)s_hook_execute_command_lists->disable(); s_hook_execute_command_lists.reset(); }
    if (s_hook_create_command_list) { (void)s_hook_create_command_list->disable(); s_hook_create_command_list.reset(); }
    if (s_hook_create_queue) { (void)s_hook_create_queue->disable(); s_hook_create_queue.reset(); }
    if (hook_create_device_) { (void)hook_create_device_->disable(); hook_create_device_.reset(); }
    s_original_create_device = nullptr;
    s_original_create_queue = nullptr;
    s_original_execute_command_lists = nullptr;
    s_original_create_graphics_pipeline_state = nullptr;
    s_original_create_command_list = nullptr;
    s_original_set_pipeline_state = nullptr;
    s_original_resource_barrier = nullptr;
    s_original_create_rtv = nullptr;
    s_original_create_dsv = nullptr;
    s_original_create_descriptor_heap = nullptr;
    s_original_create_srv = nullptr;
    s_original_copy_descriptors = nullptr;
    s_original_copy_descriptors_simple = nullptr;
    s_original_set_graphics_root_descriptor_table = nullptr;
    s_original_draw_instanced = nullptr;
    s_original_draw_indexed_instanced = nullptr;
    s_original_om_set_render_targets = nullptr;
    s_queue_hook_installed.store(false, std::memory_order_relaxed);
    s_execute_hook_installed.store(false, std::memory_order_relaxed);
    s_create_graphics_pso_hook_installed.store(false, std::memory_order_relaxed);
    s_command_list_hook_installed.store(false, std::memory_order_relaxed);
    s_set_pipeline_state_hook_installed.store(false, std::memory_order_relaxed);
    s_resource_barrier_hook_installed.store(false, std::memory_order_relaxed);
    s_create_rtv_hook_installed.store(false, std::memory_order_relaxed);
    s_create_dsv_hook_installed.store(false, std::memory_order_relaxed);
    s_create_descriptor_heap_hook_installed.store(false, std::memory_order_relaxed);
    s_create_srv_hook_installed.store(false, std::memory_order_relaxed);
    s_copy_descriptors_hook_installed.store(false, std::memory_order_relaxed);
    s_graphics_root_descriptor_table_hook_installed.store(false, std::memory_order_relaxed);
    s_draw_hooks_installed.store(false, std::memory_order_relaxed);
    s_om_set_rt_hook_installed.store(false, std::memory_order_relaxed);
    { std::lock_guard<std::mutex> lock(s_rtv_map_mutex); s_rtv_resource_map.clear(); }
    { std::lock_guard<std::mutex> lock(s_mode_pso_mutex); s_pso_shader_hashes.clear(); s_mode_list_state.clear(); s_mode0_rt_draws.clear(); s_mode2_rt_draws.clear(); }
    { std::lock_guard<std::mutex> lock(s_descriptor_trace_mutex); s_cbv_srv_uav_heaps.clear(); s_srv_resource_map.clear(); s_v6625_last_ui_table_hash.clear(); }
    s_v6625_ui_descriptor_dumps.store(0,std::memory_order_relaxed);
    s_mode_pso_probe_enabled.store(false,std::memory_order_relaxed);
    { std::lock_guard<std::mutex> lock(s_dsv_map_mutex); s_dsv_resource_map.clear(); }
    s_generic_depth_probe_enabled.store(false,std::memory_order_relaxed);
    s_rtv_descriptor_increment.store(0, std::memory_order_relaxed);
    s_native_rt_bind_count.store(0, std::memory_order_relaxed);

    {
        std::lock_guard<std::mutex> lock(s_present_resource_mutex);
        if (s_latest_present_resource) { s_latest_present_resource->Release(); s_latest_present_resource = nullptr; }
        s_latest_present_eye = 0;
        s_latest_present_serial = 0;
        s_latest_present_tick_ms = 0;
        s_latest_present_camera_frame = 0;
        s_latest_present_command_list = nullptr;
    }
    s_afr_stereo_capture.store(false, std::memory_order_relaxed);
    s_internal_vr_command_list.store(nullptr, std::memory_order_relaxed);
    s_internal_menu_command_list.store(nullptr, std::memory_order_relaxed);
    s_present_capture_count.store(0, std::memory_order_relaxed);
    s_recursive_barrier_count.store(0, std::memory_order_relaxed);
    s_recursive_rt_barrier_count.store(0, std::memory_order_relaxed);
    s_customres_probe_enabled.store(false, std::memory_order_relaxed);
    s_customres_probe_width.store(0, std::memory_order_relaxed);
    s_customres_probe_height.store(0, std::memory_order_relaxed);
    s_customres_barrier_count.store(0, std::memory_order_relaxed);
    s_customres_rt_barrier_count.store(0, std::memory_order_relaxed);
    CameraHook::SetAfrStereoEnabled(false);
    ResetBridgeCapture();

    if (ID3D12Device* d = s_captured_device.exchange(nullptr)) d->Release();
    if (ID3D12CommandQueue* q = s_captured_queue.exchange(nullptr)) q->Release();
    s_captured.store(false, std::memory_order_relaxed);
}


void DeviceHook::SetRenderTickCallback(RenderTickCallback callback)
{
    s_render_tick_callback.store(callback, std::memory_order_release);
}

void DeviceHook::WaitForRenderCallbacksIdle()
{
    while (s_active_render_callbacks.load(std::memory_order_acquire) != 0)
        Sleep(1);
}

bool DeviceHook::ExecuteCommandListsBypass(
    ID3D12CommandQueue* queue, UINT count, ID3D12CommandList* const* lists)
{
    auto original = s_original_execute_command_lists;
    if (!queue || !original)
        return false;
    original(queue, count, lists);
    return true;
}

void DeviceHook::SetInternalVrCommandList(ID3D12GraphicsCommandList* list)
{
    s_internal_vr_command_list.store(list, std::memory_order_release);
}

void DeviceHook::SetInternalMenuCommandList(ID3D12GraphicsCommandList* list)
{
    s_internal_menu_command_list.store(list, std::memory_order_release);
}
void DeviceHook::SetInternalHudCommandList(ID3D12GraphicsCommandList* list) { s_internal_hud_command_list.store(list, std::memory_order_release); }


std::uint64_t DeviceHook::PresentCaptureCount()
{
    return s_present_capture_count.load(std::memory_order_acquire);
}

bool DeviceHook::CurrentExecuteSubmittedPresent()
{
    return s_current_execute_submitted_present;
}

std::uint64_t DeviceHook::RecursiveBarrierCount()
{
    return s_recursive_barrier_count.load(std::memory_order_acquire);
}

std::uint64_t DeviceHook::RecursiveRenderTargetBarrierCount()
{
    return s_recursive_rt_barrier_count.load(std::memory_order_acquire);
}

void DeviceHook::SetCustomResolutionProbe(bool enabled, std::uint32_t width, std::uint32_t height)
{
    s_customres_probe_width.store(width, std::memory_order_release);
    s_customres_probe_height.store(height, std::memory_order_release);
    s_customres_probe_enabled.store(enabled && width > 0 && height > 0, std::memory_order_release);
}

std::uint64_t DeviceHook::CustomResolutionBarrierCount()
{
    return s_customres_barrier_count.load(std::memory_order_acquire);
}

std::uint64_t DeviceHook::CustomResolutionRenderTargetBarrierCount()
{
    return s_customres_rt_barrier_count.load(std::memory_order_acquire);
}





void DeviceHook::BridgeBeginEyeRecord(std::uint64_t serial, int eye_sign)
{
    if(serial==0 || (eye_sign!=-1 && eye_sign!=1)) return;
    const int eye=eye_sign<0?0:1;
    std::lock_guard<std::mutex> lock(s_bridge_capture_mutex);
    // Durable FIFO ownership. Do not depend on the CSystem::Render caller thread
    // still being active when D3D12 worker threads record the barrier.
    for(const auto& t:g_bridge_tickets_v6547)
        if(t.serial==serial && t.eye==eye) return;
    if(g_bridge_tickets_v6547.size()>=kBridgeTicketCapV6547) {
        g_bridge_tickets_v6547.pop_front();
        g_bridge_ticket_miss_v6547.fetch_add(1,std::memory_order_relaxed);
    }
    g_bridge_tickets_v6547.push_back({serial,eye,s_execute_submission_count.load(std::memory_order_relaxed)});
    char cm[256]{}; std::snprintf(cm,sizeof(cm),
        "V65.47-OWNER TICKET arm serial=%llu eye=%c pending=%zu",
        static_cast<unsigned long long>(serial),eye==0?'L':'R',g_bridge_tickets_v6547.size());
    debug_trace::Write(cm);
}



void DeviceHook::BridgePrepareCertifiedPair(std::uint64_t serial)
{
    std::lock_guard<std::mutex> lock(s_bridge_capture_mutex);
    // F9 is a separate bounded experiment. Remove stale FIFO/tag state from
    // earlier F8 owner archaeology so the next RT->PRESENT barrier can only
    // consume the eye ticket belonging to this tangible transaction.
    g_bridge_tickets_v6547.clear();
    for(auto& tag:g_bridge_list_tags_v6547) tag={};
    for(auto& st:g_bridge_pair_states_v6547) st={};
    g_bridge_pair_state_cursor_v6547=0;
    s_bridge_left_recorded_serial.store(0,std::memory_order_release);
    s_bridge_right_recorded_serial.store(0,std::memory_order_release);
    s_bridge_left_submitted_serial.store(0,std::memory_order_release);
    s_bridge_right_submitted_serial.store(0,std::memory_order_release);
    s_bridge_complete_pair_serial.store(0,std::memory_order_release);
    s_bridge_complete_pair_camera_frame.store(0,std::memory_order_release);
    char cm[240]{}; std::snprintf(cm,sizeof(cm),
        "V65.50-CERT prepare serial=%llu staleTickets=0",
        static_cast<unsigned long long>(serial));
    debug_trace::Write(cm);
}

void DeviceHook::BridgePrepareStreamingPair(std::uint64_t serial)
{
    if(serial==0) return;
    std::lock_guard<std::mutex> lock(s_bridge_capture_mutex);
    // Unlike the one-shot preparation above, keep the last published pair and
    // its reusable capture textures visible to XR until this new serial is fully
    // recorded AND submitted. This avoids a mono gap between consecutive stereo
    // frames. Per-serial recorded/submitted state remains independent.
    auto& st=BridgePairStateForV6547(serial);
    st.recorded_mask=0; st.submitted_mask=0; st.complete=false;
    for(auto& tag:g_bridge_list_tags_v6547) if(tag.serial==serial) tag={};
    char cm[272]{}; std::snprintf(cm,sizeof(cm),
        "V66.12-DESIGN-STREAM prepare serial=%llu publishedSerial=%llu policy=KEEP-LAST-CERTIFIED-UNTIL-REPLACED",
        static_cast<unsigned long long>(serial),
        static_cast<unsigned long long>(s_bridge_complete_pair_serial.load(std::memory_order_relaxed)));
    debug_trace::Write(cm);
}

bool DeviceHook::RecordStereoDesignCurrentMainTarget(std::uint64_t serial, int eye)
{
    if(serial==0 || (eye!=0 && eye!=1)) return false;
    std::lock_guard<std::mutex> lock(s_bridge_capture_mutex);
    if(!s_bridge_current_main_target || !s_bridge_current_main_list || !s_original_resource_barrier) return false;
    if(s_bridge_current_main_tid!=GetCurrentThreadId()) return false;
    // This diagnostic writes only while the tracked KCD2 swapchain target is
    // still a RENDER_TARGET on the same recording thread. Any other lifecycle
    // state is refused rather than guessed.
    if(s_bridge_current_main_state!=D3D12_RESOURCE_STATE_RENDER_TARGET) return false;
    auto* device=s_captured_device.load(std::memory_order_acquire);
    if(!device) return false;
    auto* source=s_bridge_current_main_target;
    auto* list=s_bridge_current_main_list;
    const auto src_desc=source->GetDesc();
    if(src_desc.Dimension!=D3D12_RESOURCE_DIMENSION_TEXTURE2D || src_desc.Width==0 || src_desc.Height==0 ||
       src_desc.SampleDesc.Count!=1) return false;
    ID3D12Resource*& capture=(eye==0)?s_bridge_left_capture:s_bridge_right_capture;
    D3D12_RESOURCE_DESC& capture_desc=(eye==0)?s_bridge_left_desc:s_bridge_right_desc;
    const bool same=capture && capture_desc.Dimension==src_desc.Dimension && capture_desc.Width==src_desc.Width &&
        capture_desc.Height==src_desc.Height && capture_desc.Format==src_desc.Format &&
        capture_desc.SampleDesc.Count==src_desc.SampleDesc.Count && capture_desc.Flags==src_desc.Flags;
    if(capture && !same){ capture->Release(); capture=nullptr; capture_desc={}; }
    if(!capture){
        D3D12_HEAP_PROPERTIES heap{}; heap.Type=D3D12_HEAP_TYPE_DEFAULT; heap.CreationNodeMask=1; heap.VisibleNodeMask=1;
        if(FAILED(device->CreateCommittedResource(&heap,D3D12_HEAP_FLAG_NONE,&src_desc,D3D12_RESOURCE_STATE_COMMON,
                                                  nullptr,IID_PPV_ARGS(&capture)))) return false;
        capture_desc=src_desc;
    }
    D3D12_RESOURCE_BARRIER pre[2]{};
    pre[0].Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION; pre[0].Transition.pResource=source;
    pre[0].Transition.Subresource=D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    pre[0].Transition.StateBefore=D3D12_RESOURCE_STATE_RENDER_TARGET; pre[0].Transition.StateAfter=D3D12_RESOURCE_STATE_COPY_SOURCE;
    pre[1].Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION; pre[1].Transition.pResource=capture;
    pre[1].Transition.Subresource=D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    pre[1].Transition.StateBefore=D3D12_RESOURCE_STATE_COMMON; pre[1].Transition.StateAfter=D3D12_RESOURCE_STATE_COPY_DEST;
    s_original_resource_barrier(list,2,pre);
    list->CopyResource(capture,source);
    D3D12_RESOURCE_BARRIER post[2]= {pre[0],pre[1]};
    post[0].Transition.StateBefore=D3D12_RESOURCE_STATE_COPY_SOURCE; post[0].Transition.StateAfter=D3D12_RESOURCE_STATE_RENDER_TARGET;
    post[1].Transition.StateBefore=D3D12_RESOURCE_STATE_COPY_DEST; post[1].Transition.StateAfter=D3D12_RESOURCE_STATE_COMMON;
    s_original_resource_barrier(list,2,post);
    if(eye==0) s_bridge_left_recorded_serial.store(serial,std::memory_order_release);
    else s_bridge_right_recorded_serial.store(serial,std::memory_order_release);
    auto& pst=BridgePairStateForV6547(serial); pst.recorded_mask |= eye==0?1u:2u;
    auto& tag=g_bridge_list_tags_v6547[static_cast<std::size_t>(g_bridge_list_tag_cursor_v6547++ % g_bridge_list_tags_v6547.size())];
    tag.list=list; tag.serial=serial; tag.eye=eye;
    const auto n=s_bridge_copy_count.fetch_add(1,std::memory_order_relaxed)+1;
    char m[448]{}; std::snprintf(m,sizeof(m),
        "V66.12-DESIGN-CAPTURE record=%llu serial=%llu eye=%c list=%p source=%p cache=%p size=%llux%u fmt=%u inList=1 boundary=PIPELINE-RETURN sourceState=RT",
        static_cast<unsigned long long>(n),static_cast<unsigned long long>(serial),eye?'R':'L',
        static_cast<void*>(list),static_cast<void*>(source),static_cast<void*>(capture),
        static_cast<unsigned long long>(src_desc.Width),src_desc.Height,static_cast<unsigned>(src_desc.Format));
    diagnostic_trace::Write(m);
    return true;
}

bool DeviceHook::BridgeWaitEyeSubmitted(std::uint64_t serial, int eye_sign, std::uint32_t timeout_ms)
{
    if(serial==0 || (eye_sign!=-1 && eye_sign!=1)) return false;
    auto& submitted = eye_sign<0 ? s_bridge_left_submitted_serial : s_bridge_right_submitted_serial;
    auto& recorded = eye_sign<0 ? s_bridge_left_recorded_serial : s_bridge_right_recorded_serial;
    std::unique_lock<std::mutex> lock(s_bridge_capture_mutex);
    const auto pred=[&](){
        return recorded.load(std::memory_order_acquire)==serial &&
               submitted.load(std::memory_order_acquire)==serial;
    };
    if(pred()) return true;
    return g_bridge_cert_cv_v6550.wait_for(lock,std::chrono::milliseconds(timeout_ms),pred);
}

void DeviceHook::BridgeAbortSerial(std::uint64_t serial)
{
    if(serial==0) return;
    std::lock_guard<std::mutex> lock(s_bridge_capture_mutex);
    if(s_bridge_left_recorded_serial.load(std::memory_order_relaxed)==serial) s_bridge_left_recorded_serial.store(0,std::memory_order_release);
    if(s_bridge_right_recorded_serial.load(std::memory_order_relaxed)==serial) s_bridge_right_recorded_serial.store(0,std::memory_order_release);
    if(s_bridge_left_submitted_serial.load(std::memory_order_relaxed)==serial) s_bridge_left_submitted_serial.store(0,std::memory_order_release);
    if(s_bridge_right_submitted_serial.load(std::memory_order_relaxed)==serial) s_bridge_right_submitted_serial.store(0,std::memory_order_release);
    if(s_bridge_complete_pair_serial.load(std::memory_order_relaxed)==serial) s_bridge_complete_pair_serial.store(0,std::memory_order_release);
    for(auto& tag:g_bridge_list_tags_v6547) if(tag.serial==serial) tag={};
    g_bridge_tickets_v6547.erase(std::remove_if(g_bridge_tickets_v6547.begin(),g_bridge_tickets_v6547.end(),
        [serial](const BridgeTicketV6547& t){return t.serial==serial;}),g_bridge_tickets_v6547.end());
    for(auto& st:g_bridge_pair_states_v6547) if(st.serial==serial) st={};
    char cm[224]{}; std::snprintf(cm,sizeof(cm),"V65.47-OWNER ABORT serial=%llu",static_cast<unsigned long long>(serial)); debug_trace::Write(cm);
}

bool DeviceHook::TryGetBridgeCompletePair(ID3D12Resource** out_left, ID3D12Resource** out_right,
                                              std::uint64_t* out_serial, std::uint64_t* out_camera_frame,
                                              native_upscaler_stereo_d82::PairOwner* out_owner)
{
    if(!out_left || !out_right) return false;
    *out_left=nullptr; *out_right=nullptr;
    if(out_serial)*out_serial=0;
    if(out_owner)*out_owner={};
    if(out_camera_frame) *out_camera_frame=0;
    std::lock_guard<std::mutex> lock(s_bridge_capture_mutex);
    // V66.34: serial + pose token + textures are read together under the lock that the
    // publisher holds, so a pair can never be mixed with a newer pair's pose token.
    const auto serial=s_bridge_complete_pair_serial.load(std::memory_order_acquire);
    if(serial==0) return false;
    // The published snapshot is immutable until the next publish; the per-eye
    // "submitted" serials are NOT checked any more because the write slot for the
    // next pair legitimately advances them while this pair is still valid.
    if(!g_v6634_pub_left || !g_v6634_pub_right || g_v6634_pub_serial!=serial ||
       !native_upscaler_stereo_d82::MayPresent(g_v6634_pub_owner_d85)) return false;
    g_v6634_pub_left->AddRef(); g_v6634_pub_right->AddRef();
    *out_left=g_v6634_pub_left; *out_right=g_v6634_pub_right;
    g_v6634_xr_claim_left=g_v6634_pub_left; // released by ReleaseBridgePairClaim() after XR submits
    if(out_serial) *out_serial=serial;
    if(out_owner)*out_owner=g_v6634_pub_owner_d85;
    if(out_camera_frame) *out_camera_frame=s_bridge_complete_pair_camera_frame.load(std::memory_order_acquire);
    return true;
}

bool DeviceHook::ReadBridgePairIdentity59c6(std::uint64_t& serial, std::uint64_t& camera_frame)
{
    serial=0;camera_frame=0;
    std::unique_lock<std::mutex> lock(s_bridge_capture_mutex,std::try_to_lock);
    if(!lock.owns_lock())return false;
    const auto published=s_bridge_complete_pair_serial.load(std::memory_order_acquire);
    if(published&&g_v6634_pub_left&&g_v6634_pub_right&&g_v6634_pub_serial==published&&
       native_upscaler_stereo_d82::MayPresent(g_v6634_pub_owner_d85)){
        serial=published;camera_frame=s_bridge_complete_pair_camera_frame.load(std::memory_order_acquire);
    }
    return true;
}

void DeviceHook::MarkBridgePairConsumed(std::uint64_t serial)
{
    if(serial) s_bridge_last_consumed_pair_serial.store(serial,std::memory_order_release);
}

std::uint64_t DeviceHook_V6634LastConsumed() noexcept
{
    return DeviceHook::V6634LastConsumedPairSerial();
}

std::uint64_t DeviceHook::V6634LastConsumedPairSerial()
{
    return s_bridge_last_consumed_pair_serial.load(std::memory_order_acquire);
}

void DeviceHook::ReleaseBridgePairClaim()
{
    std::lock_guard<std::mutex> lock(s_bridge_capture_mutex);
    g_v6634_xr_claim_left=nullptr;
}

std::uint64_t DeviceHook::BridgeCompletePairCount()
{
    return s_bridge_complete_pair_count.load(std::memory_order_acquire);
}

void DeviceHook::NotifyV6636XrFrameOpportunity()
{
    g_v6636_xr_opportunity.fetch_add(1,std::memory_order_acq_rel);
}
bool DeviceHook::V6636CadenceOwner(){ return g_v6636_cadence_owner.load(std::memory_order_acquire); }
std::uint64_t DeviceHook::V6636XrFrameOpportunity(){ return g_v6636_xr_opportunity.load(std::memory_order_acquire); }

void DeviceHook::SetV6638PairPoseMode(int mode)
{
    mode=((mode%5)+5)%5;
    g_v6638_pair_pose_mode.store(mode,std::memory_order_release);
    char m[256]{}; std::snprintf(m,sizeof(m),
        "V66.38-PAIR-POSE-MODE mode=%d name=%s",mode,
        mode==0?"CURRENT-PUBLISH":mode==1?"LEFT-FOR-BOTH":mode==2?"RIGHT-FOR-BOTH":mode==3?"PER-EYE-L/R":"MIDPAIR-FOR-BOTH");
    diagnostic_trace::Write(m);
}
int DeviceHook::V6638PairPoseMode(){ return g_v6638_pair_pose_mode.load(std::memory_order_acquire); }
void DeviceHook::GetV6638LastPairPoseFrames(std::uint64_t& left,std::uint64_t& right,std::uint64_t& shared)
{
    left=g_v6638_left_pose_frame.load(std::memory_order_acquire);
    right=g_v6638_right_pose_frame.load(std::memory_order_acquire);
    shared=g_v6638_shared_pose_frame.load(std::memory_order_acquire);
}

void DeviceHook::SetV6627PerformanceQuiet(bool enabled)
{
    const bool old=s_v6627_performance_quiet.exchange(enabled,std::memory_order_acq_rel);
    if(old!=enabled) diagnostic_trace::Write(enabled
        ? "V66.27-PERF-QUIET ON broad postfx/native-mode/UI-descriptor diagnostics suppressed"
        : "V66.27-PERF-QUIET OFF V66.26 diagnostics restored");
}

bool DeviceHook::V6627PerformanceQuiet()
{
    return s_v6627_performance_quiet.load(std::memory_order_acquire);
}

// V66.19R1: non-blocking allocator ring for the live bridge L/R copies.
// Reusing one D3D12 command allocator forced a CPU wait between the LEFT and
// RIGHT copies. A ring lets each submitted copy keep its allocator until the
// GPU fence says it is safe to reset. If every slot is still in flight, fail
// this publication attempt instead of stalling KCD2's render thread; the next
// game frame rearms the live capture and retries.
namespace {
constexpr int kBridgeAllocRingV6619=8;
ID3D12CommandAllocator* g_bridge_alloc_ring_v6619[kBridgeAllocRingV6619]{};
std::uint64_t g_bridge_alloc_fence_v6619[kBridgeAllocRingV6619]{};
int g_bridge_alloc_next_v6619=0;
int g_bridge_alloc_current_v6619=-1;

ID3D12CommandAllocator* BridgeAcquireCopyAllocatorV6619(ID3D12Device* device,ID3D12Fence* fence) noexcept
{
    if(!device || !fence) return nullptr;
    const std::uint64_t completed=fence->GetCompletedValue();
    for(int probe=0;probe<kBridgeAllocRingV6619;++probe) {
        const int slot=(g_bridge_alloc_next_v6619+probe)%kBridgeAllocRingV6619;
        const std::uint64_t need=g_bridge_alloc_fence_v6619[slot];
        if(need!=0 && completed<need) continue;
        ID3D12CommandAllocator*& a=g_bridge_alloc_ring_v6619[slot];
        if(!a) {
            if(FAILED(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,IID_PPV_ARGS(&a)))) {
                a=nullptr;
                continue;
            }
        }
        if(FAILED(a->Reset())) {
            a->Release(); a=nullptr;
            g_bridge_alloc_fence_v6619[slot]=0;
            continue;
        }
        g_bridge_alloc_current_v6619=slot;
        g_bridge_alloc_next_v6619=(slot+1)%kBridgeAllocRingV6619;
        return a;
    }
    g_bridge_alloc_current_v6619=-1;
    return nullptr;
}

void BridgeMarkCopyAllocatorSubmittedV6619(std::uint64_t fence_value) noexcept
{
    if(g_bridge_alloc_current_v6619>=0) g_bridge_alloc_fence_v6619[g_bridge_alloc_current_v6619]=fence_value;
    g_bridge_alloc_current_v6619=-1;
}

void BridgeReleaseCopyAllocatorsV6619() noexcept
{
    for(int i=0;i<kBridgeAllocRingV6619;++i) {
        if(g_bridge_alloc_ring_v6619[i]) { g_bridge_alloc_ring_v6619[i]->Release(); g_bridge_alloc_ring_v6619[i]=nullptr; }
        g_bridge_alloc_fence_v6619[i]=0;
    }
    g_bridge_alloc_next_v6619=0;
    g_bridge_alloc_current_v6619=-1;
}
} // namespace

#include "temporal_capture_device_d85.inl"

bool DeviceHook::BridgeCaptureCurrentMainTargetToLeft(std::uint64_t submit_count_before_left,
    ID3D12Resource* expected_source,ID3D12Resource** out_left,
    const D3D12_RESOURCE_STATES* state_override,std::uint64_t submitted_source)
{
    std::lock_guard<std::mutex> lock(s_bridge_capture_mutex);
    if(out_left)*out_left=nullptr;
    if(expected_source&&s_bridge_current_main_target!=expected_source)return false;
    if (!s_bridge_current_main_target || !s_bridge_current_main_list) return false;
    const auto submitted=expected_source?submitted_source:s_bridge_current_main_last_submit.load(std::memory_order_acquire);
    // Fail closed: do not copy until the exact list carrying this backbuffer was
    // submitted after the caller's pre-left marker. This proves queue order.
    if (submitted <= submit_count_before_left) return false;

    const auto desc=s_bridge_current_main_target->GetDesc();
    if (desc.Dimension != D3D12_RESOURCE_DIMENSION_TEXTURE2D || desc.Width == 0 || desc.Height == 0 || desc.SampleDesc.Count != 1)
        return false;
    auto* device=s_captured_device.load(std::memory_order_acquire);
    auto* queue=s_captured_queue.load(std::memory_order_acquire);
    if(!device || !queue || !s_original_execute_command_lists) return false;

    if(!s_bridge_copy_allocator) {
        if(FAILED(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,IID_PPV_ARGS(&s_bridge_copy_allocator)))) return false;
    }
    if(!s_bridge_copy_list) {
        if(FAILED(device->CreateCommandList(0,D3D12_COMMAND_LIST_TYPE_DIRECT,s_bridge_copy_allocator,nullptr,IID_PPV_ARGS(&s_bridge_copy_list)))) return false;
        if(FAILED(s_bridge_copy_list->Close())) return false;
    }
    if(!s_bridge_copy_fence) {
        if(FAILED(device->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&s_bridge_copy_fence)))) return false;
        s_bridge_copy_fence_event=CreateEvent(nullptr,FALSE,FALSE,nullptr);
        if(!s_bridge_copy_fence_event) return false;
    }
    // V66.19R1: no global previous-copy wait. The allocator ring below scans for
    // an idle slot and fails this publication frame instead of blocking the render thread.

    const bool same_capture=s_bridge_left_capture &&
        s_bridge_left_desc.Dimension==desc.Dimension && s_bridge_left_desc.Width==desc.Width &&
        s_bridge_left_desc.Height==desc.Height && s_bridge_left_desc.Format==desc.Format &&
        s_bridge_left_desc.SampleDesc.Count==desc.SampleDesc.Count && s_bridge_left_desc.Flags==desc.Flags;
    if(!same_capture) {
        if(s_bridge_left_capture){s_bridge_left_capture->Release();s_bridge_left_capture=nullptr;}
        D3D12_HEAP_PROPERTIES heap{}; heap.Type=D3D12_HEAP_TYPE_DEFAULT; heap.CreationNodeMask=1; heap.VisibleNodeMask=1;
        if(FAILED(device->CreateCommittedResource(&heap,D3D12_HEAP_FLAG_NONE,&desc,D3D12_RESOURCE_STATE_COMMON,nullptr,IID_PPV_ARGS(&s_bridge_left_capture)))) return false;
        s_bridge_left_desc=desc;
        char cm[256]{}; std::snprintf(cm,sizeof(cm),"V65.38-BRIDGE-CAPTURE created left cache=%p size=%llux%u fmt=%u flags=0x%X",
            s_bridge_left_capture,static_cast<unsigned long long>(desc.Width),desc.Height,static_cast<unsigned>(desc.Format),static_cast<unsigned>(desc.Flags));
        debug_trace::Write(cm);
    }

    ID3D12CommandAllocator* ring_alloc=BridgeAcquireCopyAllocatorV6619(device,s_bridge_copy_fence);
    if(!ring_alloc) return false;
    // Same single command list as before (keeps the internal-list identity check
    // at the barrier hook valid). A list may be reset as soon as it was submitted;
    // only the allocator must be idle, which the ring guarantees.
    if(FAILED(s_bridge_copy_list->Reset(ring_alloc,nullptr))) return false;
    const auto source_state=state_override?*state_override:s_bridge_current_main_state;
    D3D12_RESOURCE_BARRIER begin[2]{}; UINT bc=0;
    if(source_state!=D3D12_RESOURCE_STATE_COPY_SOURCE){
        auto& x=begin[bc++];x.Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;x.Transition.pResource=s_bridge_current_main_target;
        x.Transition.Subresource=D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;x.Transition.StateBefore=source_state;x.Transition.StateAfter=D3D12_RESOURCE_STATE_COPY_SOURCE;
    }
    auto& d=begin[bc++];d.Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;d.Transition.pResource=s_bridge_left_capture;
    d.Transition.Subresource=D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;d.Transition.StateBefore=D3D12_RESOURCE_STATE_COMMON;d.Transition.StateAfter=D3D12_RESOURCE_STATE_COPY_DEST;
    s_bridge_copy_list->ResourceBarrier(bc,begin);
    s_bridge_copy_list->CopyResource(s_bridge_left_capture,s_bridge_current_main_target);
    D3D12_RESOURCE_BARRIER endb[2]{};UINT ec=0;
    auto& de=endb[ec++];de.Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;de.Transition.pResource=s_bridge_left_capture;
    de.Transition.Subresource=D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;de.Transition.StateBefore=D3D12_RESOURCE_STATE_COPY_DEST;de.Transition.StateAfter=D3D12_RESOURCE_STATE_COMMON;
    if(source_state!=D3D12_RESOURCE_STATE_COPY_SOURCE){
        auto& x=endb[ec++];x.Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;x.Transition.pResource=s_bridge_current_main_target;
        x.Transition.Subresource=D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;x.Transition.StateBefore=D3D12_RESOURCE_STATE_COPY_SOURCE;x.Transition.StateAfter=source_state;
    }
    s_bridge_copy_list->ResourceBarrier(ec,endb);
    if(FAILED(s_bridge_copy_list->Close())) return false;
    TemporalPinsD85* d85_left_pin=nullptr;
    if(expected_source){
        ReapTemporalPinsD85();
        for(auto& p:g_temporal_pins_d85)if(!p.left){d85_left_pin=&p;break;}
        if(!d85_left_pin)return false;
        d85_left_pin->left=s_bridge_left_capture;d85_left_pin->left->AddRef();
        d85_left_pin->fenceOwner=s_bridge_copy_fence;d85_left_pin->fenceOwner->AddRef();
        d85_left_pin->generation=g_temporal_owner_d85.Current().generation;
        d85_left_pin->fence=UINT64_MAX; // retained if Signal fails
    }
    ID3D12CommandList* lists[]={s_bridge_copy_list};
    s_original_execute_command_lists(queue,1,lists);
    ++s_bridge_copy_fence_value;
    // Mark the slot before checking Signal: if Signal failed, a later reuse of this
    // slot times out and fails closed instead of resetting an in-flight allocator.
    BridgeMarkCopyAllocatorSubmittedV6619(s_bridge_copy_fence_value);
    if(FAILED(queue->Signal(s_bridge_copy_fence,s_bridge_copy_fence_value))) return false;
    if(d85_left_pin)d85_left_pin->fence=s_bridge_copy_fence_value;
    const auto n=s_bridge_copy_count.fetch_add(1,std::memory_order_relaxed)+1;
    if(n<=8 || (n%120ull)==0){char cm[352]{};std::snprintf(cm,sizeof(cm),
        "V65.38-BRIDGE-CAPTURE copy=%llu submitBefore=%llu leftSubmit=%llu source=%p leftCache=%p state=0x%X queue=%p",
        static_cast<unsigned long long>(n),static_cast<unsigned long long>(submit_count_before_left),static_cast<unsigned long long>(submitted),
        s_bridge_current_main_target,s_bridge_left_capture,static_cast<unsigned>(source_state),queue);debug_trace::Write(cm);}
    if(out_left)*out_left=s_bridge_left_capture;
    return true;
}

bool DeviceHook::BridgeCaptureCurrentMainTargetToRight(std::uint64_t submit_count_before_right)
{
    std::lock_guard<std::mutex> lock(s_bridge_capture_mutex);
    if (!s_bridge_current_main_target || !s_bridge_current_main_list) return false;
    const auto submitted=s_bridge_current_main_last_submit.load(std::memory_order_acquire);
    // Fail closed: do not copy until the exact list carrying this backbuffer was
    // submitted after the caller's pre-left marker. This proves queue order.
    if (submitted <= submit_count_before_right) return false;

    const auto desc=s_bridge_current_main_target->GetDesc();
    if (desc.Dimension != D3D12_RESOURCE_DIMENSION_TEXTURE2D || desc.Width == 0 || desc.Height == 0 || desc.SampleDesc.Count != 1)
        return false;
    auto* device=s_captured_device.load(std::memory_order_acquire);
    auto* queue=s_captured_queue.load(std::memory_order_acquire);
    if(!device || !queue || !s_original_execute_command_lists) return false;

    if(!s_bridge_copy_allocator) {
        if(FAILED(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,IID_PPV_ARGS(&s_bridge_copy_allocator)))) return false;
    }
    if(!s_bridge_copy_list) {
        if(FAILED(device->CreateCommandList(0,D3D12_COMMAND_LIST_TYPE_DIRECT,s_bridge_copy_allocator,nullptr,IID_PPV_ARGS(&s_bridge_copy_list)))) return false;
        if(FAILED(s_bridge_copy_list->Close())) return false;
    }
    if(!s_bridge_copy_fence) {
        if(FAILED(device->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&s_bridge_copy_fence)))) return false;
        s_bridge_copy_fence_event=CreateEvent(nullptr,FALSE,FALSE,nullptr);
        if(!s_bridge_copy_fence_event) return false;
    }
    // V66.19R1: no global previous-copy wait. The allocator ring below scans for
    // an idle slot and fails this publication frame instead of blocking the render thread.

    const bool same_capture=s_bridge_right_capture &&
        s_bridge_right_desc.Dimension==desc.Dimension && s_bridge_right_desc.Width==desc.Width &&
        s_bridge_right_desc.Height==desc.Height && s_bridge_right_desc.Format==desc.Format &&
        s_bridge_right_desc.SampleDesc.Count==desc.SampleDesc.Count && s_bridge_right_desc.Flags==desc.Flags;
    if(!same_capture) {
        if(s_bridge_right_capture){s_bridge_right_capture->Release();s_bridge_right_capture=nullptr;}
        D3D12_HEAP_PROPERTIES heap{}; heap.Type=D3D12_HEAP_TYPE_DEFAULT; heap.CreationNodeMask=1; heap.VisibleNodeMask=1;
        if(FAILED(device->CreateCommittedResource(&heap,D3D12_HEAP_FLAG_NONE,&desc,D3D12_RESOURCE_STATE_COMMON,nullptr,IID_PPV_ARGS(&s_bridge_right_capture)))) return false;
        s_bridge_right_desc=desc;
        char cm[256]{}; std::snprintf(cm,sizeof(cm),"V65.41-BRIDGE-RIGHT-CAPTURE created right cache=%p size=%llux%u fmt=%u flags=0x%X",
            s_bridge_right_capture,static_cast<unsigned long long>(desc.Width),desc.Height,static_cast<unsigned>(desc.Format),static_cast<unsigned>(desc.Flags));
        debug_trace::Write(cm);
    }

    ID3D12CommandAllocator* ring_alloc=BridgeAcquireCopyAllocatorV6619(device,s_bridge_copy_fence);
    if(!ring_alloc) return false;
    // Same single command list as before (keeps the internal-list identity check
    // at the barrier hook valid). A list may be reset as soon as it was submitted;
    // only the allocator must be idle, which the ring guarantees.
    if(FAILED(s_bridge_copy_list->Reset(ring_alloc,nullptr))) return false;
    const auto source_state=s_bridge_current_main_state;
    D3D12_RESOURCE_BARRIER begin[2]{}; UINT bc=0;
    if(source_state!=D3D12_RESOURCE_STATE_COPY_SOURCE){
        auto& x=begin[bc++];x.Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;x.Transition.pResource=s_bridge_current_main_target;
        x.Transition.Subresource=D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;x.Transition.StateBefore=source_state;x.Transition.StateAfter=D3D12_RESOURCE_STATE_COPY_SOURCE;
    }
    auto& d=begin[bc++];d.Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;d.Transition.pResource=s_bridge_right_capture;
    d.Transition.Subresource=D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;d.Transition.StateBefore=D3D12_RESOURCE_STATE_COMMON;d.Transition.StateAfter=D3D12_RESOURCE_STATE_COPY_DEST;
    s_bridge_copy_list->ResourceBarrier(bc,begin);
    s_bridge_copy_list->CopyResource(s_bridge_right_capture,s_bridge_current_main_target);
    D3D12_RESOURCE_BARRIER endb[2]{};UINT ec=0;
    auto& de=endb[ec++];de.Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;de.Transition.pResource=s_bridge_right_capture;
    de.Transition.Subresource=D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;de.Transition.StateBefore=D3D12_RESOURCE_STATE_COPY_DEST;de.Transition.StateAfter=D3D12_RESOURCE_STATE_COMMON;
    if(source_state!=D3D12_RESOURCE_STATE_COPY_SOURCE){
        auto& x=endb[ec++];x.Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;x.Transition.pResource=s_bridge_current_main_target;
        x.Transition.Subresource=D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;x.Transition.StateBefore=D3D12_RESOURCE_STATE_COPY_SOURCE;x.Transition.StateAfter=source_state;
    }
    s_bridge_copy_list->ResourceBarrier(ec,endb);
    if(FAILED(s_bridge_copy_list->Close())) return false;
    ID3D12CommandList* lists[]={s_bridge_copy_list};
    s_original_execute_command_lists(queue,1,lists);
    ++s_bridge_copy_fence_value;
    // Mark the slot before checking Signal: if Signal failed, a later reuse of this
    // slot times out and fails closed instead of resetting an in-flight allocator.
    BridgeMarkCopyAllocatorSubmittedV6619(s_bridge_copy_fence_value);
    if(FAILED(queue->Signal(s_bridge_copy_fence,s_bridge_copy_fence_value))) return false;
    const auto n=s_bridge_copy_count.fetch_add(1,std::memory_order_relaxed)+1;
    if(n<=8 || (n%120ull)==0){char cm[352]{};std::snprintf(cm,sizeof(cm),
        "V65.41-BRIDGE-RIGHT-CAPTURE copy=%llu submitBefore=%llu rightSubmit=%llu source=%p rightCache=%p state=0x%X queue=%p",
        static_cast<unsigned long long>(n),static_cast<unsigned long long>(submit_count_before_right),static_cast<unsigned long long>(submitted),
        s_bridge_current_main_target,s_bridge_right_capture,static_cast<unsigned>(source_state),queue);debug_trace::Write(cm);}
    return true;
}



void DeviceHook::ResetBridgeCapture()
{
    std::lock_guard<std::recursive_mutex> transaction(g_temporal_mutex_d85);
    g_temporal_owner_d85.Cancel(); // recorded copies keep their independent reference pins
    std::lock_guard<std::mutex> lock(s_bridge_capture_mutex);
    if(s_bridge_current_main_target){ s_bridge_current_main_target->Release(); s_bridge_current_main_target=nullptr; }
    if(s_bridge_current_main_list){ s_bridge_current_main_list->Release(); s_bridge_current_main_list=nullptr; }
    s_bridge_current_main_state=D3D12_RESOURCE_STATE_COMMON;
    s_bridge_current_main_tid=0;
    if(s_bridge_left_capture){ s_bridge_left_capture->Release(); s_bridge_left_capture=nullptr; }
    s_bridge_left_desc={};
    if(s_bridge_right_capture){ s_bridge_right_capture->Release(); s_bridge_right_capture=nullptr; }
    s_bridge_right_desc={};
    if(g_v6634_spare_left){ g_v6634_spare_left->Release(); g_v6634_spare_left=nullptr; }
    if(g_v6634_spare_right){ g_v6634_spare_right->Release(); g_v6634_spare_right=nullptr; }
    g_v6634_spare_left_desc={}; g_v6634_spare_right_desc={};
    if(g_v6634_pub_left){ g_v6634_pub_left->Release(); g_v6634_pub_left=nullptr; }
    if(g_v6634_pub_right){ g_v6634_pub_right->Release(); g_v6634_pub_right=nullptr; }
    g_v6634_pub_serial=0; g_v6634_pub_owner_d85={}; g_v6634_xr_claim_left=nullptr;
    if(s_bridge_copy_list){ s_bridge_copy_list->Release(); s_bridge_copy_list=nullptr; }
    if(s_bridge_copy_allocator){ s_bridge_copy_allocator->Release(); s_bridge_copy_allocator=nullptr; }
    if(s_bridge_copy_fence){ s_bridge_copy_fence->Release(); s_bridge_copy_fence=nullptr; }
    if(s_bridge_copy_fence_event){ CloseHandle(s_bridge_copy_fence_event); s_bridge_copy_fence_event=nullptr; }
    s_bridge_copy_fence_value=0;
    BridgeReleaseCopyAllocatorsV6619(); // V66.19: new fence restarts at 0, so ring fence values must reset too
    s_bridge_current_main_last_submit.store(0,std::memory_order_release);
    s_bridge_record_serial.store(0,std::memory_order_release);
    s_bridge_record_eye.store(-1,std::memory_order_release);
    s_bridge_left_recorded_serial.store(0,std::memory_order_release);
    s_bridge_right_recorded_serial.store(0,std::memory_order_release);
    s_bridge_left_submitted_serial.store(0,std::memory_order_release);
    s_bridge_right_submitted_serial.store(0,std::memory_order_release);
    s_bridge_complete_pair_serial.store(0,std::memory_order_release);
    s_bridge_complete_pair_camera_frame.store(0,std::memory_order_release);
    s_bridge_complete_pair_count.store(0,std::memory_order_release);
    s_bridge_last_consumed_pair_serial.store(0,std::memory_order_release);
    for(auto& tag:g_bridge_list_tags_v6547) tag={};
    g_bridge_list_tag_cursor_v6547=0;
    g_bridge_tickets_v6547.clear();
    for(auto& st:g_bridge_pair_states_v6547) st={};
    g_bridge_pair_state_cursor_v6547=0;
    g_bridge_owner_complete_v6547.store(0,std::memory_order_release);
    g_bridge_ticket_miss_v6547.store(0,std::memory_order_release);
}

void DeviceHook::PresentFormatChangesR111(std::uint64_t* changes,std::uint32_t* last,std::uint32_t* previous)
{
    if(changes)*changes=g_present_fmt_changes_r111.load(std::memory_order_relaxed);
    if(last)*last=g_present_fmt_last_r111.load(std::memory_order_acquire);
    if(previous)*previous=g_present_fmt_prev_r111.load(std::memory_order_acquire);
}

bool DeviceHook::TryGetLatestPresentResource(ID3D12Resource** out_resource)
{
    if (!out_resource) return false;
    *out_resource = nullptr;
    std::lock_guard<std::mutex> lock(s_present_resource_mutex);
    if (!s_latest_present_resource) return false;
    s_latest_present_resource->AddRef();
    *out_resource = s_latest_present_resource;
    return true;
}

bool DeviceHook::TryGetLatestPresentResourceTagged(ID3D12Resource** out_resource, int* out_eye, std::uint64_t* out_serial, std::uint64_t* out_camera_frame)
{
    if (!out_resource) return false;
    *out_resource = nullptr;
    std::lock_guard<std::mutex> lock(s_present_resource_mutex);
    if (!s_latest_present_resource) return false;
    s_latest_present_resource->AddRef();
    *out_resource = s_latest_present_resource;
    if (out_eye) *out_eye = s_latest_present_eye;
    if (out_serial) *out_serial = s_latest_present_serial;
    if (out_camera_frame) *out_camera_frame = s_latest_present_camera_frame;
    return true;
}

bool DeviceHook::TryGetLatestPresentResourceIdentity(ID3D12Resource** out_resource, std::uint64_t* out_serial, std::uint64_t* out_tick_ms, std::uint64_t* out_camera_frame)
{
    if (!out_resource) return false;
    *out_resource = nullptr;
    std::lock_guard<std::mutex> lock(s_present_resource_mutex);
    if (!s_latest_present_resource) return false;
    s_latest_present_resource->AddRef();
    *out_resource = s_latest_present_resource;
    if (out_serial) *out_serial = s_latest_present_serial;
    if (out_tick_ms) *out_tick_ms = s_latest_present_tick_ms;
    if (out_camera_frame) *out_camera_frame = s_latest_present_camera_frame;
    return true;
}

bool DeviceHook::GetLatestPresentIdentity(std::uint64_t* out_serial, std::uint64_t* out_tick_ms, std::uint64_t* out_width, std::uint32_t* out_height, std::uint64_t* out_camera_frame)
{
    std::lock_guard<std::mutex> lock(s_present_resource_mutex);
    if (!s_latest_present_resource) return false;
    const auto d = s_latest_present_resource->GetDesc();
    if (out_serial) *out_serial = s_latest_present_serial;
    if (out_tick_ms) *out_tick_ms = s_latest_present_tick_ms;
    if (out_width) *out_width = d.Width;
    if (out_height) *out_height = d.Height;
    if (out_camera_frame) *out_camera_frame = s_latest_present_camera_frame;
    return true;
}

void DeviceHook::SetAfrStereoCapture(bool enabled)
{
    s_afr_stereo_capture.store(enabled, std::memory_order_release);
    CameraHook::SetAfrStereoEnabled(enabled);
    CameraHook::SetAfrEye(0);
}

bool DeviceHook::TryGetDevice(ID3D12Device** out_device, ID3D12CommandQueue** out_queue)
{
    if (!s_captured.load(std::memory_order_acquire))
        return false;
    *out_device = s_captured_device.load(std::memory_order_relaxed);
    *out_queue = s_captured_queue.load(std::memory_order_relaxed);
    return *out_device != nullptr && *out_queue != nullptr;
}


bool DeviceHook::ModePsoProvenanceProbeEnabled(){ return s_mode_pso_probe_enabled.load(std::memory_order_acquire); }

void DeviceHook::NotifyModeShaderHashLearned(int lane, std::uint64_t hash)
{
    if((lane!=1 && lane!=2) || !hash) return;

    static const char* sn[5]={"VS","PS","GS","HS","DS"};
    std::size_t pso_matches=0;
    std::size_t list_updates=0;
    {
        std::lock_guard<std::mutex> lock(s_mode_pso_mutex);
        for(const auto& kv:s_pso_shader_hashes){
            auto* pso=kv.first;
            const auto& rec=kv.second;
            int stage=0;
            for(int i=0;i<5;++i){ if(rec.h[i]==hash){ stage=i+1; break; } }
            if(!stage) continue;
            ++pso_matches;
            for(auto& lv:s_mode_list_state){
                auto& st=lv.second;
                if(st.pso!=pso) continue;
                st.shader_hash=hash;
                st.lane=lane;
                st.shader_stage=stage;
                ++list_updates;
            }
            if(s_mode_pso_probe_enabled.load(std::memory_order_relaxed)){
                char m[384]{};
                std::snprintf(m,sizeof(m),"V65.82-F5-PSO RETRO-MATCH lane=MODE%d stage=%s pso=%p shaderHash=%016llX",lane==1?0:2,sn[stage-1],pso,(unsigned long long)hash);
                diagnostic_trace::Write(m);
            }
        }
    }
    if(s_mode_pso_probe_enabled.load(std::memory_order_relaxed)){
        char m[320]{};
        std::snprintf(m,sizeof(m),"V65.82-F5-PSO RETRO-SUMMARY lane=MODE%d shaderHash=%016llX psoMatches=%zu listUpdates=%zu",lane==1?0:2,(unsigned long long)hash,pso_matches,list_updates);
        diagnostic_trace::Write(m);
    }
}

bool DeviceHook::InstallQueueHook(ID3D12Device* device)
{
    if (s_queue_hook_installed.load(std::memory_order_acquire))
        return true; // already installed

    void** vtable = *reinterpret_cast<void***>(device);
    // ID3D12Device vtable layout (stable, published):
    // 0-2 IUnknown, 3-6 ID3D12Object, 7 GetNodeCount, 8 CreateCommandQueue.
    constexpr size_t kCreateCommandQueueIndex = 8;
    void* target_address = vtable[kCreateCommandQueueIndex];

    KCD2VR_DIAG_LOG(Bug,info,"DeviceHook: ID3D12Device::CreateCommandQueue vtable address = {:#x}",
               reinterpret_cast<uintptr_t>(target_address));

    hook::InlineRequest request{
        "kcd2vr_device_hook_create_queue",
        hook::Target{DetourModKit::Address{reinterpret_cast<uintptr_t>(target_address)}},
        hook::Options{}
    };

    DetourModKit::Result<hook::Hook> result = hook::inline_at(std::move(request), &DeviceHook::DetourCreateCommandQueue);
    if (!result)
    {
        KCD2VR_DIAG_LOG(Bug,error,"DeviceHook: CreateCommandQueue inline_at failed");
        return false;
    }

    s_hook_create_queue = std::move(*result);
    s_original_create_queue = s_hook_create_queue->original<CreateCommandQueueFunc>();

    DetourModKit::Result<void> enable_result = s_hook_create_queue->enable();
    if (!enable_result)
    {
        KCD2VR_DIAG_LOG(Bug,error,"DeviceHook: CreateCommandQueue hook failed to enable");
        s_original_create_queue = nullptr;
        s_hook_create_queue.reset();
        return false;
    }

    s_queue_hook_installed.store(true, std::memory_order_release);
    KCD2VR_DIAG_LOG(Bug,info,"DeviceHook: CreateCommandQueue hook installed and enabled");
    return true;
}

HRESULT WINAPI DeviceHook::DetourD3D12CreateDevice(
    IUnknown* adapter, D3D_FEATURE_LEVEL minimum_feature_level, REFIID riid, void** out_device)
{
    HRESULT hr = s_original_create_device
        ? s_original_create_device(adapter, minimum_feature_level, riid, out_device)
        : E_FAIL;

    if (SUCCEEDED(hr) && out_device && *out_device && !s_queue_hook_installed.load(std::memory_order_acquire))
    {
        ID3D12Device* device = static_cast<ID3D12Device*>(*out_device);
        V6600GpuInstall(device);
        InstallQueueHook(device);
        InstallCreateGraphicsPipelineStateHook(device);
        InstallCreateCommandListHook(device);
        InstallCreateRenderTargetViewHook(device);
        InstallCreateDepthStencilViewHook(device);
        InstallCreateDescriptorHeapHook(device);
        InstallCreateShaderResourceViewHook(device);
        InstallCopyDescriptorHooks(device);
    }

    return hr;
}


static std::uint64_t V6581HashShaderBytecode(const D3D12_SHADER_BYTECODE& bc) noexcept
{
    if(!bc.pShaderBytecode || bc.BytecodeLength<32 || bc.BytecodeLength>(16ull*1024ull*1024ull)) return 0;
    const auto* b=static_cast<const std::uint8_t*>(bc.pShaderBytecode);
    std::uint64_t h=1469598103934665603ull;
    __try { for(SIZE_T i=0;i<bc.BytecodeLength;++i){ h^=b[i]; h*=1099511628211ull; } }
    __except(EXCEPTION_EXECUTE_HANDLER) { return 0; }
    return h;
}

bool DeviceHook::InstallCreateGraphicsPipelineStateHook(ID3D12Device* device)
{
    if(!device) return false;
    if(s_create_graphics_pso_hook_installed.load(std::memory_order_acquire)) return true;
    void** vtable=*reinterpret_cast<void***>(device);
    constexpr size_t kCreateGraphicsPipelineStateIndex=10;
    hook::InlineRequest req{"kcd2vr_v6581_create_graphics_pso",hook::Target{DetourModKit::Address{reinterpret_cast<uintptr_t>(vtable[kCreateGraphicsPipelineStateIndex])}},hook::Options{}};
    auto r=hook::inline_at(std::move(req),&DeviceHook::DetourCreateGraphicsPipelineState);
    if(!r){ KCD2VR_DIAG_LOG(Bug,warning,"V65.81-F5-PSO CreateGraphicsPipelineState hook install failed"); return false; }
    s_hook_create_graphics_pipeline_state=std::move(*r);
    s_original_create_graphics_pipeline_state=s_hook_create_graphics_pipeline_state->original<CreateGraphicsPipelineStateFunc>();
    if(!s_hook_create_graphics_pipeline_state->enable()) { s_original_create_graphics_pipeline_state=nullptr; s_hook_create_graphics_pipeline_state.reset(); return false; }
    s_create_graphics_pso_hook_installed.store(true,std::memory_order_release);
    diagnostic_trace::Write("V65.81-F5-PSO CreateGraphicsPipelineState observer installed; hashes recorded continuously, logs gated by F5");
    return true;
}

HRESULT __stdcall DeviceHook::DetourCreateGraphicsPipelineState(
    ID3D12Device* device_this,const D3D12_GRAPHICS_PIPELINE_STATE_DESC* desc,REFIID riid,void** out_pso)
{
    PsoShaderRecord rec{};
    if(desc){ rec.h[0]=V6581HashShaderBytecode(desc->VS); rec.h[1]=V6581HashShaderBytecode(desc->PS); rec.h[2]=V6581HashShaderBytecode(desc->GS); rec.h[3]=V6581HashShaderBytecode(desc->HS); rec.h[4]=V6581HashShaderBytecode(desc->DS); }
    HRESULT hr=s_original_create_graphics_pipeline_state?s_original_create_graphics_pipeline_state(device_this,desc,riid,out_pso):E_FAIL;
    if(SUCCEEDED(hr) && out_pso && *out_pso){
        auto* pso=static_cast<ID3D12PipelineState*>(*out_pso);
        V6600LegacyPsoCreated(pso);
        { std::lock_guard<std::mutex> lock(s_mode_pso_mutex); if(s_pso_shader_hashes.size()<16384) s_pso_shader_hashes[pso]=rec; }
        if(s_mode_pso_probe_enabled.load(std::memory_order_relaxed)){
            static const char* sn[5]={"VS","PS","GS","HS","DS"};
            for(int i=0;i<5;++i){ const int lane=native_stereo_outer_lab::ClassifyModeShaderHash(rec.h[i]); if(lane){ char m[384]{}; std::snprintf(m,sizeof(m),"V65.81-F5-PSO CREATE lane=MODE%d stage=%s pso=%p shaderHash=%016llX numRT=%u rtv0fmt=%u dsvfmt=%u",lane==1?0:2,sn[i],pso,(unsigned long long)rec.h[i],desc?desc->NumRenderTargets:0,desc?(unsigned)desc->RTVFormats[0]:0,desc?(unsigned)desc->DSVFormat:0); diagnostic_trace::Write(m); } }
        }
    }
    return hr;
}

bool DeviceHook::InstallSetPipelineStateHook(ID3D12GraphicsCommandList* list)
{
    if(!list) return false;
    bool expected=false;
    if(!s_set_pipeline_state_hook_installed.compare_exchange_strong(expected,true,std::memory_order_acq_rel,std::memory_order_acquire)) return true;
    void** vtable=*reinterpret_cast<void***>(list);
    constexpr size_t kSetPipelineStateIndex=25;
    hook::InlineRequest req{"kcd2vr_v6581_set_pipeline_state",hook::Target{DetourModKit::Address{reinterpret_cast<uintptr_t>(vtable[kSetPipelineStateIndex])}},hook::Options{}};
    auto r=hook::inline_at(std::move(req),&DeviceHook::DetourSetPipelineState);
    if(!r){ s_set_pipeline_state_hook_installed.store(false,std::memory_order_release); KCD2VR_DIAG_LOG(Bug,warning,"V65.81-F5-PSO SetPipelineState hook install failed"); return false; }
    s_hook_set_pipeline_state=std::move(*r); s_original_set_pipeline_state=s_hook_set_pipeline_state->original<SetPipelineStateFunc>();
    if(!s_hook_set_pipeline_state->enable()){ s_original_set_pipeline_state=nullptr; s_hook_set_pipeline_state.reset(); s_set_pipeline_state_hook_installed.store(false,std::memory_order_release); return false; }
    diagnostic_trace::Write("V65.81-F5-PSO SetPipelineState observer installed");
    return true;
}

void __stdcall DeviceHook::DetourSetPipelineState(ID3D12GraphicsCommandList* list_this,ID3D12PipelineState* pso)
{
    if(s_v6627_performance_quiet.load(std::memory_order_relaxed)){
        if(s_original_set_pipeline_state) s_original_set_pipeline_state(list_this,pso);
        return;
    }
    if((s_mode_pso_probe_enabled.load(std::memory_order_relaxed) || g_v6610_output_enabled.load(std::memory_order_acquire)) && list_this){
        std::lock_guard<std::mutex> lock(s_mode_pso_mutex);
        auto& st=s_mode_list_state[list_this]; st.pso=pso; st.shader_hash=0; st.lane=0; st.shader_stage=0;
        auto it=s_pso_shader_hashes.find(pso); if(it!=s_pso_shader_hashes.end()){ for(int i=0;i<5;++i){ const int lane=native_stereo_outer_lab::ClassifyModeShaderHash(it->second.h[i]); if(lane){ st.shader_hash=it->second.h[i]; st.lane=lane; st.shader_stage=i+1; break; } } }
    }
    if(s_original_set_pipeline_state) s_original_set_pipeline_state(list_this,pso);
}

bool DeviceHook::InstallCreateCommandListHook(ID3D12Device* device)
{
    if (!device) return false;
    if (s_command_list_hook_installed.load(std::memory_order_acquire)) return true;

    void** vtable = *reinterpret_cast<void***>(device);
    // ID3D12Device::CreateCommandList is published vtable slot 12.
    constexpr size_t kCreateCommandListIndex = 12;
    void* target_address = vtable[kCreateCommandListIndex];

    hook::InlineRequest request{
        "kcd2vr_device_hook_create_command_list_v37",
        hook::Target{DetourModKit::Address{reinterpret_cast<uintptr_t>(target_address)}},
        hook::Options{}
    };
    auto result = hook::inline_at(std::move(request), &DeviceHook::DetourCreateCommandList);
    if (!result) { KCD2VR_DIAG_LOG(Bug,error,"V37-BRIDGE: CreateCommandList hook install failed"); return false; }
    s_hook_create_command_list = std::move(*result);
    s_original_create_command_list = s_hook_create_command_list->original<CreateCommandListFunc>();
    if (!s_hook_create_command_list->enable()) {
        s_original_create_command_list = nullptr;
        s_hook_create_command_list.reset();
        KCD2VR_DIAG_LOG(Bug,error,"V37-BRIDGE: CreateCommandList hook enable failed");
        return false;
    }
    s_command_list_hook_installed.store(true, std::memory_order_release);
    KCD2VR_DIAG_LOG(Bug,info,"V37-BRIDGE: CreateCommandList hook enabled");
    return true;
}

HRESULT __stdcall DeviceHook::DetourCreateCommandList(
    ID3D12Device* device_this, UINT node_mask, D3D12_COMMAND_LIST_TYPE type,
    ID3D12CommandAllocator* allocator, ID3D12PipelineState* initial_state, REFIID riid, void** out_list)
{
    HRESULT hr = s_original_create_command_list
        ? s_original_create_command_list(device_this, node_mask, type, allocator, initial_state, riid, out_list)
        : E_FAIL;
    if(SUCCEEDED(hr)&&out_list&&*out_list&&type==D3D12_COMMAND_LIST_TYPE_DIRECT){
        std::lock_guard<std::recursive_mutex> transaction(g_temporal_mutex_d85);
        ResetTemporalListD85(static_cast<ID3D12GraphicsCommandList*>(*out_list),true);
    }
    if (SUCCEEDED(hr) && out_list && *out_list && type == D3D12_COMMAND_LIST_TYPE_DIRECT &&
        !s_resource_barrier_hook_installed.load(std::memory_order_acquire))
    {
        // D56R1: concurrent CreateCommandList calls can both pass the outer
        // check. The ResourceBarrier CAS protects only that one hook, not the
        // following shared optional hooks. Serialize the entire startup chain
        // and recheck after waiting; steady-state draw/copy paths take no lock.
        static std::mutex install_mutex;
        std::lock_guard<std::mutex> install_lock(install_mutex);
        if (!s_resource_barrier_hook_installed.load(std::memory_order_acquire)) {
            auto* list = static_cast<ID3D12GraphicsCommandList*>(*out_list);
            // Do not partially install the tail when the primary hook fails:
            // a later retry must not replace a live viewport/scissor hook.
            if (InstallResourceBarrierHook(list)) {
                InstallOMSetRenderTargetsHook(list);
                InstallOutputLayoutHooks(list);
                InstallSetPipelineStateHook(list);
                InstallGraphicsRootDescriptorTableHook(list);
                InstallDrawHooks(list); // V65.81/V66.25: draw hooks feed PSO + late UI source tracing.
                InstallCommandListResetHookD82(list);
            }
        }
    }
    if(SUCCEEDED(hr) && out_list && *out_list && type==D3D12_COMMAND_LIST_TYPE_DIRECT && initial_state){
        auto* list=static_cast<ID3D12GraphicsCommandList*>(*out_list);
        std::lock_guard<std::mutex> lock(s_mode_pso_mutex); auto& st=s_mode_list_state[list]; st.pso=initial_state; auto it=s_pso_shader_hashes.find(initial_state); if(it!=s_pso_shader_hashes.end()){ for(int i=0;i<5;++i){ const int lane=native_stereo_outer_lab::ClassifyModeShaderHash(it->second.h[i]); if(lane){st.shader_hash=it->second.h[i];st.lane=lane;st.shader_stage=i+1;break;} } }
    }
    return hr;
}

bool DeviceHook::InstallResourceBarrierHook(ID3D12GraphicsCommandList* list)
{
    if (!list) return false;

    // V40.1: CreateCommandList can be called concurrently on multiple threads.
    // V40 only checked this flag and set it after the hook was enabled, so two
    // callers could both install a DetourModKit hook on the same COM vtable
    // target. Reserve ownership atomically before calling inline_at().
    bool expected = false;
    if (!s_resource_barrier_hook_installed.compare_exchange_strong(
            expected, true, std::memory_order_acq_rel, std::memory_order_acquire))
        return true;

    void** vtable = *reinterpret_cast<void***>(list);
    // ID3D12GraphicsCommandList::ResourceBarrier is slot 26.
    constexpr size_t kResourceBarrierIndex = 26;
    void* target_address = vtable[kResourceBarrierIndex];
    hook::InlineRequest request{
        "kcd2vr_resource_barrier_present_capture_v37",
        hook::Target{DetourModKit::Address{reinterpret_cast<uintptr_t>(target_address)}},
        hook::Options{}
    };
    auto result = hook::inline_at(std::move(request), &DeviceHook::DetourResourceBarrier);
    if (!result) {
        s_resource_barrier_hook_installed.store(false, std::memory_order_release);
        KCD2VR_DIAG_LOG(Bug,error,"V40.1-BRIDGE: ResourceBarrier hook install failed");
        return false;
    }
    s_hook_resource_barrier = std::move(*result);
    s_original_resource_barrier = s_hook_resource_barrier->original<ResourceBarrierFunc>();
    if (!s_hook_resource_barrier->enable()) {
        s_original_resource_barrier = nullptr;
        s_hook_resource_barrier.reset();
        s_resource_barrier_hook_installed.store(false, std::memory_order_release);
        KCD2VR_DIAG_LOG(Bug,error,"V40.1-BRIDGE: ResourceBarrier hook enable failed");
        return false;
    }
    KCD2VR_DIAG_LOG(Bug,info,"V40.1-BRIDGE: ResourceBarrier PRESENT capture hook enabled (single-owner CAS)");
    return true;
}

bool DeviceHook::InstallCreateRenderTargetViewHook(ID3D12Device* device)
{
    if (!device || s_create_rtv_hook_installed.load(std::memory_order_acquire)) return true;
    void** vtable = *reinterpret_cast<void***>(device);
    constexpr size_t kCreateRenderTargetViewIndex = 20;
    void* target = vtable[kCreateRenderTargetViewIndex];
    hook::InlineRequest req{"kcd2vr_v616_create_rtv_probe", hook::Target{DetourModKit::Address{reinterpret_cast<uintptr_t>(target)}}, hook::Options{}};
    auto result = hook::inline_at(std::move(req), &DeviceHook::DetourCreateRenderTargetView);
    if (!result) { KCD2VR_DIAG_LOG(Bug,warning,"V61.6-NATIVE-RT: CreateRenderTargetView hook install failed"); return false; }
    s_hook_create_rtv = std::move(*result);
    s_original_create_rtv = s_hook_create_rtv->original<CreateRenderTargetViewFunc>();
    if (!s_hook_create_rtv->enable()) { s_original_create_rtv=nullptr; s_hook_create_rtv.reset(); return false; }
    s_rtv_descriptor_increment.store(device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV), std::memory_order_release);
    s_create_rtv_hook_installed.store(true, std::memory_order_release);
    debug_trace::Write("V61.6-NATIVE-RT CreateRenderTargetView observer armed; descriptor->resource map is read-only");
    return true;
}



bool DeviceHook::InstallCreateDepthStencilViewHook(ID3D12Device* device)
{
    if (!device || s_create_dsv_hook_installed.load(std::memory_order_acquire)) return true;
    void** vtable = *reinterpret_cast<void***>(device);
    // ID3D12Device::CreateDepthStencilView is vtable slot 21.
    constexpr size_t kCreateDepthStencilViewIndex = 21;
    void* target = vtable[kCreateDepthStencilViewIndex];
    hook::InlineRequest req{"kcd2vr_v6134_create_dsv_probe", hook::Target{DetourModKit::Address{reinterpret_cast<uintptr_t>(target)}}, hook::Options{}};
    auto result = hook::inline_at(std::move(req), &DeviceHook::DetourCreateDepthStencilView);
    if (!result) { KCD2VR_DIAG_LOG(Bug,warning,"V61.34-DEPTH: CreateDepthStencilView hook install failed"); return false; }
    s_hook_create_dsv = std::move(*result);
    s_original_create_dsv = s_hook_create_dsv->original<CreateDepthStencilViewFunc>();
    if (!s_hook_create_dsv->enable()) { s_original_create_dsv=nullptr; s_hook_create_dsv.reset(); return false; }
    s_create_dsv_hook_installed.store(true,std::memory_order_release);
    debug_trace::Write("V61.34-DEPTH CreateDepthStencilView observer armed; descriptor->resource map only");
    return true;
}

bool DeviceHook::InstallCommandListResetHookD82(ID3D12GraphicsCommandList* list)
{
    if(!list)return false;
    bool expected=false;
    if(!g_d82_reset_install_attempted.compare_exchange_strong(expected,true,std::memory_order_acq_rel))
        return g_d82_temporal_hooks_ready.load(std::memory_order_acquire);
    void** vtable=*reinterpret_cast<void***>(list);
    // Public d3d12.h: IUnknown(3), Object(4), DeviceChild(1), CommandList(1),
    // GraphicsCommandList::Close(9), Reset(10), ClearState(11), Draw(12/13).
    constexpr std::size_t kResetIndex=10;
    hook::InlineRequest request{"kcd2vr_d82_command_list_reset",
        hook::Target{DetourModKit::Address{reinterpret_cast<std::uintptr_t>(vtable[kResetIndex])}},hook::Options{}};
    auto result=hook::inline_at(std::move(request),&DeviceHook::DetourResetCommandListD82);
    if(!result){diagnostic_trace::Write("D82 TEMPORAL_COMPOSE unavailable=RESET_HOOK_CREATE");return false;}
    s_hook_reset_command_list_d82=std::move(*result);
    const auto original=s_hook_reset_command_list_d82->original<ResetCommandListFunc>();
    s_original_reset_command_list_d82.store(original,std::memory_order_release);
    constexpr std::size_t kClearStateIndex=11;
    hook::InlineRequest clearRequest{"kcd2vr_d82_command_list_clear_state",
        hook::Target{DetourModKit::Address{reinterpret_cast<std::uintptr_t>(vtable[kClearStateIndex])}},hook::Options{}};
    auto clearResult=hook::inline_at(std::move(clearRequest),&DeviceHook::DetourClearCommandListStateD82);
    if(!clearResult){diagnostic_trace::Write("D82 TEMPORAL_COMPOSE unavailable=CLEAR_STATE_HOOK_CREATE");return false;}
    s_hook_clear_command_list_state_d82=std::move(*clearResult);
    const auto clearOriginal=s_hook_clear_command_list_state_d82->original<ClearCommandListStateFunc>();
    s_original_clear_command_list_state_d82.store(clearOriginal,std::memory_order_release);
    if(!original||!clearOriginal||!s_hook_reset_command_list_d82->enable()||!s_hook_clear_command_list_state_d82->enable()){
        // Close both admissions on partial failure, retaining every trampoline
        // and original pointer because a callback may already be in flight.
        (void)s_hook_reset_command_list_d82->disable();
        (void)s_hook_clear_command_list_state_d82->disable();
        diagnostic_trace::Write("D82 TEMPORAL_COMPOSE unavailable=RECORDING_HOOK_ENABLE trampolines=RETAINED");
        return false;
    }
    const bool ready=s_original_om_set_render_targets&&s_original_draw_instanced&&s_original_draw_indexed_instanced;
    g_d82_temporal_hooks_ready.store(ready,std::memory_order_release);
    diagnostic_trace::Write(ready
        ?"D82 TEMPORAL_COMPOSE ready=1 ownership=EXACT_RIGHT_COMPOSE_DRAW_LIST_GENERATION"
        :"D82 TEMPORAL_COMPOSE unavailable=OM_OR_DRAW_HOOK Native=UNCHANGED");
    return ready;
}

void __stdcall DeviceHook::DetourClearCommandListStateD82(ID3D12GraphicsCommandList* list,ID3D12PipelineState* initial)
{
    const auto original=s_original_clear_command_list_state_d82.load(std::memory_order_acquire);
    if(!g_d82_temporal_tracking.load(std::memory_order_acquire)){
        if(original)original(list,initial);
        return;
    }
    const DWORD entering=GetLastError();
    V6612ClearTemporalBindingsD82(list);
    SetLastError(entering);
    if(original)original(list,initial);
}

HRESULT __stdcall DeviceHook::DetourResetCommandListD82(ID3D12GraphicsCommandList* list,
    ID3D12CommandAllocator* allocator,ID3D12PipelineState* initial)
{
    const auto original=s_original_reset_command_list_d82.load(std::memory_order_acquire);
    std::lock_guard<std::recursive_mutex> transaction(g_temporal_mutex_d85);
    const DWORD entering=GetLastError();
    if(g_d82_temporal_tracking.load(std::memory_order_acquire))V6612DiscardTemporalRecordingD82(list);
    SetLastError(entering);
    const auto result=original?original(list,allocator,initial):E_FAIL;
    const auto after=GetLastError();
    // A failed Reset may leave commands intact; keep the resource reservation.
    // Only successful Reset proves a recorded-but-unsubmitted copy was discarded.
    if(SUCCEEDED(result))V6612DiscardMainWriterD85(list);
    ResetTemporalListD85(list,SUCCEEDED(result));
    SetLastError(after);return result;
}

bool DeviceHook::InstallDrawHooks(ID3D12GraphicsCommandList* list)
{
    if (!list || s_draw_hooks_installed.load(std::memory_order_acquire)) return true;
    bool expected=false;
    if(!s_draw_hooks_installed.compare_exchange_strong(expected,true,std::memory_order_acq_rel,std::memory_order_acquire)) return true;
    void** vtable=*reinterpret_cast<void***>(list);
    constexpr size_t kDrawInstancedIndex=12;
    constexpr size_t kDrawIndexedInstancedIndex=13;
    hook::InlineRequest r0{"kcd2vr_v6134_draw_probe",hook::Target{DetourModKit::Address{reinterpret_cast<uintptr_t>(vtable[kDrawInstancedIndex])}},hook::Options{}};
    auto h0=hook::inline_at(std::move(r0),&DeviceHook::DetourDrawInstanced);
    if(!h0){ s_draw_hooks_installed.store(false,std::memory_order_release); KCD2VR_DIAG_LOG(Bug,warning,"V61.34-DEPTH: DrawInstanced hook install failed"); return false; }
    s_hook_draw_instanced=std::move(*h0); s_original_draw_instanced=s_hook_draw_instanced->original<DrawInstancedFunc>();
    if(!s_hook_draw_instanced->enable()){ s_original_draw_instanced=nullptr; s_hook_draw_instanced.reset(); s_draw_hooks_installed.store(false,std::memory_order_release); return false; }
    hook::InlineRequest r1{"kcd2vr_v6134_drawindexed_probe",hook::Target{DetourModKit::Address{reinterpret_cast<uintptr_t>(vtable[kDrawIndexedInstancedIndex])}},hook::Options{}};
    auto h1=hook::inline_at(std::move(r1),&DeviceHook::DetourDrawIndexedInstanced);
    if(!h1){ KCD2VR_DIAG_LOG(Bug,warning,"V61.34-DEPTH: DrawIndexedInstanced hook install failed; non-indexed draw scoring remains available"); return true; }
    s_hook_draw_indexed_instanced=std::move(*h1); s_original_draw_indexed_instanced=s_hook_draw_indexed_instanced->original<DrawIndexedInstancedFunc>();
    if(!s_hook_draw_indexed_instanced->enable()){ s_original_draw_indexed_instanced=nullptr; s_hook_draw_indexed_instanced.reset(); }
    debug_trace::Write("V61.34-DEPTH draw observers armed; disabled fast-path until F4");
    return true;
}

void __stdcall DeviceHook::DetourCreateDepthStencilView(
    ID3D12Device* device_this, ID3D12Resource* resource,
    const D3D12_DEPTH_STENCIL_VIEW_DESC* desc, D3D12_CPU_DESCRIPTOR_HANDLE dest)
{
    if(resource && dest.ptr){ std::lock_guard<std::mutex> lock(s_dsv_map_mutex); s_dsv_resource_map[dest.ptr]=resource; }
    if(s_original_create_dsv) s_original_create_dsv(device_this,resource,desc,dest);
}

bool DeviceHook::InstallCreateDescriptorHeapHook(ID3D12Device* device)
{
    if(!device || s_create_descriptor_heap_hook_installed.load(std::memory_order_acquire)) return true;
    bool expected=false;
    if(!s_create_descriptor_heap_hook_installed.compare_exchange_strong(expected,true,std::memory_order_acq_rel,std::memory_order_acquire)) return true;
    void** vtable=*reinterpret_cast<void***>(device);
    constexpr size_t kCreateDescriptorHeapIndex=14;
    hook::InlineRequest req{"kcd2vr_v6625_create_descriptor_heap",hook::Target{DetourModKit::Address{reinterpret_cast<uintptr_t>(vtable[kCreateDescriptorHeapIndex])}},hook::Options{}};
    auto h=hook::inline_at(std::move(req),&DeviceHook::DetourCreateDescriptorHeap);
    if(!h){s_create_descriptor_heap_hook_installed.store(false,std::memory_order_release);KCD2VR_DIAG_LOG(Bug,warning,"V66.25-UI-SRV CreateDescriptorHeap hook install failed");return false;}
    s_hook_create_descriptor_heap=std::move(*h);s_original_create_descriptor_heap=s_hook_create_descriptor_heap->original<CreateDescriptorHeapFunc>();
    if(!s_hook_create_descriptor_heap->enable()){s_original_create_descriptor_heap=nullptr;s_hook_create_descriptor_heap.reset();s_create_descriptor_heap_hook_installed.store(false,std::memory_order_release);return false;}
    diagnostic_trace::Write("V66.25-UI-SRV CreateDescriptorHeap observer installed");
    return true;
}

bool DeviceHook::InstallCreateShaderResourceViewHook(ID3D12Device* device)
{
    if(!device || s_create_srv_hook_installed.load(std::memory_order_acquire)) return true;
    bool expected=false;
    if(!s_create_srv_hook_installed.compare_exchange_strong(expected,true,std::memory_order_acq_rel,std::memory_order_acquire)) return true;
    void** vtable=*reinterpret_cast<void***>(device);
    constexpr size_t kCreateShaderResourceViewIndex=18;
    hook::InlineRequest req{"kcd2vr_v6625_create_srv",hook::Target{DetourModKit::Address{reinterpret_cast<uintptr_t>(vtable[kCreateShaderResourceViewIndex])}},hook::Options{}};
    auto h=hook::inline_at(std::move(req),&DeviceHook::DetourCreateShaderResourceView);
    if(!h){s_create_srv_hook_installed.store(false,std::memory_order_release);KCD2VR_DIAG_LOG(Bug,warning,"V66.25-UI-SRV CreateShaderResourceView hook install failed");return false;}
    s_hook_create_srv=std::move(*h);s_original_create_srv=s_hook_create_srv->original<CreateShaderResourceViewFunc>();
    if(!s_hook_create_srv->enable()){s_original_create_srv=nullptr;s_hook_create_srv.reset();s_create_srv_hook_installed.store(false,std::memory_order_release);return false;}
    diagnostic_trace::Write("V66.25-UI-SRV CreateShaderResourceView observer installed");
    return true;
}

HRESULT __stdcall DeviceHook::DetourCreateDescriptorHeap(
    ID3D12Device* device_this,const D3D12_DESCRIPTOR_HEAP_DESC* desc,REFIID riid,void** out_heap)
{
    const HRESULT hr=s_original_create_descriptor_heap?s_original_create_descriptor_heap(device_this,desc,riid,out_heap):E_FAIL;
    if(s_v6627_performance_quiet.load(std::memory_order_relaxed)) return hr;
    if(SUCCEEDED(hr)&&desc&&out_heap&&*out_heap&&desc->Type==D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV){
        auto* heap=static_cast<ID3D12DescriptorHeap*>(*out_heap);
        DescriptorHeapRange r{};r.cpu=heap->GetCPUDescriptorHandleForHeapStart();
        r.shader_visible=(desc->Flags&D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)!=0;
        if(r.shader_visible)r.gpu=heap->GetGPUDescriptorHandleForHeapStart();
        r.count=desc->NumDescriptors;r.increment=device_this?device_this->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV):0;
        if(r.shader_visible&&r.cpu.ptr&&r.gpu.ptr&&r.count&&r.increment){std::lock_guard<std::mutex> lk(s_descriptor_trace_mutex);s_cbv_srv_uav_heaps[heap]=r;}
    }
    return hr;
}

void __stdcall DeviceHook::DetourCreateShaderResourceView(
    ID3D12Device* device_this,ID3D12Resource* resource,const D3D12_SHADER_RESOURCE_VIEW_DESC* desc,D3D12_CPU_DESCRIPTOR_HANDLE dest)
{
    if(s_v6627_performance_quiet.load(std::memory_order_relaxed)){
        if(s_original_create_srv)s_original_create_srv(device_this,resource,desc,dest);
        return;
    }
    if(dest.ptr){std::lock_guard<std::mutex> lk(s_descriptor_trace_mutex);s_srv_resource_map[dest.ptr]=resource;}
    if(s_original_create_srv)s_original_create_srv(device_this,resource,desc,dest);
}

bool DeviceHook::InstallCopyDescriptorHooks(ID3D12Device* device)
{
    if(!device || s_copy_descriptors_hook_installed.load(std::memory_order_acquire)) return true;
    bool expected=false;
    if(!s_copy_descriptors_hook_installed.compare_exchange_strong(expected,true,std::memory_order_acq_rel,std::memory_order_acquire)) return true;
    void** vtable=*reinterpret_cast<void***>(device);
    constexpr size_t kCopyDescriptorsIndex=23;
    constexpr size_t kCopyDescriptorsSimpleIndex=24;
    hook::InlineRequest reqA{"kcd2vr_v6626_copy_descriptors",hook::Target{DetourModKit::Address{reinterpret_cast<uintptr_t>(vtable[kCopyDescriptorsIndex])}},hook::Options{}};
    auto a=hook::inline_at(std::move(reqA),&DeviceHook::DetourCopyDescriptors);
    if(!a){s_copy_descriptors_hook_installed.store(false,std::memory_order_release);KCD2VR_DIAG_LOG(Bug,warning,"V66.26-UI-SRV CopyDescriptors hook install failed");return false;}
    s_hook_copy_descriptors=std::move(*a);s_original_copy_descriptors=s_hook_copy_descriptors->original<CopyDescriptorsFunc>();
    hook::InlineRequest reqB{"kcd2vr_v6626_copy_descriptors_simple",hook::Target{DetourModKit::Address{reinterpret_cast<uintptr_t>(vtable[kCopyDescriptorsSimpleIndex])}},hook::Options{}};
    auto b=hook::inline_at(std::move(reqB),&DeviceHook::DetourCopyDescriptorsSimple);
    if(!b){s_hook_copy_descriptors.reset();s_original_copy_descriptors=nullptr;s_copy_descriptors_hook_installed.store(false,std::memory_order_release);KCD2VR_DIAG_LOG(Bug,warning,"V66.26-UI-SRV CopyDescriptorsSimple hook install failed");return false;}
    s_hook_copy_descriptors_simple=std::move(*b);s_original_copy_descriptors_simple=s_hook_copy_descriptors_simple->original<CopyDescriptorsSimpleFunc>();
    if(!s_original_copy_descriptors||!s_original_copy_descriptors_simple||!s_hook_copy_descriptors->enable()||!s_hook_copy_descriptors_simple->enable()){
        if(s_hook_copy_descriptors){(void)s_hook_copy_descriptors->disable();s_hook_copy_descriptors.reset();}
        if(s_hook_copy_descriptors_simple){(void)s_hook_copy_descriptors_simple->disable();s_hook_copy_descriptors_simple.reset();}
        s_original_copy_descriptors=nullptr;s_original_copy_descriptors_simple=nullptr;s_copy_descriptors_hook_installed.store(false,std::memory_order_release);return false;
    }
    diagnostic_trace::Write("V66.26-UI-SRV descriptor-copy lineage installed CopyDescriptors+CopyDescriptorsSimple; copied SRVs can now resolve in shader-visible heaps");
    return true;
}

void __stdcall DeviceHook::DetourCopyDescriptorsSimple(
    ID3D12Device* device_this,UINT num_descriptors,D3D12_CPU_DESCRIPTOR_HANDLE dest_start,
    D3D12_CPU_DESCRIPTOR_HANDLE src_start,D3D12_DESCRIPTOR_HEAP_TYPE heap_type)
{
    if(s_v6627_performance_quiet.load(std::memory_order_relaxed)){
        if(s_original_copy_descriptors_simple)s_original_copy_descriptors_simple(device_this,num_descriptors,dest_start,src_start,heap_type);
        return;
    }
    if(heap_type==D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV && device_this && num_descriptors && dest_start.ptr && src_start.ptr){
        const UINT inc=device_this->GetDescriptorHandleIncrementSize(heap_type);
        if(inc){
            std::lock_guard<std::mutex> lk(s_descriptor_trace_mutex);
            for(UINT i=0;i<num_descriptors;++i){
                const SIZE_T src=src_start.ptr+static_cast<SIZE_T>(i)*inc;
                const SIZE_T dst=dest_start.ptr+static_cast<SIZE_T>(i)*inc;
                auto it=s_srv_resource_map.find(src);
                if(it!=s_srv_resource_map.end()) s_srv_resource_map[dst]=it->second;
                else s_srv_resource_map.erase(dst);
            }
        }
    }
    if(s_original_copy_descriptors_simple)s_original_copy_descriptors_simple(device_this,num_descriptors,dest_start,src_start,heap_type);
}

void __stdcall DeviceHook::DetourCopyDescriptors(
    ID3D12Device* device_this,UINT num_dest_ranges,const D3D12_CPU_DESCRIPTOR_HANDLE* dest_range_starts,const UINT* dest_range_sizes,
    UINT num_src_ranges,const D3D12_CPU_DESCRIPTOR_HANDLE* src_range_starts,const UINT* src_range_sizes,D3D12_DESCRIPTOR_HEAP_TYPE heap_type)
{
    if(s_v6627_performance_quiet.load(std::memory_order_relaxed)){
        if(s_original_copy_descriptors)s_original_copy_descriptors(device_this,num_dest_ranges,dest_range_starts,dest_range_sizes,num_src_ranges,src_range_starts,src_range_sizes,heap_type);
        return;
    }
    if(heap_type==D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV && device_this && dest_range_starts && src_range_starts){
        const UINT inc=device_this->GetDescriptorHandleIncrementSize(heap_type);
        if(inc){
            std::vector<ID3D12Resource*> lineage;
            for(UINT r=0;r<num_src_ranges;++r){
                const UINT n=src_range_sizes?src_range_sizes[r]:1u;
                for(UINT i=0;i<n;++i){
                    const SIZE_T src=src_range_starts[r].ptr+static_cast<SIZE_T>(i)*inc;ID3D12Resource* rr=nullptr;
                    {std::lock_guard<std::mutex> lk(s_descriptor_trace_mutex);auto it=s_srv_resource_map.find(src);if(it!=s_srv_resource_map.end())rr=it->second;}
                    lineage.push_back(rr);
                }
            }
            SIZE_T cursor=0;
            std::lock_guard<std::mutex> lk(s_descriptor_trace_mutex);
            for(UINT r=0;r<num_dest_ranges && cursor<lineage.size();++r){
                const UINT n=dest_range_sizes?dest_range_sizes[r]:1u;
                for(UINT i=0;i<n && cursor<lineage.size();++i,++cursor){
                    const SIZE_T dst=dest_range_starts[r].ptr+static_cast<SIZE_T>(i)*inc;
                    if(lineage[cursor]) s_srv_resource_map[dst]=lineage[cursor]; else s_srv_resource_map.erase(dst);
                }
            }
        }
    }
    if(s_original_copy_descriptors)s_original_copy_descriptors(device_this,num_dest_ranges,dest_range_starts,dest_range_sizes,num_src_ranges,src_range_starts,src_range_sizes,heap_type);
}

bool DeviceHook::InstallGraphicsRootDescriptorTableHook(ID3D12GraphicsCommandList* list)
{
    if(!list || s_graphics_root_descriptor_table_hook_installed.load(std::memory_order_acquire)) return true;
    bool expected=false;
    if(!s_graphics_root_descriptor_table_hook_installed.compare_exchange_strong(expected,true,std::memory_order_acq_rel,std::memory_order_acquire)) return true;
    void** vtable=*reinterpret_cast<void***>(list);
    constexpr size_t kSetGraphicsRootDescriptorTableIndex=32;
    hook::InlineRequest req{"kcd2vr_v6625_graphics_root_table",hook::Target{DetourModKit::Address{reinterpret_cast<uintptr_t>(vtable[kSetGraphicsRootDescriptorTableIndex])}},hook::Options{}};
    auto h=hook::inline_at(std::move(req),&DeviceHook::DetourSetGraphicsRootDescriptorTable);
    if(!h){s_graphics_root_descriptor_table_hook_installed.store(false,std::memory_order_release);KCD2VR_DIAG_LOG(Bug,warning,"V66.25-UI-SRV SetGraphicsRootDescriptorTable hook install failed");return false;}
    s_hook_graphics_root_descriptor_table=std::move(*h);s_original_set_graphics_root_descriptor_table=s_hook_graphics_root_descriptor_table->original<SetGraphicsRootDescriptorTableFunc>();
    if(!s_hook_graphics_root_descriptor_table->enable()){s_original_set_graphics_root_descriptor_table=nullptr;s_hook_graphics_root_descriptor_table.reset();s_graphics_root_descriptor_table_hook_installed.store(false,std::memory_order_release);return false;}
    diagnostic_trace::Write("V66.25-UI-SRV SetGraphicsRootDescriptorTable observer installed; read-only late-UI source mapping armed");
    return true;
}

void __stdcall DeviceHook::DetourSetGraphicsRootDescriptorTable(
    ID3D12GraphicsCommandList* list_this,UINT root_parameter_index,D3D12_GPU_DESCRIPTOR_HANDLE base_descriptor)
{
    if(s_v6627_performance_quiet.load(std::memory_order_relaxed)){
        if(s_original_set_graphics_root_descriptor_table)s_original_set_graphics_root_descriptor_table(list_this,root_parameter_index,base_descriptor);
        return;
    }
    if(g_v6610_output_enabled.load(std::memory_order_acquire)&&list_this&&root_parameter_index<32){
        std::lock_guard<std::mutex> lk(s_mode_pso_mutex);auto& st=s_mode_list_state[list_this];
        st.graphics_table[root_parameter_index]=base_descriptor;st.graphics_table_mask|=(1u<<root_parameter_index);
    }
    if(s_original_set_graphics_root_descriptor_table)s_original_set_graphics_root_descriptor_table(list_this,root_parameter_index,base_descriptor);
}

void DeviceHook::TraceV6625LateUiDescriptors(ID3D12GraphicsCommandList* list_this,const ModePsoListState& st) noexcept
{
    if(!list_this||!st.pso||!st.graphics_table_mask)return;
    std::uint64_t hash=1469598103934665603ull;
    for(UINT root=0;root<32;++root)if(st.graphics_table_mask&(1u<<root)){
        hash^=(static_cast<std::uint64_t>(root)<<56)^static_cast<std::uint64_t>(st.graphics_table[root].ptr);hash*=1099511628211ull;
    }
    const auto frame=g_v6624_late_frame.load(std::memory_order_relaxed);
    {
        std::lock_guard<std::mutex> lk(s_descriptor_trace_mutex);
        auto& prev=s_v6625_last_ui_table_hash[st.pso];if(prev==hash)return;prev=hash;
    }
    const auto dump=s_v6625_ui_descriptor_dumps.fetch_add(1,std::memory_order_relaxed)+1;if(dump>48)return;
    char head[384]{};std::snprintf(head,sizeof(head),"V66.25-UI-SRV BEGIN dump=%llu frame=%u list=%p pso=%p tableMask=0x%08X tableHash=%016llX boundary=AFTER-RIGHT-BEFORE-PRESENT",
        (unsigned long long)dump,frame,list_this,st.pso,st.graphics_table_mask,(unsigned long long)hash);diagnostic_trace::Write(head);
    ID3D12Resource* seen[64]{};UINT seenCount=0,logged=0;
    for(UINT root=0;root<32&&logged<32;++root){
        if(!(st.graphics_table_mask&(1u<<root)))continue;
        const auto base=st.graphics_table[root];if(!base.ptr)continue;
        DescriptorHeapRange range{};bool found=false;
        {
            std::lock_guard<std::mutex> lk(s_descriptor_trace_mutex);
            for(const auto& kv:s_cbv_srv_uav_heaps){const auto& r=kv.second;if(!r.shader_visible||!r.gpu.ptr||!r.increment||!r.count)continue;
                const UINT64 span=static_cast<UINT64>(r.increment)*static_cast<UINT64>(r.count);
                if(base.ptr>=r.gpu.ptr&&base.ptr<r.gpu.ptr+span){range=r;found=true;break;}}
        }
        if(!found)continue;
        const UINT64 baseDelta=base.ptr-range.gpu.ptr;if(baseDelta%range.increment)continue;
        const UINT start=static_cast<UINT>(baseDelta/range.increment);
        for(UINT off=0;off<32&&start+off<range.count&&logged<32;++off){
            const SIZE_T cpu=range.cpu.ptr+static_cast<SIZE_T>(start+off)*range.increment;ID3D12Resource* rr=nullptr;
            {std::lock_guard<std::mutex> lk(s_descriptor_trace_mutex);auto it=s_srv_resource_map.find(cpu);if(it!=s_srv_resource_map.end())rr=it->second;}
            if(!rr) continue;
            bool dup=false;
            for(UINT k=0;k<seenCount;++k) if(seen[k]==rr){dup=true;break;}
            if(dup) continue;
            if(seenCount<64) seen[seenCount++]=rr;
            D3D12_RESOURCE_DESC d{};d=rr->GetDesc();
            if(d.Dimension!=D3D12_RESOURCE_DIMENSION_TEXTURE2D)continue;
            bool sameMain=false;{std::lock_guard<std::mutex> bl(s_bridge_capture_mutex);sameMain=(rr==s_bridge_current_main_target);}
            char row[640]{};std::snprintf(row,sizeof(row),
                "V66.25-UI-SRV RESOURCE dump=%llu root=%u offset=%u gpu=0x%llX cpu=0x%llX res=%p size=%llux%u arr=%u mips=%u fmt=%u flags=0x%X allowRT=%d sameMain=%d class=%s",
                (unsigned long long)dump,root,off,(unsigned long long)(base.ptr+static_cast<UINT64>(off)*range.increment),(unsigned long long)cpu,rr,
                (unsigned long long)d.Width,d.Height,d.DepthOrArraySize,d.MipLevels,(unsigned)d.Format,(unsigned)d.Flags,
                (d.Flags&D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET)?1:0,sameMain?1:0,
                (d.Flags&D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET)?"DYNAMIC-RT-SRV-CANDIDATE":"TEXTURE-SRV");
            diagnostic_trace::Write(row);++logged;
        }
    }
    char tail[256]{};std::snprintf(tail,sizeof(tail),"V66.25-UI-SRV END dump=%llu uniqueTextureSrvs=%u logged=%u",(unsigned long long)dump,seenCount,logged);diagnostic_trace::Write(tail);
}

void DeviceHook::TraceModePsoDraw(ID3D12GraphicsCommandList* list,bool indexed) noexcept
{
    if(!DeviceHook::ModePsoProvenanceProbeEnabled() || !list) return;
    DeviceHook::ModePsoListState st{};
    {
        std::lock_guard<std::mutex> lock(DeviceHook::s_mode_pso_mutex);
        auto it=DeviceHook::s_mode_list_state.find(list); if(it==DeviceHook::s_mode_list_state.end()) return; st=it->second;
        if(!st.lane && st.pso){ auto pi=DeviceHook::s_pso_shader_hashes.find(st.pso); if(pi!=DeviceHook::s_pso_shader_hashes.end()){ for(int i=0;i<5;++i){ const int lane=native_stereo_outer_lab::ClassifyModeShaderHash(pi->second.h[i]); if(lane){ st.shader_hash=pi->second.h[i]; st.lane=lane; st.shader_stage=i+1; break; } } } }
    }
    if(st.lane!=1 && st.lane!=2) return;
    auto& dc=(st.lane==1)?DeviceHook::s_mode0_draw_count:DeviceHook::s_mode2_draw_count;
    const auto drawN=dc.fetch_add(1,std::memory_order_relaxed)+1;
    std::uint64_t ps=0,pt=0,pw=0,pc=0; std::uint32_t ph=0; DeviceHook::GetLatestPresentIdentity(&ps,&pt,&pw,&ph,&pc);
    for(UINT i=0;i<st.rt_count && i<4;++i){ auto* rr=st.rt[i]; if(!rr) continue; const auto rd=rr->GetDesc();
        const bool full=(pw&&ph&&rd.Width==pw&&rd.Height==ph);
        if(!full) continue;
        bool firstTarget=false; { std::lock_guard<std::mutex> lock(DeviceHook::s_mode_pso_mutex); auto& map=(st.lane==1)?DeviceHook::s_mode0_rt_draws:DeviceHook::s_mode2_rt_draws; auto [it,ins]=map.emplace(rr,0); firstTarget=ins; ++it->second; }
        if(firstTarget){ char t[384]{}; std::snprintf(t,sizeof(t),"V65.81-F5-PSO TARGET lane=MODE%d res=%p rtSlot=%u size=%llux%u arr=%u fmt=%u firstPso=%p stage=%d shaderHash=%016llX",st.lane==1?0:2,rr,i,(unsigned long long)rd.Width,rd.Height,rd.DepthOrArraySize,(unsigned)rd.Format,st.pso,st.shader_stage,(unsigned long long)st.shader_hash); diagnostic_trace::Write(t); }
        const auto ev=DeviceHook::s_mode_pso_event_count.fetch_add(1,std::memory_order_relaxed)+1;
        if(ev<=96 || (drawN%500)==0){ char m[512]{}; std::snprintf(m,sizeof(m),"V65.81-F5-PSO DRAW event=%llu lane=MODE%d draw=%llu indexed=%d list=%p pso=%p stage=%d shaderHash=%016llX rtSlot=%u rtv=0x%llX res=%p size=%llux%u arr=%u fmt=%u present=%llu",
            (unsigned long long)ev,st.lane==1?0:2,(unsigned long long)drawN,indexed?1:0,list,st.pso,st.shader_stage,(unsigned long long)st.shader_hash,i,(unsigned long long)st.rtv[i].ptr,rr,(unsigned long long)rd.Width,rd.Height,rd.DepthOrArraySize,(unsigned)rd.Format,(unsigned long long)ps); diagnostic_trace::Write(m); }
    }
}

void DeviceHook::TraceV6624PostFxOrLateDraw(ID3D12GraphicsCommandList* list_this,bool indexed,UINT primitive_count,UINT instance_count) noexcept
{
    if(!list_this || !g_v6610_output_enabled.load(std::memory_order_acquire)) return;
    const int eye=g_v6610_output_eye.load(std::memory_order_acquire);
    const bool late=g_v6624_late_window.load(std::memory_order_acquire);
    if(!late && (eye<0 || eye>1)) return;
    DeviceHook::ModePsoListState st{}; DeviceHook::PsoShaderRecord shader{}; bool haveShader=false;
    {
        std::lock_guard<std::mutex> lock(DeviceHook::s_mode_pso_mutex);
        auto it=DeviceHook::s_mode_list_state.find(list_this); if(it==DeviceHook::s_mode_list_state.end()) return; st=it->second;
        if(st.pso){ auto pi=DeviceHook::s_pso_shader_hashes.find(st.pso); if(pi!=DeviceHook::s_pso_shader_hashes.end()){shader=pi->second;haveShader=true;} }
    }
    if(st.rt_count==0 || !st.rt[0]) return;
    bool sameMain=false; { std::lock_guard<std::mutex> bl(DeviceHook::s_bridge_capture_mutex); sameMain=(st.rt[0]==DeviceHook::s_bridge_current_main_target); }
    const auto rd=st.rt[0]->GetDesc();
    const bool screenish=rd.Dimension==D3D12_RESOURCE_DIMENSION_TEXTURE2D && rd.Width>=640 && rd.Height>=480;
    if(!screenish) return;
    if(late){
        if(!sameMain) return;
        const auto n=g_v6624_late_draw_logs.fetch_add(1,std::memory_order_relaxed)+1; if(n>96) return;
        char m[640]{}; std::snprintf(m,sizeof(m),
            "V66.24-LATE-HUD-CAND n=%llu frame=%u list=%p draw=%s count=%u inst=%u pso=%p VS=%016llX PS=%016llX rt=%p size=%llux%u fmt=%u sameMain=1 boundary=AFTER-RIGHT-BEFORE-PRESENT",
            (unsigned long long)n,g_v6624_late_frame.load(std::memory_order_relaxed),list_this,indexed?"INDEXED":"INSTANCED",primitive_count,instance_count,st.pso,
            (unsigned long long)(haveShader?shader.h[0]:0),(unsigned long long)(haveShader?shader.h[1]:0),st.rt[0],(unsigned long long)rd.Width,rd.Height,(unsigned)rd.Format);
        diagnostic_trace::Write(m);
        TraceV6625LateUiDescriptors(list_this,st);
        return;
    }
    // Fullscreen post effects conventionally use a triangle/quad (<=6 vertices/indices).
    // Log only those to avoid returning to the retired whole-command census.
    if(primitive_count>6 || instance_count>2) return;
    const auto n=g_v6624_postfx_draw_logs.fetch_add(1,std::memory_order_relaxed)+1; if(n>192) return;
    char m[640]{}; std::snprintf(m,sizeof(m),
        "V66.24-POSTFX-DRAW n=%llu eye=%c frame=%u list=%p draw=%s count=%u inst=%u pso=%p VS=%016llX PS=%016llX rt=%p size=%llux%u fmt=%u sameMain=%d",
        (unsigned long long)n,eye?'R':'L',g_v6610_output_frame.load(std::memory_order_relaxed),list_this,indexed?"INDEXED":"INSTANCED",primitive_count,instance_count,st.pso,
        (unsigned long long)(haveShader?shader.h[0]:0),(unsigned long long)(haveShader?shader.h[1]:0),st.rt[0],(unsigned long long)rd.Width,rd.Height,(unsigned)rd.Format,sameMain?1:0);
    diagnostic_trace::Write(m);
}

void __stdcall DeviceHook::DetourDrawInstanced(
    ID3D12GraphicsCommandList* list_this, UINT vertex_count_per_instance, UINT instance_count,
    UINT start_vertex_location, UINT start_instance_location)
{
    if(hud_layer_v6664::RedirectActiveThisFrame() && g_v6624_late_window.load(std::memory_order_relaxed))
        hud_layer_v6664::OnLateDraw(false,vertex_count_per_instance,instance_count,list_this);
    if(s_v6627_performance_quiet.load(std::memory_order_relaxed)){
        if(s_original_draw_instanced) {
            s_original_draw_instanced(list_this,vertex_count_per_instance,instance_count,start_vertex_location,start_instance_location);
            if(g_d82_compose_scope.Active()||g_d82_composition_observed.load(std::memory_order_relaxed))V6612RecordTemporalDrawD82(list_this,vertex_count_per_instance,instance_count);
        }
        return;
    }
    TraceModePsoDraw(list_this,false);
    TraceV6624PostFxOrLateDraw(list_this,false,vertex_count_per_instance,instance_count);
    if(s_generic_depth_probe_enabled.load(std::memory_order_relaxed)){
        const int i=s_tls_depth_candidate; if(i>=0 && static_cast<std::size_t>(i)<kGenericDepthCandidateCount) s_generic_depth_candidates[i].draws.fetch_add(1,std::memory_order_relaxed);
    }
    if(s_original_draw_instanced) {
        s_original_draw_instanced(list_this,vertex_count_per_instance,instance_count,start_vertex_location,start_instance_location);
        if(g_d82_compose_scope.Active()||g_d82_composition_observed.load(std::memory_order_relaxed))V6612RecordTemporalDrawD82(list_this,vertex_count_per_instance,instance_count);
    }
}

void __stdcall DeviceHook::DetourDrawIndexedInstanced(
    ID3D12GraphicsCommandList* list_this, UINT index_count_per_instance, UINT instance_count,
    UINT start_index_location, INT base_vertex_location, UINT start_instance_location)
{
    if(hud_layer_v6664::RedirectActiveThisFrame() && g_v6624_late_window.load(std::memory_order_relaxed))
        hud_layer_v6664::OnLateDraw(true,index_count_per_instance,instance_count,list_this);
    if(s_v6627_performance_quiet.load(std::memory_order_relaxed)){
        if(s_original_draw_indexed_instanced) {
            s_original_draw_indexed_instanced(list_this,index_count_per_instance,instance_count,start_index_location,base_vertex_location,start_instance_location);
            if(g_d82_compose_scope.Active()||g_d82_composition_observed.load(std::memory_order_relaxed))V6612RecordTemporalDrawD82(list_this,index_count_per_instance,instance_count);
        }
        return;
    }
    TraceModePsoDraw(list_this,true);
    TraceV6624PostFxOrLateDraw(list_this,true,index_count_per_instance,instance_count);
    if(s_generic_depth_probe_enabled.load(std::memory_order_relaxed)){
        const int i=s_tls_depth_candidate; if(i>=0 && static_cast<std::size_t>(i)<kGenericDepthCandidateCount){ s_generic_depth_candidates[i].draws.fetch_add(1,std::memory_order_relaxed); s_generic_depth_candidates[i].indexed_draws.fetch_add(1,std::memory_order_relaxed); }
    }
    if(s_original_draw_indexed_instanced) {
        s_original_draw_indexed_instanced(list_this,index_count_per_instance,instance_count,start_index_location,base_vertex_location,start_instance_location);
        if(g_d82_compose_scope.Active()||g_d82_composition_observed.load(std::memory_order_relaxed))V6612RecordTemporalDrawD82(list_this,index_count_per_instance,instance_count);
    }
}

void DeviceHook::SetGenericDepthProbe(bool enabled)
{
    if(enabled){
        for(auto& c:s_generic_depth_candidates){ c.resource.store(nullptr,std::memory_order_relaxed); c.binds.store(0,std::memory_order_relaxed); c.draws.store(0,std::memory_order_relaxed); c.indexed_draws.store(0,std::memory_order_relaxed); c.width=0; c.height=0; c.format=DXGI_FORMAT_UNKNOWN; c.flags=D3D12_RESOURCE_FLAG_NONE; c.screen_match.store(false,std::memory_order_relaxed); c.aspect_match.store(false,std::memory_order_relaxed); }
        s_tls_depth_candidate=-1; s_generic_depth_probe_enabled.store(true,std::memory_order_release);
        debug_trace::Write("V61.34-F4 GENERIC-DEPTH ON: read-only DSV+draw observer; ranking by draw use + present resolution/aspect; no resource mutation");
        return;
    }
    s_generic_depth_probe_enabled.store(false,std::memory_order_release); s_tls_depth_candidate=-1;
    struct Row{std::size_t i; std::uint64_t draws; std::uint64_t binds; bool screen; bool aspect;};
    std::array<Row,kGenericDepthCandidateCount> rows{}; std::size_t n=0;
    for(std::size_t i=0;i<kGenericDepthCandidateCount;++i){ auto* r=s_generic_depth_candidates[i].resource.load(std::memory_order_acquire); if(!r) continue; rows[n++]={i,s_generic_depth_candidates[i].draws.load(),s_generic_depth_candidates[i].binds.load(),s_generic_depth_candidates[i].screen_match.load(),s_generic_depth_candidates[i].aspect_match.load()}; }
    std::sort(rows.begin(),rows.begin()+n,[](const Row&a,const Row&b){ if(a.screen!=b.screen) return a.screen>b.screen; if(a.aspect!=b.aspect) return a.aspect>b.aspect; if(a.draws!=b.draws) return a.draws>b.draws; return a.binds>b.binds; });
    char h[192]{}; std::snprintf(h,sizeof(h),"V61.34-F4 GENERIC-DEPTH OFF summary candidates=%zu (top screen/aspect/draw ranked)",n); debug_trace::Write(h);
    const std::size_t top=std::min<std::size_t>(n,12);
    for(std::size_t k=0;k<top;++k){ const auto& row=rows[k]; const auto& c=s_generic_depth_candidates[row.i]; char m[512]{}; std::snprintf(m,sizeof(m),
        "V61.34-DEPTH-CAND rank=%zu resource=%p size=%llux%u fmt=%u flags=0x%X binds=%llu draws=%llu indexed=%llu screenMatch=%s aspectMatch=%s",
        k+1,c.resource.load(),static_cast<unsigned long long>(c.width),c.height,static_cast<unsigned>(c.format),static_cast<unsigned>(c.flags),
        static_cast<unsigned long long>(c.binds.load()),static_cast<unsigned long long>(c.draws.load()),static_cast<unsigned long long>(c.indexed_draws.load()),row.screen?"YES":"no",row.aspect?"YES":"no"); debug_trace::Write(m); }
}

bool DeviceHook::GenericDepthProbeEnabled(){ return s_generic_depth_probe_enabled.load(std::memory_order_acquire); }

bool DeviceHook::GetLiveBestDepthCandidate(ID3D12Resource** out_resource, DXGI_FORMAT* out_format,
    std::uint64_t* out_width, std::uint32_t* out_height) noexcept
{
    if (!s_generic_depth_probe_enabled.load(std::memory_order_relaxed)) return false;
    constexpr std::uint64_t kMinBindsToTrust = 8; // V61.36: exact screen/aspect DSV, no draw-hook scoring
    struct Row{std::size_t i; std::uint64_t draws; std::uint64_t binds; bool screen; bool aspect;};
    std::array<Row,kGenericDepthCandidateCount> rows{}; std::size_t n=0;
    for(std::size_t i=0;i<kGenericDepthCandidateCount;++i){
        auto* r=s_generic_depth_candidates[i].resource.load(std::memory_order_acquire);
        if(!r) continue;
        const auto draws=s_generic_depth_candidates[i].draws.load(std::memory_order_relaxed);
        const auto binds=s_generic_depth_candidates[i].binds.load(std::memory_order_relaxed);
        if(binds<kMinBindsToTrust) continue;
        rows[n++]={i,draws,binds,
            s_generic_depth_candidates[i].screen_match.load(std::memory_order_relaxed),
            s_generic_depth_candidates[i].aspect_match.load(std::memory_order_relaxed)};
    }
    if(n==0) return false;
    // Only trust a candidate that actually matches present resolution AND
    // aspect ratio - this is meant to feed a live visual effect, not a log,
    // so we hold it to a higher bar than the F4-off diagnostic summary does.
    std::size_t best=n; std::uint64_t best_binds=0;
    for(std::size_t k=0;k<n;++k){ if(rows[k].screen && rows[k].aspect && rows[k].binds>best_binds){ best=k; best_binds=rows[k].binds; } }
    if(best==n) return false;
    const auto& c=s_generic_depth_candidates[rows[best].i];
    auto* r=c.resource.load(std::memory_order_acquire);
    if(!r) return false;
    if(out_resource) *out_resource=r;
    if(out_format) *out_format=c.format;
    if(out_width) *out_width=c.width;
    if(out_height) *out_height=c.height;
    return true;
}



bool DeviceHook::InstallOutputLayoutHooks(ID3D12GraphicsCommandList* list)
{
    if(!list || s_output_layout_hooks_installed.load(std::memory_order_acquire)) return true;
    void** vt=*reinterpret_cast<void***>(list);
    hook::InlineRequest vr{"kcd2vr_v6570_rssetviewports",hook::Target{DetourModKit::Address{reinterpret_cast<uintptr_t>(vt[21])}},hook::Options{}};
    auto rv=hook::inline_at(std::move(vr),&DeviceHook::DetourRSSetViewports); if(!rv) return false;
    s_hook_rs_set_viewports=std::move(*rv); s_original_rs_set_viewports=s_hook_rs_set_viewports->original<RSSetViewportsFunc>(); if(!s_hook_rs_set_viewports->enable()) return false;
    hook::InlineRequest sr{"kcd2vr_v6570_rssetscissors",hook::Target{DetourModKit::Address{reinterpret_cast<uintptr_t>(vt[22])}},hook::Options{}};
    auto rs=hook::inline_at(std::move(sr),&DeviceHook::DetourRSSetScissorRects); if(!rs) return false;
    s_hook_rs_set_scissor_rects=std::move(*rs); s_original_rs_set_scissor_rects=s_hook_rs_set_scissor_rects->original<RSSetScissorRectsFunc>(); if(!s_hook_rs_set_scissor_rects->enable()) return false;
    s_output_layout_hooks_installed.store(true,std::memory_order_release);
    diagnostic_trace::Write("V65.71-F9 output-layout hooks installed RSSetViewports[21] RSSetScissorRects[22]");
    return true;
}

static inline bool V6570LayoutLogBudget(std::atomic<std::uint64_t>& c, std::uint64_t& n) noexcept { n=c.fetch_add(1,std::memory_order_relaxed)+1; return n<=96; }

void __stdcall DeviceHook::DetourRSSetViewports(ID3D12GraphicsCommandList* list, UINT count, const D3D12_VIEWPORT* vps)
{
    if(s_output_layout_probe_enabled.load(std::memory_order_relaxed) && list!=s_internal_vr_command_list.load() && list!=s_internal_menu_command_list.load() && vps && count){
        std::uint64_t n=0; if(V6570LayoutLogBudget(s_output_layout_event_count,n)){ const auto& v=vps[0]; char m[384]{}; std::snprintf(m,sizeof(m),"V65.71-F9-VIEWPORT n=%llu f8stage=%d list=%p count=%u first={x:%.1f y:%.1f w:%.1f h:%.1f min:%.3f max:%.3f}",(unsigned long long)n,native_stereo_outer_lab::BridgeStage(),list,count,double(v.TopLeftX),double(v.TopLeftY),double(v.Width),double(v.Height),double(v.MinDepth),double(v.MaxDepth)); diagnostic_trace::Write(m); }
    }
    if(s_original_rs_set_viewports) s_original_rs_set_viewports(list,count,vps);
}

void __stdcall DeviceHook::DetourRSSetScissorRects(ID3D12GraphicsCommandList* list, UINT count, const D3D12_RECT* rs)
{
    if(s_output_layout_probe_enabled.load(std::memory_order_relaxed) && list!=s_internal_vr_command_list.load() && list!=s_internal_menu_command_list.load() && rs && count){
        std::uint64_t n=0; if(V6570LayoutLogBudget(s_output_layout_event_count,n)){ const auto& r=rs[0]; char m[320]{}; std::snprintf(m,sizeof(m),"V65.71-F9-SCISSOR n=%llu f8stage=%d list=%p count=%u first={l:%ld t:%ld r:%ld b:%ld}",(unsigned long long)n,native_stereo_outer_lab::BridgeStage(),list,count,r.left,r.top,r.right,r.bottom); diagnostic_trace::Write(m); }
    }
    if(s_original_rs_set_scissor_rects) s_original_rs_set_scissor_rects(list,count,rs);
}
bool DeviceHook::InstallOMSetRenderTargetsHook(ID3D12GraphicsCommandList* list)
{
    if (!list || s_om_set_rt_hook_installed.load(std::memory_order_acquire)) return true;
    void** vtable = *reinterpret_cast<void***>(list);
    constexpr size_t kOMSetRenderTargetsIndex = 46;
    void* target = vtable[kOMSetRenderTargetsIndex];
    hook::InlineRequest req{"kcd2vr_v616_omsetrt_probe", hook::Target{DetourModKit::Address{reinterpret_cast<uintptr_t>(target)}}, hook::Options{}};
    auto result = hook::inline_at(std::move(req), &DeviceHook::DetourOMSetRenderTargets);
    if (!result) { KCD2VR_DIAG_LOG(Bug,warning,"V61.6-NATIVE-RT: OMSetRenderTargets hook install failed"); return false; }
    s_hook_om_set_render_targets = std::move(*result);
    s_original_om_set_render_targets = s_hook_om_set_render_targets->original<OMSetRenderTargetsFunc>();
    if (!s_hook_om_set_render_targets->enable()) { s_original_om_set_render_targets=nullptr; s_hook_om_set_render_targets.reset(); return false; }
    s_om_set_rt_hook_installed.store(true, std::memory_order_release);
    debug_trace::Write("V61.6-NATIVE-RT OMSetRenderTargets observer armed; no game state writes");
    return true;
}

void __stdcall DeviceHook::DetourCreateRenderTargetView(
    ID3D12Device* device_this, ID3D12Resource* resource,
    const D3D12_RENDER_TARGET_VIEW_DESC* desc, D3D12_CPU_DESCRIPTOR_HANDLE dest)
{
    if (resource && dest.ptr) {
        std::lock_guard<std::mutex> lock(s_rtv_map_mutex);
        s_rtv_resource_map[dest.ptr] = resource;
        if (desc) s_rtv_desc_map[dest.ptr] = *desc;
        else s_rtv_desc_map.erase(dest.ptr); // default RTV; resource desc remains authoritative
    }
    if (s_original_create_rtv) s_original_create_rtv(device_this, resource, desc, dest);
}

static void V6128DescribeStereoCandidate(ID3D12Resource* resource, const D3D12_RESOURCE_DESC& rd, std::uint64_t unique_index) noexcept
{
    char debug_name[128]{}; UINT bytes=static_cast<UINT>(sizeof(debug_name)-1);
    if(resource) {
        const HRESULT hr=resource->GetPrivateData(WKPDID_D3DDebugObjectName,&bytes,debug_name);
        if(FAILED(hr)) debug_name[0]=0;
        else debug_name[sizeof(debug_name)-1]=0;
    }
    char msg[448]{};
    std::snprintf(msg,sizeof(msg),
        "V64.1-STEREO-PRODUCER UNIQUE n=%llu resource=%p size=%llux%u array=%u mips=%u fmt=%u flags=0x%X name='%s'",
        static_cast<unsigned long long>(unique_index),resource,static_cast<unsigned long long>(rd.Width),rd.Height,
        static_cast<unsigned>(rd.DepthOrArraySize),static_cast<unsigned>(rd.MipLevels),
        static_cast<unsigned>(rd.Format),static_cast<unsigned>(rd.Flags),debug_name[0]?debug_name:"<unnamed>");
    debug_trace::Write(msg);
}

void __stdcall DeviceHook::DetourOMSetRenderTargets(
    ID3D12GraphicsCommandList* list_this, UINT num_rt,
    const D3D12_CPU_DESCRIPTOR_HANDLE* rt_handles, BOOL single_range,
    const D3D12_CPU_DESCRIPTOR_HANDLE* dsv)
{
    const bool internal_vr = list_this == s_internal_vr_command_list.load(std::memory_order_acquire);
    const bool internal_menu = list_this == s_internal_menu_command_list.load(std::memory_order_acquire);

    // V66.64 HUD LAYER: after RIGHT was preserved and before PRESENT, KCD2 binds the main
    // backbuffer to draw the HUD Flash movie. Redirect exactly those binds to the private
    // transparent HUD texture (same size/format, same DSV). Nothing else is touched.
    if(g_v6624_late_window.load(std::memory_order_acquire) && hud_layer_v6664::Enabled() &&
       !internal_vr && !internal_menu && list_this && rt_handles && num_rt==1 && s_original_om_set_render_targets){
        ID3D12Resource* rr=nullptr;
        { std::lock_guard<std::mutex> ml(s_rtv_map_mutex); auto it=s_rtv_resource_map.find(rt_handles[0].ptr); if(it!=s_rtv_resource_map.end()) rr=it->second; }
        ID3D12Resource* main=nullptr;
        { std::lock_guard<std::mutex> bl(s_bridge_capture_mutex); main=s_bridge_current_main_target; }
        if(rr && rr==main && hud_layer_v6664::RedirectMainBind(s_captured_device.load(std::memory_order_acquire),list_this,main,dsv,s_original_om_set_render_targets,rt_handles)){
            if(g_d82_temporal_tracking.load(std::memory_order_acquire))V6612BindTemporalOutputD82(list_this,0);
            return;
        }
    }

    // D82: retain the actual RTV binding for this recording. Native may cache a
    // bind made before FinalCompose, so the exact scoped Draw (below), rather
    // than the occurrence of another OM call, is the semantic completion proof.
    if(g_d82_temporal_tracking.load(std::memory_order_acquire) && !internal_vr && !internal_menu && list_this){
        ID3D12Resource* rr=nullptr;ID3D12Resource* main=nullptr;
        if(rt_handles&&num_rt==1){
            std::lock_guard<std::mutex> ml(s_rtv_map_mutex);
            const auto it=s_rtv_resource_map.find(rt_handles[0].ptr);
            if(it!=s_rtv_resource_map.end())rr=it->second;
        }
        {std::lock_guard<std::mutex> bl(s_bridge_capture_mutex);main=s_bridge_current_main_target;}
        V6612BindTemporalOutputD82(list_this,reinterpret_cast<std::uintptr_t>(rr&&rr==main?rr:nullptr));
    }

    // V66.12: exact D3D12 activity generated synchronously by WHGame+0x2543A6C.
    // The CPU compose hook owns the TLS scope.  This observer only resolves the
    // game's RTV descriptors to resources already seen by CreateRenderTargetView.
    if(g_v6612_tls_compose_slot>=0 && !internal_vr && !internal_menu && list_this && rt_handles && num_rt){
        struct LocalRt { ID3D12Resource* r; bool same; }; LocalRt local[8]{}; unsigned localCount=0;
        const UINT inc=s_rtv_descriptor_increment.load(std::memory_order_acquire);
        const UINT lim=std::min<UINT>(num_rt,8);
        for(UINT i=0;i<lim;++i){
            const SIZE_T key=single_range?(rt_handles[0].ptr+static_cast<SIZE_T>(i)*inc):rt_handles[i].ptr;
            ID3D12Resource* rr=nullptr;
            {
                std::lock_guard<std::mutex> ml(s_rtv_map_mutex);
                auto it=s_rtv_resource_map.find(key);
                if(it!=s_rtv_resource_map.end()) rr=it->second;
            }
            if(!rr) continue;
            bool same=false;
            {
                std::lock_guard<std::mutex> bl(s_bridge_capture_mutex);
                same=(rr==s_bridge_current_main_target);
            }
            local[localCount++]={rr,same};
        }
        int eye=-1;std::uint32_t frame=0,call=0;
        { std::lock_guard<std::mutex> lk(g_v6610_output_mutex); const int slot=g_v6612_tls_compose_slot;
          if(slot>=0 && slot<g_v6612_compose_count){ auto& rec=g_v6612_compose_records[static_cast<std::size_t>(slot)]; eye=rec.eye;frame=rec.frame;call=++rec.om_calls;
            for(unsigned i=0;i<localCount;++i){
                int found=-1;
                for(unsigned j=0;j<rec.rt_count;++j){
                    if(rec.rt[j]==local[i].r){ found=static_cast<int>(j); break; }
                }
                if(found<0 && rec.rt_count<8) found=static_cast<int>(rec.rt_count++);
                if(found>=0){
                    rec.rt[static_cast<unsigned>(found)]=local[i].r;
                    ++rec.rt_binds[static_cast<unsigned>(found)];
                    rec.rt_same_main[static_cast<unsigned>(found)]=rec.rt_same_main[static_cast<unsigned>(found)]||local[i].same;
                }
            }
          }
        }
        if(eye>=0 && call<=8){
            char cm[768]{};std::snprintf(cm,sizeof(cm),
                "V66.12-F9-COMPOSE-RT eye=%c frame=%u call=%u list=%p numRT=%u resolved=%u rt0=%p/main%d rt1=%p/main%d rt2=%p/main%d rt3=%p/main%d",
                eye?'R':'L',frame,call,reinterpret_cast<void*>(list_this),num_rt,localCount,
                localCount>0?reinterpret_cast<void*>(local[0].r):nullptr,localCount>0&&local[0].same?1:0,
                localCount>1?reinterpret_cast<void*>(local[1].r):nullptr,localCount>1&&local[1].same?1:0,
                localCount>2?reinterpret_cast<void*>(local[2].r):nullptr,localCount>2&&local[2].same?1:0,
                localCount>3?reinterpret_cast<void*>(local[3].r):nullptr,localCount>3&&local[3].same?1:0);
            diagnostic_trace::Write(cm);
        }
    }

    // V65.93: read-only semantic candidate selector. Clear on every OM bind so
    // SECONDARY-APPLY cannot inherit stale state. This does not mutate the game.
    if(g_v6593_mode.load(std::memory_order_relaxed)!=0 && !internal_vr && !internal_menu && list_this && native_stereo_outer_lab::BridgeStage()==3){
        g_v6593_tls_scene=nullptr; g_v6593_tls_list=nullptr; g_v6593_tls_slot=0;
        if(rt_handles && num_rt){
            std::uint64_t ps=0,pt=0,pw=0,pcf=0; std::uint32_t ph=0;
            GetLatestPresentIdentity(&ps,&pt,&pw,&ph,&pcf);
            const UINT inc=s_rtv_descriptor_increment.load(std::memory_order_acquire);
            for(UINT i=0;i<num_rt && i<8 && pw && ph;++i){
                const SIZE_T key=single_range?(rt_handles[0].ptr+static_cast<SIZE_T>(i)*inc):rt_handles[i].ptr;
                ID3D12Resource* rr=nullptr;
                { std::lock_guard<std::mutex> lk(s_rtv_map_mutex); auto it=s_rtv_resource_map.find(key); if(it!=s_rtv_resource_map.end()) rr=it->second; }
                if(!rr) continue; const auto d=rr->GetDesc();
                if(d.Dimension==D3D12_RESOURCE_DIMENSION_TEXTURE2D && d.Width==pw && d.Height==ph && d.DepthOrArraySize==1 &&
                   d.Format==DXGI_FORMAT_R16G16B16A16_FLOAT && (d.Flags&D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET)){
                    g_v6593_tls_scene=rr; g_v6593_tls_list=list_this; g_v6593_tls_slot=i; break;
                }
            }
        }
    }
    // D59R1 perf review: quiet draw/PSO hooks cannot consume this diagnostic map.
    // Keep the independent certified stereo main-writer block below active.
    if(!s_v6627_performance_quiet.load(std::memory_order_relaxed) &&
       (s_mode_pso_probe_enabled.load(std::memory_order_relaxed) || g_v6610_output_enabled.load(std::memory_order_acquire)) && !internal_vr && !internal_menu && list_this){
        std::lock_guard<std::mutex> lock(s_mode_pso_mutex); auto& st=s_mode_list_state[list_this]; st.rt_count=0;
        if(rt_handles && num_rt){ const UINT inc=s_rtv_descriptor_increment.load(std::memory_order_acquire); const UINT lim=std::min<UINT>(num_rt,4);
            for(UINT i=0;i<lim;++i){ SIZE_T key=single_range?(rt_handles[0].ptr+static_cast<SIZE_T>(i)*inc):rt_handles[i].ptr; ID3D12Resource* rr=nullptr; { std::lock_guard<std::mutex> rtlock(s_rtv_map_mutex); auto it=s_rtv_resource_map.find(key); if(it!=s_rtv_resource_map.end()) rr=it->second; } st.rt[i]=rr; st.rtv[i].ptr=key; st.rt_count=i+1; }
        }
    }
    if(g_v6610_output_enabled.load(std::memory_order_acquire) && !internal_vr && !internal_menu && list_this && rt_handles && num_rt){
        const int eye=g_v6610_output_eye.load(std::memory_order_acquire);
        if(eye>=0 && eye<=1){
            std::uint64_t ps=0,pt=0,pw=0,pcf=0; std::uint32_t ph=0; GetLatestPresentIdentity(&ps,&pt,&pw,&ph,&pcf);
            const UINT inc=s_rtv_descriptor_increment.load(std::memory_order_acquire);
            const UINT lim=std::min<UINT>(num_rt,8);
            bool v6612SawScreen=false, v6612SawMain=false;
            for(UINT i=0;i<lim;++i){
                const SIZE_T key=single_range?(rt_handles[0].ptr+static_cast<SIZE_T>(i)*inc):rt_handles[i].ptr;
                ID3D12Resource* rr=nullptr;
                { std::lock_guard<std::mutex> ml(s_rtv_map_mutex); auto it=s_rtv_resource_map.find(key); if(it!=s_rtv_resource_map.end())rr=it->second; }
                if(!rr)continue; const auto rd=rr->GetDesc();
                if(rd.Dimension!=D3D12_RESOURCE_DIMENSION_TEXTURE2D || !(rd.Flags&D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET))continue;
                const bool screenish=(pw&&ph)?(rd.Width>=pw/2 && rd.Height>=ph/2):(rd.Width>=640 && rd.Height>=480);
                if(!screenish)continue;
                bool sameMain=false; { std::lock_guard<std::mutex> bl(s_bridge_capture_mutex); sameMain=(rr==s_bridge_current_main_target); }
                v6612SawScreen=true; v6612SawMain=v6612SawMain||sameMain;
                int idx=-1;
                { std::lock_guard<std::mutex> lk(g_v6610_output_mutex);
                  for(int j=0;j<g_v6610_output_count;++j)if(g_v6610_output_candidates[static_cast<std::size_t>(j)].resource==rr){idx=j;break;}
                  if(idx<0 && g_v6610_output_count<(int)g_v6610_output_candidates.size()){idx=g_v6610_output_count++; auto& c=g_v6610_output_candidates[static_cast<std::size_t>(idx)]; c.resource=rr;c.desc=rd;c.same_main=sameMain;}
                  if(idx>=0){auto& c=g_v6610_output_candidates[static_cast<std::size_t>(idx)];c.census_only=false;c.eye_mask|=(1u<<eye);++c.binds[eye];c.last_frame[eye]=g_v6610_output_frame.load(std::memory_order_relaxed);c.same_main=c.same_main||sameMain;
                    V6647NoteAccess(c,eye,true);}
                }
                const auto n=g_v6610_output_event.fetch_add(1,std::memory_order_relaxed)+1;
                if(n<=96){ char m[512]{}; std::snprintf(m,sizeof(m),
                    "V66.12-F9-RTV n=%llu eye=%c frame=%u idx=%d list=%p slot=%u numRT=%u resource=%p size=%llux%u arr=%u fmt=%u flags=0x%X layout=%u sameMainBackbuffer=%d present=%llux%u",
                    (unsigned long long)n,eye?'R':'L',g_v6610_output_frame.load(std::memory_order_relaxed),idx,list_this,i,num_rt,rr,
                    (unsigned long long)rd.Width,rd.Height,(unsigned)rd.DepthOrArraySize,(unsigned)rd.Format,(unsigned)rd.Flags,(unsigned)rd.Layout,sameMain?1:0,
                    (unsigned long long)pw,ph); diagnostic_trace::Write(m); }
            }
            if(v6612SawScreen) V6612TagList(list_this,eye,g_v6610_output_frame.load(std::memory_order_relaxed),true,false,v6612SawMain,false);
        }
    }
    if(s_output_layout_probe_enabled.load(std::memory_order_relaxed) && !internal_vr && !internal_menu && rt_handles && num_rt){
        std::uint64_t n=0; if(V6570LayoutLogBudget(s_output_layout_event_count,n)){
            const UINT inc=s_rtv_descriptor_increment.load(std::memory_order_acquire); SIZE_T key=rt_handles[0].ptr; ID3D12Resource* rr=nullptr; D3D12_RENDER_TARGET_VIEW_DESC vd{}; bool havevd=false;
            { std::lock_guard<std::mutex> lock(s_rtv_map_mutex); auto it=s_rtv_resource_map.find(key); if(it!=s_rtv_resource_map.end()) rr=it->second; auto di=s_rtv_desc_map.find(key); if(di!=s_rtv_desc_map.end()){vd=di->second;havevd=true;} }
            D3D12_RESOURCE_DESC rd{}; if(rr) rd=rr->GetDesc();
            UINT firstSlice=0,arraySize=0,mip=0; if(havevd){ if(vd.ViewDimension==D3D12_RTV_DIMENSION_TEXTURE2DARRAY){firstSlice=vd.Texture2DArray.FirstArraySlice;arraySize=vd.Texture2DArray.ArraySize;mip=vd.Texture2DArray.MipSlice;} else if(vd.ViewDimension==D3D12_RTV_DIMENSION_TEXTURE2D){mip=vd.Texture2D.MipSlice;} }
            char m[512]{}; std::snprintf(m,sizeof(m),"V65.71-F9-RT n=%llu f8stage=%d list=%p numRT=%u single=%d rtv0=0x%llX res=%p size=%llux%u arr=%u fmt=%u viewDim=%u mip=%u firstSlice=%u sliceCount=%u dsv=0x%llX",(unsigned long long)n,native_stereo_outer_lab::BridgeStage(),list_this,num_rt,single_range?1:0,(unsigned long long)key,rr,(unsigned long long)rd.Width,rd.Height,rd.DepthOrArraySize,(unsigned)rd.Format,havevd?(unsigned)vd.ViewDimension:0,mip,firstSlice,arraySize,(unsigned long long)(dsv?dsv->ptr:0)); diagnostic_trace::Write(m);
        }
    }

    // V66.51 census: a depth-stencil binding counts as a WRITE for the history census, so the
    // depth buffer is no longer misclassified as "read first" (read-only; nothing is changed).
    if(dsv && dsv->ptr && g_v6610_output_enabled.load(std::memory_order_acquire)) {
        const int ceye=g_v6610_output_eye.load(std::memory_order_acquire);
        if(ceye==0 || ceye==1) {
            ID3D12Resource* dr=nullptr;
            { std::lock_guard<std::mutex> lock(s_dsv_map_mutex); auto it=s_dsv_resource_map.find(dsv->ptr); if(it!=s_dsv_resource_map.end()) dr=it->second; }
            if(dr) {
                std::lock_guard<std::mutex> lk(g_v6610_output_mutex);
                for(int j=0;j<g_v6610_output_count;++j) {
                    auto& c=g_v6610_output_candidates[static_cast<std::size_t>(j)];
                    if(c.resource!=dr) continue;
                    ++c.dsv_binds[ceye]; c.eye_mask|=(1u<<ceye);
                    V6647NoteAccess(c,ceye,true);
                    break;
                }
            }
        }
    }

    // V61.34 F4 generic depth observer: resolve the currently bound DSV to a
    // resource, then keep only a tiny TLS candidate index for the hot draw hooks.
    // No descriptor/resource contents or states are modified.
    s_tls_depth_candidate = -1;
    if (!internal_vr && !internal_menu && s_generic_depth_probe_enabled.load(std::memory_order_relaxed) && dsv && dsv->ptr) {
        ID3D12Resource* depth_resource=nullptr;
        { std::lock_guard<std::mutex> lock(s_dsv_map_mutex); auto it=s_dsv_resource_map.find(dsv->ptr); if(it!=s_dsv_resource_map.end()) depth_resource=it->second; }
        if(depth_resource){
            const auto dd=depth_resource->GetDesc();
            if(dd.Dimension==D3D12_RESOURCE_DIMENSION_TEXTURE2D && (dd.Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL)){
                int found=-1, empty=-1;
                for(std::size_t i=0;i<kGenericDepthCandidateCount;++i){ auto* r=s_generic_depth_candidates[i].resource.load(std::memory_order_acquire); if(r==depth_resource){found=static_cast<int>(i);break;} if(!r && empty<0) empty=static_cast<int>(i); }
                if(found<0 && empty>=0){
                    auto& c=s_generic_depth_candidates[static_cast<std::size_t>(empty)]; ID3D12Resource* expected=nullptr;
                    if(c.resource.compare_exchange_strong(expected,depth_resource,std::memory_order_acq_rel)){ c.width=dd.Width; c.height=dd.Height; c.format=dd.Format; c.flags=dd.Flags; found=empty; }
                    else { for(std::size_t i=0;i<kGenericDepthCandidateCount;++i) if(s_generic_depth_candidates[i].resource.load(std::memory_order_acquire)==depth_resource){found=static_cast<int>(i);break;} }
                }
                if(found>=0){
                    auto& c=s_generic_depth_candidates[static_cast<std::size_t>(found)]; c.binds.fetch_add(1,std::memory_order_relaxed);
                    std::uint64_t ps=0,pt=0,pw=0,pc=0; std::uint32_t ph=0; GetLatestPresentIdentity(&ps,&pt,&pw,&ph,&pc);
                    if(pw>0 && ph>0){ const double wr=double(dd.Width)/double(pw), hr=double(dd.Height)/double(ph); const bool sm=std::fabs(wr-1.0)<=0.03 && std::fabs(hr-1.0)<=0.03; const double da=double(dd.Width)/double(dd.Height), pa=double(pw)/double(ph); const bool am=std::fabs(da-pa)<=0.06; if(sm)c.screen_match.store(true,std::memory_order_relaxed); if(am)c.aspect_match.store(true,std::memory_order_relaxed); }
                    s_tls_depth_candidate=found;
                }
            }
        }
    }
    const bool stereo_broad = s_stereo_producer_probe_enabled.load(std::memory_order_acquire) &&
        (s_stereo_producer_probe_width.load(std::memory_order_acquire)==0 || s_stereo_producer_probe_height.load(std::memory_order_acquire)==0);
    if (!internal_vr && !internal_menu && rt_handles && num_rt > 0 && (FrameSync::CurrentPipelineViewType() == 0 || stereo_broad)) {
        const UINT inc = s_rtv_descriptor_increment.load(std::memory_order_acquire);

        // V65.87: latch MRT0/MRT1 from one single OMSetRenderTargets call, but
        // only after COUNT2 has already crossed at least one PRESENT since arm.
        // This prevents the activation-transition frame from being mistaken for
        // the actual steady-state COUNT2 pair.
        if(g_v6587_enabled.load(std::memory_order_acquire) && native_stereo_outer_lab::BridgeStage()==3 && num_rt>=2){
            std::uint64_t ps=0,pt=0,pw=0,pc=0; std::uint32_t ph=0;
            GetLatestPresentIdentity(&ps,&pt,&pw,&ph,&pc);
            bool need=false; { std::lock_guard<std::mutex> lk(g_v6587_mutex); need=(g_v6587_list==nullptr && ps>g_v6587_arm_present_serial); }
            if(need && pw>0 && ph>0){
                ID3D12Resource* pair[2]={nullptr,nullptr};
                for(int e=0;e<2;++e){
                    SIZE_T key=single_range ? (rt_handles[0].ptr + static_cast<SIZE_T>(e)*inc) : rt_handles[e].ptr;
                    std::lock_guard<std::mutex> ml(s_rtv_map_mutex);
                    auto it=s_rtv_resource_map.find(key); if(it!=s_rtv_resource_map.end()) pair[e]=it->second;
                }
                if(pair[0] && pair[1] && pair[0]!=pair[1]){
                    const auto d0=pair[0]->GetDesc(); const auto d1=pair[1]->GetDesc();
                    const bool ok0=d0.Dimension==D3D12_RESOURCE_DIMENSION_TEXTURE2D && (d0.Flags&D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET) && d0.Width==pw && d0.Height==ph && d0.DepthOrArraySize==1;
                    const bool ok1=d1.Dimension==D3D12_RESOURCE_DIMENSION_TEXTURE2D && (d1.Flags&D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET) && d1.Width==pw && d1.Height==ph && d1.DepthOrArraySize==1;
                    if(ok0 && ok1){
                        std::lock_guard<std::mutex> lk(g_v6587_mutex);
                        if(g_v6587_list==nullptr){
                            pair[0]->AddRef(); pair[1]->AddRef(); g_v6587_source[0]=pair[0]; g_v6587_source[1]=pair[1]; g_v6587_list=list_this;
                            char m[448]{}; std::snprintf(m,sizeof(m),
                                "V65.87-PAIR LATCH present=%llu list=%p MRT0=%p fmt0=%u MRT1=%p fmt1=%u size=%llux%u numRT=%u",
                                (unsigned long long)ps,list_this,pair[0],(unsigned)d0.Format,pair[1],(unsigned)d1.Format,(unsigned long long)d0.Width,d0.Height,num_rt); diagnostic_trace::Write(m);
                        }
                    }
                }
            }
        }
        for (UINT i=0; i<num_rt; ++i) {
            SIZE_T key = single_range ? (rt_handles[0].ptr + static_cast<SIZE_T>(i) * inc) : rt_handles[i].ptr;
            ID3D12Resource* resource = nullptr;
            {
                std::lock_guard<std::mutex> lock(s_rtv_map_mutex);
                auto it = s_rtv_resource_map.find(key);
                if (it != s_rtv_resource_map.end()) resource = it->second;
            }
            if (!resource) continue;
            const auto rd = resource->GetDesc();
            if (rd.Dimension != D3D12_RESOURCE_DIMENSION_TEXTURE2D || !(rd.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET)) continue;

            // V65.85: select the first four distinct full-size color RTVs during
            // COUNT2. Selection is read-only; owned snapshots are made later at
            // the exact RT->next-state barrier.
            if (g_v6585_enabled.load(std::memory_order_acquire) && native_stereo_outer_lab::BridgeStage()==3) {
                std::uint64_t ps=0,pt=0,pw=0,pc=0; std::uint32_t ph=0;
                GetLatestPresentIdentity(&ps,&pt,&pw,&ph,&pc);
                const bool full = pw>0 && ph>0 && rd.Width==pw && rd.Height==ph && rd.DepthOrArraySize==1;
                if(full){
                    std::lock_guard<std::mutex> lk(g_v6585_mutex);
                    int found=-1; for(int j=0;j<g_v6585_count;++j) if(g_v6585_source[j]==resource){ found=j; break; }
                    if(found<0 && g_v6585_count<4){
                        const int j=g_v6585_count++;
                        resource->AddRef(); g_v6585_source[j]=resource; g_v6585_list[j]=list_this;
                        char vm[448]{}; std::snprintf(vm,sizeof(vm),
                            "V65.85-CANDIDATE SELECT idx=%d list=%p resource=%p size=%llux%u fmt=%u flags=0x%X layout=%u numRT=%u slot=%u",
                            j,list_this,resource,(unsigned long long)rd.Width,rd.Height,(unsigned)rd.Format,(unsigned)rd.Flags,(unsigned)rd.Layout,num_rt,i);
                        diagnostic_trace::Write(vm);
                    }
                }
            }

            // V65.85 MRT census: retain all color slots, including array/slice metadata.
            if (g_v6585_enabled.load(std::memory_order_acquire) && native_stereo_outer_lab::BridgeStage()==3 && num_rt>1) {
                const auto n=g_v6585_event.fetch_add(1,std::memory_order_relaxed)+1;
                if(n<=96){ char mm[320]{}; std::snprintf(mm,sizeof(mm),
                    "V65.85-MRT n=%llu list=%p numRT=%u slot=%u resource=%p size=%llux%u fmt=%u arr=%u layout=%u",
                    (unsigned long long)n,list_this,num_rt,i,resource,(unsigned long long)rd.Width,rd.Height,(unsigned)rd.Format,(unsigned)rd.DepthOrArraySize,(unsigned)rd.Layout); diagnostic_trace::Write(mm); }
            }

            // V61.28 F8: exact-size native-stereo producer archaeology. The shipped
            // Stereo.cfx final compositor expects distinct StereoL/StereoR textures.
            // Instead of re-entering a camera/view again, observe whether KCD2 binds
            // multiple distinct source-sized render targets in the same present epoch.
            if (s_stereo_producer_probe_enabled.load(std::memory_order_acquire)) {
                const auto tw=s_stereo_producer_probe_width.load(std::memory_order_acquire);
                const auto th=s_stereo_producer_probe_height.load(std::memory_order_acquire);
                std::uint64_t present_serial=0,present_tick=0,present_width=0,present_camera=0; std::uint32_t present_height=0;
                GetLatestPresentIdentity(&present_serial,&present_tick,&present_width,&present_height,&present_camera);
                const bool exact = tw>0 && th>0 && rd.Width==static_cast<UINT64>(tw) && rd.Height==th;
                // V64.1 F8 broad mode fixes the V61.30 blind spot: do not require
                // exact source dimensions or a Normal-view TLS classification.
                // Accept large screen-ish color RTs, including texture arrays and
                // differently-sized transient eye/intermediate resources.
                const bool broad = (tw==0 || th==0) &&
                    ((present_width>0 && present_height>0 && rd.Width>=std::max<UINT64>(640,present_width/2) && rd.Height>=std::max<UINT>(640,present_height/2)) ||
                     (present_width==0 && rd.Width>=960 && rd.Height>=900));
                if (exact || broad) {
                    // V65.58: this probe is armed by the proven count=2 native pipeline and persists only through the next PRESENT.
                    // Log a bounded descriptor-level route trace so same-resource array-slice stereo
                    // is distinguishable from two-texture stereo and from a collapsed mono target.
                    D3D12_RENDER_TARGET_VIEW_DESC vd{}; bool have_vd=false;
                    {
                        std::lock_guard<std::mutex> lock(s_rtv_map_mutex);
                        auto it=s_rtv_desc_map.find(key);
                        if(it!=s_rtv_desc_map.end()){vd=it->second;have_vd=true;}
                    }
                    // V65.61: visual-identity gate. Select only two consecutive distinct full-resolution
                    // DXGI_FORMAT_R8G8B8A8_UNORM (28) 2D RTVs on the SAME command list.  We only
                    // remember live sources here; actual snapshots are taken later at their real
                    // RENDER_TARGET -> next-state barriers in DetourResourceBarrier.
                    if (!g_v6561_pair_selected) {
                        const bool fmt28 = rd.Format == DXGI_FORMAT_R8G8B8A8_UNORM && rd.DepthOrArraySize==1;
                        const bool full = present_width>0 && present_height>0 && rd.Width==present_width && rd.Height==present_height;
                        std::lock_guard<std::mutex> lk(g_v6561_candidate_mutex);
                        if (fmt28 && full) {
                            if (g_v6561_prev_fmt28 && g_v6561_prev_fmt28!=resource && g_v6561_prev_fmt28_list==list_this) {
                                ID3D12Resource* candidate_a=g_v6561_prev_fmt28;
                                candidate_a->AddRef(); resource->AddRef();
                                V6561ClearCandidatePairLocked();
                                g_v6561_candidate_source[0]=candidate_a;
                                g_v6561_candidate_source[1]=resource;
                                g_v6561_candidate_list=list_this; g_v6561_candidate_desc=rd; g_v6561_pair_selected=true;
                                ++g_v6561_pair_serial;
                                char cm[320]{}; std::snprintf(cm,sizeof(cm),
                                    "V65.61-CANDIDATE-PAIR SELECT serial=%llu list=%p A=%p B=%p size=%llux%u fmt=%u gate=consecutive-same-list",
                                    (unsigned long long)g_v6561_pair_serial,list_this,g_v6561_candidate_source[0],g_v6561_candidate_source[1],
                                    (unsigned long long)rd.Width,rd.Height,(unsigned)rd.Format); diagnostic_trace::Write(cm);
                            } else {
                                g_v6561_prev_fmt28=resource; g_v6561_prev_fmt28_list=list_this;
                            }
                        } else {
                            g_v6561_prev_fmt28=nullptr; g_v6561_prev_fmt28_list=nullptr;
                        }
                    }
                    const auto detailn=s_stereo_producer_detailed_bind_count.fetch_add(1,std::memory_order_relaxed)+1;
                    if(detailn<=64) {
                        unsigned mip=0, firstSlice=0, sliceCount=1, plane=0;
                        const unsigned dim=have_vd?static_cast<unsigned>(vd.ViewDimension):0u;
                        if(have_vd) {
                            switch(vd.ViewDimension) {
                            case D3D12_RTV_DIMENSION_TEXTURE2D:
                                mip=vd.Texture2D.MipSlice; plane=vd.Texture2D.PlaneSlice; break;
                            case D3D12_RTV_DIMENSION_TEXTURE2DARRAY:
                                mip=vd.Texture2DArray.MipSlice; firstSlice=vd.Texture2DArray.FirstArraySlice;
                                sliceCount=vd.Texture2DArray.ArraySize; plane=vd.Texture2DArray.PlaneSlice; break;
                            case D3D12_RTV_DIMENSION_TEXTURE2DMSARRAY:
                                firstSlice=vd.Texture2DMSArray.FirstArraySlice; sliceCount=vd.Texture2DMSArray.ArraySize; break;
                            case D3D12_RTV_DIMENSION_TEXTURE3D:
                                mip=vd.Texture3D.MipSlice; firstSlice=vd.Texture3D.FirstWSlice; sliceCount=vd.Texture3D.WSize; break;
                            default: break;
                            }
                        }
                        const int oracle_stage=s_stereo_producer_oracle_stage.load(std::memory_order_acquire);
                        const char* oracle_name=oracle_stage==1?"LEFT":(oracle_stage==2?"RIGHT":(oracle_stage==3?"COUNT2":"UNLABELED"));
                        char rm[560]{}; std::snprintf(rm,sizeof(rm),
                            "V65.63-ROUTE BIND stage=%d eye=%s n=%llu list=%p rtv=0x%llX resource=%p size=%llux%u resourceArray=%u fmt=%u viewDesc=%d viewDim=%u mip=%u firstSlice=%u sliceCount=%u plane=%u",
                            oracle_stage,oracle_name,static_cast<unsigned long long>(detailn),list_this,static_cast<unsigned long long>(key),resource,
                            static_cast<unsigned long long>(rd.Width),rd.Height,static_cast<unsigned>(rd.DepthOrArraySize),
                            static_cast<unsigned>(rd.Format),have_vd?1:0,dim,mip,firstSlice,sliceCount,plane);
                        diagnostic_trace::Write(rm); debug_trace::Write(rm);
                    }
                    const auto bindn=s_stereo_producer_bind_count.fetch_add(1,std::memory_order_relaxed)+1;
                    const std::uint64_t epoch=present_serial, pt=present_tick, pw=present_width, pc=present_camera;
                    const std::uint32_t ph=present_height;
                    const auto oldEpoch=s_stereo_producer_epoch.load(std::memory_order_acquire);
                    if(epoch!=oldEpoch) {
                        s_stereo_producer_epoch.store(epoch,std::memory_order_release);
                        s_stereo_producer_first_resource.store(resource,std::memory_order_release);
                    } else {
                        ID3D12Resource* first=s_stereo_producer_first_resource.load(std::memory_order_acquire);
                        if(first && first!=resource) {
                            auto pairEpoch=s_stereo_producer_pair_epoch.load(std::memory_order_acquire);
                            if(pairEpoch!=epoch && s_stereo_producer_pair_epoch.compare_exchange_strong(pairEpoch,epoch,std::memory_order_acq_rel)) {
                                const auto pairs=s_stereo_producer_pair_count.fetch_add(1,std::memory_order_relaxed)+1;
                                if(pairs<=12 || (pairs%120ull)==0) {
                                    char pm[384]{}; std::snprintf(pm,sizeof(pm),
                                        "V64.1-STEREO-PRODUCER PAIR n=%llu epoch=%llu A=%p B=%p candidateB=%llux%u array=%u fmt=%u requested=%ux%u broad=%d cameraFrame=%llu",
                                        static_cast<unsigned long long>(pairs),static_cast<unsigned long long>(epoch),first,resource,
                                        static_cast<unsigned long long>(rd.Width),rd.Height,static_cast<unsigned>(rd.DepthOrArraySize),static_cast<unsigned>(rd.Format),tw,th,broad?1:0,
                                        static_cast<unsigned long long>(pc)); debug_trace::Write(pm);
                                }
                            }
                        }
                    }
                    // Fixed four-slot uniqueness tracker: zero allocation and no extra mutex on the render thread.
                    std::atomic<ID3D12Resource*>* slots[4]={&s_stereo_producer_unique0,&s_stereo_producer_unique1,&s_stereo_producer_unique2,&s_stereo_producer_unique3};
                    bool known=false;
                    for(auto* slot:slots) if(slot->load(std::memory_order_acquire)==resource){known=true;break;}
                    if(!known) {
                        for(auto* slot:slots) {
                            ID3D12Resource* expected=nullptr;
                            if(slot->compare_exchange_strong(expected,resource,std::memory_order_acq_rel)) {
                                const auto un=s_stereo_producer_unique_count.fetch_add(1,std::memory_order_relaxed)+1;
                                V6128DescribeStereoCandidate(resource,rd,un);
                                break;
                            }
                        }
                    }
                    (void)bindn;
                }
            }
            // Full-resolution-ish targets are the only candidates relevant to a hidden StereoL/StereoR pair.
            if (rd.Width < 1600 || rd.Height < 1400) continue;
            const auto n = s_native_rt_bind_count.fetch_add(1, std::memory_order_relaxed) + 1;
            if (n <= 16 || (n % 12000ull)==0) {
                std::uint64_t present_serial=0, present_tick=0, present_width=0, present_camera=0;
                std::uint32_t present_height=0;
                GetLatestPresentIdentity(&present_serial,&present_tick,&present_width,&present_height,&present_camera);
                char msg[448]{};
                std::snprintf(msg,sizeof(msg),
                    "V61.6-NATIVE-RT-BIND n=%llu presentSerial=%llu list=%p slot=%u/%u handle=0x%llX resource=%p size=%llux%u fmt=%u flags=0x%X singleRange=%s cameraFrame=%llu tid=%lu",
                    static_cast<unsigned long long>(n), static_cast<unsigned long long>(present_serial), list_this,
                    i, num_rt, static_cast<unsigned long long>(key), resource,
                    static_cast<unsigned long long>(rd.Width), rd.Height, static_cast<unsigned>(rd.Format), static_cast<unsigned>(rd.Flags),
                    single_range?"true":"false", static_cast<unsigned long long>(present_camera), static_cast<unsigned long>(GetCurrentThreadId()));
                debug_trace::Write(msg);
            }
        }
    }
    if (s_original_om_set_render_targets) s_original_om_set_render_targets(list_this,num_rt,rt_handles,single_range,dsv);
}

void DeviceHook::SetStereoProducerProbe(bool enabled, std::uint32_t width, std::uint32_t height)
{
    {
        std::lock_guard<std::mutex> lk(g_v6561_candidate_mutex);
        // V65.88: do not destroy a working pair before its command list is submitted.
        // The next COUNT2 packet clears the working slots only after ExecuteCommandLists
        // has promoted that pair to the persistent live publication. OFF still clears now.
        const bool pendingLive = enabled && g_v6588_live_enabled.load(std::memory_order_acquire) &&
            g_v6561_pair_selected && g_v6561_pair_serial!=g_v6588_source_pair_serial.load(std::memory_order_acquire);
        if(!pendingLive) V6561ClearCandidatePairLocked();
    }
    s_stereo_producer_probe_width.store(width,std::memory_order_release);
    s_stereo_producer_probe_height.store(height,std::memory_order_release);
    s_stereo_producer_bind_count.store(0,std::memory_order_release);
    s_stereo_producer_unique_count.store(0,std::memory_order_release);
    s_stereo_producer_pair_count.store(0,std::memory_order_release);
    s_stereo_producer_epoch.store(~0ull,std::memory_order_release);
    s_stereo_producer_pair_epoch.store(~0ull,std::memory_order_release);
    s_stereo_producer_first_resource.store(nullptr,std::memory_order_release);
    s_stereo_producer_unique0.store(nullptr,std::memory_order_release);
    s_stereo_producer_unique1.store(nullptr,std::memory_order_release);
    s_stereo_producer_unique2.store(nullptr,std::memory_order_release);
    s_stereo_producer_unique3.store(nullptr,std::memory_order_release);
    s_stereo_producer_detailed_bind_count.store(0,std::memory_order_release);
    if(!enabled) s_stereo_producer_auto_stop_present_serial.store(0,std::memory_order_release);
    s_stereo_producer_probe_enabled.store(enabled,std::memory_order_release);
    char m[320]{}; std::snprintf(m,sizeof(m),
        "V64.1-STEREO-PRODUCER %s target=%ux%u mode=%s; broad mode removes exact-size and Normal-view TLS restriction; renderer/camera writes=NONE",
        enabled?"ON":"OFF",width,height,(width==0||height==0)?"BROAD":"EXACT"); debug_trace::Write(m);
}
std::uint64_t DeviceHook::StereoProducerSameFramePairCount(){return s_stereo_producer_pair_count.load(std::memory_order_acquire);}

void DeviceHook::ArmStereoProducerProbeThroughNextPresent(int oracle_stage)
{
    SetStereoProducerProbe(true,0,0);
    s_stereo_producer_oracle_stage.store(oracle_stage,std::memory_order_release);
    std::uint64_t baseline=0;
    {
        std::lock_guard<std::mutex> lock(s_present_resource_mutex);
        baseline=s_latest_present_serial;
    }
    const auto target=baseline+1ull;
    s_stereo_producer_auto_stop_present_serial.store(target,std::memory_order_release);
    char m[256]{}; std::snprintf(m,sizeof(m),
        "V65.63-ROUTE WINDOW ARM stage=%d eye=%s baselinePresent=%llu stopAtPresent=%llu scope=ASYNC-UNTIL-NEXT-PRESENT",
        oracle_stage,oracle_stage==1?"LEFT":(oracle_stage==2?"RIGHT":(oracle_stage==3?"COUNT2":"UNLABELED")),
        static_cast<unsigned long long>(baseline),static_cast<unsigned long long>(target));
    diagnostic_trace::Write(m);
    debug_trace::Write(m);
}





bool DeviceHook::TryGetV6588LiveStereoPair(ID3D12Resource** out_left, ID3D12Resource** out_right, std::uint64_t* out_serial)
{
    if(!out_left || !out_right) return false; *out_left=nullptr; *out_right=nullptr;
    std::lock_guard<std::mutex> lk(g_v6588_live_mutex);
    if(!g_v6588_live_pair[0] || !g_v6588_live_pair[1] || !g_v6588_live_serial) return false;
    g_v6588_live_pair[0]->AddRef(); g_v6588_live_pair[1]->AddRef();
    *out_left=g_v6588_live_pair[0]; *out_right=g_v6588_live_pair[1];
    if(out_serial) *out_serial=g_v6588_live_serial;
    return true;
}




bool DeviceHook::TryGetV6585Candidate(int index, ID3D12Resource** out_resource, std::uint64_t* out_serial)
{
    if(!out_resource) return false; *out_resource=nullptr;
    if(index<0 || index>=4) return false;
    std::lock_guard<std::mutex> lk(g_v6585_mutex);
    if(!g_v6585_enabled.load(std::memory_order_acquire) || index>=g_v6585_count || !g_v6585_snapshot[index] || g_v6585_serial[index]==0) return false;
    g_v6585_snapshot[index]->AddRef(); *out_resource=g_v6585_snapshot[index];
    if(out_serial) *out_serial=g_v6585_serial[index];
    return true;
}

int DeviceHook::V6585CandidateCount()
{
    std::lock_guard<std::mutex> lk(g_v6585_mutex); return g_v6585_count;
}

namespace {
struct V6610ExactMatch {
    int candidate=-1;
    int depth=-1;
    int anchor=0; // 1=texture,2=backing,3=native RTV view
    std::uintptr_t match_at=0;
    std::uintptr_t via=0;
    std::size_t offset0=0,offset1=0;
};

bool V6610ReadMemory(std::uintptr_t address,void* out,std::size_t bytes) noexcept
{
    if(!address || !out || !bytes || address>~std::uintptr_t{0}-(bytes-1))return false;
    SIZE_T copied=0;
    return ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<const void*>(address),out,bytes,&copied)!=0 && copied==bytes;
}

bool V6610PlausiblePointer(std::uintptr_t p) noexcept
{
#if INTPTR_MAX == INT64_MAX
    return p>=0x10000ull && p<0x0000800000000000ull && (p&0x7ull)==0;
#else
    return p>=0x10000u && (p&0x3u)==0;
#endif
}

int V6610CandidateIndex(std::uintptr_t value) noexcept
{
    for(int i=0;i<g_v6610_output_count;++i)
        if(reinterpret_cast<std::uintptr_t>(g_v6610_output_candidates[static_cast<std::size_t>(i)].resource)==value)return i;
    return -1;
}

bool V6610ScanRegion(std::uintptr_t base,std::size_t bytes,int anchor,int depth,std::uintptr_t via,
                     std::size_t parent_offset,V6610ExactMatch& out) noexcept
{
    constexpr std::size_t kMaxQwords=128;
    if(!base || bytes<sizeof(std::uintptr_t))return false;
    const std::size_t count=std::min<std::size_t>(bytes/sizeof(std::uintptr_t),kMaxQwords);
    std::array<std::uintptr_t,kMaxQwords> words{};
    if(!V6610ReadMemory(base,words.data(),count*sizeof(std::uintptr_t)))return false;
    for(std::size_t i=0;i<count;++i){
        const int idx=V6610CandidateIndex(words[i]);
        if(idx>=0){
            out.candidate=idx;out.depth=depth;out.anchor=anchor;out.match_at=base+i*sizeof(std::uintptr_t);
            out.via=via;out.offset0=parent_offset;out.offset1=i*sizeof(std::uintptr_t);return true;
        }
    }
    return false;
}

V6610ExactMatch V6610FindExactMatch(const V6610SemanticOutput& o) noexcept
{
    V6610ExactMatch m{};
    struct Anchor{std::uintptr_t p;std::size_t bytes;int kind;};
    const Anchor anchors[]={{o.texture_object,0x180,1},{o.device_backing,0x400,2},{o.native_rtv_view,0x200,3}};
    for(const auto& a:anchors) if(V6610ScanRegion(a.p,a.bytes,a.kind,0,0,0,m))return m;
    // One pointer-indirection level is enough for the common CDeviceTexture ->
    // native resource wrapper -> ID3D12Resource ownership shape. Every read is
    // ReadProcessMemory fail-closed; no arbitrary pointer is dereferenced directly.
    for(const auto& a:anchors){
        constexpr std::size_t kParents=64;
        std::array<std::uintptr_t,kParents> words{};
        const std::size_t n=std::min<std::size_t>(a.bytes/sizeof(std::uintptr_t),kParents);
        if(!a.p || !V6610ReadMemory(a.p,words.data(),n*sizeof(std::uintptr_t)))continue;
        for(std::size_t i=0;i<n;++i){
            const auto child=words[i]; if(!V6610PlausiblePointer(child))continue;
            if(V6610ScanRegion(child,0x180,a.kind,1,child,i*sizeof(std::uintptr_t),m))return m;
        }
    }
    return m;
}

const char* V6610AnchorName(int a) noexcept
{
    return a==1?"CTexture":(a==2?"DeviceBacking":(a==3?"NativeRtvView":"NONE"));
}


V6612ListTag* V6612FindLiveLocked(ID3D12GraphicsCommandList* list) noexcept
{
    if(!list) return nullptr;
    for(std::size_t i=0;i<g_v6612_live_count;++i){
        if(g_v6612_live_lists[i].list==list) return &g_v6612_live_lists[i];
    }
    if(g_v6612_live_count>=g_v6612_live_lists.size()) return nullptr;
    auto& t=g_v6612_live_lists[g_v6612_live_count++];
    t={}; t.list=list; t.generation=1;
    return &t;
}

void V6612BindTemporalOutputD82(ID3D12GraphicsCommandList* list,std::uintptr_t output) noexcept
{
    if(!list||!g_v6613_pair_capture_active.load(std::memory_order_acquire))return;
    std::lock_guard<std::mutex> lk(g_v6612_list_mutex);
    V6612ListTag* t=nullptr;
    if(output)t=V6612FindLiveLocked(list);
    else for(std::size_t i=0;i<g_v6612_live_count;++i)if(g_v6612_live_lists[i].list==list){t=&g_v6612_live_lists[i];break;}
    if(t)t->temporal_recording.BindMain(output);
}

void V6612DiscardTemporalRecordingD82(ID3D12GraphicsCommandList* list) noexcept
{
    if(!list)return;
    std::lock_guard<std::mutex> lk(g_v6612_list_mutex);
    // Reset never allocates a tracking entry; only observed output binds do so.
    for(std::size_t i=0;i<g_v6612_live_count;++i)if(g_v6612_live_lists[i].list==list){
        g_v6612_live_lists[i].temporal_recording.Discard();break;
    }
}

void V6612DiscardMainWriterD85(ID3D12GraphicsCommandList* list) noexcept
{
    std::lock_guard<std::mutex> lk(g_v6612_list_mutex);
    for(std::size_t i=0;i<g_v6612_live_count;++i)if(g_v6612_live_lists[i].list==list){
        auto& t=g_v6612_live_lists[i];auto recording=t.temporal_recording;recording.Discard();
        const auto generation=t.generation+1;t={};t.list=list;t.generation=generation;t.temporal_recording=recording;break;
    }
}

void V6612ClearTemporalBindingsD82(ID3D12GraphicsCommandList* list) noexcept
{
    if(!list)return;
    std::lock_guard<std::mutex> lk(g_v6612_list_mutex);
    for(std::size_t i=0;i<g_v6612_live_count;++i)if(g_v6612_live_lists[i].list==list){
        g_v6612_live_lists[i].temporal_recording.ClearBindings();break;
    }
}

void V6612RecordTemporalDrawD82(ID3D12GraphicsCommandList* list,UINT primitives,UINT instances) noexcept
{
    if(!list||!primitives||!instances||!g_d82_temporal_tracking.load(std::memory_order_acquire))return;
    const bool scoped=g_d82_compose_scope.Active();
    if(scoped&&(!g_v6615_active.load(std::memory_order_acquire)||
       g_v6613_pair_frame.load(std::memory_order_acquire)!=g_d82_compose_scope.frame))return;
    if(!scoped&&!g_d82_composition_observed.load(std::memory_order_acquire))return;
    std::lock_guard<std::mutex> lk(g_v6612_list_mutex);
    for(std::size_t i=0;i<g_v6612_live_count;++i)if(g_v6612_live_lists[i].list==list){
        auto& recording=g_v6612_live_lists[i].temporal_recording;
        if(scoped){
            const bool recorded=recording.RecordDraw(reinterpret_cast<std::uintptr_t>(list),g_d82_compose_scope,primitives,instances);
            g_d82_compose_draw_seen=recorded||g_d82_compose_draw_seen;
            if(recorded){g_d82_composition_observed.store(true,std::memory_order_release);g_d85_watchdog.Reach(temporal_pair_d85::DrawRecorded);}
        }else{
            // A later known main-target Draw in this recording supersedes the
            // scoped composition. It must not inherit that draw's certificate.
            if(recording.ObserveUnscopedDraw(reinterpret_cast<std::uintptr_t>(list),primitives,instances))
                g_d85_watchdog.Refuse(temporal_pair_d85::Check::Superseded);
        }
        break;
    }
}

void V6612ResetTracking() noexcept
{
    std::lock_guard<std::mutex> lk(g_v6612_list_mutex);
    g_v6612_live_lists={}; g_v6612_live_count=0;
    g_d82_composition_observed.store(false,std::memory_order_release);
    g_v6612_submit_items={}; g_v6612_submit_count=0;
    g_v6612_event_seq.store(0,std::memory_order_release);
    g_v6612_drain_requested.store(false,std::memory_order_release);
    g_v6612_final_dump_done.store(false,std::memory_order_release);
    g_v6614_right_submitted_seen.store(false,std::memory_order_release);
    g_v6615_left_copy_queued.store(false,std::memory_order_release);
    g_v6615_right_copy_queued.store(false,std::memory_order_release);
    g_v6615_pair_published.store(false,std::memory_order_release);
    g_v6615_last_left_main_frame.store(0,std::memory_order_release);
    g_v6615_last_left_main_batch.store(0,std::memory_order_release);
}

void V6612TagList(ID3D12GraphicsCommandList* list,int eye,std::uint32_t frame,
                  bool om_call,bool barrier_call,bool main_bind,bool main_barrier,
                  bool present_boundary,ID3D12Resource* present_resource) noexcept
{
    if(!list || !g_v6613_pair_capture_active.load(std::memory_order_acquire)) return;
    const auto target=g_v6613_pair_frame.load(std::memory_order_acquire);
    // Before RIGHT starts, pair_frame==0 and LEFT warm-up frames are retained in
    // the rolling archive. Once RIGHT locks the pair frame, reject all eye tags
    // from any other frame. PRESENT tags remain allowed for drain correlation.
    if(eye>=0 && target!=0 && frame!=target) return;
    const auto ev=g_v6612_event_seq.fetch_add(1,std::memory_order_acq_rel)+1;
    std::lock_guard<std::mutex> lk(g_v6612_list_mutex);
    auto* t=V6612FindLiveLocked(list); if(!t) return;
    if(eye>=0 && eye<=1){
        const auto bit=1u<<eye; t->eye_mask|=bit;
        if(main_bind) t->main_bind_mask|=bit;
        if(main_barrier) t->main_barrier_mask|=bit;
        if(om_call) ++t->om_calls[eye];
        if(barrier_call) ++t->barrier_calls[eye];
        t->frame[eye]=frame;
        if(!t->first_event[eye]) t->first_event[eye]=ev;
        t->last_event[eye]=ev;
    }
    if(present_boundary){ t->present_boundary=true; t->present_resource=present_resource; }
}

// D85 dlss: bootstrap (pre-first-pair) phase policy, extracted so the host test
// runs this exact code. LEFT opens or re-arms the rolling window; the first
// RIGHT phase locks the pair frame.
void V6612BootstrapPhaseD85(int eye,std::uint32_t frame) noexcept
{
    if(eye==0) {
        bool expected=false;
        if(g_v6613_pair_capture_active.compare_exchange_strong(expected,true,std::memory_order_acq_rel)) {
            // Bootstrap only: direct F9/auto-start intentionally spends several LEFT
            // samples provisioning the missing right-eye velocity resource. Keep the
            // old rolling window until the first real RIGHT phase identifies the pair.
            V6612ResetTracking();
            g_v6613_pair_frame.store(0,std::memory_order_release);
            g_v6614_right_submitted_seen.store(false,std::memory_order_release);
            g_v6615_left_copy_queued.store(false,std::memory_order_release);
            g_v6615_right_copy_queued.store(false,std::memory_order_release);
            g_v6615_pair_published.store(false,std::memory_order_release);
            g_v6615_last_left_main_frame.store(0,std::memory_order_release);
            g_v6615_last_left_main_batch.store(0,std::memory_order_release);
            g_v6613_pair_capture_active.store(true,std::memory_order_release);
            char a[320]{}; std::snprintf(a,sizeof(a),
                "V66.18-BOOTSTRAP-TRACE BEGIN firstLeftFrame=%u mode=ROLLING-UNTIL-FIRST-RIGHT",
                frame); diagnostic_trace::Write(a);
        } else if(temporal_pair_d85::ShouldRearmBootstrap(false,g_v6613_pair_frame.load(std::memory_order_acquire),frame,
                      g_v6615_pair_published.load(std::memory_order_acquire),g_v6612_drain_requested.load(std::memory_order_acquire))) {
            // D85 dlss: the frame locked by the first RIGHT phase passed
            // without a pair (a temporal certificate check failed on that one
            // frame). Re-open the rolling window here, as live mode does every
            // frame, instead of rejecting all later frames for the whole run.
            const auto stale=g_v6613_pair_frame.load(std::memory_order_acquire);
            V6612ResetTracking();
            g_v6613_pair_frame.store(0,std::memory_order_release);
            g_v6614_right_submitted_seen.store(false,std::memory_order_release);
            g_v6615_left_copy_queued.store(false,std::memory_order_release);
            g_v6615_right_copy_queued.store(false,std::memory_order_release);
            g_v6615_pair_published.store(false,std::memory_order_release);
            g_v6615_last_left_main_frame.store(0,std::memory_order_release);
            g_v6615_last_left_main_batch.store(0,std::memory_order_release);
            g_v6615_active.store(true,std::memory_order_release);
            g_v6613_pair_capture_active.store(true,std::memory_order_release);
            static std::atomic<unsigned long long> rearms{0};const auto n=rearms.fetch_add(1,std::memory_order_relaxed)+1;
            if(n<=4||(n%120ull)==0){char a[256]{};std::snprintf(a,sizeof(a),
                "D85 BOOTSTRAP-REARM staleFrame=%u leftFrame=%u rearms=%llu failedCheck=%s",
                stale,frame,n,temporal_pair_d85::CheckName(g_d85_watchdog.ReportCheck()));diagnostic_trace::Write(a);}
        }
    } else {
        std::uint32_t expected=0;
        if(g_v6613_pair_frame.compare_exchange_strong(expected,frame,std::memory_order_acq_rel)) {
            g_d85_watchdog.BeginAttempt(frame);
            char l[256]{}; std::snprintf(l,sizeof(l),
                "V66.18-BOOTSTRAP-PAIR-LOCK frame=%u trigger=FIRST-DESIGN-RIGHT",frame);
            diagnostic_trace::Write(l);
        }
    }
}

bool V6612ArchiveSubmitBatch(std::uint64_t batch,UINT num_command_lists,ID3D12CommandList* const* command_lists) noexcept
{
    if(!command_lists || !num_command_lists || !g_v6613_pair_capture_active.load(std::memory_order_acquire)) return false;
    if(g_v6618_live_stream_started.load(std::memory_order_acquire)) {
        std::lock_guard<std::mutex> lk(g_v6612_list_mutex);
        for(UINT i=0;i<num_command_lists;++i){
            auto* gl=reinterpret_cast<ID3D12GraphicsCommandList*>(command_lists[i]);
            for(std::size_t j=0;j<g_v6612_live_count;++j){
                auto& t=g_v6612_live_lists[j]; if(t.list!=gl) continue;
                const auto main_mask=t.main_bind_mask|t.main_barrier_mask;
                if((main_mask&1u) && t.frame[0]!=0){
                    g_v6615_last_left_main_frame.store(t.frame[0],std::memory_order_release);
                    g_v6615_last_left_main_batch.store(batch,std::memory_order_release);
                }
                const auto next_gen=t.generation+1;
                auto next_recording=t.temporal_recording;next_recording.Discard();
                t={};t.list=gl;t.generation=next_gen;t.temporal_recording=next_recording;
                break;
            }
        }
        return false;
    }
    struct Local { V6612SubmitItem item; UINT num=0; } local[64]{};
    std::size_t local_count=0; bool saw_present=false;
    {
        std::lock_guard<std::mutex> lk(g_v6612_list_mutex);
        for(UINT i=0;i<num_command_lists;++i){
            auto* gl=reinterpret_cast<ID3D12GraphicsCommandList*>(command_lists[i]);
            V6612ListTag* t=nullptr;
            for(std::size_t j=0;j<g_v6612_live_count;++j){ if(g_v6612_live_lists[j].list==gl){ t=&g_v6612_live_lists[j]; break; } }
            if(!t)continue;
            if(!t->eye_mask&&!t->present_boundary){t->temporal_recording.Discard();continue;}
            V6612SubmitItem item{}; item.used=true; item.batch=batch; item.index=i; item.list=gl; item.generation=t->generation;
            item.eye_mask=t->eye_mask; item.main_bind_mask=t->main_bind_mask; item.main_barrier_mask=t->main_barrier_mask;
            item.present_boundary=t->present_boundary; item.present_resource=t->present_resource;
            for(int e=0;e<2;++e){ item.om_calls[e]=t->om_calls[e]; item.barrier_calls[e]=t->barrier_calls[e]; item.frame[e]=t->frame[e]; item.first_event[e]=t->first_event[e]; item.last_event[e]=t->last_event[e]; }
            if(g_v6612_submit_count<g_v6612_submit_items.size()) g_v6612_submit_items[g_v6612_submit_count++]=item;
            if(local_count<64){ local[local_count].item=item; local[local_count].num=num_command_lists; ++local_count; }
            const auto pair_frame=g_v6613_pair_frame.load(std::memory_order_acquire);
            const auto main_mask=item.main_bind_mask|item.main_barrier_mask;
            if((main_mask&1u) && item.frame[0]!=0){
                g_v6615_last_left_main_frame.store(item.frame[0],std::memory_order_release);
                g_v6615_last_left_main_batch.store(batch,std::memory_order_release);
            }
            if(pair_frame!=0 && (item.eye_mask&2u) && item.frame[1]==pair_frame)
                g_v6614_right_submitted_seen.store(true,std::memory_order_release);
            saw_present=saw_present||item.present_boundary;
            const auto next_gen=t->generation+1;
            auto next_recording=t->temporal_recording;next_recording.Discard();
            *t={};t->list=gl;t->generation=next_gen;t->temporal_recording=next_recording;
        }
    }
    if(local_count){
        char bh[256]{}; std::snprintf(bh,sizeof(bh),
            "V66.18-BOOTSTRAP-SUBMIT-BATCH execute=%llu numLists=%u tagged=%zu presentBoundary=%d",
            (unsigned long long)batch,num_command_lists,local_count,saw_present?1:0); diagnostic_trace::Write(bh);
        for(std::size_t k=0;k<local_count;++k){ const auto& it=local[k].item;
            char m[768]{}; std::snprintf(m,sizeof(m),
                "V66.18-BOOTSTRAP-SUBMIT execute=%llu index=%u/%u list=%p gen=%u eyeMask=0x%X mainBindMask=0x%X mainBarrierMask=0x%X omL/R=%u/%u barL/R=%u/%u frameL/R=%u/%u eventsL=%llu-%llu eventsR=%llu-%llu presentBoundary=%d presentResource=%p",
                (unsigned long long)it.batch,it.index,local[k].num,reinterpret_cast<void*>(it.list),it.generation,it.eye_mask,it.main_bind_mask,it.main_barrier_mask,
                it.om_calls[0],it.om_calls[1],it.barrier_calls[0],it.barrier_calls[1],it.frame[0],it.frame[1],
                (unsigned long long)it.first_event[0],(unsigned long long)it.last_event[0],(unsigned long long)it.first_event[1],(unsigned long long)it.last_event[1],
                it.present_boundary?1:0,reinterpret_cast<void*>(it.present_resource)); diagnostic_trace::Write(m);
        }
    }
    return saw_present;
}

bool V6615IncomingLockedRightMainWriter(UINT num_command_lists,ID3D12CommandList* const* command_lists) noexcept
{
    if(!g_v6615_active.load(std::memory_order_acquire) || !command_lists || !num_command_lists) return false;
    const auto target=g_v6613_pair_frame.load(std::memory_order_acquire);
    if(target==0 || g_v6615_last_left_main_frame.load(std::memory_order_acquire)!=target) return false;
    std::lock_guard<std::mutex> lk(g_v6612_list_mutex);
    if(g_d82_temporal_tracking.load(std::memory_order_acquire)){
        // Do not copy LEFT before a batch which still carries unsubmitted LEFT
        // main writes, or while another LEFT main list is outstanding. The
        // native queue call stays intact; an ambiguous pair is simply withheld.
        for(std::size_t j=0;j<g_v6612_live_count;++j){const auto& t=g_v6612_live_lists[j];
            if(((t.main_bind_mask|t.main_barrier_mask)&1u)&&t.frame[0]==target)return false;
        }
    }
    for(UINT i=0;i<num_command_lists;++i){
        auto* gl=reinterpret_cast<ID3D12GraphicsCommandList*>(command_lists[i]);
        for(std::size_t j=0;j<g_v6612_live_count;++j){
            const auto& t=g_v6612_live_lists[j];
            if(t.list!=gl) continue;
            const auto main_mask=t.main_bind_mask|t.main_barrier_mask;
            if((main_mask&2u) && t.frame[1]==target) return true;
            break;
        }
    }
    return false;
}

temporal_compose_d82::Stamp V6615IncomingRightCompositionD82(
    UINT count,ID3D12CommandList* const* lists) noexcept
{
    if(!g_v6615_active.load(std::memory_order_acquire)||!lists||!count)return {};
    const auto frame=g_v6613_pair_frame.load(std::memory_order_acquire);
    if(!frame)return {};
    std::lock_guard<std::mutex> lk(g_v6612_list_mutex);
    for(UINT i=0;i<count;++i)for(std::size_t j=0;j<g_v6612_live_count;++j){
        const auto& t=g_v6612_live_lists[j];
        if(t.list!=reinterpret_cast<ID3D12GraphicsCommandList*>(lists[i]))continue;
        const auto candidate=t.temporal_recording.SubmittedCandidate(
            reinterpret_cast<std::uintptr_t>(lists[i]),frame);
        if(candidate.Valid())return candidate;
        break;
    }
    return {};
}

const char* V6612SubmissionVerdict(std::array<V6612SubmitItem,kV6613SubmitCap> const& items,std::size_t count,
                                   std::uint64_t& left_batch,std::uint32_t& left_index,
                                   std::uint64_t& right_batch,std::uint32_t& right_index,
                                   std::uint64_t& present_batch,std::uint32_t& present_index,
                                   std::uint32_t& pair_frame) noexcept
{
    bool haveL=false,haveR=false,havePresent=false,anyBoth=false;
    std::uint64_t maxLB=0,minRB=~0ull; std::uint32_t maxLI=0,minRI=~0u;
    pair_frame=0;
    auto less=[](std::uint64_t ab,std::uint32_t ai,std::uint64_t bb,std::uint32_t bi){ return ab<bb || (ab==bb && ai<bi); };
    for(std::size_t i=0;i<count;++i){ const auto& x=items[i]; if(!x.used)continue;
        if((x.eye_mask&2u) && x.frame[1]>pair_frame) pair_frame=x.frame[1];
        if(x.present_boundary){ present_batch=x.batch;present_index=x.index;havePresent=true; }
    }
    if(!pair_frame) return havePresent?"INCOMPLETE-NO-RIGHT-PAIR-FRAME":"INCOMPLETE-NO-RIGHT-OR-PRESENT";
    for(std::size_t i=0;i<count;++i){ const auto& x=items[i]; if(!x.used)continue;
        const auto mainMask=x.main_bind_mask|x.main_barrier_mask;
        const bool leftHere=(mainMask&1u) && x.frame[0]==pair_frame;
        const bool rightHere=(mainMask&2u) && x.frame[1]==pair_frame;
        if(leftHere && rightHere) anyBoth=true;
        if(leftHere){ if(!haveL || less(maxLB,maxLI,x.batch,x.index)){ maxLB=x.batch; maxLI=x.index; } haveL=true; }
        if(rightHere){ if(!haveR || less(x.batch,x.index,minRB,minRI)){ minRB=x.batch; minRI=x.index; } haveR=true; }
    }
    left_batch=maxLB;left_index=maxLI;right_batch=minRB;right_index=minRI;
    if(anyBoth) return "SAME-COMMAND-LIST-LR-MAIN-WRITES";
    if(!haveL || !haveR) return havePresent?"INCOMPLETE-PAIR-FRAME-MAIN-WRITER-EVIDENCE":"INCOMPLETE-NO-PRESENT-BOUNDARY";
    if(less(maxLB,maxLI,minRB,minRI)){
        return maxLB<minRB?"SEPARATE-SUBMIT-BATCHES-L-THEN-R":"SAME-SUBMIT-BATCH-SEPARATE-LISTS-L-THEN-R";
    }
    return "INTERLEAVED-OR-R-BEFORE-L-MAIN-WRITES";
}
} // namespace

void DeviceHook::SetV6610OutputCensus(bool enabled)
{
    std::lock_guard<std::recursive_mutex> transaction(g_temporal_mutex_d85);
    g_temporal_owner_d85.Cancel();
    g_d82_temporal_tracking.store(enabled&&native_upscaler_stereo_d82::TemporalActive(),std::memory_order_release);
    // R90 (from R88 render): a new stereo run must not re-present the PREVIOUS
    // run's last published pair. XR found that old pair (fresh=0) and showed it
    // at its old head pose until this run published (D85R5 23:49:19.6-20.6
    // CERTIFIED_PAIR reuse 146->241 = start flicker / old picture). Without a
    // published pair XR shows the live mono frame until the first fresh pair.
    // R90 (from R89 dice): also on stop. A stopped run's last pair (D85R5
    // KCD2VR1: the dice table at 23:50:36.8) stayed published, and any later XR
    // mode 2 without a producer re-presented it forever = frozen dice image.
    {
        std::lock_guard<std::mutex> lock(s_bridge_capture_mutex);
        s_bridge_complete_pair_serial.store(0,std::memory_order_release);
        s_bridge_complete_pair_camera_frame.store(0,std::memory_order_release);
    }
    if(g_d82_temporal_tracking.load(std::memory_order_acquire))g_d85_watchdog.Arm(static_cast<std::uint64_t>(GetTickCount64()));
    else g_d85_watchdog.Disarm();
    V6612ResetTracking();
    std::lock_guard<std::mutex> lk(g_v6610_output_mutex);
    g_v6613_pair_capture_active.store(false,std::memory_order_release);
    g_v6613_pair_frame.store(0,std::memory_order_release);
    g_v6614_right_submitted_seen.store(false,std::memory_order_release);
    g_v6610_output_eye.store(-1,std::memory_order_release);
    g_v6610_output_frame.store(0,std::memory_order_release);
    g_v6610_output_event.store(0,std::memory_order_release);
    g_v6610_output_candidates={}; g_v6610_output_count=0;
    g_v6624_late_window.store(false,std::memory_order_release);
    g_v6624_late_frame.store(0,std::memory_order_release);
    g_v6624_late_draw_logs.store(0,std::memory_order_release);
    g_v6624_postfx_draw_logs.store(0,std::memory_order_release);
    g_v6624_summary_serial.store(0,std::memory_order_release);
    g_v6610_semantic_outputs={}; g_v6610_semantic_count=0;
    g_v6612_compose_records={}; g_v6612_compose_count=0; g_v6612_tls_compose_slot=-1;
    g_v6615_active.store(enabled,std::memory_order_release);
    g_v6618_live_stream_started.store(false,std::memory_order_release);
    g_v6618_live_rearm_count.store(0,std::memory_order_release);
    g_v6618_live_skip_unconsumed.store(0,std::memory_order_release);
    g_v6610_output_enabled.store(enabled,std::memory_order_release);
    diagnostic_trace::Write(enabled
        ? "V66.18-LIVE-STEREO ARMED bootstrap=ROLLING-UNTIL-FIRST-RIGHT then=PER-FRAME-REARM publication=XR-CONSUMPTION-GATED tracing=SPARSE fallback=LAST-CERTIFIED-PAIR"
        : "V66.18-LIVE-STEREO OFF");
}

static const char* V6647Fmt(unsigned f) noexcept {
    switch(f){case 10:return "RGBA16F";case 2:return "RGBA32F";case 16:return "RG32F";case 24:return "RGB10A2";case 26:return "R11G11B10F";
        case 27:return "RGBA8-TYPELESS";case 28:return "RGBA8";case 34:return "RG16F";case 41:return "R32F";case 54:return "R16F";case 56:return "R16";
        case 61:return "R8";case 49:return "RG8";case 40:return "R32-TYPELESS";case 44:return "R24G8";default:return "?";}
}

void DeviceHook::SetV6610OutputCensusPhase(int eye,std::uint32_t frame)
{
    std::lock_guard<std::recursive_mutex> transaction(g_temporal_mutex_d85);
    if(!g_v6610_output_enabled.load(std::memory_order_acquire))return;
    // No phase rollover/slot swap can reuse buffers referenced by an actual
    // recorded copy that has not reached ExecuteCommandLists or successful Reset.
    if(eye==0&&g_temporal_owner_d85.Pending()){
        g_v6610_output_eye.store(-1,std::memory_order_release);
        g_v6624_late_window.store(false,std::memory_order_release);
        return;
    }
    if(eye==0)g_d82_temporal_tracking.store(native_upscaler_stereo_d82::TemporalActive(),std::memory_order_release);
    if constexpr(kV6647HistoryCensus) {
    if(eye==0 || eye==1) {
        // V66.47: a new eye pass starts -> clear per-pass first-access markers for that eye.
        std::lock_guard<std::mutex> lk(g_v6610_output_mutex);
        for(int j=0;j<g_v6610_output_count;++j) g_v6610_output_candidates[static_cast<std::size_t>(j)].pass_first[eye]=0;
        if(eye==0) ++g_v6647_pass_serial;
    } else {
        // V66.47: every ~5 s log textures that one eye READS FIRST while the OTHER eye wrote last.
        const auto now=static_cast<std::uint64_t>(GetTickCount64());
        if(now>=g_v6647_last_log_ms+5000ull) {
            g_v6647_last_log_ms=now;
            std::lock_guard<std::mutex> lk(g_v6610_output_mutex);
            unsigned printed=0, leaking=0;
            for(int j=0;j<g_v6610_output_count;++j){
                const auto& c=g_v6610_output_candidates[static_cast<std::size_t>(j)];
                if(c.eye_mask!=3) continue;
                const bool leak=(c.reads_other_eye[0]>0||c.reads_other_eye[1]>0);
                if(leak) ++leaking;
                if(!leak) continue; // V66.50: print only cross-eye readers
                if(printed>=40) continue;
                char q[640]{}; std::snprintf(q,sizeof(q),
                    "V66.47-HISTORY res=%p %llux%u fmt=%u(%s)%s L={passes:%u readFirst:%u writeFirst:%u readsRightWrite:%u dsvBinds:%u} R={passes:%u readFirst:%u writeFirst:%u readsLeftWrite:%u dsvBinds:%u} lastWriter=%s verdict=%s",
                    c.resource,(unsigned long long)c.desc.Width,c.desc.Height,(unsigned)c.desc.Format,V6647Fmt((unsigned)c.desc.Format),c.via_uav?" UAV":"",
                    c.passes_seen[0],c.read_first[0],c.write_first[0],c.reads_other_eye[0],c.dsv_binds[0],
                    c.passes_seen[1],c.read_first[1],c.write_first[1],c.reads_other_eye[1],c.dsv_binds[1],
                    c.last_writer==0?"L":(c.last_writer==1?"R":"-"),
                    c.reads_other_eye[0]>0?"LEFT-READS-RIGHT-HISTORY":(c.reads_other_eye[1]>0?"RIGHT-READS-LEFT-DATA":"READ-FIRST-OWN-EYE"));
                diagnostic_trace::Write(q); ++printed;
            }
            char sm[256]{}; std::snprintf(sm,sizeof(sm),"V66.47-HISTORY-SUMMARY pairs=%llu candidates=%d crossEyeReaders=%u printed=%u (read-only census)",
                (unsigned long long)g_v6647_pass_serial,g_v6610_output_count,leaking,printed);
            diagnostic_trace::Write(sm);
        }
    }
    }
    if(eye<0 || eye>1){
        const int previous=g_v6610_output_eye.exchange(-1,std::memory_order_acq_rel);
        if(previous==1 && frame!=0){
            if(g_d82_temporal_tracking.load(std::memory_order_acquire)){
                const auto epoch=native_upscaler_stereo_d82::CurrentEpoch();
                (void)g_temporal_owner_d85.CompleteRight(epoch,frame,
                    native_upscaler_stereo_d82::PairReady(epoch,frame),RenderedPoseD85());
            }
            // V66.64: the post-RIGHT late window is always opened (a flag only) because the
            // HUD layer redirect needs it; the expensive census logging below stays quiet.
            g_v6624_late_frame.store(frame,std::memory_order_release);
            g_v6624_late_window.store(true,std::memory_order_release);
            hud_layer_v6664::OnLateWindowOpen(frame);
            if(s_v6627_performance_quiet.load(std::memory_order_relaxed)) return;
            const auto serial=g_v6624_summary_serial.fetch_add(1,std::memory_order_relaxed)+1;
            if(serial<=6 || (serial%60ull)==0){
                std::lock_guard<std::mutex> lk(g_v6610_output_mutex);
                unsigned printed=0;
                for(int j=0;j<g_v6610_output_count && printed<12;++j){
                    const auto& c=g_v6610_output_candidates[static_cast<std::size_t>(j)];
                    if(c.last_frame[0]!=frame && c.last_frame[1]!=frame) continue;
                    if(c.desc.Width<640 || c.desc.Height<480) continue;
                    char q[640]{}; std::snprintf(q,sizeof(q),
                        "V66.24-POSTFX-RES pair=%llu frame=%u idx=%d resource=%p size=%llux%u fmt=%u eyeMask=0x%X binds=L%u/R%u barriers=L%u/R%u toSRV=L%u/R%u fromSRV=L%u/R%u toRT=L%u/R%u last=L%u/R%u sameMain=%d historyClass=%s",
                        (unsigned long long)serial,frame,j,c.resource,(unsigned long long)c.desc.Width,c.desc.Height,(unsigned)c.desc.Format,c.eye_mask,
                        c.binds[0],c.binds[1],c.barriers[0],c.barriers[1],c.to_srv[0],c.to_srv[1],c.from_srv[0],c.from_srv[1],c.to_rt[0],c.to_rt[1],
                        c.last_frame[0],c.last_frame[1],c.same_main?1:0,
                        (c.eye_mask==3 && (c.to_srv[0]||c.to_srv[1]) && (c.to_rt[0]||c.to_rt[1]))?"SHARED-RT-SRV-CANDIDATE":
                        ((c.eye_mask==1||c.eye_mask==2)?"EYE-SPECIFIC-CANDIDATE":"OTHER"));
                    diagnostic_trace::Write(q); ++printed;
                }
                if(printed==0) diagnostic_trace::Write("V66.24-POSTFX-RES no screen-size eye RT/SRV candidates in this sampled pair");
            }
        }
        return;
    }

    // Starting another eye closes the prior post-RIGHT late-composition window.
    g_v6624_late_window.store(false,std::memory_order_release);
    const bool live_started=g_v6618_live_stream_started.load(std::memory_order_acquire);
    if(eye==0) {
        if(live_started||g_d82_temporal_tracking.load(std::memory_order_acquire)) {
            // V66.36 F3 A/B: XR-cadence owner permits at most one new stereo pair
            // per xrWaitFrame opportunity. It never blocks; extra game frames simply
            // skip the expensive second-eye transaction and keep presenting newest pair.
            if(g_v6636_cadence_owner.load(std::memory_order_acquire)) {
                const auto opp=g_v6636_xr_opportunity.load(std::memory_order_acquire);
                const auto last=g_v6636_last_started_opportunity.load(std::memory_order_relaxed);
                if(opp==0 || opp==last) {
                    const auto sk=g_v6636_cadence_skips.fetch_add(1,std::memory_order_relaxed)+1;
                    if(sk<=8 || (sk%600ull)==0){ char q[256]{}; std::snprintf(q,sizeof(q),"V66.36-CADENCE-SKIP frame=%u opp=%llu skips=%llu",frame,(unsigned long long)opp,(unsigned long long)sk); diagnostic_trace::Write(q); }
                    g_v6610_output_eye.store(-1,std::memory_order_release);
                    return;
                }
                g_v6636_last_started_opportunity.store(opp,std::memory_order_release);
            }
            // V66.34: two-slot capture. V66.18-V66.33 skipped this whole game frame
            // whenever XR had not yet picked up the previous pair (16-33% of frames in
            // V66.22/28/29 logs), because there was only ONE owned capture pair.
            // Now the write slot is moved away from the published pair, so the newest
            // pair stays intact for XR while this frame is captured into the other slot.
            // Safety rule is unchanged: a slot may be overwritten once XR has SUBMITTED
            // its read on the same DIRECT queue (XR releases its claim after submit).
            bool v6634_skip=false; bool v6634_swapped=false;
            std::uint64_t v6634_pub=0;
            {
                std::lock_guard<std::mutex> slot_lock(s_bridge_capture_mutex);
                v6634_pub=g_v6634_pub_serial;
                const bool writing_published = g_v6634_pub_left &&
                    (s_bridge_left_capture==g_v6634_pub_left || s_bridge_right_capture==g_v6634_pub_right);
                if(writing_published) {
                    std::swap(s_bridge_left_capture,g_v6634_spare_left);
                    std::swap(s_bridge_right_capture,g_v6634_spare_right);
                    std::swap(s_bridge_left_desc,g_v6634_spare_left_desc);
                    std::swap(s_bridge_right_desc,g_v6634_spare_right_desc);
                    v6634_swapped=true;
                }
                if(g_v6634_xr_claim_left && g_v6634_xr_claim_left==s_bridge_left_capture) v6634_skip=true;
            }
            if(v6634_swapped) {
                const auto sw=g_v6634_swaps.fetch_add(1,std::memory_order_relaxed)+1;
                if(sw<=4 || (sw%600ull)==0){ char q[320]{}; std::snprintf(q,sizeof(q),
                    "V66.34-TWO-SLOT swap=%llu frame=%u published=%llu claimSkips=%llu supersededUnread=%llu",
                    (unsigned long long)sw,frame,(unsigned long long)v6634_pub,
                    (unsigned long long)g_v6634_claim_skips.load(std::memory_order_relaxed),
                    (unsigned long long)g_v6634_superseded.load(std::memory_order_relaxed)); diagnostic_trace::Write(q); }
            }
            if(v6634_skip) {
                const auto n=g_v6634_claim_skips.fetch_add(1,std::memory_order_relaxed)+1;
                g_v6618_live_skip_unconsumed.fetch_add(1,std::memory_order_relaxed);
                g_v6610_output_eye.store(-1,std::memory_order_release);
                if(n<=8 || (n%120ull)==0){ char q[320]{}; std::snprintf(q,sizeof(q),
                    "V66.34-LIVE-SKIP frame=%u reason=xr-reading-target-slot published=%llu skips=%llu",
                    frame,(unsigned long long)v6634_pub,(unsigned long long)n); diagnostic_trace::Write(q); }
                return;
            }
            // Previous pair is safely consumed. Start a fresh lightweight tag window
            // for this exact game frame. No rolling archive or PRESENT drain is needed.
            V6612ResetTracking();
            g_v6613_pair_frame.store(frame,std::memory_order_release);
            g_d85_watchdog.BeginAttempt(frame);
            g_v6613_pair_capture_active.store(true,std::memory_order_release);
            g_v6615_left_copy_queued.store(false,std::memory_order_release);
            g_v6615_right_copy_queued.store(false,std::memory_order_release);
            g_v6615_pair_published.store(false,std::memory_order_release);
            g_v6615_last_left_main_frame.store(0,std::memory_order_release);
            g_v6615_last_left_main_batch.store(0,std::memory_order_release);
            // A transient copy-slot miss is frame-local in live mode. Re-enable the
            // proven queue-copy owner for the next exact L/R frame instead of
            // permanently killing publication after one busy allocator.
            g_v6615_active.store(true,std::memory_order_release);
            const auto n=g_v6618_live_rearm_count.fetch_add(1,std::memory_order_relaxed)+1;
            if(n<=4 || (n%120ull)==0){ char a[256]{}; std::snprintf(a,sizeof(a),
                "V66.18-LIVE-PAIR BEGIN frame=%u rearm=%llu mode=EXACT-FRAME",
                frame,(unsigned long long)n); diagnostic_trace::Write(a); }
        } else {
            V6612BootstrapPhaseD85(0,frame);
        }
    } else {
        if(live_started||g_d82_temporal_tracking.load(std::memory_order_acquire)) {
            const auto target=g_v6613_pair_frame.load(std::memory_order_acquire);
            if(target!=frame) {
                g_v6610_output_eye.store(-1,std::memory_order_release);
                static std::atomic<unsigned long long> mismatch{0}; const auto n=mismatch.fetch_add(1,std::memory_order_relaxed)+1;
                if(n<=4 || (n%120ull)==0){ char q[256]{}; std::snprintf(q,sizeof(q),
                    "V66.18-LIVE-RIGHT-SKIP frame=%u target=%u reason=no-matching-live-left",frame,target); diagnostic_trace::Write(q); }
                return;
            }
        } else {
            V6612BootstrapPhaseD85(1,frame);
        }
    }

    const auto target=g_v6613_pair_frame.load(std::memory_order_acquire);
    if(!g_v6613_pair_capture_active.load(std::memory_order_acquire)) {
        g_v6610_output_eye.store(-1,std::memory_order_release);
        return;
    }
    if(target!=0 && frame!=target) {
        g_v6610_output_eye.store(-1,std::memory_order_release);
        return;
    }
    if(eye==0&&g_d82_temporal_tracking.load(std::memory_order_acquire))
        (void)g_temporal_owner_d85.Begin(native_upscaler_stereo_d82::CurrentEpoch(),frame);
    if(eye==1&&g_d82_temporal_tracking.load(std::memory_order_acquire)){
        const auto pose=RenderedPoseD85();
        std::lock_guard<std::mutex> capture(s_bridge_capture_mutex);
        // Snapshot before recording RIGHT: the global tracker may later advance
        // to PRESENT while the queue is still about to consume LEFT work.
        g_temporal_owner_d85.NoteLeftBoundary(native_upscaler_stereo_d82::CurrentEpoch(),frame,
            reinterpret_cast<std::uintptr_t>(s_bridge_current_main_target),
            static_cast<std::uint32_t>(s_bridge_current_main_state),pose);
    }
    g_v6610_output_frame.store(frame,std::memory_order_release);
    g_v6610_output_eye.store(eye,std::memory_order_release);
    if(!live_started){ char m[224]{}; std::snprintf(m,sizeof(m),
        "V66.18-BOOTSTRAP-PHASE eye=%c frame=%u targetFrame=%u state=%s",
        eye?'R':'L',frame,target,target?"LOCKED":"ROLLING"); diagnostic_trace::Write(m); }
}

void DeviceHook::RecordV6610SemanticOutput(int eye,std::uint32_t frame,std::uintptr_t render_output,
                                             std::uintptr_t texture_object,std::uintptr_t device_backing,
                                             std::uintptr_t native_rtv_view,std::uint64_t native_rtv_key,
                                             std::uint16_t width,std::uint16_t height,
                                             std::uint8_t source_format,std::uint8_t device_format,
                                             std::uint32_t flags,const char* name)
{
    if(!g_v6610_output_enabled.load(std::memory_order_acquire) || eye<0 || eye>1 || !texture_object)return;
    int slot=-1;
    {
        std::lock_guard<std::mutex> lk(g_v6610_output_mutex);
        for(int i=0;i<g_v6610_semantic_count;++i){
            auto& o=g_v6610_semantic_outputs[static_cast<std::size_t>(i)];
            if(o.eye==eye && o.texture_object==texture_object && o.device_backing==device_backing){slot=i;break;}
        }
        if(slot<0 && g_v6610_semantic_count<(int)g_v6610_semantic_outputs.size())slot=g_v6610_semantic_count++;
        if(slot>=0){
            auto& o=g_v6610_semantic_outputs[static_cast<std::size_t>(slot)];
            if(!o.used){o={};o.used=true;o.eye=eye;o.texture_object=texture_object;o.device_backing=device_backing;}
            o.frame=frame;++o.hits;o.render_output=render_output;o.native_rtv_view=native_rtv_view;o.native_rtv_key=native_rtv_key;
            o.width=width;o.height=height;o.source_format=source_format;o.device_format=device_format;o.flags=flags;
            std::snprintf(o.name,sizeof(o.name),"%s",(name&&*name)?name:"<unreadable>");
        }
    }
    if(slot>=0 && !g_v6618_live_stream_started.load(std::memory_order_acquire)){
        char m[640]{};std::snprintf(m,sizeof(m),
            "V66.12-F9-SEMANTIC-OWNER eye=%c frame=%u slot=%d renderOutput=%p texture=%p backing=%p nativeRtvView=%p nativeRtvKey=0x%016llX size=%ux%u sourceFmt=%u deviceFmt=%u flags=0x%08X name='%s' boundary=WHGame+8645F8-BEFORE-FINAL-COMPOSE readOnly=1",
            eye?'R':'L',frame,slot,reinterpret_cast<void*>(render_output),reinterpret_cast<void*>(texture_object),
            reinterpret_cast<void*>(device_backing),reinterpret_cast<void*>(native_rtv_view),(unsigned long long)native_rtv_key,
            (unsigned)width,(unsigned)height,(unsigned)source_format,(unsigned)device_format,flags,(name&&*name)?name:"<unreadable>");
        diagnostic_trace::Write(m);
    }
}

bool DeviceHook::BeginTemporalCompositionD82(std::uint64_t epoch,std::uint32_t frame) noexcept
{
    using temporal_pair_d85::Check;
    if(g_d82_compose_scope.Active()||!epoch||!frame)return false;
    // D85 dlss: same checks as D82, each refusal named for the status line.
    if(!g_d82_temporal_hooks_ready.load(std::memory_order_acquire)){g_d85_watchdog.Refuse(Check::ScopeHooks);return false;}
    if(!g_d82_temporal_tracking.load(std::memory_order_acquire)||
       !g_v6615_active.load(std::memory_order_acquire)){g_d85_watchdog.Refuse(Check::ScopeInactive);return false;}
    if(g_v6613_pair_frame.load(std::memory_order_acquire)!=frame){g_d85_watchdog.Refuse(Check::ScopePairFrame);return false;}
    if(!native_upscaler_stereo_d82::PairReady(epoch,frame)){g_d85_watchdog.Refuse(Check::ScopeSdkReceipt);return false;}
    ID3D12Resource* main=nullptr;
    {std::lock_guard<std::mutex> bl(s_bridge_capture_mutex);main=s_bridge_current_main_target;}
    if(!main){g_d85_watchdog.Refuse(Check::ScopeMainTarget);return false;}
    g_d82_compose_draw_seen=false;
    g_d82_compose_scope={epoch,frame,reinterpret_cast<std::uintptr_t>(main)};
    g_d85_watchdog.Reach(temporal_pair_d85::ScopeOpened);
    return true;
}
void DeviceHook::EndTemporalCompositionD82() noexcept {
    if(g_d82_compose_scope.Active()&&!g_d82_compose_draw_seen){
        g_d85_watchdog.Refuse(temporal_pair_d85::Check::NoScopedMainDraw);
        static std::atomic<std::uint64_t> misses{0};const auto count=misses.fetch_add(1,std::memory_order_relaxed)+1;
        if(count<=4||(count%120)==0){char line[256]{};std::snprintf(line,sizeof(line),
            "D82 TEMPORAL_COMPOSE refused=NO_SCOPED_MAIN_DRAW epoch=%llu frame=%u count=%llu publication=WITHHELD",
            (unsigned long long)g_d82_compose_scope.epoch,g_d82_compose_scope.frame,(unsigned long long)count);
            diagnostic_trace::Write(line);}
    }
    g_d82_compose_scope={};g_d82_compose_draw_seen=false;
}

void DeviceHook::BeginV6611CompositionTrace(int eye,std::uint32_t frame,
                                               std::uintptr_t input_texture,std::uintptr_t input_backing,std::uintptr_t input_rtv_view,
                                               std::uintptr_t output_texture,std::uintptr_t output_backing,std::uintptr_t output_rtv_view)
{
    g_v6612_tls_compose_slot=-1;
    if(!g_v6610_output_enabled.load(std::memory_order_acquire) || eye<0 || eye>1 ||
       g_v6618_live_stream_started.load(std::memory_order_acquire))return;
    int slot=-1;
    {
        std::lock_guard<std::mutex> lk(g_v6610_output_mutex);
        if(g_v6612_compose_count<(int)g_v6612_compose_records.size())slot=g_v6612_compose_count++;
        if(slot>=0){
            auto& r=g_v6612_compose_records[static_cast<std::size_t>(slot)];
            r={};r.used=true;r.eye=eye;r.frame=frame;
            r.input_texture=input_texture;r.input_backing=input_backing;r.input_rtv_view=input_rtv_view;
            r.output_texture=output_texture;r.output_backing=output_backing;r.output_rtv_view=output_rtv_view;
        }
    }
    g_v6612_tls_compose_slot=slot;
    if(slot>=0){
        char m[448]{};std::snprintf(m,sizeof(m),
            "V66.12-F9-COMPOSE-SCOPE BEGIN eye=%c frame=%u slot=%d inputTexture=%p inputBacking=%p outputTexture=%p outputBacking=%p helper=WHGame+2543A6C roles=STATIC-EXACT:{input:RDX->42BBF8 output:R8->23CA7F8} mutation=NONE",
            eye?'R':'L',frame,slot,reinterpret_cast<void*>(input_texture),reinterpret_cast<void*>(input_backing),
            reinterpret_cast<void*>(output_texture),reinterpret_cast<void*>(output_backing));
        diagnostic_trace::Write(m);
    }
}

void DeviceHook::EndV6611CompositionTrace()
{
    const int slot=g_v6612_tls_compose_slot;g_v6612_tls_compose_slot=-1;
    if(slot<0)return;
    V6611ComposeRecord copy{};
    {
        std::lock_guard<std::mutex> lk(g_v6610_output_mutex);
        if(slot>=g_v6612_compose_count)return;
        copy=g_v6612_compose_records[static_cast<std::size_t>(slot)];
    }
    char m[1024]{};
    std::snprintf(m,sizeof(m),
        "V66.12-F9-COMPOSE-SCOPE END eye=%c frame=%u slot=%d omCalls=%u rtCount=%u rt0=%p/main%d rt1=%p/main%d rt2=%p/main%d rt3=%p/main%d barriers=%u transitions=%u readOnly=1",
        copy.eye?'R':'L',copy.frame,slot,copy.om_calls,copy.rt_count,
        copy.rt_count>0?reinterpret_cast<void*>(copy.rt[0]):nullptr,copy.rt_count>0&&copy.rt_same_main[0]?1:0,
        copy.rt_count>1?reinterpret_cast<void*>(copy.rt[1]):nullptr,copy.rt_count>1&&copy.rt_same_main[1]?1:0,
        copy.rt_count>2?reinterpret_cast<void*>(copy.rt[2]):nullptr,copy.rt_count>2&&copy.rt_same_main[2]?1:0,
        copy.rt_count>3?reinterpret_cast<void*>(copy.rt[3]):nullptr,copy.rt_count>3&&copy.rt_same_main[3]?1:0,
        copy.barrier_calls,copy.transition_count);
    diagnostic_trace::Write(m);
}

void DeviceHook::DumpV6610OutputCensus()
{
    if(g_v6610_output_enabled.load(std::memory_order_acquire) &&
       !g_v6612_drain_requested.exchange(true,std::memory_order_acq_rel)){
        g_v6610_output_eye.store(-1,std::memory_order_release);
        diagnostic_trace::Write("V66.18-BOOTSTRAP-SUBMIT-DRAIN ARMED reason=native-LR-CPU-complete wait=RIGHT-pair-frame-submission-then-next-PRESENT XR=MONO mutation=NONE");
        return;
    }
    std::lock_guard<std::mutex> lk(g_v6610_output_mutex);
    char h[320]{}; std::snprintf(h,sizeof(h),
        "V66.12-F9-OUTPUT-SUMMARY candidates=%d semanticOwners=%d events=%llu interpretation=semantic-CRenderOutput-owner-first-then-exact-D3D-correlation",
        g_v6610_output_count,g_v6610_semantic_count,(unsigned long long)g_v6610_output_event.load(std::memory_order_relaxed));
    diagnostic_trace::Write(h);
    for(int i=0;i<g_v6610_output_count;++i){
        const auto& c=g_v6610_output_candidates[static_cast<std::size_t>(i)];
        char m[448]{}; std::snprintf(m,sizeof(m),
            "V66.12-F9-OUTPUT-CAND idx=%d resource=%p size=%llux%u arr=%u mips=%u fmt=%u flags=0x%X layout=%u eyeMask=0x%X bindsL/R=%u/%u barriersL/R=%u/%u sameMainBackbuffer=%d",
            i,c.resource,(unsigned long long)c.desc.Width,c.desc.Height,(unsigned)c.desc.DepthOrArraySize,(unsigned)c.desc.MipLevels,
            (unsigned)c.desc.Format,(unsigned)c.desc.Flags,(unsigned)c.desc.Layout,c.eye_mask,c.binds[0],c.binds[1],c.barriers[0],c.barriers[1],c.same_main?1:0);
        diagnostic_trace::Write(m);
    }
    const V6610SemanticOutput* last[2]={nullptr,nullptr};
    for(int i=0;i<g_v6610_semantic_count;++i){
        const auto& o=g_v6610_semantic_outputs[static_cast<std::size_t>(i)]; if(!o.used)continue;
        if(o.eye>=0&&o.eye<=1)last[o.eye]=&o;
        const auto match=V6610FindExactMatch(o);
        int heuristic=-1,heuristicCount=0;
        if(match.candidate<0){
            for(int j=0;j<g_v6610_output_count;++j){const auto& c=g_v6610_output_candidates[static_cast<std::size_t>(j)];
                if(c.desc.Width==o.width && c.desc.Height==o.height && (c.eye_mask&(1u<<o.eye))){heuristic=j;++heuristicCount;}
            }
        }
        char m[760]{};std::snprintf(m,sizeof(m),
            "V66.12-F9-SEMANTIC-MATCH eye=%c frame=%u hits=%u renderOutput=%p texture=%p backing=%p nativeRtvView=%p nativeRtvKey=0x%016llX name='%s' size=%ux%u sourceFmt=%u deviceFmt=%u exactCandidate=%d exactDepth=%d exactAnchor=%s exactAt=%p via=%p parentOff=0x%zX childOff=0x%zX heuristicSameSizeCandidate=%d heuristicSameSizeCount=%d conclusion=%s",
            o.eye?'R':'L',o.frame,o.hits,reinterpret_cast<void*>(o.render_output),reinterpret_cast<void*>(o.texture_object),
            reinterpret_cast<void*>(o.device_backing),reinterpret_cast<void*>(o.native_rtv_view),(unsigned long long)o.native_rtv_key,o.name,
            (unsigned)o.width,(unsigned)o.height,(unsigned)o.source_format,(unsigned)o.device_format,
            match.candidate,match.depth,V6610AnchorName(match.anchor),reinterpret_cast<void*>(match.match_at),reinterpret_cast<void*>(match.via),
            match.offset0,match.offset1,heuristic,heuristicCount,match.candidate>=0?"EXACT-POINTER-CORRELATION":"NO-EXACT-POINTER-IN-BOUNDED-SCAN");
        diagnostic_trace::Write(m);
    }
    if(last[0]&&last[1]){
        char p[480]{};std::snprintf(p,sizeof(p),
            "V66.12-F9-SEMANTIC-PAIR sameRenderOutput=%d sameTexture=%d sameBacking=%d leftTexture=%p rightTexture=%p leftName='%s' rightName='%s' interpretation=%s",
            last[0]->render_output==last[1]->render_output?1:0,last[0]->texture_object==last[1]->texture_object?1:0,
            last[0]->device_backing==last[1]->device_backing?1:0,reinterpret_cast<void*>(last[0]->texture_object),reinterpret_cast<void*>(last[1]->texture_object),
            last[0]->name,last[1]->name,last[0]->texture_object==last[1]->texture_object?"ONE-ENGINE-OUTPUT-OVERWRITTEN-BY-EYES":"DISTINCT-ENGINE-OUTPUT-TEXTURES");
        diagnostic_trace::Write(p);
    }
    const V6611ComposeRecord* composeLast[2]={nullptr,nullptr};
    for(int i=0;i<g_v6612_compose_count;++i){
        const auto& r=g_v6612_compose_records[static_cast<std::size_t>(i)];if(!r.used||r.eye<0||r.eye>1)continue;composeLast[r.eye]=&r;
        V6610SemanticOutput in{};in.texture_object=r.input_texture;in.device_backing=r.input_backing;in.native_rtv_view=r.input_rtv_view;in.eye=r.eye;
        V6610SemanticOutput out{};out.texture_object=r.output_texture;out.device_backing=r.output_backing;out.native_rtv_view=r.output_rtv_view;out.eye=r.eye;
        const auto im=V6610FindExactMatch(in),om=V6610FindExactMatch(out);
        char cm[1024]{};std::snprintf(cm,sizeof(cm),
            "V66.12-F9-COMPOSE-MATCH eye=%c frame=%u inputTexture=%p inputBacking=%p inputCandidate=%d inputDepth=%d inputAnchor=%s outputTexture=%p outputBacking=%p outputCandidate=%d outputDepth=%d outputAnchor=%s omCalls=%u rtCount=%u rt0=%p/main%d rt1=%p/main%d barriers=%u transitions=%u roleContract=INPUT:RDX->42BBF8 OUTPUT:R8->23CA7F8",
            r.eye?'R':'L',r.frame,reinterpret_cast<void*>(r.input_texture),reinterpret_cast<void*>(r.input_backing),im.candidate,im.depth,V6610AnchorName(im.anchor),
            reinterpret_cast<void*>(r.output_texture),reinterpret_cast<void*>(r.output_backing),om.candidate,om.depth,V6610AnchorName(om.anchor),
            r.om_calls,r.rt_count,
            r.rt_count>0?reinterpret_cast<void*>(r.rt[0]):nullptr,r.rt_count>0&&r.rt_same_main[0]?1:0,
            r.rt_count>1?reinterpret_cast<void*>(r.rt[1]):nullptr,r.rt_count>1&&r.rt_same_main[1]?1:0,
            r.barrier_calls,r.transition_count);
        diagnostic_trace::Write(cm);
    }
    if(composeLast[0]&&composeLast[1]){
        char cp[640]{};std::snprintf(cp,sizeof(cp),
            "V66.12-F9-COMPOSE-PAIR sameInputTexture=%d sameInputBacking=%d sameOutputTexture=%d sameOutputBacking=%d leftInput=%p rightInput=%p leftOutput=%p rightOutput=%p interpretation=%s",
            composeLast[0]->input_texture==composeLast[1]->input_texture?1:0,composeLast[0]->input_backing==composeLast[1]->input_backing?1:0,
            composeLast[0]->output_texture==composeLast[1]->output_texture?1:0,composeLast[0]->output_backing==composeLast[1]->output_backing?1:0,
            reinterpret_cast<void*>(composeLast[0]->input_texture),reinterpret_cast<void*>(composeLast[1]->input_texture),
            reinterpret_cast<void*>(composeLast[0]->output_texture),reinterpret_cast<void*>(composeLast[1]->output_texture),
            composeLast[0]->input_texture==composeLast[1]->input_texture?"ONE-COMPOSITION-INPUT-OVERWRITTEN-BY-EYES":"DISTINCT-COMPOSITION-INPUTS");
        diagnostic_trace::Write(cp);
    }
    std::array<V6612SubmitItem,kV6613SubmitCap> submitted{}; std::size_t submittedCount=0;
    std::array<V6612ListTag,kV6613LiveListCap> live{}; std::size_t liveCount=0;
    {
        std::lock_guard<std::mutex> slk(g_v6612_list_mutex);
        submitted=g_v6612_submit_items; submittedCount=g_v6612_submit_count;
        live=g_v6612_live_lists; liveCount=g_v6612_live_count;
    }
    std::uint64_t lb=0,rb=0,pb=0; std::uint32_t li=0,ri=0,pi=0,pairFrame=0;
    const char* verdict=V6612SubmissionVerdict(submitted,submittedCount,lb,li,rb,ri,pb,pi,pairFrame);
    std::size_t mainL=0,mainR=0,both=0,presentItems=0;
    for(std::size_t i=0;i<submittedCount;++i){ const auto& x=submitted[i]; if(!x.used)continue;
        const auto mm=x.main_bind_mask|x.main_barrier_mask;
        if((mm&1u) && x.frame[0]==pairFrame) ++mainL;
        if((mm&2u) && x.frame[1]==pairFrame) ++mainR;
        if((mm&3u)==3u && x.frame[0]==pairFrame && x.frame[1]==pairFrame) ++both;
        if(x.present_boundary) ++presentItems;
    }
    std::size_t unsubmitted=0;
    for(std::size_t i=0;i<liveCount;++i){ const auto& x=live[i]; if(!x.list || (!x.eye_mask&&!x.present_boundary))continue; ++unsubmitted;
        if(unsubmitted<=24){ char um[640]{}; std::snprintf(um,sizeof(um),
            "V66.18-BOOTSTRAP-UNSUBMITTED list=%p gen=%u eyeMask=0x%X mainBindMask=0x%X mainBarrierMask=0x%X omL/R=%u/%u barL/R=%u/%u frameL/R=%u/%u presentBoundary=%d",
            reinterpret_cast<void*>(x.list),x.generation,x.eye_mask,x.main_bind_mask,x.main_barrier_mask,x.om_calls[0],x.om_calls[1],x.barrier_calls[0],x.barrier_calls[1],x.frame[0],x.frame[1],x.present_boundary?1:0);
            diagnostic_trace::Write(um); }
    }
    char sh[768]{}; std::snprintf(sh,sizeof(sh),
        "V66.18-BOOTSTRAP-SUBMIT-SUMMARY pairFrame=%u targetFrame=%u submittedItems=%zu submitCap=%zu mainWriterItemsL/R=%zu/%zu bothEyeItems=%zu presentItems=%zu unsubmitted=%zu leftLast=%llu:%u rightFirst=%llu:%u present=%llu:%u verdict=%s nextCapture=%s",
        pairFrame,g_v6613_pair_frame.load(std::memory_order_relaxed),submittedCount,kV6613SubmitCap,mainL,mainR,both,presentItems,unsubmitted,(unsigned long long)lb,li,(unsigned long long)rb,ri,(unsigned long long)pb,pi,verdict,
        std::strcmp(verdict,"SEPARATE-SUBMIT-BATCHES-L-THEN-R")==0?"QUEUE-COPY-BETWEEN-BATCHES":
        (std::strcmp(verdict,"SAME-SUBMIT-BATCH-SEPARATE-LISTS-L-THEN-R")==0?"SPLIT-BATCH-OR-IN-LIST-COPY":
         (std::strcmp(verdict,"SAME-COMMAND-LIST-LR-MAIN-WRITES")==0?"IN-LIST-EYE-BOUNDARY-COPY":"TRACE-NEEDS-REFINEMENT")));
    diagnostic_trace::Write(sh);
    g_v6610_output_eye.store(-1,std::memory_order_release);
    g_v6613_pair_capture_active.store(false,std::memory_order_release);
    /* V66.18 bootstrap/live mode: keep output census armed; the next design LEFT phase will rearm tracking automatically. */
}



void DeviceHook::NotifyV6593Count2Packet(std::uint32_t count)
{
    if(g_v6593_mode.load(std::memory_order_acquire)==0 || native_stereo_outer_lab::BridgeStage()!=3 || count<2) return;
    const auto frame=FrameSync::CurrentFrameSerial();
    std::lock_guard<std::mutex> lk(g_v6593_mutex);
    g_v6593_nview_frames[g_v6593_nview_cursor++%g_v6593_nview_frames.size()]=frame;
    const auto n=g_v6593_log_count.fetch_add(1,std::memory_order_relaxed)+1;
    if(n<=24){ char m[256]{}; std::snprintf(m,sizeof(m),
        "V65.93-NVIEW frame=%llu count=%u exact=1",(unsigned long long)frame,unsigned(count)); diagnostic_trace::Write(m); }
}

void DeviceHook::NotifyV6593SecondaryApply(std::uint32_t native_count, std::uintptr_t crv)
{
    const int mode=g_v6593_mode.load(std::memory_order_acquire);
    if(mode==0 || native_stereo_outer_lab::BridgeStage()!=3 || native_count==0) return;
    const int idx=native_count>1?1:0;
    auto* rr=g_v6593_tls_scene; auto* list=g_v6593_tls_list; const UINT rt_slot=g_v6593_tls_slot;
    const auto frame=FrameSync::CurrentFrameSerial();
    if(frame<=g_v6593_start_after_frame) return; // never tag the keypress/activation frame
    if(!rr || !list){
        const auto n=g_v6593_log_count.fetch_add(1,std::memory_order_relaxed)+1;
        if(n<=32){ char m[320]{}; std::snprintf(m,sizeof(m),
            "V65.93-CENSUS MISS role=%c nativeCount=%u frame=%llu crv=%p reason=no-same-thread-fullres-fmt10",
            idx==0?'A':'B',unsigned(native_count),(unsigned long long)frame,reinterpret_cast<void*>(crv)); diagnostic_trace::Write(m); }
        return;
    }
    const auto d=rr->GetDesc();
    std::uint64_t ps=0,pt=0,pw=0,pcf=0; std::uint32_t ph=0; GetLatestPresentIdentity(&ps,&pt,&pw,&ph,&pcf);
    if(ps<=g_v6593_start_after_present) return; // never arm in the keypress/transition Present
    if(!pw || !ph || d.Width!=pw || d.Height!=ph || d.Format!=DXGI_FORMAT_R16G16B16A16_FLOAT) return;

    std::lock_guard<std::mutex> lk(g_v6593_mutex);
    if(g_v6593_pair_serial) return;
    const bool have_any=g_v6593_candidate[0].source || g_v6593_candidate[1].source;
    if(have_any && (g_v6593_work_frame!=frame || g_v6593_work_crv!=crv)){
        if(g_v6593_candidate[0].source && g_v6593_candidate[1].source) return; // hold complete working pair for submit/present certification
        V6593ReleaseCandidate(g_v6593_candidate[0]); V6593ReleaseCandidate(g_v6593_candidate[1]);
        g_v6593_work_frame=0; g_v6593_work_crv=0;
    }
    if(!g_v6593_work_frame){ g_v6593_work_frame=frame; g_v6593_work_crv=crv; }
    if(g_v6593_work_frame!=frame || g_v6593_work_crv!=crv) return;
    if(g_v6593_candidate[idx].source) return;

    rr->AddRef();
    auto& c=g_v6593_candidate[idx];
    c.source=rr; c.crv=crv; c.frame_serial=frame; c.present_serial=ps; c.tag_tick_ms=GetTickCount64();
    c.native_count=native_count; c.rt_slot=rt_slot; c.state=D3D12_RESOURCE_STATE_RENDER_TARGET; c.state_known=true;
    V6593AddTouchListLocked(c,list);
    const auto n=g_v6593_log_count.fetch_add(1,std::memory_order_relaxed)+1;
    if(n<=64){ char m[640]{}; std::snprintf(m,sizeof(m),
        "V65.93-CENSUS TAG role=%c mode=%s frame=%llu crv=%p nativeCount=%u presentTelemetry=%llu tid=%lu list=%p slot=%u source=%p size=%llux%u fmt=%u initialState=RT",
        idx==0?'A':'B',mode==1?"READONLY":"CAPTURE",(unsigned long long)frame,reinterpret_cast<void*>(crv),unsigned(native_count),
        (unsigned long long)ps,(unsigned long)GetCurrentThreadId(),list,unsigned(rt_slot),rr,(unsigned long long)d.Width,d.Height,(unsigned)d.Format);
        diagnostic_trace::Write(m); }
}

bool DeviceHook::TryGetV6593SceneColorPair(ID3D12Resource** out_a, ID3D12Resource** out_b, std::uint64_t* out_serial)
{
    if(!out_a || !out_b) return false; *out_a=nullptr; *out_b=nullptr;
    std::lock_guard<std::mutex> lk(g_v6593_mutex);
    if(!g_v6593_pair_serial || !g_v6593_snapshot[0] || !g_v6593_snapshot[1]) return false;
    g_v6593_snapshot[0]->AddRef(); g_v6593_snapshot[1]->AddRef();
    *out_a=g_v6593_snapshot[0]; *out_b=g_v6593_snapshot[1]; if(out_serial) *out_serial=g_v6593_pair_serial; return true;
}


bool DeviceHook::TryGetV6587SameFramePair(ID3D12Resource** out_left, ID3D12Resource** out_right, std::uint64_t* out_serial)
{
    if(!out_left || !out_right) return false; *out_left=nullptr; *out_right=nullptr;
    std::lock_guard<std::mutex> lk(g_v6587_mutex);
    if(!g_v6587_enabled.load(std::memory_order_acquire) || !g_v6587_recorded || !g_v6587_submitted ||
       !g_v6587_snapshot[0] || !g_v6587_snapshot[1] || g_v6587_pair_serial==0) return false;
    g_v6587_snapshot[0]->AddRef(); g_v6587_snapshot[1]->AddRef();
    *out_left=g_v6587_snapshot[0]; *out_right=g_v6587_snapshot[1];
    if(out_serial) *out_serial=g_v6587_pair_serial;
    return true;
}


void DeviceHook::SetSceneColorProbe(bool enabled, std::uint32_t width, std::uint32_t height)
{
    s_scene_color_probe_width.store(width, std::memory_order_release);
    s_scene_color_probe_height.store(height, std::memory_order_release);
    s_scene_color_probe_barrier_count.store(0, std::memory_order_release);
    s_scene_color_probe_left_count.store(0, std::memory_order_release);
    s_scene_color_probe_right_count.store(0, std::memory_order_release);
    s_scene_color_probe_enabled.store(enabled, std::memory_order_release);
    char m[224]{}; std::snprintf(m,sizeof(m),
        "V59.6-SCENE-PROBE %s target=%ux%u normal-pipeline RT transitions; eye=-1 mono,0 L,1 R",
        enabled?"ON":"OFF", width, height);
    debug_trace::Write(m);
}
std::uint64_t DeviceHook::SceneColorProbeBarrierCount() { return s_scene_color_probe_barrier_count.load(std::memory_order_acquire); }
std::uint64_t DeviceHook::SceneColorProbeLeftCount() { return s_scene_color_probe_left_count.load(std::memory_order_acquire); }
std::uint64_t DeviceHook::SceneColorProbeRightCount() { return s_scene_color_probe_right_count.load(std::memory_order_acquire); }

void __stdcall DeviceHook::DetourResourceBarrier(
    ID3D12GraphicsCommandList* list_this, UINT num_barriers, const D3D12_RESOURCE_BARRIER* barriers)
{
    const bool internal_vr = list_this == s_internal_vr_command_list.load(std::memory_order_acquire);
    const bool internal_menu = list_this == s_internal_menu_command_list.load(std::memory_order_acquire);
    const bool internal_bridge = list_this == s_bridge_copy_list || list_this == g_v6593_copy_list || list_this == s_internal_hud_command_list.load(std::memory_order_acquire);
    if (!internal_vr && !internal_menu && !internal_bridge && barriers) {
        for (UINT i = 0; i < num_barriers; ++i) {
            const auto& b = barriers[i];
            if (g_v6585_enabled.load(std::memory_order_acquire) && native_stereo_outer_lab::BridgeStage()==3 && b.Type==D3D12_RESOURCE_BARRIER_TYPE_UAV && b.UAV.pResource) {
                const auto rd=b.UAV.pResource->GetDesc(); const auto n=g_v6585_event.fetch_add(1,std::memory_order_relaxed)+1;
                if(n<=160){ char um[384]{}; std::snprintf(um,sizeof(um),
                    "V65.85-UAV-BARRIER n=%llu list=%p resource=%p size=%llux%u arr=%u fmt=%u flags=0x%X layout=%u",
                    (unsigned long long)n,list_this,b.UAV.pResource,(unsigned long long)rd.Width,rd.Height,(unsigned)rd.DepthOrArraySize,(unsigned)rd.Format,(unsigned)rd.Flags,(unsigned)rd.Layout); diagnostic_trace::Write(um); }
            }
            if(g_v6593_mode.load(std::memory_order_relaxed)!=0){
                std::lock_guard<std::mutex> lk(g_v6593_mutex);
                V6593ObserveBarrierLocked(list_this,b);
            }
            if(g_v6612_tls_compose_slot>=0 && b.Type==D3D12_RESOURCE_BARRIER_TYPE_TRANSITION && b.Transition.pResource){
                int eye=-1;std::uint32_t frame=0,call=0;
                { std::lock_guard<std::mutex> lk(g_v6610_output_mutex); const int slot=g_v6612_tls_compose_slot;
                  if(slot>=0 && slot<g_v6612_compose_count){auto& rec=g_v6612_compose_records[static_cast<std::size_t>(slot)];eye=rec.eye;frame=rec.frame;call=++rec.barrier_calls;
                    if(rec.transition_count<16){auto& t=rec.transitions[rec.transition_count++];t.resource=b.Transition.pResource;t.before=b.Transition.StateBefore;t.after=b.Transition.StateAfter;} } }
                if(eye>=0 && call<=16){char cm[448]{};std::snprintf(cm,sizeof(cm),
                    "V66.12-F9-COMPOSE-BARRIER eye=%c frame=%u call=%u list=%p resource=%p sub=%u before=0x%X after=0x%X",
                    eye?'R':'L',frame,call,reinterpret_cast<void*>(list_this),reinterpret_cast<void*>(b.Transition.pResource),
                    b.Transition.Subresource,(unsigned)b.Transition.StateBefore,(unsigned)b.Transition.StateAfter);
                    diagnostic_trace::Write(cm);
                }
            }
            if (b.Type != D3D12_RESOURCE_BARRIER_TYPE_TRANSITION || !b.Transition.pResource) continue;

            if(g_v6610_output_enabled.load(std::memory_order_acquire)){
                const int eye=g_v6610_output_eye.load(std::memory_order_acquire);
                if(eye>=0 && eye<=1){
                    int idx=-1; D3D12_RESOURCE_DESC rd{}; bool sameMain=false; bool censusOnly=false;
                    { std::lock_guard<std::mutex> lk(g_v6610_output_mutex);
                      constexpr D3D12_RESOURCE_STATES kSrvStates=static_cast<D3D12_RESOURCE_STATES>(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE|D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
                      const auto after=b.Transition.StateAfter, before=b.Transition.StateBefore;
                      const bool afterWrite=(after==D3D12_RESOURCE_STATE_RENDER_TARGET)||((after&D3D12_RESOURCE_STATE_UNORDERED_ACCESS)!=0)||((after&D3D12_RESOURCE_STATE_COPY_DEST)!=0)||((after&D3D12_RESOURCE_STATE_DEPTH_WRITE)!=0);
                      const bool afterRead=!afterWrite && (((after&kSrvStates)!=0)||((after&D3D12_RESOURCE_STATE_COPY_SOURCE)!=0));
                      for(int j=0;j<g_v6610_output_count;++j)if(g_v6610_output_candidates[static_cast<std::size_t>(j)].resource==b.Transition.pResource){ idx=j; break; }
                      // V66.47: also register screen-sized textures first seen as UAV/SRV (compute
                      // lighting/history surfaces are never bound as render targets).
                      if(kV6647HistoryCensus && idx<0 && (afterWrite||afterRead) && g_v6610_output_count<200 && /* V66.50: keep 56 slots for render-target candidates */
                         !g_v6647_rejected.count(b.Transition.pResource)) {
                          const auto nd=b.Transition.pResource->GetDesc();
                          if(nd.Dimension==D3D12_RESOURCE_DIMENSION_TEXTURE2D && nd.Width>=200 && nd.Height>=150) { // V66.50: include quarter-res (SVO etc.)
                              idx=g_v6610_output_count++; auto& c=g_v6610_output_candidates[static_cast<std::size_t>(idx)];
                              c=V6610OutputCandidate{}; c.resource=b.Transition.pResource; c.desc=nd; c.census_only=true; c.via_uav=(after&D3D12_RESOURCE_STATE_UNORDERED_ACCESS)!=0;
                          } else {
                              if(g_v6647_rejected.size()>8192) g_v6647_rejected.clear();
                              g_v6647_rejected.insert(b.Transition.pResource);
                          }
                      }
                      if(idx>=0){
                          auto& c=g_v6610_output_candidates[static_cast<std::size_t>(idx)]; rd=c.desc;sameMain=c.same_main;censusOnly=c.census_only;++c.barriers[eye];c.last_frame[eye]=g_v6610_output_frame.load(std::memory_order_relaxed);
                          c.eye_mask|=(1u<<eye);
                          const bool beforeSrv=(before & kSrvStates)!=0;
                          const bool afterSrv=(after & kSrvStates)!=0;
                          if(!beforeSrv && afterSrv) ++c.to_srv[eye];
                          if(beforeSrv && !afterSrv) ++c.from_srv[eye];
                          if(after==D3D12_RESOURCE_STATE_RENDER_TARGET && before!=D3D12_RESOURCE_STATE_RENDER_TARGET) ++c.to_rt[eye];
                          if((after&D3D12_RESOURCE_STATE_UNORDERED_ACCESS)!=0) c.via_uav=true;
                          if(kV6647HistoryCensus && (afterWrite||afterRead)) V6647NoteAccess(c,eye,afterWrite);
                      } }
                    if(idx>=0 && !censusOnly){ V6612TagList(list_this,eye,g_v6610_output_frame.load(std::memory_order_relaxed),false,true,false,sameMain); const auto n=g_v6610_output_event.fetch_add(1,std::memory_order_relaxed)+1;
                        if(n<=160){ char m[448]{}; std::snprintf(m,sizeof(m),
                            "V66.12-F9-BARRIER n=%llu eye=%c frame=%u idx=%d list=%p resource=%p size=%llux%u fmt=%u sub=%u before=0x%X after=0x%X",
                            (unsigned long long)n,eye?'R':'L',g_v6610_output_frame.load(std::memory_order_relaxed),idx,list_this,b.Transition.pResource,
                            (unsigned long long)rd.Width,rd.Height,(unsigned)rd.Format,b.Transition.Subresource,(unsigned)b.Transition.StateBefore,(unsigned)b.Transition.StateAfter); diagnostic_trace::Write(m); }
                    }
                }
            }

            // V66.12: keep the outer RT->PRESENT list alive through queue submission
            // even after the L/R CPU phases have ended.  The first submitted main
            // PRESENT boundary after the one-shot run closes the command-order drain.
            if(g_v6610_output_enabled.load(std::memory_order_acquire) &&
               b.Transition.StateAfter==D3D12_RESOURCE_STATE_PRESENT){
                bool sameMain=false;
                { std::lock_guard<std::mutex> bl(s_bridge_capture_mutex); sameMain=(b.Transition.pResource==s_bridge_current_main_target); }
                // D85R6: preserve the completed temporal RIGHT scene on this
                // exact final main-target list BEFORE the desktop HUD composite.
                // The call records only; publication waits for this generation's
                // actual submission, regardless of the optional scaling blit.
                if(sameMain&&g_d82_temporal_tracking.load(std::memory_order_acquire))
                    (void)RecordTemporalFinalMainD85(list_this,b.Transition.pResource,b.Transition.StateBefore);
                // V66.64: the finished HUD texture is copied to the cache ring on this same list.
                if(sameMain && g_v6624_late_window.load(std::memory_order_acquire))
                    hud_layer_v6664::OnPresentBarrier(list_this,s_original_resource_barrier,b.Transition.pResource,b.Transition.StateBefore,s_original_om_set_render_targets,s_original_draw_instanced);
                if(sameMain && g_v6624_late_window.load(std::memory_order_acquire)) g_v6624_late_window.store(false,std::memory_order_release);
                if(sameMain && g_v6613_pair_capture_active.load(std::memory_order_acquire))
                    V6612TagList(list_this,-1,0,false,true,false,false,true,b.Transition.pResource);
            }

            // V65.38 bridge: a PRESENT -> RENDER_TARGET transition uniquely
            // identifies the live swapchain backbuffer about to receive the
            // native scene. Keep exact list/resource/state identity so F8-5 can
            // append the first-eye preservation copy before recording eye two.
            if (b.Transition.StateBefore == D3D12_RESOURCE_STATE_PRESENT &&
                b.Transition.StateAfter == D3D12_RESOURCE_STATE_RENDER_TARGET) {
                std::lock_guard<std::mutex> bridge_lock(s_bridge_capture_mutex);
                if (s_bridge_current_main_target != b.Transition.pResource) {
                    b.Transition.pResource->AddRef();
                    if (s_bridge_current_main_target) s_bridge_current_main_target->Release();
                    s_bridge_current_main_target = b.Transition.pResource;
                }
                if (s_bridge_current_main_list != list_this) {
                    list_this->AddRef();
                    if (s_bridge_current_main_list) s_bridge_current_main_list->Release();
                    s_bridge_current_main_list = list_this;
                }
                s_bridge_current_main_state = D3D12_RESOURCE_STATE_RENDER_TARGET;
                s_bridge_current_main_tid = GetCurrentThreadId();
            } else {
                std::lock_guard<std::mutex> bridge_lock(s_bridge_capture_mutex);
                if (s_bridge_current_main_target == b.Transition.pResource)
                    s_bridge_current_main_state = b.Transition.StateAfter;
            }

            // V65.47 persistent-owner capture: preserve the currently armed eye inside
            // the GAME command list itself, immediately before its main target
            // transitions RENDER_TARGET -> PRESENT.  This removes the V65.45
            // post-return race where ExecuteCommandLists often had not happened yet.
            if (b.Transition.StateBefore == D3D12_RESOURCE_STATE_RENDER_TARGET &&
                b.Transition.StateAfter == D3D12_RESOURCE_STATE_PRESENT) {
                std::lock_guard<std::mutex> bridge_lock(s_bridge_capture_mutex);
                std::uint64_t txn_serial=0; int txn_eye=-1;
                if(s_bridge_current_main_target==b.Transition.pResource && s_bridge_current_main_list==list_this && !g_bridge_tickets_v6547.empty()) {
                    const auto ticket=g_bridge_tickets_v6547.front();
                    g_bridge_tickets_v6547.pop_front();
                    txn_serial=ticket.serial; txn_eye=ticket.eye;
                    if(txn_serial!=0 && (txn_eye==0 || txn_eye==1)) {
                        auto* device=s_captured_device.load(std::memory_order_acquire);
                        ID3D12Resource*& capture=(txn_eye==0)?s_bridge_left_capture:s_bridge_right_capture;
                        D3D12_RESOURCE_DESC& capture_desc=(txn_eye==0)?s_bridge_left_desc:s_bridge_right_desc;
                        const auto src_desc=b.Transition.pResource->GetDesc();
                        const bool desc_ok=src_desc.Dimension==D3D12_RESOURCE_DIMENSION_TEXTURE2D && src_desc.Width>0 &&
                            src_desc.Height>0 && src_desc.SampleDesc.Count==1;
                        bool capture_ok=desc_ok && device;
                        if(capture_ok && capture) {
                            capture_ok=capture_desc.Dimension==src_desc.Dimension && capture_desc.Width==src_desc.Width &&
                                capture_desc.Height==src_desc.Height && capture_desc.Format==src_desc.Format &&
                                capture_desc.SampleDesc.Count==src_desc.SampleDesc.Count && capture_desc.Flags==src_desc.Flags;
                        }
                        if(capture_ok && !capture) {
                            D3D12_HEAP_PROPERTIES heap{}; heap.Type=D3D12_HEAP_TYPE_DEFAULT; heap.CreationNodeMask=1; heap.VisibleNodeMask=1;
                            if(SUCCEEDED(device->CreateCommittedResource(&heap,D3D12_HEAP_FLAG_NONE,&src_desc,D3D12_RESOURCE_STATE_COMMON,nullptr,IID_PPV_ARGS(&capture)))) {
                                capture_desc=src_desc;
                                char cm[320]{}; std::snprintf(cm,sizeof(cm),
                                    "V65.47-OWNER-CAPTURE create eye=%c cache=%p size=%llux%u fmt=%u",
                                    txn_eye==0?'L':'R',capture,static_cast<unsigned long long>(src_desc.Width),src_desc.Height,static_cast<unsigned>(src_desc.Format));
                                debug_trace::Write(cm);
                            } else capture_ok=false;
                        }
                        if(capture_ok && capture && s_original_resource_barrier) {
                            D3D12_RESOURCE_BARRIER pre[2]{};
                            pre[0].Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION; pre[0].Transition.pResource=b.Transition.pResource;
                            pre[0].Transition.Subresource=D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
                            pre[0].Transition.StateBefore=D3D12_RESOURCE_STATE_RENDER_TARGET; pre[0].Transition.StateAfter=D3D12_RESOURCE_STATE_COPY_SOURCE;
                            pre[1].Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION; pre[1].Transition.pResource=capture;
                            pre[1].Transition.Subresource=D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
                            pre[1].Transition.StateBefore=D3D12_RESOURCE_STATE_COMMON; pre[1].Transition.StateAfter=D3D12_RESOURCE_STATE_COPY_DEST;
                            s_original_resource_barrier(list_this,2,pre);
                            list_this->CopyResource(capture,b.Transition.pResource);
                            D3D12_RESOURCE_BARRIER post[2]= {pre[0],pre[1]};
                            post[0].Transition.StateBefore=D3D12_RESOURCE_STATE_COPY_SOURCE; post[0].Transition.StateAfter=D3D12_RESOURCE_STATE_RENDER_TARGET;
                            post[1].Transition.StateBefore=D3D12_RESOURCE_STATE_COPY_DEST; post[1].Transition.StateAfter=D3D12_RESOURCE_STATE_COMMON;
                            s_original_resource_barrier(list_this,2,post);
                            if(txn_eye==0) s_bridge_left_recorded_serial.store(txn_serial,std::memory_order_release);
                            else s_bridge_right_recorded_serial.store(txn_serial,std::memory_order_release);
                            g_bridge_cert_cv_v6550.notify_all();
                            auto& pst=BridgePairStateForV6547(txn_serial);
                            pst.recorded_mask |= (txn_eye==0?1u:2u);
                            auto& tag=g_bridge_list_tags_v6547[static_cast<std::size_t>(g_bridge_list_tag_cursor_v6547++ % g_bridge_list_tags_v6547.size())];
                            tag.list=list_this; tag.serial=txn_serial; tag.eye=txn_eye;
                            const auto n=s_bridge_copy_count.fetch_add(1,std::memory_order_relaxed)+1;
                            if(n<=16 || (n%240ull)==0) { char cm[384]{}; std::snprintf(cm,sizeof(cm),
                                "V65.47-OWNER-CAPTURE record=%llu serial=%llu eye=%c list=%p source=%p cache=%p inList=1 state=RT>PRESENT",
                                static_cast<unsigned long long>(n),static_cast<unsigned long long>(txn_serial),txn_eye==0?'L':'R',list_this,b.Transition.pResource,capture); debug_trace::Write(cm); }
                        }
                    }
                }
            }

            // V59.6 scene-color archaeology: exact-size normal-view textures only.
            // This is deliberately read-only. The thread-local eye comes from the
            // explicit dual Execute pass, so L/R attribution happens before PRESENT.
            if (s_scene_color_probe_enabled.load(std::memory_order_acquire) &&
                FrameSync::CurrentPipelineViewType() == 0) {
                const auto pw = s_scene_color_probe_width.load(std::memory_order_acquire);
                const auto ph = s_scene_color_probe_height.load(std::memory_order_acquire);
                const auto rd = b.Transition.pResource->GetDesc();
                const bool rt_touch = b.Transition.StateAfter == D3D12_RESOURCE_STATE_RENDER_TARGET ||
                                      b.Transition.StateBefore == D3D12_RESOURCE_STATE_RENDER_TARGET;
                if (rt_touch && rd.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D &&
                    rd.Width == static_cast<UINT64>(pw) && rd.Height == ph) {
                    const int eye = FrameSync::CurrentNativeStereoPassEye();
                    const auto cn = s_scene_color_probe_barrier_count.fetch_add(1,std::memory_order_relaxed)+1;
                    if (eye == 0) s_scene_color_probe_left_count.fetch_add(1,std::memory_order_relaxed);
                    else if (eye == 1) s_scene_color_probe_right_count.fetch_add(1,std::memory_order_relaxed);
                    if (cn <= 240 || (cn % 1200ull)==0) {
                        char sm[384]{}; std::snprintf(sm,sizeof(sm),
                            "V59.6-SCENE-RT n=%llu eye=%s list=%p resource=%p size=%llux%u fmt=%u flags=0x%X before=0x%X after=0x%X sub=%u tid=%lu",
                            static_cast<unsigned long long>(cn), eye==0?"L":eye==1?"R":"MONO",
                            list_this,b.Transition.pResource,static_cast<unsigned long long>(rd.Width),rd.Height,
                            static_cast<unsigned>(rd.Format),static_cast<unsigned>(rd.Flags),
                            static_cast<unsigned>(b.Transition.StateBefore),static_cast<unsigned>(b.Transition.StateAfter),
                            b.Transition.Subresource,static_cast<unsigned long>(GetCurrentThreadId()));
                        debug_trace::Write(sm);
                    }
                }
            }

            // V46.1 CustomRes proof: when the matrix asks CryEngine to render
            // at a specific r_CustomResWidth/Height, identify exact-size D3D12
            // textures participating in transitions. This is read-only; it does
            // not assume which one is color until a RENDER_TARGET transition is
            // observed. The target dimensions are atomically changed by the XR
            // worker between 5-second phases.
            if (s_customres_probe_enabled.load(std::memory_order_acquire)) {
                const auto tw = s_customres_probe_width.load(std::memory_order_acquire);
                const auto th = s_customres_probe_height.load(std::memory_order_acquire);
                const auto rd = b.Transition.pResource->GetDesc();
                if (rd.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D &&
                    rd.Width == static_cast<UINT64>(tw) && rd.Height == th) {
                    const auto cn = s_customres_barrier_count.fetch_add(1, std::memory_order_relaxed) + 1;
                    if (b.Transition.StateAfter == D3D12_RESOURCE_STATE_RENDER_TARGET ||
                        b.Transition.StateBefore == D3D12_RESOURCE_STATE_RENDER_TARGET)
                        s_customres_rt_barrier_count.fetch_add(1, std::memory_order_relaxed);
                    if (cn <= 96 || (cn % 600ull) == 0) {
                        char cm[336]{};
                        std::snprintf(cm, sizeof(cm),
                            "V46.1-CUSTOMRES-RESOURCE barrier=%llu target=%ux%u list=%p resource=%p size=%llux%u fmt=%u before=0x%X after=0x%X sub=%u tid=%lu",
                            static_cast<unsigned long long>(cn), tw, th, list_this, b.Transition.pResource,
                            static_cast<unsigned long long>(rd.Width), rd.Height, static_cast<unsigned>(rd.Format),
                            static_cast<unsigned>(b.Transition.StateBefore), static_cast<unsigned>(b.Transition.StateAfter),
                            b.Transition.Subresource, static_cast<unsigned long>(GetCurrentThreadId()));
                        debug_trace::Write(cm);
                    }
                }
            }

            // V45: when this command list is being recorded from a type-1
            // Recursive View pipeline on THIS SAME THREAD, tag its texture/depth
            // transitions. Thread-local classification avoids cross-thread render
            // contamination in KCD2's multi-threaded renderer.
            if (FrameSync::CurrentPipelineViewType() == 1) {
                const auto rn = s_recursive_barrier_count.fetch_add(1, std::memory_order_relaxed) + 1;
                if (b.Transition.StateAfter == D3D12_RESOURCE_STATE_RENDER_TARGET)
                    s_recursive_rt_barrier_count.fetch_add(1, std::memory_order_relaxed);
                if (rn <= 160 || (rn % 600ull) == 0) {
                    const auto rd = b.Transition.pResource->GetDesc();
                    char rm[320]{};
                    std::snprintf(rm, sizeof(rm),
                        "V45-RECURSIVE-RESOURCE barrier=%llu list=%p resource=%p dim=%u size=%llux%u fmt=%u before=0x%X after=0x%X sub=%u tid=%lu",
                        rn, list_this, b.Transition.pResource, static_cast<unsigned>(rd.Dimension),
                        static_cast<unsigned long long>(rd.Width), rd.Height, static_cast<unsigned>(rd.Format),
                        static_cast<unsigned>(b.Transition.StateBefore), static_cast<unsigned>(b.Transition.StateAfter),
                        b.Transition.Subresource, static_cast<unsigned long>(GetCurrentThreadId()));
                    debug_trace::Write(rm);
                }
            }

            // V65.93 F5 intentionally performs NO copy here. ResourceBarrier is
            // census-only; any certified copy is appended later on the same DIRECT
            // queue after the game's submitted-PRESENT batch.

            // V65.87: same-frame MRT0/MRT1 pair capture.  Only the command list
            // that latched the pair may write the owned snapshots. Each source is
            // copied exactly once at its real RT->next-state barrier.
            if(g_v6587_enabled.load(std::memory_order_acquire) && native_stereo_outer_lab::BridgeStage()==3 &&
               b.Transition.StateBefore==D3D12_RESOURCE_STATE_RENDER_TARGET && b.Transition.StateAfter!=D3D12_RESOURCE_STATE_RENDER_TARGET){
                int eye=-1; ID3D12Resource* snap=nullptr; bool claimed=false;
                {
                    std::lock_guard<std::mutex> lk(g_v6587_mutex);
                    if(list_this==g_v6587_list){
                        if(b.Transition.pResource==g_v6587_source[0] && !g_v6587_snapshot_done[0]) eye=0;
                        else if(b.Transition.pResource==g_v6587_source[1] && !g_v6587_snapshot_done[1]) eye=1;
                        if(eye>=0 && V6587EnsureSnapshotLocked(b.Transition.pResource,eye)){
                            snap=g_v6587_snapshot[eye]; g_v6587_snapshot_done[eye]=true; claimed=true;
                        }
                    }
                }
                if(claimed && snap && s_original_resource_barrier){
                    D3D12_RESOURCE_BARRIER pre[2]{};
                    pre[0].Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION; pre[0].Transition.pResource=b.Transition.pResource; pre[0].Transition.Subresource=D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES; pre[0].Transition.StateBefore=D3D12_RESOURCE_STATE_RENDER_TARGET; pre[0].Transition.StateAfter=D3D12_RESOURCE_STATE_COPY_SOURCE;
                    pre[1].Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION; pre[1].Transition.pResource=snap; pre[1].Transition.Subresource=D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES; pre[1].Transition.StateBefore=D3D12_RESOURCE_STATE_COMMON; pre[1].Transition.StateAfter=D3D12_RESOURCE_STATE_COPY_DEST;
                    s_original_resource_barrier(list_this,2,pre);
                    list_this->CopyResource(snap,b.Transition.pResource);
                    D3D12_RESOURCE_BARRIER post[2]={pre[0],pre[1]};
                    post[0].Transition.StateBefore=D3D12_RESOURCE_STATE_COPY_SOURCE; post[0].Transition.StateAfter=D3D12_RESOURCE_STATE_RENDER_TARGET;
                    post[1].Transition.StateBefore=D3D12_RESOURCE_STATE_COPY_DEST; post[1].Transition.StateAfter=D3D12_RESOURCE_STATE_COMMON;
                    s_original_resource_barrier(list_this,2,post);
                    bool both=false; { std::lock_guard<std::mutex> lk(g_v6587_mutex); both=g_v6587_snapshot_done[0]&&g_v6587_snapshot_done[1]; if(both) g_v6587_recorded=true; }
                    char m[384]{}; std::snprintf(m,sizeof(m),
                        "V65.87-PAIR SNAP eye=%c list=%p source=%p copy=%p nextState=0x%X bothRecorded=%d",
                        eye==0?'L':'R',list_this,b.Transition.pResource,snap,(unsigned)b.Transition.StateAfter,both?1:0); diagnostic_trace::Write(m);
                }
            }

            // V65.85: output-surface census. Log UAV-capable/tiled/array/subresource
            // transitions while COUNT2 owns the frame, and refresh any selected
            // full-size candidate snapshot at its exact RT->next-state barrier.
            if (g_v6585_enabled.load(std::memory_order_acquire) && native_stereo_outer_lab::BridgeStage()==3) {
                const auto rd85=b.Transition.pResource->GetDesc();
                const bool uavCap=(rd85.Flags & D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS)!=0;
                const bool arrayish=rd85.DepthOrArraySize>1;
                const bool tiled=rd85.Layout==D3D12_TEXTURE_LAYOUT_64KB_UNDEFINED_SWIZZLE || rd85.Layout==D3D12_TEXTURE_LAYOUT_64KB_STANDARD_SWIZZLE;
                if(uavCap || arrayish || tiled || b.Transition.Subresource!=D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES){
                    const auto n=g_v6585_event.fetch_add(1,std::memory_order_relaxed)+1;
                    if(n<=160){ char um[448]{}; std::snprintf(um,sizeof(um),
                        "V65.85-RESOURCE n=%llu list=%p resource=%p size=%llux%u arr=%u fmt=%u flags=0x%X layout=%u sub=%u before=0x%X after=0x%X uavCap=%d tiled=%d",
                        (unsigned long long)n,list_this,b.Transition.pResource,(unsigned long long)rd85.Width,rd85.Height,(unsigned)rd85.DepthOrArraySize,(unsigned)rd85.Format,(unsigned)rd85.Flags,(unsigned)rd85.Layout,b.Transition.Subresource,(unsigned)b.Transition.StateBefore,(unsigned)b.Transition.StateAfter,uavCap?1:0,tiled?1:0); diagnostic_trace::Write(um); }
                }
                if (b.Transition.StateBefore==D3D12_RESOURCE_STATE_RENDER_TARGET && b.Transition.StateAfter!=D3D12_RESOURCE_STATE_RENDER_TARGET) {
                    // V65.85a: one-shot, selected-candidate-only capture. V65.85 refreshed all
                    // four candidates on every matching deferred command list and crashed in
                    // D3D12Core. Claim the selected source exactly once before recording any
                    // copy commands; other lists then remain read-only. Frozen is intentional.
                    const int wanted=g_v6585_selected.load(std::memory_order_acquire);
                    int idx=-1; ID3D12Resource* snap=nullptr; bool claimed=false;
                    {
                        std::lock_guard<std::mutex> lk(g_v6585_mutex);
                        if(wanted>=0 && wanted<g_v6585_count && g_v6585_source[wanted]==b.Transition.pResource && !g_v6585_snapshot_done[wanted]) {
                            idx=wanted;
                            if(V6585EnsureSnapshotLocked(b.Transition.pResource,idx)) {
                                snap=g_v6585_snapshot[idx];
                                g_v6585_snapshot_done[idx]=true; // claim before emitting GPU commands
                                claimed=true;
                            }
                        }
                    }
                    if(claimed && idx>=0 && snap && s_original_resource_barrier){
                        D3D12_RESOURCE_BARRIER pre[2]{};
                        pre[0].Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION; pre[0].Transition.pResource=b.Transition.pResource; pre[0].Transition.Subresource=D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES; pre[0].Transition.StateBefore=D3D12_RESOURCE_STATE_RENDER_TARGET; pre[0].Transition.StateAfter=D3D12_RESOURCE_STATE_COPY_SOURCE;
                        pre[1].Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION; pre[1].Transition.pResource=snap; pre[1].Transition.Subresource=D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES; pre[1].Transition.StateBefore=D3D12_RESOURCE_STATE_COMMON; pre[1].Transition.StateAfter=D3D12_RESOURCE_STATE_COPY_DEST;
                        s_original_resource_barrier(list_this,2,pre);
                        list_this->CopyResource(snap,b.Transition.pResource);
                        D3D12_RESOURCE_BARRIER post[2]={pre[0],pre[1]};
                        post[0].Transition.StateBefore=D3D12_RESOURCE_STATE_COPY_SOURCE; post[0].Transition.StateAfter=D3D12_RESOURCE_STATE_RENDER_TARGET;
                        post[1].Transition.StateBefore=D3D12_RESOURCE_STATE_COPY_DEST; post[1].Transition.StateAfter=D3D12_RESOURCE_STATE_COMMON;
                        s_original_resource_barrier(list_this,2,post);
                        std::uint64_t ser=0; { std::lock_guard<std::mutex> lk(g_v6585_mutex); ser=++g_v6585_serial[idx]; }
                        char cm[384]{}; std::snprintf(cm,sizeof(cm),
                            "V65.85a-CANDIDATE SNAP-ONCE idx=%d serial=%llu list=%p source=%p copy=%p nextState=0x%X",
                            idx,(unsigned long long)ser,list_this,b.Transition.pResource,snap,(unsigned)b.Transition.StateAfter); diagnostic_trace::Write(cm);
                    }
                }
            }

            // V65.61: once the candidate pair has been selected, snapshot each eye at the
            // candidate's actual RENDER_TARGET -> next-state barrier. Inject a local
            // RT->COPY_SOURCE->RT detour before the game's original transition, and copy into
            // our own COMMON-state resource. No guessed source state is used.
            if (b.Transition.StateBefore == D3D12_RESOURCE_STATE_RENDER_TARGET &&
                b.Transition.StateAfter != D3D12_RESOURCE_STATE_RENDER_TARGET) {
                int eye=-1; ID3D12Resource* snap=nullptr;
                {
                    std::lock_guard<std::mutex> lk(g_v6561_candidate_mutex);
                    if (g_v6561_pair_selected && list_this==g_v6561_candidate_list) {
                        if (b.Transition.pResource==g_v6561_candidate_source[0] && !g_v6561_snapshot_done[0]) eye=0;
                        else if (b.Transition.pResource==g_v6561_candidate_source[1] && !g_v6561_snapshot_done[1]) eye=1;
                        if (eye>=0 && V6561EnsureSnapshot(b.Transition.pResource,eye)) snap=g_v6561_candidate_snapshot[eye];
                    }
                }
                if (eye>=0 && snap && s_original_resource_barrier) {
                    D3D12_RESOURCE_BARRIER pre[2]{};
                    pre[0].Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION; pre[0].Transition.pResource=b.Transition.pResource; pre[0].Transition.Subresource=D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES; pre[0].Transition.StateBefore=D3D12_RESOURCE_STATE_RENDER_TARGET; pre[0].Transition.StateAfter=D3D12_RESOURCE_STATE_COPY_SOURCE;
                    pre[1].Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION; pre[1].Transition.pResource=snap; pre[1].Transition.Subresource=D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES; pre[1].Transition.StateBefore=D3D12_RESOURCE_STATE_COMMON; pre[1].Transition.StateAfter=D3D12_RESOURCE_STATE_COPY_DEST;
                    s_original_resource_barrier(list_this,2,pre);
                    list_this->CopyResource(snap,b.Transition.pResource);
                    D3D12_RESOURCE_BARRIER post[2]= {pre[0],pre[1]};
                    post[0].Transition.StateBefore=D3D12_RESOURCE_STATE_COPY_SOURCE; post[0].Transition.StateAfter=D3D12_RESOURCE_STATE_RENDER_TARGET;
                    post[1].Transition.StateBefore=D3D12_RESOURCE_STATE_COPY_DEST; post[1].Transition.StateAfter=D3D12_RESOURCE_STATE_COMMON;
                    s_original_resource_barrier(list_this,2,post);
                    { std::lock_guard<std::mutex> lk(g_v6561_candidate_mutex); g_v6561_snapshot_done[eye]=true; }
                    char cm[320]{}; std::snprintf(cm,sizeof(cm),
                        "V65.61-CANDIDATE-SNAPSHOT eye=%c serial=%llu list=%p source=%p copy=%p nextState=0x%X",
                        eye==0?'A':'B',(unsigned long long)g_v6561_pair_serial,list_this,b.Transition.pResource,snap,(unsigned)b.Transition.StateAfter); diagnostic_trace::Write(cm);
                }
            }

            if (b.Transition.StateAfter != D3D12_RESOURCE_STATE_PRESENT) continue;
            ID3D12Resource* resource = b.Transition.pResource;
            const D3D12_RESOURCE_DESC desc = resource->GetDesc();
            if (desc.Dimension != D3D12_RESOURCE_DIMENSION_TEXTURE2D || desc.Width == 0 || desc.Height == 0) continue;
            {const auto f=static_cast<std::uint32_t>(desc.Format);const auto last=g_present_fmt_last_r111.exchange(f,std::memory_order_acq_rel);
             if(last&&last!=f){g_present_fmt_prev_r111.store(last,std::memory_order_release);g_present_fmt_changes_r111.fetch_add(1,std::memory_order_relaxed);}}
            resource->AddRef();
            ID3D12Resource* old = nullptr;
            int captured_eye = 0;
            bool advance_afr_eye = false;
            std::uint64_t captured_serial = 0;
            {
                std::lock_guard<std::mutex> lock(s_present_resource_mutex);
                old = s_latest_present_resource;
                s_latest_present_resource = resource;
                s_latest_present_command_list = list_this;
                captured_eye = s_afr_stereo_capture.load(std::memory_order_acquire) ? FrameSync::CurrentEye() : 0;
                s_latest_present_eye = captured_eye;
                captured_serial = ++s_latest_present_serial;
                s_latest_present_tick_ms = static_cast<std::uint64_t>(GetTickCount64());
                const std::uint64_t latest_global_pose = CameraHook::LastPoseFrameIndex();
                const std::uint64_t renderer_match_candidate = FrameSync::LatestRendererMatchedPoseFrame(50);
                // Fail closed if the last renderer match is stale relative to the
                // current camera publisher. The headset evidence showed the real
                // mismatch as renderer==latest or renderer==latest-1; allow a
                // two-token window for scheduling jitter, never an arbitrary old pose.
                const bool renderer_match_fresh = renderer_match_candidate != 0 &&
                    latest_global_pose >= renderer_match_candidate &&
                    (latest_global_pose - renderer_match_candidate) <= 2ull;
                const std::uint64_t renderer_owned_pose = renderer_match_fresh ? renderer_match_candidate : 0ull;
                s_latest_present_camera_frame = renderer_owned_pose ? renderer_owned_pose : latest_global_pose;
                if(FrameSync::HeadOwnershipFixEnabled()) {
                    static std::atomic<std::uint64_t> ownership_fix_samples{0};
                    const auto on=ownership_fix_samples.fetch_add(1,std::memory_order_relaxed)+1;
                    if(on<=8 || (on%600ull)==0) {
                        char om[288]{}; std::snprintf(om,sizeof(om),
                            "V65.04-F2-FIX PRESENT serial=%llu rendererPose=%llu latestGlobal=%llu selected=%llu fallback=%d",
                            (unsigned long long)captured_serial,
                            (unsigned long long)renderer_owned_pose,
                            (unsigned long long)latest_global_pose,
                            (unsigned long long)s_latest_present_camera_frame,
                            renderer_owned_pose?0:1);
                        debug_trace::Write(om);
                    }
                }
                advance_afr_eye = false; // V41: PRESENT is not a frame boundary.
            }
            if (old) old->Release();

            // V65.58: the native count=2 pipeline publishes work asynchronously.  Keep
            // the passive RTV observer alive only until the first real game PRESENT
            // transition following the proven pipeline call, then snapshot and disarm.
            auto autoStop=s_stereo_producer_auto_stop_present_serial.load(std::memory_order_acquire);
            if(autoStop!=0 && captured_serial>=autoStop &&
               s_stereo_producer_auto_stop_present_serial.compare_exchange_strong(autoStop,captured_serial,std::memory_order_acq_rel)) {
                const auto binds=s_stereo_producer_bind_count.load(std::memory_order_acquire);
                const auto unique=s_stereo_producer_unique_count.load(std::memory_order_acquire);
                const auto pairs=s_stereo_producer_pair_count.load(std::memory_order_acquire);
                const auto detail=s_stereo_producer_detailed_bind_count.load(std::memory_order_acquire);
                s_stereo_producer_probe_enabled.store(false,std::memory_order_release);
                s_stereo_producer_auto_stop_present_serial.store(0,std::memory_order_release);
                char sm[384]{}; std::snprintf(sm,sizeof(sm),
                    "V65.63-ROUTE WINDOW RETURN stage=%d eye=%s present=%llu target=%llu route={binds:%llu uniqueResources:%llu sameEpochPairs:%llu detailed:%llu} AUTO-DISARM",
                    s_stereo_producer_oracle_stage.load(std::memory_order_acquire),
                    s_stereo_producer_oracle_stage.load(std::memory_order_acquire)==1?"LEFT":(s_stereo_producer_oracle_stage.load(std::memory_order_acquire)==2?"RIGHT":(s_stereo_producer_oracle_stage.load(std::memory_order_acquire)==3?"COUNT2":"UNLABELED")),
                    static_cast<unsigned long long>(captured_serial),static_cast<unsigned long long>(autoStop),
                    static_cast<unsigned long long>(binds),static_cast<unsigned long long>(unique),
                    static_cast<unsigned long long>(pairs),static_cast<unsigned long long>(detail));
                diagnostic_trace::Write(sm);
                debug_trace::Write(sm);
            }
            const auto n = s_present_capture_count.fetch_add(1, std::memory_order_relaxed) + 1;
            if (n <= 4) {
                KCD2VR_DIAG_LOG(Bug,info,"V37-PRESENT-CAPTURE call={} resource={:#x} {}x{} fmt={} before={} after=PRESENT",
                    n, reinterpret_cast<uintptr_t>(resource), static_cast<unsigned long long>(desc.Width),
                    desc.Height, static_cast<int>(desc.Format), static_cast<unsigned>(b.Transition.StateBefore));
                if (s_afr_stereo_capture.load(std::memory_order_relaxed)) {
                    char msg[192]{}; std::snprintf(msg, sizeof(msg), "V41-PRESENT-TAG serial=%llu frame=%llu eye=%c resource=%p",
                        static_cast<unsigned long long>(captured_serial), static_cast<unsigned long long>(FrameSync::CurrentFrameSerial()), captured_eye == 0 ? 'L' : 'R', resource);
                    debug_trace::Write(msg);
                }
            }
        }
    }
    if (s_original_resource_barrier) s_original_resource_barrier(list_this, num_barriers, barriers);
}

bool DeviceHook::InstallExecuteHook(ID3D12CommandQueue* queue)
{
    if (!queue)
        return false;
    if (s_execute_hook_installed.load(std::memory_order_acquire))
        return true;

    void** vtable = *reinterpret_cast<void***>(queue);
    // ID3D12CommandQueue vtable: IUnknown 0-2, ID3D12Object 3-6,
    // ID3D12DeviceChild::GetDevice 7, queue methods start at 8.
    // ExecuteCommandLists is slot 10.
    constexpr size_t kExecuteCommandListsIndex = 10;
    void* target_address = vtable[kExecuteCommandListsIndex];

    KCD2VR_DIAG_LOG(Bug,info,"DeviceHook: ID3D12CommandQueue::ExecuteCommandLists vtable address = {:#x}",
               reinterpret_cast<uintptr_t>(target_address));

    hook::InlineRequest request{
        "kcd2vr_device_hook_execute_command_lists",
        hook::Target{DetourModKit::Address{reinterpret_cast<uintptr_t>(target_address)}},
        hook::Options{}
    };

    DetourModKit::Result<hook::Hook> result = hook::inline_at(std::move(request), &DeviceHook::DetourExecuteCommandLists);
    if (!result)
    {
        KCD2VR_DIAG_LOG(Bug,error,"DeviceHook: ExecuteCommandLists inline_at failed");
        return false;
    }

    s_hook_execute_command_lists = std::move(*result);
    s_original_execute_command_lists = s_hook_execute_command_lists->original<ExecuteCommandListsFunc>();
    auto enable_result = s_hook_execute_command_lists->enable();
    if (!enable_result)
    {
        KCD2VR_DIAG_LOG(Bug,error,"DeviceHook: ExecuteCommandLists hook failed to enable");
        s_original_execute_command_lists = nullptr;
        s_hook_execute_command_lists.reset();
        return false;
    }

    s_execute_hook_installed.store(true, std::memory_order_release);
    KCD2VR_DIAG_LOG(Bug,info,"DeviceHook: ExecuteCommandLists render-thread hook installed and enabled");
    return true;
}

void __stdcall DeviceHook::DetourExecuteCommandLists(
    ID3D12CommandQueue* queue_this, UINT num_command_lists, ID3D12CommandList* const* command_lists)
{
    // Ownership spans the pre-submit LEFT claim, the native queue call, and
    // publication. A new phase/reset cannot substitute another attempt midway.
    std::lock_guard<std::recursive_mutex> transaction(g_temporal_mutex_d85);
    ReapTemporalPinsD85();
    temporal_capture_d85::Ticket d85_ticket{};
    if(command_lists)for(UINT i=0;i<num_command_lists;++i){
        auto candidate=g_temporal_owner_d85.Incoming(reinterpret_cast<std::uintptr_t>(command_lists[i]));
        if(candidate.recorded){d85_ticket=candidate;break;}
    }
    // V66.15: V66.14 proved the useful layout is separate DIRECT-queue batches:
    // last LEFT main writer -> first RIGHT main writer -> PRESENT. Inspect the
    // already-recorded tag BEFORE submitting the first RIGHT main-writer batch,
    // so the LEFT copy can be inserted into the queue between those batches.
    const bool v6615_right_main_incoming =
        queue_this==s_captured_queue.load(std::memory_order_acquire) &&
        V6615IncomingLockedRightMainWriter(num_command_lists,command_lists);
    // D82 snapshots the exact recorded draw generation before ArchiveSubmitBatch
    // consumes it. Earlier main-resource transitions remain LEFT-copy boundaries,
    // but cannot certify temporal RIGHT or spend its once-only copy.
    const bool d82_temporal=g_d82_temporal_tracking.load(std::memory_order_acquire);
    const auto d82_composition=(d82_temporal&&queue_this==s_captured_queue.load(std::memory_order_acquire))
        ?V6615IncomingRightCompositionD82(num_command_lists,command_lists):temporal_compose_d82::Stamp{};
    if(d82_composition.Valid())g_d85_watchdog.Reach(temporal_pair_d85::StampSubmitted);
    bool v6615_left_ok=false;
    if(v6615_right_main_incoming && !g_v6615_left_copy_queued.exchange(true,std::memory_order_acq_rel)) {
        {
            const auto latest=CameraHook::LastPoseFrameIndex();
            const auto matched=FrameSync::LatestRendererMatchedPoseFrame(50);
            // V66.46: a fresh (<50 ms) renderer match IS the rendered pose, however far the
            // renderer lags the newest pose. The old "<=2 poses behind" rule replaced exactly
            // the correct token with the newest pose whenever the renderer lagged.
            const bool fresh=matched!=0 && latest>=matched;
            g_v6638_left_pose_frame.store(fresh?matched:latest,std::memory_order_release);
        }
        ID3D12Resource* d85_left=nullptr;ID3D12Resource* d85_source=nullptr;
        const auto d85_generation=g_temporal_owner_d85.Current().generation;
        if(d82_temporal){
            d85_source=reinterpret_cast<ID3D12Resource*>(g_temporal_owner_d85.Current().source);
            if(!d85_source){std::lock_guard<std::mutex> capture(s_bridge_capture_mutex);d85_source=s_bridge_current_main_target;}
        }
        const auto d85_state=static_cast<D3D12_RESOURCE_STATES>(g_temporal_owner_d85.Current().leftState);
        v6615_left_ok=BridgeCaptureCurrentMainTargetToLeft(0,d85_source,d82_temporal?&d85_left:nullptr,
            d82_temporal?&d85_state:nullptr,d82_temporal?g_v6615_last_left_main_batch.load(std::memory_order_acquire):0);
        if(d82_temporal)(void)g_temporal_owner_d85.MarkLeft(d85_generation,
            reinterpret_cast<std::uintptr_t>(d85_source),reinterpret_cast<std::uintptr_t>(d85_left),
            v6615_left_ok,g_temporal_owner_d85.Current().leftPose?g_temporal_owner_d85.Current().leftPose:
                g_v6638_left_pose_frame.load(std::memory_order_acquire));
        static std::atomic<unsigned long long> v6618left{0}; const auto ln=v6618left.fetch_add(1,std::memory_order_relaxed)+1;
        if(ln<=8 || (ln%120ull)==0 || !v6615_left_ok){ char m[448]{}; std::snprintf(m,sizeof(m),
            "V66.19R1-LIVE-COPY eye=L pair=%llu targetFrame=%u beforeRIGHT=1 lastLeftBatch=%llu result=%s source=ENGINE-MAIN queueOrder=BETWEEN-BATCHES",
            (unsigned long long)ln,g_v6613_pair_frame.load(std::memory_order_relaxed),
            (unsigned long long)g_v6615_last_left_main_batch.load(std::memory_order_relaxed),v6615_left_ok?"QUEUED":"FAIL"); diagnostic_trace::Write(m); }
        if(!v6615_left_ok) g_v6615_active.store(false,std::memory_order_release);
        if(!v6615_left_ok && d82_temporal) g_d85_watchdog.Refuse(temporal_pair_d85::Check::LeftCopy);
    }

    // Preserve the game's submission. Post-submit work below is queued after it.
    if (s_original_execute_command_lists)
        s_original_execute_command_lists(queue_this, num_command_lists, command_lists);

    // Publish HUD only after the batch carrying its copy is queued. Recording a
    // COMMON/PRESENT barrier is not evidence of command submission.
    if (s_original_execute_command_lists && queue_this == s_captured_queue.load(std::memory_order_acquire))
        hud_layer_v6664::OnSubmittedBatch(num_command_lists, command_lists);

    const auto execute_serial=s_execute_submission_count.fetch_add(1,std::memory_order_acq_rel)+1;
    if(s_original_execute_command_lists&&d85_ticket.recorded)
        FinishTemporalMainSubmissionD85(queue_this,d85_ticket,execute_serial);

    bool v6612_present_boundary_submitted=false;
    if(command_lists && g_v6610_output_enabled.load(std::memory_order_acquire) &&
       queue_this==s_captured_queue.load(std::memory_order_acquire)){
        v6612_present_boundary_submitted=V6612ArchiveSubmitBatch(execute_serial,num_command_lists,command_lists);
    }

    // V65.93: submission bookkeeping only. The game's batch has already been
    // queued. Mark every command list that touched a tagged candidate; no copy
    // or state mutation occurs here until the submitted-PRESENT boundary below.
    if(command_lists && g_v6593_mode.load(std::memory_order_acquire)!=0){
        std::lock_guard<std::mutex> lk(g_v6593_mutex);
        for(int k=0;k<2;++k){
            auto& c=g_v6593_candidate[k];
            for(std::size_t j=0;j<c.list_count;++j){
                if(c.submitted[j]) continue;
                for(UINT i=0;i<num_command_lists;++i){
                    if(command_lists[i]==static_cast<ID3D12CommandList*>(c.lists[j])){ c.submitted[j]=true; break; }
                }
            }
        }
    }

    // V65.88: publish a newly completed V65.61 COUNT2 A/B pair only after the
    // command list carrying both snapshot copies has been submitted. This is the
    // frame boundary used by F10 rung 1/2/3; the published refs survive the next
    // producer-window reset so OpenXR never races a cleared working pair.
    if(command_lists && g_v6588_live_enabled.load(std::memory_order_acquire)){
        std::lock_guard<std::mutex> clk(g_v6561_candidate_mutex);
        if(g_v6561_pair_selected && g_v6561_snapshot_done[0] && g_v6561_snapshot_done[1] &&
           g_v6561_candidate_snapshot[0] && g_v6561_candidate_snapshot[1] && g_v6561_candidate_list &&
           g_v6561_pair_serial!=g_v6588_source_pair_serial.load(std::memory_order_acquire)){
            bool submitted=false;
            for(UINT i=0;i<num_command_lists;++i) if(command_lists[i]==static_cast<ID3D12CommandList*>(g_v6561_candidate_list)){ submitted=true; break; }
            if(submitted){
                std::lock_guard<std::mutex> plk(g_v6588_live_mutex);
                for(auto*& r:g_v6588_live_pair){ if(r){ r->Release(); r=nullptr; } }
                g_v6561_candidate_snapshot[0]->AddRef(); g_v6561_candidate_snapshot[1]->AddRef();
                g_v6588_live_pair[0]=g_v6561_candidate_snapshot[0]; g_v6588_live_pair[1]=g_v6561_candidate_snapshot[1];
                g_v6588_source_pair_serial.store(g_v6561_pair_serial,std::memory_order_release); g_v6588_live_serial=execute_serial;
                char m[384]{}; std::snprintf(m,sizeof(m),
                    "V65.88-LIVE-PAIR READY frameSerial=%llu sourcePair=%llu list=%p mapping=A->L,B->R source=COUNT2 same-scene submitted",
                    (unsigned long long)g_v6588_live_serial,(unsigned long long)g_v6588_source_pair_serial.load(std::memory_order_acquire),g_v6561_candidate_list);
                diagnostic_trace::Write(m);
            }
        }
    }

    // V65.87: publish the frozen MRT0/MRT1 pair only after the exact command
    // list that recorded both copies has been submitted to the real DIRECT queue.
    if(command_lists && g_v6587_enabled.load(std::memory_order_acquire)){
        std::lock_guard<std::mutex> lk(g_v6587_mutex);
        if(g_v6587_recorded && !g_v6587_submitted && g_v6587_list){
            for(UINT i=0;i<num_command_lists;++i){
                if(command_lists[i]==static_cast<ID3D12CommandList*>(g_v6587_list)){
                    g_v6587_submitted=true; g_v6587_pair_serial=execute_serial;
                    char m[320]{}; std::snprintf(m,sizeof(m),
                        "V65.87-PAIR READY serial=%llu list=%p mapping=MRT0->L,MRT1->R sources=COMMON frozen same-frame COUNT2",
                        (unsigned long long)g_v6587_pair_serial,g_v6587_list); diagnostic_trace::Write(m);
                    break;
                }
            }
        }
    }
    {
        std::lock_guard<std::mutex> lock(s_bridge_capture_mutex);
        if(s_bridge_current_main_list && command_lists){
            for(UINT i=0;i<num_command_lists;++i){
                if(command_lists[i]==static_cast<ID3D12CommandList*>(s_bridge_current_main_list)){
                    s_bridge_current_main_last_submit.store(execute_serial,std::memory_order_release);
                    break;
                }
            }
        }
        if(command_lists) {
            for(UINT i=0;i<num_command_lists;++i) {
                auto* submitted=command_lists[i];
                for(auto& tag:g_bridge_list_tags_v6547) {
                    if(static_cast<ID3D12CommandList*>(tag.list)!=submitted || tag.serial==0 || (tag.eye!=0 && tag.eye!=1)) continue;
                    if(tag.eye==0) s_bridge_left_submitted_serial.store(tag.serial,std::memory_order_release);
                    else s_bridge_right_submitted_serial.store(tag.serial,std::memory_order_release);
                    g_bridge_cert_cv_v6550.notify_all();
                    const auto oc=g_bridge_owner_complete_v6547.fetch_add(1,std::memory_order_relaxed)+1;
                    { char om[320]{}; std::snprintf(om,sizeof(om),
                        "V65.47-OWNER COMPLETE owner=%llu serial=%llu eye=%c list=%p execute=%llu pending=%zu",
                        static_cast<unsigned long long>(oc),static_cast<unsigned long long>(tag.serial),tag.eye==0?'L':'R',submitted,
                        static_cast<unsigned long long>(execute_serial),g_bridge_tickets_v6547.size()); debug_trace::Write(om); }
                    char cm[320]{}; std::snprintf(cm,sizeof(cm),
                        "V65.47-OWNER-SUBMIT execute=%llu serial=%llu eye=%c list=%p queue=%p",
                        static_cast<unsigned long long>(execute_serial),static_cast<unsigned long long>(tag.serial),tag.eye==0?'L':'R',submitted,queue_this);
                    debug_trace::Write(cm);
                    auto& pst=BridgePairStateForV6547(tag.serial);
                    pst.submitted_mask |= (tag.eye==0?1u:2u);
                    if(!pst.complete && pst.recorded_mask==3u && pst.submitted_mask==3u) {
                        pst.complete=true;
                        V6634PublishWriteSlotLocked(s_bridge_left_capture,s_bridge_right_capture,tag.serial,pst.owner);
                        s_bridge_complete_pair_serial.store(tag.serial,std::memory_order_release);
                // H49 source pair is queue-submitted here; GPU completion/scanout is not claimed.
                {namespace oc=kcd2vr::diagnostics::ownership48;const auto now=std::uint64_t(GetTickCount64());
                 if(oc::active(now)){oc::Event e{};e.tick=now;e.stage=14;e.thread=GetCurrentThreadId();
                  e.item=tag.serial;e.publication=0;e.object=execute_serial;
                  oc::name(e,"PAIR_QUEUE_PUBLISHED_NOT_GPU_COMPLETE");oc::push(e);}}

                        const auto pc=s_bridge_complete_pair_count.fetch_add(1,std::memory_order_relaxed)+1;
                        char pm[320]{}; std::snprintf(pm,sizeof(pm),
                            "V65.47-OWNER-PAIR COMPLETE pair=%llu serial=%llu queue=%p execute=%llu masks=3/3",
                            static_cast<unsigned long long>(pc),static_cast<unsigned long long>(tag.serial),queue_this,static_cast<unsigned long long>(execute_serial));
                        debug_trace::Write(pm);
                    }
                    tag={};
                }
            }
        }
    }

    // Native0 keeps its established first-main-writer boundary. Temporal RIGHT
    // was handled above by the immutable final-main copy submission. The old
    // optional-blit observer remains diagnostic only. PairReady is lock-free: it cannot wait
    // on an SDK call which itself submits through this queue detour.
    const auto d82_pair_frame=g_v6613_pair_frame.load(std::memory_order_acquire);
    std::uintptr_t d82_current_output=0;
    if(d82_temporal&&d82_composition.Valid()){
        std::lock_guard<std::mutex> bl(s_bridge_capture_mutex);
        d82_current_output=reinterpret_cast<std::uintptr_t>(s_bridge_current_main_target);
    }
    const bool d82_sdk_ready=d82_temporal&&s_original_execute_command_lists&&d82_composition.Valid()&&
        native_upscaler_stereo_d82::PairReady(d82_composition.epoch,d82_composition.frame);
    if(d82_temporal&&d82_composition.Valid()&&d82_composition.frame==d82_pair_frame){
        if(d82_composition.output!=d82_current_output)g_d85_watchdog.Refuse(temporal_pair_d85::Check::OutputMismatch);
        else if(!d82_sdk_ready)g_d85_watchdog.Refuse(temporal_pair_d85::Check::SdkReceipt);
    }
    // Starvation status (F2 detail row). R4: a timeout alone never replaces the
    // recorded final-main copy + queue submission of the temporal transaction.
    const std::uint64_t d85_now=d82_temporal?static_cast<std::uint64_t>(GetTickCount64()):0;
    const bool d85_latched_now=d82_temporal&&g_d85_watchdog.Tick(d85_now);
    // A successful SDK call records work; it does not prove the first RIGHT
    // main-writer batch contains the finished image. Only the final-main copy
    // transaction may publish temporal output. A timeout reports starvation.
    const bool d85_fallback=false,d85_fallback_sdk=false;
    const bool d85_capture=diagnostics::capture168::active(diagnostics::capture168::Domain::Bug,d85_now);
    if(d82_temporal&&(d85_latched_now||d85_capture)){
        char d85_line[192]{};
        if(g_d85_watchdog.Report(d85_now,d85_latched_now,"DLSS/FSR",d85_line,sizeof(d85_line))){
            char detail[384]{};std::snprintf(detail,sizeof(detail),
                "D85 TEMPORAL_PAIR %s attempts=%llu temporalPairs=%llu fallbackPairs=%llu pairFrame=%u",
                d85_line,(unsigned long long)g_d85_watchdog.attempts.load(std::memory_order_relaxed),
                (unsigned long long)g_d85_watchdog.temporalPairs.load(std::memory_order_relaxed),
                (unsigned long long)g_d85_watchdog.fallbackPairs.load(std::memory_order_relaxed),d82_pair_frame);
            // One bounded file-only lifecycle receipt even when F2 is off.
            if(d85_latched_now)diagnostic_trace::WriteStatus(detail);
            else diagnostic_trace::Write(detail);
        }
    }
    bool d85_via_fallback=false;
    if(!d82_temporal && g_v6615_active.load(std::memory_order_acquire) &&
       g_v6615_left_copy_queued.load(std::memory_order_acquire) &&
       temporal_pair_d85::ClaimRightCopy(g_v6615_right_copy_queued,d82_temporal,
           v6615_right_main_incoming,d82_composition,d82_pair_frame,d82_current_output,d82_sdk_ready,
           d85_fallback,d85_fallback_sdk,d85_via_fallback)) {
        const bool right_ok=BridgeCaptureCurrentMainTargetToRight(0);
        if(!right_ok&&d82_temporal)g_d85_watchdog.Refuse(temporal_pair_d85::Check::RightCopy);
        const bool temporal_ok=(d82_temporal&&!d85_via_fallback)
            ?(g_v6613_pair_frame.load(std::memory_order_acquire)==d82_composition.frame&&
              native_upscaler_stereo_d82::PairReady(d82_composition.epoch,d82_composition.frame))
            :native_upscaler_stereo_d82::FrameReady(d82_pair_frame);
        bool published=false; std::uint64_t pair_serial=0;
        if(right_ok && temporal_ok) {
            std::lock_guard<std::mutex> lock(s_bridge_capture_mutex);
            if(s_bridge_left_capture && s_bridge_right_capture) {
                pair_serial=(static_cast<std::uint64_t>(g_v6613_pair_frame.load(std::memory_order_relaxed))<<32) ^ execute_serial;
                if(pair_serial==0) pair_serial=execute_serial?execute_serial:1;
                s_bridge_left_submitted_serial.store(pair_serial,std::memory_order_release);
                s_bridge_right_submitted_serial.store(pair_serial,std::memory_order_release);
                // V66.27 F2 stereo ownership: tag the certified pair with the exact
                // renderer-matched CameraHook/XR frame that produced the primary view.
                // This mirrors the proven V65.04 PRESENT ownership rule instead of
                // letting native mode2 fall through with a stale/current XR pose token.
                const std::uint64_t latest_global_pose=CameraHook::LastPoseFrameIndex();
                const std::uint64_t renderer_match_candidate=FrameSync::LatestRendererMatchedPoseFrame(50);
                const bool renderer_match_fresh=renderer_match_candidate!=0 &&
                    latest_global_pose>=renderer_match_candidate; // V66.46: no "<=2 behind" rule
                {
                    static std::atomic<std::uint64_t> v6646_pairs{0}, v6646_matched{0}, v6646_lag_sum{0}, v6646_lag_max{0};
                    const auto pn=v6646_pairs.fetch_add(1,std::memory_order_relaxed)+1;
                    if(renderer_match_fresh) {
                        v6646_matched.fetch_add(1,std::memory_order_relaxed);
                        const auto lag=latest_global_pose-renderer_match_candidate;
                        v6646_lag_sum.fetch_add(lag,std::memory_order_relaxed);
                        auto mx=v6646_lag_max.load(std::memory_order_relaxed);
                        while(lag>mx && !v6646_lag_max.compare_exchange_weak(mx,lag)) {}
                    }
                    if(pn<=4 || (pn%300ull)==0) {
                        const auto mt=v6646_matched.exchange(0), ls=v6646_lag_sum.exchange(0), lm=v6646_lag_max.exchange(0);
                        const auto window=(pn<=4)?1ull:300ull;
                        char q[320]{}; std::snprintf(q,sizeof(q),
                            "V66.46-POSE-TOKEN pairs=%llu window=%llu rendererMatched=%llu fallbackNewest=%llu avgLagPoses=%.2f maxLagPoses=%llu",
                            (unsigned long long)pn,(unsigned long long)window,(unsigned long long)mt,(unsigned long long)(window>mt?window-mt:0ull),
                            mt?double(ls)/double(mt):0.0,(unsigned long long)lm);
                        diagnostic_trace::Write(q);
                    }
                }
                const std::uint64_t current_pose=renderer_match_fresh?renderer_match_candidate:latest_global_pose;
                g_v6638_right_pose_frame.store(current_pose,std::memory_order_release);
                const std::uint64_t left_pose=g_v6638_left_pose_frame.load(std::memory_order_acquire);
                const std::uint64_t right_pose=current_pose;
                const int pose_mode=g_v6638_pair_pose_mode.load(std::memory_order_acquire);
                std::uint64_t pair_camera_frame=current_pose;
                if(pose_mode==1 && left_pose) pair_camera_frame=left_pose;
                else if(pose_mode==2 && right_pose) pair_camera_frame=right_pose;
                else if(pose_mode==3) pair_camera_frame=left_pose?left_pose:(right_pose?right_pose:current_pose); // shared fallback; XR uses per-eye tokens below
                else if(pose_mode==4 && left_pose && right_pose) {
                    const std::uint64_t lo=left_pose<right_pose?left_pose:right_pose, hi=left_pose<right_pose?right_pose:left_pose;
                    pair_camera_frame=lo+((hi-lo)/2ull);
                }
                g_v6638_shared_pose_frame.store(pair_camera_frame,std::memory_order_release);
                V6634PublishWriteSlotLocked(s_bridge_left_capture,s_bridge_right_capture,pair_serial,{});
                s_bridge_complete_pair_camera_frame.store(pair_camera_frame,std::memory_order_release);
                s_bridge_complete_pair_serial.store(pair_serial,std::memory_order_release);
                // H49 source pair is queue-submitted here; GPU completion/scanout is not claimed.
                {namespace oc=kcd2vr::diagnostics::ownership48;const auto now=std::uint64_t(GetTickCount64());
                 if(oc::active(now)){oc::Event e{};e.tick=now;e.stage=14;e.thread=GetCurrentThreadId();
                  e.item=pair_serial;e.publication=pair_camera_frame;e.object=execute_serial;
                  oc::name(e,"PAIR_QUEUE_PUBLISHED_NOT_GPU_COMPLETE");oc::push(e);}}

                s_bridge_complete_pair_count.fetch_add(1,std::memory_order_acq_rel);
                native_upscaler_stereo_d82::NotifyPublished(native_upscaler_stereo_d82::CurrentEpoch(),d82_pair_frame);
                g_v6615_pair_published.store(true,std::memory_order_release);
                g_v6618_live_stream_started.store(true,std::memory_order_release);
                published=true;
                if(d82_temporal)g_d85_watchdog.Published(d85_via_fallback,static_cast<std::uint64_t>(GetTickCount64()));
            }
        }
        static std::atomic<unsigned long long> v6618pub{0}; const auto pubn=v6618pub.fetch_add(1,std::memory_order_relaxed)+1;
        if(pubn<=8 || (pubn%120ull)==0 || !published){ char m[512]{}; std::snprintf(m,sizeof(m),
            "V66.27-LIVE-PUBLISH pair=%llu frame=%u execute=%llu leftCopy=%d rightCopy=%d pairPublished=%d pairSerial=%llu cameraPoseFrame=%llu XR=NORMAL-LR-LIVE fallback=%s",
            (unsigned long long)pubn,g_v6613_pair_frame.load(std::memory_order_relaxed),(unsigned long long)execute_serial,
            g_v6615_left_copy_queued.load(std::memory_order_relaxed)?1:0,right_ok?1:0,published?1:0,
            (unsigned long long)pair_serial,
            (unsigned long long)s_bridge_complete_pair_camera_frame.load(std::memory_order_relaxed),
            published?"NO":"LAST-CERTIFIED-PAIR"); diagnostic_trace::Write(m);
            if(published && (pubn<=12 || (pubn%120ull)==0)){ char pm[320]{}; std::snprintf(pm,sizeof(pm),
                "V66.38-PAIR-POSE pair=%llu mode=%d leftPose=%llu rightPose=%llu sharedPose=%llu delta=%lld",
                (unsigned long long)pubn,g_v6638_pair_pose_mode.load(std::memory_order_relaxed),
                (unsigned long long)g_v6638_left_pose_frame.load(),(unsigned long long)g_v6638_right_pose_frame.load(),
                (unsigned long long)g_v6638_shared_pose_frame.load(),
                (long long)g_v6638_right_pose_frame.load()-(long long)g_v6638_left_pose_frame.load()); diagnostic_trace::Write(pm); } }
        if(!published) g_v6615_active.store(false,std::memory_order_release);
    }

    bool submitted_present = false;
    {
        std::lock_guard<std::mutex> lock(s_present_resource_mutex);
        ID3D12GraphicsCommandList* present_list = s_latest_present_command_list;
        if (present_list && command_lists) {
            for (UINT i = 0; i < num_command_lists; ++i) {
                if (command_lists[i] == static_cast<ID3D12CommandList*>(present_list)) {
                    submitted_present = true;
                    break;
                }
            }
        }
    }
    s_current_execute_submitted_present = submitted_present;

    if(v6612_present_boundary_submitted && g_v6612_drain_requested.load(std::memory_order_acquire)){
        if(!g_v6614_right_submitted_seen.load(std::memory_order_acquire)){
            diagnostic_trace::Write("V66.18-BOOTSTRAP-SUBMIT-DRAIN HOLD reason=PRESENT-before-RIGHT-pair-frame-submission action=continue");
        } else if(!g_v6612_final_dump_done.exchange(true,std::memory_order_acq_rel)){
            diagnostic_trace::Write("V66.18-BOOTSTRAP-SUBMIT-DRAIN COMPLETE reason=RIGHT-submitted-then-main-present-boundary action=finalize-command-order");
            DumpV6610OutputCensus();
        }
    }

    // V65.93: the only place the new bridge may mutate D3D12 state. Rung 1
    // merely certifies topology. In capture mode, a fresh safe pair is copied
    // from a separate command list appended after the game's submitted-PRESENT
    // batch on this exact DIRECT queue, then restored to KCD2's final states.
    if(submitted_present && g_v6593_mode.load(std::memory_order_acquire)!=0){
        std::lock_guard<std::mutex> lk(g_v6593_mutex);
        const int mode=g_v6593_mode.load(std::memory_order_relaxed);
        if(g_v6593_pair_serial || (mode==1 && g_v6593_census_certified)) {
            // Frozen/certified state: nothing to re-evaluate on later Presents.
        } else {
        const bool haveA=g_v6593_candidate[0].source!=nullptr;
        const bool haveB=g_v6593_candidate[1].source!=nullptr;
        const bool sameFrame=haveA&&haveB&&g_v6593_candidate[0].frame_serial==g_v6593_candidate[1].frame_serial&&g_v6593_candidate[0].frame_serial==g_v6593_work_frame;
        const bool sameCrv=haveA&&haveB&&g_v6593_candidate[0].crv==g_v6593_candidate[1].crv&&g_v6593_candidate[0].crv==g_v6593_work_crv;
        const bool distinct=haveA&&haveB&&g_v6593_candidate[0].source!=g_v6593_candidate[1].source;
        const bool nview=haveA&&V6593FrameSawExactNViewLocked(g_v6593_work_frame);
        const bool safeA=haveA&&V6593CandidateSafeLocked(g_v6593_candidate[0]);
        const bool safeB=haveB&&V6593CandidateSafeLocked(g_v6593_candidate[1]);
        const bool certified=sameFrame&&sameCrv&&distinct&&nview&&safeA&&safeB;
        char m[1024]{}; std::snprintf(m,sizeof(m),
            "V65.93-CENSUS PRESENT mode=%s execute=%llu frame=%llu crv=%p have={A:%d B:%d} sameFrame=%d sameCrv=%d distinct=%d exactNview=%d safe={A:%d B:%d} A={src:%p state:0x%X split:%d mismatch:%d alias:%d rtExit:%d rewrite:%d lists:%zu trans:%u} B={src:%p state:0x%X split:%d mismatch:%d alias:%d rtExit:%d rewrite:%d lists:%zu trans:%u}",
            mode==1?"READONLY":"CAPTURE",(unsigned long long)execute_serial,(unsigned long long)g_v6593_work_frame,reinterpret_cast<void*>(g_v6593_work_crv),
            haveA?1:0,haveB?1:0,sameFrame?1:0,sameCrv?1:0,distinct?1:0,nview?1:0,safeA?1:0,safeB?1:0,
            g_v6593_candidate[0].source,(unsigned)g_v6593_candidate[0].state,g_v6593_candidate[0].split_open?1:0,g_v6593_candidate[0].state_mismatch?1:0,g_v6593_candidate[0].alias_seen?1:0,g_v6593_candidate[0].rt_exit_seen?1:0,g_v6593_candidate[0].write_after_exit?1:0,g_v6593_candidate[0].list_count,g_v6593_candidate[0].transition_count,
            g_v6593_candidate[1].source,(unsigned)g_v6593_candidate[1].state,g_v6593_candidate[1].split_open?1:0,g_v6593_candidate[1].state_mismatch?1:0,g_v6593_candidate[1].alias_seen?1:0,g_v6593_candidate[1].rt_exit_seen?1:0,g_v6593_candidate[1].write_after_exit?1:0,g_v6593_candidate[1].list_count,g_v6593_candidate[1].transition_count);
        diagnostic_trace::Write(m);
        if(mode==1 && certified){
            g_v6593_census_certified=true;
            diagnostic_trace::Write("V65.93-CENSUS CERTIFIED same-frame/same-CRV distinct FP16 candidates; exact COUNT2 packet seen; states closed; all touched lists submitted; RUNG2 MAY ARM FRESH POST-SUBMIT COPY");
        } else if(mode==2 && certified && !g_v6593_pair_serial){
            if(!V6593QueuePostSubmitCopyLocked(queue_this,execute_serial))
                diagnostic_trace::Write("V65.93-BRIDGE COPY-REFUSED reason=post-submit-copy-setup-or-submit-failed");
        }
        if(!g_v6593_pair_serial && !(mode==1 && g_v6593_census_certified)){
            // Try a fresh frame rather than pinning an incomplete/unsafe pair.
            V6593ReleaseCandidate(g_v6593_candidate[0]); V6593ReleaseCandidate(g_v6593_candidate[1]);
            g_v6593_work_frame=0; g_v6593_work_crv=0;
        }
        }
    }

    RenderTickCallback callback = s_render_tick_callback.load(std::memory_order_acquire);
    if (!callback) {
        s_current_execute_submitted_present = false;
        return;
    }

    s_active_render_callbacks.fetch_add(1, std::memory_order_acq_rel);
    callback = s_render_tick_callback.load(std::memory_order_acquire);
    if (callback)
        callback(queue_this);
    s_active_render_callbacks.fetch_sub(1, std::memory_order_acq_rel);
    s_current_execute_submitted_present = false;
}

HRESULT __stdcall DeviceHook::DetourCreateCommandQueue(
    ID3D12Device* device_this, const D3D12_COMMAND_QUEUE_DESC* desc, REFIID riid, void** out_queue)
{
    HRESULT hr = s_original_create_queue
        ? s_original_create_queue(device_this, desc, riid, out_queue)
        : E_FAIL;

    if (SUCCEEDED(hr) && out_queue && *out_queue && !s_captured.load(std::memory_order_acquire) && device_this)
    {
        auto* queue = static_cast<ID3D12CommandQueue*>(*out_queue);
        const D3D12_COMMAND_QUEUE_DESC actual_desc = queue->GetDesc();
        if (actual_desc.Type == D3D12_COMMAND_LIST_TYPE_DIRECT)
        {
            device_this->AddRef();
            queue->AddRef();
            s_captured_device.store(device_this, std::memory_order_relaxed);
            s_captured_queue.store(queue, std::memory_order_relaxed);
            s_captured.store(true, std::memory_order_release);
            if (!InstallExecuteHook(queue))
                KCD2VR_DIAG_LOG(Bug,warning,"DeviceHook: DIRECT queue captured, but render-thread ExecuteCommandLists hook could not be installed");
            KCD2VR_DIAG_LOG(Bug,info,"DeviceHook: captured game's DIRECT D3D12 queue (priority={}, flags={})",
                       actual_desc.Priority, static_cast<unsigned>(actual_desc.Flags));
        }
        else
        {
            KCD2VR_DIAG_LOG(Bug,debug,"DeviceHook: ignored non-DIRECT D3D12 queue type={}",
                        static_cast<int>(actual_desc.Type));
        }
    }

    return hr;
}

namespace stereo_design_capture {
void PrepareCertifiedPair(std::uint64_t serial) noexcept { DeviceHook::BridgePrepareCertifiedPair(serial); }
void PrepareStreamingPair(std::uint64_t serial) noexcept { DeviceHook::BridgePrepareStreamingPair(serial); }
bool RecordCurrentMainTarget(std::uint64_t serial,int eye) noexcept { return DeviceHook::RecordStereoDesignCurrentMainTarget(serial,eye); }
void AbortSerial(std::uint64_t serial) noexcept { DeviceHook::BridgeAbortSerial(serial); }
void SetOutputCensus(bool enabled) noexcept { DeviceHook::SetV6610OutputCensus(enabled); }
void SetOutputCensusEye(int eye,std::uint32_t frame) noexcept { DeviceHook::SetV6610OutputCensusPhase(eye,frame); }
void RecordSemanticOutput(int eye,std::uint32_t frame,std::uintptr_t render_output,std::uintptr_t texture_object,
                          std::uintptr_t device_backing,std::uintptr_t native_rtv_view,std::uint64_t native_rtv_key,
                          std::uint16_t width,std::uint16_t height,std::uint8_t source_format,std::uint8_t device_format,
                          std::uint32_t flags,const char* name) noexcept {
    DeviceHook::RecordV6610SemanticOutput(eye,frame,render_output,texture_object,device_backing,native_rtv_view,native_rtv_key,
                                          width,height,source_format,device_format,flags,name);
}
void BeginCompositionTrace(int eye,std::uint32_t frame,
                           std::uintptr_t input_texture,std::uintptr_t input_backing,std::uintptr_t input_rtv_view,
                           std::uintptr_t output_texture,std::uintptr_t output_backing,std::uintptr_t output_rtv_view) noexcept {
    DeviceHook::BeginV6611CompositionTrace(eye,frame,input_texture,input_backing,input_rtv_view,output_texture,output_backing,output_rtv_view);
}
void EndCompositionTrace() noexcept { DeviceHook::EndV6611CompositionTrace(); }
bool BeginTemporalCompositionD82(std::uint64_t epoch,std::uint32_t frame) noexcept {
    return DeviceHook::BeginTemporalCompositionD82(epoch,frame);
}
void EndTemporalCompositionD82() noexcept {DeviceHook::EndTemporalCompositionD82();}
void DumpOutputCensus() noexcept { DeviceHook::DumpV6610OutputCensus(); }
} // namespace stereo_design_capture


} // namespace kcd2vr

// R16b: presented image size for the windowed vr laser (ui_menu_input.cpp)
bool KCD2VR_PresentedImageSizeR16(std::uint64_t* width,std::uint32_t* height) noexcept {
    std::uint64_t serial=0,tick=0,w=0;std::uint32_t h=0;
    if(!kcd2vr::DeviceHook::GetLatestPresentIdentity(&serial,&tick,&w,&h))return false;
    if(width)*width=w;if(height)*height=h;return true;
}
