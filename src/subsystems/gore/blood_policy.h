#pragma once
// C49 BLOODY FIGHTS (Miki 18 Oct: "fights need to be exceptionally bloody - 2x more blood and gushing blood on kill moves").
// What the game does on its own for a wounding melee hit (WHGame 6A350E20 + Tables):
//  - the combat dispatcher +B24A0C plays MaterialEffects row x surface (hit_passed_<weapon>_<slash|stab>_player x flesh 116)
//    = the blood particle/decal at the contact (contact_fx178r12.inl, proven in the F3 material logs).
//  - the RPG moves 0.3..0.5 "blood" onto the struck zone of the victim's clothes/skin (rpg_param TransferedBloodToVictimMin/Max),
//    the zones of BloodMask.xml (head, torso, arm_left, arm_right, leg_left, leg_right) - the same zones the game's own Lua
//    sets through entity.actor:AddBlood(value, zone) (DeadBody_Base_Human.lua ApplyDirtAndBlood).
// BLOODY FIGHTS doubles that, only for a hit HENRY landed on an NPC (our own contact receipt, never a hit on Henry, never
// npc vs npc), once per applied native hit: the same MaterialEffects row once more at the contact + the same native blood
// amount once more on the struck zone (Lua AddBlood through the C47 ExecuteBuffer route). Kill moves (impale, decapitation,
// killing hits) may add a4s gush only while a current named neck/original struck-part wound anchor is proven.
// Ordinary lethal hits without that anchor retain their exact immediate native contact blood; no root/world fountain.
// The game's rules are untouched: damage, death, bleeding and the native blood all stay the game's own.
// Pure logic (host tested). The native calls live in vrik/blood49.inl.
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
#include "../../core/snapshot_mailbox.h"
#include "../../vrik/contact_receipt58c1.h"

