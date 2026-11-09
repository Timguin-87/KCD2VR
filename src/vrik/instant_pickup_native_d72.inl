// D72, included after the real-world prop driver. Native route, exact retail:
// normalOnUsed2A8A838 -> C_PlayerStateHandler33CA59C -> factory33CA0E8
// -> accepted C_PickUpAction3B5BE20 ->39C20C(forceImmediate=true)
// ->39926C -> vft+138/8C8910 -> typed request8C9F08 ->8CA6BC
// ->8CD484(animated=false) -> accepted transfer8CF6FC(animated=false).
// The native parent performs admission and its own completion. No Picking
// child, synthetic constructor result, action-state write or clip seek is used.
#include "../core/haptics.h"
namespace ipd72=kcd2vr::instant_pickup_d72;
template<class Fn> static Fn InstantPickupNativeD72(std::uintptr_t rva) noexcept {
#ifdef KCD2VR_INSTANT_PICKUP_HOST_TEST
    return reinterpret_cast<Fn>(HostInstantPickupLeafD72(rva));
#else
    return reinterpret_cast<Fn>(WhBase()+rva);
#endif
}
template<class T> static T InstantPickupReadD72(const void* p,std::size_t off,T fallback=T{}) noexcept {
    T v{};return p&&Read(static_cast<const unsigned char*>(p)+off,v)?v:fallback;
}
struct InstantPickupCandidateD72 {
    StagedProp596 prop{};void* player=nullptr;void* manager=nullptr;
    std::uint64_t serial=0,tick=0;
};
struct InstantPickupLeaseD72 {
    InstantPickupCandidateD72 candidate{};void* handler=nullptr;void* action=nullptr;void* control=nullptr;
};
// NormalOnUsed and its synchronous Henry service call are the same callback.
// The accepted parent alone crosses threads through the immutable mailbox.
static thread_local InstantPickupCandidateD72 g_instant_pickup_candidate_d72;
static void EndInstantPickupRequestD72() noexcept {g_instant_pickup_candidate_d72={};}
static kcd2vr::core::SnapshotMailbox<InstantPickupLeaseD72> g_instant_pickup_lease_d72;
static std::atomic<std::uint64_t> g_instant_pickup_next_d72{0},g_instant_pickup_live_d72{0},g_instant_pickup_consumed_d72{0};
static std::atomic<std::uint64_t> g_instant_pickup_hold_d72{0},g_instant_pickup_terminal_d72{0};
static std::atomic<std::uint64_t> g_instant_pickup_done_d72{0};
static std::atomic<bool> g_instant_pickup_result_d72{false};
static std::atomic<bool> g_instant_pickup_ready_d72{false};
struct InstantPickupAdmissionD72 {InstantPickupCandidateD72 candidate{};void* handler=nullptr;};
struct InstantPickupExecutionD72 {InstantPickupLeaseD72 lease{};bool transferSeen=false,result=false;};
static thread_local InstantPickupAdmissionD72* g_instant_pickup_admission_d72=nullptr;
static thread_local InstantPickupExecutionD72* g_instant_pickup_execution_d72=nullptr;
using InstantPickupHandlerFnD72=bool(__fastcall*)(void*,std::uint64_t,int,bool,bool);
using InstantPickupFactoryFnD72=void*(__fastcall*)(void*,void*,const std::uint64_t*,int);
using InstantPickupChoiceFnD72=void(__fastcall*)(void*,bool);
using InstantPickupExecuteFnD72=bool(__fastcall*)(void*);
using InstantPickupDestroyFnD72=void*(__fastcall*)(void*,unsigned);
static InstantPickupHandlerFnD72 g_instant_pickup_handler_d72=nullptr;
static InstantPickupFactoryFnD72 g_instant_pickup_factory_d72=nullptr;
static InstantPickupChoiceFnD72 g_instant_pickup_choice_d72=nullptr;
static InstantPickupExecuteFnD72 g_instant_pickup_execute_d72=nullptr;
static InstantPickupDestroyFnD72 g_instant_pickup_destroy_d72=nullptr;
static std::optional<DetourModKit::hook::Hook> g_instant_pickup_hooks_d72[5];

