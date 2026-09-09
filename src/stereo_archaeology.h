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

inline StereoRendererCVarSlot* MatchV32Slot(const char* name, StereoRendererCVarSlot* slots, std::size_t count) noexcept {
    if(!name) return nullptr;
    const auto base=WhBase();
    for(std::size_t i=0;i<count;++i) {
        const char* expected=base ? reinterpret_cast<const char*>(base+slots[i].name_rva) : nullptr;
        if(name==expected) return &slots[i];
        __try { if(expected && std::strcmp(name,expected)==0) return &slots[i]; }
        __except(EXCEPTION_EXECUTE_HANDLER) {}
    }
    return nullptr;
}

inline float SafeReadFloat(void* p) noexcept {
    float v=0.0f; __try { if(p) v=*reinterpret_cast<volatile float*>(p); } __except(EXCEPTION_EXECUTE_HANDLER) {} return v;
}
inline std::int32_t SafeReadI32(void* p) noexcept {
    std::int32_t v=0; __try { if(p) v=*reinterpret_cast<volatile std::int32_t*>(p); } __except(EXCEPTION_EXECUTE_HANDLER) {} return v;
}
inline void SnapshotObjectQwords(void* obj, std::uint64_t out[6]) noexcept {
    std::memset(out,0,6*sizeof(std::uint64_t));
    __try { if(obj) std::memcpy(out,obj,6*sizeof(std::uint64_t)); } __except(EXCEPTION_EXECUTE_HANDLER) {}
}

