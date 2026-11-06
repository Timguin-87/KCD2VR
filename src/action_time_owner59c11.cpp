#include "core/mutual_admission_t25.h"
#include "subsystems/throw_drop/hand_claim_t24.h"
#include "action_time_owner59c11.h"
#include "subsystems/ranged_reload/crossbow_native_xr34.h"
#include "vrik/visible_failure.h" // D85 xbow: crossbow refusal line
#include "subsystems/ranged_reload/bow_physical_aim_d65c.h"
#include "subsystems/ranged_reload/owned_inline_d65.h"
#include "subsystems/physics/runtime/native_weapon_body_t4.h"
#include "subsystems/ranged_reload/reload_throw_claim_t25.h"
#include "core/grasp_transaction59c12c.h"
#include "core/native_grasp13_pins.h"
#include "core/instant_pickup_d72.h"
#include "core/haptics.h"
#include "core/snapshot_mailbox.h"
#include "core/transfer_clip_d65.h"
#include "native_script_t2.h"
#include "subsystems/interactions/native_passenger_a34.h"
#include "subsystems/throw_drop/inventory_lifecycle_t8.h"
#include "subsystems/throw_drop/hand_claim_t24.h"
#include "subsystems/throw_drop/runtime/ammo_eligibility_t20.h"
#include "subsystems/throw_drop/native_transient_ammo_t20.h"
#include "subsystems/throw_drop/native_paid_ammo_release_t20.h"
#include "subsystems/throw_drop/native_ammo_service_t22.h"
#include "retail_build112.h"
#include "animation_filter.h"
#include "frik_pose_probe.h" // C10R18: HenryItemAttachManager59c15 (Henry identity by entity id, R15)
#include "subsystems/physics/native_grab_input60.h"
#include "diagnostic_trace.h"
#include "diagnostics/capture168.h"
#include "diagnostics/activity_links_d77.h"
#include "diagnostics/interaction_receipt59c10.h"
#include "activity_input163.h"
#include "vrik/alchemy_stage19c4.h"
#include "vrik/alchemy_semi46.h"
#include "vrik/alchemy_resource_scope_d67r1.h"
#include "subsystems/alchemy/native_return67.h"
#include "subsystems/alchemy/instant_native_d72.h"
#include "subsystems/alchemy/presentation_speed_d81.h"
#include "subsystems/alchemy/resource_turn_d86.h"
#include "subsystems/alchemy/carafe_pour_d85.h" // D85 craft
#include "vrik/swing_whoosh_r116.h" // R116: the clip's weapon whoosh is skipped after our early one
#include "vrik/visible_failure.h" // D85 craft: one carafe reason line
#include "subsystems/smithing/instant_native_d72.h"
#include "subsystems/alchemy/instant_native_pins_d72.h"
#include "vrik/instant_activities_d72.h"
#include "subsystems/alchemy/mortar_handoff69.h"
#include "core/alchemy_semi_pins46.h"
#include "vrik/semi_smith46.h"
#include "vrik/semi_smith_native46.h"
#include "subsystems/masterstrike_combos/ms_reaction_owner_d62c.h"
#include "subsystems/masterstrike_combos/paired_signals_d62c.h"
#include "subsystems/masterstrike_combos/paired_hands_d72.h"
#include "subsystems/masterstrike_combos/native_kill_anim_nf.h"
#include "core/semi_smith_pins46.h"
#include "core/door_use46.h"
#include "vrik/workstation178r20c1.h"
#include <DetourModKit/hook.hpp>
#include <windows.h>
#if defined(_MSC_VER)
#include <intrin.h> // C10R18: _ReturnAddress for the drop-pose getter filter
#endif
#include "core/readable_pages178r22c1.h"
#include "subsystems/alchemy/finish_observer_cb1.h" // CB1: observed native brew result for the stage receipt
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <optional>

namespace kcd2vr::animation_filter { void* PlayerSkeletonAnim() noexcept; }
#include "vrik/impale_death_speed_r106.h"

namespace kcd2vr::action_time_owner59c11 {
namespace {

// ---- exact current-retail WHGame (TimeDateStamp 0x6A350E20) ------------------------------------------------------
constexpr std::uint32_t kTs=0x6A350E20u;
// CryMannequin CActionController (RTTI .?AVCActionController@@, vtable WHGame+0x40519E8, 58 slots).
constexpr std::uintptr_t kCtrlVft=0x40519E8ull;
constexpr std::uintptr_t kCtrlQueue=0x684D8Cull;     // slot 19 Queue(IAction&, float queueTime)
constexpr std::uintptr_t kCtrlUpdate=0x4B3DF8ull;    // slot 21 Update(float timePassed)
constexpr unsigned char kQueuePin[18]{0x48,0x89,0x5C,0x24,0x08,0x57,0x48,0x83,0xEC,0x20,0x48,0x8B,0xDA,0xF3,0x0F,0x11,0x52,0x14};
constexpr unsigned char kUpdatePin[30]{0x48,0x8B,0xC4,0x48,0x89,0x58,0x10,0x48,0x89,0x68,0x20,0x56,0x57,0x41,0x54,0x41,0x56,0x41,0x57,0x48,0x83,0xEC,0x60,0xF6,0x81,0x98,0x00,0x00,0x00,0x01};
// CActionController layout (Update/SetTimeScale/Flush/GetScope read these):
constexpr std::ptrdiff_t kCtrlContext=0x20,kCtrlScopeContexts=0x28,kCtrlEntityId=0x30,kCtrlScopeCount=0x34,kCtrlScopes=0x38,kCtrlTimeScale=0x9C;
constexpr std::size_t kScopeStride=0x168,kScopeContextStride=0x60;
constexpr std::ptrdiff_t kScopeContextChar=0x10;   // scope context -> ICharacterInstance*
// CActionScope (Update: layer base/count, speed bias cache, fragment time; Flush/resolver: playing + exiting action)
constexpr std::ptrdiff_t kScopeLayer=0x30,kScopeNumLayers=0x34,kScopeSpeedBias=0x58,kScopeFragTime=0x11C,kScopeAction=0x148,kScopeExiting=0x150,kScopeOneShot=0x164;
// IAction (Queue/SetFragment/Update/Requeue read these)
constexpr std::ptrdiff_t kActContext=0x8,kActActiveTime=0x10,kActQueueTime=0x14,kActPriority=0x24,kActStatus=0x28,kActFlags=0x2C,kActRootScope=0x30,kActFragId=0x38,kActTags=0x3C,kActOption=0x50,kActToken=0x54,kActRefs=0x58,kActSpeedBias=0x5C,kActAnimWeight=0x60;
constexpr std::ptrdiff_t kActChildren=0x88;            // IAction child actions (array, count at -4), finished recursively by ForceFinish
constexpr std::uint32_t kActFlagOneShot=0x4u;            // cleared by ForceFinish (WHGame+0x681FAC and its 140 inline copies)
// CryAnimation CSkeletonAnim (vtable WHGame+0x3A711E0; pinned by animation_filter), fields read by the dispatcher 0x84351C
constexpr std::uintptr_t kSkelVft=0x3A711E0ull;
constexpr std::uintptr_t kSkelDispatch=0x84351Cull;   // slot 46: fills m_LastAnimEvent then calls (+0x518)(charInstance,+0x520)
constexpr unsigned char kDispatchPin[16]{0x48,0x83,0xEC,0x28,0x33,0xC0,0x38,0x81,0x80,0x05,0x00,0x00,0x0F,0x84,0xCD,0x00};
constexpr std::uintptr_t kSkelNumAnims=0x83A8B0ull;   // slot 15 GetNumAnimsInFIFO(layer)
constexpr std::uintptr_t kSkelAnimTime=0xA98B30ull;   // slot 52 GetAnimationNormalizedTime(layer,index)
constexpr std::uintptr_t kSkelSetLayerTime=0xAB1124ull; // slot 29 SetLayerNormalizedTime(layer,t) (single-entry FIFO only)
constexpr unsigned char kNumAnimsPin[15]{0x8B,0xC2,0x48,0xC1,0xE0,0x06,0x48,0x8B,0x44,0x08,0x48,0x8B,0x40,0xFC,0xC3};
constexpr unsigned char kAnimTimePin[14]{0x48,0x83,0xEC,0x28,0x4C,0x8B,0xC9,0x8B,0xCA,0x48,0xFF,0xC1,0x41,0x8B};
constexpr unsigned char kSetLayerTimePin[14]{0x48,0x83,0xEC,0x28,0x44,0x8B,0xC2,0x4C,0x8B,0xD1,0x49,0xC1,0xE0,0x06};
constexpr std::ptrdiff_t kSkelChar=0x4E8,kSkelCb=0x518,kSkelCbData=0x520;
constexpr std::ptrdiff_t kEvTime=0x528,kEvEndTime=0x52C,kEvAnimNum=0x530,kEvPriority=0x534,kEvAnimPath=0x538,kEvAnimId=0x540,kEvNameCrc=0x544,kEvName=0x550,kEvParam=0x558;
constexpr std::ptrdiff_t kCharSkelAnim=0x110; // CCharInstance -> embedded CSkeletonAnim (animation_filter kSkeletonAnimOffset)

using QueueFn=void(__fastcall*)(void*,void*,float);
using UpdateFn=void(__fastcall*)(void*,float);
using NumAnimsFn=int(__fastcall*)(void*,int);
using AnimTimeFn=float(__fastcall*)(void*,int,int);
using SetLayerTimeFn=void(__fastcall*)(void*,int,float);
using EventCb=int(__fastcall*)(void*,void*);

std::optional<DetourModKit::hook::Hook> g_queue_hook,g_update_hook;
QueueFn g_queue_original=nullptr; UpdateFn g_update_original=nullptr;
std::atomic<bool> g_installed{false};
static std::atomic<bool> g_crossbow_registration_ready_xr34{false};
std::atomic<int> g_mode{Own};
std::atomic<bool> g_pickup_hold62{false},g_pickup_experiment62{false};
std::atomic<float> g_hold_at{0.50f},g_resume_speed{2.0f},g_commit_pull{0.10f},g_cancel_grip{0.35f};
std::atomic<unsigned> g_hold_timeout{2500};
std::atomic<int> g_cancel_mode{0};
std::atomic<std::uintptr_t> g_base{0};
std::atomic<void*> g_henry_ctrl{nullptr};
std::atomic<EventCb> g_orig_cb{nullptr};
std::atomic<void*> g_tapped_skel{nullptr};
std::atomic<unsigned> g_queued{0},g_scope_changes{0},g_events{0},g_holds{0},g_commits{0},g_cancels{0},g_timeouts{0};

// hand samples (worker -> game thread)
struct HandSample {std::atomic<float> grip{0},dist{0};std::atomic<std::uint64_t> tick{0};};
HandSample g_hand[2];
// Only the qualified controller callback reads native roots. Use publication
// copies this POD; a Run/XR worker never dereferences a queued native receiver.
struct RootObservation55 {
    std::uintptr_t controller=0,character=0,action=0,scope=0;
    std::uint64_t epoch=0,trackingEpoch=0,tick=0,generation=0;std::uint32_t nativeToken=0;
    int fragment=-1;bool valid=false,picking=false;
};
core::SnapshotMailbox<RootObservation55> g_root_before_use55;
core::SnapshotMailbox<grasp13::AcquisitionInput55> g_acquisition_input55;
core::SnapshotMailbox<grasp13::FinishingPresentationR94> g_finishing_visual_r94;
std::atomic<std::uint64_t> g_finishing_visual_token_r94{0},g_finishing_continuity_token_r94{0};
static void RevokeFinishingVisualR94() noexcept {g_finishing_continuity_token_r94.store(0,std::memory_order_release);g_finishing_visual_token_r94.store(0,std::memory_order_release);}
struct Use13 {int side=-1;std::uint64_t tick=0,serial=0;RootObservation55 before55{};
    std::uint64_t generation55=0;grasp13::AcquisitionInput55 input55{};bool physicalPickup63=false;
    std::uint32_t expectedEntityD40=0;}; // immutable expectation for this transported physical Use only
core::SnapshotMailbox<Use13> g_use13;std::atomic<std::uint64_t> g_use_serial13{0};
std::atomic<std::uint64_t> g_use_generation55{1},g_use_retired55{0};
static bool UseLive55(const Use13& use) noexcept {
    return use.serial&&use.generation55==g_use_generation55.load(std::memory_order_acquire)&&
        use.serial>g_use_retired55.load(std::memory_order_acquire);
}
static void RetireUse55(std::uint64_t serial) noexcept {
    auto previous=g_use_retired55.load(std::memory_order_acquire);
    while(previous<serial&&!g_use_retired55.compare_exchange_weak(previous,serial,std::memory_order_acq_rel)){}
}
static bool FreshAcquisitionInput55(const grasp13::AcquisitionInput55& input,std::uint64_t now) noexcept {
    return input.character&&input.epoch&&input.trackingEpoch&&input.sample&&
        input.headTracked&&grasp13::fresh(now,input.tick,60);
}
struct PendingPhysicalPickup63 {grasp13::AcquisitionInput55 source{};std::uint64_t generation=0;int side=-1;};
core::SnapshotMailbox<PendingPhysicalPickup63> g_pickup_pending63;
static bool PendingPickupObserved63(std::uint64_t now) noexcept {
    PendingPhysicalPickup63 p{};
    return g_pickup_pending63.read(p)&&p.side>=0&&p.side<2&&p.generation==g_use_generation55.load(std::memory_order_acquire)&&
        FreshAcquisitionInput55(p.source,now)&&p.source.tracked[p.side]&&std::isfinite(p.source.grip[p.side])&&
        p.source.grip[p.side]>=g_cancel_grip.load(std::memory_order_acquire);
}

static bool CurrentUseInput55(const Use13& use,const grasp13::AcquisitionInput55& input,
                              std::uint64_t now,std::uint64_t maxAge=1500) noexcept {
    return UseLive55(use)&&use.side>=0&&use.side<2&&grasp13::fresh(now,use.tick,maxAge)&&
        FreshAcquisitionInput55(use.input55,use.tick)&&FreshAcquisitionInput55(input,now)&&
        use.input55.character==input.character&&use.input55.epoch==input.epoch&&
        use.input55.trackingEpoch==input.trackingEpoch&&input.tick>=use.input55.tick&&
        use.input55.tracked[use.side]&&std::isfinite(use.input55.grip[use.side])&&
        use.input55.grip[use.side]>=g_cancel_grip.load(std::memory_order_acquire)&&
        input.tracked[use.side]&&std::isfinite(input.grip[use.side])&&
        input.grip[use.side]>=g_cancel_grip.load(std::memory_order_acquire)&&
        !frik_pose_probe::HandClaimBlocksT24(use.side);
}

// Owner mutations are serialized, but never wait for an animation job and
// never hold this transaction around the original controller Update (which can
// join those jobs). Recursive entry is only on the same native callback stack.
static std::atomic_flag g_owner_transaction19c4=ATOMIC_FLAG_INIT;
static thread_local unsigned g_owner_transaction_depth19c4=0;
struct OwnerTransaction19c4 {
    bool acquired=false;
    OwnerTransaction19c4() noexcept {
        if(g_owner_transaction_depth19c4){++g_owner_transaction_depth19c4;acquired=true;}
        else if(!g_owner_transaction19c4.test_and_set(std::memory_order_acquire)){g_owner_transaction_depth19c4=1;acquired=true;}
    }
    ~OwnerTransaction19c4(){if(acquired&&!--g_owner_transaction_depth19c4)g_owner_transaction19c4.clear(std::memory_order_release);}
    explicit operator bool() const noexcept {return acquired;}
};
// Owner state is changed by the controller or an exact native pickup callback
// under that transaction; status and render placement use immutable mailboxes.
enum State : int {Idle=0,Armed=1,Playing=2,Held=3,Released=4,Cancelling=5,PendingAttach=6};
struct Owner {
    int state=Idle; void* action=nullptr; void* scope=nullptr; int fragId=-1; char frag[40]{}; int side=-1;
    std::uint64_t armedAt=0,heldAt=0,releasedAt=0,useSerial54=0,useGeneration55=0; float holdDist=0.f,lastT=0.f; int layer=0; bool speedWritten=false;
    grasp13::AcquisitionInput55 useInput55{};
    std::uint32_t expectedEntityD40=0; // copied with useSerial54; native transfer must resolve it
    bool dropRequested=false, stowCommitted=false,releaseUnclaimed55=false;
    float savedSpeed=1.f,lastWrittenSpeed=1.f;bool haveSavedSpeed=false;std::uint32_t nativeToken=0;
} g_owner;
static bool CurrentOwnerInput55(std::uint64_t now) noexcept {
    grasp13::AcquisitionInput55 input{};const int side=g_owner.side;
    return g_owner.useSerial54&&g_owner.useGeneration55==g_use_generation55.load(std::memory_order_acquire)&&
        g_owner.useSerial54>g_use_retired55.load(std::memory_order_acquire)&&side>=0&&side<2&&
        g_acquisition_input55.read(input)&&FreshAcquisitionInput55(input,now)&&
        input.character==g_owner.useInput55.character&&input.epoch==g_owner.useInput55.epoch&&
        input.trackingEpoch==g_owner.useInput55.trackingEpoch&&input.tracked[side]&&
        std::isfinite(input.grip[side])&&input.grip[side]>=g_cancel_grip.load(std::memory_order_acquire);
}
std::atomic<int> g_state_pub{Idle},g_side_pub{-1}; std::atomic<float> g_time_pub{0}; std::atomic<std::uint64_t> g_held_pub{0};
std::atomic<std::uint64_t> g_use_owner_pub55{0}; // copied original request, never native admission
static bool PhysicalApproachInterest55(const Use13& use) noexcept {
    // Request interest only; Attach still requires current Grip/source/item.
    return use.physicalPickup63&&g_use_owner_pub55.load(std::memory_order_acquire)==use.serial;
}

// per-scope last playing action (receipts)
constexpr unsigned kMaxScopes=24;
void* g_scope_last[kMaxScopes]{}; int g_scope_last_frag[kMaxScopes]{};
char g_scope0_frag[40]{"-"};
struct ScopeName19c4 {char text[40]{"-"};};
core::SnapshotMailbox<ScopeName19c4> g_scope_name19c4;
core::SnapshotMailbox<grasp13::Acquisition54> g_acquisition54;
core::SnapshotMailbox<grasp13::Acquisition54> g_acquisition_foundation_d56;

std::uintptr_t WhBase() noexcept {
    if(const auto b=g_base.load(std::memory_order_acquire))return b;
    HMODULE m=nullptr; if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_PIN,L"WHGame.dll",&m)||!m)return 0;
    const auto b=reinterpret_cast<std::uintptr_t>(m); g_base.store(b,std::memory_order_release); return b;
}
bool Readable(const void* p,std::size_t n) noexcept {return memory126::readableNow178r22c1(p,n);}
#ifdef KCD2VR_GRASP_HOST_TEST
extern bool HostReadAllowed19c4(const void*,std::size_t) noexcept;
#endif
bool SehCopy(const void* p,void* out,std::size_t n) noexcept {
    if(!p||!out||!n)return false;
#ifdef KCD2VR_GRASP_HOST_TEST
    if(!HostReadAllowed19c4(p,n))return false;
#endif
    __try{std::memcpy(out,p,n);return true;}__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
template<class T> T Rd(const void* base,std::ptrdiff_t off,T fallback) noexcept {
    T v{};return base&&SehCopy(static_cast<const unsigned char*>(base)+off,&v,sizeof(v))?v:fallback;
}
template<class T> bool Wr(void* base,std::ptrdiff_t off,T v) noexcept {
    if(!base)return false; auto* p=static_cast<unsigned char*>(base)+off;
#ifdef KCD2VR_GRASP_HOST_TEST
    extern bool HostWriteAllowed19(const void*,std::size_t) noexcept;
    if(!HostWriteAllowed19(p,sizeof(v)))return false; // memory-I/O fault injection, not a state-machine substitute
#endif
    __try{std::memcpy(p,&v,sizeof(v));return true;}__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}

// R106 impale fast death (see vrik/impale_death_speed_r106.h). Runs for NON-Henry controllers only. The exact
// controller field +0x9C is what native Update multiplies by (pin alchemy_semi_pins46 GetTimeScale +9C). One victim
// at a time; the saved value goes back when the servo ends the wait or 5 s pass - and only if nobody else changed it.
static void* g_impale_speed_ctrl_r106=nullptr;static float g_impale_speed_saved_r106=1.f;static std::uint32_t g_impale_speed_entity_r106=0;
static void ImpaleDeathSpeedR106(void* ctrl) noexcept {
    namespace ids=kcd2vr::impale_death_speed_r106;
    if(!ctrl)return;
    const auto now=std::uint64_t(GetTickCount64());
    if(g_impale_speed_ctrl_r106==ctrl){
        if(ids::Wants(g_impale_speed_entity_r106,now)&&Rd<unsigned>(ctrl,kCtrlEntityId,0u)==g_impale_speed_entity_r106)return; // still fast
        float cur=1.f;
        if(SehCopy(static_cast<unsigned char*>(ctrl)+kCtrlTimeScale,&cur,sizeof(cur))&&cur==ids::kFactor)
            (void)Wr<float>(ctrl,kCtrlTimeScale,g_impale_speed_saved_r106); // ours: put the saved value back; someone else's: leave it
        ids::restored.fetch_add(1,std::memory_order_relaxed);
        g_impale_speed_ctrl_r106=nullptr;g_impale_speed_entity_r106=0;return;
    }
    if(g_impale_speed_ctrl_r106)return; // one victim at a time
    const auto victim=ids::victim.load(std::memory_order_acquire);
    if(!victim||!ids::Wants(victim,now))return;
    if(Rd<unsigned>(ctrl,kCtrlEntityId,0u)!=victim)return;
    float cur=1.f;
    if(!SehCopy(static_cast<unsigned char*>(ctrl)+kCtrlTimeScale,&cur,sizeof(cur))||!std::isfinite(cur)||cur<=0.f||cur>1.5f)return; // another owner or garbage
    if(!Wr<float>(ctrl,kCtrlTimeScale,ids::kFactor))return;
    g_impale_speed_ctrl_r106=ctrl;g_impale_speed_saved_r106=cur;g_impale_speed_entity_r106=victim;
    ids::applied.fetch_add(1,std::memory_order_relaxed);
    if(diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now)){
        char m[200]{};std::snprintf(m,sizeof(m),"R106 IMPALE_FAST_DEATH entity=%u ctrl=%p timeScale %.2f->%.2f until=PIN_OR_5S",victim,ctrl,double(cur),double(ids::kFactor));
        diagnostic_trace::Write(m);
    }
}
bool ExactBuild(std::uintptr_t base) noexcept {
    if(!base)return false;
    const auto* dos=reinterpret_cast<const IMAGE_DOS_HEADER*>(base); if(!Readable(dos,sizeof(*dos))||dos->e_magic!=IMAGE_DOS_SIGNATURE)return false;
    const auto* nt=reinterpret_cast<const IMAGE_NT_HEADERS64*>(base+static_cast<std::uintptr_t>(dos->e_lfanew));
    return Readable(nt,sizeof(*nt))&&nt->Signature==IMAGE_NT_SIGNATURE&&nt->FileHeader.TimeDateStamp==kTs;
}
bool MatchBytes(std::uintptr_t addr,const unsigned char* bytes,std::size_t n) noexcept {
    const void* p=reinterpret_cast<const void*>(addr); return Readable(p,n)&&std::memcmp(p,bytes,n)==0;
}
bool CopyStr(const char* src,char* dst,std::size_t cap) noexcept {
    if(!dst||cap<2)return false; dst[0]=0; if(!src)return false;
    __try{ std::size_t i=0; for(;i+1<cap;++i){ if(!Readable(src+i,1)){dst[i]=0;return i>0;} const char c=src[i]; dst[i]=c; if(!c)return true; } dst[cap-1]=0; return true; }
    __except(EXCEPTION_EXECUTE_HANDLER){dst[0]=0;return false;}
}
bool IEq(const char* a,const char* b) noexcept {return a&&b&&_stricmp(a,b)==0;}

