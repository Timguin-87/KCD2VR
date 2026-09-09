// xr_manager.cpp
#include "xr_manager.h"
#include "debug_trace.h"
#include "device_hook.h"
#include <DetourModKit/logger.hpp>
#include <string>
#include <cstring>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <d3dcompiler.h>

namespace kcd2vr {
namespace {
struct V3f { float x,y,z; };
static XrQuaternionf QConj(const XrQuaternionf& q){ return {-q.x,-q.y,-q.z,q.w}; }
static XrQuaternionf QMul(const XrQuaternionf& a,const XrQuaternionf& b){ return {
    a.w*b.x+a.x*b.w+a.y*b.z-a.z*b.y,
    a.w*b.y-a.x*b.z+a.y*b.w+a.z*b.x,
    a.w*b.z+a.x*b.y-a.y*b.x+a.z*b.w,
    a.w*b.w-a.x*b.x-a.y*b.y-a.z*b.z}; }
static V3f QRotate(const XrQuaternionf& q,const V3f& v){ XrQuaternionf p{v.x,v.y,v.z,0}; auto r=QMul(QMul(q,p),QConj(q)); return {r.x,r.y,r.z}; }
static float VDot(const V3f& a,const V3f& b){ return a.x*b.x+a.y*b.y+a.z*b.z; }
static V3f VCross(const V3f& a,const V3f& b){ return {a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x}; }
static float VLength(const V3f& v){ return std::sqrt(VDot(v,v)); }
static V3f VNormalize(const V3f& v){ const float l=VLength(v); return l>1.0e-6f?V3f{v.x/l,v.y/l,v.z/l}:V3f{0,0,0}; }
static XrQuaternionf QFromAxes(const V3f& x,const V3f& y,const V3f& z){
    // Rotation matrix columns are the desired local X/Y/Z axes in view space.
    const float m00=x.x,m01=y.x,m02=z.x;
    const float m10=x.y,m11=y.y,m12=z.y;
    const float m20=x.z,m21=y.z,m22=z.z;
    XrQuaternionf q{};
    const float tr=m00+m11+m22;
    if(tr>0.0f){
        const float ss=std::sqrt(tr+1.0f)*2.0f;
        q.w=0.25f*ss; q.x=(m21-m12)/ss; q.y=(m02-m20)/ss; q.z=(m10-m01)/ss;
    } else if(m00>m11 && m00>m22){
        const float ss=std::sqrt(1.0f+m00-m11-m22)*2.0f;
        q.w=(m21-m12)/ss; q.x=0.25f*ss; q.y=(m01+m10)/ss; q.z=(m02+m20)/ss;
    } else if(m11>m22){
        const float ss=std::sqrt(1.0f+m11-m00-m22)*2.0f;
        q.w=(m02-m20)/ss; q.x=(m01+m10)/ss; q.y=0.25f*ss; q.z=(m12+m21)/ss;
    } else {
        const float ss=std::sqrt(1.0f+m22-m00-m11)*2.0f;
        q.w=(m10-m01)/ss; q.x=(m02+m20)/ss; q.y=(m12+m21)/ss; q.z=0.25f*ss;
    }
    return q;
}
static XrQuaternionf QYawOnly(const XrQuaternionf& q){
    // Extract OpenXR Y-up yaw and intentionally discard pitch/roll so comfort
    // panels are level with the world even if the headset is tilted at capture.
    const float siny = 2.0f * (q.w*q.y + q.x*q.z);
    const float cosy = 1.0f - 2.0f * (q.y*q.y + q.z*q.z);
    const float yaw = std::atan2(siny, cosy);
    const float h = yaw * 0.5f;
    return {0.0f, std::sin(h), 0.0f, std::cos(h)};
}

}


namespace {
std::atomic<std::uint64_t> g_xr_frame_count{0};
std::atomic<std::uint64_t> g_valid_pose_count{0};
}

XrManager& XrManager::Instance()
{
    static XrManager instance;
    return instance;
}

bool XrManager::CreateInstance()
{
    try {
        bool have_qpc_time = false;
        bool have_cylinder = false;
        uint32_t ext_count = 0;
        if (XR_SUCCEEDED(xrEnumerateInstanceExtensionProperties(nullptr, 0, &ext_count, nullptr)) && ext_count) {
            std::vector<XrExtensionProperties> props;
            props.reserve(ext_count);
            for (uint32_t i = 0; i < ext_count; ++i) props.push_back(XrExtensionProperties{XR_TYPE_EXTENSION_PROPERTIES});
            if (XR_SUCCEEDED(xrEnumerateInstanceExtensionProperties(nullptr, ext_count, &ext_count, props.data()))) {
                for (const auto& e : props) {
                    if (std::strcmp(e.extensionName, XR_KHR_WIN32_CONVERT_PERFORMANCE_COUNTER_TIME_EXTENSION_NAME) == 0)
                        have_qpc_time = true;
#if defined(XR_KHR_COMPOSITION_LAYER_CYLINDER_EXTENSION_NAME)
                    if (std::strcmp(e.extensionName, XR_KHR_COMPOSITION_LAYER_CYLINDER_EXTENSION_NAME) == 0)
                        have_cylinder = true;
#endif
                }
            }
        }

        std::vector<const char*> extensions;
        extensions.push_back(XR_KHR_D3D12_ENABLE_EXTENSION_NAME);
        if (have_qpc_time) extensions.push_back(XR_KHR_WIN32_CONVERT_PERFORMANCE_COUNTER_TIME_EXTENSION_NAME);
#if defined(XR_KHR_COMPOSITION_LAYER_CYLINDER_EXTENSION_NAME)
        if (have_cylinder) extensions.push_back(XR_KHR_COMPOSITION_LAYER_CYLINDER_EXTENSION_NAME);
#else
        have_cylinder = false;
#endif
        XrInstanceCreateInfo create_info{ XR_TYPE_INSTANCE_CREATE_INFO };
        create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        create_info.enabledExtensionNames = extensions.data();
        std::strncpy(create_info.applicationInfo.applicationName, "KCD2VR", XR_MAX_APPLICATION_NAME_SIZE - 1);
        create_info.applicationInfo.applicationVersion = 1;
        create_info.applicationInfo.engineName[0] = '\0';
        create_info.applicationInfo.engineVersion = 1;
        create_info.applicationInfo.apiVersion = XR_API_VERSION_1_0;

        XrResult result = xrCreateInstance(&create_info, &instance_);
        if (XR_FAILED(result)) {
            DetourModKit::log().error("xrCreateInstance failed - code={}", static_cast<int>(result));
            return false;
        }

        qpc_time_extension_enabled_ = have_qpc_time;
        cylinder_layer_supported_ = have_cylinder;
        if (qpc_time_extension_enabled_) {
            PFN_xrVoidFunction fn = nullptr;
            if (XR_SUCCEEDED(xrGetInstanceProcAddr(instance_, "xrConvertWin32PerformanceCounterToTimeKHR", &fn)) && fn)
                qpc_to_xr_time_ = reinterpret_cast<PFN_xrConvertWin32PerformanceCounterToTimeKHR>(fn);
            else
                qpc_time_extension_enabled_ = false;
        }
        debug_trace::Write(qpc_time_extension_enabled_
            ? "V48-LATE-POSE QPC->XrTime extension ready"
            : "V48-LATE-POSE QPC->XrTime unavailable; legacy frame pose fallback active");
        debug_trace::Write(cylinder_layer_supported_
            ? "V58-MENU XR_KHR_composition_layer_cylinder enabled"
            : "V58-MENU cylinder extension unavailable; quad fallback will be used");
        return true;
    }
    catch (...) {
        DetourModKit::log().error("Exception caught in CreateInstance()");
        return false;
    }
}

bool XrManager::GetSystem()
{
    XrSystemGetInfo system_info{ XR_TYPE_SYSTEM_GET_INFO };
    system_info.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;

    XrResult result = xrGetSystem(instance_, &system_info, &system_id_);
    if (XR_FAILED(result))
    {
        DetourModKit::log().error("xrGetSystem failed - code={}", static_cast<int>(result));
        return false;
    }

    XrInstanceProperties instance_props{ XR_TYPE_INSTANCE_PROPERTIES };
    const XrResult ipr = xrGetInstanceProperties(instance_, &instance_props);
    XrSystemProperties system_props{ XR_TYPE_SYSTEM_PROPERTIES };
    const XrResult spr = xrGetSystemProperties(instance_, system_id_, &system_props);
    uint32_t view_count = 0;
    XrResult vcr = xrEnumerateViewConfigurationViews(instance_, system_id_, XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO, 0, &view_count, nullptr);
    std::vector<XrViewConfigurationView> views;
    if (XR_SUCCEEDED(vcr) && view_count) {
        views.resize(view_count);
        for (auto& v : views) v = XrViewConfigurationView{ XR_TYPE_VIEW_CONFIGURATION_VIEW };
        vcr = xrEnumerateViewConfigurationViews(instance_, system_id_, XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO, view_count, &view_count, views.data());
    }
    DetourModKit::log().info("XR-RUNTIME-V29 runtime='{}' runtimeVersion={}.{}.{} instanceResult={} system='{}' vendorId={} systemResult={} stereoViews={} viewEnumResult={}",
        XR_SUCCEEDED(ipr) ? instance_props.runtimeName : "<unknown>",
        XR_SUCCEEDED(ipr) ? XR_VERSION_MAJOR(instance_props.runtimeVersion) : 0,
        XR_SUCCEEDED(ipr) ? XR_VERSION_MINOR(instance_props.runtimeVersion) : 0,
        XR_SUCCEEDED(ipr) ? XR_VERSION_PATCH(instance_props.runtimeVersion) : 0,
        static_cast<int>(ipr), XR_SUCCEEDED(spr) ? system_props.systemName : "<unknown>",
        XR_SUCCEEDED(spr) ? system_props.vendorId : 0u, static_cast<int>(spr), view_count, static_cast<int>(vcr));
    for (uint32_t i=0; i<view_count && i<4 && i<views.size(); ++i) {
        const auto& v=views[i];
        DetourModKit::log().info("XR-RUNTIME-V29 view={} recommended={}x{} max={}x{} samplesRec={} samplesMax={}",
            i,v.recommendedImageRectWidth,v.recommendedImageRectHeight,v.maxImageRectWidth,v.maxImageRectHeight,v.recommendedSwapchainSampleCount,v.maxSwapchainSampleCount);
    }
    return true;
}

bool XrManager::CreateD3D12Objects()
{
    if (!d3d12_device_ || !d3d12_queue_)
        return false;

    const D3D12_COMMAND_QUEUE_DESC queue_desc = d3d12_queue_->GetDesc();
    if (queue_desc.Type != D3D12_COMMAND_LIST_TYPE_DIRECT)
    {
        DetourModKit::log().error("Captured D3D12 queue is not DIRECT (type={})", static_cast<int>(queue_desc.Type));
        return false;
    }

    HRESULT hr = d3d12_device_->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&command_allocator_));
    if (FAILED(hr))
    {
        DetourModKit::log().error("CreateCommandAllocator failed, hr=0x{:08X}", static_cast<unsigned>(hr));
        return false;
    }

    hr = d3d12_device_->CreateCommandList(
        0, D3D12_COMMAND_LIST_TYPE_DIRECT, command_allocator_, nullptr,
        IID_PPV_ARGS(&command_list_));
    if (FAILED(hr))
    {
        DetourModKit::log().error("CreateCommandList failed, hr=0x{:08X}", static_cast<unsigned>(hr));
        return false;
    }

    DeviceHook::SetInternalVrCommandList(command_list_);

    // A newly created command list starts open; close it so the first RunFrame
    // can Reset() it before recording commands.
    hr = command_list_->Close();
    if (FAILED(hr))
    {
        DetourModKit::log().error("Initial command-list Close failed, hr=0x{:08X}", static_cast<unsigned>(hr));
        return false;
    }

    hr = d3d12_device_->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
    if (FAILED(hr))
    {
        DetourModKit::log().error("CreateFence failed, hr=0x{:08X}", static_cast<unsigned>(hr));
        return false;
    }

    fence_event_ = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!fence_event_)
    {
        DetourModKit::log().error("CreateEventW failed for D3D12 fence");
        return false;
    }

    return true;
}

bool XrManager::CreateSession(ID3D12Device* game_device, ID3D12CommandQueue* game_queue)
{
    if (!game_device || !game_queue) return false;

    d3d12_device_ = game_device;
    d3d12_device_->AddRef();
    d3d12_queue_ = game_queue;
    d3d12_queue_->AddRef();

    PFN_xrGetD3D12GraphicsRequirementsKHR get_requirements = nullptr;
    XrResult proc_result = xrGetInstanceProcAddr(
        instance_, "xrGetD3D12GraphicsRequirementsKHR",
        reinterpret_cast<PFN_xrVoidFunction*>(&get_requirements));
    if (XR_FAILED(proc_result) || !get_requirements)
    {
        DetourModKit::log().error("xrGetD3D12GraphicsRequirementsKHR is unavailable - code={}",
                                  static_cast<int>(proc_result));
        return false;
    }

    XrGraphicsRequirementsD3D12KHR requirements{ XR_TYPE_GRAPHICS_REQUIREMENTS_D3D12_KHR };
    XrResult result = get_requirements(instance_, system_id_, &requirements);
    if (XR_FAILED(result))
    {
        DetourModKit::log().error("xrGetD3D12GraphicsRequirementsKHR failed - code={}", static_cast<int>(result));
        return false;
    }

    // Verify that the game's actual device meets the runtime's minimum feature
    // level. This avoids the misleading "D3D12 compatible GPU" failure later.
    D3D12_FEATURE_DATA_FEATURE_LEVELS feature_levels{};
    const D3D_FEATURE_LEVEL levels[] = {
        D3D_FEATURE_LEVEL_12_2, D3D_FEATURE_LEVEL_12_1,
        D3D_FEATURE_LEVEL_12_0, D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0
    };
    feature_levels.NumFeatureLevels = static_cast<UINT>(std::size(levels));
    feature_levels.pFeatureLevelsRequested = levels;
    hr_check_feature_level_ = d3d12_device_->CheckFeatureSupport(
        D3D12_FEATURE_FEATURE_LEVELS, &feature_levels, sizeof(feature_levels));
    if (FAILED(hr_check_feature_level_))
    {
        DetourModKit::log().error("D3D12 CheckFeatureSupport failed, hr=0x{:08X}",
                                  static_cast<unsigned>(hr_check_feature_level_));
        return false;
    }

    DetourModKit::log().info("OpenXR D3D12 requirements: minFeatureLevel={}",
                             static_cast<int>(requirements.minFeatureLevel));
    DetourModKit::log().info("Game D3D12 device max feature level={}",
                             static_cast<int>(feature_levels.MaxSupportedFeatureLevel));

    if (feature_levels.MaxSupportedFeatureLevel < requirements.minFeatureLevel)
    {
        DetourModKit::log().error("Game D3D12 device does not meet OpenXR minimum feature level");
        return false;
    }

    // D3D12 devices expose their adapter LUID directly. Querying IDXGIDevice
    // from ID3D12Device is invalid and returns E_NOINTERFACE on normal D3D12
    // devices. Compare the native D3D12 LUID with the one required by OpenXR.
    const LUID game_adapter_luid = d3d12_device_->GetAdapterLuid();
    hr_check_adapter_ = S_OK;

    if (std::memcmp(&game_adapter_luid, &requirements.adapterLuid, sizeof(LUID)) != 0)
    {
        DetourModKit::log().error(
            "OpenXR D3D12 adapter LUID does not match the game's adapter. "
            "Use the same GPU for KCD2, Virtual Desktop VDXR and OpenXR. "
            "game={:08X}:{:08X}, openxr={:08X}:{:08X}",
            static_cast<unsigned>(game_adapter_luid.HighPart),
            static_cast<unsigned>(game_adapter_luid.LowPart),
            static_cast<unsigned>(requirements.adapterLuid.HighPart),
            static_cast<unsigned>(requirements.adapterLuid.LowPart));
        return false;
    }

    DetourModKit::log().info(
        "OpenXR D3D12 adapter LUID matches game's adapter: {:08X}:{:08X}",
        static_cast<unsigned>(game_adapter_luid.HighPart),
        static_cast<unsigned>(game_adapter_luid.LowPart));

    XrGraphicsBindingD3D12KHR binding{ XR_TYPE_GRAPHICS_BINDING_D3D12_KHR };
    binding.device = d3d12_device_;
    binding.queue = d3d12_queue_;

    XrSessionCreateInfo session_info{ XR_TYPE_SESSION_CREATE_INFO };
    session_info.next = &binding;
    session_info.systemId = system_id_;

    result = xrCreateSession(instance_, &session_info, &session_);
    if (XR_FAILED(result))
    {
        DetourModKit::log().error("xrCreateSession failed - code={}", static_cast<int>(result));
        return false;
    }

    return true;
}

bool XrManager::CreateReferenceSpace()
{
    XrReferenceSpaceCreateInfo local_info{ XR_TYPE_REFERENCE_SPACE_CREATE_INFO };
    local_info.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL;
    local_info.poseInReferenceSpace.orientation.w = 1.0f;
    if (XR_FAILED(xrCreateReferenceSpace(session_, &local_info, &local_space_))) return false;

    XrReferenceSpaceCreateInfo view_info{ XR_TYPE_REFERENCE_SPACE_CREATE_INFO };
    view_info.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_VIEW;
    view_info.poseInReferenceSpace.orientation.w = 1.0f;
    if (XR_FAILED(xrCreateReferenceSpace(session_, &view_info, &view_space_))) return false;

    return true;
}

