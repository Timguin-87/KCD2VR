// device_hook.h
//
// Captures the GAME's real ID3D12Device/ID3D12CommandQueue - never our own.
// A later revision briefly reintroduced creating a THROWAWAY dummy D3D12
// device+queue just to read a vtable pointer, which is exactly the "second
// device on the same adapter" pattern Nvidia's own Aftermath tooling
// identified as the cause of three confirmed DeviceRemovedNoGpuFault
// crashes earlier in this project. Reverted to the proven-safe two-stage
// chain, through the game's OWN real objects only:
//
//   1. Hook the plain EXPORTED D3D12CreateDevice function (not a COM vtable
//      at all - overlays don't touch it) to capture the game's real device
//      the moment it's created.
//   2. Use THAT real device to install a hook on ID3D12Device::
//      CreateCommandQueue (a vtable slot overlays essentially never hook)
//      to capture the game's real command queue.
//
// No device or queue we create ourselves, at any stage, ever.
//
// Stage 3 is now implemented: hook the captured DIRECT queue's
// ExecuteCommandLists entry point and use it as a render-thread rendezvous.
// OpenXR frame timing stays on the XR worker thread; custom D3D12 queue
// submission is handed off to the thread actually submitting KCD2 graphics.


#pragma once

#include <d3d12.h>
#include "native_upscaler_stereo_d82.h"
#include <atomic>
#include <optional>
#include <mutex>
#include <unordered_map>
#include <cstdint>
#include <DetourModKit/hook.hpp>

namespace kcd2vr {
namespace temporal_capture_d85 { struct Ticket; }

class DeviceHook
{
public:
    bool Install();
    void Uninstall();

    static bool TryGetDevice(ID3D12Device** out_device, ID3D12CommandQueue** out_queue);

    // Called from the game thread whenever the captured DIRECT queue executes
    // command lists. The callback must be short and must not throw.
    using RenderTickCallback = void(*)(ID3D12CommandQueue* queue);
    static void SetRenderTickCallback(RenderTickCallback callback);
    static void WaitForRenderCallbacksIdle();

    // Submit through the original ExecuteCommandLists entry point so VR work
    // issued from the render-thread callback does not recursively hit our hook.
    static bool ExecuteCommandListsBypass(
        ID3D12CommandQueue* queue, UINT count, ID3D12CommandList* const* lists);

