// Complete verbatim production functions. Native input and ray metadata are explicit fixtures.
#include <windows.h>
#include <intrin.h>
#include <atomic>
#include <cmath>
#include <mutex>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include "diagnostics/capture168.h"
#include "diagnostics/interaction_receipt59c10.h"
namespace kcd2vr::diagnostic_trace {inline std::vector<std::string> receipts;inline void Write(const char* line){receipts.emplace_back(line);}}
#include "vrik/physical_interaction.h"
#include "vrik/interaction_ray112.h"
#include "vrik/interaction_cone57c1.h"
#include "core/snapshot_mailbox.h"
#include "core/door_candidates38.h"
#include "interaction_alignment.h"
#include "subsystems/alchemy/native_selection68.h"
// This fixture exercises the ordinary/native-reference selector. It has no
// actual alchemy object-selector invocation or tracked crafting frame. The
// enabled native wrappers and source joins run in native_selection68/bridge67.
namespace kcd2vr::frik_pose_probe {
inline bool WorkstationFrame46(workstation_sample46::Frame& out) noexcept {out={};return false;}
}
namespace kcd2vr::interaction_alignment {
namespace selection68=kcd2vr::subsystems::alchemy::native_selection68;
namespace craft68=kcd2vr::subsystems::alchemy::bridge67;
static std::atomic<bool> g_selection_ready68{false};
static selection68::Scope g_selection_scope68{};
static bool SelectionScopeCurrent68(selection68::Table&,std::uint64_t) noexcept {return false;}
constexpr std::uintptr_t kScreenReturn=0x640363ull;
constexpr std::uintptr_t kObjectPolicy=0x3A69D90ull,kDogPolicy=0x3A69E48ull,kChatPolicy=0x3A69F00ull;
std::uintptr_t g_wh_base=0;
std::atomic<bool> g_split_ready57c1{false},g_enabled{true},g_visual_fallback_enabled{true},g_hand_ray_active{false};
std::atomic<bool> g_native_activity_fallback5916c3{false};
std::atomic<std::uint64_t> g_interaction_calls{1};
std::atomic<std::uint64_t> g_onscreen_checks{0},g_unknown_candidates57c1{0},g_head_candidates57c1{0},g_hand_candidates57c1{0},g_onscreen_forced{0};
std::atomic<float> g_radius_m{.21f},g_cone_half_angle_deg{10.f};
struct Candidate57c1 {std::uintptr_t player=0;std::uint64_t tick=0;interaction57c1::Kind kind=interaction57c1::Kind::Unknown;std::uint32_t id=0;char cls[32]{};}; // R59.16C10R10
thread_local Candidate57c1 g_candidate57c1{};
using OnScreenCheckFn=char(__fastcall*)(std::uintptr_t,std::uintptr_t,float*,std::uintptr_t);
OnScreenCheckFn g_onscreen_original=nullptr;
using RaySample=interaction112::Ray;
struct NativeRay112 { RaySample ray{}; float ox=0,oy=0,oz=0,reach=0;std::uint64_t tick=0; };
core::SnapshotMailbox<NativeRay112> g_native_packets112;
core::SnapshotMailbox<RaySample> g_hand_packets112;
core::SnapshotMailbox<palm59c9::Pair> g_palm_packets59c9;
thread_local palm59c9::Ranking g_palm_ranking59c9;
std::mutex g_hand_candidate_mutex178r14;HandCandidate g_hand_candidate178r14{};
door_candidates38::Records<HandCandidate> g_door_candidates38;
std::uint64_t g_door_generation38=1;
std::uint64_t g_door_acquisition_generation65=(std::uint64_t(1)<<63)|1;
static HandCandidate g_direct_door54r1{};
static std::atomic<bool> g_direct_door_retired54r1{false};
static void InvalidateVisualRay() noexcept {} // Not the branch under test; native projection is a spy.
static bool CopyDirectDoor54r1Locked(int side,std::uint64_t now,HandCandidate& out) noexcept {
    out={};
    if(side<0||side>1||!now)return false;
    const auto& c=g_direct_door54r1;
    if(g_direct_door_retired54r1.load(std::memory_order_acquire)||
       !g_enabled.load(std::memory_order_acquire)||!g_split_ready57c1.load(std::memory_order_acquire)||
       g_visual_fallback_enabled.load(std::memory_order_acquire)||
       c.sourceGeneration!=g_door_acquisition_generation65||!c.sourceGeneration||c.owner!=side||!c.entity||
       !c.tick||now<c.tick||now-c.tick>80||!c.sourceTick||now<c.sourceTick||now-c.sourceTick>80)return false;
    door_hold65::Frame hands65{};door_surface65::Witness surface65{};
    if(!door_hold65::frame.read(hands65)||!door_surface65::read(c.entity,hands65.hand[side],now,surface65)||
       hands65.hand[side].sourceGeneration!=c.sourceGeneration)return false;
    out=c;return true;
}

static bool ReadExactDoor54r1(std::uint32_t target,int side,std::uint64_t now,HandCandidate& out) noexcept {
    std::lock_guard<std::mutex> lk(g_hand_candidate_mutex178r14);
    out={};
    if(!g_enabled.load(std::memory_order_acquire)||!g_split_ready57c1.load(std::memory_order_acquire)||
       g_visual_fallback_enabled.load(std::memory_order_acquire))return false;
    HandCandidate direct{};
    if(CopyDirectDoor54r1Locked(side,now,direct)&&direct.entity==target){out=direct;return true;}
    return HandRayActive()&&g_door_candidates38.find(target,side,now,out)&&out.sourceGeneration==g_door_generation38;
}

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
               selection68::registered(table68,candidate.id)&&
               a4==g_selection_scope68.policy&&craft68::selectionNear68(table68,nominated68,now67)){
                if(candidate.id!=nominated68)return 0;
                out_screen[0]=out_screen[1]=50.f;return 1;
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

void SetEnabled(bool enabled) noexcept {
    {std::lock_guard<std::mutex> lk(g_hand_candidate_mutex178r14);
     if(g_enabled.exchange(enabled,std::memory_order_acq_rel)!=enabled){++g_door_generation38;++g_door_acquisition_generation65;g_door_candidates38.clear();}
     if(!enabled)g_hand_ray_active.store(false,std::memory_order_release);}
    if(!enabled)InvalidateVisualRay();
}

void SetVisualFallbackEnabled(bool enabled) noexcept {
    {std::lock_guard<std::mutex> lk(g_hand_candidate_mutex178r14);
     if(g_visual_fallback_enabled.exchange(enabled,std::memory_order_acq_rel)!=enabled){++g_door_generation38;++g_door_acquisition_generation65;g_door_candidates38.clear();}}
    if(!enabled)InvalidateVisualRay();
}

void SetNativeActivityFallback5916c3(bool enabled) noexcept {
    g_native_activity_fallback5916c3.store(enabled,std::memory_order_release);
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

bool HandRayActive() noexcept {
    RaySample ray{};return g_hand_ray_active.load(std::memory_order_acquire)&&
        g_hand_packets112.read(ray)&&ray.hand&&interaction112::Fresh(ray,GetTickCount64());
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
}