bool XrManager::CreateSwapchains()
{
    uint32_t view_count = 0;
    XrResult result = xrEnumerateViewConfigurationViews(
        instance_, system_id_, XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO, 0, &view_count, nullptr);
    if (XR_FAILED(result) || view_count != 2) return false;

    XrViewConfigurationView views[2] = {
        { XR_TYPE_VIEW_CONFIGURATION_VIEW }, { XR_TYPE_VIEW_CONFIGURATION_VIEW }
    };
    result = xrEnumerateViewConfigurationViews(
        instance_, system_id_, XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO, 2, &view_count, views);
    if (XR_FAILED(result)) return false;

    uint32_t format_count = 0;
    result = xrEnumerateSwapchainFormats(session_, 0, &format_count, nullptr);
    if (XR_FAILED(result) || format_count == 0) return false;

    std::vector<int64_t> supported_formats(format_count);
    result = xrEnumerateSwapchainFormats(session_, format_count, &format_count, supported_formats.data());
    if (XR_FAILED(result)) return false;

    // Pick a conventional D3D12 render-target format rather than blindly using
    // formats[0]. Both formats are standard DXGI color formats and avoid an
    // accidental typeless/sRGB-only choice causing CreateRenderTargetView or
    // resource barriers to fail.
    const DXGI_FORMAT preferred_formats[] = {
        DXGI_FORMAT_R8G8B8A8_UNORM,
        DXGI_FORMAT_B8G8R8A8_UNORM,
        DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        DXGI_FORMAT_B8G8R8A8_UNORM_SRGB
    };

    int64_t swapchain_format = 0;
    for (DXGI_FORMAT preferred : preferred_formats)
    {
        if (std::find(supported_formats.begin(), supported_formats.end(), static_cast<int64_t>(preferred)) != supported_formats.end())
        {
            swapchain_format = static_cast<int64_t>(preferred);
            break;
        }
    }

    if (swapchain_format == 0)
    {
        DetourModKit::log().error("No supported standard D3D12 color swapchain format found");
        return false;
    }

    swapchain_format_ = static_cast<DXGI_FORMAT>(swapchain_format);
    if (!CreateRtvHeap()) return false;

    DetourModKit::log().info("Using D3D12 OpenXR swapchain format={}", static_cast<int>(swapchain_format));

    for (int eye = 0; eye < 2; ++eye)
    {
        EyeSwapchain& e = eyes_[eye];
        const uint32_t recommended_w = views[eye].recommendedImageRectWidth;
        const uint32_t recommended_h = views[eye].recommendedImageRectHeight;
        const float configured_render_scale = render_scale_.load(std::memory_order_acquire);
        const float rs = (configured_render_scale < 0.5f ? 0.5f : (configured_render_scale > 2.0f ? 2.0f : configured_render_scale));
        e.width = static_cast<int32_t>(std::clamp<uint32_t>(static_cast<uint32_t>(recommended_w * rs + 0.5f), 512u, views[eye].maxImageRectWidth));
        e.height = static_cast<int32_t>(std::clamp<uint32_t>(static_cast<uint32_t>(recommended_h * rs + 0.5f), 512u, views[eye].maxImageRectHeight));
        { char msg[256]{}; std::snprintf(msg, sizeof(msg),
            "V38-RESOLUTION eye=%u recommended=%ux%u scale=%.3f swapchain=%dx%d max=%ux%u", eye,
            recommended_w, recommended_h, rs, e.width, e.height,
            views[eye].maxImageRectWidth, views[eye].maxImageRectHeight); debug_trace::Write(msg); }

        XrSwapchainCreateInfo sc_info{ XR_TYPE_SWAPCHAIN_CREATE_INFO };
        sc_info.usageFlags = XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT | XR_SWAPCHAIN_USAGE_SAMPLED_BIT;
        sc_info.format = swapchain_format;
        sc_info.sampleCount = 1;
        sc_info.width = static_cast<uint32_t>(e.width);
        sc_info.height = static_cast<uint32_t>(e.height);
        sc_info.faceCount = 1;
        sc_info.arraySize = 1;
        sc_info.mipCount = 1;

        result = xrCreateSwapchain(session_, &sc_info, &e.swapchain);
        if (XR_FAILED(result))
        {
            DetourModKit::log().error("xrCreateSwapchain eye={} failed - code={}", eye, static_cast<int>(result));
            return false;
        }

        uint32_t image_count = 0;
        result = xrEnumerateSwapchainImages(e.swapchain, 0, &image_count, nullptr);
        if (XR_FAILED(result) || image_count == 0) return false;

        std::vector<XrSwapchainImageD3D12KHR> xr_images(image_count);
        for (auto& image : xr_images)
            image.type = XR_TYPE_SWAPCHAIN_IMAGE_D3D12_KHR;

        result = xrEnumerateSwapchainImages(
            e.swapchain, image_count, &image_count,
            reinterpret_cast<XrSwapchainImageBaseHeader*>(xr_images.data()));
        if (XR_FAILED(result)) return false;

        for (auto& image : xr_images)
            e.d3d12_images.push_back(image.texture);
    }

    // V50: dedicated UI swapchain for the MIS parchment. Keep it independent
    // from both gameplay eye swapchains so F7 can be composed additively.
    const DXGI_FORMAT preferred_ui = DXGI_FORMAT_B8G8R8A8_UNORM;
    const bool ui_bgra_supported = std::find(supported_formats.begin(), supported_formats.end(), static_cast<int64_t>(preferred_ui)) != supported_formats.end();
    menu_ui_format_ = preferred_ui;
    menu_ui_.width = 1100;
    menu_ui_.height = 680;
    XrSwapchainCreateInfo ui_sc{ XR_TYPE_SWAPCHAIN_CREATE_INFO };
    ui_sc.usageFlags = XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT | XR_SWAPCHAIN_USAGE_SAMPLED_BIT | XR_SWAPCHAIN_USAGE_TRANSFER_DST_BIT;
    ui_sc.format = static_cast<int64_t>(menu_ui_format_);
    ui_sc.sampleCount = 1; ui_sc.width = static_cast<uint32_t>(menu_ui_.width); ui_sc.height = static_cast<uint32_t>(menu_ui_.height);
    ui_sc.faceCount = 1; ui_sc.arraySize = 1; ui_sc.mipCount = 1;
    result = ui_bgra_supported ? xrCreateSwapchain(session_, &ui_sc, &menu_ui_.swapchain) : XR_ERROR_SWAPCHAIN_FORMAT_UNSUPPORTED;
    if (XR_FAILED(result)) {
        debug_trace::Write("V51-F7-QUAD dedicated UI swapchain creation failed; F7 quad disabled");
        menu_ui_.swapchain = XR_NULL_HANDLE;
    } else {
        uint32_t ui_count = 0;
        result = xrEnumerateSwapchainImages(menu_ui_.swapchain, 0, &ui_count, nullptr);
        if (XR_SUCCEEDED(result) && ui_count > 0) {
            std::vector<XrSwapchainImageD3D12KHR> ui_images(ui_count);
            for (auto& image : ui_images) image.type = XR_TYPE_SWAPCHAIN_IMAGE_D3D12_KHR;
            result = xrEnumerateSwapchainImages(menu_ui_.swapchain, ui_count, &ui_count, reinterpret_cast<XrSwapchainImageBaseHeader*>(ui_images.data()));
            if (XR_SUCCEEDED(result)) for (auto& image : ui_images) menu_ui_.d3d12_images.push_back(image.texture);
        }
        if (menu_ui_.d3d12_images.empty()) { xrDestroySwapchain(menu_ui_.swapchain); menu_ui_.swapchain = XR_NULL_HANDLE; }
        else {
            char um[192]{}; std::snprintf(um,sizeof(um),"V58-F7-UI swapchain ready %dx%d fmt=%d",menu_ui_.width,menu_ui_.height,static_cast<int>(menu_ui_format_));
            debug_trace::Write(um);
        }
    }

    active_swapchain_scale_.store(std::clamp(render_scale_.load(std::memory_order_acquire),0.50f,2.0f), std::memory_order_release);
    return true;
}

bool XrManager::EnsureInitialized(ID3D12Device* game_device, ID3D12CommandQueue* game_queue)
{
    if (session_running_.load(std::memory_order_relaxed)) return true;
    if (init_attempted_) return false;

    init_attempted_ = true;
    DetourModKit::log().info("Starting XrManager initialization (native D3D12)...");
    debug_trace::Write("XrManager initialization started");

    if (!CreateInstance()) return false;
    if (!GetSystem()) return false;
    if (!CreateSession(game_device, game_queue)) return false;
    if (!CreateReferenceSpace()) return false;
    if (diagnostic_rendering_.load(std::memory_order_acquire) || active_vr_bridge_.load(std::memory_order_acquire))
    {
        if (!CreateD3D12Objects()) return false;
        if (!CreateSwapchains()) return false;
        if (active_vr_bridge_.load(std::memory_order_acquire) && !CreateBlitPipeline())
        {
            DetourModKit::log().warning("V37-BRIDGE: blit pipeline unavailable; falling back to diagnostic clears");
        }
        if (diagnostic_rendering_.load(std::memory_order_acquire))
            DetourModKit::log().warning("DiagnosticRendering=true: submitting purple D3D12 OpenXR eye textures");
        else
            DetourModKit::log().warning("V37-BRIDGE active: submitting KCD2 backbuffer into both OpenXR eyes");
        debug_trace::Write("V46-PRESENT OpenXR composition enabled; mono bridge retained; auto-matrix can cycle stretch/fit/crop/XR-FOV");
        debug_trace::Write("V46-XR-STATE D3D12 color swapchain images are RENDER_TARGET after xrWaitSwapchainImage and must remain RENDER_TARGET at xrReleaseSwapchainImage; no XR-image COMMON transitions");
    }
    else
    {
        DetourModKit::log().info("Tracking-only mode: no OpenXR swapchains or D3D12 command lists will be created");
        debug_trace::Write("tracking-only mode active: zero composition layers, no custom GPU submission");
    }

    bool got_ready = false;
    for (int i = 0; i < 20 && !got_ready; ++i)
    {
        XrEventDataBuffer event{ XR_TYPE_EVENT_DATA_BUFFER };
        while (xrPollEvent(instance_, &event) == XR_SUCCESS)
        {
            if (event.type == XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED)
            {
                auto* state_event = reinterpret_cast<XrEventDataSessionStateChanged*>(&event);
                if (state_event->state == XR_SESSION_STATE_READY)
                    got_ready = true;
                else if (state_event->state == XR_SESSION_STATE_EXITING ||
                         state_event->state == XR_SESSION_STATE_LOSS_PENDING)
                    return false;
            }
            event.type = XR_TYPE_EVENT_DATA_BUFFER;
        }
        if (!got_ready) Sleep(25);
    }

    if (!got_ready)
    {
        DetourModKit::log().error("Timed out waiting for XR_SESSION_STATE_READY");
        return false;
    }

    XrSessionBeginInfo begin_info{ XR_TYPE_SESSION_BEGIN_INFO };
    begin_info.primaryViewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
    XrResult result = xrBeginSession(session_, &begin_info);
    if (XR_FAILED(result))
    {
        DetourModKit::log().error("xrBeginSession failed - code={}", static_cast<int>(result));
        return false;
    }

    if (!input_.Initialize(instance_, session_, local_space_))
    {
        DetourModKit::log().error("OpenXR input initialization failed; continuing with HMD-only VR");
    }

    session_running_.store(true, std::memory_order_relaxed);
    debug_trace::Write("OpenXR session begun successfully");
    DetourModKit::log().info("OpenXR session started (native D3D12 binding; diagnostic rendering={})",
                             diagnostic_rendering_.load(std::memory_order_acquire) ? "on" : "off");
    return true;
}

bool XrManager::ClearD3D12Image(ID3D12Resource* image)
{
    if (!image || !command_allocator_ || !command_list_ || !fence_ || !d3d12_queue_)
        return false;

    if (fence_->GetCompletedValue() < fence_value_)
    {
        if (FAILED(fence_->SetEventOnCompletion(fence_value_, fence_event_)))
            return false;
        if (WaitForSingleObject(fence_event_, INFINITE) != WAIT_OBJECT_0)
            return false;
    }

    HRESULT hr = command_allocator_->Reset();
    if (FAILED(hr))
    {
        DetourModKit::log().error("D3D12 command allocator Reset failed, hr=0x{:08X}", static_cast<unsigned>(hr));
        return false;
    }

    hr = command_list_->Reset(command_allocator_, nullptr);
    if (FAILED(hr))
    {
        DetourModKit::log().error("D3D12 command list Reset failed, hr=0x{:08X}", static_cast<unsigned>(hr));
        return false;
    }

    // V46 / XR_KHR_D3D12_enable: after xrWaitSwapchainImage a color
    // swapchain image is already in RENDER_TARGET state, and the runtime
    // requires it to still be RENDER_TARGET when xrReleaseSwapchainImage is
    // called. Do not transition XR-owned color images through COMMON.
    const float clear_color[4] = { 0.1f, 0.1f, 0.3f, 1.0f };
    command_list_->ClearRenderTargetView(
        // We do not need an RTV descriptor heap: use a CPU descriptor created
        // in a small per-image heap below.
        GetRtvHandle(image), clear_color, 0, nullptr);

    hr = command_list_->Close();
    if (FAILED(hr)) return false;

    ID3D12CommandList* lists[] = { command_list_ };
    if (!DeviceHook::ExecuteCommandListsBypass(d3d12_queue_, 1, lists))
    {
        DetourModKit::log().error("Render-thread D3D12 submission bypass is unavailable");
        return false;
    }

    ++fence_value_;
    hr = d3d12_queue_->Signal(fence_, fence_value_);
    if (FAILED(hr)) return false;

    if (FAILED(fence_->SetEventOnCompletion(fence_value_, fence_event_))) return false;
    return WaitForSingleObject(fence_event_, INFINITE) == WAIT_OBJECT_0;
}


void XrManager::SetFlatUiMode(bool enabled)
{
    // V58.7: F6 is a real world-locked quad.  Capture the COMPLETE head pose
    // in LOCAL space once, not just yaw/pitch, and keep retrying while enabled
    // if startup happened before the first valid OpenXR pose was available.
    const bool previous = flat_ui_mode_.exchange(enabled, std::memory_order_acq_rel);

    if (enabled) {
        bool need_capture = !previous;
        {
            std::lock_guard<std::mutex> lock(flat_ui_mutex_);
            need_capture = need_capture || !flat_ui_anchor_valid_;
        }
        bool captured = false;
        if (need_capture) {
            RenderFrameState f{};
            if (CopyActiveRenderFrameState(f) && f.head_pose_valid) {
                std::lock_guard<std::mutex> lock(flat_ui_mutex_);
                flat_ui_anchor_pose_ = f.head_pose;
                flat_ui_anchor_valid_ = true;
                captured = true;
            }
        }
        if (!previous || captured) {
            char m[192]{};
            std::snprintf(m, sizeof(m), "V58.7-F6-QUAD ON anchor=%s space=LOCAL", captured ? "captured" : "pending");
            debug_trace::Write(m);
        }
    } else if (previous) {
        {
            std::lock_guard<std::mutex> lock(flat_ui_mutex_);
            flat_ui_anchor_valid_ = false;
        }
        debug_trace::Write("V58.7-F6-QUAD OFF");
    }
}

float XrManager::GetRequiredMonoSourceVFovDeg(float source_aspect)
{
    RenderFrameState f{};
    if (!CopyActiveRenderFrameState(f) || !f.stereo_views_valid) return 0.0f;
    source_aspect = std::clamp(source_aspect, 0.5f, 4.0f);
    float max_y = 0.0f;
    float max_x = 0.0f;
    for (int eye = 0; eye < 2; ++eye) {
        const XrFovf& v = f.eye_fov[eye];
        max_x = std::max(max_x, std::max(std::fabs(std::tan(v.angleLeft)), std::fabs(std::tan(v.angleRight))));
        max_y = std::max(max_y, std::max(std::fabs(std::tan(v.angleUp)), std::fabs(std::tan(v.angleDown))));
    }
    const float required_tan_y = std::max(max_y, max_x / source_aspect);
    return 2.0f * std::atan(required_tan_y) * 57.29577951308232f;
}

namespace {
DXGI_FORMAT DepthToSrvFormat(DXGI_FORMAT depth_fmt) noexcept {
    switch (depth_fmt) {
        case DXGI_FORMAT_D32_FLOAT: case DXGI_FORMAT_R32_TYPELESS: return DXGI_FORMAT_R32_FLOAT;
        case DXGI_FORMAT_D24_UNORM_S8_UINT: case DXGI_FORMAT_R24G8_TYPELESS: return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
        case DXGI_FORMAT_D16_UNORM: case DXGI_FORMAT_R16_TYPELESS: return DXGI_FORMAT_R16_UNORM;
        case DXGI_FORMAT_D32_FLOAT_S8X24_UINT: case DXGI_FORMAT_R32G8X24_TYPELESS: return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
        default: return DXGI_FORMAT_UNKNOWN;
    }
}
} // namespace

float XrManager::WriteColorAndDepthDescriptors(ID3D12Resource* color_source, DXGI_FORMAT color_format,
    D3D12_CPU_DESCRIPTOR_HANDLE color_cpu, D3D12_CPU_DESCRIPTOR_HANDLE depth_cpu) noexcept
{
    D3D12_SHADER_RESOURCE_VIEW_DESC csrv{};
    csrv.Format = color_format;
    csrv.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    csrv.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    csrv.Texture2D.MipLevels = 1;
    d3d12_device_->CreateShaderResourceView(color_source, &csrv, color_cpu);

    float weight = 0.0f;
    if (depth_push_enabled_.load(std::memory_order_relaxed)) {
        ID3D12Resource* depth_res = nullptr; DXGI_FORMAT depth_fmt = DXGI_FORMAT_UNKNOWN;
        std::uint64_t dw = 0; std::uint32_t dh = 0;
        if (DeviceHook::GetLiveBestDepthCandidate(&depth_res, &depth_fmt, &dw, &dh) && depth_res) {
            const DXGI_FORMAT srv_fmt = DepthToSrvFormat(depth_fmt);
            bool ok = false;
            if (srv_fmt != DXGI_FORMAT_UNKNOWN) {
                // depth_res is a raw pointer the probe never AddRef'd - it can in
                // principle have been released between the frame it was seen and
                // now. SEH is a best-effort guard, not a guarantee, against that;
                // if this ever proves unsafe in practice, the fix is AddRef'ing
                // in the probe itself, not more guarding here.
                __try {
                    D3D12_SHADER_RESOURCE_VIEW_DESC dsrv{};
                    dsrv.Format = srv_fmt;
                    dsrv.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
                    dsrv.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
                    dsrv.Texture2D.MipLevels = 1;
                    d3d12_device_->CreateShaderResourceView(depth_res, &dsrv, depth_cpu);
                    ok = true;
                } __except (EXCEPTION_EXECUTE_HANDLER) { ok = false; }
            }
            if (ok) {
                weight = depth_push_weight_signed_.load(std::memory_order_relaxed);
            }
        }
    }
    if (weight == 0.0f) {
        // No usable depth this frame (disabled, not yet found, or SRV creation
        // failed) - keep the depth slot bound to something valid so the
        // descriptor table is never left with an uninitialized entry. The
        // shader's DepthPushMod treats weight==0 as an unconditional no-op,
        // so what's actually in this slot is never sampled meaningfully.
        d3d12_device_->CreateShaderResourceView(color_source, &csrv, depth_cpu);
    }
    return weight;
}

