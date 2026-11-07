// V63.7.1 interaction alignment for KCD2VR.
// Interaction function identification/signature patterns were derived from
// tkhquang/KCD2Tools TPVCamera (MIT License, Copyright (c) 2025 Quang Trinh).
// This VR adaptation preserves KCD2's native eye-origin/reach and publishes
// KCD2VR's compositor-corrected view-forward ray instead of a TP camera pose.

#include "interaction_alignment.h"

#include "camera_hook.h"
#include "retail_build112.h"
#include "debug_trace.h"
#include "diagnostic_trace.h"
#include "diagnostics/capture168.h"
#include "diagnostics/interaction_receipt59c10.h"
#include "diagnostics/door_receipt63.h"
#include "diagnostics/door_path_c54r3.h"
#include "diagnostics/door_area_observer_h29.h"
#include "diagnostics/interaction_cycle_h4.h"
#include "core/door_collector_c54.h"
#include "xr_manager.h"
#include "vrik/physical_interaction.h"
#include "vrik/interaction_ray112.h"
#include "vrik/interaction_cone57c1.h"
#include "core/snapshot_mailbox.h"
#include "core/door_pre_press66.h" // DS-F16: pre-press descriptor cache (file scope: namespace kcd2vr header)
#include "core/door_candidates38.h"
#include "core/door_origin_h12.h"
#include "core/door_use46.h"
#include "vrik/workstation178r20c1.h"
#include "frik_pose_probe.h"
#include "subsystems/alchemy/bridge67.h"
#include "subsystems/alchemy/native_selection68.h"
#include "subsystems/alchemy/head_select_r116.h" // R116: alchemy selection = headset view / hand reach
#include "vrik/eye_ray_r121.h" // R121: every head look ray starts at the rendered eye
#include "subsystems/hud_ui_controls/runtime/alchemy_head_name_d75.h"
#include "subsystems/interactions/quick_talk_candidate_d78.h"

#include <DetourModKit/hook.hpp>
#include <windows.h>
#include <intrin.h>

#include <algorithm>
#include <cstddef>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <mutex>
#include <optional>
#include <utility>