static void InstantPickupNoteD72(const char* phase,const InstantPickupLeaseD72& lease,bool result) noexcept {
    if(!kcd2vr::diagnostics::capture168::active(kcd2vr::diagnostics::capture168::Domain::Interaction,GetTickCount64()))return;
    char line[400]{};const auto& c=lease.candidate;
    std::snprintf(line,sizeof(line),"D72 INSTANT_PICKUP phase=%s token=%llu target=%u key=%016llX side=%d result=%d admission=NATIVE_PARENT child=NONE inventory=DEFERRED_TO_STOW",
        phase,(unsigned long long)c.serial,c.prop.id,(unsigned long long)c.prop.key,c.prop.side,result);
    diagnostic_trace::Write(line);
}
static bool InstantPickupUnclaimedD72(const InstantPickupCandidateD72& c) noexcept {
    const int side=c.prop.side;if(side<0||side>1)return false;
    namespace admission=kcd2vr::mutual_admission_t25;
    return !action_time_owner59c11::NativePickupOutstandingD72()&&
        !(kcd2vr::corpse_grip_cg1::claims.reserved()&(1u<<side))&&
        !admission::active[side].load(std::memory_order_acquire)&&!admission::doorClaimsH41[side].load(std::memory_order_acquire)&&
        // R124: the transfer below retains pickupClaimsD72[side] and refuses an ammo-carrying hand. Those refusals came
        // AFTER forceImmediate was chosen, so the native immediate transfer put the item straight into the inventory.
        !admission::pickupClaimsD72[side].load(std::memory_order_acquire)&&
        !(kcd2vr::ammo_carry_t18::hands.load(std::memory_order_acquire)&(1u<<side))&&
        !kcd2vr::hand_claim_t24::competing(side,{reinterpret_cast<std::uintptr_t>(c.player),reinterpret_cast<std::uintptr_t>(c.manager),c.prop.character,c.prop.bodyEpoch,0})&&
        kcd2vr::reload_throw_claim_t25::observeReloadDebt(side,reinterpret_cast<std::uintptr_t>(c.player))==kcd2vr::reload_throw_claim_t25::Admission::Allowed;
}
static bool InstantPickupCurrentD72(const InstantPickupCandidateD72& c,std::uint64_t now,bool reach) noexcept {
    if(!c.serial||!c.tick||c.prop.side<0||c.prop.side>1||
       ipd72::stopping.load(std::memory_order_acquire)||!PropIdentity596(c.prop)||
       !PropHandFree596(c.prop.side)||!InstantPickupUnclaimedD72(c)||!PropInstantContextD78(now)||
       g_henry_item_attach_manager.load(std::memory_order_acquire)!=c.manager||
       !PhysVft6698(c.manager,0x3EC9958)||!PhysVft6698(c.player,0x3AA4B98)||
       InstantPickupReadD72<void*>(c.manager,0x38)!=c.player||
       InstantPickupReadD72<void*>(c.player,0x9D0)!=c.manager||
       InstantPickupReadD72<void*>(c.player,0x38)!=g_last_player_entity.load(std::memory_order_acquire))return false;
    CommittedFrame6695 f{};
    if(!ReadCommittedPacket6695(f)||!f.valid[c.prop.side]||!f.mapFoldTrackedM39||!f.headEvidence122||
       !f.stamp.tick||f.stamp.actor!=c.prop.character||
       f.stamp.epoch!=c.prop.bodyEpoch||c.prop.epoch!=c.prop.bodyEpoch||
       !std::isfinite(f.mapGripM39[c.prop.side])||f.mapGripM39[c.prop.side]<.58f||
       pickup_input_d80::refusal(f,c.prop.side,FistInputOwned101()))return false;
    ph::Pose object{},hand{};
    if(reach&&(!PropPickupHandD81(f,c.prop.side,c.prop.physics,hand)||
        !PropObjectPose5916C1(c.prop,object)||pm::length(hand.p-object.p)>.25))return false;
    now=std::uint64_t(GetTickCount64()); // validate the copies AFTER the native/pose reads
    return now>=c.tick&&now-c.tick<=80&&now>=f.stamp.tick&&now-f.stamp.tick<=80&&PropInstantContextD78(now);
}
static void* InstantPickupContainerD72(void* item) noexcept {
    auto* p=InstantPickupReadD72<void*>(item,0x98);return p?p:InstantPickupReadD72<void*>(item,0x90);
}
static bool InstantPickupOrdinaryD72(const InstantPickupCandidateD72& c,void*& inventory) noexcept {
    inventory=nullptr;bool unavailable=true,special=true;
    __try {
        inventory=InstantPickupNativeD72<void*(__fastcall*)(void*)>(0x8D285C)(c.player);
        unavailable=InstantPickupNativeD72<bool(__fastcall*)(void*,unsigned)>(0x469590)(c.prop.item,1);
        // Same native ownership classification as8CD484 at8CD531. Unsupported
        // special transfers are excluded before choosing the immediate branch.
        special=InstantPickupNativeD72<bool(__fastcall*)(void*,void*,bool)>(0x8CE1F8)(c.player,c.prop.item,false);
    }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
    return inventory&&!unavailable&&!special&&InstantPickupContainerD72(c.prop.item)!=inventory&&
        InstantPickupReadD72<std::uint32_t>(c.prop.item,0x50)!=0&&
        InstantPickupReadD72<void*>(c.prop.item,0x88)!=nullptr;
}
static bool InstantPickupActionIdentityD72(void* action,const InstantPickupLeaseD72& out) noexcept {
    if(!out.candidate.serial||out.action!=action||
       out.handler!=InstantPickupReadD72<void*>(reinterpret_cast<void*>(WhBase()),0x54961D0)||
       !PhysVft6698(out.handler,0x4001F10)||
       !PhysVft6698(action,0x3B5BE20)||
       out.control!=static_cast<unsigned char*>(action)-0x10||
       !PhysVft6698(out.control,0x3FCF7E8)||InstantPickupReadD72<int>(out.control,8)<=0||
       InstantPickupReadD72<void*>(action,0x98)!=InstantPickupReadD72<void*>(out.handler,0x248)||
       InstantPickupReadD72<std::uint64_t>(action,0x1A8)!=out.candidate.prop.key||
       InstantPickupReadD72<int>(action,0x1A0,-1)!=out.candidate.prop.side||
       InstantPickupReadD72<unsigned char>(action,0x1B8)!=1)return false;
    void* expansion=nullptr;void* manager=nullptr;
    __try {
        expansion=InstantPickupNativeD72<void*(__fastcall*)(void*)>(0x8C7758)(action);
        if(!PhysVft6698(expansion,0x3B55E30))return false;
        manager=InstantPickupNativeD72<void*(__fastcall*)(void*)>(0x8CBED8)(expansion);
    }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
    return manager==out.candidate.manager;
}
static bool InstantPickupLeaseCurrentD72(void* action,InstantPickupLeaseD72& out) noexcept {
    const auto live=g_instant_pickup_live_d72.load(std::memory_order_acquire);
    return live&&g_instant_pickup_lease_d72.read(out)&&out.candidate.serial==live&&
        g_instant_pickup_consumed_d72.load(std::memory_order_acquire)!=live&&
        InstantPickupReadD72<unsigned char>(action,0xBD)==1&&
        InstantPickupActionIdentityD72(action,out)&&InstantPickupCurrentD72(out.candidate,GetTickCount64(),true);
}
static void* __fastcall InstantPickupDestroyD72(void* action,unsigned flags) noexcept {
    // RefCountObj3FCF7E8 ->3DBE54 calls the object's real vft+0 before reuse.
    // No extra reference is taken: cancellation/destruction remains native.
    InstantPickupLeaseD72 lease{};
    if(g_instant_pickup_lease_d72.read(lease)&&lease.action==action){
        auto live=lease.candidate.serial;(void)g_instant_pickup_live_d72.compare_exchange_strong(live,0);
        auto consumed=lease.candidate.serial;(void)g_instant_pickup_consumed_d72.compare_exchange_strong(consumed,0);
    }
    return g_instant_pickup_destroy_d72?g_instant_pickup_destroy_d72(action,flags):action;
}
// D85 interact: the native Henry pickup request 33CA59C takes the hand as an
// explicit argument; normalOnUsed 2A8A8A3 passes the default from 143DF48,
// which is 0 (right) unless the right hand already holds an item, so native
// itself also picks with hand 1. The instant path below already passes the
// selected VR hand here. When instant is refused and the native animated
// pickup continues, a LEFT-hand VR Use was given to the right hand (D79
// 17:10:30.194 Use hand1, 17:10:30.610 item in hand_right) and native grasp
// then refused it. Pass the physically requesting, currently empty VR hand for
// the exact same transported Use and item; otherwise keep the native choice.
static int InstantPickupRequestedHandD85(std::uint64_t key,int nativeHand,int* requestedOut=nullptr) noexcept {
    if(requestedOut)*requestedOut=-1;
    if(nativeHand<0||nativeHand>1)return nativeHand;
    void* sys=InstantPickupReadD72<void*>(reinterpret_cast<void*>(WhBase()),0x5487A78);void* item=nullptr;
    if(!sys)return nativeHand;
    __try {item=InstantPickupNativeD72<void*(__fastcall*)(void*,std::uint64_t)>(0x23D09E0)(sys,key);}
    __except(EXCEPTION_EXECUTE_HANDLER){return nativeHand;}
    if(!PhysVft6698(item,0x3A2DA88)||InstantPickupReadD72<std::uint64_t>(item,0x30)!=key)return nativeHand;
    const auto entity=InstantPickupReadD72<std::uint32_t>(InstantPickupReadD72<void*>(item,0x88),0x30);
    int requested=-1;
    if(!entity||!action_time_owner59c11::RequestedPickupSideD85(entity,requested)||requested<0||requested>1)return nativeHand;
    if(requestedOut)*requestedOut=requested;
    return requested!=nativeHand&&PropHandFree596(requested)?requested:nativeHand;
}
static bool __fastcall InstantPickupHandlerD72(void* handler,std::uint64_t key,int hand,bool extra,bool fifth) noexcept {
    const auto original=g_instant_pickup_handler_d72;if(!original)return false;
    InstantPickupAdmissionD72 scope{};scope.handler=handler;scope.candidate=g_instant_pickup_candidate_d72;
#ifdef KCD2VR_INSTANT_PICKUP_HOST_TEST
    const auto caller=HostInstantPickupCallerD72();
#else
    const auto caller=reinterpret_cast<std::uintptr_t>(_ReturnAddress());
#endif
    const bool qualified=g_instant_pickup_ready_d72.load(std::memory_order_acquire)&&
        caller==WhBase()+0x2A8A8F4&&scope.candidate.serial&&scope.candidate.prop.key==key&&
        handler==InstantPickupReadD72<void*>(reinterpret_cast<void*>(WhBase()),0x54961D0)&&
        PhysVft6698(handler,0x4001F10)&&InstantPickupCurrentD72(scope.candidate,GetTickCount64(),true);
    if(!qualified){
        int requested=-1;
        const int applied=g_instant_pickup_ready_d72.load(std::memory_order_acquire)&&caller==WhBase()+0x2A8A8F4?
            InstantPickupRequestedHandD85(key,hand,&requested):hand;
        if(requested>=0&&kcd2vr::diagnostics::capture168::active(kcd2vr::diagnostics::capture168::Domain::Interaction,GetTickCount64())){
            char line[200]{};std::snprintf(line,sizeof(line),"D85 F5_PICKUP_HAND path=NATIVE_FALLBACK key=%016llX requested=%d native=%d applied=%d",
                (unsigned long long)key,requested,hand,applied);diagnostic_trace::Write(line);
        }
        return original(handler,key,applied,extra,fifth);
    }
    // This is native input selection: the normal API explicitly takes hand0/1.
    // All native hand constraints and accepted-request construction still run.
    auto* previous=g_instant_pickup_admission_d72;g_instant_pickup_admission_d72=&scope;
    bool result=false;
    __try {result=original(handler,key,scope.candidate.prop.side,extra,fifth);}
    __finally {g_instant_pickup_admission_d72=previous;}
    EndInstantPickupRequestD72();
    if(!result){auto serial=scope.candidate.serial;(void)g_instant_pickup_live_d72.compare_exchange_strong(serial,0);}
    return result;
}
static void* __fastcall InstantPickupFactoryD72(void* self,void* result,const std::uint64_t* key,int hand) noexcept {
    const auto original=g_instant_pickup_factory_d72;if(!original)return result;
    void* returned=original(self,result,key,hand);
    auto* scope=g_instant_pickup_admission_d72;
    if(!scope||InstantPickupReadD72<std::uint64_t>(key,0)!=scope->candidate.prop.key||
       hand!=scope->candidate.prop.side)return returned;
    auto* action=InstantPickupReadD72<void*>(result,0);auto* control=InstantPickupReadD72<void*>(result,8);
    if(!action||!control||control!=static_cast<unsigned char*>(action)-0x10||
       !PhysVft6698(control,0x3FCF7E8)||!PhysVft6698(action,0x3B5BE20)||
       InstantPickupReadD72<std::uint64_t>(action,0x1A8)!=scope->candidate.prop.key||
       InstantPickupReadD72<int>(action,0x1A0,-1)!=hand||InstantPickupReadD72<unsigned char>(action,0x1B8)!=1)return returned;
    InstantPickupLeaseD72 lease{scope->candidate,scope->handler,action,control};
    if(g_instant_pickup_lease_d72.publish(lease))g_instant_pickup_live_d72.store(scope->candidate.serial,std::memory_order_release);
    return returned;
}
// R124 (Miki 6 Nov, R123: "apple disappeared to my inventory, it did not create a held apple"): forceImmediate means
// "no animation, the transfer happens NOW". If our transfer intercept then refuses the world hold, the native immediate
// transfer runs and the item lands in the inventory with no hand and no animation. So before forcing immediate, check
// the same things the intercept needs for the held item (rebuilt world prop with the same identity = rigid body within
// 25 cm of the gripping hand, no other held prop, free hand claim). If the held item is impossible, the native choice stays (game pickup animation; the native grasp
// still puts the item in the hand) and one short line says why. Every squeeze is a new attempt, nothing is latched.
static const char* InstantPickupHoldRefusalD72(const InstantPickupCandidateD72& c) noexcept {
    StagedProp596 p{};
    if(!BuildStagedPickup596(c.prop.controller,c.prop.actor,true,&p))return "WORLD_ITEM_NOT_HOLDABLE_NOW";
    if(p.entity!=c.prop.entity||p.item!=c.prop.item||p.body!=c.prop.body||p.key!=c.prop.key||
       p.side!=c.prop.side||p.character!=c.prop.character||p.bodyEpoch!=c.prop.bodyEpoch)return "ITEM_OR_HAND_CHANGED";
    {PropLock596 lock;if(!lock.owned)return "PROP_LOCK_BUSY";if(g_prop596.id)return "ANOTHER_ITEM_ALREADY_HELD";}
    return nullptr;
}
static void __fastcall InstantPickupChoiceD72(void* action,bool immediate) noexcept {
    const auto original=g_instant_pickup_choice_d72;if(!original)return;
    InstantPickupLeaseD72 lease{};void* inventory=nullptr;
    bool ours=InstantPickupLeaseCurrentD72(action,lease)&&InstantPickupOrdinaryD72(lease.candidate,inventory);
    if(ours)if(const char* why=InstantPickupHoldRefusalD72(lease.candidate)){
        ours=false;
        // retire this lease so the execute hook passes the animated pickup straight to the game
        auto live=lease.candidate.serial;(void)g_instant_pickup_live_d72.compare_exchange_strong(live,0);
        char line[200]{};std::snprintf(line,sizeof(line),"R124 INSTANT_PICKUP_HELD_NOT_POSSIBLE reason=%s key=%016llX -> game pickup animation",
            why,(unsigned long long)lease.candidate.prop.key);diagnostic_trace::Write(line);
    }
    // The caller and normal completion remain native. forceImmediate is a real
    // native API argument, not a raw state or success byte written by the mod.
    original(action,ours?true:immediate);
}
static bool __fastcall InstantPickupExecuteD72(void* action) noexcept {
    const auto original=g_instant_pickup_execute_d72;if(!original)return false;
    InstantPickupExecutionD72 scope{};
    if(g_instant_pickup_lease_d72.read(scope.lease)&&
       g_instant_pickup_consumed_d72.load(std::memory_order_acquire)==scope.lease.candidate.serial&&
       InstantPickupActionIdentityD72(action,scope.lease))
        return g_instant_pickup_done_d72.load(std::memory_order_acquire)==scope.lease.candidate.serial&&
            g_instant_pickup_result_d72.load(std::memory_order_acquire);
    if(!InstantPickupLeaseCurrentD72(action,scope.lease))return original(action);
    const auto serial=scope.lease.candidate.serial;
    auto prior=g_instant_pickup_consumed_d72.load(std::memory_order_acquire);
    if(prior==serial)return false;
    if(!g_instant_pickup_consumed_d72.compare_exchange_strong(prior,serial,std::memory_order_acq_rel))return original(action);
    auto* previous=g_instant_pickup_execution_d72;g_instant_pickup_execution_d72=&scope;
    bool result=false;
    __try {result=original(action);}
    __finally {g_instant_pickup_execution_d72=previous;}
    g_instant_pickup_result_d72.store(result,std::memory_order_release);
    g_instant_pickup_done_d72.store(serial,std::memory_order_release);
    auto active=serial;(void)g_instant_pickup_live_d72.compare_exchange_strong(active,0);
    InstantPickupNoteD72(scope.transferSeen?"NATIVE_IMMEDIATE_RETURN":"NATIVE_IMMEDIATE_NO_TRANSFER",scope.lease,result);
    return result;
}