// Test substitutes native function addresses, never state-machine branches.
#ifdef KCD2VR_GRASP_HOST_TEST
extern std::uintptr_t HostNative13(std::uintptr_t) noexcept;
extern std::uintptr_t HostCaller13() noexcept; // C10R18: the host fixture supplies the native return address
#define KCD2VR_GRASP_CALLER() HostCaller13()
#elif defined(_MSC_VER)
#define KCD2VR_GRASP_CALLER() reinterpret_cast<std::uintptr_t>(_ReturnAddress())
#else
#define KCD2VR_GRASP_CALLER() reinterpret_cast<std::uintptr_t>(__builtin_return_address(0))
#endif
template<class Fn> Fn Native13(std::uintptr_t rva) noexcept {
#ifdef KCD2VR_GRASP_HOST_TEST
    return reinterpret_cast<Fn>(HostNative13(rva));
#else
    return reinterpret_cast<Fn>(WhBase()+rva);
#endif
}

// ---- native calls (game thread, SEH-wrapped) ---------------------------------------------------------------------
// C19C4: controller slot 18 / 399874 returns a fragment SUB-TAG DEFINITION,
// not a string. Reading it as char* produced the binary names in the F5 logs
// and made the queue-side Picking arm unreachable. 140E294 -> 78BCEC proves
// the current definition's CRC layout; use one checked entry, no table scan.
// We only name the one supported semantic action. Other fragment IDs remain
// numeric in diagnostics instead of inventing a native text ABI.
#ifdef KCD2VR_GRASP_HOST_TEST
void HostFragmentRead19c4() noexcept;
#endif
bool FragmentCrc19c4(void* ctrl,int fragId,std::uint32_t& crc) noexcept {
    const auto b=g_base.load(std::memory_order_acquire);
    if(!b||!ctrl||fragId<0||Rd<std::uintptr_t>(ctrl,0,0)!=b+kCtrlVft)return false;
#ifdef KCD2VR_GRASP_HOST_TEST
    HostFragmentRead19c4(); // read-observation only
#endif
    const void* context=Rd<void*>(ctrl,kCtrlContext,nullptr);
    const void* definition=Rd<void*>(context,0,nullptr);
    const void* fragments=Rd<void*>(definition,0x18,nullptr);
    const auto* entries=Rd<const unsigned char*>(fragments,8,nullptr);
    const int count=Rd<int>(entries,-4,-1);
    if(!entries||count<1||count>65536||fragId>=count)return false;
    std::uint32_t value=0;
    if(!SehCopy(entries+std::size_t(fragId)*0x20+0x10,&value,sizeof(value)))return false;
    // The current controller/definition must still be the one just sampled.
    if(Rd<std::uintptr_t>(ctrl,0,0)!=b+kCtrlVft||Rd<void*>(ctrl,kCtrlContext,nullptr)!=context||
       Rd<void*>(context,0,nullptr)!=definition||Rd<void*>(definition,0x18,nullptr)!=fragments||
       Rd<const unsigned char*>(fragments,8,nullptr)!=entries||Rd<int>(entries,-4,-1)!=count)return false;
    crc=value;return true;
}
const char* FragName(void* ctrl,int fragId) noexcept {
    std::uint32_t crc=0;
    return FragmentCrc19c4(ctrl,fragId,crc)&&crc==0x03E46E25u?"Picking":nullptr; // native lower-case CRC32
}
int NumAnims(void* skel,int layer) noexcept {
    const auto b=g_base.load(std::memory_order_acquire); if(!b||!skel||layer<0||layer>15)return -1;
    __try{return Native13<NumAnimsFn>(kSkelNumAnims)(skel,layer);}__except(EXCEPTION_EXECUTE_HANDLER){return -1;}
}
float AnimTime(void* skel,int layer,int index) noexcept {
    const auto b=g_base.load(std::memory_order_acquire); if(!b||!skel||layer<0||layer>15||index<0)return -1.f;
    __try{return Native13<AnimTimeFn>(kSkelAnimTime)(skel,layer,index);}__except(EXCEPTION_EXECUTE_HANDLER){return -1.f;}
}

void* HenryChar() noexcept {
    void* skel=animation_filter::PlayerSkeletonAnim(); if(!skel)return nullptr;
    void* ch=Rd<void*>(skel,kSkelChar,nullptr);
    return (ch&&static_cast<unsigned char*>(skel)-kCharSkelAnim==static_cast<unsigned char*>(ch))?ch:nullptr;
}
// C10R18 (review): R17's Henry identity by entity id, restored. C10R15: the skinning callback behind HenryChar() is null for
// minutes in some modes (the 14 Oct log lost Henry's controller for 8 minutes); a held item must not be faulted by that.
// Henry's C_ItemAttachmentManager (player+0x9D0, vtable-checked) -> player (vtable-checked) -> entity id at player+0x30.
std::uint32_t HenryEntityId18() noexcept {
    const auto b=g_base.load(std::memory_order_acquire);void* mgr=frik_pose_probe::HenryItemAttachManager59c15();
    if(!b||!mgr||Rd<std::uintptr_t>(mgr,0,0)!=b+0x3EC9958ull)return 0;
    void* pl=Rd<void*>(mgr,0x38,nullptr);
    return pl&&Rd<std::uintptr_t>(pl,0,0)==b+0x3AA4B98ull&&Rd<void*>(pl,0x9D0,nullptr)==mgr?Rd<std::uint32_t>(pl,0x30,0u):0u;
}
bool IsHenryController(void* ctrl) noexcept {
    if(!ctrl)return false;
    if(Rd<std::uintptr_t>(ctrl,0,0)!=g_base.load(std::memory_order_acquire)+kCtrlVft)return false;
    if(const auto id=HenryEntityId18())return Rd<std::uint32_t>(ctrl,kCtrlEntityId,0u)==id;
    void* henry=HenryChar(); if(!henry)return false;
    const void* contexts=Rd<const void*>(ctrl,kCtrlScopeContexts,nullptr); if(!contexts)return false;
    // scope context 0 (FullBody) carries the owning character
    return Rd<void*>(contexts,kScopeContextChar,nullptr)==henry;
}