    // V37 active bridge: returns the most recent texture the game transitioned
    // to PRESENT. Caller receives an AddRef and must Release().
    static bool TryGetLatestPresentResource(ID3D12Resource** out_resource);
    static bool TryGetLatestPresentResourceTagged(ID3D12Resource** out_resource, int* out_eye, std::uint64_t* out_serial, std::uint64_t* out_camera_frame = nullptr);
    static bool TryGetLatestPresentResourceIdentity(ID3D12Resource** out_resource, std::uint64_t* out_serial, std::uint64_t* out_tick_ms, std::uint64_t* out_camera_frame);
    // V46.9 frame-identity audit: tick/serial of the exact PRESENT resource currently exposed to XR.
    static bool GetLatestPresentIdentity(std::uint64_t* out_serial, std::uint64_t* out_tick_ms, std::uint64_t* out_width, std::uint32_t* out_height, std::uint64_t* out_camera_frame = nullptr);
    static void SetAfrStereoCapture(bool enabled);
    static void SetInternalVrCommandList(ID3D12GraphicsCommandList* list);
    static void SetInternalMenuCommandList(ID3D12GraphicsCommandList* list);
    static void SetInternalHudCommandList(ID3D12GraphicsCommandList* list);
    static std::uint64_t PresentCaptureCount();
    static bool CurrentExecuteSubmittedPresent();
    // R111: format changes of the "latest present resource" (count, current, previous) for the always-on UI row.
    static void PresentFormatChangesR111(std::uint64_t* changes,std::uint32_t* last,std::uint32_t* previous);
    static std::uint64_t RecursiveBarrierCount();
    static std::uint64_t RecursiveRenderTargetBarrierCount();
    static void SetCustomResolutionProbe(bool enabled, std::uint32_t width, std::uint32_t height);
    static std::uint64_t CustomResolutionBarrierCount();
    static std::uint64_t CustomResolutionRenderTargetBarrierCount();
    // V59.6: read-only scene-color archaeology. Logs exact-size normal-pipeline
    // textures that enter/leave RENDER_TARGET, tagged with native re-entry eye.
    static void SetSceneColorProbe(bool enabled, std::uint32_t width, std::uint32_t height);
    static std::uint64_t SceneColorProbeBarrierCount();
    static std::uint64_t SceneColorProbeLeftCount();
    static std::uint64_t SceneColorProbeRightCount();
    // V61.28 F8: passive native-stereo producer test. Looks for multiple distinct
    // same-size full-resolution render targets bound within the same present epoch.
    // This never re-enters the renderer and never writes a camera or GPU resource.
    static void SetStereoProducerProbe(bool enabled, std::uint32_t width, std::uint32_t height);
    static std::uint64_t StereoProducerSameFramePairCount();
    // V65.70 F9: read-only COUNT2 output-layout census. Does not alter F8 or render state.
    // V65.58: arm the broad producer observer through exactly the next real game
    // PRESENT transition.  The D3D12 hook snapshots and auto-disarms there, so
    // asynchronous command recording after CStandardGraphicsPipeline::Execute returns
    // is still captured without leaving a continuous tracer running.
    static void ArmStereoProducerProbeThroughNextPresent(int oracle_stage = 0);
    // V65.61: owned COMMON-state snapshots of the two consecutive full-resolution fmt=28
    // candidates observed on one command list after the tagged stereo packet.
    // V65.88: promote each completed/submitted V65.61 COUNT2 candidate pair into a
    // persistent latest-frame pair for the F10 full-stereo ladder. The published pair
    // survives the next-frame route-probe reset and is replaced only after a newer
    // same-command-list pair has actually been submitted to the game queue.
    static bool TryGetV6588LiveStereoPair(ID3D12Resource** out_left, ID3D12Resource** out_right, std::uint64_t* out_serial);
    // V65.85: COUNT2 output-surface sweep. Four full-size RTV candidates are
    // snapshotted at their exact RT->next-state barrier into owned COMMON textures.
    static bool TryGetV6585Candidate(int index, ID3D12Resource** out_resource, std::uint64_t* out_serial);
    static int V6585CandidateCount();
    // V66.12 F9: read-only eye-phase output-owner census. The native stereo
    // controller labels the current left/right pass; OMSetRenderTargets and
    // ResourceBarrier then report the actual color resources touched by that pass.
    // No GPU resource, descriptor, barrier, or command list is modified.
    static void SetV6610OutputCensus(bool enabled);
    static void SetV6610OutputCensusPhase(int eye, std::uint32_t frame);
    static void RecordV6610SemanticOutput(int eye, std::uint32_t frame, std::uintptr_t render_output,
                                          std::uintptr_t texture_object, std::uintptr_t device_backing,
                                          std::uintptr_t native_rtv_view, std::uint64_t native_rtv_key,
                                          std::uint16_t width, std::uint16_t height,
                                          std::uint8_t source_format, std::uint8_t device_format,
                                          std::uint32_t flags, const char* name);
    // V66.12: synchronous scope around the exact KCD2 final-composition helper.
    // OMSetRenderTargets/ResourceBarrier events observed in this scope are tagged
    // to the current L/R pass; no D3D12 state is changed by the probe.
    // D82 temporal output proof is independent of the one-shot diagnostic trace.
    static bool BeginTemporalCompositionD82(std::uint64_t epoch,std::uint32_t frame) noexcept;
    static void EndTemporalCompositionD82() noexcept;
    static void BeginV6611CompositionTrace(int eye, std::uint32_t frame,
                                            std::uintptr_t input_texture, std::uintptr_t input_backing, std::uintptr_t input_rtv_view,
                                            std::uintptr_t output_texture, std::uintptr_t output_backing, std::uintptr_t output_rtv_view);
    static void EndV6611CompositionTrace();
    static void DumpV6610OutputCensus();
    // V65.87: same-frame COUNT2 MRT0/MRT1 pair capture. The pair is latched from
    // one OMSetRenderTargets call after COUNT2 has already crossed at least one
    // PRESENT, snapshotted on that same command list, and exposed only after the
    // list is submitted. Sources are owned COMMON-state textures.
    static bool TryGetV6587SameFramePair(ID3D12Resource** out_left, ID3D12Resource** out_right, std::uint64_t* out_serial);
    // V65.93 F5: Crysis-style completion bridge adapted to native COUNT2.
    // mode 1 is strictly read-only lifetime census; mode 2 arms a fresh pair
    // and copies only after the game's submitted-PRESENT batch on the same
    // DIRECT queue. No copy/barrier is injected into KCD2's own command list.
    static void NotifyV6593Count2Packet(std::uint32_t count);
    static void NotifyV6593SecondaryApply(std::uint32_t native_count, std::uintptr_t crv);
    static bool TryGetV6593SceneColorPair(ID3D12Resource** out_a, ID3D12Resource** out_b, std::uint64_t* out_serial);

