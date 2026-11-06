#pragma once
#include <array>
#include <atomic>
#include <cstdint>
namespace kcd2vr::bridges168 {
struct Ticket {
    std::uint64_t serial=0,tick=0,epoch=0,policy=0,key=0,generation=0,request=0,sample=0;
    std::uintptr_t player=0,actor=0,state=0,opponent=0;
    int origin=-1;bool thrust=false;
};
inline bool zone(int z) noexcept {return z==0||z==1||z==2||z==5;}
inline bool opposite(int a,int b) noexcept {
    return (a==0&&b==5)||(a==5&&b==0)||(a==1&&b==2)||(a==2&&b==1);
}
enum class Denial : unsigned {None,NoThrust,Expired,Identity,Tracking,OpponentNotAttacking,UnknownZone,NotOpposite,AlreadyConsumed,UnarmedNative,NativeBackupD51};
inline const char* name(Denial r) noexcept {
    constexpr const char* names[]{"NONE","NO_THRUST","EXPIRED","IDENTITY","TRACKING","OPPONENT_NOT_ATTACKING","UNKNOWN_ZONE","NOT_OPPOSITE","ALREADY_CONSUMED","UNARMED_NATIVE","NATIVE_PAIRED_BACKUP_D51"};
    return names[unsigned(r)<11?unsigned(r):3];
}
struct Live {
    std::uint64_t now=0,epoch=0,policy=0,key=0,generation=0,request=0;
    std::uintptr_t actor=0,state=0,opponent=0;
    int enemyZone=-1;bool tracked=false,enemyAttacking=false;
};
inline Denial qualify(const Ticket& t,const Live& l,std::uint64_t used) noexcept {
    if(!t.serial||!t.thrust)return Denial::NoThrust;
    if(!t.tick||l.now<t.tick||l.now-t.tick>150)return Denial::Expired;
    if(!t.actor||!t.opponent||!t.key||t.actor!=l.actor||t.state!=l.state||t.opponent!=l.opponent||
       t.epoch!=l.epoch||t.policy!=l.policy||t.key!=l.key||t.generation!=l.generation||t.request!=l.request)return Denial::Identity;
    if(!l.tracked)return Denial::Tracking;
    if(!l.enemyAttacking)return Denial::OpponentNotAttacking;
    if(!zone(t.origin)||!zone(l.enemyZone))return Denial::UnknownZone;
    if(!opposite(t.origin,l.enemyZone))return Denial::NotOpposite;
    if(t.serial==used)return Denial::AlreadyConsumed;
    return Denial::None;
}
// Bounded F4 ledger, copied counters only; factory entry is not final native acceptance.
inline std::atomic<std::uint64_t> factoryQueries49{0},factoryGroupTrue49{0},factoryPermitted49{0};
inline std::atomic<unsigned> factoryDenial49{0};
// Original R3 input refusals, recorded under existing F3 capture. These name
// the first failed operation; none grants native admission or changes a call.
enum class KickGateH31 : unsigned {None=0,LegacyGate=1,PressFailed=2,ReleaseFailed=3,
 Mode,IngressDisabled,RequestExpired,Retail,InputHook,Gameplay,Reference,NativeBody,
 Tracking,OwnerResolve,ActorChanged,StateChanged,Observe,NativeMetadata,Loadout,
 AfterCancelOwner,NativeRequestPending,WeaponPrepared,ManualGuard,GuardWaitOwner,
 GuardNotQuiet,GuardAckUnknown,InputNotWritable,ZoneCall,ZoneOwner,GuardReasserted,
 ZoneRead,ZoneMismatch,FinalIngress,FinalManualGuard,FinalRequest,
 BeginNotObserved,ReleaseNotObserved,FreeGuardPendingD84,FreeGuardExpiredD84,
 CooldownD85};
inline const char* kickGateNameH31(unsigned reason) noexcept {
 constexpr const char* names[]{"NONE","LEGACY_GATE","PRESS_HANDLER_FAILED","RELEASE_HANDLER_FAILED",
 "MODE_ROUTE","INGRESS_DISABLED","ORIGINAL_REQUEST_EXPIRED_OR_CONTEXT_CHANGED","EXACT_RETAIL_NOT_READY","INPUT_HOOK_NOT_OWNED","GAMEPLAY_DISALLOWED","NATIVE_REFERENCE","PROTECTED_NATIVE_BODY",
 "FRESH_HEAD_TRACKING","NATIVE_OWNER_RESOLVE","ACTOR_REPLACED","STATE_REPLACED","NATIVE_OBSERVE","NATIVE_METADATA_NOT_WRITABLE","LOADOUT_WITHOUT_SELECTOR",
 "POST_CANCEL_OWNER_OBSERVATION","NATIVE_REQUEST_ALREADY_PENDING","WEAPON_PREPARE_STILL_OWNS_INPUT","MANUAL_GUARD","GUARD_WAIT_OWNER_CHANGED",
 "GUARD_WAIT_NOT_QUIET","GUARD_RELEASE_ACK_UNKNOWN","INPUT_NOT_WRITABLE","NATIVE_ZONE_SET_FAILED","POST_ZONE_OWNER_CHANGED","GUARD_REASSERTED",
 "DESIRED_ZONE_READ_FAILED","DESIRED_ZONE_NOT_5","FINAL_INGRESS_DISABLED","FINAL_MANUAL_GUARD","FINAL_REQUEST_EXPIRED_OR_CONTEXT_CHANGED",
 "HEAVY_BEGIN_BOUNDARY_NOT_OBSERVED","HEAVY_RELEASE_BOUNDARY_NOT_OBSERVED",
 "WAITING_FOR_NATIVE_FREE_GUARD","NATIVE_FREE_GUARD_NOT_ACKNOWLEDGED",
 "COOLDOWN_2S_D85"};
 return reason<std::size(names)?names[reason]:"UNKNOWN_GATE";
}
// D72: immutable item identity for native free/combat kick rows.
// Modifier cleanup remains actor-owned when equipment changes; only continued
// authorization compares this snapshot. Thus a new torch cannot inherit a kick.
struct FreeKickLoadoutD72 {
    std::uintptr_t manager=0;
    std::uint64_t right=0,left=0,generation=0,weaponRequest=0;
    std::uint64_t rightGeneration=0;
    int rightClass=12,leftClass=12;
};
inline bool sameFreeKickLoadoutD72(const FreeKickLoadoutD72& a,const FreeKickLoadoutD72& b) noexcept {
    return a.manager&&a.manager==b.manager&&a.right==b.right&&a.left==b.left&&
        a.generation==b.generation&&a.rightGeneration==b.rightGeneration&&a.weaponRequest==b.weaponRequest&&
        a.rightClass==b.rightClass&&a.leftClass==b.leftClass;
}
inline bool dedicatedFreeKickD72(const FreeKickLoadoutD72& x) noexcept {
    return x.rightClass!=12||(x.leftClass!=12&&x.leftClass!=11);
}
// R94: melee loadouts covered by the companion's input-class-2 COMBAT kick rows
// (tools/generate_combat_kicks_r94.py, retail combat partitions): one-hand swords /
// sabres / longsword one-handed / class 16 (group 10, left group 2 = empty, dagger,
// longsword, torch), blunt axe/mace/class 6 (group 8, left group 2), the same with a
// shield (8; 17 rows already existed), two-hand longsword 4/4 and polearm 7/7.
// Retail action-3 rows never use input class 2, so only our R3 SPECIAL selects them.
inline bool meleeCombatKickR94(const FreeKickLoadoutD72& x) noexcept {
    const auto oneHand=[](int c){return c==1||c==2||c==3||c==4||c==5||c==6||c==16;};
    const auto left=[](int c){return c==12||c==0||c==4||c==11||c==8||c==17;};
    return (oneHand(x.rightClass)&&left(x.leftClass))||(x.rightClass==7&&x.leftClass==7);
}
// R96: Experiments > HANDS page 2 "Weapon kicks vs enemies" (VrWeaponKickR96, default ON). OFF = the R95 route:
// with a melee weapon and an enemy, R3 sends the HEAVY zone-5 press again (the game then plays its zone-5 attack).
inline std::atomic<bool> weaponKickR96{true};
inline bool dedicatedCombatKickD72(const FreeKickLoadoutD72& x) noexcept {
    const auto ranged=[](int c){return c==9||c==10||c==13||c==14||c==15;};
    return ranged(x.rightClass)||ranged(x.leftClass)||x.leftClass==17||
        (x.rightClass==12&&(x.leftClass==0||x.leftClass==8))||
        (weaponKickR96.load(std::memory_order_relaxed)&&meleeCombatKickR94(x)); // R94/R96 melee
}
struct Event {
    std::uint64_t serial=0,tick=0,sample=0,key=0;
    std::uintptr_t actor=0,opponent=0;
    unsigned kind=0,reason=0;int origin=-1,enemyZone=-1,type=-1,input=-1;
    bool nativeResult=false,allowed=false;
    int beforePhaseH31=-1,beforeTypeH31=-1;unsigned inputWitnessH31=0;
    int freeGuardD84=-1,modifierD84=-1;std::uint64_t requestedTickD84=0;
    // D85 kick (F3 only): no-target facts. combat object = actor+450 (its
    // vtable and slot 0x18 decide 1315CF8 = CombatAttack without opponent).
    int opponentD85=-1,routeD85=-1;std::uint64_t combatObjVtD85=0,combatObjVf18D85=0,cooldownLeftD85=0;bool combatObjD85=false;
    int hold=-1;unsigned msPress=0,msHold=0,msRelease=0; // V66.178R18 (review F06): R3 direct hold result + per-call time
};
struct Queue {
    std::array<Event,128> events{};std::atomic_flag lock=ATOMIC_FLAG_INIT;
    unsigned head=0,tail=0;std::atomic<std::uint64_t> dropped{0};
    bool push(Event e) noexcept {
        if(lock.test_and_set(std::memory_order_acquire)){++dropped;return false;}
        const auto next=(head+1)%events.size();const bool ok=next!=tail;
        if(ok){events[head]=e;head=unsigned(next);}else ++dropped;
        lock.clear(std::memory_order_release);return ok;
    }
    bool pop(Event& e) noexcept {
        if(lock.test_and_set(std::memory_order_acquire))return false;
        const bool ok=tail!=head;if(ok){e=events[tail];tail=(tail+1)%events.size();}
        lock.clear(std::memory_order_release);return ok;
    }
};
struct SpecialRequest {
    std::uint64_t serial=0,tick=0,context=0,epoch=0;
    std::uintptr_t player=0,actor=0,state=0;
};
inline bool fresh(const SpecialRequest& r,std::uint64_t now,std::uint64_t context,std::uint64_t epoch) noexcept {
    return r.serial&&r.player&&r.actor&&r.state&&r.tick&&now>=r.tick&&now-r.tick<=120&&r.context==context&&r.epoch==epoch;
}
// R99 kick: the free kick waits for the game's own FreeGuard 23 after our one
// modifier press. 120 ms is about 7 frames; a slow native guard frame made the
// press die. Same identity rules, 300 ms for that one wait only.
inline constexpr std::uint64_t kFreeGuardWaitMsR99=300;
inline bool freshGuardWaitR99(const SpecialRequest& r,std::uint64_t now,std::uint64_t context,std::uint64_t epoch) noexcept {
    return r.serial&&r.player&&r.actor&&r.state&&r.tick&&now>=r.tick&&now-r.tick<=kFreeGuardWaitMsR99&&r.context==context&&r.epoch==epoch;
}
}
// D85 kick: route choice, 2 s cooldown and reason line (uses KickGateH31).
#include "kick_route_d85.h"