// ---- receipts -------------------------------------------------------------------------------------------------------
namespace interactionCapture19=diagnostics::capture168;
struct Throttle {std::atomic<std::uint64_t> windowStart{0},session{0};std::atomic<unsigned> inWindow{0},total{0};
    bool allow(std::uint64_t now,unsigned perSecond,unsigned sessionCap) noexcept {
        const auto current=interactionCapture19::gate.session[unsigned(interactionCapture19::Domain::Interaction)].load(std::memory_order_acquire);
        if(session.exchange(current,std::memory_order_acq_rel)!=current){
            windowStart.store(now,std::memory_order_relaxed);inWindow.store(0,std::memory_order_relaxed);total.store(0,std::memory_order_relaxed);}
        if(total.load(std::memory_order_relaxed)>=sessionCap)return false;
        auto ws=windowStart.load(std::memory_order_relaxed);
        if(now-ws>=1000){windowStart.store(now,std::memory_order_relaxed);inWindow.store(0,std::memory_order_relaxed);}
        if(inWindow.fetch_add(1,std::memory_order_relaxed)>=perSecond)return false;
        total.fetch_add(1,std::memory_order_relaxed);return true;}
};
Throttle g_t_queue,g_t_queue_other,g_t_scope,g_t_event;
bool DiagOn() noexcept {
    // No clock/name/scope work in ordinary quiet play. F3 does not enable F5.
    constexpr auto bit=1u<<unsigned(interactionCapture19::Domain::Interaction);
    return (interactionCapture19::gate.mask.load(std::memory_order_acquire)&bit)&&diagnostic_trace::Enabled()&&
        interactionCapture19::active(interactionCapture19::Domain::Interaction,GetTickCount64());
}
const char* StateName(int s) noexcept {switch(s){case Armed:return "ARMED";case Playing:return "PLAYING";case PendingAttach:return "PENDING_ATTACH";case Held:return "HELD";case Released:return "RELEASED";case Cancelling:return "CANCELLING";default:return "IDLE";}}

void TagsHex(const void* action,char* out,std::size_t cap) noexcept {
    unsigned char t[20]{}; if(!SehCopy(static_cast<const unsigned char*>(action)+kActTags,t,sizeof(t))){std::snprintf(out,cap,"?");return;}
    std::size_t n=0; for(int i=0;i<20&&n+3<cap;++i)n+=std::snprintf(out+n,cap-n,"%02x",t[i]);
}
void OwnerReceipt(const char* reason) noexcept {
    if(!DiagOn())return; const auto now=static_cast<std::uint64_t>(GetTickCount64());
    char m[360]{};std::snprintf(m,sizeof(m),"R59.16C10R11 ACTION_OWNER state=%s frag=%d(%s) action=%p layer=%d t=%.3f holdAt=%.2f heldMs=%llu side=%d reason=%s",
        StateName(g_owner.state),g_owner.fragId,g_owner.frag[0]?g_owner.frag:"-",g_owner.action,g_owner.layer,double(g_owner.lastT),double(g_hold_at.load()),
        (unsigned long long)(g_owner.heldAt?now-g_owner.heldAt:0ull),g_owner.side,reason?reason:"-");
    diagnostic_trace::Write(m);
}
void Publish() noexcept;
bool RestoreSpeed13() noexcept;

bool Owned(const char* frag) noexcept {
    return frag&&IEq(frag,"Picking");
}

// ---- anim event tap (Henry's CSkeletonAnim +0x518) --------------------------------------------------------------------
// R116: bounded copy of an anim-event string for the whoosh check (every event while the switch is on, so no
// per-byte VirtualQuery like CopyStr). false = unreadable or longer than cap-1 (never a weapon flick then).
bool TapStrR116(const char* src,char* dst,std::size_t cap) noexcept {
    if(!dst||cap<2)return false;dst[0]=0;if(!src)return false;
    __try{for(std::size_t i=0;i<cap;++i){const char c=src[i];dst[i]=c;if(!c)return i>0;}}
    __except(EXCEPTION_EXECUTE_HANDLER){dst[0]=0;return false;}
    dst[0]=0;return false;
}
int __fastcall EventTap(void* charInstance,void* userData) noexcept {
    const EventCb orig=g_orig_cb.load(std::memory_order_acquire);
    void* skel=charInstance?static_cast<unsigned char*>(charInstance)+kCharSkelAnim:nullptr;
    // R116: one whoosh per swing. after our release-time whoosh went out (swing_whoosh_r116.h) the clip's own
    // audio_trigger c_w_*flick* event of Henry's skeleton is not passed on; every other event runs unchanged.
    if(skel&&skel==g_tapped_skel.load(std::memory_order_acquire)&&kcd2vr::swing_whoosh_r116::WantsTap()){
        char evName[16]{},evParam[kcd2vr::swing_whoosh_r116::kNameCap]{};
        if(TapStrR116(Rd<const char*>(skel,kEvName,nullptr),evName,sizeof(evName))&&!std::strcmp(evName,"audio_trigger")&&
           TapStrR116(Rd<const char*>(skel,kEvParam,nullptr),evParam,sizeof(evParam))&&
           kcd2vr::swing_whoosh_r116::MuteEvent(evName,evParam,std::uint64_t(GetTickCount64()))){
            if(DiagOn()){char m[160]{};std::snprintf(m,sizeof(m),"R116 SWING_WHOOSH clipEvent=%s t=%.3f skipped=1 reason=EARLY_WHOOSH_PLAYED",
                evParam,double(Rd<float>(skel,kEvTime,-1.f)));diagnostic_trace::Write(m);}
            return 0;
        }
    }
    if(skel&&skel==g_tapped_skel.load(std::memory_order_acquire)&&DiagOn()){
        g_events.fetch_add(1,std::memory_order_relaxed);
        const auto now=static_cast<std::uint64_t>(GetTickCount64());
        if(g_t_event.allow(now,120,30000)){
            char name[48]{},param[48]{},path[96]{};
            CopyStr(Rd<const char*>(skel,kEvName,nullptr),name,sizeof(name));
            CopyStr(Rd<const char*>(skel,kEvParam,nullptr),param,sizeof(param));
            CopyStr(Rd<const char*>(skel,kEvAnimPath,nullptr),path,sizeof(path));
            ScopeName19c4 scopeName{};g_scope_name19c4.read(scopeName);
            char m[420]{};std::snprintf(m,sizeof(m),"R59.16C10R11 ANIM_EVENT name=%s param=%s t=%.3f end=%.3f animId=%d num=%d prio=%.2f path=%s scope0=%s owner=%s",
                name[0]?name:"-",param[0]?param:"-",double(Rd<float>(skel,kEvTime,-1.f)),double(Rd<float>(skel,kEvEndTime,-1.f)),Rd<int>(skel,kEvAnimId,-1),Rd<int>(skel,kEvAnimNum,-1),
                double(Rd<float>(skel,kEvPriority,-1.f)),path[0]?path:"-",scopeName.text,StateName(g_state_pub.load(std::memory_order_relaxed)));
            diagnostic_trace::Write(m);
        }
    }
    if(!orig)return 0;
    __try{return orig(charInstance,userData);}__except(EXCEPTION_EXECUTE_HANDLER){return 0;}
}
bool RemoveTap() noexcept;
void EnsureTap() noexcept {
    void* skel=animation_filter::PlayerSkeletonAnim();
    if(!skel||Rd<std::uintptr_t>(skel,0,0)!=g_base.load(std::memory_order_acquire)+kSkelVft)return;
    if(void* previous=g_tapped_skel.load(std::memory_order_acquire);previous&&previous!=skel&&!RemoveTap())return;
    const EventCb cur=Rd<EventCb>(skel,kSkelCb,nullptr);
    if(cur==&EventTap)return;
    if(!cur)return;                       // the game has not registered its receiver yet
    g_orig_cb.store(cur,std::memory_order_release); g_tapped_skel.store(skel,std::memory_order_release);
    if(Wr<EventCb>(skel,kSkelCb,&EventTap)&&DiagOn()){
        char m[200]{};std::snprintf(m,sizeof(m),"R59.16C10R11 ANIM_EVENT_TAP armed skel=%p original=%p (WHGame+%llX) userData=%p",skel,reinterpret_cast<void*>(cur),
            (unsigned long long)(reinterpret_cast<std::uintptr_t>(cur)-g_base.load()),Rd<void*>(skel,kSkelCbData,nullptr));diagnostic_trace::Write(m);}
}
bool RemoveTap() noexcept {
    void* skel=g_tapped_skel.load(std::memory_order_acquire);const EventCb orig=g_orig_cb.load(std::memory_order_acquire);
    if(!skel)return true;
    EventCb current=nullptr;
    if(!orig||!SehCopy(static_cast<unsigned char*>(skel)+kSkelCb,&current,sizeof(current)))return false;
    if(current==&EventTap&&(!Wr<EventCb>(skel,kSkelCb,orig)||
       !SehCopy(static_cast<unsigned char*>(skel)+kSkelCb,&current,sizeof(current))||current!=orig))return false;
    // A failed restore must retain the callback's original and exact binding.
    // Otherwise the still-reachable EventTap would swallow native events.
    g_tapped_skel.store(nullptr,std::memory_order_release);g_orig_cb.store(nullptr,std::memory_order_release);return true;
}

// ---- owner state machine (game thread, inside CActionController::Update of Henry's controller) -------------------------
// One current-root proof for early presentation, owner-thread Attach and its
// synchronous animation-job transfer. A pickup event alone never identifies the
// IAction currently installed on FullBody as Picking.
struct CurrentRoot57 {
    void* action=nullptr;void* scope=nullptr;void* context=nullptr;
    std::uint32_t token=0,crc=0;int fragment=-1;bool picking=false;
};
static bool ReadCurrentRootIdentity57(void* ctrl,bool requirePicking,CurrentRoot57& out,std::uintptr_t retainedCharacter=0,const char** refusalH17=nullptr) noexcept {
    out={};
    const auto refuse=[&](const char* why) noexcept {if(refusalH17)*refusalH17=why;return false;};
    if(refusalH17)*refusalH17="READY";
    if(!ctrl||ctrl!=g_henry_ctrl.load(std::memory_order_acquire)||
       !retail_build112::Ready(WhBase())||Rd<std::uintptr_t>(ctrl,0,0)!=WhBase()+kCtrlVft)return refuse("CONTROLLER_OR_RETAIL_IDENTITY");
    if(!IsHenryController(ctrl)){
        // Cleanup of an already admitted acquisition must not depend on the
        // transient skinning observer. This fallback cannot arm a new action:
        // only StillPlaying supplies its immutable original character witness.
        const void* contexts=Rd<const void*>(ctrl,kCtrlScopeContexts,nullptr);
        if(!retainedCharacter||!contexts||Rd<std::uintptr_t>(contexts,kScopeContextChar,0)!=retainedCharacter)return refuse("HENRY_CHARACTER_IDENTITY");
    }
    const unsigned count=Rd<unsigned>(ctrl,kCtrlScopeCount,0u);
    void* const scopes=Rd<void*>(ctrl,kCtrlScopes,nullptr);
    void* const context=Rd<void*>(ctrl,kCtrlContext,nullptr);
    if(!scopes||!context||!count||count>kMaxScopes)return refuse("ROOT_CONTEXT_OR_SCOPE_COUNT");
    void* const action=Rd<void*>(scopes,kScopeAction,nullptr);
    const int fragment=action?Rd<int>(action,kActFragId,-1):-1;
    const auto token=action?Rd<std::uint32_t>(action,kActToken,0):0;
    const auto vft=action?Rd<std::uintptr_t>(action,0,0):0;
    std::uint32_t crc=0;
    const int status=action?Rd<int>(action,kActStatus,-1):-1;
    if(action){
        // A pre-Use snapshot is read-only: a retiring native root is a valid
        // BEFORE identity. Requiring Playing here lost the next exact Picking
        // action and its early body/wrist ownership during native transitions.
        // Active Picking writes still require Playing below and at every lease.
        if((requirePicking&&status!=2)||status<0||status>3)return refuse("ACTION_NOT_PLAYING");
        if(Rd<void*>(action,kActRootScope,nullptr)!=scopes)return refuse("ACTION_ROOT_SCOPE_MISMATCH");
        if(Rd<void*>(action,kActContext,nullptr)!=context)return refuse("ACTION_CONTEXT_MISMATCH");
        if(!FragmentCrc19c4(ctrl,fragment,crc))return refuse("FRAGMENT_DEFINITION_UNREADABLE");
    }
    const bool picking=action&&crc==0x03E46E25u;
    if(requirePicking&&!picking)return refuse(action?"FRAGMENT_NOT_PICKING":"ROOT_ACTION_MISSING");
    if(picking&&vft!=WhBase()+0x3A450B0)return refuse("PICKING_ACTION_CLASS_MISMATCH");
    // The pre-Use root may be empty or an ordinary locomotion IAction subclass.
    // Re-read all captured mutable identity fields, not merely root membership.
    if(ctrl!=g_henry_ctrl.load(std::memory_order_acquire)||
       Rd<unsigned>(ctrl,kCtrlScopeCount,0u)!=count||Rd<void*>(ctrl,kCtrlScopes,nullptr)!=scopes||
       Rd<void*>(ctrl,kCtrlContext,nullptr)!=context||Rd<void*>(scopes,kScopeAction,nullptr)!=action||
       (action&&(Rd<std::uintptr_t>(action,0,0)!=vft||
        Rd<int>(action,kActStatus,-1)!=status||
        Rd<void*>(action,kActRootScope,nullptr)!=scopes||Rd<void*>(action,kActContext,nullptr)!=context||
        Rd<int>(action,kActFragId,-1)!=fragment||Rd<std::uint32_t>(action,kActToken,~0u)!=token)))return refuse("ROOT_CHANGED_DURING_PROOF");
    out={action,scopes,context,token,crc,fragment,picking};return true;
}
void ResetOwner() noexcept {RevokeFinishingVisualR94();RetireUse55(g_owner.useSerial54);g_owner=Owner{};Publish();}
bool StillPlaying() noexcept {
    CurrentRoot57 root57{};
    return g_owner.action&&g_owner.scope&&
        ReadCurrentRootIdentity57(g_henry_ctrl.load(std::memory_order_acquire),true,root57,g_owner.useInput55.character)&&
        root57.action==g_owner.action&&root57.scope==g_owner.scope&&root57.token==g_owner.nativeToken;
}
bool WriteSpeed(float v,const char*) noexcept {
    if(!StillPlaying()||!std::isfinite(v))return false;
    float current=0;if(!SehCopy(static_cast<unsigned char*>(g_owner.action)+kActSpeedBias,&current,4)||
        !std::isfinite(current)||current<0||current>8)return false;
    if(!g_owner.haveSavedSpeed){g_owner.savedSpeed=current;g_owner.haveSavedSpeed=true;}
    else if(g_owner.speedWritten&&std::fabs(current-g_owner.lastWrittenSpeed)>1e-6f)return false;
    if(!Wr<float>(g_owner.action,kActSpeedBias,v)||std::fabs(Rd<float>(g_owner.action,kActSpeedBias,-1)-v)>1e-6f)return false;
    g_owner.speedWritten=true;g_owner.lastWrittenSpeed=v;return true;
}
bool RestoreSpeed13() noexcept {
    if(!g_owner.haveSavedSpeed||!g_owner.speedWritten||!StillPlaying())return false;
    const float current=Rd<float>(g_owner.action,kActSpeedBias,-1.f);
    if(!std::isfinite(current)||std::fabs(current-g_owner.lastWrittenSpeed)>1e-6f)return false;
    const bool ok=Wr<float>(g_owner.action,kActSpeedBias,g_owner.savedSpeed);
    if(ok)g_owner.speedWritten=false;return ok;
}
void FinishOwner(const char* reason) noexcept;
// find the scope on which the armed action plays as root
void* FindRootScope(void* ctrl,void* action) noexcept {
    const unsigned count=Rd<unsigned>(ctrl,kCtrlScopeCount,0u); auto* scopes=Rd<unsigned char*>(ctrl,kCtrlScopes,nullptr);
    if(!scopes||count==0||count>kMaxScopes)return nullptr;
    for(unsigned i=0;i<count;++i){void* s=scopes+i*kScopeStride; if(Rd<void*>(s,kScopeAction,nullptr)==action&&Rd<void*>(action,kActRootScope,nullptr)==s)return s;}
    return nullptr;
}
static bool AmmoServiceOwnsT22() noexcept;
static bool AmmoServiceReadinessWantedT22() noexcept;
static void AmmoServiceReadinessT22(const native_script_t2::Context&) noexcept;
static void AmmoServiceScriptT22(const native_script_t2::Context&,const supply_t2::Input&) noexcept;
static bool RestorePickupClipH23() noexcept;
static void ObserveWeaponDropRecordT2(const void*) noexcept;
static bool SupplyLeaseOwnsT2(const weapon_release_t1::Request&) noexcept;
#include "native_grasp59c13.inl"
static void RetireUnownedPickupClockD65(std::uint64_t serial) noexcept;
#include "native_pickup_clock_h23.inl"
#include "native_weapon_drop_t1.inl"
#include "native_supply_t2.inl"
#include "subsystems/ranged_reload/crossbow_native_xr34.inl"
#include "subsystems/throw_drop/runtime/native_ammo_eligibility_t20.inl"
static bool ShootPrologueMatchesD65C(const unsigned char*,std::size_t) noexcept;
#include "subsystems/throw_drop/runtime/native_ammo_launch_t17.inl"
#include "subsystems/ranged_reload/bow_physical_aim_shoot_d65c.inl"
#include "subsystems/throw_drop/runtime/native_ammo_carrier_t18.inl"
#include "subsystems/throw_drop/runtime/native_transient_ammo_t20.inl"
#include "subsystems/throw_drop/runtime/native_paid_ammo_release_t20.inl"
#include "subsystems/throw_drop/runtime/native_private_launcher_t21.inl"
#include "subsystems/throw_drop/runtime/native_preview_retirement_t22.inl"
#include "subsystems/throw_drop/runtime/native_ammo_service_t22.inl"
// Alchemy follows the exact native controller Update callback, which can run
// on a different engine job thread. This scope grants only alchemy clock work
// under the existing transaction; it never changes the generic pickup owner.
// R28: the same scope also covers release/return, mortar and stage cleanup.
// It ends before original Update; native Queue receives a separate local scope.
static thread_local void* g_alchemy_update_controller_r27=nullptr;
struct AlchemyUpdateScopeR27 {
    void* previous=nullptr;
    explicit AlchemyUpdateScopeR27(void* ctrl) noexcept : previous(g_alchemy_update_controller_r27) {
        g_alchemy_update_controller_r27=g_owner_transaction_depth19c4&&ctrl&&
            ctrl==g_henry_ctrl.load(std::memory_order_acquire)&&IsHenryController(ctrl)&&
            retail_build112::Ready(g_base.load(std::memory_order_acquire))?ctrl:nullptr;
    }
    ~AlchemyUpdateScopeR27(){g_alchemy_update_controller_r27=previous;}
};
static bool OnAlchemyUpdateOwnerR27() noexcept {
    return OnOwnerThread13()||(g_owner_transaction_depth19c4&&g_alchemy_update_controller_r27&&
        g_alchemy_update_controller_r27==g_henry_ctrl.load(std::memory_order_acquire));
}
#include "alchemy_stage19c4.inl"
static bool AlchemyPresentationRateD81(const kcd2vr::alchemy_semi46::WorkWitness&,float&) noexcept;
static bool HandoffAlchemyPresentationD81(const kcd2vr::alchemy_semi46::WorkWitness&) noexcept;
#include "alchemy_semi_native46.inl"
#include "alchemy_transfer_clip_d65.inl"
#include "subsystems/alchemy/native_return67.inl"
#include "subsystems/alchemy/native_mortar69.inl"
#include "subsystems/alchemy/instant_native_d72.inl"
#include "subsystems/alchemy/presentation_speed_d81.inl"
#include "semi_smith_clock46.inl"
#include "subsystems/smithing/fall_timing_d81.inl"
#include "subsystems/smithing/instant_native_d72.inl"