    // V65.47 persistent transaction bridge. CSystem::Render enqueues durable
    // eye tickets which survive scope exit and thread hops. The real game
    // RT->PRESENT barrier consumes the oldest ticket; ExecuteCommandLists then
    // certifies submission. No temporary CPU-thread ownership window is used.
    static void BridgeBeginEyeRecord(std::uint64_t serial, int eye_sign);
    // V65.50 tangible stereo: clear stale research ownership and wait for the
    // exact in-list eye capture to be submitted by the real game DIRECT queue.
    // This is a bounded condition-variable wait, never a sleep/poll loop.
    static void BridgePrepareCertifiedPair(std::uint64_t serial);
    static void BridgePrepareStreamingPair(std::uint64_t serial);
    // V66.12 gated stereo-design path: record a copy of the live KCD2 main
    // RENDER_TARGET directly into the same still-open game command list at the
    // boundary between native eye passes. Publication still waits for the real
    // DIRECT queue submission; no CPU/GPU wait is inserted.
    static bool RecordStereoDesignCurrentMainTarget(std::uint64_t serial, int eye);
    static bool BridgeWaitEyeSubmitted(std::uint64_t serial, int eye_sign, std::uint32_t timeout_ms);
    static void BridgeAbortSerial(std::uint64_t serial);
    static bool TryGetBridgeCompletePair(ID3D12Resource** out_left, ID3D12Resource** out_right, std::uint64_t* out_serial, std::uint64_t* out_camera_frame, native_upscaler_stereo_d82::PairOwner* out_owner = nullptr);
    // Read-only readiness hint. No resource AddRef, claim, consumption or GPU work.
    // False = snapshot lock busy; true + serial zero = no certified pair available.
    static bool ReadBridgePairIdentity59c6(std::uint64_t& serial, std::uint64_t& camera_frame);
    static void MarkBridgePairConsumed(std::uint64_t serial);
    // V66.34: XR calls this after it has submitted (or abandoned) its read of the pair
    // returned by TryGetBridgeCompletePair; the producer may then reuse that slot.
    static void ReleaseBridgePairClaim();
    static std::uint64_t V6634LastConsumedPairSerial();
    static std::uint64_t BridgeCompletePairCount();
    // V66.36 diagnostic: optionally allow at most one produced stereo pair per
    // OpenXR frame opportunity. XR thread increments the opportunity after xrWaitFrame.
    static void NotifyV6636XrFrameOpportunity();
    static bool V6636CadenceOwner();
    static std::uint64_t V6636XrFrameOpportunity();
    // V66.38 F4 pose-ownership lab for one completed same-frame L/R pair.
    // 0=current publication token, 1=LEFT token for both eyes, 2=RIGHT token for both,
    // 3=per-eye LEFT/RIGHT tokens, 4=mid-pair token for both (falls back safely).
    static void SetV6638PairPoseMode(int mode);
    static int V6638PairPoseMode();
    static void GetV6638LastPairPoseFrames(std::uint64_t& left, std::uint64_t& right, std::uint64_t& shared);
    // V66.27 F2 stability experiment: suppress expensive read-only GPU archaeology
    // while testing pair-owned pose + nonblocking stereo presentation.
    static void SetV6627PerformanceQuiet(bool enabled);
    static bool V6627PerformanceQuiet();

    // Historical post-return capture API retained only for source compatibility.
    // V65.47 does not call it from the active bridge.
    static bool BridgeCaptureCurrentMainTargetToLeft(std::uint64_t submit_count_before_left, ID3D12Resource* expected_source=nullptr, ID3D12Resource** out_left=nullptr, const D3D12_RESOURCE_STATES* state_override=nullptr, std::uint64_t submitted_source=0);
    static bool BridgeCaptureCurrentMainTargetToRight(std::uint64_t submit_count_before_right);
    static void ResetBridgeCapture();
    static bool RecordTemporalFinalMainD85(ID3D12GraphicsCommandList*,ID3D12Resource*,D3D12_RESOURCE_STATES) noexcept;
    static void FinishTemporalMainSubmissionD85(ID3D12CommandQueue*,const temporal_capture_d85::Ticket&,std::uint64_t) noexcept;

    // V61.34 F4: generic read-only D3D12 depth-candidate observer. It maps
    // DSV descriptors to real resources, tracks whichever DSV is bound while
    // DIRECT command lists record draws, and ranks candidates by draw usage
    // plus source-resolution/aspect match. No resource state/content is changed.
    static void SetGenericDepthProbe(bool enabled);
    static bool GenericDepthProbeEnabled();

    // V65.81 F5: read-only MODE0/MODE2 -> PSO -> command-list -> full-frame
    // render-target provenance. Hooks are installed on the game's own D3D12
    // objects and remain dormant unless this toggle is enabled.
    static bool ModePsoProvenanceProbeEnabled();
    // V65.82: retroactively join a newly discovered MODE0/MODE2 DXBC hash
    // against graphics PSOs cached since device startup, and refresh any
    // command-list state that already has a matching PSO bound. Read-only.
    static void NotifyModeShaderHashLearned(int lane, std::uint64_t hash);

private:
    using D3D12CreateDeviceFunc = HRESULT(WINAPI*)(
        IUnknown* adapter, D3D_FEATURE_LEVEL minimum_feature_level, REFIID riid, void** out_device);

    using CreateCommandQueueFunc = HRESULT(__stdcall*)(
        ID3D12Device* device_this, const D3D12_COMMAND_QUEUE_DESC* desc, REFIID riid, void** out_queue);

    using ExecuteCommandListsFunc = void(__stdcall*)(
        ID3D12CommandQueue* queue_this, UINT num_command_lists, ID3D12CommandList* const* command_lists);

    using CreateGraphicsPipelineStateFunc = HRESULT(__stdcall*)(
        ID3D12Device* device_this, const D3D12_GRAPHICS_PIPELINE_STATE_DESC* desc, REFIID riid, void** out_pso);