bool XrManager::CreateBlitPipeline()
{
    if (!d3d12_device_ || swapchain_format_ == DXGI_FORMAT_UNKNOWN) return false;
    if (blit_pipeline_state_ && blit_root_signature_ && srv_heap_) return true;

    const char* shader = R"(
Texture2D srcTex : register(t0);
Texture2D<float> DepthTex : register(t1);
SamplerState srcSampler : register(s0);
cbuffer PresentParams : register(b0) { float2 uvScale; float2 uvOffset; float4 xrTan; float2 sourceTan; float2 fovFlags; float2 texelSize; float sharpenStrength; float presentPad; float4 poseDeltaQ; float4 reprojParams; float4 colorAdjust; };
struct VSOut { float4 pos : SV_POSITION; float2 uv : TEXCOORD0; };
VSOut VSMain(uint id : SV_VertexID) {
    VSOut o;
    float2 p = id == 0 ? float2(-1.0, -1.0) : (id == 1 ? float2(-1.0, 3.0) : float2(3.0, -1.0));
    o.pos = float4(p, 0.0, 1.0);
    o.uv = float2((p.x + 1.0) * 0.5, 1.0 - (p.y + 1.0) * 0.5);
    return o;
}
float3 QRotateVec(float4 q, float3 v) {
    return v + 2.0 * cross(q.xyz, cross(q.xyz, v) + q.w * v);
}
float DepthPushMod(float2 uv01) {
    // Legacy UV-offset path. Kept as a strict no-op unless a caller actually
    // supplies a UV binocular offset; V61.37 depth stereo uses DepthAwareTans.
    if (presentPad == 0.0) return 1.0;
    // V61.41: uv01 is the normalized (0..1) output coordinate, not a raw pixel
    // position - the depth texture is generally NOT the same resolution as the
    // output eye buffer (it's sized to the source game frame), so indexing it
    // with raw output pixel coordinates only ever hit real data in whatever
    // corner region happened to overlap, reading 0 (Load()'s out-of-range
    // value) everywhere else. texelSize is 1/sourceWidth,1/sourceHeight of the
    // color source; the depth buffer comes from the same render pass so its
    // reciprocal is a reasonable stand-in for depth texture dimensions.
    int2 depthPx = int2(saturate(uv01) * (1.0 / texelSize));
    float rawDepth = DepthTex.Load(int3(depthPx, 0)).r;
    float nearness = saturate(1.0 - rawDepth);
    return lerp(1.0, 0.85, saturate(presentPad) * nearness);
}
float4 DepthAwareTans(float2 uv01) {
    // V61.37 depth mode: poseDeltaQ is repurposed only in FOV-map mode as the
    // cyclopean/common tangent bounds {L,R,D,U}. xrTan carries the real eye
    // tangent bounds. Far pixels stay at full eye asymmetry (1.00); close
    // pixels are pulled toward 0.70 at 100% influence. This is deliberately
    // a small depth-conditioned pseudo disparity, not DIBR/reprojection.
    if (reprojParams.w < 0.5 || presentPad <= 0.0) return xrTan;
    // V61.41: same normalized-coordinate fix as DepthPushMod above - see there
    // for why raw i.pos.xy against the depth texture was wrong.
    int2 depthPx = int2(saturate(uv01) * (1.0 / texelSize));
    float rawDepth = DepthTex.Load(int3(depthPx, 0)).r;
    // KCD2's sampled scene depth is treated as conventional 0=near, 1=far in
    // this first clean implementation; the modulation remains bounded even if
    // the convention proves inverted and can be flipped in a follow-up.
    float nearness = saturate(1.0 - rawDepth);
    float effectiveAsym = 1.0 - 0.30 * saturate(presentPad) * nearness;
    return poseDeltaQ + effectiveAsym * (xrTan - poseDeltaQ);
}
float4 PSMain(VSOut i) : SV_TARGET {
    float2 uv;
    if (fovFlags.x > 1.5) {
        // Flat UI: uvScale > 1 shrinks the source into a centered virtual screen;
        // out-of-range pixels become the black surround below.
        uv = (i.uv - 0.5) * uvScale + 0.5 + uvOffset;
    } else if (fovFlags.x > 0.5) {
        if (reprojParams.x > 1.5) {
            // V60.5 STATIC INTRINSICS HOMOGRAPHY.  No temporal pose delta,
            // no depth, no lens distortion.  This is the exact projective
            // mapping from the asymmetric OpenXR eye frustum into KCD2's
            // symmetric source-camera tangent plane.  Written explicitly as
            // a 3x3 homography so the F8 ladder cannot accidentally fall back
            // to the older zoom/UV/reprojection paths.
            float sx = max(sourceTan.x, 1e-4);
            float sy = max(sourceTan.y, 1e-4);
            float m00 = (xrTan.y - xrTan.x) / (2.0 * sx);
            float m02 = 0.5 + xrTan.x / (2.0 * sx);
            float m11 = -(xrTan.z - xrTan.w) / (2.0 * sy);
            float m12 = 0.5 - xrTan.w / (2.0 * sy);
            float3 srcH = float3(m00 * i.uv.x + m02, m11 * i.uv.y + m12, 1.0);
            uv = srcH.xy / srcH.z;
            uv = (uv - 0.5) * uvScale + 0.5 + uvOffset;
        } else {
            float4 eyeTan = DepthAwareTans(i.uv);
            float tanX = lerp(eyeTan.x, eyeTan.y, i.uv.x);
            float tanY = lerp(eyeTan.w, eyeTan.z, i.uv.y);
            uv.x = 0.5 + tanX / (2.0 * max(sourceTan.x, 1e-4));
            uv.y = 0.5 - tanY / (2.0 * max(sourceTan.y, 1e-4));
            uv = (uv - 0.5) * uvScale + 0.5 + uvOffset;
        }
        // V60.3: make the retained tangent pseudo-stereo UV presets real.
        // Earlier builds stored/logged reprojParams.y but only consumed it in
        // the retired mode-2 reprojection branch, so PS4/PS5 could look equal.
        // V61.35: DepthPushMod is a no-op (returns 1.0) unless a real depth
        // resource is confirmed and bound this frame.
        uv.x += reprojParams.y * DepthPushMod(i.uv);
    } else {
        if (reprojParams.x > 0.5) {
            // V59.1 rotation-only mono reprojection. Build the current output
            // ray from a centered XR frustum, transform it into the camera
            // orientation that actually rendered the captured PRESENT, then
            // project that ray into KCD2's symmetric source camera.
            float tanX = lerp(xrTan.x, xrTan.y, i.uv.x);
            float tanY = lerp(xrTan.w, xrTan.z, i.uv.y);
            float3 ray = normalize(float3(tanX, tanY, -1.0));
            ray = QRotateVec(poseDeltaQ, ray);
            float invForward = 1.0 / max(-ray.z, 1e-4);
            uv.x = 0.5 + (ray.x * invForward) / (2.0 * max(sourceTan.x, 1e-4));
            uv.y = 0.5 - (ray.y * invForward) / (2.0 * max(sourceTan.y, 1e-4));
            uv.x += reprojParams.y * DepthPushMod(i.uv);
            uv = (uv - 0.5) * uvScale + 0.5 + uvOffset;
        } else {
            uv = (i.uv - 0.5) * uvScale + 0.5 + uvOffset;
        }
    }
    // V46 route-identification marker. fovFlags.y = 1 for canonical LEFT
    // texture, 2 for canonical RIGHT texture. It is intentionally drawn in
    // output-eye coordinates BEFORE both black-bar rejection and final OpenXR
    // swapchain routing, so Fit mode cannot hide it and a single final swap
    // must visibly exchange the two markers.
    if (fovFlags.y > 0.5) {
        bool leftMark = fovFlags.y < 1.5;
        bool inside = i.uv.y < 0.08 && (leftMark ? i.uv.x < 0.10 : i.uv.x > 0.90);
        if (inside) return leftMark ? float4(1,0,0,1) : float4(0,1,0,1);
    }

    // V59.2 bottom-edge-only diagnostics. These run AFTER the canonical
    // OpenXR tangent -> source UV mapping and deliberately never touch X.
    // reprojParams.z carries mode. The intent is to preserve the good FOV-110
    // geometry everywhere except the missing lower strip.
    int bottomMode = (int)round(reprojParams.z);
    if (fovFlags.x > 0.5 && bottomMode > 0) {
        if (bottomMode == 1) {
            // Pure vertical translation: trades some upper coverage for lower.
            uv.y -= 0.080;
        } else if (bottomMode == 2) {
            // Mild Y-only compression around center. X is bit-identical.
            uv.y = 0.5 + (uv.y - 0.5) * 0.88;
        } else if (bottomMode == 3 || bottomMode == 4) {
            // Preserve the upper image exactly and compress only the lower tail.
            float threshold = bottomMode == 3 ? 0.80 : 0.90;
            float rawBottom = 0.5 - xrTan.z / (2.0 * max(sourceTan.y, 1e-4));
            if (uv.y > threshold && rawBottom > threshold + 1e-4) {
                uv.y = threshold + (uv.y - threshold) * ((1.0 - threshold) / (rawBottom - threshold));
            }
        } else if (bottomMode == 5) {
            // Edge-hold only for the lower edge. No X edge hold and no upper hold.
            if (uv.y > 1.0) uv.y = 1.0;
        }
    }

    // V58.7 Crysis-style crop: never globally shrink the whole image and
    // never stretch/edge-hold missing peripheral pixels. A tiny epsilon is
    // clamped only for floating-point seam noise; real uncovered rays are black.
    // The startup F6 coverage guard keeps immersive mode disabled until the
    // published KCD2 source frustum can cover the current OpenXR views.
    const float seamEps = 0.0015;
    if (uv.x < -seamEps || uv.x > 1.0 + seamEps || uv.y < -seamEps || uv.y > 1.0 + seamEps) {
        return float4(0,0,0,1);
    }
    uv = clamp(uv, 0.0, 1.0);
    float4 c = srcTex.SampleLevel(srcSampler, uv, 0);
    if (sharpenStrength > 0.0001) {
        float2 dx = float2(texelSize.x, 0.0);
        float2 dy = float2(0.0, texelSize.y);
        float3 l = srcTex.SampleLevel(srcSampler, uv - dx, 0).rgb;
        float3 r = srcTex.SampleLevel(srcSampler, uv + dx, 0).rgb;
        float3 u = srcTex.SampleLevel(srcSampler, uv - dy, 0).rgb;
        float3 d = srcTex.SampleLevel(srcSampler, uv + dy, 0).rgb;
        float k = saturate(sharpenStrength) * 0.35;
        c.rgb = saturate(c.rgb + k * (4.0 * c.rgb - l - r - u - d));
    }
    // V62.6: crash-safe VR-output color adjustment. The KCD2 renderer remains untouched.
    float gammaSafe = max(colorAdjust.x, 0.05);
    float3 adjusted = (c.rgb - 0.5) * colorAdjust.z + 0.5 + colorAdjust.y;
    adjusted = saturate(adjusted);
    adjusted = pow(max(adjusted, 0.0), 1.0 / gammaSafe);
    c.rgb = saturate(adjusted);
    return c;
}
)";

    ID3DBlob* vs = nullptr; ID3DBlob* ps = nullptr; ID3DBlob* errors = nullptr;
    HRESULT hr = D3DCompile(shader, std::strlen(shader), "KCD2VR_V38", nullptr, nullptr,
                            "VSMain", "vs_5_0", D3DCOMPILE_OPTIMIZATION_LEVEL3, 0, &vs, &errors);
    if (FAILED(hr)) {
        if (errors) { DetourModKit::log().error("V38-PRESENT VS compile failed: {}", static_cast<const char*>(errors->GetBufferPointer())); errors->Release(); }
        return false;
    }
    if (errors) { errors->Release(); errors = nullptr; }
    hr = D3DCompile(shader, std::strlen(shader), "KCD2VR_V38", nullptr, nullptr,
                    "PSMain", "ps_5_0", D3DCOMPILE_OPTIMIZATION_LEVEL3, 0, &ps, &errors);
    if (FAILED(hr)) {
        if (errors) { DetourModKit::log().error("V38-PRESENT PS compile failed: {}", static_cast<const char*>(errors->GetBufferPointer())); errors->Release(); }
        vs->Release(); return false;
    }
    if (errors) errors->Release();

    D3D12_DESCRIPTOR_RANGE range{};
    range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    range.NumDescriptors = 2; // V61.35: t0=color source, t1=depth (DepthPushMod no-ops if unbound/invalid)
    range.BaseShaderRegister = 0;
    range.RegisterSpace = 0;
    range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    D3D12_ROOT_PARAMETER params[2]{};
    params[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    params[0].DescriptorTable.NumDescriptorRanges = 1;
    params[0].DescriptorTable.pDescriptorRanges = &range;
    params[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    params[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
    params[1].Constants.ShaderRegister = 0;
    params[1].Constants.RegisterSpace = 0;
    params[1].Constants.Num32BitValues = 28;
    params[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    D3D12_STATIC_SAMPLER_DESC sampler{};
    sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    sampler.AddressU = sampler.AddressV = sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    sampler.MipLODBias = 0.0f;
    sampler.MaxAnisotropy = 1;
    sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
    sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
    sampler.MinLOD = 0.0f;
    sampler.MaxLOD = D3D12_FLOAT32_MAX;
    sampler.ShaderRegister = 0;
    sampler.RegisterSpace = 0;
    sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    D3D12_ROOT_SIGNATURE_DESC rs_desc{};
    rs_desc.NumParameters = 2;
    rs_desc.pParameters = params;
    rs_desc.NumStaticSamplers = 1;
    rs_desc.pStaticSamplers = &sampler;
    rs_desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    ID3DBlob* rs_blob = nullptr; ID3DBlob* rs_err = nullptr;
    hr = D3D12SerializeRootSignature(&rs_desc, D3D_ROOT_SIGNATURE_VERSION_1, &rs_blob, &rs_err);
    if (FAILED(hr)) {
        if (rs_err) rs_err->Release(); vs->Release(); ps->Release(); return false;
    }
    if (rs_err) rs_err->Release();
    hr = d3d12_device_->CreateRootSignature(0, rs_blob->GetBufferPointer(), rs_blob->GetBufferSize(), IID_PPV_ARGS(&blit_root_signature_));
    rs_blob->Release();
    if (FAILED(hr)) { vs->Release(); ps->Release(); return false; }

    D3D12_GRAPHICS_PIPELINE_STATE_DESC pso{};
    pso.pRootSignature = blit_root_signature_;
    pso.VS = { vs->GetBufferPointer(), vs->GetBufferSize() };
    pso.PS = { ps->GetBufferPointer(), ps->GetBufferSize() };
    pso.BlendState.AlphaToCoverageEnable = FALSE;
    pso.BlendState.IndependentBlendEnable = FALSE;
    for (auto& rt : pso.BlendState.RenderTarget) {
        rt.BlendEnable = FALSE; rt.LogicOpEnable = FALSE;
        rt.SrcBlend = D3D12_BLEND_ONE; rt.DestBlend = D3D12_BLEND_ZERO; rt.BlendOp = D3D12_BLEND_OP_ADD;
        rt.SrcBlendAlpha = D3D12_BLEND_ONE; rt.DestBlendAlpha = D3D12_BLEND_ZERO; rt.BlendOpAlpha = D3D12_BLEND_OP_ADD;
        rt.LogicOp = D3D12_LOGIC_OP_NOOP; rt.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
    }
    pso.SampleMask = UINT_MAX;
    pso.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
    pso.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
    pso.RasterizerState.FrontCounterClockwise = FALSE;
    pso.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
    pso.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
    pso.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
    pso.RasterizerState.DepthClipEnable = TRUE;
    pso.RasterizerState.MultisampleEnable = FALSE;
    pso.RasterizerState.AntialiasedLineEnable = FALSE;
    pso.RasterizerState.ForcedSampleCount = 0;
    pso.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
    pso.DepthStencilState.DepthEnable = FALSE;
    pso.DepthStencilState.StencilEnable = FALSE;
    pso.InputLayout = { nullptr, 0 };
    pso.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pso.NumRenderTargets = 1;
    pso.RTVFormats[0] = swapchain_format_;
    pso.SampleDesc.Count = 1;
    pso.SampleDesc.Quality = 0;
    hr = d3d12_device_->CreateGraphicsPipelineState(&pso, IID_PPV_ARGS(&blit_pipeline_state_));
    vs->Release(); ps->Release();
    if (FAILED(hr)) return false;

    D3D12_DESCRIPTOR_HEAP_DESC heap{};
    heap.NumDescriptors = 4; // V61.35: 2 slots (color,depth) per eye x 2 eyes for AFR; mono uses only the first pair
    heap.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    heap.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    hr = d3d12_device_->CreateDescriptorHeap(&heap, IID_PPV_ARGS(&srv_heap_));
    if (FAILED(hr)) return false;
    srv_descriptor_size_ = d3d12_device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    debug_trace::Write("V59.6-PRESENT BUILD=SQUARE-STEREO-PROBE-20260905-B square mono baseline + scene-color native-stereo archaeology");
    return true;
}

static DXGI_FORMAT Kcd2VrSrvFormat(DXGI_FORMAT fmt)
{
    switch (fmt) {
        case DXGI_FORMAT_R8G8B8A8_TYPELESS: return DXGI_FORMAT_R8G8B8A8_UNORM;
        case DXGI_FORMAT_B8G8R8A8_TYPELESS: return DXGI_FORMAT_B8G8R8A8_UNORM;
        case DXGI_FORMAT_R10G10B10A2_TYPELESS: return DXGI_FORMAT_R10G10B10A2_UNORM;
        default: return fmt;
    }
}


void XrManager::DestroyAfrCaches()
{
    for (int eye = 0; eye < 2; ++eye) {
        afr_cache_valid_[eye] = false;
        if (afr_cache_[eye]) { afr_cache_[eye]->Release(); afr_cache_[eye] = nullptr; }
    }
    afr_cache_desc_ = {};
    afr_last_present_serial_ = 0;
}

bool XrManager::EnsureAfrCaches(const D3D12_RESOURCE_DESC& source_desc)
{
    if (!d3d12_device_ || source_desc.Dimension != D3D12_RESOURCE_DIMENSION_TEXTURE2D ||
        source_desc.SampleDesc.Count != 1 || source_desc.Width == 0 || source_desc.Height == 0)
        return false;

    const bool same = afr_cache_[0] && afr_cache_[1] &&
        afr_cache_desc_.Width == source_desc.Width && afr_cache_desc_.Height == source_desc.Height &&
        afr_cache_desc_.Format == source_desc.Format && afr_cache_desc_.SampleDesc.Count == source_desc.SampleDesc.Count;
    if (same) return true;

    DestroyAfrCaches();
    D3D12_RESOURCE_DESC desc = source_desc;
    desc.Flags = D3D12_RESOURCE_FLAG_NONE;
    desc.MipLevels = 1;
    D3D12_HEAP_PROPERTIES heap{};
    heap.Type = D3D12_HEAP_TYPE_DEFAULT;
    heap.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heap.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    heap.CreationNodeMask = 1;
    heap.VisibleNodeMask = 1;

    for (int eye = 0; eye < 2; ++eye) {
        HRESULT hr = d3d12_device_->CreateCommittedResource(
            &heap, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_COMMON,
            nullptr, IID_PPV_ARGS(&afr_cache_[eye]));
        if (FAILED(hr)) {
            DestroyAfrCaches();
            return false;
        }
    }
    afr_cache_desc_ = desc;
    char msg[256]{};
    std::snprintf(msg, sizeof(msg), "V39-AFR cache-created %llux%u fmt=%d",
        static_cast<unsigned long long>(desc.Width), desc.Height, static_cast<int>(desc.Format));
    debug_trace::Write(msg);
    return true;
}

bool XrManager::BlitAfrStereoToEyes(ID3D12Resource* source, int source_eye, std::uint64_t serial,
                                     ID3D12Resource* left, ID3D12Resource* right)
{
    if (!source || !left || !right || !command_allocator_ || !command_list_ || !fence_ ||
        !d3d12_queue_ || !blit_pipeline_state_ || !blit_root_signature_ || !srv_heap_) return false;
    source_eye &= 1;
    const D3D12_RESOURCE_DESC src_desc = source->GetDesc();
    if (!EnsureAfrCaches(src_desc)) return false;

    const int presentation_mode = presentation_mode_.load(std::memory_order_acquire);
    const float presentation_zoom = presentation_zoom_.load(std::memory_order_acquire);
    const float source_vertical_fov_deg = source_vertical_fov_deg_.load(std::memory_order_acquire);
    const bool angular_correct_mono = angular_correct_mono_.load(std::memory_order_acquire);
    const int uv_reproj_mode = mono_uv_reprojection_mode_.load(std::memory_order_acquire);
    const float eye_offset_x[2] = {
        eye_offset_x_[0].load(std::memory_order_acquire), eye_offset_x_[1].load(std::memory_order_acquire) };
    const float eye_offset_y[2] = {
        eye_offset_y_[0].load(std::memory_order_acquire), eye_offset_y_[1].load(std::memory_order_acquire) };

    if (fence_->GetCompletedValue() < fence_value_) {
        if (FAILED(fence_->SetEventOnCompletion(fence_value_, fence_event_))) return false;
        if (WaitForSingleObject(fence_event_, 1000) != WAIT_OBJECT_0) return false;
    }
    HRESULT hr = command_allocator_->Reset(); if (FAILED(hr)) return false;
    hr = command_list_->Reset(command_allocator_, blit_pipeline_state_); if (FAILED(hr)) return false;

    const bool new_frame = serial != 0 && serial != afr_last_present_serial_;
    if (new_frame) {
        D3D12_RESOURCE_BARRIER copy_begin[2]{};
        copy_begin[0].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        copy_begin[0].Transition.pResource = source;
        copy_begin[0].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        copy_begin[0].Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
        copy_begin[0].Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;
        copy_begin[1].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        copy_begin[1].Transition.pResource = afr_cache_[source_eye];
        copy_begin[1].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        copy_begin[1].Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
        copy_begin[1].Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
        command_list_->ResourceBarrier(2, copy_begin);
        command_list_->CopyResource(afr_cache_[source_eye], source);
        D3D12_RESOURCE_BARRIER copy_end[2] = { copy_begin[0], copy_begin[1] };
        copy_end[0].Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
        copy_end[0].Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
        copy_end[1].Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
        copy_end[1].Transition.StateAfter = D3D12_RESOURCE_STATE_COMMON;
        command_list_->ResourceBarrier(2, copy_end);
    }

    ID3D12Resource* eye_sources[2] = {
        afr_cache_valid_[0] ? afr_cache_[0] : afr_cache_[source_eye],
        afr_cache_valid_[1] ? afr_cache_[1] : afr_cache_[source_eye]
    };
    if (new_frame) eye_sources[source_eye] = afr_cache_[source_eye];

    // V46: cached AFR textures are ours and start in COMMON, so transition
    // only those to SRV. OpenXR color swapchain images are already guaranteed
    // RENDER_TARGET after xrWaitSwapchainImage and must remain RENDER_TARGET
    // until xrReleaseSwapchainImage.
    D3D12_RESOURCE_BARRIER begin[2]{};
    UINT begin_count = 0;
    for (int eye = 0; eye < 2; ++eye) {
        if (eye == 1 && eye_sources[1] == eye_sources[0]) continue;
        auto& b = begin[begin_count++];
        b.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        b.Transition.pResource = eye_sources[eye];
        b.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        b.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
        b.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    }
    ID3D12Resource* dsts[2] = { left, right }; // canonical L->L, R->R; swap only at final XR layer
    bool used_openxr_fov_this_blit = false;
    if (begin_count) command_list_->ResourceBarrier(begin_count, begin);

    ID3D12DescriptorHeap* heaps[] = { srv_heap_ };
    command_list_->SetDescriptorHeaps(1, heaps);
    command_list_->SetGraphicsRootSignature(blit_root_signature_);
    command_list_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    for (int eye = 0; eye < 2; ++eye) {
        const D3D12_RESOURCE_DESC cache_desc = eye_sources[eye]->GetDesc();
        // V61.35: 2 contiguous heap slots per eye now (color, depth) instead
        // of 1, so eye 0 -> slots[0,1], eye 1 -> slots[2,3].
        D3D12_CPU_DESCRIPTOR_HANDLE color_cpu = srv_heap_->GetCPUDescriptorHandleForHeapStart();
        color_cpu.ptr += static_cast<SIZE_T>(eye) * 2 * srv_descriptor_size_;
        D3D12_CPU_DESCRIPTOR_HANDLE depth_cpu = color_cpu;
        depth_cpu.ptr += static_cast<SIZE_T>(srv_descriptor_size_);
        const float depth_weight = WriteColorAndDepthDescriptors(
            eye_sources[eye], Kcd2VrSrvFormat(cache_desc.Format), color_cpu, depth_cpu);
        D3D12_GPU_DESCRIPTOR_HANDLE srv_gpu = srv_heap_->GetGPUDescriptorHandleForHeapStart();
        srv_gpu.ptr += static_cast<UINT64>(eye) * 2 * srv_descriptor_size_;
        command_list_->SetGraphicsRootDescriptorTable(0, srv_gpu);

        D3D12_VIEWPORT vp{};
        vp.Width = static_cast<float>(eyes_[eye].width); vp.Height = static_cast<float>(eyes_[eye].height);
        vp.MinDepth = 0.0f; vp.MaxDepth = 1.0f;
        D3D12_RECT sc{0, 0, eyes_[eye].width, eyes_[eye].height};
        command_list_->RSSetViewports(1, &vp); command_list_->RSSetScissorRects(1, &sc);
        const auto rtv = GetRtvHandle(dsts[eye]);
        command_list_->OMSetRenderTargets(1, &rtv, FALSE, nullptr);

        const float src_aspect = static_cast<float>(cache_desc.Width) / static_cast<float>(cache_desc.Height);
        const float dst_aspect = static_cast<float>(eyes_[eye].width) / static_cast<float>(eyes_[eye].height);
        float sx = 1.0f, sy = 1.0f;
        if (presentation_mode == 1) { if (src_aspect > dst_aspect) sy = src_aspect / dst_aspect; else sx = dst_aspect / src_aspect; }
        else if (presentation_mode == 2) { if (src_aspect > dst_aspect) sx = dst_aspect / src_aspect; else sy = src_aspect / dst_aspect; }
        const float zoom = presentation_zoom < 0.25f ? 0.25f : (presentation_zoom > 4.0f ? 4.0f : presentation_zoom);
        RenderFrameState frame_snapshot{};
        const bool have_frame = CopyActiveRenderFrameState(frame_snapshot);
        const bool flat_ui = flat_ui_mode_.load(std::memory_order_acquire);
        const bool have_fov = !flat_ui && presentation_mode == 3 && have_frame && frame_snapshot.stereo_views_valid;
        const float vfov_deg = source_vertical_fov_deg < 20.0f ? 20.0f : (source_vertical_fov_deg > 160.0f ? 160.0f : source_vertical_fov_deg);
        const float source_tan_y = std::tan(vfov_deg * 0.008726646259971648f);
        const float source_tan_x = source_tan_y * src_aspect;
        const bool marker = presentation_test_marker_.load(std::memory_order_acquire);
        const float sharp = std::clamp(sharpening_strength_.load(std::memory_order_acquire), 0.0f, 0.75f);
        const float common_center_y = presentation_center_y_.load(std::memory_order_acquire);
        float present[28] = { sx / zoom, sy / zoom, eye_offset_x[eye], eye_offset_y[eye] + common_center_y, 0,0,0,0, source_tan_x, source_tan_y, have_fov ? 1.0f : 0.0f, marker ? static_cast<float>(eye + 1) : 0.0f,
                              1.0f / static_cast<float>(std::max<UINT64>(1, cache_desc.Width)), 1.0f / static_cast<float>(std::max<UINT>(1, cache_desc.Height)), sharp, 0.0f };
        present[15] = depth_weight; // V61.35: presentPad now carries depthPushWeight (see WriteColorAndDepthDescriptors)
        present[24] = output_gamma_.load(std::memory_order_acquire);
        present[25] = output_brightness_.load(std::memory_order_acquire);
        present[26] = output_contrast_.load(std::memory_order_acquire);
        present[27] = 0.0f;
        if (flat_ui) {
            // V58.7: normalized source copy for a real LOCAL-space quad.
            // No head-relative counter-shift, FOV warp, or baked letterboxing.
            present[0] = 1.0f;
            present[1] = 1.0f;
            present[2] = 0.0f;
            present[3] = 0.0f;
            present[10] = 0.0f;
        }
        if (have_fov && !flat_ui) {
            used_openxr_fov_this_blit = true;
            // V60.7 diagnostic mode 4: use the same asymmetry-adjusted FOV for
            // source-ray mapping that the projection layer will submit to OpenXR.
            // This is a static intrinsics consistency test only -- no pose delta.
            XrFovf mapped_fov = frame_snapshot.eye_fov[eye];
            if (uv_reproj_mode == 4) {
                XrFovf cf{};
                cf.angleLeft  = 0.5f * (frame_snapshot.eye_fov[0].angleLeft  + frame_snapshot.eye_fov[1].angleLeft);
                cf.angleRight = 0.5f * (frame_snapshot.eye_fov[0].angleRight + frame_snapshot.eye_fov[1].angleRight);
                cf.angleUp    = 0.5f * (frame_snapshot.eye_fov[0].angleUp    + frame_snapshot.eye_fov[1].angleUp);
                cf.angleDown  = 0.5f * (frame_snapshot.eye_fov[0].angleDown  + frame_snapshot.eye_fov[1].angleDown);
                const bool depth_asym = depth_push_enabled_.load(std::memory_order_acquire);
                const float asym = depth_asym ? 1.0f : mono_fov_asymmetry_scale_.load(std::memory_order_acquire);
                const XrFovf& rf = frame_snapshot.eye_fov[eye];
                mapped_fov.angleLeft  = cf.angleLeft  + asym * (rf.angleLeft  - cf.angleLeft);
                mapped_fov.angleRight = cf.angleRight + asym * (rf.angleRight - cf.angleRight);
                mapped_fov.angleUp    = cf.angleUp    + asym * (rf.angleUp    - cf.angleUp);
                mapped_fov.angleDown  = cf.angleDown  + asym * (rf.angleDown  - cf.angleDown);
                if (depth_asym) {
                    // In FOV-map mode poseDeltaQ is otherwise unused. Pass the
                    // common/cyclopean tangent bounds to the pixel shader so it
                    // can interpolate eye asymmetry per pixel from depth.
                    present[16] = std::tan(cf.angleLeft);
                    present[17] = std::tan(cf.angleRight);
                    present[18] = std::tan(cf.angleDown);
                    present[19] = std::tan(cf.angleUp);
                    present[23] = 1.0f;
                }
            }
            const XrFovf& f = mapped_fov;
            present[4] = std::tan(f.angleLeft);
            present[5] = std::tan(f.angleRight);
            present[6] = std::tan(f.angleDown);
            present[7] = std::tan(f.angleUp);
            const float max_abs_x = std::max(std::fabs(present[4]), std::fabs(present[5]));
            const float max_abs_y = std::max(std::fabs(present[6]), std::fabs(present[7]));
            const float max_scale_x = source_tan_x > 1e-4f && max_abs_x > 1e-4f ? (source_tan_x / max_abs_x) : 1.0f;
            const float max_scale_y = source_tan_y > 1e-4f && max_abs_y > 1e-4f ? (source_tan_y / max_abs_y) : 1.0f;
            const float requested_scale = 1.0f / zoom;
            const float coverage_limit = 0.995f * std::min(max_scale_x, max_scale_y);
            // V58.7: CROP mode preserves the requested angular scale exactly.
            // F5 retains the old global coverage clamp only as an A/B fallback.
            const float safe_scale = angular_correct_mono ? requested_scale : std::min(requested_scale, coverage_limit);
            present[0] = safe_scale;
            present[1] = safe_scale;
        }
        command_list_->SetGraphicsRoot32BitConstants(1, 28, present, 0);
        command_list_->DrawInstanced(3, 1, 0, 0);
    }

    D3D12_RESOURCE_BARRIER end[2]{};
    UINT end_count = 0;
    for (int eye = 0; eye < 2; ++eye) {
        if (eye == 1 && eye_sources[1] == eye_sources[0]) continue;
        auto& b = end[end_count++];
        b.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        b.Transition.pResource = eye_sources[eye];
        b.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        b.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
        b.Transition.StateAfter = D3D12_RESOURCE_STATE_COMMON;
    }
    if (end_count) command_list_->ResourceBarrier(end_count, end);
    hr = command_list_->Close(); if (FAILED(hr)) return false;
    ID3D12CommandList* lists[] = { command_list_ };
    if (!DeviceHook::ExecuteCommandListsBypass(d3d12_queue_, 1, lists)) return false;
    ++fence_value_;
    if (FAILED(d3d12_queue_->Signal(fence_, fence_value_))) return false;
    if (FAILED(fence_->SetEventOnCompletion(fence_value_, fence_event_))) return false;
    if (WaitForSingleObject(fence_event_, 1000) != WAIT_OBJECT_0) return false;

    if (new_frame) {
        afr_cache_valid_[source_eye] = true;
        afr_last_present_serial_ = serial;
    }
    test_afr_blits_.fetch_add(1, std::memory_order_relaxed);
    if (used_openxr_fov_this_blit) test_fov_blits_.fetch_add(1, std::memory_order_relaxed);
    static std::atomic<unsigned long long> afr_blits{0};
    const auto n = afr_blits.fetch_add(1, std::memory_order_relaxed) + 1;
    if (n <= 12 || (n % 300) == 0) {
        char msg[320]{};
        std::snprintf(msg, sizeof(msg), "V46.5-AFR-BLIT call=%llu serial=%llu capturedEye=%c cacheValid=%d%d src=%llux%u -> eyes=%dx%d xrFov=%s",
            n, static_cast<unsigned long long>(serial), source_eye == 0 ? 'L' : 'R',
            afr_cache_valid_[0] ? 1 : 0, afr_cache_valid_[1] ? 1 : 0,
            static_cast<unsigned long long>(src_desc.Width), src_desc.Height, eyes_[0].width, eyes_[0].height, used_openxr_fov_this_blit ? "true" : "false");
        debug_trace::Write(msg);
    }
    return true;
}

bool XrManager::BlitGameImageToEyes(ID3D12Resource* source, ID3D12Resource* left, ID3D12Resource* right)
{
    if (!source || !left || !right || !command_allocator_ || !command_list_ || !fence_ ||
        !d3d12_queue_ || !blit_pipeline_state_ || !blit_root_signature_ || !srv_heap_) return false;

    const D3D12_RESOURCE_DESC src_desc = source->GetDesc();
    if (src_desc.Dimension != D3D12_RESOURCE_DIMENSION_TEXTURE2D || src_desc.SampleDesc.Count != 1) return false;
    last_source_width_.store(static_cast<std::uint32_t>(src_desc.Width),std::memory_order_release);
    last_source_height_.store(static_cast<std::uint32_t>(src_desc.Height),std::memory_order_release);
    const int presentation_mode = presentation_mode_.load(std::memory_order_acquire);
    const float presentation_zoom = presentation_zoom_.load(std::memory_order_acquire);
    const float source_vertical_fov_deg = source_vertical_fov_deg_.load(std::memory_order_acquire);
    const bool angular_correct_mono = angular_correct_mono_.load(std::memory_order_acquire);
    const float eye_offset_x[2] = {
        eye_offset_x_[0].load(std::memory_order_acquire), eye_offset_x_[1].load(std::memory_order_acquire) };
    const float eye_offset_y[2] = {
        eye_offset_y_[0].load(std::memory_order_acquire), eye_offset_y_[1].load(std::memory_order_acquire) };
    const DXGI_FORMAT srv_format = Kcd2VrSrvFormat(src_desc.Format);

    // V58.6: make the real quality bottleneck explicit. XR target resolution
    // can be much larger than the single captured KCD2 PRESENT source; raising
    // only VrRenderScale cannot reconstruct source detail. Log whenever the
    // captured source dimensions change.
    {
        static std::atomic<unsigned long long> last_size_key{0};
        const unsigned long long key = (static_cast<unsigned long long>(src_desc.Width) << 32) | static_cast<unsigned long long>(src_desc.Height);
        const auto prev = last_size_key.exchange(key, std::memory_order_acq_rel);
        if (prev != key) {
            const double raw_x = static_cast<double>(eyes_[0].width) / static_cast<double>(std::max<UINT64>(1, src_desc.Width));
            const double raw_y = static_cast<double>(eyes_[0].height) / static_cast<double>(std::max<UINT>(1, src_desc.Height));
            char q[384]{};
            std::snprintf(q,sizeof(q),
                "V58.6-SOURCE-QUALITY capturedPRESENT=%llux%u -> XR-eye=%dx%d rawDimScale=(%.3fx,%.3fx) note=VrRenderScale changes XR target only; source detail remains bounded by captured PRESENT",
                static_cast<unsigned long long>(src_desc.Width), src_desc.Height, eyes_[0].width, eyes_[0].height, raw_x, raw_y);
            debug_trace::Write(q);
        }
    }

    if (fence_->GetCompletedValue() < fence_value_) {
        if (FAILED(fence_->SetEventOnCompletion(fence_value_, fence_event_))) return false;
        if (WaitForSingleObject(fence_event_, 1000) != WAIT_OBJECT_0) return false;
    }
    HRESULT hr = command_allocator_->Reset(); if (FAILED(hr)) return false;
    hr = command_list_->Reset(command_allocator_, blit_pipeline_state_); if (FAILED(hr)) return false;

    D3D12_CPU_DESCRIPTOR_HANDLE mono_color_cpu = srv_heap_->GetCPUDescriptorHandleForHeapStart();
    D3D12_CPU_DESCRIPTOR_HANDLE mono_depth_cpu = mono_color_cpu;
    mono_depth_cpu.ptr += static_cast<SIZE_T>(srv_descriptor_size_);
    const float mono_depth_weight = WriteColorAndDepthDescriptors(source, srv_format, mono_color_cpu, mono_depth_cpu);

    D3D12_RESOURCE_BARRIER source_to_srv{};
    source_to_srv.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    source_to_srv.Transition.pResource = source;
    source_to_srv.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    source_to_srv.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    source_to_srv.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    command_list_->ResourceBarrier(1, &source_to_srv);

    ID3D12DescriptorHeap* heaps[] = { srv_heap_ };
    command_list_->SetDescriptorHeaps(1, heaps);
    command_list_->SetGraphicsRootSignature(blit_root_signature_);
    command_list_->SetGraphicsRootDescriptorTable(0, srv_heap_->GetGPUDescriptorHandleForHeapStart());
    command_list_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    ID3D12Resource* dsts[2] = { left, right };
    bool used_openxr_fov_this_blit = false;

    // V59.1: derive the rotation from the newest XR output pose back into
    // the pose that rendered the PRESENT image. Translation is intentionally
    // not reprojected: a color-only mono frame has no depth with which to do it.
    const int uv_reproj_mode = mono_uv_reprojection_mode_.load(std::memory_order_acquire);
    const float uv_binocular = mono_uv_binocular_offset_.load(std::memory_order_acquire);
    const int bottom_edge_mode = mono_bottom_edge_mode_.load(std::memory_order_acquire);
    RenderFrameState reproj_current{}, reproj_render{};
    XrQuaternionf reproj_q{0.0f,0.0f,0.0f,1.0f};
    XrFovf reproj_common_fov{};
    bool reproj_available = false;
    if (uv_reproj_mode > 0 && presentation_mode == 2 && CopyActiveRenderFrameState(reproj_current) && reproj_current.head_pose_valid && reproj_current.stereo_views_valid) {
        const std::uint64_t render_frame = last_submitted_camera_frame_.load(std::memory_order_acquire);
        if (CopyRenderFrameStateByIndex(render_frame, reproj_render) && reproj_render.head_pose_valid) {
            reproj_q = (uv_reproj_mode == 2)
                ? QMul(QConj(reproj_current.head_pose.orientation), reproj_render.head_pose.orientation)
                : QMul(QConj(reproj_render.head_pose.orientation), reproj_current.head_pose.orientation);
            reproj_common_fov.angleLeft  = 0.5f * (reproj_current.eye_fov[0].angleLeft  + reproj_current.eye_fov[1].angleLeft);
            reproj_common_fov.angleRight = 0.5f * (reproj_current.eye_fov[0].angleRight + reproj_current.eye_fov[1].angleRight);
            reproj_common_fov.angleUp    = 0.5f * (reproj_current.eye_fov[0].angleUp    + reproj_current.eye_fov[1].angleUp);
            reproj_common_fov.angleDown  = 0.5f * (reproj_current.eye_fov[0].angleDown  + reproj_current.eye_fov[1].angleDown);
            reproj_available = true;
        }
    }

    float safe_uv_scale_eye[2] = { 1.0f, 1.0f };
    float requested_uv_scale_eye[2] = { 1.0f, 1.0f };
    bool coverage_clamped_eye[2] = { false, false };
    const bool marker_enabled_this_blit = presentation_test_marker_.load(std::memory_order_acquire);
    for (int eye = 0; eye < 2; ++eye) {
        D3D12_VIEWPORT vp{};
        vp.Width = static_cast<float>(eyes_[eye].width); vp.Height = static_cast<float>(eyes_[eye].height);
        vp.MinDepth = 0.0f; vp.MaxDepth = 1.0f;
        D3D12_RECT sc{0, 0, eyes_[eye].width, eyes_[eye].height};
        command_list_->RSSetViewports(1, &vp); command_list_->RSSetScissorRects(1, &sc);
        const auto rtv = GetRtvHandle(dsts[eye]);
        command_list_->OMSetRenderTargets(1, &rtv, FALSE, nullptr);

        const float src_aspect = static_cast<float>(src_desc.Width) / static_cast<float>(src_desc.Height);
        const float dst_aspect = static_cast<float>(eyes_[eye].width) / static_cast<float>(eyes_[eye].height);
        float sx = 1.0f, sy = 1.0f;
        if (presentation_mode == 1) { // fit: preserve whole image, black bars where needed
            if (src_aspect > dst_aspect) sy = src_aspect / dst_aspect;
            else sx = dst_aspect / src_aspect;
        } else if (presentation_mode == 2) { // crop: fill eye without stretching
            if (src_aspect > dst_aspect) sx = dst_aspect / src_aspect;
            else sy = src_aspect / dst_aspect;
        }
        const float zoom = presentation_zoom < 0.25f ? 0.25f : (presentation_zoom > 4.0f ? 4.0f : presentation_zoom);
        RenderFrameState frame_snapshot{};
        const bool have_frame = CopyActiveRenderFrameState(frame_snapshot);
        const bool flat_ui = flat_ui_mode_.load(std::memory_order_acquire);
        const bool have_fov = !flat_ui && presentation_mode == 3 && have_frame && frame_snapshot.stereo_views_valid;
        const float vfov_deg = source_vertical_fov_deg < 20.0f ? 20.0f : (source_vertical_fov_deg > 160.0f ? 160.0f : source_vertical_fov_deg);
        const float source_tan_y = std::tan(vfov_deg * 0.008726646259971648f);
        const float source_tan_x = source_tan_y * src_aspect;
        const float sharp = std::clamp(sharpening_strength_.load(std::memory_order_acquire), 0.0f, 0.75f);
        const float common_center_y = presentation_center_y_.load(std::memory_order_acquire);
        float present[28] = { sx / zoom, sy / zoom, eye_offset_x[eye], eye_offset_y[eye] + common_center_y, 0,0,0,0, source_tan_x, source_tan_y, have_fov ? 1.0f : 0.0f, marker_enabled_this_blit ? static_cast<float>(eye + 1) : 0.0f,
                              1.0f / static_cast<float>(std::max<UINT64>(1, src_desc.Width)), 1.0f / static_cast<float>(std::max<UINT>(1, src_desc.Height)), sharp, 0.0f,
                              0,0,0,1, 0,(eye == 0 ? -uv_binocular : uv_binocular), static_cast<float>(bottom_edge_mode),0 };
        present[15] = mono_depth_weight; // V61.35: presentPad now carries depthPushWeight
        present[24] = output_gamma_.load(std::memory_order_acquire);
        present[25] = output_brightness_.load(std::memory_order_acquire);
        present[26] = output_contrast_.load(std::memory_order_acquire);
        present[27] = 0.0f;
        // V60.5 mode 3 is a static source->XR intrinsics homography.
        // It is intentionally independent of render/current pose availability.
        if (uv_reproj_mode == 3 && !flat_ui && presentation_mode == 3) {
            present[20] = 2.0f;
        }
        if (reproj_available && !flat_ui && presentation_mode == 2) {
            present[4] = std::tan(reproj_common_fov.angleLeft);
            present[5] = std::tan(reproj_common_fov.angleRight);
            present[6] = std::tan(reproj_common_fov.angleDown);
            present[7] = std::tan(reproj_common_fov.angleUp);
            present[16] = reproj_q.x; present[17] = reproj_q.y; present[18] = reproj_q.z; present[19] = reproj_q.w;
            present[20] = 1.0f;
            present[21] = (eye == 0 ? -uv_binocular : uv_binocular);
        }
        if (flat_ui) {
            // V58.7: normalized source copy for a real LOCAL-space quad.
            // No head-relative counter-shift, FOV warp, or baked letterboxing.
            present[0] = 1.0f;
            present[1] = 1.0f;
            present[2] = 0.0f;
            present[3] = 0.0f;
            present[10] = 0.0f;
        }
        if (have_fov && !flat_ui) {
            used_openxr_fov_this_blit = true;
            // V60.7 diagnostic mode 4: use the same asymmetry-adjusted FOV for
            // source-ray mapping that the projection layer will submit to OpenXR.
            // This is a static intrinsics consistency test only -- no pose delta.
            XrFovf mapped_fov = frame_snapshot.eye_fov[eye];
            if (uv_reproj_mode == 4) {
                XrFovf cf{};
                cf.angleLeft  = 0.5f * (frame_snapshot.eye_fov[0].angleLeft  + frame_snapshot.eye_fov[1].angleLeft);
                cf.angleRight = 0.5f * (frame_snapshot.eye_fov[0].angleRight + frame_snapshot.eye_fov[1].angleRight);
                cf.angleUp    = 0.5f * (frame_snapshot.eye_fov[0].angleUp    + frame_snapshot.eye_fov[1].angleUp);
                cf.angleDown  = 0.5f * (frame_snapshot.eye_fov[0].angleDown  + frame_snapshot.eye_fov[1].angleDown);
                const bool depth_asym = depth_push_enabled_.load(std::memory_order_acquire);
                const float asym = depth_asym ? 1.0f : mono_fov_asymmetry_scale_.load(std::memory_order_acquire);
                const XrFovf& rf = frame_snapshot.eye_fov[eye];
                mapped_fov.angleLeft  = cf.angleLeft  + asym * (rf.angleLeft  - cf.angleLeft);
                mapped_fov.angleRight = cf.angleRight + asym * (rf.angleRight - cf.angleRight);
                mapped_fov.angleUp    = cf.angleUp    + asym * (rf.angleUp    - cf.angleUp);
                mapped_fov.angleDown  = cf.angleDown  + asym * (rf.angleDown  - cf.angleDown);
                if (depth_asym) {
                    // In FOV-map mode poseDeltaQ is otherwise unused. Pass the
                    // common/cyclopean tangent bounds to the pixel shader so it
                    // can interpolate eye asymmetry per pixel from depth.
                    present[16] = std::tan(cf.angleLeft);
                    present[17] = std::tan(cf.angleRight);
                    present[18] = std::tan(cf.angleDown);
                    present[19] = std::tan(cf.angleUp);
                    present[23] = 1.0f;
                }
            }
            const XrFovf& f = mapped_fov;
            present[4] = std::tan(f.angleLeft);
            present[5] = std::tan(f.angleRight);
            present[6] = std::tan(f.angleDown);
            present[7] = std::tan(f.angleUp);
            // V46.6: OpenXR-FOV mode must never expose UVs outside the flat
            // source image. Clamp the requested uniform framing scale to the
            // largest value that still covers the most extreme eye ray. This
            // specifically removes the bottom black bar seen with narrow FOVs.
            const float max_abs_x = std::max(std::fabs(present[4]), std::fabs(present[5]));
            const float max_abs_y = std::max(std::fabs(present[6]), std::fabs(present[7]));
            const float max_scale_x = source_tan_x > 1e-4f && max_abs_x > 1e-4f ? (source_tan_x / max_abs_x) : 1.0f;
            const float max_scale_y = source_tan_y > 1e-4f && max_abs_y > 1e-4f ? (source_tan_y / max_abs_y) : 1.0f;
            const float requested_scale = 1.0f / zoom;
            const float coverage_limit = 0.995f * std::min(max_scale_x, max_scale_y);
            // V58.7: Crysis-style mono crop keeps the true requested angular scale.
            // The global no-black clamp survives only behind the F5 comparison.
            const float safe_scale = angular_correct_mono ? requested_scale : std::min(requested_scale, coverage_limit);
            requested_uv_scale_eye[eye] = requested_scale;
            coverage_clamped_eye[eye] = !angular_correct_mono && (safe_scale + 0.0005f < requested_scale);
            clamp_total_count_.fetch_add(1, std::memory_order_relaxed);
            if (coverage_clamped_eye[eye])
                clamp_active_count_.fetch_add(1, std::memory_order_relaxed);
            present[0] = safe_scale;
            present[1] = safe_scale;
            safe_uv_scale_eye[eye] = safe_scale;
        }
        command_list_->SetGraphicsRoot32BitConstants(1, 28, present, 0);
        command_list_->DrawInstanced(3, 1, 0, 0);
    }

    D3D12_RESOURCE_BARRIER source_to_present = source_to_srv;
    source_to_present.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    source_to_present.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    command_list_->ResourceBarrier(1, &source_to_present);
    hr = command_list_->Close(); if (FAILED(hr)) return false;
    ID3D12CommandList* lists[] = { command_list_ };
    if (!DeviceHook::ExecuteCommandListsBypass(d3d12_queue_, 1, lists)) return false;
    ++fence_value_;
    if (FAILED(d3d12_queue_->Signal(fence_, fence_value_))) return false;
    if (FAILED(fence_->SetEventOnCompletion(fence_value_, fence_event_))) return false;
    if (WaitForSingleObject(fence_event_, 1000) != WAIT_OBJECT_0) return false;

    static std::atomic<unsigned long long> blits{0};
    const auto n = blits.fetch_add(1, std::memory_order_relaxed) + 1;
    test_mono_blits_.fetch_add(1, std::memory_order_relaxed);
    if (used_openxr_fov_this_blit)
        test_fov_blits_.fetch_add(1, std::memory_order_relaxed);
    if (marker_enabled_this_blit)
        test_marker_blits_.fetch_add(1, std::memory_order_relaxed);

    if (n <= 8 || (n % 300) == 0) {
        char msg[384]{};
        std::snprintf(msg, sizeof(msg), "V60.7-MONO-BLIT call=%llu src=%p %llux%u -> eyes=%dx%d mode=%d sourceVFov=%.1f bottom=%d mapMode=%d matchedSubmitFov=%s staticH=%s avail=%s uvBinoc=%.4f q=(%.4f,%.4f,%.4f,%.4f) clampLR=(%s,%s)",
            n, source, static_cast<unsigned long long>(src_desc.Width), src_desc.Height,
            eyes_[0].width, eyes_[0].height, presentation_mode, source_vertical_fov_deg, bottom_edge_mode, uv_reproj_mode, (uv_reproj_mode==4)?"YES":"no", (uv_reproj_mode==3)?"YES":"no", reproj_available?"YES":"no", uv_binocular,
            reproj_q.x,reproj_q.y,reproj_q.z,reproj_q.w, coverage_clamped_eye[0]?"YES":"no", coverage_clamped_eye[1]?"YES":"no");
        debug_trace::Write(msg);
    }
    return true;
}

bool XrManager::CopyMenuTextureToUiImage(ID3D12Resource* source, ID3D12Resource* target)
{
    if (!source || !target || !command_allocator_ || !command_list_ || !d3d12_queue_ || !fence_) return false;
    const auto sd = source->GetDesc(); const auto td = target->GetDesc();
    const auto bgra8_family=[](DXGI_FORMAT f) {
        return f==DXGI_FORMAT_B8G8R8A8_UNORM || f==DXGI_FORMAT_B8G8R8A8_UNORM_SRGB || f==DXGI_FORMAT_B8G8R8A8_TYPELESS;
    };
    const bool format_compatible = sd.Format == td.Format || (bgra8_family(sd.Format) && bgra8_family(td.Format));
    if (sd.Dimension != D3D12_RESOURCE_DIMENSION_TEXTURE2D || td.Dimension != D3D12_RESOURCE_DIMENSION_TEXTURE2D ||
        sd.Width != td.Width || sd.Height != td.Height || !format_compatible) {
        static std::atomic<unsigned> bad{0}; const auto n=bad.fetch_add(1)+1;
        if(n<=4) { char m[256]{}; std::snprintf(m,sizeof(m),"V51-F7-QUAD incompatible src=%llux%u fmt=%d dst=%llux%u fmt=%d",
            static_cast<unsigned long long>(sd.Width),sd.Height,static_cast<int>(sd.Format),static_cast<unsigned long long>(td.Width),td.Height,static_cast<int>(td.Format)); debug_trace::Write(m); }
        return false;
    }
    if (sd.Format != td.Format) {
        static std::atomic<unsigned> compat{0}; if(compat.fetch_add(1)==0) debug_trace::Write("V51-F7-QUAD typed/typeless BGRA copy accepted");
    }
    if (fence_->GetCompletedValue() < fence_value_) {
        if (FAILED(fence_->SetEventOnCompletion(fence_value_, fence_event_))) return false;
        if (WaitForSingleObject(fence_event_, 1000) != WAIT_OBJECT_0) return false;
    }
    if (FAILED(command_allocator_->Reset()) || FAILED(command_list_->Reset(command_allocator_, nullptr))) return false;
    D3D12_RESOURCE_BARRIER b[2]{};
    b[0].Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION; b[0].Transition.pResource=source; b[0].Transition.Subresource=D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    b[0].Transition.StateBefore=D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE; b[0].Transition.StateAfter=D3D12_RESOURCE_STATE_COPY_SOURCE;
    b[1].Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION; b[1].Transition.pResource=target; b[1].Transition.Subresource=D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    b[1].Transition.StateBefore=D3D12_RESOURCE_STATE_RENDER_TARGET; b[1].Transition.StateAfter=D3D12_RESOURCE_STATE_COPY_DEST;
    command_list_->ResourceBarrier(2,b); command_list_->CopyResource(target,source);
    std::swap(b[0].Transition.StateBefore,b[0].Transition.StateAfter); std::swap(b[1].Transition.StateBefore,b[1].Transition.StateAfter); command_list_->ResourceBarrier(2,b);
    if (FAILED(command_list_->Close())) return false; ID3D12CommandList* lists[]={command_list_};
    if(!DeviceHook::ExecuteCommandListsBypass(d3d12_queue_,1,lists)) return false;
    ++fence_value_; if(FAILED(d3d12_queue_->Signal(fence_,fence_value_))) return false;
    if(FAILED(fence_->SetEventOnCompletion(fence_value_,fence_event_)) || WaitForSingleObject(fence_event_,1000)!=WAIT_OBJECT_0) return false;
    return true;
}

bool XrManager::SubmitBridgeImagesOnRenderThread(ID3D12Resource* left, ID3D12Resource* right)
{
    ID3D12Resource* source = nullptr;
    int source_eye = 0;
    std::uint64_t serial = 0;
    const int stereo_mode = stereo_experiment_.load(std::memory_order_acquire);
    std::uint64_t present_tick_for_source = 0;
    std::uint64_t camera_frame_for_source = 0;
    const bool got = stereo_mode == 1
        ? DeviceHook::TryGetLatestPresentResourceTagged(&source, &source_eye, &serial)
        : DeviceHook::TryGetLatestPresentResourceIdentity(&source, &serial, &present_tick_for_source, &camera_frame_for_source);
    if (got) {
        const bool ok = stereo_mode == 1
            ? BlitAfrStereoToEyes(source, source_eye, serial, left, right)
            : BlitGameImageToEyes(source, left, right);
        // V47.0: correlate EVERY XR copy with the exact KCD2 PRESENT and the
        // camera/XR frame that rendered it. Logging remains throttled, ownership does not.
        std::uint64_t ps=serial, pt=present_tick_for_source, pw=0, pcf=camera_frame_for_source; std::uint32_t ph=0;
        if (stereo_mode == 1) DeviceHook::GetLatestPresentIdentity(&ps,&pt,&pw,&ph,&pcf);
        else { const auto sd = source->GetDesc(); pw = sd.Width; ph = sd.Height; }
        last_submitted_present_serial_.store(ps, std::memory_order_release);
        last_submitted_camera_frame_.store(pcf, std::memory_order_release);
        static std::atomic<unsigned long long> timing_calls{0};
        const auto tc = timing_calls.fetch_add(1, std::memory_order_relaxed) + 1;
        if (tc <= 12 || (tc % 300) == 0) {
            RenderFrameState tf{}; CopyActiveRenderFrameState(tf);
            const std::uint64_t now = static_cast<std::uint64_t>(GetTickCount64());
            const std::uint64_t cam_tick = CameraHook::LastPoseApplyTickMs();
            char tm[448]{};
            std::snprintf(tm,sizeof(tm),"V47.0-FRAME-ID xrFrame=%llu predicted=%lld presentSerial=%llu presentCameraFrame=%llu presentAgeMs=%llu cameraApplyAgeMs=%llu present=%llux%u stereoMode=%d paired=%s blitOk=%s",
                static_cast<unsigned long long>(tf.frame_index), static_cast<long long>(tf.predicted_display_time),
                static_cast<unsigned long long>(ps), static_cast<unsigned long long>(pcf), static_cast<unsigned long long>(pt?now-pt:0),
                static_cast<unsigned long long>(cam_tick?now-cam_tick:0),
                static_cast<unsigned long long>(pw), ph, stereo_mode, present_pose_pairing_mode_.load(std::memory_order_relaxed) ? "true" : "false", ok?"true":"false");
            debug_trace::Write(tm);
        }
        source->Release();
        if (ok) return true;
        debug_trace::Write(stereo_mode == 1
            ? "V39-AFR stereo blit failed; falling back to V38 mono bridge"
            : "V38-PRESENT primary PRESENT-source blit failed; using diagnostic fallback");
        if (stereo_mode == 1 && DeviceHook::TryGetLatestPresentResource(&source)) {
            const bool mono_ok = BlitGameImageToEyes(source, left, right);
            source->Release();
            if (mono_ok) return true;
        }
    } else {
        static std::atomic<unsigned> misses{0};
        const auto n = misses.fetch_add(1, std::memory_order_relaxed) + 1;
        if (n <= 8 || (n % 300) == 0) debug_trace::Write("V39-BRIDGE no PRESENT-transition source yet; using diagnostic fallback");
    }
    return SubmitDiagnosticImagesOnRenderThread(left, right);
}

bool XrManager::SubmitDiagnosticImagesOnRenderThread(ID3D12Resource* left, ID3D12Resource* right)
{
    if (!left || !right)
        return false;
    return ClearD3D12Image(left) && ClearD3D12Image(right);
}

void XrManager::OnRenderThreadTick(ID3D12CommandQueue* queue)
{
    const auto tick = render_tick_count_.fetch_add(1, std::memory_order_relaxed) + 1;
    if (tick == 1)
        debug_trace::Write("first DIRECT-queue render-thread tick observed");
    if ((tick % 600) == 0)
        debug_trace::WriteCounter("direct_queue_render_ticks", tick);

    if ((!diagnostic_rendering_.load(std::memory_order_acquire) && !active_vr_bridge_.load(std::memory_order_acquire)) || queue != d3d12_queue_)
        return;

    ID3D12Resource* left = nullptr;
    ID3D12Resource* right = nullptr;
    ID3D12Resource* ui_image = nullptr;
    {
        std::unique_lock<std::mutex> lock(render_mutex_);
        if (!render_request_pending_ || render_request_in_progress_)
            return;
        render_request_in_progress_ = true;
        left = render_request_images_[0];
        right = render_request_images_[1];
        ui_image = render_request_ui_image_;
        render_cv_.notify_all();
    }

    bool ok = active_vr_bridge_.load(std::memory_order_acquire) && !diagnostic_rendering_.load(std::memory_order_acquire)
        ? SubmitBridgeImagesOnRenderThread(left, right)
        : SubmitDiagnosticImagesOnRenderThread(left, right);
    if (ok && ui_image && menu_quad_visible_.load(std::memory_order_acquire)) {
        ID3D12Resource* menu = menu_quad_source_.load(std::memory_order_acquire);
        const bool ui_ok = menu && CopyMenuTextureToUiImage(menu, ui_image);
        if (!ui_ok) debug_trace::Write("V51-F7-QUAD menu texture copy failed; gameplay layer preserved");
    }

    {
        std::lock_guard<std::mutex> lock(render_mutex_);
        render_request_success_ = ok;
        render_request_done_ = true;
        render_request_pending_ = false;
        render_request_in_progress_ = false;
    }
    render_cv_.notify_all();
}

bool XrManager::QueueRenderThreadDiagnosticWork(ID3D12Resource* left, ID3D12Resource* right, ID3D12Resource* ui_image)
{
    if (!left || !right)
        return false;

    std::unique_lock<std::mutex> lock(render_mutex_);
    render_request_images_[0] = left;
    render_request_images_[1] = right;
    render_request_ui_image_ = ui_image;
    render_request_pending_ = true;
    render_request_in_progress_ = false;
    render_request_done_ = false;
    render_request_success_ = false;

    // First wait for the game render thread to pick the request up. If no
    // DIRECT-queue submission occurs in this window, cancel before releasing
    // the XR swapchain images. Once work has started, wait for completion so
    // an image is never released while our GPU commands still reference it.
    const bool picked_up = render_cv_.wait_for(
        lock, std::chrono::milliseconds(500),
        [this] { return render_request_in_progress_ || render_request_done_; });

    if (!picked_up)
    {
        render_request_pending_ = false;
        render_request_images_[0] = nullptr;
        render_request_images_[1] = nullptr;
        render_request_ui_image_ = nullptr;
        debug_trace::Write("render-thread handoff timed out before pickup");
        return false;
    }

    render_cv_.wait(lock, [this] { return render_request_done_; });
    const bool ok = render_request_success_;
    render_request_images_[0] = nullptr;
    render_request_images_[1] = nullptr;
    render_request_ui_image_ = nullptr;
    return ok;
}

D3D12_CPU_DESCRIPTOR_HANDLE XrManager::GetRtvHandle(ID3D12Resource* image)
{
    for (auto& entry : rtv_entries_)
    {
        if (entry.resource == image)
            return entry.handle;
    }

    D3D12_CPU_DESCRIPTOR_HANDLE null_handle{};
    if (!rtv_heap_ || !image)
        return null_handle;

    const UINT index = static_cast<UINT>(rtv_entries_.size());
    D3D12_CPU_DESCRIPTOR_HANDLE handle = rtv_heap_->GetCPUDescriptorHandleForHeapStart();
    handle.ptr += static_cast<SIZE_T>(index) * rtv_descriptor_size_;

    D3D12_RENDER_TARGET_VIEW_DESC desc{};
    desc.Format = static_cast<DXGI_FORMAT>(swapchain_format_);
    desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
    desc.Texture2D.MipSlice = 0;
    d3d12_device_->CreateRenderTargetView(image, &desc, handle);

    rtv_entries_.push_back({ image, handle });
    return handle;
}

bool XrManager::CreateRtvHeap()
{
    if (rtv_heap_) return true;

    D3D12_DESCRIPTOR_HEAP_DESC heap_desc{};
    heap_desc.NumDescriptors = 16;
    heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    HRESULT hr = d3d12_device_->CreateDescriptorHeap(
        &heap_desc, IID_PPV_ARGS(&rtv_heap_));
    if (FAILED(hr))
    {
        DetourModKit::log().error("CreateDescriptorHeap(RTV) failed, hr=0x{:08X}", static_cast<unsigned>(hr));
        return false;
    }

    rtv_descriptor_size_ = d3d12_device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    return true;
}


bool XrManager::CopyLatestStereoViews(StereoViewSnapshot& out)
{
    std::lock_guard<std::mutex> lock(stereo_view_mutex_);
    out = latest_stereo_views_;
    return out.captured_tick_ms != 0;
}

bool XrManager::CopyLastGoodStereoViews(StereoViewSnapshot& out)
{
    std::lock_guard<std::mutex> lock(stereo_view_mutex_);
    out = last_good_stereo_views_;
    return out.valid && out.captured_tick_ms != 0;
}

bool XrManager::CopyActiveRenderFrameState(RenderFrameState& out)
{
    std::lock_guard<std::mutex> lock(stereo_view_mutex_);
    out = active_render_frame_;
    return out.frame_index != 0;
}

bool XrManager::CopyRenderFrameStateByIndex(std::uint64_t frame_index, RenderFrameState& out)
{
    if (frame_index == 0) return false;
    std::lock_guard<std::mutex> lock(stereo_view_mutex_);
    const RenderFrameState& candidate = render_frame_history_[frame_index % kRenderFrameHistoryCount];
    if (candidate.frame_index != frame_index) return false;
    out = candidate;
    return true;
}

XrManager::TestCounters XrManager::GetTestCounters() const
{
    TestCounters out{};
    out.xr_frames = test_xr_frames_.load(std::memory_order_acquire);
    out.valid_frame_snapshots = test_valid_frame_snapshots_.load(std::memory_order_acquire);
    out.mono_blits = test_mono_blits_.load(std::memory_order_acquire);
    out.afr_blits = test_afr_blits_.load(std::memory_order_acquire);
    out.fov_blits = test_fov_blits_.load(std::memory_order_acquire);
    out.marker_blits = test_marker_blits_.load(std::memory_order_acquire);
    out.swapped_compositions = test_swapped_compositions_.load(std::memory_order_acquire);
    return out;
}

bool XrManager::RunFrame(XrPose& out_head_pose, float head_pose_scale)
{
    if (!session_running_.load(std::memory_order_relaxed)) return false;

    // V58 live XR-resolution apply. Execute only on the XR worker before
    // xrWaitFrame/acquire, when no image from the new frame is owned. Finish
    // our last D3D12 submission, destroy/recreate only XR swapchains, and keep
    // the KCD2 desktop/backbuffer resolution untouched. If the requested size
    // fails, rebuild the previous scale as a fail-safe.
    const float requested_scale=requested_render_scale_.exchange(0.0f,std::memory_order_acq_rel);
    if(requested_scale>=0.50f) {
        const float previous_scale=active_swapchain_scale_.load(std::memory_order_acquire);
        if(fence_ && fence_->GetCompletedValue()<fence_value_ && fence_event_ && SUCCEEDED(fence_->SetEventOnCompletion(fence_value_,fence_event_)))
            WaitForSingleObject(fence_event_,INFINITE);
        rtv_entries_.clear();
        DestroySwapchains();
        render_scale_.store(std::clamp(requested_scale,0.50f,2.0f),std::memory_order_release);
        bool rebuilt=CreateSwapchains();
        if(!rebuilt) {
            char e[256]{}; std::snprintf(e,sizeof(e),"V58-RESOLUTION requested %.3fx failed; rebuilding previous %.3fx",requested_scale,previous_scale); debug_trace::Write(e);
            DestroySwapchains(); rtv_entries_.clear();
            render_scale_.store(previous_scale,std::memory_order_release);
            rebuilt=CreateSwapchains();
        }
        if(!rebuilt) { debug_trace::Write("V58-RESOLUTION fatal: swapchain rebuild and rollback both failed; stopping XR session safely"); session_running_.store(false,std::memory_order_release); return false; }
        int rw=0,rh=0; GetEyeSize(rw,rh);
        char m[256]{}; std::snprintf(m,sizeof(m),"V58-RESOLUTION rebuild complete activeScale=%.3f eye=%dx%d desktop=UNCHANGED",active_swapchain_scale_.load(std::memory_order_relaxed),rw,rh); debug_trace::Write(m);
    }

    const auto frame_no = g_xr_frame_count.fetch_add(1, std::memory_order_relaxed) + 1;
    test_xr_frames_.fetch_add(1, std::memory_order_relaxed);
    if (frame_no == 1 || (frame_no % 300) == 0)
        debug_trace::WriteCounter("xr_frames", frame_no);

    XrEventDataBuffer event{ XR_TYPE_EVENT_DATA_BUFFER };
    while (xrPollEvent(instance_, &event) == XR_SUCCESS)
    {
        if (event.type == XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED)
        {
            auto* state = reinterpret_cast<XrEventDataSessionStateChanged*>(&event);
            if (state->state == XR_SESSION_STATE_STOPPING ||
                state->state == XR_SESSION_STATE_EXITING ||
                state->state == XR_SESSION_STATE_LOSS_PENDING)
            {
                session_running_.store(false, std::memory_order_relaxed);
    qpc_to_xr_time_ = nullptr;
    qpc_time_extension_enabled_ = false;
            }
        }
        else if (event.type == XR_TYPE_EVENT_DATA_REFERENCE_SPACE_CHANGE_PENDING)
        {
            // V59.3: Meta/Quest recenter changes the runtime reference space.
            // Rebase the game-camera origin on the next valid pose and force a
            // fresh F6 anchor so world-fixed comfort panels follow the new origin.
            CameraHook::RequestRecenter();
            { std::lock_guard<std::mutex> lock(flat_ui_mutex_); flat_ui_anchor_valid_ = false; }
            debug_trace::Write("V59.3-RECENTER OpenXR reference-space change -> camera origin + F6 anchor reset");
        }
        event.type = XR_TYPE_EVENT_DATA_BUFFER;
    }
    if (!session_running_.load(std::memory_order_relaxed)) return false;

    XrFrameState frame_state{ XR_TYPE_FRAME_STATE };
    XrFrameWaitInfo wait_info{ XR_TYPE_FRAME_WAIT_INFO };
    if (XR_FAILED(xrWaitFrame(session_, &wait_info, &frame_state))) { debug_trace::Write("xrWaitFrame failed"); return false; }

    // V49: derive the active headset cadence from OpenXR predicted display times.
    // XrTime is nanoseconds. Smooth the estimate so 72/80/90/120 Hz sessions
    // can drive diagnostics without any hard-coded refresh assumption.
    if (previous_predicted_display_time_ != 0 && frame_state.predictedDisplayTime > previous_predicted_display_time_) {
        const double dt_ns = static_cast<double>(frame_state.predictedDisplayTime - previous_predicted_display_time_);
        const float hz = static_cast<float>(1.0e9 / dt_ns);
        if (std::isfinite(hz) && hz >= 30.0f && hz <= 240.0f) {
            const float old = estimated_refresh_hz_.load(std::memory_order_relaxed);
            estimated_refresh_hz_.store(old > 1.0f ? (old * 0.90f + hz * 0.10f) : hz, std::memory_order_release);
        }
    }
    previous_predicted_display_time_ = frame_state.predictedDisplayTime;
    latest_predicted_display_time_.store(static_cast<std::int64_t>(frame_state.predictedDisplayTime), std::memory_order_release);

    XrFrameBeginInfo begin_info{ XR_TYPE_FRAME_BEGIN_INFO };
    if (XR_FAILED(xrBeginFrame(session_, &begin_info))) { debug_trace::Write("xrBeginFrame failed"); return false; }

    bool got_pose = false;
    XrPosef located_head_pose{};
    // Crysis VR uses the predicted display time to obtain tracking for the
    // current XR frame. Keep HMD tracking independent from rendering/camera
    // hooks so the camera can consume the latest pose on the game thread.
    {
        XrSpaceLocation location{ XR_TYPE_SPACE_LOCATION };
        std::lock_guard<std::mutex> pose_lock(pose_locate_mutex_);
        if (XR_SUCCEEDED(xrLocateSpace(view_space_, local_space_, frame_state.predictedDisplayTime, &location)) &&
            (location.locationFlags & XR_SPACE_LOCATION_POSITION_VALID_BIT) &&
            (location.locationFlags & XR_SPACE_LOCATION_ORIENTATION_VALID_BIT))
        {
            out_head_pose.pos_x = location.pose.position.x;
            out_head_pose.pos_y = location.pose.position.y;
            out_head_pose.pos_z = location.pose.position.z;
            out_head_pose.qx = location.pose.orientation.x;
            out_head_pose.qy = location.pose.orientation.y;
            out_head_pose.qz = location.pose.orientation.z;
            out_head_pose.qw = location.pose.orientation.w;
            located_head_pose = location.pose;
            got_pose = true;
            const auto pose_no = g_valid_pose_count.fetch_add(1, std::memory_order_relaxed) + 1;
            if (pose_no == 1)
                debug_trace::Write("first valid HMD pose acquired");
            if ((pose_no % 300) == 0)
                debug_trace::WriteCounter("valid_hmd_poses", pose_no);
        }
    }

    input_.Sync(frame_state.predictedDisplayTime);

    // V61.13 menu pointer: use a large flat XR quad even when the cylinder
    // extension is available, so the existing right-controller ray intersection
    // is exact and menu input no longer falls back to mouse-look.
    // V61.31 also retains the exact controller->menu segment for an OpenXR-only
    // laser ribbon. It never enters the KCD2 render path or camera state.
    bool menu_laser_valid=false;
    V3f menu_laser_start{};
    V3f menu_laser_end{};
    menu_pointer_active_.store(false,std::memory_order_release);
    if (got_pose && menu_quad_visible_.load(std::memory_order_acquire)) {
        const auto& rc=input_.Right();
        if (rc.active || rc.aim_active) {
            // V62.4: use OpenXR's dedicated aim pose for the menu ray whenever
            // the runtime supplies it. Grip-pose -Z was noticeably finicky on
            // Touch/Quest and forced the user's hand close to the torso.
            const XrPose& pp = rc.aim_active ? rc.aim_pose : rc.pose;
            XrQuaternionf hq{out_head_pose.qx,out_head_pose.qy,out_head_pose.qz,out_head_pose.qw};
            XrQuaternionf cq{pp.qx,pp.qy,pp.qz,pp.qw};
            const auto invh=QConj(hq);
            V3f dp{pp.pos_x-out_head_pose.pos_x,pp.pos_y-out_head_pose.pos_y,pp.pos_z-out_head_pose.pos_z};
            V3f ro=QRotate(invh,dp);
            XrQuaternionf rq=QMul(invh,cq);
            V3f rd=QRotate(rq,{0,0,-1});
            if (rd.z < -0.005f) {
                constexpr float kMenuZ=-1.45f;
                const float t=(kMenuZ-ro.z)/rd.z;
                if (t>0.0f && t<8.0f) {
                    const float hx=ro.x+rd.x*t, hy=ro.y+rd.y*t;
                    constexpr float w=1.64f;
                    constexpr float h=w*(680.0f/1100.0f);
                    const float u=hx/w+0.5f, v=0.5f-hy/h;
                    // Small overscan keeps the pointer alive at the parchment
                    // edge; clamp only after the ray has intersected near it.
                    if(u>=-0.10f&&u<=1.10f&&v>=-0.10f&&v<=1.10f){
                        const float uc=std::clamp(u,0.0f,1.0f), vc=std::clamp(v,0.0f,1.0f);
                        menu_pointer_x_.store((int)(uc*1099.0f+0.5f),std::memory_order_release);
                        menu_pointer_y_.store((int)(vc*679.0f+0.5f),std::memory_order_release);
                        menu_pointer_active_.store(true,std::memory_order_release);
                        menu_laser_start=ro;
                        menu_laser_end={std::clamp(hx,-w*0.60f,w*0.60f),std::clamp(hy,-h*0.60f,h*0.60f),kMenuZ};
                        const V3f beam_delta{menu_laser_end.x-menu_laser_start.x,menu_laser_end.y-menu_laser_start.y,menu_laser_end.z-menu_laser_start.z};
                        menu_laser_valid=VLength(beam_delta)>0.03f;
                    }
                }
            }
        }
    }

    // V62.9: same Touch AIM ray projected onto the real F6 LOCAL theater quad.
    // This is intentionally independent from the book pointer and is normalized
    // to the captured KCD2 source so Win32 mouse injection can drive inventory,
    // shops, cooking, map and other flat UI without guessing a 16:9 surface.
    flat_pointer_active_.store(false,std::memory_order_release);
    if (flat_ui_mode_.load(std::memory_order_acquire)) {
        const auto& rc=input_.Right();
        if (rc.active || rc.aim_active) {
            XrPosef anchor{}; bool anchor_valid=false;
            { std::lock_guard<std::mutex> lock(flat_ui_mutex_); anchor=flat_ui_anchor_pose_; anchor_valid=flat_ui_anchor_valid_; }
            if(anchor_valid) {
                const XrPose& pp=rc.aim_active?rc.aim_pose:rc.pose;
                const XrQuaternionf level_q=QYawOnly(anchor.orientation);
                const float distance=std::clamp(flat_ui_distance_.load(std::memory_order_acquire),0.80f,3.20f);
                const V3f forward=QRotate(level_q,{0.0f,0.0f,-distance});
                const V3f centre{anchor.position.x+forward.x,anchor.position.y+forward.y,anchor.position.z+forward.z};
                const XrQuaternionf invq=QConj(level_q);
                const V3f ro=QRotate(invq,{pp.pos_x-centre.x,pp.pos_y-centre.y,pp.pos_z-centre.z});
                const XrQuaternionf cq{pp.qx,pp.qy,pp.qz,pp.qw};
                const V3f rd=QRotate(QMul(invq,cq),{0.0f,0.0f,-1.0f});
                if(std::fabs(rd.z)>0.005f) {
                    const float t=-ro.z/rd.z;
                    if(t>0.0f && t<8.0f) {
                        const float hx=ro.x+rd.x*t, hy=ro.y+rd.y*t;
                        const float ui_scale=std::clamp(flat_ui_scale_.load(std::memory_order_acquire),0.60f,3.00f);
                        const float width=std::clamp(1.82f*(1.58f/ui_scale),1.05f,2.65f);
                        const float sw=float(std::max<std::uint32_t>(1,last_source_width_.load(std::memory_order_acquire)));
                        const float sh=float(std::max<std::uint32_t>(1,last_source_height_.load(std::memory_order_acquire)));
                        const float height=width/std::clamp(sw/sh,0.75f,2.50f);
                        const float u=hx/width+0.5f, v=0.5f-hy/height;
                        if(u>=-0.08f&&u<=1.08f&&v>=-0.08f&&v<=1.08f) {
                            flat_pointer_u_.store(std::clamp(u,0.0f,1.0f),std::memory_order_release);
                            flat_pointer_v_.store(std::clamp(v,0.0f,1.0f),std::memory_order_release);
                            flat_pointer_active_.store(true,std::memory_order_release);
                        }
                    }
                }
            }
        }
    }

    // V46 Gemini correction: the pose sampled at predictedDisplayTime is
    // published to the game camera immediately, not after xrEndFrame. This is
    // still a mono/main-camera feed; true per-eye use comes later from the same
    // RenderFrameState snapshot when a Recursive right-eye render is activated.
    if (input_.RecenterRequested()) {
        CameraHook::RequestRecenter();
        input_.ClearRecenterRequest();
    }
    if (got_pose)
        CameraHook::SetHeadPoseForFrame(out_head_pose, head_pose_scale, frame_no);

    XrView views[2] = { { XR_TYPE_VIEW }, { XR_TYPE_VIEW } };
    uint32_t view_count = 0;
    XrViewLocateInfo view_locate_info{ XR_TYPE_VIEW_LOCATE_INFO };
    view_locate_info.viewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
    view_locate_info.displayTime = frame_state.predictedDisplayTime;
    view_locate_info.space = local_space_;
    XrViewState view_state{ XR_TYPE_VIEW_STATE };
    XrResult view_result = xrLocateViews(session_, &view_locate_info, &view_state, 2, &view_count, views);

    const bool stereo_views_valid = XR_SUCCEEDED(view_result) && view_count == 2 &&
        (view_state.viewStateFlags & XR_VIEW_STATE_POSITION_VALID_BIT) &&
        (view_state.viewStateFlags & XR_VIEW_STATE_ORIENTATION_VALID_BIT);

    // Injection has a separate validity policy from composition. Some runtimes
    // can transiently omit view-state validity bits while still returning two
    // finite, geometrically sane eye poses. For native-eye injection we cache
    // that last plausible pair briefly; composition below remains strict.
    bool injection_views_valid = false;
    float injection_ipd = 0.0f;
    if (XR_SUCCEEDED(view_result) && view_count == 2)
    {
        const float dx = views[1].pose.position.x - views[0].pose.position.x;
        const float dy = views[1].pose.position.y - views[0].pose.position.y;
        const float dz = views[1].pose.position.z - views[0].pose.position.z;
        injection_ipd = std::sqrt(dx*dx + dy*dy + dz*dz);
        injection_views_valid = std::isfinite(injection_ipd) && injection_ipd >= 0.040f && injection_ipd <= 0.090f;
        if (injection_views_valid && stereo_experiment_.load(std::memory_order_acquire) == 1)
            CameraHook::SetAfrIpd(injection_ipd, afr_ipd_scale_.load(std::memory_order_acquire));
    }

    // V17 always caches the raw xrLocateViews outcome. This makes one launch
    // diagnose viewCount/flags/raw positions while also feeding the fallback
    // ladder. A plausible pair is tagged valid; implausible or incomplete
    // results are retained only for diagnostics/fallback selection.
    {
        std::lock_guard<std::mutex> lock(stereo_view_mutex_);
        latest_stereo_views_.predicted_display_time = frame_state.predictedDisplayTime;
        latest_stereo_views_.locate_result = view_result;
        latest_stereo_views_.locate_succeeded = XR_SUCCEEDED(view_result);
        latest_stereo_views_.view_count = view_count;
        latest_stereo_views_.view_state_flags = view_state.viewStateFlags;
        latest_stereo_views_.raw_ipd = injection_ipd;
        latest_stereo_views_.valid = injection_views_valid;
        if (view_count >= 1) { latest_stereo_views_.eye_pose[0] = views[0].pose; latest_stereo_views_.eye_fov[0] = views[0].fov; }
        if (view_count >= 2) { latest_stereo_views_.eye_pose[1] = views[1].pose; latest_stereo_views_.eye_fov[1] = views[1].fov; }
        latest_stereo_views_.captured_tick_ms = static_cast<std::uint64_t>(GetTickCount64());
        if (injection_views_valid)
            last_good_stereo_views_ = latest_stereo_views_;

        // Gemini idea, corrected for this architecture: the XR worker owns
        // xrWaitFrame/xrBeginFrame/xrLocateViews exactly once. Cache that
        // immutable frame identity here. Rendering/submission reads this
        // snapshot; it must never call xrLocateViews independently.
        active_render_frame_.frame_index = frame_no;
        active_render_frame_.predicted_display_time = frame_state.predictedDisplayTime;
        active_render_frame_.should_render = frame_state.shouldRender == XR_TRUE;
        active_render_frame_.head_pose_valid = got_pose;
        active_render_frame_.head_pose = located_head_pose;
        active_render_frame_.stereo_views_valid = stereo_views_valid;
        active_render_frame_.injection_views_valid = injection_views_valid;
        active_render_frame_.view_count = view_count;
        active_render_frame_.view_state_flags = view_state.viewStateFlags;
        active_render_frame_.locate_result = view_result;
        active_render_frame_.raw_ipd = injection_ipd;
        if (view_count >= 1) {
            active_render_frame_.eye_pose[0] = views[0].pose;
            active_render_frame_.eye_fov[0] = views[0].fov;
        }
        if (view_count >= 2) {
            active_render_frame_.eye_pose[1] = views[1].pose;
            active_render_frame_.eye_fov[1] = views[1].fov;
        }
        render_frame_history_[frame_no % kRenderFrameHistoryCount] = active_render_frame_;
    }

    if (got_pose && stereo_views_valid)
        test_valid_frame_snapshots_.fetch_add(1, std::memory_order_relaxed);

    if (frame_no == 1 || (frame_no % 300) == 0)
    {
        char msg[512]{};
        std::snprintf(msg, sizeof(msg),
            "XR-VIEWS-V21 frame=%llu result=%d count=%u flags=0x%llX raw_ipd=%.6f L=(%.5f,%.5f,%.5f) R=(%.5f,%.5f,%.5f) plausible=%s",
            static_cast<unsigned long long>(frame_no), static_cast<int>(view_result), view_count,
            static_cast<unsigned long long>(view_state.viewStateFlags), static_cast<double>(injection_ipd),
            static_cast<double>(views[0].pose.position.x), static_cast<double>(views[0].pose.position.y), static_cast<double>(views[0].pose.position.z),
            static_cast<double>(views[1].pose.position.x), static_cast<double>(views[1].pose.position.y), static_cast<double>(views[1].pose.position.z),
            injection_views_valid ? "true" : "false");
        debug_trace::Write(msg);

        char snap[512]{};
        std::snprintf(snap, sizeof(snap),
            "V46-FRAME-SNAPSHOT frame=%llu predicted=%lld shouldRender=%s head=%s views=%u strict=%s plausible=%s ipd=%.6f FOVL=(%.5f,%.5f,%.5f,%.5f) FOVR=(%.5f,%.5f,%.5f,%.5f)",
            static_cast<unsigned long long>(frame_no),
            static_cast<long long>(frame_state.predictedDisplayTime),
            frame_state.shouldRender ? "true" : "false", got_pose ? "valid" : "invalid", view_count,
            stereo_views_valid ? "true" : "false", injection_views_valid ? "true" : "false",
            static_cast<double>(injection_ipd),
            static_cast<double>(views[0].fov.angleLeft), static_cast<double>(views[0].fov.angleRight),
            static_cast<double>(views[0].fov.angleUp), static_cast<double>(views[0].fov.angleDown),
            static_cast<double>(views[1].fov.angleLeft), static_cast<double>(views[1].fov.angleRight),
            static_cast<double>(views[1].fov.angleUp), static_cast<double>(views[1].fov.angleDown));
        debug_trace::Write(snap);
    }

    RenderFrameState active_frame{};
    const bool have_active_frame = CopyActiveRenderFrameState(active_frame);
    XrCompositionLayerProjectionView proj_views[2]{};
    bool have_layer = (diagnostic_rendering_.load(std::memory_order_acquire) || active_vr_bridge_.load(std::memory_order_acquire)) && have_active_frame &&
        active_frame.should_render && active_frame.stereo_views_valid;

    RenderFrameState layer_frame = active_frame;
    bool layer_frame_paired = false;
    uint32_t image_indices[2] = { 0, 0 };
    uint32_t ui_image_index = 0;
    bool ui_acquired = false;
    bool have_ui_layer = false;
    if (have_layer)
    {
        bool acquired[2] = { false, false };
        bool images_ready = true;

        for (int eye = 0; eye < 2; ++eye)
        {
            EyeSwapchain& e = eyes_[eye];
            XrSwapchainImageAcquireInfo acquire_info{ XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO };
            if (XR_FAILED(xrAcquireSwapchainImage(e.swapchain, &acquire_info, &image_indices[eye])))
            {
                images_ready = false;
                break;
            }
            acquired[eye] = true;

            XrSwapchainImageWaitInfo wait_img{ XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO };
            wait_img.timeout = XR_INFINITE_DURATION;
            if (XR_FAILED(xrWaitSwapchainImage(e.swapchain, &wait_img)) ||
                image_indices[eye] >= e.d3d12_images.size())
            {
                images_ready = false;
                break;
            }
        }

        ID3D12Resource* ui_image = nullptr;
        bool ui_ready = false;
        const bool want_ui = menu_quad_visible_.load(std::memory_order_acquire) &&
            menu_quad_source_.load(std::memory_order_acquire) != nullptr && menu_ui_.swapchain != XR_NULL_HANDLE;
        if (images_ready && want_ui) {
            XrSwapchainImageAcquireInfo ai{XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO};
            if (XR_SUCCEEDED(xrAcquireSwapchainImage(menu_ui_.swapchain,&ai,&ui_image_index))) {
                ui_acquired=true; XrSwapchainImageWaitInfo wi{XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO}; wi.timeout=XR_INFINITE_DURATION;
                if (XR_SUCCEEDED(xrWaitSwapchainImage(menu_ui_.swapchain,&wi)) && ui_image_index < menu_ui_.d3d12_images.size()) { ui_image=menu_ui_.d3d12_images[ui_image_index]; ui_ready=true; }
            }
            if (!ui_ready) debug_trace::Write("V51-F7-QUAD acquire/wait failed; gameplay continues without menu layer this frame");
        }
        if (images_ready)
        {
            ID3D12Resource* left = eyes_[0].d3d12_images[image_indices[0]];
            ID3D12Resource* right = eyes_[1].d3d12_images[image_indices[1]];
            images_ready = QueueRenderThreadDiagnosticWork(left, right, ui_ready ? ui_image : nullptr);
            have_ui_layer = images_ready && ui_ready && ui_image != nullptr;
        }

        // V47.0: the projection layer must describe the pose that rendered the pixels,
        // not simply the newest XR pose. The PRESENT barrier was tagged with the
        // CameraHook frame actually consumed by KCD2. Resolve that immutable frame
        // snapshot from the small ring; fall back to current XR state if unavailable.
        layer_frame = active_frame;
        layer_frame_paired = false;
        if (present_pose_pairing_mode_.load(std::memory_order_acquire) != 0) {
            const std::uint64_t camera_frame = last_submitted_camera_frame_.load(std::memory_order_acquire);
            RenderFrameState paired{};
            if (CopyRenderFrameStateByIndex(camera_frame, paired) && paired.stereo_views_valid) {
                layer_frame = paired;
                layer_frame_paired = true;
            }
        }

        // Release ownership only after render-thread GPU work has completed (or
        // after a request was cancelled before pickup). OpenXR swapchain calls
        // remain serialized on this XR worker thread.
        for (int eye = 0; eye < 2; ++eye)
        {
            if (!acquired[eye]) continue;
            XrSwapchainImageReleaseInfo release_info{ XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO };
            if (XR_FAILED(xrReleaseSwapchainImage(eyes_[eye].swapchain, &release_info))) images_ready = false;
        }
        if (ui_acquired) {
            XrSwapchainImageReleaseInfo release_info{XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO};
            if (XR_FAILED(xrReleaseSwapchainImage(menu_ui_.swapchain,&release_info))) { have_ui_layer=false; }
        }

        have_layer = images_ready;
        if (have_layer)
        {
            const int proj_pose_mode = projection_pose_mode_.load(std::memory_order_acquire);
            if (proj_pose_mode == 1 && !frozen_projection_valid_.load(std::memory_order_acquire)) {
                frozen_projection_eye_pose_[0]=layer_frame.eye_pose[0]; frozen_projection_eye_pose_[1]=layer_frame.eye_pose[1];
                frozen_projection_valid_.store(true,std::memory_order_release);
                debug_trace::Write("V52-PROJECTION-POSE frozen render pose captured");
            }
            const bool swap_eyes = swap_eyes_.load(std::memory_order_acquire);
            if (swap_eyes)
                test_swapped_compositions_.fetch_add(1, std::memory_order_relaxed);
            const bool flat_ui_projection = flat_ui_mode_.load(std::memory_order_acquire);
            XrPosef flat_pose[2]{};
            XrFovf flat_fov[2]{};
            if (flat_ui_projection) {
                // V56.1 F6 convergence/IPD diagnostic. The same 2D game image is copied
                // to both eye swapchains, so normal asymmetric eye geometry can make it
                // look crossed/swapped. Interpolate eye position + asymmetric FOV around
                // a common centered view. 0 mm = identical geometry; negative values
                // deliberately reverse the binocular geometry for diagnosis.
                const XrPosef base0 = (projection_pose_mode_.load(std::memory_order_relaxed)==1 && frozen_projection_valid_.load(std::memory_order_relaxed)) ? frozen_projection_eye_pose_[0] : layer_frame.eye_pose[0];
                const XrPosef base1 = (projection_pose_mode_.load(std::memory_order_relaxed)==1 && frozen_projection_valid_.load(std::memory_order_relaxed)) ? frozen_projection_eye_pose_[1] : layer_frame.eye_pose[1];
                const float dx=base1.position.x-base0.position.x, dy=base1.position.y-base0.position.y, dz=base1.position.z-base0.position.z;
                const float raw=std::sqrt(dx*dx+dy*dy+dz*dz);
                const float requested_m=std::clamp(flat_ui_ipd_mm_.load(std::memory_order_acquire),-80.0f,80.0f)*0.001f;
                const float factor=raw>1e-5f ? std::clamp(requested_m/raw,-1.5f,1.5f) : 0.0f;
                XrPosef mid=base0;
                mid.position.x=0.5f*(base0.position.x+base1.position.x); mid.position.y=0.5f*(base0.position.y+base1.position.y); mid.position.z=0.5f*(base0.position.z+base1.position.z);
                // Use the current head orientation as a common orientation for both eyes.
                if (layer_frame.head_pose_valid) mid.orientation=layer_frame.head_pose.orientation;
                const XrFovf common{
                    0.5f*(layer_frame.eye_fov[0].angleLeft + layer_frame.eye_fov[1].angleLeft),
                    0.5f*(layer_frame.eye_fov[0].angleRight + layer_frame.eye_fov[1].angleRight),
                    0.5f*(layer_frame.eye_fov[0].angleUp + layer_frame.eye_fov[1].angleUp),
                    0.5f*(layer_frame.eye_fov[0].angleDown + layer_frame.eye_fov[1].angleDown)
                };
                for(int eye=0;eye<2;++eye){
                    const XrPosef& b = eye==0 ? base0 : base1;
                    flat_pose[eye]=mid;
                    flat_pose[eye].position.x = mid.position.x + factor*(b.position.x-mid.position.x);
                    flat_pose[eye].position.y = mid.position.y + factor*(b.position.y-mid.position.y);
                    flat_pose[eye].position.z = mid.position.z + factor*(b.position.z-mid.position.z);
                    const XrFovf& bf=layer_frame.eye_fov[eye];
                    flat_fov[eye].angleLeft = common.angleLeft + factor*(bf.angleLeft-common.angleLeft);
                    flat_fov[eye].angleRight = common.angleRight + factor*(bf.angleRight-common.angleRight);
                    flat_fov[eye].angleUp = common.angleUp + factor*(bf.angleUp-common.angleUp);
                    flat_fov[eye].angleDown = common.angleDown + factor*(bf.angleDown-common.angleDown);
                }
                static std::atomic<unsigned long long> ui_ipd_log{0}; const auto n=ui_ipd_log.fetch_add(1,std::memory_order_relaxed)+1;
                if(n<=4 || (n%300)==0){ char m[256]{}; std::snprintf(m,sizeof(m),
                    "V56.1-FLAT-UI-IPD requestedMm=%.1f rawMm=%.1f factor=%.3f commonFov=(%.3f,%.3f,%.3f,%.3f)",
                    flat_ui_ipd_mm_.load(std::memory_order_relaxed),raw*1000.0f,factor,common.angleLeft,common.angleRight,common.angleUp,common.angleDown); debug_trace::Write(m); }
            }
            // V61.4 LEGACY_GOOD_MONO: restore the pre-cyclopean mono submission.
            // One captured KCD2 source is duplicated to both eye textures, while OpenXR
            // receives the runtime's real per-eye pose + asymmetric FOV for each eye.
            // No common-center pose and no cyclopean/asymmetry remapping in MONO.
            const bool legacy_good_mono = stereo_experiment_.load(std::memory_order_acquire) == 0 && !flat_ui_projection;
            for (int eye = 0; eye < 2; ++eye)
            {
                const int source_eye = swap_eyes ? (eye ^ 1) : eye;
                EyeSwapchain& e = eyes_[source_eye];
                proj_views[eye].type = XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW;
                if (flat_ui_projection) {
                    proj_views[eye].pose = flat_pose[eye];
                    proj_views[eye].fov = flat_fov[eye];
                } else {
                    proj_views[eye].pose = (projection_pose_mode_.load(std::memory_order_relaxed)==1 && frozen_projection_valid_.load(std::memory_order_relaxed))
                        ? frozen_projection_eye_pose_[eye]
                        : layer_frame.eye_pose[eye];
                    proj_views[eye].fov = layer_frame.eye_fov[eye];
                }
                proj_views[eye].subImage.swapchain = e.swapchain;
                proj_views[eye].subImage.imageRect.offset = { 0, 0 };
                proj_views[eye].subImage.imageRect.extent = { e.width, e.height };
            }
            if (legacy_good_mono) {
                static std::atomic<unsigned long long> mono_log{0};
                const auto n = mono_log.fetch_add(1, std::memory_order_relaxed) + 1;
                if (n <= 4 || (n % 300) == 0) {
                    char m[320]{};
                    std::snprintf(m,sizeof(m),
                        "V61.4-LEGACY-GOOD-MONO per-eye OpenXR poses/FOVs restored paired=%s Lx=%.5f Rx=%.5f",
                        layer_frame_paired?"true":"false",
                        layer_frame.eye_pose[0].position.x, layer_frame.eye_pose[1].position.x);
                    debug_trace::Write(m);
                }
            }
        }
    }

    static std::atomic<unsigned long long> pair_log_count{0};
    const auto plc = pair_log_count.fetch_add(1, std::memory_order_relaxed) + 1;
    if (have_layer && (plc <= 8 || (plc % 300) == 0)) {
        const auto pcf = last_submitted_camera_frame_.load(std::memory_order_acquire);
        char pm[256]{};
        std::snprintf(pm,sizeof(pm),"V47.0-POSE-PAIR currentXr=%llu presentSerial=%llu renderPoseFrame=%llu mode=%d paired=%s",
            static_cast<unsigned long long>(active_frame.frame_index),
            static_cast<unsigned long long>(last_submitted_present_serial_.load(std::memory_order_acquire)),
            static_cast<unsigned long long>(pcf), present_pose_pairing_mode_.load(std::memory_order_relaxed),
            layer_frame_paired ? "true" : "false");
        debug_trace::Write(pm);
    }

    // V58.7: immersive gameplay uses the projection layer. F6 no longer tries
    // to counter-warp that projection; it is a genuine LOCAL-space world quad.
    // F7 remains an additive UI layer on top of either presentation.
    XrCompositionLayerProjection projection_layer{ XR_TYPE_COMPOSITION_LAYER_PROJECTION };
    projection_layer.space = local_space_;
    projection_layer.viewCount = 2;
    projection_layer.views = proj_views;

    const bool flat_game_requested = have_layer && flat_ui_mode_.load(std::memory_order_acquire);
    bool have_flat_game_quad = false;
    XrCompositionLayerQuad flat_game_quad{ XR_TYPE_COMPOSITION_LAYER_QUAD };
    if (flat_game_requested) {
        XrPosef anchor{};
        bool anchor_valid = false;
        {
            std::lock_guard<std::mutex> lock(flat_ui_mutex_);
            anchor = flat_ui_anchor_pose_;
            anchor_valid = flat_ui_anchor_valid_;
        }
        if (anchor_valid) {
            const XrQuaternionf level_q = QYawOnly(anchor.orientation);
            const float distance = std::clamp(flat_ui_distance_.load(std::memory_order_acquire),0.80f,3.20f);
            const V3f forward = QRotate(level_q, {0.0f,0.0f,-distance});
            flat_game_quad.space = local_space_;
            flat_game_quad.eyeVisibility = XR_EYE_VISIBILITY_BOTH;
            flat_game_quad.subImage.swapchain = eyes_[0].swapchain;
            flat_game_quad.subImage.imageRect.offset = {0,0};
            flat_game_quad.subImage.imageRect.extent = {eyes_[0].width,eyes_[0].height};
            flat_game_quad.pose.orientation = level_q;
            flat_game_quad.pose.position = {
                anchor.position.x + forward.x,
                anchor.position.y + forward.y,
                anchor.position.z + forward.z
            };
            const float ui_scale = std::clamp(flat_ui_scale_.load(std::memory_order_acquire),0.60f,3.00f);
            flat_game_quad.size.width = std::clamp(1.82f * (1.58f / ui_scale),1.05f,2.65f);
            // V59.7: F6 must preserve the actual KCD2 source aspect. The old
            // hard-coded 16:9 quad squashed 1920x1800 / near-square sources.
            const float sw=static_cast<float>(std::max<std::uint32_t>(1,last_source_width_.load(std::memory_order_acquire)));
            const float sh=static_cast<float>(std::max<std::uint32_t>(1,last_source_height_.load(std::memory_order_acquire)));
            const float source_aspect=std::clamp(sw/sh,0.75f,2.50f);
            flat_game_quad.size.height = flat_game_quad.size.width / source_aspect;
            have_flat_game_quad = true;
        }
    }

    XrCompositionLayerQuad menu_quad{ XR_TYPE_COMPOSITION_LAYER_QUAD };
    if (have_ui_layer) {
        menu_quad.layerFlags = XR_COMPOSITION_LAYER_BLEND_TEXTURE_SOURCE_ALPHA_BIT;
        menu_quad.space = view_space_;
        menu_quad.eyeVisibility = XR_EYE_VISIBILITY_BOTH;
        menu_quad.subImage.swapchain = menu_ui_.swapchain;
        menu_quad.subImage.imageRect.offset = {0,0};
        menu_quad.subImage.imageRect.extent = {menu_ui_.width,menu_ui_.height};
        menu_quad.pose.orientation = {0.0f,0.0f,0.0f,1.0f};
        menu_quad.pose.position = {0.0f,0.0f,-1.45f};
        menu_quad.size.width = 1.64f; // V61.27: 20% smaller; texture remains 1100x680
        menu_quad.size.height = menu_quad.size.width * (static_cast<float>(menu_ui_.height) / static_cast<float>(menu_ui_.width));
    }

    // V61.31 right-arm laser pointer. Reuse the 3x3 solid-gold texel in the
    // menu texture as a thin billboard ribbon from the live right grip pose to
    // the exact menu hit. This is a composition layer only: no game backbuffer,
    // game cursor, camera, or controller pose is modified.
    XrCompositionLayerQuad menu_laser_quad{ XR_TYPE_COMPOSITION_LAYER_QUAD };
    bool have_menu_laser=false;
    if(have_ui_layer && menu_laser_valid){
        V3f beam{menu_laser_end.x-menu_laser_start.x,menu_laser_end.y-menu_laser_start.y,menu_laser_end.z-menu_laser_start.z};
        const float beam_len=VLength(beam);
        if(beam_len>0.03f && std::isfinite(beam_len)){
            const V3f xaxis=VNormalize(beam);
            const V3f center{(menu_laser_start.x+menu_laser_end.x)*0.5f,(menu_laser_start.y+menu_laser_end.y)*0.5f,(menu_laser_start.z+menu_laser_end.z)*0.5f};
            const V3f to_eye=VNormalize({-center.x,-center.y,-center.z});
            V3f yaxis=VCross(to_eye,xaxis);
            if(VLength(yaxis)<1.0e-4f) yaxis=VCross({0.0f,1.0f,0.0f},xaxis);
            if(VLength(yaxis)<1.0e-4f) yaxis=VCross({1.0f,0.0f,0.0f},xaxis);
            yaxis=VNormalize(yaxis);
            const V3f zaxis=VNormalize(VCross(xaxis,yaxis));
            if(VLength(yaxis)>0.5f && VLength(zaxis)>0.5f){
                menu_laser_quad.layerFlags=XR_COMPOSITION_LAYER_BLEND_TEXTURE_SOURCE_ALPHA_BIT;
                menu_laser_quad.space=view_space_;
                menu_laser_quad.eyeVisibility=XR_EYE_VISIBILITY_BOTH;
                menu_laser_quad.subImage.swapchain=menu_ui_.swapchain;
                menu_laser_quad.subImage.imageRect.offset={0,0};
                menu_laser_quad.subImage.imageRect.extent={3,3};
                menu_laser_quad.pose.orientation=QFromAxes(xaxis,yaxis,zaxis);
                menu_laser_quad.pose.position={center.x,center.y,center.z};
                menu_laser_quad.size.width=beam_len;
                menu_laser_quad.size.height=0.008f;
                have_menu_laser=true;
                static bool s_logged_menu_laser=false;
                if(!s_logged_menu_laser){
                    s_logged_menu_laser=true;
                    debug_trace::Write("V61.32-MENU-LASER OpenXR right-grip ribbon active; UI-only composition layer, exact menu ray unchanged");
                }
            }
        }
    }

#if defined(XR_KHR_COMPOSITION_LAYER_CYLINDER_EXTENSION_NAME)
    XrCompositionLayerCylinderKHR menu_cylinder{ XR_TYPE_COMPOSITION_LAYER_CYLINDER_KHR };
    const bool use_menu_cylinder = false; // V61.13: flat quad required for exact controller-ray pointer
    if (use_menu_cylinder) {
        menu_cylinder.layerFlags = XR_COMPOSITION_LAYER_BLEND_TEXTURE_SOURCE_ALPHA_BIT;
        menu_cylinder.space = view_space_;
        menu_cylinder.eyeVisibility = XR_EYE_VISIBILITY_BOTH;
        menu_cylinder.subImage.swapchain = menu_ui_.swapchain;
        menu_cylinder.subImage.imageRect.offset = {0,0};
        menu_cylinder.subImage.imageRect.extent = {menu_ui_.width,menu_ui_.height};
        menu_cylinder.pose.orientation = {0.0f,0.0f,0.0f,1.0f};
        // pose is the center point of the visible cylinder section in view space.
        menu_cylinder.pose.position = {0.0f,0.0f,-1.35f};
        menu_cylinder.radius = 1.65f;
        menu_cylinder.centralAngle = 1.05f;
        menu_cylinder.aspectRatio = static_cast<float>(menu_ui_.width) / static_cast<float>(menu_ui_.height);
    }
#else
    const bool use_menu_cylinder = false;
#endif

    const XrCompositionLayerBaseHeader* layers[3]{};
    uint32_t layer_count=0;
    if (have_layer) {
        if (flat_game_requested) {
            if (have_flat_game_quad) layers[layer_count++] = reinterpret_cast<const XrCompositionLayerBaseHeader*>(&flat_game_quad);
        } else {
            layers[layer_count++] = reinterpret_cast<const XrCompositionLayerBaseHeader*>(&projection_layer);
        }
    }
    if (have_layer && have_ui_layer) {
#if defined(XR_KHR_COMPOSITION_LAYER_CYLINDER_EXTENSION_NAME)
        if (use_menu_cylinder) layers[layer_count++] = reinterpret_cast<const XrCompositionLayerBaseHeader*>(&menu_cylinder);
        else
#endif
        layers[layer_count++] = reinterpret_cast<const XrCompositionLayerBaseHeader*>(&menu_quad);
        if(have_menu_laser) layers[layer_count++] = reinterpret_cast<const XrCompositionLayerBaseHeader*>(&menu_laser_quad);
    }

    XrFrameEndInfo end_info{ XR_TYPE_FRAME_END_INFO };
    end_info.displayTime = have_active_frame ? active_frame.predicted_display_time : frame_state.predictedDisplayTime;
    end_info.environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE;
    end_info.layerCount = layer_count;
    end_info.layers = layer_count ? layers : nullptr;
    static bool last_flat_game_quad=false;
    if (have_flat_game_quad != last_flat_game_quad) {
        debug_trace::Write(have_flat_game_quad
            ? "V58.7-F6-LAYER LOCAL quad active; HMD movement is independent of screen pose"
            : "V58.7-F6-LAYER quad inactive; immersive projection active");
        last_flat_game_quad=have_flat_game_quad;
    }
    static bool last_menu_layer_visible=false;
    static bool last_menu_layer_cylinder=false;
    if (have_ui_layer != last_menu_layer_visible || (have_ui_layer && use_menu_cylinder != last_menu_layer_cylinder)) {
        if (!have_ui_layer) debug_trace::Write("V58-F7-MENU OFF gameplay projection remains active");
        else if (use_menu_cylinder) debug_trace::Write("V58-F7-MENU ON CYLINDER radius=1.55m angle=0.95rad; controller ray disabled; LEFT-stick nav + A/trigger active");
        else debug_trace::Write("V58-F7-MENU ON QUAD distance=1.35m width=1.20m");
        last_menu_layer_visible=have_ui_layer;
        last_menu_layer_cylinder=use_menu_cylinder;
    }
    XrResult end_result = xrEndFrame(session_, &end_info);
    if (XR_FAILED(end_result)) {
        debug_trace::Write("xrEndFrame failed");
        return false;
    }

    return got_pose;
}

bool XrManager::SampleHeadPoseAtTime(XrTime target_time, XrPose& out_pose)
{
    if (!session_running_.load(std::memory_order_acquire) || target_time <= 0 ||
        view_space_ == XR_NULL_HANDLE || local_space_ == XR_NULL_HANDLE) return false;
    XrSpaceLocation location{ XR_TYPE_SPACE_LOCATION };
    {
        std::lock_guard<std::mutex> lock(pose_locate_mutex_);
        if (XR_FAILED(xrLocateSpace(view_space_, local_space_, target_time, &location))) return false;
    }
    const XrSpaceLocationFlags need = XR_SPACE_LOCATION_POSITION_VALID_BIT | XR_SPACE_LOCATION_ORIENTATION_VALID_BIT;
    if ((location.locationFlags & need) != need) return false;
    out_pose.pos_x=location.pose.position.x; out_pose.pos_y=location.pose.position.y; out_pose.pos_z=location.pose.position.z;
    out_pose.qx=location.pose.orientation.x; out_pose.qy=location.pose.orientation.y; out_pose.qz=location.pose.orientation.z; out_pose.qw=location.pose.orientation.w;
    return true;
}

bool XrManager::SampleHeadPoseNow(XrPose& out_pose, XrTime* out_time)
{
    if (!session_running_.load(std::memory_order_acquire) || !qpc_time_extension_enabled_ || !qpc_to_xr_time_) return false;
    LARGE_INTEGER qpc{}; if (!QueryPerformanceCounter(&qpc)) return false;
    XrTime now=0; if (XR_FAILED(qpc_to_xr_time_(instance_, &qpc, &now))) return false;
    if (out_time) *out_time=now;
    return SampleHeadPoseAtTime(now,out_pose);
}

bool XrManager::SampleLateHeadPose(XrPose& out_pose)
{
    if (!late_pose_enabled_.load(std::memory_order_acquire) ||
        !session_running_.load(std::memory_order_acquire) ||
        !qpc_time_extension_enabled_ || !qpc_to_xr_time_ ||
        view_space_ == XR_NULL_HANDLE || local_space_ == XR_NULL_HANDLE)
        return false;

    LARGE_INTEGER qpc{};
    if (!QueryPerformanceCounter(&qpc)) return false;
    XrTime now = 0;
    if (XR_FAILED(qpc_to_xr_time_(instance_, &qpc, &now))) {
        late_pose_failures_.fetch_add(1, std::memory_order_relaxed);
        return false;
    }

    XrSpaceLocation location{ XR_TYPE_SPACE_LOCATION };
    {
        std::lock_guard<std::mutex> lock(pose_locate_mutex_);
        if (XR_FAILED(xrLocateSpace(view_space_, local_space_, now, &location))) {
            late_pose_failures_.fetch_add(1, std::memory_order_relaxed);
            return false;
        }
    }
    const XrSpaceLocationFlags need = XR_SPACE_LOCATION_POSITION_VALID_BIT | XR_SPACE_LOCATION_ORIENTATION_VALID_BIT;
    if ((location.locationFlags & need) != need) {
        late_pose_failures_.fetch_add(1, std::memory_order_relaxed);
        return false;
    }
    out_pose.pos_x = location.pose.position.x; out_pose.pos_y = location.pose.position.y; out_pose.pos_z = location.pose.position.z;
    out_pose.qx = location.pose.orientation.x; out_pose.qy = location.pose.orientation.y; out_pose.qz = location.pose.orientation.z; out_pose.qw = location.pose.orientation.w;
    const auto n = late_pose_samples_.fetch_add(1, std::memory_order_relaxed) + 1;
    if (n == 1 || (n % 1000ull) == 0) {
        char m[192]{}; std::snprintf(m, sizeof(m), "V48-LATE-POSE samples=%llu failures=%llu",
            static_cast<unsigned long long>(n), static_cast<unsigned long long>(late_pose_failures_.load(std::memory_order_relaxed)));
        debug_trace::Write(m);
    }
    return true;
}

bool XrManager::ConsumeRecenterRequest()
{
    if (!input_.RecenterRequested()) return false;
    input_.ClearRecenterRequest();
    return true;
}

void XrManager::Shutdown()
{
    session_running_.store(false, std::memory_order_relaxed);
    DeviceHook::SetInternalVrCommandList(nullptr);
    DeviceHook::SetAfrStereoCapture(false);

    {
        std::lock_guard<std::mutex> lock(render_mutex_);
        render_request_pending_ = false;
        render_request_done_ = true;
        render_request_success_ = false;
    }
    render_cv_.notify_all();

    input_.Shutdown();

    if (fence_ && fence_->GetCompletedValue() < fence_value_)
    {
        if (fence_event_ && SUCCEEDED(fence_->SetEventOnCompletion(fence_value_, fence_event_)))
            WaitForSingleObject(fence_event_, INFINITE);
    }

    DestroyAfrCaches();
    DestroySwapchains();

    if (view_space_ != XR_NULL_HANDLE) { xrDestroySpace(view_space_); view_space_ = XR_NULL_HANDLE; }
    if (local_space_ != XR_NULL_HANDLE) { xrDestroySpace(local_space_); local_space_ = XR_NULL_HANDLE; }
    if (session_ != XR_NULL_HANDLE)
    {
        xrEndSession(session_);
        xrDestroySession(session_);
        session_ = XR_NULL_HANDLE;
    }

    rtv_entries_.clear();
    if (blit_pipeline_state_) { blit_pipeline_state_->Release(); blit_pipeline_state_ = nullptr; }
    if (blit_root_signature_) { blit_root_signature_->Release(); blit_root_signature_ = nullptr; }
    if (srv_heap_) { srv_heap_->Release(); srv_heap_ = nullptr; }
    if (rtv_heap_) { rtv_heap_->Release(); rtv_heap_ = nullptr; }
    if (fence_event_) { CloseHandle(fence_event_); fence_event_ = nullptr; }
    if (fence_) { fence_->Release(); fence_ = nullptr; }
    if (command_list_) { command_list_->Release(); command_list_ = nullptr; }
    if (command_allocator_) { command_allocator_->Release(); command_allocator_ = nullptr; }

    if (d3d12_queue_) { d3d12_queue_->Release(); d3d12_queue_ = nullptr; }
    if (d3d12_device_) { d3d12_device_->Release(); d3d12_device_ = nullptr; }
    if (instance_ != XR_NULL_HANDLE) { xrDestroyInstance(instance_); instance_ = XR_NULL_HANDLE; }

    fence_value_ = 0;
    swapchain_format_ = DXGI_FORMAT_UNKNOWN;
    init_attempted_ = false;
}

void XrManager::DestroySwapchains()
{
    for (int eye = 0; eye < 2; ++eye)
    {
        EyeSwapchain& e = eyes_[eye]; e.d3d12_images.clear();
        if (e.swapchain != XR_NULL_HANDLE) { xrDestroySwapchain(e.swapchain); e.swapchain = XR_NULL_HANDLE; }
    }
    menu_ui_.d3d12_images.clear();
    if(menu_ui_.swapchain != XR_NULL_HANDLE) { xrDestroySwapchain(menu_ui_.swapchain); menu_ui_.swapchain=XR_NULL_HANDLE; }
}

} // namespace kcd2vr