static RootObservation55 ReadCurrentRoot55(void* ctrl,std::uint64_t now) noexcept {
    RootObservation55 out{};grasp13::AcquisitionInput55 input{};
    if(!OnPickupOwnerD44()||(!NativeGraspEnabled()&&!PendingPickupObserved63(now))||!g_context13.load(std::memory_order_acquire)||
       ctrl!=g_henry_ctrl.load(std::memory_order_acquire)||!IsHenryController(ctrl)||
       !g_acquisition_input55.read(input)||!FreshAcquisitionInput55(input,now)||
       input.character!=reinterpret_cast<std::uintptr_t>(HenryCharacter18()))return out;
    CurrentRoot57 root57{};
    if(!ReadCurrentRootIdentity57(ctrl,false,root57))return out;
    out.controller=reinterpret_cast<std::uintptr_t>(ctrl);out.character=input.character;
    out.action=reinterpret_cast<std::uintptr_t>(root57.action);out.scope=reinterpret_cast<std::uintptr_t>(root57.scope);
    out.epoch=input.epoch;out.trackingEpoch=input.trackingEpoch;out.tick=now;
    out.generation=g_use_generation55.load(std::memory_order_acquire);
    out.nativeToken=root57.token;out.fragment=root57.fragment;out.picking=root57.picking;out.valid=true;
    return out;
}
static bool CurrentUse55(const Use13& use,const grasp13::AcquisitionInput55& input,std::uint64_t now) noexcept {
    const auto& before=use.before55;
    return CurrentUseInput55(use,input,now)&&
        before.valid&&!before.picking&&before.controller&&before.character&&before.epoch&&
        before.generation==use.generation55&&
        grasp13::fresh(use.tick,before.tick,80)&&input.character==before.character&&input.epoch==before.epoch&&
        input.trackingEpoch==before.trackingEpoch;
}
// Some native Picking actions are installed without passing the observed Queue
// callback on this thread. Match the NEW installed root to a transported Use's
// copied pre-Use root. Reading a currently installed root never revives a queued
// pointer, skips Attach, or grants an item/inventory result.
#include "subsystems/interactions/runtime/native_passenger_a34.inl"
static void ObserveRootAndArm55(void* ctrl,std::uint64_t now) noexcept {
    const auto current=ReadCurrentRoot55(ctrl,now);(void)g_root_before_use55.publish(current);
    if(!NativeGraspEnabled())return; // pre-Use Pickable observation cannot arm any action
    if(g_owner.state!=Idle||g_pick13.attachStarted||!current.valid||!current.picking)return;
    Use13 use{};grasp13::AcquisitionInput55 input{};
    if(!g_use13.read(use)||use.serial==g_consumed_use13||!g_acquisition_input55.read(input)||!CurrentUse55(use,input,now))return;
    const auto& before=use.before55;
    if(current.controller!=before.controller||current.character!=before.character||current.epoch!=before.epoch||
       current.trackingEpoch!=before.trackingEpoch||
       current.tick<use.tick||(current.action==before.action&&current.nativeToken==before.nativeToken))return;
    g_consumed_use13=use.serial;g_owner=Owner{};g_owner.state=Playing;
    g_owner.action=reinterpret_cast<void*>(current.action);g_owner.scope=reinterpret_cast<void*>(current.scope);
    g_owner.fragId=current.fragment;std::snprintf(g_owner.frag,sizeof(g_owner.frag),"Picking");
    g_owner.nativeToken=current.nativeToken;g_owner.side=use.side;g_owner.useSerial54=use.serial;
    g_owner.useGeneration55=use.generation55;g_owner.useInput55=use.input55;g_owner.armedAt=use.tick;
    g_owner.expectedEntityD40=use.expectedEntityD40;
    g_owner.layer=Rd<int>(g_owner.scope,kScopeLayer,0);
    OwnerReceipt("ARMED_AT_NEW_INSTALLED_PICKING_AFTER_TRANSPORTED_USE");Publish();
}

#include "native_unowned_pickup_clock_d65.inl"

// Publication runs only on the current owner transaction. No render-side native
// dereference or action pointer lifetime is granted by this copied witness.
static void PublishAcquisition54() noexcept {
    // A real Attach may run on an animation job between the body producer and
    // its attachment consumers. Publish() used to erase both pose witnesses
    // here merely because that job is not the controller Update thread.
    // Reuse only the receipt already qualified after Attach/hand-key readback;
    // no controller clock, semantic HELD or new input timestamp is granted.
    grasp13::Binding acceptedR92{};
    if(!OnPickupOwnerD44()&&PickupTransferAccess19c4()&&g_owner.state==PendingAttach&&
       !g_owner.releaseUnclaimed55&&!g_foreign_cancel13.load(std::memory_order_acquire)&&
       g_pick13.controllerPausePending19c4&&!g_pick13.finishing&&!g_pick13.cleanupSeen&&
       g_pick13.bind.logic==reinterpret_cast<std::uintptr_t>(g_event_logic13)&&
       ReadPendingAttachmentPresentationD72(acceptedR92)){
        const auto acquisitionR92=acceptedR92.acceptedAcquisitionD72;
        auto foundationR92=acquisitionR92;
        foundationR92.graspToken=foundationR92.graspKey=foundationR92.graspItem=0;
        foundationR92.graspEntity=0; // the shared body basis never grants an item write
        (void)g_acquisition_foundation_d56.publish(foundationR92);
        (void)g_acquisition54.publish(acquisitionR92);
        return;
    }
    grasp13::Acquisition54 out{};const auto now=std::uint64_t(GetTickCount64());
    const bool state=g_owner.state==Armed||g_owner.state==Playing||g_owner.state==PendingAttach||g_owner.state==Held;
    // D56: certify the installed native action independently of a concurrent
    // copy of raw input. BeginCoherent and the camera join fresh tracking and
    // the original body/source epoch themselves. A busy raw-input mailbox must
    // not alternate their shared base between pre-Use and animated pelvis Z.
    // This record never carries a grasp token and cannot place/transfer an item.
    grasp13::Acquisition54 foundation{};
    if(state&&OnPickupOwnerD44()&&NativeGraspAvailable63()&&g_context13.load(std::memory_order_acquire)&&
       !g_owner.releaseUnclaimed55&&g_owner.useSerial54&&g_owner.action&&g_owner.scope&&g_owner.armedAt&&
       g_owner.side>=0&&g_owner.side<2&&IEq(g_owner.frag,"Picking")&&!g_pick13.cleanupSeen&&
       g_owner.useGeneration55==g_use_generation55.load(std::memory_order_acquire)&&
       g_owner.useSerial54>g_use_retired55.load(std::memory_order_acquire)&&
       g_owner.useInput55.character==reinterpret_cast<std::uintptr_t>(HenryCharacter18())&&
       g_owner.useInput55.epoch&&g_owner.useInput55.trackingEpoch&&
       g_pick13.pendingTerminal==grasp13::Terminal::None&&g_pick13.terminal.chosen==grasp13::Terminal::None&&
       (!g_pick13.finishing||(g_pick13.retainAfterCleanup19c4r2&&g_pick13.attached))){
        CurrentRoot57 current{};
        if(ReadCurrentRootIdentity57(g_henry_ctrl.load(std::memory_order_acquire),true,current)&&
           current.action==g_owner.action&&current.scope==g_owner.scope&&current.token==g_owner.nativeToken){
            foundation.character=g_owner.useInput55.character;foundation.action=reinterpret_cast<std::uintptr_t>(g_owner.action);
            foundation.scope=reinterpret_cast<std::uintptr_t>(g_owner.scope);foundation.serial=g_owner.useSerial54;
            foundation.epoch=g_owner.useInput55.epoch;foundation.startedAt=g_owner.armedAt;foundation.tick=now;
            foundation.nativeToken=g_owner.nativeToken;foundation.side=g_owner.side;foundation.active=true;
            foundation.generation55=g_owner.useGeneration55;foundation.trackingEpoch55=g_owner.useInput55.trackingEpoch;
        }
    }
    (void)g_acquisition_foundation_d56.publish(foundation);
    grasp13::AcquisitionInput55 input{};
    if(state&&OnPickupOwnerD44()&&NativeGraspEnabled()&&g_context13.load(std::memory_order_acquire)&&
       !g_owner.releaseUnclaimed55&&g_owner.useSerial54&&g_owner.action&&g_owner.armedAt&&g_owner.side>=0&&g_owner.side<2&&
       g_owner.useGeneration55==g_use_generation55.load(std::memory_order_acquire)&&
       g_owner.useSerial54>g_use_retired55.load(std::memory_order_acquire)&&
       IEq(g_owner.frag,"Picking")&&!g_pick13.cleanupSeen&&
       (!g_pick13.finishing||(g_pick13.retainAfterCleanup19c4r2&&g_pick13.attached&&
         g_pick13.pendingTerminal==grasp13::Terminal::None&&g_pick13.terminal.chosen==grasp13::Terminal::None&&
         HoldPublication13(g_pick13.bind.token)))&&
       g_acquisition_input55.read(input)&&FreshAcquisitionInput55(input,now)&&
       input.character==reinterpret_cast<std::uintptr_t>(HenryCharacter18())&&
       input.character==g_owner.useInput55.character&&input.epoch==g_owner.useInput55.epoch&&
       input.trackingEpoch==g_owner.useInput55.trackingEpoch&&input.tracked[g_owner.side]&&
       std::isfinite(input.grip[g_owner.side])&&input.grip[g_owner.side]>=g_cancel_grip.load(std::memory_order_acquire)){
        const bool queued=g_owner.state==Armed;
        void* ctrl=g_henry_ctrl.load(std::memory_order_acquire);
        CurrentRoot57 root57{};
        const bool active=g_owner.scope&&ReadCurrentRootIdentity57(ctrl,true,root57)&&
            root57.action==g_owner.action&&root57.scope==g_owner.scope&&root57.token==g_owner.nativeToken;
        if(queued||active){
            out.character=input.character;out.action=reinterpret_cast<std::uintptr_t>(g_owner.action);
            out.scope=active?reinterpret_cast<std::uintptr_t>(g_owner.scope):0;
            out.serial=g_owner.useSerial54;out.epoch=input.epoch;out.startedAt=g_owner.armedAt;out.tick=now;
            out.generation55=g_owner.useGeneration55;out.trackingEpoch55=input.trackingEpoch;
            out.nativeToken=g_owner.nativeToken;out.side=g_owner.side;out.active=active;
            const auto& binding=g_pick13.bind;
            if(active&&g_pick13.attachStarted&&binding.token&&binding.logic&&
               binding.character==out.character&&binding.epoch==out.epoch&&binding.side==out.side&&
               g_pick13.pendingTerminal==grasp13::Terminal::None&&g_pick13.terminal.chosen==grasp13::Terminal::None&&
               HoldPublication13(binding.token)){
                out.graspToken=binding.token;out.graspKey=binding.key;out.graspItem=binding.item;out.graspEntity=binding.entity;
            }
        }
    }
    (void)g_acquisition54.publish(out);
}
void Publish() noexcept {
    g_use_owner_pub55.store((g_owner.state==Armed||g_owner.state==Playing)&&
        !g_pick13.attachStarted&&!g_owner.releaseUnclaimed55?g_owner.useSerial54:0,std::memory_order_release);
    PublishAcquisition54();
    g_state_pub.store(g_owner.state,std::memory_order_release);g_side_pub.store(g_owner.side,std::memory_order_release);
    Status x{};x.state=g_owner.state;x.layerTime=g_owner.lastT;
    std::memcpy(x.fragment,g_owner.frag,sizeof(x.fragment));
    const auto now=std::uint64_t(GetTickCount64());x.heldMs=g_owner.heldAt&&now>=g_owner.heldAt?now-g_owner.heldAt:0;
    x.queued=g_queued.load();x.scopeChanges=g_scope_changes.load();x.events=g_events.load();x.holds=g_holds.load();
    x.commits=g_commits.load();x.cancels=g_cancels.load();x.timeouts=g_timeouts.load();
    x.henryController=g_henry_ctrl.load()!=nullptr;x.eventTap=g_tapped_skel.load()!=nullptr;g_status13.publish(x);
}
void FinishOwner(const char* reason) noexcept {
    if(!RestorePickupClipH23())return;
    if(AdoptNativeCleanup19c4())return;
    if(g_pick13.attachStarted&&g_pick13.terminal.chosen==ng13::Terminal::None){
        if(g_pick13.attached)Terminal13(ng13::Terminal::Drop,false,reason);
        else CancelPendingAttach19(reason);
    }
    if(g_pick13.terminal.chosen==ng13::Terminal::Fault){OwnerReceipt("QUARANTINED_NO_TERMINAL_RETRY");return;}
    if(AdoptNativeCleanup19c4())return;
    if(g_pick13.attachStarted)return; // native cleanup has not acknowledged this item yet
    (void)RestoreSpeed13();OwnerReceipt(reason);RememberRetired13();g_pick13={};g_foreign_cancel13.store(false);PublishBinding13();ResetOwner();
}
void TickOwner(void* ctrl,std::uint64_t now) noexcept {
    if(RestoreUnclaimedApproach55())return;
    if(AdoptNativeCleanup19c4())return;
    if(g_pick13.attachStarted&&(g_pick13.pendingTerminal!=ng13::Terminal::None||g_pick13.cleanupSeen)){TickGrasp13(now);return;}
    if(g_owner.state==Idle)return;
    if(g_pick13.terminal.chosen==ng13::Terminal::Fault)return;
    void* scope=FindRootScope(ctrl,g_owner.action);
    // A queued pointer never authorizes a different class/fragment/context that
    // later occupies FullBody, including before a pre-Attach cancel/Finish.
    if(scope&&!g_pick13.attachStarted){
        CurrentRoot57 root57{};
        if(!ReadCurrentRootIdentity57(ctrl,true,root57)||root57.action!=g_owner.action||root57.scope!=scope||
           (g_owner.state!=Armed&&root57.token!=g_owner.nativeToken)){
            OwnerReceipt("CURRENT_ROOT_NOT_EXACT_PICKING_NATIVE_UNCHANGED");ResetOwner();return;
        }
    }
    // Don't dereference a queued/stale action until a live scope owns it.
    if(g_owner.state==Armed){
        if(now<g_owner.armedAt||now-g_owner.armedAt>4000){FinishOwner("QUEUE_NOT_INSTALLED");return;}
        if(!scope)return;
        const int status=Rd<int>(g_owner.action,kActStatus,-1);if(status!=2)return;
        g_owner.scope=scope;g_owner.nativeToken=Rd<std::uint32_t>(g_owner.action,kActToken,0);
        g_owner.layer=Rd<int>(scope,kScopeLayer,0);g_owner.state=Playing;OwnerReceipt("INSTALLED");
    }
    if(!scope||!StillPlaying()){
        FinishOwner("ACTION_SCOPE_ENDED");return;
    }
    const int n=NumAnims(animation_filter::PlayerSkeletonAnim(),g_owner.layer);
    g_owner.lastT=n>0?AnimTime(animation_filter::PlayerSkeletonAnim(),g_owner.layer,n-1):-1.f;
    if(g_pick13.attachStarted){TickGrasp13(now);Publish();return;}
    if(!NativeGraspEnabled()||!g_context13.load()||
       g_owner.useGeneration55!=g_use_generation55.load(std::memory_order_acquire)||
       g_owner.useSerial54<=g_use_retired55.load(std::memory_order_acquire)){
        // Pre-Attach cancellation is safe: no native transfer was intercepted yet.
        if(CallFinish13(g_owner.action))FinishOwner("CANCEL_BEFORE_ATTACH");return;
    }
    // IAction stays at native speed: scaling it caused outer End/cleanup to
    // precede Attach. H23's separate post-Update clip-only lease accelerates the
    // event-bearing clip and restores before each native ownership change.
    // Native Attach remains the only hold trigger; no normalized-time seek.
    // Completed unsupported/native actions simply retire their visual lease.
    Publish();
}