    using CreateCommandListFunc = HRESULT(__stdcall*)(
        ID3D12Device* device_this, UINT node_mask, D3D12_COMMAND_LIST_TYPE type,
        ID3D12CommandAllocator* allocator, ID3D12PipelineState* initial_state, REFIID riid, void** out_list);

    using SetPipelineStateFunc = void(__stdcall*)(ID3D12GraphicsCommandList* list_this, ID3D12PipelineState* pso);

    using ResourceBarrierFunc = void(__stdcall*)(
        ID3D12GraphicsCommandList* list_this, UINT num_barriers, const D3D12_RESOURCE_BARRIER* barriers);

    using CreateDepthStencilViewFunc = void(__stdcall*)(
        ID3D12Device* device_this, ID3D12Resource* resource,
        const D3D12_DEPTH_STENCIL_VIEW_DESC* desc, D3D12_CPU_DESCRIPTOR_HANDLE dest);

    using ClearCommandListStateFunc = void(__stdcall*)(ID3D12GraphicsCommandList*,ID3D12PipelineState*);
    using ResetCommandListFunc = HRESULT(__stdcall*)(
        ID3D12GraphicsCommandList*,ID3D12CommandAllocator*,ID3D12PipelineState*);
    using DrawInstancedFunc = void(__stdcall*)(
        ID3D12GraphicsCommandList* list_this, UINT vertex_count_per_instance, UINT instance_count,
        UINT start_vertex_location, UINT start_instance_location);

    using DrawIndexedInstancedFunc = void(__stdcall*)(
        ID3D12GraphicsCommandList* list_this, UINT index_count_per_instance, UINT instance_count,
        UINT start_index_location, INT base_vertex_location, UINT start_instance_location);

    using CreateRenderTargetViewFunc = void(__stdcall*)(
        ID3D12Device* device_this, ID3D12Resource* resource,
        const D3D12_RENDER_TARGET_VIEW_DESC* desc, D3D12_CPU_DESCRIPTOR_HANDLE dest);

    using CreateDescriptorHeapFunc = HRESULT(__stdcall*)(
        ID3D12Device* device_this, const D3D12_DESCRIPTOR_HEAP_DESC* desc, REFIID riid, void** out_heap);

    using CreateShaderResourceViewFunc = void(__stdcall*)(
        ID3D12Device* device_this, ID3D12Resource* resource,
        const D3D12_SHADER_RESOURCE_VIEW_DESC* desc, D3D12_CPU_DESCRIPTOR_HANDLE dest);

    using CopyDescriptorsFunc = void(__stdcall*)(
        ID3D12Device* device_this, UINT num_dest_ranges,
        const D3D12_CPU_DESCRIPTOR_HANDLE* dest_range_starts, const UINT* dest_range_sizes,
        UINT num_src_ranges, const D3D12_CPU_DESCRIPTOR_HANDLE* src_range_starts, const UINT* src_range_sizes,
        D3D12_DESCRIPTOR_HEAP_TYPE heap_type);

    using CopyDescriptorsSimpleFunc = void(__stdcall*)(
        ID3D12Device* device_this, UINT num_descriptors, D3D12_CPU_DESCRIPTOR_HANDLE dest_start,
        D3D12_CPU_DESCRIPTOR_HANDLE src_start, D3D12_DESCRIPTOR_HEAP_TYPE heap_type);

    using SetGraphicsRootDescriptorTableFunc = void(__stdcall*)(
        ID3D12GraphicsCommandList* list_this, UINT root_parameter_index, D3D12_GPU_DESCRIPTOR_HANDLE base_descriptor);

    using RSSetViewportsFunc = void(__stdcall*)(ID3D12GraphicsCommandList*, UINT, const D3D12_VIEWPORT*);
    using RSSetScissorRectsFunc = void(__stdcall*)(ID3D12GraphicsCommandList*, UINT, const D3D12_RECT*);

    using OMSetRenderTargetsFunc = void(__stdcall*)(
        ID3D12GraphicsCommandList* list_this, UINT num_render_target_descriptors,
        const D3D12_CPU_DESCRIPTOR_HANDLE* render_target_descriptors,
        BOOL rts_single_handle_to_descriptor_range,
        const D3D12_CPU_DESCRIPTOR_HANDLE* depth_stencil_descriptor);

    static HRESULT WINAPI DetourD3D12CreateDevice(
        IUnknown* adapter, D3D_FEATURE_LEVEL minimum_feature_level, REFIID riid, void** out_device);

    static HRESULT __stdcall DetourCreateCommandQueue(
        ID3D12Device* device_this, const D3D12_COMMAND_QUEUE_DESC* desc, REFIID riid, void** out_queue);

    static void __stdcall DetourExecuteCommandLists(
        ID3D12CommandQueue* queue_this, UINT num_command_lists, ID3D12CommandList* const* command_lists);

    static HRESULT __stdcall DetourCreateGraphicsPipelineState(
        ID3D12Device* device_this, const D3D12_GRAPHICS_PIPELINE_STATE_DESC* desc, REFIID riid, void** out_pso);