namespace kcd2vr::interaction_alignment {
namespace {

// Retail anchors independently identified by KCD2Tools TPVCamera and used only
// after validating the supported KCD2 WHGame timestamp and entry bytes.
constexpr std::uint32_t kSupportedWhTimestamp = 0x6A350E20u;
constexpr std::uintptr_t kInteractionRayBuildRva = 0x530324ull; // revalidated against retail timestamp 0x6A350E20
constexpr std::uintptr_t kInteractorLookRayRva   = 0x82C7D8ull; // revalidated exact function entry
constexpr std::uintptr_t kInteractionOnScreenRva = 0x935600ull; // revalidated exact function entry
constexpr std::uintptr_t kInteractorLookRaySpan  = 0x500ull;
// Same retail candidate loop: +64033B calls the held-item exclusion; +64035E
// immediately calls the projection gate with its native interaction policy.
constexpr std::uintptr_t kCandidateFilterRva=0x82CF14ull;
constexpr std::uintptr_t kCandidateFilterReturn=0x640340ull, kScreenReturn=0x640363ull;
constexpr std::uintptr_t kObjectPolicy=0x3A69D90ull,kDogPolicy=0x3A69E48ull,kChatPolicy=0x3A69F00ull;
std::uintptr_t g_wh_base=0;
std::atomic<bool> g_split_ready57c1{false};
using CandidateFilterFn=char(__fastcall*)(std::uintptr_t,std::uint32_t);
std::optional<DetourModKit::hook::Hook> g_candidate_hook57c1;
std::optional<DetourModKit::hook::Hook> g_direct_door_hook54r1;
CandidateFilterFn g_candidate_original57c1=nullptr;
struct Candidate57c1 {std::uintptr_t player=0;std::uint64_t tick=0;interaction57c1::Kind kind=interaction57c1::Kind::Unknown;std::uint32_t id=0;char cls[32]{};}; // R59.16C10R10: bounded class name for the receipts
thread_local Candidate57c1 g_candidate57c1{};
std::atomic<std::uint64_t> g_head_candidates57c1{0},g_hand_candidates57c1{0},g_unknown_candidates57c1{0};


using RayQueryBuildFn = std::uintptr_t(__fastcall*)(std::uintptr_t out, std::uintptr_t origin,
    std::uintptr_t dir, int objtypes, int flags, std::uintptr_t skip_ents,
    unsigned __int8 n, char mode);
using OnScreenCheckFn = char(__fastcall*)(std::uintptr_t a1, std::uintptr_t world_point,
    float* out_screen, std::uintptr_t a4);

std::optional<DetourModKit::hook::Hook> g_ray_hook;
std::optional<DetourModKit::hook::Hook> g_onscreen_hook;
RayQueryBuildFn g_ray_original = nullptr;
OnScreenCheckFn g_onscreen_original = nullptr;
std::uintptr_t g_lookray_lo = 0;
std::uintptr_t g_lookray_hi = 0;

std::atomic<bool> g_installed{false};
std::atomic<bool> g_enabled{true};
std::atomic<bool> g_visual_fallback_enabled{true};
std::atomic<bool> g_native_activity_fallback5916c3{false};
std::atomic<float> g_radius_m{0.21f};
std::atomic<float> g_cone_half_angle_deg{10.0f};
std::atomic<std::uint64_t> g_ray_builds{0};
std::atomic<std::uint64_t> g_interaction_calls{0};
std::atomic<std::uint64_t> g_redirects{0};
std::atomic<std::uint64_t> g_onscreen_checks{0};
std::atomic<std::uint64_t> g_onscreen_forced{0};
std::mutex g_hand_candidate_mutex178r14;HandCandidate g_hand_candidate178r14{}; // V66.178R14
kcd2vr::door_candidates38::Records<HandCandidate> g_door_candidates38;
std::uint64_t g_door_generation38=1; // protected by g_hand_candidate_mutex178r14
std::uint64_t g_door_acquisition_generation65=(std::uint64_t(1)<<63)|1; // same mutex; distinct from legacy ray identity
std::atomic<std::uint64_t> g_last_log_ms{0};

// Tear-free published visual ray. The producer is the KCD2VR main loop; hook
// callbacks are consumers and fail closed if a coherent sample is unavailable.
std::atomic<std::uint64_t> g_ray_seq{0};
std::atomic<float> g_dir_x{0.0f}, g_dir_y{1.0f}, g_dir_z{0.0f};
std::atomic<float> g_src_tan_x{0.0f}, g_src_tan_y{0.0f};
std::atomic<float> g_center_x{0.0f}, g_center_y{0.0f};
std::atomic<float> g_last_native_to_visual_deg{0.0f};
std::atomic<float> g_native_dir_x{0.0f}, g_native_dir_y{1.0f}, g_native_dir_z{0.0f};
std::atomic<bool> g_native_dir_valid{false};
std::atomic<float> g_native_reach_m{0.0f};
std::atomic<bool> g_ray_valid{false};

// V66.69 optional hand-owned interaction ray. Main loop publishes a game-world
// origin/direction; native ray builder still owns reach, object masks and actions.
std::atomic<bool> g_hand_ray_active{false};
using RaySample=interaction112::Ray;
core::SnapshotMailbox<RaySample> g_hand_packets112;
core::SnapshotMailbox<palm59c9::Pair> g_palm_packets59c9;
thread_local palm59c9::Ranking g_palm_ranking59c9;
struct NativeRay112 { RaySample ray{}; float ox=0,oy=0,oz=0,reach=0;std::uint64_t tick=0; };
core::SnapshotMailbox<NativeRay112> g_native_packets112;

// Last native player-interactor origin. The on-screen candidate hook has no eye
// pointer argument, so it uses this recent origin solely to measure distance to
// the corrected VR ray. A stale sample is rejected.
std::atomic<float> g_eye_x{0.0f}, g_eye_y{0.0f}, g_eye_z{0.0f};
std::atomic<std::uint64_t> g_eye_tick_ms{0};
std::atomic<std::uint64_t> g_eye_rays_r121{0}; // R121: look rays that started at the rendered eye

bool SafeReadable(const void* p, std::size_t need) noexcept {
    if (!p) return false;
    MEMORY_BASIC_INFORMATION mbi{};
    if (!VirtualQuery(p, &mbi, sizeof(mbi))) return false;
    if (mbi.State != MEM_COMMIT || (mbi.Protect & (PAGE_NOACCESS | PAGE_GUARD))) return false;
    const auto a = reinterpret_cast<std::uintptr_t>(p);
    const auto end = reinterpret_cast<std::uintptr_t>(mbi.BaseAddress) + mbi.RegionSize;
    return a + need <= end;
}

bool ReadWhTimestamp(std::uintptr_t base, std::uint32_t& out) noexcept {
    out = 0;
    if (!base) return false;
    __try {
        const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
        if (dos->e_magic != IMAGE_DOS_SIGNATURE) return false;
        const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS64*>(base + dos->e_lfanew);
        if (nt->Signature != IMAGE_NT_SIGNATURE) return false;
        out = nt->FileHeader.TimeDateStamp;
        return true;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
}

bool MatchEntry(std::uintptr_t address, const unsigned char* bytes, std::size_t n) noexcept {
    if (!SafeReadable(reinterpret_cast<const void*>(address), n)) return false;
    __try {
        return std::memcmp(reinterpret_cast<const void*>(address), bytes, n) == 0;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
}

void InvalidateVisualRay() noexcept {
    const auto s = g_ray_seq.fetch_add(1, std::memory_order_acq_rel) + 1; (void)s;
    g_ray_valid.store(false, std::memory_order_relaxed);
    g_ray_seq.fetch_add(1, std::memory_order_release);
}


// Bounded reads of an already-enumerated native candidate, never a world scan.
// Entity lookup +4B4F98 and CEntity/CEntityClass getters are pinned at install.
static interaction57c1::Kind CandidateKind57c1(std::uintptr_t interactor,std::uint32_t id,std::uintptr_t& player,char* clsOut=nullptr,std::uintptr_t* entityOut=nullptr) noexcept {
    player=0;if(clsOut)clsOut[0]=0;if(entityOut)*entityOut=0;
    __try {
        if(!id||!interactor||*reinterpret_cast<std::uintptr_t*>(interactor)!=g_wh_base+0x3A6A0C8ull)return interaction57c1::Kind::Unknown;
        player=*reinterpret_cast<std::uintptr_t*>(interactor+8);
        if(!player||*reinterpret_cast<std::uintptr_t*>(player)!=g_wh_base+0x3AA4B98ull)return interaction57c1::Kind::Unknown;
        const auto sys=*reinterpret_cast<std::uintptr_t*>(g_wh_base+0x492D8A0ull);
        if(!sys||*reinterpret_cast<std::uintptr_t*>(sys)!=g_wh_base+0x4077048ull)return interaction57c1::Kind::Unknown;
        const auto entity=reinterpret_cast<std::uintptr_t(__fastcall*)(std::uintptr_t,std::uint32_t)>(g_wh_base+0x4B4F98ull)(sys,id);
        if(!entity||*reinterpret_cast<std::uintptr_t*>(entity)!=g_wh_base+0x4072C78ull||*reinterpret_cast<std::uint32_t*>(entity+0xC)!=id)return interaction57c1::Kind::Unknown;
        const auto cls=*reinterpret_cast<std::uintptr_t*>(entity+0x20);
        if(!cls||*reinterpret_cast<std::uintptr_t*>(cls)!=g_wh_base+0x3B282A0ull)return interaction57c1::Kind::Unknown;
        const auto name=*reinterpret_cast<const char**>(cls+0x10);
        if(!name)return interaction57c1::Kind::Unknown;
        char bounded[96]{};unsigned i=0;
        for(;i<sizeof(bounded)-1&&name[i];++i)bounded[i]=name[i];
        if(!i||name[i])return interaction57c1::Kind::Unknown;
        if(clsOut){unsigned k=0;for(;k<31&&bounded[k];++k)clsOut[k]=bounded[k];clsOut[k]=0;}
        if(entityOut)*entityOut=entity;
        return interaction57c1::Classify(bounded);
    } __except(EXCEPTION_EXECUTE_HANDLER){player=0;return interaction57c1::Kind::Unknown;}
}
#include "interaction_native_selection68.inl"
#include "subsystems/interactions/dog_candidate_d59r1.inl"
#include "subsystems/interactions/quick_talk_candidate_d78.inl"
#include "interaction_door_origin_h12.inl"
static char __fastcall CandidateFilterDetour57c1(std::uintptr_t interactor,std::uint32_t id) noexcept {
    const auto caller=reinterpret_cast<std::uintptr_t>(_ReturnAddress());
    const char nativeResult=g_candidate_original57c1(interactor,id); // exclusion/eligibility remains native
    const char result=caller==g_wh_base+kCandidateFilterReturn?
        FilterNativeCandidate68(interactor,id,nativeResult,GetTickCount64()):nativeResult;
    if(caller==g_wh_base+kCandidateFilterReturn){
        DoorDiagPredicate3(dd3::HeldFilter,nativeResult,result);
        g_candidate57c1={};
        if(result&&g_split_ready57c1&&g_enabled.load(std::memory_order_relaxed)&&
           (!g_visual_fallback_enabled.load(std::memory_order_relaxed)||subsystems::alchemy::bridge67::enabled.load(std::memory_order_acquire))){
            g_candidate57c1.kind=CandidateKind57c1(interactor,id,g_candidate57c1.player,g_candidate57c1.cls);
            g_candidate57c1.tick=GetTickCount64();g_candidate57c1.id=id;
        }
    }
    return result;
}
static bool LoadHeadRay57c1(RaySample& out) noexcept {
    float x=0,y=1,z=0;
    if(!CameraHook::GetLatestVisualForwardWorld(x,y,z,100))return false;
    out={};out.x=x;out.y=y;out.z=z;out.tick=GetTickCount64();
    return interaction112::Normalize(out);
}

bool LoadVisualRay(RaySample& out) noexcept {
    RaySample hand{};
    if(g_hand_ray_active.load(std::memory_order_acquire)&&g_hand_packets112.read(hand)&&
       hand.hand&&interaction112::Fresh(hand,GetTickCount64())){out=hand;return true;}
    // V66.156: in VRIK, hand interaction owns the backend and visual fallback is
    // deliberately disabled. Outside VRIK, final OpenXR visual forward is the
    // normal interaction source.
    if(!g_visual_fallback_enabled.load(std::memory_order_acquire)) return false;
    // V64.38 primary source: exact final OpenXR-composed camera forward.
    float fx=0.0f,fy=1.0f,fz=0.0f;
    if(CameraHook::GetLatestVisualForwardWorld(fx,fy,fz,100)) {
        out.x=fx; out.y=fy; out.z=fz; out.tan_x=0.0f; out.tan_y=0.0f;
        return true;
    }
    // Legacy publisher is retained only as a fail-closed fallback.
    for (int i = 0; i < 4; ++i) {
        const auto a = g_ray_seq.load(std::memory_order_acquire);
        if (!a || (a & 1ull)) continue;
        const bool valid = g_ray_valid.load(std::memory_order_relaxed);
        RaySample local{};
        local.x = g_dir_x.load(std::memory_order_relaxed);
        local.y = g_dir_y.load(std::memory_order_relaxed);
        local.z = g_dir_z.load(std::memory_order_relaxed);
        local.tan_x = g_src_tan_x.load(std::memory_order_relaxed);
        local.tan_y = g_src_tan_y.load(std::memory_order_relaxed);
        std::atomic_thread_fence(std::memory_order_acquire);
        if (g_ray_seq.load(std::memory_order_relaxed) != a) continue;
        if (!valid) return false;
        out = local;
        return true;
    }
    return false;
}

void MaybeLog() noexcept {
    const auto now = static_cast<std::uint64_t>(GetTickCount64());
    auto prev = g_last_log_ms.load(std::memory_order_relaxed);
    if (now - prev < 2000ull || !g_last_log_ms.compare_exchange_strong(
            prev, now, std::memory_order_relaxed)) return;
    RaySample ray{};
    const bool valid = LoadVisualRay(ray);
    char msg[640]{};
    std::snprintf(msg, sizeof(msg),
        "V64.3-INTERACT enabled=%d valid=%d builds=%llu playerCalls=%llu redirects=%llu screenChecks=%llu screenForced=%llu dir=(%.4f,%.4f,%.4f) sourceTan=(%.4f,%.4f) center=(%+.4f,%+.4f) nativeToVisual=%.2fdeg reach=%.3f baseRadius=%.3f coneHalf=%.2fdeg split=%d headCandidates=%llu handCandidates=%llu unknown=%llu eyeRayR121=%d/%llu",
        g_enabled.load(std::memory_order_relaxed) ? 1 : 0, valid ? 1 : 0,
        (unsigned long long)g_ray_builds.load(std::memory_order_relaxed),
        (unsigned long long)g_interaction_calls.load(std::memory_order_relaxed),
        (unsigned long long)g_redirects.load(std::memory_order_relaxed),
        (unsigned long long)g_onscreen_checks.load(std::memory_order_relaxed),
        (unsigned long long)g_onscreen_forced.load(std::memory_order_relaxed),
        double(ray.x), double(ray.y), double(ray.z), double(ray.tan_x), double(ray.tan_y),
        double(g_center_x.load(std::memory_order_relaxed)), double(g_center_y.load(std::memory_order_relaxed)),
        double(g_last_native_to_visual_deg.load(std::memory_order_relaxed)),
        double(g_native_reach_m.load(std::memory_order_relaxed)),
        double(g_radius_m.load(std::memory_order_relaxed)),
        double(g_cone_half_angle_deg.load(std::memory_order_relaxed)),g_split_ready57c1?1:0,
        (unsigned long long)g_head_candidates57c1.load(),(unsigned long long)g_hand_candidates57c1.load(),(unsigned long long)g_unknown_candidates57c1.load(),
        kcd2vr::eye_ray_r121::enabled.load(std::memory_order_relaxed)?1:0,(unsigned long long)g_eye_rays_r121.load(std::memory_order_relaxed));
    diagnostic_trace::Write(msg);
}

// R121 (vrik/eye_ray_r121.h): the head look ray starts at the RENDERED eye with the rendered forward, like R118 at
// the alchemy table, whenever the rendered eye is within 0.6 m of the game eye (your lean/step from the recenter
// point). before, only the direction was the headset's: after a station the headset stood 27 cm forward of the
// recenter point (R118 log 11:03:41) and the ray ran 25-30 cm beside your view line until you recentered.
// false = switch OFF, no current rendered ray (recenter, menu), or a camera that is not your eye -> game eye.
static bool EyeRayR121(const float* native,float& ox,float& oy,float& oz,float& dx,float& dy,float& dz) noexcept {
    if(!kcd2vr::eye_ray_r121::enabled.load(std::memory_order_acquire))return false;
    visual_ray_d75::Sample ray{};
    if(!CameraHook::GetLatestVisualRayD75(ray,80))return false;
    if(!kcd2vr::eye_ray_r121::useRenderedEye(true,native,ray.origin))return false;
    ox=ray.origin[0];oy=ray.origin[1];oz=ray.origin[2];dx=ray.forward[0];dy=ray.forward[1];dz=ray.forward[2];
    g_eye_rays_r121.fetch_add(1,std::memory_order_relaxed);
    return true;
}

std::uintptr_t __fastcall RayBuildDetour(std::uintptr_t out, std::uintptr_t origin,
    std::uintptr_t dir, int objtypes, int flags, std::uintptr_t skip_ents,
    unsigned __int8 n, char mode) noexcept {
    g_ray_builds.fetch_add(1, std::memory_order_relaxed);
    const auto ra = reinterpret_cast<std::uintptr_t>(_ReturnAddress());
    if (ra >= g_lookray_lo && ra < g_lookray_hi) {
        g_interaction_calls.fetch_add(1, std::memory_order_relaxed);
        RaySample ray{};
        const bool split57c1=g_split_ready57c1&&!g_visual_fallback_enabled.load(std::memory_order_relaxed);
        // This ONE native ray is shared by object, dog and chat policies. Its
        // long range must not be shrunk to the hand's two-metre prompt cap.
        // Keep native eye/LOS and use a separate hand projection below.
        const bool rayReady57c1=split57c1?LoadHeadRay57c1(ray):LoadVisualRay(ray);
        if (g_enabled.load(std::memory_order_relaxed) && origin && dir && rayReady57c1) {
            bool redirected = false;
            __try {
                auto* o = reinterpret_cast<float*>(origin);
                auto* d = reinterpret_cast<float*>(dir);
                const float old_x=d[0], old_y=d[1], old_z=d[2];
                const float native_reach = std::sqrt(old_x*old_x + old_y*old_y + old_z*old_z);
                if (native_reach > 1e-3f && native_reach < 100.0f) {
                    const bool scopedHand=ray.hand;
                    const float handLimit=ray.limit;
                    const float reach=physical_interaction106::EffectiveReach(native_reach,scopedHand,handLimit);
                    // V64.38: preserve KCD2's native eye origin and exact reach,
                    // but point the interaction segment along the FINAL visible HMD
                    // world-forward. This decouples pickup/use from native camera timing.
                    const bool hand=ray.hand;
                    float useOx=hand?ray.ox:o[0],useOy=hand?ray.oy:o[1],useOz=hand?ray.oz:o[2];
                    float useDx=ray.x,useDy=ray.y,useDz=ray.z;
                    // R118: at the alchemy table (6DOF bridge, VrAlchemyViewSelectR116) the head look ray
                    // starts at the RENDERED eye with the rendered forward, not at the station camera.
                    // the rendered eye = station camera + your head movement (C38 hmdDeltaM up to
                    // 0.22 m sideways in R117, 09:47:30), so a ray from the camera in your view direction
                    // hit to the LEFT of what you saw when you leaned right to the rightmost herb.
                    const bool eyeR118=!hand&&AlchemyEyeRayR118(useOx,useOy,useOz,useDx,useDy,useDz);
                    // R121: everywhere else the head ray starts at the rendered eye too (EyeRayR121 above).
                    const bool eyeR121=!hand&&!eyeR118&&EyeRayR121(o,useOx,useOy,useOz,useDx,useDy,useDz);
                    if(hand||eyeR118||eyeR121){o[0]=useOx;o[1]=useOy;o[2]=useOz;}
                    if(eyeR121){ray.x=useDx;ray.y=useDy;ray.z=useDz;} // the head cone (OnScreenDetour) uses the same eye ray
                    ray.ox=useOx;ray.oy=useOy;ray.oz=useOz;ray.tick=GetTickCount64();
                    NativeRay112 native{ray,useOx,useOy,useOz,reach,ray.tick};
                    (void)g_native_packets112.publish(native);
                    g_eye_x.store(useOx, std::memory_order_relaxed);
                    g_eye_y.store(useOy, std::memory_order_relaxed);
                    g_eye_z.store(useOz, std::memory_order_relaxed);
                    g_native_reach_m.store(reach, std::memory_order_relaxed);
                    g_eye_tick_ms.store(static_cast<std::uint64_t>(GetTickCount64()), std::memory_order_release);
                    const float inv_native_reach=1.0f/native_reach;
                    const float nx=old_x*inv_native_reach, ny=old_y*inv_native_reach, nz=old_z*inv_native_reach;
                    g_native_dir_x.store(nx,std::memory_order_relaxed);
                    g_native_dir_y.store(ny,std::memory_order_relaxed);
                    g_native_dir_z.store(nz,std::memory_order_relaxed);
                    g_native_dir_valid.store(true,std::memory_order_release);
                    const float dot=std::clamp(nx*useDx + ny*useDy + nz*useDz,-1.0f,1.0f);
                    g_last_native_to_visual_deg.store(std::acos(dot)*57.29577951308232f,std::memory_order_relaxed);
                    d[0] = useDx * reach;
                    d[1] = useDy * reach;
                    d[2] = useDz * reach;
                    redirected = true;
                }
            } __except (EXCEPTION_EXECUTE_HANDLER) {
                redirected = false;
            }
            if (redirected) g_redirects.fetch_add(1, std::memory_order_relaxed);
        }
    }
    MaybeLog();
    return g_ray_original(out, origin, dir, objtypes, flags, skip_ents, n, mode);
}

// The projection hook observes offered candidates, not the native final action.
// ActionRequest/ActionReturn in F8_LIFECYCLE supplies that final target identity.
static void SelectionReceipt59c10(std::uint64_t now,std::uint32_t target,int side,
    int reason,float distance=0,float score=0,std::uint32_t locked=0) noexcept {
    if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,now)||!target)return;
    using namespace diagnostics::interaction_receipt59c10;
    static thread_local Gate receipt;
    const auto session=diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Interaction)].load();
    if(!receipt.observe(session,now,{0,target,side,reason},250,768))return;
    static constexpr const char* reasons[]{"NATIVE_ACTIVITY_FALLBACK","NATIVE_RAY_STALE","POLICY_UNKNOWN",
        "HAND_RAY_STALE","SOURCE_OWNER_CHANGED","PALM_OUTSIDE_OR_STALE","GRIP_TARGET_LOCK",
        "OFFERED","LOWER_RANKED_CANDIDATE","OUTSIDE_CONE_OR_REACH","HEAD_POLICY",
        "DIRECT_NATIVE_DOOR_OFFERED","DIRECT_NATIVE_DOOR_NO_FRESH_PALM_WITNESS"};
    char line[480]{};std::snprintf(line,sizeof(line),
        "C10 F5_SELECTION session=%llu target=%u hand=%d reason=%s lockedTarget=%u distance=%.3f score=%.4f observed=%llu changes=%llu coalesced=%llu scope=CANDIDATE_NOT_FINAL_ACTION",
        (unsigned long long)session,target,side,reasons[reason],locked,double(distance),double(score),
        (unsigned long long)receipt.seen,(unsigned long long)receipt.changes,(unsigned long long)receipt.coalesced);
    diagnostic_trace::Write(line);
}
// MSVC C2712: the mutex guard (object unwinding) must not share a function with __try.
static void PublishHandCandidate178r16(std::uint64_t now,const float* c,const RaySample& ray,bool object,std::uint32_t entity,const char* cls,std::uint64_t generation) noexcept {
    if(!c)return;
    std::lock_guard<std::mutex> lk(g_hand_candidate_mutex178r14);
    // The offered candidate carries the EXACT admitted palm ray, rather than
    // optional door-only provenance that generic tool admission could never see.
    // Source revocation during the projection cannot repopulate an old squeeze.
    if(object&&(!generation||generation!=g_door_generation38||!g_enabled.load(std::memory_order_acquire)||
       !g_split_ready57c1.load(std::memory_order_acquire)||!HandRayActive()||
       g_visual_fallback_enabled.load(std::memory_order_acquire)||!ray.hand||ray.owner<0||ray.owner>1||
       !interaction112::Fresh(ray,now)))return;
    g_hand_candidate178r14={now,c[0],c[1],c[2],c[3],ray.owner,object,entity};
    // Preserve ordinary native candidate/hover behavior when a committed palm
    // has no optional player annotation. It remains UNKNOWN for Guided input.
    if(object&&ray.sourceEpoch&&ray.sourcePlayer){g_hand_candidate178r14.sourceGeneration=generation;g_hand_candidate178r14.sourceEpoch=ray.sourceEpoch;
        g_hand_candidate178r14.sourcePlayer=ray.sourcePlayer;g_hand_candidate178r14.sourceTick=ray.tick;}
    if(cls){unsigned k=0;for(;k<31&&cls[k];++k)g_hand_candidate178r14.cls[k]=cls[k];g_hand_candidate178r14.cls[k]=0;}
}
static void DoorCandidateObservation38(const Candidate57c1& native,const float* point,const RaySample& ray,
                                       std::uint64_t now,bool inside,std::uint64_t generation) noexcept {
    if(!native.id||std::strcmp(native.cls,"AnimDoor"))return;
    std::lock_guard<std::mutex> lk(g_hand_candidate_mutex178r14);
    // A callback already in flight when its source was revoked cannot repopulate
    // the cache after a disable/re-enable cycle, even inside the 80 ms budget.
    if(!generation||generation!=g_door_generation38||!g_enabled.load(std::memory_order_acquire)||
       !g_split_ready57c1.load(std::memory_order_acquire)||!HandRayActive()||g_visual_fallback_enabled.load(std::memory_order_acquire))return;
    // A new observation supersedes the previous geometry for this exact door,
    // including an explicit outside-cone result or changed hand ownership.
    g_door_candidates38.withdraw(native.id);
    if(!inside||!point||!ray.sourceEpoch||!ray.sourcePlayer)return;
    HandCandidate value{now,point[0],point[1],point[2],point[3],ray.owner,true,native.id};
    std::memcpy(value.cls,native.cls,sizeof(value.cls));value.sourceGeneration=generation;
    value.sourceEpoch=ray.sourceEpoch;value.sourcePlayer=ray.sourcePlayer;value.sourceTick=ray.tick;
    g_door_candidates38.record(value);
}
#include "interaction_direct_door54r1.inl"
char __fastcall OnScreenDetour(std::uintptr_t a1, std::uintptr_t world_point,
    float* out_screen, std::uintptr_t a4) noexcept {
    const auto caller=reinterpret_cast<std::uintptr_t>(_ReturnAddress());
    g_onscreen_checks.fetch_add(1, std::memory_order_relaxed);
    const auto doorGeneration38=DoorSourceGeneration38();
    const auto candidate=g_candidate57c1;g_candidate57c1={}; // same-thread, single-use metadata
    if (!g_enabled.load(std::memory_order_relaxed) || !world_point || !out_screen)
        return g_onscreen_original(a1, world_point, out_screen, a4);
    // G1R1: bounds were copied at the original LocalBounds callback BEFORE the
    // head-facing/projection rejection. Never replace that geometry with this
    // gaze-relative world_point. Only the same scoped native table nominee may
    // pass projection; native final selection/verb remain independently needed.
    if(candidate.id&&caller==g_wh_base+kScreenReturn&&
       g_selection_ready68.load(std::memory_order_acquire)&&
       subsystems::alchemy::bridge67::enabled.load(std::memory_order_acquire)){
        bool copied67=false;
        __try {
            const auto now=std::uint64_t(GetTickCount64());
            copied67=a4&&*reinterpret_cast<const std::uintptr_t*>(a4)==g_wh_base+kObjectPolicy&&
                *reinterpret_cast<const std::uintptr_t*>(a4+8)==candidate.player&&candidate.tick&&now>=candidate.tick&&now-candidate.tick<=80;}
        __except(EXCEPTION_EXECUTE_HANDLER){copied67=false;}
        if(copied67){
            const auto now67=std::uint64_t(GetTickCount64());
            selection68::Table table68{};std::uint32_t nominated68=0;
            if(SelectionScopeCurrent68(table68,now67)&&table68.player==candidate.player&&
               selection68::registered(table68,candidate.id)&&a4==g_selection_scope68.policy){
                if(craft68::selectionNear68(table68,nominated68,now67)){
                    if(candidate.id!=nominated68)return 0;
                    out_screen[0]=out_screen[1]=50.f;return 1;
                }
                // R116: no hand owns the station selection -> the headset view decides. Before, a
                // station item outside the 10 deg head cone fell through to the NATIVE camera's
                // screen test below (authored station camera, 16-65 deg off your head in R114),
                // so you had to look ~30 cm beside an item to get it. Looking at no item = none.
                std::uint32_t head116=0;
                if(HeadTargetR116(table68,now67,head116)){
                    if(candidate.id!=head116)return 0;
                    out_screen[0]=out_screen[1]=50.f;return 1;
                }
            }
        }
    }

    // No controller ray: use the ORIGINAL activity selector, not a stale hand cone
    // or an invented head ray. Scoped by the live Alchemy witness in mod.cpp.
    if(g_native_activity_fallback5916c3.load(std::memory_order_acquire)&&!HandRayActive()){
        SelectionReceipt59c10(GetTickCount64(),candidate.id,-1,0);
        return g_onscreen_original(a1,world_point,out_screen,a4);}

    NativeRay112 native{};
    const auto now=static_cast<std::uint64_t>(GetTickCount64());
    if(!g_native_packets112.read(native)||!native.tick||now<native.tick||now-native.tick>100){
        SelectionReceipt59c10(now,candidate.id,-1,1);
        return g_onscreen_original(a1,world_point,out_screen,a4);}

    const bool split=g_split_ready57c1&&!g_visual_fallback_enabled.load(std::memory_order_relaxed);
    RaySample ray=native.ray;float reach=native.reach;
    bool known=false,head=false,chatPolicyD78=false;
    if(split){
        // a4 is the policy object, not a target entity (retail +640348).
        __try {
            const auto vf=a4?*reinterpret_cast<std::uintptr_t*>(a4):0;
            const bool paired=caller==g_wh_base+kScreenReturn&&candidate.tick&&now>=candidate.tick&&now-candidate.tick<=100&&
                candidate.player&&a4&&*reinterpret_cast<std::uintptr_t*>(a4+8)==candidate.player;
            if(paired&&(vf==g_wh_base+kDogPolicy||vf==g_wh_base+kChatPolicy)){known=true;head=true;chatPolicyD78=(vf==g_wh_base+kChatPolicy);}
            else if(paired&&vf==g_wh_base+kObjectPolicy&&candidate.kind!=interaction57c1::Kind::Unknown){known=true;head=candidate.kind==interaction57c1::Kind::Actor;}
        } __except(EXCEPTION_EXECUTE_HANDLER){known=false;}
        if(!known){g_unknown_candidates57c1.fetch_add(1,std::memory_order_relaxed);SelectionReceipt59c10(now,candidate.id,-1,2);return g_onscreen_original(a1,world_point,out_screen,a4);}
        // Keep the exact native forge trigger projection and all native Use checks.
        // Its world-texture page is not a hand pickup or a UI focus grant.
        if(candidate.kind==interaction57c1::Kind::NativeContext)
            return g_onscreen_original(a1,world_point,out_screen,a4);
        if(head){
            // Observe only a qualified native Dog point for the separate pet
            // gesture channel. Native actor/head projection below is unchanged.
            DogCandidateObservationD59R1(candidate,world_point,now);
            g_head_candidates57c1.fetch_add(1,std::memory_order_relaxed);
            // Preserve the native policy's dog/day/night/chat/horse eligibility
            // and distance checks after this projection, without a 2 m cap.
        }else{
            g_hand_candidates57c1.fetch_add(1,std::memory_order_relaxed);
            if(!g_hand_ray_active.load(std::memory_order_acquire)){
                SelectionReceipt59c10(now,candidate.id,-1,3);
                return g_onscreen_original(a1,world_point,out_screen,a4); // H11: no hand owner = native gaze selection
            }
            if(!g_hand_packets112.read(ray)||!ray.hand||!interaction112::Fresh(ray,now)){
                SelectionReceipt59c10(now,candidate.id,-1,3);
                return g_native_activity_fallback5916c3.load(std::memory_order_acquire)?
                    g_onscreen_original(a1,world_point,out_screen,a4):0;}

            reach=physical_interaction106::EffectiveReach(native.reach,true,ray.limit);
        }
    }else if(native.ray.hand!=HandRayActive()){
        SelectionReceipt59c10(now,candidate.id,-1,4);return g_onscreen_original(a1,world_point,out_screen,a4);}
    bool force=false;float point[4]{};float score59c9=0;bool palm59=false;
    int refusal59c10=9;std::uint32_t locked59c10=0;

    __try {
        const auto* p=reinterpret_cast<const float*>(world_point);
        const float radius=physical_interaction106::EffectiveRadius(g_radius_m.load(std::memory_order_relaxed),ray.hand,ray.radius);
        const float angle=ray.hand?interaction57c1::HandHalfAngle:g_cone_half_angle_deg.load(std::memory_order_relaxed);
        auto projection=interaction57c1::Project(ray,p[0],p[1],p[2],reach,radius,angle);
        palm59c9::Pair palms{};
        if(ray.hand&&split&&known&&!head&&g_palm_packets59c9.read(palms)){
            const auto selected=palm59c9::choose(palms,p[0],p[1],p[2],now,ray.owner);
            // A published pair is authoritative. Invalid/stale hands do not
            // inherit the old single controller-aim cone.
            projection.inside=palm59c9::accepts(palms,candidate.id,selected.side);
            locked59c10=palms.lockedEntity;
            refusal59c10=selected.side<0?5:6;
            if(projection.inside){ray=palms.ray[selected.side];projection.distance=selected.distance;palm59=true;
                score59c9=g_palm_ranking59c9.evaluate(g_interaction_calls.load(std::memory_order_relaxed),now,candidate.id,selected.score);
                projection.x=50.f+10.f*score59c9;projection.y=50.f;}
        }
        if(projection.inside){
            out_screen[0]=projection.x;out_screen[1]=projection.y;force=true;
            point[0]=p[0];point[1]=p[1];point[2]=p[2];point[3]=projection.distance;
        }
    } __except(EXCEPTION_EXECUTE_HANDLER){force=false;}
    // Retain the exact ordinary-door geometry independently of nearest-hover
    // ranking. This does not select the door or grant Use; native acceptance is
    // still required before ExactDoorCandidate38 can arm a held-door request.
    if(split&&known&&head&&chatPolicyD78)ObserveQuickTalkCandidateD78(candidate,force,now);
    if(split&&known&&!head)DoorCandidateObservation38(candidate,point,ray,now,force&&ray.hand,doorGeneration38);
    if(force){
        if(ray.hand){
            const bool offered=!palm59||g_palm_ranking59c9.offer(now,candidate.id,score59c9);
            if(offered)PublishHandCandidate178r16(now,point,ray,split&&known&&!head,candidate.id,candidate.cls,doorGeneration38);
            SelectionReceipt59c10(now,candidate.id,ray.owner,offered?7:8,point[3],score59c9,locked59c10);
        }else SelectionReceipt59c10(now,candidate.id,-1,10,point[3]);
        g_onscreen_forced.fetch_add(1,std::memory_order_relaxed);return 1;
    }
    SelectionReceipt59c10(now,candidate.id,ray.owner,refusal59c10,point[3],score59c9,locked59c10);
    // A head projection must not resurrect an object outside the hand cone.
    return split?0:g_onscreen_original(a1,world_point,out_screen,a4);
}

} // namespace

