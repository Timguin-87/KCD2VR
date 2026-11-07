// Generated from the actual complete grace consumer; only native timer-start leaf substituted.
#include "diagnostics/combo_snapshot.h"
#include "vrik/combat_r9.h"
namespace combo59c17 {
using PhaseObserverFn=void(__fastcall*)(void*,void*,void*,int);
using TimerStartFn=void(__fastcall*)(void*,float,float);
constexpr std::uintptr_t kManagerPhase=0x911250ull,kComboTriggerPhase=0x91115Cull,kTimerStart=0x9D6A34ull;
constexpr unsigned char kManagerPhaseEntry[]{0x40,0x53,0x48,0x83,0xEC,0x20,0x48,0x8B,0xD9,0x41,0x83,0xF9,0x04,0x74,0x29,0x41};
constexpr unsigned char kComboTriggerPhaseEntry[]{0x48,0x89,0x5C,0x24,0x08,0x57,0x48,0x83,0xEC,0x40,0x0F,0x29,0x74,0x24,0x30,0x41};
constexpr unsigned char kTimerStartEntry[]{0x40,0x53,0x48,0x83,0xEC,0x40,0x0F,0x29,0x74,0x24,0x30,0x48,0x8B,0xD9,0x48,0x8B};
// The trigger's native timer start (0x911224..0x911239): xorps xmm2,xmm2; addss xmm1,xmm6; movss [rbx+0x1C],xmm1 (slot length);
// call 9D6A34 (rcx = rbx+0x28 from 0x911209); mov byte [rbx+0x1B],1 (timed close armed).
constexpr unsigned char kTimerCallSite[]{0x0F,0x57,0xD2,0xF3,0x0F,0x58,0xCE,0xF3,0x0F,0x11,0x4B,0x1C,0xE8,0xFF,0x57,0x0C,0x00,0xC6,0x43,0x1B,0x01};
std::atomic<unsigned> g_window_ms{1500};
std::atomic<bool> g_window_extend{true}; // R25: explicit gate for the existing qualified Henry window
// A native timer exception retires this session's timing ownership. Settings
// reload/mode changes and the explicit test cannot reacquire the faulting boundary.
std::atomic<bool> g_timer_fault_h19{false};
static unsigned WindowMsH3()noexcept {
    if(!g_window_extend.load(std::memory_order_acquire)||g_timer_fault_h19.load(std::memory_order_acquire))return 0;
    if(::kcd2vr::combo_test_ds::force.load(std::memory_order_relaxed))return ::kcd2vr::combo_test_ds::kWindowMs;
    return kcd2vr::routerH3::comboWindow(g_window_ms.load(std::memory_order_relaxed));
}
std::optional<DetourModKit::hook::Hook> g_manager_hook,g_trigger_hook;
PhaseObserverFn g_manager_original=nullptr,g_trigger_original=nullptr;
std::atomic<bool> g_ready{false},g_teardown_pending19{false};
std::atomic<std::uint64_t> g_deferred{0},g_late_resets{0},g_extended{0},g_kept_open{0},g_native_resets{0};
std::atomic<std::uint64_t> g_native_open_f1{0},g_query_refused_f1{0},g_repeat_f1{0};
std::atomic<unsigned> g_note_count_f1{0},g_last_result_f1{0},g_last_query_f1{0};
namespace cg=kcd2vr::combo_lineage19;
cg::Owner g_deferred_owner19{}; // game thread only; mode changes invalidate via atomic generation
// The earned native slot is wider than ordinary attack metadata lifetime.
// 93A1BC clears the original hit property during cleanup; 90872C rechecks it
// before its finite timer expires. Keep only that UPDATE read after observing
// the exact original action's cleanup. Never write the property or create OPEN.
struct UpdateContinuationH10 {
    cg::Owner owner{};std::uintptr_t trigger=0,actionManager=0;
    std::uint64_t inputGeneration=0;unsigned window=0;bool cleaned=false;
};
UpdateContinuationH10 g_update_continuation_h10{}; // native callback thread only
static bool ArmUpdateContinuationH10() noexcept;
static bool DropUpdateContinuationH10() noexcept;
static void NoteF1(unsigned result,unsigned query,std::uint64_t release=0,float native=0,float wanted=0) noexcept {
    const auto old=g_last_result_f1.exchange(result,std::memory_order_relaxed);
    const auto oldQuery=g_last_query_f1.exchange(query,std::memory_order_relaxed);
    // Only native callback transitions/first4, at most16 session records. No
    // frame polling, copied actor pointers, or recorder requirement.
    if(g_note_count_f1.load(std::memory_order_relaxed)>=16)return;
    if(old==result&&oldQuery==query&&g_note_count_f1.load(std::memory_order_relaxed)>=4)return;
    if(g_note_count_f1.fetch_add(1,std::memory_order_relaxed)>=16)return;
    const char* label=result==1?"NATIVE_NOT_OPENED":result==2?"QUERY_REFUSED":result==3?"OWNER_CHANGED":
        result==4?"RELEASE_ALREADY_SPENT":result==5?"REARMED_ONCE":result==6?"NATIVE_ALREADY_LONG":result==7?"EXPIRED":"SNAPSHOT_UNAVAILABLE";
    char line[260]{};std::snprintf(line,sizeof(line),"F1 COMBO_NATIVE_SLOT result=%s queryReason=%u release=%llu nativeSeconds=%.3f wantedSeconds=%.3f NPC=UNCHANGED",
        label,query,(unsigned long long)release,double(native),double(wanted));diagnostic_trace::Write(line);
}
// Menu/control changes also call this. Atomic lineage retirement invalidates the
// native-thread receipt; never write that thread's plain receipt from here.
// CB2 (Claude, 25 Oct 2026): the window above is anchored ONCE to the release whose phase16 opened the native
// slot. Miki's 23/24 Oct logs: every "one hit left" combo was still reset before the finisher swing -
//  * a continuation attack whose VR hit lands after its native phase16 never opens its own slot (909A90 sees no
//    hit flag yet) -> the old lease expires (F1 EXPIRED, ownerAge 2031 ms) -> idle phase2 resets (C5C9C7/C5E49B);
//  * after the attack cleanup clears the hit flag (93A1BC), the slot update (90872C -> 909A90) closes the slot at
//    idle long before its 1.5 s timer -> C5B418 reset (C5B45D) when combo-active (+0x360) is not set.
// CB2 keeps the SAME window per REAL native combo step of Henry (C5C594 returned with live states): the slot of that
// hit is opened by the native phase16 handler once its hit flag is real (909A90 still decides), its timer gets the
// VR window, the idle reset and the cleanup close are held while that window, the same opponent and live native combo
// states exist. nothing else: recipes, zone match, finisher (9397CC), costs and NPC reactions stay native.
struct GraceCB2 {std::uintptr_t actor=0,manager=0,trigger=0,opponent=0;std::uint64_t until=0;bool deferred=false;
    std::uintptr_t openedActionCB3=0,openedManagerCB3=0;std::uint64_t openedInputCB3=0;bool cleanedCB3=false;};
GraceCB2 g_cb2{}; // native combat callback thread only
static bool ActiveCB2(const void* actor,std::uint64_t now) noexcept;
static void ObserveOpenedCleanupCB3(const void* state,std::uint64_t now) noexcept;
static std::uintptr_t CurrentMainCB3(const void* actor) noexcept {
    const void* manager=Rd<const void*>(actor,0x200,nullptr);
    const void* slots=manager?Rd<const void*>(manager,0x38,nullptr):nullptr;
    return slots?Rd<std::uintptr_t>(slots,0,0):0; // membership only; never dereference a retired action
}
static void RetireOpenedActionCB3() noexcept {g_cb2.openedActionCB3=0;g_cb2.openedManagerCB3=0;g_cb2.openedInputCB3=0;g_cb2.cleanedCB3=false;}
std::uintptr_t g_cb2_trigger_seen=0; // same native callback thread; never a control-thread pointer
cg::Owner g_cb2_owner{}; // original current VR owner, native callback thread only
std::atomic<std::uint64_t> g_cb2_generation{1};
std::uint64_t g_cb2_seen_generation=0; // native callback thread only
static std::uint64_t SyncCB2() noexcept {
    const auto generation=g_cb2_generation.load(std::memory_order_acquire);
    if(generation!=g_cb2_seen_generation){g_cb2={};g_cb2_owner={};g_cb2_trigger_seen=0;g_cb2_seen_generation=generation;}
    return generation;
}
static bool CurrentCB2(std::uint64_t generation) noexcept {
    if(generation&&generation==g_cb2_generation.load(std::memory_order_acquire)&&generation==g_cb2_seen_generation)return true;
    (void)SyncCB2();return false;
}
std::atomic<std::uint64_t> g_cb2_armed{0},g_cb2_held{0},g_cb2_reopened{0},g_cb2_rearmed{0},g_cb2_read_kept{0},g_cb2_late_reset{0};
void ClearLineage19() noexcept {
    // Menu/control threads publish invalidation only. The native callback
    // thread retires its own plain receipt AND borrowed trigger at entry.
    g_cb2_generation.fetch_add(1,std::memory_order_acq_rel);
    cg::grace.clear();cg::nativeSlot.retire();
}
// Timing continuation is input ownership, not permission for a new collision.
// H30 validates the current tracked intent; strict contact ownership stays in
// hit delivery. Recovery/item/body/request/mode/opponent retirement stays here.
static bool CurrentOwner19(const cg::Owner& saved,const void* actor,cg::Owner& current,bool* publishing=nullptr) noexcept {
    if(publishing)*publishing=false;
    current=saved;
    if(!cg::valid(saved)||!actor||actor!=geom27::vrstate596::g_henry_actor.load(std::memory_order_relaxed)||
       !geom27::g_enabled.load()||!geom27::g_sync59c10r9.load()||saved.mode!=geom27::ComboMode19()||
       saved.modeGeneration!=cg::grace.generation()||saved.key.recovery!=kcd2vr::contact176::recoveryEpoch55c1.load()||
       reinterpret_cast<std::uintptr_t>(actor)!=saved.actor)return false;
    const void* state=Rd<const void*>(actor,0x210,nullptr),*player=Rd<const void*>(actor,0x1F8,nullptr),*manager=Rd<const void*>(actor,0x3B0,nullptr);
    const void* target=state?Rd<const void*>(state,0xCF8,nullptr):nullptr;
    const void* entity=nullptr,*targetState=nullptr,*henryEntity=nullptr,*henryState=nullptr;
    if(!CombatActor(target,entity,targetState,false)||!CombatActor(actor,henryEntity,henryState,false)||henryState!=state||!IsPlayer(henryEntity)||
       !state||!player||!manager||!entity||Rd<const void*>(state,0xCE0,nullptr)!=actor||
       Rd<const void*>(manager,8,nullptr)!=actor)return false;
    current.actor=reinterpret_cast<std::uintptr_t>(actor);current.state=reinterpret_cast<std::uintptr_t>(state);
    current.key.player=reinterpret_cast<std::uintptr_t>(player);current.manager=reinterpret_cast<std::uintptr_t>(manager);
    current.target=reinterpret_cast<std::uintptr_t>(target);current.targetEntity=reinterpret_cast<std::uintptr_t>(entity);current.targetId=EntityId(entity);
    current.mode=geom27::ComboMode19();current.modeGeneration=cg::grace.generation();
    if(!cg::sameLineage(saved,current))return false;
    const auto intent=frik_pose_probe::GeometryIntentOwnerStateH30(saved.key.weapon,saved.key.generation,saved.key.body,saved.key.request);
    if(publishing)*publishing=intent==2;
    return intent==1;
}
static bool Active(const void* actor,std::uint64_t now,cg::Owner* ownerOut=nullptr,bool* publishing=nullptr) noexcept {
    if(publishing)*publishing=false;
    const unsigned window=WindowMsH3();
    if(!window||!actor||actor!=geom27::vrstate596::g_henry_actor.load(std::memory_order_relaxed))return false;
    cg::Owner saved{};const auto* offer=cg::offer;
    const auto* stack=kcd2vr::geometry40c1::Scope::current;const auto& result=geom27::g_inject_result178r35;
    const bool qualifiedOffer=offer&&cg::valid(*offer)&&stack&&
        reinterpret_cast<std::uintptr_t>(stack->actor)==offer->actor&&reinterpret_cast<std::uintptr_t>(stack->target)==offer->target&&
        result.seen&&result.ok&&result.fields&&result.process==1&&result.target==offer->targetId;
    if(qualifiedOffer)saved=*offer;else (void)cg::grace.read(saved);
    cg::SlotLeaseF1 slot{};std::uint64_t consumed=0;
    const bool nativeSlot=cg::nativeSlot.read(slot,consumed);
    unsigned ageLimit=window;
    if(!cg::valid(saved)||saved.release<=consumed){
        if(!nativeSlot)return false;
        saved=slot.owner;ageLimit=std::min(window,slot.window);
    }
    cg::Owner current{};bool inProgress=false;
    if(!CurrentOwner19(saved,actor,current,&inProgress)){
        // A new pose is visible before PhysPublish6698 completes its command
        // queue. Keep only a previously earned native slot, without renewing
        // its original clock or treating this frame as input/contact authority.
        // The current native manager must still own the ORIGINAL main action;
        // membership is checked before any borrowed action could be read.
        if(inProgress&&nativeSlot&&cg::sameLineage(saved,slot.owner)&&
           kcd2vr::motion59c4::fresh(slot.owner.tick,now,std::min(window,slot.window))){
            const void* actions=Rd<const void*>(actor,0x200,nullptr);
            const void* slots=actions?Rd<const void*>(actions,0x38,nullptr):nullptr;
            if(slots&&Rd<const void*>(slots,0,nullptr)==reinterpret_cast<const void*>(slot.owner.action)){
                if(ownerOut)*ownerOut=slot.owner;
                if(publishing)*publishing=true;
                return false;
            }
        }
        ClearLineage19();return false;
    }
    if(!kcd2vr::motion59c4::fresh(saved.tick,now,ageLimit)){if(nativeSlot)NoteF1(7,0,saved.release);return false;}
    if(ownerOut)*ownerOut=saved;return true;
}
static cg::Owner NativeOpenedOwnerF1(const void* actor,std::uint64_t now,unsigned* reason=nullptr,
    kcd2vr::subsystems::masterstrike_combos::NativeSlotF1* observation=nullptr) noexcept {
    cg::Owner owner{};kcd2vr::subsystems::masterstrike_combos::NativeSlotF1 native{};
    if(reason)*reason=10; // disabled/foreign actor; no native query made
    if(!WindowMsH3()||!geom27::g_enabled.load()||!geom27::g_sync59c10r9.load()||
       !actor||actor!=geom27::vrstate596::g_henry_actor.load(std::memory_order_relaxed))return owner;
    const bool read=kcd2vr::frik_pose_probe::ReadNativeComboSlotF1(actor,native);
    if(observation)*observation=native;
    if(reason)*reason=unsigned(native.reason);
    if(!read||native.actor!=reinterpret_cast<std::uintptr_t>(actor))return owner;
    const auto* state=reinterpret_cast<const void*>(native.state);
    const void* manager=Rd<const void*>(actor,0x3B0,nullptr),*target=Rd<const void*>(state,0xCF8,nullptr),*entity=nullptr,*targetState=nullptr;
    if(!CombatActor(target,entity,targetState,false)||!entity)return owner;
    const auto& t=native.release;
    owner.key={t.gesture59c3,t.key,t.generation,t.epoch,t.request,t.recovery59c3,t.player};
    owner.actor=native.actor;owner.state=native.state;owner.manager=reinterpret_cast<std::uintptr_t>(manager);
    owner.target=reinterpret_cast<std::uintptr_t>(target);owner.targetEntity=reinterpret_cast<std::uintptr_t>(entity);owner.targetId=EntityId(entity);
    owner.action=native.action;owner.release=t.serial;owner.modeGeneration=cg::grace.generation();owner.mode=geom27::ComboMode19();owner.tick=now;
    cg::Owner current{};return CurrentOwner19(owner,actor,current)?owner:cg::Owner{};
}
struct TriggerSnapshotF1 {std::uintptr_t actor=0,target=0;std::uint64_t deadline=0;float duration=0;bool requested=false,open=false,timed=false;};
static bool ReadTriggerF1(const void* trigger,TriggerSnapshotF1& out) noexcept {
    out={};unsigned char bytes[0x40]{};
    if(!SehCopy146(trigger,bytes,sizeof(bytes))||bytes[0x18]>1||bytes[0x19]>1||bytes[0x1B]>1)return false;
    std::memcpy(&out.actor,bytes+8,8);std::memcpy(&out.target,bytes+0x38,8);
    std::memcpy(&out.deadline,bytes+0x28,8);std::memcpy(&out.duration,bytes+0x1C,4);
    out.requested=bytes[0x18]!=0;out.open=bytes[0x19]!=0;out.timed=bytes[0x1B]!=0;return true;
}
static bool SameReleaseF1(const cg::Owner& a,const cg::Owner& b) noexcept {
    return cg::valid(a)&&cg::valid(b)&&cg::sameLineage(a,b)&&a.action==b.action&&a.release==b.release&&a.key.gesture==b.key.gesture;
}
static bool SameNativeSlotF1(const kcd2vr::subsystems::masterstrike_combos::NativeSlotF1& a,
    const kcd2vr::subsystems::masterstrike_combos::NativeSlotF1& b) noexcept {
    return a.action&&b.action&&a.action==b.action&&a.helper==b.helper&&a.clock==b.clock&&a.manager==b.manager&&
        a.row==b.row&&a.entity==b.entity&&a.admission==b.admission&&a.policy==b.policy;
}
static bool UpdateLeaseCurrentH10(std::uint64_t now) noexcept {
    const auto& q=g_update_continuation_h10;cg::SlotLeaseF1 lease{};std::uint64_t consumed=0;
    if(!g_ready.load(std::memory_order_acquire)||g_teardown_pending19.load(std::memory_order_acquire)||
       !q.trigger||!q.actionManager||!q.inputGeneration||!cg::nativeSlot.read(lease,consumed)||
       consumed!=q.owner.release||lease.window!=q.window||lease.owner.tick!=q.owner.tick||
       !SameReleaseF1(lease.owner,q.owner)||!kcd2vr::motion59c4::fresh(q.owner.tick,now,q.window)||
       kcd2vr::frik_pose_probe::NativeComboInputGenerationH10()!=q.inputGeneration)return false;
    // Native908CF4 selects explicit trigger+38 first, otherwise current CF8.
    // Active validates the fallback; an explicit replacement cannot borrow A's hit.
    const auto selected=Rd<const void*>(reinterpret_cast<const void*>(q.trigger),0x38,nullptr);
    if(selected&&reinterpret_cast<std::uintptr_t>(selected)!=q.owner.target)return false;
    cg::Owner current{};bool publishing=false;
    const bool active=Active(reinterpret_cast<const void*>(q.owner.actor),now,&current,&publishing);
    return (active||publishing)&&SameReleaseF1(current,q.owner)&&
        Rd<const void*>(reinterpret_cast<const void*>(q.owner.actor),0x200,nullptr)==reinterpret_cast<const void*>(q.actionManager);
}
static void CaptureUpdateContinuationH10(void* trigger,const cg::Owner& owner,unsigned window,std::uint64_t inputGeneration) noexcept {
    g_update_continuation_h10={};
    if(!inputGeneration||inputGeneration!=kcd2vr::frik_pose_probe::NativeComboInputGenerationH10())return;
    const auto manager=Rd<const void*>(reinterpret_cast<const void*>(owner.actor),0x200,nullptr);
    const auto slots=manager?Rd<const void*>(manager,0x38,nullptr):nullptr;
    if(!slots||Rd<const void*>(slots,0,nullptr)!=reinterpret_cast<const void*>(owner.action))return;
    g_update_continuation_h10={owner,reinterpret_cast<std::uintptr_t>(trigger),reinterpret_cast<std::uintptr_t>(manager),inputGeneration,window,false};
}
static void ObserveOrdinaryCleanupH10(const void* state,std::uint64_t now) noexcept {
    ObserveOpenedCleanupCB3(state,now);
    if(reinterpret_cast<std::uintptr_t>(state)!=g_update_continuation_h10.owner.state||!state)return;
    if(!UpdateLeaseCurrentH10(now)){g_update_continuation_h10={};return;}
    auto& q=g_update_continuation_h10;
    if(reinterpret_cast<std::uintptr_t>(state)!=q.owner.state||Rd<std::uint8_t>(state,0x488,0)!=1)return;
    const int phase=Rd<int>(state,0x38,-1);
    if(phase==4||phase==64)return; // actual native terminal; ordinary Exit's phase is not fixed
    const auto slots=Rd<const void*>(reinterpret_cast<const void*>(q.actionManager),0x38,nullptr);
    if(slots&&Rd<const void*>(slots,0,nullptr)==reinterpret_cast<const void*>(q.owner.action))q.cleaned=true;
    else g_update_continuation_h10={};
}
static bool KeepUpdateHitReadH10(const void* trigger,const void* actor,std::uintptr_t nativeReturn,std::uint64_t now) noexcept {
    // 909A90 is also the initial phase16 eligibility predicate. Its UPDATE caller
    // alone returns to90874E; the phase16 caller and every other caller stay native.
    if(nativeReturn!=g_base+0x90874E||!g_update_continuation_h10.cleaned||
       reinterpret_cast<std::uintptr_t>(trigger)!=g_update_continuation_h10.trigger||
       reinterpret_cast<std::uintptr_t>(actor)!=g_update_continuation_h10.owner.actor)return false;
    if(!UpdateLeaseCurrentH10(now)){g_update_continuation_h10={};return false;}
    const auto& q=g_update_continuation_h10;
    if(Rd<const void*>(trigger,8,nullptr)!=actor||Rd<std::uint8_t>(trigger,0x19,0)!=1||
       Rd<std::uint8_t>(trigger,0x1B,0)!=1||Rd<std::uint8_t>(reinterpret_cast<const void*>(q.owner.state),0x868,0)!=1)return false;
    const int phase=Rd<int>(reinterpret_cast<const void*>(q.owner.state),0x38,-1);
    if(phase==4||phase==64){g_update_continuation_h10={};return false;}
    const auto slots=Rd<const void*>(reinterpret_cast<const void*>(q.actionManager),0x38,nullptr);
    if(!slots){g_update_continuation_h10={};return false;}
    const auto current=Rd<const void*>(slots,0,nullptr);
    // The old action is never dereferenced after native manager removal. A new
    // action cannot borrow this grant; a completed ordinary cleanup may leave MAIN empty.
    if(current&&current!=reinterpret_cast<const void*>(q.owner.action)){g_update_continuation_h10={};return false;}
    return true;
}
// Existing native callback decisions, copied only under F3. No extra native
// reads, timer work or gameplay state; repeated identical phase observations
// collapse until their actual owner/slot/decision changes.
struct PhaseCaptureH15 {std::uint64_t session=0,epoch=0;};
static PhaseCaptureH15 BeginPhaseCaptureH15(std::uint64_t now) noexcept {
    namespace capture=kcd2vr::diagnostics::capture168;
    if(!capture::active(capture::Domain::Combat,now))return {};
    return {capture::gate.session[unsigned(capture::Domain::Combat)].load(std::memory_order_acquire),
        kcd2vr::diagnostics::lab::Snapshot()>>4};
}
static void TracePhaseH15(unsigned lane,const PhaseCaptureH15& record,const void* object,const void* actor,int phase,const char* decision,
    const cg::Owner& owner,std::uint64_t now,bool publishing=false,const TriggerSnapshotF1* before=nullptr,
    const TriggerSnapshotF1* after=nullptr,unsigned query=0) noexcept {
    namespace capture=kcd2vr::diagnostics::capture168;
    if(!record.session||!capture::active(capture::Domain::Combat,std::uint64_t(GetTickCount64()))||record.epoch!=(kcd2vr::diagnostics::lab::Snapshot()>>4))return;
    const auto session=capture::gate.session[unsigned(capture::Domain::Combat)].load(std::memory_order_acquire);
    if(session!=record.session)return;
    const TriggerSnapshotF1 empty{};const auto& a=before?*before:empty;const auto& b=after?*after:empty;
    using U=unsigned long long;
    std::uint32_t durationA=0,durationB=0;std::memcpy(&durationA,&a.duration,4);std::memcpy(&durationB,&b.duration,4);
    const std::array<std::uint64_t,35> stamp{session,record.epoch,reinterpret_cast<std::uintptr_t>(object),reinterpret_cast<std::uintptr_t>(actor),
        std::uint64_t(phase),reinterpret_cast<std::uintptr_t>(decision),owner.release,owner.action,owner.key.gesture,owner.key.weapon,
        owner.key.generation,owner.key.body,owner.key.request,owner.key.recovery,owner.target,owner.targetId,owner.modeGeneration,owner.mode,
        a.deadline,b.deadline,std::uint64_t(a.requested)|(std::uint64_t(a.timed)<<1),std::uint64_t(b.requested)|(std::uint64_t(b.timed)<<1),
        a.target,b.target,std::uint64_t(publishing),query,owner.key.player,owner.manager,owner.targetEntity,owner.tick,
        std::uint64_t(before!=nullptr),std::uint64_t(after!=nullptr),durationA,durationB,owner.state};
    static thread_local std::array<std::array<std::uint64_t,35>,2> last{};
    if(lane>1||last[lane]==stamp)return;last[lane]=stamp;
    char line[2048]{};std::snprintf(line,sizeof(line),
        "H15 F3_COMBO_PHASE tick=%llu captureSession=%llu captureEpoch=%llu source=%s object=%llX actor=%llX phase=%d decision=%s gesture=%llu key=%016llX generation=%llu body=%llu request=%llu recovery=%llu player=%llX release=%llu action=%llX manager=%llX state=%llX target=%u targetActor=%llX targetEntity=%llX ownerTick=%llu modeGeneration=%llu mode=%u publishing=%d slotBefore{known=%d requested=%d timed=%d deadlineRaw=%llu duration=%.6g target=%llX} slotAfter{known=%d requested=%d timed=%d deadlineRaw=%llu duration=%.6g target=%llX} queryReason=%u slotSnapshots=BEFORE_OPTIONAL_EXTENSION nativeAcceptance=UNKNOWN nativeResetCause=UNKNOWN observerWrites=NONE",
        U(now),U(session),U(record.epoch),lane?"TRIGGER":"MANAGER",U(reinterpret_cast<std::uintptr_t>(object)),U(reinterpret_cast<std::uintptr_t>(actor)),phase,decision,
        U(owner.key.gesture),U(owner.key.weapon),U(owner.key.generation),U(owner.key.body),U(owner.key.request),U(owner.key.recovery),U(owner.key.player),
        U(owner.release),U(owner.action),U(owner.manager),U(owner.state),owner.targetId,U(owner.target),U(owner.targetEntity),U(owner.tick),U(owner.modeGeneration),owner.mode,int(publishing),
        int(before!=nullptr),int(a.requested),int(a.timed),U(a.deadline),double(a.duration),U(a.target),
        int(after!=nullptr),int(b.requested),int(b.timed),U(b.deadline),double(b.duration),U(b.target),query);
    diagnostic_trace::Write(line);
}
// CB3: strike phases whose hit flag edge may open the slot (attack running 8, phase16, phase32).
constexpr bool ReopenPhaseCB3(int phase) noexcept {return phase==8||phase==16||phase==32;}
static bool ComboStatesCB2(const void* manager) noexcept {
    std::uintptr_t v[2]{};return manager&&SehCopy146(static_cast<const unsigned char*>(manager)+0xC0,v,sizeof(v))&&v[0]&&v[1]>v[0];
}
static const void* OpponentCB2(const void* actor) noexcept {const void* st=Rd<const void*>(actor,0x210,nullptr);return st?Rd<const void*>(st,0xCF8,nullptr):nullptr;}
static bool ActiveCB2(const void* actor,std::uint64_t now) noexcept {
    const auto generation=SyncCB2();const auto& g=g_cb2;
    if(!g.until||now>=g.until||!actor||reinterpret_cast<std::uintptr_t>(actor)!=g.actor||!WindowMsH3()||
       actor!=geom27::vrstate596::g_henry_actor.load(std::memory_order_relaxed))return false;
    const void* manager=Rd<const void*>(actor,0x3B0,nullptr);
    if(!manager||reinterpret_cast<std::uintptr_t>(manager)!=g.manager||Rd<const void*>(manager,8,nullptr)!=actor)return false;
    const void* opponent=OpponentCB2(actor);cg::Owner current{};
    if(!opponent||reinterpret_cast<std::uintptr_t>(opponent)!=g.opponent||!CurrentOwner19(g_cb2_owner,actor,current))return false;
    return ComboStatesCB2(manager)&&CurrentCB2(generation);
}
// A CB3 native OPEN can precede ordinary phase16. Keep its exact current
// MAIN membership separately from the older accepted-hit/grace release.
static bool OpenedActionCurrentCB3(const void* actor,std::uint64_t now,bool allowRemoved=false) noexcept {
    if(!g_ready.load(std::memory_order_acquire)||g_teardown_pending19.load(std::memory_order_acquire)||
       !g_cb2.openedActionCB3||!g_cb2.openedInputCB3||
       kcd2vr::frik_pose_probe::NativeComboInputGenerationH10()!=g_cb2.openedInputCB3||!ActiveCB2(actor,now))return false;
    const auto* manager=Rd<const void*>(actor,0x200,nullptr);
    const auto* slots=manager?Rd<const void*>(manager,0x38,nullptr):nullptr;
    if(!slots||reinterpret_cast<std::uintptr_t>(manager)!=g_cb2.openedManagerCB3)return false;
    const auto current=Rd<std::uintptr_t>(slots,0,0);
    return current==g_cb2.openedActionCB3||(allowRemoved&&g_cb2.cleanedCB3&&!current);
}
static void ObserveOpenedCleanupCB3(const void* state,std::uint64_t now) noexcept {
    const auto* actor=reinterpret_cast<const void*>(g_cb2.actor);
    if(!actor||!OpenedActionCurrentCB3(actor,now)||state!=Rd<const void*>(actor,0x210,nullptr)||
       Rd<std::uint8_t>(state,0x488,0)!=1)return;
    const int phase=Rd<int>(state,0x38,-1);
    if(phase!=4&&phase!=64)g_cb2.cleanedCB3=true; // existing native93A236 is about to clear this action's hit property
}
static bool ConsumeDuplicateOpenCB3(void* trigger,const void* actor,const TriggerSnapshotF1& before,
    const TriggerSnapshotF1& after,std::uintptr_t actionBefore,std::uint64_t generation,std::uint64_t now) noexcept {
    if(!CurrentCB2(generation)||!actionBefore||actionBefore!=g_cb2.openedActionCB3||
       !OpenedActionCurrentCB3(actor,now)||CurrentMainCB3(actor)!=actionBefore||
       reinterpret_cast<std::uintptr_t>(trigger)!=g_cb2.trigger||
       before.actor!=after.actor||after.actor!=g_cb2.actor||!before.open||before.requested||
       !after.open||!after.requested||!after.timed||
       (before.target&&before.target!=g_cb2.opponent)||(after.target&&after.target!=g_cb2.opponent))return false;
    // Native909E68 closes an already-open slot before opening it again.
    // The original phase16 eligibility/timer already ran. Consume only its
    // redundant deferred OPEN (+18); keep actual OPEN (+19), timer and RPG state.
    __try{*reinterpret_cast<unsigned char*>(static_cast<unsigned char*>(trigger)+0x18)=0;return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){RetireOpenedActionCB3();return false;}
}
static bool RearmTimerCB2(void* trigger,float wanted) noexcept {
    __try{HostTimerStart(static_cast<unsigned char*>(trigger)+0x28,wanted,0.f);
        *reinterpret_cast<float*>(static_cast<unsigned char*>(trigger)+0x1C)=wanted;return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){g_timer_fault_h19.store(true,std::memory_order_release);g_window_ms.store(0,std::memory_order_relaxed);g_cb2={};
        diagnostic_trace::Write("CB2 COMBO_STEP_WINDOW_FAULT timer re-arm raised an exception; VR combo window OFF for this session");return false;}
}
// Capture a current VR lineage before the native step. This authorizes no
// attack or contact; it only scopes a possible native-earned timing extension.
std::uint64_t BeginStepCB2(void* manager,cg::Owner& saved) noexcept {
    saved={};const auto generation=SyncCB2();
    if(!g_ready.load(std::memory_order_acquire)||g_teardown_pending19.load(std::memory_order_acquire)||!g_trigger_original||
       !geom27::g_enabled.load()||!geom27::g_sync59c10r9.load()||!WindowMsH3())return 0;
    const void* actor=Rd<const void*>(manager,8,nullptr);
    if(!actor||actor!=geom27::vrstate596::g_henry_actor.load(std::memory_order_relaxed)||Rd<const void*>(actor,0x3B0,nullptr)!=manager)return 0;
    // The same-stack offer is still provisional. Only actual native cursor
    // progression below can earn the extension; this does not invent a hit.
    if(cg::offer&&cg::valid(*cg::offer))saved=*cg::offer;
    else if(!cg::grace.read(saved)){cg::SlotLeaseF1 slot{};std::uint64_t consumed=0;if(cg::nativeSlot.read(slot,consumed))saved=slot.owner;}
    cg::Owner current{};
    return saved.manager==reinterpret_cast<std::uintptr_t>(manager)&&CurrentOwner19(saved,actor,current)&&CurrentCB2(generation)?generation:0;
}
static bool AdvancedStepCB2(const kcd2vr::diagnostics::combo::Snapshot& before,
    const kcd2vr::diagnostics::combo::Snapshot& after) noexcept {
    namespace native=kcd2vr::diagnostics::combo;
    if(!native::Comparable(before,after)||!after.stateCount)return false;
    if(after.stateCount>before.stateCount)return true;
    // Native compacts states while pruning. Compare maximum cursor for the
    // same exact candidate recipe, never state indexes after compaction.
    for(unsigned a=0;a<after.candidatesCopied;++a){const auto& candidate=after.candidates[a];
        if(candidate.state>=after.statesCopied||!candidate.recipe)continue;
        unsigned priorMax=0;bool found=false;
        for(unsigned b=0;b<before.candidatesCopied;++b){const auto& old=before.candidates[b];
            if(old.recipe==candidate.recipe&&old.state<before.statesCopied){found=true;priorMax=std::max(priorMax,before.states[old.state].cursor);}}
        if(found&&after.states[candidate.state].cursor>priorMax)return true;
    }
    return false;
}
// C3A (Claude, 25 Oct): F3 combo lineage per REAL native step, only while the F3 combat capture is on. one line:
// every live candidate (recipe name, cursor/required = "one hit left" when cursor+1==required), Henry's and the
// opponent's native combat phase (state+0x38), the slot trigger after our handling (requested/timed/duration) and
// whether CB3 re-opened it. no extra native writes; reads are the snapshot already taken by the step observer.
static void TraceStepC3A(const kcd2vr::diagnostics::combo::Snapshot& after,const void* actor,const void* opponent,
    const void* trigger,const TriggerSnapshotF1& t,int phase,bool reopened,unsigned window,std::uint64_t now) noexcept {
    const auto record=BeginPhaseCaptureH15(now);if(!record.session)return;
    char list[640]{};int n=0;
    for(unsigned i=0;i<after.candidatesCopied&&n>=0&&n<int(sizeof(list))-80;++i){const auto& c=after.candidates[i];
        const unsigned cursor=c.state<after.statesCopied?after.states[c.state].cursor:0u;
        n+=std::snprintf(list+n,sizeof(list)-n,"%s%s:%u/%u",i?",":"",c.nameExact?c.name.data():"?",cursor,c.requiredSteps);}
    const void* oppState=opponent?Rd<const void*>(opponent,0x210,nullptr):nullptr;
    const int oppPhase=oppState?Rd<int>(oppState,0x38,-1):-1;
    char line[960]{};std::snprintf(line,sizeof(line),
        "C3A F3_COMBO_STEP session=%llu tick=%llu actor=%llX opponent=%llX henryPhase=%d opponentPhase=%d candidates=[%s] trigger=%llX slotRequested=%d timed=%d durationS=%.3f reopenedCB3=%d windowMs=%u armed=%llu held=%llu readKept=%llu lateResets=%llu native=DECIDES_ELIGIBILITY",
        (unsigned long long)record.session,(unsigned long long)now,(unsigned long long)reinterpret_cast<std::uintptr_t>(actor),
        (unsigned long long)reinterpret_cast<std::uintptr_t>(opponent),phase,oppPhase,list,(unsigned long long)reinterpret_cast<std::uintptr_t>(trigger),
        t.requested?1:0,t.timed?1:0,std::isfinite(t.duration)?double(t.duration):0.,reopened?1:0,window,
        (unsigned long long)g_cb2_armed.load(),(unsigned long long)g_cb2_held.load(),(unsigned long long)g_cb2_read_kept.load(),(unsigned long long)g_cb2_late_reset.load());
    diagnostic_trace::Write(line);
}
// Exactly one native C5C594 returned. Unchanged/pruned/reset state is not a step.
void RealStepCB2(void* manager,const cg::Owner& saved,std::uint64_t generation,
    const kcd2vr::diagnostics::combo::Snapshot& before,const kcd2vr::diagnostics::combo::Snapshot& after) noexcept {
    (void)SyncCB2();if(!CurrentCB2(generation)||!AdvancedStepCB2(before,after))return;
    const void* actor=Rd<const void*>(manager,8,nullptr);cg::Owner current{};
    if(!g_ready.load(std::memory_order_acquire)||g_teardown_pending19.load(std::memory_order_acquire)||!g_trigger_original||
       !CurrentOwner19(saved,actor,current)||before.identity.manager!=saved.manager||before.identity.actor!=saved.actor||
       before.identity.player!=saved.key.player||before.identity.entity!=reinterpret_cast<std::uintptr_t>(Rd<const void*>(reinterpret_cast<const void*>(saved.key.player),0x38,nullptr)))return;
    const unsigned window=WindowMsH3();const void* opponent=OpponentCB2(actor);
    if(!window||reinterpret_cast<std::uintptr_t>(manager)!=saved.manager||reinterpret_cast<std::uintptr_t>(opponent)!=saved.target||!ComboStatesCB2(manager))return;
    const auto now=std::uint64_t(GetTickCount64());
    void* trigger=reinterpret_cast<void*>(g_cb2_trigger_seen);
    if(trigger&&Rd<const void*>(trigger,8,nullptr)!=actor)trigger=nullptr;
    if(!CurrentCB2(generation))return;
    const auto previousOpenedCB3=g_cb2;const auto mainCB3=CurrentMainCB3(actor);
    const auto inputCB3=kcd2vr::frik_pose_probe::NativeComboInputGenerationH10();
    g_cb2={reinterpret_cast<std::uintptr_t>(actor),reinterpret_cast<std::uintptr_t>(manager),reinterpret_cast<std::uintptr_t>(trigger),
        reinterpret_cast<std::uintptr_t>(opponent),now+window,false};
    if(mainCB3&&mainCB3==previousOpenedCB3.openedActionCB3&&inputCB3==previousOpenedCB3.openedInputCB3&&
       g_cb2.trigger==previousOpenedCB3.trigger&&g_cb2.actor==previousOpenedCB3.actor&&g_cb2.opponent==previousOpenedCB3.opponent){
        g_cb2.openedActionCB3=mainCB3;g_cb2.openedManagerCB3=reinterpret_cast<std::uintptr_t>(Rd<const void*>(actor,0x200,nullptr));g_cb2.openedInputCB3=inputCB3;g_cb2.cleanedCB3=previousOpenedCB3.cleanedCB3;}
    g_cb2_owner=saved;
    g_cb2_armed.fetch_add(1,std::memory_order_relaxed);
    if(!trigger)return;
    TriggerSnapshotF1 t{};if(!ReadTriggerF1(trigger,t)||t.actor!=reinterpret_cast<std::uintptr_t>(actor))return;
    const int phase=Rd<int>(Rd<const void*>(actor,0x210,nullptr),0x38,-1);
    bool reopened=false;
    // CB3 (Claude, 25 Oct H50 log): C5C52C is the native delegate of the hit flag (state+0x480 -> true, bound at
    // 10F0BC8), so at THIS step the flag 909A90 needs is real. 17 of 18 steps came in attack phase 8 and the old
    // phase16/32-only rule never re-opened (reopenedByNativePhase16=0); the native phase16 of a VR strike came before
    // the hit (909A90 false -> 911195 returns without opening, it never closes) or not at all (NO_PHASE16 42/100),
    // so 85/100 presses found NO_SLOT. phase 8 (strike running) is the same strike: open it now. 4 = a new attack
    // and 64 = Henry hit keep their native handling.
    if(!t.requested&&!t.open&&ReopenPhaseCB3(phase)){ // our VR hit came after (or without) this attack's phase16: the native open runs now
        if(!CurrentCB2(generation))return;
        g_trigger_original(trigger,nullptr,nullptr,16);if(!CurrentCB2(generation))return;reopened=ReadTriggerF1(trigger,t)&&t.requested&&t.timed;
        if(reopened){g_cb2_reopened.fetch_add(1,std::memory_order_relaxed);
            if(mainCB3&&CurrentMainCB3(actor)==mainCB3&&inputCB3&&inputCB3==kcd2vr::frik_pose_probe::NativeComboInputGenerationH10()){
                g_cb2.openedActionCB3=mainCB3;g_cb2.openedManagerCB3=reinterpret_cast<std::uintptr_t>(Rd<const void*>(actor,0x200,nullptr));g_cb2.openedInputCB3=inputCB3;g_cb2.cleanedCB3=false;}}}
    const float wanted=float(window)*.001f;
    if(CurrentCB2(generation)&&(t.requested||t.open)&&t.timed&&std::isfinite(t.duration)&&RearmTimerCB2(trigger,std::max(wanted,t.duration)))g_cb2_rearmed.fetch_add(1,std::memory_order_relaxed);
    TraceStepC3A(after,actor,opponent,trigger,t,phase,reopened,window,now);
    static std::atomic<unsigned> notes{0};
    if(notes.fetch_add(1,std::memory_order_relaxed)<24){char line[240]{};std::snprintf(line,sizeof(line),
        "CB2 COMBO_STEP_WINDOW armed=%llu phase=%d slotRequested=%d reopenedByNativePhase16=%d windowMs=%u held=%llu readKept=%llu lateResets=%llu",
        (unsigned long long)g_cb2_armed.load(),phase,t.requested?1:0,reopened?1:0,window,(unsigned long long)g_cb2_held.load(),
        (unsigned long long)g_cb2_read_kept.load(),(unsigned long long)g_cb2_late_reset.load());diagnostic_trace::Write(line);}
}
static bool KeepReadCB2(const void* trigger,const void* actor,std::uintptr_t nativeReturn,std::uint64_t now) noexcept {
    (void)SyncCB2();
    if(nativeReturn!=g_base+0x90874E||!trigger||reinterpret_cast<std::uintptr_t>(trigger)!=g_cb2.trigger||!ActiveCB2(actor,now))return false;
    if(Rd<const void*>(trigger,8,nullptr)!=actor||!(Rd<std::uint8_t>(trigger,0x19,0)==1||Rd<std::uint8_t>(trigger,0x18,0)==1))return false;
    const int phase=Rd<int>(Rd<const void*>(actor,0x210,nullptr),0x38,-1);
    if(phase==4||phase==64)return false;
    if(phase!=8)return true; // retained existing CB2 continuation
    // CB3's direct native OPEN bypasses the F1 lease observer. Only the exact
    // ordinary cleanup of that action may retain UPDATE's individual hit read.
    return g_cb2.cleanedCB3&&OpenedActionCurrentCB3(actor,now,true)&&
        Rd<std::uint8_t>(trigger,0x19,0)==1&&Rd<std::uint8_t>(trigger,0x1B,0)==1&&
        Rd<std::uint8_t>(Rd<const void*>(actor,0x210,nullptr),0x868,0)==1;
}
static void __fastcall ManagerPhaseDetour(void* manager,void* a,void* b,int phase) noexcept {
    (void)SyncCB2();
    const auto original=g_manager_original;if(!original)return;
    if(g_teardown_pending19.load(std::memory_order_acquire)){original(manager,a,b,phase);return;}
    const void* actor=Rd<const void*>(manager,8,nullptr);
    if(actor&&actor==geom27::vrstate596::g_henry_actor.load(std::memory_order_relaxed)){
        const auto now=std::uint64_t(GetTickCount64());
        const auto phaseCaptureH15=BeginPhaseCaptureH15(now);
        cg::Owner activeOwner{};bool publishing=false;
        // A phase4 observed for another opponent retires the prior grace immediately.
        // Returning to A later in the old window cannot resurrect A's context.
        if(phase==4){RetireOpenedActionCB3();g_update_continuation_h10={};(void)Active(actor,now);(void)SyncCB2();}
        if(phase==1||phase==2){const bool active=Active(actor,now,&activeOwner,&publishing);
            if((active||publishing)&&activeOwner.manager==reinterpret_cast<std::uintptr_t>(manager)){
                g_deferred_owner19=activeOwner;g_deferred.fetch_add(1,std::memory_order_relaxed);
                TracePhaseH15(0,phaseCaptureH15,manager,actor,phase,"RETAIN_EXISTING_LINEAGE_SKIP_NATIVE_PHASE",activeOwner,now,publishing);return;}
            if(ActiveCB2(actor,now)&&g_cb2.manager==reinterpret_cast<std::uintptr_t>(manager)){ // CB2: same window per real combo step
                g_cb2.deferred=true;g_cb2_held.fetch_add(1,std::memory_order_relaxed);
                TracePhaseH15(0,phaseCaptureH15,manager,actor,phase,"CB2_REAL_STEP_WINDOW_SKIP_NATIVE_PHASE",{},now);return;}}
        if(phase==4&&g_cb2.deferred&&g_cb2.manager==reinterpret_cast<std::uintptr_t>(manager)&&g_deferred_owner19.manager!=reinterpret_cast<std::uintptr_t>(manager)){
            g_cb2.deferred=false;
            if(!ActiveCB2(actor,now)){g_cb2_late_reset.fetch_add(1,std::memory_order_relaxed);
                TracePhaseH15(0,phaseCaptureH15,manager,actor,1,"CB2_EXPIRED_DEFERRED_RESET_FORWARD_PLANNED",{},now);original(manager,a,b,1);}}
        if(phase==4&&g_deferred_owner19.manager==reinterpret_cast<std::uintptr_t>(manager)){
            const auto deferred=g_deferred_owner19;g_deferred_owner19={};cg::Owner current{};
            // A different opponent/manager/lifecycle cannot inherit A's deferred reset.
            if(CurrentOwner19(deferred,actor,current)&&!Active(actor,now)){g_late_resets.fetch_add(1,std::memory_order_relaxed);
                TracePhaseH15(0,phaseCaptureH15,manager,actor,1,"EXPIRED_DEFERRED_RESET_FORWARD_PLANNED",deferred,now);original(manager,a,b,1);}
        }
        if(phase==1||phase==2){g_deferred_owner19={};g_native_resets.fetch_add(1,std::memory_order_relaxed);}
        if(phase==64){ClearLineage19();(void)SyncCB2();}
        TracePhaseH15(0,phaseCaptureH15,manager,actor,phase,"NATIVE_PHASE_FORWARD_PLANNED",{},now);
    }
    original(manager,a,b,phase);
}
static void __fastcall TriggerPhaseDetour(void* trigger,void* a,void* b,int phase) noexcept {
    (void)SyncCB2();
    const auto original=g_trigger_original;if(!original)return;
    if(g_teardown_pending19.load(std::memory_order_acquire)){original(trigger,a,b,phase);return;}
    const void* actor=Rd<const void*>(trigger,8,nullptr);
    const auto now=std::uint64_t(GetTickCount64());
    const auto phaseCaptureH15=BeginPhaseCaptureH15(now);
    if(phase==4&&actor==geom27::vrstate596::g_henry_actor.load(std::memory_order_relaxed)){RetireOpenedActionCB3();g_update_continuation_h10={};}
    if(actor&&actor==geom27::vrstate596::g_henry_actor.load(std::memory_order_relaxed))g_cb2_trigger_seen=reinterpret_cast<std::uintptr_t>(trigger);
    // R9: phase 2 (attack end) closed the slot natively while the manager's phase 1/2 reset was skipped (182 forwarded,
    // NO_SLOT_AT_PRESS 119/119). inside a live window the trigger now keeps its slot for phase 1 AND 2.
    if(phase==1||(phase==2&&kcd2vr::combat_r9::keepSlotPhase2.load(std::memory_order_relaxed))){bool publishing=false;cg::Owner activeOwner{};const bool active=Active(actor,now,&activeOwner,&publishing)||ActiveCB2(actor,now);
        if(kcd2vr::combat_r9::keepTriggerSlot(phase,active||publishing,Rd<std::uint8_t>(trigger,0x19,0)||Rd<std::uint8_t>(trigger,0x18,0),
               kcd2vr::combat_r9::keepSlotPhase2.load(std::memory_order_relaxed))){
            g_kept_open.fetch_add(1,std::memory_order_relaxed);
            TracePhaseH15(1,phaseCaptureH15,trigger,actor,phase,"KEEP_EXISTING_SLOT_SKIP_NATIVE_PHASE",activeOwner,now,publishing);return;}} // original finite slot, never a new OPEN
    if(phase==64&&actor==geom27::vrstate596::g_henry_actor.load(std::memory_order_relaxed)){ClearLineage19();(void)SyncCB2();}
    // Flags alone may belong to an earlier call. Native 9D6A34 stores its
    // absolute deadline at trigger+28; require this call's open/timed transition
    // or a changed native deadline, without retaining the trigger afterwards.
    TriggerSnapshotF1 before{};cg::Owner openingOwner{};
    kcd2vr::subsystems::masterstrike_combos::NativeSlotF1 nativeBefore{},nativeAfter{};
    const bool henry=actor==geom27::vrstate596::g_henry_actor.load(std::memory_order_relaxed);
    const bool knownBefore=phase==16&&henry&&ReadTriggerF1(trigger,before);
    const auto inputGenerationH10=knownBefore?kcd2vr::frik_pose_probe::NativeComboInputGenerationH10():0;
    unsigned queryReason=0;
    if(knownBefore){openingOwner=NativeOpenedOwnerF1(actor,now,&queryReason,&nativeBefore);if(!cg::valid(openingOwner))(void)Active(actor,now,&openingOwner);}
    const auto mainBeforeCB3=knownBefore?CurrentMainCB3(actor):0;
    const auto generationCB3=SyncCB2();
    original(trigger,a,b,phase);
    if(knownBefore){TriggerSnapshotF1 afterCB3{};
        if(ReadTriggerF1(trigger,afterCB3))ConsumeDuplicateOpenCB3(trigger,actor,before,afterCB3,mainBeforeCB3,generationCB3,std::uint64_t(GetTickCount64()));}
    if(g_teardown_pending19.load(std::memory_order_acquire)){cg::nativeSlot.spend(openingOwner.release);return;}
    if(phase==16){ // C23b OUTCOME_CHAIN receipt: did THIS native phase-16 call request Henry's combo slot (+0x18, 909A90 passed)?
        const void *e23=nullptr,*s23=nullptr;
        if(actor&&CombatActor(actor,e23,s23,false)&&IsPlayer(e23)){const bool requested=Rd<std::uint8_t>(trigger,0x18,0)!=0;
            kcd2vr::outcome23::with([&](kcd2vr::outcome23::Core& k){k.slot16(requested,now,kcd2vr::outcome23::counters);});}}
    // CB3: a later native phase16 of the same strike re-starts the slot timer with its short native duration (0.5 s);
    // inside a live CB2 step window the VR window is kept (never shortened, same trigger/owner only).
    if(phase==16&&henry&&reinterpret_cast<std::uintptr_t>(trigger)==g_cb2.trigger&&ActiveCB2(actor,now)){
        TriggerSnapshotF1 cb3{};const float wanted=float(WindowMsH3())*.001f;
        if(ReadTriggerF1(trigger,cb3)&&(cb3.requested||cb3.open)&&cb3.timed&&std::isfinite(cb3.duration)&&cb3.duration<wanted&&RearmTimerCB2(trigger,wanted))
            g_cb2_rearmed.fetch_add(1,std::memory_order_relaxed);}
    // +0x18 = the deferred OPEN this very phase-16 call just requested (909A90 eligible); otherwise nothing to extend.
    TriggerSnapshotF1 after{};
    if(phase!=16||!henry){if(henry)TracePhaseH15(1,phaseCaptureH15,trigger,actor,phase,"NATIVE_PHASE_RETURNED",{},now);return;}
    if(!knownBefore||!ReadTriggerF1(trigger,after)){TracePhaseH15(1,phaseCaptureH15,trigger,actor,phase,"NATIVE_RETURN_SLOT_SNAPSHOT_UNAVAILABLE",openingOwner,now,false,knownBefore?&before:nullptr,nullptr,queryReason);NoteF1(8,queryReason);return;}
    if(before.actor!=after.actor||after.actor!=reinterpret_cast<std::uintptr_t>(actor)){TracePhaseH15(1,phaseCaptureH15,trigger,actor,phase,"NATIVE_RETURN_OWNER_CHANGED",openingOwner,now,false,&before,&after,queryReason);NoteF1(3,queryReason);return;}
    if(!after.requested||!after.timed||(before.requested&&before.timed&&after.deadline==before.deadline)){TracePhaseH15(1,phaseCaptureH15,trigger,actor,phase,"NATIVE_RETURN_NO_NEW_TIMED_OPEN",openingOwner,now,false,&before,&after,queryReason);NoteF1(1,queryReason);return;}
    TracePhaseH15(1,phaseCaptureH15,trigger,actor,phase,"NATIVE_RETURN_TIMED_OPEN_OBSERVED",openingOwner,now,false,&before,&after,queryReason);
    g_native_open_f1.fetch_add(1,std::memory_order_relaxed);
    const unsigned window=WindowMsH3();
    const bool explicitFiveSecondTest=window==::kcd2vr::combo_test_ds::kWindowMs&&::kcd2vr::combo_test_ds::force.load(std::memory_order_relaxed);
    const float native=after.duration,wanted=float(window)*.001f;
    if(!std::isfinite(native)||native<=0||!std::isfinite(wanted)||wanted<=0)return;
    auto opened=NativeOpenedOwnerF1(actor,now,&queryReason,&nativeAfter);
    if(!cg::valid(opened)&&!Active(actor,now,&opened)){TracePhaseH15(1,phaseCaptureH15,trigger,actor,phase,"OPEN_WITHOUT_CURRENT_RELEASE_LINEAGE",openingOwner,now,false,&before,&after,queryReason);cg::nativeSlot.spend(openingOwner.release);g_query_refused_f1.fetch_add(1,std::memory_order_relaxed);NoteF1(2,queryReason);return;} // retain qualified accepted-hit fallback
    if(!SameReleaseF1(openingOwner,opened)||((nativeBefore.action||nativeAfter.action)&&!SameNativeSlotF1(nativeBefore,nativeAfter))||
       (before.target&&before.target!=openingOwner.target)||
       (after.target&&after.target!=opened.target)){TracePhaseH15(1,phaseCaptureH15,trigger,actor,phase,"OPEN_RELEASE_OWNER_CHANGED",openingOwner,now,false,&before,&after,queryReason);cg::nativeSlot.spend(openingOwner.release);NoteF1(3,queryReason);return;}
    opened.tick=now; // immutable native-open anchor, never the later hit receipt
    if(!cg::nativeSlot.consume(opened,window,explicitFiveSecondTest)){TracePhaseH15(1,phaseCaptureH15,trigger,actor,phase,"ORIGINAL_RELEASE_ALREADY_CONSUMED",opened,now,false,&before,&after,queryReason);g_repeat_f1.fetch_add(1,std::memory_order_relaxed);NoteF1(4,queryReason,opened.release);return;}
    CaptureUpdateContinuationH10(trigger,opened,window,inputGenerationH10);
    cg::Owner current{};
    if(!CurrentOwner19(opened,actor,current)){TracePhaseH15(1,phaseCaptureH15,trigger,actor,phase,"OPEN_LINEAGE_RETIRED_BEFORE_EXTENSION",opened,now,false,&before,&after,queryReason);ClearLineage19();return;}
    if(!(wanted>native)){TracePhaseH15(1,phaseCaptureH15,trigger,actor,phase,"NATIVE_WINDOW_ALREADY_LONG",opened,now,false,&before,&after,queryReason);NoteF1(6,queryReason,opened.release,native,wanted);return;} // already-long native timers are never shortened
    __try{HostTimerStart(static_cast<unsigned char*>(trigger)+0x28,wanted,0.f);
        *reinterpret_cast<float*>(static_cast<unsigned char*>(trigger)+0x1C)=wanted;g_extended.fetch_add(1,std::memory_order_relaxed);TracePhaseH15(1,phaseCaptureH15,trigger,actor,phase,"EXISTING_TIMER_EXTENSION_RETURNED",opened,now,false,&before,&after,queryReason);NoteF1(5,queryReason,opened.release,native,wanted);}
    __except(EXCEPTION_EXECUTE_HANDLER){g_timer_fault_h19.store(true,std::memory_order_release);g_window_ms.store(0,std::memory_order_relaxed);ClearLineage19();g_deferred_owner19={};
        diagnostic_trace::Write("R59.16C10R17 COMBO_WINDOW_FAULT timer re-arm raised an exception; the VR combo window is OFF for this session reason=NATIVE_TIMER_START_EXCEPTION faultLatch=SESSION testOverride=REFUSED");}
}
// A failed disarm leaves the detour live: retain both its handle and trampoline.
// Teardown-pending detours only forward native; retry Uninstall rather than install over them.
static bool DropHook19(std::optional<DetourModKit::hook::Hook>& hook,PhaseObserverFn& original) noexcept {
    if(!hook){original=nullptr;return true;}
    if(hook->is_enabled()&&!hook->disable())return false;
    if(hook->is_enabled())return false;
    hook.reset();original=nullptr;return true;
}
static void Uninstall() noexcept {
    g_teardown_pending19.store(true,std::memory_order_release);g_ready.store(false);
    ClearLineage19();g_deferred_owner19={};
    const bool update=DropUpdateContinuationH10();
    const bool trigger=DropHook19(g_trigger_hook,g_trigger_original);
    const bool manager=DropHook19(g_manager_hook,g_manager_original);
    g_teardown_pending19.store(!(update&&trigger&&manager),std::memory_order_release);
    if(!update||!trigger||!manager)diagnostic_trace::Write("R59.16C10R19 COMBO_TEARDOWN_PENDING retained live observer/trampoline; native forwarding only; Uninstall may retry");
}
static bool Install() noexcept {
    if(g_teardown_pending19.load(std::memory_order_acquire))return false;
    if(g_ready.load())return true;
    // Retained handles must never be replaced, even if a caller lost ready state.
    if(g_manager_hook||g_trigger_hook){Uninstall();return false;}
    unsigned char site[sizeof(kTimerCallSite)]{},timer[sizeof(kTimerStartEntry)]{};
    if(!SehCopy146(reinterpret_cast<const void*>(g_base+0x911224),site,sizeof(site))||std::memcmp(site,kTimerCallSite,sizeof(site))||
       !SehCopy146(reinterpret_cast<const void*>(g_base+kTimerStart),timer,sizeof(timer))||std::memcmp(timer,kTimerStartEntry,sizeof(timer))){
        diagnostic_trace::Write("R59.16C10R17 COMBO_WINDOW unavailable: timer call site / entry mismatch (native combos unchanged)");return false;}
    if(!Hook(g_manager_hook,g_manager_original,kManagerPhase,kManagerPhaseEntry,&ManagerPhaseDetour,"kcd2vr_r59c17_combo_manager_phase")){Uninstall();return false;}
    if(!Hook(g_trigger_hook,g_trigger_original,kComboTriggerPhase,kComboTriggerPhaseEntry,&TriggerPhaseDetour,"kcd2vr_r59c17_combo_slot_phase")){Uninstall();return false;}
    if(!ArmUpdateContinuationH10())diagnostic_trace::Write("H10 COMBO_UPDATE_CONTINUATION unavailable; existing native-open window remains active");
    g_ready.store(true);
    diagnostic_trace::Write("F1 COMBO_NATIVE_SLOT ready=1 authority=NATIVE_OPEN_TIMED_CURRENT_HENRY_ORDINARY_RELEASE anchor=ONCE_PER_RELEASE acceptedHitCannotRefresh=1 NPC=UNCHANGED");
    return true;
}
}
