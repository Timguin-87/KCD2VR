// device_hook.cpp

#include "device_hook.h"
#include "camera_hook.h"
#include "frame_sync.h"
#include "debug_trace.h"

#include <DetourModKit/logger.hpp>
#include <windows.h>
#include <cstdio>
#include <algorithm>
#include <array>
#include <cmath>

namespace kcd2vr {

using DetourModKit::log;
namespace hook = DetourModKit::hook;

DeviceHook::D3D12CreateDeviceFunc DeviceHook::s_original_create_device = nullptr;
DeviceHook::CreateCommandQueueFunc DeviceHook::s_original_create_queue = nullptr;
DeviceHook::ExecuteCommandListsFunc DeviceHook::s_original_execute_command_lists = nullptr;
DeviceHook::CreateCommandListFunc DeviceHook::s_original_create_command_list = nullptr;
DeviceHook::ResourceBarrierFunc DeviceHook::s_original_resource_barrier = nullptr;
DeviceHook::CreateRenderTargetViewFunc DeviceHook::s_original_create_rtv = nullptr;
DeviceHook::CreateDepthStencilViewFunc DeviceHook::s_original_create_dsv = nullptr;
DeviceHook::DrawInstancedFunc DeviceHook::s_original_draw_instanced = nullptr;
DeviceHook::DrawIndexedInstancedFunc DeviceHook::s_original_draw_indexed_instanced = nullptr;
DeviceHook::OMSetRenderTargetsFunc DeviceHook::s_original_om_set_render_targets = nullptr;
std::atomic<ID3D12Device*> DeviceHook::s_captured_device{nullptr};
std::atomic<ID3D12CommandQueue*> DeviceHook::s_captured_queue{nullptr};
std::atomic<bool> DeviceHook::s_captured{false};
std::atomic<bool> DeviceHook::s_queue_hook_installed{false};
std::atomic<bool> DeviceHook::s_execute_hook_installed{false};
std::atomic<bool> DeviceHook::s_command_list_hook_installed{false};
std::atomic<bool> DeviceHook::s_resource_barrier_hook_installed{false};
std::atomic<bool> DeviceHook::s_create_rtv_hook_installed{false};
std::atomic<bool> DeviceHook::s_create_dsv_hook_installed{false};
std::atomic<bool> DeviceHook::s_draw_hooks_installed{false};
std::atomic<bool> DeviceHook::s_om_set_rt_hook_installed{false};
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
std::atomic<bool> DeviceHook::s_generic_depth_probe_enabled{false};
DeviceHook::GenericDepthCandidate DeviceHook::s_generic_depth_candidates[DeviceHook::kGenericDepthCandidateCount]{};
thread_local int DeviceHook::s_tls_depth_candidate = -1;
std::mutex DeviceHook::s_dsv_map_mutex;
std::unordered_map<SIZE_T, ID3D12Resource*> DeviceHook::s_dsv_resource_map;

std::mutex DeviceHook::s_rtv_map_mutex;
std::unordered_map<SIZE_T, ID3D12Resource*> DeviceHook::s_rtv_resource_map;
std::atomic<UINT> DeviceHook::s_rtv_descriptor_increment{0};
std::atomic<std::uint64_t> DeviceHook::s_native_rt_bind_count{0};
std::optional<hook::Hook> DeviceHook::s_hook_create_queue;
std::optional<hook::Hook> DeviceHook::s_hook_execute_command_lists;
std::optional<hook::Hook> DeviceHook::s_hook_create_command_list;
std::optional<hook::Hook> DeviceHook::s_hook_resource_barrier;
std::optional<hook::Hook> DeviceHook::s_hook_create_rtv;
std::optional<hook::Hook> DeviceHook::s_hook_create_dsv;
std::optional<hook::Hook> DeviceHook::s_hook_draw_instanced;
std::optional<hook::Hook> DeviceHook::s_hook_draw_indexed_instanced;
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
        log().error("DeviceHook: d3d12.dll not loaded in process after polling");
        return false;
    }
    void* target_address = reinterpret_cast<void*>(GetProcAddress(d3d12_module, "D3D12CreateDevice"));
    if (!target_address)
    {
        log().error("DeviceHook: GetProcAddress(D3D12CreateDevice) failed");
        return false;
    }

    log().info("DeviceHook: D3D12CreateDevice address = {:#x}", reinterpret_cast<uintptr_t>(target_address));

    hook::InlineRequest request{
        "kcd2vr_device_hook_create_device",
        hook::Target{DetourModKit::Address{reinterpret_cast<uintptr_t>(target_address)}},
        hook::Options{}
    };

    DetourModKit::Result<hook::Hook> result = hook::inline_at(std::move(request), &DeviceHook::DetourD3D12CreateDevice);
    if (!result)
    {
        log().error("DeviceHook: D3D12CreateDevice inline_at failed");
        return false;
    }

    hook_create_device_ = std::move(*result);
    s_original_create_device = hook_create_device_->original<D3D12CreateDeviceFunc>();

    DetourModKit::Result<void> enable_result = hook_create_device_->enable();
    if (!enable_result)
    {
        log().error("DeviceHook: D3D12CreateDevice hook failed to enable");
        s_original_create_device = nullptr;
        hook_create_device_.reset();
        return false;
    }

    log().info("DeviceHook: D3D12CreateDevice hook installed and enabled - waiting for the game to create its device");
    return true;
}