inline void* __fastcall RegisterFloatCVarDetour(const char* name, void* storage, float default_value,
                                                  std::uint64_t flags, const char* help, void* callback) noexcept {
    const auto n=g_register_float_cvar_calls.fetch_add(1,std::memory_order_relaxed)+1;
    auto* slot=MatchV32Slot(name,g_v32_float_cvars,std::size(g_v32_float_cvars));
    const float before=slot?SafeReadFloat(storage):0.0f;
    if(slot) Tracef("STEREO-RENDERER-V32 float-enter call=%llu caller=0x%llX name='%s' storage=%p before=%.9f default=%.9f flags=0x%llX help=%p callback=%p",
        (unsigned long long)n,(unsigned long long)CallerRva(),slot->label,storage,(double)before,(double)default_value,
        (unsigned long long)flags,help,callback);
    void* ret=g_register_float_cvar_original ? g_register_float_cvar_original(name,storage,default_value,flags,help,callback) : nullptr;
    if(slot) {
        slot->storage.store(reinterpret_cast<std::uintptr_t>(storage),std::memory_order_release);
        slot->object.store(reinterpret_cast<std::uintptr_t>(ret),std::memory_order_release);
        slot->registrations.fetch_add(1,std::memory_order_relaxed);
        std::uint64_t q[6]{}; SnapshotObjectQwords(ret,q);
        Tracef("STEREO-RENDERER-V32 float-leave name='%s' result=%p result_vt=0x%llX after=%.9f q=%016llX,%016llX,%016llX,%016llX,%016llX,%016llX",
            slot->label,ret,(unsigned long long)ObjectVtableRva(ret),(double)SafeReadFloat(storage),
            (unsigned long long)q[0],(unsigned long long)q[1],(unsigned long long)q[2],(unsigned long long)q[3],(unsigned long long)q[4],(unsigned long long)q[5]);
    }
    return ret;
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

inline void SnapshotSmallStorage(void* storage, std::uint32_t out[8]) noexcept {
    if(!out) return;
    std::memset(out,0,8*sizeof(std::uint32_t));
    __try { if(storage) std::memcpy(out,storage,8*sizeof(std::uint32_t)); }
    __except(EXCEPTION_EXECUTE_HANDLER) {}
}

inline void* __fastcall RegisterIntCVarDetour(const char* name, void* storage, std::uint64_t default_value,
                                               std::uint64_t flags, const char* help, void* callback) noexcept {
    const auto n=g_register_int_cvar_calls.fetch_add(1,std::memory_order_relaxed)+1;
    const auto base=WhBase();
    const auto target_name=base ? reinterpret_cast<const char*>(base+kStereoFrameworkNameRva) : nullptr;
    const bool is_framework=(name && target_name && name==target_name);
    auto* v32_int_slot=MatchV32Slot(name,g_v32_int_cvars,std::size(g_v32_int_cvars));
    if(v32_int_slot) Tracef("STEREO-RENDERER-V32 int-enter call=%llu caller=0x%llX name='%s' storage=%p before=%d default=%llu flags=0x%llX help=%p callback=%p",
        (unsigned long long)n,(unsigned long long)CallerRva(),v32_int_slot->label,storage,(int)SafeReadI32(storage),
        (unsigned long long)default_value,(unsigned long long)flags,help,callback);
    std::uint32_t before[8]{};
    if(is_framework) {
        SnapshotSmallStorage(storage,before);
        Tracef("STEREO-FRAMEWORK-V31 register-enter call=%llu caller=0x%llX name=%p storage=%p default=%llu flags=0x%llX help=%p callback=%p before=%08X,%08X,%08X,%08X,%08X,%08X,%08X,%08X",
            (unsigned long long)n,(unsigned long long)CallerRva(),name,storage,
            (unsigned long long)default_value,(unsigned long long)flags,help,callback,
            before[0],before[1],before[2],before[3],before[4],before[5],before[6],before[7]);
    }
    void* ret=g_register_int_cvar_original ? g_register_int_cvar_original(name,storage,default_value,flags,help,callback) : nullptr;
    if(v32_int_slot) {
        v32_int_slot->storage.store(reinterpret_cast<std::uintptr_t>(storage),std::memory_order_release);
        v32_int_slot->object.store(reinterpret_cast<std::uintptr_t>(ret),std::memory_order_release);
        v32_int_slot->registrations.fetch_add(1,std::memory_order_relaxed);
        std::uint64_t q[6]{}; SnapshotObjectQwords(ret,q);
        Tracef("STEREO-RENDERER-V32 int-leave name='%s' result=%p result_vt=0x%llX after=%d q=%016llX,%016llX,%016llX,%016llX,%016llX,%016llX",
            v32_int_slot->label,ret,(unsigned long long)ObjectVtableRva(ret),(int)SafeReadI32(storage),
            (unsigned long long)q[0],(unsigned long long)q[1],(unsigned long long)q[2],(unsigned long long)q[3],(unsigned long long)q[4],(unsigned long long)q[5]);
    }
    if(is_framework) {
        std::uint32_t after[8]{}; SnapshotSmallStorage(storage,after);
        g_stereo_framework_storage.store(reinterpret_cast<std::uintptr_t>(storage),std::memory_order_release);
        g_stereo_framework_result.store(reinterpret_cast<std::uintptr_t>(ret),std::memory_order_release);
        Tracef("STEREO-FRAMEWORK-V31 register-leave call=%llu result=%p storage=%p after=%08X,%08X,%08X,%08X,%08X,%08X,%08X,%08X help='%s'",
            (unsigned long long)n,ret,storage,after[0],after[1],after[2],after[3],after[4],after[5],after[6],after[7],
            help?help:"");
    }
    return ret;
}

inline void TraceStereoFrameworkStaticOnce() noexcept {
    static std::atomic<bool> done{false}; bool expected=false;
    if(!done.compare_exchange_strong(expected,true,std::memory_order_relaxed)) return;
    const auto base=WhBase(); if(!base) return;
    std::uint8_t bytes[24]{};
    __try { std::memcpy(bytes,reinterpret_cast<const void*>(base+kStereoFrameworkRegisterSiteRva),sizeof(bytes)); }
    __except(EXCEPTION_EXECUTE_HANDLER) {}
    Tracef("STEREO-FRAMEWORK-V31 static name_rva=0x%llX help_rva=0x%llX register_site=0x%llX helper=0x%llX expected_default=1 bytes=%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
        (unsigned long long)kStereoFrameworkNameRva,(unsigned long long)kStereoFrameworkHelpRva,
        (unsigned long long)kStereoFrameworkRegisterSiteRva,(unsigned long long)kRegisterIntCVarRva,
        bytes[0],bytes[1],bytes[2],bytes[3],bytes[4],bytes[5],bytes[6],bytes[7],bytes[8],bytes[9],bytes[10],bytes[11],
        bytes[12],bytes[13],bytes[14],bytes[15],bytes[16],bytes[17],bytes[18],bytes[19],bytes[20],bytes[21],bytes[22],bytes[23]);
}

inline void TrackCRenderView(void* p, void* manager, std::uintptr_t caller, std::uint32_t mode) noexcept {
    if(!p) return;
    const auto pv=reinterpret_cast<std::uintptr_t>(p);
    for(auto& e:g_tracked_crv) {
        auto cur=e.ptr.load(std::memory_order_acquire);
        if(cur==pv) return;
        if(!cur) {
            std::uintptr_t zero=0;
            if(e.ptr.compare_exchange_strong(zero,pv,std::memory_order_acq_rel)) {
                e.manager.store(reinterpret_cast<std::uintptr_t>(manager),std::memory_order_release);
                e.checkout_caller.store(caller,std::memory_order_release);
                e.mode.store(mode,std::memory_order_release);
                Tracef("CRV-CORRELATE tag crv=%p mode=%u manager=%p checkout_caller=0x%llX",p,mode,manager,(unsigned long long)caller);
                return;
            }
        }
    }
}

inline bool MarkRenderObjectSeen(void* p) noexcept {
    if(!p) return false;
    const auto pv=reinterpret_cast<std::uintptr_t>(p);
    for(auto& a:g_seen_render_objects) {
        auto cur=a.load(std::memory_order_acquire);
        if(cur==pv) return false;
        if(!cur) { std::uintptr_t z=0; if(a.compare_exchange_strong(z,pv,std::memory_order_acq_rel)) return true; }
    }
    return false;
}

inline void CorrelateRenderObject(void* obj) noexcept {
    if(!obj || !MarkRenderObjectSeen(obj)) return;
    const auto ov=reinterpret_cast<std::uintptr_t>(obj);
    Tracef("CRV-CORRELATE new-render-object obj=%p vt=0x%llX b1FD=%u u204=%u b224=%u u1AC=0x%X u1B0=0x%X u1548=0x%X u154C=0x%X u1550=0x%X",
        obj,(unsigned long long)ObjectVtableRva(obj),SafeReadU32At(obj,0x1FD)&0xFFu,SafeReadU32At(obj,0x204),SafeReadU32At(obj,0x224)&0xFFu,
        SafeReadU32At(obj,0x1AC),SafeReadU32At(obj,0x1B0),SafeReadU32At(obj,0x1548),SafeReadU32At(obj,0x154C),SafeReadU32At(obj,0x1550));
    for(auto& e:g_tracked_crv) {
        const auto cv=e.ptr.load(std::memory_order_acquire); if(!cv) continue;
        const auto mode=e.mode.load(std::memory_order_acquire);
        const auto mgr=e.manager.load(std::memory_order_acquire);
        const auto cc=e.checkout_caller.load(std::memory_order_acquire);
        std::uintptr_t obj_to_crv=~std::uintptr_t(0), crv_to_obj=~std::uintptr_t(0);
        __try {
            for(std::uintptr_t off=0;off<0x1600;off+=8) if(*reinterpret_cast<std::uintptr_t*>(ov+off)==cv){obj_to_crv=off;break;}
        } __except(EXCEPTION_EXECUTE_HANDLER) {}
        __try {
            for(std::uintptr_t off=0;off<0xA770;off+=8) if(*reinterpret_cast<std::uintptr_t*>(cv+off)==ov){crv_to_obj=off;break;}
        } __except(EXCEPTION_EXECUTE_HANDLER) {}
        if(obj_to_crv!=~std::uintptr_t(0) || crv_to_obj!=~std::uintptr_t(0))
            Tracef("CRV-CORRELATE LINK render_obj=%p crv=%p mode=%u manager=%p checkout_caller=0x%llX obj_to_crv_off=%s0x%llX crv_to_obj_off=%s0x%llX",
                obj,(void*)cv,mode,(void*)mgr,(unsigned long long)cc,
                obj_to_crv==~std::uintptr_t(0)?"NONE/":"",(unsigned long long)(obj_to_crv==~std::uintptr_t(0)?0:obj_to_crv),
                crv_to_obj==~std::uintptr_t(0)?"NONE/":"",(unsigned long long)(crv_to_obj==~std::uintptr_t(0)?0:crv_to_obj));
    }
}

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
inline void CountCaller(CallerBucket* b,std::size_t n,std::uintptr_t r) noexcept {
    if(!r)return; for(std::size_t i=0;i<n;++i){auto k=b[i].rva.load(std::memory_order_relaxed); if(k==r){b[i].count.fetch_add(1,std::memory_order_relaxed);return;} if(!k){std::uintptr_t z=0;if(b[i].rva.compare_exchange_strong(z,r,std::memory_order_relaxed)){b[i].count.store(1,std::memory_order_relaxed);return;}}}
}
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
inline bool ReadCSystemSemanticV35(void* self, unsigned char* bytes, std::size_t byte_count, std::uintptr_t* slots, std::size_t slot_count) noexcept {
    if(!self || !bytes || byte_count<0x49 || !slots || slot_count<11) return false;
    __try {
        auto base=reinterpret_cast<std::uintptr_t>(self);
        for(std::size_t i=0;i<0x49;++i) bytes[i]=*reinterpret_cast<unsigned char*>(base+0x580+i);
        auto vt=*reinterpret_cast<std::uintptr_t*>(self);
        if(!vt) return false;
        for(std::size_t i=0;i<11;++i) slots[i]=*reinterpret_cast<std::uintptr_t*>(vt+0xD8+i*8);
        return true;
    } __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
}

inline void TraceCSystemSemanticV35(void* self, std::uint64_t n, std::uintptr_t caller) noexcept {
    unsigned char b[0x49]{}; std::uintptr_t slots[11]{};
    if(!ReadCSystemSemanticV35(self,b,sizeof(b),slots,11)) return;
    Tracef("CSYSTEM-SEMANTIC-V35 call=%llu caller=0x%llX bytes580=%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X bytes588=%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X bytes590=%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X bytes598=%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X bytes5A0=%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X bytes5A8=%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X bytes5B0=%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X bytes5B8=%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X bytes5C0=%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X bytes5C8=%02X",
        (unsigned long long)n,(unsigned long long)caller,
        b[0],b[1],b[2],b[3],b[4],b[5],b[6],b[7],b[8],b[9],b[10],b[11],b[12],b[13],b[14],b[15],
        b[16],b[17],b[18],b[19],b[20],b[21],b[22],b[23],b[24],b[25],b[26],b[27],b[28],b[29],b[30],b[31],
        b[32],b[33],b[34],b[35],b[36],b[37],b[38],b[39],b[40],b[41],b[42],b[43],b[44],b[45],b[46],b[47],
        b[48],b[49],b[50],b[51],b[52],b[53],b[54],b[55],b[56],b[57],b[58],b[59],b[60],b[61],b[62],b[63],b[64],b[65],b[66],b[67],b[68],b[69],b[70],b[71],b[72]);
    Tracef("CSYSTEM-VTABLE-V35 call=%llu D8=0x%llX E0=0x%llX E8=0x%llX F0=0x%llX F8=0x%llX 100=0x%llX 108=0x%llX 110=0x%llX 118=0x%llX 120=0x%llX 128=0x%llX",
        (unsigned long long)n,(unsigned long long)ToRva(slots[0]),(unsigned long long)ToRva(slots[1]),(unsigned long long)ToRva(slots[2]),
        (unsigned long long)ToRva(slots[3]),(unsigned long long)ToRva(slots[4]),(unsigned long long)ToRva(slots[5]),
        (unsigned long long)ToRva(slots[6]),(unsigned long long)ToRva(slots[7]),(unsigned long long)ToRva(slots[8]),
        (unsigned long long)ToRva(slots[9]),(unsigned long long)ToRva(slots[10]));
}

inline bool __fastcall StereoGateCandidateDetour(void* self) noexcept {
    const auto n=g_stereo_gate_candidate_calls.fetch_add(1,std::memory_order_relaxed)+1;
    const auto caller=CallerRva();
    const bool ret=g_stereo_gate_candidate_original ? g_stereo_gate_candidate_original(self) : false;
    if(ret) g_stereo_gate_candidate_true.fetch_add(1,std::memory_order_relaxed);
    else g_stereo_gate_candidate_false.fetch_add(1,std::memory_order_relaxed);
    if(n<=64 || n%600==0) {
        char rtti[160]{}; ObjectRttiName(self,rtti,sizeof(rtti));
        Tracef("STEREO-GATE-V35 call=%llu caller=0x%llX self=%p vt=0x%llX rtti='%s' field598=%s tid=%lu",
            (unsigned long long)n,(unsigned long long)caller,self,
            (unsigned long long)ObjectVtableRva(self),rtti,ret?"true":"false",
            (unsigned long)GetCurrentThreadId());
        if(n<=16 || n%6000==0) TraceCSystemSemanticV35(self,n,caller);
    }
    return ret;
}

inline bool ResolveStereoGateCandidateV34(std::uintptr_t base, void*& obj, std::uintptr_t& vt, std::uintptr_t& target) noexcept {
    obj=nullptr; vt=0; target=0;
    __try {
        obj=*reinterpret_cast<void**>(base+kStereoGateObjectGlobalRva);
        if(obj) {
            vt=*reinterpret_cast<std::uintptr_t*>(obj);
            if(vt) target=*reinterpret_cast<std::uintptr_t*>(vt+kStereoGateVslot);
        }
        return obj!=nullptr && target!=0;
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        obj=nullptr; vt=0; target=0;
        return false;
    }
}

inline bool InstallStereoGateCandidateV34() noexcept {
    const auto base=WhBase(); if(!base) return false;
    void* obj=nullptr; std::uintptr_t vt=0,target=0;
    ResolveStereoGateCandidateV34(base,obj,vt,target);
    char rtti[160]{}; ObjectRttiName(obj,rtti,sizeof(rtti));
    Tracef("STEREO-GATE-V35 static global_rva=0x%llX object=%p vt_rva=0x%llX rtti='%s' slot=0x%llX target=%p target_rva=0x%llX",
        (unsigned long long)kStereoGateObjectGlobalRva,obj,
        (unsigned long long)ToRva(vt),rtti,
        (unsigned long long)kStereoGateVslot,(void*)target,
        (unsigned long long)ToRva(target));
    if(!obj || !target) return false;
    DetourModKit::hook::InlineRequest req{
        "CSystem boolean semantic probe V35",
        DetourModKit::hook::Target{DetourModKit::Address{target}},
        DetourModKit::hook::Options{}};
    auto result=DetourModKit::hook::inline_at(std::move(req),&StereoGateCandidateDetour);
    if(!result) return false;
    g_stereo_gate_candidate_hook=std::move(*result);
    g_stereo_gate_candidate_original=g_stereo_gate_candidate_hook->template original<StereoGateCandidateFunc>();
    auto e=g_stereo_gate_candidate_hook->enable();
    if(!e) { g_stereo_gate_candidate_original=nullptr; g_stereo_gate_candidate_hook.reset(); return false; }
    return true;
}

inline void TraceFrameNexusOnce() noexcept {
    static std::atomic<bool> done{false};
    bool expected=false;
    if(!done.compare_exchange_strong(expected,true,std::memory_order_relaxed)) return;
    const auto base=WhBase();
    if(!base) return;
    // In frame-orchestrator-B (0x669C88..0x66A029), immediately after GetViewCamera,
    // global WHGame+0x492D808 receives (camera, out-structure) through vtable slot +0x40.
    constexpr std::uintptr_t kFrameNexusGlobalRva=0x492D808ull;
    void* obj=nullptr; std::uintptr_t vt=0;
    __try { obj=*reinterpret_cast<void**>(base+kFrameNexusGlobalRva); if(obj) vt=*reinterpret_cast<std::uintptr_t*>(obj); } __except(EXCEPTION_EXECUTE_HANDLER){}
    char rtti[192]{}; ObjectRttiName(obj,rtti,sizeof(rtti));
    Tracef("FRAME-NEXUS global_rva=0x%llX object=%p vtable_rva=0x%llX rtti='%s'",
        (unsigned long long)kFrameNexusGlobalRva,obj,(unsigned long long)ToRva(vt),rtti);
    if(!obj || !vt) return;
    // Dump a useful band of virtual targets around the observed +0x40 call.
    for(std::uintptr_t off=0; off<=0x100; off+=8){
        std::uintptr_t fn=0;
        __try { fn=*reinterpret_cast<std::uintptr_t*>(vt+off); } __except(EXCEPTION_EXECUTE_HANDLER){}
        Tracef("FRAME-NEXUS vslot=0x%llX target_rva=0x%llX target=%p",
            (unsigned long long)off,(unsigned long long)ToRva(fn),(void*)fn);
    }
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
inline CallerBucket* FindCallerBucket(CallerBucket* b,std::size_t n,std::uintptr_t r) noexcept {
    if(!r) return nullptr;
    for(std::size_t i=0;i<n;++i) if(b[i].rva.load(std::memory_order_relaxed)==r) return &b[i];
    return nullptr;
}
inline void TraceStackOnce(CallerBucket* bucket,std::uintptr_t caller) noexcept {
    if(!bucket) return;
    bool expected=false;
    if(!bucket->stack_logged.compare_exchange_strong(expected,true,std::memory_order_relaxed)) return;
    void* frames[16]{};
    const USHORT captured = CaptureStackBackTrace(0,16,frames,nullptr);
    char chain[1100]{};
    std::size_t used=0;
    for(USHORT i=0;i<captured;++i){
        auto r=ToRva(reinterpret_cast<std::uintptr_t>(frames[i]));
        int n=0;
        if(r) n=std::snprintf(chain+used,sizeof(chain)-used,"%s0x%llX",used?" > ":"",(unsigned long long)r);
        else n=std::snprintf(chain+used,sizeof(chain)-used,"%s%p",used?" > ":"",frames[i]);
        if(n<=0 || static_cast<std::size_t>(n)>=sizeof(chain)-used) break;
        used+=static_cast<std::size_t>(n);
    }
    Tracef("GETVIEW-STACK caller=0x%llX label=%s tid=%lu frames=%u chain=%s",(unsigned long long)caller,ConsumerLabel(caller),(unsigned long)GetCurrentThreadId(),(unsigned)captured,chain);
}

inline void CameraSummary(void* cam,float& x,float& y,float& z,float& a,float& b,float& c,float& d) noexcept {
    x=y=z=a=b=c=0.0f; d=1.0f;
    // CCamera stores its matrix at the front. Translation is matrix row/column values +0x0C/+0x1C/+0x2C in this build.
    __try { if(cam){auto* f=reinterpret_cast<float*>(cam); x=f[3]; y=f[7]; z=f[11]; a=f[0]; b=f[5]; c=f[10]; d=f[12];} } __except(EXCEPTION_EXECUTE_HANDLER){}
}


using C3DEngineCameraFunc = void(__fastcall*)(void* self, void* camera, void* out_state);
inline C3DEngineCameraFunc g_c3dengine_camera_original=nullptr;
inline std::optional<DetourModKit::hook::Hook> g_c3dengine_camera_hook;
inline std::atomic<std::uint64_t> g_c3dengine_camera_calls{0};
inline std::atomic<std::uintptr_t> g_last_published_view_camera{0};

inline void __fastcall C3DEngineCameraDetour(void* self, void* camera, void* out_state) noexcept {
    const auto n=g_c3dengine_camera_calls.fetch_add(1,std::memory_order_relaxed)+1;
    const auto cr=CallerRva();
    char rtti[192]{};
    ObjectRttiName(self,rtti,sizeof(rtti));
    float x=0,y=0,z=0,a=0,b=0,c=0,d=0;
    CameraSummary(camera,x,y,z,a,b,c,d);
    if(n<=96 || (n%600)==0){
        Tracef("C3D-CAMERA call=%llu caller=0x%llX self=%p vt=0x%llX rtti='%s' camera=%p pos=(%.3f,%.3f,%.3f) diag=(%.3f,%.3f,%.3f) extra=%.3f out=%p tid=%lu",
            (unsigned long long)n,(unsigned long long)cr,self,(unsigned long long)ObjectVtableRva(self),rtti,
            camera,x,y,z,a,b,c,d,out_state,(unsigned long)GetCurrentThreadId());
    }
    if(g_c3dengine_camera_original) g_c3dengine_camera_original(self,camera,out_state);
    if(n<=32 && out_state){
        std::uint32_t q[8]{};
        __try { std::memcpy(q,out_state,sizeof(q)); } __except(EXCEPTION_EXECUTE_HANDLER) {}
        Tracef("C3D-CAMERA-OUT call=%llu out=%p dwords=%08X,%08X,%08X,%08X,%08X,%08X,%08X,%08X",
            (unsigned long long)n,out_state,q[0],q[1],q[2],q[3],q[4],q[5],q[6],q[7]);
    }
}


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

inline std::uint32_t TraceCheckoutStack(std::uint64_t n, std::uint32_t mode) noexcept {
    if(mode!=2 || n>32) return 0;
    void* frames[12]{};
    const USHORT captured=CaptureStackBackTrace(0,12,frames,nullptr);
    char chain[768]{}; std::size_t used=0;
    std::uint32_t route=0; // 1=main 0x4E6C/4E5A family, 2=alternate 0x4E73 family
    for(USHORT i=0;i<captured && used+32<sizeof(chain);++i) {
        const auto a=reinterpret_cast<std::uintptr_t>(frames[i]);
        const auto b=WhBase();
        int w=0;
        if(b && a>=b && a<b+0x7000000ull) {
            const auto rva=a-b;
            if(rva>=0x4E7300ull && rva<0x4E7500ull) route=2;
            else if(route==0 && ((rva>=0x4E6900ull && rva<0x4E7300ull) || (rva>=0x4E5800ull && rva<0x4E6500ull))) route=1;
            w=std::snprintf(chain+used,sizeof(chain)-used,"%s0x%llX",i?" > ":"",(unsigned long long)rva);
        } else w=std::snprintf(chain+used,sizeof(chain)-used,"%s%p",i?" > ":"",frames[i]);
        if(w<=0) break; used+=static_cast<std::size_t>(w);
    }
    Tracef("CRV-CHECKOUT-STACK call=%llu mode=%u route=%s tid=%lu frames=%u chain=%s",
        (unsigned long long)n,mode,route==2?"alternate-4E73":route==1?"main-4E6C":"unknown",
        (unsigned long)GetCurrentThreadId(),(unsigned)captured,chain);
    return route;
}

inline void* __fastcall CRenderViewCtorObserver(void* self, void* captured_owner, std::uint32_t mode) noexcept {
    const auto n=g_crv_ctor_calls.fetch_add(1,std::memory_order_relaxed)+1;
    Tracef("CRV-CTOR-OBS enter call=%llu caller=0x%llX self=%p captured_owner=%p mode=%u tid=%lu",
        (unsigned long long)n,(unsigned long long)CallerRva(),self,captured_owner,mode,(unsigned long)GetCurrentThreadId());
    void* ret=g_crv_ctor_original ? g_crv_ctor_original(self,captured_owner,mode) : self;
    char rtti[192]{}; ObjectRttiName(ret,rtti,sizeof(rtti));
    Tracef("CRV-CTOR-OBS leave call=%llu ret=%p vt=0x%llX rtti='%s' mode_field=%u p18=%p p30=%p",
        (unsigned long long)n,ret,(unsigned long long)ObjectVtableRva(ret),rtti,
        SafeReadU32At(ret,0x14),(void*)SafeReadPtrAt(ret,0x18),(void*)SafeReadPtrAt(ret,0x30));
    return ret;
}

inline std::uint64_t SafeVectorCount(void* self, std::uintptr_t begin_off, std::uintptr_t end_off) noexcept {
    __try {
        if(!self) return 0;
        const auto b=*reinterpret_cast<std::uintptr_t*>(reinterpret_cast<std::uintptr_t>(self)+begin_off);
        const auto e=*reinterpret_cast<std::uintptr_t*>(reinterpret_cast<std::uintptr_t>(self)+end_off);
        if(!b || e<b || ((e-b)&7ull)!=0 || (e-b)>0x100000ull) return 0;
        return (e-b)/8ull;
    } __except(EXCEPTION_EXECUTE_HANDLER) { return 0; }
}


struct DeferredCrvSnapshot {
    void* crv{};
    std::uint64_t checkout_call{};
    std::uintptr_t checkout_caller{};
    std::uint32_t mode{};
    std::uint32_t route{};
};
inline thread_local DeferredCrvSnapshot g_deferred_crv_snapshot{};


inline std::intptr_t FindExactPointerOffset(void* obj, std::size_t size, std::uintptr_t target) noexcept {
    if(!obj || !target || size<sizeof(std::uintptr_t)) return -1;
    __try {
        const auto base=reinterpret_cast<std::uintptr_t>(obj);
        for(std::size_t off=0; off+sizeof(std::uintptr_t)<=size; off+=sizeof(std::uintptr_t)) {
            if(*reinterpret_cast<std::uintptr_t*>(base+off)==target) return static_cast<std::intptr_t>(off);
        }
    } __except(EXCEPTION_EXECUTE_HANDLER) {}
    return -1;
}

inline std::intptr_t FindCameraMatrixOffset(void* obj, std::size_t size, void* camera) noexcept {
    if(!obj || !camera || size<0x30) return -1;
    unsigned char signature[0x30]{};
    __try { std::memcpy(signature,camera,sizeof(signature)); }
    __except(EXCEPTION_EXECUTE_HANDLER) { return -1; }
    __try {
        const auto base=reinterpret_cast<std::uintptr_t>(obj);
        for(std::size_t off=0; off+sizeof(signature)<=size; off+=4) {
            if(std::memcmp(reinterpret_cast<void*>(base+off),signature,sizeof(signature))==0)
                return static_cast<std::intptr_t>(off);
        }
    } __except(EXCEPTION_EXECUTE_HANDLER) {}
    return -1;
}


struct CameraTranslationBackup {
    bool valid{};
    float x{},y{},z{};
};

inline CameraTranslationBackup ShiftEmbeddedCameraRight(void* camera,float meters) noexcept {
    CameraTranslationBackup b{};
    __try {
        if(!camera) return b;
        auto* f=reinterpret_cast<float*>(camera);
        b.x=f[3]; b.y=f[7]; b.z=f[11]; b.valid=true;
        // CCamera is a row-major 3x4 transform in this build:
        // translation = [3,7,11], local X/right basis = [0,4,8].
        f[3]  = b.x + f[0]*meters;
        f[7]  = b.y + f[4]*meters;
        f[11] = b.z + f[8]*meters;
    } __except(EXCEPTION_EXECUTE_HANDLER) { b.valid=false; }
    return b;
}

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

inline OpenXrStereoInjection GetOpenXrStereoInjection() noexcept {
    OpenXrStereoInjection r{};
    auto& xr=kcd2vr::XrManager::Instance();
    kcd2vr::XrManager::StereoViewSnapshot latest{};
    kcd2vr::XrManager::StereoViewSnapshot good{};
    const bool have_latest=xr.CopyLatestStereoViews(latest);
    const bool have_good=xr.CopyLastGoodStereoViews(good);
    const auto now=static_cast<std::uint64_t>(GetTickCount64());

    auto fill_meta=[&](const kcd2vr::XrManager::StereoViewSnapshot& snap) noexcept {
        r.predicted_display_time=snap.predicted_display_time;
        r.view_count=snap.view_count;
        r.view_state_flags=snap.view_state_flags;
        r.locate_result=snap.locate_result;
        r.age_ms=(snap.captured_tick_ms && now>=snap.captured_tick_ms) ? now-snap.captured_tick_ms : ~std::uint64_t(0);
    };
    auto sane_ipd=[](const kcd2vr::XrManager::StereoViewSnapshot& snap,float& ipd) noexcept {
        const float dx=snap.eye_pose[1].position.x-snap.eye_pose[0].position.x;
        const float dy=snap.eye_pose[1].position.y-snap.eye_pose[0].position.y;
        const float dz=snap.eye_pose[1].position.z-snap.eye_pose[0].position.z;
        ipd=std::sqrt(dx*dx+dy*dy+dz*dz);
        return std::isfinite(ipd) && ipd>=kMinOpenXrIpdMeters && ipd<=kMaxOpenXrIpdMeters;
    };

    float ipd=0.0f;
    // Tier 1: newest raw result, only when it is itself a fresh good pair.
    if(have_latest && latest.valid && latest.locate_succeeded && latest.view_count==2 &&
       latest.captured_tick_ms && now>=latest.captured_tick_ms &&
       (now-latest.captured_tick_ms)<=80 && sane_ipd(latest,ipd)) {
        fill_meta(latest); r.valid=true; r.ipd=ipd; r.source="fresh-xr";
    }
    // Tier 2: separate last-known-good cache. A transient failed xrLocateViews
    // no longer overwrites/poisons the injection pair.
    else if(have_good && good.captured_tick_ms && now>=good.captured_tick_ms &&
            (now-good.captured_tick_ms)<=500 && sane_ipd(good,ipd)) {
        fill_meta(good); r.valid=true; r.ipd=ipd; r.source="cached-xr";
    }
    // Tier 3: relaxed latest successful pair, useful when strict validity bits
    // are incomplete but geometry is still finite and sane.
    else if(have_latest && latest.locate_succeeded && latest.view_count==2 &&
            latest.captured_tick_ms && now>=latest.captured_tick_ms &&
            (now-latest.captured_tick_ms)<=750 && sane_ipd(latest,ipd)) {
        fill_meta(latest); r.valid=true; r.ipd=ipd; r.source="relaxed-xr";
    }

    if(!r.valid) {
        r.valid=true;
        r.ipd=0.064f;
        r.left_offset=-0.032f;
        r.right_offset=+0.032f;
        r.source="fixed-64mm";
        return r;
    }

    r.left_offset=-0.5f*r.ipd;
    r.right_offset=+0.5f*r.ipd;
    return r;
}

inline void RestoreEmbeddedCameraTranslation(void* camera,const CameraTranslationBackup& b) noexcept {
    if(!b.valid || !camera) return;
    __try {
        auto* f=reinterpret_cast<float*>(camera);
        f[3]=b.x; f[7]=b.y; f[11]=b.z;
    } __except(EXCEPTION_EXECUTE_HANDLER) {}
}



inline std::uint64_t V21ElapsedMs() noexcept {
    const auto now=static_cast<std::uint64_t>(GetTickCount64());
    auto first=g_v21_first_tick.load(std::memory_order_acquire);
    if(!first) {
        std::uint64_t zero=0;
        if(g_v21_first_tick.compare_exchange_strong(zero,now,std::memory_order_acq_rel)) first=now;
        else first=g_v21_first_tick.load(std::memory_order_acquire);
    }
    return now>=first ? now-first : 0;
}

inline const char* V21PhaseName(std::uint64_t elapsed) noexcept {
    if(elapsed < 5000) return "isolate-baseline";
    if(elapsed < 10000) return "isolate-A-plus100";
    if(elapsed < 15000) return "isolate-B-plus100";
    if(elapsed < 20000) return "isolate-A-minus100";
    if(elapsed < 25000) return "isolate-B-minus100";
    return "isolate-xr-split";
}

inline void __fastcall CRenderViewCameraBuildDetourV21(void* self) noexcept {
    const auto call=g_v21_broad_calls.fetch_add(1,std::memory_order_relaxed)+1;
    const auto source_a=g_native_eye_source.load(std::memory_order_acquire);
    const auto source_b=g_native_eye_source_b.load(std::memory_order_acquire);
    const auto self_u=reinterpret_cast<std::uintptr_t>(self);
    const bool is_a=self_u==source_a;
    const bool is_b=source_b && self_u==source_b;
    if((!is_a && !is_b) || ObjectVtableRva(self)!=kCRenderViewVtableRva ||
       !g_native_eye_write_enabled.load(std::memory_order_acquire)) {
        if(g_crv_camera_build_original) g_crv_camera_build_original(self);
        return;
    }

    const auto elapsed=V21ElapsedMs();
    if(elapsed < 5000 || !source_b) {
        if(call<=8 || call%240==0) Tracef("PARENT-ISOLATE-V27 baseline build_call=%llu elapsed_ms=%llu role=%s parent=%p pairA=%p pairB=%p tid=%lu",
            (unsigned long long)call,(unsigned long long)elapsed,is_a?"A":"B",self,(void*)source_a,(void*)source_b,(unsigned long)GetCurrentThreadId());
        if(g_crv_camera_build_original) g_crv_camera_build_original(self);
        return;
    }

    const auto selector=SafeReadU32At(self,0x8EE0);
    const auto idx=selector & 1u;
    const auto stride=static_cast<std::uintptr_t>(idx)*0x310ull;
    void* cam0=reinterpret_cast<void*>(self_u+0x8EE8ull+stride);
    float off=0.0f;
    const char* source_name="fixed";
    if(elapsed < 10000) off=is_a ? +0.100f : 0.0f;
    else if(elapsed < 15000) off=is_b ? +0.100f : 0.0f;
    else if(elapsed < 20000) off=is_a ? -0.100f : 0.0f;
    else if(elapsed < 25000) off=is_b ? -0.100f : 0.0f;
    else {
        const auto xr=GetOpenXrStereoInjection();
        source_name=xr.source;
        off=is_a ? xr.left_offset : xr.right_offset;
    }

    const auto b0=ShiftEmbeddedCameraRight(cam0,off);
    const auto applied=g_v21_broad_applied.fetch_add(1,std::memory_order_relaxed)+1;
    g_native_eye_applied_calls.fetch_add(1,std::memory_order_relaxed);
    if(applied<=64 || applied%120==0) {
        Tracef("PARENT-ISOLATE-V27 apply=%llu build_call=%llu elapsed_ms=%llu phase=%s source=%s role=%s parent=%p pairA=%p pairB=%p selector=%u selected_index=%u camA=%p off=%.6f before=(%.3f,%.3f,%.3f) tid=%lu",
            (unsigned long long)applied,(unsigned long long)call,(unsigned long long)elapsed,V21PhaseName(elapsed),source_name,is_a?"A":"B",self,
            (void*)source_a,(void*)source_b,selector,idx,cam0,(double)off,(double)b0.x,(double)b0.y,(double)b0.z,(unsigned long)GetCurrentThreadId());
    }
    if(g_crv_camera_build_original) g_crv_camera_build_original(self);
    RestoreEmbeddedCameraTranslation(cam0,b0);
}

inline void TraceNativePairOutputV17(std::uint64_t pair_call, void* owner, void* out_state, void* cam_a, void* cam_b, void* context, float arg5, float arg6) noexcept {
    if(!owner || !out_state) return;
    const auto source=SafeReadPtrAt(owner,0x38);
    void* source_cam = source ? reinterpret_cast<void*>(source + 0x8EE8) : nullptr;
    const auto match_source = source_cam ? FindCameraMatrixOffset(out_state,0x600,source_cam) : -1;
    const auto ptr_cam_a = FindExactPointerOffset(out_state,0x600,reinterpret_cast<std::uintptr_t>(cam_a));
    const auto ptr_cam_b = FindExactPointerOffset(out_state,0x600,reinterpret_cast<std::uintptr_t>(cam_b));
    const auto ptr_source_cam = source_cam ? FindExactPointerOffset(out_state,0x600,reinterpret_cast<std::uintptr_t>(source_cam)) : -1;
    const auto ptr_context = context ? FindExactPointerOffset(out_state,0x600,reinterpret_cast<std::uintptr_t>(context)) : -1;

    std::uint64_t q[12]{}; std::uint64_t s2q[12]{};
    SafeReadBytesAt(out_state,0,q,sizeof(q));
    SafeReadBytesAt(out_state,0x290,s2q,sizeof(s2q));
    float sx=0,sy=0,sz=0,sa=0,sb=0,sc=0,sd=0;
    if(source_cam) CameraSummary(source_cam,sx,sy,sz,sa,sb,sc,sd);

    if(pair_call<=48 || pair_call%120==0) {
        Tracef("NATIVE-PAIR-V21 post pair_call=%llu child=%p source=%p sourceCam=%p sourcePos=(%.3f,%.3f,%.3f) out=%p matrixMatch=%lld ptrA=%lld ptrB=%lld ptrSourceCam=%lld ptrContext=%lld arg5=%.6f arg6=%.6f q0=%016llX q1=%016llX q2=%016llX q3=%016llX q4=%016llX q5=%016llX q6=%016llX q7=%016llX q8=%016llX q9=%016llX q10=%016llX q11=%016llX s2q0=%016llX s2q1=%016llX s2q2=%016llX s2q3=%016llX s2q4=%016llX s2q5=%016llX tid=%lu",
            (unsigned long long)pair_call,owner,(void*)source,source_cam,(double)sx,(double)sy,(double)sz,out_state,
            (long long)match_source,(long long)ptr_cam_a,(long long)ptr_cam_b,(long long)ptr_source_cam,(long long)ptr_context,
            (double)arg5,(double)arg6,
            (unsigned long long)q[0],(unsigned long long)q[1],(unsigned long long)q[2],(unsigned long long)q[3],
            (unsigned long long)q[4],(unsigned long long)q[5],(unsigned long long)q[6],(unsigned long long)q[7],
            (unsigned long long)q[8],(unsigned long long)q[9],(unsigned long long)q[10],(unsigned long long)q[11],
            (unsigned long long)s2q[0],(unsigned long long)s2q[1],(unsigned long long)s2q[2],
            (unsigned long long)s2q[3],(unsigned long long)s2q[4],(unsigned long long)s2q[5],
            (unsigned long)GetCurrentThreadId());
    }
}


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

inline void V30CaptureAndDiff(std::uint64_t call, std::uintptr_t owner, void* records, std::uint32_t mode) noexcept {
    if(!owner || !records || mode!=0) return;
    auto a=g_v30_owner_a.load(std::memory_order_acquire);
    if(!a) { std::uintptr_t z=0; if(g_v30_owner_a.compare_exchange_strong(z,owner,std::memory_order_acq_rel)) { a=owner; Tracef("RECORD-DIFF-V30 selected-A owner=%p",(void*)owner); } else a=g_v30_owner_a.load(std::memory_order_acquire); }
    auto b=g_v30_owner_b.load(std::memory_order_acquire);
    if(owner!=a && !b) { std::uintptr_t z=0; if(g_v30_owner_b.compare_exchange_strong(z,owner,std::memory_order_acq_rel)) { b=owner; Tracef("RECORD-DIFF-V30 selected-B owner=%p",(void*)owner); } else b=g_v30_owner_b.load(std::memory_order_acquire); }
    if(owner!=a && owner!=b) return;

    auto& dst=(owner==a)?g_v30_snap_a:g_v30_snap_b;
    __try { std::memcpy(dst.bytes.data(),records,dst.bytes.size()); dst.owner=owner; dst.call=call; dst.valid=true; }
    __except(EXCEPTION_EXECUTE_HANDLER) { dst.valid=false; return; }
    if(!g_v30_snap_a.valid || !g_v30_snap_b.valid) return;
    const auto ca=g_v30_snap_a.call, cb=g_v30_snap_b.call;
    const auto gap=ca>cb?ca-cb:cb-ca;
    if(gap>32) return;
    const auto report=++g_v30_pair_reports;
    if(report>8 && (report%120)!=0) return;

    std::uint32_t differing=0, matrix_differing=0;
    for(std::size_t off=0; off<0x400; off+=16) {
        if(std::memcmp(g_v30_snap_a.bytes.data()+off,g_v30_snap_b.bytes.data()+off,16)!=0) {
            ++differing; if(off>=0x100 && off<0x300) ++matrix_differing;
        }
    }
    Tracef("RECORD-DIFF-V30 pair=%llu ownerA=%p callA=%llu ownerB=%p callB=%llu gap=%llu differing16=%u matrixRangeDiff16=%u",
        (unsigned long long)report,(void*)a,(unsigned long long)ca,(void*)b,(unsigned long long)cb,(unsigned long long)gap,(unsigned)differing,(unsigned)matrix_differing);

    // The V24 static trace identified +0x190..+0x1C0 as renderer-read matrix-looking
    // state.  Print that neighborhood plus nearby projection candidates without writes.
    for(std::size_t off=0x140; off<=0x260; off+=16) {
        float fa[4]{},fb[4]{}; std::memcpy(fa,g_v30_snap_a.bytes.data()+off,16); std::memcpy(fb,g_v30_snap_b.bytes.data()+off,16);
        bool finite=true; for(int i=0;i<4;++i) finite &= std::isfinite(fa[i]) && std::isfinite(fb[i]);
        if(!finite) continue;
        float md=0.0f; for(int i=0;i<4;++i) md=(std::max)(md,std::fabs(fa[i]-fb[i]));
        if(md>1.0e-5f || off==0x190 || off==0x1A0 || off==0x1B0 || off==0x1C0)
            Tracef("RECORD-DIFF-V30 block off=0x%03llX maxDelta=%.7f A=(%.6f,%.6f,%.6f,%.6f) B=(%.6f,%.6f,%.6f,%.6f)",
                (unsigned long long)off,(double)md,(double)fa[0],(double)fa[1],(double)fa[2],(double)fa[3],(double)fb[0],(double)fb[1],(double)fb[2],(double)fb[3]);
    }

    kcd2vr::XrManager::StereoViewSnapshot xr{};
    if(kcd2vr::XrManager::Instance().CopyLastGoodStereoViews(xr) && xr.view_count==2) {
        Tracef("XR-FOV-V30 pair=%llu L=(left=%.7f right=%.7f up=%.7f down=%.7f) R=(left=%.7f right=%.7f up=%.7f down=%.7f) raw_ipd=%.6f age_ms=%llu",
            (unsigned long long)report,(double)xr.eye_fov[0].angleLeft,(double)xr.eye_fov[0].angleRight,(double)xr.eye_fov[0].angleUp,(double)xr.eye_fov[0].angleDown,
            (double)xr.eye_fov[1].angleLeft,(double)xr.eye_fov[1].angleRight,(double)xr.eye_fov[1].angleUp,(double)xr.eye_fov[1].angleDown,(double)xr.raw_ipd,
            (unsigned long long)((xr.captured_tick_ms && GetTickCount64()>=xr.captured_tick_ms)?GetTickCount64()-xr.captured_tick_ms:~std::uint64_t(0)));
    }
}

inline void __fastcall RendererConsumeViewRecordsDetour(void* self, void* records, std::uint32_t count, void* context, std::uintptr_t arg5) noexcept {
    const auto n=g_renderer_consume_records_calls.fetch_add(1,std::memory_order_relaxed)+1;
    const auto rec=reinterpret_cast<std::uintptr_t>(records);
    const auto target=g_native_eye_target.load(std::memory_order_acquire);
    bool target_match = target && rec==(target+0x9B30);
    std::uintptr_t tracked_owner=0;
    std::uint32_t tracked_mode=0;
    const char* tier="generic";
    if(target_match) {
        tracked_owner=target;
        tier="target";
    } else {
        for(auto& e:g_tracked_crv) {
            const auto cv=e.ptr.load(std::memory_order_acquire);
            if(cv && rec==(cv+0x9B30)) {
                tracked_owner=cv;
                tracked_mode=e.mode.load(std::memory_order_acquire);
                tier="tracked";
                break;
            }
        }
    }

    std::uint64_t q0=0,q1=0,q2=0,q3=0;
    float m190[4]{},m1a0[4]{},m1b0[4]{},m1c0[4]{};
    bool readable=false;
    __try {
        if(records && count>0) {
            auto* q=reinterpret_cast<std::uint64_t*>(records);
            q0=q[0]; q1=q[1]; q2=q[2]; q3=q[3];
            std::memcpy(m190,reinterpret_cast<void*>(rec+0x190),sizeof(m190));
            std::memcpy(m1a0,reinterpret_cast<void*>(rec+0x1A0),sizeof(m1a0));
            std::memcpy(m1b0,reinterpret_cast<void*>(rec+0x1B0),sizeof(m1b0));
            std::memcpy(m1c0,reinterpret_cast<void*>(rec+0x1C0),sizeof(m1c0));
            readable=true;
        }
    } __except(EXCEPTION_EXECUTE_HANDLER) { readable=false; }

    // Fallback logging ladder: exact selected target first; otherwise any known
    // CRenderView record; otherwise sparse generic samples so we can discover
    // a different visible owner without flooding the log.
    const bool log = target_match || (tracked_owner && (n<=8 || (n%240==0))) || n<=4 || (n%2400==0);
    if(log) {
        Tracef("RENDER-CONSUME-V29 call=%llu tier=%s self=%p records=%p count=%u context=%p arg5=0x%llX target=%p owner=%p mode=%u readable=%s q0=%016llX q1=%016llX q2=%016llX q3=%016llX m190=(%.4f,%.4f,%.4f,%.4f) m1a0=(%.4f,%.4f,%.4f,%.4f) m1b0=(%.4f,%.4f,%.4f,%.4f) m1c0=(%.4f,%.4f,%.4f,%.4f) tid=%lu",
            (unsigned long long)n,tier,self,records,(unsigned)count,context,(unsigned long long)arg5,(void*)target,(void*)tracked_owner,(unsigned)tracked_mode,readable?"true":"false",
            (unsigned long long)q0,(unsigned long long)q1,(unsigned long long)q2,(unsigned long long)q3,
            (double)m190[0],(double)m190[1],(double)m190[2],(double)m190[3],
            (double)m1a0[0],(double)m1a0[1],(double)m1a0[2],(double)m1a0[3],
            (double)m1b0[0],(double)m1b0[1],(double)m1b0[2],(double)m1b0[3],
            (double)m1c0[0],(double)m1c0[1],(double)m1c0[2],(double)m1c0[3],
            (unsigned long)GetCurrentThreadId());
    }
    if(readable && tracked_owner) V30CaptureAndDiff(n,tracked_owner,records,tracked_mode);
    if(g_renderer_consume_records_original)
        g_renderer_consume_records_original(self,records,count,context,arg5);
}

inline void __fastcall CRenderViewCameraPairBuildDetour(void* out_state, void* cam_a, void* cam_b, void* context, float arg5, float arg6) noexcept {
    const auto n=g_native_eye_build_calls.fetch_add(1,std::memory_order_relaxed)+1;
    const auto target=g_native_eye_target.load(std::memory_order_acquire);

    // Static proof: WHGame+0x865550 passes out_state = CRenderView+0x9B30
    // (first active slot), with RDX/R8 already pointing at the currently
    // selected 0x310 camera pair using index=(CRenderView+0x8EE0)&1.
    void* owner=nullptr;
    __try {
        if(out_state) owner=reinterpret_cast<void*>(
            reinterpret_cast<std::uintptr_t>(out_state)-0x9B30);
    } __except(EXCEPTION_EXECUTE_HANDLER) { owner=nullptr; }

    if(reinterpret_cast<std::uintptr_t>(owner)!=target ||
       ObjectVtableRva(owner)!=kCRenderViewVtableRva) {
        if(g_crv_camera_pair_build_original)
            g_crv_camera_pair_build_original(out_state,cam_a,cam_b,context,arg5,arg6);
        return;
    }

    if(!g_native_eye_write_enabled.load(std::memory_order_acquire)) {
        // Correlation confirmed the target, but the write gate is off - log
        // that this call WOULD have qualified, without touching memory, so
        // the correlation logic itself can be validated first.
        const auto seen=g_native_eye_build_calls.load(std::memory_order_relaxed);
        std::uintptr_t off_a=~std::uintptr_t(0),off_b=~std::uintptr_t(0);
        std::uint32_t selector=0xFFFFFFFFu;
        CameraTranslationBackup a{},b{};
        __try {
            const auto o=reinterpret_cast<std::uintptr_t>(owner);
            off_a=reinterpret_cast<std::uintptr_t>(cam_a)-o;
            off_b=reinterpret_cast<std::uintptr_t>(cam_b)-o;
            selector=*reinterpret_cast<std::uint32_t*>(o+0x8EE0);
            if(cam_a) { auto* f=reinterpret_cast<float*>(cam_a); a.x=f[3]; a.y=f[7]; a.z=f[11]; a.valid=true; }
            if(cam_b) { auto* f=reinterpret_cast<float*>(cam_b); b.x=f[3]; b.y=f[7]; b.z=f[11]; b.valid=true; }
        } __except(EXCEPTION_EXECUTE_HANDLER) { a.valid=false; b.valid=false; selector=0xFFFFFFFFu; }
        if(seen<=48 || seen%120==0) {
            Tracef("NATIVE-PAIR-V21 write-disabled pair_call=%llu child=%p selector=%u selected_index=%u out=%p camA=%p offA=0x%llX A=(%.3f,%.3f,%.3f) camB=%p offB=0x%llX B=(%.3f,%.3f,%.3f) context=%p arg5=%.6f arg6=%.6f tid=%lu",
                (unsigned long long)seen,owner,selector,selector&1u,out_state,
                cam_a,(unsigned long long)off_a,(double)a.x,(double)a.y,(double)a.z,
                cam_b,(unsigned long long)off_b,(double)b.x,(double)b.y,(double)b.z,
                context,(double)arg5,(double)arg6,(unsigned long)GetCurrentThreadId());
        }
        if(g_crv_camera_pair_build_original)
            g_crv_camera_pair_build_original(out_state,cam_a,cam_b,context,arg5,arg6);
        TraceNativePairOutputV17(seen,owner,out_state,cam_a,cam_b,context,arg5,arg6);
        return;
    }

    const auto v21_elapsed=V21ElapsedMs();
    if(v21_elapsed >= 0) {
        // Later V21 phases are owned by the broad 0x865550 hook. Keep this
        // exact pair hook observer-only so the fallback methods never stack.
        if(g_crv_camera_pair_build_original)
            g_crv_camera_pair_build_original(out_state,cam_a,cam_b,context,arg5,arg6);
        TraceNativePairOutputV17(n,owner,out_state,cam_a,cam_b,context,arg5,arg6);
        if(n<=48 || n%120==0)
            Tracef("NATIVE-PAIR-V21 observer pair_call=%llu elapsed_ms=%llu phase=%s child=%p tid=%lu",
                (unsigned long long)n,(unsigned long long)v21_elapsed,V21PhaseName(v21_elapsed),owner,(unsigned long)GetCurrentThreadId());
        return;
    }

    // XR fallback ladder: fresh XR -> cached XR -> relaxed raw XR -> fixed 64mm.
    // Existing head tracking already owns absolute HMD pose; only relative eye
    // separation is injected here, avoiding double application of head motion.
    const auto xr_eye=GetOpenXrStereoInjection();
    if(!xr_eye.valid) {
        if(n<=48 || n%120==0) {
            Tracef("NATIVE-PAIR-V21 stereo-source-unavailable pair_call=%llu child=%p out=%p arg5=%.6f arg6=%.6f tid=%lu",
                (unsigned long long)n,owner,out_state,(double)arg5,(double)arg6,(unsigned long)GetCurrentThreadId());
        }
        if(g_crv_camera_pair_build_original)
            g_crv_camera_pair_build_original(out_state,cam_a,cam_b,context,arg5,arg6);
        TraceNativePairOutputV17(n,owner,out_state,cam_a,cam_b,context,arg5,arg6);
        return;
    }

    const auto a=ShiftEmbeddedCameraRight(cam_a,xr_eye.left_offset);
    const auto b=ShiftEmbeddedCameraRight(cam_b,xr_eye.right_offset);

    std::uintptr_t off_a=~std::uintptr_t(0),off_b=~std::uintptr_t(0);
    __try {
        const auto o=reinterpret_cast<std::uintptr_t>(owner);
        off_a=reinterpret_cast<std::uintptr_t>(cam_a)-o;
        off_b=reinterpret_cast<std::uintptr_t>(cam_b)-o;
    } __except(EXCEPTION_EXECUTE_HANDLER) {}

    const auto applied=g_native_eye_applied_calls.fetch_add(1,std::memory_order_relaxed)+1;
    if(applied<=48 || applied%120==0) {
        Tracef("NATIVE-PAIR-V21 apply=%llu pair_call=%llu child=%p out=%p camA=%p offA=0x%llX beforeA=(%.3f,%.3f,%.3f) camB=%p offB=0x%llX beforeB=(%.3f,%.3f,%.3f) context=%p arg5=%.6f arg6=%.6f xr_ipd=%.6f left=%.6f right=%.6f source=%s age_ms=%llu view_count=%u flags=0x%llX locate_result=%d predicted=%lld tid=%lu",
            (unsigned long long)applied,(unsigned long long)n,owner,out_state,
            cam_a,(unsigned long long)off_a,(double)a.x,(double)a.y,(double)a.z,
            cam_b,(unsigned long long)off_b,(double)b.x,(double)b.y,(double)b.z,
            context,(double)arg5,(double)arg6,(double)xr_eye.ipd,(double)xr_eye.left_offset,(double)xr_eye.right_offset,xr_eye.source,
            (unsigned long long)xr_eye.age_ms,(unsigned)xr_eye.view_count,(unsigned long long)xr_eye.view_state_flags,(int)xr_eye.locate_result,
            (long long)xr_eye.predicted_display_time,(unsigned long)GetCurrentThreadId());
    }

    if(g_crv_camera_pair_build_original)
        g_crv_camera_pair_build_original(out_state,cam_a,cam_b,context,arg5,arg6);

    TraceNativePairOutputV17(n,owner,out_state,cam_a,cam_b,context,arg5,arg6);
    RestoreEmbeddedCameraTranslation(cam_a,a);
    RestoreEmbeddedCameraTranslation(cam_b,b);
}

inline void FlushDeferredCRenderViewSnapshot(const char* trigger) noexcept {
    auto pending = g_deferred_crv_snapshot;
    if(!pending.crv) return;
    g_deferred_crv_snapshot = {};

    const auto vt = ObjectVtableRva(pending.crv);
    if(vt != kCRenderViewVtableRva) {
        Tracef("CRV-DEFERRED invalid call=%llu trigger=%s crv=%p vt=0x%llX",
            (unsigned long long)pending.checkout_call, trigger ? trigger : "?",
            pending.crv, (unsigned long long)vt);
        return;
    }

    const auto source = SafeReadPtrAt(pending.crv,0x38);
    char source_rtti[192]{};
    ObjectRttiName(reinterpret_cast<void*>(source),source_rtti,sizeof(source_rtti));
    std::uint32_t s40[4]{},s50[4]{};
    SafeReadBytesAt(pending.crv,0x8D40,s40,sizeof(s40));
    SafeReadBytesAt(pending.crv,0x8D50,s50,sizeof(s50));

    const auto view_camera=g_last_published_view_camera.load(std::memory_order_acquire);
    const auto child_cam_ptr_off=FindExactPointerOffset(pending.crv,0xA770,view_camera);
    const auto source_cam_ptr_off=FindExactPointerOffset(reinterpret_cast<void*>(source),0xA770,view_camera);
    const auto child_cam_matrix_off=FindCameraMatrixOffset(pending.crv,0xA770,reinterpret_cast<void*>(view_camera));
    const auto source_cam_matrix_off=FindCameraMatrixOffset(reinterpret_cast<void*>(source),0xA770,reinterpret_cast<void*>(view_camera));

    if(pending.route==1 && source_cam_matrix_off==0x8EE8 && source &&
       ObjectVtableRva(reinterpret_cast<void*>(source))==kCRenderViewVtableRva) {
        std::uintptr_t zero=0;
        const auto pv=reinterpret_cast<std::uintptr_t>(pending.crv);
        if(g_native_eye_target.compare_exchange_strong(zero,pv,std::memory_order_acq_rel)) {
            g_native_eye_source.store(source,std::memory_order_release);
            Tracef("PARENT-ISOLATE-V27 selected-A child=%p parentA=%p route=main-4E6C source_camera_match=0x8EE8 experiment=isolated-consumed-parent-causality",
                pending.crv,(void*)source);
        } else {
            const auto a=g_native_eye_source.load(std::memory_order_acquire);
            if(source!=a) {
                std::uintptr_t zero_b=0;
                if(g_native_eye_source_b.compare_exchange_strong(zero_b,source,std::memory_order_acq_rel)) {
                    Tracef("PARENT-ISOLATE-V27 selected-B child=%p parentB=%p parentA=%p route=main-4E6C source_camera_match=0x8EE8 experiment=isolated-consumed-parent-causality",
                        pending.crv,(void*)source,(void*)a);
                }
            }
        }
    }

    Tracef("CRV-CANDIDATE post-config call=%llu trigger=%s route=%s crv=%p mode=%u checkout_caller=0x%llX p18=%p source38=%p source_vt=0x%llX source_rtti='%s' view_camera=%p child_cam_ptr_off=0x%llX source_cam_ptr_off=0x%llX child_cam_matrix_off=0x%llX source_cam_matrix_off=0x%llX state8D40=%08X,%08X,%08X,%08X state8D50=%08X,%08X,%08X,%08X",
        (unsigned long long)pending.checkout_call, trigger ? trigger : "?",
        pending.route==2?"alternate-4E73":pending.route==1?"main-4E6C":"unknown",
        pending.crv, pending.mode, (unsigned long long)pending.checkout_caller,
        (void*)SafeReadPtrAt(pending.crv,0x18), (void*)source,
        (unsigned long long)ObjectVtableRva(reinterpret_cast<void*>(source)), source_rtti,
        (void*)view_camera,
        (unsigned long long)child_cam_ptr_off,(unsigned long long)source_cam_ptr_off,
        (unsigned long long)child_cam_matrix_off,(unsigned long long)source_cam_matrix_off,
        s40[0],s40[1],s40[2],s40[3],s50[0],s50[1],s50[2],s50[3]);
}

inline void* __fastcall CRenderViewManagerCheckoutObserver(void* manager) noexcept {
    FlushDeferredCRenderViewSnapshot("next-checkout");
    const auto n=g_crv_checkout_calls.fetch_add(1,std::memory_order_relaxed)+1;
    const auto cr=CallerRva();
    const auto provider=SafeReadPtrAt(manager,0x90);
    const auto begin_before=SafeReadPtrAt(manager,0x08);
    const auto end_before=SafeReadPtrAt(manager,0x10);
    const auto cap_before=SafeReadPtrAt(manager,0x18);
    const auto count_before=SafeVectorCount(manager,0x08,0x10);
    const auto state50_before=SafeReadU32At(manager,0x50);
    char provider_rtti[192]{}; ObjectRttiName((void*)provider,provider_rtti,sizeof(provider_rtti));
    if(n<=96 || n%300==0) Tracef(
        "CRV-CHECKOUT-OBS enter call=%llu caller=0x%llX manager=%p provider=%p provider_vt=0x%llX provider_rtti='%s' vec=[%p,%p,%p] count=%llu state50=%u tid=%lu",
        (unsigned long long)n,(unsigned long long)cr,manager,(void*)provider,
        (unsigned long long)ObjectVtableRva((void*)provider),provider_rtti,
        (void*)begin_before,(void*)end_before,(void*)cap_before,
        (unsigned long long)count_before,state50_before,(unsigned long)GetCurrentThreadId());

    void* ret=g_crv_checkout_original?g_crv_checkout_original(manager):nullptr;

    const auto begin_after=SafeReadPtrAt(manager,0x08);
    const auto end_after=SafeReadPtrAt(manager,0x10);
    const auto cap_after=SafeReadPtrAt(manager,0x18);
    const auto count_after=SafeVectorCount(manager,0x08,0x10);
    const auto state50_after=SafeReadU32At(manager,0x50);
    std::uintptr_t last=0;
    __try { if(end_after && end_after>=begin_after+8) last=*reinterpret_cast<std::uintptr_t*>(end_after-8); } __except(EXCEPTION_EXECUTE_HANDLER) { last=0; }
    char rtti[192]{}; ObjectRttiName(ret,rtti,sizeof(rtti));
    if(n<=96 || n%300==0) Tracef(
        "CRV-CHECKOUT-OBS leave call=%llu caller=0x%llX ret=%p vt=0x%llX rtti='%s' mode=%u vec=[%p,%p,%p] count=%llu last=%p registered=%s state50=%u tid=%lu",
        (unsigned long long)n,(unsigned long long)cr,ret,(unsigned long long)ObjectVtableRva(ret),rtti,
        SafeReadU32At(ret,0x14),(void*)begin_after,(void*)end_after,(void*)cap_after,
        (unsigned long long)count_after,(void*)last,(ret && last==(std::uintptr_t)ret)?"true":"false",
        state50_after,(unsigned long)GetCurrentThreadId());
    const auto mode_after=SafeReadU32At(ret,0x14);
    TrackCRenderView(ret,manager,cr,mode_after);
    if(ret && mode_after==2) {
        const auto route=TraceCheckoutStack(n,mode_after);
        g_deferred_crv_snapshot = {ret,n,cr,mode_after,route};
        Tracef("CRV-CANDIDATE queued call=%llu crv=%p mode=%u route=%s checkout_caller=0x%llX tid=%lu",
            (unsigned long long)n,ret,mode_after,
            route==2?"alternate-4E73":route==1?"main-4E6C":"unknown",
            (unsigned long long)cr,(unsigned long)GetCurrentThreadId());
    }
    return ret;
}

inline void __fastcall RendererWorldOrchestratorObserver(void* engine, void* pass, void* list_a, void* list_b) noexcept {
    const auto n=g_world_orchestrator_calls.fetch_add(1,std::memory_order_relaxed)+1;
    if(n<=64 || n%300==0) Tracef("CRV-DOWNSTREAM world-orchestrator enter call=%llu caller=0x%llX engine=%p engine_vt=0x%llX pass=%p pass_vt=0x%llX listA=%p listB=%p tid=%lu",
        (unsigned long long)n,(unsigned long long)CallerRva(),engine,(unsigned long long)ObjectVtableRva(engine),pass,(unsigned long long)ObjectVtableRva(pass),list_a,list_b,(unsigned long)GetCurrentThreadId());
    if(g_world_orchestrator_original) g_world_orchestrator_original(engine,pass,list_a,list_b);
    if(n<=64 || n%300==0) Tracef("CRV-DOWNSTREAM world-orchestrator leave call=%llu engine=%p pass=%p",(unsigned long long)n,engine,pass);
}

inline int __fastcall RenderPassViewQueryObserver(void* obj) noexcept {
    const auto n=g_renderpass_query_calls.fetch_add(1,std::memory_order_relaxed)+1;
    CorrelateRenderObject(obj);
    const int ret=g_renderpass_query_original?g_renderpass_query_original(obj):0;
    if(n<=128 || n%600==0) Tracef("CRV-DOWNSTREAM render-object-query call=%llu caller=0x%llX obj=%p vt=0x%llX exact_crv=%s ret=%d b1FD=%u u204=%u b224=%u u1548=0x%X u154C=0x%X u1550=0x%X tid=%lu",
        (unsigned long long)n,(unsigned long long)CallerRva(),obj,(unsigned long long)ObjectVtableRva(obj),ObjectVtableRva(obj)==kCRenderViewVtableRva?"true":"false",ret,
        SafeReadU32At(obj,0x1FD)&0xFFu,SafeReadU32At(obj,0x204),SafeReadU32At(obj,0x224)&0xFFu,SafeReadU32At(obj,0x1548),SafeReadU32At(obj,0x154C),SafeReadU32At(obj,0x1550),(unsigned long)GetCurrentThreadId());
    return ret;
}

inline void* __fastcall CRenderViewFactoryObserver(void* closure) noexcept {
    const auto n=g_crv_factory_calls.fetch_add(1,std::memory_order_relaxed)+1;
    const auto mode=SafeReadU32At(closure,0x8);
    const auto owner=SafeReadPtrAt(closure,0x10);
    Tracef("CRV-FACTORY-OBS enter call=%llu caller=0x%llX closure=%p mode=%u captured_owner=%p tid=%lu",
        (unsigned long long)n,(unsigned long long)CallerRva(),closure,mode,(void*)owner,(unsigned long)GetCurrentThreadId());
    void* ret=g_crv_factory_original ? g_crv_factory_original(closure) : nullptr;
    char rtti[192]{}; ObjectRttiName(ret,rtti,sizeof(rtti));
    Tracef("CRV-FACTORY-OBS leave call=%llu ret=%p vt=0x%llX rtti='%s' mode_field=%u",
        (unsigned long long)n,ret,(unsigned long long)ObjectVtableRva(ret),rtti,SafeReadU32At(ret,0x14));
    return ret;
}

inline void __fastcall ViewSystemUpdateDetour(void* self,float dt) noexcept {
    auto n=g_vs_calls.fetch_add(1)+1; auto cr=CallerRva(); CountCaller(g_vs_callers,16,cr);
    std::uintptr_t active=0,list=0,sys=0; __try { if(self){list=*reinterpret_cast<std::uintptr_t*>(reinterpret_cast<std::uintptr_t>(self)+0x20); sys=*reinterpret_cast<std::uintptr_t*>(reinterpret_cast<std::uintptr_t>(self)+0x18); auto** vt=*reinterpret_cast<void***>(self); if(vt&&vt[7]){using F=void*(__fastcall*)(void*);active=reinterpret_cast<std::uintptr_t>(reinterpret_cast<F>(vt[7])(self));}} } __except(EXCEPTION_EXECUTE_HANDLER){}
    if(n<=48||n%600==0) Tracef("MULTIHOP CVIEWSYS call=%llu caller=0x%llX self=%p vt=0x%llX dt=%.6f system=%p system_vt=0x%llX active=%p list=%p tid=%lu",(unsigned long long)n,(unsigned long long)cr,self,(unsigned long long)ObjectVtableRva(self),dt,(void*)sys,(unsigned long long)ObjectVtableRva((void*)sys),(void*)active,(void*)list,(unsigned long)GetCurrentThreadId());
    if(g_vs_original) g_vs_original(self,dt);
}

inline void __fastcall ActiveCameraHandoffDetour(void* cv,void* cam) noexcept {
    auto n=g_handoff_calls.fetch_add(1)+1; auto cr=CallerRva(); CountCaller(g_handoff_callers,16,cr); std::uintptr_t ent=0; __try{if(cv)ent=*reinterpret_cast<std::uintptr_t*>(reinterpret_cast<std::uintptr_t>(cv)+0x418);}__except(EXCEPTION_EXECUTE_HANDLER){}
    if(n<=48||n%600==0) Tracef("MULTIHOP LINKED-ENTITY call=%llu caller=0x%llX cview=%p camera=%p linked_entity=%p entity_vt=0x%llX",(unsigned long long)n,(unsigned long long)cr,cv,cam,(void*)ent,(unsigned long long)ObjectVtableRva((void*)ent));
    if(g_handoff_original) g_handoff_original(cv,cam);
}

inline void __fastcall EntityCameraApplyDetour(void* ent,void* cam,std::uint32_t flags) noexcept {
    auto n=g_entity_apply_calls.fetch_add(1)+1; auto cr=CallerRva(); CountCaller(g_entity_apply_callers,16,cr); std::uintptr_t node=0,node_payload=0; __try{if(ent){node=*reinterpret_cast<std::uintptr_t*>(reinterpret_cast<std::uintptr_t>(ent)+0x98);if(node)node_payload=*reinterpret_cast<std::uintptr_t*>(node+0x18);}}__except(EXCEPTION_EXECUTE_HANDLER){}
    if(n<=48||n%600==0) Tracef("MULTIHOP ENTITY-CAMERA call=%llu caller=0x%llX entity=%p vt=0x%llX camera=%p flags=0x%X node=%p node_payload=%p",(unsigned long long)n,(unsigned long long)cr,ent,(unsigned long long)ObjectVtableRva(ent),cam,flags,(void*)node,(void*)node_payload);
    if(g_entity_apply_original) g_entity_apply_original(ent,cam,flags);
}

inline void __fastcall EntityCameraFallbackDetour(void* ent,void* cam,std::uint32_t flags) noexcept {
    auto n=g_entity_fallback_calls.fetch_add(1)+1; auto cr=CallerRva(); CountCaller(g_entity_fallback_callers,16,cr);
    if(n<=32||n%600==0) Tracef("MULTIHOP ENTITY-FALLBACK call=%llu caller=0x%llX entity=%p camera=%p flags=0x%X",(unsigned long long)n,(unsigned long long)cr,ent,cam,flags);
    if(g_entity_fallback_original) g_entity_fallback_original(ent,cam,flags);
}

inline void __fastcall SystemSetViewCameraDetour(void* system,void* cam) noexcept {
    auto n=g_set_view_camera_calls.fetch_add(1)+1; auto cr=CallerRva(); CountCaller(g_set_view_camera_callers,16,cr); float x,y,z,a,b,c,d; CameraSummary(cam,x,y,z,a,b,c,d);
    void* expected_storage = system ? reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(system)+0x288) : nullptr;
    if(n<=96||n%300==0) Tracef("MULTIHOP PUBLISH-VIEW-CAMERA call=%llu caller=0x%llX system=%p system_vt=0x%llX camera=%p expected_storage=%p pos=(%.3f,%.3f,%.3f) diag=(%.3f,%.3f,%.3f) extra=%.3f tid=%lu",(unsigned long long)n,(unsigned long long)cr,system,(unsigned long long)ObjectVtableRva(system),cam,expected_storage,x,y,z,a,b,c,d,(unsigned long)GetCurrentThreadId());
    if(g_set_view_camera_original) g_set_view_camera_original(system,cam);
    if(expected_storage) g_last_published_view_camera.store(reinterpret_cast<std::uintptr_t>(expected_storage),std::memory_order_release);
    FlushDeferredCRenderViewSnapshot("camera-publish");
}

inline void* __fastcall SystemGetViewCameraDetour(void* system) noexcept {
    auto n=g_get_view_camera_calls.fetch_add(1)+1; auto cr=CallerRva(); CountCaller(g_get_view_camera_callers,32,cr);
    auto* bucket=FindCallerBucket(g_get_view_camera_callers,32,cr);
    TraceStackOnce(bucket,cr);
    if(cr>=0x669C88ull && cr<0x66A029ull) TraceFrameNexusOnce();
    void* ret=g_get_view_camera_original?g_get_view_camera_original(system):nullptr; float x,y,z,a,b,c,d; CameraSummary(ret,x,y,z,a,b,c,d);
    if(n<=128||n%600==0) Tracef("MULTIHOP GET-VIEW-CAMERA call=%llu caller=0x%llX label=%s system=%p system_vt=0x%llX ret=%p expected=%p pos=(%.3f,%.3f,%.3f) tid=%lu",(unsigned long long)n,(unsigned long long)cr,ConsumerLabel(cr),system,(unsigned long long)ObjectVtableRva(system),ret,system?reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(system)+0x288):nullptr,x,y,z,(unsigned long)GetCurrentThreadId());
    return ret;
}

