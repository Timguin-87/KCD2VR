#include "vrik/hand_prediction_d69.h"
#include "xr_eye_srgb_r114.h" // R114: sRGB eye swapchain preference
#include "diagnostics/log_sink_d48.h"
#include "diagnostics/motion_trace_d68.h"
// xr_manager.cpp
#include "xr_manager.h"
#include "subsystems/dice/selection_marker_r92.h"
#include "subsystems/dice/hud_depth_r92r3.h"
#include "activity_input163.h"
#include "core/publication_wait_d67.h"
#include "subsystems/experimental/deepseek_options_h29.h"
#include "core/render_pacing59c6.h"
#include "hud_layer_v6664.h"
#include "debug_trace.h"
#include "diagnostic_trace.h"
#include "diagnostics/xr_timing165.h"
#include "device_hook.h"
#include "frame_sync.h"
#include "native_stereo_outer_lab.h"
#include "vrik/hand_same_frame_hj1.h"
#include "vrik/smooth_lab_f2.h"
#include "vrik/motion_trials_d67.h"
#include <DetourModKit/logger.hpp>
#include <string>
#include <cstring>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <chrono>
#include "xr_layer_guard_r104.h"
#include "vrik/start_facing_r148.h" // R148: head tracked flag + runtime name for the start/load facing recenter
#include <cmath>
#include <d3dcompiler.h>

namespace kcd2vr {

// Extra hand prediction is an opt-in target offset. The head target, source
// clocks and stereo work are unchanged. Each caller passes velocity from its
// OWN base locate; no velocity is borrowed from another family.
static void ApplyExtraHandsD69(XrInput& input,tracking59c6::Frame& family,
        const XrSpaceVelocity (&velocity)[2],std::int64_t extra,diagnostics::motion_d68::Event* deferredTrace=nullptr) {
    if(extra<=0)return;
    namespace md=diagnostics::motion_d68;md::Event trace{};static thread_local md::Cadence cadence;
    bool tracing=md::begin(trace,md::HandPrediction)&&md::inWindow(trace);
    if(tracing&&!cadence.due(trace.session,trace.ns)){md::noteCadenceSkip(md::HandPrediction);tracing=false;}
    if(tracing)for(unsigned h=0;h<2;++h){const auto& p=family.poses[h+1];const float q[]{p.x,p.y,p.z,p.qx,p.qy,p.qz,p.qw};std::copy_n(q,7,trace.v+h*7);}
    hand_prediction_d69::Velocity v[2]{};
    for(unsigned h=0;h<2;++h){const auto& x=velocity[h];
        v[h]={{x.linearVelocity.x,x.linearVelocity.y,x.linearVelocity.z},
              {x.angularVelocity.x,x.angularVelocity.y,x.angularVelocity.z},
              (x.velocityFlags&XR_SPACE_VELOCITY_LINEAR_VALID_BIT)!=0,(x.velocityFlags&XR_SPACE_VELOCITY_ANGULAR_VALID_BIT)!=0};}
    hand_prediction_d69::Apply(family,v,extra,[&](std::int64_t target,tracking59c6::Pose (&out)[2],bool (&tracked)[2]){
        XrPosef p[2]{};XrSpaceLocationFlags flags[2]{};
        if(!input.TryLocateGripPair(target,p,flags))return false;
        constexpr auto need=XR_SPACE_LOCATION_POSITION_VALID_BIT|XR_SPACE_LOCATION_ORIENTATION_VALID_BIT|
            XR_SPACE_LOCATION_POSITION_TRACKED_BIT|XR_SPACE_LOCATION_ORIENTATION_TRACKED_BIT;
        for(unsigned h=0;h<2;++h){out[h]={p[h].position.x,p[h].position.y,p[h].position.z,
            p[h].orientation.x,p[h].orientation.y,p[h].orientation.z,p[h].orientation.w};tracked[h]=(flags[h]&need)==need;}
        return true;
    });
    // Effective offset changes (including a one-hand failed future locate) retire
    // intent derivatives. Switching velocity/future-locate at the SAME target does not.
    family.handPolicyD69=hand_prediction_d69::EffectivePolicy(family,extra);
    if(tracing){trace.sourceNs=family.ns;trace.sourceTick=family.tick;trace.trackingEpoch=family.epoch;trace.targetXrNs=family.locateTime;
        trace.leftTargetD69=family.handTargetD69[0];trace.rightTargetD69=family.handTargetD69[1];trace.handPolicyD69=family.handPolicyD69;
        trace.leftLeadD69=family.handLeadSourceD69[0];trace.rightLeadD69=family.handLeadSourceD69[1];trace.thread=GetCurrentThreadId();
        for(unsigned h=0;h<2;++h){const auto& p=family.poses[h+1];const float q[]{p.x,p.y,p.z,p.qx,p.qy,p.qz,p.qw};std::copy_n(q,7,trace.v+14+h*7);
            std::copy_n(v[h].linear,3,trace.v+28+h*6);std::copy_n(v[h].angular,3,trace.v+31+h*6);trace.flags|=(unsigned(v[h].linearValid)<<(h*2))|(unsigned(v[h].angularValid)<<(h*2+1));}
        trace.count=40;trace.endNs=diagnostics::perf_d61::nowNs();if(deferredTrace)*deferredTrace=trace;else (void)md::push(trace);
    }
}

namespace {
static DWORD TimedFenceWait165(HANDLE event,DWORD timeout) noexcept {
    diagnostics::xr165::Timer timing(diagnostics::xr165::FenceWait);
    return WaitForSingleObject(event,timeout);
}
static XrResult TimedAcquireImage59c6(XrSwapchain chain,const XrSwapchainImageAcquireInfo* info,std::uint32_t* index) noexcept {
    diagnostics::xr165::Timer timing(diagnostics::xr165::AcquireImage);
    return xrAcquireSwapchainImage(chain,info,index);
}
static XrResult TimedSwapchainWait165(XrSwapchain chain,const XrSwapchainImageWaitInfo* info) noexcept {
    diagnostics::xr165::Timer timing(diagnostics::xr165::SwapchainWait);
    return xrWaitSwapchainImage(chain,info);
}
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
    // R109 (ChatGPT review): the heading is the horizontal projection of the forward vector: its denominator is
    // 1 - 2(x^2 + y^2), not 1 - 2(y^2 + z^2) (that one is the X axis). With roll or a strong pitch the old value
    // turned the anchored screens the wrong way (the inverted SteamVR start pose gave 174 deg instead of 5).
    const float siny = 2.0f * (q.w*q.y + q.x*q.z);
    const float cosy = 1.0f - 2.0f * (q.x*q.x + q.y*q.y);
    const float yaw = std::atan2(siny, cosy);
    const float h = yaw * 0.5f;
    return {0.0f, std::sin(h), 0.0f, std::cos(h)};
}

static XrQuaternionf QGazeNoRoll(const XrQuaternionf& q){
    // V66.20: large/fullscreen menus should appear exactly where the player was
    // looking when the UI opened, but must not inherit headset roll. Preserve the
    // captured yaw+pitch by rebuilding an orthonormal frame from the HMD forward
    // vector and world-up. Local -Z remains the plane's forward direction.
    V3f forward=VNormalize(QRotate(q,{0.0f,0.0f,-1.0f}));
    if(VLength(forward)<0.5f) return QYawOnly(q);
    V3f z{-forward.x,-forward.y,-forward.z};
    V3f x=VCross({0.0f,1.0f,0.0f},z);
    if(VLength(x)<1.0e-4f) return QYawOnly(q);
    x=VNormalize(x);
    const V3f y=VNormalize(VCross(z,x));
    if(VLength(y)<0.5f) return QYawOnly(q);
    return QFromAxes(x,y,z);
}


}


namespace {
// C22b (C20 port): small quaternion helpers for the lagging HUD.
XrQuaternionf QNormalize20(const XrQuaternionf& q){
    const float n=std::sqrt(q.x*q.x+q.y*q.y+q.z*q.z+q.w*q.w);
    return (std::isfinite(n)&&n>1.0e-6f)?XrQuaternionf{q.x/n,q.y/n,q.z/n,q.w/n}:XrQuaternionf{0,0,0,1};}
XrQuaternionf QNlerp20(const XrQuaternionf& a,XrQuaternionf b,float t){
    if(a.x*b.x+a.y*b.y+a.z*b.z+a.w*b.w<0.0f)b={-b.x,-b.y,-b.z,-b.w};
    return QNormalize20({a.x+(b.x-a.x)*t,a.y+(b.y-a.y)*t,a.z+(b.z-a.z)*t,a.w+(b.w-a.w)*t});}
float QAngle20(const XrQuaternionf& a,const XrQuaternionf& b){
    const float d=std::fabs(a.x*b.x+a.y*b.y+a.z*b.z+a.w*b.w);return 2.0f*std::acos(std::min(1.0f,d));}
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
            KCD2VR_DIAG_LOG(Bug,error,"xrCreateInstance failed - code={}", static_cast<int>(result));
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
        if(qpc_time_extension_enabled_){
            kcd2vr::hj1::nativeFrame.store(&native_stereo_outer_lab::CurrentNativeFrameHJ1,std::memory_order_release);
            kcd2vr::hj1::locate.store(static_cast<kcd2vr::hj1::LocateFn>([](kcd2vr::hj1::Sample& s) noexcept -> bool {
                const auto epoch=tracking59c6::epoch.load(std::memory_order_acquire);
                XrPosef p[3]{};XrSpaceLocationFlags f[3]{};XrTime target=0,now=0;int ahead=0;diagnostics::motion_d68::Event predictionTraceD69{};
                s.tick=0;s.steadyNs=0; // active trial returns the base-family completion stamp, before extra locate
                if(!XrManager::Instance().LocateFamilyForGameFrameHJ1(p,f,target,ahead,now,&s,&predictionTraceD69)||epoch!=tracking59c6::epoch.load(std::memory_order_acquire))return false;
                const XrSpaceLocationFlags need=XR_SPACE_LOCATION_POSITION_VALID_BIT|XR_SPACE_LOCATION_ORIENTATION_VALID_BIT;
                const XrSpaceLocationFlags tracked=XR_SPACE_LOCATION_POSITION_TRACKED_BIT|XR_SPACE_LOCATION_ORIENTATION_TRACKED_BIT;
                auto pose=[](const XrPosef& p){return kcd2vr::hj1::Pose7{{p.position.x,p.position.y,p.position.z},{p.orientation.x,p.orientation.y,p.orientation.z,p.orientation.w}};};
                s.head=pose(p[0]);s.headValid=(f[0]&need)==need;s.headTracked=(f[0]&tracked)==tracked;
                for(int i=0;i<2;++i){s.valid[i]=(f[i+1]&need)==need;s.tracked[i]=(f[i+1]&tracked)==tracked;s.hand[i]=pose(p[i+1]);}
                s.sourceEpoch=epoch;if(!s.tick){s.tick=std::uint64_t(GetTickCount64());s.steadyNs=tracking59c6::NowNs();}s.sequence=tracking59c6::Next();
                s.target=target;s.readXrNow=now;s.ahead=ahead;s.ok=s.headValid&&s.valid[0]&&s.valid[1];
                if(predictionTraceD69.session){predictionTraceD69.sourceNs=s.steadyNs;predictionTraceD69.sourceTick=s.tick;predictionTraceD69.trackingEpoch=s.sourceEpoch;predictionTraceD69.sample=s.sequence;diagnostics::motion_d68::push(predictionTraceD69);}return s.ok;
            }),std::memory_order_release);
        }else{
            diagnostic_trace::Write("HJ1 HAND_SAME_FRAME off: runtime has no XR_KHR_win32_convert_performance_counter_time, hands keep the XR-thread sample");
        }
        debug_trace::Write(cylinder_layer_supported_
            ? "V58-MENU XR_KHR_composition_layer_cylinder enabled"
            : "V58-MENU cylinder extension unavailable; quad fallback will be used");
        return true;
    }
    catch (...) {
        KCD2VR_DIAG_LOG(Bug,error,"Exception caught in CreateInstance()");
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
        KCD2VR_DIAG_LOG(Bug,error,"xrGetSystem failed - code={}", static_cast<int>(result));
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
    KCD2VR_DIAG_LOG(Bug,info,"XR-RUNTIME-V29 runtime='{}' runtimeVersion={}.{}.{} instanceResult={} system='{}' vendorId={} systemResult={} stereoViews={} viewEnumResult={}",
        XR_SUCCEEDED(ipr) ? instance_props.runtimeName : "<unknown>",
        XR_SUCCEEDED(ipr) ? XR_VERSION_MAJOR(instance_props.runtimeVersion) : 0,
        XR_SUCCEEDED(ipr) ? XR_VERSION_MINOR(instance_props.runtimeVersion) : 0,
        XR_SUCCEEDED(ipr) ? XR_VERSION_PATCH(instance_props.runtimeVersion) : 0,
        static_cast<int>(ipr), XR_SUCCEEDED(spr) ? system_props.systemName : "<unknown>",
        XR_SUCCEEDED(spr) ? system_props.vendorId : 0u, static_cast<int>(spr), view_count, static_cast<int>(vcr));
    { // R102: runtime and headset always go to KCD2VR.log (SteamVR vs VDXR problems must be visible without F2).
        char rt[480]{};std::snprintf(rt,sizeof(rt),"R102 XR_RUNTIME runtime='%s' version=%u.%u.%u system='%s' vendorId=%u maxLayers=%u maxSwapchain=%ux%u",
            XR_SUCCEEDED(ipr)?instance_props.runtimeName:"<unknown>",
            XR_SUCCEEDED(ipr)?XR_VERSION_MAJOR(instance_props.runtimeVersion):0u,XR_SUCCEEDED(ipr)?XR_VERSION_MINOR(instance_props.runtimeVersion):0u,
            XR_SUCCEEDED(ipr)?XR_VERSION_PATCH(instance_props.runtimeVersion):0u,XR_SUCCEEDED(spr)?system_props.systemName:"<unknown>",
            XR_SUCCEEDED(spr)?system_props.vendorId:0u,XR_SUCCEEDED(spr)?system_props.graphicsProperties.maxLayerCount:0u,
            XR_SUCCEEDED(spr)?system_props.graphicsProperties.maxSwapchainImageWidth:0u,XR_SUCCEEDED(spr)?system_props.graphicsProperties.maxSwapchainImageHeight:0u);
        kcd2vr::start_facing_r148::setRuntime(XR_SUCCEEDED(ipr)?instance_props.runtimeName:"<unknown>"); // R148: named in the AUTO_RECENTER row
        diagnostic_trace::WriteStatus(rt);
        max_layer_count_r104_=XR_SUCCEEDED(spr)&&system_props.graphicsProperties.maxLayerCount?system_props.graphicsProperties.maxLayerCount:16u;
    }
    for (uint32_t i=0; i<view_count && i<4 && i<views.size(); ++i) {
        const auto& v=views[i];
        KCD2VR_DIAG_LOG(Bug,info,"XR-RUNTIME-V29 view={} recommended={}x{} max={}x{} samplesRec={} samplesMax={}",
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
        KCD2VR_DIAG_LOG(Bug,error,"Captured D3D12 queue is not DIRECT (type={})", static_cast<int>(queue_desc.Type));
        return false;
    }

    HRESULT hr = d3d12_device_->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&command_allocator_));
    if (FAILED(hr))
    {
        KCD2VR_DIAG_LOG(Bug,error,"CreateCommandAllocator failed, hr=0x{:08X}", static_cast<unsigned>(hr));
        return false;
    }

    hr = d3d12_device_->CreateCommandList(
        0, D3D12_COMMAND_LIST_TYPE_DIRECT, command_allocator_, nullptr,
        IID_PPV_ARGS(&command_list_));
    if (FAILED(hr))
    {
        KCD2VR_DIAG_LOG(Bug,error,"CreateCommandList failed, hr=0x{:08X}", static_cast<unsigned>(hr));
        return false;
    }

    DeviceHook::SetInternalVrCommandList(command_list_);

    // A newly created command list starts open; close it so the first RunFrame
    // can Reset() it before recording commands.
    hr = command_list_->Close();
    if (FAILED(hr))
    {
        KCD2VR_DIAG_LOG(Bug,error,"Initial command-list Close failed, hr=0x{:08X}", static_cast<unsigned>(hr));
        return false;
    }

    hr = d3d12_device_->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
    if (FAILED(hr))
    {
        KCD2VR_DIAG_LOG(Bug,error,"CreateFence failed, hr=0x{:08X}", static_cast<unsigned>(hr));
        return false;
    }

    fence_event_ = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!fence_event_)
    {
        KCD2VR_DIAG_LOG(Bug,error,"CreateEventW failed for D3D12 fence");
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
        KCD2VR_DIAG_LOG(Bug,error,"xrGetD3D12GraphicsRequirementsKHR is unavailable - code={}",
                                  static_cast<int>(proc_result));
        return false;
    }

    XrGraphicsRequirementsD3D12KHR requirements{ XR_TYPE_GRAPHICS_REQUIREMENTS_D3D12_KHR };
    XrResult result = get_requirements(instance_, system_id_, &requirements);
    if (XR_FAILED(result))
    {
        KCD2VR_DIAG_LOG(Bug,error,"xrGetD3D12GraphicsRequirementsKHR failed - code={}", static_cast<int>(result));
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
        KCD2VR_DIAG_LOG(Bug,error,"D3D12 CheckFeatureSupport failed, hr=0x{:08X}",
                                  static_cast<unsigned>(hr_check_feature_level_));
        return false;
    }

    KCD2VR_DIAG_LOG(Bug,info,"OpenXR D3D12 requirements: minFeatureLevel={}",
                             static_cast<int>(requirements.minFeatureLevel));
    KCD2VR_DIAG_LOG(Bug,info,"Game D3D12 device max feature level={}",
                             static_cast<int>(feature_levels.MaxSupportedFeatureLevel));

    if (feature_levels.MaxSupportedFeatureLevel < requirements.minFeatureLevel)
    {
        KCD2VR_DIAG_LOG(Bug,error,"Game D3D12 device does not meet OpenXR minimum feature level");
        return false;
    }

    // D3D12 devices expose their adapter LUID directly. Querying IDXGIDevice
    // from ID3D12Device is invalid and returns E_NOINTERFACE on normal D3D12
    // devices. Compare the native D3D12 LUID with the one required by OpenXR.
    const LUID game_adapter_luid = d3d12_device_->GetAdapterLuid();
    hr_check_adapter_ = S_OK;

    if (std::memcmp(&game_adapter_luid, &requirements.adapterLuid, sizeof(LUID)) != 0)
    {
        KCD2VR_DIAG_LOG(Bug,error,
            "OpenXR D3D12 adapter LUID does not match the game's adapter. "
            "Use the same GPU for KCD2, Virtual Desktop VDXR and OpenXR. "
            "game={:08X}:{:08X}, openxr={:08X}:{:08X}",
            static_cast<unsigned>(game_adapter_luid.HighPart),
            static_cast<unsigned>(game_adapter_luid.LowPart),
            static_cast<unsigned>(requirements.adapterLuid.HighPart),
            static_cast<unsigned>(requirements.adapterLuid.LowPart));
        return false;
    }

    KCD2VR_DIAG_LOG(Bug,info,
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
        KCD2VR_DIAG_LOG(Bug,error,"xrCreateSession failed - code={}", static_cast<int>(result));
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

    // V66.107: STAGE (floor-level) space is optional; it only feeds the head-height reference.
    XrReferenceSpaceCreateInfo stage_info{ XR_TYPE_REFERENCE_SPACE_CREATE_INFO };
    stage_info.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_STAGE;
    stage_info.poseInReferenceSpace.orientation.w = 1.0f;
    if (XR_FAILED(xrCreateReferenceSpace(session_, &stage_info, &stage_space_))) stage_space_ = XR_NULL_HANDLE;
    return true;
}

// R104: overlay layer evidence, always in KCD2VR.log (SteamVR vs VDXR must be visible without F2). Few rows:
// first repairs/drops per kind, each refused xrEndFrame (max 1/s), a 30 s summary only when something changed.
static void LayerKindsR104(unsigned kinds,char* out,std::size_t n) noexcept {
    std::size_t at=0;out[0]=0;
    for(unsigned k=0;k<xr_layer_guard_r104::Count&&at+16<n;++k)if((kinds>>k)&1u)
        at+=std::snprintf(out+at,n-at,"%s%s",at?",":"",xr_layer_guard_r104::name(k));
    if(!at)std::snprintf(out,n,"NONE");
}

void XrManager::NoteLayerDropR104(unsigned kind,unsigned drop) noexcept {
    if(kind>=xr_layer_guard_r104::Count)return;
    const auto c=++layer_drop_r104_[kind];
    if(c<=3||(c%500)==0){char m[200]{};std::snprintf(m,sizeof(m),"R104 XR_LAYER dropped kind=%s reason=%s count=%llu (not sent; the rest of the frame is)",
        xr_layer_guard_r104::name(kind),xr_layer_guard_r104::dropName(drop),(unsigned long long)c);diagnostic_trace::WriteStatus(m);}
}

void XrManager::NoteLayerFixR104(unsigned kind,unsigned fixes) noexcept {
    if(kind>=xr_layer_guard_r104::Count)return;
    const auto c=++layer_fix_r104_[kind];
    if(c<=2){char m[200]{};std::snprintf(m,sizeof(m),"R104 XR_LAYER repaired kind=%s fixes=%s%s%s count=%llu",xr_layer_guard_r104::name(kind),
        (fixes&xr_layer_guard_r104::FixQuat)?"QUAT_NORMALIZED ":"",(fixes&xr_layer_guard_r104::FixRect)?"RECT_CLAMPED ":"",
        (fixes&xr_layer_guard_r104::FixSize)?"SIZE_MIN ":"",(unsigned long long)c);diagnostic_trace::WriteStatus(m);}
}

void XrManager::NoteEndFrameR105(unsigned kinds,int result,std::uint32_t sitOutMs,std::uint64_t now) noexcept {
    for(unsigned k=0;k<xr_layer_guard_r104::Count;++k)if((kinds>>k)&1u)++layer_sent_r104_[k];
    char names[200]{};
    if(result>=0)++end_ok_r104_;
    else{
        ++end_fail_r104_;
        if(now>=layer_fail_next_r104_&&layer_fail_rows_r104_<200){
            layer_fail_next_r104_=now+1000;++layer_fail_rows_r104_;LayerKindsR104(kinds,names,sizeof(names));
            const unsigned b=layer_guard_r104_.lastBlamed;
            char m[480]{};std::snprintf(m,sizeof(m),"R106 XR_LAYERS xrEndFrame refused code=%d layers=%s endCalls=1 layerError=%s blamed=%s sitOutMs=%u refusedTotal=%llu okTotal=%llu",
                result,names,xr_layer_guard_r104::layerError(result)?"YES":"NO(nobody sits out)",
                b<xr_layer_guard_r104::Count?xr_layer_guard_r104::name(b):(xr_layer_guard_r104::layerError(result)&&(kinds&~(1u<<xr_layer_guard_r104::FlatGame))?"NONE(all in frame wait 1 s, return one by one)":"NONE"),
                sitOutMs,(unsigned long long)end_fail_r104_,(unsigned long long)end_ok_r104_);
            diagnostic_trace::WriteStatus(m);
        }
    }
    if(kinds!=last_kinds_r104_&&layer_kind_rows_r104_<40&&now>=layer_kind_next_r104_){
        last_kinds_r104_=kinds;layer_kind_next_r104_=now+2000;++layer_kind_rows_r104_;LayerKindsR104(kinds,names,sizeof(names));
        char m[300]{};std::snprintf(m,sizeof(m),"R106 XR_LAYERS submitted=%s (projection not listed) result=%d",names,result);diagnostic_trace::WriteStatus(m);
    }
    if(now>=layer_status_next_r104_){
        layer_status_next_r104_=now+30000;
        const std::uint64_t sig=end_ok_r104_+end_fail_r104_*7919ull;
        if(sig!=layer_status_sig_r104_){layer_status_sig_r104_=sig;
            char m[1400]{};int at=std::snprintf(m,sizeof(m),"R106 XR_LAYERS 30s ok=%llu refused=%llu endCallsPerFrame=1",(unsigned long long)end_ok_r104_,(unsigned long long)end_fail_r104_);
            for(unsigned k=0;k<xr_layer_guard_r104::Count&&at<int(sizeof(m))-120;++k){
                if(!layer_sent_r104_[k]&&!layer_held_r104_[k]&&!layer_drop_r104_[k]&&!layer_fix_r104_[k])continue;
                at+=std::snprintf(m+at,sizeof(m)-at," %s{submitted:%llu held:%llu dropped:%llu repaired:%llu}",xr_layer_guard_r104::name(k),
                    (unsigned long long)layer_sent_r104_[k],(unsigned long long)layer_held_r104_[k],(unsigned long long)layer_drop_r104_[k],(unsigned long long)layer_fix_r104_[k]);
            }
            if(at<int(sizeof(m))-380&&(eye_hold_r141_[0]|eye_hold_r141_[1]|eye_hold_r141_[2]|eye_hold_r141_[3]))
                at+=std::snprintf(m+at,sizeof(m)-at," EYE_HOLD_R141{reshown:%llu noImage:%llu tooOld:%llu mismatch:%llu}",
                    (unsigned long long)eye_hold_r141_[0],(unsigned long long)eye_hold_r141_[1],(unsigned long long)eye_hold_r141_[2],(unsigned long long)eye_hold_r141_[3]);
            if(at<int(sizeof(m))-300){m[at++]=' ';hud_layer_v6664::StatusR104(m+at,sizeof(m)-at);}
            diagnostic_trace::WriteStatus(m);
        }
    }
}

// R111: ONE always-on row every 5 s, written only when the menu counters changed (i.e. while the menu was asked for).
// menu{...} = frames the menu layer was asked for / sent (fresh image or the last released one re-shown) / not sent.
// noNewImage{...} = why no new image was copied that frame (cumulative); notSent{...} = why the layer still missed.
// hud{...} and presentFmt{...} ride along (they never trigger a row by themselves).
void XrManager::NoteMenuFrameR111(std::uint64_t now) noexcept {
    if(now<menu_r111_next_ms_)return;
    menu_r111_next_ms_=now+5000;
    std::uint64_t sig=1469598103934665603ull;
    for(unsigned i=0;i<MenuR111Count;++i){sig^=menu_r111_[i]+0x9E3779B97F4A7C15ull*(i+1);sig*=1099511628211ull;}
    if(sig==menu_r111_sig_)return;
    menu_r111_sig_=sig;
    std::uint64_t hudSkip=0,hudRecopy=0,hudPub=0,hudShown=0;hud_layer_v6664::StatusR111(&hudSkip,&hudRecopy,&hudPub,&hudShown);
    std::uint64_t fmtChanges=0;std::uint32_t fmtNow=0,fmtBefore=0;DeviceHook::PresentFormatChangesR111(&fmtChanges,&fmtNow,&fmtBefore);
    const auto& c=menu_r111_;
    char m[900]{};std::snprintf(m,sizeof(m),
        "R111 XR_UI_LAYERS 5s menu{requested:%llu sent:%llu fresh:%llu reusedLast:%llu notSent:%llu laserSent:%llu sourceChanges:%llu} "
        "noNewImage{eyesNotReady:%llu noSwapchain:%llu noSource:%llu acquireFail:%llu waitFail:%llu copyFail:%llu releaseFail:%llu} "
        "notSent{noScene:%llu sceneCancelled:%llu guard:%llu} hud{published:%llu shown:%llu recopied:%llu emptyCapturesSkipped:%llu} "
        "presentFmt{changes:%llu now:%u before:%u}",
        (unsigned long long)c[MenuRequested],(unsigned long long)c[MenuSent],(unsigned long long)c[MenuFresh],(unsigned long long)c[MenuReused],
        (unsigned long long)c[MenuNotSent],(unsigned long long)c[MenuLaserSent],(unsigned long long)c[MenuSourceChanges],
        (unsigned long long)c[MissEyes],(unsigned long long)c[MissNoSwapchain],(unsigned long long)c[MissNoSource],
        (unsigned long long)c[MissAcquire],(unsigned long long)c[MissWait],(unsigned long long)c[MissCopy],(unsigned long long)c[MissRelease],
        (unsigned long long)c[MissNoScene],(unsigned long long)c[MissSceneCancelled],(unsigned long long)c[MissGuard],
        (unsigned long long)hudPub,(unsigned long long)hudShown,(unsigned long long)hudRecopy,(unsigned long long)hudSkip,
        (unsigned long long)fmtChanges,(unsigned)fmtNow,(unsigned)fmtBefore);
    diagnostic_trace::WriteStatus(m);
}

