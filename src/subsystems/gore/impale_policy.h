#pragma once
// C40 IMPALE BY ARMOUR (Miki 17 Oct): "the sword goes through the enemy according to the energy and their armour -
// through with light armour and high energy, or only ~30 cm deep with hard armour or lower energy".
// Before C40 an impale needed a native LETHAL thrust; the C37C2 log had 10 thrusts, none lethal (FINISHER_RECEIPT
// 61 NOT_THRUST, 8 NOT_LETHAL, 2 BLOCKED_OR_DEFENCE_UNKNOWN), so it never latched.
// C40 (VrImpaleByArmour, default OFF): any native thrust the game let through (body contact, not blocked/parried,
// native health damage > 0) is a candidate. How deep the blade sinks = our model from the stroke energy and the
// NATIVE armour stab defence of the struck zone (the game's own per-zone armour sum, read once per candidate).
//  - living victim: STUCK - the blade stays in the wound (display only, the NPC is never driven: he stays native);
//    pull back out of the wound, kick (R3), push with the free palm, or he tears free.
//  - dead victim (native death on this or a later receipt): HANG - the existing C32 corpse hang on the blade.
// Pure logic, host tested. Native sides: diagnostics/impale_armour40.inl (armour read) and vrik/impale_part59c7g.inl.
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
#include "../../vrik/contact_receipt58c1.h"
#include "../../core/snapshot_mailbox.h"

