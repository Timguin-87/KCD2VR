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
#include <atomic>
#include <optional>
#include <mutex>
#include <unordered_map>
#include <cstdint>
#include <DetourModKit/hook.hpp>

namespace kcd2vr {

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
    static bool TryGetLatestPresentResourceTagged(ID3D12Resource** out_resource, int* out_eye, std::uint64_t* out_serial);
    static bool TryGetLatestPresentResourceIdentity(ID3D12Resource** out_resource, std::uint64_t* out_serial, std::uint64_t* out_tick_ms, std::uint64_t* out_camera_frame);
    // V46.9 frame-identity audit: tick/serial of the exact PRESENT resource currently exposed to XR.
    static bool GetLatestPresentIdentity(std::uint64_t* out_serial, std::uint64_t* out_tick_ms, std::uint64_t* out_width, std::uint32_t* out_height, std::uint64_t* out_camera_frame = nullptr);
    static void SetAfrStereoCapture(bool enabled);
    static void SetInternalVrCommandList(ID3D12GraphicsCommandList* list);
    static void SetInternalMenuCommandList(ID3D12GraphicsCommandList* list);
    static std::uint64_t PresentCaptureCount();
    static bool CurrentExecuteSubmittedPresent();
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
    static std::uint64_t StereoProducerBindCount();
    static std::uint64_t StereoProducerUniqueCount();
    static std::uint64_t StereoProducerSameFramePairCount();

    // V61.34 F4: generic read-only D3D12 depth-candidate observer. It maps
    // DSV descriptors to real resources, tracks whichever DSV is bound while
    // DIRECT command lists record draws, and ranks candidates by draw usage
    // plus source-resolution/aspect match. No resource state/content is changed.
    static void SetGenericDepthProbe(bool enabled);
    static bool GenericDepthProbeEnabled();

private:
    using D3D12CreateDeviceFunc = HRESULT(WINAPI*)(
        IUnknown* adapter, D3D_FEATURE_LEVEL minimum_feature_level, REFIID riid, void** out_device);

    using CreateCommandQueueFunc = HRESULT(__stdcall*)(
        ID3D12Device* device_this, const D3D12_COMMAND_QUEUE_DESC* desc, REFIID riid, void** out_queue);

    using ExecuteCommandListsFunc = void(__stdcall*)(
        ID3D12CommandQueue* queue_this, UINT num_command_lists, ID3D12CommandList* const* command_lists);

    using CreateCommandListFunc = HRESULT(__stdcall*)(
        ID3D12Device* device_this, UINT node_mask, D3D12_COMMAND_LIST_TYPE type,
        ID3D12CommandAllocator* allocator, ID3D12PipelineState* initial_state, REFIID riid, void** out_list);

    using ResourceBarrierFunc = void(__stdcall*)(
        ID3D12GraphicsCommandList* list_this, UINT num_barriers, const D3D12_RESOURCE_BARRIER* barriers);

    using CreateDepthStencilViewFunc = void(__stdcall*)(
        ID3D12Device* device_this, ID3D12Resource* resource,
        const D3D12_DEPTH_STENCIL_VIEW_DESC* desc, D3D12_CPU_DESCRIPTOR_HANDLE dest);

    using DrawInstancedFunc = void(__stdcall*)(
        ID3D12GraphicsCommandList* list_this, UINT vertex_count_per_instance, UINT instance_count,
        UINT start_vertex_location, UINT start_instance_location);

    using DrawIndexedInstancedFunc = void(__stdcall*)(
        ID3D12GraphicsCommandList* list_this, UINT index_count_per_instance, UINT instance_count,
        UINT start_index_location, INT base_vertex_location, UINT start_instance_location);

    using CreateRenderTargetViewFunc = void(__stdcall*)(
        ID3D12Device* device_this, ID3D12Resource* resource,
        const D3D12_RENDER_TARGET_VIEW_DESC* desc, D3D12_CPU_DESCRIPTOR_HANDLE dest);

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

    static HRESULT __stdcall DetourCreateCommandList(
        ID3D12Device* device_this, UINT node_mask, D3D12_COMMAND_LIST_TYPE type,
        ID3D12CommandAllocator* allocator, ID3D12PipelineState* initial_state, REFIID riid, void** out_list);

    static void __stdcall DetourResourceBarrier(
        ID3D12GraphicsCommandList* list_this, UINT num_barriers, const D3D12_RESOURCE_BARRIER* barriers);