    static HRESULT __stdcall DetourCreateCommandList(
        ID3D12Device* device_this, UINT node_mask, D3D12_COMMAND_LIST_TYPE type,
        ID3D12CommandAllocator* allocator, ID3D12PipelineState* initial_state, REFIID riid, void** out_list);

    static void __stdcall DetourSetPipelineState(ID3D12GraphicsCommandList* list_this, ID3D12PipelineState* pso);

    static void __stdcall DetourResourceBarrier(
        ID3D12GraphicsCommandList* list_this, UINT num_barriers, const D3D12_RESOURCE_BARRIER* barriers);

    static void __stdcall DetourCreateDepthStencilView(
        ID3D12Device* device_this, ID3D12Resource* resource,
        const D3D12_DEPTH_STENCIL_VIEW_DESC* desc, D3D12_CPU_DESCRIPTOR_HANDLE dest);

    static void TraceModePsoDraw(ID3D12GraphicsCommandList* list_this, bool indexed) noexcept;
    static void TraceV6624PostFxOrLateDraw(ID3D12GraphicsCommandList* list_this, bool indexed, UINT primitive_count, UINT instance_count) noexcept;

    static void __stdcall DetourClearCommandListStateD82(ID3D12GraphicsCommandList*,ID3D12PipelineState*);
    static HRESULT __stdcall DetourResetCommandListD82(
        ID3D12GraphicsCommandList*,ID3D12CommandAllocator*,ID3D12PipelineState*);
    static void __stdcall DetourDrawInstanced(
        ID3D12GraphicsCommandList* list_this, UINT vertex_count_per_instance, UINT instance_count,
        UINT start_vertex_location, UINT start_instance_location);

    static void __stdcall DetourDrawIndexedInstanced(
        ID3D12GraphicsCommandList* list_this, UINT index_count_per_instance, UINT instance_count,
        UINT start_index_location, INT base_vertex_location, UINT start_instance_location);

    static HRESULT __stdcall DetourCreateDescriptorHeap(
        ID3D12Device* device_this, const D3D12_DESCRIPTOR_HEAP_DESC* desc, REFIID riid, void** out_heap);

    static void __stdcall DetourCreateShaderResourceView(
        ID3D12Device* device_this, ID3D12Resource* resource,
        const D3D12_SHADER_RESOURCE_VIEW_DESC* desc, D3D12_CPU_DESCRIPTOR_HANDLE dest);

    static void __stdcall DetourCopyDescriptors(
        ID3D12Device* device_this, UINT num_dest_ranges,
        const D3D12_CPU_DESCRIPTOR_HANDLE* dest_range_starts, const UINT* dest_range_sizes,
        UINT num_src_ranges, const D3D12_CPU_DESCRIPTOR_HANDLE* src_range_starts, const UINT* src_range_sizes,
        D3D12_DESCRIPTOR_HEAP_TYPE heap_type);

    static void __stdcall DetourCopyDescriptorsSimple(
        ID3D12Device* device_this, UINT num_descriptors, D3D12_CPU_DESCRIPTOR_HANDLE dest_start,
        D3D12_CPU_DESCRIPTOR_HANDLE src_start, D3D12_DESCRIPTOR_HEAP_TYPE heap_type);

    static void __stdcall DetourSetGraphicsRootDescriptorTable(
        ID3D12GraphicsCommandList* list_this, UINT root_parameter_index, D3D12_GPU_DESCRIPTOR_HANDLE base_descriptor);

    static void __stdcall DetourCreateRenderTargetView(
        ID3D12Device* device_this, ID3D12Resource* resource,
        const D3D12_RENDER_TARGET_VIEW_DESC* desc, D3D12_CPU_DESCRIPTOR_HANDLE dest);

    static void __stdcall DetourRSSetViewports(ID3D12GraphicsCommandList*, UINT, const D3D12_VIEWPORT*);
    static void __stdcall DetourRSSetScissorRects(ID3D12GraphicsCommandList*, UINT, const D3D12_RECT*);

    static void __stdcall DetourOMSetRenderTargets(
        ID3D12GraphicsCommandList* list_this, UINT num_render_target_descriptors,
        const D3D12_CPU_DESCRIPTOR_HANDLE* render_target_descriptors,
        BOOL rts_single_handle_to_descriptor_range,
        const D3D12_CPU_DESCRIPTOR_HANDLE* depth_stencil_descriptor);