// R108 (SteamVR start "upside down"): his first SteamVR pose was quaternion (-0.047,-0.033,-0.998,-0.019) = a 180 deg
// roll, head 0.95 m UNDER the floor, flags 0xf (valid+tracked). No human head is ever fully inverted in play, and
// such a pose at start poisons the camera, the recenter and the body frame. A pose whose up vector points down
// (upY = 1 - 2(x^2 + z^2) < 0) is treated as not located: the frame keeps the last good pose, nothing latches,
// the moment the runtime delivers a sane pose everything runs. Few log rows.
static bool HeadUprightR108(const XrQuaternionf& q) noexcept {
    const double upY=1.0-2.0*(double(q.x)*q.x+double(q.z)*q.z);
    if(upY>=0.0||!std::isfinite(upY))return true;
    static std::atomic<unsigned> rows{0};static std::uint64_t next=0;const auto now=std::uint64_t(GetTickCount64());
    if(rows.load()<8&&now>=next){next=now+2000;++rows;char m[200]{};
        std::snprintf(m,sizeof(m),"R108 XR_HEAD inverted pose rejected upY=%.3f q=(%.3f,%.3f,%.3f,%.3f) (runtime start glitch; last good pose kept)",upY,double(q.x),double(q.y),double(q.z),double(q.w));
        diagnostic_trace::WriteStatus(m);}
    return false;
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
    // R114 (xr_eye_srgb_r114.h): sRGB first when VrEyeSrgbR114 is ON - the game image is sRGB-encoded; a UNORM
    // eye swapchain is read as linear by the compositor (VDXR looked overblown, SteamVR sRGB-only looked right).
    const bool eyeSrgbR114=kcd2vr::xr_eye_srgb_r114::eyeSrgb.load(std::memory_order_acquire);
    const DXGI_FORMAT preferred_formats[] = {
        eyeSrgbR114?DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:DXGI_FORMAT_R8G8B8A8_UNORM,
        eyeSrgbR114?DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:DXGI_FORMAT_B8G8R8A8_UNORM,
        eyeSrgbR114?DXGI_FORMAT_R8G8B8A8_UNORM:DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        eyeSrgbR114?DXGI_FORMAT_B8G8R8A8_UNORM:DXGI_FORMAT_B8G8R8A8_UNORM_SRGB
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
        KCD2VR_DIAG_LOG(Bug,error,"No supported standard D3D12 color swapchain format found");
        return false;
    }

    swapchain_format_ = static_cast<DXGI_FORMAT>(swapchain_format);
    {   // R108: the runtime's swapchain formats always in KCD2VR.log (SteamVR "Meta compatibility mode" refused
        // DXGI 28 for the HUD with -26 and nothing said which formats it does accept).
        char m[400]{};int at=std::snprintf(m,sizeof(m),"R108 XR_FORMATS eye=%d menu=%s supported=",int(swapchain_format),
            std::find(supported_formats.begin(),supported_formats.end(),int64_t(DXGI_FORMAT_B8G8R8A8_UNORM))!=supported_formats.end()?"87":
            std::find(supported_formats.begin(),supported_formats.end(),int64_t(DXGI_FORMAT_B8G8R8A8_UNORM_SRGB))!=supported_formats.end()?"91(sRGB fallback)":"NONE(no BGRA: no VR menu/lasers)");
        for(size_t i=0;i<supported_formats.size()&&at<int(sizeof(m))-16;++i)at+=std::snprintf(m+at,sizeof(m)-at,"%s%lld",i?",":"",(long long)supported_formats[i]);
        diagnostic_trace::WriteStatus(m);
    }
    if (!CreateRtvHeap()) return false;

    KCD2VR_DIAG_LOG(Bug,info,"Using D3D12 OpenXR swapchain format={}", static_cast<int>(swapchain_format));

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
            KCD2VR_DIAG_LOG(Bug,error,"xrCreateSwapchain eye={} failed - code={}", eye, static_cast<int>(result));
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
    // R108: SteamVR (Meta compatibility mode) lists only sRGB formats; the menu copy already accepts the whole BGRA
    // family (CopyMenuTextureToUiImage), so 91 is a valid fallback for the 87 panel texture. Without it the menu
    // swapchain silently failed = no VR menu, no lasers, no cursor (they all draw from this swapchain).
    const bool ui_bgra_unorm = std::find(supported_formats.begin(), supported_formats.end(), static_cast<int64_t>(DXGI_FORMAT_B8G8R8A8_UNORM)) != supported_formats.end();
    const bool ui_bgra_srgb = std::find(supported_formats.begin(), supported_formats.end(), static_cast<int64_t>(DXGI_FORMAT_B8G8R8A8_UNORM_SRGB)) != supported_formats.end();
    // R114: GDI menu bytes are display (sRGB) encoded too - with VrEyeSrgbR114 the sRGB swapchain goes first.
    const DXGI_FORMAT preferred_ui = (eyeSrgbR114&&ui_bgra_srgb) ? DXGI_FORMAT_B8G8R8A8_UNORM_SRGB :
        ui_bgra_unorm ? DXGI_FORMAT_B8G8R8A8_UNORM : DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
    const bool ui_bgra_supported = ui_bgra_unorm || ui_bgra_srgb;
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
        {char m[200]{};std::snprintf(m,sizeof(m),"R108 XR_FORMATS menu swapchain FAILED result=%d fmt=%d - no VR menu / lasers / cursor this session",int(result),int(menu_ui_format_));diagnostic_trace::WriteStatus(m);}
        menu_ui_.swapchain = XR_NULL_HANDLE;
    } else {
        uint32_t ui_count = 0;
        result = xrEnumerateSwapchainImages(menu_ui_.swapchain, 0, &ui_count, nullptr);
        if (XR_SUCCEEDED(result) && ui_count > 0) {
            std::vector<XrSwapchainImageD3D12KHR> ui_images(ui_count);
            for (auto& image : ui_images) image.type = XR_TYPE_SWAPCHAIN_IMAGE_D3D12_KHR;
            result = xrEnumerateSwapchainImages(menu_ui_.swapchain, ui_count, &ui_count, reinterpret_cast<XrSwapchainImageBaseHeader*>(ui_images.data()));
            if (XR_SUCCEEDED(result)) for (auto& image : ui_images) menu_ui_.d3d12_images.push_back(image.texture);
            menu_ui_image_initialized_.assign(menu_ui_.d3d12_images.size(), false);
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

bool XrManager::EnsureInitialized(ID3D12Device* game_device, ID3D12CommandQueue* game_queue, const std::atomic<bool>* keep_running)
{
    if (session_running_.load(std::memory_order_relaxed)) return true;
    if (init_attempted_) return false;

    init_attempted_ = true;
    KCD2VR_DIAG_LOG(Bug,info,"Starting XrManager initialization (native D3D12)...");
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
            KCD2VR_DIAG_LOG(Bug,warning,"V37-BRIDGE: blit pipeline unavailable; falling back to diagnostic clears");
        }
        if (diagnostic_rendering_.load(std::memory_order_acquire))
            KCD2VR_DIAG_LOG(Bug,warning,"DiagnosticRendering=true: submitting black D3D12 OpenXR eye textures");
        else
            KCD2VR_DIAG_LOG(Bug,warning,"V37-BRIDGE active: submitting KCD2 backbuffer into both OpenXR eyes");
        debug_trace::Write("V46-PRESENT OpenXR composition enabled; mono bridge retained; auto-matrix can cycle stretch/fit/crop/XR-FOV");
        debug_trace::Write("V46-XR-STATE D3D12 color swapchain images are RENDER_TARGET after xrWaitSwapchainImage and must remain RENDER_TARGET at xrReleaseSwapchainImage; no XR-image COMMON transitions");
    }
    else
    {
        KCD2VR_DIAG_LOG(Bug,info,"Tracking-only mode: no OpenXR swapchains or D3D12 command lists will be created");
        debug_trace::Write("tracking-only mode active: zero composition layers, no custom GPU submission");
    }

    bool got_ready = false;
    // R105: READY may wait for headset/user engagement. Keep this initialization
    // wait cancellable instead of permanently disabling VR after a fixed deadline.
    // Healthy startup and live pose timing are unchanged; every sleep is 25 ms.
    for (std::uint64_t pollsR105=0;!got_ready;++pollsR105)
    {
        if(keep_running&&!keep_running->load(std::memory_order_acquire))return false;
        if(pollsR105==20||(pollsR105>=600&&(pollsR105-600)%2400==0))
            diagnostic_trace::WriteStatus("R105 XR_SESSION waiting for runtime READY/user engagement; cancellation=ENABLED timeoutLatch=NONE");
        XrEventDataBuffer event{ XR_TYPE_EVENT_DATA_BUFFER };
        XrResult pollResultR105=XR_SUCCESS;
        while ((pollResultR105=xrPollEvent(instance_, &event)) == XR_SUCCESS)
        {
            if (event.type == XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED)
            {
                auto* state_event = reinterpret_cast<XrEventDataSessionStateChanged*>(&event);
                if(state_event->session==session_){
                    if (state_event->state == XR_SESSION_STATE_READY)got_ready=true;
                    else if(state_event->state==XR_SESSION_STATE_EXITING||state_event->state==XR_SESSION_STATE_LOSS_PENDING)return false;
                }
            }else if(event.type==XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING)return false;
            event.type = XR_TYPE_EVENT_DATA_BUFFER;
        }
        if(XR_FAILED(pollResultR105)){
            char m[160]{};std::snprintf(m,sizeof(m),"R105 XR_SESSION startup event polling failed result=%d",int(pollResultR105));diagnostic_trace::WriteStatus(m);return false;
        }
        if (!got_ready) Sleep(25);
    }

    XrSessionBeginInfo begin_info{ XR_TYPE_SESSION_BEGIN_INFO };
    begin_info.primaryViewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
    XrResult result = xrBeginSession(session_, &begin_info);
    if (XR_FAILED(result))
    {
        KCD2VR_DIAG_LOG(Bug,error,"xrBeginSession failed - code={}", static_cast<int>(result));
        return false;
    }

    if (!input_.Initialize(instance_, session_, local_space_))
    {
        KCD2VR_DIAG_LOG(Bug,error,"OpenXR input initialization failed; continuing with HMD-only VR");
        diagnostic_trace::WriteStatus("R102 XR_INPUT initialization failed: HMD-only VR, no controller input (see the R102 XR_INPUT rows above)");
    }

    comfort_envelope51_.Reset();
    session_running_.store(true, std::memory_order_relaxed);
    debug_trace::Write("OpenXR session begun successfully");
    KCD2VR_DIAG_LOG(Bug,info,"OpenXR session started (native D3D12 binding; diagnostic rendering={})",
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
        if (TimedFenceWait165(fence_event_, INFINITE) != WAIT_OBJECT_0)
            return false;
    }

    HRESULT hr = command_allocator_->Reset();
    if (FAILED(hr))
    {
        KCD2VR_DIAG_LOG(Bug,error,"D3D12 command allocator Reset failed, hr=0x{:08X}", static_cast<unsigned>(hr));
        return false;
    }

    hr = command_list_->Reset(command_allocator_, nullptr);
    if (FAILED(hr))
    {
        KCD2VR_DIAG_LOG(Bug,error,"D3D12 command list Reset failed, hr=0x{:08X}", static_cast<unsigned>(hr));
        return false;
    }

    // V46 / XR_KHR_D3D12_enable: after xrWaitSwapchainImage a color
    // swapchain image is already in RENDER_TARGET state, and the runtime
    // requires it to still be RENDER_TARGET when xrReleaseSwapchainImage is
    // called. Do not transition XR-owned color images through COMMON.
    const float clear_color[4] = { 0.0f, 0.0f, 0.0f, 1.0f }; // V66.85 black startup
    command_list_->ClearRenderTargetView(
        // We do not need an RTV descriptor heap: use a CPU descriptor created
        // in a small per-image heap below.
        GetRtvHandle(image), clear_color, 0, nullptr);

    hr = command_list_->Close();
    if (FAILED(hr)) return false;

    ID3D12CommandList* lists[] = { command_list_ };
    if (!DeviceHook::ExecuteCommandListsBypass(d3d12_queue_, 1, lists))
    {
        KCD2VR_DIAG_LOG(Bug,error,"Render-thread D3D12 submission bypass is unavailable");
        return false;
    }

    ++fence_value_;
    hr = d3d12_queue_->Signal(fence_, fence_value_);
    if (FAILED(hr)) return false;

    if (FAILED(fence_->SetEventOnCompletion(fence_value_, fence_event_))) return false;
    return TimedFenceWait165(fence_event_, INFINITE) == WAIT_OBJECT_0;
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
        char r148[320]{};
        namespace sf=kcd2vr::start_facing_r148;
        if (need_capture) {
            RenderFrameState f{};
            if (CopyActiveRenderFrameState(f) && f.head_pose_valid) {
                std::lock_guard<std::mutex> lock(flat_ui_mutex_);
                flat_ui_anchor_pose_ = f.head_pose;
                flat_ui_anchor_valid_ = true;
                captured = true;
                // R148: a re-capture of an open screen right after YOUR runtime recenter is yours: never moved by the look-away rule
                ui_anchor_user_r148_=previous&&ui_reference_change_r148_;ui_reference_change_r148_=false;ui_away_since_r148_=0;
                if(!ui_anchor_logged_r148_){
                    ui_anchor_logged_r148_=true;const auto& q=f.head_pose.orientation;const auto fw=sf::forwardOf(q.x,q.y,q.z,q.w);
                    std::snprintf(r148,sizeof(r148),"R148 UI_ANCHOR first runtime='%s' yawDeg=%.1f pitchDeg=%.1f tracked=%d user=%d (screen placed in front of this head pose)",
                        sf::runtime(),double(kcd2vr::start_r111::yawDeg(q.x,q.y,q.z,q.w)),double(sf::pitchDeg(fw)),sf::headTracked.load(std::memory_order_acquire)?1:0,ui_anchor_user_r148_?1:0);
                }
            }
        }
        // R148: the automatic Theater screen (intro / main menu / pause) placed from an early SteamVR pose can sit
        // where the headset pointed before you looked in. a tracked, level head looking > 60 deg away from it for
        // 1.5 s re-places it in front of you (start_facing_r148.h uiLookAway). only while the automatic Theater is on.
        if (!captured && native_menu_plane_mode_.load(std::memory_order_acquire)) {
            RenderFrameState f{};
            if (CopyActiveRenderFrameState(f) && f.head_pose_valid) {
                std::lock_guard<std::mutex> lock(flat_ui_mutex_);
                if (flat_ui_anchor_valid_) {
                    const auto& h=f.head_pose.orientation;const auto& a=flat_ui_anchor_pose_.orientation;
                    const auto hf=sf::forwardOf(h.x,h.y,h.z,h.w),af=sf::forwardOf(a.x,a.y,a.z,a.w);
                    const float angle=sf::angleDeg(hf,af),pitch=sf::pitchDeg(hf);
                    if(sf::uiLookAway(ui_away_since_r148_,kcd2vr::start_r111::henryEyes.load(std::memory_order_acquire),ui_anchor_user_r148_,
                                      sf::headTracked.load(std::memory_order_acquire),angle,pitch,std::uint64_t(GetTickCount64()))){
                        const float before=kcd2vr::start_r111::yawDeg(a.x,a.y,a.z,a.w);
                        flat_ui_anchor_pose_=f.head_pose;ui_away_since_r148_=0;
                        std::snprintf(r148,sizeof(r148),"R148 UI_ANCHOR replaced runtime='%s' screenYawDeg before=%.1f after=%.1f awayDeg=%.1f headPitchDeg=%.1f (you looked away from the start/menu screen 1.5 s)",
                            sf::runtime(),double(before),double(kcd2vr::start_r111::yawDeg(h.x,h.y,h.z,h.w)),double(angle),double(pitch));
                    }
                }
            }
        }
        if(r148[0])diagnostic_trace::WriteStatus(r148); // KCD2VR.log only, few rows
        if (!previous || captured) {
            char m[192]{};
            std::snprintf(m, sizeof(m), "V66.20-FLAT-PLANE ON anchor=%s space=LOCAL orientation=YAW-ONLY", captured ? "captured" : "pending");
            debug_trace::Write(m);
        }
    } else if (previous) {
        // Retain the last anchor for an already queued Theater frame. The
        // next OFF->ON edge still captures a new pose because !previous is
        // true. Clearing it here raced the frame's presentation snapshot.
        debug_trace::Write("V66.20-FLAT-PLANE OFF");
    }
}

void XrManager::SetNativeMenuPlaneMode(bool enabled)
{
    // Separate semantic owner from manual F6: automatic fullscreen menus use the
    // same proven LOCAL-space quad infrastructure, but capture yaw+pitch so the
    // plane spawns in front of the eyes instead of being horizon-level.
    native_menu_plane_mode_.store(enabled,std::memory_order_release);
    SetFlatUiMode(enabled);
    if(enabled) {
        static std::atomic<std::uint64_t> log_count{0};
        const auto n=log_count.fetch_add(1,std::memory_order_relaxed)+1;
        if(n<=4 || (n%600ull)==0)
            debug_trace::Write("V66.20-NATIVE-MENU-PLANE requested space=LOCAL anchor=GAZE-YAW-PITCH roll=0 surroundings=BLACK source=KCD2-FINAL-FLAT");
    }
}


bool XrManager::DrainBridgePairForTheater()
{
    ID3D12Resource* left=nullptr;
    ID3D12Resource* right=nullptr;
    std::uint64_t serial=0;
    std::uint64_t camera_frame=0;
    const bool have=DeviceHook::TryGetBridgeCompletePair(&left,&right,&serial,&camera_frame);
    bool consumed=false;
    if(have && serial!=0 && serial!=bridge_last_consumed_pair_serial_) {
        bridge_last_consumed_pair_serial_=serial;
        DeviceHook::MarkBridgePairConsumed(serial);
        consumed=true;
        static std::atomic<std::uint64_t> drains{0};
        const auto n=drains.fetch_add(1,std::memory_order_relaxed)+1;
        if(n<=12 || (n%120ull)==0) {
            char m[320]{};
            std::snprintf(m,sizeof(m),
                "V66.33-THEATER-DRAIN n=%llu serial=%llu poseFrame=%llu thread=XR-RENDER action=CONSUME-WITHOUT-XR-SCENE-PRESENT producer=KEEP-RUNNING",
                static_cast<unsigned long long>(n),static_cast<unsigned long long>(serial),
                static_cast<unsigned long long>(camera_frame));
            diagnostic_trace::Write(m);
        }
    }
    if(left) left->Release();
    if(right) right->Release();
    DeviceHook::ReleaseBridgePairClaim(); // V66.34: theater never reads the pair
    return consumed;
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

// V66.80 compatibility flag: HUD eye-blit is retired; kept false for the shader path.
static thread_local bool g_v6678_hud_active_for_blit = false;
static DXGI_FORMAT Kcd2VrSrvFormat(DXGI_FORMAT fmt); // defined below

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
                if (dw > 0 && dw <= 0xFFFFFFFFull && dh > 0) {
                    depth_tex_width_.store(static_cast<std::uint32_t>(dw), std::memory_order_release);
                    depth_tex_height_.store(dh, std::memory_order_release);
                }
            }
        }
    }
    // V66.80: slot 2 stays valid but HUD is NOT composited into either eye. The native
    // KCD2 HUD capture is submitted once as an OpenXR composition layer later in RunFrame.
    {
        D3D12_CPU_DESCRIPTOR_HANDLE hud_cpu = depth_cpu;
        hud_cpu.ptr += static_cast<SIZE_T>(srv_descriptor_size_);
        d3d12_device_->CreateShaderResourceView(color_source, &csrv, hud_cpu);
        g_v6678_hud_active_for_blit = false;
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
Texture2D hudTex : register(t2);
SamplerState srcSampler : register(s0);
cbuffer PresentParams : register(b0) { float2 uvScale; float2 uvOffset; float4 xrTan; float2 sourceTan; float2 fovFlags; float2 texelSize; float sharpenStrength; float presentPad; float4 poseDeltaQ; float4 reprojParams; float4 colorAdjust; float2 depthTexelSize; float hudEnable; float comfortVignette; };
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
    int2 depthPx = int2(saturate(uv01) * (1.0 / depthTexelSize));
    float rawDepth = DepthTex.Load(int3(depthPx, 0)).r;
    float nearness = saturate(rawDepth);
    return lerp(1.0, 0.85, saturate(presentPad) * nearness);
}
float4 DepthAwareTans(float2 uv01) {
    // V64.35 hybrid: restore the headset-positive V64.22C depth-pseudo math
    // verbatim. poseDeltaQ is the cyclopean/common frustum and xrTan is the
    // real eye frustum. Far stays at 1.00; near is pulled only toward 0.70 at
    // maximum strength. This is intentionally conservative pseudo disparity.
    if (reprojParams.w < 0.5 || presentPad <= 0.0) return xrTan;
    int2 depthPx = int2(saturate(uv01) * (1.0 / depthTexelSize));
    float rawDepth = DepthTex.Load(int3(depthPx, 0)).r;
    float nearness = saturate(rawDepth);
    float effectiveAsym = 1.0 - 0.30 * saturate(presentPad) * nearness;
    return poseDeltaQ + effectiveAsym * (xrTan - poseDeltaQ);
}
float4 PSMainInner51(VSOut i) {
    float2 uv;
    if (fovFlags.x > 1.5) {
        // Flat UI: uvScale > 1 shrinks the source into a centered virtual screen;
        // out-of-range pixels become the black surround below.
        uv = (i.uv - 0.5) * uvScale + 0.5 + uvOffset;
    } else if (fovFlags.x > 0.5) {
        if (reprojParams.x > 3.5) {
            // V65.77 TRUE-STEREO OPENXR MAP. xrTan is the current/submitted
            // OpenXR eye frustum. poseDeltaQ carries the source eye frustum
            // that generated the native COUNT2 eye record: {L,R,D,U} tangents.
            // Map ray-to-ray between the two asymmetric frusta, then uvScale/
            // uvOffset selects this eye's source region. No symmetric VR-FOV
            // calibration, no cyclopean/common frustum, no guessed aspect.
            float tanX = lerp(xrTan.x, xrTan.y, i.uv.x);
            float tanY = lerp(xrTan.w, xrTan.z, i.uv.y);
            float srcL = poseDeltaQ.x, srcR = poseDeltaQ.y;
            float srcD = poseDeltaQ.z, srcU = poseDeltaQ.w;
            float spanX = max(srcR - srcL, 1e-4);
            float spanY = max(srcU - srcD, 1e-4);
            uv.x = (tanX - srcL) / spanX;
            uv.y = (srcU - tanY) / spanY;
            uv = (uv - 0.5) * uvScale + 0.5 + uvOffset;
        } else if (reprojParams.x > 2.5) {
            // V65.27 CORRECT FOV. IMPORTANT: xrTan remains the REAL per-eye
            // OpenXR frustum. A physical ray therefore evaluates to the same
            // source coordinate in both eyes. poseDeltaQ.xy carries the user's
            // virtual/VR target tan half-extents. This is resolution-independent
            // image-plane/intrinsics calibration; Game FOV only changes what the
            // source camera rendered, while VR FOV controls how that raster is
            // angularly fitted in the headset. No common-eye/intersection frustum.
            float eyeTanX = lerp(xrTan.x, xrTan.y, i.uv.x);
            float eyeTanY = lerp(xrTan.w, xrTan.z, i.uv.y);
            float targetTanX = max(abs(poseDeltaQ.x), 1e-4);
            float targetTanY = max(abs(poseDeltaQ.y), 1e-4);
            float tanX = eyeTanX;
            float tanY = eyeTanY;

            float pitchRad = colorAdjust.w;
            if (abs(pitchRad) > 1e-5) {
                float cp = cos(pitchRad), sp = sin(pitchRad);
                float denom = -tanY * sp + cp;
                denom = abs(denom) < 1e-4 ? (denom < 0.0 ? -1e-4 : 1e-4) : denom;
                float tanY2 = (tanY * cp + sp) / denom;
                float tanX2 = tanX / denom;
                tanX = tanX2; tanY = tanY2;
            }

            // Treat the complete captured KCD raster as the image plane of the
            // user-selected VR FOV. This intentionally compresses a wider Game
            // FOV into a narrower VR FOV without nonlinear edge warp.
            uv.x = 0.5 + tanX / (2.0 * targetTanX);
            uv.y = 0.5 - tanY / (2.0 * targetTanY);
            uv = (uv - 0.5) * uvScale + 0.5 + uvOffset;
        } else if (reprojParams.x > 1.5) {
            // V60.5 STATIC INTRINSICS HOMOGRAPHY.  No temporal pose delta,
            // no depth, no lens distortion.  This is the exact projective
            // mapping from the asymmetric OpenXR eye frustum into KCD2's
            // symmetric source-camera tangent plane.
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
            // V64.46: rotation-only late reprojection for the normal OpenXR-FOV
            // gameplay path.  Build the CURRENT output ray, rotate it back into
            // the exact camera orientation that rendered the captured PRESENT,
            // then continue through the existing projection-center correction.
            if (reprojParams.x > 0.5 && reprojParams.x < 1.5) {
                float3 rr = normalize(float3(tanX, tanY, -1.0));
                rr = QRotateVec(poseDeltaQ, rr);
                float rif = 1.0 / max(-rr.z, 1e-4);
                tanX = rr.x * rif;
                tanY = rr.y * rif;
            }
            // V64.42: real angular vertical projection correction. colorAdjust.w
            // carries radians; this rotates tangent rays rather than shifting UVs.
            float pitchRad = colorAdjust.w;
            if (abs(pitchRad) > 1e-5) {
                float cp = cos(pitchRad), sp = sin(pitchRad);
                float denom = -tanY * sp + cp;
                denom = abs(denom) < 1e-4 ? (denom < 0.0 ? -1e-4 : 1e-4) : denom;
                float tanY2 = (tanY * cp + sp) / denom;
                float tanX2 = tanX / denom;
                tanX = tanX2; tanY = tanY2;
            }
            uv.x = 0.5 + tanX / (2.0 * max(sourceTan.x, 1e-4));
            uv.y = 0.5 - tanY / (2.0 * max(sourceTan.y, 1e-4));
            uv = (uv - 0.5) * uvScale + 0.5 + uvOffset;
        }
        // V65.17: mode 5 never uses reprojParams.y as a binocular/depth offset.
        // PROJECTIVE FOV REMAP owns the full source->eye tangent mapping; legacy
        // modes retain their historical tiny X offset behavior.
        if (reprojParams.x < 2.5)
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
    // V66.78: late HUD (redirected KCD2 Flash HUD/fades/menus) composited at the exact source uv.
    if (hudEnable > 0.5) {
        float4 h = hudTex.SampleLevel(srcSampler, uv, 0);
        c.rgb = h.rgb + c.rgb * (1.0 - saturate(h.a));
    }
    // V62.6: crash-safe VR-output color adjustment. The KCD2 renderer remains untouched.
    float gammaSafe = max(colorAdjust.x, 0.05);
    float3 adjusted = (c.rgb - 0.5) * colorAdjust.z + 0.5 + colorAdjust.y;
    adjusted = saturate(adjusted);
    adjusted = pow(max(adjusted, 0.0), 1.0 / gammaSafe);
    c.rgb = saturate(adjusted);
    return c;
}

// C51 constant31: magnitude=strength, negative selects opt-in H29 angular mask.
// Both eyes consume the same immutable frame value; no GPU layout change.
float4 PSMain(VSOut i) : SV_TARGET {
    float4 c = PSMainInner51(i);
    if (abs(comfortVignette) > 0.001) {
      if (comfortVignette < 0.0) {
        float tanX = lerp(xrTan.x, xrTan.y, i.uv.x);
        float tanY = lerp(xrTan.w, xrTan.z, i.uv.y);
        float ang = atan(length(float2(tanX, tanY)));
        float s = saturate(-comfortVignette);
        float inner = lerp(1.10, 0.52, s);
        c.rgb *= 1.0 - s * smoothstep(inner, inner + 0.50, ang);
      } else {
        float2 d = (i.uv - 0.5) * 2.0;
        float r = length(d);
        float s = saturate(comfortVignette);
        float inner = lerp(0.95, 0.40, s);
        c.rgb *= 1.0 - s * smoothstep(inner, inner + 0.45, r);
      }
    }
    return c;
}
)";

    ID3DBlob* vs = nullptr; ID3DBlob* ps = nullptr; ID3DBlob* errors = nullptr;
    HRESULT hr = D3DCompile(shader, std::strlen(shader), "KCD2VR_V38", nullptr, nullptr,
                            "VSMain", "vs_5_0", D3DCOMPILE_OPTIMIZATION_LEVEL3, 0, &vs, &errors);
    if (FAILED(hr)) {
        if (errors) { KCD2VR_DIAG_LOG(Bug,error,"V38-PRESENT VS compile failed: {}", static_cast<const char*>(errors->GetBufferPointer())); errors->Release(); }
        return false;
    }
    if (errors) { errors->Release(); errors = nullptr; }
    hr = D3DCompile(shader, std::strlen(shader), "KCD2VR_V38", nullptr, nullptr,
                    "PSMain", "ps_5_0", D3DCOMPILE_OPTIMIZATION_LEVEL3, 0, &ps, &errors);
    if (FAILED(hr)) {
        if (errors) { KCD2VR_DIAG_LOG(Bug,error,"V38-PRESENT PS compile failed: {}", static_cast<const char*>(errors->GetBufferPointer())); errors->Release(); }
        vs->Release(); return false;
    }
    if (errors) errors->Release();

    D3D12_DESCRIPTOR_RANGE range{};
    range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    range.NumDescriptors = 3; // V61.35: t0=color, t1=depth; V66.78: t2=HUD
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
    params[1].Constants.Num32BitValues = 32; // V66.78: +hudEnable,+pad
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
    heap.NumDescriptors = 6; // V66.78: 3 slots (color,depth,hud) x 2 eyes // V61.35: 2 slots (color,depth) per eye x 2 eyes for AFR; mono uses only the first pair
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

void XrManager::DestroyBridgePresentCaches()
{
    for(int eye=0; eye<2; ++eye) {
        if(bridge_present_cache_[eye]) { bridge_present_cache_[eye]->Release(); bridge_present_cache_[eye]=nullptr; }
    }
    bridge_present_cache_desc_={};
    bridge_present_cache_valid_=false;
    bridge_present_cache_serial_=0;
    bridge_present_cache_owner_d85_={};
    bridge_present_cache_camera_frame_=0;
}

bool XrManager::EnsureBridgePresentCaches(const D3D12_RESOURCE_DESC& source_desc)
{
    if(!d3d12_device_ || source_desc.Dimension!=D3D12_RESOURCE_DIMENSION_TEXTURE2D ||
       source_desc.SampleDesc.Count!=1 || source_desc.Width==0 || source_desc.Height==0) return false;
    const bool same=bridge_present_cache_[0] && bridge_present_cache_[1] &&
        bridge_present_cache_desc_.Width==source_desc.Width &&
        bridge_present_cache_desc_.Height==source_desc.Height &&
        bridge_present_cache_desc_.Format==source_desc.Format &&
        bridge_present_cache_desc_.SampleDesc.Count==source_desc.SampleDesc.Count;
    if(same) return true;

    DestroyBridgePresentCaches();
    D3D12_RESOURCE_DESC desc=source_desc;
    desc.Flags=D3D12_RESOURCE_FLAG_NONE;
    desc.MipLevels=1;
    D3D12_HEAP_PROPERTIES heap{};
    heap.Type=D3D12_HEAP_TYPE_DEFAULT;
    heap.CPUPageProperty=D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heap.MemoryPoolPreference=D3D12_MEMORY_POOL_UNKNOWN;
    heap.CreationNodeMask=1;
    heap.VisibleNodeMask=1;
    for(int eye=0; eye<2; ++eye) {
        if(FAILED(d3d12_device_->CreateCommittedResource(&heap,D3D12_HEAP_FLAG_NONE,&desc,
            D3D12_RESOURCE_STATE_COMMON,nullptr,IID_PPV_ARGS(&bridge_present_cache_[eye])))) {
            DestroyBridgePresentCaches();
            return false;
        }
    }
    bridge_present_cache_desc_=desc;
    char m[320]{}; std::snprintf(m,sizeof(m),
        "V66.22-XR-STABLE-CACHE CREATE L=%p R=%p size=%llux%u fmt=%u",
        bridge_present_cache_[0],bridge_present_cache_[1],
        (unsigned long long)desc.Width,desc.Height,(unsigned)desc.Format);
    debug_trace::Write(m);
    return true;
}


bool XrManager::EnsureStereoSubmitRing()
{
    if(stereo_submit_ring_disabled_) return false;
    if(stereo_submit_ring_ready_) return true;
    if(!d3d12_device_ || !fence_ || srv_descriptor_size_==0) return false;

    for(std::size_t i=0;i<kStereoSubmitRingCount;++i) {
        auto& slot=stereo_submit_ring_[i];
        HRESULT hr=d3d12_device_->CreateCommandAllocator(
            D3D12_COMMAND_LIST_TYPE_DIRECT,IID_PPV_ARGS(&slot.allocator));
        if(FAILED(hr)) { DestroyStereoSubmitRing(); stereo_submit_ring_disabled_=true; return false; }

        hr=d3d12_device_->CreateCommandList(
            0,D3D12_COMMAND_LIST_TYPE_DIRECT,slot.allocator,nullptr,IID_PPV_ARGS(&slot.list));
        if(FAILED(hr)) { DestroyStereoSubmitRing(); stereo_submit_ring_disabled_=true; return false; }
        if(FAILED(slot.list->Close())) { DestroyStereoSubmitRing(); stereo_submit_ring_disabled_=true; return false; }

        D3D12_DESCRIPTOR_HEAP_DESC heap{};
        heap.NumDescriptors=6; // V66.78: 3 per eye
        heap.Type=D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        heap.Flags=D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        hr=d3d12_device_->CreateDescriptorHeap(&heap,IID_PPV_ARGS(&slot.srv_heap));
        if(FAILED(hr)) { DestroyStereoSubmitRing(); stereo_submit_ring_disabled_=true; return false; }
        slot.fence_value=0;
    }
    stereo_submit_ring_cursor_=0;
    stereo_submit_ring_ready_=true;
    debug_trace::Write("V66.27-F2-PERF XR submit ring READY slots=16 policy=NO-CPU-WAIT-ON-NORMAL-FRAME");
    return true;
}

void XrManager::DestroyStereoSubmitRing()
{
    const UINT64 completed=fence_?fence_->GetCompletedValue():0;
    for(auto& slot:stereo_submit_ring_) {
        if((slot.ui_source || slot.ui_target) && slot.fence_value>completed) {
            // Execute succeeded but its Signal may have failed. Never destroy a
            // queued UI command/resource based on an earlier unrelated fence.
            // This fault-only quarantine deliberately retains the COM references.
            debug_trace::Write("R13 UI copy shutdown: retaining uncompleted submission resources");
            slot={};
            continue;
        }
        if(slot.ui_source) { slot.ui_source->Release(); slot.ui_source=nullptr; }
        if(slot.ui_target) { slot.ui_target->Release(); slot.ui_target=nullptr; }
        if(slot.list) { slot.list->Release(); slot.list=nullptr; }
        if(slot.allocator) { slot.allocator->Release(); slot.allocator=nullptr; }
        if(slot.srv_heap) { slot.srv_heap->Release(); slot.srv_heap=nullptr; }
        slot.fence_value=0;
    }
    stereo_submit_ring_cursor_=0;
    stereo_submit_ring_ready_=false;
}

XrManager::StereoSubmitSlot* XrManager::AcquireStereoSubmitSlot()
{
    if(!EnsureStereoSubmitRing() || !fence_) return nullptr;
    const UINT64 completed=fence_->GetCompletedValue();
    for(std::size_t step=0;step<kStereoSubmitRingCount;++step) {
        const std::size_t idx=(stereo_submit_ring_cursor_+step)%kStereoSubmitRingCount;
        auto& slot=stereo_submit_ring_[idx];
        if(slot.fence_value==0 || completed>=slot.fence_value) {
            if(slot.ui_source) { slot.ui_source->Release(); slot.ui_source=nullptr; }
            if(slot.ui_target) { slot.ui_target->Release(); slot.ui_target=nullptr; }
            stereo_submit_ring_cursor_=(idx+1)%kStereoSubmitRingCount;
            return &slot;
        }
    }
    return nullptr;
}

bool XrManager::StereoSubmitRingIdle() const
{
    if(!stereo_submit_ring_ready_ || !fence_) return true;
    const UINT64 completed=fence_->GetCompletedValue();
    for(const auto& slot:stereo_submit_ring_) {
        if(slot.fence_value!=0 && completed<slot.fence_value) return false;
    }
    return true;
}

