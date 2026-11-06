#pragma once
// D62C (Claude, 30 Oct 2026). Miki: "full verbatim combos with npc reacting to our blade and masterstrike without
// delayed npc reaction".
//
// OWNERSHIP (proven, see REPAIR_D62C.md / D61C): a masterstrike or combo finisher is a PAIR of Mannequin actions:
//   attacker  CombatMasterStrikeGen 0xBF91494F / ...DeathGen 0x17106D4F / CombatAttackComboGen 0xDEEB2887 / ...DeathGen 0x686FE953
//   victim    CombatMasterStrikeHitGen 0xD62EA2F9 / ...DeathGen 0xF3D43F01 / CombatHitComboGen 0xFB64521F / ...DeathGen 0x8F014920
// each on its own CActionController, queued through Queue (+684D8C) and living Queued -> Root (scope+148) -> Exiting
// (scope+150) -> gone. The clip inside each group is the ADB tag query (tools/ms_reaction_table_d61c.py, 185/185 unique).
//
// WHY THE NPC REACTS LATE: the native hit of a paired action is authored into the clip pair. Masterstrike master clips
// put their AnimationHit at 0.47-1.05 s (first) and up to 3.2 s (last, median 1.27 s); 42 of 90 have no AnimationHit
// at all and hit through the fragment's CombatAttack procedure. Both actors play a preliminary paired movement first.
// A native pair starting does not establish that the tracked VR blade met the
// victim. Contact timing and native pair identity are separate evidence.
//
// HISTORICAL D62C TIMING (quarantined in D63; see below): when Henry's paired attack is PLAYING, the pair's IAction speed bias
// (+5C, the same Mannequin field the pickup and alchemy owners already lease) is raised to `speed` on BOTH actions
// from the moment our blade touched that victim (or 200 ms after the attack started) until the native hit is APPLIED
// to that victim (D568B0 ok && apply), or 700 ms, or either action stops playing. Then both get their own speed back.
// Both run at the same rate, so the authored pair alignment is kept. Native selection, damage, stamina, perks, the
// reaction row, death and every refusal stay native; only WHEN the authored hit arrives changes.
// D63: the supplied run proves the presumed pair join was absent. The runtime
// now records finite diagnostic observations only and acquires no speed lease,
// even with the saved option ON. A future native pair join must be independently
// established before this pure timing policy may grant either actor a lease.
// INVESTIGATE_D67R1[COMBAT_NATIVE_PAIR]: the retained pure timing policy is not
// an installed paired-animation repair. Recover the real attacker/victim link
// and its lifetime from native pair construction/activation, then join the
// exact reaction fragment, hit event/procedure and applied-hit victim. Queue
// proximity/candidateD65 alone cannot authorize retiming, skipping a lead-in,
// replacing a reaction or attaching an impaled NPC to the tracked blade.
// D72 added a reciprocal native reader and a 64x admitted-pair policy, but its
// callback owner walk skipped the native completion wrapper. D84 reproduces
// that defect with actual retail constructor/clone bytes and fixes the reader.
// D84 deliberately removes the unsupported acceleration: a proved pair keeps
// native 1x progression while authored damage/contact ownership is unresolved.
// The ordinary physical final-combo request is separate and remains enabled.
#include "native_pair_d72.h"
#include "../../vrik/instant_activities_d72.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>
namespace kcd2vr::ms_reaction_d62c {
enum class Kind : unsigned char {None,Master,MasterDeath,Hit,HitDeath,ComboAttack,ComboAttackDeath,ComboHit,ComboHitDeath};
inline Kind kindOf(std::uint32_t c) noexcept {
    switch(c){case 0xBF91494Fu:return Kind::Master;case 0x17106D4Fu:return Kind::MasterDeath;
              case 0xD62EA2F9u:return Kind::Hit;case 0xF3D43F01u:return Kind::HitDeath;
              case 0xDEEB2887u:return Kind::ComboAttack;case 0x686FE953u:return Kind::ComboAttackDeath;
              case 0xFB64521Fu:return Kind::ComboHit;case 0x8F014920u:return Kind::ComboHitDeath;default:return Kind::None;}
}
inline constexpr std::uint32_t kCrcs[8]{0xBF91494Fu,0x17106D4Fu,0xD62EA2F9u,0xF3D43F01u,0xDEEB2887u,0x686FE953u,0xFB64521Fu,0x8F014920u};
inline bool isAttacker(Kind k) noexcept {return k==Kind::Master||k==Kind::MasterDeath||k==Kind::ComboAttack||k==Kind::ComboAttackDeath;}
inline bool isVictim(Kind k) noexcept {return k==Kind::Hit||k==Kind::HitDeath||k==Kind::ComboHit||k==Kind::ComboHitDeath;}
inline bool combo(Kind k) noexcept {return k==Kind::ComboAttack||k==Kind::ComboAttackDeath||k==Kind::ComboHit||k==Kind::ComboHitDeath;}
inline bool death(Kind k) noexcept {return k==Kind::MasterDeath||k==Kind::HitDeath||k==Kind::ComboAttackDeath||k==Kind::ComboHitDeath;}
inline bool pairs(Kind a,Kind v) noexcept {return isAttacker(a)&&isVictim(v)&&combo(a)==combo(v)&&death(a)==death(v);}
inline const char* name(Kind k) noexcept {
    switch(k){case Kind::Master:return "MASTER";case Kind::MasterDeath:return "MASTER_DEATH";case Kind::Hit:return "HIT";case Kind::HitDeath:return "HIT_DEATH";
              case Kind::ComboAttack:return "COMBO_ATTACK";case Kind::ComboAttackDeath:return "COMBO_ATTACK_DEATH";case Kind::ComboHit:return "COMBO_HIT";
              case Kind::ComboHitDeath:return "COMBO_HIT_DEATH";default:return "NONE";}
}
enum class Seen : int {Unknown=-1,Absent=0,Queued=1,Root=2,Exiting=3}; // = semi_smith46::native::Membership
enum class State : unsigned char {Empty,Queued,Playing,Exiting,Retired,Lost};
inline const char* name(State s) noexcept {
    switch(s){case State::Queued:return "QUEUED";case State::Playing:return "PLAYING";case State::Exiting:return "EXITING";
              case State::Retired:return "RETIRED";case State::Lost:return "LOST";default:return "EMPTY";}
}
enum class Phase : unsigned char {Idle,Accel,Done};
inline const char* name(Phase p) noexcept {return p==Phase::Accel?"ACCEL":p==Phase::Done?"DONE":"IDLE";}
constexpr std::uint64_t kPairMs=1500,kQueuedMaxMs=3000,kUnknownMaxMs=500,kLiveMaxMs=12000;
constexpr std::uint64_t kContactBeforeMs=400; // blade contact this long before the attacker was queued still counts
constexpr std::uint64_t kNoContactStartMs=200;// no contact seen: start anyway this long after the attacker plays
constexpr std::uint64_t kAccelMaxMs=700;      // never run faster longer than this
struct Owner {
    std::uint64_t serial=0,pair=0;std::uintptr_t controller=0,action=0;std::uint32_t entity=0,token=0,fragCrc=0;
    int fragId=-1,option=-1;Kind kind=Kind::None;State state=State::Empty;bool henry=false;
    std::array<unsigned char,16> tags{};
    std::uint64_t queued=0,playing=0,exiting=0,ended=0,unknownSince=0;
    // timing (attacker row holds the pair's phase)
    Phase phase=Phase::Idle;std::uint64_t accelStart=0,accelEnd=0,contactTick=0,hitTick=0;std::uint32_t victim=0;const char* endWhy="";
    // this row's own speed lease
    bool leased=false;float saved=1.f,ours=1.f;unsigned writes=0,restores=0,refused=0;
    // D72 policy is sampled for this admitted occurrence. A later preference
    // change affects the next pair; exact native teardown still retires this one.
    bool instantD72=false,nativeProofD72=false;float speedD72=64.f;
    kcd2vr::native_pair_d72::Pair nativeD72{};
    // Copied only while F3 captures the existing relationship walk.
    kcd2vr::native_pair_d72::ReadTraceD84 readD84{};
    // D65: diagnostic queue candidates only. Never a native pair/retiming grant.
    std::uint64_t candidateD65=0;unsigned candidatesD65=0;
    bool live() const noexcept {return state==State::Queued||state==State::Playing||state==State::Exiting;}
};
struct Edge {bool changed=false;State from=State::Empty,to=State::Empty;};
// Every update retains the same two admitted fragment occurrences. A current
// native pair for another victim (or a reused fragment address) cannot borrow
// this pair's speed policy or restoration state.
inline bool ownsPairD72(const Owner& a,const Owner& v,const native_pair_d72::Pair& p) noexcept {
    return a.live()&&v.live()&&a.henry&&!v.henry&&a.serial&&v.serial&&a.serial!=v.serial&&
        a.pair==v.serial&&v.pair==a.serial&&a.controller&&v.controller&&a.controller!=v.controller&&
        a.nativeProofD72&&v.nativeProofD72&&pairs(a.kind,v.kind)&&combo(a.kind)==p.combo&&
        a.action==p.attackFragment&&v.action==p.hitFragment&&a.entity==p.attackEntity&&v.entity==p.hitEntity&&
        a.victim==v.entity&&native_pair_d72::same(p,a.nativeD72)&&native_pair_d72::same(p,v.nativeD72);
}
template<unsigned N=12> struct Table {
    std::array<Owner,N> rows{};std::uint64_t next=1,dropped=0;
    Owner* offer(std::uintptr_t controller,std::uintptr_t action,std::uint32_t entity,std::uint32_t token,std::uint32_t crc,
                 int fragId,int option,const unsigned char* tags16,bool henry,std::uint64_t now) noexcept {
        const Kind k=kindOf(crc);if(k==Kind::None||!controller||!action)return nullptr;
        for(auto& r:rows)if(r.live()&&r.controller==controller&&r.action==action&&r.token==token)return &r;
        Owner* slot=nullptr;
        for(auto& r:rows)if(!r.live()&&!r.leased&&(!slot||r.serial<slot->serial))slot=&r;
        if(!slot){++dropped;return nullptr;}
        Owner o{};o.serial=next++;o.controller=controller;o.action=action;o.entity=entity;o.token=token;o.fragCrc=crc;o.fragId=fragId;
        o.option=option;o.kind=k;o.state=State::Queued;o.henry=henry;o.queued=now;if(tags16)std::memcpy(o.tags.data(),tags16,16);
        // D63: queue order and proximity do not identify an animation pair.
        // The supplied run queues the victim before Henry in both observed
        // Henry masterstrikes. The old one-way heuristic therefore paired
        // neither and still accelerated Henry alone. Retain separate observer
        // rows until a verified native attacker/victim relationship exists.
        *slot=o;refreshCandidatesD65();return slot;
    }
    // Either queue order is visible. Recompute the whole small observation
    // graph when a row arrives, so a late third candidate invalidates both
    // earlier endpoints. Queue time/type are still NOT native pair identity.
    void refreshCandidatesD65() noexcept {
        for(auto& r:rows){r.candidateD65=0;r.candidatesD65=0;}
        const auto compatible=[](const Owner& a,const Owner& b) noexcept {
            const auto dt=a.queued>=b.queued?a.queued-b.queued:b.queued-a.queued;
            return a.live()&&b.live()&&a.serial!=b.serial&&a.controller&&b.controller&&
                a.controller!=b.controller&&a.entity&&b.entity&&a.entity!=b.entity&&
                a.henry!=b.henry&&dt<=40&&
                (isAttacker(a.kind)?pairs(a.kind,b.kind):pairs(b.kind,a.kind));
        };
        for(auto& r:rows)for(const auto& other:rows)if(compatible(r,other))++r.candidatesD65;
        for(auto& r:rows)if(r.candidatesD65==1)for(const auto& other:rows)
            if(other.candidatesD65==1&&compatible(r,other)){r.candidateD65=other.serial;break;}
    }
    Edge observe(Owner& r,Seen seen,std::uint64_t now) noexcept {
        Edge e{};e.from=r.state;if(!r.live())return e;
        State to=r.state;
        if(seen==Seen::Unknown){if(!r.unknownSince)r.unknownSince=now;if(now-r.unknownSince>=kUnknownMaxMs)to=State::Lost;}
        else{
            r.unknownSince=0;
            if(seen==Seen::Queued)to=r.state==State::Queued?State::Queued:State::Lost;
            else if(seen==Seen::Root)to=r.state==State::Exiting?State::Lost:State::Playing;
            else if(seen==Seen::Exiting)to=r.state==State::Queued?State::Lost:State::Exiting;
            else to=r.state==State::Queued?State::Lost:State::Retired;
        }
        if(to==State::Queued&&now-r.queued>kQueuedMaxMs)to=State::Lost;
        if((to==State::Playing||to==State::Exiting)&&now-r.queued>kLiveMaxMs)to=State::Lost;
        if(to!=r.state){
            if(to==State::Playing&&!r.playing)r.playing=now;
            if(to==State::Exiting&&!r.exiting)r.exiting=now;
            if(to==State::Retired||to==State::Lost)r.ended=now;
            if(to==State::Retired)r.leased=false; // gone from every container: nothing of ours left to restore (LOST keeps the lease for the cleanup walk)
            r.state=to;e.changed=true;refreshCandidatesD65();
        }
        e.to=r.state;return e;
    }
    Owner* find(std::uint64_t serial) noexcept {for(auto& r:rows)if(serial&&r.serial==serial)return &r;return nullptr;}
    const Owner* find(std::uint64_t serial) const noexcept {for(auto& r:rows)if(serial&&r.serial==serial)return &r;return nullptr;}
    const Owner* liveReaction(std::uint32_t entity) const noexcept {
        const Owner* best=nullptr;for(auto& r:rows)if(isVictim(r.kind)&&r.live()&&r.entity==entity&&(!best||r.serial>best->serial))best=&r;return best;
    }
    bool anyLive() const noexcept {for(auto& r:rows)if(r.live()||r.leased)return true;return false;}
    bool henryAttackLive() const noexcept {for(auto& r:rows)if(r.henry&&isAttacker(r.kind)&&r.live())return true;return false;}
    bool tracks(std::uintptr_t controller) const noexcept {for(auto& r:rows)if((r.live()||r.leased)&&r.controller==controller)return true;return false;}
    // the attacker row that owns this row's timing (itself, or its pair)
    Owner* timingOwner(Owner& r) noexcept {if(isAttacker(r.kind))return &r;return find(r.pair);}
};
// Pair phase step, on the attacker row. contact = newest VR blade contact tick on the victim (0 none),
// applied = newest applied native hit tick on the victim (0 none). Returns true on a phase change.
inline bool step(Owner& a,const Owner* victimRow,bool enabled,std::uint64_t contact,std::uint64_t applied,std::uint64_t now,
                 bool nativePairVerified=false) noexcept {
    if(!isAttacker(a.kind)||!a.henry)return false;
    const Phase before=a.phase;
    const bool aPlaying=a.state==State::Playing;
    // A missing, merely queued or unrelated victim cannot lend timing authority.
    // nativePairVerified must come from an independently verified native join;
    // queue proximity and a blade contact are explicitly insufficient evidence.
    const bool pairOk=nativePairVerified&&victimRow&&a.serial&&victimRow->serial&&
        a.pair==victimRow->serial&&victimRow->pair==a.serial&&pairs(a.kind,victimRow->kind)&&
        a.controller&&victimRow->controller&&a.controller!=victimRow->controller&&
        a.entity&&victimRow->entity&&a.entity!=victimRow->entity&&a.victim==victimRow->entity&&
        !victimRow->henry;
    const bool vOk=pairOk&&victimRow->state==State::Playing;
    if(!pairOk){
        if(a.phase==Phase::Accel||(enabled&&aPlaying)){
            a.phase=Phase::Done;a.accelEnd=now;a.endWhy="PAIR_NATIVE_JOIN_UNVERIFIED";
        }
        return a.phase!=before;
    }
    if(a.phase==Phase::Idle){
        if(!enabled||!aPlaying){if(!a.live())a.phase=Phase::Done;}
        else{
            if(contact&&contact+kContactBeforeMs>=a.queued)a.contactTick=contact;
            if(vOk&&(a.contactTick||now-a.playing>=kNoContactStartMs)){a.phase=Phase::Accel;a.accelStart=now;}
        }
    }else if(a.phase==Phase::Accel){
        const char* why=nullptr;
        if(!enabled)why="SWITCHED_OFF";
        else if(applied&&applied>=a.accelStart){why="NATIVE_HIT_APPLIED";a.hitTick=applied;}
        else if(!aPlaying)why="ATTACKER_NOT_PLAYING";
        else if(!vOk)why="VICTIM_NOT_PLAYING";
        else if(now-a.accelStart>=kAccelMaxMs)why="TIME_CAP";
        if(why){a.phase=Phase::Done;a.accelEnd=now;a.endWhy=why;}
    }
    return a.phase!=before;
}
// What speed this row wants right now: >0 = lease to that, 0 = give the native speed back / do nothing.
inline float wanted(const Owner& r,const Owner* timing,float speed) noexcept {
    if(!timing||timing->phase!=Phase::Accel||r.state!=State::Playing)return 0.f;
    if(!(speed>1.f))return 0.f;
    return std::min(speed,8.f);
}
// One lease decision for this row given the CURRENT native speed read from the action. Returns the speed to write
// (NaN = no write). Never takes over a speed someone else changed; never restores over a foreign value.
inline float lease(Owner& r,float current,float want,float ceiling=8.f) noexcept {
    const float none=std::nanf("");
    if(!std::isfinite(current)||current<0.f||current>ceiling){if(r.leased){r.leased=false;++r.refused;}return none;}
    if(want>0.f){
        if(r.leased){
            if(current==r.ours)return none;                       // still ours, nothing to do
            r.leased=false;++r.refused;return none;                // someone else wrote it: stand down for good this phase
        }
        if(r.writes)return none;                                   // one lease per row per pair, never re-take
        if(current<=0.f)return none;                               // a stopped/held action belongs to another owner
        r.saved=current;r.ours=std::min(current*want,ceiling);r.leased=true;++r.writes;return r.ours;
    }
    if(!r.leased)return none;
    r.leased=false;
    if(current!=r.ours){++r.refused;return none;}                  // foreign value now: never overwrite it
    ++r.restores;return r.saved;
}
// D84: the fixed reciprocal reader proves WHICH native pair is current. It
// does not prove WHEN its authored damage event corresponds to tracked blade
// contact. The old 64x path was dormant behind a broken callback-owner walk;
// enabling it on identity alone would advance native air hits. Keep ordinary
// native 1x progression until a real event/contact ownership bridge exists.
// No elapsed-time fallback, entity/tick proximity or menu switch grants that
// missing authority. An existing lease is retired through the same cleanup.
inline bool stepInstantD72(Owner& a,const Owner* v,bool proof,bool bothRoots,std::uint64_t now) noexcept {
 const auto before=a.phase;
 if(!a.instantD72||!a.henry||!isAttacker(a.kind))return false;
 if(a.phase==Phase::Idle){
  if(proof&&bothRoots&&v&&a.pair==v->serial&&v->pair==a.serial&&pairs(a.kind,v->kind)){
   a.phase=Phase::Done;a.endWhy="PAIR_DAMAGE_CONTACT_UNPROVEN";
  }
 }else if(a.phase==Phase::Accel){
  a.phase=Phase::Done;a.accelEnd=now;
  a.endWhy=!proof?"NATIVE_PAIR_RETIRED":!bothRoots?"NATIVE_ROOT_EXIT":"PAIR_DAMAGE_CONTACT_UNPROVEN";
 }
 return before!=a.phase;
}
inline constexpr float instantSpeedD72=64.f;
} // namespace kcd2vr::ms_reaction_d62c