// An Attach job can run/join INSIDE the original controller Update, after our
// normal pre-Update owner tick. Adopt that pending operation on the same
// controller's return, before another native frame can advance past Attach.
// This is not action replay or permission to recover an already-ended action.
static void AdoptPendingPickupAfterUpdateH5(void* ctrl,std::uint64_t now) noexcept {
    const auto binding=g_pick13.bind;
    const auto sourceCurrent=[&]() noexcept {
        ng13::AcquisitionInput55 input{};const auto& original=g_owner.useInput55;
        return g_acquisition_input55.read(input)&&FreshAcquisitionInput55(input,now)&&
            input.character==original.character&&input.epoch==original.epoch&&input.trackingEpoch==original.trackingEpoch&&
            input.sample>=original.sample&&input.tick>=original.tick&&binding.side>=0&&binding.side<2&&
            input.tracked[binding.side]&&std::isfinite(input.grip[binding.side])&&input.grip[binding.side]>=g_cancel_grip.load();
    };
    const auto current=[&]() noexcept {
        return binding.token&&binding.side>=0&&binding.side<2&&OnPickupOwnerD44()&&
            ctrl==g_henry_ctrl.load(std::memory_order_acquire)&&IsHenryController(ctrl)&&
            NativeGraspEnabled()&&g_context13.load(std::memory_order_acquire)&&
            !g_foreign_cancel13.load(std::memory_order_acquire)&&CurrentOwnerInput55(now)&&sourceCurrent()&&
            g_owner.state==PendingAttach&&g_owner.side==binding.side&&
            g_owner.useInput55.character==binding.character&&g_owner.useInput55.epoch==binding.epoch&&
            ng13::same(binding,g_pick13.bind)&&g_pick13.attachStarted&&
            g_pick13.attachedAt&&now>=g_pick13.attachedAt&&now-g_pick13.attachedAt<=1000&&
            g_pick13.controllerPausePending19c4&&!g_pick13.attached&&!g_pick13.finishing&&!g_pick13.cleanupSeen&&
            g_pick13.terminal.chosen==ng13::Terminal::None&&g_pick13.pendingTerminal==ng13::Terminal::None&&
            g_cleanup_claim19c4.load(std::memory_order_acquire)!=binding.token&&
            g_cleanup_done19c4.load(std::memory_order_acquire)!=binding.token&&
            g_outer_callbacks19c4.load(std::memory_order_acquire)==(binding.token<<16)&&
            g_outer_unobserved19c4.load(std::memory_order_acquire)!=binding.token;
    };
    if(!current())return;
    // Raw AcquisitionInput55.sample is sourceNs; completed HandFrame.sample
    // is the XR sequence. Their shared tick can be ordered, their samples cannot.
    const auto handsCurrent=[&]() noexcept {
        ng13::HandFrame hands{};
        return g_hands13.read(hands)&&hands.sample&&hands.character==binding.character&&hands.epoch==binding.epoch&&
            hands.tick>=g_owner.useInput55.tick&&ng13::fresh(now,hands.tick,80)&&hands.tracked[binding.side]&&
            std::isfinite(hands.grip[binding.side])&&hands.grip[binding.side]>=g_cancel_grip.load();
    };
    if(!handsCurrent())return;
    const auto rootCurrent=[&]() noexcept {
        CurrentRoot57 root{};
        return ReadCurrentRootIdentity57(ctrl,true,root)&&root.action==g_owner.action&&
            root.scope==g_owner.scope&&root.token==g_owner.nativeToken;
    };
    void* attachment=nullptr;ng13::Pose world{};
    if(!rootCurrent()||!Identity13()||!NativeAttachment13(attachment,world)||
       !current()||!rootCurrent()||!handsCurrent())return;
    // The existing owner path rechecks cleanup/current root and controls the
    // one native pause, observed attachment, canonical grip and terminal chain.
    TickOwner(ctrl,now);
}