    static void __stdcall DetourCreateDepthStencilView(
        ID3D12Device* device_this, ID3D12Resource* resource,
        const D3D12_DEPTH_STENCIL_VIEW_DESC* desc, D3D12_CPU_DESCRIPTOR_HANDLE dest);

    static void __stdcall DetourDrawInstanced(
        ID3D12GraphicsCommandList* list_this, UINT vertex_count_per_instance, UINT instance_count,
        UINT start_vertex_location, UINT start_instance_location);

    static void __stdcall DetourDrawIndexedInstanced(
        ID3D12GraphicsCommandList* list_this, UINT index_count_per_instance, UINT instance_count,
        UINT start_index_location, INT base_vertex_location, UINT start_instance_location);

    static void __stdcall DetourCreateRenderTargetView(
        ID3D12Device* device_this, ID3D12Resource* resource,
        const D3D12_RENDER_TARGET_VIEW_DESC* desc, D3D12_CPU_DESCRIPTOR_HANDLE dest);

    static void __stdcall DetourOMSetRenderTargets(
        ID3D12GraphicsCommandList* list_this, UINT num_render_target_descriptors,
        const D3D12_CPU_DESCRIPTOR_HANDLE* render_target_descriptors,
        BOOL rts_single_handle_to_descriptor_range,
        const D3D12_CPU_DESCRIPTOR_HANDLE* depth_stencil_descriptor);

    static bool InstallQueueHook(ID3D12Device* device);
    static bool InstallCreateCommandListHook(ID3D12Device* device);
    static bool InstallExecuteHook(ID3D12CommandQueue* queue);
    static bool InstallResourceBarrierHook(ID3D12GraphicsCommandList* list);
    static bool InstallCreateRenderTargetViewHook(ID3D12Device* device);
    static bool InstallCreateDepthStencilViewHook(ID3D12Device* device);
    static bool InstallDrawHooks(ID3D12GraphicsCommandList* list);
    static bool InstallOMSetRenderTargetsHook(ID3D12GraphicsCommandList* list);

    static D3D12CreateDeviceFunc s_original_create_device;
    static CreateCommandQueueFunc s_original_create_queue;
    static ExecuteCommandListsFunc s_original_execute_command_lists;
    static CreateCommandListFunc s_original_create_command_list;
    static ResourceBarrierFunc s_original_resource_barrier;
    static CreateRenderTargetViewFunc s_original_create_rtv;
    static CreateDepthStencilViewFunc s_original_create_dsv;
    static DrawInstancedFunc s_original_draw_instanced;
    static DrawIndexedInstancedFunc s_original_draw_indexed_instanced;
    static OMSetRenderTargetsFunc s_original_om_set_render_targets;
    static std::atomic<ID3D12Device*> s_captured_device;
    static std::atomic<ID3D12CommandQueue*> s_captured_queue;
    static std::atomic<bool> s_captured;
    static std::atomic<bool> s_queue_hook_installed;
    static std::atomic<bool> s_execute_hook_installed;
    static std::atomic<bool> s_command_list_hook_installed;
    static std::atomic<bool> s_resource_barrier_hook_installed;
    static std::atomic<bool> s_create_rtv_hook_installed;
    static std::atomic<bool> s_create_dsv_hook_installed;
    static std::atomic<bool> s_draw_hooks_installed;
    static std::atomic<bool> s_om_set_rt_hook_installed;
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
    static std::optional<DetourModKit::hook::Hook> s_hook_draw_instanced;
    static std::optional<DetourModKit::hook::Hook> s_hook_draw_indexed_instanced;

    static std::mutex s_rtv_map_mutex;
    static std::unordered_map<SIZE_T, ID3D12Resource*> s_rtv_resource_map;
    static std::atomic<UINT> s_rtv_descriptor_increment;
    static std::atomic<std::uint64_t> s_native_rt_bind_count;
    static std::optional<DetourModKit::hook::Hook> s_hook_create_queue;
    static std::optional<DetourModKit::hook::Hook> s_hook_execute_command_lists;
    static std::optional<DetourModKit::hook::Hook> s_hook_create_command_list;
    static std::optional<DetourModKit::hook::Hook> s_hook_resource_barrier;
    static std::optional<DetourModKit::hook::Hook> s_hook_create_rtv;
    static std::optional<DetourModKit::hook::Hook> s_hook_om_set_render_targets;

    std::optional<DetourModKit::hook::Hook> hook_create_device_;
};

} // namespace kcd2vr