void DeviceHook::Uninstall()
{
    SetRenderTickCallback(nullptr);
    WaitForRenderCallbacksIdle();
    if (s_hook_om_set_render_targets) { (void)s_hook_om_set_render_targets->disable(); s_hook_om_set_render_targets.reset(); }
    if (s_hook_draw_indexed_instanced) { (void)s_hook_draw_indexed_instanced->disable(); s_hook_draw_indexed_instanced.reset(); }
    if (s_hook_draw_instanced) { (void)s_hook_draw_instanced->disable(); s_hook_draw_instanced.reset(); }
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
    s_original_create_command_list = nullptr;
    s_original_resource_barrier = nullptr;
    s_original_create_rtv = nullptr;
    s_original_create_dsv = nullptr;
    s_original_draw_instanced = nullptr;
    s_original_draw_indexed_instanced = nullptr;
    s_original_om_set_render_targets = nullptr;
    s_queue_hook_installed.store(false, std::memory_order_relaxed);
    s_execute_hook_installed.store(false, std::memory_order_relaxed);
    s_command_list_hook_installed.store(false, std::memory_order_relaxed);
    s_resource_barrier_hook_installed.store(false, std::memory_order_relaxed);
    s_create_rtv_hook_installed.store(false, std::memory_order_relaxed);
    s_create_dsv_hook_installed.store(false, std::memory_order_relaxed);
    s_draw_hooks_installed.store(false, std::memory_order_relaxed);
    s_om_set_rt_hook_installed.store(false, std::memory_order_relaxed);
    { std::lock_guard<std::mutex> lock(s_rtv_map_mutex); s_rtv_resource_map.clear(); }
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

bool DeviceHook::TryGetLatestPresentResourceTagged(ID3D12Resource** out_resource, int* out_eye, std::uint64_t* out_serial)
{
    if (!out_resource) return false;
    *out_resource = nullptr;
    std::lock_guard<std::mutex> lock(s_present_resource_mutex);
    if (!s_latest_present_resource) return false;
    s_latest_present_resource->AddRef();
    *out_resource = s_latest_present_resource;
    if (out_eye) *out_eye = s_latest_present_eye;
    if (out_serial) *out_serial = s_latest_present_serial;
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

bool DeviceHook::InstallQueueHook(ID3D12Device* device)
{
    if (s_queue_hook_installed.load(std::memory_order_acquire))
        return true; // already installed

    void** vtable = *reinterpret_cast<void***>(device);
    // ID3D12Device vtable layout (stable, published):
    // 0-2 IUnknown, 3-6 ID3D12Object, 7 GetNodeCount, 8 CreateCommandQueue.
    constexpr size_t kCreateCommandQueueIndex = 8;
    void* target_address = vtable[kCreateCommandQueueIndex];

    log().info("DeviceHook: ID3D12Device::CreateCommandQueue vtable address = {:#x}",
               reinterpret_cast<uintptr_t>(target_address));

    hook::InlineRequest request{
        "kcd2vr_device_hook_create_queue",
        hook::Target{DetourModKit::Address{reinterpret_cast<uintptr_t>(target_address)}},
        hook::Options{}
    };

    DetourModKit::Result<hook::Hook> result = hook::inline_at(std::move(request), &DeviceHook::DetourCreateCommandQueue);
    if (!result)
    {
        log().error("DeviceHook: CreateCommandQueue inline_at failed");
        return false;
    }

    s_hook_create_queue = std::move(*result);
    s_original_create_queue = s_hook_create_queue->original<CreateCommandQueueFunc>();

    DetourModKit::Result<void> enable_result = s_hook_create_queue->enable();
    if (!enable_result)
    {
        log().error("DeviceHook: CreateCommandQueue hook failed to enable");
        s_original_create_queue = nullptr;
        s_hook_create_queue.reset();
        return false;
    }

    s_queue_hook_installed.store(true, std::memory_order_release);
    log().info("DeviceHook: CreateCommandQueue hook installed and enabled");
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
        InstallQueueHook(device);
        InstallCreateCommandListHook(device);
        InstallCreateRenderTargetViewHook(device);
        InstallCreateDepthStencilViewHook(device);
    }

    return hr;
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
    if (!result) { log().error("V37-BRIDGE: CreateCommandList hook install failed"); return false; }
    s_hook_create_command_list = std::move(*result);
    s_original_create_command_list = s_hook_create_command_list->original<CreateCommandListFunc>();
    if (!s_hook_create_command_list->enable()) {
        s_original_create_command_list = nullptr;
        s_hook_create_command_list.reset();
        log().error("V37-BRIDGE: CreateCommandList hook enable failed");
        return false;
    }
    s_command_list_hook_installed.store(true, std::memory_order_release);
    log().info("V37-BRIDGE: CreateCommandList hook enabled");
    return true;
}

HRESULT __stdcall DeviceHook::DetourCreateCommandList(
    ID3D12Device* device_this, UINT node_mask, D3D12_COMMAND_LIST_TYPE type,
    ID3D12CommandAllocator* allocator, ID3D12PipelineState* initial_state, REFIID riid, void** out_list)
{
    HRESULT hr = s_original_create_command_list
        ? s_original_create_command_list(device_this, node_mask, type, allocator, initial_state, riid, out_list)
        : E_FAIL;
    if (SUCCEEDED(hr) && out_list && *out_list && type == D3D12_COMMAND_LIST_TYPE_DIRECT &&
        !s_resource_barrier_hook_installed.load(std::memory_order_acquire))
    {
        auto* list = static_cast<ID3D12GraphicsCommandList*>(*out_list);
        InstallResourceBarrierHook(list);
        InstallOMSetRenderTargetsHook(list);
        // V61.36 CLEAN: no per-draw depth hooks. Exact source-size DSV selection is bind-based.
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
        log().error("V40.1-BRIDGE: ResourceBarrier hook install failed");
        return false;
    }
    s_hook_resource_barrier = std::move(*result);
    s_original_resource_barrier = s_hook_resource_barrier->original<ResourceBarrierFunc>();
    if (!s_hook_resource_barrier->enable()) {
        s_original_resource_barrier = nullptr;
    s_original_create_rtv = nullptr;
    s_original_create_dsv = nullptr;
    s_original_draw_instanced = nullptr;
    s_original_draw_indexed_instanced = nullptr;
    s_original_om_set_render_targets = nullptr;
        s_hook_resource_barrier.reset();
        s_resource_barrier_hook_installed.store(false, std::memory_order_release);
        log().error("V40.1-BRIDGE: ResourceBarrier hook enable failed");
        return false;
    }
    log().info("V40.1-BRIDGE: ResourceBarrier PRESENT capture hook enabled (single-owner CAS)");
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
    if (!result) { log().warning("V61.6-NATIVE-RT: CreateRenderTargetView hook install failed"); return false; }
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
    if (!result) { log().warning("V61.34-DEPTH: CreateDepthStencilView hook install failed"); return false; }
    s_hook_create_dsv = std::move(*result);
    s_original_create_dsv = s_hook_create_dsv->original<CreateDepthStencilViewFunc>();
    if (!s_hook_create_dsv->enable()) { s_original_create_dsv=nullptr; s_hook_create_dsv.reset(); return false; }
    s_create_dsv_hook_installed.store(true,std::memory_order_release);
    debug_trace::Write("V61.34-DEPTH CreateDepthStencilView observer armed; descriptor->resource map only");
    return true;
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
    if(!h0){ s_draw_hooks_installed.store(false,std::memory_order_release); log().warning("V61.34-DEPTH: DrawInstanced hook install failed"); return false; }
    s_hook_draw_instanced=std::move(*h0); s_original_draw_instanced=s_hook_draw_instanced->original<DrawInstancedFunc>();
    if(!s_hook_draw_instanced->enable()){ s_original_draw_instanced=nullptr; s_hook_draw_instanced.reset(); s_draw_hooks_installed.store(false,std::memory_order_release); return false; }
    hook::InlineRequest r1{"kcd2vr_v6134_drawindexed_probe",hook::Target{DetourModKit::Address{reinterpret_cast<uintptr_t>(vtable[kDrawIndexedInstancedIndex])}},hook::Options{}};
    auto h1=hook::inline_at(std::move(r1),&DeviceHook::DetourDrawIndexedInstanced);
    if(!h1){ log().warning("V61.34-DEPTH: DrawIndexedInstanced hook install failed; non-indexed draw scoring remains available"); return true; }
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

void __stdcall DeviceHook::DetourDrawInstanced(
    ID3D12GraphicsCommandList* list_this, UINT vertex_count_per_instance, UINT instance_count,
    UINT start_vertex_location, UINT start_instance_location)
{
    if(s_generic_depth_probe_enabled.load(std::memory_order_relaxed)){
        const int i=s_tls_depth_candidate; if(i>=0 && static_cast<std::size_t>(i)<kGenericDepthCandidateCount) s_generic_depth_candidates[i].draws.fetch_add(1,std::memory_order_relaxed);
    }
    if(s_original_draw_instanced) s_original_draw_instanced(list_this,vertex_count_per_instance,instance_count,start_vertex_location,start_instance_location);
}

void __stdcall DeviceHook::DetourDrawIndexedInstanced(
    ID3D12GraphicsCommandList* list_this, UINT index_count_per_instance, UINT instance_count,
    UINT start_index_location, INT base_vertex_location, UINT start_instance_location)
{
    if(s_generic_depth_probe_enabled.load(std::memory_order_relaxed)){
        const int i=s_tls_depth_candidate; if(i>=0 && static_cast<std::size_t>(i)<kGenericDepthCandidateCount){ s_generic_depth_candidates[i].draws.fetch_add(1,std::memory_order_relaxed); s_generic_depth_candidates[i].indexed_draws.fetch_add(1,std::memory_order_relaxed); }
    }
    if(s_original_draw_indexed_instanced) s_original_draw_indexed_instanced(list_this,index_count_per_instance,instance_count,start_index_location,base_vertex_location,start_instance_location);
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

bool DeviceHook::InstallOMSetRenderTargetsHook(ID3D12GraphicsCommandList* list)
{
    if (!list || s_om_set_rt_hook_installed.load(std::memory_order_acquire)) return true;
    void** vtable = *reinterpret_cast<void***>(list);
    constexpr size_t kOMSetRenderTargetsIndex = 46;
    void* target = vtable[kOMSetRenderTargetsIndex];
    hook::InlineRequest req{"kcd2vr_v616_omsetrt_probe", hook::Target{DetourModKit::Address{reinterpret_cast<uintptr_t>(target)}}, hook::Options{}};
    auto result = hook::inline_at(std::move(req), &DeviceHook::DetourOMSetRenderTargets);
    if (!result) { log().warning("V61.6-NATIVE-RT: OMSetRenderTargets hook install failed"); return false; }
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
        "V61.30-STEREO-PRODUCER UNIQUE n=%llu resource=%p size=%llux%u fmt=%u flags=0x%X name='%s'",
        static_cast<unsigned long long>(unique_index),resource,static_cast<unsigned long long>(rd.Width),rd.Height,
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
    if (!internal_vr && !internal_menu && rt_handles && num_rt > 0 && FrameSync::CurrentPipelineViewType() == 0) {
        const UINT inc = s_rtv_descriptor_increment.load(std::memory_order_acquire);
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

            // V61.28 F8: exact-size native-stereo producer archaeology. The shipped
            // Stereo.cfx final compositor expects distinct StereoL/StereoR textures.
            // Instead of re-entering a camera/view again, observe whether KCD2 binds
            // multiple distinct source-sized render targets in the same present epoch.
            if (s_stereo_producer_probe_enabled.load(std::memory_order_acquire)) {
                const auto tw=s_stereo_producer_probe_width.load(std::memory_order_acquire);
                const auto th=s_stereo_producer_probe_height.load(std::memory_order_acquire);
                if (tw>0 && th>0 && rd.Width==static_cast<UINT64>(tw) && rd.Height==th) {
                    const auto bindn=s_stereo_producer_bind_count.fetch_add(1,std::memory_order_relaxed)+1;
                    std::uint64_t epoch=0,pt=0,pw=0,pc=0; std::uint32_t ph=0;
                    GetLatestPresentIdentity(&epoch,&pt,&pw,&ph,&pc);
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
                                        "V61.30-STEREO-PRODUCER PAIR n=%llu epoch=%llu A=%p B=%p size=%ux%u cameraFrame=%llu",
                                        static_cast<unsigned long long>(pairs),static_cast<unsigned long long>(epoch),first,resource,tw,th,
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
    s_stereo_producer_probe_enabled.store(enabled,std::memory_order_release);
    char m[320]{}; std::snprintf(m,sizeof(m),
        "V61.30-STEREO-PRODUCER %s target=%ux%u exact-size RT pair capture; StereoL/R hypothesis; renderer/camera writes=NONE",
        enabled?"ON":"OFF",width,height); debug_trace::Write(m);
}
std::uint64_t DeviceHook::StereoProducerBindCount(){return s_stereo_producer_bind_count.load(std::memory_order_acquire);}
std::uint64_t DeviceHook::StereoProducerUniqueCount(){return s_stereo_producer_unique_count.load(std::memory_order_acquire);}
std::uint64_t DeviceHook::StereoProducerSameFramePairCount(){return s_stereo_producer_pair_count.load(std::memory_order_acquire);}

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
    if (!internal_vr && !internal_menu && barriers) {
        for (UINT i = 0; i < num_barriers; ++i) {
            const auto& b = barriers[i];
            if (b.Type != D3D12_RESOURCE_BARRIER_TYPE_TRANSITION || !b.Transition.pResource) continue;

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

            if (b.Transition.StateAfter != D3D12_RESOURCE_STATE_PRESENT) continue;
            ID3D12Resource* resource = b.Transition.pResource;
            const D3D12_RESOURCE_DESC desc = resource->GetDesc();
            if (desc.Dimension != D3D12_RESOURCE_DIMENSION_TEXTURE2D || desc.Width == 0 || desc.Height == 0) continue;
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
                s_latest_present_camera_frame = CameraHook::LastPoseFrameIndex();
                advance_afr_eye = false; // V41: PRESENT is not a frame boundary.
            }
            if (old) old->Release();
            const auto n = s_present_capture_count.fetch_add(1, std::memory_order_relaxed) + 1;
            if (n <= 8 || (n % 600) == 0) {
                log().info("V37-PRESENT-CAPTURE call={} resource={:#x} {}x{} fmt={} before={} after=PRESENT",
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

    log().info("DeviceHook: ID3D12CommandQueue::ExecuteCommandLists vtable address = {:#x}",
               reinterpret_cast<uintptr_t>(target_address));

    hook::InlineRequest request{
        "kcd2vr_device_hook_execute_command_lists",
        hook::Target{DetourModKit::Address{reinterpret_cast<uintptr_t>(target_address)}},
        hook::Options{}
    };

    DetourModKit::Result<hook::Hook> result = hook::inline_at(std::move(request), &DeviceHook::DetourExecuteCommandLists);
    if (!result)
    {
        log().error("DeviceHook: ExecuteCommandLists inline_at failed");
        return false;
    }

    s_hook_execute_command_lists = std::move(*result);
    s_original_execute_command_lists = s_hook_execute_command_lists->original<ExecuteCommandListsFunc>();
    auto enable_result = s_hook_execute_command_lists->enable();
    if (!enable_result)
    {
        log().error("DeviceHook: ExecuteCommandLists hook failed to enable");
        s_original_execute_command_lists = nullptr;
        s_hook_execute_command_lists.reset();
        return false;
    }

    s_execute_hook_installed.store(true, std::memory_order_release);
    log().info("DeviceHook: ExecuteCommandLists render-thread hook installed and enabled");
    return true;
}

void __stdcall DeviceHook::DetourExecuteCommandLists(
    ID3D12CommandQueue* queue_this, UINT num_command_lists, ID3D12CommandList* const* command_lists)
{
    // Preserve the game's submission first. The callback then runs on the same
    // thread/context immediately after the game's queue submission.
    if (s_original_execute_command_lists)
        s_original_execute_command_lists(queue_this, num_command_lists, command_lists);

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
                log().warning("DeviceHook: DIRECT queue captured, but render-thread ExecuteCommandLists hook could not be installed");
            log().info("DeviceHook: captured game's DIRECT D3D12 queue (priority={}, flags={})",
                       actual_desc.Priority, static_cast<unsigned>(actual_desc.Flags));
        }
        else
        {
            log().debug("DeviceHook: ignored non-DIRECT D3D12 queue type={}",
                        static_cast<int>(actual_desc.Type));
        }
    }

    return hr;
}

} // namespace kcd2vr
