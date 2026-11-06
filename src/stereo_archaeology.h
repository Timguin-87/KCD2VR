#pragma once

#include "debug_trace.h"
#include "xr_manager.h"
#include <DetourModKit/hook.hpp>

#include <windows.h>
#include <intrin.h>
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstdarg>
#include <optional>
#include <utility>
#include <type_traits>
#include <array>
#include <cmath>
#include <cstring>

namespace kcd2vr::stereo_archaeology {
namespace detail {

// V65.35: wide archaeology hooks are process-lifetime singletons. F8 only arms
// collection/snapshots; it never installs or tears down these detours. This
// eliminates self-stacking and stale-trampoline crashes across repeated F8 cycles.
inline std::atomic<bool> g_wide_hooks_installed{false};
inline std::atomic<bool> g_wide_collection_armed{false};


constexpr std::uintptr_t kViewSystemUpdateRva       = 0x7F0220ull;
constexpr std::uintptr_t kActiveCameraHandoffRva    = 0x7F0AECull;
constexpr std::uintptr_t kEntityCameraApplyRva      = 0x7F0B88ull;
constexpr std::uintptr_t kEntityCameraFallbackRva   = 0x7F0DF4ull;
constexpr std::uintptr_t kSystemSetViewCameraRva    = 0x7F0AE0ull; // CSystem vtable +0x430 thunk; unique ViewSystem publish path
constexpr std::uintptr_t kSystemGetViewCameraRva    = 0x5D77F0ull; // CSystem vtable +0x438; returns this+0x288
constexpr std::uintptr_t kCViewCtorRva              = 0xB4BEFCull;
constexpr std::uintptr_t kCViewVtableRva            = 0x3A7A048ull;
constexpr std::uintptr_t kCViewSystemVtableRva      = 0x405D6C8ull;
constexpr std::uintptr_t kCEntityVtableRva          = 0x4072C78ull;
constexpr std::uintptr_t kCSystemPrimaryVtableRva   = 0x3DC2858ull;

constexpr std::uintptr_t kCRenderViewVtableRva      = 0x3DF25C0ull;
constexpr std::uintptr_t kCRenderViewCtorRva        = 0xBBCFF0ull;
constexpr std::uintptr_t kCRenderViewFactoryRva     = 0x1DED8A0ull;
constexpr std::uintptr_t kCRenderViewManagerCheckoutRva = 0x16F78F4ull;
constexpr std::uintptr_t kCRenderViewCameraBuildRva = 0x865550ull;
constexpr std::uintptr_t kCRenderViewCameraPairBuildRva = 0x866F6Cull;
constexpr std::uintptr_t kRendererConsumeViewRecordsRva = 0x653940ull; // downstream consumer of CRenderView+0x9B30 records
constexpr std::uintptr_t kGraphicsPipelineExecuteRva = 0x57278Cull; // CGraphicsPipeline vtable +0x28; dispatches active stages
constexpr std::uintptr_t kSceneGBufferStageExecuteRva = 0x7BB6B0ull; // CSceneGBufferStage vtable +0x20
constexpr std::uintptr_t kGBufferMrtSetupRva = 0x7BC700ull; // configures four G-buffer target inputs + depth for a scene pass
constexpr float kNativeEyeDiagnosticOffset = 0.032f; // legacy fixed half-IPD reference only
constexpr float kMinOpenXrIpdMeters = 0.040f;
constexpr float kMaxOpenXrIpdMeters = 0.090f;

// SAFETY GATE added during review: the V10 write (ShiftEmbeddedCameraRight/
// RestoreEmbeddedCameraTranslation) was previously unconditional once target
// correlation locked on - no independent opt-in, unlike every other risk
// category in this project (DiagnosticRendering=false gates GPU submission).
// This applies the same philosophy: correlation/targeting logging still runs
// unconditionally (safe, matches V3-V9's proven pattern), but the actual
// memory write is now behind its own explicit flag, defaulting OFF, so the
// first test can validate WHICH object gets targeted and WHICH offsets get
// selected via logging alone, before a separate deliberate test ever
// executes the write. Set via KCD2VR.ini [Settings] EnableNativeEyeWrite.
inline std::atomic<bool> g_native_eye_write_enabled{false};
constexpr std::uintptr_t kRendererWorldOrchestratorRva = 0x4E6924ull;
constexpr std::uintptr_t kRenderPassViewQueryRva = 0x4E7E64ull;

using ViewSystemUpdateFunc     = void(__fastcall*)(void*, float);
using ActiveCameraHandoffFunc  = void(__fastcall*)(void*, void*);
using EntityCameraApplyFunc    = void(__fastcall*)(void*, void*, std::uint32_t);
using EntityCameraFallbackFunc = void(__fastcall*)(void*, void*, std::uint32_t);
using SystemSetViewCameraFunc  = void(__fastcall*)(void*, void*);
using SystemGetViewCameraFunc  = void*(__fastcall*)(void*);
using CViewCtorFunc            = void*(__fastcall*)(void*, void*);

using CRenderViewCtorFunc    = void*(__fastcall*)(void*, void*, std::uint32_t);
using CRenderViewFactoryFunc = void*(__fastcall*)(void*);
using CRenderViewManagerCheckoutFunc = void*(__fastcall*)(void*);
using RendererWorldOrchestratorFunc = void(__fastcall*)(void*, void*, void*, void*);
using RenderPassViewQueryFunc = int(__fastcall*)(void*);

inline ViewSystemUpdateFunc g_vs_original = nullptr;
inline ActiveCameraHandoffFunc g_handoff_original = nullptr;
inline EntityCameraApplyFunc g_entity_apply_original = nullptr;
inline EntityCameraFallbackFunc g_entity_fallback_original = nullptr;
inline SystemSetViewCameraFunc g_set_view_camera_original = nullptr;
inline SystemGetViewCameraFunc g_get_view_camera_original = nullptr;
inline CViewCtorFunc g_cview_ctor_original = nullptr;

inline CRenderViewCtorFunc g_crv_ctor_original=nullptr;
inline CRenderViewFactoryFunc g_crv_factory_original=nullptr;
inline CRenderViewManagerCheckoutFunc g_crv_checkout_original=nullptr;
inline RendererWorldOrchestratorFunc g_world_orchestrator_original=nullptr;
inline RenderPassViewQueryFunc g_renderpass_query_original=nullptr;
inline std::optional<DetourModKit::hook::Hook> g_crv_ctor_hook;
inline std::optional<DetourModKit::hook::Hook> g_crv_factory_hook;
inline std::optional<DetourModKit::hook::Hook> g_crv_checkout_hook;
inline std::optional<DetourModKit::hook::Hook> g_world_orchestrator_hook;
inline std::optional<DetourModKit::hook::Hook> g_renderpass_query_hook;
inline std::atomic<std::uint64_t> g_crv_ctor_calls{0};
inline std::atomic<std::uint64_t> g_crv_factory_calls{0};
inline std::atomic<std::uint64_t> g_crv_checkout_calls{0};
inline std::atomic<std::uint64_t> g_world_orchestrator_calls{0};
inline std::atomic<std::uint64_t> g_renderpass_query_calls{0};
inline std::atomic<std::uintptr_t> g_native_eye_target{0};
inline std::atomic<std::uintptr_t> g_native_eye_source{0};
inline std::atomic<std::uintptr_t> g_native_eye_source_b{0};
inline std::atomic<std::uint64_t> g_native_eye_build_calls{0};
inline std::atomic<std::uint64_t> g_native_eye_applied_calls{0};
using CRenderViewCameraBuildFunc = void(__fastcall*)(void* self);
inline CRenderViewCameraBuildFunc g_crv_camera_build_original=nullptr;
inline std::optional<DetourModKit::hook::Hook> g_crv_camera_build_hook;
inline std::atomic<std::uint64_t> g_v21_broad_calls{0};
inline std::atomic<std::uint64_t> g_v21_broad_applied{0};
inline std::atomic<std::uint64_t> g_v21_first_tick{0};

using CRenderViewCameraPairBuildFunc = void(__fastcall*)(void* out_state, void* cam_a, void* cam_b, void* context, float arg5, float arg6);
using RendererConsumeViewRecordsFunc = void(__fastcall*)(void* self, void* records, std::uint32_t count, void* context, std::uintptr_t arg5);
inline CRenderViewCameraPairBuildFunc g_crv_camera_pair_build_original=nullptr;
inline std::optional<DetourModKit::hook::Hook> g_crv_camera_pair_build_hook;
inline RendererConsumeViewRecordsFunc g_renderer_consume_records_original=nullptr;
inline std::optional<DetourModKit::hook::Hook> g_renderer_consume_records_hook;
inline std::atomic<std::uint64_t> g_renderer_consume_records_calls{0};

// V30: identify whether the two frequently consumed mode-0 CRenderViews are
// true simultaneous stereo contributors or renderer/frame-buffer alternates.
// This probe is read-only.  It snapshots the first 0x400 bytes of each
// CRenderView+0x9B30 record and reports 16-byte float blocks that differ when
// the two owners are observed close together on the renderer thread.
inline std::atomic<std::uintptr_t> g_v30_owner_a{0};
inline std::atomic<std::uintptr_t> g_v30_owner_b{0};
struct V30RecordSnapshot { std::uintptr_t owner{}; std::uint64_t call{}; bool valid{}; std::array<std::uint8_t,0x400> bytes{}; };
inline thread_local V30RecordSnapshot g_v30_snap_a{};
inline thread_local V30RecordSnapshot g_v30_snap_b{};
inline thread_local std::uint64_t g_v30_pair_reports{};

using GraphicsPipelineExecuteFunc = void(__fastcall*)(void* self);
using SceneGBufferStageExecuteFunc = void(__fastcall*)(void* self);
using GBufferMrtSetupFunc = void(__fastcall*)(void* pass, void* depth, void* r8_resource, void* r9_resource, void* stack5_resource, void* stack6_resource);
inline GraphicsPipelineExecuteFunc g_graphics_pipeline_execute_original=nullptr;
inline SceneGBufferStageExecuteFunc g_scene_gbuffer_execute_original=nullptr;
inline GBufferMrtSetupFunc g_gbuffer_mrt_setup_original=nullptr;
inline std::optional<DetourModKit::hook::Hook> g_graphics_pipeline_execute_hook;
inline std::optional<DetourModKit::hook::Hook> g_scene_gbuffer_execute_hook;
inline std::optional<DetourModKit::hook::Hook> g_gbuffer_mrt_setup_hook;
inline std::atomic<std::uint64_t> g_graphics_pipeline_execute_calls{0};
inline std::atomic<std::uint64_t> g_scene_gbuffer_execute_calls{0};
inline std::atomic<std::uint64_t> g_gbuffer_mrt_setup_calls{0};
inline thread_local void* g_tls_gbuffer_stage=nullptr;
inline thread_local void* g_tls_gbuffer_crv=nullptr;

// V31: observe the retained CryEngine game-stereo CVar registration path.
// Static WHGame analysis identifies g_stereoFrameworkEnable at RVA 0x3E0EB20,
// registered from 0x1594A90 through the integer-CVar helper at 0x23D8168.
constexpr std::uintptr_t kStereoFrameworkNameRva = 0x3E0EB20ull;
constexpr std::uintptr_t kStereoFrameworkHelpRva = 0x3E0EAD0ull;
constexpr std::uintptr_t kStereoFrameworkRegisterSiteRva = 0x1594A90ull;
constexpr std::uintptr_t kRegisterIntCVarRva = 0x23D8168ull;

using RegisterIntCVarFunc = void*(__fastcall*)(const char* name, void* storage, std::uint64_t default_value,
                                                std::uint64_t flags, const char* help, void* callback);
inline RegisterIntCVarFunc g_register_int_cvar_original = nullptr;
inline std::optional<DetourModKit::hook::Hook> g_register_int_cvar_hook;
inline std::atomic<std::uint64_t> g_register_int_cvar_calls{0};
inline std::atomic<std::uintptr_t> g_stereo_framework_storage{0};
inline std::atomic<std::uintptr_t> g_stereo_framework_result{0};


// V32: DLL-guided renderer-stereo CVar probe.  WHGame static analysis shows a
// compact renderer registration block at 0x10178xx-0x10179xx.  Instead of
// guessing which CRenderView is an eye, observe the actual renderer stereo
// controls and their live backing stores / CVar objects.
constexpr std::uintptr_t kRegisterFloatCVarRva = 0x23D8B0Cull;
constexpr std::uintptr_t kStereoStrengthNameRva = 0x3DE8A18ull;
constexpr std::uintptr_t kStereoEyeDistNameRva = 0x3DE8A70ull;
constexpr std::uintptr_t kStereoScreenDistNameRva = 0x3DE8AC8ull;
constexpr std::uintptr_t kStereoEnableMgpuNameRva = 0x3DE8D18ull;
constexpr std::uintptr_t kStereoFlipEyesNameRva = 0x3DE8DB0ull;
constexpr std::uintptr_t kStereoScaleCoefficientNameRva = 0x3DE89A0ull;
constexpr std::uintptr_t kStereoNearGeoScaleNameRva = 0x3DE9030ull;
constexpr std::uintptr_t kStereoHudScreenDistNameRva = 0x3DE90D8ull;
constexpr std::uintptr_t kStereoGammaAdjustmentNameRva = 0x3DE9128ull;

// V33: the renderer stereo registration block executes before our late ASI hook.
// These are the fixed backing globals recovered directly from WHGame.dll at
// WHGame+0x1017877..0x10179FC. Read-only sampling avoids registration timing.
constexpr std::uintptr_t kStereoScaleCoefficientStorageRva = 0x50DBDECull;
constexpr std::uintptr_t kStereoStrengthStorageRva = 0x50DBDE8ull;
constexpr std::uintptr_t kStereoEyeDistStorageRva = 0x50DBDCCull;
constexpr std::uintptr_t kStereoScreenDistStorageRva = 0x50DBDC8ull;
constexpr std::uintptr_t kStereoNearGeoScaleStorageRva = 0x50DBDC4ull;
constexpr std::uintptr_t kStereoHudScreenDistStorageRva = 0x50DBDC0ull;
constexpr std::uintptr_t kStereoGammaAdjustmentStorageRva = 0x50DBDD4ull;
constexpr std::uintptr_t kStereoEnableMgpuStorageRva = 0x5571030ull;
constexpr std::uintptr_t kStereoFlipEyesStorageRva = 0x5571034ull;

// V35: semantic identification of the V34 CSystem boolean candidate.
// V34 proved global WHGame+0x492D8C0 is CSystem and vslot +0xF8 resolves to
// WHGame+0x1A736D0, whose body is `movzx eax, byte ptr [rcx+0x598]; ret`.
// The byte stays false even while OpenXR has valid stereo views. Because this
// getter also has heavy non-stereo callers, V35 DOES NOT force it true. Instead
// it snapshots nearby CSystem status bytes and neighboring virtual targets so
// we can identify the semantic family before any causality write.
constexpr std::uintptr_t kStereoGateObjectGlobalRva = 0x492D8C0ull;
constexpr std::uintptr_t kStereoGateVslot = 0xF8ull;

// V36 multi-theory sweep. Observer-only fallbacks intended to eliminate several
// stereo hypotheses in a single run rather than advancing one branch at a time.
constexpr std::uintptr_t kVolCloudStereoReprojectionBackingRva = 0x50DBCD8ull;
constexpr std::uintptr_t kVolCloudLeftEyeStringRva = 0x3E01CE8ull;
constexpr std::uintptr_t kVolCloudLeftEyeRegisterSiteRva = 0x742413ull;
constexpr std::uintptr_t kVolCloudLeftEyeHandleStoreSiteRva = 0x742438ull;
constexpr std::uintptr_t kEngineGlobalsSweepBeginRva = 0x492D780ull;
constexpr std::uintptr_t kEngineGlobalsSweepEndRva = 0x492D980ull; // V61.17: extend through renderer/UI globals; read-only RTTI discovery
constexpr std::size_t kPipelineStateSweepBytes = 0x2000;
inline std::array<unsigned char,kPipelineStateSweepBytes> g_v36_prev_pipeline_pre{};
inline std::atomic<bool> g_v36_prev_pipeline_valid{false};

using StereoGateCandidateFunc = bool(__fastcall*)(void* self);
inline StereoGateCandidateFunc g_stereo_gate_candidate_original=nullptr;
inline std::optional<DetourModKit::hook::Hook> g_stereo_gate_candidate_hook;
inline std::atomic<std::uint64_t> g_stereo_gate_candidate_calls{0};
inline std::atomic<std::uint64_t> g_stereo_gate_candidate_true{0};
inline std::atomic<std::uint64_t> g_stereo_gate_candidate_false{0};

struct StereoRendererCVarSlot {
    const char* label{};
    std::uintptr_t name_rva{};
    std::atomic<std::uintptr_t> storage{0};
    std::atomic<std::uintptr_t> object{0};
    std::atomic<std::uint64_t> registrations{0};
};
inline StereoRendererCVarSlot g_v32_float_cvars[] = {
    {"r_stereoScaleCoefficient",kStereoScaleCoefficientNameRva},
    {"r_StereoStrength",kStereoStrengthNameRva},
    {"r_StereoEyeDist",kStereoEyeDistNameRva},
    {"r_StereoScreenDist",kStereoScreenDistNameRva},
    {"r_StereoNearGeoScale",kStereoNearGeoScaleNameRva},
    {"r_StereoHudScreenDist",kStereoHudScreenDistNameRva},
    {"r_StereoGammaAdjustment",kStereoGammaAdjustmentNameRva},
};
inline StereoRendererCVarSlot g_v32_int_cvars[] = {
    {"r_StereoEnableMgpu",kStereoEnableMgpuNameRva},
    {"r_StereoFlipEyes",kStereoFlipEyesNameRva},
};

inline void Tracef(const char* fmt, ...) noexcept;
inline std::uintptr_t WhBase() noexcept;
inline std::uintptr_t CallerRva() noexcept;
inline std::uintptr_t ObjectVtableRva(void* p) noexcept;
inline std::uint32_t SafeReadU32At(void* p,std::uintptr_t off) noexcept;

using RegisterFloatCVarFunc = void*(__fastcall*)(const char* name, void* storage, float default_value,
                                                  std::uint64_t flags, const char* help, void* callback);
inline RegisterFloatCVarFunc g_register_float_cvar_original = nullptr;
inline std::optional<DetourModKit::hook::Hook> g_register_float_cvar_hook;
inline std::atomic<std::uint64_t> g_register_float_cvar_calls{0};


inline float SafeReadFloat(void* p) noexcept {
    float v=0.0f; __try { if(p) v=*reinterpret_cast<volatile float*>(p); } __except(EXCEPTION_EXECUTE_HANDLER) {} return v;
}
inline std::int32_t SafeReadI32(void* p) noexcept {
    std::int32_t v=0; __try { if(p) v=*reinterpret_cast<volatile std::int32_t*>(p); } __except(EXCEPTION_EXECUTE_HANDLER) {} return v;
}


inline void TraceV32RendererStereoState(std::uint64_t pipeline_call) noexcept {
    if(pipeline_call>24 && pipeline_call%600!=0) return;
    const auto base=WhBase(); if(!base) return;
    struct F { const char* n; std::uintptr_t r; };
    static constexpr F fs[] = {
        {"r_stereoScaleCoefficient",kStereoScaleCoefficientStorageRva},
        {"r_StereoStrength",kStereoStrengthStorageRva},
        {"r_StereoEyeDist",kStereoEyeDistStorageRva},
        {"r_StereoScreenDist",kStereoScreenDistStorageRva},
        {"r_StereoNearGeoScale",kStereoNearGeoScaleStorageRva},
        {"r_StereoHudScreenDist",kStereoHudScreenDistStorageRva},
        {"r_StereoGammaAdjustment",kStereoGammaAdjustmentStorageRva},
    };
    for(const auto& f:fs) {
        auto* ptr=reinterpret_cast<void*>(base+f.r);
        Tracef("STEREO-RENDERER-V33 direct call=%llu type=float name='%s' rva=0x%llX storage=%p value=%.9f bits=%08X",
            (unsigned long long)pipeline_call,f.n,(unsigned long long)f.r,ptr,(double)SafeReadFloat(ptr),SafeReadU32At(ptr,0));
    }
    struct I { const char* n; std::uintptr_t r; };
    static constexpr I is[] = {
        {"r_StereoEnableMgpu",kStereoEnableMgpuStorageRva},
        {"r_StereoFlipEyes",kStereoFlipEyesStorageRva},
    };
    for(const auto& i:is) {
        auto* ptr=reinterpret_cast<void*>(base+i.r);
        Tracef("STEREO-RENDERER-V33 direct call=%llu type=int name='%s' rva=0x%llX storage=%p value=%d bits=%08X",
            (unsigned long long)pipeline_call,i.n,(unsigned long long)i.r,ptr,(int)SafeReadI32(ptr),SafeReadU32At(ptr,0));
    }
}
struct TrackedCRenderView {
    std::atomic<std::uintptr_t> ptr{0};
    std::atomic<std::uintptr_t> manager{0};
    std::atomic<std::uintptr_t> checkout_caller{0};
    std::atomic<std::uint32_t> mode{0};
};
inline TrackedCRenderView g_tracked_crv[96]{};
inline std::atomic<std::uintptr_t> g_seen_render_objects[96]{};







inline std::optional<DetourModKit::hook::Hook> g_vs_hook;
inline std::optional<DetourModKit::hook::Hook> g_handoff_hook;
inline std::optional<DetourModKit::hook::Hook> g_entity_apply_hook;
inline std::optional<DetourModKit::hook::Hook> g_entity_fallback_hook;
inline std::optional<DetourModKit::hook::Hook> g_set_view_camera_hook;
inline std::optional<DetourModKit::hook::Hook> g_get_view_camera_hook;
inline std::optional<DetourModKit::hook::Hook> g_cview_ctor_hook;

inline std::atomic<std::uint64_t> g_vs_calls{0}, g_handoff_calls{0}, g_entity_apply_calls{0},
    g_entity_fallback_calls{0}, g_set_view_camera_calls{0}, g_get_view_camera_calls{0}, g_cview_ctor_calls{0};

struct CallerBucket {
    std::atomic<std::uintptr_t> rva{0};
    std::atomic<std::uint64_t> count{0};
    std::atomic<bool> stack_logged{false};
};
inline CallerBucket g_vs_callers[16]{}, g_handoff_callers[16]{}, g_entity_apply_callers[16]{},
    g_entity_fallback_callers[16]{}, g_set_view_camera_callers[16]{}, g_get_view_camera_callers[32]{}, g_cview_ctor_callers[16]{};

inline void Tracef(const char* fmt, ...) noexcept {
    char buf[1400]{}; va_list ap; va_start(ap,fmt); std::vsnprintf(buf,sizeof(buf),fmt,ap); va_end(ap); debug_trace::Write(buf);
}
inline std::uintptr_t WhBase() noexcept { return reinterpret_cast<std::uintptr_t>(GetModuleHandleA("WHGame.dll")); }
inline std::uintptr_t ToRva(std::uintptr_t p) noexcept { const auto b=WhBase(); return (b && p>=b && p<b+0x08000000ull)?p-b:0; }
inline std::uintptr_t CallerRva() noexcept { return ToRva(reinterpret_cast<std::uintptr_t>(_ReturnAddress())); }
inline std::uintptr_t ObjectVtableRva(void* p) noexcept { std::uintptr_t r=0; __try { if(p) r=ToRva(*reinterpret_cast<std::uintptr_t*>(p)); } __except(EXCEPTION_EXECUTE_HANDLER){} return r; }

inline const char* ObjectRttiName(void* p, char* out, std::size_t out_size) noexcept {
    if(!out || out_size < 2) return "";
    out[0]=0;
    __try {
        if(!p) return out;
        auto vt=*reinterpret_cast<std::uintptr_t*>(p);
        if(!vt) return out;
        auto col=*reinterpret_cast<std::uintptr_t*>(vt-sizeof(void*));
        if(!col) return out;
        // MSVC x64 CompleteObjectLocator uses image-relative TypeDescriptor RVA at +0x0C.
        auto type_rva=*reinterpret_cast<std::uint32_t*>(col+0x0C);
        auto base=WhBase();
        if(!base || !type_rva) return out;
        auto name=reinterpret_cast<const char*>(base+type_rva+0x10);
        std::snprintf(out,out_size,"%.*s",(int)out_size-1,name);
    } __except(EXCEPTION_EXECUTE_HANDLER) { out[0]=0; }
    return out;
}





inline const char* ConsumerLabel(std::uintptr_t r) noexcept {
    if(r>=0x5DFCE4ull && r<0x5DFD4Bull) return "projection-scale";
    if(r>=0x6678C4ull && r<0x668D45ull) return "frame-orchestrator-A";
    if(r>=0x669C88ull && r<0x66A029ull) return "frame-orchestrator-B";
    if(r>=0xD85224ull && r<0xD855B4ull) return "camera-relative-worker";
    if(r>=0x52EF84ull && r<0x52F258ull) return "consumer-52EF84";
    if(r>=0x534F60ull && r<0x534FE1ull) return "consumer-534F60";
    if(r>=0x552D78ull && r<0x552F99ull) return "consumer-552D78";
    if(r>=0xFFCFC8ull && r<0xFFD389ull) return "consumer-FFCFC8";
    if(r>=0x3D44C8ull && r<0x3D45E0ull) return "consumer-3D44C8";
    if(r>=0x5D744Cull && r<0x5D77E1ull) return "consumer-5D744C";
    return "unclassified";
}



using C3DEngineCameraFunc = void(__fastcall*)(void* self, void* camera, void* out_state);
inline C3DEngineCameraFunc g_c3dengine_camera_original=nullptr;
inline std::optional<DetourModKit::hook::Hook> g_c3dengine_camera_hook;
inline std::atomic<std::uint64_t> g_c3dengine_camera_calls{0};
inline std::atomic<std::uintptr_t> g_last_published_view_camera{0};



inline std::uintptr_t SafeReadPtrAt(void* p,std::uintptr_t off) noexcept {
    std::uintptr_t v=0;
    __try { if(p) v=*reinterpret_cast<std::uintptr_t*>(reinterpret_cast<std::uintptr_t>(p)+off); }
    __except(EXCEPTION_EXECUTE_HANDLER) {}
    return v;
}
inline std::uint32_t SafeReadU32At(void* p,std::uintptr_t off) noexcept {
    std::uint32_t v=0;
    __try { if(p) v=*reinterpret_cast<std::uint32_t*>(reinterpret_cast<std::uintptr_t>(p)+off); }
    __except(EXCEPTION_EXECUTE_HANDLER) {}
    return v;
}


inline void SafeReadBytesAt(void* p,std::uintptr_t off,void* out,std::size_t size) noexcept {
    if(!out || !size) return;
    std::memset(out,0,size);
    __try { if(p) std::memcpy(out,reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(p)+off),size); }
    __except(EXCEPTION_EXECUTE_HANDLER) { std::memset(out,0,size); }
}





struct DeferredCrvSnapshot {
    void* crv{};
    std::uint64_t checkout_call{};
    std::uintptr_t checkout_caller{};
    std::uint32_t mode{};
    std::uint32_t route{};
};
inline thread_local DeferredCrvSnapshot g_deferred_crv_snapshot{};





struct CameraTranslationBackup {
    bool valid{};
    float x{},y{},z{};
};


struct OpenXrStereoInjection {
    bool valid{};
    float ipd{};
    float left_offset{};
    float right_offset{};
    XrTime predicted_display_time{};
    std::uint64_t age_ms{};
    const char* source = "none";
    std::uint32_t view_count{};
    XrViewStateFlags view_state_flags{};
    XrResult locate_result{XR_ERROR_RUNTIME_FAILURE};
};










inline std::uint32_t TrackedModeFor(void* p) noexcept {
    const auto pv=reinterpret_cast<std::uintptr_t>(p);
    if(!pv) return 0xFFFFFFFFu;
    for(auto& e:g_tracked_crv) if(e.ptr.load(std::memory_order_acquire)==pv) return e.mode.load(std::memory_order_acquire);
    return 0xFFFFFFFFu;
}


inline void TraceV36VolCloudState(std::uint64_t call) noexcept {
    if(call>12 && call%600!=0) return;
    const auto base=WhBase(); if(!base) return;
    const auto v=SafeReadI32(reinterpret_cast<void*>(base+kVolCloudStereoReprojectionBackingRva));
    Tracef("VOLCLOUD-STEREO-V36 call=%llu reprojection_rva=0x%llX value=%d leftEyeString_rva=0x%llX register_site=0x%llX handle_store_site=0x%llX expected_handle_object_off=0xA0",
        (unsigned long long)call,(unsigned long long)kVolCloudStereoReprojectionBackingRva,(int)v,
        (unsigned long long)kVolCloudLeftEyeStringRva,(unsigned long long)kVolCloudLeftEyeRegisterSiteRva,
        (unsigned long long)kVolCloudLeftEyeHandleStoreSiteRva);
}

inline void TraceV36EngineGlobalObjectsOnce() noexcept {
    static std::atomic<bool> done{false}; bool expected=false;
    if(!done.compare_exchange_strong(expected,true,std::memory_order_relaxed)) return;
    const auto base=WhBase(); if(!base) return;
    std::uint32_t found=0;
    for(std::uintptr_t rva=kEngineGlobalsSweepBeginRva; rva<kEngineGlobalsSweepEndRva; rva+=8) {
        const auto p=SafeReadPtrAt(reinterpret_cast<void*>(base),rva);
        if(!p) continue;
        char rtti[160]{}; ObjectRttiName(reinterpret_cast<void*>(p),rtti,sizeof(rtti));
        if(!rtti[0]) continue;
        Tracef("ENGINE-GLOBAL-SWEEP-V36 global_rva=0x%llX object=%p vt=0x%llX rtti='%s'",
            (unsigned long long)rva,(void*)p,(unsigned long long)ObjectVtableRva((void*)p),rtti);
        if (std::strstr(rtti, "CFlashUI") != nullptr || std::strstr(rtti, "FlashUI") != nullptr) {
            Tracef("V61.17-HUD-FLASH-DISCOVERY candidate global_rva=0x%llX object=%p vt=0x%llX rtti='%s'",
                (unsigned long long)rva,(void*)p,(unsigned long long)ObjectVtableRva((void*)p),rtti);
        }
        if(++found>=48) break;
    }
    Tracef("ENGINE-GLOBAL-SWEEP-V36 summary found=%u range=0x%llX..0x%llX",
        found,(unsigned long long)kEngineGlobalsSweepBeginRva,(unsigned long long)kEngineGlobalsSweepEndRva);
}

inline void TraceV36PipelineObjectPointersOnce(void* self) noexcept {
    static std::atomic<bool> done{false}; bool expected=false;
    if(!done.compare_exchange_strong(expected,true,std::memory_order_relaxed)) return;
    if(!self) return;
    std::uint32_t found=0;
    for(std::uintptr_t off=0; off<0x800; off+=8) {
        const auto p=SafeReadPtrAt(self,off);
        if(!p) continue;
        char rtti[160]{}; ObjectRttiName(reinterpret_cast<void*>(p),rtti,sizeof(rtti));
        if(!rtti[0]) continue;
        Tracef("PIPELINE-OBJECT-SWEEP-V36 off=0x%llX object=%p vt=0x%llX rtti='%s'",
            (unsigned long long)off,(void*)p,(unsigned long long)ObjectVtableRva((void*)p),rtti);
        if(++found>=48) break;
    }
    Tracef("PIPELINE-OBJECT-SWEEP-V36 summary pipeline=%p found=%u scanned=0x800",self,found);
}

inline void TraceV36SmallDiff(const char* kind, std::uint64_t call, const unsigned char* a, const unsigned char* b, std::size_t n) noexcept {
    if(!a || !b) return;
    std::uint32_t emitted=0,total=0;
    for(std::size_t i=0;i<n;++i) {
        if(a[i]==b[i]) continue;
        if(a[i]<=3 || b[i]<=3) {
            ++total;
            if(emitted<48) {
                Tracef("PIPELINE-STATE-V36 kind=%s call=%llu off=0x%llX before=%u after=%u",
                    kind,(unsigned long long)call,(unsigned long long)i,(unsigned)a[i],(unsigned)b[i]);
                ++emitted;
            }
        }
    }
    if(total) Tracef("PIPELINE-STATE-V36 kind=%s call=%llu candidate_small_changes=%u emitted=%u",
        kind,(unsigned long long)call,total,emitted);
}

inline void CaptureV36PipelineState(void* self, unsigned char* dst) noexcept {
    if(!dst) return;
    std::memset(dst,0,kPipelineStateSweepBytes);
    SafeReadBytesAt(self,0,dst,kPipelineStateSweepBytes);
}

inline void __fastcall GraphicsPipelineExecuteDetour(void* self) noexcept {
    const auto n=g_graphics_pipeline_execute_calls.fetch_add(1,std::memory_order_relaxed)+1;
    unsigned char v36_pre[kPipelineStateSweepBytes]{};
    unsigned char v36_post[kPipelineStateSweepBytes]{};
    const bool v36_capture=(n<=16);
    if(v36_capture) {
        CaptureV36PipelineState(self,v36_pre);
        if(g_v36_prev_pipeline_valid.load(std::memory_order_acquire))
            TraceV36SmallDiff("inter-pre",n,g_v36_prev_pipeline_pre.data(),v36_pre,kPipelineStateSweepBytes);
        std::memcpy(g_v36_prev_pipeline_pre.data(),v36_pre,kPipelineStateSweepBytes);
        g_v36_prev_pipeline_valid.store(true,std::memory_order_release);
    }
    TraceV36EngineGlobalObjectsOnce();
    TraceV36PipelineObjectPointersOnce(self);
    TraceV36VolCloudState(n);
    void* crv=(void*)SafeReadPtrAt(self,0x1978);
    if(n<=12 || n%600==0) {
        char rtti[128]{}; ObjectRttiName(self,rtti,sizeof(rtti));
        Tracef("PIPELINE-V29 dispatch call=%llu pipeline=%p vt=0x%llX rtti='%s' crv=%p crv_vt=0x%llX mode=%u source=%p count=%u tid=%lu",
            (unsigned long long)n,self,(unsigned long long)ObjectVtableRva(self),rtti,crv,
            (unsigned long long)ObjectVtableRva(crv),TrackedModeFor(crv),(void*)SafeReadPtrAt(crv,0x38),SafeReadU32At(crv,0xA330),(unsigned long)GetCurrentThreadId());
    }
    TraceV32RendererStereoState(n);
    if(g_graphics_pipeline_execute_original) g_graphics_pipeline_execute_original(self);
    if(v36_capture) {
        CaptureV36PipelineState(self,v36_post);
        TraceV36SmallDiff("intra-dispatch",n,v36_pre,v36_post,kPipelineStateSweepBytes);
    }
}

inline void __fastcall SceneGBufferStageExecuteDetour(void* self) noexcept {
    const auto n=g_scene_gbuffer_execute_calls.fetch_add(1,std::memory_order_relaxed)+1;
    void* crv=(void*)SafeReadPtrAt(self,0x28);
    void* prev_stage=g_tls_gbuffer_stage; void* prev_crv=g_tls_gbuffer_crv;
    g_tls_gbuffer_stage=self; g_tls_gbuffer_crv=crv;
    if(n<=24 || n%240==0) {
        char rtti[128]{}; ObjectRttiName(self,rtti,sizeof(rtti));
        Tracef("GBUFFER-V29 enter call=%llu stage=%p vt=0x%llX rtti='%s' crv=%p crv_vt=0x%llX mode=%u source=%p records=%p count=%u stage8=%p stage10=%p stage20=%p tid=%lu",
            (unsigned long long)n,self,(unsigned long long)ObjectVtableRva(self),rtti,crv,
            (unsigned long long)ObjectVtableRva(crv),TrackedModeFor(crv),(void*)SafeReadPtrAt(crv,0x38),
            crv?(void*)(reinterpret_cast<std::uintptr_t>(crv)+0x9B30):nullptr,SafeReadU32At(crv,0xA330),
            (void*)SafeReadPtrAt(self,0x8),(void*)SafeReadPtrAt(self,0x10),(void*)SafeReadPtrAt(self,0x20),(unsigned long)GetCurrentThreadId());
    }
    if(g_scene_gbuffer_execute_original) g_scene_gbuffer_execute_original(self);
    g_tls_gbuffer_stage=prev_stage; g_tls_gbuffer_crv=prev_crv;
}

inline void __fastcall GBufferMrtSetupDetour(void* pass, void* depth, void* r8_resource, void* r9_resource, void* stack5_resource, void* stack6_resource) noexcept {
    const auto n=g_gbuffer_mrt_setup_calls.fetch_add(1,std::memory_order_relaxed)+1;
    if(g_tls_gbuffer_stage && (n<=32 || n%240==0)) {
        char d[96]{},a[96]{},bb[96]{},c[96]{},e[96]{};
        ObjectRttiName(depth,d,sizeof(d)); ObjectRttiName(r8_resource,a,sizeof(a)); ObjectRttiName(r9_resource,bb,sizeof(bb));
        ObjectRttiName(stack5_resource,c,sizeof(c)); ObjectRttiName(stack6_resource,e,sizeof(e));
        Tracef("GBUFFER-MRT-V29 call=%llu stage=%p crv=%p mode=%u source=%p pass=%p depth=%p(vt=0x%llX,'%s') r8=%p(vt=0x%llX,'%s') r9=%p(vt=0x%llX,'%s') s5=%p(vt=0x%llX,'%s') s6=%p(vt=0x%llX,'%s') tid=%lu",
            (unsigned long long)n,g_tls_gbuffer_stage,g_tls_gbuffer_crv,TrackedModeFor(g_tls_gbuffer_crv),(void*)SafeReadPtrAt(g_tls_gbuffer_crv,0x38),pass,
            depth,(unsigned long long)ObjectVtableRva(depth),d,r8_resource,(unsigned long long)ObjectVtableRva(r8_resource),a,
            r9_resource,(unsigned long long)ObjectVtableRva(r9_resource),bb,stack5_resource,(unsigned long long)ObjectVtableRva(stack5_resource),c,
            stack6_resource,(unsigned long long)ObjectVtableRva(stack6_resource),e,(unsigned long)GetCurrentThreadId());
    }
    if(g_gbuffer_mrt_setup_original) g_gbuffer_mrt_setup_original(pass,depth,r8_resource,r9_resource,stack5_resource,stack6_resource);
}
















template<class Fn,class Det>
inline bool Install(const char* name,std::uintptr_t rva,Det det,std::optional<DetourModKit::hook::Hook>& hook,Fn& original) noexcept {
    auto base=WhBase(); if(!base)return false; DetourModKit::hook::InlineRequest req{name,DetourModKit::hook::Target{DetourModKit::Address{base+rva}},DetourModKit::hook::Options{}}; auto result=DetourModKit::hook::inline_at(std::move(req),det); if(!result)return false; hook=std::move(*result); original=hook->template original<Fn>(); auto e=hook->enable(); if(!e){original=nullptr;hook.reset();return false;} return true;
}
inline void Dump(const char* tag,CallerBucket* b,std::size_t n) noexcept { for(std::size_t i=0;i<n;++i){auto r=b[i].rva.load();if(r)Tracef("MULTIHOP %s caller-summary rva=0x%llX label=%s count=%llu",tag,(unsigned long long)r,ConsumerLabel(r),(unsigned long long)b[i].count.load());} }

} // detail