static const char* InstantPickupHelperD72(const char* source) noexcept {
    // Same retained char* contract as native8D0404. Never borrow a request-owned
    // string or reuse storage while native callbacks could still refer to it.
    static char strings[32][96]{};static unsigned count=0;
    char copy[96]{};bool terminated=false;
    for(unsigned i=0;i<sizeof(copy);++i){if(!source||!Read(source+i,copy[i]))return nullptr;if(!copy[i]){terminated=true;break;}}
    if(!terminated)return nullptr;
    //8CFDEC initializes a shared EMPTY string. The immediate builder returns
    // before animation helper selection, and request+10=true skips native hand
    // attachment on stow. Preserve that valid empty helper byte-for-byte.
    static constexpr char empty[]{0};if(!copy[0])return empty;
    for(unsigned i=0;i<count;++i)if(!std::strcmp(strings[i],copy))return strings[i];
    if(count==32)return nullptr;
    char* p=strings[count++];std::memcpy(p,copy,sizeof(copy));return p;
}
static bool InstantPickupNotifyD72(ipd72::Notice notice,void* item,void* player) noexcept {
    __try {notice(nullptr,item,player);return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
static bool InstantPickupTransferD72(void* manager,const void* request,bool animate,
    ipd72::Transfer original,ipd72::Notice notice,bool& handled) noexcept {
    handled=false;auto* scope=g_instant_pickup_execution_d72;
    if(!scope||animate||!original||!notice)return false;
    auto& c=scope->lease.candidate;
    if(manager!=c.manager||InstantPickupReadD72<std::uint64_t>(request,0)!=c.prop.key||
       InstantPickupReadD72<int>(request,8,-1)!=c.prop.side)return false;
    if(scope->transferSeen){handled=true;return scope->result;}
    // R124: from here every refusal means the native immediate transfer stores the item (no hand, no animation).
    // One short line, always, so the log says why the held item was impossible for this pickup.
    const auto toInventory=[&](const char* why) noexcept {
        char line[200]{};std::snprintf(line,sizeof(line),"R124 INSTANT_PICKUP_TO_INVENTORY reason=%s key=%016llX (held item impossible, game stored it)",
            why,(unsigned long long)c.prop.key);diagnostic_trace::Write(line);return false;
    };
    void* inventory=nullptr;
    if(!InstantPickupCurrentD72(c,GetTickCount64(),true))return toInventory("HAND_OR_ITEM_NOT_CURRENT");
    if(!InstantPickupOrdinaryD72(c,inventory))return toInventory("NOT_AN_ORDINARY_PICKUP");
    if(
       InstantPickupReadD72<unsigned char>(request,0xD)!=1||
       InstantPickupReadD72<unsigned char>(request,0xE)!=0||InstantPickupReadD72<unsigned char>(request,0xF)!=0||
       InstantPickupReadD72<unsigned char>(request,0x10)!=1||
       InstantPickupReadD72<void*>(request,0xA8)!=nullptr|| // no retained native callback object
       InstantPickupReadD72<std::uint32_t>(request,0xB8,~0u)!=0||
       (InstantPickupReadD72<void*>(request,0xC0)&&InstantPickupReadD72<void*>(request,0xC0)!=inventory))return toInventory("NATIVE_REQUEST_SHAPE");
    // Re-read the actual world object and coherent hand, rather than refreshing
    // an old request's timestamp. This still performs no inventory mutation.
    StagedProp596 p{};
    if(!BuildStagedPickup596(c.prop.controller,c.prop.actor,true,&p))return toInventory("WORLD_ITEM_NOT_HOLDABLE_NOW");
    if(p.entity!=c.prop.entity||p.item!=c.prop.item||p.body!=c.prop.body||p.key!=c.prop.key||
       p.side!=c.prop.side||p.character!=c.prop.character||p.bodyEpoch!=c.prop.bodyEpoch)return toInventory("ITEM_OR_HAND_CHANGED");
    PropLock596 lock;if(!lock.owned||g_prop596.id)return toInventory("ANOTHER_ITEM_ALREADY_HELD");
    if(!Safe(request,sizeof(p.acceptedD72)))return toInventory("NATIVE_REQUEST_UNREADABLE");
    std::memcpy(p.acceptedD72,request,sizeof(p.acceptedD72));
    const auto* helper=InstantPickupHelperD72(InstantPickupReadD72<const char*>(request,0x18));if(!helper)return toInventory("NATIVE_HELPER_STRING");
    std::memcpy(p.acceptedD72+0x18,&helper,sizeof(helper));
    p.managerD72=manager;p.playerD72=c.player;p.inventoryD72=inventory;
    p.containerD72=InstantPickupContainerD72(p.item);p.ownerD72=InstantPickupReadD72<void*>(p.item,0x90);
    p.countD72=InstantPickupReadD72<std::uint32_t>(p.item,0x50);p.transferD72=original;
    p.instantD72=true;p.noticeD72=true;p.instantTokenD72=c.serial;
    if(!p.physics&&!PropVelocity596(p.entity))return toInventory("BODY_NOT_RIGID");
    // R153 (VrPropFirmHoldR116 "Held small props: firm grip", ON): the item is carried locked to the hand from the
    // first frame with the entity transform - the same carry R96 already switched to after the body did not follow
    // the velocity servo for 100 ms (R152 run 11:18:47.037 accepted -> 11:18:47.289 KINEMATIC_CARRY_R96 = the visible
    // delay with the hand glued to the item). OFF = the old servo first, R96 switch after 100 ms.
    if(p.physics&&prop5916::firmHoldR116.load(std::memory_order_acquire)){p.physics=false;p.kinematicR96=true;}
    if(!kcd2vr::mutual_admission_t25::RetainPickupD72(p.side,c.serial))return toInventory("HAND_CLAIMED_BY_OTHER_GRAB");
    // A peer may have completed and published persistent debt between the
    // earlier observation and our CAS. Retain no claim if that hand is owned.
    if(action_time_owner59c11::NativePickupOutstandingD72()||
       (kcd2vr::ammo_carry_t18::hands.load(std::memory_order_acquire)&(1u<<p.side))||
       kcd2vr::reload_throw_claim_t25::observeReloadDebt(p.side,reinterpret_cast<std::uintptr_t>(c.player))!=kcd2vr::reload_throw_claim_t25::Admission::Allowed){
        (void)kcd2vr::mutual_admission_t25::RetirePickupD72(p.side,c.serial);return toInventory("HAND_BUSY_NATIVE_PICKUP_AMMO_OR_RELOAD");
    }
    // Claim before the first semantic side effect. A notice fault is terminal;
    // never fall through to another notice/transfer or retry this occurrence.
    scope->transferSeen=true;handled=true;g_prop596=p;
    g_instant_pickup_terminal_d72.store(0,std::memory_order_release);
    g_instant_pickup_hold_d72.store(c.serial,std::memory_order_release);ipd72::active.store(1,std::memory_order_release);
    const bool notified=InstantPickupNotifyD72(notice,p.item,c.player);
    if(!notified||!PropIdentity596(p)||!PropHandFree596(p.side)||
       InstantPickupContainerD72(p.item)!=p.containerD72||InstantPickupReadD72<void*>(p.item,0x90)!=p.ownerD72){
        PropRetireInstantD72(g_prop596);g_prop596={};scope->result=false;return false;
    }
    scope->result=true;
    kcd2vr::instant_d72::Publish(kcd2vr::instant_d72::Family::Interactions,kcd2vr::instant_d72::State::Active);
    PropNote596("INSTANT_NATIVE_ACCEPTED_WORLD_HOLD",p,true,"NATIVE_NOTICE_ONCE_NO_PICKING_CHILD");
    PropSelfCollisionR155(g_prop596,GetTickCount64()); // R155: from the first frame the item does not push Henry (R103 lease)
    // D85 interact: "taken" tick on the acquiring hand after the native notice
    // and identity readback succeeded; refused/failed transfers never buzz.
    (void)kcd2vr::haptics101::publish_event(p.side,p.key,.35f,35,GetTickCount64());
    return true;
}

static bool InstallInstantPickupD72() noexcept {
    if(g_instant_pickup_ready_d72.load(std::memory_order_acquire))return true;
    for(const auto& h:g_instant_pickup_hooks_d72)if(h)return false;
    if(!retail_build112::Ready(WhBase())||!action_time_owner59c11::Installed())return false;
    static constexpr StageLeafPin596 pins[]{
        {0x33CA59C,{0x48,0x89,0x5C,0x24,0x18,0x48,0x89,0x74}},
        {0x33CA0E8,{0x48,0x89,0x5C,0x24,0x08,0x48,0x89,0x74}},
        {0x39C20C,{0x48,0x89,0x5C,0x24,0x08,0x57,0x48,0x83}},
        {0x8C8910,{0x48,0x89,0x5C,0x24,0x08,0x48,0x89,0x74}},
        {0x6C7724,{0x48,0x89,0x5C,0x24,0x08,0x57,0x48,0x83}},
        {0x8C7758,{0x48,0x83,0xEC,0x28,0x48,0x8B,0x89,0x98}},
        {0x8CBED8,{0x48,0x83,0xEC,0x28,0x48,0x8B,0x49,0x08}},
        {0x8D285C,{0x48,0x83,0xEC,0x28,0x48,0x8B,0x89,0x68}},
        {0x469590,{0x40,0x53,0x48,0x83,0xEC,0x20,0x8B,0xDA}},
        {0x8CE1F8,{0x48,0x89,0x5C,0x24,0x08,0x48,0x89,0x6C}},
        {0x8CA6BC,{0x48,0x89,0x5C,0x24,0x18,0x55,0x56,0x57}},
        {0x8CD484,{0x48,0x8B,0xC4,0x48,0x89,0x58,0x08,0x48}},
        {0x8CFDEC,{0x48,0x89,0x5C,0x24,0x08,0x48,0x89,0x74}},
        {0x2A8A8EA,{0x4C,0x8B,0x51,0x10,0x48,0x8B,0xC8,0x41}},
        {0x39B2EB,{0x33,0xD2,0x48,0x8B,0xCB,0xE8,0x17,0x0F}},
    };
    if(!StageLeafPins596(pins)||InstantPickupReadD72<std::uintptr_t>(reinterpret_cast<void*>(WhBase()),0x3B5BF58)!=WhBase()+0x8C8910||
       InstantPickupReadD72<std::uintptr_t>(reinterpret_cast<void*>(WhBase()),0x3B55EB8)!=WhBase()+0x8CA6BC)return false;
    const auto install=[&](unsigned i,const char* name,std::uintptr_t rva,auto detour,auto& original) noexcept {
        DetourModKit::hook::InlineRequest r{name,DetourModKit::hook::Target{DetourModKit::Address{WhBase()+rva}},DetourModKit::hook::Options{}};
        auto h=DetourModKit::hook::inline_at(std::move(r),detour);if(!h)return false;
        g_instant_pickup_hooks_d72[i]=std::move(*h);
        original=g_instant_pickup_hooks_d72[i]->original<std::decay_t<decltype(original)>>();
        return original&&g_instant_pickup_hooks_d72[i]->enable();
    };
    const bool ok=install(0,"kcd2vr_d72_pickup_henry_request",0x33CA59C,&InstantPickupHandlerD72,g_instant_pickup_handler_d72)&&
        install(1,"kcd2vr_d72_pickup_parent_factory",0x33CA0E8,&InstantPickupFactoryD72,g_instant_pickup_factory_d72)&&
        install(2,"kcd2vr_d72_pickup_native_choice",0x39C20C,&InstantPickupChoiceD72,g_instant_pickup_choice_d72)&&
        install(3,"kcd2vr_d72_pickup_immediate_execute",0x8C8910,&InstantPickupExecuteD72,g_instant_pickup_execute_d72)&&
        install(4,"kcd2vr_d72_pickup_parent_destroy",0x6C7724,&InstantPickupDestroyD72,g_instant_pickup_destroy_d72);
    if(!ok){
        for(auto& h:g_instant_pickup_hooks_d72)if(h&&h->is_enabled())(void)h->disable();
        // Retain originals/handles after any partial install. No unsafe retry or
        // replacement while an unproved inline detour might remain reachable.
        return false;
    }
    ipd72::stopping.store(false,std::memory_order_release);
    ipd72::intercept.store(&InstantPickupTransferD72,std::memory_order_release);
    g_instant_pickup_ready_d72.store(true,std::memory_order_release);return true;
}
// 0: preference off (legacy native policy);1: an existing held occurrence;
// 2: exact candidate prepared;3: unsupported, leave this native Use unchanged.
static int InstantPickupRequestD72(void* controller,std::uint32_t actor) noexcept {
    EndInstantPickupRequestD72();
    if(g_prop_replay596)return 3;
    if(ipd72::active.load(std::memory_order_acquire))return LifecycleHenry165(actor)?1:3;
    {PropLock596 lock;if(!lock.owned)return 3;if(g_prop596.id&&g_prop596.instantD72)return LifecycleHenry165(actor)?1:3;}
    if(!kcd2vr::instant_d72::Interactions())return 0;
    InstantPickupCandidateD72 c{};
    if(!InstallInstantPickupD72()||!BuildStagedPickup596(controller,actor,true,&c.prop)){
        // R120: one short line, always (the F5 refusal row above only exists while F5 records)
        diagnostic_trace::Write(g_instant_pickup_ready_d72.load(std::memory_order_acquire)?
            "R120 INSTANT_PICKUP_REFUSED stage=WORLD_ITEM_NOT_HOLDABLE (static/unprepared body, hand or frame not admitted; F5 row C10 F5_PICKUP_REFUSED names it) -> game animation":
            "R120 INSTANT_PICKUP_REFUSED stage=NATIVE_HOOKS_NOT_INSTALLED -> game animation");
        kcd2vr::instant_d72::Publish(kcd2vr::instant_d72::Family::Interactions,kcd2vr::instant_d72::State::Unsupported);return 3;
    }
    c.tick=GetTickCount64();
    c.manager=g_henry_item_attach_manager.load(std::memory_order_acquire);
    c.player=InstantPickupReadD72<void*>(c.manager,0x38);c.serial=++g_instant_pickup_next_d72;
    void* inventory=nullptr;
    if(!InstantPickupCurrentD72(c,c.tick,true)||!InstantPickupOrdinaryD72(c,inventory)){
        diagnostic_trace::Write("R120 INSTANT_PICKUP_REFUSED stage=NOT_AN_ORDINARY_CURRENT_PICKUP -> game animation");return 3;}
    g_instant_pickup_candidate_d72=c;
    kcd2vr::instant_d72::Publish(kcd2vr::instant_d72::Family::Interactions,kcd2vr::instant_d72::State::Available);
    return 2;
}
static bool PropCommitInstantNativeD72(const StagedProp596& p) noexcept {
    bool result=false;
    __try {result=p.transferD72(p.managerD72,p.acceptedD72,false);}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
    // World-link destruction is allowed here. Revalidate the actual inventory
    // item by WUID; never read the former controller/entity after native stow.
    void* sys=InstantPickupReadD72<void*>(reinterpret_cast<void*>(WhBase()),0x5487A78);void* item=nullptr;
    __try {if(sys)item=InstantPickupNativeD72<void*(__fastcall*)(void*,std::uint64_t)>(0x23D09E0)(sys,p.key);}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
    return result&&PhysVft6698(item,0x3A2DA88)&&InstantPickupReadD72<std::uint64_t>(item,0x30)==p.key&&
        InstantPickupContainerD72(item)==p.inventoryD72;
}
static bool PropCommitInstantD72(const StagedProp596& p) noexcept {
    if(!p.instantD72||!p.instantTokenD72||g_instant_pickup_hold_d72.load(std::memory_order_acquire)!=p.instantTokenD72||
       !p.noticeD72||!p.transferD72||!PropIdentity596(p)||!PropHandFree596(p.side)||
       InstantPickupReadD72<void*>(p.managerD72,0x38)!=p.playerD72||
       InstantPickupReadD72<void*>(p.playerD72,0x9D0)!=p.managerD72||
       InstantPickupContainerD72(p.item)!=p.containerD72||InstantPickupReadD72<void*>(p.item,0x90)!=p.ownerD72||
       InstantPickupReadD72<std::uint32_t>(p.item,0x50)!=p.countD72)return false;
    void* inventory=nullptr;
    __try {inventory=InstantPickupNativeD72<void*(__fastcall*)(void*)>(0x8D285C)(p.playerD72);}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
    if(inventory!=p.inventoryD72)return false;
    std::uint64_t none=0;if(!g_instant_pickup_terminal_d72.compare_exchange_strong(none,p.instantTokenD72,std::memory_order_acq_rel))return false;
    const auto previous=ipd72::noticeScope;ipd72::noticeScope={p.item,p.playerD72};
    bool result=false;
    __try {result=PropCommitInstantNativeD72(p);}
    __finally {ipd72::noticeScope=previous;}
    // The caller removed the held occurrence before entering this one native
    // transfer. A false/partial result is terminal and is never replayed.
    return result;
}
static void PropRetireInstantD72(const StagedProp596& p) noexcept {
    if(!p.instantD72)return;
    auto token=p.instantTokenD72;
    if(!token||!g_instant_pickup_hold_d72.compare_exchange_strong(token,0,std::memory_order_acq_rel))return;
    // R155: Henry collides with the item again once it is clear of him (or after 4 s); a stowed item's body is gone
    // by then and the lease just ends (self_collision_native_r103.inl Tick -> RESTORE / RESTORE_SKIPPED)
    if(p.side>=0&&p.side<2)self_collision_r103n::Release(kcd2vr::self_collision_r103::instantOwner(p.side),std::uint64_t(GetTickCount64()));
    (void)kcd2vr::mutual_admission_t25::RetirePickupD72(p.side,p.instantTokenD72);
    ipd72::active.store(0,std::memory_order_release);
    kcd2vr::instant_d72::Publish(kcd2vr::instant_d72::Family::Interactions,kcd2vr::instant_d72::State::Native);
}
static bool ShutdownInstantPickupD72() noexcept {
    ipd72::stopping.store(true,std::memory_order_release);
    g_instant_pickup_ready_d72.store(false,std::memory_order_release);
    g_instant_pickup_live_d72.store(0,std::memory_order_release);
    EndInstantPickupRequestD72();
    if(ipd72::active.load(std::memory_order_acquire))return false; // retain the frame driver until it retires the real world object
    bool disabled=true;
    for(auto& h:g_instant_pickup_hooks_d72)if(h&&h->is_enabled()){
        const bool ok=bool(h->disable())&&!h->is_enabled();disabled=ok&&disabled;
    }
    if(!disabled)return false;
    for(auto& h:g_instant_pickup_hooks_d72)h.reset();
    ipd72::intercept.store(nullptr,std::memory_order_release);
    g_instant_pickup_handler_d72=nullptr;g_instant_pickup_factory_d72=nullptr;
    g_instant_pickup_choice_d72=nullptr;g_instant_pickup_execute_d72=nullptr;
    g_instant_pickup_destroy_d72=nullptr;
    return true;
}