namespace kcd2vr::blood49 {
inline std::atomic<bool> on{true}; // VrBloodyFights (IMMERSION page BLOODY FIGHTS), default ON (Miki asked for it)

// ---------------------------------------------------------------- one applied hit by Henry
enum class Skip : int {None=0,Off,NoReceipt,NotApplied,NotBody,Defended,NoDamage,BadPoint};
inline const char* name(Skip s) noexcept {
    constexpr const char* n[]{"OK","OFF","NO_RECEIPT","NOT_APPLIED","NOT_BODY","BLOCKED_OR_PARRIED","NO_DAMAGE","BAD_POINT"};
    return n[unsigned(s)<8?unsigned(s):0];}
// a wounding hit: the game applied it on his body, undefended, with damage (the native blood transfer is on injuries)
inline Skip qualify(const contact58c1::Receipt& r,bool enabled) noexcept {
    if(!enabled)return Skip::Off;
    if(!r.serial||!r.target||!r.targetIdentity)return Skip::NoReceipt;
    if(!r.seen||!r.accepted||!r.applied||!r.fields)return Skip::NotApplied;
    if(!r.bodyContact)return Skip::NotBody;
    if(r.blocked!=0||r.perfect!=0||r.parry!=0)return Skip::Defended;
    if(!std::isfinite(r.damage)||r.damage<=0)return Skip::NoDamage;
    if(!std::isfinite(r.point[0])||!std::isfinite(r.point[1])||!std::isfinite(r.point[2]))return Skip::BadPoint;
    return Skip::None;
}
struct Hit {std::uint64_t serial=0,tick=0;std::uint32_t target=0;std::uintptr_t identity=0;float point[3]{};
    int kind=-1,weaponClass=-1,bodyMaterial=-1,part=-1,death=0;float damage=0,energy=-1;};
inline Hit fromReceipt(const contact58c1::Receipt& r) noexcept {
    Hit h{};h.serial=r.serial;h.tick=r.tick;h.target=r.target;h.identity=r.targetIdentity;for(int i=0;i<3;++i)h.point[i]=r.point[i];
    h.kind=r.kind;h.weaponClass=r.weaponClass;h.bodyMaterial=r.bodyMaterial;h.part=r.nativePart>=0?r.nativePart:r.rawPart;
    h.death=r.death==1?1:0;h.damage=r.damage;h.energy=r.energy;return h;}
// combat TU (receipt publisher) -> native frame (frik TU). Only qualified hits are published.
inline core::SnapshotMailbox<Hit> hits;
inline bool offer(const contact58c1::Receipt& r) noexcept {
    if(qualify(r,on.load(std::memory_order_relaxed))!=Skip::None)return false;return hits.publish(fromReceipt(r));}

// once per applied hit: the mailbox is read every native frame, a serial is acted on once
template<unsigned N=16> struct Once {
    std::uint64_t seen[N]{};unsigned next=0;
    bool take(std::uint64_t serial) noexcept {
        if(!serial)return false;for(auto s:seen)if(s==serial)return false;seen[next++%N]=serial;return true;}
};

// the game's blood row for Henry's weapon (material_effect_chains rows, the same as contact_fx178r12 "passed" rows).
// weapon_class.xml: 0 dagger, 1 sword, 2 sabre, 4 longsword, 16 hunting sword, 3 axe, 5 mace, 6 flail, 7 halberd.
// anything else (fists, bows, unknown) = no blood row: no extra blood.
inline const char* row(int weaponClass,bool thrust) noexcept {
    switch(weaponClass){
    case 0:case 1:case 2:case 4:case 16:return thrust?"hit_passed_sword_stab_player":"hit_passed_sword_slash_player";
    case 3:return "hit_passed_axe_player";
    case 5:case 6:return "hit_passed_mace_player";
    case 7:return "hit_passed_halberd_player";
    default:return nullptr;}
}
// struck human body material (body_material2subpart.xml race 0) -> BloodMask zone (BloodMask.xml ZoneIdx 0..5)
inline const char* zone(int m) noexcept {
    if((m>=1&&m<=5)||m==38)return "head";
    if(m>=6&&m<=11)return "torso";
    if(m>=12&&m<=18)return "arm_right";
    if(m>=19&&m<=25)return "arm_left";
    if(m>=26&&m<=31)return "leg_right";
    if(m>=32&&m<=37)return "leg_left";
    return nullptr;
}
// the native transfer per wounding hit is 0.3..0.5 (rpg_param): the same once more, more for a stronger stroke
inline float extraAmount(float energy) noexcept {
    const float e=std::isfinite(energy)&&energy>=0?std::min(energy,1.f):.5f;return .3f+.2f*e;}

// ---------------------------------------------------------------- kill-move gush
enum class Kill : int {None=0,Lethal=1,Impale=2,Decap=3};
inline const char* name(Kill k) noexcept {constexpr const char* n[]{"NONE","LETHAL","IMPALE","DECAP"};return n[unsigned(k)<4?unsigned(k):0];}
// impale latch / decapitation / killing receipt, all raised on the native game thread (impale tick, gore32n::Tick, blood49n::Tick)
// in the same frame possibly: a small FIFO (native game thread only, no other thread touches it), drained by blood49n::Tick.
struct KillEvent {std::uint64_t tick=0;std::uint32_t target=0;std::uintptr_t identity=0;Kill kill=Kill::None;float point[3]{},dir[3]{0,0,1};int bodyMaterial=-1;};
template<unsigned N=8> struct KillQueue {
    KillEvent q[N]{};unsigned head=0,count=0;unsigned dropped=0;
    bool push(const KillEvent& e) noexcept {if(count==N){++dropped;return false;}q[(head+count++)%N]=e;return true;}
    bool pop(KillEvent& e) noexcept {if(!count)return false;e=q[head];head=(head+1)%N;--count;return true;}
};
inline KillQueue<8> kills;
inline bool makeKill(Kill k,std::uint32_t target,std::uintptr_t identity,const double p[3],const double d[3],int bodyMaterial,std::uint64_t now,KillEvent& e) noexcept {
    if(k==Kill::None||!target||!identity)return false;
    e={};e.tick=now;e.target=target;e.identity=identity;e.kill=k;e.bodyMaterial=bodyMaterial;
    for(int i=0;i<3;++i){e.point[i]=float(p[i]);e.dir[i]=float(d[i]);if(!std::isfinite(e.point[i])||!std::isfinite(e.dir[i]))return false;}
    return true;}
inline bool offerKill(Kill k,std::uint32_t target,std::uintptr_t identity,const double p[3],const double d[3],int bodyMaterial,std::uint64_t now) noexcept {
    KillEvent e{};return on.load(std::memory_order_relaxed)&&makeKill(k,target,identity,p,d,bodyMaterial,now,e)&&kills.push(e);}

// C55 cosmetic wound publication from the ALREADY qualified exact struck
// collider/raw-part anchor in ImpalePartTick. No nearest-part/body replacement.
struct PartWound55 {std::uint64_t serial=0,tick=0,epoch=0,request=0;std::uint32_t target=0;std::uintptr_t identity=0;
    double point[3]{},dir[3]{0,0,1};};
inline core::SnapshotMailbox<PartWound55> partWound55;
inline bool freshPart55(const PartWound55& p,std::uint32_t target,std::uintptr_t identity,std::uint64_t serial,
    std::uint64_t now,std::uint64_t epoch,std::uint64_t request) noexcept {
    if(!p.serial||(serial&&serial!=p.serial)||p.target!=target||p.identity!=identity||!target||!identity||
       !epoch||!request||p.epoch!=epoch||p.request!=request||!p.tick||now<p.tick||now-p.tick>80)return false;
    for(int i=0;i<3;++i)if(!std::isfinite(p.point[i])||!std::isfinite(p.dir[i]))return false;
    return true;
}
// 4 pulses over 4 s, each a bit weaker (game blood row strength)
inline constexpr std::uint64_t kPulseMs[]{0,1000,2100,3300};
inline constexpr float kPulseStrength[]{1.6f,1.4f,1.2f,1.0f};
inline constexpr int kPulses=4;
inline constexpr std::uint64_t kGushMs=4000;
struct Gush {
    bool active=false;std::uint32_t target=0;std::uintptr_t identity=0;Kill kill=Kill::None;std::uint64_t start=0;int pulses=0,okPulses=0;
    double local[3]{},dir[3]{0,0,1};bool modelFrame=false;int bodyMaterial=-1;
    std::uintptr_t character55=0,skeleton55=0;int neckJoint55=-1;std::uint64_t anchorSerial55=0; // legacy scheduler payload retained for fixtures; current production requires anchor55 below
};
enum class Start : int {Started=0,Upgraded=1,Duplicate=2,Refused=3};
inline const char* name(Start s) noexcept {constexpr const char* n[]{"STARTED","UPGRADED","DUPLICATE","REFUSED"};return n[unsigned(s)<4?unsigned(s):3];}
template<unsigned N=3> struct Gushes {
    Gush slot[N]{};
    Gush* find(std::uint32_t target,std::uintptr_t identity) noexcept {
        for(auto& g:slot)if(g.active&&g.target==target&&g.identity==identity)return &g;return nullptr;}
    // a kill on the same body: a stronger kill move (DECAP > IMPALE > LETHAL) moves the wound and restarts the 4 s,
    // the same or a weaker one is a duplicate (the lethal receipt usually comes first, the impale latch / head later)
    Start start(const Gush& in,std::uint64_t now,Gush*& out) noexcept {
        out=nullptr;if(in.kill==Kill::None||!in.target||!in.identity)return Start::Refused;
        if(Gush* g=find(in.target,in.identity)){out=g;if(int(in.kill)<=int(g->kill))return Start::Duplicate;
            *g=in;g->active=true;g->start=now;g->pulses=g->okPulses=0;return Start::Upgraded;}
        Gush* free=nullptr;for(auto& g:slot)if(!g.active){free=&g;break;}
        if(!free){free=&slot[0];for(auto& g:slot)if(g.start<free->start)free=&g;} // all busy: the oldest gives way
        *free=in;free->active=true;free->start=now;free->pulses=free->okPulses=0;out=free;return Start::Started;}
    static bool due(const Gush& g,std::uint64_t now) noexcept {return g.active&&g.pulses<kPulses&&now>=g.start+kPulseMs[g.pulses];}
    static bool done(const Gush& g,std::uint64_t now) noexcept {return g.active&&g.pulses>=kPulses&&now>=g.start+kGushMs;}
    bool any() const noexcept {for(auto& g:slot)if(g.active)return true;return false;}
};
}