    static bool InstallQueueHook(ID3D12Device* device);
    static bool InstallCreateGraphicsPipelineStateHook(ID3D12Device* device);
    static bool InstallCreateCommandListHook(ID3D12Device* device);
    static bool InstallExecuteHook(ID3D12CommandQueue* queue);
    static bool InstallSetPipelineStateHook(ID3D12GraphicsCommandList* list);
    static bool InstallResourceBarrierHook(ID3D12GraphicsCommandList* list);
    static bool InstallCreateRenderTargetViewHook(ID3D12Device* device);
    static bool InstallCreateDepthStencilViewHook(ID3D12Device* device);
    static bool InstallCreateDescriptorHeapHook(ID3D12Device* device);
    static bool InstallCreateShaderResourceViewHook(ID3D12Device* device);
    static bool InstallCopyDescriptorHooks(ID3D12Device* device);
    static bool InstallGraphicsRootDescriptorTableHook(ID3D12GraphicsCommandList* list);
    static bool InstallDrawHooks(ID3D12GraphicsCommandList* list);
    static bool InstallCommandListResetHookD82(ID3D12GraphicsCommandList* list);
    static bool InstallOMSetRenderTargetsHook(ID3D12GraphicsCommandList* list);
    static bool InstallOutputLayoutHooks(ID3D12GraphicsCommandList* list);

    static D3D12CreateDeviceFunc s_original_create_device;
    static CreateCommandQueueFunc s_original_create_queue;
    static ExecuteCommandListsFunc s_original_execute_command_lists;
    static CreateGraphicsPipelineStateFunc s_original_create_graphics_pipeline_state;
    static CreateCommandListFunc s_original_create_command_list;
    static SetPipelineStateFunc s_original_set_pipeline_state;
    static ResourceBarrierFunc s_original_resource_barrier;
    static CreateRenderTargetViewFunc s_original_create_rtv;
    static CreateDepthStencilViewFunc s_original_create_dsv;
    static CreateDescriptorHeapFunc s_original_create_descriptor_heap;
    static CreateShaderResourceViewFunc s_original_create_srv;
    static CopyDescriptorsFunc s_original_copy_descriptors;
    static CopyDescriptorsSimpleFunc s_original_copy_descriptors_simple;
    static SetGraphicsRootDescriptorTableFunc s_original_set_graphics_root_descriptor_table;
    static std::atomic<ClearCommandListStateFunc> s_original_clear_command_list_state_d82;
    static std::atomic<ResetCommandListFunc> s_original_reset_command_list_d82;
    static DrawInstancedFunc s_original_draw_instanced;
    static DrawIndexedInstancedFunc s_original_draw_indexed_instanced;
    static OMSetRenderTargetsFunc s_original_om_set_render_targets;
    static RSSetViewportsFunc s_original_rs_set_viewports;
    static RSSetScissorRectsFunc s_original_rs_set_scissor_rects;
    static std::atomic<ID3D12Device*> s_captured_device;
    static std::atomic<ID3D12CommandQueue*> s_captured_queue;
    static std::atomic<bool> s_captured;
    static std::atomic<bool> s_queue_hook_installed;
    static std::atomic<bool> s_execute_hook_installed;
    static std::atomic<bool> s_create_graphics_pso_hook_installed;
    static std::atomic<bool> s_command_list_hook_installed;
    static std::atomic<bool> s_set_pipeline_state_hook_installed;
    static std::atomic<bool> s_resource_barrier_hook_installed;
    static std::atomic<bool> s_create_rtv_hook_installed;
    static std::atomic<bool> s_create_dsv_hook_installed;
    static std::atomic<bool> s_create_descriptor_heap_hook_installed;
    static std::atomic<bool> s_create_srv_hook_installed;
    static std::atomic<bool> s_copy_descriptors_hook_installed;
    static std::atomic<bool> s_graphics_root_descriptor_table_hook_installed;
    static std::atomic<bool> s_draw_hooks_installed;
    static std::atomic<bool> s_om_set_rt_hook_installed;
    static std::atomic<bool> s_output_layout_hooks_installed;
    static std::atomic<bool> s_output_layout_probe_enabled;
    static std::atomic<std::uint64_t> s_output_layout_event_count;
    static ID3D12Resource* s_latest_present_resource;
    static int s_latest_present_eye;
    static std::uint64_t s_latest_present_serial;
    static std::uint64_t s_latest_present_tick_ms;
    static std::uint64_t s_latest_present_camera_frame;
    static ID3D12GraphicsCommandList* s_latest_present_command_list;
    static thread_local bool s_current_execute_submitted_present;
    static std::atomic<bool> s_afr_stereo_capture;
    static std::mutex s_present_resource_mutex;
    static std::atomic<ID3D12GraphicsCommandList*> s_internal_vr_command_list;
    static std::atomic<ID3D12GraphicsCommandList*> s_internal_menu_command_list;
    static std::atomic<ID3D12GraphicsCommandList*> s_internal_hud_command_list;
    static std::atomic<RenderTickCallback> s_render_tick_callback;
    static std::atomic<unsigned> s_active_render_callbacks;
    static std::atomic<std::uint64_t> s_present_capture_count;
    static std::atomic<std::uint64_t> s_recursive_barrier_count;
    static std::atomic<std::uint64_t> s_recursive_rt_barrier_count;
    static std::atomic<bool> s_customres_probe_enabled;
    static std::atomic<std::uint32_t> s_customres_probe_width;
    static std::atomic<std::uint32_t> s_customres_probe_height;
    static std::atomic<std::uint64_t> s_customres_barrier_count;
    static std::atomic<std::uint64_t> s_customres_rt_barrier_count;
    static std::atomic<bool> s_scene_color_probe_enabled;
    static std::atomic<std::uint32_t> s_scene_color_probe_width;
    static std::atomic<std::uint32_t> s_scene_color_probe_height;
    static std::atomic<std::uint64_t> s_scene_color_probe_barrier_count;
    static std::atomic<std::uint64_t> s_scene_color_probe_left_count;
    static std::atomic<std::uint64_t> s_scene_color_probe_right_count;
    static std::atomic<bool> s_stereo_producer_probe_enabled;
    static std::atomic<std::uint32_t> s_stereo_producer_probe_width;
    static std::atomic<std::uint32_t> s_stereo_producer_probe_height;
    static std::atomic<std::uint64_t> s_stereo_producer_bind_count;
    static std::atomic<std::uint64_t> s_stereo_producer_unique_count;
    static std::atomic<std::uint64_t> s_stereo_producer_pair_count;
    static std::atomic<std::uint64_t> s_stereo_producer_epoch;
    static std::atomic<ID3D12Resource*> s_stereo_producer_first_resource;
    static std::atomic<std::uint64_t> s_stereo_producer_pair_epoch;
    static std::atomic<ID3D12Resource*> s_stereo_producer_unique0;
    static std::atomic<ID3D12Resource*> s_stereo_producer_unique1;
    static std::atomic<ID3D12Resource*> s_stereo_producer_unique2;
    static std::atomic<ID3D12Resource*> s_stereo_producer_unique3;
    static std::atomic<std::uint64_t> s_stereo_producer_detailed_bind_count;
    static std::atomic<std::uint64_t> s_stereo_producer_auto_stop_present_serial;
    static std::atomic<int> s_stereo_producer_oracle_stage;