#include "interaction_alchemy_names_d75.inl"

void SetDoorDiagnosticsCaptureC54R3(bool active) noexcept {
 if(!active){const bool was=g_door_diag_ready3.load(std::memory_order_acquire);DoorDiagDisable3();
  if(was){char line[340]{};std::snprintf(line,sizeof(line),"C54R3 F5_DOOR_CAPTURE_END callsEnteredProcess=%llu completedJoinedProcess=%llu completedAtCaptureBoundaryProcess=%llu observerPatches=DISABLED_OR_PASSIVE handles=PROCESS_RETAINED noGameplayWrites=1",
   (unsigned long long)g_door_diag_entered3.load(),(unsigned long long)g_door_diag_completed3.load(),(unsigned long long)g_door_diag_boundary3.load());diagnostic_trace::Write(line);}return;}
 const bool ready=DoorDiagEnable3();unsigned readyMask=0;
 for(unsigned i=0;i<sizeof(kDoorDiagPins3)/sizeof(kDoorDiagPins3[0]);++i){try{if(g_door_diag_hooks3[i]&&g_door_diag_hooks3[i]->is_enabled())readyMask|=1u<<i;}catch(...){}}
 DoorDiagReportReady3(ready,readyMask);
}

bool Install() {
    if (g_installed.load(std::memory_order_acquire)) return true;
    const auto base = reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll"));
    std::uint32_t ts = 0;
    if (!base || !retail_build112::Verify(base) || !ReadWhTimestamp(base, ts) || ts != kSupportedWhTimestamp) {
        char m[192]{};
        std::snprintf(m, sizeof(m), "V64.3-INTERACT install refused WHGame timestamp=%08X expected=%08X", ts, kSupportedWhTimestamp);
        debug_trace::Write(m);
        return false;
    }

    static constexpr unsigned char ray_head[] = {0xF2,0x0F,0x10,0x02,0x4C,0x8B,0xD1,0x4C,0x8B,0x5C,0x24,0x30,0xF2,0x0F,0x11,0x01};
    static constexpr unsigned char look_head[] = {0x48,0x8B,0xC4,0x48,0x89,0x58,0x10,0x48,0x89,0x70,0x18,0x55,0x57,0x41,0x54,0x41,0x56,0x41,0x57,0x48,0x8D,0xA8,0x48,0xF8};
    static constexpr unsigned char screen_head[] = {0x4C,0x8B,0xDC,0x49,0x89,0x5B,0x10,0x49,0x89,0x73,0x18,0x49,0x89,0x4B,0x08,0x57,0x48,0x83,0xEC,0x70};
    if (!MatchEntry(base + kInteractionRayBuildRva, ray_head, sizeof(ray_head)) ||
        !MatchEntry(base + kInteractorLookRayRva, look_head, sizeof(look_head))) {
        diagnostic_trace::Write("V64.3-INTERACT install refused: retail ray/look entry bytes do not match");
        return false;
    }

    g_wh_base=base;
    PrepareNativeNomineeLos68(base); // optional packet pins precede our existing ray hook
    static constexpr unsigned char candidateHead[]{0x48,0x89,0x5C,0x24,0x08,0x48,0x89,0x6C,0x24,0x10,0x48,0x89,0x74,0x24,0x18};
    static constexpr unsigned char candidateCall[]{0xE8,0xD4,0xCB,0x1E,0x00,0x84,0xC0};
    static constexpr unsigned char screenCall[]{0xE8,0x9D,0x52,0x2F,0x00,0x84,0xC0};
    static constexpr unsigned char classGetter[]{0x48,0x8B,0x41,0x20,0xC3};
    static constexpr unsigned char nameGetter[]{0x48,0x8B,0x41,0x10,0xC3};
    const std::uintptr_t pins[][2]{{0x4077048+0x70,0x4B4F98},{0x4072C78+0x18,0x19A2A90},
        {0x3B282A0+0x10,0x638480},{kObjectPolicy+0x38,0x82CDBC},{kDogPolicy+0x38,0x535534},
        {kChatPolicy+0x38,0x82CDB0},{kObjectPolicy+0x58,0x9CBBA4},{kDogPolicy+0x58,0x82A89C},{kChatPolicy+0x58,0x82A89C}};
    bool splitPins=MatchEntry(base+kCandidateFilterRva,candidateHead,sizeof(candidateHead))&&
        MatchEntry(base+0x64033B,candidateCall,sizeof(candidateCall))&&MatchEntry(base+0x64035E,screenCall,sizeof(screenCall))&&
        MatchEntry(base+0x19A2A90,classGetter,sizeof(classGetter))&&MatchEntry(base+0x638480,nameGetter,sizeof(nameGetter));
    for(const auto& pin:pins){const auto value=base+pin[1];splitPins=splitPins&&MatchEntry(base+pin[0],reinterpret_cast<const unsigned char*>(&value),sizeof(value));}
    g_lookray_lo = base + kInteractorLookRayRva;
    g_lookray_hi = g_lookray_lo + kInteractorLookRaySpan;

    DetourModKit::hook::InlineRequest req{
        "kcd2vr_v641r_interaction_ray",
        DetourModKit::hook::Target{DetourModKit::Address{base + kInteractionRayBuildRva}},
        DetourModKit::hook::Options{}};
    auto result = DetourModKit::hook::inline_at(std::move(req), &RayBuildDetour);
    if (!result) {
        diagnostic_trace::Write("V64.3-INTERACT player look-ray hook install failed");
        return false;
    }
    g_ray_hook = std::move(*result);
    g_ray_original = g_ray_hook->original<RayQueryBuildFn>();
    if (!g_ray_hook->enable()) {
        g_ray_original = nullptr;
        g_ray_hook.reset();
        diagnostic_trace::Write("V64.3-INTERACT player look-ray hook enable failed");
        return false;
    }

    // Companion candidate projection gate is best-effort. Entity/loot look-ray
    // alignment stays useful even if this entry is unavailable.
    if (MatchEntry(base + kInteractionOnScreenRva, screen_head, sizeof(screen_head))) {
        DetourModKit::hook::InlineRequest sreq{
            "kcd2vr_v641r_interaction_onscreen",
            DetourModKit::hook::Target{DetourModKit::Address{base + kInteractionOnScreenRva}},
            DetourModKit::hook::Options{}};
        auto sr = DetourModKit::hook::inline_at(std::move(sreq), &OnScreenDetour);
        if (sr) {
            g_onscreen_hook = std::move(*sr);
            g_onscreen_original = g_onscreen_hook->original<OnScreenCheckFn>();
            if (!g_onscreen_hook->enable()) {
                g_onscreen_original = nullptr;
                g_onscreen_hook.reset();
                diagnostic_trace::Write("V64.3-INTERACT on-screen gate enable failed; look-ray remains active");
            }
        } else {
            diagnostic_trace::Write("V64.3-INTERACT on-screen gate install failed; look-ray remains active");
        }
    } else {
        diagnostic_trace::Write("V64.3-INTERACT on-screen gate entry mismatch; look-ray remains active");
    }

    if(splitPins&&g_onscreen_hook){
        DetourModKit::hook::InlineRequest creq{"kcd2vr_interaction_candidate57c1",DetourModKit::hook::Target{DetourModKit::Address{base+kCandidateFilterRva}},DetourModKit::hook::Options{}};
        auto cr=DetourModKit::hook::inline_at(std::move(creq),&CandidateFilterDetour57c1);
        if(cr){g_candidate_hook57c1=std::move(*cr);g_candidate_original57c1=g_candidate_hook57c1->original<CandidateFilterFn>();
            g_split_ready57c1=bool(g_candidate_hook57c1->enable());
            if(!g_split_ready57c1){g_candidate_hook57c1.reset();g_candidate_original57c1=nullptr;}}
    }
    diagnostic_trace::Write(g_split_ready57c1?"V66.178R57C1 INTERACT split=READY objects=HAND_2M_18DEG actors=HEAD_NATIVE_RANGE sharedLOS=HEAD nativeCandidateEnumeration=UNCHANGED":"V66.178R57C1 INTERACT split=UNAVAILABLE legacy projection retained; no guessed category routing");
    diagnostic_trace::Write(g_split_ready57c1&&InstallNativeSelection68(base)?
        "G1R1 ALCHEMY_NEAR68 source=NATIVE_LOCAL_BOUNDS scope=HENRY_OBJECT_POLICY facing=ONE_CLOSE_PALM nativeEligibility=UNCHANGED":
        "G1R1 ALCHEMY_NEAR68 observer=UNAVAILABLE nativeGaze=UNCHANGED");
    // Exact completed direct-candidate seam. Native policy +60 already ran,
    // +7F3F04 accepted the descriptor, and RDX names the pending native record.
    // +6404E5 appends it unchanged after this read-only observer returns.
    static constexpr unsigned char directEmit54r1[]{0x48,0x8D,0x8D,0xB0,0x00,0x00,0x00,0xE8,0x2E,0x3A,0x1B,0x00,
        0x84,0xC0,0x74,0x10,0x48,0x8D,0x95,0xA0,0x00,0x00,0x00,0x48,0x8D,0x4D,0xA0,0xE8,0xC6,0x42,0x89,0x01};
    static constexpr unsigned char directPolicy54r1[]{0x49,0x8B,0x04,0x24,0x48,0x8B,0xD6,0x49,0x8B,0xCC,0xFF,0x50,0x60,
        0x84,0xC0,0x0F,0x84,0x85,0x03,0x00,0x00};
    static constexpr unsigned char directOwner54r1[]{0x4C,0x8B,0x20,0x49,0x8B,0xCC,0x49,0x8B,0x04,0x24,0xFF,0x50,0x38,
        0x49,0x8B,0x47,0x08,0x48,0x8D,0x95,0x80,0x00,0x00,0x00,0x44,0x0F,0x28,0xC8,0x48,0x8B,0x58,0x38};
    static constexpr unsigned char directDescriptor54r1[]{0x8B,0xD0,0x48,0x8D,0x4D,0x48,0xE8,0x0E,0x07,0x00,0x00};
    static constexpr unsigned char directCopy54r1[]{0xF3,0x0F,0x11,0x55,0x64,0x0F,0x10,0x55,0x48,0x0F,0x10,0x5D,0x58,
        0xF3,0x0F,0x11,0x6D,0x68,0xF2,0x0F,0x10,0x4D,0x68,0x0F,0x11,0x95,0xB0,0x00,0x00,0x00,
        0x0F,0x11,0x9D,0xC0,0x00,0x00,0x00,0x0F,0x11,0x85,0xA0,0x00,0x00,0x00,0xF2,0x0F,0x11,0x8D,0xD0,0x00,0x00,0x00};
    const auto directPolicyFn54r1=base+0x8AC3C8;
    if(!g_direct_door_retired54r1.load(std::memory_order_acquire)&&g_split_ready57c1&&
       MatchEntry(base+0x6404CA,directEmit54r1,sizeof(directEmit54r1))&&
       MatchEntry(base+0x640124,directPolicy54r1,sizeof(directPolicy54r1))&&
       MatchEntry(base+0x63FA3A,directOwner54r1,sizeof(directOwner54r1))&&
       MatchEntry(base+0x63FED7,directDescriptor54r1,sizeof(directDescriptor54r1))&&
       MatchEntry(base+0x640235,directCopy54r1,sizeof(directCopy54r1))&&
       MatchEntry(base+kObjectPolicy+0x60,reinterpret_cast<const unsigned char*>(&directPolicyFn54r1),sizeof(directPolicyFn54r1))){
        // A retained trampoline also needs its callback module to stay mapped.
        // Pin here rather than depending on optional render/menu features.
        HMODULE directModule54r1=nullptr;
        if(GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_PIN,
            reinterpret_cast<LPCWSTR>(&NativeDirectDoor54r1),&directModule54r1)&&directModule54r1){
            using namespace DetourModKit::hook;Options directOptions54r1{};directOptions54r1.fail_if_already_hooked=true;
            auto direct54r1=mid_at(MidRequest{"kcd2vr_direct_door54r1",Target{DetourModKit::Address{base+0x6404E1}},directOptions54r1},NativeDirectDoor54r1);
            if(direct54r1){g_direct_door_hook54r1=std::move(*direct54r1);
                if(!g_direct_door_hook54r1->enable()||!g_direct_door_hook54r1->is_enabled())StopDirectDoor54r1();}
        }
    }
    diagnostic_trace::Write(g_direct_door_hook54r1&&!g_direct_door_retired54r1.load(std::memory_order_acquire)&&g_direct_door_hook54r1->is_enabled()?
        "C54R1 DOOR_DIRECT bridge=READY source=NATIVE_DIRECT_CANDIDATE actionAcceptance=STILL_REQUIRED":
        "C54R1 DOOR_DIRECT bridge=UNAVAILABLE nativeControls=UNCHANGED");
    diagnostic_trace::Write(InstallDoorPointH12(base)?
        "H12 DOOR_PALM_ORIGIN ready=1 boundary=640117_FBB860 nativePointBox=ORIGINAL_ONCE outsideAndBroadphase=UNCHANGED":
        "H12 DOOR_PALM_ORIGIN ready=0 nativeControls=UNCHANGED");
    g_installed.store(true, std::memory_order_release);
    diagnostic_trace::Write("V64.38-INTERACT installed: native eye/reach preserved; direction follows final OpenXR visual forward; on-screen gate follows same HMD ray");
    return true;
}