bool XrManager::BlitBridgePairStableToEyes(ID3D12Resource* left_source, ID3D12Resource* right_source,
                                             std::uint64_t pair_serial, std::uint64_t pair_camera_frame,
                                             bool refresh_pair,
                                             ID3D12Resource* left, ID3D12Resource* right,
                                             bool* safe_to_drop,
                                             native_upscaler_stereo_d82::PairOwner pair_owner)
{
    if (safe_to_drop) *safe_to_drop = true;
    // Check the candidate's publication owner for refresh, or the owner saved
    // with the already-copied pixels for reuse. The latter also covers BOTH
    // historical F2 stable-cache fallbacks; rejection does not free GPU work.
    if(!native_upscaler_stereo_d82::MayPresent(
        refresh_pair?pair_owner:bridge_present_cache_owner_d85_))return false;
    if (!left || !right || !command_allocator_ || !command_list_ || !fence_ ||
        !d3d12_queue_ || !blit_pipeline_state_ || !blit_root_signature_ || !srv_heap_) return false;

    const bool f2_stability=stereo_stability_experiment_.load(std::memory_order_acquire);
    if(refresh_pair && (!left_source || !right_source)) return false;

    D3D12_RESOURCE_DESC left_desc{};
    D3D12_RESOURCE_DESC right_desc{};
    if(refresh_pair) {
        left_desc=left_source->GetDesc();
        right_desc=right_source->GetDesc();
    } else {
        if(!bridge_present_cache_valid_) return false;
        left_desc=bridge_present_cache_desc_;
        right_desc=bridge_present_cache_desc_;
    }
    if (left_desc.Dimension != D3D12_RESOURCE_DIMENSION_TEXTURE2D ||
        right_desc.Dimension != D3D12_RESOURCE_DIMENSION_TEXTURE2D ||
        left_desc.SampleDesc.Count != 1 || right_desc.SampleDesc.Count != 1 ||
        left_desc.Width == 0 || left_desc.Height == 0 || right_desc.Width == 0 || right_desc.Height == 0 ||
        left_desc.Width != right_desc.Width || left_desc.Height != right_desc.Height ||
        left_desc.Format != right_desc.Format || left_desc.SampleDesc.Count != right_desc.SampleDesc.Count) {
        return false;
    }
    auto fail = [&](const char* stage) -> bool {
        static std::atomic<unsigned long long> failures{0};
        const auto n = failures.fetch_add(1, std::memory_order_relaxed) + 1;
        if (n <= 24 || (n % 120ull) == 0) {
            char msg[320]{};
            std::snprintf(msg, sizeof(msg),
                "V66.22-XR-STABLE-PAIR FAIL n=%llu serial=%llu stage=%s fenceCompleted=%llu fenceTarget=%llu safeDrop=%d",
                static_cast<unsigned long long>(n),
                static_cast<unsigned long long>(pair_serial),
                stage ? stage : "unknown",
                static_cast<unsigned long long>(fence_->GetCompletedValue()),
                static_cast<unsigned long long>(fence_value_),
                safe_to_drop && *safe_to_drop ? 1 : 0);
            debug_trace::Write(msg);
        }
        DeviceHook::SetInternalVrCommandList(command_list_);
        return false;
    };
    const int presentation_mode = presentation_mode_.load(std::memory_order_acquire);
    const float presentation_zoom = presentation_zoom_.load(std::memory_order_acquire) *
        dialogue_zoom_multiplier_.load(std::memory_order_acquire);
    const float source_vertical_fov_deg = source_vertical_fov_deg_.load(std::memory_order_acquire);
    const bool angular_correct_mono = angular_correct_mono_.load(std::memory_order_acquire);
    const int uv_reproj_mode = mono_uv_reprojection_mode_.load(std::memory_order_acquire);
    const float eye_offset_x[2] = {
        eye_offset_x_[0].load(std::memory_order_acquire), eye_offset_x_[1].load(std::memory_order_acquire) };
    const float eye_offset_y[2] = {
        eye_offset_y_[0].load(std::memory_order_acquire), eye_offset_y_[1].load(std::memory_order_acquire) };

    StereoSubmitSlot* submit_slot=nullptr;
    bool nonblocking=false;
    // V66.34: the fence-gated submit ring is production (unless F2 turns it off).
    // Only the ring is shared with V66.27; its stale-cache re-present fallback is NOT.
    if(f2_stability || nonblocking_pair_submit_.load(std::memory_order_acquire)) {
        submit_slot=AcquireStereoSubmitSlot();
        nonblocking=submit_slot!=nullptr;
        if(!nonblocking) {
            static std::atomic<unsigned long long> ring_busy{0};
            const auto n=ring_busy.fetch_add(1,std::memory_order_relaxed)+1;
            if(n<=8 || (n%120ull)==0) debug_trace::Write("V66.34-XR-SUBMIT ring16 busy -> bounded blocking fallback for this XR frame");
        }
        const bool cache_same=bridge_present_cache_[0] && bridge_present_cache_[1] &&
            bridge_present_cache_desc_.Width==left_desc.Width &&
            bridge_present_cache_desc_.Height==left_desc.Height &&
            bridge_present_cache_desc_.Format==left_desc.Format &&
            bridge_present_cache_desc_.SampleDesc.Count==left_desc.SampleDesc.Count;
        if(nonblocking && !cache_same && !StereoSubmitRingIdle()) nonblocking=false;
    }

    ID3D12CommandAllocator* active_allocator=nonblocking?submit_slot->allocator:command_allocator_;
    ID3D12GraphicsCommandList* active_list=nonblocking?submit_slot->list:command_list_;
    ID3D12DescriptorHeap* active_srv_heap=nonblocking?submit_slot->srv_heap:srv_heap_;

    if (!nonblocking && fence_->GetCompletedValue() < fence_value_) {
        if (FAILED(fence_->SetEventOnCompletion(fence_value_, fence_event_))) return fail("prewait-set-event");
        if (TimedFenceWait165(fence_event_, 1000) != WAIT_OBJECT_0) return fail("prewait-timeout");
    }
    // Cache reallocation/release is safe only after all earlier submissions on the
    // same DIRECT queue are complete. Stable dimensions normally make this a one-time path.
    if(!EnsureBridgePresentCaches(left_desc)) return fail("stable-cache-ensure");
    if(!refresh_pair && !bridge_present_cache_valid_) return fail("stable-cache-empty");
    HRESULT hr = active_allocator->Reset(); if (FAILED(hr)) return fail("allocator-reset");
    hr = active_list->Reset(active_allocator, blit_pipeline_state_); if (FAILED(hr)) return fail("list-reset");
    DeviceHook::SetInternalVrCommandList(active_list);

    ID3D12Resource* eye_sources[2] = { bridge_present_cache_[0], bridge_present_cache_[1] };
    ID3D12Resource* dsts[2] = { left, right }; // certified L cache -> XR L, certified R cache -> XR R
    bool used_openxr_fov_this_blit = false;

    if(refresh_pair) {
        // Refresh BOTH stable eye caches atomically in this one command list.
        // Source bridge textures remain producer-owned until this command list is
        // submitted. After submission, same-DIRECT-queue ordering is sufficient to
        // allow future overwrite copies without waiting for GPU completion.
        D3D12_RESOURCE_BARRIER pre[4]{};
        UINT pc=0;
        auto& sl=pre[pc++]; sl.Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION; sl.Transition.pResource=left_source;
        sl.Transition.Subresource=D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES; sl.Transition.StateBefore=D3D12_RESOURCE_STATE_COMMON; sl.Transition.StateAfter=D3D12_RESOURCE_STATE_COPY_SOURCE;
        if(right_source!=left_source) {
            auto& sr=pre[pc++]; sr.Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION; sr.Transition.pResource=right_source;
            sr.Transition.Subresource=D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES; sr.Transition.StateBefore=D3D12_RESOURCE_STATE_COMMON; sr.Transition.StateAfter=D3D12_RESOURCE_STATE_COPY_SOURCE;
        }
        for(int eye=0; eye<2; ++eye) {
            auto& d=pre[pc++]; d.Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION; d.Transition.pResource=bridge_present_cache_[eye];
            d.Transition.Subresource=D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES; d.Transition.StateBefore=D3D12_RESOURCE_STATE_COMMON; d.Transition.StateAfter=D3D12_RESOURCE_STATE_COPY_DEST;
        }
        active_list->ResourceBarrier(pc,pre);
        active_list->CopyResource(bridge_present_cache_[0],left_source);
        active_list->CopyResource(bridge_present_cache_[1],right_source);

        D3D12_RESOURCE_BARRIER post[4]{};
        UINT qc=0;
        auto& sl2=post[qc++]; sl2.Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION; sl2.Transition.pResource=left_source;
        sl2.Transition.Subresource=D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES; sl2.Transition.StateBefore=D3D12_RESOURCE_STATE_COPY_SOURCE; sl2.Transition.StateAfter=D3D12_RESOURCE_STATE_COMMON;
        if(right_source!=left_source) {
            auto& sr2=post[qc++]; sr2.Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION; sr2.Transition.pResource=right_source;
            sr2.Transition.Subresource=D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES; sr2.Transition.StateBefore=D3D12_RESOURCE_STATE_COPY_SOURCE; sr2.Transition.StateAfter=D3D12_RESOURCE_STATE_COMMON;
        }
        for(int eye=0; eye<2; ++eye) {
            auto& d=post[qc++]; d.Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION; d.Transition.pResource=bridge_present_cache_[eye];
            d.Transition.Subresource=D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES; d.Transition.StateBefore=D3D12_RESOURCE_STATE_COPY_DEST; d.Transition.StateAfter=D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
        }
        active_list->ResourceBarrier(qc,post);
    } else {
        // The DeviceHook capture pair may already be getting overwritten for the
        // next game frame after consumption. Never sample it again. Re-present only
        // our stable atomic cache until a newer certified pair is available.
        D3D12_RESOURCE_BARRIER begin[2]{};
        for(int eye=0; eye<2; ++eye) {
            begin[eye].Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            begin[eye].Transition.pResource=bridge_present_cache_[eye];
            begin[eye].Transition.Subresource=D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
            begin[eye].Transition.StateBefore=D3D12_RESOURCE_STATE_COMMON;
            begin[eye].Transition.StateAfter=D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
        }
        active_list->ResourceBarrier(2,begin);
    }

    ID3D12DescriptorHeap* heaps[] = { active_srv_heap };
    active_list->SetDescriptorHeaps(1, heaps);
    active_list->SetGraphicsRootSignature(blit_root_signature_);
    active_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    for (int eye = 0; eye < 2; ++eye) {
        const D3D12_RESOURCE_DESC cache_desc = eye_sources[eye]->GetDesc();
        // V61.35: 2 contiguous heap slots per eye now (color, depth) instead
        // of 1, so eye 0 -> slots[0,1], eye 1 -> slots[2,3].
        D3D12_CPU_DESCRIPTOR_HANDLE color_cpu = active_srv_heap->GetCPUDescriptorHandleForHeapStart();
        color_cpu.ptr += static_cast<SIZE_T>(eye) * 3 * srv_descriptor_size_;
        D3D12_CPU_DESCRIPTOR_HANDLE depth_cpu = color_cpu;
        depth_cpu.ptr += static_cast<SIZE_T>(srv_descriptor_size_);
        const float depth_weight = WriteColorAndDepthDescriptors(
            eye_sources[eye], Kcd2VrSrvFormat(cache_desc.Format), color_cpu, depth_cpu);
        D3D12_GPU_DESCRIPTOR_HANDLE srv_gpu = active_srv_heap->GetGPUDescriptorHandleForHeapStart();
        srv_gpu.ptr += static_cast<UINT64>(eye) * 3 * srv_descriptor_size_;
        active_list->SetGraphicsRootDescriptorTable(0, srv_gpu);

        D3D12_VIEWPORT vp{};
        vp.Width = static_cast<float>(eyes_[eye].width); vp.Height = static_cast<float>(eyes_[eye].height);
        vp.MinDepth = 0.0f; vp.MaxDepth = 1.0f;
        D3D12_RECT sc{0, 0, eyes_[eye].width, eyes_[eye].height};
        active_list->RSSetViewports(1, &vp); active_list->RSSetScissorRects(1, &sc);
        const auto rtv = GetRtvHandle(dsts[eye]);
        active_list->OMSetRenderTargets(1, &rtv, FALSE, nullptr);

        const float src_aspect = static_cast<float>(cache_desc.Width) / static_cast<float>(cache_desc.Height);
        const float dst_aspect = static_cast<float>(eyes_[eye].width) / static_cast<float>(eyes_[eye].height);
        float sx = 1.0f, sy = 1.0f;
        if (presentation_mode == 1) { if (src_aspect > dst_aspect) sy = src_aspect / dst_aspect; else sx = dst_aspect / src_aspect; }
        else if (presentation_mode == 2) { if (src_aspect > dst_aspect) sx = dst_aspect / src_aspect; else sy = src_aspect / dst_aspect; }
        const float zoom = presentation_zoom < 0.25f ? 0.25f : (presentation_zoom > 4.0f ? 4.0f : presentation_zoom);
        RenderFrameState frame_snapshot{};
        const bool have_frame = CopyActiveRenderFrameState(frame_snapshot);
        const bool flat_ui = render_presentation59c2_.policy.flat || render_presentation59c2_.policy.physical_map;
        const bool have_fov = !flat_ui && presentation_mode == 3 && have_frame && frame_snapshot.stereo_views_valid;
        const float vfov_deg = source_vertical_fov_deg < 20.0f ? 20.0f : (source_vertical_fov_deg > 160.0f ? 160.0f : source_vertical_fov_deg);
        const float source_tan_y = std::tan(vfov_deg * 0.008726646259971648f);
        const float source_tan_x = source_tan_y * src_aspect;
        // V65.18 CYCLOPEAN FIT REMAP: vfov_deg is the full geometry KCD2 actually
        // rendered (hard 120). Mode 5 maps the complete source angular extent
        // into each runtime eye; there is no fitted/crop FOV scalar.
        const bool marker = presentation_test_marker_.load(std::memory_order_acquire);
        const float sharp = std::clamp(sharpening_strength_.load(std::memory_order_acquire), 0.0f, 0.75f);
        const float common_center_x = presentation_center_x_.load(std::memory_order_acquire);
        const float common_center_y = presentation_center_y_.load(std::memory_order_acquire);
        float present[32] = { (sx / zoom), (sy / zoom), eye_offset_x[eye] + common_center_x, eye_offset_y[eye] + common_center_y, 0,0,0,0, source_tan_x, source_tan_y, have_fov ? 1.0f : 0.0f, marker ? static_cast<float>(eye + 1) : 0.0f,
                              1.0f / static_cast<float>(std::max<UINT64>(1, cache_desc.Width)), 1.0f / static_cast<float>(std::max<UINT>(1, cache_desc.Height)), sharp, 0.0f };
        present[15] = depth_weight; // V61.35: presentPad now carries depthPushWeight (see WriteColorAndDepthDescriptors)
        present[24] = output_gamma_.load(std::memory_order_acquire);
        present[25] = output_brightness_.load(std::memory_order_acquire);
        present[26] = output_contrast_.load(std::memory_order_acquire);
        { const auto dw=depth_tex_width_.load(std::memory_order_acquire); const auto dh=depth_tex_height_.load(std::memory_order_acquire);
          present[28]=1.0f/static_cast<float>(dw?dw:std::max<UINT64>(1, cache_desc.Width));
          present[29]=1.0f/static_cast<float>(dh?dh:std::max<UINT>(1, cache_desc.Height)); }
        float effective_pitch_rad = pitch_correction_rad_.load(std::memory_order_acquire);
        if (auto_projection_center_.load(std::memory_order_acquire) && have_frame && frame_snapshot.stereo_views_valid) {
            // V65.07: map 5 already maps the asymmetric OpenXR rays into the
            // actual KCD2 source frustum. Do not add the legacy automatic optical
            // center here. CENTER PROJECTION writes a one-shot residual pitch into
            // pitch_correction_rad_; legacy mappings retain their old auto residual.
            if (uv_reproj_mode != 5) {
                const float up = 0.5f * (frame_snapshot.eye_fov[0].angleUp + frame_snapshot.eye_fov[1].angleUp);
                const float down = 0.5f * (frame_snapshot.eye_fov[0].angleDown + frame_snapshot.eye_fov[1].angleDown);
                effective_pitch_rad += -0.5f * (up + down);
            }
        }
        effective_pitch_rad = std::clamp(effective_pitch_rad, -0.261799388f, 0.261799388f);
        effective_pitch_correction_deg_.store(effective_pitch_rad * 57.29577951308232f, std::memory_order_release);
        present[27] = (!flat_ui && presentation_mode == 3) ? effective_pitch_rad : 0.0f;
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
            if (uv_reproj_mode == 5) {
                // V65.27: keep the REAL per-eye FOV for sampling and submission.
                // Both eyes then evaluate the same physical ray through the same
                // VR-FOV calibration, preventing the old common-frustum double image.
                const float target_vfov=std::clamp(corrected_view_vertical_fov_deg_.load(std::memory_order_acquire),60.0f,150.0f);
                const float target_y=std::tan(target_vfov*0.5f*0.01745329251994329577f);
                const float target_x=target_y*src_aspect;
                present[16]=target_x;
                present[17]=target_y;
                present[18]=0.0f;
                present[19]=0.0f;
                present[20]=3.0f;
                present[21]=0.0f;
                mapped_fov=frame_snapshot.eye_fov[eye];
            }
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
            const float requested_scale = (uv_reproj_mode == 5) ? 1.0f : (1.0f / zoom);
            const float coverage_limit = 0.995f * std::min(max_scale_x, max_scale_y);
            // V58.7: CROP mode preserves the requested angular scale exactly.
            // F5 retains the old global coverage clamp only as an A/B fallback.
            const float safe_scale = (uv_reproj_mode == 5 || angular_correct_mono) ? requested_scale : std::min(requested_scale, coverage_limit);
            present[0] = safe_scale;
            present[1] = safe_scale;
        }
        present[30] = g_v6678_hud_active_for_blit ? 1.0f : 0.0f; present[31] = render_presentation59c2_.comfort_vignette51; // one captured value for both eyes
        active_list->SetGraphicsRoot32BitConstants(1, 32, present, 0);
        active_list->DrawInstanced(3, 1, 0, 0);
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
    if (end_count) active_list->ResourceBarrier(end_count, end);
    hr = active_list->Close(); if (FAILED(hr)) return fail("list-close");
    ID3D12CommandList* lists[] = { active_list };
    if (!DeviceHook::ExecuteCommandListsBypass(d3d12_queue_, 1, lists)) return fail("execute");
    if (safe_to_drop) *safe_to_drop = false;
    ++fence_value_;
    if (FAILED(d3d12_queue_->Signal(fence_, fence_value_))) {
        if(nonblocking) stereo_submit_ring_disabled_=true;
        return fail("signal");
    }
    if(nonblocking) {
        submit_slot->fence_value=fence_value_;
        // OpenXR's D3D12 binding owns this same DIRECT queue. Once commands that
        // reference the waited swapchain image are submitted and its resource state
        // remains RENDER_TARGET, xrReleaseSwapchainImage can follow without a CPU
        // fence wait. Slot reuse is guarded by the per-slot fence above.
    } else {
        if (FAILED(fence_->SetEventOnCompletion(fence_value_, fence_event_))) return fail("postwait-set-event");
        if (TimedFenceWait165(fence_event_, 1000) != WAIT_OBJECT_0) return fail("postwait-timeout");
    }
    DeviceHook::SetInternalVrCommandList(command_list_);

    if(refresh_pair) {
        bridge_present_cache_valid_=true;
        bridge_present_cache_serial_=pair_serial;
        bridge_present_cache_owner_d85_=pair_owner;
        bridge_present_cache_camera_frame_=pair_camera_frame;
    }
    static std::atomic<unsigned long long> stable_blits{0};
    const auto n = stable_blits.fetch_add(1, std::memory_order_relaxed) + 1;
    if (n <= 24 || (n % 300ull) == 0) {
        char msg[448]{};
        std::snprintf(msg, sizeof(msg),
            "V66.27-XR-STABLE-PAIR OK call=%llu serial=%llu poseFrame=%llu action=%s cacheSerial=%llu cachePose=%llu submit=%s srcL=%p srcR=%p size=%llux%u xr=%dx%d xrFov=%s",
            static_cast<unsigned long long>(n),
            static_cast<unsigned long long>(pair_serial),
            static_cast<unsigned long long>(pair_camera_frame),
            refresh_pair?"ATOMIC-REFRESH+PRESENT":"REPRESENT-STABLE-CACHE",
            static_cast<unsigned long long>(bridge_present_cache_serial_),
            static_cast<unsigned long long>(bridge_present_cache_camera_frame_),
            nonblocking?"RING-NONBLOCKING":"BLOCKING-FALLBACK",
            left_source,right_source,
            static_cast<unsigned long long>(left_desc.Width),left_desc.Height,
            eyes_[0].width,eyes_[0].height,
            used_openxr_fov_this_blit ? "true" : "false");
        debug_trace::Write(msg);
    }
    return true;
}