    // V61.6: passive native-stereo RT-binding archaeology.  Descriptor handles
    // are resolved back to actual ID3D12Resource objects at OMSetRenderTargets.
    // No renderer state is modified.
    struct GenericDepthCandidate {
        std::atomic<ID3D12Resource*> resource{nullptr};
        std::atomic<std::uint64_t> binds{0};
        std::atomic<std::uint64_t> draws{0};
        std::atomic<std::uint64_t> indexed_draws{0};
        std::uint64_t width{0};
        std::uint32_t height{0};
        DXGI_FORMAT format{DXGI_FORMAT_UNKNOWN};
        D3D12_RESOURCE_FLAGS flags{D3D12_RESOURCE_FLAG_NONE};
        std::atomic<bool> screen_match{false};
        std::atomic<bool> aspect_match{false};
    };
    static constexpr std::size_t kGenericDepthCandidateCount = 64;
    static std::atomic<bool> s_generic_depth_probe_enabled;
    static GenericDepthCandidate s_generic_depth_candidates[kGenericDepthCandidateCount];
    static thread_local int s_tls_depth_candidate;
    static std::mutex s_dsv_map_mutex;
public:
    // V61.35: live version of the same rank used by the F4-off summary, so a
    // consumer (the compositor) can pick up the current best-guess depth
    // resource continuously instead of waiting for F4 to be turned off.
    // Requires a minimum draw count before considering a candidate stable
    // enough to use, so we don't grab something seen only once or twice.
    // Returns false (leaves outputs untouched) if nothing qualifies yet.
    static bool GetLiveBestDepthCandidate(ID3D12Resource** out_resource, DXGI_FORMAT* out_format,
        std::uint64_t* out_width, std::uint32_t* out_height) noexcept;
private:
    static std::unordered_map<SIZE_T, ID3D12Resource*> s_dsv_resource_map;
    static std::optional<DetourModKit::hook::Hook> s_hook_create_dsv;
    static std::optional<DetourModKit::hook::Hook> s_hook_clear_command_list_state_d82;
    static std::optional<DetourModKit::hook::Hook> s_hook_reset_command_list_d82;
    static std::optional<DetourModKit::hook::Hook> s_hook_draw_instanced;
    static std::optional<DetourModKit::hook::Hook> s_hook_draw_indexed_instanced;
    static std::optional<DetourModKit::hook::Hook> s_hook_rs_set_viewports;
    static std::optional<DetourModKit::hook::Hook> s_hook_rs_set_scissor_rects;

    static std::mutex s_rtv_map_mutex;
    static std::unordered_map<SIZE_T, ID3D12Resource*> s_rtv_resource_map;
    static std::unordered_map<SIZE_T, D3D12_RENDER_TARGET_VIEW_DESC> s_rtv_desc_map;
    static std::atomic<UINT> s_rtv_descriptor_increment;
    static std::atomic<std::uint64_t> s_native_rt_bind_count;