void Uninstall() {
    SetDoorDiagnosticsCaptureC54R3(false);
    StopDoorPointH12();
    (void)StopNativeSelection68();
    {std::lock_guard<std::mutex> lk(g_hand_candidate_mutex178r14);
     g_split_ready57c1=false;g_native_activity_fallback5916c3=false;++g_door_generation38;++g_door_acquisition_generation65;g_door_candidates38.clear();}
    StopDirectDoor54r1();
    if(g_candidate_hook57c1){(void)g_candidate_hook57c1->disable();g_candidate_hook57c1.reset();}g_candidate_original57c1=nullptr;
    if (g_onscreen_hook) { (void)g_onscreen_hook->disable(); g_onscreen_hook.reset(); }
    if (g_ray_hook) { (void)g_ray_hook->disable(); g_ray_hook.reset(); }
    g_onscreen_original = nullptr;
    g_ray_original = nullptr;
    g_installed.store(false, std::memory_order_release);
    g_hand_ray_active.store(false,std::memory_order_release);
    InvalidateVisualRay();
}

void SetEnabled(bool enabled) noexcept {
    {std::lock_guard<std::mutex> lk(g_hand_candidate_mutex178r14);
     if(g_enabled.exchange(enabled,std::memory_order_acq_rel)!=enabled){++g_door_generation38;++g_door_acquisition_generation65;g_door_candidates38.clear();}
     if(!enabled)g_hand_ray_active.store(false,std::memory_order_release);}
    if(!enabled)InvalidateVisualRay();
}
void SetNativeActivityFallback5916c3(bool enabled) noexcept {
    g_native_activity_fallback5916c3.store(enabled,std::memory_order_release);
}
void SetVisualFallbackEnabled(bool enabled) noexcept {
    {std::lock_guard<std::mutex> lk(g_hand_candidate_mutex178r14);
     if(g_visual_fallback_enabled.exchange(enabled,std::memory_order_acq_rel)!=enabled){++g_door_generation38;++g_door_acquisition_generation65;g_door_candidates38.clear();}}
    if(!enabled)InvalidateVisualRay();
}