// ---- detours --------------------------------------------------------------------------------------------------------------
#include "subsystems/masterstrike_combos/ms_reaction_owner_d62c.inl"
void __fastcall QueueDetour(void* ctrl,void* action,float time) noexcept {
    (void)RestoreSmithFallBoundaryD81(ctrl);
    (void)RestoreAlchemyPresentationBoundaryD81(ctrl);
    (void)RestoreAlchemyTransferClipD65(ctrl);
    if(PickupClockControllerH23(ctrl)){
        (void)RestorePickupClipH23(); // only this native owner thread mutates the clip lease
    }
    if(g_queue_original)g_queue_original(ctrl,action,time);
    MsReactionQueueD62C(ctrl,action,ctrl==g_henry_ctrl.load(std::memory_order_acquire)); // D62C: paired attack/reaction ownership (8 fragment ids)
    QueueSemiAlchemy46(ctrl,action); // exact queued work child, before native time/event propagation
    SeedSemiSmithClock46(ctrl,action,std::uint64_t(GetTickCount64()));
    const bool capture=DiagOn();
    if(!ctrl||!action||(g_mode.load(std::memory_order_acquire)==Off&&!capture))return;
    // Only the already-qualified Henry controller can arm this owner. NPC
    // queues remain entirely native and never contend with an owned pickup.
    const bool henry=ctrl==g_henry_ctrl.load(std::memory_order_acquire)&&OnOwnerThread13();
    if(!henry)return;
    OwnerTransaction19c4 transaction;if(!transaction)return;
    // Quiet ownership still needs Henry's Picking identity; NPC names and
    // comparison-only names are diagnostic work and never queried without F5.
    if(!capture&&(!henry||!NativeGraspEnabled()))return;
    const auto now=static_cast<std::uint64_t>(GetTickCount64());
    const int fragId=Rd<int>(action,kActFragId,-1);
    char frag[40]{}; CopyStr(FragName(ctrl,fragId),frag,sizeof(frag));
    if(henry){
        g_queued.fetch_add(1,std::memory_order_relaxed);
        if(DiagOn()&&g_t_queue.allow(now,40,8000)){
            char tags[48]{};TagsHex(action,tags,sizeof(tags));
            char m[400]{};std::snprintf(m,sizeof(m),"R59.16C10R11 MANNEQUIN_QUEUE henry=1 ctrl=%p entity=%u action=%p vt=WHGame+%llX frag=%d(%s) tags=%s opt=%d prio=%d status=%d flags=%X queueTime=%.2f token=%u speed=%.2f",
                ctrl,Rd<unsigned>(ctrl,kCtrlEntityId,0u),action,(unsigned long long)(Rd<std::uintptr_t>(action,0,0)-g_base.load()),fragId,frag[0]?frag:"-",tags,Rd<int>(action,kActOption,-1),
                Rd<int>(action,kActPriority,-1),Rd<int>(action,kActStatus,-1),Rd<unsigned>(action,kActFlags,0u),double(time),Rd<unsigned>(action,kActToken,0u),double(Rd<float>(action,kActSpeedBias,0.f)));
            diagnostic_trace::Write(m);
        }
        if(NativeGraspEnabled()&&Owned(frag)){
            Use13 use{};g_use13.read(use);grasp13::AcquisitionInput55 input{};
            const int side=use.side;const auto useTick=use.tick;
            if(use.serial!=g_consumed_use13&&g_acquisition_input55.read(input)&&CurrentUseInput55(use,input,now)&&
               input.character==reinterpret_cast<std::uintptr_t>(HenryCharacter18())){
                if(g_owner.state!=Idle)FinishOwner("REPLACED_BY_NEW_QUEUE");
                if(g_owner.state!=Idle)return;g_consumed_use13=use.serial;
                g_owner.state=Armed;g_owner.useSerial54=use.serial;g_owner.useGeneration55=use.generation55;g_owner.action=action;g_owner.fragId=fragId;std::memcpy(g_owner.frag,frag,sizeof(g_owner.frag));g_owner.side=side;g_owner.armedAt=useTick;g_owner.nativeToken=Rd<std::uint32_t>(action,kActToken,0);
                g_owner.useInput55=use.input55;
                g_owner.expectedEntityD40=use.expectedEntityD40;
                OwnerReceipt("ARMED(queued after a VR hand Use)");Publish();
            } else if(DiagOn()){char m[200]{};std::snprintf(m,sizeof(m),"R59.16C10R11 ACTION_OWNER state=IDLE frag=%s reason=NOT_ARMED(no VR hand Use within 1.5 s; side=%d age=%llu ms)",frag,side,(unsigned long long)(now-useTick));diagnostic_trace::Write(m);}
        }
    } else if(DiagOn()&&frag[0]&&(strstr(frag,"Dog")||strstr(frag,"dog")||strstr(frag,"Pet")||strstr(frag,"Horse"))&&g_t_queue_other.allow(now,10,2000)){
        char m[260]{};std::snprintf(m,sizeof(m),"R59.16C10R11 MANNEQUIN_QUEUE henry=0 ctrl=%p entity=%u frag=%d(%s) status=%d flags=%X queueTime=%.2f",ctrl,Rd<unsigned>(ctrl,kCtrlEntityId,0u),fragId,frag,Rd<int>(action,kActStatus,-1),Rd<unsigned>(action,kActFlags,0u),double(time));
        diagnostic_trace::Write(m);
    }
}
void ScopeReceipts(void* ctrl,std::uint64_t now) noexcept {
    if(!DiagOn())return;
    static std::uint64_t scopeSession=0;
    const auto session=interactionCapture19::gate.session[unsigned(interactionCapture19::Domain::Interaction)].load(std::memory_order_acquire);
    if(scopeSession!=session){scopeSession=session;for(unsigned i=0;i<kMaxScopes;++i){g_scope_last[i]=nullptr;g_scope_last_frag[i]=-1;}std::strcpy(g_scope0_frag,"-");g_scope_name19c4.publish(ScopeName19c4{});}
    const unsigned count=Rd<unsigned>(ctrl,kCtrlScopeCount,0u); auto* scopes=Rd<unsigned char*>(ctrl,kCtrlScopes,nullptr);
    if(!scopes||count==0)return; const unsigned n=count<kMaxScopes?count:kMaxScopes;
    for(unsigned i=0;i<n;++i){
        void* s=scopes+i*kScopeStride; void* a=Rd<void*>(s,kScopeAction,nullptr); const int fragId=a?Rd<int>(a,kActFragId,-1):-1;
        if(a==g_scope_last[i]&&fragId==g_scope_last_frag[i])continue;
        void* prev=g_scope_last[i]; const int prevFrag=g_scope_last_frag[i]; g_scope_last[i]=a; g_scope_last_frag[i]=fragId; g_scope_changes.fetch_add(1,std::memory_order_relaxed);
        char frag[40]{}; CopyStr(FragName(ctrl,fragId),frag,sizeof(frag)); if(i==0){std::snprintf(g_scope0_frag,sizeof(g_scope0_frag),"%s",frag[0]?frag:"-");ScopeName19c4 name{};std::memcpy(name.text,g_scope0_frag,sizeof(name.text));g_scope_name19c4.publish(name);}
        if(!DiagOn()||!g_t_scope.allow(now,60,12000))continue;
        char prevName[40]{}; CopyStr(FragName(ctrl,prevFrag),prevName,sizeof(prevName));
        char m[420]{};std::snprintf(m,sizeof(m),"R59.16C10R11 MANNEQUIN_SCOPE scope=%u layer=%d+%d action=%p vt=WHGame+%llX frag=%d(%s) root=%d status=%d flags=%X speed=%.2f weight=%.2f fragTime=%.2f oneShot=%d prev=%p(%s)",
            i,Rd<int>(s,kScopeLayer,-1),Rd<int>(s,kScopeNumLayers,-1),a,a?(unsigned long long)(Rd<std::uintptr_t>(a,0,0)-g_base.load()):0ull,fragId,frag[0]?frag:"-",a&&Rd<void*>(a,kActRootScope,nullptr)==s?1:0,
            a?Rd<int>(a,kActStatus,-1):-1,a?Rd<unsigned>(a,kActFlags,0u):0u,a?double(Rd<float>(a,kActSpeedBias,0.f)):0.0,a?double(Rd<float>(a,kActAnimWeight,0.f)):0.0,double(Rd<float>(s,kScopeFragTime,0.f)),Rd<unsigned char>(s,kScopeOneShot,0),prev,prevName[0]?prevName:"-");
        diagnostic_trace::Write(m);
    }
    // D30 F5 ANIM (Miki: "better diagnostics for the animations in f5"): 4 Hz snapshot of every Henry scope's current
    // fragment, its fragment time, speed and weight - the same fields MANNEQUIN_SCOPE prints on change, but periodic,
    // so a stuck or never-ending fragment is visible. Henry's controller only, F5 only, no new native call.
    static std::uint64_t lastAnimD30=0;
    if(DiagOn()&&(now<lastAnimD30||now-lastAnimD30>=250)){lastAnimD30=now;
        char line[1400]{};int o=std::snprintf(line,sizeof(line),"F5 ANIM actor=HENRY ctrl=%p scopes=%u",ctrl,n);
        for(unsigned i=0;i<n&&o>0&&o<int(sizeof(line))-120;++i){
            void* sc=scopes+i*kScopeStride;void* a=Rd<void*>(sc,kScopeAction,nullptr);if(!a)continue;
            const int fragId=Rd<int>(a,kActFragId,-1);char frag[40]{};CopyStr(FragName(ctrl,fragId),frag,sizeof(frag));
            o+=std::snprintf(line+o,sizeof(line)-o," s%u=%s(t=%.2f sp=%.2f w=%.2f st=%d)",i,frag[0]?frag:"-",double(Rd<float>(sc,kScopeFragTime,0.f)),
                double(Rd<float>(a,kActSpeedBias,0.f)),double(Rd<float>(a,kActAnimWeight,0.f)),Rd<int>(a,kActStatus,-1));
        }
        diagnostic_trace::Write(line);
    }
}
void __fastcall UpdateDetour(void* ctrl,float timePassed) noexcept {
    (void)RestoreSmithFallBoundaryD81(ctrl);
    (void)RestoreAlchemyPresentationBoundaryD81(ctrl);
    (void)RestoreAlchemyTransferClipD65(ctrl);
    MsReactionUpdateD62C(ctrl); // D62C: only while a paired row lives on THIS controller: membership walk + its own speed lease
    if(PickupClockControllerH23(ctrl)){
        (void)RestorePickupClipH23();
    }else if(g_pickup_clock_live_h23.load(std::memory_order_acquire)&&
             ctrl==g_pickup_clock_controller_d44.load(std::memory_order_acquire)){
        // A busy general owner transaction must not strand clip acceleration
        // on a migrated Update. This independent lease has its own guard;
        // current controller/skeleton/character/pins still must match exactly.
        (void)RestorePickupClipBoundaryD44(ctrl);
    }
    // Quiet Off is O(1), including NPCs. During discovery only an exact Henry
    // match proceeds to our state transaction; no native callback waits on it.
    kcd2vr::door_use46::Intent doorInput46{};
    const bool doorNeeded46=kcd2vr::door_use46::read(doorInput46,GetTickCount64());
    const bool passengerNeededA34=kcd2vr::frik_pose_probe::CombatCensusMode()>0||kcd2vr::frik_pose_probe::WeaponControlMode()>=2;
    const bool needed=ctrl&&(g_pickup_clock_live_h23.load(std::memory_order_acquire)||xr34::Interested()||passengerNeededA34||g_mode.load()!=Off||g_state_pub.load()!=Idle||DiagOn()||weapon_release_t1::Mode()==1||DropWorkPendingT9()||supply_t2::enabled.load()||
        (kcd2vr::ammo_service_t22::integrated&&AmmoServiceReadinessWantedT22())||
        g_alchemy_pending19c4.load()||g_semi_pending46.load()||g_semi_smith_pending46.load()||
        returnAlchemy67::interested.load(std::memory_order_acquire)||mortar69::interested.load(std::memory_order_acquire)||
        kcd2vr::semi_smith46::clockInterested.load(std::memory_order_acquire)||doorNeeded46||
        (semi46::hooksReady.load()&&kcd2vr::activity_input163::witness175.active[0].load())||g_tapped_skel.load());
    const bool candidate=needed&&(ctrl==g_henry_ctrl.load(std::memory_order_acquire)||IsHenryController(ctrl));
    if(!candidate){
        ImpaleDeathSpeedR106(ctrl); // R106: the waiting lethal-thrust victim plays his death clip faster (never Henry)
        if(g_update_original)g_update_original(ctrl,timePassed);
        if(ctrl&&ctrl==g_henry_ctrl.load(std::memory_order_acquire)&&OnOwnerThread13()&&IsHenryController(ctrl))
            kcd2vr::frik_pose_probe::TickNativeDoor19c4();
        return;
    }
    {
    OwnerTransaction19c4 transaction;
    if(transaction){
    const bool capture=DiagOn();
    const bool whooshTapR116=kcd2vr::swing_whoosh_r116::WantsTap(); // R116: the tap also mutes the clip whoosh
    if(!capture&&!whooshTapR116&&ctrl==g_henry_ctrl.load(std::memory_order_acquire)&&OnOwnerThread13()&&
       g_tapped_skel.load(std::memory_order_acquire))RemoveTap();
    if((g_pickup_clock_live_h23.load(std::memory_order_acquire)||xr34::Interested()||passengerNeededA34||g_mode.load(std::memory_order_acquire)!=Off||g_state_pub.load()!=Idle||capture||weapon_release_t1::Mode()==1||DropWorkPendingT9()||supply_t2::enabled.load()||
        (kcd2vr::ammo_service_t22::integrated&&AmmoServiceReadinessWantedT22())||g_semi_pending46.load()||g_semi_smith_pending46.load()||
        kcd2vr::semi_smith46::clockInterested.load(std::memory_order_acquire)||doorNeeded46||
        (semi46::hooksReady.load()&&kcd2vr::activity_input163::witness175.active[0].load()))&&ctrl){
        void* henry=g_henry_ctrl.load(std::memory_order_acquire);
        if(ctrl==henry){ if(!IsHenryController(ctrl)){g_henry_ctrl.store(nullptr,std::memory_order_release);henry=nullptr;if(g_owner.state!=Idle)FinishOwner("HENRY_CONTROLLER_LOST");} }
        else if(IsHenryController(ctrl)){henry=ctrl;g_henry_ctrl.store(ctrl,std::memory_order_release);g_owner_thread13.store(GetCurrentThreadId(),std::memory_order_release);
            if(DiagOn()){char m[200]{};std::snprintf(m,sizeof(m),"R59.16C10R11 HENRY_CONTROLLER ctrl=%p entity=%u scopes=%u timeScale=%.2f",ctrl,Rd<unsigned>(ctrl,kCtrlEntityId,0u),Rd<unsigned>(ctrl,kCtrlScopeCount,0u),double(Rd<float>(ctrl,kCtrlTimeScale,0.f)));diagnostic_trace::Write(m);} }
        if(ctrl==henry&&whooshTapR116&&OnOwnerThread13())EnsureTap(); // R116 (EnsureTap is idempotent, follows a new skeleton)
        if(ctrl==henry){
            XR34PreControllerNative(ctrl,timePassed);
            if(OnOwnerThread13())ServiceWeaponDropT9(); // only XR34 has callback-scoped migration authority
        }
        if(ctrl==henry){
            PickupControllerScopeD44 pickupUpdate(ctrl);
            if(OnPickupControllerD44()){
                // The previous frame's clip lease must be restored on this
                // current callback even when the job scheduler changed thread.
                const bool clockRestored=RestorePickupClipH23();
                if(clockRestored&&(capture||g_owner.state!=Idle||NativeGraspEnabled()||PendingPickupObserved63(GetTickCount64()))){
                    const auto now=static_cast<std::uint64_t>(GetTickCount64());
                    ObserveRootAndArm55(ctrl,now);
                    if(capture){EnsureTap();ScopeReceipts(ctrl,now);}
                    TickOwner(ctrl,now);
                }
            }
        } // no pickup callback authority escapes into original Update
    }
    {
    AlchemyUpdateScopeR27 alchemyUpdate(ctrl);
    if(OnAlchemyUpdateOwnerR27()){
        const auto now=std::uint64_t(GetTickCount64());
        TickInstantAlchemyD72(ctrl,now);
        if(g_alchemy_pending19c4.load(std::memory_order_acquire)||
           (kcd2vr::combat_rules178r10::f5Rung.load()==2&&kcd2vr::workstation178r20c1::experimentArmed36.load()))
            TickAlchemyStage19c4(ctrl,now);
        if(returnAlchemy67::interested.load(std::memory_order_acquire))TickGestureAlchemyReturn67(ctrl,now);
        if(mortar69::interested.load(std::memory_order_acquire))TickMortarStop69(ctrl,now);
        if(g_semi_pending46.load(std::memory_order_acquire)||
           (semi46::hooksReady.load(std::memory_order_acquire)&&kcd2vr::activity_input163::witness175.active[0].load()))
            TickSemiAlchemy46(ctrl,timePassed,now);
        TickAlchemyPresentationD81(ctrl,timePassed,now);
        TickSmithFallD81(ctrl,timePassed,now);
    }
    } // no alchemy callback authority escapes into original Update
    if((g_semi_smith_pending46.load(std::memory_order_acquire)||kcd2vr::semi_smith46::clockInterested.load(std::memory_order_acquire))&&
       ctrl==g_henry_ctrl.load(std::memory_order_acquire)&&OnOwnerThread13())
        TickSemiSmithClock46(ctrl,timePassed,std::uint64_t(GetTickCount64()));
    }
    } // release before native Update: it may join/reenter animation callbacks
    if(g_update_original)g_update_original(ctrl,timePassed);
    if(ctrl==g_henry_ctrl.load(std::memory_order_acquire)&&IsHenryController(ctrl)){
        OwnerTransaction19c4 tx;if(tx){
            const auto now=std::uint64_t(GetTickCount64());
            // D81 presentation speed is applied BEFORE native Update so action
            // completion, procedural work and CAFs use one clock. No second
            // post-update clip-only speed owner may compete with it.
            XR34PostControllerNative(ctrl); // exact Henry callback under transaction; XR34 qualifies its own lease
            ObserveNativePassengerA34(ctrl,now);
            PickupControllerScopeD44 pickupUpdate(ctrl);
            if(OnPickupControllerD44()){
                // A nested native job may reject the transported target while
                // original Update runs. Retire that approach on this return;
                // the rejected native pickup must not receive another clip lease.
                if(!RestoreUnclaimedApproach55()){
                    ObserveRootAndArm55(ctrl,now);
                    AdoptPendingPickupAfterUpdateH5(ctrl,now);
                    TickPickupClipH23(now);
                    TickUnownedPickupClipD65(ctrl,now);
                }
            }
        }
        if(OnOwnerThread13())kcd2vr::frik_pose_probe::TickNativeDoor19c4();
    }
}

} // namespace