namespace kcd2vr::impale40 {
// ---------------------------------------------------------------- settings (menu COMBAT TUNING 2 + INI)
inline std::atomic<int> mode{0};                 // VrImpaleByArmour: optional armour-depth experiment OFF; existing lethal-only path unchanged
inline std::atomic<float> lightArmour{15.f};     // VrImpaleLightArmour: zone stab defence at/below = light (through possible)
inline std::atomic<float> hardArmour{60.f};      // VrImpaleHardArmour: zone stab defence at/above = hard
inline std::atomic<float> minEnergy{.30f};       // VrImpaleMinEnergy: stroke energy needed against no armour (hard armour +0.35)
inline std::atomic<float> throughEnergy{.75f};   // VrImpaleThroughEnergy: energy for a full pass-through (light armour only)
inline std::atomic<float> partialCm{30.f};       // VrImpalePartialCm: deepest partial penetration
inline std::atomic<float> throughCm{55.f};       // VrImpaleThroughCm: depth of a pass-through (also capped by the blade)
inline bool enabled() noexcept {return mode.load(std::memory_order_relaxed)>0;}

// ---------------------------------------------------------------- native armour zones
// Retail flowgraph armour node (GetConfiguration 2AA33A4, Process 2AAEAAC): three float[4] per damage type, index
// = the armour layer's zone field (+0x24 of the record the lambda 2AB06AC receives): 0 head, 1 hands/arms, 2 body,
// 3 legs (port names headStab / handsStab / bodyStab / legsStab -> output ids 0x101/0x107/0x104/0x10A).
enum class Zone : int {Unknown=-1,Head=0,Arms=1,Body=2,Legs=3};
inline const char* name(Zone z) noexcept {
    switch(z){case Zone::Head:return "HEAD";case Zone::Arms:return "ARMS";case Zone::Body:return "BODY";case Zone::Legs:return "LEGS";default:return "UNKNOWN";}}
// Struck zone from the ORIGINAL body material of the contact (human race body_material2subpart.xml, the same table
// as bodycombo59c7g::humanMaterial): 1..5 head, 38 neck, 6..11 torso, 12..25 arms, 26..37 legs.
inline Zone zoneFromMaterial(int m) noexcept {
    if((m>=1&&m<=5)||m==38)return Zone::Head;
    if(m>=6&&m<=11)return Zone::Body;
    if(m>=12&&m<=25)return Zone::Arms;
    if(m>=26&&m<=37)return Zone::Legs;
    return Zone::Unknown;
}

// ---------------------------------------------------------------- the per-candidate native verdict (combat TU -> game thread)
enum class ArmourStatus : int {NotRead=0,Read,Disabled,PinMismatch,NoActor,NoSoul,NoEquipment,Fault,Invalid};
inline const char* name(ArmourStatus s) noexcept {
    constexpr const char* n[]{"NOT_READ","READ","OFF","PIN_MISMATCH","NO_ACTOR","NO_SOUL","NO_EQUIPMENT","FAULT","INVALID"};
    const auto i=unsigned(s);return i<9?n[i]:"?";}
struct Verdict {
    std::uint64_t serial=0,tick=0;std::uint32_t target=0;std::uintptr_t identity=0;
    float energy=0,damage=0;int death=-1,kind=-1,material=-1;ArmourStatus armour=ArmourStatus::NotRead;
    float stab[4]{},slash[4]{},smash[4]{};unsigned items=0;
};
inline core::SnapshotMailbox<Verdict> verdict;

// ---------------------------------------------------------------- eligibility (native outcome stays native)
// Same identity/freshness contract as contact58c1::lethalThrust, minus the death requirement. The native game
// still decides the hit: blocked/parried or zero health damage never impales.
inline bool candidateThrust(const contact58c1::Receipt& r,std::uint64_t now,std::uint64_t key,
                            std::uint64_t generation,std::uint64_t epoch,std::uint64_t request) noexcept {
    return r.serial&&r.attempt&&r.target&&r.targetIdentity&&r.collider&&r.rawPart>=0&&r.continuation&&
        r.seen&&r.accepted&&r.applied&&r.fields&&r.bodyContact&&r.kind==1&&
        contact58c1::piercing97(r)&&
        r.blocked==0&&r.perfect==0&&r.parry==0&&
        std::isfinite(r.damage)&&r.damage>0&&r.key==key&&r.generation==generation&&
        r.epoch==epoch&&r.request==request&&r.tick&&now>=r.tick&&now-r.tick<=80&&
        r.impactTick&&r.tick>=r.impactTick&&r.tick-r.impactTick<=600&&
        std::isfinite(r.point[0])&&std::isfinite(r.point[1])&&std::isfinite(r.point[2]);
}
// Cheap pre-filter for the armour read (combat TU, completed receipt): a body thrust the game did not defend.
inline bool wantsArmour(const contact58c1::Receipt& r) noexcept {
    return r.accepted&&r.applied&&r.fields&&r.bodyContact&&r.kind==1&&
        contact58c1::piercing97(r)&&r.blocked==0&&r.perfect==0&&r.parry==0&&r.target;
}

// ---------------------------------------------------------------- penetration model (ours)
enum class Refusal : int {None=0,NoWound,TooWeak,TooShallow,Invalid};
inline const char* name(Refusal r) noexcept {
    constexpr const char* n[]{"ELIGIBLE","NO_NATIVE_WOUND","TOO_WEAK_FOR_ARMOUR","TOO_SHALLOW","INVALID_INPUT"};
    const auto i=unsigned(r);return i<5?n[i]:"?";}
struct Tuning {
    double light=15,hard=60,minEnergy=.30,throughEnergy=.75,partial=.30,through=.55;bool halfArmour=false;
    static Tuning live() noexcept {
        Tuning t{};t.light=lightArmour.load(std::memory_order_relaxed);t.hard=hardArmour.load(std::memory_order_relaxed);
        t.minEnergy=impale40::minEnergy.load(std::memory_order_relaxed);t.throughEnergy=impale40::throughEnergy.load(std::memory_order_relaxed);
        t.partial=partialCm.load(std::memory_order_relaxed)*.01;t.through=throughCm.load(std::memory_order_relaxed)*.01;
        t.halfArmour=mode.load(std::memory_order_relaxed)==2;return t;}
};
struct Depth {Refusal refusal=Refusal::Invalid;double depth=0,hardness=.5,needEnergy=0;bool through=false;};
// energy: our stroke energy ratio (1 = a proper full stroke). armour: native stab defence at the struck zone, < 0 = unknown
// (treated as medium). damage: native health damage of this hit. lethal: the native hit killed him.
// bladeM: usable blade length in metres (<= 0 = unknown, no cap).
inline Depth depthFor(double energy,double armour,double damage,bool lethal,double bladeM,const Tuning& t) noexcept {
    Depth d{};
    if(!std::isfinite(energy)||!std::isfinite(damage)||!std::isfinite(armour)||!(t.hard>t.light)||!(t.partial>0)||!(t.through>0))return d;
    if(!(damage>0)){d.refusal=Refusal::NoWound;return d;}
    const double a=armour<0?-1:armour*(t.halfArmour?.5:1.0);
    d.hardness=a<0?.5:std::clamp((a-t.light)/(t.hard-t.light),0.0,1.0);
    const double e=std::clamp(energy,0.0,1.5);
    d.needEnergy=std::clamp(t.minEnergy+.35*d.hardness,0.0,1.0);
    if(!lethal&&e<d.needEnergy){d.refusal=Refusal::TooWeak;return d;}
    d.through=d.hardness<=.25&&e>=t.throughEnergy;
    if(d.through)d.depth=t.through;
    else{
        const double span=std::max(.05,1.0-d.needEnergy);
        d.depth=t.partial*std::clamp(.35+.65*(e-d.needEnergy)/span,0.0,1.0);
    }
    if(lethal)d.depth=std::max(d.depth,.12); // the native kill was a stab: the point is in
    if(bladeM>0){const double cap=std::max(0.0,bladeM-.10);if(d.depth>cap){d.depth=cap;d.through=d.through&&cap>=.35;}}
    if(d.depth<.05){d.refusal=Refusal::TooShallow;d.through=false;return d;}
    d.refusal=Refusal::None;return d;
}

// ---------------------------------------------------------------- living victim: the blade stays in the wound
// Our own state machine (game thread, one step per native frame). Inputs are measured from the physical blade and
// the struck part: pen = how far the blade tip is past the wound along the blade (m), lateral = distance of the
// wound from the blade line (m). The display keeps the blade in the wound; nothing native is written.
enum class Release : int {None=0,PullOut,Kick,HandPush,TornFree,Timeout,Shallow,Lost,Weapon};
inline const char* name(Release r) noexcept {
    constexpr const char* n[]{"NONE","PULL_OUT","KICK","HAND_PUSH","TORN_FREE","TIMEOUT","NEVER_WENT_IN","LOST","WEAPON_SIDE"};
    const auto i=unsigned(r);return i<9?n[i]:"?";}
struct StuckTuning {double in=.04,out=-.03,torn=.25;std::uint64_t tornMs=150,shallowMs=450,maxMs=4000;};
struct StuckIn {std::uint64_t now=0;double pen=0,lateral=0;bool kick=false,palmPush=false;};
struct Stuck {
    bool active=false,wasIn=false;std::uint64_t since=0,last=0,farSince=0;double cap=0,deepest=0;
    void start(std::uint64_t now,double depthCap) noexcept {*this={};active=true;since=last=now;cap=std::isfinite(depthCap)?std::clamp(depthCap,.05,.80):.15;}
    Release step(const StuckIn& in,const StuckTuning& t={}) noexcept {
        if(!active)return Release::None;
        const auto end=[&](Release r){active=false;return r;};
        if(!in.now||in.now<last||!std::isfinite(in.pen)||!std::isfinite(in.lateral))return end(Release::Lost);
        last=in.now;
        if(in.kick)return end(Release::Kick);
        if(in.palmPush)return end(Release::HandPush);
        const double pen=std::min(in.pen,cap);
        if(pen>=t.in){wasIn=true;deepest=std::max(deepest,pen);}
        if(wasIn&&in.pen<=t.out)return end(Release::PullOut);
        if(!wasIn&&in.now-since>=t.shallowMs)return end(Release::Shallow);
        if(in.lateral>t.torn){if(!farSince)farSince=in.now;else if(in.now-farSince>=t.tornMs)return end(Release::TornFree);}
        else farSince=0;
        if(in.now-since>t.maxMs)return end(Release::Timeout);
        return Release::None;
    }
};
// Free palm push (either mode): palm close to the wound and moving into the body fast enough.
inline bool palmPush(const double palm[3],const double palmVel[3],const double wound[3],double radius=.35,double speed=1.0) noexcept {
    double to[3],d2=0,v2=0,dot=0;for(int i=0;i<3;++i){to[i]=wound[i]-palm[i];d2+=to[i]*to[i];v2+=palmVel[i]*palmVel[i];dot+=palmVel[i]*to[i];}
    return std::isfinite(d2)&&std::isfinite(v2)&&d2<=radius*radius&&v2>=speed*speed&&dot>0;
}
}