void SetHandRayWorld(bool active,float ox,float oy,float oz,float dx,float dy,float dz,float maxReach,float radius,int owner) noexcept {
    RaySample packet{};packet.hand=active;packet.ox=ox;packet.oy=oy;packet.oz=oz;
    packet.x=dx;packet.y=dy;packet.z=dz;packet.limit=maxReach;packet.radius=radius;
    packet.tick=GetTickCount64();packet.owner=owner;
    // Owner publication and owner-qualified cleanup share one transaction.
    // A legacy cleanup cannot observe an old owner, then revoke a newer Grip ray.
    std::lock_guard<std::mutex> lk(g_hand_candidate_mutex178r14);
    if(!active||!interaction112::Normalize(packet)){
        g_hand_ray_active.store(false,std::memory_order_release);++g_door_generation38;g_door_candidates38.clear();return;
    }
    // Publication is one transaction; a busy writer revokes the previous ray.
    const bool published=g_hand_packets112.publish(packet);
    g_hand_ray_active.store(published,std::memory_order_release);
    if(!published){++g_door_generation38;g_door_candidates38.clear();}
}
void ClearLegacyHandRayWorldH5() noexcept {
    std::lock_guard<std::mutex> lk(g_hand_candidate_mutex178r14);
    RaySample current{};
    // The HUD laser publishes the default owner -1. Ordinary Grip rays carry
    // physical side 0/1 and retain their own release/context/freshness rules.
    if(!g_hand_ray_active.load(std::memory_order_acquire)||
       !g_hand_packets112.read(current)||!current.hand||current.owner!=-1)return;
    g_hand_ray_active.store(false,std::memory_order_release);
    ++g_door_generation38;g_door_candidates38.clear();
}
void SetPalmRays59c9(const palm59c9::Pair& pair) noexcept {
    (void)g_palm_packets59c9.publish(pair);
}
bool LastHandCandidate(HandCandidate& out) noexcept {
    std::lock_guard<std::mutex> lk(g_hand_candidate_mutex178r14);out=g_hand_candidate178r14;
    if(out.sourceGeneration){
        const auto now=std::uint64_t(GetTickCount64());palm59c9::Pair current{};
        if(out.sourceGeneration!=g_door_generation38||!g_enabled.load(std::memory_order_acquire)||
           !g_split_ready57c1.load(std::memory_order_acquire)||!HandRayActive()||g_visual_fallback_enabled.load(std::memory_order_acquire)||
           out.owner<0||out.owner>1||!out.tick||now<out.tick||now-out.tick>80||!g_palm_packets59c9.read(current)||
           !current.eligible[out.owner]||current.ray[out.owner].sourceEpoch!=out.sourceEpoch||
           current.ray[out.owner].sourcePlayer!=out.sourcePlayer||!interaction112::Fresh(current.ray[out.owner],now)){
            out={};return false;
        }
    }
    return out.tick!=0;
}
void SetDogGestureObservationD59R1(bool enabled) noexcept {SetDogObservationImplD59R1(enabled);}
bool ReadDogHandCandidateD59R1(int side,std::uint64_t now,DogHandCandidateD59R1& out) noexcept {
    return ReadDogCandidateImplD59R1(side,now,out);
}
bool NativeDirectDoorCandidate54r1(int side,std::uint64_t now,HandCandidate& out) noexcept {return ReadDirectDoor54r1(side,now,out);}
bool ExactDoorCandidate38(std::uint32_t target,int side,std::uint64_t now,HandCandidate& out) noexcept {
    return ReadExactDoor54r1(target,side,now,out);
}
std::uint64_t DoorSourceGeneration38() noexcept {
    std::lock_guard<std::mutex> lk(g_hand_candidate_mutex178r14);
    return g_enabled.load(std::memory_order_acquire)&&g_split_ready57c1.load(std::memory_order_acquire)&&
        !g_visual_fallback_enabled.load(std::memory_order_acquire)&&HandRayActive()?g_door_generation38:0;
}
bool DoorSourceCurrent38(std::uint64_t generation) noexcept {
    return generation&&DoorSourceGeneration38()==generation;
}
std::uint64_t DoorAcquisitionGeneration65() noexcept {
    std::lock_guard<std::mutex> lk(g_hand_candidate_mutex178r14);
    return g_enabled.load(std::memory_order_acquire)&&g_split_ready57c1.load(std::memory_order_acquire)&&
        !g_visual_fallback_enabled.load(std::memory_order_acquire)?g_door_acquisition_generation65:0;
}
bool DoorAcquisitionCurrent65(std::uint64_t generation) noexcept {
    return generation&&DoorAcquisitionGeneration65()==generation;
}
int LockedPalmOwner19c4(std::uint64_t now) noexcept {
    palm59c9::Pair pair{};RaySample ray{};
    if(!g_enabled.load(std::memory_order_acquire)||!g_hand_ray_active.load(std::memory_order_acquire)||
       !g_palm_packets59c9.read(pair)||pair.locked<0||pair.locked>1||!g_hand_packets112.read(ray))return -1;
    const auto side=pair.locked;
    return pair.eligible[side]&&pair.ray[side].hand&&interaction112::Fresh(pair.ray[side],now)&&
        ray.hand&&ray.owner==side&&interaction112::Fresh(ray,now)?side:-1;
}
bool HandRayActive() noexcept {
    RaySample ray{};return g_hand_ray_active.load(std::memory_order_acquire)&&
        g_hand_packets112.read(ray)&&ray.hand&&interaction112::Fresh(ray,GetTickCount64());
}