// V61.18: HUD discovery must not depend on enabling the heavy stereo observer suite.
// Safe, read-only one-shot RTTI scan of the known engine-global pointer range.
inline void DiscoverHudFlashUiOnce() noexcept { detail::TraceV36EngineGlobalObjectsOnce(); }

// V61.20: keep the stereo chase alive without touching hot render/game paths.
// V61.19 proved that the CSystem gate is called millions of times and that adding
// a second detour at CStandardGraphicsPipeline+0x57278C can perturb VR timing.
// We already captured the useful alternating CRenderView / LeftEye evidence, so
// this build records only static addresses and lets the existing stable renderer
// hooks run unchanged. No new stereo detours are installed here.


inline void SetWideCollectionArmed(bool armed) noexcept {
    detail::g_wide_collection_armed.store(armed,std::memory_order_release);
    detail::Tracef("V65.35-WIDE collection=%d hooksInstalled=%d lifecycle=STARTUP-SINGLETON", armed?1:0, detail::g_wide_hooks_installed.load(std::memory_order_acquire)?1:0);
}

struct WideCounterSnapshot {
    std::uint64_t crvCtor{},crvFactory{},crvCheckout{},rendererConsume{},gbufferExecute{},gbufferMrt{},
                  viewSystem{},cameraHandoff{},entityApply{},entityFallback{},getView{},cviewCtor{},c3dCamera{},
                  stereoGate{},nativeEyeBuild{},nativeEyeApplied{};
};