bool XrManager::BlitAfrStereoToEyes(ID3D12Resource* source, int source_eye, std::uint64_t serial,
                                     ID3D12Resource* left, ID3D12Resource* right, D3D12_RESOURCE_STATES source_state)
{
    if (!source || !left || !right || !command_allocator_ || !command_list_ || !fence_ ||
        !d3d12_queue_ || !blit_pipeline_state_ || !blit_root_signature_ || !srv_heap_) return false;
    source_eye &= 1;
    const D3D12_RESOURCE_DESC src_desc = source->GetDesc();
    if (!EnsureAfrCaches(src_desc)) return false;

    const int presentation_mode = presentation_mode_.load(std::memory_order_acquire);
    const float presentation_zoom = presentation_zoom_.load(std::memory_order_acquire) *
        dialogue_zoom_multiplier_.load(std::memory_order_acquire);
    const float source_vertical_fov_deg = source_vertical_fov_deg_.load(std::memory_order_acquire);
    const bool angular_correct_mono = angular_correct_mono_.load(std::memory_order_acquire);
    const int uv_reproj_mode = mono_uv_reprojection_mode_.load(std::memory_order_acquire);
    const float eye_offset_x[2] = {
        eye_offset_x_[0].load(std::memory_order_acquire), eye_offset_x_[1].load(std::memory_order_acquire) };
    const float eye_offset_y[2] = {
        eye_offset_y_[0].load(std::memory_order_acquire), eye_offset_y_[1].load(std::memory_order_acquire) };

    if (fence_->GetCompletedValue() < fence_value_) {
        if (FAILED(fence_->SetEventOnCompletion(fence_value_, fence_event_))) return false;
        if (TimedFenceWait165(fence_event_, 1000) != WAIT_OBJECT_0) return false;
    }
    HRESULT hr = command_allocator_->Reset(); if (FAILED(hr)) return false;
    hr = command_list_->Reset(command_allocator_, blit_pipeline_state_); if (FAILED(hr)) return false;

    const bool new_frame = serial != 0 && serial != afr_last_present_serial_;
    if (new_frame) {
        D3D12_RESOURCE_BARRIER copy_begin[2]{};
        copy_begin[0].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        copy_begin[0].Transition.pResource = source;
        copy_begin[0].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        copy_begin[0].Transition.StateBefore = source_state;
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
        copy_end[0].Transition.StateAfter = source_state;
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
        color_cpu.ptr += static_cast<SIZE_T>(eye) * 3 * srv_descriptor_size_;
        D3D12_CPU_DESCRIPTOR_HANDLE depth_cpu = color_cpu;
        depth_cpu.ptr += static_cast<SIZE_T>(srv_descriptor_size_);
        const float depth_weight = WriteColorAndDepthDescriptors(
            eye_sources[eye], Kcd2VrSrvFormat(cache_desc.Format), color_cpu, depth_cpu);
        D3D12_GPU_DESCRIPTOR_HANDLE srv_gpu = srv_heap_->GetGPUDescriptorHandleForHeapStart();
        srv_gpu.ptr += static_cast<UINT64>(eye) * 3 * srv_descriptor_size_;
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
        const bool flat_ui = render_presentation59c2_.policy.flat || render_presentation59c2_.policy.physical_map;
        const bool have_fov = !flat_ui && presentation_mode == 3 && have_frame && frame_snapshot.stereo_views_valid;
        const float vfov_deg = source_vertical_fov_deg < 20.0f ? 20.0f : (source_vertical_fov_deg > 160.0f ? 160.0f : source_vertical_fov_deg);
        const float source_tan_y = std::tan(vfov_deg * 0.008726646259971648f);
        const float source_tan_x = source_tan_y * src_aspect;
        // V65.18 CYCLOPEAN FIT REMAP: vfov_deg is the full geometry KCD2 actually
        // rendered (hard 120). Mode 5 maps the complete source angular extent
        // into each runtime eye; there is no fitted/crop FOV scalar.
        const bool marker = presentation_test_marker_.load(std::memory_order_acquire);
        const float sharp = std::clamp(sharpening_strength_.load(std::memory_order_acquire), 0.0f, 0.75f);
        const float common_center_x = presentation_center_x_.load(std::memory_order_acquire);
        const float common_center_y = presentation_center_y_.load(std::memory_order_acquire);
        float present[32] = { (sx / zoom), (sy / zoom), eye_offset_x[eye] + common_center_x, eye_offset_y[eye] + common_center_y, 0,0,0,0, source_tan_x, source_tan_y, have_fov ? 1.0f : 0.0f, marker ? static_cast<float>(eye + 1) : 0.0f,
                              1.0f / static_cast<float>(std::max<UINT64>(1, cache_desc.Width)), 1.0f / static_cast<float>(std::max<UINT>(1, cache_desc.Height)), sharp, 0.0f };
        present[15] = depth_weight; // V61.35: presentPad now carries depthPushWeight (see WriteColorAndDepthDescriptors)
        present[24] = output_gamma_.load(std::memory_order_acquire);
        present[25] = output_brightness_.load(std::memory_order_acquire);
        present[26] = output_contrast_.load(std::memory_order_acquire);
        { const auto dw=depth_tex_width_.load(std::memory_order_acquire); const auto dh=depth_tex_height_.load(std::memory_order_acquire);
          present[28]=1.0f/static_cast<float>(dw?dw:std::max<UINT64>(1, cache_desc.Width));
          present[29]=1.0f/static_cast<float>(dh?dh:std::max<UINT>(1, cache_desc.Height)); }
        float effective_pitch_rad = pitch_correction_rad_.load(std::memory_order_acquire);
        if (auto_projection_center_.load(std::memory_order_acquire) && have_frame && frame_snapshot.stereo_views_valid) {
            // V65.07: map 5 already maps the asymmetric OpenXR rays into the
            // actual KCD2 source frustum. Do not add the legacy automatic optical
            // center here. CENTER PROJECTION writes a one-shot residual pitch into
            // pitch_correction_rad_; legacy mappings retain their old auto residual.
            if (uv_reproj_mode != 5) {
                const float up = 0.5f * (frame_snapshot.eye_fov[0].angleUp + frame_snapshot.eye_fov[1].angleUp);
                const float down = 0.5f * (frame_snapshot.eye_fov[0].angleDown + frame_snapshot.eye_fov[1].angleDown);
                effective_pitch_rad += -0.5f * (up + down);
            }
        }
        effective_pitch_rad = std::clamp(effective_pitch_rad, -0.261799388f, 0.261799388f);
        effective_pitch_correction_deg_.store(effective_pitch_rad * 57.29577951308232f, std::memory_order_release);
        present[27] = (!flat_ui && presentation_mode == 3) ? effective_pitch_rad : 0.0f;
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
            if (uv_reproj_mode == 5) {
                // V65.27: keep the REAL per-eye FOV for sampling and submission.
                // Both eyes then evaluate the same physical ray through the same
                // VR-FOV calibration, preventing the old common-frustum double image.
                const float target_vfov=std::clamp(corrected_view_vertical_fov_deg_.load(std::memory_order_acquire),60.0f,150.0f);
                const float target_y=std::tan(target_vfov*0.5f*0.01745329251994329577f);
                const float target_x=target_y*src_aspect;
                present[16]=target_x;
                present[17]=target_y;
                present[18]=0.0f;
                present[19]=0.0f;
                present[20]=3.0f;
                present[21]=0.0f;
                mapped_fov=frame_snapshot.eye_fov[eye];
            }
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
            const float requested_scale = (uv_reproj_mode == 5) ? 1.0f : (1.0f / zoom);
            const float coverage_limit = 0.995f * std::min(max_scale_x, max_scale_y);
            // V58.7: CROP mode preserves the requested angular scale exactly.
            // F5 retains the old global coverage clamp only as an A/B fallback.
            const float safe_scale = (uv_reproj_mode == 5 || angular_correct_mono) ? requested_scale : std::min(requested_scale, coverage_limit);
            present[0] = safe_scale;
            present[1] = safe_scale;
        }
        present[30] = g_v6678_hud_active_for_blit ? 1.0f : 0.0f; present[31] = render_presentation59c2_.comfort_vignette51; // one captured value for both eyes
        command_list_->SetGraphicsRoot32BitConstants(1, 32, present, 0);
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
    if (TimedFenceWait165(fence_event_, 1000) != WAIT_OBJECT_0) return false;

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

bool XrManager::CaptureV6589PackedPresent(ID3D12Resource* source, std::uint64_t present_serial)
{
    if (!source || !present_serial || !d3d12_device_ || !d3d12_queue_ || !command_allocator_ || !command_list_ || !fence_)
        return false;
    const auto sd = source->GetDesc();
    if (sd.Dimension != D3D12_RESOURCE_DIMENSION_TEXTURE2D || sd.Width == 0 || sd.Height == 0 || sd.SampleDesc.Count != 1)
        return false;
    const bool same = v6589_packed_snapshot_ && v6589_packed_desc_.Dimension == sd.Dimension &&
        v6589_packed_desc_.Width == sd.Width && v6589_packed_desc_.Height == sd.Height &&
        v6589_packed_desc_.Format == sd.Format && v6589_packed_desc_.SampleDesc.Count == sd.SampleDesc.Count &&
        v6589_packed_desc_.Flags == sd.Flags;
    if (!same) {
        if (v6589_packed_snapshot_) { v6589_packed_snapshot_->Release(); v6589_packed_snapshot_ = nullptr; }
        D3D12_HEAP_PROPERTIES hp{}; hp.Type=D3D12_HEAP_TYPE_DEFAULT; hp.CreationNodeMask=1; hp.VisibleNodeMask=1;
        if (FAILED(d3d12_device_->CreateCommittedResource(&hp,D3D12_HEAP_FLAG_NONE,&sd,D3D12_RESOURCE_STATE_COMMON,nullptr,IID_PPV_ARGS(&v6589_packed_snapshot_))))
            return false;
        v6589_packed_desc_=sd;
        char m[320]{}; std::snprintf(m,sizeof(m),"V65.89-PACKED-SNAPSHOT create=%p size=%llux%u fmt=%u flags=0x%X",
            v6589_packed_snapshot_,(unsigned long long)sd.Width,sd.Height,(unsigned)sd.Format,(unsigned)sd.Flags); diagnostic_trace::Write(m);
    }
    if (fence_->GetCompletedValue() < fence_value_) {
        if (FAILED(fence_->SetEventOnCompletion(fence_value_, fence_event_)) || TimedFenceWait165(fence_event_,1000)!=WAIT_OBJECT_0) return false;
    }
    if (FAILED(command_allocator_->Reset()) || FAILED(command_list_->Reset(command_allocator_, nullptr))) return false;
    D3D12_RESOURCE_BARRIER pre[2]{};
    pre[0].Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION; pre[0].Transition.pResource=source; pre[0].Transition.Subresource=D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    pre[0].Transition.StateBefore=D3D12_RESOURCE_STATE_PRESENT; pre[0].Transition.StateAfter=D3D12_RESOURCE_STATE_COPY_SOURCE;
    pre[1].Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION; pre[1].Transition.pResource=v6589_packed_snapshot_; pre[1].Transition.Subresource=D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    pre[1].Transition.StateBefore=D3D12_RESOURCE_STATE_COMMON; pre[1].Transition.StateAfter=D3D12_RESOURCE_STATE_COPY_DEST;
    command_list_->ResourceBarrier(2,pre); command_list_->CopyResource(v6589_packed_snapshot_,source);
    D3D12_RESOURCE_BARRIER post[2]={pre[0],pre[1]};
    post[0].Transition.StateBefore=D3D12_RESOURCE_STATE_COPY_SOURCE; post[0].Transition.StateAfter=D3D12_RESOURCE_STATE_PRESENT;
    post[1].Transition.StateBefore=D3D12_RESOURCE_STATE_COPY_DEST; post[1].Transition.StateAfter=D3D12_RESOURCE_STATE_COMMON;
    command_list_->ResourceBarrier(2,post);
    if (FAILED(command_list_->Close())) return false;
    ID3D12CommandList* lists[]={command_list_}; if(!DeviceHook::ExecuteCommandListsBypass(d3d12_queue_,1,lists)) return false;
    ++fence_value_; if(FAILED(d3d12_queue_->Signal(fence_,fence_value_))) return false;
    if(FAILED(fence_->SetEventOnCompletion(fence_value_,fence_event_)) || TimedFenceWait165(fence_event_,1000)!=WAIT_OBJECT_0) return false;
    v6589_packed_source_present_serial_=present_serial;
    const auto frame_serial=v6589_packed_frame_serial_.fetch_add(1,std::memory_order_acq_rel)+1;
    char m[384]{}; std::snprintf(m,sizeof(m),"V65.89-PACKED-SNAPSHOT READY frameSerial=%llu presentSerial=%llu source=%p copy=%p state=COMMON",
        (unsigned long long)frame_serial,(unsigned long long)present_serial,source,v6589_packed_snapshot_); diagnostic_trace::Write(m);
    return true;
}

bool XrManager::BlitGameImageToEyes(ID3D12Resource* source, ID3D12Resource* left, ID3D12Resource* right, D3D12_RESOURCE_STATES source_state)
{
    if (!source || !left || !right || !command_allocator_ || !command_list_ || !fence_ ||
        !d3d12_queue_ || !blit_pipeline_state_ || !blit_root_signature_ || !srv_heap_) return false;

    const D3D12_RESOURCE_DESC src_desc = source->GetDesc();
    if (src_desc.Dimension != D3D12_RESOURCE_DIMENSION_TEXTURE2D || src_desc.SampleDesc.Count != 1) return false;
    last_source_width_.store(static_cast<std::uint32_t>(src_desc.Width),std::memory_order_release);
    last_source_height_.store(static_cast<std::uint32_t>(src_desc.Height),std::memory_order_release);
    const int presentation_mode = presentation_mode_.load(std::memory_order_acquire);
    const float presentation_zoom = presentation_zoom_.load(std::memory_order_acquire) *
        dialogue_zoom_multiplier_.load(std::memory_order_acquire);
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
        if (TimedFenceWait165(fence_event_, 1000) != WAIT_OBJECT_0) return false;
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
    source_to_srv.Transition.StateBefore = source_state;
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
    if (uv_reproj_mode > 0 && (presentation_mode == 2 || presentation_mode == 3) && CopyActiveRenderFrameState(reproj_current) && reproj_current.head_pose_valid && reproj_current.stereo_views_valid) {
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
        const bool flat_ui = render_presentation59c2_.policy.flat || render_presentation59c2_.policy.physical_map;
        const bool have_fov = !flat_ui && presentation_mode == 3 && have_frame && frame_snapshot.stereo_views_valid;
        const float vfov_deg = source_vertical_fov_deg < 20.0f ? 20.0f : (source_vertical_fov_deg > 160.0f ? 160.0f : source_vertical_fov_deg);
        const float source_tan_y = std::tan(vfov_deg * 0.008726646259971648f);
        const float source_tan_x = source_tan_y * src_aspect;
        // V65.18 CYCLOPEAN FIT REMAP: vfov_deg is the full geometry KCD2 actually
        // rendered (hard 120). Mode 5 maps the complete source angular extent
        // into each runtime eye; there is no fitted/crop FOV scalar.
        const float sharp = std::clamp(sharpening_strength_.load(std::memory_order_acquire), 0.0f, 0.75f);
        const float common_center_x = presentation_center_x_.load(std::memory_order_acquire);
        const float common_center_y = presentation_center_y_.load(std::memory_order_acquire);
        const int count2_present_mode = render_presentation59c2_.policy.stereo;
        const bool count2_half_reference = count2_present_mode == 3;
        const bool count2_full_raw = count2_present_mode == 4;
        const bool count2_full_projective = count2_present_mode == 5;
        const bool count2_packed_projective = count2_present_mode == 12;
        const bool count2_special_source =
            count2_half_reference || count2_full_raw || count2_full_projective || count2_packed_projective;
        if (count2_half_reference || count2_packed_projective) {
            // V65.89 resurrects the V65.76-79 hypothesis: one final COUNT2 raster
            // contains an eye-local image in each horizontal region. Mode 12 applies
            // projective intrinsics first, then selects the matching half.
            sx = 0.5f; sy = 1.0f;
        } else if (count2_full_raw || count2_full_projective) {
            // V65.81: native COUNT2 observable raster state is full 1920x1800.
            // Do NOT impose an SBS crop. Each XR eye samples the full source frame.
            sx = 1.0f; sy = 1.0f;
        }
        float present[32] = { (sx / zoom), (sy / zoom), eye_offset_x[eye] + common_center_x, eye_offset_y[eye] + common_center_y, 0,0,0,0, source_tan_x, source_tan_y, have_fov ? 1.0f : 0.0f, marker_enabled_this_blit ? static_cast<float>(eye + 1) : 0.0f,
                              1.0f / static_cast<float>(std::max<UINT64>(1, src_desc.Width)), 1.0f / static_cast<float>(std::max<UINT>(1, src_desc.Height)), sharp, 0.0f,
                              0,0,0,1, 0,(eye == 0 ? -uv_binocular : uv_binocular), static_cast<float>(bottom_edge_mode),0 };
        if (count2_half_reference) {
            present[0] = 0.5f;
            present[1] = 1.0f;
            present[2] = (eye == 0 ? -0.25f : +0.25f);
            present[3] = 0.0f;
            present[10] = 0.0f;
            present[20] = 0.0f;
        } else if (count2_packed_projective) {
            present[0] = 0.5f;
            present[1] = 1.0f;
            present[2] = (eye == 0 ? -0.25f : +0.25f);
            present[3] = 0.0f;
            present[20] = 0.0f;
            if (have_frame && frame_snapshot.stereo_views_valid && !flat_ui) {
                const XrFovf& outF = frame_snapshot.eye_fov[eye];
                present[4] = std::tan(outF.angleLeft);
                present[5] = std::tan(outF.angleRight);
                present[6] = std::tan(outF.angleDown);
                present[7] = std::tan(outF.angleUp);
                present[20] = 2.0f; // existing STATIC INTRINSICS HOMOGRAPHY branch
                present[21] = 0.0f;
                present[10] = 1.0f;
                static std::atomic<unsigned long long> s_v6589_map_logs{0};
                const auto n=s_v6589_map_logs.fetch_add(1,std::memory_order_relaxed)+1;
                if(n<=12 || (n%240)==0) {
                    char bm[768]{};
                    std::snprintf(bm,sizeof(bm),
                        "V65.89-PACKED-PROJECTIVE eye=%d src=%llux%u sourceVFov=%.3f sourceTan={%.6f,%.6f} xrTan={%.6f,%.6f,%.6f,%.6f} region=%s mapper=STATIC-INTRINSICS-HOMOGRAPHY-THEN-HALF",
                        eye,static_cast<unsigned long long>(src_desc.Width),src_desc.Height,double(vfov_deg),
                        double(source_tan_x),double(source_tan_y),
                        double(present[4]),double(present[5]),double(present[6]),double(present[7]),eye==0?"LEFT-HALF":"RIGHT-HALF");
                    debug_trace::Write(bm); diagnostic_trace::Write(bm);
                }
            }
        } else if (count2_full_raw) {
            present[0] = 1.0f;
            present[1] = 1.0f;
            present[2] = 0.0f;
            present[3] = 0.0f;
            present[10] = 0.0f;
            present[20] = 0.0f;
        } else if (count2_full_projective) {
            present[0] = 1.0f;
            present[1] = 1.0f;
            present[2] = 0.0f;
            present[3] = 0.0f;
            present[20] = 0.0f;
            if (have_frame && frame_snapshot.stereo_views_valid && !flat_ui) {
                const XrFovf& outF = frame_snapshot.eye_fov[eye];
                present[4] = std::tan(outF.angleLeft);
                present[5] = std::tan(outF.angleRight);
                present[6] = std::tan(outF.angleDown);
                present[7] = std::tan(outF.angleUp);
                present[20] = 2.0f; // existing STATIC INTRINSICS HOMOGRAPHY branch
                present[21] = 0.0f;
                present[10] = 1.0f;
                static std::atomic<unsigned long long> s_v6580_logs{0};
                const auto n=s_v6580_logs.fetch_add(1,std::memory_order_relaxed)+1;
                if(n<=8 || (n%240)==0) {
                    char bm[704]{};
                    std::snprintf(bm,sizeof(bm),
                        "V65.81-FULL-PROJECTIVE eye=%d src=%llux%u sourceVFov=%.3f sourceTan={%.6f,%.6f} xrTan={%.6f,%.6f,%.6f,%.6f} uv={scale:1.0 off:0.0} mapper=STATIC-INTRINSICS-HOMOGRAPHY sbsAssumption=NO",
                        eye,static_cast<unsigned long long>(src_desc.Width),src_desc.Height,double(vfov_deg),
                        double(source_tan_x),double(source_tan_y),
                        double(present[4]),double(present[5]),double(present[6]),double(present[7]));
                    debug_trace::Write(bm); diagnostic_trace::Write(bm);
                }
            }
        }
        present[15] = mono_depth_weight; // V61.35: presentPad now carries depthPushWeight
        present[24] = output_gamma_.load(std::memory_order_acquire);
        present[25] = output_brightness_.load(std::memory_order_acquire);
        present[26] = output_contrast_.load(std::memory_order_acquire);
        { const auto dw=depth_tex_width_.load(std::memory_order_acquire); const auto dh=depth_tex_height_.load(std::memory_order_acquire);
          present[28]=1.0f/static_cast<float>(dw?dw:std::max<UINT64>(1, src_desc.Width));
          present[29]=1.0f/static_cast<float>(dh?dh:std::max<UINT>(1, src_desc.Height)); }
        float effective_pitch_rad = pitch_correction_rad_.load(std::memory_order_acquire);
        if (auto_projection_center_.load(std::memory_order_acquire) && have_frame && frame_snapshot.stereo_views_valid) {
            // V65.07: map 5 already maps the asymmetric OpenXR rays into the
            // actual KCD2 source frustum. Do not add the legacy automatic optical
            // center here. CENTER PROJECTION writes a one-shot residual pitch into
            // pitch_correction_rad_; legacy mappings retain their old auto residual.
            if (uv_reproj_mode != 5) {
                const float up = 0.5f * (frame_snapshot.eye_fov[0].angleUp + frame_snapshot.eye_fov[1].angleUp);
                const float down = 0.5f * (frame_snapshot.eye_fov[0].angleDown + frame_snapshot.eye_fov[1].angleDown);
                effective_pitch_rad += -0.5f * (up + down);
            }
        }
        effective_pitch_rad = std::clamp(effective_pitch_rad, -0.261799388f, 0.261799388f);
        effective_pitch_correction_deg_.store(effective_pitch_rad * 57.29577951308232f, std::memory_order_release);
        present[27] = (!flat_ui && presentation_mode == 3) ? effective_pitch_rad : 0.0f;
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
        if (reproj_available && !flat_ui && presentation_mode == 3 && (uv_reproj_mode == 1 || uv_reproj_mode == 2)) {
            present[16] = reproj_q.x; present[17] = reproj_q.y; present[18] = reproj_q.z; present[19] = reproj_q.w;
            present[20] = 1.0f;
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
        if (have_fov && !flat_ui && !count2_special_source) {
            used_openxr_fov_this_blit = true;
            // V60.7 diagnostic mode 4: use the same asymmetry-adjusted FOV for
            // source-ray mapping that the projection layer will submit to OpenXR.
            // This is a static intrinsics consistency test only -- no pose delta.
            XrFovf mapped_fov = frame_snapshot.eye_fov[eye];
            if (uv_reproj_mode == 5) {
                // V65.27: keep the REAL per-eye FOV for sampling and submission.
                // Both eyes then evaluate the same physical ray through the same
                // VR-FOV calibration, preventing the old common-frustum double image.
                const float target_vfov=std::clamp(corrected_view_vertical_fov_deg_.load(std::memory_order_acquire),60.0f,150.0f);
                const float target_y=std::tan(target_vfov*0.5f*0.01745329251994329577f);
                const float target_x=target_y*src_aspect;
                present[16]=target_x;
                present[17]=target_y;
                present[18]=0.0f;
                present[19]=0.0f;
                present[20]=3.0f;
                present[21]=0.0f;
                mapped_fov=frame_snapshot.eye_fov[eye];
            }
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
            const float requested_scale = (uv_reproj_mode == 5) ? 1.0f : (1.0f / zoom);
            const float coverage_limit = 0.995f * std::min(max_scale_x, max_scale_y);
            // V58.7: Crysis-style mono crop keeps the true requested angular scale.
            // The global no-black clamp survives only behind the F5 comparison.
            const float safe_scale = (uv_reproj_mode == 5 || angular_correct_mono) ? requested_scale : std::min(requested_scale, coverage_limit);
            requested_uv_scale_eye[eye] = requested_scale;
            coverage_clamped_eye[eye] = !angular_correct_mono && (safe_scale + 0.0005f < requested_scale);
            clamp_total_count_.fetch_add(1, std::memory_order_relaxed);
            if (coverage_clamped_eye[eye])
                clamp_active_count_.fetch_add(1, std::memory_order_relaxed);
            present[0] = safe_scale;
            present[1] = safe_scale;
            safe_uv_scale_eye[eye] = safe_scale;
        }
        present[30] = g_v6678_hud_active_for_blit ? 1.0f : 0.0f; present[31] = render_presentation59c2_.comfort_vignette51; // one captured value for both eyes
        command_list_->SetGraphicsRoot32BitConstants(1, 32, present, 0);
        command_list_->DrawInstanced(3, 1, 0, 0);
    }

    D3D12_RESOURCE_BARRIER source_to_present = source_to_srv;
    source_to_present.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    source_to_present.Transition.StateAfter = source_state;
    command_list_->ResourceBarrier(1, &source_to_present);
    hr = command_list_->Close(); if (FAILED(hr)) return false;
    ID3D12CommandList* lists[] = { command_list_ };
    if (!DeviceHook::ExecuteCommandListsBypass(d3d12_queue_, 1, lists)) return false;
    ++fence_value_;
    if (FAILED(d3d12_queue_->Signal(fence_, fence_value_))) return false;
    if (FAILED(fence_->SetEventOnCompletion(fence_value_, fence_event_))) return false;
    if (TimedFenceWait165(fence_event_, 1000) != WAIT_OBJECT_0) return false;

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
    // Same queue and waited XR image contract as the existing stereo submit
    // ring. The normal UI path must not wait for the scene just submitted.
    StereoSubmitSlot* slot=nullptr;
    if(stereo_stability_experiment_.load(std::memory_order_acquire) ||
       nonblocking_pair_submit_.load(std::memory_order_acquire))
        slot=AcquireStereoSubmitSlot();
    auto* allocator=slot?slot->allocator:command_allocator_;
    auto* list=slot?slot->list:command_list_;
    if (!slot && fence_->GetCompletedValue() < fence_value_) {
        if (FAILED(fence_->SetEventOnCompletion(fence_value_, fence_event_))) return false;
        if (TimedFenceWait165(fence_event_, 1000) != WAIT_OBJECT_0) return false;
    }
    if (FAILED(allocator->Reset()) || FAILED(list->Reset(allocator, nullptr))) return false;
    DeviceHook::SetInternalVrCommandList(list);
    struct InternalListRestoreR13 {
        ID3D12GraphicsCommandList* prior;
        ~InternalListRestoreR13(){DeviceHook::SetInternalVrCommandList(prior);}
    } restore{command_list_};
    D3D12_RESOURCE_BARRIER b[2]{};
    b[0].Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION; b[0].Transition.pResource=source; b[0].Transition.Subresource=D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    b[0].Transition.StateBefore=D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE; b[0].Transition.StateAfter=D3D12_RESOURCE_STATE_COPY_SOURCE;
    b[1].Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION; b[1].Transition.pResource=target; b[1].Transition.Subresource=D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    b[1].Transition.StateBefore=D3D12_RESOURCE_STATE_RENDER_TARGET; b[1].Transition.StateAfter=D3D12_RESOURCE_STATE_COPY_DEST;
    list->ResourceBarrier(2,b); list->CopyResource(target,source);
    std::swap(b[0].Transition.StateBefore,b[0].Transition.StateAfter); std::swap(b[1].Transition.StateBefore,b[1].Transition.StateAfter); list->ResourceBarrier(2,b);
    if (FAILED(list->Close())) return false;
    if(slot) {
        source->AddRef(); target->AddRef();
        slot->ui_source=source; slot->ui_target=target;
    }
    ID3D12CommandList* lists[]={list};
    if(!DeviceHook::ExecuteCommandListsBypass(d3d12_queue_,1,lists)) {
        if(slot) {
            slot->ui_source->Release(); slot->ui_source=nullptr;
            slot->ui_target->Release(); slot->ui_target=nullptr;
        }
        return false;
    }
    if(!slot) {
        ++fence_value_; if(FAILED(d3d12_queue_->Signal(fence_,fence_value_))) return false;
        if(FAILED(fence_->SetEventOnCompletion(fence_value_,fence_event_)) ||
           TimedFenceWait165(fence_event_,1000)!=WAIT_OBJECT_0) return false;
        return true;
    }
    const UINT64 submitted_fence=fence_value_+1;
    slot->fence_value=submitted_fence;
    if(FAILED(d3d12_queue_->Signal(fence_,submitted_fence))) {
        // The list is already queued. Keep its leases/allocator pinned, and let
        // a later successful same-queue fence or shutdown retire it safely.
        stereo_submit_ring_disabled_=true;
        return false;
    }
    fence_value_=submitted_fence;
    return true;
}

XrManager::FramePresentation59c2 XrManager::CapturePresentation59c2()
{
    FramePresentation59c2 frame{};
    frame.ticket=production_presentation59c2_.Read();
    frame.policy=ui_menu_transition59c2::ResolveFrame(frame.ticket,{
        stereo_experiment_.load(std::memory_order_acquire),flat_ui_mode_.load(std::memory_order_acquire),
        native_menu_plane_mode_.load(std::memory_order_acquire),physical_map_mode_.load(std::memory_order_acquire),
        theater_drain_active_.load(std::memory_order_acquire)});
    if(frame.policy.flat||frame.policy.physical_map) {
        std::lock_guard<std::mutex> lock(flat_ui_mutex_);
        frame.anchor=flat_ui_anchor_pose_;frame.anchor_valid=flat_ui_anchor_valid_;
    }
    return frame;
}

bool XrManager::SubmitBridgeImagesOnRenderThread(ID3D12Resource* left, ID3D12Resource* right)
{
    diagnostics::xr165::Timer timing165(diagnostics::xr165::Bridge);
    ID3D12Resource* source = nullptr;
    int source_eye = 0;
    std::uint64_t serial = 0;
    const int stereo_mode = render_presentation59c2_.policy.stereo;
    // V66.33: theater drain runs here, on the XR render thread that owns
    // bridge_last_consumed_pair_serial_. Mode 0 never samples bridge pair textures,
    // so marking the newest pair consumed only releases producer backpressure.
    if(stereo_mode==0 && render_presentation59c2_.policy.drain)
        (void)DrainBridgePairForTheater();
    // V65.93 F5 rungs 2-5: owned scene-color pair produced only by the
    // post-submit completion bridge. Rung 1 is read-only and never reaches here.
    // A/B are COMMON-state mod-owned snapshots, frozen across control rungs.
    if (stereo_mode >= 17 && stereo_mode <= 20) {
        ID3D12Resource* a=nullptr; ID3D12Resource* b=nullptr; std::uint64_t pair_serial=0;
        const bool have=DeviceHook::TryGetV6593SceneColorPair(&a,&b,&pair_serial);
        bool lok=false,rok=false;
        if(have && pair_serial){
            const auto base=(pair_serial<<4)|1ull;
            if(stereo_mode==17){ lok=BlitAfrStereoToEyes(a,0,base,left,right,D3D12_RESOURCE_STATE_COMMON); if(lok) rok=BlitAfrStereoToEyes(b,1,base+1,left,right,D3D12_RESOURCE_STATE_COMMON); }
            else if(stereo_mode==18){ lok=BlitAfrStereoToEyes(b,0,base+2,left,right,D3D12_RESOURCE_STATE_COMMON); if(lok) rok=BlitAfrStereoToEyes(a,1,base+3,left,right,D3D12_RESOURCE_STATE_COMMON); }
            else if(stereo_mode==19){ lok=BlitAfrStereoToEyes(a,0,base+4,left,right,D3D12_RESOURCE_STATE_COMMON); if(lok) rok=BlitAfrStereoToEyes(a,1,base+5,left,right,D3D12_RESOURCE_STATE_COMMON); }
            else { lok=BlitAfrStereoToEyes(b,0,base+6,left,right,D3D12_RESOURCE_STATE_COMMON); if(lok) rok=BlitAfrStereoToEyes(b,1,base+7,left,right,D3D12_RESOURCE_STATE_COMMON); }
        }
        static std::atomic<unsigned long long> n91{0}; const auto n=n91.fetch_add(1,std::memory_order_relaxed)+1;
        if(n<=48 || (n%300ull)==0){ const char* map=stereo_mode==17?"A1->L/B2->R":stereo_mode==18?"B2->L/A1->R":stereo_mode==19?"A1->BOTH":"B2->BOTH"; char m[448]{}; std::snprintf(m,sizeof(m),
            "V65.93-XR-BRIDGE call=%llu mode=%d mapping=%s havePair=%d serial=%llu pairBlit=%d source=POST-SUBMIT-OWNED-FP16",
            n,stereo_mode,map,have?1:0,(unsigned long long)pair_serial,rok?1:0); diagnostic_trace::Write(m); }
        if(a) a->Release(); if(b) b->Release(); if(rok) return true;
    }

    // V65.90: deterministically reproduce the one interesting V65.85a F9 rung-2
    // observation. In that first run candidate1 accidentally resolved to a real
    // swapchain/PRESENT backbuffer and was SNAP-ONCE, while the ordinary current
    // PRESENT kept feeding the other XR eye. Keep COUNT2 alive, take exactly one
    // owned snapshot when armed, then compare the SAME frozen snapshot against the
    // live current PRESENT without relying on unstable candidate-slot ordering:
    //   13 = exact reproduction: current PRESENT -> L, frozen PRESENT snapshot -> R
    //   14 = swapped:            frozen snapshot -> L, current PRESENT -> R
    //   15 = temporal control:   frozen snapshot -> BOTH
    //   16 = mono control:       current PRESENT -> BOTH
    if (stereo_mode >= 13 && stereo_mode <= 16) {
        if (v6589_packed_capture_enabled_.load(std::memory_order_acquire) && DeviceHook::CurrentExecuteSubmittedPresent()) {
            ID3D12Resource* psrc=nullptr; std::uint64_t ps=0,pt=0,pcf=0;
            if (DeviceHook::TryGetLatestPresentResourceIdentity(&psrc,&ps,&pt,&pcf) && psrc) {
                if (ps && ps!=v6589_packed_source_present_serial_) {
                    if (CaptureV6589PackedPresent(psrc,ps)) {
                        v6590_ghost_ready_.store(true,std::memory_order_release);
                        v6589_packed_capture_enabled_.store(false,std::memory_order_release); // exactly one submitted Present
                    }
                }
                psrc->Release();
            }
        }
        ID3D12Resource* primary=nullptr; std::uint64_t ps=0,pt=0,pcf=0;
        const bool haveP=DeviceHook::TryGetLatestPresentResourceIdentity(&primary,&ps,&pt,&pcf);
        const bool haveS=v6590_ghost_ready_.load(std::memory_order_acquire) && v6589_packed_snapshot_!=nullptr;
        bool lok=false,rok=false;
        if (stereo_mode==13) {
            if(haveP && primary) lok=BlitAfrStereoToEyes(primary,0,(ps<<4)|1ull,left,right,D3D12_RESOURCE_STATE_PRESENT);
            if(lok && haveS) rok=BlitAfrStereoToEyes(v6589_packed_snapshot_,1,(v6589_packed_source_present_serial_<<4)|2ull,left,right,D3D12_RESOURCE_STATE_COMMON);
        } else if (stereo_mode==14) {
            if(haveS) lok=BlitAfrStereoToEyes(v6589_packed_snapshot_,0,(v6589_packed_source_present_serial_<<4)|3ull,left,right,D3D12_RESOURCE_STATE_COMMON);
            if(lok && haveP && primary) rok=BlitAfrStereoToEyes(primary,1,(ps<<4)|4ull,left,right,D3D12_RESOURCE_STATE_PRESENT);
        } else if (stereo_mode==15) {
            if(haveS) lok=BlitAfrStereoToEyes(v6589_packed_snapshot_,0,(v6589_packed_source_present_serial_<<4)|5ull,left,right,D3D12_RESOURCE_STATE_COMMON);
            if(lok && haveS) rok=BlitAfrStereoToEyes(v6589_packed_snapshot_,1,(v6589_packed_source_present_serial_<<4)|6ull,left,right,D3D12_RESOURCE_STATE_COMMON);
        } else { // 16 current PRESENT -> BOTH
            if(haveP && primary) lok=BlitAfrStereoToEyes(primary,0,(ps<<4)|7ull,left,right,D3D12_RESOURCE_STATE_PRESENT);
            if(lok && haveP && primary) rok=BlitAfrStereoToEyes(primary,1,(ps<<4)|8ull,left,right,D3D12_RESOURCE_STATE_PRESENT);
        }
        static std::atomic<unsigned long long> n90{0}; const auto n=n90.fetch_add(1,std::memory_order_relaxed)+1;
        if(n<=48 || (n%300ull)==0){ char m[640]{}; const char* name=stereo_mode==13?"LIVE-L/FROZEN-R":stereo_mode==14?"FROZEN-L/LIVE-R":stereo_mode==15?"FROZEN-BOTH":"LIVE-BOTH"; std::snprintf(m,sizeof(m),
            "V65.90-XR-PRESENT-GHOST call=%llu mode=%d(%s) capture=%d snapshotSerial=%llu sourcePresent=%llu livePresent=%llu primary=%p snapshot=%p leftOk=%d rightOk=%d COUNT2=kept-live",
            n,stereo_mode,name,v6589_packed_capture_enabled_.load()?1:0,(unsigned long long)v6589_packed_frame_serial_.load(),
            (unsigned long long)v6589_packed_source_present_serial_,(unsigned long long)ps,primary,v6589_packed_snapshot_,lok?1:0,rok?1:0); diagnostic_trace::Write(m); }
        if(primary) primary->Release();
        if(rok) return true;
    }

    // V65.89: resurrect the V65.76-79 packed COUNT2 hypothesis using the FINAL
    // submitted PRESENT raster, not an arbitrary MRT. While capture is enabled, each
    // newly submitted game PRESENT is copied into one owned COMMON-state texture.
    // Bounded F10 rungs disable capture after 1/10 frames, freezing that exact COUNT2
    // raster; mode 12 then keeps sampling LEFT/RIGHT horizontal regions independently.
    if (stereo_mode == 12) {
        if (v6589_packed_capture_enabled_.load(std::memory_order_acquire) && DeviceHook::CurrentExecuteSubmittedPresent()) {
            ID3D12Resource* psrc=nullptr; std::uint64_t ps=0,pt=0,pcf=0;
            if (DeviceHook::TryGetLatestPresentResourceIdentity(&psrc,&ps,&pt,&pcf) && psrc) {
                if (ps && ps!=v6589_packed_source_present_serial_) (void)CaptureV6589PackedPresent(psrc,ps);
                psrc->Release();
            }
        }
        bool ok=false;
        if (v6589_packed_snapshot_) ok=BlitGameImageToEyes(v6589_packed_snapshot_,left,right,D3D12_RESOURCE_STATE_COMMON);
        static std::atomic<unsigned long long> n89{0}; const auto n=n89.fetch_add(1,std::memory_order_relaxed)+1;
        if(n<=32 || (n%300ull)==0){ char m[448]{}; std::snprintf(m,sizeof(m),
            "V65.89-XR-PACKED call=%llu capture=%d frameSerial=%llu sourcePresent=%llu snapshot=%p blit=%d mapping=leftHalf->L,rightHalf->R projective=YES",
            n,v6589_packed_capture_enabled_.load()?1:0,(unsigned long long)v6589_packed_frame_serial_.load(),
            (unsigned long long)v6589_packed_source_present_serial_,v6589_packed_snapshot_,ok?1:0); diagnostic_trace::Write(m); }
        if(ok) return true;
    }

    // V65.85: candidate browser. Left eye is the ordinary COUNT2 PRESENT
    // (primary/LEFT-like); right eye is one owned COMMON-state candidate snapshot.
    if (stereo_mode == 5) {
        const int idx=v6585_candidate_index_.load(std::memory_order_acquire);
        ID3D12Resource* primary=nullptr; ID3D12Resource* cand=nullptr;
        std::uint64_t ps=0,pt=0,pcf=0,cser=0;
        const bool haveP=DeviceHook::TryGetLatestPresentResourceIdentity(&primary,&ps,&pt,&pcf);
        const bool haveC=DeviceHook::TryGetV6585Candidate(idx,&cand,&cser);
        bool lok=false,rok=false;
        if(haveP && primary){ lok=BlitAfrStereoToEyes(primary,0,(ps<<4)|1ull,left,right,D3D12_RESOURCE_STATE_PRESENT); }
        if(lok && haveC && cand){ const std::uint64_t cs=(cser<<8)|static_cast<std::uint64_t>((idx&3)+2); rok=BlitAfrStereoToEyes(cand,1,cs,left,right,D3D12_RESOURCE_STATE_COMMON); }
        static std::atomic<unsigned long long> n85{0}; const auto n=n85.fetch_add(1,std::memory_order_relaxed)+1;
        if(n<=24 || (n%300ull)==0){ char m[384]{}; std::snprintf(m,sizeof(m),
            "V65.85-XR-CANDIDATE call=%llu idx=%d haveP=%d presentSerial=%llu haveC=%d candidateSerial=%llu leftOk=%d rightOk=%d candidates=%d",
            n,idx,haveP?1:0,(unsigned long long)ps,haveC?1:0,(unsigned long long)cser,lok?1:0,rok?1:0,DeviceHook::V6585CandidateCount()); diagnostic_trace::Write(m); }
        if(primary) primary->Release(); if(cand) cand->Release();
        if(rok) return true;
    }
    // V65.88 F10 full-stereo ladder: consume the newest submitted same-scene
    // COUNT2 A/B pair. A/B are the old V65.61 consecutive full-resolution fmt28
    // outputs that visually tracked the historical LEFT/RIGHT oracle; publication is
    // delayed until their copy command list has reached ExecuteCommandLists.
    if (stereo_mode == 11) {
        ID3D12Resource* a=nullptr; ID3D12Resource* b=nullptr; std::uint64_t pair_serial=0;
        const bool have_pair=DeviceHook::TryGetV6588LiveStereoPair(&a,&b,&pair_serial);
        bool left_ok=false,pair_ok=false;
        if(have_pair && pair_serial){
            const auto base=(pair_serial<<3)|1ull;
            left_ok=BlitAfrStereoToEyes(a,0,base,left,right,D3D12_RESOURCE_STATE_COMMON);
            if(left_ok) pair_ok=BlitAfrStereoToEyes(b,1,base+1,left,right,D3D12_RESOURCE_STATE_COMMON);
        }
        static std::atomic<unsigned long long> n88{0}; const auto n=n88.fetch_add(1,std::memory_order_relaxed)+1;
        if(n<=32 || (n%300ull)==0){ char m[448]{}; std::snprintf(m,sizeof(m),
            "V65.88-XR-LIVE call=%llu havePair=%d serial=%llu pairBlit=%d mapping=A->L,B->R source=COUNT2 same-scene submitted",
            n,have_pair?1:0,(unsigned long long)pair_serial,pair_ok?1:0); diagnostic_trace::Write(m); }
        if(a) a->Release(); if(b) b->Release();
        if(pair_ok) return true;
    }

    // V65.61 one-shot visual identity test: when the tagged packet probe has produced
    // two owned COMMON-state snapshots, present A->left and B->right.  This precedes
    // the normal mono bridge only while the pair exists; F8 OFF clears it.
    if (stereo_mode == 8 || stereo_mode == 9 || stereo_mode == 10) {
        ID3D12Resource* a=nullptr; ID3D12Resource* b=nullptr; std::uint64_t pair_serial=0;
        const bool have_pair=DeviceHook::TryGetV6587SameFramePair(&a,&b,&pair_serial);
        bool left_ok=false,pair_ok=false;
        if(have_pair && pair_serial!=0){
            const auto base=(pair_serial<<3)|1ull;
            if(stereo_mode==9){
                left_ok=BlitAfrStereoToEyes(b,0,base+2,left,right,D3D12_RESOURCE_STATE_COMMON);
                if(left_ok) pair_ok=BlitAfrStereoToEyes(a,1,base+3,left,right,D3D12_RESOURCE_STATE_COMMON);
            } else if(stereo_mode==10){
                left_ok=BlitAfrStereoToEyes(a,0,base+4,left,right,D3D12_RESOURCE_STATE_COMMON);
                if(left_ok) pair_ok=BlitAfrStereoToEyes(a,1,base+5,left,right,D3D12_RESOURCE_STATE_COMMON);
            } else {
                left_ok=BlitAfrStereoToEyes(a,0,base,left,right,D3D12_RESOURCE_STATE_COMMON);
                if(left_ok) pair_ok=BlitAfrStereoToEyes(b,1,base+1,left,right,D3D12_RESOURCE_STATE_COMMON);
            }
        }
        static std::atomic<unsigned long long> n87{0}; const auto n=n87.fetch_add(1,std::memory_order_relaxed)+1;
        if(n<=24 || (n%300ull)==0){ char m[448]{}; std::snprintf(m,sizeof(m),
            "V65.87-XR-PAIR call=%llu mode=%d mapping=%s havePair=%d serial=%llu pairBlit=%d source=MRT0/MRT1 same-frame COUNT2 frozen",
            n,stereo_mode,stereo_mode==9?"SWAPPED-MRT1/MRT0":(stereo_mode==10?"MRT0-BOTH":"NORMAL-MRT0/MRT1"),
            have_pair?1:0,(unsigned long long)pair_serial,pair_ok?1:0); diagnostic_trace::Write(m); }
        if(a) a->Release(); if(b) b->Release();
        if(pair_ok) return true;
    }
    if (stereo_mode == 2 || stereo_mode == 6 || stereo_mode == 7) {
        ID3D12Resource* left_source=nullptr; ID3D12Resource* right_source=nullptr;
        std::uint64_t pair_serial=0; std::uint64_t pair_camera_frame=0;
        native_upscaler_stereo_d82::PairOwner pair_owner{};
        const bool have_pair=DeviceHook::TryGetBridgeCompletePair(&left_source,&right_source,&pair_serial,&pair_camera_frame,&pair_owner);
        const std::uint64_t candidate_pair_serial=pair_serial;
        const bool fresh_pair=have_pair && pair_serial!=0 && pair_serial!=bridge_last_consumed_pair_serial_;
        if(fresh_pair) {
            static std::uint64_t s_v6636_last_pose_frame=0; static std::uint64_t s_v6636_consumed=0; static std::uint64_t s_v6637_last_produced=0;
            const auto latest_pose=CameraHook::LastPoseFrameIndex();
            const auto pose_age=(latest_pose>=pair_camera_frame)?(latest_pose-pair_camera_frame):0ull;
            const auto pose_gap=(s_v6636_last_pose_frame && pair_camera_frame>=s_v6636_last_pose_frame)?(pair_camera_frame-s_v6636_last_pose_frame):0ull;
            s_v6636_last_pose_frame=pair_camera_frame; ++s_v6636_consumed;
            const auto produced=DeviceHook::BridgeCompletePairCount();
            const auto producer_gap=(s_v6637_last_produced && produced>=s_v6637_last_produced)?(produced-s_v6637_last_produced):0ull;
            const auto superseded_between=(producer_gap>0)?(producer_gap-1):0ull;
            s_v6637_last_produced=produced;
            if(s_v6636_consumed<=12 || (s_v6636_consumed%120ull)==0){ char tm[448]{}; std::snprintf(tm,sizeof(tm),
                "V66.37-XR-CONSUME n=%llu pairSerial=%llu pairPoseFrame=%llu latestPoseFrame=%llu poseAgeFrames=%llu poseGap=%llu producedPairs=%llu producerGap=%llu supersededBetween=%llu xrOpp=%llu cadence=%s",
                (unsigned long long)s_v6636_consumed,(unsigned long long)pair_serial,(unsigned long long)pair_camera_frame,(unsigned long long)latest_pose,
                (unsigned long long)pose_age,(unsigned long long)pose_gap,(unsigned long long)produced,(unsigned long long)producer_gap,(unsigned long long)superseded_between,
                (unsigned long long)DeviceHook::V6636XrFrameOpportunity(),DeviceHook::V6636CadenceOwner()?"XR-DRIVEN":"FREE-RUN"); diagnostic_trace::Write(tm); }
        }
        const bool f2_stability=stereo_stability_experiment_.load(std::memory_order_acquire);
        bool pair_ok=false;
        bool safe_to_drop=false;
        bool candidate_released=false;
        // V66.22: production native stereo no longer travels through the historical
        // one-eye-at-a-time AFR cache. A fresh certified pair atomically refreshes
        // a dedicated stable L/R presentation cache and draws both XR eyes once.
        // Already-consumed bridge textures are never sampled again; XR re-presents
        // only the stable cache until a newer certified pair arrives.
        if(have_pair && pair_serial!=0) {
            ID3D12Resource* xr_left_source=left_source;
            ID3D12Resource* xr_right_source=right_source;
            if(stereo_mode==6) { // swapped control: R -> XR left, L -> XR right
                xr_left_source=right_source;
                xr_right_source=left_source;
            } else if(stereo_mode==7) { // mono control: same LEFT final image -> both XR eyes
                xr_left_source=left_source;
                xr_right_source=left_source;
            }
            pair_ok=BlitBridgePairStableToEyes(
                xr_left_source,xr_right_source,pair_serial,pair_camera_frame,fresh_pair,left,right,&safe_to_drop,pair_owner);
        } else if(f2_stability && bridge_present_cache_valid_) {
            // F2 stability experiment: producer handoff can transiently disappear
            // while the cached pair is still perfectly valid. Re-present that exact
            // L/R cache rather than falling through to the live mono desktop frame.
            pair_serial=bridge_present_cache_serial_;
            pair_camera_frame=bridge_present_cache_camera_frame_;
            pair_ok=BlitBridgePairStableToEyes(
                nullptr,nullptr,pair_serial,pair_camera_frame,false,left,right,&safe_to_drop);
        }
        // If a fresh pair failed before any command list referenced it, F2 ON
        // keeps presentation stereo-only: release that producer pair, then render
        // the previously certified stable cache into the current XR swapchain
        // images instead of falling through to the HUD-containing mono backbuffer.
        if(!pair_ok && f2_stability && have_pair && candidate_pair_serial!=0 && safe_to_drop &&
           bridge_present_cache_valid_ && bridge_present_cache_serial_!=0) {
            bridge_last_consumed_pair_serial_=candidate_pair_serial;
            DeviceHook::MarkBridgePairConsumed(candidate_pair_serial);
            candidate_released=true;
            const std::uint64_t fallback_serial=bridge_present_cache_serial_;
            const std::uint64_t fallback_pose=bridge_present_cache_camera_frame_;
            bool fallback_safe=true;
            pair_ok=BlitBridgePairStableToEyes(
                nullptr,nullptr,fallback_serial,fallback_pose,false,left,right,&fallback_safe);
            if(pair_ok) {
                pair_serial=fallback_serial;
                pair_camera_frame=fallback_pose;
                safe_to_drop=fallback_safe;
                static std::atomic<unsigned long long> stable_retries{0};
                const auto rn=stable_retries.fetch_add(1,std::memory_order_relaxed)+1;
                if(rn<=12 || (rn%120ull)==0) {
                    char rm[320]{}; std::snprintf(rm,sizeof(rm),
                        "V66.27-F2-STABLE-RETRY n=%llu droppedFresh=%llu representedCache=%llu poseFrame=%llu fallback=STEREO-ONLY",
                        (unsigned long long)rn,(unsigned long long)candidate_pair_serial,
                        (unsigned long long)fallback_serial,(unsigned long long)fallback_pose);
                    debug_trace::Write(rm);
                }
            }
        }
        if(pair_ok) {
            if(have_pair && candidate_pair_serial!=0 && !candidate_released) {
                bridge_last_consumed_pair_serial_=candidate_pair_serial;
                DeviceHook::MarkBridgePairConsumed(candidate_pair_serial);
            }
            // V66.30: the long-standing V65.04/V65.10 "OpenXR follows rendered pixels"
            // ownership fix is production behavior and remains independent from V66.27 pacing.
            // The certified L/R pair already carries the renderer-matched pose token
            // selected at publication. Apply that exact token to BOTH
            // XR projection views after a successful pair blit. Do not enable the
            // V66.27 nonblocking ring/stale-cache fallback just to get correct head
            // following.
            if(FrameSync::HeadOwnershipFixEnabled() && pair_camera_frame!=0) {
                last_submitted_present_serial_.store(pair_serial,std::memory_order_release);
                last_submitted_camera_frame_.store(pair_camera_frame,std::memory_order_release);
                std::uint64_t left_pair_pose=0,right_pair_pose=0,shared_pair_pose=0;
                DeviceHook::GetV6638LastPairPoseFrames(left_pair_pose,right_pair_pose,shared_pair_pose);
                if(DeviceHook::V6638PairPoseMode()==3 && left_pair_pose && right_pair_pose) {
                    last_submitted_camera_frame_by_eye_[0].store(left_pair_pose,std::memory_order_release);
                    last_submitted_camera_frame_by_eye_[1].store(right_pair_pose,std::memory_order_release);
                } else {
                    last_submitted_camera_frame_by_eye_[0].store(pair_camera_frame,std::memory_order_release);
                    last_submitted_camera_frame_by_eye_[1].store(pair_camera_frame,std::memory_order_release);
                }
                static std::atomic<unsigned long long> f2_pair_pose_applies{0};
                const auto pn=f2_pair_pose_applies.fetch_add(1,std::memory_order_relaxed)+1;
                if(pn<=12 || (pn%300ull)==0) {
                    std::uint64_t lp=0,rp=0,sp=0; DeviceHook::GetV6638LastPairPoseFrames(lp,rp,sp);
                    char pm[384]{}; std::snprintf(pm,sizeof(pm),
                        "V66.38-PAIR-POSE-APPLY n=%llu pairSerial=%llu shared=%llu mode=%d left=%llu right=%llu owner=RENDERER-MATCHED-LR",
                        pn,(unsigned long long)pair_serial,(unsigned long long)pair_camera_frame,DeviceHook::V6638PairPoseMode(),
                        (unsigned long long)lp,(unsigned long long)rp);
                    debug_trace::Write(pm);
                }
            }
        } else if(have_pair && candidate_pair_serial!=0 && !candidate_released && !safe_to_drop) {
            // V66.23: the certified bridge pair has already been referenced by a
            // command list successfully submitted to the SAME DIRECT queue used by
            // future bridge overwrite copies. Queue ordering guarantees those later
            // overwrites cannot overtake this read, even if the XR completion fence
            // wait times out. Release producer ownership now so one late/failed XR
            // fence wait cannot wedge publication forever in published!=consumed.
            bridge_last_consumed_pair_serial_=candidate_pair_serial;
            DeviceHook::MarkBridgePairConsumed(candidate_pair_serial);
            static std::atomic<unsigned long long> submitted_release{0};
            const auto sn=submitted_release.fetch_add(1,std::memory_order_relaxed)+1;
            if(sn<=24 || (sn%120ull)==0){ char sm[352]{}; std::snprintf(sm,sizeof(sm),
                "V66.23-XR-PAIR-RELEASE n=%llu serial=%llu reason=QUEUE-SUBMITTED-SAME-DIRECT-QUEUE pairBlit=0 nextPair=ALLOWED",
                (unsigned long long)sn,(unsigned long long)candidate_pair_serial); debug_trace::Write(sm); }
        } else if(have_pair && candidate_pair_serial!=0 && !candidate_released && safe_to_drop) {
            // Failure happened before any GPU submission used the certified pair.
            // Drop this pair so one reset/descriptor/list failure cannot freeze live
            // publication forever. The normal mono bridge below owns this XR frame;
            // the next game frame may publish a fresh L/R pair.
            bridge_last_consumed_pair_serial_=candidate_pair_serial;
            DeviceHook::MarkBridgePairConsumed(candidate_pair_serial);
            static std::atomic<unsigned long long> dropped_pairs{0};
            const auto dn=dropped_pairs.fetch_add(1,std::memory_order_relaxed)+1;
            if(dn<=24 || (dn%120ull)==0){ char dm[320]{}; std::snprintf(dm,sizeof(dm),
                "V66.22-XR-STABLE-PAIR DROP n=%llu serial=%llu reason=PRE-SUBMIT-FAIL fallback=MONO nextPair=ALLOWED",
                (unsigned long long)dn,(unsigned long long)candidate_pair_serial); debug_trace::Write(dm); }
        }
        if(left_source) left_source->Release();
        if(right_source) right_source->Release();
        // V66.34: the stable-cache copy (if any) has been submitted on the game's DIRECT
        // queue by now, so the producer may reuse this capture slot.
        DeviceHook::ReleaseBridgePairClaim();

        // V65.50: mode 2 accepts ONLY a same-serial pair that was copied inside
        // the game's real RT->PRESENT command lists and certified by ExecuteCommandLists.
        // Never fall back to the old independent LEFT/RIGHT caches: those could contain
        // different attempts and were the likely source of partial/blue presentation.
        static std::atomic<unsigned long long> bridge_submit{0}; const auto n=bridge_submit.fetch_add(1,std::memory_order_relaxed)+1;
        if(n<=24 || (n%300)==0){ char m[448]{}; std::snprintf(m,sizeof(m),
            "V66.27-XR-LIVE-BRIDGE call=%llu mode=%d f2=%d mapping=%s havePair=%d fresh=%d serial=%llu poseFrame=%llu directPairBlit=%d completedPairs=%llu fallback=%s",
            static_cast<unsigned long long>(n),stereo_mode,f2_stability?1:0,stereo_mode==6?"SWAPPED-RL":(stereo_mode==7?"LEFT-BOTH":"NORMAL-LR"),
            have_pair?1:0,fresh_pair?1:0,static_cast<unsigned long long>(pair_serial),
            static_cast<unsigned long long>(f2_stability?bridge_present_cache_camera_frame_:pair_camera_frame),
            pair_ok?1:0,static_cast<unsigned long long>(DeviceHook::BridgeCompletePairCount()),
            pair_ok?(fresh_pair?"LIVE-PAIR":"STABLE-CACHE"):(f2_stability?"MONO-LAST-RESORT":"MONO")); debug_trace::Write(m);}
        if(pair_ok) return true;
    }
    // A native-pair pickup may now run before desktop PRESENT. If its pair/cache
    // copy failed, never fall through to an unsubmitted desktop resource.
    if(pacing59c6::Paired(stereo_mode)&&!DeviceHook::CurrentExecuteSubmittedPresent())return false;
    std::uint64_t present_tick_for_source = 0;
    std::uint64_t camera_frame_for_source = 0;
    const bool got = stereo_mode == 1
        ? DeviceHook::TryGetLatestPresentResourceTagged(&source, &source_eye, &serial, &camera_frame_for_source)
        : DeviceHook::TryGetLatestPresentResourceIdentity(&source, &serial, &present_tick_for_source, &camera_frame_for_source);
    if (got) {
        const bool ok = stereo_mode == 1
            ? BlitAfrStereoToEyes(source, source_eye, serial, left, right)
            : BlitGameImageToEyes(source, left, right);
        // V47.0: correlate EVERY XR copy with the exact KCD2 PRESENT and the
        // camera/XR frame that rendered it. Logging remains throttled, ownership does not.
        std::uint64_t ps=serial, pt=present_tick_for_source, pw=0, pcf=camera_frame_for_source; std::uint32_t ph=0;
        if (stereo_mode == 1) {
            // V65.33: pcf came from the same lock-protected tagged PRESENT lookup as source_eye.
            // Query only display metadata here; do not overwrite the eye-specific camera identity with a newer PRESENT.
            std::uint64_t meta_serial=0, meta_cf=0; DeviceHook::GetLatestPresentIdentity(&meta_serial,&pt,&pw,&ph,&meta_cf);
            const int tagged_eye = source_eye & 1;
            last_submitted_camera_frame_by_eye_[tagged_eye].store(pcf, std::memory_order_release);
        } else { const auto sd = source->GetDesc(); pw = sd.Width; ph = sd.Height; }
        if(ok)last_submitted_desktop_serial59c6_.store(ps,std::memory_order_release);
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

bool XrManager::WaitForNextRenderFrameToken(std::uint64_t& inout_frame, std::uint32_t timeout_ms, bool force, bool fresh_after_arrival_d67)
{
    if (!force && xr_render_gate_mode_.load(std::memory_order_acquire) == 0) return true;
    std::unique_lock<std::mutex> lock(frame_gate_mutex_);
    const auto floorD67=publication_wait_d67::floor(inout_frame,frame_gate_published_frame_.load(std::memory_order_acquire),fresh_after_arrival_d67);
    const auto pred=[this,floorD67,force]{
        return (!force && xr_render_gate_mode_.load(std::memory_order_acquire)==0) ||
               publication_wait_d67::ready(frame_gate_published_frame_.load(std::memory_order_acquire),floorD67);
    };
    if (!frame_gate_cv_.wait_for(lock,std::chrono::milliseconds(timeout_ms),pred)) return false;
    if (!force && xr_render_gate_mode_.load(std::memory_order_acquire)==0) return true;
    inout_frame=frame_gate_published_frame_.load(std::memory_order_acquire);
    return inout_frame!=0;
}

// H49 passive pacing receipts. XrTime is retained separately from steady CPU time.
// stage 9 wait return, 10 source pickup, 11 copy routine return, 12 EndFrame return,
// 13 handoff enqueue. generation joins one request; sample joins its XR frame.
static void PacingReceiptH49(unsigned stage,const char* reason,std::uint64_t request,
    std::uint64_t xrFrame,std::uint64_t source,std::uint64_t camera,
    std::int64_t display,int code,unsigned flags) noexcept {
    namespace mt=kcd2vr::diagnostics::motion_d68;
    mt::Event motion{};const auto traceStage=stage==12?mt::Submit:mt::RenderRequest;
    if(mt::begin(motion,traceStage)&&mt::inWindow(motion)){
        static thread_local mt::Cadence cadence[5];
        if(stage>=9&&stage<=13&&cadence[stage-9].due(motion.session,motion.ns)){
            motion.request=request;motion.frame=xrFrame;motion.sample=source;motion.publication=camera;
            motion.targetXrNs=display;motion.flags=flags;motion.thread=GetCurrentThreadId();
            motion.v[0]=float(stage);motion.v[1]=float(code);motion.count=2;mt::push(motion);
        }else mt::noteCadenceSkip(traceStage);
    }
    namespace oc=kcd2vr::diagnostics::ownership48;
    const auto now=std::uint64_t(GetTickCount64());if(!oc::active(now))return;
    oc::Event e{};e.stage=stage;e.tick=now;e.thread=GetCurrentThreadId();
    e.generation=request;e.sample=xrFrame;e.item=source;e.publication=camera;
    e.poseTime=display;e.code=code;e.flags=flags;oc::name(e,reason);oc::push(e);
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
    std::uint64_t receiptRequestH49=0,receiptCameraH49=0;
    bool fresh_present_pickup = false;
    std::uint64_t pickup_present_serial = 0;
    bool pickup_paired_r18 = false; // diagnostic source kind; never used by presentation ownership
    std::uint64_t pickup_wait_ms = 0;
    {
        std::unique_lock<std::mutex> lock(render_mutex_);
        if (!render_request_pending_ || render_request_in_progress_)
            return;

        // Cancel an obsolete scene request before touching its swapchain images.
        // Normal completion/cancellation ownership is unchanged; no timer, extra
        // GPU copy or producer teardown is introduced.
        if(!production_presentation59c2_.Current(render_request_presentation59c2_.ticket)) {
            render_request_success_=false;render_request_ui_copy_success_=false;
            render_request_pending_=false;render_request_done_=true;
            render_cv_.notify_all();return;
        }

        // R59.16C6: use this request's source/layer snapshot, not a newly read
        // global mode. Pair tokens are not desktop PRESENT sequence numbers.
        const bool bridge_active=active_vr_bridge_.load(std::memory_order_acquire)&&
                                 !diagnostic_rendering_.load(std::memory_order_acquire);
        if(bridge_active){
            pacing59c6::Input p{};
            p.paired=pacing59c6::Paired(render_request_presentation59c2_.policy.stereo);
            if(p.paired){
                p.known=DeviceHook::ReadBridgePairIdentity59c6(p.serial,p.camera);
                p.have=p.serial!=0;
                p.last=DeviceHook::V6634LastConsumedPairSerial();
                // Published L/R copies are already submitted on this game queue.
                p.submitted=true;
            }else{
                std::uint64_t tick=0,width=0;std::uint32_t height=0;
                p.known=true;
                p.have=DeviceHook::GetLatestPresentIdentity(&p.serial,&tick,&width,&height,&p.camera);
                p.submitted=DeviceHook::CurrentExecuteSubmittedPresent();
                p.last=last_submitted_desktop_serial59c6_.load(std::memory_order_acquire);
            }
            p.nowNs=pacing59c6::NowNs();p.enqueuedNs=render_request_enqueue_ns59c6_;
            p.hz=estimated_refresh_hz_.load(std::memory_order_acquire);
            p.mode=present_pacing_mode_.load(std::memory_order_acquire);
            p.gate=xr_render_gate_mode_.load(std::memory_order_acquire);
            p.expected=expected_present_camera_frame_.load(std::memory_order_acquire);
            const auto decision=pacing59c6::Decide(p);pacing59c6::Observe(p.paired,decision);
            if(!decision.take())return;
            receiptRequestH49=p.enqueuedNs;receiptCameraH49=p.camera;
            PacingReceiptH49(10,"SOURCE_PICKUP",p.enqueuedNs,0,p.serial,p.camera,0,int(decision.result),
                (p.paired?1u:0u)|(decision.fresh?2u:0u)|(p.submitted?4u:0u));
            fresh_present_pickup=decision.fresh;
            pickup_wait_ms=p.enqueuedNs&&p.nowNs>=p.enqueuedNs?(p.nowNs-p.enqueuedNs)/1000000ull:0;
            pickup_present_serial=p.have?p.serial:0;
            pickup_paired_r18=p.paired;
        }

        render_request_in_progress_ = true;
        render_presentation59c2_=render_request_presentation59c2_;
        left = render_request_images_[0];
        right = render_request_images_[1];
        ui_image = render_request_ui_image_;
        render_cv_.notify_all();
    }

    bool ok = active_vr_bridge_.load(std::memory_order_acquire) && !diagnostic_rendering_.load(std::memory_order_acquire)
        ? SubmitBridgeImagesOnRenderThread(left, right)
        : SubmitDiagnosticImagesOnRenderThread(left, right);
    PacingReceiptH49(11,"COPY_ROUTINE_RETURN_NOT_SCANOUT",receiptRequestH49,0,pickup_present_serial,receiptCameraH49,0,ok?1:0,0);
    bool ui_copy_ok = false;
    if (ok && ui_image && render_request_copy_ui_) {
        ID3D12Resource* menu = menu_quad_source_.load(std::memory_order_acquire);
        ui_copy_ok = menu && CopyMenuTextureToUiImage(menu, ui_image);
        if (!ui_copy_ok) debug_trace::Write("V63.0-F6/F7 UI texture copy failed; gameplay layer preserved");
    }

    if (active_vr_bridge_.load(std::memory_order_acquire) && !diagnostic_rendering_.load(std::memory_order_acquire)) {
        const auto count = fresh_present_pickup
            ? present_pacing_fresh_count_.fetch_add(1, std::memory_order_relaxed) + 1
            : present_pacing_reuse_count_.fetch_add(1, std::memory_order_relaxed) + 1;
        present_pacing_last_wait_ms_.store(pickup_wait_ms,std::memory_order_release);
        diagnostics::xr165::PickupObserved(pickup_present_serial,DeviceHook::BridgeCompletePairCount(),pickup_wait_ms,pickup_paired_r18); // W3 F2 r1 cadence, Bug recorder only
        present_pacing_last_serial_.store(pickup_present_serial,std::memory_order_release);
        if (count <= 8 || (count % 300ull) == 0) {
            char m[320]{};
            std::snprintf(m,sizeof(m),
                "R59.16C6 PRESENT_PACING mode=%d pickup=%s waitMs=%llu serial=%llu lastSourceToken=%llu blitOk=%d source=%s clock=STEADY_NS",
                present_pacing_mode_.load(std::memory_order_acquire),
                fresh_present_pickup ? "FRESH" : "REUSE",
                static_cast<unsigned long long>(pickup_wait_ms),
                static_cast<unsigned long long>(pickup_present_serial),
                static_cast<unsigned long long>(last_submitted_present_serial_.load(std::memory_order_acquire)),
                ok ? 1 : 0,pacing59c6::Paired(render_presentation59c2_.policy.stereo)?"CERTIFIED_PAIR":"DESKTOP_PRESENT");
            debug_trace::Write(m);
        }
    }

    {
        std::lock_guard<std::mutex> lock(render_mutex_);
        render_request_success_ = ok;
        render_request_ui_copy_success_ = ui_copy_ok;
        render_request_done_ = true;
        render_request_pending_ = false;
        render_request_in_progress_ = false;
    }
    render_cv_.notify_all();
}


bool XrManager::QueueRenderThreadDiagnosticWork(ID3D12Resource* left, ID3D12Resource* right, ID3D12Resource* ui_image,
                                                   bool copy_ui, bool* ui_copy_success, const FramePresentation59c2& presentation)
{
    diagnostics::xr165::Timer timing165(diagnostics::xr165::Handoff);
    if (!left || !right)
        return false;

    RenderFrameState receiptFrameH49{};
    if(diagnostics::ownership48::active(GetTickCount64()))CopyActiveRenderFrameState(receiptFrameH49);
    std::unique_lock<std::mutex> lock(render_mutex_);
    render_request_images_[0] = left;
    render_request_images_[1] = right;
    render_request_ui_image_ = ui_image;
    render_request_copy_ui_ = copy_ui;
    render_request_presentation59c2_ = presentation;
    render_request_ui_copy_success_ = false;
    render_request_pending_ = true;
    render_request_enqueue_ns59c6_ = pacing59c6::NowNs();
    if(diagnostics::ownership48::active(GetTickCount64())||diagnostics::motion_d68::requested()){
        PacingReceiptH49(13,"HANDOFF_ENQUEUE",render_request_enqueue_ns59c6_,receiptFrameH49.frame_index,0,
            expected_present_camera_frame_.load(std::memory_order_acquire),receiptFrameH49.predicted_display_time,0,0);
    }
    render_request_in_progress_ = false;
    render_request_done_ = false;
    render_request_success_ = false;

    // First wait for the game render thread to pick the request up. If no
    // DIRECT-queue submission occurs in this window, cancel before releasing
    // the XR swapchain images. Once work has started, wait for completion so
    // an image is never released while our GPU commands still reference it.
    bool picked_up=false;
    {diagnostics::xr165::Timer timing(diagnostics::xr165::HandoffPickup);
    picked_up = render_cv_.wait_for(
        lock, std::chrono::milliseconds(500),
        [this] { return render_request_in_progress_ || render_request_done_; });}

    if (!picked_up)
    {
        render_request_pending_ = false;
        render_request_enqueue_ns59c6_ = 0;
        render_request_images_[0] = nullptr;
        render_request_images_[1] = nullptr;
        render_request_ui_image_ = nullptr;
        render_request_copy_ui_ = false;
        if (ui_copy_success) *ui_copy_success = false;
        debug_trace::Write("render-thread handoff timed out before pickup");
        return false;
    }

    {diagnostics::xr165::Timer timing(diagnostics::xr165::HandoffComplete);
    render_cv_.wait(lock, [this] { return render_request_done_; });}
    const bool ok = render_request_success_;
    if (ui_copy_success) *ui_copy_success = render_request_ui_copy_success_;
    render_request_enqueue_ns59c6_ = 0;
    render_request_images_[0] = nullptr;
    render_request_images_[1] = nullptr;
    render_request_ui_image_ = nullptr;
    render_request_copy_ui_ = false;

    // V64.45: XR-thread aggregate telemetry. This path executes once per completed
    // XR copy request, so it remains visible even if render-thread event logging
    // is sparse on a particular runtime.
    static std::uint64_t pacing_diag_last_ms=0;
    const std::uint64_t pacing_now=static_cast<std::uint64_t>(GetTickCount64());
    if(pacing_now-pacing_diag_last_ms>=1000ull){
        pacing_diag_last_ms=pacing_now;
        char pm[320]{};
        std::snprintf(pm,sizeof(pm),
            "V64.45-PACING-SUMMARY mode=%d fresh=%llu reuse=%llu lastWaitMs=%llu lastSerial=%llu refresh=%.2fHz ok=%d",
            present_pacing_mode_.load(std::memory_order_acquire),
            static_cast<unsigned long long>(present_pacing_fresh_count_.load(std::memory_order_acquire)),
            static_cast<unsigned long long>(present_pacing_reuse_count_.load(std::memory_order_acquire)),
            static_cast<unsigned long long>(present_pacing_last_wait_ms_.load(std::memory_order_acquire)),
            static_cast<unsigned long long>(present_pacing_last_serial_.load(std::memory_order_acquire)),
            double(estimated_refresh_hz_.load(std::memory_order_acquire)), ok?1:0);
        debug_trace::Write(pm);
    }
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
        KCD2VR_DIAG_LOG(Bug,error,"CreateDescriptorHeap(RTV) failed, hr=0x{:08X}", static_cast<unsigned>(hr));
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

static std::array<ui_surface_geometry::Eye,2> UiEyes(const XrManager::RenderFrameState& frame) noexcept {
    std::array<ui_surface_geometry::Eye,2> result{};
    const auto inv=QConj(frame.head_pose.orientation);
    for(int i=0;i<2;++i){
        const auto& p=frame.eye_pose[i];
        const auto position=QRotate(inv,{p.position.x-frame.head_pose.position.x,p.position.y-frame.head_pose.position.y,p.position.z-frame.head_pose.position.z});
        const auto rotation=QMul(inv,p.orientation);const auto& f=frame.eye_fov[i];
        result[i]={{position.x,position.y,position.z},{rotation.x,rotation.y,rotation.z,rotation.w},{f.angleLeft,f.angleRight,f.angleUp,f.angleDown}};
    }
    return result;
}

// XR-worker-only lifecycle work. Continue polling while stopped so READY can
// resume the existing session and its already attached action set.
void XrManager::ResetUiAnchorsR105() noexcept {
    {std::lock_guard<std::mutex> lock(flat_ui_mutex_);flat_ui_anchor_valid_=false;ui_anchor_logged_r148_=false;ui_away_since_r148_=0;}
    ui_layer_test_anchor_valid_=false;hud_lag_valid_=false;
    head_receipt_pending_r105_=true;
}
void XrManager::PollSessionEventsR105() {
    if(instance_==XR_NULL_HANDLE||session_==XR_NULL_HANDLE)return;
    XrEventDataBuffer event{XR_TYPE_EVENT_DATA_BUFFER};
    while(xrPollEvent(instance_,&event)==XR_SUCCESS){
        if(event.type==XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED){
            const auto* state=reinterpret_cast<const XrEventDataSessionStateChanged*>(&event);
            if(state->session==session_){
                if(state->state==XR_SESSION_STATE_STOPPING){
                    const bool wasRunning=session_running_.exchange(false,std::memory_order_acq_rel);
                    input_.OnSessionStoppedR105();ResetUiAnchorsR105();
                    if(wasRunning){
                        const auto result=xrEndSession(session_);
                        char m[160]{};std::snprintf(m,sizeof(m),"R105 XR_SESSION STOPPING endResult=%d eventPolling=ACTIVE actions=NEUTRAL",int(result));diagnostic_trace::WriteStatus(m);
                    }
                }else if(state->state==XR_SESSION_STATE_READY&&!session_running_.load(std::memory_order_acquire)){
                    XrSessionBeginInfo begin{XR_TYPE_SESSION_BEGIN_INFO};
                    begin.primaryViewConfigurationType=XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
                    const auto result=xrBeginSession(session_,&begin);
                    if(XR_SUCCEEDED(result)){
                        ResetUiAnchorsR105();CameraHook::RequestRecenter();
                        previous_predicted_display_time_=0;comfort_envelope51_.Reset();
                        session_running_.store(true,std::memory_order_release);
                    }
                    char m[160]{};std::snprintf(m,sizeof(m),"R105 XR_SESSION READY beginResult=%d actionSet=RETAINED headTiming=UNCHANGED",int(result));diagnostic_trace::WriteStatus(m);
                }else if(state->state==XR_SESSION_STATE_EXITING||state->state==XR_SESSION_STATE_LOSS_PENDING){
                    session_running_.store(false,std::memory_order_release);input_.OnSessionStoppedR105();ResetUiAnchorsR105();
                    diagnostic_trace::WriteStatus("R105 XR_SESSION exiting/lost; input cleared");
                }
            }
        }else if(event.type==XR_TYPE_EVENT_DATA_REFERENCE_SPACE_CHANGE_PENDING){
            const auto* change=reinterpret_cast<const XrEventDataReferenceSpaceChangePending*>(&event);
            if(change->session==session_&&change->referenceSpaceType==XR_REFERENCE_SPACE_TYPE_LOCAL)
                pending_reference_changes125_.push_back(change->changeTime);
        }else if(event.type==XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING){
            session_running_.store(false,std::memory_order_release);input_.OnSessionStoppedR105();ResetUiAnchorsR105();
            diagnostic_trace::WriteStatus("R105 XR_SESSION instance loss pending; input cleared");
        }
        event.type=XR_TYPE_EVENT_DATA_BUFFER;
    }
}

bool XrManager::RunFrame(XrPose& out_head_pose, float head_pose_scale)
{
    diagnostics::xr165::Timer timing165(diagnostics::xr165::Frame);
    PollSessionEventsR105();
    if (!session_running_.load(std::memory_order_relaxed)) { kcd2vr::start_facing_r148::headTracked.store(false,std::memory_order_release);input_.InvalidateFrameR105();comfort_envelope51_.Reset();Sleep(8);return false; } // R106: no xrWaitFrame pacing while stopped - don't spin the worker

    // V58 live XR-resolution apply. Execute only on the XR worker before
    // xrWaitFrame/acquire, when no image from the new frame is owned. Finish
    // our last D3D12 submission, destroy/recreate only XR swapchains, and keep
    // the KCD2 desktop/backbuffer resolution untouched. If the requested size
    // fails, rebuild the previous scale as a fail-safe.
    const float requested_scale=requested_render_scale_.exchange(0.0f,std::memory_order_acq_rel);
    if(requested_scale>=0.50f) {
        const float previous_scale=active_swapchain_scale_.load(std::memory_order_acquire);
        if(fence_ && fence_->GetCompletedValue()<fence_value_ && fence_event_ && SUCCEEDED(fence_->SetEventOnCompletion(fence_value_,fence_event_)))
            TimedFenceWait165(fence_event_,INFINITE);
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
        if(!rebuilt) { debug_trace::Write("V58-RESOLUTION fatal: swapchain rebuild and rollback both failed; stopping XR session safely"); session_running_.store(false,std::memory_order_release); input_.InvalidateFrameR105();return false; }
        int rw=0,rh=0; GetEyeSize(rw,rh);
        char m[256]{}; std::snprintf(m,sizeof(m),"V58-RESOLUTION rebuild complete activeScale=%.3f eye=%dx%d desktop=UNCHANGED",active_swapchain_scale_.load(std::memory_order_relaxed),rw,rh); debug_trace::Write(m);
    }

    const auto frame_no = g_xr_frame_count.fetch_add(1, std::memory_order_relaxed) + 1;
    test_xr_frames_.fetch_add(1, std::memory_order_relaxed);
    if (frame_no == 1 || (frame_no % 300) == 0)
        debug_trace::WriteCounter("xr_frames", frame_no);

    XrFrameState frame_state{ XR_TYPE_FRAME_STATE };
    XrFrameWaitInfo wait_info{ XR_TYPE_FRAME_WAIT_INFO };
    { diagnostics::xr165::Timer timing165(diagnostics::xr165::WaitFrame);
      if (XR_FAILED(xrWaitFrame(session_, &wait_info, &frame_state))) { input_.InvalidateFrameR105();comfort_envelope51_.Reset();debug_trace::Write("xrWaitFrame failed"); return false; } }
    diagnostics::xr165::WaitReceipt(frame_state.predictedDisplayTime,frame_state.predictedDisplayPeriod);
    PacingReceiptH49(9,"XR_WAIT_RETURN",0,0,0,0,frame_state.predictedDisplayTime,0,frame_state.shouldRender?1u:0u);
    DeviceHook::NotifyV6636XrFrameOpportunity();
    const auto due=std::remove_if(pending_reference_changes125_.begin(),pending_reference_changes125_.end(),
        [&](XrTime when){return when<=frame_state.predictedDisplayTime;});
    if(due!=pending_reference_changes125_.end()){
        pending_reference_changes125_.erase(due,pending_reference_changes125_.end());
        CameraHook::RequestRecenter();
        ResetUiAnchorsR105();
        {std::lock_guard<std::mutex> lock(flat_ui_mutex_);ui_reference_change_r148_=true;} // R148: the next screen anchor is yours (runtime recenter)
        diagnostic_trace::WriteStatus("R105 XR_REFERENCE runtimeLocalChange=APPLIED_AT_DISPLAY_TIME uiAnchors=RECAPTURE bodyReanchor=ON_VALID_POSE");
    }

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
    { // HJ1: the moment this predicted time became the camera's mode-1 target, and the headset period.
        LARGE_INTEGER qpc{};XrTime now=0;
        if((kcd2vr::hj1::enabled.load(std::memory_order_relaxed)||motion_trials_d67::UncappedSelectedAt(0))&&qpc_time_extension_enabled_&&qpc_to_xr_time_&&QueryPerformanceCounter(&qpc)&&XR_SUCCEEDED(qpc_to_xr_time_(instance_,&qpc,&now)))
            hj1_predicted_now_.store(static_cast<std::int64_t>(now),std::memory_order_release);
        if(motion_trials_d67::UncappedSelectedAt(0)&&now>0&&frame_state.predictedDisplayPeriod>0)
            (void)fresh_hands_r94::anchors.publish({frame_state.predictedDisplayTime,now,
                frame_state.predictedDisplayPeriod,tracking59c6::epoch.load(std::memory_order_acquire)});
        if(frame_state.predictedDisplayPeriod>0)hj1_period_ns_.store(static_cast<std::int64_t>(frame_state.predictedDisplayPeriod),std::memory_order_release);
    }

    XrFrameBeginInfo begin_info{ XR_TYPE_FRAME_BEGIN_INFO };
    {diagnostics::xr165::Timer timing(diagnostics::xr165::BeginFrame);
    if (XR_FAILED(xrBeginFrame(session_, &begin_info))) { input_.InvalidateFrameR105();comfort_envelope51_.Reset();debug_trace::Write("xrBeginFrame failed"); return false; }}

    // D61 optional free-hand DISPLAY sampling. No change to the shared family.
    const auto handExtraD69=claude_motion_d69::ExtraNs();
    const bool handPresentD61=hand_present_d61::Enabled()&&handExtraD69==0;
    const auto handPolicyD61=handPresentD61?hand_present_d61::policy.load(std::memory_order_acquire):0;
    if(handPresentD61)hand_present_lead_d61_.sync(tracking59c6::epoch.load(std::memory_order_acquire),handPolicyD61,frame_state.predictedDisplayPeriod);
    else if(hand_present_was_enabled_d61_)hand_present_lead_d61_.reset();
    hand_present_was_enabled_d61_=handPresentD61;
    const auto familyPolicyR24=hj1::enabled.load(std::memory_order_acquire)?hand_smooth_h50::policyEpoch.load(std::memory_order_acquire):0;
    hj1::Sample cameraFamilyR24{};
    const tracking59c6::Stamp source59c6{std::uint64_t(GetTickCount64()),tracking59c6::NowNs(),tracking59c6::epoch.load(),frame_state.predictedDisplayTime};
    bool got_pose = false;
    XrSpaceLocationFlags headFlags59c6=0;
    XrPosef located_head_pose{};
    // Crysis VR uses the predicted display time to obtain tracking for the
    // current XR frame. Keep HMD tracking independent from rendering/camera
    // hooks so the camera can consume the latest pose on the game thread.
    {
        XrSpaceLocation location{ XR_TYPE_SPACE_LOCATION };
        diagnostics::xr165::Timer timing165(diagnostics::xr165::PredictedPose);
        std::lock_guard<std::mutex> pose_lock(pose_locate_mutex_);
        if (XR_SUCCEEDED(xrLocateSpace(view_space_, local_space_, frame_state.predictedDisplayTime, &location)) &&
            (location.locationFlags & XR_SPACE_LOCATION_POSITION_VALID_BIT) &&
            (location.locationFlags & XR_SPACE_LOCATION_ORIENTATION_VALID_BIT) &&
            HeadUprightR108(location.pose.orientation))
        {
            out_head_pose.pos_x = location.pose.position.x;
            out_head_pose.pos_y = location.pose.position.y;
            out_head_pose.pos_z = location.pose.position.z;
            out_head_pose.qx = location.pose.orientation.x;
            out_head_pose.qy = location.pose.orientation.y;
            out_head_pose.qz = location.pose.orientation.z;
            out_head_pose.qw = location.pose.orientation.w;
            located_head_pose = location.pose;
            headFlags59c6=location.locationFlags;
            got_pose = true;
            if(head_receipt_pending_r105_){
                head_receipt_pending_r105_=false;
                const auto& q=location.pose.orientation;const auto& p=location.pose.position;
                char m[340]{};std::snprintf(m,sizeof(m),"R105 XR_HEAD firstAfterStartOrReferenceChange=1 flags=0x%llx position=(%.4f,%.4f,%.4f) quaternion=(%.5f,%.5f,%.5f,%.5f) upY=%.5f poseCorrection=NONE",
                    (unsigned long long)location.locationFlags,double(p.x),double(p.y),double(p.z),double(q.x),double(q.y),double(q.z),double(q.w),1.0-2.0*(double(q.x)*q.x+double(q.z)*q.z));
                diagnostic_trace::WriteStatus(m);
            }
            const auto pose_no = g_valid_pose_count.fetch_add(1, std::memory_order_relaxed) + 1;
            if (pose_no == 1)
                debug_trace::Write("first valid HMD pose acquired");
            if ((pose_no % 300) == 0)
                debug_trace::WriteCounter("valid_hmd_poses", pose_no);
        }
    }
    // R148: the start/load facing recenter (start_facing_r148.h) only trusts a head the runtime really tracks
    kcd2vr::start_facing_r148::headTracked.store(got_pose&&(headFlags59c6&XR_SPACE_LOCATION_ORIENTATION_TRACKED_BIT)!=0,std::memory_order_release);

    bool inputFresh51=false;
    { diagnostics::xr165::Timer timing165(diagnostics::xr165::Input);inputFresh51=input_.Sync(frame_state.predictedDisplayTime); }
    (void)input_stamp59c6_.publish(source59c6);
    // Publish the already located predicted family before the camera and GPU
    // waits. The later button/permission packet keeps its original input age.
    if(tracking59c6::enabled.load(std::memory_order_acquire)&&!tracking59c6::cameraNow.load(std::memory_order_acquire)){
        std::lock_guard<std::mutex> pose_lock(pose_locate_mutex_);
        tracking59c6::Frame family{};family.epoch=source59c6.epoch;family.tick=source59c6.tick;
        family.ns=source59c6.ns;family.locateTime=source59c6.locateTime;family.controllersSampled=true;
        const XrSpaceLocationFlags valid=XR_SPACE_LOCATION_POSITION_VALID_BIT|XR_SPACE_LOCATION_ORIENTATION_VALID_BIT;
        const XrSpaceLocationFlags tracked=XR_SPACE_LOCATION_POSITION_TRACKED_BIT|XR_SPACE_LOCATION_ORIENTATION_TRACKED_BIT;
        auto fill=[&](unsigned i,const XrPose& p,XrSpaceLocationFlags flags){
            family.poses[i]={p.pos_x,p.pos_y,p.pos_z,p.qx,p.qy,p.qz,p.qw};
            family.valid[i]=(flags&valid)==valid;family.tracked[i]=(flags&tracked)==tracked;
        };
        fill(0,out_head_pose,headFlags59c6);
        fill(1,input_.Left().pose,inputFresh51?input_.Left().pose_flags:0);
        fill(2,input_.Right().pose,inputFresh51?input_.Right().pose_flags:0);
        if(handExtraD69>0){const XrSpaceVelocity velocity[2]{input_.Left().velocityD69,input_.Right().velocityD69};ApplyExtraHandsD69(input_,family,velocity,handExtraD69);}
        // D62: locate optional display data before publishing the raw family.
        // The final tracking prepare assigns its exact sequence/held flags,
        // seals the optional receipt, then exposes raw tracking atomically.
        // A missing/failed visual receipt is a sealed baseline choice, not a
        // reason to delay a consumer or later replace the same raw family.
        tracking59c6::Frame completedR24{};
        bool familyPublishedD61=false;
        if(handPresentD61){
        hand_present_d61::Packet displayD62{};bool locatedDisplayD62=false;
        const auto leadD61=hand_present_lead_d61_.leadNs();
        if(inputFresh51&&leadD61>0&&
           hand_present_d61::WantsSample(source59c6.epoch,source59c6.tick)&&
           tracking59c6::cameraSource.load(std::memory_order_acquire)==0&&
           family.valid[0]&&family.tracked[0]&&
           source59c6.epoch==tracking59c6::epoch.load(std::memory_order_acquire)&&
           hand_present_d61::CurrentPolicy(handPolicyD61)&&
           family.locateTime<=std::numeric_limits<XrTime>::max()-leadD61){
            XrPosef gripsD61[2]{};XrSpaceLocationFlags flagsD61[2]{};
            const auto targetD61=family.locateTime+leadD61;
            if(input_.TryLocateGripPair(targetD61,gripsD61,flagsD61)){
                displayD62.policyEpoch=handPolicyD61;
                displayD62.headTime=displayD62.baseHandTime=family.locateTime;
                displayD62.presentHandTime=targetD61;
                for(unsigned i=0;i<2;++i){const auto& g=gripsD61[i];
                    displayD62.tracked[i]=(flagsD61[i]&(valid|tracked))==(valid|tracked);
                    displayD62.hand[i]={{g.position.x,g.position.y,g.position.z},{g.orientation.x,g.orientation.y,g.orientation.z,g.orientation.w}};
                }
                locatedDisplayD62=true;
            }
        }
        familyPublishedD61=tracking59c6::PublishPrepared(family,&completedR24,
            [&](const tracking59c6::Frame& sealed) noexcept {
                if(!locatedDisplayD62||!sealed.tracked[0]||sealed.held[0]||
                   source59c6.epoch!=tracking59c6::epoch.load(std::memory_order_acquire))return;
                displayD62.sourceEpoch=sealed.epoch;displayD62.sequence=sealed.sequence;
                displayD62.tick=sealed.tick;displayD62.ns=sealed.ns;
                for(unsigned i=0;i<2;++i)displayD62.tracked[i]=displayD62.tracked[i]&&sealed.tracked[i+1]&&!sealed.held[i+1];
                (void)hand_present_d61::Publish(displayD62);
            });
        }else{
            // OFF takes the ordinary publication path without an optional
            // packet, callback capture, extra pose copies or runtime locates.
            familyPublishedD61=tracking59c6::Publish(family,familyPolicyR24?&completedR24:nullptr);
        }
        if(familyPublishedD61&&familyPolicyR24&&hj1::enabled.load(std::memory_order_acquire)&&
           familyPolicyR24==hand_smooth_h50::policyEpoch.load(std::memory_order_acquire))
            cameraFamilyR24=tracking59c6::CompletedCameraFamilyR24(completedR24,familyPolicyR24);
    }
    // NOW mode retains its existing acquisition and camera time domain.
    XrPose currentFamily59c6{};if(tracking59c6::enabled.load(std::memory_order_acquire)&&tracking59c6::cameraNow.load(std::memory_order_acquire))(void)SampleHeadPoseNow(currentFamily59c6);

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
            // V65.03c: controller laser and stick navigation coexist. Opening the
            // VR menu with Right Thumbrest + LS Down must never disable the right AIM ray.
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

    // Capture once before hit-testing. The ray, queued pixel copy and displayed
    // panel must share one production ticket and one anchor, including edges.
    auto presentation59c2=CapturePresentation59c2();

    // V63.0: same Touch AIM ray projected onto the real F6 LOCAL theater quad.
    // This is intentionally independent from the book pointer and is normalized
    // to the captured KCD2 source so Win32 mouse injection can drive inventory,
    // shops, cooking, map and other flat UI without guessing a 16:9 surface.
    // Compute once; the pointer and displayed flat panel share these exact dimensions.
    const float ui88_distance=std::clamp(flat_ui_distance_.load(std::memory_order_acquire),.80f,3.20f);
    const float ui88_aspect=std::clamp(float(std::max(1u,last_source_width_.load()))/float(std::max(1u,last_source_height_.load())),.20f,6.f);
    float ui88_width=std::clamp(1.82f*(1.58f/std::clamp(flat_ui_scale_.load(),.60f,3.f)),1.05f,2.65f);
    if(full_menu_fit_.load(std::memory_order_acquire)){
        RenderFrameState measured{};
        if(CopyActiveRenderFrameState(measured)&&measured.head_pose_valid&&measured.stereo_views_valid){
            const auto fit=ui_surface_geometry::Fit(UiEyes(measured),ui88_aspect,ui88_distance,ui88_width);
            if(fit.valid)ui88_width=fit.width;
        }
        // V66.97 passive full-frame zoom: loading/cutscene screens may be enlarged
        // independently from Inventory/Shop. 1.0 for interactive menus; default 1.20
        // only when the main loop explicitly marks a passive fullscreen scene.
        ui88_width=std::clamp(ui88_width*passive_full_frame_scale_.load(std::memory_order_acquire),1.05f,3.60f); // R107: slider up to 2x
    }
    // R107 cutscene bars: the screen shows only the central 16:9 band of the (near square) source frame.
    const float cropAspectR107=flat_crop_aspect_r107_.load(std::memory_order_acquire);
    const bool cropR107=cropAspectR107>ui88_aspect+0.01f&&cropAspectR107<6.f;
    const float ui88_height=ui88_width/(cropR107?cropAspectR107:ui88_aspect);
    const int ui88_pointer_hand=native_menu_pointer_hand_.load(std::memory_order_acquire);
    flat_pointer_hand_.store(ui88_pointer_hand,std::memory_order_release);
    bool flat_laser_valid=false, flat_cursor_world_valid=false;
    V3f flat_laser_start{}, flat_laser_end{}, flat_cursor_world{};
    XrQuaternionf flat_pointer_plane_q{0.0f,0.0f,0.0f,1.0f};
    flat_pointer_active_.store(false,std::memory_order_release);
    if (presentation59c2.policy.flat && ui88_pointer_hand>=0 && ui88_pointer_hand<=1 && !presentation59c2.policy.physical_map) {
        const auto anchor=presentation59c2.anchor;const bool anchor_valid=presentation59c2.anchor_valid;
        if(anchor_valid) {
            const bool native_menu=presentation59c2.policy.native_menu;
            const XrQuaternionf level_q=native_menu ? QGazeNoRoll(anchor.orientation) : QYawOnly(anchor.orientation);
            flat_pointer_plane_q=level_q;
            const float distance=ui88_distance;
            const V3f forward=QRotate(level_q,{0.0f,0.0f,-distance});
            const V3f centre{anchor.position.x+forward.x,anchor.position.y+forward.y,anchor.position.z+forward.z};
            const float width=ui88_width,height=ui88_height;
            const auto& rc=ui88_pointer_hand==0?input_.Left():input_.Right();
            if (rc.active || rc.aim_active) {
                const XrPose& pp=rc.aim_active?rc.aim_pose:rc.pose;
                const XrQuaternionf invq=QConj(level_q);
                const V3f ro=QRotate(invq,{pp.pos_x-centre.x,pp.pos_y-centre.y,pp.pos_z-centre.z});
                const XrQuaternionf cq{pp.qx,pp.qy,pp.qz,pp.qw};
                const V3f rd=QRotate(QMul(invq,cq),{0.0f,0.0f,-1.0f});
                if(std::fabs(rd.z)>0.005f) {
                    const float t=-ro.z/rd.z;
                    if(t>0.0f && t<8.0f) {
                        const float hx=ro.x+rd.x*t, hy=ro.y+rd.y*t;
                        const float u=hx/width+0.5f, v=0.5f-hy/height;
                        if(u>=0.0f&&u<=1.0f&&v>=0.0f&&v<=1.0f) {
                            const float uc=std::clamp(u,0.0f,1.0f), vc=std::clamp(v,0.0f,1.0f);
                            flat_pointer_u_.store(uc,std::memory_order_release);
                            flat_pointer_v_.store(vc,std::memory_order_release);
                            flat_pointer_active_.store(true,std::memory_order_release);
                            flat_laser_start={pp.pos_x,pp.pos_y,pp.pos_z};
                            const V3f hit_world=QRotate(level_q,{hx,hy,0.0f});
                            flat_laser_end={centre.x+hit_world.x,centre.y+hit_world.y,centre.z+hit_world.z};
                            flat_cursor_world=flat_laser_end; flat_cursor_world_valid=true;
                            flat_laser_valid=VLength({flat_laser_end.x-flat_laser_start.x,flat_laser_end.y-flat_laser_start.y,flat_laser_end.z-flat_laser_start.z})>0.03f;
                        }
                    }
                }
            }
            // A miss has no marker and cannot become a click at old coordinates.

        }
    }

    // Standalone loot/transfer is drawn with late HUD on the view-space panel.
    // Hit-test that EXACT surface, not the last Theater anchor. Native Flash hit
    // testing sees the same full-source UVs after its content-parent transform.
    bool immersive_laser_surface=false;
    if(immersive_menu_pointer_.load(std::memory_order_acquire)&&!flat_ui_mode_.load()&&got_pose&&ui88_pointer_hand>=0&&ui88_pointer_hand<=1){
        ui_surface_geometry::Hud geometry{};std::uint64_t epoch=0;
        if(GetHudSurfaceGeometry(geometry,epoch)&&hud_layer_v6664::Enabled()){
            const auto&rc=ui88_pointer_hand==0?input_.Left():input_.Right();
            if(rc.active||rc.aim_active){
                const auto&pp=rc.aim_active?rc.aim_pose:rc.pose;
                const XrQuaternionf hq{out_head_pose.qx,out_head_pose.qy,out_head_pose.qz,out_head_pose.qw};
                const auto&sf=geometry.surface;
                const auto c=QRotate(hq,{sf.x,sf.y,-sf.distance});
                const V3f centre{out_head_pose.pos_x+c.x,out_head_pose.pos_y+c.y,out_head_pose.pos_z+c.z};
                const auto ro=QRotate(QConj(hq),{pp.pos_x-centre.x,pp.pos_y-centre.y,pp.pos_z-centre.z});
                const XrQuaternionf rq{pp.qx,pp.qy,pp.qz,pp.qw};const auto rd=QRotate(QMul(QConj(hq),rq),{0,0,-1});
                if(std::fabs(rd.z)>.005f){const float t=-ro.z/rd.z;
                    if(t>0&&t<8){const float hx=ro.x+rd.x*t,hy=ro.y+rd.y*t,u=.5f+hx/sf.width,v=.5f-hy/sf.height;
                        if(u>=0&&u<=1&&v>=0&&v<=1){
                            flat_pointer_u_.store(u);flat_pointer_v_.store(v);flat_pointer_active_.store(true);
                            const auto hit=QRotate(hq,{hx,hy,0});flat_laser_start={pp.pos_x,pp.pos_y,pp.pos_z};
                            flat_laser_end={centre.x+hit.x,centre.y+hit.y,centre.z+hit.z};flat_cursor_world=flat_laser_end;
                            flat_pointer_plane_q=hq;flat_cursor_world_valid=true;flat_laser_valid=true;immersive_laser_surface=true;
                        }
                    }
                }
            }
        }
    }

    // V66.85 F2: right-controller ray against the fixed captured KCD2 UI plane.
    ui_layer_test_pointer_active_.store(false,std::memory_order_release);
    bool ui_test_laser_valid=false; V3f ui_test_laser_start{},ui_test_laser_end{};
    if(ui88_pointer_hand>=0 && ui88_pointer_hand<=1 && ui_layer_test_enabled_.load(std::memory_order_acquire) && ui_layer_test_menu_active_.load(std::memory_order_acquire)){
        XrPosef plane{};float width=0,height=0;{std::lock_guard<std::mutex>lk(ui_layer_test_mutex_);plane=ui_layer_test_plane_pose_;width=ui_layer_test_plane_width_;height=ui_layer_test_plane_height_;}
        const auto& rc=ui88_pointer_hand==0?input_.Left():input_.Right(); if(width>0.2f&&height>0.2f&&(rc.active||rc.aim_active)){
            const XrPose& pp=rc.aim_active?rc.aim_pose:rc.pose;const XrQuaternionf inv=QConj(plane.orientation);const V3f ro=QRotate(inv,{pp.pos_x-plane.position.x,pp.pos_y-plane.position.y,pp.pos_z-plane.position.z});const XrQuaternionf cq{pp.qx,pp.qy,pp.qz,pp.qw};const V3f rd=QRotate(QMul(inv,cq),{0,0,-1});
            if(std::fabs(rd.z)>0.005f){const float t=-ro.z/rd.z;if(t>0&&t<8){const float hx=ro.x+rd.x*t,hy=ro.y+rd.y*t,u=hx/width+0.5f,v=0.5f-hy/height;if(u>=0&&u<=1&&v>=0&&v<=1){const float uc=std::clamp(u,0.0f,1.0f),vc=std::clamp(v,0.0f,1.0f);ui_layer_test_pointer_u_.store(uc);ui_layer_test_pointer_v_.store(vc);ui_layer_test_pointer_active_.store(true);ui_test_laser_start={pp.pos_x,pp.pos_y,pp.pos_z};const V3f hw=QRotate(plane.orientation,{hx,hy,0});ui_test_laser_end={plane.position.x+hw.x,plane.position.y+hw.y,plane.position.z+hw.z};ui_test_laser_valid=VLength({ui_test_laser_end.x-ui_test_laser_start.x,ui_test_laser_end.y-ui_test_laser_start.y,ui_test_laser_end.z-ui_test_laser_start.z})>0.03f;}}}
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
    if (got_pose) {
        // HJ1: publish the target of the head the camera will now pick up (mode 0), stamped with the XR time
        // of this publication. order: target first, then the head, then the time it became visible.
        hj1_head_now_.store(0,std::memory_order_release); // readers see 0 = pair in flight -> keep the family hands
        hj1_head_target_.store(static_cast<std::int64_t>(frame_state.predictedDisplayTime),std::memory_order_release);
        CameraHook::SetHeadPoseForFrame(out_head_pose, head_pose_scale, frame_no, familyPolicyR24?&cameraFamilyR24:nullptr);
        LARGE_INTEGER qpc{};XrTime now=0;
        if(kcd2vr::hj1::enabled.load(std::memory_order_relaxed)&&qpc_time_extension_enabled_&&qpc_to_xr_time_&&QueryPerformanceCounter(&qpc)&&XR_SUCCEEDED(qpc_to_xr_time_(instance_,&qpc,&now)))
            hj1_head_now_.store(static_cast<std::int64_t>(now),std::memory_order_release);
    }

    if (stage_space_ != XR_NULL_HANDLE) {
        XrSpaceLocation sl{ XR_TYPE_SPACE_LOCATION };
        const bool ok = XR_SUCCEEDED(xrLocateSpace(view_space_, stage_space_, frame_state.predictedDisplayTime, &sl)) &&
            (sl.locationFlags & XR_SPACE_LOCATION_POSITION_VALID_BIT) && std::isfinite(sl.pose.position.y);
        if (ok) eye_height_floor_.store(sl.pose.position.y, std::memory_order_release);
        eye_height_floor_valid_.store(ok, std::memory_order_release);
    }
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
        (view_state.viewStateFlags & XR_VIEW_STATE_ORIENTATION_VALID_BIT) &&
        HeadUprightR108(views[0].pose.orientation) && HeadUprightR108(views[1].pose.orientation); // R108

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
    // V64.48: publish a complete immutable XR frame token only after pose/FOV
    // lookup has finished. KCD2 camera publication may wait on this token.
    // D67R1: use the waiter's mutex for the predicate transition. An atomic
    // store plus notify without it can consume the entire 20 ms timeout even
    // though a new token was published in the check-to-sleep window. No frame
    // work or other lock is held here; normal play still does not enable a gate.
    publication_wait_d67::notifyChange(frame_gate_mutex_,frame_gate_cv_,[&]{
        frame_gate_published_frame_.store(frame_no,std::memory_order_release);
    });

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

    // The policy captured before hit-testing travels with the queued pixel copy
    // through xrEndFrame; do not recapture a different plane after computing UVs.
    // C51R1: compute the envelope AFTER capturing this frame's presentation
    // owner. UI-only stick motion cannot build a hidden vignette. Its value
    // travels with the queued pixel copy, never re-read separately per eye.
    {
        const auto context51=comfort_context51_.load(std::memory_order_acquire);
        const auto& l51=input_.Left();const auto& r51=input_.Right();
        comfort_vignette51::Frame v51{};
        v51.level=comfort_vignette_level_.load(std::memory_order_acquire);
        v51.gameplay=(context51&1u)&&!presentation59c2.policy.flat&&!presentation59c2.policy.native_menu&&
            !presentation59c2.policy.physical_map&&!menu_quad_visible_.load(std::memory_order_acquire)&&
            !immersive_menu_pointer_.load(std::memory_order_acquire)&&!ui_layer_test_menu_active_.load(std::memory_order_acquire);
        v51.headValid=got_pose&&have_active_frame&&active_frame.should_render&&active_frame.stereo_views_valid;
        v51.inputFresh=inputFresh51&&source59c6.epoch==tracking59c6::epoch.load(std::memory_order_acquire);
        v51.smoothTurn=(context51&2u)!=0;v51.leftActive=l51.active;v51.rightActive=r51.active;
        v51.leftX=l51.thumb_x;v51.leftY=l51.thumb_y;v51.rightX=r51.thumb_x;
        v51.nowNs=source59c6.ns;v51.epoch=source59c6.epoch;
        presentation59c2.comfort_vignette51=deepseek_h29::EncodeVignette(comfort_envelope51_.Step(v51),true);
    }
    RenderFrameState layer_frame = active_frame;
    bool layer_frame_paired = false;
    uint32_t image_indices[2] = { 0, 0 };
    uint32_t ui_image_index = 0;
    bool ui_acquired = false;
    bool have_ui_layer = false;
    bool have_ui_texture = false;
    const bool have_scene_r111 = have_layer;      // R111: scene layer possible before any swapchain work
    bool eyes_ready_r111 = false, ui_reused_r111 = false;
    bool menu_requested_r111 = menu_quad_visible_.load(std::memory_order_acquire) ||
        (menu_quad_diagnostic_h24_.load(std::memory_order_acquire) && !presentation59c2.policy.flat);
    unsigned ui_miss_r111 = MenuR111Count;        // why the menu swapchain got no NEW image this frame (Count = got one)
    if (have_layer)
    {
        bool acquired[2] = { false, false };
        bool images_ready = true;
        bool eyes_written_r141 = false;                // R141: this frame's copy wrote the acquired eye images
        bool released_r141[2] = { false, false };      // R141: released = the image the runtime shows for that swapchain

        for (int eye = 0; eye < 2; ++eye)
        {
            EyeSwapchain& e = eyes_[eye];
            XrSwapchainImageAcquireInfo acquire_info{ XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO };
            if (XR_FAILED(TimedAcquireImage59c6(e.swapchain, &acquire_info, &image_indices[eye])))
            {
                images_ready = false;
                break;
            }
            acquired[eye] = true;

            XrSwapchainImageWaitInfo wait_img{ XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO };
            wait_img.timeout = XR_INFINITE_DURATION;
            if (XR_FAILED(TimedSwapchainWait165(e.swapchain, &wait_img)) ||
                image_indices[eye] >= e.d3d12_images.size())
            {
                images_ready = false;
                break;
            }
        }

        ID3D12Resource* ui_image = nullptr;
        bool ui_ready = false;
        const bool menu_visible=menu_quad_visible_.load(std::memory_order_acquire);
        const bool diagnostic_h24=menu_quad_diagnostic_h24_.load(std::memory_order_acquire)&&!presentation59c2.policy.flat;
        dice_marker_r92::Selection diceUiR92{};
        const bool diceTextureR92=dice_marker_r92::selected.read(diceUiR92)&&dice_marker_r92::fresh(diceUiR92,GetTickCount64())&&!presentation59c2.policy.flat;
        ID3D12Resource* const menu_source_r111 = menu_quad_source_.load(std::memory_order_acquire);
        if (menu_source_r111 && menu_source_r111 != menu_source_seen_r111_) { if (menu_source_seen_r111_) ++menu_r111_[MenuSourceChanges]; menu_source_seen_r111_ = menu_source_r111; }
        menu_requested_r111 = menu_visible || diagnostic_h24;
        const bool ui_wanted_r111 = menu_ui_.swapchain != XR_NULL_HANDLE &&
            (diceTextureR92 || menu_visible || diagnostic_h24 || immersive_menu_pointer_.load(std::memory_order_acquire) || presentation59c2.policy.flat || ui_layer_test_menu_active_.load(std::memory_order_acquire) || hand_interaction_laser_active_.load(std::memory_order_acquire));
        const bool want_ui_texture = menu_source_r111 != nullptr && ui_wanted_r111;
        eyes_ready_r111 = images_ready;
        if (!images_ready) ui_miss_r111 = MissEyes;
        else if (menu_ui_.swapchain == XR_NULL_HANDLE) ui_miss_r111 = MissNoSwapchain;
        else if (!menu_source_r111) ui_miss_r111 = MissNoSource;
        if (images_ready && want_ui_texture) {
            XrSwapchainImageAcquireInfo ai{XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO};
            if (XR_SUCCEEDED(TimedAcquireImage59c6(menu_ui_.swapchain,&ai,&ui_image_index))) {
                ui_acquired=true; XrSwapchainImageWaitInfo wi{XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO}; wi.timeout=XR_INFINITE_DURATION;
                if (XR_SUCCEEDED(TimedSwapchainWait165(menu_ui_.swapchain,&wi)) && ui_image_index < menu_ui_.d3d12_images.size()) { ui_image=menu_ui_.d3d12_images[ui_image_index]; ui_ready=true; }
                else ui_miss_r111 = MissWait;
            } else ui_miss_r111 = MissAcquire;
            if (!ui_ready) debug_trace::Write("V51-F7-QUAD acquire/wait failed; gameplay continues without menu layer this frame");
        }
        if (images_ready)
        {
            ID3D12Resource* left = eyes_[0].d3d12_images[image_indices[0]];
            ID3D12Resource* right = eyes_[1].d3d12_images[image_indices[1]];
            const bool ui_needs_init = ui_ready && ui_image_index < menu_ui_image_initialized_.size() && !menu_ui_image_initialized_[ui_image_index];
            const bool copy_ui = ui_ready && (menu_visible || diagnostic_h24 || ui_needs_init);
            bool ui_copy_ok = false;
            images_ready = QueueRenderThreadDiagnosticWork(left, right, ui_ready ? ui_image : nullptr, copy_ui, &ui_copy_ok, presentation59c2);
            eyes_written_r141 = images_ready;
            if (ui_copy_ok && ui_image_index < menu_ui_image_initialized_.size()) menu_ui_image_initialized_[ui_image_index] = true;
            const bool ui_initialized = ui_ready && ui_image_index < menu_ui_image_initialized_.size() && menu_ui_image_initialized_[ui_image_index];
            have_ui_texture = images_ready && ui_ready && ui_image != nullptr && ui_initialized;
            have_ui_layer = have_ui_texture && (menu_visible||diagnostic_h24);
            if (!images_ready) ui_miss_r111 = MissEyes;
            else if (ui_ready && !have_ui_texture) ui_miss_r111 = MissCopy;
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

        // V65.33 AFR per-eye timing refinement. AFR caches left/right pixels from different
        // game frames; describe each cached eye with the immutable XR frame that actually
        // rendered those pixels. Fail closed independently per eye if the ring entry was
        // overwritten. Mono and legacy shared-pose AFR remain byte-for-byte on layer_frame.
        RenderFrameState afr_eye_frame[2]{};
        bool afr_eye_frame_valid[2]{false,false};
        const bool native_pair_per_eye = presentation59c2.policy.stereo == 2 &&
            DeviceHook::V6638PairPoseMode()==3 && present_pose_pairing_mode_.load(std::memory_order_acquire) != 0;
        const bool afr_per_eye_history = (presentation59c2.policy.stereo == 1 &&
            afr_per_eye_historical_pose_.load(std::memory_order_acquire) &&
            present_pose_pairing_mode_.load(std::memory_order_acquire) != 0) || native_pair_per_eye;
        if (afr_per_eye_history) {
            for (int e=0;e<2;++e) {
                const std::uint64_t fi=last_submitted_camera_frame_by_eye_[e].load(std::memory_order_acquire);
                afr_eye_frame_valid[e]=CopyRenderFrameStateByIndex(fi,afr_eye_frame[e]) && afr_eye_frame[e].stereo_views_valid;
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
            else released_r141[eye] = true;
        }
        if (ui_acquired) {
            XrSwapchainImageReleaseInfo release_info{XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO};
            if (XR_FAILED(xrReleaseSwapchainImage(menu_ui_.swapchain,&release_info))) { have_ui_layer=false; have_ui_texture=false; menu_ui_has_released_r111_=false; ui_miss_r111=MissRelease; }
            else {
                // R111: the image just released is what the runtime shows for this swapchain until the next release.
                // It holds menu pixels only if it was ever filled (initialized); an empty one must not be re-shown.
                menu_ui_has_released_r111_ = ui_image_index < menu_ui_image_initialized_.size() && menu_ui_image_initialized_[ui_image_index];
            }
        }
        // R111: no new image this frame -> re-submit the last released one (OpenXR shows a swapchain's last released
        // image; no acquire needed). Menu and every laser drawn from the menu texture stay on screen every frame.
        if (!have_ui_texture && images_ready && ui_wanted_r111 && menu_ui_has_released_r111_ &&
            menu_ui_.swapchain != XR_NULL_HANDLE && ui_miss_r111 != MissRelease) {
            have_ui_texture = true;
            have_ui_layer = menu_visible || diagnostic_h24;
            ui_reused_r111 = true;
        }

        have_layer = images_ready&&production_presentation59c2_.Current(presentation59c2.ticket);
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
            const bool flat_ui_projection = presentation59c2.policy.flat;
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
            const bool legacy_good_mono = presentation59c2.policy.stereo == 0 && !flat_ui_projection;
            for (int eye = 0; eye < 2; ++eye)
            {
                const int source_eye = swap_eyes ? (eye ^ 1) : eye;
                EyeSwapchain& e = eyes_[source_eye];
                proj_views[eye].type = XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW;
                if (flat_ui_projection) {
                    proj_views[eye].pose = flat_pose[eye];
                    proj_views[eye].fov = flat_fov[eye];
                } else {
                    const int history_eye = swap_eyes ? (eye ^ 1) : eye;
                    const RenderFrameState* eye_frame = (afr_per_eye_history && afr_eye_frame_valid[history_eye])
                        ? &afr_eye_frame[history_eye] : &layer_frame;
                    proj_views[eye].pose = (projection_pose_mode_.load(std::memory_order_relaxed)==1 && frozen_projection_valid_.load(std::memory_order_relaxed))
                        ? frozen_projection_eye_pose_[eye]
                        : eye_frame->eye_pose[history_eye];
                    proj_views[eye].fov = eye_frame->eye_fov[history_eye];
                }
                proj_views[eye].subImage.swapchain = e.swapchain;
                proj_views[eye].subImage.imageRect.offset = { 0, 0 };
                proj_views[eye].subImage.imageRect.extent = { e.width, e.height };
            }
            if (afr_per_eye_history) {
                static std::atomic<unsigned long long> afr_hist_log{0};
                const auto n=afr_hist_log.fetch_add(1,std::memory_order_relaxed)+1;
                if(n<=8 || (n%300ull)==0) {
                    const auto lf=last_submitted_camera_frame_by_eye_[0].load(std::memory_order_acquire);
                    const auto rf=last_submitted_camera_frame_by_eye_[1].load(std::memory_order_acquire);
                    char m[384]{}; std::snprintf(m,sizeof(m),
                        "V65.33-AFR-PEREYE history L={frame:%llu valid:%d} R={frame:%llu valid:%d} delta=%lld swap=%d",
                        (unsigned long long)lf,afr_eye_frame_valid[0]?1:0,(unsigned long long)rf,afr_eye_frame_valid[1]?1:0,
                        (long long)rf-(long long)lf,swap_eyes?1:0); debug_trace::Write(m);
                }
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
            // R141: remember what each released eye image now holds. only a projection picture can be re-shown;
            // a theater/map frame overwrote the image with something else.
            const bool projection_r141 = !presentation59c2.policy.flat && !presentation59c2.policy.physical_map;
            const std::uint64_t now_r141 = GetTickCount64();
            for (int eye = 0; eye < 2; ++eye) {
                const int source_eye = swap_eyes ? (eye ^ 1) : eye;
                if (projection_r141)
                    xr_eye_hold_r141::Record(eyes_[source_eye].held_r141, image_indices[source_eye],
                        eye, proj_views[eye].pose, proj_views[eye].fov, active_frame.frame_index, now_r141);
                else xr_eye_hold_r141::Forget(eyes_[source_eye].held_r141, image_indices[source_eye]);
            }
        }
        else {
            // R141: this frame wrote no eye image (handoff timed out after 500 ms while the game stalled, no
            // presentable stereo pair, copy refused). before R141 the frame went out with NO projection and no HUD =
            // black, and the runtime kept that black until the next good frame (up to ~0.5 s in a stall). the
            // images just released still hold their last picture: send it again with the pose/fov it was drawn
            // with (runtime reprojects it world-locked). both eyes or nothing, younger than 2 s, decided every frame.
            if (eyes_written_r141) {
                for (int eye = 0; eye < 2; ++eye) xr_eye_hold_r141::Forget(eyes_[eye].held_r141, image_indices[eye]);
            } else if (xr_eye_hold_r141::enabled.load(std::memory_order_acquire) &&
                       !presentation59c2.policy.flat && !presentation59c2.policy.physical_map &&
                       acquired[0] && acquired[1] && released_r141[0] && released_r141[1] &&
                       production_presentation59c2_.Current(presentation59c2.ticket)) {
                const bool swap_r141 = swap_eyes_.load(std::memory_order_acquire);
                for (int eye = 0; eye < 2; ++eye) {
                    EyeSwapchain& e = eyes_[swap_r141 ? (eye ^ 1) : eye];
                    proj_views[eye].type = XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW;
                    proj_views[eye].subImage.swapchain = e.swapchain;
                    proj_views[eye].subImage.imageRect.offset = { 0, 0 };
                    proj_views[eye].subImage.imageRect.extent = { e.width, e.height };
                }
                const auto held = xr_eye_hold_r141::Pick(eyes_[0].held_r141, image_indices[0], eyes_[1].held_r141, image_indices[1],
                    swap_r141, GetTickCount64(), proj_views);
                ++eye_hold_r141_[static_cast<unsigned>(held)];
                if (held == xr_eye_hold_r141::Result::Held) { have_layer = true; layer_frame_paired = false; } // old picture: no D61 lead sample
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

    const bool physical_map_requested = presentation59c2.policy.physical_map;
    const bool flat_game_requested = have_layer && (presentation59c2.policy.flat || physical_map_requested);
    bool have_flat_game_quad = false;
    XrCompositionLayerQuad flat_game_quad{ XR_TYPE_COMPOSITION_LAYER_QUAD };
    if (flat_game_requested) {
        const auto anchor=presentation59c2.anchor;
        const bool anchor_valid=presentation59c2.anchor_valid;
        if (anchor_valid) {
            const bool native_menu=presentation59c2.policy.native_menu;
            const XrQuaternionf level_q = native_menu ? QGazeNoRoll(anchor.orientation) : QYawOnly(anchor.orientation);
            const float distance = ui88_distance;
            const V3f forward = QRotate(level_q, {0.0f,0.0f,-distance});
            flat_game_quad.space = local_space_;
            flat_game_quad.eyeVisibility = XR_EYE_VISIBILITY_BOTH;
            flat_game_quad.subImage.swapchain = eyes_[0].swapchain;
            flat_game_quad.subImage.imageRect.offset = {0,0};
            flat_game_quad.subImage.imageRect.extent = {eyes_[0].width,eyes_[0].height};
            flat_game_quad.pose.orientation = level_q;
            if(physical_map_requested) {
                // Crop away the native map/menu chrome; keep the central ApseMap artwork.
                // This uses KCD2's own rendered map/fog/POIs rather than minimap DDS assets.
                const int cropX=std::max(0,static_cast<int>(eyes_[0].width*0.08f));
                const int cropTop=std::max(0,static_cast<int>(eyes_[0].height*0.07f));
                const int cropBottom=std::max(0,static_cast<int>(eyes_[0].height*0.10f));
                flat_game_quad.subImage.imageRect.offset={cropX,cropTop};
                flat_game_quad.subImage.imageRect.extent={std::max(1,eyes_[0].width-2*cropX),std::max(1,eyes_[0].height-cropTop-cropBottom)};
                const auto& lc=input_.Left(); const auto& rc=input_.Right();
                if(lc.active && rc.active) {
                    const V3f lp{lc.pose.pos_x,lc.pose.pos_y,lc.pose.pos_z};
                    const V3f rp{rc.pose.pos_x,rc.pose.pos_y,rc.pose.pos_z};
                    const V3f center{0.5f*(lp.x+rp.x),0.5f*(lp.y+rp.y)+0.035f,0.5f*(lp.z+rp.z)};
                    V3f xaxis=VNormalize({rp.x-lp.x,rp.y-lp.y,rp.z-lp.z});
                    V3f zaxis=VNormalize({anchor.position.x-center.x,anchor.position.y-center.y,anchor.position.z-center.z});
                    V3f yaxis=VNormalize(VCross(zaxis,xaxis));
                    if(VLength(xaxis)>0.5f && VLength(yaxis)>0.5f && VLength(zaxis)>0.5f){
                        zaxis=VNormalize(VCross(xaxis,yaxis));
                        if(VDot(zaxis,{anchor.position.x-center.x,anchor.position.y-center.y,anchor.position.z-center.z})<0.0f){yaxis={-yaxis.x,-yaxis.y,-yaxis.z};zaxis={-zaxis.x,-zaxis.y,-zaxis.z};}
                        flat_game_quad.pose.orientation=QFromAxes(xaxis,yaxis,zaxis);
                    }
                    flat_game_quad.pose.position={center.x,center.y,center.z};
                } else if(lc.active) flat_game_quad.pose.position={lc.pose.pos_x,lc.pose.pos_y+0.035f,lc.pose.pos_z};
                else if(rc.active) flat_game_quad.pose.position={rc.pose.pos_x,rc.pose.pos_y+0.035f,rc.pose.pos_z};
                else flat_game_quad.pose.position={anchor.position.x+forward.x*0.45f,anchor.position.y+forward.y*0.45f-0.20f,anchor.position.z+forward.z*0.45f};
            } else {
                flat_game_quad.pose.position = {
                    anchor.position.x + forward.x,
                    anchor.position.y + forward.y,
                    anchor.position.z + forward.z
                };
            }
            flat_game_quad.size.width = physical_map_requested ? 0.72f : ui88_width;
            // V59.7: F6 must preserve the actual KCD2 source aspect. The old
            // hard-coded 16:9 quad squashed 1920x1800 / near-square sources.
            const float sw=static_cast<float>(std::max<std::uint32_t>(1,last_source_width_.load(std::memory_order_acquire)));
            const float sh=static_cast<float>(std::max<std::uint32_t>(1,last_source_height_.load(std::memory_order_acquire)));
            const float source_aspect=physical_map_requested
                ? std::clamp(static_cast<float>(flat_game_quad.subImage.imageRect.extent.width)/static_cast<float>(std::max(1,flat_game_quad.subImage.imageRect.extent.height)),0.75f,2.50f)
                : std::clamp(sw/sh,0.75f,2.50f);
            flat_game_quad.size.height = physical_map_requested ? flat_game_quad.size.width / source_aspect : ui88_height;
            if(cropR107&&!physical_map_requested){ // R107: keep the central band, same width -> the band fills the screen
                const int bandH=std::max(1,int(float(eyes_[0].width)/cropAspectR107+0.5f));
                const int cropTop=std::max(0,(eyes_[0].height-bandH)/2);
                flat_game_quad.subImage.imageRect.offset={0,cropTop};
                flat_game_quad.subImage.imageRect.extent={eyes_[0].width,std::min(bandH,eyes_[0].height-cropTop)};
            }
            have_flat_game_quad = true;
        }
    }

#if defined(XR_KHR_COMPOSITION_LAYER_CYLINDER_EXTENSION_NAME)
    XrCompositionLayerCylinderKHR flat_game_cylinder{ XR_TYPE_COMPOSITION_LAYER_CYLINDER_KHR };
    bool have_flat_game_cylinder=false;
    if(flat_game_requested && have_flat_game_quad && cylinder_layer_supported_ && (physical_map_requested || (!presentation59c2.policy.native_menu && flat_ui_curved_.load(std::memory_order_acquire)))){
        flat_game_cylinder.layerFlags=0;
        flat_game_cylinder.space=flat_game_quad.space;
        flat_game_cylinder.eyeVisibility=XR_EYE_VISIBILITY_BOTH;
        flat_game_cylinder.subImage=flat_game_quad.subImage;
        flat_game_cylinder.pose=flat_game_quad.pose;
        // V66.69: the physical-map proof uses a slightly more curved sheet than Theater.
        // It is still the game's native Map texture; no custom minimap DDS is required.
        const float shallow_radius=physical_map_requested?1.65f:3.20f;
        flat_game_cylinder.radius=shallow_radius;
        flat_game_cylinder.centralAngle=std::clamp(flat_game_quad.size.width/shallow_radius,physical_map_requested?0.34f:0.28f,physical_map_requested?0.58f:0.70f);
        flat_game_cylinder.aspectRatio=flat_game_quad.size.width/std::max(0.01f,flat_game_quad.size.height);
        have_flat_game_cylinder=true;
    }
#else
    const bool have_flat_game_cylinder=false;
#endif

    // Native HUD has one texture but still has binocular depth. Dice needs its
    // reading plane near the selected die; sharing pixels alone does not remove disparity.
    const auto xr_hud = (have_layer && hud_layer_v6664::Enabled())
        ? hud_layer_v6664::XrPrepare(session_, d3d12_device_, d3d12_queue_)
        : hud_layer_v6664::XrHudFrame{};
    XrCompositionLayerQuad hud_quad{ XR_TYPE_COMPOSITION_LAYER_QUAD };
    dice_hud_depth_r92r3::Receipt diceHudReceiptR93{};
    const bool have_hud_quad=xr_hud.ok;
    bool hudNormalR125=false; // R125: the ordinary head-locked / lagged HUD (not F2 world anchor, not dice depth)
    if(have_hud_quad){
        hud_quad.layerFlags=XR_COMPOSITION_LAYER_BLEND_TEXTURE_SOURCE_ALPHA_BIT;hud_quad.eyeVisibility=XR_EYE_VISIBILITY_BOTH;hud_quad.subImage.swapchain=xr_hud.swapchain;hud_quad.subImage.imageRect.offset={0,0};hud_quad.subImage.imageRect.extent={xr_hud.width,xr_hud.height};
        ui_surface_geometry::Hud ui88_hud{};
        {
            std::lock_guard<std::mutex> lock(hud_geometry_mutex_);
            if(hud_geometry_latch_.Update(UiEyes(layer_frame),float(std::max(1,xr_hud.width))/float(std::max(1,xr_hud.height)))){
                hud_geometry_=hud_geometry_latch_.Read();++hud_geometry_epoch_;
            }
            ui88_hud=hud_geometry_;
        }
        // R110: draw at the depth the surface was sized for (VrHudDistanceR110); the old fixed 1.55 only as the
        // fallback together with its own 1.82 m width (same angular size either way). Layout + laser use the surface.
        const float hudDistance=ui88_hud.surface.valid?ui88_hud.surface.distance:1.55f;
        const float fovW=ui88_hud.surface.valid?ui88_hud.surface.width:1.82f;
        const float fovH=ui88_hud.surface.valid?ui88_hud.surface.height:fovW*float(std::max(1,xr_hud.height))/float(std::max(1,xr_hud.width));
        const bool f2Menu=ui_layer_test_enabled_.load(std::memory_order_acquire)&&ui_layer_test_menu_active_.load(std::memory_order_acquire);
        if(f2Menu&&layer_frame.head_pose_valid){if(!ui_layer_test_anchor_valid_){ui_layer_test_anchor_pose_=layer_frame.head_pose;ui_layer_test_anchor_valid_=true;}const XrQuaternionf q=QGazeNoRoll(ui_layer_test_anchor_pose_.orientation);const V3f fw=QRotate(q,{0,0,-hudDistance});hud_quad.space=local_space_;hud_quad.pose.orientation=q;hud_quad.pose.position={ui_layer_test_anchor_pose_.position.x+fw.x,ui_layer_test_anchor_pose_.position.y+fw.y-0.02f,ui_layer_test_anchor_pose_.position.z+fw.z};hud_quad.size.width=fovW;hud_quad.size.height=fovH;{std::lock_guard<std::mutex>lk(ui_layer_test_mutex_);ui_layer_test_plane_pose_=hud_quad.pose;ui_layer_test_plane_width_=hud_quad.size.width;ui_layer_test_plane_height_=hud_quad.size.height;}}
        else{hudNormalR125=true;ui_layer_test_anchor_valid_=false;hud_quad.space=view_space_;hud_quad.pose.orientation={0,0,0,1};hud_quad.pose.position={0,-0.02f,-hudDistance};hud_quad.size.width=fovW;hud_quad.size.height=fovH;
            // C32: retain slow following but NEVER crop the combined native HUD
            // texture to its readable safe rectangle. Damage/fader pixels and
            // user-expanded lower bars live outside that rectangle too. Preserve
            // the original full texture and physical scale. Only orientation
            // lags by the existing bounded3-10deg; menu owners remain fixed.
            const float lag=hud_lag_.load(std::memory_order_relaxed);
            const auto& sr=ui88_hud.safe;
            const bool lazy=lag>0.001f&&layer_frame.head_pose_valid&&ui88_hud.surface.valid&&hud_lag_gameplay_.load(std::memory_order_relaxed)&&
                sr.right-sr.left>64.0f&&sr.bottom-sr.top>64.0f;
            const auto nowNs=std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
            if(lazy){
                const XrQuaternionf head=QNormalize20(layer_frame.head_pose.orientation);
                const float dt=hud_lag_valid_?std::clamp(float(double(nowNs-hud_lag_last_ns_)*1.0e-9),0.0f,0.05f):0.0f;
                if(!hud_lag_valid_){hud_lag_q_=head;hud_lag_valid_=true;}
                const float tau=0.05f+0.30f*lag;                         // seconds
                hud_lag_q_=QNlerp20(hud_lag_q_,head,dt>0.0f?1.0f-std::exp(-dt/tau):0.0f);
                const float maxLag=(3.0f+7.0f*lag)*0.01745329f;          // 3..10 degrees
                const float angle=QAngle20(head,hud_lag_q_);
                if(std::isfinite(angle)&&angle>maxLag)hud_lag_q_=QNlerp20(head,hud_lag_q_,maxLag/angle);
                const V3f c{0,-0.02f,-hudDistance};
                const V3f w=QRotate(hud_lag_q_,c);const auto& hp=layer_frame.head_pose.position;
                hud_quad.space=local_space_;hud_quad.pose.orientation=hud_lag_q_;hud_quad.pose.position={hp.x+w.x,hp.y+w.y,hp.z+w.z};
            }else hud_lag_valid_=false;
            hud_lag_last_ns_=nowNs;}
    }

    const auto normalHudQuadR93=hud_quad;
    const auto diceHudCurrentR93=[](std::uintptr_t instance,std::uint64_t visit,std::uint64_t lifetime) noexcept {
        const auto& w=activity_input163::witness175;
        return instance&&visit&&lifetime&&w.ready.load(std::memory_order_acquire)&&
            !w.transitionDepth175.load(std::memory_order_acquire)&&
            w.dice.load(std::memory_order_acquire)==instance&&
            w.diceStart.load(std::memory_order_acquire)==visit&&
            w.lifetime175.load(std::memory_order_acquire)==lifetime;
    };
    // Dice-only presentation: keep the native HUD contents/UVs and gameplay
    // camera intact, but put the reading plane at the current selected die's
    // depth. Recompute binocular coverage rather than scaling IPD with distance:
    // fader/vignette pixels must still cover BOTH full eye frusta. This is local
    // geometry, not a HUD-latch update or a native Flash relayout every frame.
    if(have_hud_quad&&!flat_game_requested&&!have_ui_layer&&
       !menu_quad_visible_.load()&&!immersive_menu_pointer_.load()&&
       !(ui_layer_test_enabled_.load()&&ui_layer_test_menu_active_.load())&&
       layer_frame.head_pose_valid&&layer_frame.stereo_views_valid){
        dice_marker_r92::Selection selectedR93{};
        visual_ray_d75::Sample cameraR93{};RenderFrameState sourceR93{};float pointR93[3]{};
        const auto nowR93=std::uint64_t(GetTickCount64());
        if(dice_marker_r92::selected.read(selectedR93)&&dice_marker_r92::fresh(selectedR93,nowR93)&&
           diceHudCurrentR93(selectedR93.instance,selectedR93.visit,selectedR93.lifetime)&&
           CameraHook::GetLatestVisualRayD75(cameraR93,80)&&
           CopyRenderFrameStateByIndex(cameraR93.poseFrame,sourceR93)&&sourceR93.head_pose_valid&&
           dice_marker_r92::viewPoint(selectedR93,cameraR93,pointR93)){
            const auto local=QRotate(sourceR93.head_pose.orientation,{pointR93[0],pointR93[1],pointR93[2]});
            // The visual camera and its XR head are one recorded frame. Express
            // that actual die point in this layer frame before deriving depth.
            const auto view=QRotate(QConj(layer_frame.head_pose.orientation),{
                sourceR93.head_pose.position.x+local.x-layer_frame.head_pose.position.x,
                sourceR93.head_pose.position.y+local.y-layer_frame.head_pose.position.y,
                sourceR93.head_pose.position.z+local.z-layer_frame.head_pose.position.z});
            const auto surface=dice_hud_depth_r92r3::computeSurface(UiEyes(layer_frame),
                float(std::max(1,xr_hud.width))/float(std::max(1,xr_hud.height)),-view.z);
            if(surface.valid){
                hud_quad.space=view_space_;hud_quad.pose.orientation={0,0,0,1};
                hud_quad.pose.position={surface.x,surface.y,-surface.distance};
                hud_quad.size={surface.width,surface.height};
                hud_lag_valid_=false; // coverage was computed for this orientation, not a lazy HUD angle
                diceHudReceiptR93={selectedR93.instance,selectedR93.visit,selectedR93.lifetime,
                    nowR93,surface.distance,surface.width,surface.height,true};
            }
        }
    }

    XrCompositionLayerQuad menu_quad{ XR_TYPE_COMPOSITION_LAYER_QUAD };
    if (have_ui_layer) {
        // R111: the menu bitmap is GDI BGRA with a uniform STRAIGHT alpha (238, 51 while previewing), colours are not
        // multiplied by alpha. Without this flag the runtime treats them as premultiplied = background added on top
        // (washed out, the see-through preview almost white).
        menu_quad.layerFlags = XR_COMPOSITION_LAYER_BLEND_TEXTURE_SOURCE_ALPHA_BIT | XR_COMPOSITION_LAYER_UNPREMULTIPLIED_ALPHA_BIT;
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
    if(ui_laser_visible_.load(std::memory_order_acquire) && have_ui_layer && menu_laser_valid){
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
                menu_laser_quad.size.height=0.0018f;
                have_menu_laser=true;
                static bool s_logged_menu_laser=false;
                if(!s_logged_menu_laser){
                    s_logged_menu_laser=true;
                    debug_trace::Write("V61.32-MENU-LASER OpenXR right-grip ribbon active; UI-only composition layer, exact menu ray unchanged");
                }
            }
        }
    }

    // V63.0 F6 pointer visualization. Reuse the same 3x3 gold texel as the F7
    // laser, but keep the layer in LOCAL space so it remains attached to the theater screen.
    XrCompositionLayerQuad flat_laser_quad{XR_TYPE_COMPOSITION_LAYER_QUAD};
    XrCompositionLayerQuad flat_cursor_quad{XR_TYPE_COMPOSITION_LAYER_QUAD};
    bool have_flat_laser=false, have_flat_cursor=false;
    if(ui_laser_visible_.load(std::memory_order_acquire) && have_ui_texture && (have_flat_game_quad || immersive_laser_surface) && flat_laser_valid){
        V3f beam{flat_laser_end.x-flat_laser_start.x,flat_laser_end.y-flat_laser_start.y,flat_laser_end.z-flat_laser_start.z};
        const float beam_len=VLength(beam);
        if(beam_len>0.03f && std::isfinite(beam_len)){
            const V3f xaxis=VNormalize(beam);
            const V3f center{(flat_laser_start.x+flat_laser_end.x)*0.5f,(flat_laser_start.y+flat_laser_end.y)*0.5f,(flat_laser_start.z+flat_laser_end.z)*0.5f};
            V3f to_eye{0.0f,1.0f,0.0f};
            if(got_pose) to_eye=VNormalize({located_head_pose.position.x-center.x,located_head_pose.position.y-center.y,located_head_pose.position.z-center.z});
            V3f yaxis=VCross(to_eye,xaxis);
            if(VLength(yaxis)<1.0e-4f) yaxis=VCross({0.0f,1.0f,0.0f},xaxis);
            if(VLength(yaxis)<1.0e-4f) yaxis=VCross({1.0f,0.0f,0.0f},xaxis);
            yaxis=VNormalize(yaxis); const V3f zaxis=VNormalize(VCross(xaxis,yaxis));
            if(VLength(yaxis)>0.5f && VLength(zaxis)>0.5f){
                flat_laser_quad.layerFlags=XR_COMPOSITION_LAYER_BLEND_TEXTURE_SOURCE_ALPHA_BIT;
                flat_laser_quad.space=local_space_; flat_laser_quad.eyeVisibility=XR_EYE_VISIBILITY_BOTH;
                flat_laser_quad.subImage.swapchain=menu_ui_.swapchain; flat_laser_quad.subImage.imageRect.offset={0,0}; flat_laser_quad.subImage.imageRect.extent={3,3};
                flat_laser_quad.pose.orientation=QFromAxes(xaxis,yaxis,zaxis); flat_laser_quad.pose.position={center.x,center.y,center.z};
                flat_laser_quad.size.width=beam_len; flat_laser_quad.size.height=0.0018f; have_flat_laser=true;
            }
        }
    }
    if(ui_laser_visible_.load(std::memory_order_acquire) && have_ui_texture && (have_flat_game_quad || immersive_laser_surface) && flat_cursor_world_valid){
        flat_cursor_quad.layerFlags=XR_COMPOSITION_LAYER_BLEND_TEXTURE_SOURCE_ALPHA_BIT;
        flat_cursor_quad.space=local_space_; flat_cursor_quad.eyeVisibility=XR_EYE_VISIBILITY_BOTH;
        flat_cursor_quad.subImage.swapchain=menu_ui_.swapchain; flat_cursor_quad.subImage.imageRect.offset={0,0}; flat_cursor_quad.subImage.imageRect.extent={3,3};
        flat_cursor_quad.pose.orientation=flat_pointer_plane_q;
        const V3f cursor_forward=QRotate(flat_pointer_plane_q,{0.0f,0.0f,0.002f});
        flat_cursor_quad.pose.position={flat_cursor_world.x+cursor_forward.x,flat_cursor_world.y+cursor_forward.y,flat_cursor_world.z+cursor_forward.z};
        flat_cursor_quad.size.width=0.006f; flat_cursor_quad.size.height=0.006f; have_flat_cursor=true;
        static bool logged=false; if(!logged){logged=true; debug_trace::Write("V64.17-F6-LASER thin medieval-gold laser + small hit dot; visual can be disabled while hit-test remains active");}
    }

    // V66.69 F4 immersive hand interaction beam. This visual consumes the same LOCAL-space
    // controller AIM pose used to construct the authoritative KCD2 world interaction ray.
    // It is intentionally a composition-only ribbon; no camera/game-render state is touched.
    XrCompositionLayerQuad ui_test_laser_quad{XR_TYPE_COMPOSITION_LAYER_QUAD}; bool have_ui_test_laser=false;
    if(ui_test_laser_valid&&have_ui_texture&&ui_laser_visible_.load(std::memory_order_acquire)){V3f beam{ui_test_laser_end.x-ui_test_laser_start.x,ui_test_laser_end.y-ui_test_laser_start.y,ui_test_laser_end.z-ui_test_laser_start.z};const float len=VLength(beam);if(len>0.03f){const V3f x=VNormalize(beam);const V3f c{(ui_test_laser_start.x+ui_test_laser_end.x)*0.5f,(ui_test_laser_start.y+ui_test_laser_end.y)*0.5f,(ui_test_laser_start.z+ui_test_laser_end.z)*0.5f};V3f y=VCross(VNormalize({-c.x,-c.y,-c.z}),x);if(VLength(y)<1e-4f)y=VCross({0,1,0},x);y=VNormalize(y);const V3f z=VNormalize(VCross(x,y));if(VLength(y)>0.5f&&VLength(z)>0.5f){ui_test_laser_quad.layerFlags=XR_COMPOSITION_LAYER_BLEND_TEXTURE_SOURCE_ALPHA_BIT;ui_test_laser_quad.space=local_space_;ui_test_laser_quad.eyeVisibility=XR_EYE_VISIBILITY_BOTH;ui_test_laser_quad.subImage.swapchain=menu_ui_.swapchain;ui_test_laser_quad.subImage.imageRect.offset={0,0};ui_test_laser_quad.subImage.imageRect.extent={3,3};ui_test_laser_quad.pose.orientation=QFromAxes(x,y,z);ui_test_laser_quad.pose.position={c.x,c.y,c.z};ui_test_laser_quad.size.width=len;ui_test_laser_quad.size.height=0.0022f;have_ui_test_laser=true;}}}

    XrCompositionLayerQuad hand_interaction_laser_quad{XR_TYPE_COMPOSITION_LAYER_QUAD};
    bool have_hand_interaction_laser=false;
    if(hand_interaction_laser_active_.load(std::memory_order_acquire) && have_ui_texture){
        const V3f a{hand_interaction_laser_ox_.load(std::memory_order_relaxed),hand_interaction_laser_oy_.load(std::memory_order_relaxed),hand_interaction_laser_oz_.load(std::memory_order_relaxed)};
        const V3f b{hand_interaction_laser_ex_.load(std::memory_order_relaxed),hand_interaction_laser_ey_.load(std::memory_order_relaxed),hand_interaction_laser_ez_.load(std::memory_order_relaxed)};
        const V3f beam{b.x-a.x,b.y-a.y,b.z-a.z};
        const float beam_len=VLength(beam);
        if(beam_len>0.03f && std::isfinite(beam_len)){
            const V3f xaxis=VNormalize(beam);
            const V3f center{(a.x+b.x)*0.5f,(a.y+b.y)*0.5f,(a.z+b.z)*0.5f};
            V3f to_eye{0.0f,1.0f,0.0f};
            if(got_pose) to_eye=VNormalize({located_head_pose.position.x-center.x,located_head_pose.position.y-center.y,located_head_pose.position.z-center.z});
            V3f yaxis=VCross(to_eye,xaxis);
            if(VLength(yaxis)<1.0e-4f) yaxis=VCross({0.0f,1.0f,0.0f},xaxis);
            if(VLength(yaxis)<1.0e-4f) yaxis=VCross({1.0f,0.0f,0.0f},xaxis);
            yaxis=VNormalize(yaxis); const V3f zaxis=VNormalize(VCross(xaxis,yaxis));
            if(VLength(yaxis)>0.5f && VLength(zaxis)>0.5f){
                hand_interaction_laser_quad.layerFlags=XR_COMPOSITION_LAYER_BLEND_TEXTURE_SOURCE_ALPHA_BIT;
                hand_interaction_laser_quad.space=local_space_;
                hand_interaction_laser_quad.eyeVisibility=XR_EYE_VISIBILITY_BOTH;
                hand_interaction_laser_quad.subImage.swapchain=menu_ui_.swapchain;
                hand_interaction_laser_quad.subImage.imageRect.offset={3,0}; // V66.69 reserved red 3x3 texel
                hand_interaction_laser_quad.subImage.imageRect.extent={3,3};
                hand_interaction_laser_quad.pose.orientation=QFromAxes(xaxis,yaxis,zaxis);
                hand_interaction_laser_quad.pose.position={center.x,center.y,center.z};
                hand_interaction_laser_quad.size.width=beam_len;
                hand_interaction_laser_quad.size.height=0.0015f;
                have_hand_interaction_laser=true;
                static bool logged=false; if(!logged){logged=true; debug_trace::Write("V66.69-F4-HAND-LASER LOCAL-space thin red composition ribbon active; authoritative native interaction ray remains KCD2-owned");}
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

    // A scene edge while the GPU copy was in flight invalidates this scene
    // submission, after normal fence waits/releases. Never put the new scene's
    // pixels on the previous scene's plane (including a close/reopen ABA).
    if(!production_presentation59c2_.Current(presentation59c2.ticket)){
        have_layer=false;
        // A hit on a cancelled scene cannot survive as a mouse click or an
        // independently submitted ray after its panel was discarded.
        flat_pointer_active_.store(false,std::memory_order_release);
        have_flat_laser=have_flat_cursor=false;
    }
    // A die selector is world UI, not a second head-locked 1.55m HUD image.
    // Reuse the tiny immutable atlas already copied for menu pointers.
    XrCompositionLayerQuad diceQuadR92{XR_TYPE_COMPOSITION_LAYER_QUAD};
    dice_marker_r92::Selection diceR92{};dice_marker_r92::Presented dicePresentedR92{};
    bool haveDiceR92=false;
    if(have_layer&&!flat_game_requested&&!menu_quad_visible_.load()&&have_ui_texture&&
       dice_marker_r92::selected.read(diceR92)&&dice_marker_r92::fresh(diceR92,GetTickCount64())){
        visual_ray_d75::Sample cameraR92{};RenderFrameState sourceR92{};float viewR92[3]{};
        if(CameraHook::GetLatestVisualRayD75(cameraR92,80)&&
           CopyRenderFrameStateByIndex(cameraR92.poseFrame,sourceR92)&&sourceR92.head_pose_valid&&
           dice_marker_r92::viewPoint(diceR92,cameraR92,viewR92)){
            const auto q=sourceR92.head_pose.orientation;
            const auto d=QRotate(q,{viewR92[0],viewR92[1],viewR92[2]});
            const auto x=VNormalize(QRotate(q,{cameraR92.rightR92[0],cameraR92.upR92[0],-cameraR92.forward[0]}));
            const auto y=VNormalize(QRotate(q,{cameraR92.rightR92[1],cameraR92.upR92[1],-cameraR92.forward[1]}));
            const auto z=VNormalize(VCross(x,y));
            diceQuadR92.layerFlags=XR_COMPOSITION_LAYER_BLEND_TEXTURE_SOURCE_ALPHA_BIT;
            diceQuadR92.space=local_space_;diceQuadR92.eyeVisibility=XR_EYE_VISIBILITY_BOTH;
            diceQuadR92.subImage.swapchain=menu_ui_.swapchain;
            diceQuadR92.subImage.imageRect.offset={dice_marker_r92::atlasX,dice_marker_r92::atlasY};
            diceQuadR92.subImage.imageRect.extent={dice_marker_r92::atlasSize,dice_marker_r92::atlasSize};
            diceQuadR92.pose.orientation=QFromAxes(x,y,z);
            diceQuadR92.pose.position={sourceR92.head_pose.position.x+d.x,sourceR92.head_pose.position.y+d.y,sourceR92.head_pose.position.z+d.z};
            diceQuadR92.size={.045f/cameraR92.worldScaleR92,.045f/cameraR92.worldScaleR92};haveDiceR92=VLength(z)>.9f;
            if(haveDiceR92)dicePresentedR92={diceR92.instance,diceR92.visit,diceR92.lifetime,std::uint64_t(GetTickCount64()),diceR92.index,true};
        }
    }
    (void)dice_marker_r92::prepared.publish(dicePresentedR92);
    dice_marker_r92::Presented diceHiddenR92{};
    haveDiceR92=haveDiceR92&&dice_marker_r92::flatOwnerReady.load()&&dice_marker_r92::hidden.read(diceHiddenR92)&&
        dice_marker_r92::ownsFlatCursor(diceR92,diceHiddenR92,GetTickCount64());
    if(!haveDiceR92)dicePresentedR92={};
    // R106: repair/check every overlay before xrEndFrame (SteamVR refuses the whole frame for one bad layer; VDXR
    // draws it anyway). Each begun frame is sent ONCE (Codex R105: no second xrEndFrame). When the runtime answers
    // with a LAYER error code, the overlay kinds of that frame sit out shortly and return one by one; the kind
    // whose return makes a frame fail sits out 10/20/40/60 s, then is tried again (xr_layer_guard_r104.h).
    namespace lgR104=xr_layer_guard_r104;
    const std::uint64_t nowR104=GetTickCount64();
    unsigned fixMaskR104[lgR104::Count]{};
    auto quadR104=[&](unsigned kind,bool have,XrCompositionLayerQuad& l,std::int32_t w,std::int32_t h)->bool{
        if(!have)return false;
        if(!layer_guard_r104_.admit(kind,nowR104)){++layer_held_r104_[kind];return false;} // R106 sit-out
        const unsigned drop=lgR104::fixQuad(l,w,h,&fixMaskR104[kind]);
        if(drop){NoteLayerDropR104(kind,drop);return false;}
        if(fixMaskR104[kind])NoteLayerFixR104(kind,fixMaskR104[kind]);
        return true;
    };
    const std::int32_t menuWR104=menu_ui_.swapchain?menu_ui_.width:0,menuHR104=menu_ui_.swapchain?menu_ui_.height:0;
    const bool sendFlatQuadR104=quadR104(lgR104::FlatGame,have_layer&&flat_game_requested&&have_flat_game_quad
#if defined(XR_KHR_COMPOSITION_LAYER_CYLINDER_EXTENSION_NAME)
        &&!have_flat_game_cylinder
#endif
        ,flat_game_quad,eyes_[0].width,eyes_[0].height);
#if defined(XR_KHR_COMPOSITION_LAYER_CYLINDER_EXTENSION_NAME)
    bool sendFlatCylR104=false;
    if(have_layer&&flat_game_requested&&have_flat_game_cylinder){
        if(!layer_guard_r104_.admit(lgR104::FlatGame,nowR104))++layer_held_r104_[lgR104::FlatGame];
        else{const unsigned drop=lgR104::fixCylinder(flat_game_cylinder,eyes_[0].width,eyes_[0].height,&fixMaskR104[lgR104::FlatGame]);
            if(drop)NoteLayerDropR104(lgR104::FlatGame,drop);else{sendFlatCylR104=true;if(fixMaskR104[lgR104::FlatGame])NoteLayerFixR104(lgR104::FlatGame,fixMaskR104[lgR104::FlatGame]);}}
    }
#endif
    const bool sendFlatLaserR104=quadR104(lgR104::FlatLaser,have_flat_laser,flat_laser_quad,menuWR104,menuHR104);
    const bool sendFlatCursorR104=quadR104(lgR104::FlatCursor,have_flat_cursor,flat_cursor_quad,menuWR104,menuHR104);
    const bool sendDiceR104=quadR104(lgR104::Dice,haveDiceR92,diceQuadR92,menuWR104,menuHR104);
    const bool sendTestLaserR104=quadR104(lgR104::TestLaser,have_ui_test_laser,ui_test_laser_quad,menuWR104,menuHR104);
    const bool sendHandLaserR104=quadR104(lgR104::HandLaser,have_hand_interaction_laser,hand_interaction_laser_quad,menuWR104,menuHR104);
    const bool sendHudR104=quadR104(lgR104::Hud,have_hud_quad,hud_quad,xr_hud.width,xr_hud.height);
    const bool sendMenuR104=quadR104(lgR104::Menu,have_layer&&have_ui_layer&&!use_menu_cylinder,menu_quad,menuWR104,menuHR104);
    const bool sendMenuLaserR104=sendMenuR104&&quadR104(lgR104::MenuLaser,have_menu_laser,menu_laser_quad,menuWR104,menuHR104);
    unsigned kindsR104=0;
    const XrCompositionLayerBaseHeader* layers[13]{};
    uint32_t layer_count=0;
    if (have_layer) {
        // V66.118R1: one visual owner at a time.  The projection-under-Theater
        // experiment produced a second, incorrectly head-related background image
        // on headset.  Theater replaces projection; gameplay restores projection.
        if (!flat_game_requested) {
            layers[layer_count++] = reinterpret_cast<const XrCompositionLayerBaseHeader*>(&projection_layer);
        } else {
#if defined(XR_KHR_COMPOSITION_LAYER_CYLINDER_EXTENSION_NAME)
            if (sendFlatCylR104) {layers[layer_count++] = reinterpret_cast<const XrCompositionLayerBaseHeader*>(&flat_game_cylinder);kindsR104|=1u<<lgR104::FlatGame;}
            else
#endif
            if (sendFlatQuadR104) {layers[layer_count++] = reinterpret_cast<const XrCompositionLayerBaseHeader*>(&flat_game_quad);kindsR104|=1u<<lgR104::FlatGame;}
        }
    }
    const uint32_t layerCapacityR105=std::min(13u,max_layer_count_r104_?max_layer_count_r104_:13u);
    auto pushR104=[&](bool send,unsigned kind,const void* l){
        if(!send)return;
        if(layer_count>=layerCapacityR105){++layer_held_r104_[kind];return;}
        layers[layer_count++]=reinterpret_cast<const XrCompositionLayerBaseHeader*>(l);kindsR104|=1u<<kind;
    };
    pushR104(sendFlatLaserR104,lgR104::FlatLaser,&flat_laser_quad);
    pushR104(sendFlatCursorR104,lgR104::FlatCursor,&flat_cursor_quad);
    pushR104(sendDiceR104,lgR104::Dice,&diceQuadR92);
    pushR104(sendTestLaserR104,lgR104::TestLaser,&ui_test_laser_quad);
    pushR104(sendHandLaserR104,lgR104::HandLaser,&hand_interaction_laser_quad);
    pushR104(sendHudR104,lgR104::Hud,&hud_quad);
    pushR104(sendMenuR104,lgR104::Menu,&menu_quad);
    pushR104(sendMenuLaserR104,lgR104::MenuLaser,&menu_laser_quad);

    // Receipt from the actual submit gates, not a native-UI probe. Pair this
    // native surface ticket with UI_LASER_INPUT's qualified menu owner. Four
    // structural changes per presentation ticket; never log UV jitter per frame.
    if(presentation59c2.policy.native_menu){
        static std::uint64_t lastTicketM1=~std::uint64_t{0};
        static std::uint32_t lastStateM1=~std::uint32_t{0};static unsigned receiptsM1=0;
        if(lastTicketM1!=presentation59c2.ticket){lastTicketM1=presentation59c2.ticket;lastStateM1=~std::uint32_t{0};receiptsM1=0;}
        const auto& handM1=ui88_pointer_hand==0?input_.Left():input_.Right();
        const bool currentM1=production_presentation59c2_.Current(presentation59c2.ticket);
        const bool enabledM1=ui_laser_visible_.load(std::memory_order_acquire);
        const auto stateM1=unsigned(presentation59c2.anchor_valid)|(unsigned(presentation59c2.policy.flat)<<1)|
            (unsigned(currentM1)<<2)|(unsigned(have_layer)<<3)|(unsigned(have_flat_game_quad)<<4)|
            (unsigned(have_ui_texture)<<5)|(unsigned(enabledM1)<<6)|(unsigned(ui88_pointer_hand+1)<<7)|
            (unsigned(handM1.active)<<9)|(unsigned(handM1.aim_active)<<10)|(unsigned(flat_laser_valid)<<11)|
            (unsigned(have_flat_laser)<<12)|(unsigned(have_flat_cursor)<<13);
        if(stateM1!=lastStateM1&&receiptsM1<4){lastStateM1=stateM1;++receiptsM1;char m[420]{};
            std::snprintf(m,sizeof(m),"M1 UI_LASER_XR surface=NATIVE_MENU ticket=%llu receipt=%u/4 flat=%d anchor=%d ticketCurrent=%d sceneSubmitted=%d panelReady=%d uiTextureReady=%d laserEnabled=%d hand=%d tracked=%d aimTracked=%d rayHit=%d laserSubmitted=%d cursorSubmitted=%d plane=ONE_FRAME_SNAPSHOT",
                (unsigned long long)presentation59c2.ticket,receiptsM1,presentation59c2.policy.flat,presentation59c2.anchor_valid,currentM1,have_layer,have_flat_game_quad,have_ui_texture,enabledM1,ui88_pointer_hand,handM1.active,handM1.aim_active,flat_laser_valid,have_flat_laser,have_flat_cursor);
            diagnostic_trace::Write(m);
        }
    }

    // Transition-only evidence from values already used by this compositor.
    // No hooks, native camera queries, waits or loading-time polling.
    static std::uint32_t lastComposition135=~std::uint32_t{0};
    const bool nativePlane135=native_menu_plane_mode_.load(std::memory_order_acquire);
    const bool fullFit135=full_menu_fit_.load(std::memory_order_acquire);
    const std::uint32_t composition135=(flat_game_requested?1u:0u)|(nativePlane135?2u:0u)|(fullFit135?4u:0u);
    if(composition135!=lastComposition135){
        lastComposition135=composition135;
        char m[512]{};std::snprintf(m,sizeof(m),
            "V66.135R2-COMPOSITION theaterRequested=%d nativePlane=%d fullMenuFit=%d projectionSubmitted=%d flatQuadReady=%d cachedGameFov=%.2f xrLeftFovRad=(%.3f,%.3f,%.3f,%.3f) flatSizeMeters=(%.3f,%.3f) nativeReadback=NONE",
            flat_game_requested,nativePlane135,fullFit135,have_layer&&!flat_game_requested,have_flat_game_quad,
            double(CameraHook::LatestGameplayFovDegrees()),double(proj_views[0].fov.angleLeft),double(proj_views[0].fov.angleRight),
            double(proj_views[0].fov.angleUp),double(proj_views[0].fov.angleDown),double(flat_game_quad.size.width),double(flat_game_quad.size.height));
        diagnostic_trace::Write(m);
    }

    // The native activity can exit while XR composition is being assembled.
    // A copied last-die point must not carry its close HUD plane into sharpening
    // or ordinary play; revoke through the existing lifecycle atomics.
    if(diceHudReceiptR93.valid&&!diceHudCurrentR93(diceHudReceiptR93.instance,
       diceHudReceiptR93.visit,diceHudReceiptR93.lifetime)){
        hud_quad=normalHudQuadR93;diceHudReceiptR93={};
    }

    // R125 HUD at infinity (VrHudInfinityR125, default ON). Miki 6 Nov: "the HUD is still slightly double, i need to
    // look closer to see it". the one HUD quad at VrHudDistanceR110 (5 m in his ini) is seen by both eyes like a real
    // board 5 m away: disparity IPD/D = 0.069/5 = 0.8 deg. while the eyes converge on the world further away that is
    // outside fusion = double; to read it he has to converge on it. here the one HUD quad becomes two, LEFT eye only and
    // RIGHT eye only, each moved sideways with its own eye (the eye position relative to the head, in the quad's space).
    // seen from each eye every HUD pixel lies in exactly the same direction = zero disparity = optical infinity, so it
    // fuses whatever the eyes look at. size, layout, safe box, palm reveal and the laser keep the same (angular) layout,
    // the lag rotation keeps working. not used for the F2 world-anchored test plane and the dice-depth plane (those
    // want depth). a frame without valid eye poses or without a free layer slot keeps the old single quad.
    XrCompositionLayerQuad hudEyesR125[2]{hud_quad,hud_quad};
    if(hudNormalR125&&!diceHudReceiptR93.valid&&ui_surface_geometry::hudInfinityR125.load(std::memory_order_acquire)&&
       layer_frame.head_pose_valid&&layer_frame.stereo_views_valid){
        int atR125=-1;
        for(uint32_t i=0;i<layer_count;++i)if(layers[i]==reinterpret_cast<const XrCompositionLayerBaseHeader*>(&hud_quad))atR125=int(i);
        const auto eyesR125=UiEyes(layer_frame); // eye position relative to the head, head (VIEW) axes
        V3f offR125[2]{};bool okR125=atR125>=0&&layer_count<layerCapacityR105&&layer_count<13u;
        for(int e=0;e<2&&okR125;++e){
            const auto& p=eyesR125[e].position;
            okR125=std::isfinite(p.x)&&std::isfinite(p.y)&&std::isfinite(p.z)&&std::fabs(p.x)<0.10f&&std::fabs(p.y)<0.10f&&std::fabs(p.z)<0.10f;
            const auto& ep=layer_frame.eye_pose[e].position;const auto& hp=layer_frame.head_pose.position;
            offR125[e]=hud_quad.space==view_space_?V3f{p.x,p.y,p.z}:V3f{ep.x-hp.x,ep.y-hp.y,ep.z-hp.z}; // lag path: LOCAL axes
        }
        okR125=okR125&&std::isfinite(offR125[0].x+offR125[0].y+offR125[0].z+offR125[1].x+offR125[1].y+offR125[1].z);
        if(okR125){
            for(int e=0;e<2;++e){
                hudEyesR125[e].eyeVisibility=e==0?XR_EYE_VISIBILITY_LEFT:XR_EYE_VISIBILITY_RIGHT;
                hudEyesR125[e].pose.position.x+=offR125[e].x;hudEyesR125[e].pose.position.y+=offR125[e].y;hudEyesR125[e].pose.position.z+=offR125[e].z;
            }
            for(uint32_t i=layer_count;i>uint32_t(atR125)+1u;--i)layers[i]=layers[i-1];
            layers[atR125]=reinterpret_cast<const XrCompositionLayerBaseHeader*>(&hudEyesR125[0]);
            layers[atR125+1]=reinterpret_cast<const XrCompositionLayerBaseHeader*>(&hudEyesR125[1]);
            ++layer_count;
        }
    }

    XrFrameEndInfo end_info{ XR_TYPE_FRAME_END_INFO };
    end_info.displayTime = have_active_frame ? active_frame.predicted_display_time : frame_state.predictedDisplayTime;
    end_info.environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE;
    end_info.layerCount = layer_count;
    end_info.layers = layer_count ? layers : nullptr;
    static bool last_flat_game_quad=false;
    if (have_flat_game_quad != last_flat_game_quad) {
        if(have_flat_game_quad) {
            const bool native_menu=presentation59c2.policy.native_menu;
            debug_trace::Write(native_menu
                ? "V66.20-NATIVE-MENU-LAYER LOCAL gaze-anchored plane active; black surroundings; head may look away freely"
                : (have_flat_game_cylinder ? "V64.12-F6-LAYER shallow CONCAVE cylinder active; HMD movement independent of screen pose" : "V58.7-F6-LAYER LOCAL quad active; HMD movement is independent of screen pose"));
        } else debug_trace::Write("V66.20-FLAT-LAYER inactive; immersive projection active");
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
    XrResult end_result;
    { diagnostics::xr165::Timer timing165(diagnostics::xr165::EndFrame);end_result=xrEndFrame(session_, &end_info); }
    // R106: xrEndFrame consumes this submission (never a second xrEndFrame on the same frame). A layer error code
    // puts the overlay kinds of this frame into the sit-out; other codes (time, session) blame nobody.
    {const unsigned overlayKindsR106=kindsR104&~(1u<<lgR104::FlatGame);
     const std::uint32_t sitOutR106=layer_guard_r104_.onResult(XR_SUCCEEDED(end_result)?kindsR104:overlayKindsR106,XR_SUCCEEDED(end_result),nowR104,int(end_result));
     NoteEndFrameR105(kindsR104,int(end_result),sitOutR106,nowR104);}
    if (menu_requested_r111) {
        ++menu_r111_[MenuRequested];
        const bool sentR111 = XR_SUCCEEDED(end_result) && ((kindsR104>>lgR104::Menu)&1u);
        if (ui_miss_r111 < MenuR111Count) ++menu_r111_[ui_miss_r111];
        if (sentR111) { ++menu_r111_[MenuSent]; ++menu_r111_[ui_reused_r111 ? MenuReused : MenuFresh]; }
        else {
            ++menu_r111_[MenuNotSent];
            if (!have_scene_r111) ++menu_r111_[MissNoScene];
            else if (eyes_ready_r111 && ui_miss_r111 == MenuR111Count && !have_layer) ++menu_r111_[MissSceneCancelled];
            else if (eyes_ready_r111 && have_ui_layer && !sendMenuR104) ++menu_r111_[MissGuard];
        }
        if (XR_SUCCEEDED(end_result) && ((kindsR104>>lgR104::MenuLaser)&1u)) ++menu_r111_[MenuLaserSent];
    }
    NoteMenuFrameR111(nowR104);
    (void)dice_marker_r92::presented.publish(XR_SUCCEEDED(end_result)&&((kindsR104>>lgR104::Dice)&1u)?dicePresentedR92:dice_marker_r92::Presented{});
    (void)dice_hud_depth_r92r3::applied.publish(XR_SUCCEEDED(end_result)&&((kindsR104>>lgR104::Hud)&1u)?diceHudReceiptR93:dice_hud_depth_r92r3::Receipt{});
    PacingReceiptH49(12,"XR_END_RETURN_NOT_SCANOUT",0,have_active_frame?active_frame.frame_index:0,0,0,
        end_info.displayTime,int(end_result),unsigned(layer_count));
    if (XR_FAILED(end_result)) {
        debug_trace::Write("xrEndFrame failed");
        return false;
    }

    // Observe only successfully submitted immersive projection frames. The
    // exact paired camera frame is already resolved above; no new GPU readback.
    if(handPresentD61&&hand_present_d61::CurrentPolicy(handPolicyD61)){
        const bool pairedD61=have_layer&&layer_frame_paired&&!presentation59c2.policy.flat&&
            !presentation59c2.policy.native_menu&&!presentation59c2.policy.physical_map;
        hand_present_lead_d61_.observe(have_active_frame?active_frame.frame_index:0,
            pairedD61?layer_frame.frame_index:0,pairedD61);
        if(diagnostics::capture168::active(diagnostics::capture168::Domain::Bug,GetTickCount64())){
            static std::uint64_t nextD61=0;const auto nowD61=std::uint64_t(GetTickCount64());
            if(nowD61>=nextD61){nextD61=nowD61+1000;char noteD61[400]{};
                std::snprintf(noteD61,sizeof(noteD61),"D61 HAND_PRESENT_LEAD scope=FREE_HAND_DISPLAY leadMs=%.3f pairedAccepted=%llu rejected=%llu warm=%u headTime=UNCHANGED rawGameplay=UNCHANGED heldWeapons=UNCHANGED",
                    double(hand_present_lead_d61_.leadNs())/1e6,(unsigned long long)hand_present_lead_d61_.accepted,
                    (unsigned long long)hand_present_lead_d61_.rejected,hand_present_lead_d61_.n);diagnostic_trace::Write(noteD61);}
        }
    }
    return got_pose;
}

XrManager::FreshPoseUncappedReceipt XrManager::RefreshUncappedPoseFamily() noexcept
{
    FreshPoseUncappedReceipt receipt{};
    receipt.source=CameraHook::GetPoseSourceMode();
    const auto retained=[&]() noexcept {
        tracking59c6::Frame cached{};
        if(!receipt.published&&tracking59c6::Read(cached)){
            receipt.sequence=cached.sequence;receipt.sourceNs=cached.ns;receipt.sourceTick=cached.tick;
            receipt.target=cached.locateTime;
            for(unsigned i=0;i<3;++i){receipt.validMask|=unsigned(cached.valid[i])<<i;receipt.trackedMask|=unsigned(cached.tracked[i])<<i;}
        }
        return receipt;
    };
    if(receipt.source==3){receipt.result=FreshPoseUncappedReceipt::SourceOffset;return retained();}
    const auto generation=motion_trials_d67::handRefreshGenerationR94.load(std::memory_order_acquire);
    const auto samplingEpoch=tracking59c6::epoch.load(std::memory_order_acquire);
    const auto current=[&]() noexcept {
        return receipt.source>=0&&receipt.source<=2&&motion_trials_d67::UncappedSelectedAt(0)&&
            tracking59c6::enabled.load(std::memory_order_acquire)&&
            tracking59c6::cameraSource.load(std::memory_order_acquire)==receipt.source&&
            tracking59c6::epoch.load(std::memory_order_acquire)==samplingEpoch&&
            motion_trials_d67::handRefreshGenerationR94.load(std::memory_order_acquire)==generation;
    };
    if(!current()){receipt.result=FreshPoseUncappedReceipt::TrackingDisabled;return retained();}
    if(!session_running_.load(std::memory_order_acquire)||view_space_==XR_NULL_HANDLE||local_space_==XR_NULL_HANDLE)return retained();
    receipt.result=FreshPoseUncappedReceipt::ClockUnavailable;
    if(!qpc_time_extension_enabled_||!qpc_to_xr_time_)return retained();
    // Snapshot the prediction before measuring its age. A worker publication
    // after a previously read QPC time is newer, not a stale hand prediction.
    fresh_hands_r94::Anchor anchor{};
    if(receipt.source!=2&&!fresh_hands_r94::anchors.read(anchor)){
        receipt.result=FreshPoseUncappedReceipt::StalePrediction;return retained();
    }
    LARGE_INTEGER qpc{};XrTime now=0;
    if(!QueryPerformanceCounter(&qpc)||XR_FAILED(qpc_to_xr_time_(instance_,&qpc,&now))||now<=0)return retained();
    XrTime target=now;
    if(receipt.source!=2&&!fresh_hands_r94::Target(anchor,now,samplingEpoch,target)){
        receipt.result=FreshPoseUncappedReceipt::StalePrediction;return retained();
    }
    receipt.requestedTarget=target;
    std::unique_lock<std::mutex> lock(pose_locate_mutex_,std::try_to_lock);
    if(!lock.owns_lock()){receipt.result=FreshPoseUncappedReceipt::PoseBusy;return retained();}
    if(!current()){receipt.result=FreshPoseUncappedReceipt::TrackingDisabled;return retained();}
    tracking59c6::Frame family{};
    family.epoch=samplingEpoch;family.refreshGenerationR94=generation;
    family.tick=std::uint64_t(GetTickCount64());family.ns=tracking59c6::NowNs();family.locateTime=target;
    // The head lookup is only the common-space reference for these two hands.
    // This function NEVER calls PublishHead, SampleHeadPoseAtTime, xrSyncActions,
    // or modifies the render/camera snapshot. The accepted view path is separate.
    XrSpaceLocation reference{XR_TYPE_SPACE_LOCATION};
    const auto result=xrLocateSpace(view_space_,local_space_,target,&reference);
    constexpr auto valid=XR_SPACE_LOCATION_POSITION_VALID_BIT|XR_SPACE_LOCATION_ORIENTATION_VALID_BIT;
    constexpr auto tracked=XR_SPACE_LOCATION_POSITION_TRACKED_BIT|XR_SPACE_LOCATION_ORIENTATION_TRACKED_BIT;
    const auto fill=[&](unsigned i,const XrPosef& p,XrSpaceLocationFlags flags){
        family.valid[i]=(flags&valid)==valid;family.tracked[i]=family.valid[i]&&(flags&tracked)==tracked;
        // OpenXR forbids reading a pose component whose valid bit is unset.
        if(family.valid[i])family.poses[i]={p.position.x,p.position.y,p.position.z,p.orientation.x,p.orientation.y,p.orientation.z,p.orientation.w};
    };
    fill(0,reference.pose,XR_SUCCEEDED(result)?reference.locationFlags:0);
    if(!current()){receipt.result=FreshPoseUncappedReceipt::TrackingDisabled;return retained();}
    XrPosef poses[2]{};XrSpaceLocationFlags flags[2]{};XrSpaceVelocity velocities[2]{};
    const auto extra=claude_motion_d69::ExtraNs();
    family.controllersSampled=input_.TryLocateGripPair(target,poses,flags,extra>0?velocities:nullptr);
    if(!family.controllersSampled){receipt.result=FreshPoseUncappedReceipt::GripBusy;return retained();}
    fill(1,poses[0],flags[0]);fill(2,poses[1],flags[1]);
    if(extra>0)ApplyExtraHandsD69(input_,family,velocities,extra);
    if(!current()){receipt.result=FreshPoseUncappedReceipt::TrackingDisabled;return retained();}
    tracking59c6::Frame completed{};
    receipt.published=tracking59c6::Publish(family,&completed);
    receipt.result=receipt.published?FreshPoseUncappedReceipt::Published:FreshPoseUncappedReceipt::PublicationRejected;
    if(receipt.published){
        receipt.sequence=completed.sequence;receipt.sourceNs=completed.ns;receipt.sourceTick=completed.tick;receipt.target=completed.locateTime;
        for(unsigned i=0;i<3;++i){receipt.validMask|=unsigned(completed.valid[i])<<i;receipt.trackedMask|=unsigned(completed.tracked[i])<<i;}
    }
    return retained();
}

bool XrManager::SampleHeadPoseAtTime(XrTime target_time, XrPose& out_pose, hj1::Sample* out_family,
                                    FreshPoseUncappedReceipt* out_refresh)
{
    if(out_family)*out_family={};
    if(out_refresh)out_refresh->result=FreshPoseUncappedReceipt::SessionUnavailable;
    const auto familyPolicyR24=out_family&&hj1::enabled.load(std::memory_order_acquire)?hand_smooth_h50::policyEpoch.load(std::memory_order_acquire):0;
    tracking59c6::Frame completedR24{};
    diagnostics::xr165::Timer timing165(diagnostics::xr165::SyncPose);
    if (!session_running_.load(std::memory_order_acquire) || target_time <= 0 ||
        view_space_ == XR_NULL_HANDLE || local_space_ == XR_NULL_HANDLE) return false;
    // Synchronous camera routes preserve their chosen predicted/NOW target.
    // Preserve its head sampling semantics. Pose-only controller reads use the
    // same target time, never xrSyncActions or a second button/haptic producer.
    std::unique_lock<std::mutex> lock(pose_locate_mutex_,std::defer_lock);
    if(out_refresh||motion_trials_d67::UncappedSelectedAt(0)){
        if(!lock.try_lock()){
            if(out_refresh)out_refresh->result=FreshPoseUncappedReceipt::PoseBusy;
            return false; // caller retains the completed snapshot with its actual age
        }
    }else lock.lock(); // Existing paced/baseline camera modes are unchanged.
    const auto samplingEpoch=tracking59c6::epoch.load(std::memory_order_acquire);
    if(out_refresh&&(!motion_trials_d67::UncappedSelectedAt(0)||
       tracking59c6::cameraSource.load(std::memory_order_acquire)!=out_refresh->source||
       !tracking59c6::enabled.load(std::memory_order_acquire))){
        out_refresh->result=FreshPoseUncappedReceipt::TrackingDisabled;return false;
    }
    tracking59c6::Frame family{};family.epoch=samplingEpoch;
    family.tick=std::uint64_t(GetTickCount64());family.ns=tracking59c6::NowNs();family.locateTime=target_time;
    XrSpaceLocation location{ XR_TYPE_SPACE_LOCATION };
    const auto result=xrLocateSpace(view_space_, local_space_, target_time, &location);
    const XrSpaceLocationFlags need=XR_SPACE_LOCATION_POSITION_VALID_BIT|XR_SPACE_LOCATION_ORIENTATION_VALID_BIT;
    const XrSpaceLocationFlags tracked=XR_SPACE_LOCATION_POSITION_TRACKED_BIT|XR_SPACE_LOCATION_ORIENTATION_TRACKED_BIT;
    auto fill=[&](unsigned i,const XrPosef& p,XrSpaceLocationFlags f){
        family.poses[i]={p.position.x,p.position.y,p.position.z,p.orientation.x,p.orientation.y,p.orientation.z,p.orientation.w};
        family.valid[i]=(f&need)==need;family.tracked[i]=(f&tracked)==tracked;
    };
    fill(0,location.pose,XR_SUCCEEDED(result)?location.locationFlags:0);
    if(tracking59c6::enabled.load(std::memory_order_acquire)&&tracking59c6::cameraSource.load(std::memory_order_acquire)!=0){
        XrPosef poses[2]{};XrSpaceLocationFlags flags[2]{};
        const auto extraD69=claude_motion_d69::ExtraNs();XrSpaceVelocity velocityD69[2]{};
        family.controllersSampled=input_.TryLocateGripPair(target_time,poses,flags,extraD69>0?velocityD69:nullptr);
        if(out_refresh&&!family.controllersSampled){
            out_refresh->result=FreshPoseUncappedReceipt::GripBusy;
            return false; // neither half of this attempted family becomes a new head/grip publication
        }
        fill(1,poses[0],flags[0]);fill(2,poses[1],flags[1]);
        if(extraD69>0)ApplyExtraHandsD69(input_,family,velocityD69,extraD69);
        if(out_refresh&&(!motion_trials_d67::UncappedSelectedAt(0)||
           tracking59c6::cameraSource.load(std::memory_order_acquire)!=out_refresh->source||
           family.epoch!=tracking59c6::epoch.load(std::memory_order_acquire))){
            out_refresh->result=FreshPoseUncappedReceipt::TrackingDisabled;return false;
        }
        const bool published=tracking59c6::Publish(family,(familyPolicyR24||out_refresh)?&completedR24:nullptr);
        if(out_refresh){
            out_refresh->published=published;
            out_refresh->result=published?FreshPoseUncappedReceipt::Published:FreshPoseUncappedReceipt::PublicationRejected;
            if(published){
                out_refresh->sequence=completedR24.sequence;out_refresh->sourceNs=completedR24.ns;
                out_refresh->sourceTick=completedR24.tick;out_refresh->target=completedR24.locateTime;
                for(unsigned i=0;i<3;++i){out_refresh->validMask|=unsigned(completedR24.valid[i])<<i;out_refresh->trackedMask|=unsigned(completedR24.tracked[i])<<i;}
            }else return false;
        }
    }
    if(out_refresh&&!out_refresh->published){
        out_refresh->result=FreshPoseUncappedReceipt::TrackingDisabled;return false;
    }
    tracking59c6::PublishHead(family);
    if(XR_FAILED(result)||(location.locationFlags&need)!=need)return false;
    out_pose.pos_x=location.pose.position.x;out_pose.pos_y=location.pose.position.y;out_pose.pos_z=location.pose.position.z;
    out_pose.qx=location.pose.orientation.x;out_pose.qy=location.pose.orientation.y;out_pose.qz=location.pose.orientation.z;out_pose.qw=location.pose.orientation.w;
    if(familyPolicyR24&&hj1::enabled.load(std::memory_order_acquire)&&familyPolicyR24==hand_smooth_h50::policyEpoch.load(std::memory_order_acquire)&&
       completedR24.epoch==tracking59c6::epoch.load(std::memory_order_acquire))
        *out_family=tracking59c6::CompletedCameraFamilyR24(completedR24,familyPolicyR24);
    return true;
}

bool XrManager::LocateFamilyForGameFrameHJ1(XrPosef (&poses)[3], XrSpaceLocationFlags (&flags)[3], XrTime& target, int& ahead, XrTime& now, hj1::Sample* extraMetadataD69, diagnostics::motion_d68::Event* predictionTraceD69)
{
    flags[0]=flags[1]=flags[2]=0;target=0;ahead=0;now=0;
    if (!session_running_.load(std::memory_order_acquire) || !qpc_time_extension_enabled_ || !qpc_to_xr_time_) return false;
    // mode 0 = the camera takes the XR thread's published head, mode 1 = it locates at the latest predicted time.
    // mode 2 (NOW) / 3 (offset) already sample their own family on the camera route: not touched.
    const int mode=CameraHook::GetPoseSourceMode();
    if (mode!=0 && mode!=1) return false;
    const std::int64_t base=mode==0?hj1_head_target_.load(std::memory_order_acquire):latest_predicted_display_time_.load(std::memory_order_acquire);
    const std::int64_t at=mode==0?hj1_head_now_.load(std::memory_order_acquire):hj1_predicted_now_.load(std::memory_order_acquire);
    const std::int64_t period=hj1_period_ns_.load(std::memory_order_acquire);
    if (base<=0 || at<=0 || period<=0) return false;
    LARGE_INTEGER qpc{}; if (!QueryPerformanceCounter(&qpc)) return false;
    if (XR_FAILED(qpc_to_xr_time_(instance_, &qpc, &now))) return false;
    const auto phase=static_cast<std::int64_t>(now)-at;
    if (phase<0 || phase>4*period) return false; // XR thread stalled/paused: no prediction, keep the family hands
    ahead=kcd2vr::hj1::Ahead(phase,kcd2vr::hj1::leadNs.load(std::memory_order_relaxed),period);
    target=static_cast<XrTime>(kcd2vr::hj1::Target(base,ahead,period));
    std::unique_lock<std::mutex> lock(pose_locate_mutex_,std::try_to_lock);
    if (!lock.owns_lock()) return false;
    if(view_space_==XR_NULL_HANDLE||local_space_==XR_NULL_HANDLE)return false;
    XrSpaceLocation head{XR_TYPE_SPACE_LOCATION};
    if(XR_FAILED(xrLocateSpace(view_space_,local_space_,target,&head)))return false;
    poses[0]=head.pose;flags[0]=head.locationFlags;
    XrPosef hands[2]{};XrSpaceLocationFlags handFlags[2]{};
    const auto extraD69=claude_motion_d69::ExtraNs();XrSpaceVelocity velocityD69[2]{};
    if(!input_.TryLocateGripPair(target,hands,handFlags,extraD69>0?velocityD69:nullptr))return false;
    for(unsigned i=0;i<2;++i){poses[i+1]=hands[i];flags[i+1]=handFlags[i];}
    if(extraD69>0){
        // Preserve base-sample freshness: optional extra runtime work below cannot renew its age.
        if(extraMetadataD69){extraMetadataD69->tick=std::uint64_t(GetTickCount64());extraMetadataD69->steadyNs=tracking59c6::NowNs();}
        tracking59c6::Frame family{};family.locateTime=target;
        constexpr auto valid=XR_SPACE_LOCATION_POSITION_VALID_BIT|XR_SPACE_LOCATION_ORIENTATION_VALID_BIT;
        constexpr auto tracked=XR_SPACE_LOCATION_POSITION_TRACKED_BIT|XR_SPACE_LOCATION_ORIENTATION_TRACKED_BIT;
        for(unsigned i=0;i<3;++i){const auto& p=poses[i];family.poses[i]={p.position.x,p.position.y,p.position.z,p.orientation.x,p.orientation.y,p.orientation.z,p.orientation.w};family.valid[i]=(flags[i]&valid)==valid;family.tracked[i]=(flags[i]&tracked)==tracked;}
        ApplyExtraHandsD69(input_,family,velocityD69,extraD69,predictionTraceD69);
        for(unsigned h=0;h<2;++h){const auto& p=family.poses[h+1];poses[h+1]={{p.qx,p.qy,p.qz,p.qw},{p.x,p.y,p.z}};
            if(extraMetadataD69){extraMetadataD69->handTargetD69[h]=family.handTargetD69[h];extraMetadataD69->handLeadSourceD69[h]=family.handLeadSourceD69[h];}}
        if(extraMetadataD69)extraMetadataD69->handPolicyD69=family.handPolicyD69;
    }
    return flags[0]&&(flags[1]|flags[2])!=0;
}

void XrManager::NoteCameraApplyHJ1(XrTime camera_target, std::uint64_t camera_frame)
{
    if (!kcd2vr::hj1::enabled.load(std::memory_order_acquire) || !qpc_time_extension_enabled_ || !qpc_to_xr_time_) return; // W3: lab off = H50
    const int mode=CameraHook::GetPoseSourceMode();
    if (mode!=0 && mode!=1) return;
    if (!camera_target) camera_target=static_cast<XrTime>(hj1_head_target_.load(std::memory_order_acquire));
    LARGE_INTEGER qpc{}; XrTime now=0;
    if (!QueryPerformanceCounter(&qpc) || XR_FAILED(qpc_to_xr_time_(instance_, &qpc, &now))) return;
    const std::int64_t period=hj1_period_ns_.load(std::memory_order_acquire);
    kcd2vr::hj1::NoteCamera(static_cast<std::int64_t>(camera_target),static_cast<std::int64_t>(now),period,camera_frame);
    // the receipt is the one SMOOTH_LAB line of diagnostics/xr_timing165.h (every 5 s while the lab is on).
}

bool XrManager::SampleHeadPoseNow(XrPose& out_pose, XrTime* out_time, hj1::Sample* out_family)
{
    if(out_family)*out_family={};
    if (!session_running_.load(std::memory_order_acquire) || !qpc_time_extension_enabled_ || !qpc_to_xr_time_) return false;
    LARGE_INTEGER qpc{}; if (!QueryPerformanceCounter(&qpc)) return false;
    XrTime now=0; if (XR_FAILED(qpc_to_xr_time_(instance_, &qpc, &now))) return false;
    if (out_time) *out_time=now;
    return SampleHeadPoseAtTime(now,out_pose,out_family);
}

bool XrManager::SampleLateHeadPose(XrPose& out_pose)
{
    diagnostics::xr165::Timer timing165(diagnostics::xr165::LatePose);
    if (!late_pose_enabled_.load(std::memory_order_acquire) ||
        !session_running_.load(std::memory_order_acquire) ||
        !qpc_time_extension_enabled_ || !qpc_to_xr_time_ ||
        view_space_ == XR_NULL_HANDLE || local_space_ == XR_NULL_HANDLE) return false;
    // Do not make a camera callback wait behind a duplicate locate. The cached
    // completed family is used at the caller, with its original sample age.
    std::unique_lock<std::mutex> lock(pose_locate_mutex_,std::try_to_lock);
    if(!lock.owns_lock())return false;
    tracking59c6::Frame frame{};frame.epoch=tracking59c6::epoch.load(std::memory_order_acquire);
    frame.tick=std::uint64_t(GetTickCount64());frame.ns=tracking59c6::NowNs();
    LARGE_INTEGER qpc{};if(!QueryPerformanceCounter(&qpc))return false;
    XrTime now=0;
    if(XR_FAILED(qpc_to_xr_time_(instance_,&qpc,&now)))return false;
    frame.locateTime=now;
    XrSpaceLocation head{XR_TYPE_SPACE_LOCATION};
    const auto headResult=xrLocateSpace(view_space_,local_space_,now,&head);
    XrPosef grips[2]{};XrSpaceLocationFlags flags[2]{};
    const auto extraD69=claude_motion_d69::ExtraNs();XrSpaceVelocity velocityD69[2]{};
    frame.controllersSampled=input_.TryLocateGripPair(now,grips,flags,extraD69>0?velocityD69:nullptr);
    const XrSpaceLocationFlags valid=XR_SPACE_LOCATION_POSITION_VALID_BIT|XR_SPACE_LOCATION_ORIENTATION_VALID_BIT;
    const XrSpaceLocationFlags tracked=XR_SPACE_LOCATION_POSITION_TRACKED_BIT|XR_SPACE_LOCATION_ORIENTATION_TRACKED_BIT;
    auto fill=[&](unsigned index,const XrPosef& pose,XrSpaceLocationFlags f){
        frame.poses[index]={pose.position.x,pose.position.y,pose.position.z,pose.orientation.x,pose.orientation.y,pose.orientation.z,pose.orientation.w};
        frame.valid[index]=(f&valid)==valid;frame.tracked[index]=(f&tracked)==tracked;
    };
    fill(0,head.pose,XR_SUCCEEDED(headResult)?head.locationFlags:0);
    fill(1,grips[0],flags[0]);fill(2,grips[1],flags[1]);
    if(extraD69>0&&tracking59c6::cameraNow.load(std::memory_order_acquire))ApplyExtraHandsD69(input_,frame,velocityD69,extraD69);
    tracking59c6::PublishHead(frame);
    // Late frustum comfort remains head-owned. Its NOW target must not replace
    // the predicted gameplay family selected by camera modes 0/1.
    if(tracking59c6::cameraNow.load(std::memory_order_acquire))(void)tracking59c6::Publish(frame);
    if(!frame.valid[0]||!tracking59c6::Valid(frame.poses[0])){
        late_pose_failures_.fetch_add(1,std::memory_order_relaxed);return false;
    }
    out_pose.pos_x=head.pose.position.x;out_pose.pos_y=head.pose.position.y;out_pose.pos_z=head.pose.position.z;
    out_pose.qx=head.pose.orientation.x;out_pose.qy=head.pose.orientation.y;out_pose.qz=head.pose.orientation.z;out_pose.qw=head.pose.orientation.w;
    late_pose_samples_.fetch_add(1,std::memory_order_relaxed);
    return true;
}


void XrManager::Shutdown()
{
    ResetUiAnchorsR105();
    hand_present_lead_d61_.reset();hand_present_was_enabled_d61_=false;
    comfort_envelope51_.Reset();
    comfort_context51_.store(0,std::memory_order_release);
    tracking59c6::Reset();
    {std::lock_guard<std::mutex> lock(hud_geometry_mutex_);hud_geometry_latch_.Reset();hud_geometry_={};++hud_geometry_epoch_;}

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
            TimedFenceWait165(fence_event_, INFINITE);
    }

    DestroyAfrCaches();
    DestroyBridgePresentCaches();
    DestroyStereoSubmitRing();
    if (v6589_packed_snapshot_) { v6589_packed_snapshot_->Release(); v6589_packed_snapshot_ = nullptr; }
    v6589_packed_desc_ = {}; v6589_packed_capture_enabled_.store(false,std::memory_order_release); v6590_ghost_ready_.store(false,std::memory_order_release);
    DestroySwapchains();

    if (stage_space_ != XR_NULL_HANDLE) { xrDestroySpace(stage_space_); stage_space_ = XR_NULL_HANDLE; }
    eye_height_floor_valid_.store(false);
    if (view_space_ != XR_NULL_HANDLE) { xrDestroySpace(view_space_); view_space_ = XR_NULL_HANDLE; }
    if (local_space_ != XR_NULL_HANDLE) { xrDestroySpace(local_space_); local_space_ = XR_NULL_HANDLE; }
    if (session_ != XR_NULL_HANDLE)
    {
        // R105: xrEndSession belongs to STOPPING (handled by the event pump).
        // Shutdown may destroy a session in any state after workers have stopped.
        xrDestroySession(session_);
        session_ = XR_NULL_HANDLE;
        pending_reference_changes125_.clear();
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
    stereo_submit_ring_disabled_=false;
    bridge_present_cache_camera_frame_=0;
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
    menu_ui_image_initialized_.clear();
    menu_ui_has_released_r111_ = false; menu_source_seen_r111_ = nullptr;
    hud_layer_v6664::XrShutdown();
    if(menu_ui_.swapchain != XR_NULL_HANDLE) { xrDestroySwapchain(menu_ui_.swapchain); menu_ui_.swapchain=XR_NULL_HANDLE; }
}

} // namespace kcd2vr