Status GetStatus() noexcept {
    Status s{};
    s.installed = g_installed.load(std::memory_order_acquire);
    s.enabled = g_enabled.load(std::memory_order_acquire);
    RaySample ray{};
    s.visual_ray_valid = LoadVisualRay(ray);
    s.ray_builds = g_ray_builds.load(std::memory_order_relaxed);
    s.interaction_calls = g_interaction_calls.load(std::memory_order_relaxed);
    s.redirects = g_redirects.load(std::memory_order_relaxed);
    s.onscreen_checks = g_onscreen_checks.load(std::memory_order_relaxed);
    s.onscreen_forced = g_onscreen_forced.load(std::memory_order_relaxed);
    s.dir_x = ray.x; s.dir_y = ray.y; s.dir_z = ray.z;
    s.native_dir_valid = g_native_dir_valid.load(std::memory_order_acquire);
    s.native_dir_x = g_native_dir_x.load(std::memory_order_relaxed);
    s.native_dir_y = g_native_dir_y.load(std::memory_order_relaxed);
    s.native_dir_z = g_native_dir_z.load(std::memory_order_relaxed);
    s.source_tan_x = ray.tan_x; s.source_tan_y = ray.tan_y;
    s.center_x = g_center_x.load(std::memory_order_relaxed); s.center_y = g_center_y.load(std::memory_order_relaxed);
    s.last_native_to_visual_deg = g_last_native_to_visual_deg.load(std::memory_order_relaxed);
    s.native_reach_m = g_native_reach_m.load(std::memory_order_relaxed);
    s.base_radius_m = g_radius_m.load(std::memory_order_relaxed);
    s.cone_half_angle_deg = g_cone_half_angle_deg.load(std::memory_order_relaxed);
    return s;
}

} // namespace kcd2vr::interaction_alignment