bool Install() noexcept {
    if(g_state_pub.load()!=Idle)return false;
    Uninstall();
    // A failed disarm may still route into an old inline callback. Its original
    // trampoline and handle must survive; never replace that partially installed set.
    if(g_update_hook||g_queue_hook||g_tapped_skel.load(std::memory_order_acquire))return false;
    for(const auto& h:g_native_hooks13)if(h)return false;
    for(const auto& h:g_crossbow_hooks_xr34)if(h)return false;
    const auto b=WhBase(); if(!ExactBuild(b)||!(retail_build112::Ready(b)||retail_build112::Verify(b)))return false;
    const bool pins=MatchBytes(b+kCtrlQueue,kQueuePin,sizeof(kQueuePin))&&MatchBytes(b+kCtrlUpdate,kUpdatePin,sizeof(kUpdatePin))&&MatchBytes(b+native_grasp13_pins::kFragmentDefinition,native_grasp13_pins::pFragmentDefinition,sizeof(native_grasp13_pins::pFragmentDefinition))&&
        MatchBytes(b+native_grasp13_pins::kFragmentCrcLookup,native_grasp13_pins::pFragmentCrcLookup,sizeof(native_grasp13_pins::pFragmentCrcLookup))&&
        MatchBytes(b+kSkelDispatch,kDispatchPin,sizeof(kDispatchPin))&&MatchBytes(b+kSkelNumAnims,kNumAnimsPin,sizeof(kNumAnimsPin))&&MatchBytes(b+kSkelAnimTime,kAnimTimePin,sizeof(kAnimTimePin))&&MatchBytes(b+kSkelSetLayerTime,kSetLayerTimePin,sizeof(kSetLayerTimePin));
    bool nativePins=true;for(const auto& p:np13::all)nativePins=nativePins&&MatchBytes(b+p.rva,p.bytes,p.size);
    if(!pins||!nativePins){diagnostic_trace::Write("R59.16C10R11 ACTION_OWNER pins mismatch; Mannequin receipts and the action time owner stay OFF (fail-closed)");return false;}
    auto install13=[&](unsigned index,const char* name,std::uintptr_t rva,auto fn,auto& orig)->bool {
        DetourModKit::hook::InlineRequest r{name,DetourModKit::hook::Target{DetourModKit::Address{b+rva}},DetourModKit::hook::Options{}};
        auto h=DetourModKit::hook::inline_at(std::move(r),fn);if(!h)return false;
        g_native_hooks13[index]=std::move(*h);orig=g_native_hooks13[index]->original<std::decay_t<decltype(orig)>>();
        return orig&&g_native_hooks13[index]->enable();
    };
    bool nativeOk=install13(0,"kcd2vr_grasp13_pickup_event",np13::kPickupEvent,&PickupEventDetour13,g_pick_event13)&&
        install13(1,"kcd2vr_grasp13_pickup_transfer",np13::kPickupTransfer,&PickupTransferDetour13,g_pick_transfer13)&&
        install13(2,"kcd2vr_grasp13_pickup_cleanup",np13::kPickupCleanup,&PickupCleanupDetour13,g_pick_cleanup13)&&
        install13(3,"kcd2vr_grasp13_pickup_notice",np13::kPickupNotice,&PickupNoticeDetour13,g_notice13)&&
        install13(4,"kcd2vr_grasp13_drop_queue",np13::kDropQueue,&DropQueueDetour13,g_drop_queue13)&&
        // C10R18: function-entry hook on the attachment world getter (filtered by caller), never a mid hook inside 1085770's loop
        install13(5,"kcd2vr_grasp18_attachment_world",np13::kAttachmentWorld,&AttachmentWorldDetour13,g_attachment_world13)&&
        install13(6,"kcd2vr_grasp19c4_pickup_end",np13::kPickupEnd,&PickupEndDetour19c4,g_pick_end19c4);
    if(!nativeOk){Uninstall();diagnostic_trace::Write("C10R13 NATIVE_GRASP INSTALL_FAILED rolledBack=1");return false;}
    DetourModKit::hook::InlineRequest q{"kcd2vr_r59c10r11_mannequin_queue",DetourModKit::hook::Target{DetourModKit::Address{b+kCtrlQueue}},DetourModKit::hook::Options{}};
    auto hq=DetourModKit::hook::inline_at(std::move(q),&QueueDetour); if(!hq){Uninstall();return false;}
    g_queue_hook=std::move(*hq); g_queue_original=g_queue_hook->original<QueueFn>();
    if(!g_queue_original||!g_queue_hook->enable()){Uninstall();return false;}
    DetourModKit::hook::InlineRequest u{"kcd2vr_r59c10r11_mannequin_update",DetourModKit::hook::Target{DetourModKit::Address{b+kCtrlUpdate}},DetourModKit::hook::Options{}};
    auto hu=DetourModKit::hook::inline_at(std::move(u),&UpdateDetour); if(!hu){Uninstall();return false;}
    g_update_hook=std::move(*hu); g_update_original=g_update_hook->original<UpdateFn>();
    if(!g_update_original||!g_update_hook->enable()){Uninstall();return false;}
    g_native_ready13.store(true,std::memory_order_release);
    g_installed.store(true,std::memory_order_release);
    (void)InstallWeaponDropConsumerT2();
    if(kcd2vr::ammo_service_t22::integrated&&!AmmoServiceInstallT22())diagnostic_trace::Write("T22 AMMO readiness=UNAVAILABLE noNativeCarrierAdmission=1");
    (void)InstallBowShootD65C(); // optional; rejection leaves native shooting intact
    (void)InstallSupplyT2();
    g_alchemy_stop19c4.store(false,std::memory_order_release);
    g_alchemy_transfer_stop_d65.store(false,std::memory_order_release);
    (void)InstallSemiAlchemy46(); // optional exact-work adapters; original action owners survive refusal
    (void)InstallInstantAlchemyD72();
    (void)InstallSemiSmithClock46();
    (void)InstallInstantSmithD72();
    const bool crossbowNativeXR34=InstallCrossbowNativeXR34();
    g_crossbow_registration_ready_xr34.store(crossbowNativeXR34&&g_semi_scan_original46&&
        g_semi_hooks46[2]&&g_semi_hooks46[2]->is_enabled()&&g_update_original&&g_update_hook&&g_update_hook->is_enabled(),std::memory_order_release);
    kcd2vr::frik_pose_probe::StartNativeDoor19c4();
    diagnostic_trace::Write("C10R18 NATIVE_GRASP READY exactRetail=SHA_AND_PINS arm=NATIVE_ATTACH_EVENT(C_PickUpItemLogic) hold=ATTACH_ONLY inventory=DEFERRED_TO_STOW drop=NATIVE_HAND_DROP pose=GETTER_ENTRY_HOOK(0x423974 caller 0x108583A) wrist=CONTROLLER");
    return true;
}
static void UninstallDeferred19c4(unsigned bit,const char* reason) noexcept {
    g_mode.store(Off);g_context13.store(false);
    static std::atomic<unsigned> reported{0};
    if(reported.fetch_or(bit,std::memory_order_relaxed)&bit)return;
    char line[240]{};std::snprintf(line,sizeof(line),"C19C4R1 NATIVE_GRASP SHUTDOWN_DEFERRED reason=%s hooks=RETAINED nativeOwnerCleanupRequired=1 externalQuiescenceRequired=1",reason);
    diagnostic_trace::Write(line);
}
void Uninstall() noexcept {
    g_alchemy_transfer_stop_d65.store(true,std::memory_order_release);
    g_semi_smith_stop46.store(true,std::memory_order_release);
    if(kcd2vr::instant_pickup_d72::active.load(std::memory_order_acquire)){
        kcd2vr::instant_pickup_d72::stopping.store(true,std::memory_order_release);
        UninstallDeferred19c4(8192,"INSTANT_WORLD_PICKUP_RELEASE_PENDING");return;
    }
    g_crossbow_registration_ready_xr34.store(false,std::memory_order_release);
    kcd2vr::crossbow_native_xr34::WithdrawRegistration();
    (void)kcd2vr::native_passenger_a34::witness.publish({});
    OwnerTransaction19c4 transaction;if(!transaction){UninstallDeferred19c4(1,"OWNER_TRANSACTION_BUSY");return;}
    // Shutdown is not an action callback: a worker may revoke permission but
    // cannot manufacture callback ownership to restore native action memory.
    if(g_alchemy_presentation_pending_d81.load()&&(!OnOwnerThread13()||!RestoreAlchemyPresentationD81())){
        UninstallDeferred19c4(16384,"ALCHEMY_PRESENTATION_RESTORE_PENDING");return;}
    if(g_smith_fall_pending_d81.load()&&(!OnOwnerThread13()||!RestoreSmithFallD81())){
        UninstallDeferred19c4(32768,"SMITH_FALL_RESTORE_PENDING");return;}
    if(!StopCrossbowNativeXR34()){UninstallDeferred19c4(128,"CROSSBOW_RESTORE_PENDING");return;}
    if(!RestorePickupClipH23()){UninstallDeferred19c4(64,"PICKUP_CLIP_RESTORE_PENDING");return;}
    // A worker shutdown cannot discard a game-thread-owned zero-speed lease.
    // Retain the existing update hook until compare-owned restoration completes.
    if(!StopAlchemyStage19c4()){UninstallDeferred19c4(2,"ALCHEMY_RESTORE_PENDING");return;}
    if(!StopInstantAlchemyD72()){UninstallDeferred19c4(2048,"INSTANT_ALCHEMY_NATIVE_COMPLETION_PENDING");return;}
    if(!StopSemiAlchemy46()){UninstallDeferred19c4(16,"SEMI_ALCHEMY_RESTORE_PENDING");return;}
    if(!StopAlchemyTransferClipD65()){UninstallDeferred19c4(256,"ALCHEMY_TRANSFER_CLIP_RESTORE_PENDING");return;}
    if(!StopInstantSmithD72()){UninstallDeferred19c4(4096,"INSTANT_SMITH_PUMP_RELEASE_PENDING");return;}
    if(!StopSemiSmithClock46()){UninstallDeferred19c4(32,"SEMI_SMITH_RESTORE_PENDING");return;}
    if(!kcd2vr::frik_pose_probe::StopNativeDoor19c4()){UninstallDeferred19c4(4,"DOOR_RESTORE_PENDING");return;}
    if(g_state_pub.load()!=Idle){
        g_mode.store(Off);g_context13.store(false);
        if(OnOwnerThread13())FinishOwner("UNINSTALL_CANCEL");
        // Never unload callback code while the game still owns an unresolved item.
        if(g_state_pub.load()!=Idle){UninstallDeferred19c4(8,"HELD_ITEM_CLEANUP_PENDING");return;}
    }
    g_native_ready13.store(false);g_pick13={};PublishBinding13();g_owner=Owner{};Publish();
    // DMK explicitly permits disable() failure while an INLINE hook remains
    // reachable. Destruction may then pin the backend, so clearing our original
    // function pointers would turn a retained hook into swallowed native events
    // (or a null world-pose return). Keep the complete set until disarm is proved.
    // As before, genuine DLL unload also requires external callback quiescence:
    // inline hooks do not provide callback rundown merely by being disabled.
    const auto disarm=[](auto& h) noexcept {
        if(!h||!h->is_enabled())return true; // C10R19 (review): never ask DMK to disable a hook that is not enabled (repeat Uninstall)
        const auto result=h->disable();
        return bool(result)&&!h->is_enabled();
    };
    bool disabled=DisarmBowShootD65C();
    disabled=DisarmCrossbowNativeXR34()&&disabled;
    disabled=DisarmInstantAlchemyD72()&&disabled;
    disabled=DisarmSemiAlchemy46()&&disabled;
    disabled=DisarmInstantSmithD72()&&disabled;
    disabled=DisarmSemiSmithClock46()&&disabled;
    disabled=disarm(g_update_hook)&&disabled;
    const bool queueDisabled=disarm(g_queue_hook);disabled=queueDisabled&&disabled;
    for(auto& h:g_native_hooks13){const bool one=disarm(h);disabled=one&&disabled;}
    if(!disabled){
        g_mode.store(Off);g_context13.store(false);g_installed.store(true,std::memory_order_release);
        RemoveTap(); // C10R19 (review): the skeleton callback does not depend on the inline hooks; give it back to the game
        static std::atomic<bool> noted{false};
        if(!noted.exchange(true))diagnostic_trace::Write("C10R18 NATIVE_GRASP RETAINED_LIVE_HOOK disarm=UNPROVED originals=RETAINED replacement=REFUSED externalQuiescenceRequired=1");
        return;
    }
    if(!RemoveTap()){
        g_mode.store(Off);g_context13.store(false);g_installed.store(true,std::memory_order_release);return;
    }
    g_henry_ctrl.store(nullptr);g_owner_thread13.store(0);
    g_update_hook.reset();g_update_original=nullptr;
    g_queue_hook.reset();g_queue_original=nullptr;
    for(auto& h:g_native_hooks13)h.reset();
    g_pick_event13=nullptr;g_pick_transfer13=nullptr;g_pick_cleanup13=nullptr;g_pick_end19c4=nullptr;g_notice13=nullptr;g_drop_queue13=nullptr;g_attachment_world13=nullptr;
    g_installed.store(false);
}
bool Installed() noexcept {return g_installed.load(std::memory_order_acquire);}
bool NativePickupOutstandingD72() noexcept {
    const int state=g_state_pub.load(std::memory_order_acquire);return state!=Idle&&state!=Armed;
}
bool CrossbowNativeReadyXR34() noexcept {return g_installed.load(std::memory_order_acquire)&&g_crossbow_registration_ready_xr34.load(std::memory_order_acquire);}
bool CrossbowNativeAdmissionBusyXR34() noexcept {return g_crossbow_inflight_xr34.load(std::memory_order_acquire)!=0;}
bool WristOwnedByController(int side) noexcept {
    if(!NativeGraspEnabled()||side<0||side>1||g_side_pub.load(std::memory_order_acquire)!=side)return false;
    const int st=g_state_pub.load(std::memory_order_relaxed);return st==Playing||st==PendingAttach||st==Held||st==Released||st==Cancelling;
}
void SetMode(int mode) noexcept {
    const int m=mode<0?0:mode>2?2:mode; if(g_mode.exchange(m,std::memory_order_acq_rel)==m)return;
    g_use_generation55.fetch_add(1,std::memory_order_acq_rel);
    if(m!=Own&&OnOwnerThread13())(void)RestorePickupClipH23();
    char line[160]{};std::snprintf(line,sizeof(line),"R59.16C10R11 ACTION_OWNER mode=%d (%s) installed=%d",m,m==Own?"OWN":m==Observe?"OBSERVE":"OFF",Installed()?1:0);diagnostic_trace::Write(line);
}
int Mode() noexcept {return g_mode.load(std::memory_order_acquire);}
void SetAlchemyTransferSpeedD65(float rate) noexcept {g_alchemy_transfer_rate_d65.store(transfer65::rate(rate),std::memory_order_release);}
float AlchemyTransferSpeedD65() noexcept {return g_alchemy_transfer_rate_d65.load(std::memory_order_acquire);}
void SetUnownedPickupClockD65(bool on) noexcept {g_unowned_pickup_clock_d65.store(on,std::memory_order_release);}
bool UnownedPickupClockD65() noexcept {return g_unowned_pickup_clock_d65.load(std::memory_order_acquire);}
void SetPickupHold62(bool on) noexcept {
    if(g_pickup_hold62.exchange(on,std::memory_order_acq_rel)!=on)
        g_use_generation55.fetch_add(1,std::memory_order_acq_rel);
}
void SetPickupExperiment62(bool on) noexcept {
    if(g_pickup_experiment62.exchange(on,std::memory_order_acq_rel)!=on)
        g_use_generation55.fetch_add(1,std::memory_order_acq_rel);
}
void SetTuning(const Tuning& t) noexcept {
    g_hold_at.store(grasp_transaction59c12c::SafeHoldAt(t.holdAt));g_resume_speed.store(t.resumeSpeed<0.25f?0.25f:t.resumeSpeed>8.f?8.f:t.resumeSpeed);
    g_hold_timeout.store(t.holdTimeoutMs<300u?300u:t.holdTimeoutMs>15000u?15000u:t.holdTimeoutMs);g_commit_pull.store(std::isfinite(t.commitPullM)?std::clamp(t.commitPullM,.02f,.6f):.10f);g_cancel_grip.store(std::isfinite(t.cancelGrip)?std::clamp(t.cancelGrip,.05f,.9f):.35f);g_cancel_mode.store(t.cancelMode==0?0:1);
}
void SetPropPresentation35(float outwardMeters,float stowRadiusMeters) noexcept {
    grasp13::setPresentation35(outwardMeters,stowRadiusMeters);
}
Tuning GetTuning() noexcept {Tuning t;t.holdAt=g_hold_at.load();t.resumeSpeed=g_resume_speed.load();t.holdTimeoutMs=g_hold_timeout.load();t.commitPullM=g_commit_pull.load();t.cancelGrip=g_cancel_grip.load();t.cancelMode=g_cancel_mode.load();return t;}
void SetOwnedFragments(const char*) noexcept {
    // Retained INI/API compatibility only. Exact Picking is the sole qualified
    // semantic owner; CSV strings cannot enable/disable its Attach fallback or
    // authorize unproven fragments. Pickup preference is SetPickupHold62.
}
static void NoteUseSent63(int side,std::uint64_t nowMs,bool physicalPickup,std::uint32_t expectedEntity=0) noexcept {
    if(side<0||side>1||!nowMs)return;
    Use13 use{side,nowMs,g_use_serial13.fetch_add(1)+1};use.physicalPickup63=physicalPickup;RootObservation55 before{};
    use.expectedEntityD40=physicalPickup?expectedEntity:0;
    use.generation55=g_use_generation55.load(std::memory_order_acquire);
    (void)g_acquisition_input55.read(use.input55);
    if((NativeGraspEnabled()||(physicalPickup&&NativeGraspAvailable63()))&&g_context13.load(std::memory_order_acquire)&&g_root_before_use55.read(before)&&
       before.valid&&!before.picking&&before.generation==use.generation55&&grasp13::fresh(nowMs,before.tick,80))use.before55=before;
    g_use13.publish(use);
}
void NoteUseSent(int side,std::uint64_t nowMs) noexcept {NoteUseSent63(side,nowMs,false);}
bool NativeGraspAvailable63() noexcept {
    return g_mode.load()==Own&&g_native_ready13.load()&&!g_native_fault13.load();
}
void PreparePhysicalPickup63(bool pending,int side,std::uint64_t nowMs) noexcept {
    PendingPhysicalPickup63 p{};
    if(pending&&side>=0&&side<2&&NativeGraspAvailable63()&&g_context13.load(std::memory_order_acquire)&&
       g_acquisition_input55.read(p.source)&&FreshAcquisitionInput55(p.source,nowMs)&&p.source.tracked[side]&&
       std::isfinite(p.source.grip[side])&&p.source.grip[side]>=g_cancel_grip.load(std::memory_order_acquire)){
        p.side=side;p.generation=g_use_generation55.load(std::memory_order_acquire);
    }
    (void)g_pickup_pending63.publish(p);
}
void NotePhysicalPickupUse63(int side,std::uint64_t nowMs,std::uint32_t expectedEntity) noexcept {
    if(!expectedEntity||!NativeGraspAvailable63()||!g_context13.load(std::memory_order_acquire))return;
    // Producer calls only after actual contextual Use transport succeeds. This
    // expectation shares that serial's original input, generation and epochs;
    // later hovering cannot replace it or authorize a different native item.
    NoteUseSent63(side,nowMs,true,expectedEntity);
}
void NoteUseInput(bool pulseSent,bool heldSent,int side,std::uint64_t nowMs) noexcept {
    // Called by the one existing input producer AFTER its actual transport result.
    // Both a tap and the F5=2 ordinary-mode held route can start Picking. Holding
    // the same request never renews its timestamp or creates another action token.
    static thread_local int heldSide=-1;
    static thread_local std::uint64_t previousTick=0;
    if(!nowMs||nowMs<previousTick){heldSide=-1;previousTick=nowMs;return;}
    previousTick=nowMs;
    const int next=heldSent&&side>=0&&side<2?side:-1;
    const bool edge=next>=0&&next!=heldSide;heldSide=next;
    if((pulseSent||edge)&&side>=0&&side<2)NoteUseSent(side,nowMs);
}
void SetContextAvailable(bool allowed) noexcept {
    if(!allowed&&OnOwnerThread13())(void)RestorePickupClipH23();
    if(g_context13.exchange(allowed,std::memory_order_acq_rel)!=allowed)g_use_generation55.fetch_add(1,std::memory_order_acq_rel);
}
void PublishAcquisitionInput55(const grasp13::AcquisitionInput55& input) noexcept {
    // Raw valid opening retires only the previously consumed pickup Grip;
    // presentation gates or tracking loss do not manufacture a new squeeze.
    kcd2vr::pickup_grip_d64::retirement.observe(input);
    Use13 use{};
    if(g_use13.read(use)&&UseLive55(use)&&use.side>=0&&use.side<2){
      const bool sameSourceOwner=input.character==use.input55.character&&input.epoch==use.input55.epoch&&
          input.trackingEpoch==use.input55.trackingEpoch;
      // Transport consumption may occur AFTER a newer physical source sample.
      // Compare original source clocks, never the later transported Use clock.
      const bool observedAfterUseInput=!sameSourceOwner||!input.tick||!input.sample||
          (input.tick>=use.input55.tick&&input.sample>=use.input55.sample);
      if(observedAfterUseInput&&(!sameSourceOwner||!input.tick||!input.sample||
        !input.headTracked||!input.tracked[use.side]||!std::isfinite(input.grip[use.side])||
        (input.grip[use.side]<g_cancel_grip.load(std::memory_order_acquire)&&!PhysicalApproachInterest55(use))||
        !use.input55.sample))RetireUse55(use.serial);
    }
    (void)g_acquisition_input55.publish(input);
}
void PublishHands(const grasp13::HandFrame& frame) noexcept {
    // This completed-body frame retains native chest/placement/terminal use.
    // A presentation gate is not proof that raw physical tracking was lost.
    (void)g_hands13.publish(frame);
}
bool NativeGraspEnabled() noexcept {
    const auto rung=kcd2vr::combat_rules178r10::f5Rung.load(std::memory_order_acquire);
    const bool test=g_pickup_experiment62.load(std::memory_order_acquire)&&rung>=2&&rung<=3&&
        kcd2vr::workstation178r20c1::experimentArmed36.load(std::memory_order_acquire);
    if(!NativeGraspAvailable63())return false;
    if(g_pickup_hold62.load(std::memory_order_acquire)||test)return true;
    Use13 use{};grasp13::AcquisitionInput55 input{};
    const bool physical=g_use13.read(use)&&use.physicalPickup63&&UseLive55(use)&&use.side>=0&&use.side<2&&
        g_acquisition_input55.read(input)&&FreshAcquisitionInput55(input,std::uint64_t(GetTickCount64()))&&
        input.character==use.input55.character&&input.epoch==use.input55.epoch&&input.trackingEpoch==use.input55.trackingEpoch&&
        input.tracked[use.side]&&std::isfinite(input.grip[use.side])&&
        (input.grip[use.side]>=g_cancel_grip.load(std::memory_order_acquire)||PhysicalApproachInterest55(use));
    return physical;
}
// D85 interact: read-only. The physical Use was transported by the VR hand that
// squeezed; the native pickup request reads the side in the same frame (D79:
// Use to request ~90-110 ms). 400 ms bounds a stale serial without timers.
bool RequestedPickupSideD85(std::uint32_t entity,int& side) noexcept {
    side=-1;Use13 use{};grasp13::AcquisitionInput55 input{};const auto now=std::uint64_t(GetTickCount64());
    if(!entity||!g_use13.read(use)||!use.physicalPickup63||use.expectedEntityD40!=entity||
       !g_acquisition_input55.read(input)||!CurrentUseInput55(use,input,now,400))return false;
    side=use.side;return true;
}
bool ReadPendingUse55(grasp13::PendingUse55& out) noexcept {
    out={};Use13 use{};grasp13::AcquisitionInput55 input{};
    if(!NativeGraspEnabled()||!g_context13.load(std::memory_order_acquire)||
       !g_use13.read(use)||!g_acquisition_input55.read(input)||!CurrentUse55(use,input,std::uint64_t(GetTickCount64())))return false;
    out.character=use.before55.character;out.epoch=use.before55.epoch;out.serial=use.serial;
    out.requestedAt=use.tick;out.side=use.side;return true;
}
bool ReadAcquisition54(grasp13::Acquisition54& out) noexcept {
    out={};grasp13::AcquisitionInput55 input{};const auto now=std::uint64_t(GetTickCount64());
    return NativeGraspEnabled()&&g_context13.load(std::memory_order_acquire)&&
        g_acquisition54.read(out)&&grasp13::freshAcquisition54(out,now)&&out.generation55&&
        out.generation55==g_use_generation55.load(std::memory_order_acquire)&&
        out.serial>g_use_retired55.load(std::memory_order_acquire)&&
        g_acquisition_input55.read(input)&&FreshAcquisitionInput55(input,now)&&
        input.character==out.character&&input.epoch==out.epoch&&input.trackingEpoch==out.trackingEpoch55&&
        input.tracked[out.side]&&std::isfinite(input.grip[out.side])&&
        input.grip[out.side]>=g_cancel_grip.load(std::memory_order_acquire)&&
        (!out.graspToken||HoldPublication13(out.graspToken))&&
        out.generation55==g_use_generation55.load(std::memory_order_acquire)&&
        out.serial>g_use_retired55.load(std::memory_order_acquire);
}
bool ReadAcquisitionFoundationD56(grasp13::Acquisition54& out) noexcept {
    out={};const auto now=std::uint64_t(GetTickCount64());
    const auto generation=g_use_generation55.load(std::memory_order_acquire);
    return NativeGraspAvailable63()&&g_context13.load(std::memory_order_acquire)&&
        g_acquisition_foundation_d56.read(out)&&out.active&&grasp13::freshAcquisition54(out,now)&&
        out.generation55&&out.generation55==generation&&out.trackingEpoch55&&
        out.serial>g_use_retired55.load(std::memory_order_acquire)&&
        generation==g_use_generation55.load(std::memory_order_acquire)&&
        out.serial>g_use_retired55.load(std::memory_order_acquire)&&g_context13.load(std::memory_order_acquire);
}
bool ReadGraspBinding(grasp13::Binding& b) noexcept {return NativeGraspEnabled()&&g_binding13.read(b)&&b.token&&(b.attached||b.socketOnly28)&&HoldPublication13(b.token);}
bool ReadPendingAttachmentPresentationD72(grasp13::Binding& out) noexcept {
    out={};grasp13::Binding b{},after{};grasp13::AcquisitionInput55 input{};
    const auto now=std::uint64_t(GetTickCount64());
    if(!NativeGraspEnabled()||g_native_fault13.load(std::memory_order_acquire)||
       !g_context13.load(std::memory_order_acquire)||!g_binding13.read(b))return false;
    const auto& a=b.acceptedAcquisitionD72;
    if(!grasp13::pendingAttachmentPresentationD72(b,a,now)||!a.generation55||!a.trackingEpoch55||
       a.generation55!=g_use_generation55.load(std::memory_order_acquire)||
       a.serial<=g_use_retired55.load(std::memory_order_acquire)||
       !g_acquisition_input55.read(input)||!FreshAcquisitionInput55(input,now)||
       input.character!=a.character||input.epoch!=a.epoch||input.trackingEpoch!=a.trackingEpoch55||
       !input.tracked[a.side]||!std::isfinite(input.grip[a.side])||input.grip[a.side]<g_cancel_grip.load(std::memory_order_acquire)||
       !HoldPublication13(b.token)||!g_binding13.read(after)||!grasp13::same(b,after)||
       after.acceptedAttachTickD72!=b.acceptedAttachTickD72||
       !grasp13::pendingAttachmentPresentationD72(after,after.acceptedAcquisitionD72,now)||
       !grasp13::sameAcquisition55(a,after.acceptedAcquisitionD72)||!HoldPublication13(b.token)||
       a.generation55!=g_use_generation55.load(std::memory_order_acquire)||
       a.serial<=g_use_retired55.load(std::memory_order_acquire)||!g_context13.load(std::memory_order_acquire))return false;
    out=b;return true;
}
static bool ReadFinishingVisualImplR94(grasp13::FinishingPresentationR94& out,std::uint64_t token,bool allowAdopted) noexcept {
    out={};grasp13::FinishingPresentationR94 p{};grasp13::Binding current{};
    grasp13::AcquisitionInput55 input{};
    const auto live=[&]() noexcept {
        return token&&NativeGraspEnabled()&&g_context13.load(std::memory_order_acquire)&&
            !g_native_fault13.load(std::memory_order_acquire)&&!g_foreign_cancel13.load(std::memory_order_acquire)&&
            g_finishing_continuity_token_r94.load(std::memory_order_acquire)==token&&
            (g_finishing_visual_token_r94.load(std::memory_order_acquire)==token||
             (allowAdopted&&g_free_hold19c4r2.load(std::memory_order_acquire)==token))&&
            g_cleanup_requested19c4.load(std::memory_order_acquire)==token&&
            g_cleanup_claim19c4.load(std::memory_order_acquire)==token&&
            g_cleanup_original19c4.load(std::memory_order_acquire)==token&&
            g_cleanup_done19c4.load(std::memory_order_acquire)==token&&
            g_outer_callbacks19c4.load(std::memory_order_acquire)==(token<<16)&&
            g_outer_unobserved19c4.load(std::memory_order_acquire)!=token&&
            (allowAdopted||g_free_hold19c4r2.load(std::memory_order_acquire)!=token);
    };
    // R95: the cleanup bridge expires after 80 ms until semantic adoption.
    // Once the exact item is genuinely Held, fresh physical input and the live
    // held token authorize presentation while the independent gameplay cache
    // catches up. Never move requestedAt forward to manufacture freshness.
    if(!live()||!g_finishing_visual_r94.read(p)||!g_acquisition_input55.read(input))return false;
    // Validate the exact copied input against a clock sampled AFTER it. A new
    // XR input published during this read is not a future/stale source.
    const auto now=std::uint64_t(GetTickCount64());
    const bool adopted=allowAdopted&&token&&g_free_hold19c4r2.load(std::memory_order_acquire)==token;
    if(!live()||p.binding.token!=token||
       !p.requestedAt||now<p.requestedAt||(!adopted&&now-p.requestedAt>80)||
       !p.serial||!p.generation||!p.trackingEpoch||
       p.generation!=g_use_generation55.load(std::memory_order_acquire)||
       p.serial<=g_use_retired55.load(std::memory_order_acquire)||
       !grasp13::sameHeldResource54(p.binding,p.binding)||p.binding.side<0||p.binding.side>1||
       p.binding.logic||p.binding.attached||p.binding.place||p.binding.socketOnly28||
       !p.binding.gripReady55||!grasp13::valid(p.binding.grip55)||
       !FreshAcquisitionInput55(input,now)||
       input.character!=p.binding.character||input.epoch!=p.binding.epoch||input.trackingEpoch!=p.trackingEpoch||
       !input.tracked[p.binding.side]||!std::isfinite(input.grip[p.binding.side])||
       input.grip[p.binding.side]<g_cancel_grip.load(std::memory_order_acquire)||
       !g_binding13.read(current)||!grasp13::sameHeldResource54(p.binding,current)||
       !current.gripReady55||std::memcmp(&current.grip55,&p.binding.grip55,sizeof(current.grip55))||
       (g_free_hold19c4r2.load(std::memory_order_acquire)==token&&(!current.attached||!current.place))||
       !live()||p.generation!=g_use_generation55.load(std::memory_order_acquire)||
       p.serial<=g_use_retired55.load(std::memory_order_acquire))return false;
    p.sourceTick=input.tick;out=p;return true;
}
bool ReadFinishingAttachmentPresentationR94(grasp13::FinishingPresentationR94& out) noexcept {
    return ReadFinishingVisualImplR94(out,g_finishing_visual_token_r94.load(std::memory_order_acquire),false);
}
bool ReadAdoptedAttachmentPresentationR95(grasp13::FinishingPresentationR94& out) noexcept {
    const auto token=g_free_hold19c4r2.load(std::memory_order_acquire);
    return ReadFinishingVisualImplR94(out,token,true)&&
        g_free_hold19c4r2.load(std::memory_order_acquire)==token;
}
bool AdoptedAttachmentPresentationCurrentR95(const grasp13::FinishingPresentationR94& expected) noexcept {
    grasp13::FinishingPresentationR94 current{};
    return ReadAdoptedAttachmentPresentationR95(current)&&
        grasp13::sameFinishingPresentationR94(expected,current)&&
        std::memcmp(&expected.binding.grip55,&current.binding.grip55,sizeof(current.binding.grip55))==0;
}
bool FinishingOrHeldPresentationCurrentR94(const grasp13::FinishingPresentationR94& expected) noexcept {
    grasp13::FinishingPresentationR94 current{};
    return ReadFinishingVisualImplR94(current,expected.binding.token,true)&&
        grasp13::sameFinishingPresentationR94(expected,current)&&
        std::memcmp(&expected.binding.grip55,&current.binding.grip55,sizeof(current.binding.grip55))==0;
}
void PublishGraspSocket28(const grasp13::SocketProof28& p) noexcept {
    grasp13::Binding b{};const auto now=std::uint64_t(GetTickCount64());
    if(NativeGraspEnabled()&&g_context13.load(std::memory_order_acquire)&&ReadGraspBinding(b)&&!b.gripReady55&&(b.socketOnly28||(b.attached&&b.place))&&
       grasp13::same(b,p.binding)&&p.self&&p.attachment&&p.joint>=0&&p.sample&&grasp13::fresh(now,p.tick,80))
        (void)g_socket28.publish(p);
}
void PublishGraspPlacement(const grasp13::Placement& p) noexcept {
    grasp13::Binding b{};if(ReadGraspBinding(b)&&b.place&&grasp13::same(b,p.binding)&&grasp13::valid(p.world))g_placement13.publish(p);
}
void GetStatus(Status& out) noexcept {out={};g_status13.read(out);}
#include "subsystems/throw_drop/runtime/action_owner_leaves.inl" // M2_OWNED_SOURCE
}