inline void Shutdown(){ using namespace detail; g_wide_collection_armed.store(false,std::memory_order_release); g_wide_hooks_installed.store(false,std::memory_order_release); if(g_stereo_gate_candidate_hook){(void)g_stereo_gate_candidate_hook->disable();g_stereo_gate_candidate_hook.reset();} if(g_register_float_cvar_hook){(void)g_register_float_cvar_hook->disable();g_register_float_cvar_hook.reset();} if(g_register_int_cvar_hook){(void)g_register_int_cvar_hook->disable();g_register_int_cvar_hook.reset();} if(g_gbuffer_mrt_setup_hook){(void)g_gbuffer_mrt_setup_hook->disable();g_gbuffer_mrt_setup_hook.reset();} if(g_scene_gbuffer_execute_hook){(void)g_scene_gbuffer_execute_hook->disable();g_scene_gbuffer_execute_hook.reset();} if(g_graphics_pipeline_execute_hook){(void)g_graphics_pipeline_execute_hook->disable();g_graphics_pipeline_execute_hook.reset();} if(g_crv_camera_build_hook){(void)g_crv_camera_build_hook->disable();g_crv_camera_build_hook.reset();} if(g_crv_camera_pair_build_hook){(void)g_crv_camera_pair_build_hook->disable();g_crv_camera_pair_build_hook.reset();} if(g_renderpass_query_hook){(void)g_renderpass_query_hook->disable();g_renderpass_query_hook.reset();} if(g_world_orchestrator_hook){(void)g_world_orchestrator_hook->disable();g_world_orchestrator_hook.reset();} if(g_crv_checkout_hook){(void)g_crv_checkout_hook->disable();g_crv_checkout_hook.reset();} if(g_crv_factory_hook){(void)g_crv_factory_hook->disable();g_crv_factory_hook.reset();} if(g_crv_ctor_hook){(void)g_crv_ctor_hook->disable();g_crv_ctor_hook.reset();} Tracef("STEREO-FRAMEWORK-V31 summary register_calls=%llu storage=%p result=%p",(unsigned long long)g_register_int_cvar_calls.load(),(void*)g_stereo_framework_storage.load(),(void*)g_stereo_framework_result.load()); for(auto& slot:g_v32_float_cvars) Tracef("STEREO-RENDERER-V32 summary type=float name='%s' registrations=%llu storage=%p object=%p value=%.9f",slot.label,(unsigned long long)slot.registrations.load(),(void*)slot.storage.load(),(void*)slot.object.load(),(double)SafeReadFloat((void*)slot.storage.load())); for(auto& slot:g_v32_int_cvars) Tracef("STEREO-RENDERER-V32 summary type=int name='%s' registrations=%llu storage=%p object=%p value=%d",slot.label,(unsigned long long)slot.registrations.load(),(void*)slot.storage.load(),(void*)slot.object.load(),(int)SafeReadI32((void*)slot.storage.load())); Tracef("STEREO-GATE-V36-control summary calls=%llu true=%llu false=%llu",(unsigned long long)g_stereo_gate_candidate_calls.load(),(unsigned long long)g_stereo_gate_candidate_true.load(),(unsigned long long)g_stereo_gate_candidate_false.load()); Tracef("MULTIHOP summary vs=%llu link=%llu entity=%llu fallback=%llu publish=%llu getview=%llu ctor=%llu c3d=%llu native_eye_build=%llu native_eye_applied=%llu",(unsigned long long)g_vs_calls.load(),(unsigned long long)g_handoff_calls.load(),(unsigned long long)g_entity_apply_calls.load(),(unsigned long long)g_entity_fallback_calls.load(),(unsigned long long)g_set_view_camera_calls.load(),(unsigned long long)g_get_view_camera_calls.load(),(unsigned long long)g_cview_ctor_calls.load(),(unsigned long long)g_c3dengine_camera_calls.load(),(unsigned long long)g_native_eye_build_calls.load(),(unsigned long long)g_native_eye_applied_calls.load()); Dump("CVIEWSYS",g_vs_callers,16);Dump("LINK",g_handoff_callers,16);Dump("ENTITY",g_entity_apply_callers,16);Dump("FALLBACK",g_entity_fallback_callers,16);Dump("PUBLISH",g_set_view_camera_callers,16);Dump("GETVIEW",g_get_view_camera_callers,32);Dump("CTOR",g_cview_ctor_callers,16);
    auto off=[](auto& h){if(h){(void)h->disable();h.reset();}}; off(g_c3dengine_camera_hook);off(g_cview_ctor_hook);off(g_get_view_camera_hook);off(g_set_view_camera_hook);off(g_entity_fallback_hook);off(g_entity_apply_hook);off(g_handoff_hook);off(g_vs_hook); debug_trace::Write("MULTIHOP END"); }

} // namespace kcd2vr::stereo_archaeology
