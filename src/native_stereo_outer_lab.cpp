#include "native_stereo_outer_lab.h"
#include "frik_pose_probe.h"
#include "native_stereo_preflight_v6602.h"
#include "stereo_single_run_plan.h"
#include "debug_trace.h"
#include "diagnostic_trace.h"
#include "device_hook.h"
#include "frame_sync.h"
#include "camera_hook.h"

#include <DetourModKit/hook.hpp>
#include <windows.h>
#include <intrin.h>
#include <atomic>
#include <optional>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <utility>
#include <array>
#include <cmath>
#include <mutex>

namespace kcd2vr::native_stereo_outer_lab {
namespace {

constexpr std::uint32_t kSupportedTimestamp = 0x6A350E20u;
constexpr std::uintptr_t kFrameNexusRva = 0x669C88ull;
constexpr std::uintptr_t kCCryActionVtableRva = 0x40472D0ull;
constexpr std::size_t kCCryActionFrameNexusSlot = 0x58 / sizeof(void*);
constexpr std::uintptr_t kPhase7e8CallsiteRva = 0x669D48ull;
constexpr std::uintptr_t kCameraHandoffCallsiteRva = 0x669DF3ull;
constexpr std::uintptr_t kRendererPhaseCallsiteRva = 0x669E60ull;
constexpr std::uintptr_t kPhase7f8CallsiteRva = 0x669FB9ull;
constexpr std::uintptr_t kEngineGlobalRva = 0x492D808ull;
constexpr std::uintptr_t kGetOrCreateRenderViewRva = 0x43EB60ull;
constexpr std::uintptr_t kCRenderViewCameraBuildRva = 0x865550ull;
constexpr std::uintptr_t kCRenderViewCameraPairBuildRva = 0x866F6Cull;
// Legacy identifier retained: re-audit identifies +0x57278C as pipeline Update, not geometry Execute.
constexpr std::uintptr_t kGraphicsPipelineExecuteRva = 0x57278Cull;
constexpr std::uintptr_t kMultiviewPacketDispatchRva = 0x42DA44ull;
constexpr std::uintptr_t kBackendSlot68Rva = 0x42DCDCull;
constexpr std::uintptr_t kBackendExecRva = 0x42DE94ull;
constexpr std::uintptr_t kBackendRangeRva = 0x42E050ull;
constexpr std::uintptr_t kSecondaryGateCheckRva = 0xF7516Cull;
constexpr std::uintptr_t kSecondaryStageApplyRva = 0x864A70ull;
constexpr std::uintptr_t kSecondaryShaderModeRva = 0x7C5A20ull;
// V66.31 exact-build RTTI/static recovery: post-FX ownership probes.
constexpr std::uintptr_t kV6631CloudReadinessRva = 0x578714ull;      // CVolumetricCloudsStage resource/backing readiness
constexpr std::uintptr_t kV6631CloudShadowSetupRva = 0x56C18Cull;    // cloud-local shadow inputs, selector*0x170 state
constexpr std::uintptr_t kV6631CloudEyeSetupRva = 0x64F46Cull;       // cloud per-eye setup, selector*0x170 state
constexpr std::uintptr_t kV6631ShadowMapStageVtableRva = 0x3DFEE00ull;
constexpr std::uintptr_t kV6631ShadowMaskStageVtableRva = 0x3C28480ull;
constexpr std::uintptr_t kV6639ShadowMaskExecuteRva = 0x4EAB1Cull; // CShadowMaskStage::Execute, vslot +0x20
// V66.39R1 exact stage entry points recovered from the retail vtables/disassembly.
// ShadowMap gate already rejects selector=1, so RIGHT map generation is natively shared.
// The proof lab therefore targets LEFT/primary ShadowMap::Update inputs only.
constexpr std::uintptr_t kV6639R1ShadowMapUpdateRva = 0xD9A3B0ull;  // CShadowMapStage::Update, vslot +0x18
constexpr std::uintptr_t kV6639R1ShadowMapExecuteRva = 0xD98CB0ull; // CShadowMapStage::Execute, vslot +0x20
// Exact post-refresh consumer tail-called by ShadowMap::Update. This is the first
// instruction sequence that reads the proven CRenderView matrix/camera fields,
// after Update's native +0x4E893C refresh has already completed.
constexpr std::uintptr_t kV6639R1ShadowMapConsumeRva = 0xD9A3E8ull;
constexpr std::uintptr_t kV6639R1SsrExecuteRva = 0x115AA00ull;     // CScreenSpaceReflectionsStage::Execute, vslot +0x20
constexpr std::uintptr_t kV6631VolCloudStageVtableRva = 0x3D2C920ull;
constexpr std::uintptr_t kV6631PostAAStageVtableRva = 0x3C3FE80ull;
constexpr std::uintptr_t kV6631ScreenSpaceReflectionsStageVtableRva = 0x3D2CB28ull;
constexpr std::uintptr_t kTxnCommandRva = 0x1DF78B0ull;
constexpr std::uintptr_t kTxnSetViewRva = 0x7C2D40ull;
constexpr std::uintptr_t kTxnPipelineRva = 0x864728ull;
constexpr std::uintptr_t kTxnFinalizeRva = 0x86466Cull;
// V65.99 REAUDIT: +0x7BB6B0 is CSceneGBufferStage::Update, NOT Execute.
// RTTI identifies the class only. The actual GBUFFER draw orchestrator is +0x7C2110.
constexpr std::uintptr_t kSceneGBufferUpdateRva = 0x7BB6B0ull;
constexpr std::uintptr_t kSceneGBufferVtableRva = 0x3DFCD80ull;
constexpr std::uintptr_t kSceneGBufferCtorRva = 0x741A48ull;
constexpr std::uintptr_t kModeResultAssignRva = 0x5740ECull;
constexpr std::uintptr_t kMode0Assign0ReturnRva = 0x575BB7ull;
constexpr std::uintptr_t kMode0Assign1ReturnRva = 0x575C14ull;
constexpr std::uintptr_t kMode2Assign0ReturnRva = 0x575C7Aull;
constexpr std::uintptr_t kMode2Assign1ReturnRva = 0x575CD7ull;
constexpr std::uintptr_t kModeQuartetDispatchReturnRva = 0x575F02ull;
// V65.83: exact return addresses after the three engine packet-channel dispatches.
// These all call the shared +0x42DA44 backend dispatcher, but originate from
// distinct persistent state channels: +118 (0x60), +110 (0x30), +108 (variable).
constexpr std::uintptr_t kChannel118DispatchReturnRva = 0x575F02ull;
constexpr std::uintptr_t kChannel110DispatchReturnRva = 0x576113ull;
constexpr std::uintptr_t kChannel108DispatchReturnRva = 0x578BB1ull;
constexpr unsigned char kBackendExecPrologue[] = {0x40,0x53,0x55,0x56,0x57,0x41,0x54,0x41};
constexpr unsigned char kBackendRangePrologue[] = {0x40,0x53,0x55,0x56,0x57,0x41,0x54,0x41};

constexpr unsigned char kSecondaryGateCheckPrologue[] = {
    0x48,0x8B,0x41,0x08,0xF7,0x80,0x68,0x19,0x00,0x00,0x00,0x40,0x00,0x00
};
constexpr unsigned char kSecondaryStageApplyPrologue[] = {
    0x48,0x8B,0xC4,0x55,0x53,0x56,0x57,0x41,0x54,0x41,0x55,0x41,0x56,0x41,0x57
};
constexpr unsigned char kSecondaryShaderModePrologue[] = {
    0x48,0x89,0x5C,0x24,0x08,0x4C,0x89,0x44,0x24,0x18,0x55,0x56,0x57,0x41,0x54
};
constexpr unsigned char kV6631CloudReadinessPrologue[] = {
    0x48,0x8B,0xC4,0x48,0x89,0x58,0x08,0x48,0x89,0x68,0x10,0x48,0x89,0x70,0x18,0x48,0x89,0x78,0x20,0x41,0x56,0x48,0x83,0xEC,0x20
};
constexpr unsigned char kV6631CloudShadowSetupPrologue[] = {
    0x48,0x89,0x5C,0x24,0x08,0x48,0x89,0x6C,0x24,0x10,0x48,0x89,0x74,0x24,0x18,0x57,0x41,0x56,0x41,0x57,0x48,0x83,0xEC,0x50
};
constexpr unsigned char kV6639ShadowMaskExecutePrologue[] = {
    0x40,0x55,0x53,0x56,0x57,0x41,0x54,0x41,0x55,0x41,0x56,0x41,0x57,0x48,0x8D,0x6C,0x24,0xE1
};
constexpr unsigned char kV6639R1ShadowMapUpdatePrologue[] = {
    0x48,0x89,0x5C,0x24,0x08,0x57,0x48,0x83,0xEC,0x20,0x48,0x8B,0x59,0x28,0x48,0x8B,0xF9
};
constexpr unsigned char kV6639R1ShadowMapExecutePrologue[] = {
    0x48,0x8B,0xC4,0x48,0x89,0x58,0x08,0x48,0x89,0x68,0x10,0x48,0x89,0x70,0x18,0x48,0x89,0x78,0x20
};
constexpr unsigned char kV6639R1ShadowMapConsumePrologue[] = {
    0x48,0x8B,0x82,0x60,0x8D,0x00,0x00,0x4C,0x8B,0xC9,0x4C,0x8B,0x80,0x40,0x08,0x00,0x00
};
constexpr unsigned char kV6639R1SsrExecutePrologue[] = {
    0x48,0x83,0xEC,0x38,0x48,0x8B,0x41,0x20,0x0F,0x57,0xC0,0x8B,0x90,0x34,0x03,0x00,0x00
};
constexpr unsigned char kV6631CloudEyeSetupPrologue[] = {
    0x48,0x89,0x5C,0x24,0x10,0x48,0x89,0x6C,0x24,0x18,0x48,0x89,0x74,0x24,0x20,0x57,0x41,0x54,0x41,0x55,0x41,0x56,0x41,0x57,0x48,0x83,0xEC,0x50
};

constexpr unsigned char kModeResultAssignPrologue[] = {
    0x48,0x89,0x5C,0x24,0x10,0x57,0x48,0x83,0xEC,0x20,0x48,0x8B,0x02
};
constexpr unsigned char kTxnCommandPrologue[] = { 0x40,0x53,0x48,0x83,0xEC,0x20,0x48,0x8B,0xD9 };
constexpr unsigned char kTxnSetViewPrologue[] = { 0x48,0x89,0x5C,0x24,0x18,0x48,0x89,0x6C,0x24,0x20,0x56,0x57,0x41,0x56 };
constexpr unsigned char kTxnPipelinePrologue[] = { 0x48,0x89,0x5C,0x24,0x18,0x55,0x56,0x57,0x41,0x56,0x41,0x57 };
constexpr unsigned char kTxnFinalizePrologue[] = { 0x48,0x8B,0xC4,0x48,0x89,0x58,0x08,0x48,0x89,0x68,0x10,0x48,0x89,0x70,0x18 };
constexpr unsigned char kSceneGBufferUpdatePrologue[] = {
    0x48,0x89,0x5C,0x24,0x08,0x48,0x89,0x74,0x24,0x10,0x57,0x48,0x83,0xEC,0x30
};

constexpr std::uintptr_t kNViewDispatchReturnRva = 0x654E37ull; // exact return after +0x653940 call +0x654E32 -> +0x42DA44
constexpr std::uintptr_t kCSystemRenderRva = 0x43E464ull;
// V65.54: exact retail CSystem system-level render transaction owner.
// CSystem vslot +0x50 is called by normal CCryAction frame code at +0x6675D9,
// immediately before the render section. The wrapper itself calls the already-known
// CD3D9Renderer::BeginFrame (+0x52EF84) and then performs additional system/renderer
// frame initialization. This is the Crysis-style RenderBegin layer that was missing
// from the raw duplicate-render experiments.
constexpr std::uintptr_t kCSystemVtableRva = 0x3DC2858ull;
constexpr std::uintptr_t kCSystemRenderBeginRva = 0x52EA10ull;
constexpr std::uintptr_t kCSystemRenderEndRva = 0x9A7AE0ull;
// V65.55a: exact retail CSystem view-camera owner. These two high vtable slots are
// normal KCD2 APIs, not guessed CRenderView fields: +0x438 returns CSystem+0x288;
// +0x430 assigns a full 0x310-byte camera through WHGame+0x426E80, which preserves
// the camera's internal owned/reference-counted member semantics.
constexpr std::uintptr_t kCSystemSetViewCameraRva = 0x7F0AE0ull;
constexpr std::uintptr_t kCSystemGetViewCameraRva = 0x5D77F0ull;
constexpr std::size_t kSystemViewCameraSize = 0x310;
constexpr std::uintptr_t kCSystemRenderBeginNormalReturnRva = 0x6675DCull;
constexpr std::uintptr_t kCSystemRenderBeginCallsiteRva = 0x6675C2ull;
// V65.52 exact retail CRenderView destruction/lifetime chain. These addresses are
// exact-build audited against WHGame timestamp 0x6A350E20 and are diagnostic only.
constexpr std::uintptr_t kCRenderViewDtorBodyRva = 0xBBFB3Cull;
constexpr std::uintptr_t kCRenderViewDeletingDtorRva = 0xBC01F0ull;
constexpr std::uintptr_t kCRenderViewResetRva = 0x427C1Cull;
constexpr std::uintptr_t kCRenderViewFaultWriteRva = 0x427CB7ull; // mov [rsi+0x8DB0], rax; BugSplat MFA resolves inside this instruction
constexpr std::uintptr_t kCRenderViewVtableRva = 0x3DF25C0ull;
// V65.43 exact retail CD3D9Renderer::BeginFrame recovered from the literal
// "******************************* BeginFrame %d ********************************".
// The sole direct retail caller at +0x25260AA passes RCX=renderer object,
// RDX=&16-byte frame-state stack local, R8=frame/context pointer. Static retail
// disassembly proves BeginFrame consumes exactly the 16-byte RDX payload and the
// first 32-bit value at R8 along this path. Never retain either caller pointer:
// snapshot the values and replay them from KCD2VR-owned storage between eyes.
constexpr std::uintptr_t kRendererBeginFrameRva = 0x52EF84ull;
constexpr std::uintptr_t kIConsoleGlobalRva = 0x492D8A8ull;
constexpr std::uintptr_t kRendererGlobalsRva = 0x547DB60ull;
constexpr std::size_t kCameraRecordStride = 0x310;
constexpr std::size_t kCameraBankA = 0x8EE8;
constexpr std::size_t kCameraBankB = 0x9508;
constexpr std::size_t kBuiltOutput0 = 0x9B30;
constexpr std::size_t kBuiltOutputStride = 0x400;
constexpr std::size_t kBuiltRecordCount = 0xA330;
constexpr std::size_t kCameraHandoffSlot = 0x40 / sizeof(void*);
constexpr std::size_t kPhase7e8Slot = 0x7E8 / sizeof(void*);
constexpr std::size_t kPhase7f8Slot = 0x7F8 / sizeof(void*);

// Exact WHGame(10).dll gate. This is the CCryAction virtual body recovered by
// static RE and is intentionally narrower than the retired DoFrame/GetView lab.
constexpr unsigned char kFrameNexusPrologue[] = {
    0x41,0xF6,0xC0,0x20,0x0F,0x85,0x65,0x03,0x00,0x00,
    0x48,0x8B,0xC4,0x48,0x89,0x58,0x08,0x48,0x89,0x70,0x10
};
constexpr unsigned char kPhase7e8Callsite[] = {
    0x48,0x8B,0x0D,0xB9,0x3A,0x2C,0x04,0x48,0x8B,0x01,0xFF,0x90,0xE8,0x07,0x00,0x00
};
constexpr unsigned char kCameraHandoffCallsite[] = {
    0x48,0x8B,0x3D,0x0E,0x3A,0x2C,0x04,0x48,0x8B,0x4E,0x48,0x48,0x8B,0x07,
    0xC7,0x45,0xC0,0x00,0x00,0x00,0x00,0x48,0x8B,0x58,0x40
};
constexpr unsigned char kRendererPhaseCallsite[] = {
    0x48,0x8B,0x0D,0xA1,0x3A,0x2C,0x04,0x48,0x85,0xC9,0x74,0x09,0x48,0x8B,0x01,
    0xFF,0x90,0x80,0x03,0x00,0x00
};
constexpr unsigned char kPhase7f8Callsite[] = {
    0x48,0x8B,0x0D,0x48,0x38,0x2C,0x04,0x48,0x8B,0x01,0xFF,0x90,0xF8,0x07,0x00,0x00
};
constexpr unsigned char kGetOrCreatePrologue[] = {
    0x48,0x89,0x5C,0x24,0x08,0x57,0x48,0x83,0xEC,0x30,
    0x48,0x63,0xC2,0x48,0x81,0xC1,0xD8,0x23,0x00,0x00
};
constexpr unsigned char kCRenderViewCameraBuildPrologue[] = {
    0x48,0x8B,0xC4,0x48,0x89,0x58,0x08,0x48,0x89,0x68,0x10,
    0x48,0x89,0x70,0x18,0x48,0x89,0x78,0x20,0x41,0x56,0x48,0x83,0xEC,0x30
};
constexpr unsigned char kCRenderViewCameraPairBuildPrologue[] = {
    0x48,0x8B,0xC4,0x4C,0x89,0x48,0x20,0x55,0x53,0x56,0x57,0x41,0x54,0x41,0x56,0x41,0x57,
    0x48,0x8D,0xA8,0xB8,0xFA,0xFF,0xFF,0x48,0x81,0xEC,0x10,0x06,0x00,0x00
};
// V65.56 exact downstream native-multiview consumer. One normal
// CStandardGraphicsPipeline execution loads the built-view array at +0x9B30
// and its count at +0xA330, then forwards both to WHGame+0x653940.
constexpr std::uintptr_t kGraphicsPipelineMultiviewLoadRva = 0x5727AFull;
constexpr unsigned char kGraphicsPipelineMultiviewLoad[] = {
    0x4C,0x8B,0x87,0x78,0x19,0x00,0x00,
    0x4C,0x8B,0xC8,0x48,0x8B,0xCF,
    0x48,0xC7,0x44,0x24,0x20,0x00,0x00,0x00,0x00,
    0x49,0x8D,0x90,0x30,0x9B,0x00,0x00,
    0x45,0x8B,0x80,0x30,0xA3,0x00,0x00,
    0xE8,0x68,0x11,0x0E,0x00
};

constexpr unsigned char kMultiviewPacketDispatchPrologue[] = {
    0x48,0x8B,0xC4,0x48,0x89,0x58,0x08,0x48,0x89,0x70,0x10,0x48,0x89,0x78,0x18,0x4C,0x89,0x48,0x20,0x55,0x41,0x54,0x41,0x55,0x41,0x56,0x41,0x57
};
constexpr unsigned char kCSystemRenderPrologue[] = {
    0x48,0x8B,0xC4,0x48,0x89,0x58,0x08,0x48,0x89,0x70,0x10,0x48,0x89,0x78,0x18,
    0x55,0x41,0x56,0x41,0x57,0x48,0x8D,0x68,0xA1,0x48,0x81,0xEC,0xC0,0x00,0x00,0x00
};
constexpr unsigned char kCSystemRenderBeginPrologue[] = {
    0x40,0x53,0x48,0x83,0xEC,0x20,0x80,0xB9,0xC2,0x05,0x00,0x00,0x00,
    0x48,0x8B,0xD9,0x0F,0x85,0x83,0x00,0x00,0x00,0x48,0x8B,0x41,0x20
};
constexpr unsigned char kCSystemRenderBeginCallsite[] = {
    0x48,0x8B,0x4F,0x48,
    0x4C,0x8B,0x84,0x24,0x90,0x00,0x00,0x00,
    0x48,0x8B,0x94,0x24,0x88,0x00,0x00,0x00,
    0x48,0x8B,0x01,0xFF,0x50,0x50
};
constexpr unsigned char kCSystemGetViewCameraPrologue[] = {
    0x48,0x8D,0x81,0x88,0x02,0x00,0x00,0xC3
};
constexpr unsigned char kCSystemSetViewCameraPrologue[] = {
    0x48,0x81,0xC1,0x88,0x02,0x00,0x00,0xE9,0x94,0x63,0xC3,0xFF
};
constexpr unsigned char kCRenderViewDtorBodyPrologue[] = {
    0x48,0x89,0x5C,0x24,0x08,0x48,0x89,0x74,0x24,0x10,0x48,0x89,0x7C,0x24,0x18,0x41,0x56,0x48,0x83,0xEC,0x20
};
constexpr unsigned char kCRenderViewDeletingDtorPrologue[] = {
    0x48,0x89,0x5C,0x24,0x08,0x57,0x48,0x83,0xEC,0x20,0x8B,0xDA,0x48,0x8B,0xF9
};
constexpr unsigned char kCRenderViewResetPrologue[] = {
    0x48,0x8B,0xC4,0x48,0x89,0x58,0x10,0x48,0x89,0x70,0x18,0x48,0x89,0x78,0x20,0x55,0x41,0x54,0x41,0x55,0x41,0x56,0x41,0x57
};
constexpr unsigned char kRendererBeginFramePrologue[] = {
    0x40,0x53,0x55,0x56,0x57,0x41,0x56,0x48,0x81,0xEC,0x80,0x03,0x00,0x00,
    0x48,0x8B,0x05,0x27,0x00,0x3D,0x04
};

using FrameNexusFn = void(__fastcall*)(void* self, std::uintptr_t unused_arg, std::uint32_t flags);
using CameraHandoffFn = void(__fastcall*)(void* engine, void* camera, std::uint32_t* out_state);
using GetOrCreateRenderViewFn = void*(__fastcall*)(void* renderer, int view_type);
using Phase7e8Fn = void*(__fastcall*)(void* engine);
using Phase7f8Fn = void(__fastcall*)(void* engine);
using CRenderViewCameraBuildFn = void(__fastcall*)(void* self);
using CRenderViewCameraPairBuildFn = void(__fastcall*)(void* out_state,void* cam_a,void* cam_b,void* context,float arg5,float arg6);
using CSystemRenderFn = void(__fastcall*)(void* self, void* render_params);
using CSystemRenderBeginFn = void(__fastcall*)(void* self, void* frame_state, void* frame_context);
using CSystemGetViewCameraFn = void*(__fastcall*)(void* self);
using CSystemSetViewCameraFn = void(__fastcall*)(void* self, const void* camera);
using CRenderViewDtorBodyFn = void(__fastcall*)(void* self);
using CRenderViewDeletingDtorFn = void*(__fastcall*)(void* self, unsigned int flags);
using CRenderViewResetFn = void(__fastcall*)(void* self);
using RendererBeginFrameFn = void(__fastcall*)(void* renderer, void* frame_state, void* frame_context);
using MultiviewPacketDispatchFn = bool(__fastcall*)(void* owner, void* records, std::uint32_t stride, void* arg4, std::uint32_t count);
using BackendExecFn = void(__fastcall*)(void* ctx, std::uintptr_t arg2, std::uint32_t start, std::uint32_t count, std::uint32_t flags, std::uintptr_t arg6);
using BackendRangeFn = int(__fastcall*)(void* ctx, std::uintptr_t arg2, const void* range, std::uintptr_t arg4);
using SecondaryGateCheckFn = bool(__fastcall*)(void* stage);
using SecondaryStageApplyFn = void(__fastcall*)(void* stage);
using SecondaryShaderModeFn = bool(__fastcall*)(void* owner, void* desc, void* view, std::uint8_t mode, void* out);
using V6631CloudReadinessFn = bool(__fastcall*)(void* stage);
using V6639ShadowMaskExecuteFn = void(__fastcall*)(void* stage);
using V6639R1ShadowMapStageFn = void(__fastcall*)(void* stage);
using V6639R1ShadowMapConsumeFn = void(__fastcall*)(void* shadow_state, void* render_view);
using V6639R1SsrExecuteFn = void(__fastcall*)(void* stage);
using V6631CloudSetupFn = void(__fastcall*)(void* stage, void* desc);
using ModeResultAssignFn = void*(__fastcall*)(void* dst, const void* src);
using TxnCommandFn = void(__fastcall*)(void* command);
using TxnViewFn = void(__fastcall*)(void* owner, void* render_view);
using SceneGBufferUpdateFn = void(__fastcall*)(void* stage);

std::optional<DetourModKit::hook::Hook> g_nexus_hook;
std::optional<DetourModKit::hook::Hook> g_camera_hook;
std::optional<DetourModKit::hook::Hook> g_get_or_create_hook;
std::optional<DetourModKit::hook::Hook> g_phase7e8_hook;
std::optional<DetourModKit::hook::Hook> g_phase7f8_hook;
std::optional<DetourModKit::hook::Hook> g_stereo_build_hook;
std::optional<DetourModKit::hook::Hook> g_camera_pair_hook;
std::optional<DetourModKit::hook::Hook> g_csystem_render_hook;
std::optional<DetourModKit::hook::Hook> g_csystem_render_begin_hook;
std::optional<DetourModKit::hook::Hook> g_crv_dtor_body_hook;
std::optional<DetourModKit::hook::Hook> g_crv_deleting_dtor_hook;
std::optional<DetourModKit::hook::Hook> g_crv_reset_hook;
std::optional<DetourModKit::hook::Hook> g_renderer_begin_frame_hook;
std::optional<DetourModKit::hook::Hook> g_multiview_packet_dispatch_hook;
std::optional<DetourModKit::hook::Hook> g_backend_exec_hook;
std::optional<DetourModKit::hook::Hook> g_backend_range_hook;
std::optional<DetourModKit::hook::Hook> g_secondary_gate_check_hook;
std::optional<DetourModKit::hook::Hook> g_secondary_stage_apply_hook;
std::optional<DetourModKit::hook::Hook> g_secondary_shader_mode_hook;
std::optional<DetourModKit::hook::Hook> g_v6631_cloud_readiness_hook;
std::optional<DetourModKit::hook::Hook> g_v6631_cloud_shadow_setup_hook;
std::optional<DetourModKit::hook::Hook> g_v6631_cloud_eye_setup_hook;
std::optional<DetourModKit::hook::Hook> g_v6639_shadowmask_execute_hook;
std::optional<DetourModKit::hook::Hook> g_v6639r1_shadowmap_update_hook;
std::optional<DetourModKit::hook::Hook> g_v6639r1_shadowmap_execute_hook;
std::optional<DetourModKit::hook::Hook> g_v6639r1_shadowmap_consume_hook;
std::optional<DetourModKit::hook::Hook> g_v6639r1_ssr_execute_hook;
std::optional<DetourModKit::hook::Hook> g_mode_result_assign_hook;
std::optional<DetourModKit::hook::Hook> g_txn_command_hook;
std::optional<DetourModKit::hook::Hook> g_txn_set_view_hook;
std::optional<DetourModKit::hook::Hook> g_txn_pipeline_hook;
std::optional<DetourModKit::hook::Hook> g_txn_finalize_hook;
std::optional<DetourModKit::hook::Hook> g_scene_gbuffer_update_hook;

FrameNexusFn g_nexus_original = nullptr;
CameraHandoffFn g_camera_original = nullptr;
GetOrCreateRenderViewFn g_get_or_create_original = nullptr;
Phase7e8Fn g_phase7e8_original = nullptr;
Phase7f8Fn g_phase7f8_original = nullptr;
CRenderViewCameraBuildFn g_stereo_build_original = nullptr;
CRenderViewCameraPairBuildFn g_camera_pair_builder = nullptr;
CRenderViewCameraPairBuildFn g_camera_pair_original = nullptr;
CSystemRenderFn g_csystem_render_original = nullptr;
CSystemRenderBeginFn g_csystem_render_begin_original = nullptr;
CRenderViewDtorBodyFn g_crv_dtor_body_original = nullptr;
CRenderViewDeletingDtorFn g_crv_deleting_dtor_original = nullptr;
CRenderViewResetFn g_crv_reset_original = nullptr;
RendererBeginFrameFn g_renderer_begin_frame_original = nullptr;
MultiviewPacketDispatchFn g_multiview_packet_dispatch_original = nullptr;
BackendExecFn g_backend_exec_original = nullptr;
BackendRangeFn g_backend_range_original = nullptr;
SecondaryGateCheckFn g_secondary_gate_check_original = nullptr;
SecondaryStageApplyFn g_secondary_stage_apply_original = nullptr;
SecondaryShaderModeFn g_secondary_shader_mode_original = nullptr;
V6631CloudReadinessFn g_v6631_cloud_readiness_original = nullptr;
V6631CloudSetupFn g_v6631_cloud_shadow_setup_original = nullptr;
V6631CloudSetupFn g_v6631_cloud_eye_setup_original = nullptr;
V6639ShadowMaskExecuteFn g_v6639_shadowmask_execute_original = nullptr;
V6639R1ShadowMapStageFn g_v6639r1_shadowmap_update_original = nullptr;
V6639R1ShadowMapStageFn g_v6639r1_shadowmap_execute_original = nullptr;
V6639R1ShadowMapConsumeFn g_v6639r1_shadowmap_consume_original = nullptr;
V6639R1SsrExecuteFn g_v6639r1_ssr_execute_original = nullptr;
std::atomic<std::uint64_t> g_v6639_shadowmask_eye0{0}, g_v6639_shadowmask_eye1{0};
std::atomic<std::uint64_t> g_v6639r1_shadowmap_update_calls{0},g_v6639r1_shadowmap_execute_calls{0},g_v6639r1_shadowmap_consume_calls{0};
std::atomic<std::uint64_t> g_v6639r1_ssr_eye0{0},g_v6639r1_ssr_eye1{0};
std::atomic<int> g_v6639r1_pairwide_step{0}; // 0 census, 1 center shadow, 2 SSR census, 3 center shadow + SSR census

// Only the fields statically proven to be read by the post-refresh ShadowMap
// consumer at WHGame+0xD9A3E8 are ever substituted. ShadowMap::Update first
// refreshes the CRenderView at +0x4E893C and then tail-calls this exact consumer.
// Snapshot publication and consumption occur on the same render thread.
struct V6639R1CenterShadowSnapshot {
    std::uintptr_t crv{};
    std::uint32_t frame{};
    std::uint64_t center_record_hash{};
    std::uint64_t source_camera_hash{};
    bool valid{};
    std::array<unsigned char,12> block9b40{}; // 0x9B40..0x9B4B
    std::array<unsigned char,20> block9b64{}; // 0x9B64..0x9B77
    std::array<unsigned char,64> block9bc0{}; // 0x9BC0..0x9BFF
    std::array<unsigned char,64> block9cc0{}; // 0x9CC0..0x9CFF
};
thread_local V6639R1CenterShadowSnapshot g_v6639r1_center_shadow{};
ModeResultAssignFn g_mode_result_assign_original = nullptr;
TxnCommandFn g_txn_command_original = nullptr;
TxnViewFn g_txn_set_view_original = nullptr;
TxnViewFn g_txn_pipeline_original = nullptr;
TxnViewFn g_txn_finalize_original = nullptr;
SceneGBufferUpdateFn g_scene_gbuffer_update_original = nullptr;
std::atomic<int> g_v6594_diag_mode{0}; // 0 off, 1 observer, 2 COUNT2 block1 control, 3 replay +7C2D40, 4 replay +1DF78B0
std::atomic<std::uint64_t> g_v6595_replay_calls{0};
std::atomic<std::uint64_t> g_v6596_pair_serial{1};
thread_local bool g_v6595_replay_eye1 = false;
thread_local bool g_v6595_replay_guard = false;
thread_local void* g_v6595_last_txn_owner = nullptr;
thread_local void* g_v6595_last_txn_crv = nullptr;
// V65.97: exact COUNT2 block1 snapshot retained on the render thread so a replay
// can explicitly re-upload right-eye CBPerViewGlobal even when the replayed
// transaction does not revisit +42DA44/+653940.
alignas(16) thread_local unsigned char g_v6597_block1_copy[0x470]{};
thread_local void* g_v6597_block1_owner = nullptr;
thread_local void* g_v6597_block1_arg4 = nullptr;
thread_local bool g_v6597_block1_ready = false;
std::atomic<std::uint64_t> g_v6597_block1_snapshots{0};
std::atomic<std::uint64_t> g_v6594_txn_command_calls{0};
std::atomic<std::uint64_t> g_v6594_txn_setview_calls{0};
std::atomic<std::uint64_t> g_v6594_txn_pipeline_calls{0};
std::atomic<std::uint64_t> g_v6594_txn_finalize_calls{0};
std::atomic<std::uint64_t> g_v6594_block1_forces{0};
// V66.38: narrow Crysis-style inter-eye reset experiment. This is deliberately
// independent from old outer-render replay modes.
std::atomic<bool> g_v6638_intereye_renderbegin_reset{false};
// Legacy V65.98 counter names retained; +7BB6B0 observes GBuffer Update/setup.
// These counters do NOT establish opaque-geometry execution.
// The V65.99 include supplies separate Execute/opaque/requested-range probes.
std::atomic<std::uint64_t> g_v6598_gbuffer_calls{0};
std::atomic<std::uint64_t> g_v6598_gbuffer_replay_calls{0};
std::atomic<std::uint64_t> g_v6598_gbuffer_mode3_calls{0};
std::atomic<std::uint64_t> g_v6598_gbuffer_mode4_calls{0};
std::atomic<std::uint32_t> g_v6565_gate_calls{0};
std::atomic<std::uint32_t> g_v6565_apply_calls{0};
std::atomic<std::uint32_t> g_v6565_mode_calls{0};
std::atomic<std::uint32_t> g_v6565_mode2_calls{0};
std::atomic<std::uint32_t> g_v6565_secondary_flag_seen{0};
std::atomic<bool> g_v6626_native_stage_census{false};
// V66.36 F11: cloud-state ownership diagnostic. The V66.35 broad RIGHT
// mode0->mode2 forcing is retired because normal LEFT itself legitimately mixes modes.
// 0 baseline, 1 observe, 2 clone LEFT cloud-eye state to RIGHT,
// 3 clone LEFT cloud-shadow state to RIGHT, 4 clone both.
std::atomic<int> g_v6635_secondary_view_ladder{0};
std::atomic<std::uint64_t> g_v6635_secondary_forces{0}; // retained as mutation counter
unsigned char g_v6636_cloud_eye_left[0x170]{};
unsigned char g_v6636_cloud_shadow_left[0x170]{};
std::atomic<bool> g_v6636_cloud_eye_left_valid{false};
std::atomic<bool> g_v6636_cloud_shadow_left_valid{false};
std::atomic<std::uint64_t> g_v6626_stage_apply_eye0{0};
std::atomic<std::uint64_t> g_v6626_stage_apply_eye1{0};
std::atomic<std::uint64_t> g_v6626_stage_gate_eye0{0};
std::atomic<std::uint64_t> g_v6626_stage_gate_eye1{0};
std::atomic<std::uint64_t> g_v6626_mode_eye0{0};
std::atomic<std::uint64_t> g_v6626_mode_eye1{0};
std::atomic<std::uint64_t> g_v6631_cloud_ready_eye0{0};
std::atomic<std::uint64_t> g_v6631_cloud_ready_eye1{0};
std::atomic<std::uint64_t> g_v6631_cloud_ready_fail_eye0{0};
std::atomic<std::uint64_t> g_v6631_cloud_ready_fail_eye1{0};
std::atomic<std::uint64_t> g_v6631_cloud_eye_setup_eye0{0};
std::atomic<std::uint64_t> g_v6631_cloud_eye_setup_eye1{0};
std::atomic<std::uint64_t> g_v6631_cloud_shadow_setup_eye0{0};
std::atomic<std::uint64_t> g_v6631_cloud_shadow_setup_eye1{0};
// V66.27 F2 rung2: lightweight capture/replay of the exact retail +0x7C2D40
// pre-pipeline transaction. Old V65.97 proved this function can be replayed
// without issuing GBuffer geometry; it is upstream preparation, not the render
// itself. Only exact normal-frame owner/CRV/thread matches are eligible.
std::atomic<bool> g_v6627_right_prep_enabled{false};
std::atomic<std::uintptr_t> g_v6627_setview_owner{0};
std::atomic<std::uintptr_t> g_v6627_setview_crv{0};
std::atomic<unsigned long> g_v6627_setview_tid{0};
std::atomic<std::uint64_t> g_v6627_setview_captures{0};
std::atomic<std::uint64_t> g_v6627_right_prep_calls{0};
std::atomic<std::uint64_t> g_v6627_right_prep_skips{0};
thread_local bool g_v6627_right_prep_replay=false;
thread_local int g_v6626_last_stage_selector=-1;
// V66.28: always-on, cheap per-eye work counters (atomic increments only, no logging
// here). The V66.26 census logs are switched off in F2 stage 1/2, but the hooks stay
// installed, so these counters let mod.cpp log LEFT vs RIGHT work every few seconds.
std::atomic<std::uint64_t> g_v6628_mode_eye[2]{};
std::atomic<std::uint64_t> g_v6628_gate_eye[2]{};
std::atomic<std::uint32_t> g_v6566_assign_calls{0};
std::atomic<std::uint32_t> g_v6566_quartet_dispatch_calls{0};
std::atomic<std::uint32_t> g_v6567_fingerprint_calls{0};
std::atomic<std::uintptr_t> g_v6567_seen_objects[8]{};
std::atomic<std::uint32_t> g_v6567_quartet_snapshots{0};
std::atomic<std::uint32_t> g_v6568_provenance_lines{0};
std::atomic<std::uintptr_t> g_v6568_seen_ptrs[32]{};
std::atomic<std::uint32_t> g_v6569_child_payloads{0};
std::atomic<std::uintptr_t> g_v6569_seen_children[8]{};
std::atomic<std::uint32_t> g_v6571_q8_fingerprints{0};
std::atomic<std::uintptr_t> g_v6571_seen_q8[8]{};
// V65.81: actual compiled shader fingerprints recovered from the proven
// MODE0/MODE2 child q8/q9 DXBC fields. Multiple hashes per lane are expected
// because several renderer objects/permutations can exist simultaneously.
std::atomic<std::uint64_t> g_v6581_mode0_ps_hashes[16]{};
std::atomic<std::uint64_t> g_v6581_mode2_ps_hashes[16]{};
// V65.83: bounded read-only census of the three statically proven state packet
// channels. Counters reset on each LEFT/RIGHT/COUNT2 oracle activation.
std::atomic<std::uint32_t> g_v6583_ch108_calls{0};
std::atomic<std::uint32_t> g_v6583_ch110_calls{0};
std::atomic<std::uint32_t> g_v6583_ch118_calls{0};
// V65.84: retain up to four distinct +108 packet snapshots per oracle stage.
// Each +108 packet is 0x300 bytes in the observed path, i.e. 24 x 0x20 entries.
// LEFT/RIGHT snapshots are then used to classify COUNT2 entries without any writes.
std::mutex g_v6584_108_mutex;
std::array<std::array<std::array<std::uint64_t,24>,4>,4> g_v6584_108_entry_hashes{};
std::array<std::array<std::uint64_t,4>,4> g_v6584_108_full_hashes{};
std::array<unsigned,4> g_v6584_108_snapshot_count{};
thread_local bool g_v6564_backend_scope=false;
thread_local int g_v6564_backend_stage=0;
thread_local std::uint32_t g_v6564_exec_calls=0;
thread_local std::uint32_t g_v6564_range_calls=0;

// Forward declarations: V65.17 boundary hook is defined before the legacy helper
// implementations below, so MSVC needs these names declared before use.
bool EnsureNexusHook(bool physicalOnly=false) noexcept;
bool EnsureGetOrCreateHook() noexcept;

std::atomic<int> g_mode{0};
std::atomic<int> g_bridge_stage{0};
std::atomic<int> g_v6573_sbs_stage{0};
std::atomic<std::uint64_t> g_v6573_sbs_packets{0};
std::atomic<std::uint64_t> g_v6573_sbs_split_ok{0};
std::atomic<std::uint64_t> g_v6573_sbs_split_refused{0};
std::atomic<bool> g_v6574_extent_sane{false};
std::atomic<std::uint64_t> g_v6574_extent_logs{0};
// V65.75: stable rec0-vs-rec1 differential census. Read-only; never mutates view records.
std::atomic<std::uint32_t> g_v6575_source_samples{0};
std::atomic<std::uint32_t> g_v6575_xform_samples{0};
std::array<std::atomic<std::uint16_t>, 0x400/4> g_v6575_source_diff_hits{};
std::array<std::atomic<std::uint16_t>, 0x470/4> g_v6575_xform_diff_hits{};
// V65.60: claim exactly one transformed native-view packet.  +0x653940 emits
// a stride=0x470/count=2 packet for the proven native pair, but +0x42DA44 is a
// generic dispatcher used by many other paired packet types.  Claim the first
// matching 0x470 packet only after the active multiview pipeline has been seen.
std::atomic<bool> g_v6560_packet_claimed{false};
std::atomic<std::uintptr_t> g_v6560_packet_owner{0};
std::atomic<std::uintptr_t> g_v6560_packet_records{0};
std::atomic<std::uintptr_t> g_v6560_packet_object{0};
std::atomic<std::uintptr_t> g_v6560_packet_backend{0};
std::atomic<int> g_bridge_pass_eye{0}; // V65.38: -1 LEFT, +1 RIGHT during same-frame dual pass
std::atomic<std::uint64_t> g_bridge_dual_calls{0};
std::atomic<std::uint64_t> g_bridge_dual_copies{0};
std::atomic<std::uint64_t> g_bridge_right_renders{0};
std::atomic<std::uint64_t> g_bridge_left_copy_misses{0};
std::atomic<std::uint64_t> g_bridge_policy_failures{0};
// V65.45: read-only CRenderView lifetime telemetry. The old raw-continuous crash
// resolves inside CRenderView cleanup, so snapshot identity/refcount/build state
// at the already-proven camera-build hook without adding any new engine writes.
std::atomic<std::uintptr_t> g_bridge_left_crv{0};
std::atomic<std::uintptr_t> g_bridge_right_crv{0};
std::atomic<std::uint32_t> g_bridge_left_ref{0};
std::atomic<std::uint32_t> g_bridge_right_ref{0};
std::atomic<std::uint32_t> g_bridge_left_selector{0};
std::atomic<std::uint32_t> g_bridge_right_selector{0};
std::atomic<std::uint32_t> g_bridge_left_built_before{0};
std::atomic<std::uint32_t> g_bridge_right_built_before{0};
std::atomic<std::uint32_t> g_bridge_left_built_after{0};
std::atomic<std::uint32_t> g_bridge_right_built_after{0};
std::atomic<bool> g_bridge_pair_latched{false}; // retained compatibility; V65.41 continuous ladder leaves this false
std::atomic<int> g_second_eye_postfx_stage{0};
std::atomic<int> g_tangible_stage{0}; // V65.51 F9 certified same-frame XR-IPD ladder
std::atomic<std::uint64_t> g_tangible_txn_counter{0};
std::atomic<std::uint64_t> g_tangible_left_cert_miss{0};
std::atomic<std::uint64_t> g_tangible_right_cert_miss{0};
std::atomic<std::uint64_t> g_tangible_cert_pairs{0};
// V65.55a system-level RenderBegin snapshot/replay. Unlike V65.43, this never
// calls CD3D9Renderer::BeginFrame directly. We detour and replay the owning
// CSystem vslot +0x50 wrapper with value-copied current-frame inputs.
std::atomic<bool> g_csystem_render_begin_installed{false};
std::atomic<std::uintptr_t> g_sysbegin_self{0};
std::atomic<std::uint64_t> g_sysbegin_state_lo{0};
std::atomic<std::uint64_t> g_sysbegin_state_hi{0};
std::atomic<std::uint32_t> g_sysbegin_context_value{0};
std::atomic<std::uint32_t> g_sysbegin_tid{0};
std::atomic<std::uint64_t> g_sysbegin_tick_ms{0};
std::atomic<std::uint64_t> g_sysbegin_capture_serial{0};
std::atomic<std::uint64_t> g_sysbegin_last_used_serial{0};
std::atomic<std::uint64_t> g_sysbegin_injected_calls{0};
std::atomic<std::uintptr_t> g_sysbegin_last_caller_rva{0};
thread_local bool t_bridge_injected_sysbegin=false;

std::atomic<bool> g_renderer_begin_frame_installed{false};
std::atomic<std::uintptr_t> g_beginframe_renderer{0};
std::atomic<std::uint64_t> g_beginframe_state_lo{0};
std::atomic<std::uint64_t> g_beginframe_state_hi{0};
std::atomic<std::uint32_t> g_beginframe_context_value{0};
std::atomic<std::uint32_t> g_beginframe_tid{0};
std::atomic<std::uint64_t> g_beginframe_tick_ms{0};
std::atomic<std::uint64_t> g_beginframe_capture_serial{0};
std::atomic<std::uint64_t> g_beginframe_last_used_serial{0};
std::atomic<std::uint64_t> g_beginframe_injected_calls{0};
thread_local bool t_bridge_injected_beginframe=false;
std::atomic<bool> g_supported{false};
std::atomic<bool> g_lifetime_autopsy_installed{false};
std::atomic<std::uint64_t> g_lifetime_dtor_body_calls{0};
std::atomic<std::uint64_t> g_lifetime_deleting_dtor_calls{0};
std::atomic<std::uint64_t> g_lifetime_reset_calls{0};
std::atomic<std::uint64_t> g_lifetime_trace_events{0};
std::atomic<std::uint64_t> g_lifetime_last_duplicate_return_tick{0};
std::atomic<std::uint64_t> g_lifetime_last_duplicate_nexus{0};
std::atomic<std::uintptr_t> g_lifetime_last_duplicate_left_crv{0};
std::atomic<std::uintptr_t> g_lifetime_last_duplicate_right_crv{0};
thread_local int t_lifetime_outer_phase=0; // -1 LEFT original, +1 RIGHT original, 0 normal/post-return
struct LifetimeSeenSlot { std::atomic<std::uintptr_t> ptr{0}; std::atomic<std::uint32_t> dtor_entries{0}; };
constexpr std::size_t kLifetimeSeenSlots=64;
std::array<LifetimeSeenSlot,kLifetimeSeenSlots> g_lifetime_seen{};
std::atomic<bool> g_nexus_installed{false};
std::atomic<bool> g_camera_installed{false};
std::atomic<bool> g_get_or_create_installed{false};
std::atomic<bool> g_phase_hooks_installed{false};
std::atomic<bool> g_stereo_build_installed{false};
std::atomic<bool> g_camera_pair_installed{false};
std::atomic<bool> g_csystem_render_installed{false};
std::atomic<std::uint64_t> g_native_loop_promotions{0};
std::atomic<float> g_runtime_ipd{0.064f};
std::atomic<std::uint64_t> g_stereo_build_calls{0};
std::atomic<std::uint64_t> g_stereo_build_writes{0};
std::atomic<std::uint64_t> g_stereo_build_restores{0};
std::atomic<std::uint64_t> g_stereo_build_rejects{0};
std::atomic<float> g_last_applied_ipd{0.0f};
// V65.56 one-shot native multiview transaction. This does NOT duplicate
// CSystem::Render or CStandardGraphicsPipeline::Execute. It asks the retail
// CRenderView camera-build loop to publish two view records inside one owner.
std::atomic<bool> g_multiview_claimed{false};
std::atomic<bool> g_multiview_build_ok{false};
std::atomic<bool> g_multiview_pipeline_seen{false};
std::atomic<bool> g_multiview_pipeline_returned{false};
std::atomic<std::uintptr_t> g_multiview_crv{0};
std::atomic<std::uint32_t> g_multiview_built_count{0};
std::atomic<std::uint32_t> g_multiview_pair_calls{0};
std::atomic<bool> g_multiview_owned_pipeline_probe{false};
std::atomic<std::uint64_t> g_v6562a_oracle_frames{0};
std::atomic<std::uintptr_t> g_camera_target_rva{0};
std::atomic<std::uintptr_t> g_phase7e8_target_rva{0};
std::atomic<std::uintptr_t> g_phase7f8_target_rva{0};
std::atomic<std::uintptr_t> g_csystem_render_last_caller_rva{0};
std::atomic<std::uintptr_t> g_csystem_render_begin_target_rva{0};
std::atomic<std::uintptr_t> g_csystem_render_end_target_rva{0};

std::atomic<std::uint64_t> g_nexus_calls{0};
std::atomic<std::uint64_t> g_nexus_present_advances{0};
std::atomic<std::uint64_t> g_camera_calls{0};
std::atomic<std::uint64_t> g_camera_in_nexus{0};
std::atomic<std::uint64_t> g_get_or_create_calls{0};
std::atomic<std::uint64_t> g_get_or_create_in_nexus{0};
std::atomic<std::uint64_t> g_pipeline_calls{0};
std::atomic<std::uint64_t> g_pipeline_in_nexus{0};
std::atomic<std::uint64_t> g_normal_pipeline_in_nexus{0};
// V65.07: tiny F8-only cross-thread identity cache. GetOrCreateRenderView publishes
// recent returned pointers with their game-thread nexus serial; the existing
// pipeline observer then matches the exact pointer on the render thread. No game
// object is modified and no additional renderer hook is installed.
constexpr std::size_t kRenderViewHandoffSlots=64;
struct RenderViewHandoffSlot {
    std::atomic<std::uintptr_t> ptr{0};
    std::atomic<std::uint64_t> nexus{0};
    std::atomic<std::uint64_t> call{0};
    std::atomic<std::uint64_t> present{0};
    std::atomic<std::uint32_t> requested_type{0};
    std::atomic<std::uint32_t> producer_tid{0};
    std::atomic<std::uint64_t> publish_tick_ms{0};
    std::atomic<int> eye_phase{0}; // V65.53: -1 LEFT, +1 RIGHT, 0 ordinary
    std::atomic<std::uint64_t> pipeline_tick_ms{0};
    std::atomic<std::uint32_t> pipeline_tid{0};
    std::atomic<std::uint64_t> reset_tick_ms{0};
    std::atomic<std::uint32_t> reset_tid{0};
};
std::array<RenderViewHandoffSlot,kRenderViewHandoffSlots> g_handoff{};
std::atomic<std::uint64_t> g_handoff_matches{0};
std::atomic<std::uint64_t> g_handoff_misses{0};
std::atomic<std::uint64_t> g_last_handoff_nexus{0};
std::atomic<std::uintptr_t> g_last_render_view_token{0};
std::atomic<std::uint64_t> g_render_view_token_epochs{0};
std::atomic<std::uint64_t> g_render_view_token_reuses{0};
constexpr std::size_t kBoundarySlots=64;
struct BoundarySlot {
    std::atomic<std::uint64_t> serial{0};
    std::atomic<std::uint64_t> nexus{0};
    std::atomic<std::uintptr_t> caller_rva{0};
    std::atomic<std::uintptr_t> self{0};
    std::atomic<std::uintptr_t> params{0};
    std::atomic<std::uint64_t> entry_present{0};
    std::atomic<std::uint32_t> producer_tid{0};
};
std::array<BoundarySlot,kBoundarySlots> g_boundary{};
std::atomic<std::uint64_t> g_phase7e8_calls{0};
std::atomic<std::uint64_t> g_phase7f8_calls{0};
std::atomic<std::uint64_t> g_csystem_render_calls{0};
std::atomic<std::uint64_t> g_csystem_render_in_nexus{0};
std::atomic<std::uint64_t> g_csystem_render_pipeline_matches{0};
std::atomic<std::uint64_t> g_csystem_render_present_advances{0};
std::atomic<std::uint64_t> g_last_csystem_log_ms{0};
std::atomic<std::uint64_t> g_last_nexus_serial{0};
std::atomic<std::uint64_t> g_last_camera_nexus_serial{0};
std::atomic<std::uint64_t> g_last_pipeline_nexus_serial{0};
std::atomic<std::uint32_t> g_last_flags{0};
std::atomic<std::uint32_t> g_last_camera_out{0};
std::atomic<std::uint64_t> g_last_nexus_log_ms{0};
std::atomic<std::uint64_t> g_last_camera_log_ms{0};
std::atomic<std::uint64_t> g_last_render_log_ms{0};
std::atomic<std::uint64_t> g_last_handoff_log_ms{0};
std::atomic<std::uint64_t> g_last_phase_log_ms{0};

// V65.53 publication/consumption boundary autopsy. These two manual-reset events
// are signalled only when the exact newest LEFT publication reaches the render-thread
// pipeline consumer or the later +0x427C1C reset path. No polling and no game object writes.
HANDLE g_boundary_left_pipeline_event=nullptr;
HANDLE g_boundary_left_reset_event=nullptr;
std::atomic<std::uint64_t> g_boundary_left_pub_call{0};
std::atomic<std::uintptr_t> g_boundary_left_pub_ptr{0};
std::atomic<std::uint64_t> g_boundary_left_pub_tick{0};
std::atomic<std::uint64_t> g_boundary_left_pipeline_tick{0};
std::atomic<std::uint64_t> g_boundary_left_reset_tick{0};
std::atomic<std::uint32_t> g_boundary_left_pipeline_tid{0};
std::atomic<std::uint32_t> g_boundary_left_reset_tid{0};
std::atomic<std::uint64_t> g_boundary_pipeline_signals{0};
std::atomic<std::uint64_t> g_boundary_reset_signals{0};

thread_local std::uint64_t t_nexus_serial = 0;
thread_local int t_nexus_depth = 0;
thread_local std::uint32_t t_nexus_flags = 0;

std::uintptr_t WhBase() noexcept {
    return reinterpret_cast<std::uintptr_t>(GetModuleHandleA("WHGame.dll"));
}

std::uintptr_t ToRva(std::uintptr_t p) noexcept {
    const auto b=WhBase();
    return b && p>=b ? p-b : 0;
}

bool ReadPe(std::uintptr_t base, std::uint32_t& ts) noexcept {
    ts=0;
    if(!base) return false;
    __try {
        const auto* dos=reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
        if(dos->e_magic!=IMAGE_DOS_SIGNATURE) return false;
        const auto* nt=reinterpret_cast<const IMAGE_NT_HEADERS64*>(base+dos->e_lfanew);
        if(nt->Signature!=IMAGE_NT_SIGNATURE) return false;
        ts=nt->FileHeader.TimeDateStamp;
        return true;
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
}

bool Executable(const void* p) noexcept {
    if(!p) return false;
    MEMORY_BASIC_INFORMATION mbi{};
    if(!VirtualQuery(p,&mbi,sizeof(mbi)) || mbi.State!=MEM_COMMIT ||
       (mbi.Protect&(PAGE_GUARD|PAGE_NOACCESS))) return false;
    const DWORD prot=mbi.Protect&0xffu;
    return prot==PAGE_EXECUTE || prot==PAGE_EXECUTE_READ ||
           prot==PAGE_EXECUTE_READWRITE || prot==PAGE_EXECUTE_WRITECOPY;
}

bool MatchBytes(std::uintptr_t p,const unsigned char* expected,std::size_t n) noexcept {
    if(!p||!expected||!n) return false;
    __try { return std::memcmp(reinterpret_cast<const void*>(p),expected,n)==0; }
    __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
}

#include "native_stereo_single_run_state_v6601.inc"

void Trace(const char* text) noexcept {
    if(!text) return;
    debug_trace::Write(text);
    diagnostic_trace::Write(text);
}

template<class... A> void Tracef(const char* fmt,A... args) noexcept {
    char b[1024]{};
    std::snprintf(b,sizeof(b),fmt,args...);
    Trace(b);
}

std::uint32_t SafeReadU32(const void* p) noexcept {
    if(!p) return 0;
    __try { return *reinterpret_cast<const volatile std::uint32_t*>(p); }
    __except(EXCEPTION_EXECUTE_HANDLER) { return 0; }
}

std::uintptr_t SafeReadPtr(const void* p) noexcept {
    if(!p) return 0;
    __try { return *reinterpret_cast<const volatile std::uintptr_t*>(p); }
    __except(EXCEPTION_EXECUTE_HANDLER) { return 0; }
}

std::uint32_t LifetimeDtorOrdinal(std::uintptr_t ptr) noexcept {
    if(!ptr) return 0;
    std::size_t start=(ptr>>4)%kLifetimeSeenSlots;
    for(std::size_t i=0;i<kLifetimeSeenSlots;++i) {
        auto& slot=g_lifetime_seen[(start+i)%kLifetimeSeenSlots];
        auto cur=slot.ptr.load(std::memory_order_acquire);
        if(cur==ptr) return slot.dtor_entries.fetch_add(1,std::memory_order_relaxed)+1;
        if(cur==0) {
            std::uintptr_t expected=0;
            if(slot.ptr.compare_exchange_strong(expected,ptr,std::memory_order_acq_rel)) {
                slot.dtor_entries.store(1,std::memory_order_release); return 1;
            }
        }
    }
    return 0;
}

bool LifetimeAutopsyArmed() noexcept {
    const int mode=g_mode.load(std::memory_order_acquire);
    const int bridge=V6601EffectiveBridgeStage();
    const auto tick=g_lifetime_last_duplicate_return_tick.load(std::memory_order_acquire);
    const auto now=static_cast<std::uint64_t>(GetTickCount64());
    const bool recent_duplicate=tick && now>=tick && (now-tick)<1500ull;
    return mode==6 || (bridge>=2 && bridge<=7) || t_lifetime_outer_phase!=0 || recent_duplicate;
}

void TraceLifetimeObject(const char* where,void* self,unsigned int flags=0) noexcept {
    if(!LifetimeAutopsyArmed() || !self) return;
    const int bridge_now=V6601EffectiveBridgeStage();
    const auto tick_now=g_lifetime_last_duplicate_return_tick.load(std::memory_order_acquire);
    const auto ms_now=static_cast<std::uint64_t>(GetTickCount64());
    const bool recent_duplicate=tick_now && ms_now>=tick_now && (ms_now-tick_now)<1500ull;
    const auto event_no=g_lifetime_trace_events.fetch_add(1,std::memory_order_relaxed)+1;
    // Keep normal rung-6 baseline sparse. During an active/recent duplicate transaction,
    // log every destruction-chain entry because those few events are the crash evidence.
    if(bridge_now<2 && t_lifetime_outer_phase==0 && !recent_duplicate && event_no>12) return;
    const auto u=reinterpret_cast<std::uintptr_t>(self);
    MEMORY_BASIC_INFORMATION mbi{}; SIZE_T q=VirtualQuery(self,&mbi,sizeof(mbi));
    const auto region_base=q?reinterpret_cast<std::uintptr_t>(mbi.BaseAddress):0;
    const auto region_end=q?region_base+mbi.RegionSize:0;
    const auto target=u+0x8DB0ull;
    const bool target_in_region=q && target>=region_base && (target+sizeof(std::uintptr_t))<=region_end;
    const auto now=static_cast<std::uint64_t>(GetTickCount64());
    const auto last_tick=g_lifetime_last_duplicate_return_tick.load(std::memory_order_acquire);
    const auto age=(last_tick && now>=last_tick)?(now-last_tick):~0ull;
    const auto vt=SafeReadPtr(reinterpret_cast<const void*>(u));
    const auto ordinal=LifetimeDtorOrdinal(u);
    Tracef("V65.53-CRV-LIFE %s self=%p ordinal=%u flags=0x%X vtRva=0x%llX expectedVt=%d ref=%u viewType=%u selector=%u built=%u field8DB0=0x%llX region={base:%p size:0x%llX state:0x%lX protect:0x%lX targetIn:%d} pass=%d nexus=%llu recentRightAgeMs=%llu recentRightNexus=%llu matchLR={L:%d R:%d lastL:%d lastR:%d} tid=%lu",
        where,self,ordinal,flags,static_cast<unsigned long long>(ToRva(vt)),vt==(WhBase()+kCRenderViewVtableRva)?1:0,
        unsigned(SafeReadU32(reinterpret_cast<const void*>(u+0x8))),unsigned(SafeReadU32(reinterpret_cast<const void*>(u+0x14))),
        unsigned(SafeReadU32(reinterpret_cast<const void*>(u+0x8EE0))),unsigned(SafeReadU32(reinterpret_cast<const void*>(u+kBuiltRecordCount))),
        static_cast<unsigned long long>(SafeReadPtr(reinterpret_cast<const void*>(target))),
        reinterpret_cast<void*>(region_base),static_cast<unsigned long long>(q?mbi.RegionSize:0),
        static_cast<unsigned long>(q?mbi.State:0),static_cast<unsigned long>(q?mbi.Protect:0),target_in_region?1:0,
        t_lifetime_outer_phase,static_cast<unsigned long long>(t_nexus_serial),static_cast<unsigned long long>(age),
        static_cast<unsigned long long>(g_lifetime_last_duplicate_nexus.load(std::memory_order_acquire)),
        u==g_bridge_left_crv.load(std::memory_order_acquire)?1:0,u==g_bridge_right_crv.load(std::memory_order_acquire)?1:0,
        u==g_lifetime_last_duplicate_left_crv.load(std::memory_order_acquire)?1:0,u==g_lifetime_last_duplicate_right_crv.load(std::memory_order_acquire)?1:0,
        static_cast<unsigned long>(GetCurrentThreadId()));
}

bool EnsureBoundaryEventsV6553() noexcept {
    if(!g_boundary_left_pipeline_event) g_boundary_left_pipeline_event=CreateEventW(nullptr,TRUE,FALSE,nullptr);
    if(!g_boundary_left_reset_event) g_boundary_left_reset_event=CreateEventW(nullptr,TRUE,FALSE,nullptr);
    return g_boundary_left_pipeline_event && g_boundary_left_reset_event;
}

void ResetBoundaryTransactionV6553() noexcept {
    if(EnsureBoundaryEventsV6553()) {
        ResetEvent(g_boundary_left_pipeline_event);
        ResetEvent(g_boundary_left_reset_event);
    }
    g_boundary_left_pub_call.store(0,std::memory_order_release);
    g_boundary_left_pub_ptr.store(0,std::memory_order_release);
    g_boundary_left_pub_tick.store(0,std::memory_order_release);
    g_boundary_left_pipeline_tick.store(0,std::memory_order_release);
    g_boundary_left_reset_tick.store(0,std::memory_order_release);
    g_boundary_left_pipeline_tid.store(0,std::memory_order_release);
    g_boundary_left_reset_tid.store(0,std::memory_order_release);
}

RenderViewHandoffSlot* FindNewestHandoffForPtrV6553(std::uintptr_t ptr) noexcept {
    if(!ptr) return nullptr;
    RenderViewHandoffSlot* best=nullptr; std::uint64_t best_call=0;
    for(auto& slot:g_handoff) {
        if(slot.ptr.load(std::memory_order_acquire)!=ptr) continue;
        const auto call=slot.call.load(std::memory_order_acquire);
        if(call>=best_call) { best=&slot; best_call=call; }
    }
    return best;
}

void SignalResetBoundaryV6553(void* self) noexcept {
    const auto ptr=reinterpret_cast<std::uintptr_t>(self);
    auto* slot=FindNewestHandoffForPtrV6553(ptr);
    if(!slot) return;
    const auto call=slot->call.load(std::memory_order_acquire);
    const auto phase=slot->eye_phase.load(std::memory_order_acquire);
    const auto now=static_cast<std::uint64_t>(GetTickCount64());
    slot->reset_tick_ms.store(now,std::memory_order_release);
    slot->reset_tid.store(static_cast<std::uint32_t>(GetCurrentThreadId()),std::memory_order_release);
    if(phase==-1 && call==g_boundary_left_pub_call.load(std::memory_order_acquire) &&
       ptr==g_boundary_left_pub_ptr.load(std::memory_order_acquire)) {
        g_boundary_left_reset_tick.store(now,std::memory_order_release);
        g_boundary_left_reset_tid.store(static_cast<std::uint32_t>(GetCurrentThreadId()),std::memory_order_release);
        g_boundary_reset_signals.fetch_add(1,std::memory_order_relaxed);
        if(g_boundary_left_reset_event) SetEvent(g_boundary_left_reset_event);
        const auto pub=slot->publish_tick_ms.load(std::memory_order_acquire);
        Tracef("V65.53-BOUNDARY RESET-CONSUME pubCall=%llu ptr=%p nexus=%llu latencyMs=%llu producerTid=%u resetTid=%lu",
            static_cast<unsigned long long>(call),self,static_cast<unsigned long long>(slot->nexus.load()),
            static_cast<unsigned long long>((pub&&now>=pub)?now-pub:~0ull),unsigned(slot->producer_tid.load()),
            static_cast<unsigned long>(GetCurrentThreadId()));
    }
}

std::uintptr_t SafeVslotTarget(void* object,std::size_t slot) noexcept {
    if(!object) return 0;
    __try {
        auto** vt=*reinterpret_cast<void***>(object);
        if(!vt) return 0;
        return reinterpret_cast<std::uintptr_t>(vt[slot]);
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        return 0;
    }
}

void* EngineObject() noexcept {
    const auto b=WhBase();
    if(!b) return nullptr;
    __try { return *reinterpret_cast<void**>(b+kEngineGlobalRva); }
    __except(EXCEPTION_EXECUTE_HANDLER) { return nullptr; }
}

bool ExactBuildReady() noexcept {
    const auto b=WhBase();
    std::uint32_t ts=0;
    const bool pe=ReadPe(b,ts);
    bool vtable_slot=false;
    if(b) {
        __try {
            const auto target=*reinterpret_cast<const std::uintptr_t*>(b+kCCryActionVtableRva+kCCryActionFrameNexusSlot*sizeof(void*));
            vtable_slot=(target==b+kFrameNexusRva);
        } __except(EXCEPTION_EXECUTE_HANDLER) { vtable_slot=false; }
    }
    const bool nexus_bytes=b && MatchBytes(b+kFrameNexusRva,kFrameNexusPrologue,sizeof(kFrameNexusPrologue));
    const bool phase7e8_bytes=b && MatchBytes(b+kPhase7e8CallsiteRva,kPhase7e8Callsite,sizeof(kPhase7e8Callsite));
    const bool camera_bytes=b && MatchBytes(b+kCameraHandoffCallsiteRva,kCameraHandoffCallsite,sizeof(kCameraHandoffCallsite));
    const bool renderer_bytes=b && MatchBytes(b+kRendererPhaseCallsiteRva,kRendererPhaseCallsite,sizeof(kRendererPhaseCallsite));
    const bool phase7f8_bytes=b && MatchBytes(b+kPhase7f8CallsiteRva,kPhase7f8Callsite,sizeof(kPhase7f8Callsite));
    const bool render_view_bytes=b && MatchBytes(b+kGetOrCreateRenderViewRva,kGetOrCreatePrologue,sizeof(kGetOrCreatePrologue));
    const bool stereo_build_bytes=b && MatchBytes(b+kCRenderViewCameraBuildRva,kCRenderViewCameraBuildPrologue,sizeof(kCRenderViewCameraBuildPrologue));
    const bool pair_build_bytes=b && MatchBytes(b+kCRenderViewCameraPairBuildRva,kCRenderViewCameraPairBuildPrologue,sizeof(kCRenderViewCameraPairBuildPrologue));
    const bool csystem_render_bytes=b && MatchBytes(b+kCSystemRenderRva,kCSystemRenderPrologue,sizeof(kCSystemRenderPrologue));
    const bool csystem_begin_bytes=b && MatchBytes(b+kCSystemRenderBeginRva,kCSystemRenderBeginPrologue,sizeof(kCSystemRenderBeginPrologue));
    const bool csystem_begin_callsite=b && MatchBytes(b+kCSystemRenderBeginCallsiteRva,kCSystemRenderBeginCallsite,sizeof(kCSystemRenderBeginCallsite));
    const bool csystem_get_camera_bytes=b && MatchBytes(b+kCSystemGetViewCameraRva,kCSystemGetViewCameraPrologue,sizeof(kCSystemGetViewCameraPrologue));
    // FrameSync installs KCD2VR's normal PublishViewCamera observer on this exact
    // retail thunk at startup, so untouched entry bytes are not a valid F8
    // prerequisite once the mod is running. Accept either pristine retail bytes
    // or our known live self-hook; the CSystem vtable target is still validated
    // independently below.
    const bool csystem_set_camera_raw_bytes=b && MatchBytes(b+kCSystemSetViewCameraRva,kCSystemSetViewCameraPrologue,sizeof(kCSystemSetViewCameraPrologue));
    const bool csystem_set_camera_self_hook=FrameSync::PublishViewCameraHookInstalled();
    const bool csystem_set_camera_route=csystem_set_camera_raw_bytes || csystem_set_camera_self_hook;
    bool csystem_vtable=false;
    if(b) {
        __try {
            const auto* vt=reinterpret_cast<const std::uintptr_t*>(b+kCSystemVtableRva);
            csystem_vtable=(vt[0x48/sizeof(void*)]==b+kCSystemRenderRva &&
                            vt[0x50/sizeof(void*)]==b+kCSystemRenderBeginRva &&
                            vt[0x58/sizeof(void*)]==b+kCSystemRenderEndRva &&
                            vt[0x430/sizeof(void*)]==b+kCSystemSetViewCameraRva &&
                            vt[0x438/sizeof(void*)]==b+kCSystemGetViewCameraRva);
        } __except(EXCEPTION_EXECUTE_HANDLER) { csystem_vtable=false; }
    }
    const bool ok=pe && ts==kSupportedTimestamp && vtable_slot && nexus_bytes &&
        phase7e8_bytes && camera_bytes && renderer_bytes && phase7f8_bytes && render_view_bytes && stereo_build_bytes && pair_build_bytes && csystem_render_bytes &&
        csystem_begin_bytes && csystem_begin_callsite && csystem_get_camera_bytes && csystem_set_camera_route && csystem_vtable;
    g_supported.store(ok,std::memory_order_release);
    if(ok) {
        Tracef("V65.56-F8 STATIC-GATE PASS setViewCamRoute=1 setViewCamRaw=%d selfHook=%d cSystemVtable=1",
            csystem_set_camera_raw_bytes?1:0,csystem_set_camera_self_hook?1:0);
    }
    if(!ok) {
        Tracef("V65.56-F8 STATIC-GATE FAIL pe=%d timestamp=0x%08X expected=0x%08X ccrySlot=%d nexus=%d cSystemRender=%d cSystemBegin=%d beginCallsite=%d getViewCam=%d setViewCamRoute=%d setViewCamRaw=%d selfHook=%d cSystemVtable=%d; no hooks installed",
            pe?1:0,ts,kSupportedTimestamp,vtable_slot?1:0,nexus_bytes?1:0,csystem_render_bytes?1:0,csystem_begin_bytes?1:0,csystem_begin_callsite?1:0,csystem_get_camera_bytes?1:0,csystem_set_camera_route?1:0,csystem_set_camera_raw_bytes?1:0,csystem_set_camera_self_hook?1:0,csystem_vtable?1:0);
    }
    return ok;
}

// V66.99: physical commands require the native frame boundary, not pristine
// renderer routines already owned by production stereo. Keep ExactBuildReady()
// above unchanged for its original renderer/stereo callers.
static bool ExactPhysicalFrameReady6699() noexcept {
    const auto b=WhBase();std::uint32_t ts=0;
    const bool pe=ReadPe(b,ts);
    bool slot=false;
    if(b){__try {slot=*reinterpret_cast<const std::uintptr_t*>(
        b+kCCryActionVtableRva+kCCryActionFrameNexusSlot*sizeof(void*))==b+kFrameNexusRva;
    } __except(EXCEPTION_EXECUTE_HANDLER) {slot=false;}}
    const bool entry=b&&MatchBytes(b+kFrameNexusRva,kFrameNexusPrologue,sizeof(kFrameNexusPrologue));
    const bool phaseA=b&&MatchBytes(b+kPhase7e8CallsiteRva,kPhase7e8Callsite,sizeof(kPhase7e8Callsite));
    const bool camera=b&&MatchBytes(b+kCameraHandoffCallsiteRva,kCameraHandoffCallsite,sizeof(kCameraHandoffCallsite));
    const bool renderer=b&&MatchBytes(b+kRendererPhaseCallsiteRva,kRendererPhaseCallsite,sizeof(kRendererPhaseCallsite));
    const bool phaseB=b&&MatchBytes(b+kPhase7f8CallsiteRva,kPhase7f8Callsite,sizeof(kPhase7f8Callsite));
    const bool ok=pe&&ts==kSupportedTimestamp&&slot&&entry&&phaseA&&camera&&renderer&&phaseB;
    Tracef("V66.99-PHYSICAL FRAME_GATE %s pe=%d timestamp=0x%08X slot=%d entry=%d phaseA=%d cameraCallsite=%d rendererCallsite=%d phaseB=%d cameraEntryPrerequisite=NONE",
        ok?"PASS":"FAIL",pe?1:0,ts,slot?1:0,entry?1:0,phaseA?1:0,camera?1:0,renderer?1:0,phaseB?1:0);
    return ok;
}

bool ExactMultiviewReadyV6556() noexcept {
    const auto b=WhBase(); std::uint32_t ts=0; const bool pe=ReadPe(b,ts);
    const bool build=b && MatchBytes(b+kCRenderViewCameraBuildRva,kCRenderViewCameraBuildPrologue,sizeof(kCRenderViewCameraBuildPrologue));
    const bool pair=b && MatchBytes(b+kCRenderViewCameraPairBuildRva,kCRenderViewCameraPairBuildPrologue,sizeof(kCRenderViewCameraPairBuildPrologue));
    const bool pipeline=b && MatchBytes(b+kGraphicsPipelineMultiviewLoadRva,kGraphicsPipelineMultiviewLoad,sizeof(kGraphicsPipelineMultiviewLoad));
    const bool ok=pe && ts==kSupportedTimestamp && build && pair && pipeline;
    Tracef("V65.56-STATIC-GATE %s pe=%d timestamp=0x%08X buildLoop=%d pairBuilder=%d pipelineArrayCount=%d pipelineRva=0x%llX",
        ok?"PASS":"FAIL",pe?1:0,ts,build?1:0,pair?1:0,pipeline?1:0,static_cast<unsigned long long>(kGraphicsPipelineExecuteRva));
    return ok;
}

void __fastcall FrameNexusDetour(void* self,std::uintptr_t unused_arg,std::uint32_t flags) noexcept {
    const auto serial=g_nexus_calls.fetch_add(1,std::memory_order_relaxed)+1;
    const auto present_before=DeviceHook::PresentCaptureCount();
    const auto old_serial=t_nexus_serial;
    const auto old_depth=t_nexus_depth;
    const auto old_flags=t_nexus_flags;
    t_nexus_serial=serial;
    t_nexus_depth=old_depth+1;
    t_nexus_flags=flags;
    g_last_nexus_serial.store(serial,std::memory_order_release);
    g_last_flags.store(flags,std::memory_order_release);

    // One gameplay pump per outer native frame, never a second-eye render.
    // Receipt eligibility cannot survive an exceptional native return. Restore
    // only metadata in finally; the existing physics End remains normal-path.
    std::uint64_t present_after=present_before;
    __try {
        // F2/2 only. Before native/physics/pose locks and before the original
        // gameplay update can sample or schedule this frame's hand pose.
        if(old_depth==0 && !(flags&0x20u)) FrameSync::PaceGameplayBeforeUpdateD45();
        if(old_depth==0 && !(flags&0x20u)) frik_pose_probe::PhysicalNativeFrameTick6698();
        if(g_nexus_original) g_nexus_original(self,unused_arg,flags);
        if(old_depth==0 && !(flags&0x20u)) frik_pose_probe::PhysicalNativeFrameEnd104();
        present_after=DeviceHook::PresentCaptureCount();
        if(present_after>present_before) g_nexus_present_advances.fetch_add(present_after-present_before,std::memory_order_relaxed);
    } __finally {
        t_nexus_serial=old_serial;
        t_nexus_depth=old_depth;
        t_nexus_flags=old_flags;
    }

    const auto now=static_cast<std::uint64_t>(GetTickCount64());
    auto last=g_last_nexus_log_ms.load(std::memory_order_relaxed);
    if(g_mode.load()!=0 && (serial<=6 || (now>=last+1000ull && g_last_nexus_log_ms.compare_exchange_strong(last,now,std::memory_order_acq_rel)))) {
        Tracef("V65.07-F8-NEXUS serial=%llu self=%p arg=0x%llX flags=0x%X bits={20:%d 10:%d 04:%d} presentBefore=%llu presentAfter=%llu presentDelta=%llu tid=%lu READ-ONLY",
            static_cast<unsigned long long>(serial),self,static_cast<unsigned long long>(unused_arg),unsigned(flags),
            (flags&0x20u)?1:0,(flags&0x10u)?1:0,(flags&0x04u)?1:0,
            static_cast<unsigned long long>(present_before),static_cast<unsigned long long>(present_after),
            static_cast<unsigned long long>(present_after-present_before),static_cast<unsigned long>(GetCurrentThreadId()));
    }
}

void __fastcall CameraHandoffDetour(void* engine,void* camera,std::uint32_t* out_state) noexcept {
    const auto n=g_camera_calls.fetch_add(1,std::memory_order_relaxed)+1;
    const bool in_nexus=t_nexus_depth>0 && t_nexus_serial!=0;
    if(in_nexus) g_camera_in_nexus.fetch_add(1,std::memory_order_relaxed);
    const auto nexus=t_nexus_serial;
    if(nexus) g_last_camera_nexus_serial.store(nexus,std::memory_order_release);
    const auto before=SafeReadU32(out_state);
    const auto caller=ToRva(reinterpret_cast<std::uintptr_t>(_ReturnAddress()));

    if(g_camera_original) g_camera_original(engine,camera,out_state);

    const auto after=SafeReadU32(out_state);
    g_last_camera_out.store(after,std::memory_order_release);
    const auto now=static_cast<std::uint64_t>(GetTickCount64());
    auto last=g_last_camera_log_ms.load(std::memory_order_relaxed);
    if(n<=8 || (now>=last+1000ull && g_last_camera_log_ms.compare_exchange_strong(last,now,std::memory_order_acq_rel))) {
        Tracef("V65.07-F8-CAMERA call=%llu nexus=%llu inNexus=%d callerRva=0x%llX engine=%p camera=%p out=%p before=%u after=%u presents=%llu tid=%lu READ-ONLY",
            static_cast<unsigned long long>(n),static_cast<unsigned long long>(nexus),in_nexus?1:0,
            static_cast<unsigned long long>(caller),engine,camera,out_state,unsigned(before),unsigned(after),
            static_cast<unsigned long long>(DeviceHook::PresentCaptureCount()),static_cast<unsigned long>(GetCurrentThreadId()));
    }
}

void* __fastcall GetOrCreateRenderViewDetour(void* renderer,int view_type) noexcept {
    const auto n=g_get_or_create_calls.fetch_add(1,std::memory_order_relaxed)+1;
    const bool in_nexus=t_nexus_depth>0 && t_nexus_serial!=0;
    if(in_nexus) g_get_or_create_in_nexus.fetch_add(1,std::memory_order_relaxed);
    const auto nexus=t_nexus_serial;
    const auto caller=ToRva(reinterpret_cast<std::uintptr_t>(_ReturnAddress()));
    void* ret=g_get_or_create_original?g_get_or_create_original(renderer,view_type):nullptr;
    if(in_nexus && ret && (g_mode.load(std::memory_order_relaxed)>=1 || V6601EffectiveBridgeStage()>=1)) {
        auto& slot=g_handoff[static_cast<std::size_t>(n%kRenderViewHandoffSlots)];
        // Clear the publication token first so a recycled slot cannot expose a
        // mixed old-pointer/new-metadata snapshot when the same RenderView pointer recurs.
        slot.ptr.store(0,std::memory_order_release);
        slot.nexus.store(nexus,std::memory_order_relaxed);
        slot.call.store(n,std::memory_order_relaxed);
        slot.present.store(DeviceHook::PresentCaptureCount(),std::memory_order_relaxed);
        slot.requested_type.store(static_cast<std::uint32_t>(view_type),std::memory_order_relaxed);
        slot.producer_tid.store(static_cast<std::uint32_t>(GetCurrentThreadId()),std::memory_order_relaxed);
        const auto publish_tick=static_cast<std::uint64_t>(GetTickCount64());
        const int eye_phase=t_lifetime_outer_phase;
        slot.publish_tick_ms.store(publish_tick,std::memory_order_relaxed);
        slot.eye_phase.store(eye_phase,std::memory_order_relaxed);
        slot.pipeline_tick_ms.store(0,std::memory_order_relaxed); slot.pipeline_tid.store(0,std::memory_order_relaxed);
        slot.reset_tick_ms.store(0,std::memory_order_relaxed); slot.reset_tid.store(0,std::memory_order_relaxed);
        slot.ptr.store(reinterpret_cast<std::uintptr_t>(ret),std::memory_order_release);
        if(eye_phase==-1) {
            g_boundary_left_pub_call.store(n,std::memory_order_release);
            g_boundary_left_pub_ptr.store(reinterpret_cast<std::uintptr_t>(ret),std::memory_order_release);
            g_boundary_left_pub_tick.store(publish_tick,std::memory_order_release);
            Tracef("V65.53-BOUNDARY LEFT-PUBLISH pubCall=%llu nexus=%llu ptr=%p producerTid=%lu",
                static_cast<unsigned long long>(n),static_cast<unsigned long long>(nexus),ret,static_cast<unsigned long>(GetCurrentThreadId()));
        }
    }
    if(view_type!=0) {
        Tracef("V65.32-F8-NONZERO-VIEW call=%llu nexus=%llu requestedType=%d callerRva=0x%llX renderer=%p ret=%p CLASSIFY={1:RECURSION_STATIC_PROOF,2+:UNKNOWN} READ-ONLY",
            static_cast<unsigned long long>(n),static_cast<unsigned long long>(nexus),view_type,
            static_cast<unsigned long long>(caller),renderer,ret);
    }
    const auto now=static_cast<std::uint64_t>(GetTickCount64());
    auto last=g_last_render_log_ms.load(std::memory_order_relaxed);
    if(n<=8 || view_type!=0 || (in_nexus && (now>=last+1000ull && g_last_render_log_ms.compare_exchange_strong(last,now,std::memory_order_acq_rel)))) {
        Tracef("V65.07-F8-RENDERVIEW call=%llu nexus=%llu inNexus=%d requestedType=%d callerRva=0x%llX renderer=%p ret=%p presents=%llu tid=%lu READ-ONLY",
            static_cast<unsigned long long>(n),static_cast<unsigned long long>(nexus),in_nexus?1:0,view_type,
            static_cast<unsigned long long>(caller),renderer,ret,
            static_cast<unsigned long long>(DeviceHook::PresentCaptureCount()),static_cast<unsigned long>(GetCurrentThreadId()));
    }
    return ret;
}

void* __fastcall Phase7e8Detour(void* engine) noexcept {
    const auto n=g_phase7e8_calls.fetch_add(1,std::memory_order_relaxed)+1;
    const auto nexus=t_nexus_serial;
    const auto caller=ToRva(reinterpret_cast<std::uintptr_t>(_ReturnAddress()));
    void* ret=g_phase7e8_original?g_phase7e8_original(engine):nullptr;
    const auto now=static_cast<std::uint64_t>(GetTickCount64());
    auto last=g_last_phase_log_ms.load(std::memory_order_relaxed);
    if(n<=8 || (now>=last+1000ull && g_last_phase_log_ms.compare_exchange_strong(last,now,std::memory_order_acq_rel))) {
        std::uintptr_t child_vt=0;
        __try { if(ret) child_vt=reinterpret_cast<std::uintptr_t>(*reinterpret_cast<void***>(ret)); }
        __except(EXCEPTION_EXECUTE_HANDLER) { child_vt=0; }
        Tracef("V65.07-F8-PHASE7E8 call=%llu nexus=%llu callerRva=0x%llX engine=%p child=%p childVtRva=0x%llX presents=%llu tid=%lu READ-ONLY",
            static_cast<unsigned long long>(n),static_cast<unsigned long long>(nexus),
            static_cast<unsigned long long>(caller),engine,ret,static_cast<unsigned long long>(ToRva(child_vt)),
            static_cast<unsigned long long>(DeviceHook::PresentCaptureCount()),static_cast<unsigned long>(GetCurrentThreadId()));
    }
    return ret;
}

void __fastcall Phase7f8Detour(void* engine) noexcept {
    const auto n=g_phase7f8_calls.fetch_add(1,std::memory_order_relaxed)+1;
    const auto nexus=t_nexus_serial;
    const auto caller=ToRva(reinterpret_cast<std::uintptr_t>(_ReturnAddress()));
    if(g_phase7f8_original) g_phase7f8_original(engine);
    const auto now=static_cast<std::uint64_t>(GetTickCount64());
    auto last=g_last_phase_log_ms.load(std::memory_order_relaxed);
    if(n<=8 || (now>=last+1000ull && g_last_phase_log_ms.compare_exchange_strong(last,now,std::memory_order_acq_rel))) {
        Tracef("V65.07-F8-PHASE7F8 call=%llu nexus=%llu callerRva=0x%llX engine=%p presents=%llu tid=%lu READ-ONLY",
            static_cast<unsigned long long>(n),static_cast<unsigned long long>(nexus),
            static_cast<unsigned long long>(caller),engine,
            static_cast<unsigned long long>(DeviceHook::PresentCaptureCount()),static_cast<unsigned long>(GetCurrentThreadId()));
    }
}




void* SafeGetCryConsoleV6541() noexcept {
    const auto wh=WhBase(); if(!wh) return nullptr;
    __try { return *reinterpret_cast<void**>(wh+kIConsoleGlobalRva); }
    __except(EXCEPTION_EXECUTE_HANDLER) { return nullptr; }
}
void* SafeConsoleGetCVarV6541(void* console,const char* name) noexcept {
    if(!console||!name) return nullptr;
    __try { auto** vt=*reinterpret_cast<void***>(console); if(!vt||!vt[0xB8/sizeof(void*)]) return nullptr;
        using Fn=void*(__fastcall*)(void*,const char*); return reinterpret_cast<Fn>(vt[0xB8/sizeof(void*)])(console,name); }
    __except(EXCEPTION_EXECUTE_HANDLER) { return nullptr; }
}
int SafeCVarGetIntV6541(void* cvar,int fallback) noexcept {
    if(!cvar) return fallback;
    __try { auto** vt=*reinterpret_cast<void***>(cvar); if(!vt||!vt[0x10/sizeof(void*)]) return fallback;
        using Fn=int(__fastcall*)(void*); return reinterpret_cast<Fn>(vt[0x10/sizeof(void*)])(cvar); }
    __except(EXCEPTION_EXECUTE_HANDLER) { return fallback; }
}
bool SafeCVarSetIntV6541(void* cvar,int value) noexcept {
    if(!cvar) return false;
    __try { auto** vt=*reinterpret_cast<void***>(cvar); if(!vt||!vt[0x38/sizeof(void*)]) return false;
        using Fn=void(__fastcall*)(void*,int); reinterpret_cast<Fn>(vt[0x38/sizeof(void*)])(cvar,value); return true; }
    __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
}

struct ScopedRightEyeCVarV6541 { void* cvar=nullptr; int old_value=0; const char* name=nullptr; bool changed=false; };
struct RightEyeCVarBatchV6541 { std::array<ScopedRightEyeCVarV6541,10> item{}; int count=0; unsigned mask=0; };

bool PushRightEyeDisableV6541(RightEyeCVarBatchV6541& b,void* console,const char* name,unsigned bit) noexcept {
    if(b.count>=static_cast<int>(b.item.size())) return false;
    void* c=SafeConsoleGetCVarV6541(console,name); if(!c) return false;
    const int old=SafeCVarGetIntV6541(c,0);
    auto& e=b.item[static_cast<std::size_t>(b.count++)]; e.cvar=c; e.old_value=old; e.name=name;
    e.changed=SafeCVarSetIntV6541(c,0) && SafeCVarGetIntV6541(c,old)==0; if(e.changed) b.mask|=bit; return e.changed;
}
bool PushRightEyeValueV6546(RightEyeCVarBatchV6541& b,void* console,const char* name,int value,unsigned bit) noexcept {
    if(b.count>=static_cast<int>(b.item.size())) return false;
    void* c=SafeConsoleGetCVarV6541(console,name); if(!c) return false;
    const int old=SafeCVarGetIntV6541(c,0);
    auto& e=b.item[static_cast<std::size_t>(b.count++)]; e.cvar=c; e.old_value=old; e.name=name;
    e.changed=SafeCVarSetIntV6541(c,value) && SafeCVarGetIntV6541(c,old)==value; if(e.changed) b.mask|=bit; return e.changed;
}
void RestoreRightEyeCVarsV6541(RightEyeCVarBatchV6541& b) noexcept {
    for(int i=b.count-1;i>=0;--i) if(b.item[static_cast<std::size_t>(i)].changed)
        (void)SafeCVarSetIntV6541(b.item[static_cast<std::size_t>(i)].cvar,b.item[static_cast<std::size_t>(i)].old_value);
}
bool ApplyRightEyePolicyV6544(int bridge_stage,int postfx_stage,RightEyeCVarBatchV6541& b) noexcept {
    (void)postfx_stage;
    void* console=SafeGetCryConsoleV6541(); if(!console) return false;
    if(bridge_stage<2 || bridge_stage>5) return true; // stage 1 is capture-only control
    // V65.47 Crysis-minimal RIGHT policy: do not rebuild shadow maps and freeze
    // particle-system updating for the second eye.  Preserve every pre-existing
    // particle debug bit; AlphaBit('z') in this retail build is bit 31.
    void* particles=SafeConsoleGetCVarV6541(console,"e_ParticlesDebug");
    if(!particles) return false;
    const int old_particles=SafeCVarGetIntV6541(particles,0);
    bool ok=true;
    ok=PushRightEyeDisableV6541(b,console,"r_ShadowMapsUpdate",1u<<0)&&ok;
    ok=PushRightEyeValueV6546(b,console,"e_ParticlesDebug",old_particles|static_cast<int>(0x80000000u),1u<<1)&&ok;
    return ok;
}

thread_local RightEyeCVarBatchV6541 g_v6625_production_right_policy{};
thread_local bool g_v6625_production_right_policy_active=false;

bool ApplyTangibleRightPolicyV6548(int tangible_stage,RightEyeCVarBatchV6541& b) noexcept {
    if(tangible_stage<=1) return true; // rung 1 deliberately reproduces the proven raw one-shot
    void* console=SafeGetCryConsoleV6541(); if(!console) return false;
    void* particles=SafeConsoleGetCVarV6541(console,"e_ParticlesDebug");
    if(!particles) return false;
    const int old_particles=SafeCVarGetIntV6541(particles,0);
    bool ok=true;
    ok=PushRightEyeDisableV6541(b,console,"r_ShadowMapsUpdate",1u<<0)&&ok;
    ok=PushRightEyeValueV6546(b,console,"e_ParticlesDebug",old_particles|static_cast<int>(0x80000000u),1u<<1)&&ok;
    if(tangible_stage>=4) {
        ok=PushRightEyeDisableV6541(b,console,"r_ssdo",1u<<2)&&ok;
        ok=PushRightEyeDisableV6541(b,console,"r_SSReflections",1u<<3)&&ok;
        ok=PushRightEyeDisableV6541(b,console,"r_MotionBlur",1u<<4)&&ok;
        // V66.79a SKYFIX: never touch cloud reprojection CVars, even transiently per-eye.
        ok=PushRightEyeDisableV6541(b,console,"r_Reflections",1u<<7)&&ok;
        ok=PushRightEyeDisableV6541(b,console,"r_WaterReflections",1u<<8)&&ok;
    }
    return ok;
}

std::uint64_t TangibleAttemptLimitV6548(int stage) noexcept {
    switch(stage){case 1:return 1; case 2:return 1; case 3:return 4; case 4:return 4; case 5:return 8; default:return 0;}
}

std::uint32_t TangibleCertificateWaitMsV6550(int stage) noexcept {
    // Rung 1 is a tight one-shot proof. Higher rungs allow most of one 90-Hz
    // frame for KCD2's worker/render thread to record and submit the exact list.
    return stage<=1 ? 3u : 8u;
}

bool ValidateCSystemV6555(void* self) noexcept {
    if(!self) return false;
    const auto b=WhBase(); if(!b) return false;
    __try {
        const auto vt=reinterpret_cast<std::uintptr_t>(*reinterpret_cast<void***>(self));
        if(vt!=b+kCSystemVtableRva) return false;
        const auto* slots=reinterpret_cast<const std::uintptr_t*>(vt);
        return slots[0x48/sizeof(void*)]==b+kCSystemRenderRva &&
               slots[0x50/sizeof(void*)]==b+kCSystemRenderBeginRva &&
               slots[0x58/sizeof(void*)]==b+kCSystemRenderEndRva &&
               slots[0x430/sizeof(void*)]==b+kCSystemSetViewCameraRva &&
               slots[0x438/sizeof(void*)]==b+kCSystemGetViewCameraRva;
    } __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
}

// Keep SEH in tiny leaf helpers only. The render detour itself owns C++ stack objects
// and must not contain any new __try scopes that would trigger MSVC C2712.
void* GetSystemViewCameraV6555(void* self) noexcept {
    if(!ValidateCSystemV6555(self)) return nullptr;
    __try {
        auto** vt=*reinterpret_cast<void***>(self);
        auto fn=reinterpret_cast<CSystemGetViewCameraFn>(vt[0x438/sizeof(void*)]);
        return fn?fn(self):nullptr;
    } __except(EXCEPTION_EXECUTE_HANDLER) { return nullptr; }
}

bool SetSystemViewCameraV6555(void* self,const void* camera,CameraPublicationR24* copied_family=nullptr) noexcept {
    if(!camera || !ValidateCSystemV6555(self)){if(copied_family)*copied_family={};return false;}
    // Do not call the live vtable entry here: FrameSync intentionally detours the
    // same PublishViewCamera thunk to compose HMD tracking for normal game camera
    // publications. These temporary LEFT/RIGHT snapshots are copied from the
    // already-composed live system camera; routing them through that detour would
    // apply the HMD transform a second time. Invoke FrameSync's saved retail
    // original/trampoline instead.
    return FrameSync::SetViewCameraRetail(self,const_cast<void*>(camera),copied_family);
}

bool CopySystemViewCameraV6555(const void* src,void* dst) noexcept {
    if(!src || !dst) return false;
    __try { std::memcpy(dst,src,kSystemViewCameraSize); return true; }
    __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
}

bool ShiftSystemViewCameraSnapshotV6555(void* camera,float meters) noexcept {
    if(!camera || !std::isfinite(meters)) return false;
    __try {
        auto* f=reinterpret_cast<float*>(camera);
        const float rx=f[0], ry=f[4], rz=f[8];
        if(!std::isfinite(rx)||!std::isfinite(ry)||!std::isfinite(rz)) return false;
        f[3]  += rx*meters;
        f[7]  += ry*meters;
        f[11] += rz*meters;
        return std::isfinite(f[3])&&std::isfinite(f[7])&&std::isfinite(f[11]);
    } __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
}

void __fastcall CSystemRenderBeginDetourV6554(void* self,void* frame_state,void* frame_context) noexcept {
    const auto caller=ToRva(reinterpret_cast<std::uintptr_t>(_ReturnAddress()));
    if(!t_bridge_injected_sysbegin && self && frame_state && frame_context &&
       caller==kCSystemRenderBeginNormalReturnRva && ValidateCSystemV6555(self)) {
        std::uint64_t lo=0,hi=0; std::uint32_t ctx=0; bool copied=false;
        __try {
            std::memcpy(&lo,frame_state,sizeof(lo));
            std::memcpy(&hi,reinterpret_cast<const unsigned char*>(frame_state)+8,sizeof(hi));
            std::memcpy(&ctx,frame_context,sizeof(ctx));
            copied=true;
        } __except(EXCEPTION_EXECUTE_HANDLER) { copied=false; }
        if(copied) {
            g_sysbegin_self.store(reinterpret_cast<std::uintptr_t>(self),std::memory_order_release);
            g_sysbegin_state_lo.store(lo,std::memory_order_release);
            g_sysbegin_state_hi.store(hi,std::memory_order_release);
            g_sysbegin_context_value.store(ctx,std::memory_order_release);
            g_sysbegin_tid.store(static_cast<std::uint32_t>(GetCurrentThreadId()),std::memory_order_release);
            g_sysbegin_tick_ms.store(static_cast<std::uint64_t>(GetTickCount64()),std::memory_order_release);
            g_sysbegin_last_caller_rva.store(caller,std::memory_order_release);
            const auto serial=g_sysbegin_capture_serial.fetch_add(1,std::memory_order_acq_rel)+1;
            if(serial<=4) Tracef("V65.55a-SYSBEGIN CAPTURE serial=%llu self=%p callerRva=0x%llX ctx=%u tid=%lu",
                static_cast<unsigned long long>(serial),self,static_cast<unsigned long long>(caller),unsigned(ctx),static_cast<unsigned long>(GetCurrentThreadId()));
        }
    }
    if(g_csystem_render_begin_original) g_csystem_render_begin_original(self,frame_state,frame_context);
}

bool ValidateCSystemStaticVtableV6554(std::uintptr_t b) noexcept {
    if(!b) return false;
    __try {
        const auto* vt=reinterpret_cast<const std::uintptr_t*>(b+kCSystemVtableRva);
        return vt[0x48/sizeof(void*)]==b+kCSystemRenderRva &&
               vt[0x50/sizeof(void*)]==b+kCSystemRenderBeginRva &&
               vt[0x58/sizeof(void*)]==b+kCSystemRenderEndRva &&
               vt[0x430/sizeof(void*)]==b+kCSystemSetViewCameraRva &&
               vt[0x438/sizeof(void*)]==b+kCSystemGetViewCameraRva;
    } __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
}

bool EnsureCSystemRenderBeginHookV6554() noexcept {
    if(g_csystem_render_begin_installed.load(std::memory_order_acquire)) return true;
    const auto b=WhBase();
    if(!b || !MatchBytes(b+kCSystemRenderBeginRva,kCSystemRenderBeginPrologue,sizeof(kCSystemRenderBeginPrologue)) ||
       !MatchBytes(b+kCSystemRenderBeginCallsiteRva,kCSystemRenderBeginCallsite,sizeof(kCSystemRenderBeginCallsite))) {
        Trace("V65.55a-SYSBEGIN hook REFUSED: exact wrapper/callsite bytes mismatch"); return false;
    }
    if(!ValidateCSystemStaticVtableV6554(b)) {
        Trace("V65.55a-SYSBEGIN hook REFUSED: CSystem vtable render/begin/end/view-camera slots mismatch/unreadable"); return false;
    }
    DetourModKit::hook::InlineRequest req{"kcd2vr_v6554_csystem_renderbegin",DetourModKit::hook::Target{DetourModKit::Address{b+kCSystemRenderBeginRva}},DetourModKit::hook::Options{}};
    auto h=DetourModKit::hook::inline_at(std::move(req),&CSystemRenderBeginDetourV6554); if(!h) return false;
    g_csystem_render_begin_hook=std::move(*h);
    g_csystem_render_begin_original=g_csystem_render_begin_hook->original<CSystemRenderBeginFn>();
    if(!g_csystem_render_begin_original || !g_csystem_render_begin_hook->enable()) {
        g_csystem_render_begin_original=nullptr; g_csystem_render_begin_hook.reset(); return false;
    }
    g_csystem_render_begin_installed.store(true,std::memory_order_release);
    Trace("V65.55a-SYSBEGIN installed exact CSystem+0x50 wrapper WHGame+0x52EA10; naked CD3D9Renderer::BeginFrame remains RETIRED");
    return true;
}

void DisableCSystemRenderBeginHookV6554() noexcept {
    if(g_csystem_render_begin_hook){ (void)g_csystem_render_begin_hook->disable(); g_csystem_render_begin_hook.reset(); }
    g_csystem_render_begin_original=nullptr; g_csystem_render_begin_installed.store(false,std::memory_order_release);
    g_sysbegin_self.store(0); g_sysbegin_state_lo.store(0); g_sysbegin_state_hi.store(0); g_sysbegin_context_value.store(0);
    g_sysbegin_tid.store(0); g_sysbegin_tick_ms.store(0); g_sysbegin_capture_serial.store(0); g_sysbegin_last_used_serial.store(0); g_sysbegin_last_caller_rva.store(0);
}

bool FreshSystemRenderBeginCaptureReadyV6554(void* expected_self) noexcept {
    if(!g_csystem_render_begin_original || !g_csystem_render_begin_installed.load(std::memory_order_acquire) || !expected_self) return false;
    const auto self=g_sysbegin_self.load(std::memory_order_acquire);
    const auto tid=g_sysbegin_tid.load(std::memory_order_acquire);
    const auto tick=g_sysbegin_tick_ms.load(std::memory_order_acquire);
    const auto serial=g_sysbegin_capture_serial.load(std::memory_order_acquire);
    const auto last_used=g_sysbegin_last_used_serial.load(std::memory_order_acquire);
    const auto caller=g_sysbegin_last_caller_rva.load(std::memory_order_acquire);
    const auto now=static_cast<std::uint64_t>(GetTickCount64());
    const auto age=now>=tick?now-tick:~0ull;
    return self==reinterpret_cast<std::uintptr_t>(expected_self) && ValidateCSystemV6555(expected_self) &&
           serial && serial!=last_used && caller==kCSystemRenderBeginNormalReturnRva &&
           tid==static_cast<std::uint32_t>(GetCurrentThreadId()) && age<=50ull;
}

bool InvokeFreshSystemRenderBeginV6554(void* expected_self,std::uint64_t* age_out=nullptr,std::uint64_t* serial_out=nullptr) noexcept {
    if(!g_csystem_render_begin_original || !g_csystem_render_begin_installed.load(std::memory_order_acquire) || !expected_self) return false;
    const auto self=g_sysbegin_self.load(std::memory_order_acquire);
    const auto tid=g_sysbegin_tid.load(std::memory_order_acquire);
    const auto tick=g_sysbegin_tick_ms.load(std::memory_order_acquire);
    const auto serial=g_sysbegin_capture_serial.load(std::memory_order_acquire);
    const auto last_used=g_sysbegin_last_used_serial.load(std::memory_order_acquire);
    const auto caller=g_sysbegin_last_caller_rva.load(std::memory_order_acquire);
    const auto now=static_cast<std::uint64_t>(GetTickCount64()); const auto age=now>=tick?now-tick:~0ull;
    if(age_out) *age_out=age; if(serial_out) *serial_out=serial;
    if(!self || self!=reinterpret_cast<std::uintptr_t>(expected_self) || !ValidateCSystemV6555(expected_self) ||
       !serial || serial==last_used || caller!=kCSystemRenderBeginNormalReturnRva ||
       tid!=static_cast<std::uint32_t>(GetCurrentThreadId()) || age>50ull) return false;
    alignas(16) std::uint64_t state_copy[2]{
        g_sysbegin_state_lo.load(std::memory_order_acquire),
        g_sysbegin_state_hi.load(std::memory_order_acquire)
    };
    std::uint32_t context_copy=g_sysbegin_context_value.load(std::memory_order_acquire);
    t_bridge_injected_sysbegin=true; bool ok=true;
    __try { g_csystem_render_begin_original(expected_self,state_copy,&context_copy); }
    __except(EXCEPTION_EXECUTE_HANDLER) { ok=false; }
    t_bridge_injected_sysbegin=false;
    if(!ok) return false;
    g_sysbegin_last_used_serial.store(serial,std::memory_order_release);
    g_sysbegin_injected_calls.fetch_add(1,std::memory_order_relaxed);
    return true;
}


void DisableRendererBeginFrameHookV6541() noexcept {
    if(g_renderer_begin_frame_hook){ (void)g_renderer_begin_frame_hook->disable(); g_renderer_begin_frame_hook.reset(); }
    g_renderer_begin_frame_original=nullptr; g_renderer_begin_frame_installed.store(false,std::memory_order_release);
    g_beginframe_renderer.store(0); g_beginframe_state_lo.store(0); g_beginframe_state_hi.store(0); g_beginframe_context_value.store(0);
    g_beginframe_tid.store(0); g_beginframe_tick_ms.store(0); g_beginframe_capture_serial.store(0); g_beginframe_last_used_serial.store(0);
}

// Each ORIGINAL native CSystem invocation owns its stack primary pass/view.
// The detour can render two eyes: never let either receipt span the other call.
static void CallSystemRenderScopedR24(void* self,void* render_params) noexcept {
    const auto mark=FrameSync::BeginSystemRenderFamilyR24(self,CurrentNativeFrameHJ1());
    __try {g_csystem_render_original(self,render_params);}
    __finally {FrameSync::EndSystemRenderFamilyR24(mark);}
}

void __fastcall CSystemRenderDetour(void* self, void* render_params) noexcept {
    const auto n=g_csystem_render_calls.fetch_add(1,std::memory_order_relaxed)+1;
    const auto nexus=t_nexus_serial;
    const bool in_nexus=t_nexus_depth>0 && nexus!=0;
    if(in_nexus) g_csystem_render_in_nexus.fetch_add(1,std::memory_order_relaxed);
    const auto caller=ToRva(reinterpret_cast<std::uintptr_t>(_ReturnAddress()));
    g_csystem_render_last_caller_rva.store(caller,std::memory_order_release);
    const auto present_before=DeviceHook::PresentCaptureCount();
    const auto pipeline_before=g_pipeline_calls.load(std::memory_order_acquire);
    std::uintptr_t begin_target=0,end_target=0,env_child_vt=0;
    __try {
        auto* env=*reinterpret_cast<void**>(reinterpret_cast<std::uintptr_t>(self)+0x20);
        auto* child=env?*reinterpret_cast<void**>(reinterpret_cast<std::uintptr_t>(env)+0x08):nullptr;
        if(child) {
            env_child_vt=reinterpret_cast<std::uintptr_t>(*reinterpret_cast<void***>(child));
            begin_target=SafeVslotTarget(child,0xD0/sizeof(void*));
            end_target=SafeVslotTarget(child,0xD8/sizeof(void*));
        }
    } __except(EXCEPTION_EXECUTE_HANDLER) { begin_target=end_target=env_child_vt=0; }
    if(begin_target) g_csystem_render_begin_target_rva.store(ToRva(begin_target),std::memory_order_release);
    if(end_target) g_csystem_render_end_target_rva.store(ToRva(end_target),std::memory_order_release);
    auto& slot=g_boundary[static_cast<std::size_t>(n%kBoundarySlots)];
    slot.serial.store(0,std::memory_order_release);
    slot.nexus.store(nexus,std::memory_order_relaxed);
    slot.caller_rva.store(caller,std::memory_order_relaxed);
    slot.self.store(reinterpret_cast<std::uintptr_t>(self),std::memory_order_relaxed);
    slot.params.store(reinterpret_cast<std::uintptr_t>(render_params),std::memory_order_relaxed);
    slot.entry_present.store(present_before,std::memory_order_relaxed);
    slot.producer_tid.store(static_cast<std::uint32_t>(GetCurrentThreadId()),std::memory_order_relaxed);
    slot.serial.store(n,std::memory_order_release);

    const int bridge_stage=V6601EffectiveBridgeStage();
    const int tangible_stage=g_tangible_stage.load(std::memory_order_acquire);
    if(g_csystem_render_original && tangible_stage>=1 && tangible_stage<=5 && in_nexus && g_bridge_pass_eye.load(std::memory_order_acquire)==0) {
        const auto attempt=g_bridge_dual_calls.fetch_add(1,std::memory_order_relaxed)+1;
        const auto txn_seq=g_tangible_txn_counter.fetch_add(1,std::memory_order_relaxed)+1;
        const std::uint64_t txn_serial=0xF950000000000000ull | (txn_seq & 0x0000FFFFFFFFFFFFull);
        const auto cert_wait_ms=TangibleCertificateWaitMsV6550(tangible_stage);
        g_bridge_left_crv.store(0,std::memory_order_relaxed); g_bridge_right_crv.store(0,std::memory_order_relaxed);

        // V65.50: do not infer eye completion from CSystem::Render return. The actual
        // game RT->PRESENT command list records the cache copy, and ExecuteCommandLists
        // certifies that exact list before we advance to the other eye.
        DeviceHook::BridgePrepareCertifiedPair(txn_serial);
        DeviceHook::BridgeBeginEyeRecord(txn_serial,-1);
        g_bridge_pass_eye.store(-1,std::memory_order_release);
        CallSystemRenderScopedR24(self,render_params);
        g_bridge_pass_eye.store(0,std::memory_order_release);
        const bool left_cert=DeviceHook::BridgeWaitEyeSubmitted(txn_serial,-1,cert_wait_ms);
        if(!left_cert) {
            g_bridge_left_copy_misses.fetch_add(1,std::memory_order_relaxed);
            g_tangible_left_cert_miss.fetch_add(1,std::memory_order_relaxed);
            DeviceHook::BridgeAbortSerial(txn_serial);
        }

        bool policy_ok=true, right_render=false, right_cert=false; RightEyeCVarBatchV6541 cvars{};
        if(left_cert) {
            policy_ok=ApplyTangibleRightPolicyV6548(tangible_stage,cvars);
            if(policy_ok) {
                DeviceHook::BridgeBeginEyeRecord(txn_serial,+1);
                g_bridge_pass_eye.store(+1,std::memory_order_release);
                CallSystemRenderScopedR24(self,render_params);
                g_bridge_pass_eye.store(0,std::memory_order_release);
                right_render=true; g_bridge_right_renders.fetch_add(1,std::memory_order_relaxed);
                right_cert=DeviceHook::BridgeWaitEyeSubmitted(txn_serial,+1,cert_wait_ms);
                if(!right_cert) {
                    g_tangible_right_cert_miss.fetch_add(1,std::memory_order_relaxed);
                    DeviceHook::BridgeAbortSerial(txn_serial);
                } else {
                    g_tangible_cert_pairs.fetch_add(1,std::memory_order_relaxed);
                }
            } else {
                DeviceHook::BridgeAbortSerial(txn_serial);
                g_bridge_policy_failures.fetch_add(1,std::memory_order_relaxed);
            }
            RestoreRightEyeCVarsV6541(cvars);
        }
        if(attempt<=16) Tracef("V65.50-F9-CERT attempt=%llu rung=%d nexus=%llu txn=%llu ipd=%.6f waitMs=%u leftCert=%d policy=%d mask=0x%03X right=%d rightCert=%d pairCertified=%d beginReplay=0",
            static_cast<unsigned long long>(attempt),tangible_stage,static_cast<unsigned long long>(nexus),static_cast<unsigned long long>(txn_serial),double(g_runtime_ipd.load(std::memory_order_acquire)),
            cert_wait_ms,left_cert?1:0,policy_ok?1:0,cvars.mask,right_render?1:0,right_cert?1:0,(left_cert&&right_cert)?1:0);
        const auto limit=TangibleAttemptLimitV6548(tangible_stage);
        if(limit && attempt>=limit) {
            g_tangible_stage.store(0,std::memory_order_release);
            Tracef("V65.50-F9-CERT BOUNDED complete rung=%d attempts=%llu certifiedPairs=%llu rightRenders=%llu leftCertMiss=%llu rightCertMiss=%llu policyFail=%llu -> AUTO-DISARM",
                tangible_stage,static_cast<unsigned long long>(attempt),
                static_cast<unsigned long long>(g_tangible_cert_pairs.load()),
                static_cast<unsigned long long>(g_bridge_right_renders.load()),
                static_cast<unsigned long long>(g_tangible_left_cert_miss.load()),
                static_cast<unsigned long long>(g_tangible_right_cert_miss.load()),
                static_cast<unsigned long long>(g_bridge_policy_failures.load()));
        }
    } else if(g_csystem_render_original && bridge_stage==1 && in_nexus &&
              g_bridge_pass_eye.load(std::memory_order_acquire)==0 && FreshSystemRenderBeginCaptureReadyV6554(self)) {
        const auto attempt=g_bridge_dual_calls.fetch_add(1,std::memory_order_relaxed)+1;
        const float ipd=std::clamp(g_runtime_ipd.load(std::memory_order_acquire),0.050f,0.075f);
        alignas(16) std::array<unsigned char,kSystemViewCameraSize> original_cam{};
        alignas(16) std::array<unsigned char,kSystemViewCameraSize> left_cam{};
        alignas(16) std::array<unsigned char,kSystemViewCameraSize> right_cam{};

        CameraPublicationR24 copiedFamilyR24{};
        void* live_cam=GetSystemViewCameraV6555(self);
        bool camera_ready=live_cam && CopySystemViewCameraV6555(live_cam,original_cam.data());
        if(camera_ready&&hj1::enabled.load(std::memory_order_acquire)){
            if(FrameSync::ReadSystemCameraFamilyR24(self,live_cam,copiedFamilyR24)&&
               std::memcmp(original_cam.data(),copiedFamilyR24.matrix,sizeof(copiedFamilyR24.matrix)))copiedFamilyR24={};
        }
        if(camera_ready) {
            left_cam=original_cam; right_cam=original_cam;
            camera_ready=ShiftSystemViewCameraSnapshotV6555(left_cam.data(),-0.5f*ipd) &&
                         ShiftSystemViewCameraSnapshotV6555(right_cam.data(),+0.5f*ipd);
        }
        const bool set_left=camera_ready && SetSystemViewCameraV6555(self,left_cam.data(),&copiedFamilyR24);
        if(!set_left) {
            // If native assignment ever faults after a partial copy, make one best-effort
            // restore from the captured original before falling back to the ordinary render.
            const bool fallback_restore=camera_ready && SetSystemViewCameraV6555(self,original_cam.data(),&copiedFamilyR24);
            Tracef("V65.55a-VIEWCAM REFUSED attempt=%llu nexus=%llu cameraReady=%d setLeft=0 fallbackRestore=%d -> NORMAL-SINGLE-RENDER",
                static_cast<unsigned long long>(attempt),static_cast<unsigned long long>(nexus),camera_ready?1:0,fallback_restore?1:0);
            CallSystemRenderScopedR24(self,render_params);
            g_bridge_stage.store(0,std::memory_order_release);
        } else {
            Tracef("V65.55a-VIEWCAM LEFT-SET attempt=%llu nexus=%llu live=%p ipd=%.6f tid=%lu",
                static_cast<unsigned long long>(attempt),static_cast<unsigned long long>(nexus),live_cam,double(ipd),static_cast<unsigned long>(GetCurrentThreadId()));
            t_lifetime_outer_phase=-1;
            Tracef("V65.55a-OUTER LEFT-ENTER attempt=%llu nexus=%llu self=%p sysBeginSerial=%llu",
                static_cast<unsigned long long>(attempt),static_cast<unsigned long long>(nexus),self,
                static_cast<unsigned long long>(g_sysbegin_capture_serial.load(std::memory_order_acquire)));
            CallSystemRenderScopedR24(self,render_params);
            Tracef("V65.55a-OUTER LEFT-RETURN attempt=%llu nexus=%llu",
                static_cast<unsigned long long>(attempt),static_cast<unsigned long long>(nexus));
            t_lifetime_outer_phase=0;

            // Crysis ordering: restore the original system-owned view camera BEFORE
            // the inter-eye RenderBegin. This is the key difference from V65.54b.
            const bool restore_after_left=SetSystemViewCameraV6555(self,original_cam.data(),&copiedFamilyR24);
            Tracef("V65.55a-VIEWCAM RESTORE-AFTER-LEFT attempt=%llu nexus=%llu ok=%d",
                static_cast<unsigned long long>(attempt),static_cast<unsigned long long>(nexus),restore_after_left?1:0);

            std::uint64_t begin_age=~0ull,begin_serial=0;
            const bool begin_ok=restore_after_left && InvokeFreshSystemRenderBeginV6554(self,&begin_age,&begin_serial);
            Tracef("V65.55a-SYSBEGIN REPLAY attempt=%llu nexus=%llu ok=%d serial=%llu ageMs=%llu selfMatch=%d injectedTotal=%llu",
                static_cast<unsigned long long>(attempt),static_cast<unsigned long long>(nexus),begin_ok?1:0,
                static_cast<unsigned long long>(begin_serial),static_cast<unsigned long long>(begin_age),
                g_sysbegin_self.load(std::memory_order_acquire)==reinterpret_cast<std::uintptr_t>(self)?1:0,
                static_cast<unsigned long long>(g_sysbegin_injected_calls.load(std::memory_order_acquire)));

            bool policy_ok=false, set_right=false, right_render=false, restore_after_right=true;
            RightEyeCVarBatchV6541 cvars{};
            if(begin_ok) {
                policy_ok=ApplyRightEyePolicyV6544(2,0,cvars);
                if(policy_ok) {
                    set_right=SetSystemViewCameraV6555(self,right_cam.data(),&copiedFamilyR24);
                    Tracef("V65.55a-VIEWCAM RIGHT-SET attempt=%llu nexus=%llu ok=%d",
                        static_cast<unsigned long long>(attempt),static_cast<unsigned long long>(nexus),set_right?1:0);
                    if(set_right) {
                        t_lifetime_outer_phase=+1;
                        Tracef("V65.55a-OUTER RIGHT-ENTER attempt=%llu nexus=%llu camera=SYSTEM-VIEWCAM-XR-IPD policyMask=0x%02X",
                            static_cast<unsigned long long>(attempt),static_cast<unsigned long long>(nexus),cvars.mask);
                        CallSystemRenderScopedR24(self,render_params);
                        Tracef("V65.55a-OUTER RIGHT-RETURN attempt=%llu nexus=%llu",
                            static_cast<unsigned long long>(attempt),static_cast<unsigned long long>(nexus));
                        t_lifetime_outer_phase=0;
                        right_render=true;
                        g_bridge_right_renders.fetch_add(1,std::memory_order_relaxed);
                        restore_after_right=SetSystemViewCameraV6555(self,original_cam.data(),&copiedFamilyR24);
                        Tracef("V65.55a-VIEWCAM RESTORE-AFTER-RIGHT attempt=%llu nexus=%llu ok=%d",
                            static_cast<unsigned long long>(attempt),static_cast<unsigned long long>(nexus),restore_after_right?1:0);
                    }
                } else {
                    g_bridge_policy_failures.fetch_add(1,std::memory_order_relaxed);
                }
            } else {
                g_bridge_policy_failures.fetch_add(1,std::memory_order_relaxed);
            }
            RestoreRightEyeCVarsV6541(cvars);
            // If RIGHT could not be installed, the original camera is already restored
            // from the LEFT phase. Never attempt a second render on a failed ownership step.
            Tracef("V65.55a-BRIDGE RESULT attempt=%llu nexus=%llu leftRestore=%d begin=%d policy=%d setRight=%d right=%d rightRestore=%d ipd=%.6f -> AUTO-DISARM",
                static_cast<unsigned long long>(attempt),static_cast<unsigned long long>(nexus),restore_after_left?1:0,begin_ok?1:0,policy_ok?1:0,set_right?1:0,right_render?1:0,restore_after_right?1:0,double(ipd));
            g_bridge_stage.store(0,std::memory_order_release);
        }
    } else if(g_csystem_render_original) {
        CallSystemRenderScopedR24(self,render_params);
    }

    const auto present_after=DeviceHook::PresentCaptureCount();
    const auto pipeline_after=g_pipeline_calls.load(std::memory_order_acquire);
    const auto pipeline_delta=pipeline_after>=pipeline_before?pipeline_after-pipeline_before:0ull;
    if(in_nexus && pipeline_delta) g_csystem_render_pipeline_matches.fetch_add(pipeline_delta,std::memory_order_relaxed);
    if(present_after>present_before) g_csystem_render_present_advances.fetch_add(present_after-present_before,std::memory_order_relaxed);
    const auto now=static_cast<std::uint64_t>(GetTickCount64());
    auto last=g_last_csystem_log_ms.load(std::memory_order_relaxed);
    if(n<=12 || (now>=last+1000ull && g_last_csystem_log_ms.compare_exchange_strong(last,now,std::memory_order_acq_rel))) {
        Tracef("V65.18-F8-CSYSTEM-RENDER call=%llu nexus=%llu inNexus=%d callerRva=0x%llX self=%p params=%p envChildVtRva=0x%llX bracket={D0:0x%llX D8:0x%llX} pipelineDelta=%llu presentBefore=%llu presentAfter=%llu presentDelta=%llu tid=%lu READ-ONLY",
            static_cast<unsigned long long>(n),static_cast<unsigned long long>(nexus),in_nexus?1:0,
            static_cast<unsigned long long>(caller),self,render_params,
            static_cast<unsigned long long>(ToRva(env_child_vt)),
            static_cast<unsigned long long>(ToRva(begin_target)),static_cast<unsigned long long>(ToRva(end_target)),
            static_cast<unsigned long long>(pipeline_delta),static_cast<unsigned long long>(present_before),static_cast<unsigned long long>(present_after),
            static_cast<unsigned long long>(present_after-present_before),static_cast<unsigned long>(GetCurrentThreadId()));
    }
}

void __fastcall CRenderViewDtorBodyDetour(void* self) noexcept {
    const auto n=g_lifetime_dtor_body_calls.fetch_add(1,std::memory_order_relaxed)+1;
    (void)n; TraceLifetimeObject("DTOR-BODY-ENTER",self);
    if(g_crv_dtor_body_original) g_crv_dtor_body_original(self);
}

void* __fastcall CRenderViewDeletingDtorDetour(void* self,unsigned int flags) noexcept {
    g_lifetime_deleting_dtor_calls.fetch_add(1,std::memory_order_relaxed);
    TraceLifetimeObject("DELETING-DTOR-ENTER",self,flags);
    return g_crv_deleting_dtor_original?g_crv_deleting_dtor_original(self,flags):self;
}

void __fastcall CRenderViewResetDetour(void* self) noexcept {
    g_lifetime_reset_calls.fetch_add(1,std::memory_order_relaxed);
    SignalResetBoundaryV6553(self);
    TraceLifetimeObject("RESET-427C1C-ENTER",self);
    // If the known +0x8DB0 target already lies outside the current allocation region,
    // record it before entering the retail function. Do NOT suppress or repair the call.
    if(g_crv_reset_original) g_crv_reset_original(self);
}

bool EnsureLifecycleAutopsyHooks() noexcept {
    if(g_lifetime_autopsy_installed.load(std::memory_order_acquire)) return true;
    const auto b=WhBase();
    if(!b ||
       !MatchBytes(b+kCRenderViewDtorBodyRva,kCRenderViewDtorBodyPrologue,sizeof(kCRenderViewDtorBodyPrologue)) ||
       !MatchBytes(b+kCRenderViewDeletingDtorRva,kCRenderViewDeletingDtorPrologue,sizeof(kCRenderViewDeletingDtorPrologue)) ||
       !MatchBytes(b+kCRenderViewResetRva,kCRenderViewResetPrologue,sizeof(kCRenderViewResetPrologue))) {
        Trace("V65.53-CRV-LIFE hooks REFUSED: exact retail prologue mismatch");
        return false;
    }
    DetourModKit::hook::InlineRequest req1{"kcd2vr_v6552_crv_dtor_body",DetourModKit::hook::Target{DetourModKit::Address{b+kCRenderViewDtorBodyRva}},DetourModKit::hook::Options{}};
    DetourModKit::hook::InlineRequest req2{"kcd2vr_v6552_crv_deleting_dtor",DetourModKit::hook::Target{DetourModKit::Address{b+kCRenderViewDeletingDtorRva}},DetourModKit::hook::Options{}};
    DetourModKit::hook::InlineRequest req3{"kcd2vr_v6552_crv_reset_427c1c",DetourModKit::hook::Target{DetourModKit::Address{b+kCRenderViewResetRva}},DetourModKit::hook::Options{}};
    auto h1=DetourModKit::hook::inline_at(std::move(req1),&CRenderViewDtorBodyDetour);
    auto h2=DetourModKit::hook::inline_at(std::move(req2),&CRenderViewDeletingDtorDetour);
    auto h3=DetourModKit::hook::inline_at(std::move(req3),&CRenderViewResetDetour);
    if(!h1 || !h2 || !h3) { Trace("V65.53-CRV-LIFE hook creation failed; fail-closed"); return false; }
    g_crv_dtor_body_hook=std::move(*h1); g_crv_deleting_dtor_hook=std::move(*h2); g_crv_reset_hook=std::move(*h3);
    g_crv_dtor_body_original=g_crv_dtor_body_hook->original<CRenderViewDtorBodyFn>();
    g_crv_deleting_dtor_original=g_crv_deleting_dtor_hook->original<CRenderViewDeletingDtorFn>();
    g_crv_reset_original=g_crv_reset_hook->original<CRenderViewResetFn>();
    if(!g_crv_dtor_body_original || !g_crv_deleting_dtor_original || !g_crv_reset_original) {
        Trace("V65.53-CRV-LIFE original trampoline unavailable; fail-closed");
        g_crv_dtor_body_hook.reset(); g_crv_deleting_dtor_hook.reset(); g_crv_reset_hook.reset();
        g_crv_dtor_body_original=nullptr; g_crv_deleting_dtor_original=nullptr; g_crv_reset_original=nullptr;
        return false;
    }
    const auto e1_result=g_crv_dtor_body_hook->enable();
    const bool e1=e1_result.has_value();
    bool e2=false;
    if(e1) { const auto e2_result=g_crv_deleting_dtor_hook->enable(); e2=e2_result.has_value(); }
    bool e3=false;
    if(e2) { const auto e3_result=g_crv_reset_hook->enable(); e3=e3_result.has_value(); }
    if(!e3) {
        if(g_crv_reset_hook) (void)g_crv_reset_hook->disable();
        if(g_crv_deleting_dtor_hook) (void)g_crv_deleting_dtor_hook->disable();
        if(g_crv_dtor_body_hook) (void)g_crv_dtor_body_hook->disable();
        g_crv_dtor_body_hook.reset(); g_crv_deleting_dtor_hook.reset(); g_crv_reset_hook.reset();
        g_crv_dtor_body_original=nullptr; g_crv_deleting_dtor_original=nullptr; g_crv_reset_original=nullptr;
        Trace("V65.53-CRV-LIFE enable failed; all partial hooks removed");
        return false;
    }
    g_lifetime_autopsy_installed.store(true,std::memory_order_release);
    Tracef("V65.53-CRV-LIFE installed exact chain dtorBody=+0x%llX deletingDtor=+0x%llX reset=+0x%llX faultWrite=+0x%llX field=+0x8DB0 READ-ONLY-INSTRUMENTATION",
        static_cast<unsigned long long>(kCRenderViewDtorBodyRva),static_cast<unsigned long long>(kCRenderViewDeletingDtorRva),
        static_cast<unsigned long long>(kCRenderViewResetRva),static_cast<unsigned long long>(kCRenderViewFaultWriteRva));
    return true;
}

bool EnsureCSystemRenderHook() noexcept {
    if(g_csystem_render_installed.load(std::memory_order_acquire)) return true;
    if(!EnsureNexusHook() || !EnsureGetOrCreateHook()) return false;
    const auto b=WhBase();
    if(!b || !MatchBytes(b+kCSystemRenderRva,kCSystemRenderPrologue,sizeof(kCSystemRenderPrologue))) {
        Trace("V65.17-F8 CSystem::Render boundary hook refused: exact prologue mismatch");
        return false;
    }
    DetourModKit::hook::InlineRequest req{
        "kcd2vr_v6517_f8_csystem_render_boundary",
        DetourModKit::hook::Target{DetourModKit::Address{b+kCSystemRenderRva}},
        DetourModKit::hook::Options{}};
    auto h=DetourModKit::hook::inline_at(std::move(req),&CSystemRenderDetour);
    if(!h) return false;
    g_csystem_render_hook=std::move(*h);
    g_csystem_render_original=g_csystem_render_hook->original<CSystemRenderFn>();
    if(!g_csystem_render_original || !g_csystem_render_hook->enable()) {
        g_csystem_render_original=nullptr; g_csystem_render_hook.reset(); return false;
    }
    g_csystem_render_installed.store(true,std::memory_order_release);
    Trace("V65.47-OWNER exact CSystem::Render WHGame+0x43E464 installed; ordinary mode remains observational, F8 transaction rungs may issue bounded duplicate renders; D0/D8 bracket remains READ-ONLY/UNPROVEN");
    return true;
}

void DisableCSystemRenderHook() noexcept {
    if(g_csystem_render_hook){ (void)g_csystem_render_hook->disable(); g_csystem_render_hook.reset(); }
    g_csystem_render_original=nullptr;
    g_csystem_render_installed.store(false,std::memory_order_release);
}

bool RecentHandoffContains(void* render_view) noexcept {
    const auto wanted=reinterpret_cast<std::uintptr_t>(render_view);
    if(!wanted) return false;
    for(auto& slot:g_handoff) {
        if(slot.ptr.load(std::memory_order_acquire)==wanted) return true;
    }
    return false;
}

bool CurrentNexusHandoffContains(void* render_view) noexcept {
    const auto wanted=reinterpret_cast<std::uintptr_t>(render_view);
    const auto nexus=t_nexus_serial;
    if(!wanted || nexus==0) return false;
    for(auto& slot:g_handoff) {
        if(slot.ptr.load(std::memory_order_acquire)==wanted &&
           slot.nexus.load(std::memory_order_relaxed)==nexus) return true;
    }
    return false;
}

float ActiveEyeSeparation() noexcept {
    const int mode=g_mode.load(std::memory_order_acquire);
    const int bridge_stage=V6601EffectiveBridgeStage();
    // Legacy V65.16 rungs 1..4 and the V65.37 bridge single-eye stages 3/4
    // both consume the live OpenXR IPD.  The bridge intentionally keeps the
    // retired legacy stereo mode at zero, so it must not depend on g_mode.
    if(mode>=1 && mode<=4)
        return std::clamp(g_runtime_ipd.load(std::memory_order_acquire),0.050f,0.075f);
    // V65.49 fix: the V65.48 F9 tangible path used g_bridge_pass_eye but did not
    // participate in ActiveEyeSeparation(), so its eye-build hook could report wrote=1
    // while applying zero physical separation. Tangible XR-IPD rungs must consume the
    // same live OpenXR IPD as the proven legacy camera path.
    if(g_tangible_stage.load(std::memory_order_acquire)>=1)
        return std::clamp(g_runtime_ipd.load(std::memory_order_acquire),0.050f,0.075f);
    if(bridge_stage>=1 && bridge_stage<=3)
        return std::clamp(g_runtime_ipd.load(std::memory_order_acquire),0.050f,0.075f); // V65.62 LEFT/RIGHT oracle + proven count2 path
    return 0.0f;
}

bool ShiftCameraRecord(void* camera,float meters) noexcept;


bool ApplyEyeProofTransform(void* camera,int eyeSign,int mode) noexcept {
    (void)mode;
    if(!camera) return false;
    const float sep=ActiveEyeSeparation();
    return sep>0.0f && ShiftCameraRecord(camera,0.5f*sep*float(eyeSign));
}

bool ShiftCameraRecord(void* camera,float meters) noexcept {
    if(!camera) return false;
    __try {
        auto* f=reinterpret_cast<float*>(camera);
        if(!std::isfinite(f[0])||!std::isfinite(f[4])||!std::isfinite(f[8])||
           !std::isfinite(f[3])||!std::isfinite(f[7])||!std::isfinite(f[11])) return false;
        f[3]  += f[0]*meters;
        f[7]  += f[4]*meters;
        f[11] += f[8]*meters;
        return true;
    } __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
}

thread_local bool t_stereo_build_active=false;
thread_local void* t_native_loop_self=nullptr;
thread_local unsigned t_native_loop_pair_calls=0;

void __fastcall CameraPairBuildDetour(void* out_state,void* cam_a,void* cam_b,void* context,float arg5,float arg6) noexcept {
    if(g_camera_pair_original) g_camera_pair_original(out_state,cam_a,cam_b,context,arg5,arg6);
    const bool legacy_native_loop=g_mode.load(std::memory_order_acquire)==4;
    const bool v6556_multiview=V6601EffectiveBridgeStage()==3 && g_multiview_claimed.load(std::memory_order_acquire);
    if((!legacy_native_loop && !v6556_multiview) || !t_stereo_build_active || !t_native_loop_self) return;
    ++t_native_loop_pair_calls;
    if(v6556_multiview) g_multiview_pair_calls.store(t_native_loop_pair_calls,std::memory_order_release);
    if(t_native_loop_pair_calls==1) {
        // Exact retail outer-loop proof: after the pair builder returns, native
        // code appends this eye's 0x28 metadata and then compares r14 against
        // self+0xA330. Raising the requested count here makes KCD2 execute its
        // own second selector iteration, including native shadow/culling metadata.
        __try {
            auto u=reinterpret_cast<std::uintptr_t>(t_native_loop_self);
            *reinterpret_cast<volatile std::uint64_t*>(u+kBuiltRecordCount)=2ull;
            g_native_loop_promotions.fetch_add(1,std::memory_order_relaxed);
        } __except(EXCEPTION_EXECUTE_HANDLER) {}
    }
}

void __fastcall StereoCameraBuildDetour(void* self) noexcept {
    const auto call=g_stereo_build_calls.fetch_add(1,std::memory_order_relaxed)+1;
    if(!g_stereo_build_original) return;
    if(g_v6601_owned.load(std::memory_order_acquire) &&
       (!g_v6601_command_active || self!=g_v6601_command_crv)) {
        g_stereo_build_original(self);return;
    }
    const int mode=g_mode.load(std::memory_order_acquire);
    const int bridge_stage=V6601EffectiveBridgeStage();
    const int tangible_stage=g_tangible_stage.load(std::memory_order_acquire);
    const int bridge_pass_eye=g_bridge_pass_eye.load(std::memory_order_acquire);
    const auto expected_main_crv=g_last_render_view_token.load(std::memory_order_acquire);
    const bool bridge_crv_match=self && (CurrentNexusHandoffContains(self) || (expected_main_crv!=0 && expected_main_crv==reinterpret_cast<std::uintptr_t>(self)));

    // V65.62 eye-isolation oracle. Stages 1/2 run one normal retail view only,
    // but temporarily offset that single source camera LEFT/RIGHT before KCD2 builds
    // its normal record. Stage 3 retains the proven V65.56 native count=2 path.
    if((bridge_stage==1 || bridge_stage==2) && self && !t_stereo_build_active) {
        const auto u=reinterpret_cast<std::uintptr_t>(self);
        const auto view_type=SafeReadU32(reinterpret_cast<void*>(u+0x14));
        if(view_type==0u && !g_multiview_claimed.exchange(true,std::memory_order_acq_rel)) {
            std::array<unsigned char,kCameraRecordStride> a0{},b0{};
            bool saved=false,wrote=false; std::uint32_t selector=0;
            const int sign=(bridge_stage==1)?-1:+1;
            __try {
                selector=*reinterpret_cast<volatile std::uint32_t*>(u+0x8EE0);
                const std::size_t idx=selector&1u;
                void* curA=reinterpret_cast<void*>(u+kCameraBankA+idx*kCameraRecordStride);
                void* curB=reinterpret_cast<void*>(u+kCameraBankB+idx*kCameraRecordStride);
                std::memcpy(a0.data(),curA,kCameraRecordStride);
                std::memcpy(b0.data(),curB,kCameraRecordStride);
                saved=true;
                wrote=ApplyEyeProofTransform(curA,sign,4) && ApplyEyeProofTransform(curB,sign,4);
            } __except(EXCEPTION_EXECUTE_HANDLER) { saved=false; wrote=false; }

            t_stereo_build_active=true;
            g_multiview_crv.store(u,std::memory_order_release);
            g_stereo_build_original(self);
            t_stereo_build_active=false;

            const auto count=SafeReadU32(reinterpret_cast<void*>(u+kBuiltRecordCount));
            const bool ok=saved && wrote && count==1u;
            g_multiview_pair_calls.store(1,std::memory_order_release);
            g_multiview_built_count.store(count,std::memory_order_release);
            g_multiview_build_ok.store(ok,std::memory_order_release);
            if(ok) { g_stereo_build_writes.fetch_add(1,std::memory_order_relaxed); g_last_applied_ipd.store(ActiveEyeSeparation(),std::memory_order_release); }
            else g_stereo_build_rejects.fetch_add(1,std::memory_order_relaxed);

            if(saved) {
                __try {
                    const std::size_t idx=selector&1u;
                    std::memcpy(reinterpret_cast<void*>(u+kCameraBankA+idx*kCameraRecordStride),a0.data(),kCameraRecordStride);
                    std::memcpy(reinterpret_cast<void*>(u+kCameraBankB+idx*kCameraRecordStride),b0.data(),kCameraRecordStride);
                    g_stereo_build_restores.fetch_add(1,std::memory_order_relaxed);
                } __except(EXCEPTION_EXECUTE_HANDLER) {}
            }
            const auto oracle_frame=g_v6562a_oracle_frames.fetch_add(1,std::memory_order_relaxed)+1;
            if(oracle_frame<=3 || (oracle_frame%120ull)==0ull) {
                Tracef("V65.63-EYE-ISOLATE frame=%llu eye=%s crv=%p selector=%u ipd=%.6f records=%u buildOk=%d sourceRestored=%d persistent=1 oneCSystemRender=1 onePipelineExecute=1",
                    static_cast<unsigned long long>(oracle_frame),sign<0?"LEFT":"RIGHT",self,unsigned(selector),double(ActiveEyeSeparation()),unsigned(count),ok?1:0,saved?1:0);
            }
            return;
        }
    }

    // V65.56/V65.62 stage 3: one normal render publication, two native built-view records.
    // Persistently claim one Normal CRenderView camera build per completed normal pipeline frame while stage 3 is armed.
    if(bridge_stage==3 && self && !t_stereo_build_active) {
        const auto u=reinterpret_cast<std::uintptr_t>(self);
        const auto view_type=SafeReadU32(reinterpret_cast<void*>(u+0x14));
        if(view_type==0u && !g_multiview_claimed.exchange(true,std::memory_order_acq_rel)) {
            std::array<unsigned char,kCameraRecordStride> a0{},a1{},b0{},b1{};
            bool saved=false,wrote=false; std::uint32_t selector=0;
            __try {
                selector=*reinterpret_cast<volatile std::uint32_t*>(u+0x8EE0);
                const std::size_t idx=selector&1u, alt=idx^1u;
                std::memcpy(a0.data(),reinterpret_cast<void*>(u+kCameraBankA+idx*kCameraRecordStride),kCameraRecordStride);
                std::memcpy(a1.data(),reinterpret_cast<void*>(u+kCameraBankA+alt*kCameraRecordStride),kCameraRecordStride);
                std::memcpy(b0.data(),reinterpret_cast<void*>(u+kCameraBankB+idx*kCameraRecordStride),kCameraRecordStride);
                std::memcpy(b1.data(),reinterpret_cast<void*>(u+kCameraBankB+alt*kCameraRecordStride),kCameraRecordStride);
                saved=true; // snapshots are complete before any temporary bank write
                void* curA=reinterpret_cast<void*>(u+kCameraBankA+idx*kCameraRecordStride);
                void* curB=reinterpret_cast<void*>(u+kCameraBankB+idx*kCameraRecordStride);
                void* altA=reinterpret_cast<void*>(u+kCameraBankA+alt*kCameraRecordStride);
                void* altB=reinterpret_cast<void*>(u+kCameraBankB+alt*kCameraRecordStride);
                std::memcpy(altA,a0.data(),kCameraRecordStride);
                std::memcpy(altB,b0.data(),kCameraRecordStride);
                const float sep=ActiveEyeSeparation();
                wrote=sep>0.0f && ApplyEyeProofTransform(curA,-1,4) && ApplyEyeProofTransform(curB,-1,4) &&
                      ApplyEyeProofTransform(altA,+1,4) && ApplyEyeProofTransform(altB,+1,4);
            } __except(EXCEPTION_EXECUTE_HANDLER) { saved=false; wrote=false; }

            t_stereo_build_active=true;
            t_native_loop_self=wrote?self:nullptr;
            t_native_loop_pair_calls=0;
            g_multiview_crv.store(u,std::memory_order_release);
            g_stereo_build_original(self);
            t_stereo_build_active=false;
            t_native_loop_self=nullptr;

            const auto count=SafeReadU32(reinterpret_cast<void*>(u+kBuiltRecordCount));
            const bool ok=saved && wrote && t_native_loop_pair_calls>=2 && count==2u;
            g_multiview_pair_calls.store(t_native_loop_pair_calls,std::memory_order_release);
            g_multiview_built_count.store(count,std::memory_order_release);
            g_multiview_build_ok.store(ok,std::memory_order_release);
            if(ok) { g_stereo_build_writes.fetch_add(1,std::memory_order_relaxed); g_last_applied_ipd.store(ActiveEyeSeparation(),std::memory_order_release); }
            else g_stereo_build_rejects.fetch_add(1,std::memory_order_relaxed);

            if(saved) {
                __try {
                    const std::size_t idx=selector&1u, alt=idx^1u;
                    std::memcpy(reinterpret_cast<void*>(u+kCameraBankA+idx*kCameraRecordStride),a0.data(),kCameraRecordStride);
                    std::memcpy(reinterpret_cast<void*>(u+kCameraBankA+alt*kCameraRecordStride),a1.data(),kCameraRecordStride);
                    std::memcpy(reinterpret_cast<void*>(u+kCameraBankB+idx*kCameraRecordStride),b0.data(),kCameraRecordStride);
                    std::memcpy(reinterpret_cast<void*>(u+kCameraBankB+alt*kCameraRecordStride),b1.data(),kCameraRecordStride);
                    g_stereo_build_restores.fetch_add(1,std::memory_order_relaxed);
                } __except(EXCEPTION_EXECUTE_HANDLER) {}
            }
            const auto oracle_frame=g_v6562a_oracle_frames.fetch_add(1,std::memory_order_relaxed)+1;
            if(oracle_frame<=3 || (oracle_frame%120ull)==0ull) {
                Tracef("V65.63-VIEWBUILD frame=%llu crv=%p selector=%u ipd=%.6f pairCalls=%u records=%u nativeSecondIteration=%d sourceRestored=%d persistent=1 oneCSystemRender=1 onePipelineExecute=1",
                    static_cast<unsigned long long>(oracle_frame),self,unsigned(selector),double(ActiveEyeSeparation()),unsigned(t_native_loop_pair_calls),unsigned(count),ok?1:0,saved?1:0);
            }
            t_native_loop_pair_calls=0;
            return;
        }
    }
    // V65.48: F8 research keeps the strict same-nexus owner requirement. F9 tangible
    // intentionally returns to the proven V65.39/V65.44 eligibility: active eye pass +
    // main view + recent GetOrCreateRenderView handoff. This is what previously produced
    // visible geometric stereo; bounded F9 prevents it from becoming continuous/unbounded.
    const bool tangible_crv_match=(tangible_stage>=1 && tangible_stage<=5) && self && RecentHandoffContains(self);
    const bool research_crv_match=(bridge_stage>=1 && bridge_stage<=2) && bridge_crv_match;
    if((tangible_crv_match || research_crv_match) && bridge_pass_eye!=0 && !t_stereo_build_active) {
        const auto u=reinterpret_cast<std::uintptr_t>(self);
        const auto view_type=SafeReadU32(reinterpret_cast<void*>(u+0x14));
        if(view_type==0u) {
            std::array<unsigned char,kCameraRecordStride> a0{},b0{};
            bool saved=false,wrote=false; std::uint32_t selector=0; int sign=0;
            __try {
                selector=*reinterpret_cast<volatile std::uint32_t*>(u+0x8EE0);
                const auto built_before=SafeReadU32(reinterpret_cast<void*>(u+kBuiltRecordCount));
                if(bridge_pass_eye<0) g_bridge_left_built_before.store(built_before,std::memory_order_relaxed);
                else g_bridge_right_built_before.store(built_before,std::memory_order_relaxed);
                const std::size_t idx=selector&1u;
                void* curA=reinterpret_cast<void*>(u+kCameraBankA+idx*kCameraRecordStride);
                void* curB=reinterpret_cast<void*>(u+kCameraBankB+idx*kCameraRecordStride);
                std::memcpy(a0.data(),curA,kCameraRecordStride); std::memcpy(b0.data(),curB,kCameraRecordStride); saved=true;
                sign=bridge_pass_eye;
                const float active_sep=ActiveEyeSeparation();
                wrote=(active_sep<=0.0f) ? true : (ApplyEyeProofTransform(curA,sign,1) && ApplyEyeProofTransform(curB,sign,1));
            } __except(EXCEPTION_EXECUTE_HANDLER) { saved=false; wrote=false; }
            t_stereo_build_active=true;
            g_stereo_build_original(self);
            t_stereo_build_active=false;
            if(saved) {
                __try { const std::size_t idx=selector&1u; std::memcpy(reinterpret_cast<void*>(u+kCameraBankA+idx*kCameraRecordStride),a0.data(),kCameraRecordStride); std::memcpy(reinterpret_cast<void*>(u+kCameraBankB+idx*kCameraRecordStride),b0.data(),kCameraRecordStride); } __except(EXCEPTION_EXECUTE_HANDLER) {}
            }
            const auto crv_ref=SafeReadU32(reinterpret_cast<void*>(u+0x8));
            const auto built_after=SafeReadU32(reinterpret_cast<void*>(u+kBuiltRecordCount));
            const auto built_before=(sign<0)
                ? g_bridge_left_built_before.load(std::memory_order_relaxed)
                : g_bridge_right_built_before.load(std::memory_order_relaxed);
            if(sign<0) {
                g_bridge_left_crv.store(u,std::memory_order_relaxed);
                g_bridge_left_ref.store(crv_ref,std::memory_order_relaxed);
                g_bridge_left_selector.store(selector,std::memory_order_relaxed);
                g_bridge_left_built_after.store(built_after,std::memory_order_relaxed);
            } else {
                g_bridge_right_crv.store(u,std::memory_order_relaxed);
                g_bridge_right_ref.store(crv_ref,std::memory_order_relaxed);
                g_bridge_right_selector.store(selector,std::memory_order_relaxed);
                g_bridge_right_built_after.store(built_after,std::memory_order_relaxed);
            }
            if(wrote) g_stereo_build_writes.fetch_add(1,std::memory_order_relaxed); else g_stereo_build_rejects.fetch_add(1,std::memory_order_relaxed);
            if(call<=16 || (call%300ull)==0) {
                const unsigned long long build_id = static_cast<unsigned long long>(call);
                const char* eye_name = (sign < 0) ? "LEFT" : "RIGHT";
                const unsigned int selector_value = static_cast<unsigned int>(selector);
                const double active_ipd = static_cast<double>(ActiveEyeSeparation());
                const int wrote_value = wrote ? 1 : 0;
                const int restored_value = saved ? 1 : 0;
                Tracef("V65.48-EYE-BUILD build=%llu stage=%d eye=%s crv=%p ref=%u selector=%u built=%u->%u ipd=%.6f wrote=%d sourceRestored=%d READ-ONLY-LIFETIME",
                       build_id, tangible_stage?100+tangible_stage:bridge_stage, eye_name, self, static_cast<unsigned int>(crv_ref), selector_value,
                       static_cast<unsigned int>(built_before), static_cast<unsigned int>(built_after),
                       active_ipd, wrote_value, restored_value);
            }
            return;
        }
    }
    if(mode<=0 || t_stereo_build_active || !self || !RecentHandoffContains(self)) {
        g_stereo_build_original(self);
        if(mode>0 && self && !RecentHandoffContains(self)) g_stereo_build_rejects.fetch_add(1,std::memory_order_relaxed);
        return;
    }
    if(mode==5) {
        g_stereo_build_original(self);
        return; // outer CSystem::Render/RenderBegin ownership remains read-only/locked
    }
    const auto u=reinterpret_cast<std::uintptr_t>(self);
    const auto view_type=SafeReadU32(reinterpret_cast<void*>(u+0x14));
    if(view_type!=0u) { g_stereo_build_original(self); return; }

    t_stereo_build_active=true;
    t_native_loop_self=nullptr;
    t_native_loop_pair_calls=0;
    std::array<unsigned char,kCameraRecordStride> a0{},a1{},b0{},b1{};
    bool saved=false;
    std::uint32_t selector=0;
    __try {
        selector=*reinterpret_cast<volatile std::uint32_t*>(u+0x8EE0);
        const std::size_t idx=selector&1u, alt=idx^1u;
        std::memcpy(a0.data(),reinterpret_cast<void*>(u+kCameraBankA+idx*kCameraRecordStride),kCameraRecordStride);
        std::memcpy(a1.data(),reinterpret_cast<void*>(u+kCameraBankA+alt*kCameraRecordStride),kCameraRecordStride);
        std::memcpy(b0.data(),reinterpret_cast<void*>(u+kCameraBankB+idx*kCameraRecordStride),kCameraRecordStride);
        std::memcpy(b1.data(),reinterpret_cast<void*>(u+kCameraBankB+alt*kCameraRecordStride),kCameraRecordStride);
        saved=true;
    } __except(EXCEPTION_EXECUTE_HANDLER) { saved=false; }

    const float sep=ActiveEyeSeparation();
    bool wrote=false;
    if(saved && sep>0.0f) {
        const std::size_t idx=selector&1u, alt=idx^1u;
        void* curA=reinterpret_cast<void*>(u+kCameraBankA+idx*kCameraRecordStride);
        void* curB=reinterpret_cast<void*>(u+kCameraBankB+idx*kCameraRecordStride);
        if(mode==4) {
            // Native-loop/metadata proof: prepare both selector banks up front.
            void* altA=reinterpret_cast<void*>(u+kCameraBankA+alt*kCameraRecordStride);
            void* altB=reinterpret_cast<void*>(u+kCameraBankB+alt*kCameraRecordStride);
            __try {
                std::memcpy(altA,a0.data(),kCameraRecordStride);
                std::memcpy(altB,b0.data(),kCameraRecordStride);
                wrote=ApplyEyeProofTransform(curA,-1,mode) && ApplyEyeProofTransform(curB,-1,mode) &&
                      ApplyEyeProofTransform(altA,+1,mode) && ApplyEyeProofTransform(altB,+1,mode);
            } __except(EXCEPTION_EXECUTE_HANDLER) { wrote=false; }
            if(wrote) t_native_loop_self=self;
        } else {
            // Ordinary first output becomes LEFT; modes 1/2/3 use the older IPD clone path.
            wrote=ApplyEyeProofTransform(curA,-1,mode) && ApplyEyeProofTransform(curB,-1,mode);
        }
    }

    g_stereo_build_original(self);

    if(mode==4) {
        const auto count=SafeReadU32(reinterpret_cast<void*>(u+kBuiltRecordCount));
        wrote = saved && wrote && t_native_loop_pair_calls>=2 && count>=2;
        if(wrote) {
            g_stereo_build_writes.fetch_add(1,std::memory_order_relaxed);
            g_last_applied_ipd.store(sep,std::memory_order_release);
        }
    } else if(saved && wrote && g_camera_pair_builder) {
        const std::size_t idx=selector&1u, alt=idx^1u;
        __try {
            void* curA=reinterpret_cast<void*>(u+kCameraBankA+idx*kCameraRecordStride);
            void* curB=reinterpret_cast<void*>(u+kCameraBankB+idx*kCameraRecordStride);
            void* altA=reinterpret_cast<void*>(u+kCameraBankA+alt*kCameraRecordStride);
            void* altB=reinterpret_cast<void*>(u+kCameraBankB+alt*kCameraRecordStride);
            std::memcpy(curA,a0.data(),kCameraRecordStride);
            std::memcpy(curB,b0.data(),kCameraRecordStride);
            std::memcpy(altA,a0.data(),kCameraRecordStride);
            std::memcpy(altB,b0.data(),kCameraRecordStride);
            if(ApplyEyeProofTransform(altA,+1,mode) && ApplyEyeProofTransform(altB,+1,mode)) {
                void* globals=*reinterpret_cast<void**>(WhBase()+kRendererGlobalsRva);
                void* context=*reinterpret_cast<void**>(u+0xA720);
                void* camera_owner=*reinterpret_cast<void**>(u+0x8D60);
                float arg5=globals?*reinterpret_cast<float*>(reinterpret_cast<std::uintptr_t>(globals)+0x1678):0.0f;
                float arg6=camera_owner?*reinterpret_cast<float*>(reinterpret_cast<std::uintptr_t>(camera_owner)+0x18C8):0.0f;
                void* out0=reinterpret_cast<void*>(u+kBuiltOutput0);
                void* out1=reinterpret_cast<void*>(u+kBuiltOutput0+kBuiltOutputStride);
                g_camera_pair_builder(out1,altA,altB,context,arg5,arg6);
                // Historical A/B path: manually cloned tail. Kept unchanged so
                // rung 5 can be compared directly against this known visual result.
                std::memcpy(reinterpret_cast<unsigned char*>(out1)+0x3CC,
                            reinterpret_cast<unsigned char*>(out0)+0x3CC,0x28);
                *reinterpret_cast<volatile std::uint64_t*>(u+kBuiltRecordCount)=2ull;
                wrote=true;
                g_stereo_build_writes.fetch_add(1,std::memory_order_relaxed);
                g_last_applied_ipd.store(sep,std::memory_order_release);
            } else wrote=false;
        } __except(EXCEPTION_EXECUTE_HANDLER) { wrote=false; }
    }

    if(saved) {
        __try {
            const std::size_t idx=selector&1u, alt=idx^1u;
            std::memcpy(reinterpret_cast<void*>(u+kCameraBankA+idx*kCameraRecordStride),a0.data(),kCameraRecordStride);
            std::memcpy(reinterpret_cast<void*>(u+kCameraBankA+alt*kCameraRecordStride),a1.data(),kCameraRecordStride);
            std::memcpy(reinterpret_cast<void*>(u+kCameraBankB+idx*kCameraRecordStride),b0.data(),kCameraRecordStride);
            std::memcpy(reinterpret_cast<void*>(u+kCameraBankB+alt*kCameraRecordStride),b1.data(),kCameraRecordStride);
            g_stereo_build_restores.fetch_add(1,std::memory_order_relaxed);
        } __except(EXCEPTION_EXECUTE_HANDLER) {}
    }
    if(!wrote) g_stereo_build_rejects.fetch_add(1,std::memory_order_relaxed);

    if(call<=8 || (wrote && (call%600ull)==0)) {
        Tracef("V65.11-F8-STEREO build=%llu mode=%d crv=%p selector=%u separation=%.6f wrote=%d count=%u pairCalls=%u nativeMeta=%d singlePipeline=1",
            static_cast<unsigned long long>(call),mode,self,unsigned(selector),double(sep),wrote?1:0,
            unsigned(SafeReadU32(reinterpret_cast<void*>(u+kBuiltRecordCount))),t_native_loop_pair_calls,mode==4?1:0);
    }
    t_native_loop_self=nullptr;
    t_native_loop_pair_calls=0;
    t_stereo_build_active=false;
}

bool EnsureStereoBuildHook() noexcept {
    if(g_stereo_build_installed.load(std::memory_order_acquire)) return true;
    const auto b=WhBase();
    if(!b ||
       !MatchBytes(b+kCRenderViewCameraBuildRva,kCRenderViewCameraBuildPrologue,sizeof(kCRenderViewCameraBuildPrologue)) ||
       !MatchBytes(b+kCRenderViewCameraPairBuildRva,kCRenderViewCameraPairBuildPrologue,sizeof(kCRenderViewCameraPairBuildPrologue))) {
        Trace("V65.10-F8 stereo hook refused: camera-build or native pair-builder exact bytes mismatch");
        return false;
    }
    g_camera_pair_builder=reinterpret_cast<CRenderViewCameraPairBuildFn>(b+kCRenderViewCameraPairBuildRva);
    DetourModKit::hook::InlineRequest req{
        "kcd2vr_v6510_f8_stereo_camera_build",
        DetourModKit::hook::Target{DetourModKit::Address{b+kCRenderViewCameraBuildRva}},
        DetourModKit::hook::Options{}};
    auto h=DetourModKit::hook::inline_at(std::move(req),&StereoCameraBuildDetour);
    if(!h) return false;
    g_stereo_build_hook=std::move(*h);
    g_stereo_build_original=g_stereo_build_hook->original<CRenderViewCameraBuildFn>();
    if(!g_stereo_build_original || !g_stereo_build_hook->enable()) {
        g_stereo_build_original=nullptr; g_stereo_build_hook.reset(); return false;
    }
    g_stereo_build_installed.store(true,std::memory_order_release);
    Trace("V65.10-F8 active camera-build hook installed at WHGame+0x865550; native pair builder +0x866F6C; whole-frame duplicate path remains retired");
    return true;
}

void DisableCameraPairHook() noexcept {
    if(g_camera_pair_hook){ (void)g_camera_pair_hook->disable(); g_camera_pair_hook.reset(); }
    g_camera_pair_original=nullptr;
    g_camera_pair_installed.store(false,std::memory_order_release);
}

bool EnsureCameraPairHook() noexcept {
    if(g_camera_pair_installed.load(std::memory_order_acquire)) return true;
    const auto b=WhBase();
    if(!b || !MatchBytes(b+kCRenderViewCameraPairBuildRva,kCRenderViewCameraPairBuildPrologue,sizeof(kCRenderViewCameraPairBuildPrologue))) return false;
    DetourModKit::hook::InlineRequest req{
        "kcd2vr_v6511_f8_native_pair_loop",
        DetourModKit::hook::Target{DetourModKit::Address{b+kCRenderViewCameraPairBuildRva}},
        DetourModKit::hook::Options{}};
    auto h=DetourModKit::hook::inline_at(std::move(req),&CameraPairBuildDetour);
    if(!h) return false;
    g_camera_pair_hook=std::move(*h);
    g_camera_pair_original=g_camera_pair_hook->original<CRenderViewCameraPairBuildFn>();
    if(!g_camera_pair_original || !g_camera_pair_hook->enable()) {
        g_camera_pair_original=nullptr; g_camera_pair_hook.reset(); return false;
    }
    g_camera_pair_installed.store(true,std::memory_order_release);
    Trace("V65.14-F8 rung4 native pair-loop hook installed at WHGame+0x866F6C; eye-2 metadata remains game-owned");
    return true;
}



void DisableBackendExecutionHooks() noexcept {
    if(g_backend_range_hook){ (void)g_backend_range_hook->disable(); g_backend_range_hook.reset(); }
    if(g_backend_exec_hook){ (void)g_backend_exec_hook->disable(); g_backend_exec_hook.reset(); }
    g_backend_range_original=nullptr; g_backend_exec_original=nullptr;
}



static std::uintptr_t WhRva(std::uintptr_t p) noexcept;
static std::uint64_t SafeFnv1a64(const unsigned char* b,std::uint64_t bytes) noexcept;


#include "native_stereo_reaudit_v6599.inc"

void LogSecondaryStageState(const char* tag, void* stage, std::uint32_t callno, std::uintptr_t callerRva) noexcept {
    const int bridgeStage=V6601EffectiveBridgeStage();
    const int diagMode=V6601EffectiveDiag();
    const bool prod=g_v6626_native_stage_census.load(std::memory_order_acquire);
    if(!stage) return;
    const auto su=reinterpret_cast<std::uintptr_t>(stage);
    const auto state=SafeReadPtr(reinterpret_cast<const void*>(su+0x8));
    const auto crv=SafeReadPtr(reinterpret_cast<const void*>(su+0x28));
    const auto flags=state?SafeReadU32(reinterpret_cast<const void*>(state+0x1968)):0u;
    const auto count=crv?SafeReadU32(reinterpret_cast<const void*>(crv+kBuiltRecordCount)):0u;
    const auto selector=crv?SafeReadU32(reinterpret_cast<const void*>(crv+0x8EE0)):0u;
    const auto stageVt=SafeReadPtr(stage);
    g_v6626_last_stage_selector=(selector<=1u)?static_cast<int>(selector):-1;
    if(selector<=1u && tag && tag[0]=='G') g_v6628_gate_eye[selector].fetch_add(1,std::memory_order_relaxed); // V66.28
    std::uint32_t secondarySeen=0;
    if(flags&0x4000u) secondarySeen=g_v6565_secondary_flag_seen.fetch_add(1,std::memory_order_relaxed)+1;
    if(prod && selector<=1u){
        auto& ctr=(tag && tag[0]=='G')?(selector?g_v6626_stage_gate_eye1:g_v6626_stage_gate_eye0):(selector?g_v6626_stage_apply_eye1:g_v6626_stage_apply_eye0);
        const auto n=ctr.fetch_add(1,std::memory_order_relaxed)+1;
        if(n<=16 || (n%600ull)==0){
            const auto vtRva=WhRva(stageVt);
            const char* semantic=(vtRva==kV6631ShadowMapStageVtableRva)?"CShadowMapStage":
                                 (vtRva==kV6631ShadowMaskStageVtableRva)?"CShadowMaskStage":
                                 (vtRva==kV6631VolCloudStageVtableRva)?"CVolumetricCloudsStage":
                                 (vtRva==kV6631PostAAStageVtableRva)?"CPostAAStage":
                                 (vtRva==kV6631ScreenSpaceReflectionsStageVtableRva)?"CScreenSpaceReflectionsStage":"UNKNOWN-STAGE";
            Tracef("V66.31-STAGE %s semantic=%s eye=%s n=%llu selector=%u secondary4000=%u flags=0x%08X nativeCount=%u callerRva=0x%llX stageVtRva=0x%llX contract=%s",
                tag,semantic,selector?"RIGHT":"LEFT",(unsigned long long)n,unsigned(selector),unsigned((flags&0x4000u)!=0),unsigned(flags),unsigned(count),
                (unsigned long long)callerRva,(unsigned long long)vtRva,
                selector?"MANUAL-RIGHT-REENTRY":"NORMAL-PRIMARY-FRAME");
        }
    }
    // Preserve the older bounded research log when its explicit lab is active.
    if(diagMode==2 && (bridgeStage==2 || bridgeStage==3) && (callno<=32 || ((flags&0x4000u) && secondarySeen<=32))) {
        Tracef("V65.95A-SHADOW-LINEAGE %s call=%u eye=%s callerRva=0x%llX stage=%p stageVtRva=0x%llX state=%p flags=0x%08X secondary4000=%u crv=%p nativeCount=%u rec0=%p rec1=%p",
            tag,unsigned(callno),bridgeStage==2?"RIGHT":"COUNT2+BLOCK1",static_cast<unsigned long long>(callerRva),stage,
            static_cast<unsigned long long>(WhRva(stageVt)),reinterpret_cast<void*>(state),unsigned(flags),unsigned((flags&0x4000u)!=0),
            reinterpret_cast<void*>(crv),unsigned(count),crv?reinterpret_cast<void*>(crv+kBuiltOutput0):nullptr,
            (crv && count>1)?reinterpret_cast<void*>(crv+kBuiltOutput0+kBuiltOutputStride):nullptr);
    }
}


bool V6639R1SafeCopy(const void* src,void* dst,std::size_t bytes) noexcept {
    if(!src||!dst||!bytes)return false;
    __try { std::memcpy(dst,src,bytes); return true; }
    __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
}
bool V6639R1SafeWrite(void* dst,const void* src,std::size_t bytes) noexcept {
    if(!src||!dst||!bytes)return false;
    __try { std::memcpy(dst,src,bytes); return true; }
    __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
}
std::uint64_t V6639R1HashShadowInputs(std::uintptr_t crv) noexcept {
    if(!crv)return 0;
    std::array<unsigned char,160> b{}; std::size_t o=0;
    auto cp=[&](std::size_t off,std::size_t n)->bool{
        if(o+n>b.size())return false;
        if(!V6639R1SafeCopy(reinterpret_cast<const void*>(crv+off),b.data()+o,n))return false;
        o+=n; return true;
    };
    if(!cp(0x9B40,12)||!cp(0x9B64,20)||!cp(0x9BC0,64)||!cp(0x9CC0,64))return 0;
    return SafeFnv1a64(b.data(),o);
}
bool V6639R1ReadShadowInputs(std::uintptr_t crv,V6639R1CenterShadowSnapshot& out) noexcept {
    if(!crv)return false;
    return V6639R1SafeCopy(reinterpret_cast<const void*>(crv+0x9B40),out.block9b40.data(),out.block9b40.size()) &&
           V6639R1SafeCopy(reinterpret_cast<const void*>(crv+0x9B64),out.block9b64.data(),out.block9b64.size()) &&
           V6639R1SafeCopy(reinterpret_cast<const void*>(crv+0x9BC0),out.block9bc0.data(),out.block9bc0.size()) &&
           V6639R1SafeCopy(reinterpret_cast<const void*>(crv+0x9CC0),out.block9cc0.data(),out.block9cc0.size());
}
bool V6639R1WriteShadowInputs(std::uintptr_t crv,const V6639R1CenterShadowSnapshot& in) noexcept {
    if(!crv)return false;
    return V6639R1SafeWrite(reinterpret_cast<void*>(crv+0x9B40),in.block9b40.data(),in.block9b40.size()) &&
           V6639R1SafeWrite(reinterpret_cast<void*>(crv+0x9B64),in.block9b64.data(),in.block9b64.size()) &&
           V6639R1SafeWrite(reinterpret_cast<void*>(crv+0x9BC0),in.block9bc0.data(),in.block9bc0.size()) &&
           V6639R1SafeWrite(reinterpret_cast<void*>(crv+0x9CC0),in.block9cc0.data(),in.block9cc0.size());
}
bool V6639R1EqualShadowInputs(const V6639R1CenterShadowSnapshot& a,const V6639R1CenterShadowSnapshot& b) noexcept {
    return std::memcmp(a.block9b40.data(),b.block9b40.data(),a.block9b40.size())==0 &&
           std::memcmp(a.block9b64.data(),b.block9b64.data(),a.block9b64.size())==0 &&
           std::memcmp(a.block9bc0.data(),b.block9bc0.data(),a.block9bc0.size())==0 &&
           std::memcmp(a.block9cc0.data(),b.block9cc0.data(),a.block9cc0.size())==0;
}

void __fastcall V6639R1ShadowMapUpdateDetour(void* stage) noexcept {
    const auto n=g_v6639r1_shadowmap_update_calls.fetch_add(1,std::memory_order_relaxed)+1ull;
    const auto su=reinterpret_cast<std::uintptr_t>(stage);
    const auto crv=stage?SafeReadPtr(reinterpret_cast<const void*>(su+0x28)):0;
    const auto selector=crv?SafeReadU32(reinterpret_cast<const void*>(crv+0x8EE0)):~0u;
    const auto frame=crv?SafeReadU32(reinterpret_cast<const void*>(crv+0x2C)):0u;
    const auto before=V6639R1HashShadowInputs(crv);
    bool nativeReturned=false;
    if(g_v6639r1_shadowmap_update_original) {
        __try { g_v6639r1_shadowmap_update_original(stage); nativeReturned=true; }
        __except(EXCEPTION_EXECUTE_HANDLER) { nativeReturned=false; }
    }
    const auto after=V6639R1HashShadowInputs(crv);
    if(n<=24ull || (n%600ull)==0ull || !nativeReturned) Tracef(
        "V66.39R1-PAIR-WIDE stage=SHADOWMAP-UPDATE n=%llu selector=%u frame=%u crv=%p inputBefore=%016llX inputAfter=%016llX nativeRefreshMayChange=%d nativeReturned=%d mutation=NONE proofPoint=POST-REFRESH-CONSUMER",
        (unsigned long long)n,unsigned(selector),unsigned(frame),reinterpret_cast<void*>(crv),
        (unsigned long long)before,(unsigned long long)after,before!=after?1:0,nativeReturned?1:0);
}

// Proof-gated mutation at the exact post-refresh ShadowMap consumer. ShadowMap::Update
// first calls WHGame+0x4E893C on the CRenderView and only then tail-calls this helper.
// Therefore readback here proves the exact values the native shadow preparation is
// about to consume; no intervening native camera refresh can silently overwrite them.
void __fastcall V6639R1ShadowMapConsumeDetour(void* shadow_state,void* render_view) noexcept {
    const auto n=g_v6639r1_shadowmap_consume_calls.fetch_add(1,std::memory_order_relaxed)+1ull;
    const auto crv=reinterpret_cast<std::uintptr_t>(render_view);
    const auto selector=crv?SafeReadU32(reinterpret_cast<const void*>(crv+0x8EE0)):~0u;
    const auto frame=crv?SafeReadU32(reinterpret_cast<const void*>(crv+0x2C)):0u;
    const auto recHash=crv?SafeFnv1a64(reinterpret_cast<const unsigned char*>(crv+kBuiltOutput0),kBuiltOutputStride):0ull;
    const auto inputHashBefore=V6639R1HashShadowInputs(crv);
    const int step=g_v6639r1_pairwide_step.load(std::memory_order_acquire);
    const bool requested=(step==1 || step==3);
    bool mutationAttempted=false,subReadback=false,nativeReturned=false,restoreReadback=false,distinct=false;
    const char* reason="CENSUS";
    V6639R1CenterShadowSnapshot left{};

    if(requested) {
        if(!shadow_state||!crv){reason="NO-STATE-OR-CRV";}
        else if(selector!=0u){reason="NOT-PRIMARY-SELECTOR";}
        else if(!g_v6639r1_center_shadow.valid){reason="NO-CENTER-SNAPSHOT";}
        else if(g_v6639r1_center_shadow.crv!=crv){reason="CENTER-CRV-MISMATCH";}
        else if(g_v6639r1_center_shadow.frame!=frame){reason="CENTER-FRAME-MISMATCH";}
        else if(!V6639R1ReadShadowInputs(crv,left)){reason="LEFT-INPUT-READ-FAILED";}
        else {
            distinct=!V6639R1EqualShadowInputs(left,g_v6639r1_center_shadow) &&
                     g_v6639r1_center_shadow.center_record_hash!=recHash;
            if(!distinct){reason="CENTER-NOT-DISTINCT-FROM-LEFT";}
            else {
                mutationAttempted=true;
                const bool writeOk=V6639R1WriteShadowInputs(crv,g_v6639r1_center_shadow);
                V6639R1CenterShadowSnapshot rb{};
                subReadback=writeOk&&V6639R1ReadShadowInputs(crv,rb)&&V6639R1EqualShadowInputs(rb,g_v6639r1_center_shadow);
                if(subReadback) reason="ARMED-AT-EXACT-CONSUMER";
                else reason=writeOk?"SUBSTITUTE-READBACK-FAILED":"SUBSTITUTE-WRITE-FAILED";
            }
        }
    }

    // Call native only with a fully verified center substitution or untouched LEFT data.
    // If a partial write occurred but did not read back, restore LEFT before native use.
    if(mutationAttempted && !subReadback) {
        const bool rw=V6639R1WriteShadowInputs(crv,left);
        V6639R1CenterShadowSnapshot rr{};
        restoreReadback=rw&&V6639R1ReadShadowInputs(crv,rr)&&V6639R1EqualShadowInputs(rr,left);
        if(!restoreReadback) reason="PRE-NATIVE-RESTORE-FAILED";
    }

    const bool safeToCall=!mutationAttempted || subReadback || restoreReadback;
    if(safeToCall && g_v6639r1_shadowmap_consume_original) {
        __try { g_v6639r1_shadowmap_consume_original(shadow_state,render_view); nativeReturned=true; }
        __except(EXCEPTION_EXECUTE_HANDLER) { nativeReturned=false; }
    }

    if(subReadback) {
        const bool rw=V6639R1WriteShadowInputs(crv,left);
        V6639R1CenterShadowSnapshot rr{};
        restoreReadback=rw&&V6639R1ReadShadowInputs(crv,rr)&&V6639R1EqualShadowInputs(rr,left);
        if(!nativeReturned) reason="NATIVE-CONSUMER-FAULT";
        else if(!restoreReadback) reason="POST-NATIVE-RESTORE-FAILED";
    } else if(!mutationAttempted) {
        restoreReadback=true;
    }

    const bool active=requested&&distinct&&subReadback&&nativeReturned&&restoreReadback;
    if(requested || n<=24ull || (n%600ull)==0ull) Tracef(
        "V66.39R1-PAIR-WIDE stage=SHADOWMAP-CONSUME n=%llu step=%d requested=%d selector=%u frame=%u crv=%p shadowState=%p recHash=%016llX centerRec=%016llX sourceCam=%016llX inputAtExactConsumer=%016llX distinct=%d mutationAttempted=%d substituteReadback=%d nativeReturned=%d restoreReadback=%d result=%s reason=%s",
        (unsigned long long)n,step,requested?1:0,unsigned(selector),unsigned(frame),render_view,shadow_state,
        (unsigned long long)recHash,(unsigned long long)g_v6639r1_center_shadow.center_record_hash,
        (unsigned long long)g_v6639r1_center_shadow.source_camera_hash,(unsigned long long)inputHashBefore,
        distinct?1:0,mutationAttempted?1:0,subReadback?1:0,nativeReturned?1:0,restoreReadback?1:0,
        active?"ACTIVE":(requested?"NOT-ACTIVE":"CENSUS"),reason);
}

void __fastcall V6639R1ShadowMapExecuteDetour(void* stage) noexcept {
    const auto n=g_v6639r1_shadowmap_execute_calls.fetch_add(1,std::memory_order_relaxed)+1ull;
    const auto su=reinterpret_cast<std::uintptr_t>(stage);
    const auto crv=stage?SafeReadPtr(reinterpret_cast<const void*>(su+0x28)):0;
    const auto selector=crv?SafeReadU32(reinterpret_cast<const void*>(crv+0x8EE0)):~0u;
    const auto frame=crv?SafeReadU32(reinterpret_cast<const void*>(crv+0x2C)):0u;
    if(n<=24ull || (n%600ull)==0ull) {
        Tracef("V66.39R1-PAIR-WIDE stage=SHADOWMAP-EXECUTE n=%llu selector=%u frame=%u crv=%p rec0=%016llX shadowInputs=%016llX note=RIGHT-GENERATION-NATIVELY-GATED",
            (unsigned long long)n,unsigned(selector),unsigned(frame),reinterpret_cast<void*>(crv),
            (unsigned long long)(crv?SafeFnv1a64(reinterpret_cast<const unsigned char*>(crv+kBuiltOutput0),kBuiltOutputStride):0ull),
            (unsigned long long)V6639R1HashShadowInputs(crv));
    }
    if(g_v6639r1_shadowmap_execute_original) g_v6639r1_shadowmap_execute_original(stage);
}

void __fastcall V6639R1SsrExecuteDetour(void* stage) noexcept {
    const auto su=reinterpret_cast<std::uintptr_t>(stage);
    const auto state=stage?SafeReadPtr(reinterpret_cast<const void*>(su+0x20)):0;
    const auto owner=stage?SafeReadPtr(reinterpret_cast<const void*>(su+0x10)):0;
    const auto mode=state?SafeReadU32(reinterpret_cast<const void*>(state+0x334)):0u;
    const std::uintptr_t selectedOff=mode?0x188ull:0x230ull;
    const auto selected=owner?SafeReadPtr(reinterpret_cast<const void*>(owner+selectedOff)):0;
    const auto backing=selected?SafeReadPtr(reinterpret_cast<const void*>(selected+0x80)):0;
    std::uint16_t w=0,h=0;
    if(selected) {
        __try { w=*reinterpret_cast<const volatile std::uint16_t*>(selected+0x90); h=*reinterpret_cast<const volatile std::uint16_t*>(selected+0x92); }
        __except(EXCEPTION_EXECUTE_HANDLER) { w=h=0; }
    }
    const auto crv=stage?SafeReadPtr(reinterpret_cast<const void*>(su+0x28)):0;
    const auto selector=crv?SafeReadU32(reinterpret_cast<const void*>(crv+0x8EE0)):~0u;
    const auto frame=crv?SafeReadU32(reinterpret_cast<const void*>(crv+0x2C)):0u;
    static thread_local std::uint32_t leftFrame=0;
    static thread_local std::uintptr_t leftSelected=0,leftBacking=0;
    if(selector==0u){leftFrame=frame;leftSelected=selected;leftBacking=backing;}
    const bool paired=(selector==1u && frame==leftFrame);
    const bool sameSelected=paired && selected==leftSelected;
    const bool sameBacking=paired && backing==leftBacking;
    auto& ctr=(selector==1u)?g_v6639r1_ssr_eye1:g_v6639r1_ssr_eye0;
    const auto n=ctr.fetch_add(1,std::memory_order_relaxed)+1ull;
    const int step=g_v6639r1_pairwide_step.load(std::memory_order_acquire);
    if(selector<=1u && (step>=2 || n<=24ull || (n%600ull)==0ull)) {
        Tracef("V66.39R1-SSR-SOURCE eye=%s n=%llu step=%d frame=%u mode334=%u owner=%p selectedOff=0x%llX selected=%p backing=%p size=%ux%u crv=%p rec0=%016llX pairedWithLeft=%d sameSelected=%d sameBacking=%d leftSelected=%p leftBacking=%p poseLast=%llu poseCurrent=%llu result=CENSUS-ONLY",
            selector?"RIGHT":"LEFT",(unsigned long long)n,step,unsigned(frame),unsigned(mode),reinterpret_cast<void*>(owner),
            (unsigned long long)selectedOff,reinterpret_cast<void*>(selected),reinterpret_cast<void*>(backing),unsigned(w),unsigned(h),
            reinterpret_cast<void*>(crv),(unsigned long long)(crv?SafeFnv1a64(reinterpret_cast<const unsigned char*>(crv+kBuiltOutput0),kBuiltOutputStride):0ull),
            paired?1:0,sameSelected?1:0,sameBacking?1:0,reinterpret_cast<void*>(leftSelected),reinterpret_cast<void*>(leftBacking),
            (unsigned long long)CameraHook::LastPoseFrameIndex(),(unsigned long long)CameraHook::CurrentPoseFrameIndex());
    }
    if(g_v6639r1_ssr_execute_original) g_v6639r1_ssr_execute_original(stage);
}

std::atomic<bool> g_v6644_skip_right_shadowmask{false};
std::atomic<std::uint64_t> g_v6644_right_shadowmask_skips{0};

void __fastcall V6639ShadowMaskExecuteDetour(void* stage) noexcept {
    if(!stage){ if(g_v6639_shadowmask_execute_original) g_v6639_shadowmask_execute_original(stage); return; }
    const auto su=reinterpret_cast<std::uintptr_t>(stage);
    const auto pipeline=SafeReadPtr(reinterpret_cast<const void*>(su+0x8));
    const auto crv=SafeReadPtr(reinterpret_cast<const void*>(su+0x28));
    const auto pipelineCrv=pipeline?SafeReadPtr(reinterpret_cast<const void*>(pipeline+0x1978)):0;
    const auto chosen=crv?crv:pipelineCrv;
    const auto count=chosen?SafeReadU32(reinterpret_cast<const void*>(chosen+kBuiltRecordCount)):0u;
    const auto selector=chosen?SafeReadU32(reinterpret_cast<const void*>(chosen+0x8EE0)):0u;
    const auto flags=pipeline?SafeReadU32(reinterpret_cast<const void*>(pipeline+0x2C)):0u;
    const auto h0=chosen?SafeFnv1a64(reinterpret_cast<const unsigned char*>(chosen+kBuiltOutput0),kBuiltOutputStride):0ull;
    const auto h1=(chosen && count>1)?SafeFnv1a64(reinterpret_cast<const unsigned char*>(chosen+kBuiltOutput0+kBuiltOutputStride),kBuiltOutputStride):0ull;
    // V66.44 bisect: RIGHT-pass ShadowMask skip (same effect as its gate returning false).
    if(selector==1u && g_v6644_skip_right_shadowmask.load(std::memory_order_relaxed)) {
        g_v6644_right_shadowmask_skips.fetch_add(1,std::memory_order_relaxed);
        return;
    }
    auto& ctr=(selector==1u)?g_v6639_shadowmask_eye1:g_v6639_shadowmask_eye0;
    const auto n=ctr.fetch_add(1,std::memory_order_relaxed)+1ull;
    if(selector<=1u && (n<=24ull || (n%600ull)==0ull)) {
        Tracef("V66.39R1-PRIMARY-VIEW stage=SHADOWMASK phase=ENTER eye=%s n=%llu stage=%p pipeline=%p stageCrv=%p pipelineCrv=%p chosenCrv=%p selector=%u builtCount=%u rec0=%016llX rec1=%016llX shadowInputs=%016llX pipelineFlags=0x%08X callerRva=0x%llX poseLast=%llu poseCurrent=%llu",
            selector?"RIGHT":"LEFT",(unsigned long long)n,stage,reinterpret_cast<void*>(pipeline),reinterpret_cast<void*>(crv),reinterpret_cast<void*>(pipelineCrv),reinterpret_cast<void*>(chosen),
            unsigned(selector),unsigned(count),(unsigned long long)h0,(unsigned long long)h1,(unsigned long long)V6639R1HashShadowInputs(chosen),unsigned(flags),(unsigned long long)WhRva(reinterpret_cast<std::uintptr_t>(_ReturnAddress())),
            (unsigned long long)CameraHook::LastPoseFrameIndex(),(unsigned long long)CameraHook::CurrentPoseFrameIndex());
    }
    if(g_v6639_shadowmask_execute_original) g_v6639_shadowmask_execute_original(stage);
    if(selector<=1u && (n<=24ull || (n%600ull)==0ull)) {
        const auto h0b=chosen?SafeFnv1a64(reinterpret_cast<const unsigned char*>(chosen+kBuiltOutput0),kBuiltOutputStride):0ull;
        Tracef("V66.39R1-PRIMARY-VIEW stage=SHADOWMASK phase=EXIT eye=%s n=%llu rec0Before=%016llX rec0After=%016llX mutated=%d",
            selector?"RIGHT":"LEFT",(unsigned long long)n,(unsigned long long)h0,(unsigned long long)h0b,h0!=h0b?1:0);
    }
}

bool __fastcall SecondaryGateCheckDetour(void* stage) noexcept {
    const auto n=g_v6565_gate_calls.fetch_add(1,std::memory_order_relaxed)+1;
    const auto callerRva=WhRva(reinterpret_cast<std::uintptr_t>(_ReturnAddress()));
    LogSecondaryStageState("GATE",stage,n,callerRva);
    const bool result=g_secondary_gate_check_original?g_secondary_gate_check_original(stage):false;
    if(stage) {
        const auto su=reinterpret_cast<std::uintptr_t>(stage);
        const auto vt=SafeReadPtr(stage);
        if(WhRva(vt)==kV6631ShadowMapStageVtableRva) {
            const auto crv=SafeReadPtr(reinterpret_cast<const void*>(su+0x28));
            const auto selector=crv?SafeReadU32(reinterpret_cast<const void*>(crv+0x8EE0)):~0u;
            static std::atomic<std::uint64_t> l{0},r{0};
            auto& c=(selector==1u)?r:l; const auto k=c.fetch_add(1,std::memory_order_relaxed)+1ull;
            if(selector<=1u && (k<=16ull || (k%600ull)==0ull)) Tracef(
                "V66.39R1-SHADOWMAP-GATE eye=%s n=%llu selector=%u nativeResult=%d interpretation=%s",
                selector?"RIGHT":"LEFT",(unsigned long long)k,unsigned(selector),result?1:0,
                selector?(result?"UNEXPECTED-RIGHT-GENERATION":"RIGHT-REUSES-PRIMARY-MAPS"):(result?"PRIMARY-GENERATES-MAPS":"UNEXPECTED-PRIMARY-SKIP"));
        }
    }
    return result;
}

void __fastcall SecondaryStageApplyDetour(void* stage) noexcept {
    const auto n=g_v6565_apply_calls.fetch_add(1,std::memory_order_relaxed)+1;
    const int bridgeStage=V6601EffectiveBridgeStage();
    const auto callerRva=WhRva(reinterpret_cast<std::uintptr_t>(_ReturnAddress()));
    LogSecondaryStageState("APPLY",stage,n,callerRva);
    if(bridgeStage==3) {
        const auto su=reinterpret_cast<std::uintptr_t>(stage);
        const auto crv=stage?SafeReadPtr(reinterpret_cast<const void*>(su+0x28)):0;
        const auto count=crv?SafeReadU32(reinterpret_cast<const void*>(crv+kBuiltRecordCount)):0u;
        DeviceHook::NotifyV6593SecondaryApply(count,crv);
    }
    if(g_secondary_stage_apply_original) g_secondary_stage_apply_original(stage);
}

bool __fastcall SecondaryShaderModeDetour(void* owner,void* desc,void* view,std::uint8_t mode,void* out) noexcept {
    std::uint32_t n=0,mode2n=0;
    const int bridgeStage=V6601EffectiveBridgeStage();
    const bool count2=bridgeStage==3;
    const bool shadowCompare=(V6601EffectiveDiag()>=2) && (bridgeStage==2 || bridgeStage==3);
    if(count2) {
        n=g_v6565_mode_calls.fetch_add(1,std::memory_order_relaxed)+1;
        if(mode==2) mode2n=g_v6565_mode2_calls.fetch_add(1,std::memory_order_relaxed)+1;
    }
    const auto caller=reinterpret_cast<std::uintptr_t>(_ReturnAddress());
    const auto callerRva=WhRva(caller);
    // V66.36: mode mutation is retired. Log mode lineage only; cloud-state
    // ownership is tested at the exact cloud setup functions below.
    const int v6635_rung=g_v6635_secondary_view_ladder.load(std::memory_order_relaxed);
    const bool v6635_right=(g_v6626_last_stage_selector==1);
    const bool result=g_secondary_shader_mode_original?g_secondary_shader_mode_original(owner,desc,view,mode,out):false;
    if(v6635_rung>=1 && v6635_right) {
        static std::atomic<std::uint64_t> s_v6636_mode_log{0};
        const auto ln=s_v6636_mode_log.fetch_add(1,std::memory_order_relaxed)+1;
        if(ln<=32 || (ln%1200ull)==0)
            Tracef("V66.36-MODE-OBSERVE rung=%d eye=RIGHT mode=%u result=%u callerRva=0x%llX owner=%p view=%p mutation=NONE",
                v6635_rung,unsigned(mode),unsigned(result),(unsigned long long)callerRva,owner,view);
    }
    if(g_v6626_last_stage_selector==0 || g_v6626_last_stage_selector==1)
        g_v6628_mode_eye[g_v6626_last_stage_selector].fetch_add(1,std::memory_order_relaxed); // V66.28
    if(g_v6626_native_stage_census.load(std::memory_order_acquire) && (g_v6626_last_stage_selector==0 || g_v6626_last_stage_selector==1)) {
        auto& ctr=g_v6626_last_stage_selector?g_v6626_mode_eye1:g_v6626_mode_eye0;
        const auto pn=ctr.fetch_add(1,std::memory_order_relaxed)+1;
        if(pn<=24 || (mode==2 && pn<=64) || (pn%1200ull)==0)
            Tracef("V66.26-NATIVE-MODE eye=%s n=%llu mode=%u secondaryMode=%u result=%u callerRva=0x%llX owner=%p view=%p",
                g_v6626_last_stage_selector?"RIGHT":"LEFT",(unsigned long long)pn,unsigned(mode),unsigned(mode==2),unsigned(result),
                (unsigned long long)callerRva,owner,view);
    }
    if(count2 && (n<=24 || (mode==2 && mode2n<=16))) {
        const auto produced=out?SafeReadPtr(out):0;
        const auto vtable=produced?SafeReadPtr(reinterpret_cast<const void*>(produced)):0;
        Tracef("V65.69-MODE-RESULT call=%u mode=%u secondaryMode=%u result=%u owner=%p desc=%p view=%p out=%p produced=%p vtable=%p",
            unsigned(n),unsigned(mode),unsigned(mode==2),unsigned(result),owner,desc,view,out,
            reinterpret_cast<void*>(produced),reinterpret_cast<void*>(vtable));
    }
    if(shadowCompare) {
        static std::atomic<std::uint32_t> s_right{0}, s_count2{0};
        auto& ctr=(bridgeStage==2)?s_right:s_count2;
        const auto sn=ctr.fetch_add(1,std::memory_order_relaxed)+1;
        if(sn<=64 || (mode==2 && sn<=128)) {
            const auto produced=out?SafeReadPtr(out):0;
            Tracef("V65.95A-SHADER-MODE eye=%s n=%u callerRva=0x%llX mode=%u secondary=%u result=%u owner=%p desc=%p view=%p produced=%p producedVtRva=0x%llX",
                bridgeStage==2?"RIGHT":"COUNT2+BLOCK1",unsigned(sn),static_cast<unsigned long long>(callerRva),unsigned(mode),unsigned(mode==2),unsigned(result),
                owner,desc,view,reinterpret_cast<void*>(produced),static_cast<unsigned long long>(WhRva(produced?SafeReadPtr(reinterpret_cast<const void*>(produced)):0)));
        }
    }
    return result;
}

static std::uint64_t SafeReadU64(const void* p) noexcept {
    if(!p) return 0;
    __try { return *reinterpret_cast<const std::uint64_t*>(p); }
    __except(EXCEPTION_EXECUTE_HANDLER) { return 0; }
}

static std::uint64_t SafeFnv1a64(const unsigned char* b,std::uint64_t bytes) noexcept {
    if(!b || bytes==0 || bytes>(4ull*1024ull*1024ull)) return 0ull;
    __try {
        std::uint64_t h=1469598103934665603ull;
        for(std::uint64_t i=0;i<bytes;++i) h=(h^b[i])*1099511628211ull;
        return h;
    } __except(EXCEPTION_EXECUTE_HANDLER) { return 0ull; }
}

static void SafeCopyU64s(const unsigned char* b,std::uint64_t bytes,std::uint64_t* out,unsigned maxQwords) noexcept {
    if(!out || maxQwords==0) return;
    for(unsigned i=0;i<maxQwords;++i) out[i]=0;
    if(!b) return;
    __try {
        for(unsigned i=0;i<maxQwords && (static_cast<std::uint64_t>(i)+1ull)*8ull<=bytes;++i)
            std::memcpy(&out[i],b+static_cast<std::uint64_t>(i)*8ull,8);
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        for(unsigned i=0;i<maxQwords;++i) out[i]=0;
    }
}

static bool SafeCopyBytes(void* dst,const void* src,std::size_t bytes) noexcept {
    if(!dst || !src || bytes==0) return false;
    __try { std::memcpy(dst,src,bytes); return true; }
    __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
}

static std::uintptr_t WhRva(std::uintptr_t p) noexcept {
    const auto b=WhBase();
    return (b && p>=b && p<b+0x05B2D000ull)?(p-b):0;
}

static bool SeenProvenancePtr(std::uintptr_t p) noexcept {
    if(!p) return true;
    for(auto& cell:g_v6568_seen_ptrs) {
        auto cur=cell.load(std::memory_order_acquire);
        if(cur==p) return true;
        if(cur==0) {
            std::uintptr_t expected=0;
            if(cell.compare_exchange_strong(expected,p,std::memory_order_acq_rel)) return false;
            if(expected==p) return true;
        }
    }
    return true;
}

static const char* ClassifyPtr(std::uintptr_t p,std::uintptr_t& vt,std::uintptr_t& vtRva) noexcept {
    vt=0; vtRva=0;
    if(!p) return "NULL";
    const auto r=WhRva(p);
    if(r) return "WHGAME";
    vt=SafeReadPtr(reinterpret_cast<const void*>(p));
    vtRva=WhRva(vt);
    if(vtRva) return "HEAP-WHVT";
    if(vt) {
        const auto m0=SafeReadPtr(reinterpret_cast<const void*>(vt));
        const auto m1=SafeReadPtr(reinterpret_cast<const void*>(vt+sizeof(void*)));
        if(m0 && m1) return "HEAP-EXTVT";
    }
    return "SCALAR/UNKNOWN";
}

static void TracePointerProvenance(const char* root,const char* lane,unsigned slot,std::uintptr_t owner,unsigned field,std::uintptr_t p) noexcept {
    if(!p || SeenProvenancePtr(p)) return;
    const auto n=g_v6568_provenance_lines.fetch_add(1,std::memory_order_relaxed)+1;
    if(n>48) return;
    std::uintptr_t vt=0,vtRva=0;
    const char* kind=ClassifyPtr(p,vt,vtRva);
    std::uint64_t q[6]{};
    if(std::strcmp(kind,"HEAP-WHVT")==0 || std::strcmp(kind,"HEAP-EXTVT")==0) {
        for(unsigned i=0;i<6;++i) q[i]=SafeReadU64(reinterpret_cast<const void*>(p+i*8ull));
    }
    Tracef("V65.71-POINTER-PROVENANCE n=%u root=%s lane=%s slot=%u owner=%p field=%u ptr=%p kind=%s ptrRva=0x%llX vtable=%p vtableRva=0x%llX qwords={%016llX,%016llX,%016llX,%016llX,%016llX,%016llX}",
        unsigned(n),root,lane?lane:"NA",slot,reinterpret_cast<void*>(owner),field,reinterpret_cast<void*>(p),kind,
        static_cast<unsigned long long>(WhRva(p)),reinterpret_cast<void*>(vt),static_cast<unsigned long long>(vtRva),
        static_cast<unsigned long long>(q[0]),static_cast<unsigned long long>(q[1]),static_cast<unsigned long long>(q[2]),static_cast<unsigned long long>(q[3]),static_cast<unsigned long long>(q[4]),static_cast<unsigned long long>(q[5]));
}


static void TraceQ8Family(const char* lane,unsigned slot,std::uintptr_t child,const std::uint64_t* q) noexcept;

static bool MarkChildFirstSeen(std::uintptr_t p) noexcept {
    if(!p) return false;
    for(auto& cell:g_v6569_seen_children) {
        auto cur=cell.load(std::memory_order_acquire);
        if(cur==p) return false;
        if(cur==0) {
            std::uintptr_t expected=0;
            if(cell.compare_exchange_strong(expected,p,std::memory_order_acq_rel)) return true;
            if(expected==p) return false;
        }
    }
    return false;
}

static void TraceModeChildPayload(const char* lane,unsigned slot,std::uintptr_t parent,std::uintptr_t child) noexcept {
    if(!child || !MarkChildFirstSeen(child)) return;
    const auto vt=SafeReadPtr(reinterpret_cast<const void*>(child));
    if(WhRva(vt)!=0x3A54440ull) return;
    const auto n=g_v6569_child_payloads.fetch_add(1,std::memory_order_relaxed)+1;
    std::uint64_t q[40]{};
    for(unsigned i=0;i<40;++i) q[i]=SafeReadU64(reinterpret_cast<const void*>(child+i*8ull));
    Tracef("V65.71-CHILD-HEADER n=%u lane=%s slot=%u parent=%p child=%p vtableRva=0x%llX flags08=0x%08X owner10=%p field18=0x%016llX field20=%p",
        unsigned(n),lane,slot,reinterpret_cast<void*>(parent),reinterpret_cast<void*>(child),
        static_cast<unsigned long long>(WhRva(vt)),unsigned(SafeReadU32(reinterpret_cast<const void*>(child+0x8))),
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(q[2])),static_cast<unsigned long long>(q[3]),reinterpret_cast<void*>(static_cast<std::uintptr_t>(q[4])));
    for(unsigned base=0;base<40;base+=8) {
        Tracef("V65.71-CHILD-PAYLOAD n=%u lane=%s slot=%u child=%p q%02u-%02u={%016llX,%016llX,%016llX,%016llX,%016llX,%016llX,%016llX,%016llX}",
            unsigned(n),lane,slot,reinterpret_cast<void*>(child),base,base+7,
            static_cast<unsigned long long>(q[base+0]),static_cast<unsigned long long>(q[base+1]),static_cast<unsigned long long>(q[base+2]),static_cast<unsigned long long>(q[base+3]),
            static_cast<unsigned long long>(q[base+4]),static_cast<unsigned long long>(q[base+5]),static_cast<unsigned long long>(q[base+6]),static_cast<unsigned long long>(q[base+7]));
    }
    TraceQ8Family(lane,slot,child,q);
    // +0x28 onward is the statically proven copied payload. Classify only plausible nonzero qwords from that region.
    for(unsigned i=5;i<40;++i) {
        const auto v=static_cast<std::uintptr_t>(q[i]);
        if(v) TracePointerProvenance("MODECHILD",lane,slot,child,i,v);
    }
}


static bool MarkQ8FirstSeen(std::uintptr_t p) noexcept {
    if(!p) return false;
    for(auto& cell:g_v6571_seen_q8){
        auto cur=cell.load(std::memory_order_acquire);
        if(cur==p) return false;
        if(cur==0){ std::uintptr_t e=0; if(cell.compare_exchange_strong(e,p,std::memory_order_acq_rel)) return true; if(e==p) return false; }
    }
    return false;
}

static std::uint64_t V6581HashDxbc(std::uintptr_t p,std::uint64_t n) noexcept {
    if(!p || n<32 || n>(16ull*1024ull*1024ull)) return 0;
    std::uint64_t h=1469598103934665603ull;
    __try {
        const auto* b=reinterpret_cast<const std::uint8_t*>(p);
        for(std::uint64_t i=0;i<n;++i){ h^=b[i]; h*=1099511628211ull; }
    } __except(EXCEPTION_EXECUTE_HANDLER) { return 0; }
    return h;
}

static bool V6581RegisterModeHash(const char* lane,std::uint64_t h) noexcept {
    if(!lane || !h) return false;
    auto* arr=(std::strcmp(lane,"MODE2")==0)?g_v6581_mode2_ps_hashes:
              ((std::strcmp(lane,"MODE0")==0)?g_v6581_mode0_ps_hashes:nullptr);
    if(!arr) return false;
    for(unsigned i=0;i<16;++i){
        auto cur=arr[i].load(std::memory_order_acquire);
        if(cur==h) return false;
        if(cur==0){ std::uint64_t expected=0; if(arr[i].compare_exchange_strong(expected,h,std::memory_order_acq_rel)) return true; if(expected==h) return false; }
    }
    return false;
}

static void TraceQ8Family(const char* lane,unsigned slot,std::uintptr_t child,const std::uint64_t* q) noexcept {
    const auto q6=static_cast<std::uintptr_t>(q[6]);
    const auto q7=q[7];
    const auto q8=static_cast<std::uintptr_t>(q[8]);
    const auto q9=q[9];
    Tracef("V65.71-Q8Q9 lane=%s slot=%u child=%p q6=%p q7=0x%llX q8=%p q9=0x%llX",lane,slot,reinterpret_cast<void*>(child),reinterpret_cast<void*>(q6),(unsigned long long)q7,reinterpret_cast<void*>(q8),(unsigned long long)q9);
    const auto shaderHash=V6581HashDxbc(q8,q9);
    if(shaderHash && V6581RegisterModeHash(lane,shaderHash)) {
        Tracef("V65.82-MODE-DXBC lane=%s slot=%u hash=%016llX bytes=%llu q8=%p",lane,slot,(unsigned long long)shaderHash,(unsigned long long)q9,reinterpret_cast<void*>(q8));
        const int laneId=(std::strcmp(lane,"MODE2")==0)?2:1;
        DeviceHook::NotifyModeShaderHashLearned(laneId,shaderHash);
    }
    if(!q8 || !MarkQ8FirstSeen(q8)) return;
    const auto n=g_v6571_q8_fingerprints.fetch_add(1,std::memory_order_relaxed)+1;
    std::uintptr_t vt=0,vtRva=0; const char* kind=ClassifyPtr(q8,vt,vtRva);
    std::uint64_t f[24]{}; for(unsigned i=0;i<24;++i) f[i]=SafeReadU64(reinterpret_cast<const void*>(q8+i*8ull));
    Tracef("V65.71-Q8-FINGERPRINT n=%u lane=%s slot=%u obj=%p kind=%s vtable=%p vtableRva=0x%llX q0-11={%016llX,%016llX,%016llX,%016llX,%016llX,%016llX,%016llX,%016llX,%016llX,%016llX,%016llX,%016llX}",
        unsigned(n),lane,slot,reinterpret_cast<void*>(q8),kind,reinterpret_cast<void*>(vt),(unsigned long long)vtRva,
        (unsigned long long)f[0],(unsigned long long)f[1],(unsigned long long)f[2],(unsigned long long)f[3],(unsigned long long)f[4],(unsigned long long)f[5],(unsigned long long)f[6],(unsigned long long)f[7],(unsigned long long)f[8],(unsigned long long)f[9],(unsigned long long)f[10],(unsigned long long)f[11]);
    for(unsigned i=0;i<24;++i){ const auto v=static_cast<std::uintptr_t>(f[i]); if(v) TracePointerProvenance("Q8OBJ",lane,slot,q8,i,v); }
}

static void FingerprintModeObject(const char* lane,unsigned slot,std::uintptr_t obj) noexcept {
    if(!obj) return;
    bool fresh=false;
    for(auto& cell:g_v6567_seen_objects) {
        auto cur=cell.load(std::memory_order_acquire);
        if(cur==obj) return;
        if(cur==0) {
            std::uintptr_t expected=0;
            if(cell.compare_exchange_strong(expected,obj,std::memory_order_acq_rel)) { fresh=true; break; }
            if(expected==obj) return;
        }
    }
    if(!fresh) return;
    const auto n=g_v6567_fingerprint_calls.fetch_add(1,std::memory_order_relaxed)+1;
    const auto vt=SafeReadPtr(reinterpret_cast<const void*>(obj));
    const auto vtRva=WhRva(vt);
    const auto col=vt?SafeReadPtr(reinterpret_cast<const void*>(vt-sizeof(void*))):0;
    const auto colRva=WhRva(col);
    std::uintptr_t methods[10]{};
    for(unsigned i=0;i<10;++i) methods[i]=vt?SafeReadPtr(reinterpret_cast<const void*>(vt+i*sizeof(void*))):0;
    std::uint64_t q[12]{};
    for(unsigned i=0;i<12;++i) q[i]=SafeReadU64(reinterpret_cast<const void*>(obj+i*8ull));
    for(unsigned i=1;i<12;++i) TracePointerProvenance("MODEOBJ",lane,slot,obj,i,static_cast<std::uintptr_t>(q[i]));
    TraceModeChildPayload(lane,slot,obj,static_cast<std::uintptr_t>(q[10]));
    Tracef("V65.71-OBJECT-FINGERPRINT n=%u lane=%s slot=%u obj=%p vtable=%p vtableRva=0x%llX col=%p colRva=0x%llX methods={0x%llX,0x%llX,0x%llX,0x%llX,0x%llX,0x%llX,0x%llX,0x%llX,0x%llX,0x%llX} qwords={%016llX,%016llX,%016llX,%016llX,%016llX,%016llX,%016llX,%016llX,%016llX,%016llX,%016llX,%016llX}",
        n,lane,slot,reinterpret_cast<void*>(obj),reinterpret_cast<void*>(vt),
        static_cast<unsigned long long>(vtRva),reinterpret_cast<void*>(col),static_cast<unsigned long long>(colRva),
        static_cast<unsigned long long>(WhRva(methods[0])),static_cast<unsigned long long>(WhRva(methods[1])),static_cast<unsigned long long>(WhRva(methods[2])),static_cast<unsigned long long>(WhRva(methods[3])),static_cast<unsigned long long>(WhRva(methods[4])),
        static_cast<unsigned long long>(WhRva(methods[5])),static_cast<unsigned long long>(WhRva(methods[6])),static_cast<unsigned long long>(WhRva(methods[7])),static_cast<unsigned long long>(WhRva(methods[8])),static_cast<unsigned long long>(WhRva(methods[9])),
        static_cast<unsigned long long>(q[0]),static_cast<unsigned long long>(q[1]),static_cast<unsigned long long>(q[2]),static_cast<unsigned long long>(q[3]),static_cast<unsigned long long>(q[4]),static_cast<unsigned long long>(q[5]),
        static_cast<unsigned long long>(q[6]),static_cast<unsigned long long>(q[7]),static_cast<unsigned long long>(q[8]),static_cast<unsigned long long>(q[9]),static_cast<unsigned long long>(q[10]),static_cast<unsigned long long>(q[11]));
}

void* __fastcall ModeResultAssignDetour(void* dst,const void* src) noexcept {
    if(!g_mode_result_assign_original) return dst;
    const auto base=WhBase();
    const auto ret=reinterpret_cast<std::uintptr_t>(_ReturnAddress());
    const auto rva=(base && ret>=base)?ret-base:ret;
    const bool exact=rva==kMode0Assign0ReturnRva||rva==kMode0Assign1ReturnRva||rva==kMode2Assign0ReturnRva||rva==kMode2Assign1ReturnRva;
    const auto srcObj=src?SafeReadPtr(src):0;
    const auto oldObj=dst?SafeReadPtr(dst):0;
    void* result=g_mode_result_assign_original(dst,src);
    if(V6601EffectiveBridgeStage()==3 && exact) {
        const auto after=dst?SafeReadPtr(dst):0;
        const char* lane=(rva==kMode2Assign0ReturnRva||rva==kMode2Assign1ReturnRva)?"MODE2":"MODE0";
        const unsigned slot=(rva==kMode0Assign0ReturnRva||rva==kMode2Assign0ReturnRva)?0u:1u;
        const auto n=g_v6566_assign_calls.fetch_add(1,std::memory_order_relaxed)+1;
        if(n<=32) Tracef("V65.69-MODE-ASSIGN call=%u lane=%s slot=%u callerRva=0x%llX dst=%p old=%p src=%p srcObj=%p after=%p",
            unsigned(n),lane,slot,static_cast<unsigned long long>(rva),dst,reinterpret_cast<void*>(oldObj),src,
            reinterpret_cast<void*>(srcObj),reinterpret_cast<void*>(after));
        FingerprintModeObject(lane,slot,after);
    }
    return result;
}

static std::uint32_t V6631SelectorFromStage(void* stage) noexcept {
    if(!stage) return 0xFFFFFFFFu;
    const auto su=reinterpret_cast<std::uintptr_t>(stage);
    const auto crv=SafeReadPtr(reinterpret_cast<const void*>(su+0x28));
    return crv?SafeReadU32(reinterpret_cast<const void*>(crv+0x8EE0)):0xFFFFFFFFu;
}

static std::uint32_t V6631BackingMask(void* stage,std::uint32_t& objectMask) noexcept {
    objectMask=0;
    if(!stage) return 0;
    constexpr std::size_t offs[]={0x30,0x38,0x40,0x48,0x50,0x58,0x70,0x78,0x88,0x90,0xC8};
    const auto su=reinterpret_cast<std::uintptr_t>(stage);
    std::uint32_t backing=0;
    for(std::uint32_t i=0;i<sizeof(offs)/sizeof(offs[0]);++i){
        const auto obj=SafeReadPtr(reinterpret_cast<const void*>(su+offs[i]));
        if(obj){ objectMask|=(1u<<i); if(SafeReadPtr(reinterpret_cast<const void*>(obj+0x80))) backing|=(1u<<i); }
    }
    return backing;
}

bool __fastcall V6631CloudReadinessDetour(void* stage) noexcept {
    const auto selector=V6631SelectorFromStage(stage);
    std::uint32_t objectMask=0; const auto backingMask=V6631BackingMask(stage,objectMask);
    std::uint64_t resourceSig=1469598103934665603ull;
    if(stage) {
        constexpr std::size_t offs[]={0x30,0x38,0x40,0x48,0x50,0x58,0x70,0x78,0x88,0x90,0xC8};
        const auto su=reinterpret_cast<std::uintptr_t>(stage);
        for(auto off:offs){ const auto obj=SafeReadPtr(reinterpret_cast<const void*>(su+off)); const auto backing=obj?SafeReadPtr(reinterpret_cast<const void*>(obj+0x80)):0; resourceSig^=obj; resourceSig*=1099511628211ull; resourceSig^=backing; resourceSig*=1099511628211ull; }
    }
    const bool result=g_v6631_cloud_readiness_original?g_v6631_cloud_readiness_original(stage):false;
    if(selector<=1u){
        auto& calls=selector?g_v6631_cloud_ready_eye1:g_v6631_cloud_ready_eye0;
        auto& fails=selector?g_v6631_cloud_ready_fail_eye1:g_v6631_cloud_ready_fail_eye0;
        const auto n=calls.fetch_add(1,std::memory_order_relaxed)+1;
        const auto fn=result?fails.load(std::memory_order_relaxed):(fails.fetch_add(1,std::memory_order_relaxed)+1);
        if(n<=20 || !result || (n%600ull)==0ull){
            Tracef("V66.36-CLOUD-READY eye=%s n=%llu result=%d objects=0x%03X backings=0x%03X resourceSig=%016llX failCount=%llu stage=%p callerRva=0x%llX",
                selector?"RIGHT":"LEFT",(unsigned long long)n,result?1:0,unsigned(objectMask),unsigned(backingMask),(unsigned long long)resourceSig,(unsigned long long)fn,stage,
                (unsigned long long)WhRva(reinterpret_cast<std::uintptr_t>(_ReturnAddress())));
        }
    }
    return result;
}

void __fastcall V6631CloudEyeSetupDetour(void* stage,void* desc) noexcept {
    const auto selector=V6631SelectorFromStage(stage);
    std::uint64_t before=0,after=0; std::uintptr_t state=0;
    if(stage && selector<=1u){ state=reinterpret_cast<std::uintptr_t>(stage)+0x6B0ull+std::uintptr_t(selector)*0x170ull; before=SafeFnv1a64(reinterpret_cast<const unsigned char*>(state),0x170); }
    if(g_v6631_cloud_eye_setup_original) g_v6631_cloud_eye_setup_original(stage,desc);
    const int v6636_rung=g_v6635_secondary_view_ladder.load(std::memory_order_relaxed);
    if(state && selector==0u) {
        __try { std::memcpy(g_v6636_cloud_eye_left,reinterpret_cast<const void*>(state),0x170); g_v6636_cloud_eye_left_valid.store(true,std::memory_order_release); }
        __except(EXCEPTION_EXECUTE_HANDLER) { g_v6636_cloud_eye_left_valid.store(false,std::memory_order_release); }
    } else if(state && selector==1u && (v6636_rung==2 || v6636_rung==4) && g_v6636_cloud_eye_left_valid.load(std::memory_order_acquire)) {
        __try { std::memcpy(reinterpret_cast<void*>(state),g_v6636_cloud_eye_left,0x170); g_v6635_secondary_forces.fetch_add(1,std::memory_order_relaxed); }
        __except(EXCEPTION_EXECUTE_HANDLER) {}
    }
    if(state) after=SafeFnv1a64(reinterpret_cast<const unsigned char*>(state),0x170);
    if(selector<=1u){
        auto& ctr=selector?g_v6631_cloud_eye_setup_eye1:g_v6631_cloud_eye_setup_eye0;
        const auto n=ctr.fetch_add(1,std::memory_order_relaxed)+1;
        if(n<=20 || before!=after || (n%600ull)==0ull) Tracef("V66.31-CLOUD-EYE-STATE eye=%s n=%llu state=%p hashBefore=%016llX hashAfter=%016llX changed=%d desc=%p",
            selector?"RIGHT":"LEFT",(unsigned long long)n,reinterpret_cast<void*>(state),(unsigned long long)before,(unsigned long long)after,before!=after?1:0,desc);
    }
}

void __fastcall V6631CloudShadowSetupDetour(void* stage,void* desc) noexcept {
    const auto selector=V6631SelectorFromStage(stage);
    std::uint64_t before=0,after=0; std::uintptr_t state=0;
    if(stage && selector<=1u){ state=reinterpret_cast<std::uintptr_t>(stage)+0x3D0ull+std::uintptr_t(selector)*0x170ull; before=SafeFnv1a64(reinterpret_cast<const unsigned char*>(state),0x170); }
    if(g_v6631_cloud_shadow_setup_original) g_v6631_cloud_shadow_setup_original(stage,desc);
    const int v6636_rung=g_v6635_secondary_view_ladder.load(std::memory_order_relaxed);
    if(state && selector==0u) {
        __try { std::memcpy(g_v6636_cloud_shadow_left,reinterpret_cast<const void*>(state),0x170); g_v6636_cloud_shadow_left_valid.store(true,std::memory_order_release); }
        __except(EXCEPTION_EXECUTE_HANDLER) { g_v6636_cloud_shadow_left_valid.store(false,std::memory_order_release); }
    } else if(state && selector==1u && (v6636_rung==3 || v6636_rung==4) && g_v6636_cloud_shadow_left_valid.load(std::memory_order_acquire)) {
        __try { std::memcpy(reinterpret_cast<void*>(state),g_v6636_cloud_shadow_left,0x170); g_v6635_secondary_forces.fetch_add(1,std::memory_order_relaxed); }
        __except(EXCEPTION_EXECUTE_HANDLER) {}
    }
    if(state) after=SafeFnv1a64(reinterpret_cast<const unsigned char*>(state),0x170);
    if(selector<=1u){
        auto& ctr=selector?g_v6631_cloud_shadow_setup_eye1:g_v6631_cloud_shadow_setup_eye0;
        const auto n=ctr.fetch_add(1,std::memory_order_relaxed)+1;
        if(n<=20 || before!=after || (n%600ull)==0ull) Tracef("V66.31-CLOUD-SHADOW-STATE eye=%s n=%llu state=%p hashBefore=%016llX hashAfter=%016llX changed=%d desc=%p NOTE=cloud-local-not-general-shadow-map",
            selector?"RIGHT":"LEFT",(unsigned long long)n,reinterpret_cast<void*>(state),(unsigned long long)before,(unsigned long long)after,before!=after?1:0,desc);
    }
}

void DisableSecondaryViewGateCensusHooks() noexcept {
    if(g_v6639r1_ssr_execute_hook){ (void)g_v6639r1_ssr_execute_hook->disable(); g_v6639r1_ssr_execute_hook.reset(); }
    if(g_v6639r1_shadowmap_consume_hook){ (void)g_v6639r1_shadowmap_consume_hook->disable(); g_v6639r1_shadowmap_consume_hook.reset(); }
    if(g_v6639r1_shadowmap_execute_hook){ (void)g_v6639r1_shadowmap_execute_hook->disable(); g_v6639r1_shadowmap_execute_hook.reset(); }
    if(g_v6639r1_shadowmap_update_hook){ (void)g_v6639r1_shadowmap_update_hook->disable(); g_v6639r1_shadowmap_update_hook.reset(); }
    if(g_v6639_shadowmask_execute_hook){ (void)g_v6639_shadowmask_execute_hook->disable(); g_v6639_shadowmask_execute_hook.reset(); }
    if(g_v6631_cloud_eye_setup_hook){ (void)g_v6631_cloud_eye_setup_hook->disable(); g_v6631_cloud_eye_setup_hook.reset(); }
    if(g_v6631_cloud_shadow_setup_hook){ (void)g_v6631_cloud_shadow_setup_hook->disable(); g_v6631_cloud_shadow_setup_hook.reset(); }
    if(g_v6631_cloud_readiness_hook){ (void)g_v6631_cloud_readiness_hook->disable(); g_v6631_cloud_readiness_hook.reset(); }
    if(g_mode_result_assign_hook){ (void)g_mode_result_assign_hook->disable(); g_mode_result_assign_hook.reset(); }
    if(g_secondary_shader_mode_hook){ (void)g_secondary_shader_mode_hook->disable(); g_secondary_shader_mode_hook.reset(); }
    if(g_secondary_stage_apply_hook){ (void)g_secondary_stage_apply_hook->disable(); g_secondary_stage_apply_hook.reset(); }
    if(g_secondary_gate_check_hook){ (void)g_secondary_gate_check_hook->disable(); g_secondary_gate_check_hook.reset(); }
    g_secondary_gate_check_original=nullptr; g_secondary_stage_apply_original=nullptr; g_secondary_shader_mode_original=nullptr; g_mode_result_assign_original=nullptr;
    g_v6631_cloud_readiness_original=nullptr; g_v6631_cloud_shadow_setup_original=nullptr; g_v6631_cloud_eye_setup_original=nullptr; g_v6639_shadowmask_execute_original=nullptr;
    g_v6639r1_shadowmap_update_original=nullptr; g_v6639r1_shadowmap_execute_original=nullptr; g_v6639r1_shadowmap_consume_original=nullptr; g_v6639r1_ssr_execute_original=nullptr;
}

bool EnsureSecondaryViewGateCensusHooks() noexcept {
    if(g_secondary_gate_check_hook && g_secondary_stage_apply_hook && g_secondary_shader_mode_hook && g_mode_result_assign_hook &&
       g_v6631_cloud_readiness_hook && g_v6631_cloud_shadow_setup_hook && g_v6631_cloud_eye_setup_hook && g_v6639_shadowmask_execute_hook &&
       g_v6639r1_shadowmap_update_hook && g_v6639r1_shadowmap_execute_hook && g_v6639r1_shadowmap_consume_hook && g_v6639r1_ssr_execute_hook) return true;
    const auto b=WhBase();
    if(!b || !MatchBytes(b+kSecondaryGateCheckRva,kSecondaryGateCheckPrologue,sizeof(kSecondaryGateCheckPrologue)) ||
       !MatchBytes(b+kSecondaryStageApplyRva,kSecondaryStageApplyPrologue,sizeof(kSecondaryStageApplyPrologue)) ||
       !MatchBytes(b+kSecondaryShaderModeRva,kSecondaryShaderModePrologue,sizeof(kSecondaryShaderModePrologue)) ||
       !MatchBytes(b+kModeResultAssignRva,kModeResultAssignPrologue,sizeof(kModeResultAssignPrologue)) ||
       !MatchBytes(b+kV6631CloudReadinessRva,kV6631CloudReadinessPrologue,sizeof(kV6631CloudReadinessPrologue)) ||
       !MatchBytes(b+kV6631CloudShadowSetupRva,kV6631CloudShadowSetupPrologue,sizeof(kV6631CloudShadowSetupPrologue)) ||
       !MatchBytes(b+kV6631CloudEyeSetupRva,kV6631CloudEyeSetupPrologue,sizeof(kV6631CloudEyeSetupPrologue)) ||
       !MatchBytes(b+kV6639ShadowMaskExecuteRva,kV6639ShadowMaskExecutePrologue,sizeof(kV6639ShadowMaskExecutePrologue)) ||
       !MatchBytes(b+kV6639R1ShadowMapUpdateRva,kV6639R1ShadowMapUpdatePrologue,sizeof(kV6639R1ShadowMapUpdatePrologue)) ||
       !MatchBytes(b+kV6639R1ShadowMapExecuteRva,kV6639R1ShadowMapExecutePrologue,sizeof(kV6639R1ShadowMapExecutePrologue)) ||
       !MatchBytes(b+kV6639R1ShadowMapConsumeRva,kV6639R1ShadowMapConsumePrologue,sizeof(kV6639R1ShadowMapConsumePrologue)) ||
       !MatchBytes(b+kV6639R1SsrExecuteRva,kV6639R1SsrExecutePrologue,sizeof(kV6639R1SsrExecutePrologue))) {
        Trace("V66.31-POSTFX-CENSUS REFUSED exact stage/cloud prologue mismatch"); return false;
    }
    DetourModKit::hook::InlineRequest reqA{"kcd2vr_v6565_shadowmap_gate",DetourModKit::hook::Target{DetourModKit::Address{b+kSecondaryGateCheckRva}},DetourModKit::hook::Options{}};
    auto a=DetourModKit::hook::inline_at(std::move(reqA),&SecondaryGateCheckDetour);
    if(!a) return false; g_secondary_gate_check_hook=std::move(*a); g_secondary_gate_check_original=g_secondary_gate_check_hook->original<SecondaryGateCheckFn>();
    DetourModKit::hook::InlineRequest reqC{"kcd2vr_v6565_ssr_apply",DetourModKit::hook::Target{DetourModKit::Address{b+kSecondaryStageApplyRva}},DetourModKit::hook::Options{}};
    auto c=DetourModKit::hook::inline_at(std::move(reqC),&SecondaryStageApplyDetour);
    if(!c){DisableSecondaryViewGateCensusHooks();return false;} g_secondary_stage_apply_hook=std::move(*c); g_secondary_stage_apply_original=g_secondary_stage_apply_hook->original<SecondaryStageApplyFn>();
    DetourModKit::hook::InlineRequest reqD{"kcd2vr_v6565_particle_shader_mode",DetourModKit::hook::Target{DetourModKit::Address{b+kSecondaryShaderModeRva}},DetourModKit::hook::Options{}};
    auto d=DetourModKit::hook::inline_at(std::move(reqD),&SecondaryShaderModeDetour);
    if(!d){DisableSecondaryViewGateCensusHooks();return false;} g_secondary_shader_mode_hook=std::move(*d); g_secondary_shader_mode_original=g_secondary_shader_mode_hook->original<SecondaryShaderModeFn>();
    DetourModKit::hook::InlineRequest reqE{"kcd2vr_v6566_mode_result_assign",DetourModKit::hook::Target{DetourModKit::Address{b+kModeResultAssignRva}},DetourModKit::hook::Options{}};
    auto e=DetourModKit::hook::inline_at(std::move(reqE),&ModeResultAssignDetour);
    if(!e){DisableSecondaryViewGateCensusHooks();return false;} g_mode_result_assign_hook=std::move(*e); g_mode_result_assign_original=g_mode_result_assign_hook->original<ModeResultAssignFn>();

    DetourModKit::hook::InlineRequest reqM{"kcd2vr_v6639_shadowmask_execute",DetourModKit::hook::Target{DetourModKit::Address{b+kV6639ShadowMaskExecuteRva}},DetourModKit::hook::Options{}};
    auto m=DetourModKit::hook::inline_at(std::move(reqM),&V6639ShadowMaskExecuteDetour);
    if(!m){DisableSecondaryViewGateCensusHooks();return false;} g_v6639_shadowmask_execute_hook=std::move(*m); g_v6639_shadowmask_execute_original=g_v6639_shadowmask_execute_hook->original<V6639ShadowMaskExecuteFn>();
    if(!g_v6639_shadowmask_execute_original || !g_v6639_shadowmask_execute_hook->enable()){DisableSecondaryViewGateCensusHooks();return false;}

    DetourModKit::hook::InlineRequest reqSMU{"kcd2vr_v6639r1_shadowmap_update",DetourModKit::hook::Target{DetourModKit::Address{b+kV6639R1ShadowMapUpdateRva}},DetourModKit::hook::Options{}};
    auto smu=DetourModKit::hook::inline_at(std::move(reqSMU),&V6639R1ShadowMapUpdateDetour);
    if(!smu){DisableSecondaryViewGateCensusHooks();return false;} g_v6639r1_shadowmap_update_hook=std::move(*smu); g_v6639r1_shadowmap_update_original=g_v6639r1_shadowmap_update_hook->original<V6639R1ShadowMapStageFn>();
    if(!g_v6639r1_shadowmap_update_original || !g_v6639r1_shadowmap_update_hook->enable()){DisableSecondaryViewGateCensusHooks();return false;}

    DetourModKit::hook::InlineRequest reqSME{"kcd2vr_v6639r1_shadowmap_execute",DetourModKit::hook::Target{DetourModKit::Address{b+kV6639R1ShadowMapExecuteRva}},DetourModKit::hook::Options{}};
    auto sme=DetourModKit::hook::inline_at(std::move(reqSME),&V6639R1ShadowMapExecuteDetour);
    if(!sme){DisableSecondaryViewGateCensusHooks();return false;} g_v6639r1_shadowmap_execute_hook=std::move(*sme); g_v6639r1_shadowmap_execute_original=g_v6639r1_shadowmap_execute_hook->original<V6639R1ShadowMapStageFn>();
    if(!g_v6639r1_shadowmap_execute_original || !g_v6639r1_shadowmap_execute_hook->enable()){DisableSecondaryViewGateCensusHooks();return false;}

    DetourModKit::hook::InlineRequest reqSMC{"kcd2vr_v6639r1_shadowmap_consume",DetourModKit::hook::Target{DetourModKit::Address{b+kV6639R1ShadowMapConsumeRva}},DetourModKit::hook::Options{}};
    auto smc=DetourModKit::hook::inline_at(std::move(reqSMC),&V6639R1ShadowMapConsumeDetour);
    if(!smc){DisableSecondaryViewGateCensusHooks();return false;} g_v6639r1_shadowmap_consume_hook=std::move(*smc); g_v6639r1_shadowmap_consume_original=g_v6639r1_shadowmap_consume_hook->original<V6639R1ShadowMapConsumeFn>();
    if(!g_v6639r1_shadowmap_consume_original || !g_v6639r1_shadowmap_consume_hook->enable()){DisableSecondaryViewGateCensusHooks();return false;}

    DetourModKit::hook::InlineRequest reqSSR{"kcd2vr_v6639r1_ssr_execute",DetourModKit::hook::Target{DetourModKit::Address{b+kV6639R1SsrExecuteRva}},DetourModKit::hook::Options{}};
    auto ssr=DetourModKit::hook::inline_at(std::move(reqSSR),&V6639R1SsrExecuteDetour);
    if(!ssr){DisableSecondaryViewGateCensusHooks();return false;} g_v6639r1_ssr_execute_hook=std::move(*ssr); g_v6639r1_ssr_execute_original=g_v6639r1_ssr_execute_hook->original<V6639R1SsrExecuteFn>();
    if(!g_v6639r1_ssr_execute_original || !g_v6639r1_ssr_execute_hook->enable()){DisableSecondaryViewGateCensusHooks();return false;}

    DetourModKit::hook::InlineRequest reqR{"kcd2vr_v6631_cloud_readiness",DetourModKit::hook::Target{DetourModKit::Address{b+kV6631CloudReadinessRva}},DetourModKit::hook::Options{}};
    auto r=DetourModKit::hook::inline_at(std::move(reqR),&V6631CloudReadinessDetour);
    if(!r){DisableSecondaryViewGateCensusHooks();return false;} g_v6631_cloud_readiness_hook=std::move(*r); g_v6631_cloud_readiness_original=g_v6631_cloud_readiness_hook->original<V6631CloudReadinessFn>();
    DetourModKit::hook::InlineRequest reqS{"kcd2vr_v6631_cloud_shadow_state",DetourModKit::hook::Target{DetourModKit::Address{b+kV6631CloudShadowSetupRva}},DetourModKit::hook::Options{}};
    auto sh=DetourModKit::hook::inline_at(std::move(reqS),&V6631CloudShadowSetupDetour);
    if(!sh){DisableSecondaryViewGateCensusHooks();return false;} g_v6631_cloud_shadow_setup_hook=std::move(*sh); g_v6631_cloud_shadow_setup_original=g_v6631_cloud_shadow_setup_hook->original<V6631CloudSetupFn>();
    DetourModKit::hook::InlineRequest reqI{"kcd2vr_v6631_cloud_eye_state",DetourModKit::hook::Target{DetourModKit::Address{b+kV6631CloudEyeSetupRva}},DetourModKit::hook::Options{}};
    auto i=DetourModKit::hook::inline_at(std::move(reqI),&V6631CloudEyeSetupDetour);
    if(!i){DisableSecondaryViewGateCensusHooks();return false;} g_v6631_cloud_eye_setup_hook=std::move(*i); g_v6631_cloud_eye_setup_original=g_v6631_cloud_eye_setup_hook->original<V6631CloudSetupFn>();

    if(!g_secondary_gate_check_original||!g_secondary_stage_apply_original||!g_secondary_shader_mode_original||!g_mode_result_assign_original||
       !g_v6631_cloud_readiness_original||!g_v6631_cloud_shadow_setup_original||!g_v6631_cloud_eye_setup_original||!g_v6639_shadowmask_execute_original||
       !g_v6639r1_shadowmap_update_original||!g_v6639r1_shadowmap_execute_original||!g_v6639r1_shadowmap_consume_original||!g_v6639r1_ssr_execute_original||
       !g_secondary_gate_check_hook->enable()||!g_secondary_stage_apply_hook->enable()||!g_secondary_shader_mode_hook->enable()||!g_mode_result_assign_hook->enable()||
       !g_v6631_cloud_readiness_hook->enable()||!g_v6631_cloud_shadow_setup_hook->enable()||!g_v6631_cloud_eye_setup_hook->enable()) { DisableSecondaryViewGateCensusHooks(); return false; }
    Trace("V66.39R1-PAIR-WIDE-PROOF hooks installed: exact ShadowMap Update/Execute/post-refresh-consumer + ShadowMask Execute + exact SSR Execute + cloud census; mutations proof-gated");
    return true;
}

struct V6573ViewportTuple { std::size_t off{}; bool is_float{}; };

static bool FindV6573ViewportTuple(void* records,std::uint32_t stride,std::uint32_t count,V6573ViewportTuple& out,std::uint64_t width,std::uint32_t height) noexcept {
    if(!records || stride!=0x470u || count!=2u || width<640 || height<480 || width>16384 || height>16384) return false;
    const auto* a=reinterpret_cast<const unsigned char*>(records);
    const auto* b=a+stride;
    unsigned matches=0; V6573ViewportTuple found{};
    auto same_i=[&](std::size_t off)->bool {
        const auto* ai=reinterpret_cast<const std::uint32_t*>(a+off); const auto* bi=reinterpret_cast<const std::uint32_t*>(b+off);
        return ai[0]==0u && ai[1]==0u && ai[2]==static_cast<std::uint32_t>(width) && ai[3]==height &&
               bi[0]==0u && bi[1]==0u && bi[2]==static_cast<std::uint32_t>(width) && bi[3]==height;
    };
    auto same_f=[&](std::size_t off)->bool {
        const auto* af=reinterpret_cast<const float*>(a+off); const auto* bf=reinterpret_cast<const float*>(b+off);
        const float w=static_cast<float>(width), h=static_cast<float>(height);
        auto eq=[](float x,float y){ return std::isfinite(x)&&std::fabs(x-y)<0.01f; };
        return eq(af[0],0.f)&&eq(af[1],0.f)&&eq(af[2],w)&&eq(af[3],h)&&
               eq(bf[0],0.f)&&eq(bf[1],0.f)&&eq(bf[2],w)&&eq(bf[3],h);
    };
    __try {
        for(std::size_t off=0; off+16<=stride; off+=4) {
            if(same_i(off)){ ++matches; found={off,false}; }
            if(same_f(off)){ ++matches; found={off,true}; }
        }
    } __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
    if(matches!=1u) return false; out=found; return true;
}


static void V6575AccumulateDiff(const char* tag,const unsigned char* a,const unsigned char* b,std::size_t bytes,
    std::atomic<std::uint32_t>& samples,std::atomic<std::uint16_t>* hits,std::size_t hitCount) noexcept {
    if(!a || !b || bytes<4 || !hits) return;
    const auto sample=samples.fetch_add(1,std::memory_order_relaxed)+1u;
    std::uint32_t diffs=0,tailDiffs=0;
    __try {
        const auto* qa=reinterpret_cast<const std::uint32_t*>(a);
        const auto* qb=reinterpret_cast<const std::uint32_t*>(b);
        const std::size_t n=std::min<std::size_t>(bytes/4,hitCount);
        for(std::size_t i=0;i<n;++i) if(qa[i]!=qb[i]) {
            ++diffs; if(i*4>=0x300) ++tailDiffs;
            const auto old=hits[i].load(std::memory_order_relaxed);
            if(old!=0xFFFFu) hits[i].store(static_cast<std::uint16_t>(old+1u),std::memory_order_relaxed);
        }
        if(sample==1u || sample==8u || sample==32u) {
            Tracef("V65.75-DIFF-SUMMARY tag=%s sample=%u diffs=%u tailGE300=%u bytes=0x%zX",tag,unsigned(sample),unsigned(diffs),unsigned(tailDiffs),bytes);
            unsigned emitted=0;
            for(std::size_t i=0;i<n && emitted<64u;++i) {
                const auto h=hits[i].load(std::memory_order_relaxed);
                const bool stable=(sample<32u)?(h==sample):(h>=28u);
                if(!stable || qa[i]==qb[i]) continue;
                float fa=0.f,fb=0.f; std::memcpy(&fa,&qa[i],4); std::memcpy(&fb,&qb[i],4);
                Tracef("V65.75-DIFF tag=%s sample=%u off=0x%03zX hits=%u/%u A=%08X B=%08X fA=%.7g fB=%.7g xor=%08X",
                    tag,unsigned(sample),i*4,unsigned(h),unsigned(sample),qa[i],qb[i],double(fa),double(fb),qa[i]^qb[i]);
                ++emitted;
            }
            Tracef("V65.75-DIFF-END tag=%s sample=%u emitted=%u stableRule=%s",tag,unsigned(sample),emitted,sample<32u?"all-so-far":"28-of-32");
        }
    } __except(EXCEPTION_EXECUTE_HANDLER) { Tracef("V65.75-DIFF-FAULT tag=%s sample=%u",tag,unsigned(sample)); }
}

static std::uint64_t V6594HashBuiltView(void* crv, unsigned index) noexcept {
    if(!crv || index>1) return 0;
    const auto p=reinterpret_cast<const unsigned char*>(crv)+kBuiltOutput0+index*kBuiltOutputStride;
    return SafeFnv1a64(p,kBuiltOutputStride);
}

static void V6594TraceTxn(const char* where, std::uint64_t call, void* owner, void* crv) noexcept {
    if(V6601EffectiveDiag()<=0) return;
    const int stage=V6601EffectiveBridgeStage();
    if(stage<1 || stage>3 || !crv) return;
    const auto n=call;
    if(n>32 && (n%120ull)!=0ull) return;
    std::uintptr_t pipeline=0,aux=0; std::uint32_t count=0,selector=0;
    __try {
        auto u=reinterpret_cast<std::uintptr_t>(crv);
        pipeline=*reinterpret_cast<std::uintptr_t*>(u+0x8D60);
        aux=*reinterpret_cast<std::uintptr_t*>(u+0x8D78);
        count=*reinterpret_cast<std::uint32_t*>(u+kBuiltRecordCount);
        selector=*reinterpret_cast<std::uint32_t*>(u+0x8EE0);
    } __except(EXCEPTION_EXECUTE_HANDLER) { pipeline=aux=0; count=selector=0xFFFFFFFFu; }
    const auto h0=V6594HashBuiltView(crv,0);
    const auto h1=count>1?V6594HashBuiltView(crv,1):0;
    Tracef("V65.95B-TXN %s call=%llu diag=%d stage=%s owner=%p crv=%p pipeline=%p aux=%p builtCount=%u selector=%u rec0=%016llX rec1=%016llX tid=%lu",
        where,(unsigned long long)n,V6601EffectiveDiag(),stage==1?"LEFT":(stage==2?"RIGHT":"COUNT2"),
        owner,crv,reinterpret_cast<void*>(pipeline),reinterpret_cast<void*>(aux),unsigned(count),unsigned(selector),
        (unsigned long long)h0,(unsigned long long)h1,(unsigned long)GetCurrentThreadId());
}

#include "native_stereo_single_run_v6601.inc"

// Keep all added writes inside one validated outer command on one render thread.
// Nested/unqualified commands use the original trampoline with the suite policy disabled.
void __fastcall V6594TxnSetViewDetour(void* owner,void* crv) noexcept {
    const auto n=g_v6594_txn_setview_calls.fetch_add(1,std::memory_order_relaxed)+1;
    if(!g_v6595_replay_guard && V6601EffectiveDiag()==3 &&
       V6601EffectiveBridgeStage()==3 && owner && crv) {
        g_v6595_last_txn_owner=owner;
        g_v6595_last_txn_crv=crv;
    }
    // V66.27: capture only the normal retail transaction. The replay itself runs
    // through this same detour, so a TLS guard prevents it from overwriting the
    // owner snapshot with its own call. No renderer state is changed here.
    if(g_v6627_right_prep_enabled.load(std::memory_order_acquire) &&
       !g_v6627_right_prep_replay && owner && crv) {
        g_v6627_setview_owner.store(reinterpret_cast<std::uintptr_t>(owner),std::memory_order_release);
        g_v6627_setview_crv.store(reinterpret_cast<std::uintptr_t>(crv),std::memory_order_release);
        g_v6627_setview_tid.store(GetCurrentThreadId(),std::memory_order_release);
        g_v6627_setview_captures.fetch_add(1,std::memory_order_relaxed);
    }
    V6594TraceTxn("SETVIEW-IN",n,owner,crv);
    if(g_txn_set_view_original) g_txn_set_view_original(owner,crv);
    V6594TraceTxn("SETVIEW-OUT",n,owner,crv);
}

bool __fastcall MultiviewPacketDispatchDetour(void* owner,void* records,std::uint32_t stride,void* arg4,std::uint32_t count) noexcept {
    if(!g_multiview_packet_dispatch_original) return false;

    const int stage=V6601EffectiveBridgeStage();
    const auto base=WhBase();
    const auto ret=reinterpret_cast<std::uintptr_t>(_ReturnAddress());
    const auto retRva=(base && ret>=base)?ret-base:ret;

    // V65.83 THREE-CHANNEL ORACLE. The hook is the already-existing exact
    // +0x42DA44 observer; caller return address identifies which persistent
    // state channel dispatched the packet, so no new engine hooks are needed.
    if(stage>=1 && stage<=3 && records && count>0) {
        const char* channel=nullptr;
        std::atomic<std::uint32_t>* counter=nullptr;
        if(retRva==kChannel108DispatchReturnRva){ channel="108"; counter=&g_v6583_ch108_calls; }
        else if(retRva==kChannel110DispatchReturnRva){ channel="110"; counter=&g_v6583_ch110_calls; }
        else if(retRva==kChannel118DispatchReturnRva){ channel="118"; counter=&g_v6583_ch118_calls; }
        if(channel && counter) {
            const auto n=counter->fetch_add(1,std::memory_order_relaxed)+1u;
            // Generic dispatcher semantics for these exact callsites are count=1
            // and R8D carries the byte size (0x30, 0x60, or variable for +108).
            const std::uint64_t bytes64=static_cast<std::uint64_t>(stride)*static_cast<std::uint64_t>(count);
            const auto* b=reinterpret_cast<const unsigned char*>(records);
            const auto fullHash=SafeFnv1a64(b,bytes64);
            const auto headBytes=std::min<std::uint64_t>(bytes64,64ull);
            const auto tailBytes=std::min<std::uint64_t>(bytes64,64ull);
            const auto headHash=SafeFnv1a64(b,headBytes);
            const auto tailHash=SafeFnv1a64(b+(bytes64-tailBytes),tailBytes);
            std::uint64_t q[6]{};
            SafeCopyU64s(b,bytes64,q,6u);

            // V65.84: +108 is the only dynamic multi-record channel. Preserve
            // distinct 0x300 snapshots and fingerprint its 24 x 0x20 records.
            // F9 ordering (LEFT -> RIGHT -> COUNT2) lets COUNT2 classify each
            // entry against already-observed isolated-eye baselines.
            if(retRva==kChannel108DispatchReturnRva && bytes64==0x300ull && stage>=1 && stage<=3 && fullHash) {
                std::lock_guard<std::mutex> lk(g_v6584_108_mutex);
                const unsigned si=static_cast<unsigned>(stage);
                bool seen=false;
                for(unsigned j=0;j<g_v6584_108_snapshot_count[si];++j) if(g_v6584_108_full_hashes[si][j]==fullHash){ seen=true; break; }
                if(!seen && g_v6584_108_snapshot_count[si]<4u) {
                    const unsigned snap=g_v6584_108_snapshot_count[si]++;
                    g_v6584_108_full_hashes[si][snap]=fullHash;
                    for(unsigned i=0;i<24u;++i) g_v6584_108_entry_hashes[si][snap][i]=SafeFnv1a64(b+i*0x20ull,0x20ull);
                    Tracef("V65.84-108-SNAPSHOT stage=%s snap=%u fullHash=%016llX entries=24 records=%p",
                        stage==1?"LEFT":(stage==2?"RIGHT":"COUNT2"),snap,
                        static_cast<unsigned long long>(fullHash),records);
                    for(unsigned i=0;i<24u;++i) {
                        std::uint64_t eq[4]{};
                        SafeCopyU64s(b+i*0x20ull,0x20ull,eq,4u);
                        const auto eh=g_v6584_108_entry_hashes[si][snap][i];
                        if(stage==1) {
                            Tracef("V65.84-108-ENTRY stage=LEFT snap=%u idx=%02u hash=%016llX q={%016llX,%016llX,%016llX,%016llX}",snap,i,
                                static_cast<unsigned long long>(eh),static_cast<unsigned long long>(eq[0]),static_cast<unsigned long long>(eq[1]),static_cast<unsigned long long>(eq[2]),static_cast<unsigned long long>(eq[3]));
                        } else {
                            bool matchL=false,matchR=false;
                            for(unsigned j=0;j<g_v6584_108_snapshot_count[1];++j) if(g_v6584_108_entry_hashes[1][j][i]==eh){matchL=true;break;}
                            for(unsigned j=0;j<g_v6584_108_snapshot_count[2];++j) if(g_v6584_108_entry_hashes[2][j][i]==eh){matchR=true;break;}
                            const char* cls=stage==2 ? (matchL?"BOTH":"RIGHT-DIFF") : (matchL?(matchR?"BOTH":"LEFT-LIKE"):(matchR?"RIGHT-LIKE":"UNIQUE"));
                            Tracef("V65.84-108-ENTRY stage=%s snap=%u idx=%02u class=%s hash=%016llX q={%016llX,%016llX,%016llX,%016llX}",
                                stage==2?"RIGHT":"COUNT2",snap,i,cls,static_cast<unsigned long long>(eh),
                                static_cast<unsigned long long>(eq[0]),static_cast<unsigned long long>(eq[1]),static_cast<unsigned long long>(eq[2]),static_cast<unsigned long long>(eq[3]));
                        }
                    }
                }
            }
            if(n<=8u) {
                Tracef("V65.83-CHANNEL stage=%s channel=+%s call=%u owner=%p records=%p bytes=0x%llX count=%u arg4=%p fullHash=%016llX head64=%016llX tail64=%016llX entries20=%llu q0-5={%016llX,%016llX,%016llX,%016llX,%016llX,%016llX} callerRva=0x%llX",
                    stage==1?"LEFT":(stage==2?"RIGHT":"COUNT2"),channel,unsigned(n),owner,records,
                    static_cast<unsigned long long>(bytes64),unsigned(count),arg4,
                    static_cast<unsigned long long>(fullHash),static_cast<unsigned long long>(headHash),static_cast<unsigned long long>(tailHash),
                    static_cast<unsigned long long>((std::strcmp(channel,"108")==0)?(bytes64/0x20ull):0ull),
                    static_cast<unsigned long long>(q[0]),static_cast<unsigned long long>(q[1]),static_cast<unsigned long long>(q[2]),
                    static_cast<unsigned long long>(q[3]),static_cast<unsigned long long>(q[4]),static_cast<unsigned long long>(q[5]),
                    static_cast<unsigned long long>(retRva));
            }
        }
    }
    if(stage==3 && retRva==kModeQuartetDispatchReturnRva) {
        const auto n=g_v6566_quartet_dispatch_calls.fetch_add(1,std::memory_order_relaxed)+1;
        if(n<=12) Tracef("V65.69-MODE-QUARTET-DISPATCH call=%u owner=%p records=%p stride=0x%X arg4=%p count=%u callerRva=0x%llX",
            unsigned(n),owner,records,unsigned(stride),arg4,unsigned(count),static_cast<unsigned long long>(retRva));
        const auto sn=g_v6567_quartet_snapshots.fetch_add(1,std::memory_order_relaxed)+1;
        if(sn<=4 && records && stride==0x60u && count==1u) {
            std::uint64_t q[12]{};
            for(unsigned i=0;i<12;++i) q[i]=SafeReadU64(reinterpret_cast<const unsigned char*>(records)+i*8ull);
            for(unsigned i=0;i<12;++i) TracePointerProvenance("QUARTET","PACKET",0,reinterpret_cast<std::uintptr_t>(records),i,static_cast<std::uintptr_t>(q[i]));
            Tracef("V65.69-QUARTET-SNAPSHOT n=%u records=%p qwords={%016llX,%016llX,%016llX,%016llX,%016llX,%016llX,%016llX,%016llX,%016llX,%016llX,%016llX,%016llX}",
                unsigned(sn),records,
                static_cast<unsigned long long>(q[0]),static_cast<unsigned long long>(q[1]),static_cast<unsigned long long>(q[2]),static_cast<unsigned long long>(q[3]),static_cast<unsigned long long>(q[4]),static_cast<unsigned long long>(q[5]),
                static_cast<unsigned long long>(q[6]),static_cast<unsigned long long>(q[7]),static_cast<unsigned long long>(q[8]),static_cast<unsigned long long>(q[9]),static_cast<unsigned long long>(q[10]),static_cast<unsigned long long>(q[11]));
        }
    }
    const std::uint32_t expectedCount=stage==3?2u:((stage==1||stage==2)?1u:0u);
    const bool exactCaller=base && ret==base+kNViewDispatchReturnRva;
    const bool exactNViewPacket=stage>=1 && stage<=3 && exactCaller && stride==0x470u && count==expectedCount && owner && records &&
        g_multiview_pipeline_seen.load(std::memory_order_acquire) && g_multiview_build_ok.load(std::memory_order_acquire);

    if(exactNViewPacket && stage==3 && count==2u && stride==0x470u && Count2SbsStage()>0) {
        const auto* r0=reinterpret_cast<const unsigned char*>(records);
        V6575AccumulateDiff("XFORM470",r0,r0+stride,0x470,g_v6575_xform_samples,g_v6575_xform_diff_hits.data(),g_v6575_xform_diff_hits.size());
    }
    if(exactNViewPacket && Count2SbsStage()>0) {
        const auto n=g_v6573_sbs_packets.fetch_add(1,std::memory_order_relaxed)+1;
        std::uint64_t ps=0,pt=0,pw=0,pc=0; std::uint32_t ph=0;
        const bool haveSize=DeviceHook::GetLatestPresentIdentity(&ps,&pt,&pw,&ph,&pc);
        // V65.74: sample the original 0x400 built-view records at the exact downstream
        // N-view packet boundary.  This avoids depending on F9 having been armed before
        // the first CStandardGraphicsPipeline notification of the F8 activation.
        bool sourceExtentSane=false;
        const auto crv=g_multiview_crv.load(std::memory_order_acquire);
        if(crv) {
            const auto* s0=reinterpret_cast<const unsigned char*>(crv+kBuiltOutput0);
            float e0[4]{}; std::uint32_t q0[4]{};
            const bool ok0=SafeCopyBytes(e0,s0+0x3B0,sizeof(e0)) && SafeCopyBytes(q0,s0+0x3B0,sizeof(q0));
            auto eq=[](float x,float y){ return std::isfinite(x) && std::fabs(x-y)<0.01f; };
            sourceExtentSane=ok0 && haveSize && eq(e0[0],static_cast<float>(pw)) && eq(e0[1],static_cast<float>(ph));
            if(stage==3 && count>1) {
                const auto* s1=s0+kBuiltOutputStride; float e1[4]{}; std::uint32_t q1[4]{};
                const bool ok1=SafeCopyBytes(e1,s1+0x3B0,sizeof(e1)) && SafeCopyBytes(q1,s1+0x3B0,sizeof(q1));
                sourceExtentSane=sourceExtentSane && ok1 && eq(e1[0],static_cast<float>(pw)) && eq(e1[1],static_cast<float>(ph));
                if(n<=12) Tracef("V65.74-EXTENT-SOURCE stage=3 present=%llux%u rec0={%.6f,%.6f,%.6f,%.6f} raw0={%08X,%08X,%08X,%08X} rec1={%.6f,%.6f,%.6f,%.6f} raw1={%08X,%08X,%08X,%08X} sane=%d",
                    static_cast<unsigned long long>(pw),unsigned(ph),double(e0[0]),double(e0[1]),double(e0[2]),double(e0[3]),q0[0],q0[1],q0[2],q0[3],double(e1[0]),double(e1[1]),double(e1[2]),double(e1[3]),q1[0],q1[1],q1[2],q1[3],sourceExtentSane?1:0);
            } else if(n<=12) {
                Tracef("V65.74-EXTENT-SOURCE stage=%d present=%llux%u rec0={%.6f,%.6f,%.6f,%.6f} raw0={%08X,%08X,%08X,%08X} sane=%d",
                    stage,static_cast<unsigned long long>(pw),unsigned(ph),double(e0[0]),double(e0[1]),double(e0[2]),double(e0[3]),q0[0],q0[1],q0[2],q0[3],sourceExtentSane?1:0);
            }
        }
        g_v6574_extent_sane.store(sourceExtentSane,std::memory_order_release);
        g_v6574_extent_logs.fetch_add(1,std::memory_order_relaxed);
        if((stage==1 || stage==2 || stage==3) && n<=12) {
            const auto* r0=reinterpret_cast<const unsigned char*>(records);
            float f0[4]{},e0[4]{}; std::uint32_t q0[4]{};
            const bool ok0=SafeCopyBytes(f0,r0+0x60,sizeof(f0)) && SafeCopyBytes(e0,r0+0x70,sizeof(e0)) && SafeCopyBytes(q0,r0+0x70,sizeof(q0));
            if(ok0) Tracef("V65.74-EXTENT-XFORM n=%llu stage=%d rec=0 out60={%.6f,%.6f,%.6f,%.6f} out70={%.6f,%.6f,%.6f,%.6f} raw70={%08X,%08X,%08X,%08X}",
                static_cast<unsigned long long>(n),stage,double(f0[0]),double(f0[1]),double(f0[2]),double(f0[3]),double(e0[0]),double(e0[1]),double(e0[2]),double(e0[3]),q0[0],q0[1],q0[2],q0[3]);
            if(stage==3 && count>1) {
                const auto* r1=r0+stride; float f1[4]{},e1[4]{}; std::uint32_t q1[4]{};
                const bool ok1=SafeCopyBytes(f1,r1+0x60,sizeof(f1)) && SafeCopyBytes(e1,r1+0x70,sizeof(e1)) && SafeCopyBytes(q1,r1+0x70,sizeof(q1));
                if(ok1) Tracef("V65.74-EXTENT-XFORM n=%llu stage=3 rec=1 out60={%.6f,%.6f,%.6f,%.6f} out70={%.6f,%.6f,%.6f,%.6f} raw70={%08X,%08X,%08X,%08X}",
                    static_cast<unsigned long long>(n),double(f1[0]),double(f1[1]),double(f1[2]),double(f1[3]),double(e1[0]),double(e1[1]),double(e1[2]),double(e1[3]),q1[0],q1[1],q1[2],q1[3]);
            }
        }
        if(stage==3) {
            V6573ViewportTuple tuple{};
            const bool unique=haveSize && FindV6573ViewportTuple(records,stride,count,tuple,pw,ph);
            const bool extentSane=g_v6574_extent_sane.load(std::memory_order_acquire);
            if(n<=12 || !unique || !extentSane) Tracef("V65.74-SBS-PACKET n=%llu size=%llux%u haveSize=%d extentSane=%d uniqueViewport=%d off=0x%zX kind=%s mode=%d",
                static_cast<unsigned long long>(n),static_cast<unsigned long long>(pw),unsigned(ph),haveSize?1:0,extentSane?1:0,unique?1:0,tuple.off,tuple.is_float?"float":"uint",Count2SbsStage());
            if(Count2SbsStage()==2) {
                const auto r=g_v6573_sbs_split_refused.fetch_add(1,std::memory_order_relaxed)+1;
                if(r<=12) Tracef("V65.75-SBS-WRITE DISABLED n=%llu reason=NO-PROVEN-RECORD-VIEWPORT XR-half-sampler-only",static_cast<unsigned long long>(n));
            }
        }
    }

    bool claimed_now=false;
    if(exactNViewPacket) {
        bool expected=false;
        if(g_v6560_packet_claimed.compare_exchange_strong(expected,true,std::memory_order_acq_rel)) {
            g_v6560_packet_owner.store(reinterpret_cast<std::uintptr_t>(owner),std::memory_order_release);
            g_v6560_packet_records.store(reinterpret_cast<std::uintptr_t>(records),std::memory_order_release);
            claimed_now=true;
            const auto h0=SafeFnv1a64(reinterpret_cast<const unsigned char*>(records),0x470ull);
            const auto h1=count>1?SafeFnv1a64(reinterpret_cast<const unsigned char*>(records)+0x470,0x470ull):0ull;
            Tracef("V65.63-NVIEW-PACKET CLAIM stage=%d eye=%s owner=%p records=%p stride=0x%X count=%u callerRva=0x%llX exactCaller=1 rec0Hash=%016llX rec1Hash=%016llX",
                stage,stage==1?"LEFT":(stage==2?"RIGHT":"COUNT2"),owner,records,unsigned(stride),unsigned(count),
                static_cast<unsigned long long>(retRva),static_cast<unsigned long long>(h0),static_cast<unsigned long long>(h1));
            if(!g_v6601_owned.load(std::memory_order_acquire)) {
                if(stage==3 && count>=2) DeviceHook::NotifyV6593Count2Packet(count);
                DeviceHook::ArmStereoProducerProbeThroughNextPresent(stage);
            }
        }
    }

    // V65.97: retain an exact copy of COUNT2 block1 for explicit replay-time upload.
    // The source buffer passed here may be temporary, so never keep only its pointer.
    if(exactNViewPacket && stage==3 && stride==0x470u && count==2u && owner && records) {
        const bool copied=SafeCopyBytes(g_v6597_block1_copy,reinterpret_cast<const unsigned char*>(records)+0x470u,0x470u);
        g_v6597_block1_owner=copied?owner:nullptr;
        g_v6597_block1_arg4=copied?arg4:nullptr;
        g_v6597_block1_ready=copied;
        if(g_v6601_command_active) {
            const bool matched=copied && g_v6599_scope.command && g_v6599_scope.crv==g_v6601_command_crv;
            g_v6601_snapshot_serial=matched?g_v6599_scope.serial:0;
            g_v6601_snapshot_crv=matched?g_v6599_scope.crv:nullptr;
        }
        const auto snap=g_v6597_block1_snapshots.fetch_add(1,std::memory_order_relaxed)+1ull;
        if(copied && (snap<=24ull || (snap%120ull)==0ull))
            Tracef("V65.97-BLOCK1 SNAPSHOT call=%llu owner=%p arg4=%p hash=%016llX tid=%lu",
                (unsigned long long)snap,owner,arg4,(unsigned long long)SafeFnv1a64(g_v6597_block1_copy,0x470ull),(unsigned long)GetCurrentThreadId());
    }

    const bool scope6564=claimed_now;
    if(scope6564){ g_v6564_backend_scope=true; g_v6564_backend_stage=stage; g_v6564_exec_calls=0; g_v6564_range_calls=0; }
    void* dispatchRecords=records;
    std::uint32_t dispatchCount=count;
    const int v6595Mode=V6601EffectiveDiag();
    const bool forceBlock1 = exactNViewPacket && stage==3 && stride==0x470u && count==2u &&
        (v6595Mode==2 || ((v6595Mode==3 || v6595Mode==4) && g_v6595_replay_eye1));
    if(forceBlock1) {
        dispatchRecords=reinterpret_cast<unsigned char*>(records)+0x470u;
        dispatchCount=1u;
        const auto fn=g_v6594_block1_forces.fetch_add(1,std::memory_order_relaxed)+1;
        if(fn<=24 || (fn%120ull)==0ull) Tracef("V65.97-BLOCK1 FORCE n=%llu owner=%p original=%p forced=%p stride=0x470 originalBlocks=2 forcedBlocks=1 rec1Hash=%016llX callerRva=0x%llX",
            (unsigned long long)fn,owner,records,dispatchRecords,(unsigned long long)SafeFnv1a64(reinterpret_cast<const unsigned char*>(dispatchRecords),0x470ull),(unsigned long long)retRva);
    }
    const bool result=g_multiview_packet_dispatch_original(owner,dispatchRecords,stride,arg4,dispatchCount);
    if(g_v6601_command_active && !result && exactNViewPacket) {
        g_v6597_block1_ready=false;g_v6601_snapshot_serial=0;g_v6601_snapshot_crv=nullptr;
    }
    if(exactNViewPacket && stride==0x470u && dispatchCount>0u) V6599ObserveUpload(owner,dispatchRecords,stride,count,dispatchCount,forceBlock1,result);
    if(scope6564){ Tracef("V65.64-BACKEND-SUMMARY stage=%d eye=%s execCalls=%u rangeCalls=%u expectedRecords=%u",stage,stage==1?"LEFT":(stage==2?"RIGHT":"COUNT2"),unsigned(g_v6564_exec_calls),unsigned(g_v6564_range_calls),unsigned(count)); g_v6564_backend_scope=false; g_v6564_backend_stage=0; }
    if(claimed_now) {
        std::uintptr_t packet=SafeReadPtr(owner),backend=0,vtable=0,slot68=0;
        if(packet && packet!=~std::uintptr_t{0}) backend=SafeReadPtr(reinterpret_cast<const void*>(packet+0x8));
        if(backend) vtable=SafeReadPtr(reinterpret_cast<const void*>(backend));
        if(vtable) slot68=SafeReadPtr(reinterpret_cast<const void*>(vtable+0x68));
        const auto inImage=[&](std::uintptr_t p){ return base && p>=base && p<base+0x5B2D000ull; };
        g_v6560_packet_object.store(packet,std::memory_order_release);
        g_v6560_packet_backend.store(backend,std::memory_order_release);
        Tracef("V65.63-NVIEW-PACKET RETURN stage=%d eye=%s result=%d packet=%p backend=%p vtable=%p slot68=%p slot68Rva=%s0x%llX expectedSlot68=0x%llX",
            stage,stage==1?"LEFT":(stage==2?"RIGHT":"COUNT2"),result?1:0,reinterpret_cast<void*>(packet),reinterpret_cast<void*>(backend),reinterpret_cast<void*>(vtable),reinterpret_cast<void*>(slot68),
            inImage(slot68)?"":"OUTSIDE-IMAGE/",static_cast<unsigned long long>(inImage(slot68)?slot68-base:slot68),static_cast<unsigned long long>(kBackendSlot68Rva));
    }
    return result;
}

void DisableV6598GBufferHook() noexcept {
    if(g_scene_gbuffer_update_hook){ (void)g_scene_gbuffer_update_hook->disable(); g_scene_gbuffer_update_hook.reset(); }
    g_scene_gbuffer_update_original=nullptr;
}


bool EnsureV6627RightPrepHook() noexcept {
    if(g_txn_set_view_hook && g_txn_set_view_original) return true;
    const auto b=WhBase();
    if(!b || !MatchBytes(b+kTxnSetViewRva,kTxnSetViewPrologue,sizeof(kTxnSetViewPrologue))) {
        Trace("V66.27-RIGHT-PREP REFUSED exact +0x7C2D40 prologue mismatch");
        return false;
    }
    DetourModKit::hook::InlineRequest req{"kcd2vr_v6627_right_prep_setview",DetourModKit::hook::Target{DetourModKit::Address{b+kTxnSetViewRva}},DetourModKit::hook::Options{}};
    auto h=DetourModKit::hook::inline_at(std::move(req),&V6594TxnSetViewDetour);
    if(!h) return false;
    g_txn_set_view_hook=std::move(*h);
    g_txn_set_view_original=g_txn_set_view_hook->original<TxnViewFn>();
    if(!g_txn_set_view_original || !g_txn_set_view_hook->enable()) {
        if(g_txn_set_view_hook){ (void)g_txn_set_view_hook->disable(); g_txn_set_view_hook.reset(); }
        g_txn_set_view_original=nullptr;
        return false;
    }
    Trace("V66.27-RIGHT-PREP +0x7C2D40 owner-capture hook installed policy=LIGHTWEIGHT-NORMAL-FRAME-CAPTURE");
    return true;
}

void DisableV6594TransactionHooks() noexcept {
    DisableV6599DrawHooks();
    DisableV6598GBufferHook();
    if(g_txn_command_hook){ (void)g_txn_command_hook->disable(); g_txn_command_hook.reset(); }
    if(g_txn_set_view_hook){ (void)g_txn_set_view_hook->disable(); g_txn_set_view_hook.reset(); }
    if(g_txn_pipeline_hook){ (void)g_txn_pipeline_hook->disable(); g_txn_pipeline_hook.reset(); }
    if(g_txn_finalize_hook){ (void)g_txn_finalize_hook->disable(); g_txn_finalize_hook.reset(); }
    g_txn_command_original=nullptr; g_txn_set_view_original=nullptr; g_txn_pipeline_original=nullptr; g_txn_finalize_original=nullptr;
}


void DisableMultiviewPacketDispatchHook() noexcept {
    DisableSecondaryViewGateCensusHooks();
    DisableBackendExecutionHooks();
    if(g_multiview_packet_dispatch_hook){ (void)g_multiview_packet_dispatch_hook->disable(); g_multiview_packet_dispatch_hook.reset(); }
    g_multiview_packet_dispatch_original=nullptr;
}

bool EnsureMultiviewPacketDispatchHook() noexcept {
    if(g_multiview_packet_dispatch_original) return true;
    const auto b=WhBase();
    if(!b || !MatchBytes(b+kMultiviewPacketDispatchRva,kMultiviewPacketDispatchPrologue,sizeof(kMultiviewPacketDispatchPrologue))) {
        Trace("V65.60-BACKEND-DISPATCH REFUSED exact +0x42DA44 prologue mismatch"); return false;
    }
    DetourModKit::hook::InlineRequest req{"kcd2vr_v6560_multiview_packet_dispatch",DetourModKit::hook::Target{DetourModKit::Address{b+kMultiviewPacketDispatchRva}},DetourModKit::hook::Options{}};
    auto h=DetourModKit::hook::inline_at(std::move(req),&MultiviewPacketDispatchDetour); if(!h) return false;
    g_multiview_packet_dispatch_hook=std::move(*h);
    g_multiview_packet_dispatch_original=g_multiview_packet_dispatch_hook->original<MultiviewPacketDispatchFn>();
    if(!g_multiview_packet_dispatch_original || !g_multiview_packet_dispatch_hook->enable()){ DisableMultiviewPacketDispatchHook(); return false; }
    if(!EnsureSecondaryViewGateCensusHooks()){ DisableMultiviewPacketDispatchHook(); return false; }
    Trace("V65.69-BACKEND-DISPATCH installed exact +0x42DA44 packet qualifier + mode-output lineage census"); return true;
}

void DisableStereoBuildHook() noexcept {
    DisableCameraPairHook();
    if(g_stereo_build_hook){ (void)g_stereo_build_hook->disable(); g_stereo_build_hook.reset(); }
    g_stereo_build_original=nullptr; g_camera_pair_builder=nullptr;
    g_stereo_build_installed.store(false,std::memory_order_release);
}

bool EnsureNexusHook(bool physicalOnly) noexcept {
    if(g_nexus_hook) return g_nexus_original&&g_nexus_installed.load(std::memory_order_acquire);
    if(physicalOnly?!ExactPhysicalFrameReady6699():!ExactBuildReady()) return false;
    const auto b=WhBase();
    const auto target=b+kFrameNexusRva;
    if(!Executable(reinterpret_cast<void*>(target))) return false;
    DetourModKit::hook::InlineRequest req{
        "kcd2vr_v6504a_f8_ccryaction_frame_nexus",
        DetourModKit::hook::Target{DetourModKit::Address{target}},
        DetourModKit::hook::Options{}};
    auto h=DetourModKit::hook::inline_at(std::move(req),&FrameNexusDetour);
    if(!h) return false;
    g_nexus_hook=std::move(*h);
    g_nexus_original=g_nexus_hook->original<FrameNexusFn>();
    if(!g_nexus_original || !g_nexus_hook->enable()) {
        g_nexus_original=nullptr;
        g_nexus_hook.reset();
        return false;
    }
    g_nexus_installed.store(true,std::memory_order_release);
    Trace("V65.07-F8 rung1 installed exact CCryAction+0x669C88 frame-nexus observer; no DoFrame/GetView hooks, no writes");
    return true;
}

bool ResolveEngineSlots(std::uintptr_t& camera,std::uintptr_t& p7e8,std::uintptr_t& p7f8) noexcept {
    camera=p7e8=p7f8=0;
    void* engine=EngineObject();
    if(!engine) return false;
    camera=SafeVslotTarget(engine,kCameraHandoffSlot);
    p7e8=SafeVslotTarget(engine,kPhase7e8Slot);
    p7f8=SafeVslotTarget(engine,kPhase7f8Slot);
    g_camera_target_rva.store(ToRva(camera),std::memory_order_release);
    g_phase7e8_target_rva.store(ToRva(p7e8),std::memory_order_release);
    g_phase7f8_target_rva.store(ToRva(p7f8),std::memory_order_release);
    return Executable(reinterpret_cast<void*>(camera)) &&
           Executable(reinterpret_cast<void*>(p7e8)) &&
           Executable(reinterpret_cast<void*>(p7f8));
}

bool EnsureCameraHook() noexcept {
    if(g_camera_hook) return true;
    if(!EnsureNexusHook()) return false;
    std::uintptr_t camera=0,p7e8=0,p7f8=0;
    if(!ResolveEngineSlots(camera,p7e8,p7f8)) {
        Tracef("V65.07-F8 rung2 FAIL-CLOSED engine=%p cameraRva=0x%llX phase7e8Rva=0x%llX phase7f8Rva=0x%llX",
            EngineObject(),static_cast<unsigned long long>(ToRva(camera)),
            static_cast<unsigned long long>(ToRva(p7e8)),static_cast<unsigned long long>(ToRva(p7f8)));
        return false;
    }
    DetourModKit::hook::InlineRequest req{
        "kcd2vr_v6504a_f8_engine_camera_handoff",
        DetourModKit::hook::Target{DetourModKit::Address{camera}},
        DetourModKit::hook::Options{}};
    auto h=DetourModKit::hook::inline_at(std::move(req),&CameraHandoffDetour);
    if(!h) return false;
    g_camera_hook=std::move(*h);
    g_camera_original=g_camera_hook->original<CameraHandoffFn>();
    if(!g_camera_hook->enable()) {
        g_camera_original=nullptr;
        g_camera_hook.reset();
        return false;
    }
    g_camera_installed.store(true,std::memory_order_release);
    Tracef("V65.07-F8 rung2 camera handoff observer installed global=WHGame+0x%llX slot=0x40 targetRva=0x%llX READ-ONLY",
        static_cast<unsigned long long>(kEngineGlobalRva),static_cast<unsigned long long>(ToRva(camera)));
    return true;
}

bool EnsureGetOrCreateHook() noexcept {
    if(g_get_or_create_hook) return true;
    if(!EnsureNexusHook()) return false;
    const auto b=WhBase();
    const auto target=b+kGetOrCreateRenderViewRva;
    if(!Executable(reinterpret_cast<void*>(target)) ||
       !MatchBytes(target,kGetOrCreatePrologue,sizeof(kGetOrCreatePrologue))) return false;
    DetourModKit::hook::InlineRequest req{
        "kcd2vr_v6504a_f8_get_or_create_render_view",
        DetourModKit::hook::Target{DetourModKit::Address{target}},
        DetourModKit::hook::Options{}};
    auto h=DetourModKit::hook::inline_at(std::move(req),&GetOrCreateRenderViewDetour);
    if(!h) return false;
    g_get_or_create_hook=std::move(*h);
    g_get_or_create_original=g_get_or_create_hook->original<GetOrCreateRenderViewFn>();
    if(!g_get_or_create_hook->enable()) {
        g_get_or_create_original=nullptr;
        g_get_or_create_hook.reset();
        return false;
    }
    g_get_or_create_installed.store(true,std::memory_order_release);
    Trace("V65.07-F8 rung3 GetOrCreateRenderView observer installed; existing FrameSync pipeline observer supplies renderer correlation; no recursive-pass hook");
    return true;
}

bool EnsurePhaseHooks() noexcept {
    if(g_phase7e8_hook && g_phase7f8_hook) return true;
    if(!EnsureGetOrCreateHook()) return false;
    std::uintptr_t camera=0,p7e8=0,p7f8=0;
    if(!ResolveEngineSlots(camera,p7e8,p7f8)) return false;
    if(p7e8==p7f8 || p7e8==camera || p7f8==camera) {
        Tracef("V65.07-F8 rung4 FAIL-CLOSED overlapping engine vslots camera=0x%llX 7e8=0x%llX 7f8=0x%llX",
            static_cast<unsigned long long>(ToRva(camera)),static_cast<unsigned long long>(ToRva(p7e8)),static_cast<unsigned long long>(ToRva(p7f8)));
        return false;
    }

    if(!g_phase7e8_hook) {
        DetourModKit::hook::InlineRequest req{
            "kcd2vr_v6504a_f8_engine_phase_7e8",
            DetourModKit::hook::Target{DetourModKit::Address{p7e8}},
            DetourModKit::hook::Options{}};
        auto h=DetourModKit::hook::inline_at(std::move(req),&Phase7e8Detour);
        if(!h) return false;
        g_phase7e8_hook=std::move(*h);
        g_phase7e8_original=g_phase7e8_hook->original<Phase7e8Fn>();
        if(!g_phase7e8_hook->enable()) {
            g_phase7e8_original=nullptr;
            g_phase7e8_hook.reset();
            return false;
        }
    }
    if(!g_phase7f8_hook) {
        DetourModKit::hook::InlineRequest req{
            "kcd2vr_v6504a_f8_engine_phase_7f8",
            DetourModKit::hook::Target{DetourModKit::Address{p7f8}},
            DetourModKit::hook::Options{}};
        auto h=DetourModKit::hook::inline_at(std::move(req),&Phase7f8Detour);
        if(!h) {
            if(g_phase7e8_hook) { (void)g_phase7e8_hook->disable(); g_phase7e8_hook.reset(); g_phase7e8_original=nullptr; }
            return false;
        }
        g_phase7f8_hook=std::move(*h);
        g_phase7f8_original=g_phase7f8_hook->original<Phase7f8Fn>();
        if(!g_phase7f8_hook->enable()) {
            g_phase7f8_original=nullptr;
            g_phase7f8_hook.reset();
            if(g_phase7e8_hook) { (void)g_phase7e8_hook->disable(); g_phase7e8_hook.reset(); g_phase7e8_original=nullptr; }
            return false;
        }
    }
    g_phase_hooks_installed.store(true,std::memory_order_release);
    Tracef("V65.07-F8 rung4 engine phase observers installed slots={0x7E8:rva0x%llX,0x7F8:rva0x%llX}; still READ-ONLY, second render intentionally unavailable",
        static_cast<unsigned long long>(ToRva(p7e8)),static_cast<unsigned long long>(ToRva(p7f8)));
    return true;
}

void DisablePhaseHooks() noexcept {
    if(g_phase7f8_hook) { (void)g_phase7f8_hook->disable(); g_phase7f8_hook.reset(); }
    if(g_phase7e8_hook) { (void)g_phase7e8_hook->disable(); g_phase7e8_hook.reset(); }
    g_phase7e8_original=nullptr;
    g_phase7f8_original=nullptr;
    g_phase_hooks_installed.store(false,std::memory_order_release);
}

void DisableGetOrCreateHook() noexcept {
    if(g_get_or_create_hook) { (void)g_get_or_create_hook->disable(); g_get_or_create_hook.reset(); }
    g_get_or_create_original=nullptr;
    g_get_or_create_installed.store(false,std::memory_order_release);
}

void DisableCameraHook() noexcept {
    if(g_camera_hook) { (void)g_camera_hook->disable(); g_camera_hook.reset(); }
    g_camera_original=nullptr;
    g_camera_installed.store(false,std::memory_order_release);
}

void DisableNexusHook() noexcept {
    if(g_nexus_hook) { (void)g_nexus_hook->disable(); g_nexus_hook.reset(); }
    g_nexus_original=nullptr;
    g_nexus_installed.store(false,std::memory_order_release);
}

void DisableAbove(int mode) noexcept {
    if(mode!=4) DisableCameraPairHook();
    if(!(mode==2 || mode==4)) DisablePhaseHooks();
    if(mode<1 || mode>=5) DisableStereoBuildHook();
    if(mode<1 || mode>=5) DisableCameraHook();
    if(mode<5) DisableCSystemRenderHook();
    if(mode<1) DisableGetOrCreateHook();
    if(mode<1) DisableNexusHook();
    if(mode!=3 && mode!=4) DeviceHook::SetStereoProducerProbe(false,0,0);
}

} // namespace

bool EnsurePhysicalFramePump6698() noexcept {return EnsureNexusHook(true);}
std::uint64_t CurrentNativeFrameHJ1() noexcept {
    // Exact outer gameplay call only. Async workers have no proven association
    // with this thread's nexus and must retain their normal tracking family.
    return t_nexus_depth==1 && !(t_nexus_flags&0x20u) ? t_nexus_serial : 0;
}


// V66.12: F11 proof plus gated F9 stereo-design path owns the normal-launch stereo lab.
// The older five-phase replay controller remains history and is not installed.
bool SingleRunBusy() noexcept { return native_stereo_preflight::Busy(); }
int SingleRunPhase() noexcept { return native_stereo_preflight::Phase(); }
bool Eye1Verified() noexcept { return native_stereo_preflight::Eye1Verified(); }
unsigned Eye1Outcome() noexcept { return native_stereo_preflight::Eye1Outcome(); }
bool StartStereoDesign(int stage,float ipd) noexcept { return native_stereo_preflight::StartStereoDesign(stage,ipd); }
int LastRunKind() noexcept { return native_stereo_preflight::LastRunKind(); }
bool LastRunPassed() noexcept { return native_stereo_preflight::LastRunPassed(); }
int LastDesignStagePassed() noexcept { return native_stereo_preflight::LastDesignStagePassed(); }
std::uint64_t LastDesignPairSerial() noexcept { return native_stereo_preflight::LastDesignPairSerial(); }
void StopSingleRun() noexcept { native_stereo_preflight::Stop(); }
void PollSingleRun() noexcept { native_stereo_preflight::Poll(); }


bool Initialize() noexcept {
    if(g_v6601_owned.load(std::memory_order_acquire)) return g_v6601_ready.load(std::memory_order_acquire);
    g_mode.store(0,std::memory_order_release);
    g_v6594_diag_mode.store(0,std::memory_order_release);
    g_tangible_stage.store(0,std::memory_order_release);
    g_nexus_calls.store(0); g_nexus_present_advances.store(0);
    g_camera_calls.store(0); g_camera_in_nexus.store(0);
    g_get_or_create_calls.store(0); g_get_or_create_in_nexus.store(0);
    g_pipeline_calls.store(0); g_pipeline_in_nexus.store(0); g_normal_pipeline_in_nexus.store(0);
    g_handoff_matches.store(0); g_handoff_misses.store(0); g_last_handoff_nexus.store(0); g_last_render_view_token.store(0); g_render_view_token_epochs.store(0); g_render_view_token_reuses.store(0); g_last_handoff_log_ms.store(0);
    for(auto& slot:g_handoff){ slot.ptr.store(0); slot.nexus.store(0); slot.call.store(0); slot.present.store(0); slot.requested_type.store(0); slot.producer_tid.store(0); slot.publish_tick_ms.store(0); slot.eye_phase.store(0); slot.pipeline_tick_ms.store(0); slot.pipeline_tid.store(0); slot.reset_tick_ms.store(0); slot.reset_tid.store(0); }
    g_phase7e8_calls.store(0); g_phase7f8_calls.store(0);
    g_csystem_render_calls.store(0); g_csystem_render_in_nexus.store(0);
    g_csystem_render_pipeline_matches.store(0); g_csystem_render_present_advances.store(0);
    g_csystem_render_last_caller_rva.store(0); g_csystem_render_begin_target_rva.store(0); g_csystem_render_end_target_rva.store(0);
    g_sysbegin_self.store(0); g_sysbegin_state_lo.store(0); g_sysbegin_state_hi.store(0); g_sysbegin_context_value.store(0);
    g_sysbegin_tid.store(0); g_sysbegin_tick_ms.store(0); g_sysbegin_capture_serial.store(0); g_sysbegin_last_used_serial.store(0); g_sysbegin_injected_calls.store(0); g_sysbegin_last_caller_rva.store(0);
    g_lifetime_dtor_body_calls.store(0); g_lifetime_deleting_dtor_calls.store(0); g_lifetime_reset_calls.store(0); g_lifetime_trace_events.store(0);
    g_lifetime_last_duplicate_return_tick.store(0); g_lifetime_last_duplicate_nexus.store(0); g_lifetime_last_duplicate_left_crv.store(0); g_lifetime_last_duplicate_right_crv.store(0);
    for(auto& slot:g_lifetime_seen){ slot.ptr.store(0); slot.dtor_entries.store(0); }
    ResetBoundaryTransactionV6553(); g_boundary_pipeline_signals.store(0); g_boundary_reset_signals.store(0);
    g_stereo_build_calls.store(0); g_stereo_build_writes.store(0); g_stereo_build_restores.store(0); g_stereo_build_rejects.store(0); g_last_applied_ipd.store(0.0f); g_native_loop_promotions.store(0);
    g_multiview_claimed.store(false); g_multiview_build_ok.store(false); g_multiview_pipeline_seen.store(false); g_multiview_pipeline_returned.store(false); g_multiview_crv.store(0); g_multiview_built_count.store(0); g_multiview_pair_calls.store(0); g_multiview_owned_pipeline_probe.store(false);
    g_last_nexus_serial.store(0); g_last_camera_nexus_serial.store(0); g_last_pipeline_nexus_serial.store(0);
    const bool ready=ExactMultiviewReadyV6556();
    if(ready) Trace("V65.59-F8 ready: native multiview + asynchronous next-PRESENT target-routing probe; then OFF");
    return ready;
}

void Shutdown() noexcept {
    native_stereo_preflight::Stop(); // eye-control hooks stay pinned until process exit
    DeviceHook::SetStereoProducerProbe(false,0,0);
    DisableAbove(0);
    DisableV6594TransactionHooks();
    g_v6594_diag_mode.store(0,std::memory_order_release);
    g_mode.store(0,std::memory_order_release);
    g_bridge_stage.store(0,std::memory_order_release);
    g_tangible_stage.store(0,std::memory_order_release);
    g_second_eye_postfx_stage.store(0,std::memory_order_release);
    DisableCSystemRenderBeginHookV6554();
    DisableRendererBeginFrameHookV6541();
    g_bridge_pass_eye.store(0,std::memory_order_release); g_bridge_dual_calls.store(0); g_bridge_dual_copies.store(0); g_bridge_right_renders.store(0); g_bridge_left_copy_misses.store(0); g_bridge_policy_failures.store(0); g_bridge_pair_latched.store(false);
    Tracef("V65.10-F8 shutdown nexus=%llu camera=%llu renderView=%llu pipeline=%llu stereoBuild=%llu writes=%llu restores=%llu rejects=%llu duplicateRenders=0",
        static_cast<unsigned long long>(g_nexus_calls.load()),
        static_cast<unsigned long long>(g_camera_calls.load()),
        static_cast<unsigned long long>(g_get_or_create_calls.load()),
        static_cast<unsigned long long>(g_pipeline_calls.load()),
        static_cast<unsigned long long>(g_stereo_build_calls.load()),
        static_cast<unsigned long long>(g_stereo_build_writes.load()),
        static_cast<unsigned long long>(g_stereo_build_restores.load()),
        static_cast<unsigned long long>(g_stereo_build_rejects.load()));
}

void SetMode(int mode,float openxr_ipd_meters) noexcept {
    if(g_v6601_owned.load(std::memory_order_acquire)) return; // V66.01: pinned one-run owner

    mode=std::clamp(mode,0,6);
    if(std::isfinite(openxr_ipd_meters) && openxr_ipd_meters>0.03f && openxr_ipd_meters<0.10f)
        g_runtime_ipd.store(openxr_ipd_meters,std::memory_order_release);
    const int old=g_mode.exchange(mode,std::memory_order_acq_rel);
    if(mode==0) { DisableAbove(0); for(auto& slot:g_handoff) slot.ptr.store(0,std::memory_order_release); Trace("V65.18-F8 mode=0 OFF; production owner may restore Stereo"); return; }
    if(old==0) { g_handoff_matches.store(0); g_handoff_misses.store(0); g_last_handoff_nexus.store(0); g_last_render_view_token.store(0); g_render_view_token_epochs.store(0); g_render_view_token_reuses.store(0); g_last_handoff_log_ms.store(0); for(auto& slot:g_handoff) slot.ptr.store(0); }

    bool ok=true;
    if(mode>=5){
        // Boundary/lifecycle proof must not perturb camera geometry or stereo metadata.
        DeviceHook::SetStereoProducerProbe(false,0,0);
        DisableCameraPairHook(); DisableStereoBuildHook();
        ok=EnsureNexusHook() && EnsureGetOrCreateHook() && EnsureCSystemRenderHook();
        if(mode==6) {
            // V65.49 rung6 expands the old boundary trace with the two already-known
            // engine phase observers. These remain READ-ONLY; the D0/D8 bracket is logged
            // but never called as a reset until its identity is actually proven.
            ok=ok && EnsureCameraHook() && EnsurePhaseHooks() && EnsureLifecycleAutopsyHooks();
            Trace("V65.53-F8 rung6 LIFECYCLE TRACE: CSystem + phases + exact CRenderView deleting-dtor/body/+0x427C1C instrumentation; D0/D8 remain READ-ONLY, no reset call");
        } else {
            DisablePhaseHooks(); DisableCameraHook();
        }
    } else {
        DisableCSystemRenderHook();
        // Rungs 1..4 retain the older camera/resource experiments for comparison.
        ok=EnsureNexusHook() && EnsureCameraHook() && EnsureGetOrCreateHook() && EnsureStereoBuildHook();
        if(mode==4) ok=ok&&EnsureCameraPairHook(); else DisableCameraPairHook();
        if(mode==2 || mode==4) ok=ok&&EnsurePhaseHooks(); else DisablePhaseHooks();
        if(mode==3 || mode==4) DeviceHook::SetStereoProducerProbe(true,0,0); else DeviceHook::SetStereoProducerProbe(false,0,0);
    }
    Tracef("V65.18-F8 mode=%d %s ready=%d requestedIpd=%.6f activeSeparation=%.6f producerPairs=%llu csystemCalls=%llu pipelineInside=%llu",
        mode,ModeName(mode),ok?1:0,double(g_runtime_ipd.load()),double(ActiveEyeSeparation()),
        static_cast<unsigned long long>(DeviceHook::StereoProducerSameFramePairCount()),
        static_cast<unsigned long long>(g_csystem_render_calls.load()),
        static_cast<unsigned long long>(g_csystem_render_pipeline_matches.load()));
}


int BridgeStage() noexcept { return V6601EffectiveBridgeStage(); }

const char* BridgeStageName(int stage) noexcept {
    switch(stage){
        case 1:return "LEFT-ONLY ORACLE (ONE RENDER)";
        case 2:return "RIGHT-ONLY ORACLE (ONE RENDER)";
        case 3:return "COUNT2 NATIVE MULTIVIEW (ONE RENDER)";
        default:return "OFF";
    }
}

int Count2SbsStage() noexcept { return g_v6573_sbs_stage.load(std::memory_order_acquire); }


void SetBridgeStage(int stage,float openxr_ipd_meters) noexcept {
    if(g_v6601_owned.load(std::memory_order_acquire)) return; // V66.01: pinned one-run owner

    stage=std::clamp(stage,0,3);
    g_tangible_stage.store(0,std::memory_order_release);
    if(std::isfinite(openxr_ipd_meters) && openxr_ipd_meters>0.03f && openxr_ipd_meters<0.10f)
        g_runtime_ipd.store(openxr_ipd_meters,std::memory_order_release);
    g_mode.store(0,std::memory_order_release);
    DeviceHook::SetStereoProducerProbe(false,0,0);
    DisableCSystemRenderBeginHookV6554(); DisableRendererBeginFrameHookV6541(); DisableCSystemRenderHook();
    DisableCameraHook(); DisableGetOrCreateHook(); DisablePhaseHooks(); DisableNexusHook();
    FrameSync::SetNativeStereoReentryMode(0,g_runtime_ipd.load(std::memory_order_acquire));

    if(stage==0) {
        const auto oldStage=V6601EffectiveBridgeStage();
        if(oldStage>=1 && oldStage<=3) Tracef("V65.83-CHANNEL-SUMMARY stage=%s calls={108:%u 110:%u 118:%u}",
            oldStage==1?"LEFT":(oldStage==2?"RIGHT":"COUNT2"),
            unsigned(g_v6583_ch108_calls.load(std::memory_order_acquire)),unsigned(g_v6583_ch110_calls.load(std::memory_order_acquire)),unsigned(g_v6583_ch118_calls.load(std::memory_order_acquire)));
        const auto gateCalls=g_v6565_gate_calls.load(std::memory_order_acquire);
        const auto applyCalls=g_v6565_apply_calls.load(std::memory_order_acquire);
        const auto modeCalls=g_v6565_mode_calls.load(std::memory_order_acquire);
        const auto mode2Calls=g_v6565_mode2_calls.load(std::memory_order_acquire);
        const auto secondarySeen=g_v6565_secondary_flag_seen.load(std::memory_order_acquire);
        if(gateCalls||applyCalls||modeCalls) Tracef("V65.71-MODE-LINEAGE-SUMMARY gateCalls=%u applyCalls=%u shaderModeCalls=%u mode2Calls=%u secondary4000Seen=%u assignCalls=%u quartetDispatchCalls=%u fingerprints=%u quartetSnapshots=%u provenance=%u childPayloads=%u q8Fingerprints=%u",unsigned(gateCalls),unsigned(applyCalls),unsigned(modeCalls),unsigned(mode2Calls),unsigned(secondarySeen),unsigned(g_v6566_assign_calls.load(std::memory_order_acquire)),unsigned(g_v6566_quartet_dispatch_calls.load(std::memory_order_acquire)),unsigned(g_v6567_fingerprint_calls.load(std::memory_order_acquire)),unsigned(g_v6567_quartet_snapshots.load(std::memory_order_acquire)),unsigned(g_v6568_provenance_lines.load(std::memory_order_acquire)),unsigned(g_v6569_child_payloads.load(std::memory_order_acquire)),unsigned(g_v6571_q8_fingerprints.load(std::memory_order_acquire)));
        g_bridge_stage.store(0,std::memory_order_release);
        DisableCameraPairHook(); DisableStereoBuildHook(); DisableMultiviewPacketDispatchHook();
        if(g_multiview_owned_pipeline_probe.exchange(false,std::memory_order_acq_rel)) (void)FrameSync::SetPipelineProbeEnabled(false);
        Trace("V65.71-F8 OFF; dual-path lineage census disarmed; normal mono bridge restored; duplicate-render paths remain RETIRED");
        return;
    }

    {
        const auto oldStage=V6601EffectiveBridgeStage();
        if(oldStage>=1 && oldStage<=3 && oldStage!=stage) Tracef("V65.83-CHANNEL-SUMMARY stage=%s calls={108:%u 110:%u 118:%u}",
            oldStage==1?"LEFT":(oldStage==2?"RIGHT":"COUNT2"),
            unsigned(g_v6583_ch108_calls.load(std::memory_order_acquire)),unsigned(g_v6583_ch110_calls.load(std::memory_order_acquire)),unsigned(g_v6583_ch118_calls.load(std::memory_order_acquire)));
    }
    g_bridge_stage.store(0,std::memory_order_release);
    if(stage==1) {
        std::lock_guard<std::mutex> lk(g_v6584_108_mutex);
        g_v6584_108_snapshot_count.fill(0u);
        for(auto& a:g_v6584_108_full_hashes) a.fill(0ull);
        for(auto& st:g_v6584_108_entry_hashes) for(auto& sn:st) sn.fill(0ull);
        Trace("V65.84-108 BASELINES RESET; capture order=LEFT->RIGHT->COUNT2");
    }
    g_v6583_ch108_calls.store(0,std::memory_order_release);
    g_v6583_ch110_calls.store(0,std::memory_order_release);
    g_v6583_ch118_calls.store(0,std::memory_order_release);
    g_multiview_claimed.store(false,std::memory_order_release);
    g_multiview_build_ok.store(false,std::memory_order_release);
    g_multiview_pipeline_seen.store(false,std::memory_order_release);
    g_multiview_pipeline_returned.store(false,std::memory_order_release);
    g_multiview_crv.store(0,std::memory_order_release);
    g_multiview_built_count.store(0,std::memory_order_release);
    g_multiview_pair_calls.store(0,std::memory_order_release);
    g_v6560_packet_claimed.store(false,std::memory_order_release);
    g_v6560_packet_owner.store(0,std::memory_order_release);
    g_v6560_packet_records.store(0,std::memory_order_release);
    g_v6560_packet_object.store(0,std::memory_order_release);
    g_v6560_packet_backend.store(0,std::memory_order_release);
    g_v6565_gate_calls.store(0,std::memory_order_release);
    g_v6565_apply_calls.store(0,std::memory_order_release);
    g_v6565_mode_calls.store(0,std::memory_order_release);
    g_v6565_mode2_calls.store(0,std::memory_order_release);
    g_v6565_secondary_flag_seen.store(0,std::memory_order_release);
    g_v6566_assign_calls.store(0,std::memory_order_release);
    g_v6566_quartet_dispatch_calls.store(0,std::memory_order_release);
    g_v6567_fingerprint_calls.store(0,std::memory_order_release);
    g_v6567_quartet_snapshots.store(0,std::memory_order_release);
    g_v6568_provenance_lines.store(0,std::memory_order_release);
    g_v6569_child_payloads.store(0,std::memory_order_release);
    g_v6571_q8_fingerprints.store(0,std::memory_order_release);
    for(auto& cell:g_v6567_seen_objects) cell.store(0,std::memory_order_release);
    for(auto& cell:g_v6568_seen_ptrs) cell.store(0,std::memory_order_release);
    for(auto& cell:g_v6569_seen_children) cell.store(0,std::memory_order_release);
    for(auto& cell:g_v6571_seen_q8) cell.store(0,std::memory_order_release);
    DisableCameraPairHook(); DisableStereoBuildHook();
    const bool exact=ExactMultiviewReadyV6556();
    const bool pipeline_already=FrameSync::PipelineProbeEnabled();
    g_multiview_owned_pipeline_probe.store(!pipeline_already,std::memory_order_release);
    const bool pipeline_ok=pipeline_already || FrameSync::SetPipelineProbeEnabled(true);
    const bool need_pair=(stage==3);
    const bool ok=exact && EnsureStereoBuildHook() && (!need_pair || EnsureCameraPairHook()) && pipeline_ok && EnsureMultiviewPacketDispatchHook();
    if(!ok) {
        DisableCameraPairHook(); DisableStereoBuildHook(); DisableMultiviewPacketDispatchHook();
        if(g_multiview_owned_pipeline_probe.exchange(false,std::memory_order_acq_rel)) (void)FrameSync::SetPipelineProbeEnabled(false);
        Trace("V65.69-F8 REFUSED: mode-output lineage prerequisites unavailable; normal mono render left untouched");
        return;
    }
    if(!need_pair) DisableCameraPairHook();
    g_bridge_stage.store(stage,std::memory_order_release);
    g_v6562a_oracle_frames.store(0,std::memory_order_release);
    Tracef("V65.69-ORACLE ARM stage=%d name=%s ipd=%.6f contract=ONE-CSystemRender/ONE-CStandardGraphicsPipeline persistent=YES duplicateRender=NO XRoverride=NO",
        stage,BridgeStageName(stage),double(g_runtime_ipd.load(std::memory_order_acquire)));
}


bool SetV6627RightPreparationExperiment(bool enabled) noexcept {
    if(enabled) {
        if(!EnsureV6627RightPrepHook()) {
            g_v6627_right_prep_enabled.store(false,std::memory_order_release);
            Trace("V66.27-RIGHT-PREP enabled=0 reason=hook-unavailable");
            return false;
        }
        g_v6627_setview_owner.store(0,std::memory_order_release);
        g_v6627_setview_crv.store(0,std::memory_order_release);
        g_v6627_setview_tid.store(0,std::memory_order_release);
        g_v6627_setview_captures.store(0,std::memory_order_release);
        g_v6627_right_prep_calls.store(0,std::memory_order_release);
        g_v6627_right_prep_skips.store(0,std::memory_order_release);
        g_v6627_right_prep_enabled.store(true,std::memory_order_release);
        Trace("V66.27-RIGHT-PREP enabled=1 entry=WHGame+0x7C2D40 then existing +0x864728 render; exact normal owner/CRV/thread required; no +0x864984 duplicate");
        return true;
    }
    g_v6627_right_prep_enabled.store(false,std::memory_order_release);
    Tracef("V66.27-RIGHT-PREP enabled=0 summary captures=%llu calls=%llu skips=%llu",
        (unsigned long long)g_v6627_setview_captures.load(std::memory_order_acquire),
        (unsigned long long)g_v6627_right_prep_calls.load(std::memory_order_acquire),
        (unsigned long long)g_v6627_right_prep_skips.load(std::memory_order_acquire));
    return true;
}

bool V6627RightPreparationEnabled() noexcept { return g_v6627_right_prep_enabled.load(std::memory_order_acquire); }

void GetV6628EyeWork(V6628EyeWork& out) noexcept {
    out.modeLeft=g_v6628_mode_eye[0].load(std::memory_order_relaxed);
    out.modeRight=g_v6628_mode_eye[1].load(std::memory_order_relaxed);
    out.gateLeft=g_v6628_gate_eye[0].load(std::memory_order_relaxed);
    out.gateRight=g_v6628_gate_eye[1].load(std::memory_order_relaxed);
    out.prepCalls=g_v6627_right_prep_calls.load(std::memory_order_relaxed);
    out.prepSkips=g_v6627_right_prep_skips.load(std::memory_order_relaxed);
}



bool V6639R1CenterShadowPrepRequested() noexcept {
    const int step=g_v6639r1_pairwide_step.load(std::memory_order_acquire);
    return step==1 || step==3;
}
bool PublishV6639R1CenterShadowRecord(void* crvPtr,std::uint32_t frame,const void* recordPtr,std::size_t bytes,std::uint64_t source_camera_hash) noexcept {
    g_v6639r1_center_shadow.valid=false;
    if(!V6639R1CenterShadowPrepRequested() || !crvPtr || !recordPtr || bytes<kBuiltOutputStride) return false;
    const auto record=reinterpret_cast<const unsigned char*>(recordPtr);
    V6639R1CenterShadowSnapshot snap{};
    snap.crv=reinterpret_cast<std::uintptr_t>(crvPtr);
    snap.frame=frame;
    snap.source_camera_hash=source_camera_hash;
    snap.center_record_hash=SafeFnv1a64(record,kBuiltOutputStride);
    const bool ok=V6639R1SafeCopy(record+0x10,snap.block9b40.data(),snap.block9b40.size()) &&
                  V6639R1SafeCopy(record+0x34,snap.block9b64.data(),snap.block9b64.size()) &&
                  V6639R1SafeCopy(record+0x90,snap.block9bc0.data(),snap.block9bc0.size()) &&
                  V6639R1SafeCopy(record+0x190,snap.block9cc0.data(),snap.block9cc0.size()) &&
                  snap.center_record_hash!=0;
    snap.valid=ok;
    g_v6639r1_center_shadow=snap;
    static thread_local std::uint64_t n=0; ++n;
    if(n<=24ull || (n%600ull)==0ull || !ok) Tracef(
        "V66.39R1-CENTER-BUILD n=%llu crv=%p frame=%u sourceCameraHash=%016llX centerRecordHash=%016llX publishReadback=%d result=%s",
        (unsigned long long)n,crvPtr,unsigned(frame),(unsigned long long)source_camera_hash,
        (unsigned long long)snap.center_record_hash,ok?1:0,ok?"PUBLISHED":"NOT-ACTIVE");
    return ok;
}

bool V6638InterEyeRenderBeginReset() noexcept { return g_v6638_intereye_renderbegin_reset.load(std::memory_order_acquire); }
bool RunV6638InterEyeRenderBeginReset() noexcept {
    if(!g_v6638_intereye_renderbegin_reset.load(std::memory_order_acquire)) return true;
    const auto sys=reinterpret_cast<void*>(g_sysbegin_self.load(std::memory_order_acquire));
    std::uint64_t age=~0ull, serial=0;
    const bool ok=sys && InvokeFreshSystemRenderBeginV6554(sys,&age,&serial);
    static std::atomic<std::uint64_t> n{0}; const auto k=n.fetch_add(1,std::memory_order_relaxed)+1;
    if(k<=12 || (k%120ull)==0 || !ok) Tracef("V66.38-INTEREYE-RESET n=%llu ok=%d serial=%llu ageMs=%llu placement=AFTER-LEFT-BEFORE-RIGHT",
        (unsigned long long)k,ok?1:0,(unsigned long long)serial,(unsigned long long)age);
    return ok;
}




bool SetV6626NativeStageCensus(bool enabled) noexcept {
    if(enabled){
        const bool begin_ok=EnsureCSystemRenderBeginHookV6554();
        const bool secondary_ok=EnsureSecondaryViewGateCensusHooks();
        const bool ok=begin_ok&&secondary_ok;
        g_v6626_stage_apply_eye0.store(0);g_v6626_stage_apply_eye1.store(0);
        g_v6626_stage_gate_eye0.store(0);g_v6626_stage_gate_eye1.store(0);
        g_v6626_mode_eye0.store(0);g_v6626_mode_eye1.store(0);
        g_v6631_cloud_ready_eye0.store(0);g_v6631_cloud_ready_eye1.store(0);g_v6631_cloud_ready_fail_eye0.store(0);g_v6631_cloud_ready_fail_eye1.store(0);
        g_v6631_cloud_eye_setup_eye0.store(0);g_v6631_cloud_eye_setup_eye1.store(0);g_v6631_cloud_shadow_setup_eye0.store(0);g_v6631_cloud_shadow_setup_eye1.store(0);
        g_v6626_native_stage_census.store(ok,std::memory_order_release);
        Tracef("V66.31-POSTFX-CENSUS enabled=%d renderBegin=%d stageAndCloudHooks=%d policy=READ-ONLY purpose=CLOUD-SHADOW-POSTFX-OWNERSHIP",ok?1:0,begin_ok?1:0,secondary_ok?1:0);
        return ok;
    }
    g_v6626_native_stage_census.store(false,std::memory_order_release);
    Trace("V66.31-POSTFX-CENSUS disabled; hooks retained if owned by other diagnostics");
    return true;
}


void Cycle(float ipd) noexcept {
    const int m=Mode();
    SetMode(m>=6?0:m+1,ipd);
}

int Mode() noexcept {
    return g_mode.load(std::memory_order_acquire);
}

const char* ModeName(int mode) noexcept {
    switch(mode) {
        case 0:return "OFF / RESTORE PRODUCTION";
        case 1:return "OLD V65.10 RUNG3: OPENXR IPD";
        case 2:return "OLD V65.10 RUNG4: OPENXR IPD + PHASE OBSERVERS";
        case 3:return "OPENXR IPD + SAME-FRAME RTV RESOURCE PROOF";
        case 4:return "OPENXR IPD + NATIVE 2-EYE LOOP + RESOURCE PROOF";
        case 5:return "CRENDERVIEW TOKEN EPOCH + CSYSTEM BOUNDARY (READ-ONLY)";
        case 6:return "LIFECYCLE TRACE: CSYSTEM + PHASES + D0/D8 (READ-ONLY)";
        default:return "UNKNOWN";
    }
}

Status GetStatus() noexcept {
    Status s{};
    s.mode=Mode();
    s.supported_build=g_supported.load(std::memory_order_acquire);
    s.frame_nexus_hook_installed=g_nexus_installed.load(std::memory_order_acquire);
    s.camera_handoff_hook_installed=g_camera_installed.load(std::memory_order_acquire);
    s.get_or_create_hook_installed=g_get_or_create_installed.load(std::memory_order_acquire);
    s.phase_hooks_installed=g_phase_hooks_installed.load(std::memory_order_acquire);
    s.frame_nexus_rva=kFrameNexusRva;
    s.camera_handoff_target_rva=g_camera_target_rva.load(std::memory_order_acquire);
    s.phase7e8_target_rva=g_phase7e8_target_rva.load(std::memory_order_acquire);
    s.phase7f8_target_rva=g_phase7f8_target_rva.load(std::memory_order_acquire);
    s.csystem_render_hook_installed=g_csystem_render_installed.load(std::memory_order_acquire);
    s.csystem_render_rva=kCSystemRenderRva;
    s.csystem_render_last_caller_rva=g_csystem_render_last_caller_rva.load(std::memory_order_acquire);
    s.csystem_render_begin_target_rva=g_csystem_render_begin_target_rva.load(std::memory_order_acquire);
    s.csystem_render_end_target_rva=g_csystem_render_end_target_rva.load(std::memory_order_acquire);
    s.nexus_calls=g_nexus_calls.load(std::memory_order_acquire);
    s.nexus_present_advances=g_nexus_present_advances.load(std::memory_order_acquire);
    s.camera_handoff_calls=g_camera_calls.load(std::memory_order_acquire);
    s.camera_handoff_in_nexus=g_camera_in_nexus.load(std::memory_order_acquire);
    s.get_or_create_calls=g_get_or_create_calls.load(std::memory_order_acquire);
    s.get_or_create_in_nexus=g_get_or_create_in_nexus.load(std::memory_order_acquire);
    s.pipeline_calls=g_pipeline_calls.load(std::memory_order_acquire);
    s.pipeline_in_nexus=g_pipeline_in_nexus.load(std::memory_order_acquire);
    s.normal_pipeline_in_nexus=g_normal_pipeline_in_nexus.load(std::memory_order_acquire);
    s.handoff_matches=g_handoff_matches.load(std::memory_order_acquire);
    s.handoff_misses=g_handoff_misses.load(std::memory_order_acquire);
    s.last_handoff_nexus_serial=g_last_handoff_nexus.load(std::memory_order_acquire);
    s.render_view_token_epochs=g_render_view_token_epochs.load(std::memory_order_acquire);
    s.render_view_token_reuses=g_render_view_token_reuses.load(std::memory_order_acquire);
    s.last_render_view_token=g_last_render_view_token.load(std::memory_order_acquire);
    s.phase7e8_calls=g_phase7e8_calls.load(std::memory_order_acquire);
    s.phase7f8_calls=g_phase7f8_calls.load(std::memory_order_acquire);
    s.csystem_render_calls=g_csystem_render_calls.load(std::memory_order_acquire);
    s.csystem_render_in_nexus=g_csystem_render_in_nexus.load(std::memory_order_acquire);
    s.csystem_render_pipeline_matches=g_csystem_render_pipeline_matches.load(std::memory_order_acquire);
    s.csystem_render_present_advances=g_csystem_render_present_advances.load(std::memory_order_acquire);
    s.last_nexus_serial=g_last_nexus_serial.load(std::memory_order_acquire);
    s.last_camera_nexus_serial=g_last_camera_nexus_serial.load(std::memory_order_acquire);
    s.last_pipeline_nexus_serial=g_last_pipeline_nexus_serial.load(std::memory_order_acquire);
    s.last_flags=g_last_flags.load(std::memory_order_acquire);
    s.last_camera_out=g_last_camera_out.load(std::memory_order_acquire);
    s.stereo_build_hook_installed=g_stereo_build_installed.load(std::memory_order_acquire);
    s.camera_pair_hook_installed=g_camera_pair_installed.load(std::memory_order_acquire);
    s.native_loop_promotions=g_native_loop_promotions.load(std::memory_order_acquire);
    s.stereo_build_calls=g_stereo_build_calls.load(std::memory_order_acquire);
    s.stereo_build_writes=g_stereo_build_writes.load(std::memory_order_acquire);
    s.stereo_build_restores=g_stereo_build_restores.load(std::memory_order_acquire);
    s.stereo_build_rejects=g_stereo_build_rejects.load(std::memory_order_acquire);
    s.last_applied_ipd=g_last_applied_ipd.load(std::memory_order_acquire);
    return s;
}

void NotifyPipelineExecute(void* pipeline,void* render_view,std::uint32_t view_type,std::uint32_t selector) noexcept {
    V6600ObserveUpdate(pipeline,true);
    // V65.56 one-shot proof: the already-existing pipeline detour observes the
    // retail CRenderView array/count without re-entering the pipeline.
    if(V6601EffectiveBridgeStage()>=1 && V6601EffectiveBridgeStage()<=3 && view_type==0u && render_view &&
       reinterpret_cast<std::uintptr_t>(render_view)==g_multiview_crv.load(std::memory_order_acquire) &&
       !g_multiview_pipeline_seen.exchange(true,std::memory_order_acq_rel)) {
        const auto u=reinterpret_cast<std::uintptr_t>(render_view);
        const auto count=SafeReadU32(reinterpret_cast<void*>(u+kBuiltRecordCount));
        std::uint64_t h0=1469598103934665603ull,h1=1469598103934665603ull;
        std::uint32_t firstDiff=0xFFFFFFFFu,diffDwords=0; bool readable=false;
        __try {
            auto* a=reinterpret_cast<const std::uint32_t*>(u+kBuiltOutput0);
            auto* b=reinterpret_cast<const std::uint32_t*>(u+kBuiltOutput0+kBuiltOutputStride);
            if(V6601EffectiveBridgeStage()==3 && count==2u && Count2SbsStage()>0) {
                V6575AccumulateDiff("SOURCE400",reinterpret_cast<const unsigned char*>(a),reinterpret_cast<const unsigned char*>(b),0x400,
                    g_v6575_source_samples,g_v6575_source_diff_hits.data(),g_v6575_source_diff_hits.size());
            }
            for(std::uint32_t i=0;i<kBuiltOutputStride/4;++i) {
                const auto va=a[i],vb=b[i];
                h0=(h0^va)*1099511628211ull; h1=(h1^vb)*1099511628211ull;
                if(va!=vb) { if(firstDiff==0xFFFFFFFFu) firstDiff=i*4u; ++diffDwords; }
            }
            readable=true;
        } __except(EXCEPTION_EXECUTE_HANDLER) { readable=false; }
        if(Count2SbsStage()>0) {
            std::uint64_t ps=0,pt=0,pw=0,pc=0; std::uint32_t ph=0;
            const bool haveSize=DeviceHook::GetLatestPresentIdentity(&ps,&pt,&pw,&ph,&pc);
            bool sane=false;
            __try {
                const auto* a=reinterpret_cast<const unsigned char*>(u+kBuiltOutput0);
                const auto* e0=reinterpret_cast<const float*>(a+0x3B0);
                const auto* q0=reinterpret_cast<const std::uint32_t*>(a+0x3B0);
                auto eq=[](float x,float y){ return std::isfinite(x) && std::fabs(x-y)<0.01f; };
                sane=haveSize && eq(e0[0],static_cast<float>(pw)) && eq(e0[1],static_cast<float>(ph));
                if(count>1) {
                    const auto* b=a+kBuiltOutputStride; const auto* e1=reinterpret_cast<const float*>(b+0x3B0); const auto* q1=reinterpret_cast<const std::uint32_t*>(b+0x3B0);
                    sane=sane && eq(e1[0],static_cast<float>(pw)) && eq(e1[1],static_cast<float>(ph));
                    Tracef("V65.74-EXTENT-SOURCE stage=%d present=%llux%u rec0={%.6f,%.6f,%.6f,%.6f} raw0={%08X,%08X,%08X,%08X} rec1={%.6f,%.6f,%.6f,%.6f} raw1={%08X,%08X,%08X,%08X} sane=%d",
                        V6601EffectiveBridgeStage(),static_cast<unsigned long long>(pw),unsigned(ph),double(e0[0]),double(e0[1]),double(e0[2]),double(e0[3]),q0[0],q0[1],q0[2],q0[3],double(e1[0]),double(e1[1]),double(e1[2]),double(e1[3]),q1[0],q1[1],q1[2],q1[3],sane?1:0);
                } else {
                    Tracef("V65.74-EXTENT-SOURCE stage=%d present=%llux%u rec0={%.6f,%.6f,%.6f,%.6f} raw0={%08X,%08X,%08X,%08X} sane=%d",
                        V6601EffectiveBridgeStage(),static_cast<unsigned long long>(pw),unsigned(ph),double(e0[0]),double(e0[1]),double(e0[2]),double(e0[3]),q0[0],q0[1],q0[2],q0[3],sane?1:0);
                }
            } __except(EXCEPTION_EXECUTE_HANDLER) { sane=false; }
            g_v6574_extent_sane.store(sane,std::memory_order_release);
            g_v6574_extent_logs.fetch_add(1,std::memory_order_relaxed);
        }
        const auto oracle_frame=g_v6562a_oracle_frames.load(std::memory_order_relaxed);
        if(oracle_frame<=3 || (oracle_frame%120ull)==0ull) {
            Tracef("V65.63-PIPELINE ENTER frame=%llu stage=%d pipeline=%p crv=%p selector=%u records=%u buildOk=%d pairCalls=%u rec0Hash=%016llX rec1Hash=%016llX diffDwords=%u firstDiff=0x%X readable=%d downstream=+0x653940(count=%u,strideIn=0x400,strideOut=0x470)",
                static_cast<unsigned long long>(oracle_frame),V6601EffectiveBridgeStage(),pipeline,render_view,unsigned(selector),unsigned(count),g_multiview_build_ok.load()?1:0,unsigned(g_multiview_pair_calls.load()),
                static_cast<unsigned long long>(h0),static_cast<unsigned long long>(h1),unsigned(diffDwords),unsigned(firstDiff),readable?1:0,unsigned(count));
        }
        // V65.63: the bounded GPU observer is armed only by the exact +0x653940
        // return-address-qualified 0x470 packet in +0x42DA44.  LEFT/RIGHT/COUNT2
        // each get one next-PRESENT routing window per activation.
        if(g_multiview_build_ok.load(std::memory_order_acquire) && !g_v6560_packet_claimed.load(std::memory_order_acquire)) {
            Tracef("V65.63-NVIEW-PACKET waiting stage=%d eye=%s count=%u caller=+0x654E37",
                V6601EffectiveBridgeStage(),V6601EffectiveBridgeStage()==1?"LEFT":(V6601EffectiveBridgeStage()==2?"RIGHT":"COUNT2"),unsigned(count));
        }
    }
    if(g_mode.load(std::memory_order_relaxed)<1) return;
    const auto n=g_pipeline_calls.fetch_add(1,std::memory_order_relaxed)+1;
    const bool in_nexus=t_nexus_depth>0 && t_nexus_serial!=0;
    if(in_nexus) {
        g_pipeline_in_nexus.fetch_add(1,std::memory_order_relaxed);
        if(view_type==0u) g_normal_pipeline_in_nexus.fetch_add(1,std::memory_order_relaxed);
        g_last_pipeline_nexus_serial.store(t_nexus_serial,std::memory_order_release);
    }
    bool handoff_match=false;
    std::uint64_t producer_nexus=0,producer_call=0,producer_present=0;
    std::uint32_t producer_type=0,producer_tid=0;
    const auto wanted=reinterpret_cast<std::uintptr_t>(render_view);
    if(wanted) {
        // A RenderView object can be reused across frames. Scan the bounded recent
        // publication ring and select the newest stable publication for this exact
        // pointer rather than accepting the first (possibly stale) match.
        for(auto& slot:g_handoff) {
            if(slot.ptr.load(std::memory_order_acquire)!=wanted) continue;
            const auto candidate_nexus=slot.nexus.load(std::memory_order_relaxed);
            const auto candidate_call=slot.call.load(std::memory_order_relaxed);
            const auto candidate_present=slot.present.load(std::memory_order_relaxed);
            const auto candidate_type=slot.requested_type.load(std::memory_order_relaxed);
            const auto candidate_tid=slot.producer_tid.load(std::memory_order_relaxed);
            // Recheck the publication token after reading metadata. If the producer
            // recycled this slot concurrently, discard this snapshot.
            if(slot.ptr.load(std::memory_order_acquire)!=wanted) continue;
            if(!handoff_match || candidate_call>producer_call) {
                handoff_match=true;
                producer_nexus=candidate_nexus;
                producer_call=candidate_call;
                producer_present=candidate_present;
                producer_type=candidate_type;
                producer_tid=candidate_tid;
            }
        }
    }
    if(handoff_match) {
        g_handoff_matches.fetch_add(1,std::memory_order_relaxed);
        g_last_handoff_nexus.store(producer_nexus,std::memory_order_release);
        // V65.20 stereo-token test: treat the exact renderView pointer as the
        // asynchronous frame/render identity already proven to cross game->render threads.
        // Count pointer epochs vs same-pointer reuse; this is strictly read-only.
        const auto prior=g_last_render_view_token.exchange(wanted,std::memory_order_acq_rel);
        if(prior!=wanted) g_render_view_token_epochs.fetch_add(1,std::memory_order_relaxed);
        else g_render_view_token_reuses.fetch_add(1,std::memory_order_relaxed);
    } else if(wanted) g_handoff_misses.fetch_add(1,std::memory_order_relaxed);

    if(handoff_match && wanted) {
        if(auto* matched=FindNewestHandoffForPtrV6553(wanted)) {
            const auto now_tick=static_cast<std::uint64_t>(GetTickCount64());
            matched->pipeline_tick_ms.store(now_tick,std::memory_order_release);
            matched->pipeline_tid.store(static_cast<std::uint32_t>(GetCurrentThreadId()),std::memory_order_release);
            const auto call=matched->call.load(std::memory_order_acquire);
            const auto phase=matched->eye_phase.load(std::memory_order_acquire);
            if(phase==-1 && call==g_boundary_left_pub_call.load(std::memory_order_acquire) &&
               wanted==g_boundary_left_pub_ptr.load(std::memory_order_acquire)) {
                g_boundary_left_pipeline_tick.store(now_tick,std::memory_order_release);
                g_boundary_left_pipeline_tid.store(static_cast<std::uint32_t>(GetCurrentThreadId()),std::memory_order_release);
                g_boundary_pipeline_signals.fetch_add(1,std::memory_order_relaxed);
                if(g_boundary_left_pipeline_event) SetEvent(g_boundary_left_pipeline_event);
                const auto pub=matched->publish_tick_ms.load(std::memory_order_acquire);
                Tracef("V65.53-BOUNDARY PIPELINE-CONSUME pubCall=%llu ptr=%p nexus=%llu latencyMs=%llu producerTid=%u renderTid=%lu",
                    static_cast<unsigned long long>(call),render_view,static_cast<unsigned long long>(matched->nexus.load()),
                    static_cast<unsigned long long>((pub&&now_tick>=pub)?now_tick-pub:~0ull),unsigned(matched->producer_tid.load()),
                    static_cast<unsigned long>(GetCurrentThreadId()));
            }
        }
    }

    // V65.18: correlate the render-thread pipeline execution back to the newest
    // CSystem::Render submission carrying the same game-thread frame nexus. This
    // is read-only evidence for the asynchronous queue boundary; no job/pointer is
    // called or modified.
    bool boundary_match=false; std::uint64_t boundary_serial=0,boundary_present=0;
    std::uintptr_t boundary_self=0,boundary_params=0,boundary_caller=0; std::uint32_t boundary_tid=0;
    if(producer_nexus) {
        for(auto& bs:g_boundary) {
            const auto serial=bs.serial.load(std::memory_order_acquire);
            if(!serial || bs.nexus.load(std::memory_order_relaxed)!=producer_nexus) continue;
            if(!boundary_match || serial>boundary_serial) {
                boundary_match=true; boundary_serial=serial;
                boundary_present=bs.entry_present.load(std::memory_order_relaxed);
                boundary_self=bs.self.load(std::memory_order_relaxed);
                boundary_params=bs.params.load(std::memory_order_relaxed);
                boundary_caller=bs.caller_rva.load(std::memory_order_relaxed);
                boundary_tid=bs.producer_tid.load(std::memory_order_relaxed);
            }
        }
    }
    const auto now=static_cast<std::uint64_t>(GetTickCount64());
    auto last=g_last_handoff_log_ms.load(std::memory_order_relaxed);
    if(n<=8 || (handoff_match && now>=last+1000ull && g_last_handoff_log_ms.compare_exchange_strong(last,now,std::memory_order_acq_rel))) {
        const auto now_present=DeviceHook::PresentCaptureCount();
        Tracef("V65.20-F8-PIPELINE call=%llu pipeline=%p renderView=%p viewType=%u selector=%u renderTid=%lu handoff={match:%d producerNexus:%llu getCall:%llu requestedType:%u producerTid:%u presentDelta:%lld} token={epochs:%llu reuse:%llu last:%p} submit={match:%d csCall:%llu callerRva:0x%llX self:%p params:%p producerTid:%u presentDelta:%lld} cameraNexus=%llu activeStereoWrites=%llu READ-ONLY",
            static_cast<unsigned long long>(n),pipeline,render_view,unsigned(view_type),unsigned(selector),static_cast<unsigned long>(GetCurrentThreadId()),
            handoff_match?1:0,static_cast<unsigned long long>(producer_nexus),static_cast<unsigned long long>(producer_call),unsigned(producer_type),unsigned(producer_tid),
            static_cast<long long>(now_present)-static_cast<long long>(producer_present),
            static_cast<unsigned long long>(g_render_view_token_epochs.load(std::memory_order_acquire)),
            static_cast<unsigned long long>(g_render_view_token_reuses.load(std::memory_order_acquire)),
            reinterpret_cast<void*>(g_last_render_view_token.load(std::memory_order_acquire)),
            boundary_match?1:0,static_cast<unsigned long long>(boundary_serial),static_cast<unsigned long long>(boundary_caller),
            reinterpret_cast<void*>(boundary_self),reinterpret_cast<void*>(boundary_params),unsigned(boundary_tid),
            static_cast<long long>(now_present)-static_cast<long long>(boundary_present),
            static_cast<unsigned long long>(g_last_camera_nexus_serial.load(std::memory_order_acquire)),
            static_cast<unsigned long long>(g_stereo_build_writes.load(std::memory_order_acquire)));
    }
}

void NotifyPipelineExecuteReturn(void* pipeline,void* render_view,std::uint32_t view_type,std::uint32_t selector) noexcept {
    V6600ObserveUpdate(pipeline,false);
    const int oracle_stage=V6601EffectiveBridgeStage();
    if(oracle_stage<1 || oracle_stage>3 || view_type!=0u || !render_view) return;
    if(reinterpret_cast<std::uintptr_t>(render_view)!=g_multiview_crv.load(std::memory_order_acquire)) return;
    if(!g_multiview_pipeline_seen.load(std::memory_order_acquire) || g_multiview_pipeline_returned.exchange(true,std::memory_order_acq_rel)) return;
    const auto count=SafeReadU32(reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(render_view)+kBuiltRecordCount));
    const auto oracle_frame=g_v6562a_oracle_frames.load(std::memory_order_relaxed);
    if(oracle_frame<=3 || (oracle_frame%120ull)==0ull) {
        Tracef("V65.63-PIPELINE RETURN frame=%llu stage=%d eye=%s pipeline=%p crv=%p selector=%u records=%u RESULT nativeViews=%u pipelineRecords=%u persistent=1 oneCSystemRender=1 onePipelineExecute=1 duplicateRender=0",
            static_cast<unsigned long long>(oracle_frame),oracle_stage,oracle_stage==1?"LEFT":(oracle_stage==2?"RIGHT":"COUNT2"),pipeline,render_view,unsigned(selector),unsigned(count),unsigned(g_multiview_built_count.load()),unsigned(count));
    }
    // V65.62a: the pipeline return is the safe per-frame re-arm boundary. The next
    // normal CRenderView build may apply the same selected oracle mode again.
    g_multiview_pipeline_seen.store(false,std::memory_order_release);
    g_multiview_pipeline_returned.store(false,std::memory_order_release);
    g_multiview_claimed.store(false,std::memory_order_release);
}

int ClassifyModeShaderHash(std::uint64_t hash) noexcept {
    if(!hash) return 0;
    for(auto& h:g_v6581_mode0_ps_hashes) if(h.load(std::memory_order_acquire)==hash) return 1;
    for(auto& h:g_v6581_mode2_ps_hashes) if(h.load(std::memory_order_acquire)==hash) return 2;
    return 0;
}




std::uint64_t V6644RightShadowMaskSkips() noexcept { return g_v6644_right_shadowmask_skips.exchange(0,std::memory_order_relaxed); }

} // namespace kcd2vr::native_stereo_outer_lab