    static std::mutex s_bridge_capture_mutex;
    static ID3D12Resource* s_bridge_current_main_target;
    static ID3D12GraphicsCommandList* s_bridge_current_main_list;
    static D3D12_RESOURCE_STATES s_bridge_current_main_state;
    static DWORD s_bridge_current_main_tid;
    static ID3D12Resource* s_bridge_left_capture;
    static D3D12_RESOURCE_DESC s_bridge_left_desc;
    static ID3D12Resource* s_bridge_right_capture;
    static D3D12_RESOURCE_DESC s_bridge_right_desc;
    static std::atomic<std::uint64_t> s_bridge_copy_count;
    static std::atomic<std::uint64_t> s_execute_submission_count;
    static std::atomic<std::uint64_t> s_bridge_current_main_last_submit;
    static std::atomic<std::uint64_t> s_bridge_record_serial;
    static std::atomic<int> s_bridge_record_eye;
    static std::atomic<std::uint64_t> s_bridge_left_recorded_serial;
    static std::atomic<std::uint64_t> s_bridge_right_recorded_serial;
    static std::atomic<std::uint64_t> s_bridge_left_submitted_serial;
    static std::atomic<std::uint64_t> s_bridge_right_submitted_serial;
    static std::atomic<std::uint64_t> s_bridge_complete_pair_serial;
    static std::atomic<std::uint64_t> s_bridge_complete_pair_camera_frame;
    static std::atomic<std::uint64_t> s_bridge_complete_pair_count;
    static std::atomic<std::uint64_t> s_bridge_last_consumed_pair_serial;
    static std::atomic<bool> s_v6627_performance_quiet;
    static ID3D12CommandAllocator* s_bridge_copy_allocator;
    static ID3D12GraphicsCommandList* s_bridge_copy_list;
    static ID3D12Fence* s_bridge_copy_fence;
    static HANDLE s_bridge_copy_fence_event;
    static std::uint64_t s_bridge_copy_fence_value;
    struct ModePsoListState {
        ID3D12PipelineState* pso{};
        std::uint64_t shader_hash{};
        int lane{};
        int shader_stage{}; // 1=VS 2=PS 3=GS 4=HS 5=DS
        ID3D12Resource* rt[4]{};
        D3D12_CPU_DESCRIPTOR_HANDLE rtv[4]{};
        UINT rt_count{};
        D3D12_GPU_DESCRIPTOR_HANDLE graphics_table[32]{};
        std::uint32_t graphics_table_mask{};
    };
    static std::atomic<bool> s_mode_pso_probe_enabled;
    static std::atomic<std::uint64_t> s_mode_pso_event_count;
    static std::atomic<std::uint64_t> s_mode0_draw_count;
    static std::atomic<std::uint64_t> s_mode2_draw_count;
    static std::mutex s_mode_pso_mutex;
    struct PsoShaderRecord { std::uint64_t h[5]{}; };
    static std::unordered_map<ID3D12PipelineState*, PsoShaderRecord> s_pso_shader_hashes;
    static std::unordered_map<ID3D12GraphicsCommandList*, ModePsoListState> s_mode_list_state;
    static std::unordered_map<ID3D12Resource*, std::uint64_t> s_mode0_rt_draws;
    static std::unordered_map<ID3D12Resource*, std::uint64_t> s_mode2_rt_draws;

    struct DescriptorHeapRange {
        D3D12_CPU_DESCRIPTOR_HANDLE cpu{};
        D3D12_GPU_DESCRIPTOR_HANDLE gpu{};
        UINT count{};
        UINT increment{};
        bool shader_visible{};
    };
    static std::mutex s_descriptor_trace_mutex;
    static std::unordered_map<ID3D12DescriptorHeap*, DescriptorHeapRange> s_cbv_srv_uav_heaps;
    static std::unordered_map<SIZE_T, ID3D12Resource*> s_srv_resource_map;
    static std::unordered_map<ID3D12PipelineState*, std::uint64_t> s_v6625_last_ui_table_hash;
    static std::atomic<std::uint64_t> s_v6625_ui_descriptor_dumps;
    static void TraceV6625LateUiDescriptors(ID3D12GraphicsCommandList* list_this, const ModePsoListState& st) noexcept;

    static std::optional<DetourModKit::hook::Hook> s_hook_create_graphics_pipeline_state;
    static std::optional<DetourModKit::hook::Hook> s_hook_set_pipeline_state;
    static std::optional<DetourModKit::hook::Hook> s_hook_create_queue;
    static std::optional<DetourModKit::hook::Hook> s_hook_execute_command_lists;
    static std::optional<DetourModKit::hook::Hook> s_hook_create_command_list;
    static std::optional<DetourModKit::hook::Hook> s_hook_resource_barrier;
    static std::optional<DetourModKit::hook::Hook> s_hook_create_rtv;
    static std::optional<DetourModKit::hook::Hook> s_hook_create_descriptor_heap;
    static std::optional<DetourModKit::hook::Hook> s_hook_create_srv;
    static std::optional<DetourModKit::hook::Hook> s_hook_copy_descriptors;
    static std::optional<DetourModKit::hook::Hook> s_hook_copy_descriptors_simple;
    static std::optional<DetourModKit::hook::Hook> s_hook_graphics_root_descriptor_table;
    static std::optional<DetourModKit::hook::Hook> s_hook_om_set_render_targets;

    std::optional<DetourModKit::hook::Hook> hook_create_device_;
};

} // namespace kcd2vr