inline void* __fastcall CViewCtorDetour(void* self,void* owner) noexcept {
    auto n=g_cview_ctor_calls.fetch_add(1)+1; auto cr=CallerRva(); CountCaller(g_cview_ctor_callers,16,cr);
    void* ret=g_cview_ctor_original?g_cview_ctor_original(self,owner):self;
    if(n<=64) { std::uintptr_t linked=0,stored_owner=0; __try{if(self){stored_owner=*reinterpret_cast<std::uintptr_t*>(reinterpret_cast<std::uintptr_t>(self)+0x3F8);linked=*reinterpret_cast<std::uintptr_t*>(reinterpret_cast<std::uintptr_t>(self)+0x418);}}__except(EXCEPTION_EXECUTE_HANDLER){}
        Tracef("MULTIHOP CVIEW-CTOR call=%llu caller=0x%llX self=%p ret=%p owner_arg=%p stored_owner=%p owner_vt=0x%llX linked_entity=%p cview_vt=0x%llX",(unsigned long long)n,(unsigned long long)cr,self,ret,owner,(void*)stored_owner,(unsigned long long)ObjectVtableRva((void*)stored_owner),(void*)linked,(unsigned long long)ObjectVtableRva(self)); }
    return ret;
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
inline void StartFocusedStereoChaseV61_20() noexcept {
    using namespace detail;
    TraceStereoFrameworkStaticOnce();
    const auto base=WhBase();
    if(base) {
        void* obj=nullptr; std::uintptr_t vt=0,target=0;
        ResolveStereoGateCandidateV34(base,obj,vt,target);
        char rtti[160]{}; ObjectRttiName(obj,rtti,sizeof(rtti));
        Tracef("V61.21-STEREO STATIC gate_global=0x%llX object=%p rtti='%s' gate_slot=0x%llX gate_target_rva=0x%llX pipeline=0x%llX leftEyeString=0x%llX leftEyeRegister=0x%llX leftEyeStore=0x%llX",
            (unsigned long long)kStereoGateObjectGlobalRva,obj,rtti,
            (unsigned long long)kStereoGateVslot,(unsigned long long)ToRva(target),
            (unsigned long long)kGraphicsPipelineExecuteRva,
            (unsigned long long)kVolCloudLeftEyeStringRva,
            (unsigned long long)kVolCloudLeftEyeRegisterSiteRva,
            (unsigned long long)kVolCloudLeftEyeHandleStoreSiteRva);
    }
    debug_trace::Write("V61.21-STEREO static-only: CSystem+0x598 retired; g_stereoFrameworkEnable already defaults/live=1; VolCloudLeftEye is a RESOURCE NAME, not proof of scene-eye identity; no hot stereo detours");
}

inline void ScanLoadedModules(){ using namespace detail; if(!WhBase()){debug_trace::Write("MULTIHOP WHGame.dll missing");return;} Tracef("MULTIHOP BEGIN observer-only hooks: CViewSystem=0x%llX linkedEntity=0x%llX entityApply=0x%llX entityFallback=0x%llX PublishViewCamera=0x%llX GetViewCamera=0x%llX CViewCtor=0x%llX",(unsigned long long)kViewSystemUpdateRva,(unsigned long long)kActiveCameraHandoffRva,(unsigned long long)kEntityCameraApplyRva,(unsigned long long)kEntityCameraFallbackRva,(unsigned long long)kSystemSetViewCameraRva,(unsigned long long)kSystemGetViewCameraRva,(unsigned long long)kCViewCtorRva);
#define INST(TAG,RVA,DET,HOOK,ORIG) debug_trace::Write(Install(TAG,RVA,DET,HOOK,ORIG) ? "MULTIHOP hook enabled: " TAG : "MULTIHOP hook FAILED: " TAG)
    TraceStereoFrameworkStaticOnce();
    debug_trace::Write(InstallStereoGateCandidateV34() ? "MULTIHOP hook enabled: CSystem boolean semantic probe V35" : "MULTIHOP hook FAILED: CSystem boolean semantic probe V35");
    INST("Renderer float-CVar registration V32",kRegisterFloatCVarRva,&RegisterFloatCVarDetour,g_register_float_cvar_hook,g_register_float_cvar_original);
    INST("Stereo/renderer integer-CVar registration V32",kRegisterIntCVarRva,&RegisterIntCVarDetour,g_register_int_cvar_hook,g_register_int_cvar_original);
    INST("CViewSystem::Update",kViewSystemUpdateRva,&ViewSystemUpdateDetour,g_vs_hook,g_vs_original);
    INST("CView linked entity helper",kActiveCameraHandoffRva,&ActiveCameraHandoffDetour,g_handoff_hook,g_handoff_original);
    INST("CEntity camera transform",kEntityCameraApplyRva,&EntityCameraApplyDetour,g_entity_apply_hook,g_entity_apply_original);
    INST("CEntity camera fallback",kEntityCameraFallbackRva,&EntityCameraFallbackDetour,g_entity_fallback_hook,g_entity_fallback_original);
    INST("CSystem publish ViewCamera thunk",kSystemSetViewCameraRva,&SystemSetViewCameraDetour,g_set_view_camera_hook,g_set_view_camera_original);
    INST("CSystem GetViewCamera",kSystemGetViewCameraRva,&SystemGetViewCameraDetour,g_get_view_camera_hook,g_get_view_camera_original);
    INST("CView constructor",kCViewCtorRva,&CViewCtorDetour,g_cview_ctor_hook,g_cview_ctor_original);
    INST("C3DEngine camera handoff",0x4E46ACull,&C3DEngineCameraDetour,g_c3dengine_camera_hook,g_c3dengine_camera_original);
    INST("CRenderView constructor observer",kCRenderViewCtorRva,&CRenderViewCtorObserver,g_crv_ctor_hook,g_crv_ctor_original);
    INST("CRenderView factory observer",kCRenderViewFactoryRva,&CRenderViewFactoryObserver,g_crv_factory_hook,g_crv_factory_original);
    INST("CRenderView manager checkout observer",kCRenderViewManagerCheckoutRva,&CRenderViewManagerCheckoutObserver,g_crv_checkout_hook,g_crv_checkout_original);
    // V29 deliberately retires the old camera-offset experiments. This run is renderer-resource observation only.
    INST("Renderer consume CRenderView records V30 differential observer",kRendererConsumeViewRecordsRva,&RendererConsumeViewRecordsDetour,g_renderer_consume_records_hook,g_renderer_consume_records_original);
    INST("CGraphicsPipeline stage dispatcher V29",kGraphicsPipelineExecuteRva,&GraphicsPipelineExecuteDetour,g_graphics_pipeline_execute_hook,g_graphics_pipeline_execute_original);
    INST("CSceneGBufferStage Execute V29",kSceneGBufferStageExecuteRva,&SceneGBufferStageExecuteDetour,g_scene_gbuffer_execute_hook,g_scene_gbuffer_execute_original);
    INST("GBuffer MRT setup V29 six-arg ABI",kGBufferMrtSetupRva,&GBufferMrtSetupDetour,g_gbuffer_mrt_setup_hook,g_gbuffer_mrt_setup_original);
    Tracef("PIPELINE-V36 multi-theory read-only sweep: CSystem+0x598 retained only as retired semantic control; tests VolCloud stereo reprojection/LeftEye anchor, engine-global RTTI singletons, CStandardGraphicsPipeline embedded objects, and small enum-like pipeline state diffs; no return/CVar/camera/projection writes");
#undef INST
}

inline void Shutdown(){ using namespace detail; if(g_stereo_gate_candidate_hook){(void)g_stereo_gate_candidate_hook->disable();g_stereo_gate_candidate_hook.reset();} if(g_register_float_cvar_hook){(void)g_register_float_cvar_hook->disable();g_register_float_cvar_hook.reset();} if(g_register_int_cvar_hook){(void)g_register_int_cvar_hook->disable();g_register_int_cvar_hook.reset();} if(g_gbuffer_mrt_setup_hook){(void)g_gbuffer_mrt_setup_hook->disable();g_gbuffer_mrt_setup_hook.reset();} if(g_scene_gbuffer_execute_hook){(void)g_scene_gbuffer_execute_hook->disable();g_scene_gbuffer_execute_hook.reset();} if(g_graphics_pipeline_execute_hook){(void)g_graphics_pipeline_execute_hook->disable();g_graphics_pipeline_execute_hook.reset();} if(g_crv_camera_build_hook){(void)g_crv_camera_build_hook->disable();g_crv_camera_build_hook.reset();} if(g_crv_camera_pair_build_hook){(void)g_crv_camera_pair_build_hook->disable();g_crv_camera_pair_build_hook.reset();} if(g_renderpass_query_hook){(void)g_renderpass_query_hook->disable();g_renderpass_query_hook.reset();} if(g_world_orchestrator_hook){(void)g_world_orchestrator_hook->disable();g_world_orchestrator_hook.reset();} if(g_crv_checkout_hook){(void)g_crv_checkout_hook->disable();g_crv_checkout_hook.reset();} if(g_crv_factory_hook){(void)g_crv_factory_hook->disable();g_crv_factory_hook.reset();} if(g_crv_ctor_hook){(void)g_crv_ctor_hook->disable();g_crv_ctor_hook.reset();} Tracef("STEREO-FRAMEWORK-V31 summary register_calls=%llu storage=%p result=%p",(unsigned long long)g_register_int_cvar_calls.load(),(void*)g_stereo_framework_storage.load(),(void*)g_stereo_framework_result.load()); for(auto& slot:g_v32_float_cvars) Tracef("STEREO-RENDERER-V32 summary type=float name='%s' registrations=%llu storage=%p object=%p value=%.9f",slot.label,(unsigned long long)slot.registrations.load(),(void*)slot.storage.load(),(void*)slot.object.load(),(double)SafeReadFloat((void*)slot.storage.load())); for(auto& slot:g_v32_int_cvars) Tracef("STEREO-RENDERER-V32 summary type=int name='%s' registrations=%llu storage=%p object=%p value=%d",slot.label,(unsigned long long)slot.registrations.load(),(void*)slot.storage.load(),(void*)slot.object.load(),(int)SafeReadI32((void*)slot.storage.load())); Tracef("STEREO-GATE-V36-control summary calls=%llu true=%llu false=%llu",(unsigned long long)g_stereo_gate_candidate_calls.load(),(unsigned long long)g_stereo_gate_candidate_true.load(),(unsigned long long)g_stereo_gate_candidate_false.load()); Tracef("MULTIHOP summary vs=%llu link=%llu entity=%llu fallback=%llu publish=%llu getview=%llu ctor=%llu c3d=%llu native_eye_build=%llu native_eye_applied=%llu",(unsigned long long)g_vs_calls.load(),(unsigned long long)g_handoff_calls.load(),(unsigned long long)g_entity_apply_calls.load(),(unsigned long long)g_entity_fallback_calls.load(),(unsigned long long)g_set_view_camera_calls.load(),(unsigned long long)g_get_view_camera_calls.load(),(unsigned long long)g_cview_ctor_calls.load(),(unsigned long long)g_c3dengine_camera_calls.load(),(unsigned long long)g_native_eye_build_calls.load(),(unsigned long long)g_native_eye_applied_calls.load()); Dump("CVIEWSYS",g_vs_callers,16);Dump("LINK",g_handoff_callers,16);Dump("ENTITY",g_entity_apply_callers,16);Dump("FALLBACK",g_entity_fallback_callers,16);Dump("PUBLISH",g_set_view_camera_callers,16);Dump("GETVIEW",g_get_view_camera_callers,32);Dump("CTOR",g_cview_ctor_callers,16);
    auto off=[](auto& h){if(h){(void)h->disable();h.reset();}}; off(g_c3dengine_camera_hook);off(g_cview_ctor_hook);off(g_get_view_camera_hook);off(g_set_view_camera_hook);off(g_entity_fallback_hook);off(g_entity_apply_hook);off(g_handoff_hook);off(g_vs_hook); debug_trace::Write("MULTIHOP END"); }

} // namespace kcd2vr::stereo_archaeology
