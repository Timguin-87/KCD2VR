#pragma once
#include "ammo_carry_t18.h"
#include "../../core/mutual_admission_t25.h"
#include <cstdint>

namespace kcd2vr::hand_claim_t24 {
// Callers supply already-qualified current actor identities; this policy never
// follows native pointers, changes hand keys, expires a lease or performs cleanup.
struct Owner {
    std::uintptr_t player=0,manager=0,character=0;
    std::uint64_t body=0,request=0;
};
enum class State : unsigned char { Free,CurrentGrant,SameActorBusy,ForeignActor,Unknown };
// CurrentGrant names an exact identity join only. Fresh completed pose/native
// mutation permission is still a separate T22/T23 capability.
struct Observation { State state=State::Unknown;ammo_carry_t18::Claim claim{}; };
inline bool blocks(State s) noexcept {
    return s==State::CurrentGrant||s==State::SameActorBusy||s==State::Unknown;
}
inline bool samePublication(const ammo_carry_t18::Claim& a,const ammo_carry_t18::Claim& b) noexcept {
    return a.serial==b.serial&&a.player==b.player&&a.manager==b.manager&&a.character==b.character&&
        a.body==b.body&&a.request==b.request&&a.key==b.key&&a.preview==b.preview&&a.entity==b.entity&&a.stage==b.stage;
}
// Explicit bounded read boundary also lets fixtures substitute only mailbox
// delivery, while exercising this exact production policy and its consumers.
struct Reads {
    unsigned before=0,after=0;bool firstKnown=false,secondKnown=false;
    ammo_carry_t18::Claim first{},second{};
};
inline Observation classify(int side,const Owner& o,const Reads& r) noexcept {
    if(side<0||side>1)return {};
    const unsigned bit=1u<<side;
    const bool busy=((r.before|r.after)&bit)!=0;
    const bool first=r.firstKnown&&r.first.serial,second=r.secondKnown&&r.second.serial;
    // A first publication is also visible before an older producer sets its
    // bit. An uninitialized OFF mailbox with both bit reads clear is free.
    if(!busy&&!first&&!second)return {State::Free,{}};
    if(!first||!second||!samePublication(r.first,r.second))return {};
    const auto& c=r.second;
    if(!c.player||!c.manager||!c.character||!c.body||!c.request||
       static_cast<unsigned>(c.stage)>static_cast<unsigned>(ammo_carry_t18::Stage::Fault))return {};
    const bool exact=o.player&&o.manager&&o.character&&o.body&&o.request&&
        o.player==c.player&&o.manager==c.manager&&o.character==c.character&&o.body==c.body&&o.request==c.request;
    if(exact)return {State::CurrentGrant,c};
    // The same native player can still own its old model hand after pose/body,
    // request, manager or character changes. Lost grant is not native retirement.
    if(o.player&&o.player==c.player)return {State::SameActorBusy,c};
    if(o.manager&&o.manager==c.manager)return {State::Unknown,c};
    if(o.player&&o.manager&&o.player!=c.player&&o.manager!=c.manager)return {State::ForeignActor,c};
    return {State::Unknown,c};
}
inline Observation observe(int side,const Owner& owner) noexcept {
    if(side<0||side>1)return {};
    if(mutual_admission_t25::pickupClaimsD72[side].load(std::memory_order_acquire))return {State::SameActorBusy,{}};
    Reads r{};
    r.before=ammo_carry_t18::hands.load(std::memory_order_acquire);
    r.firstKnown=ammo_carry_t18::claims[side].read(r.first);
    r.secondKnown=ammo_carry_t18::claims[side].read(r.second);
    r.after=ammo_carry_t18::hands.load(std::memory_order_acquire);
    return classify(side,owner,r);
}
inline bool competing(int side,const Owner& owner) noexcept {return blocks(observe(side,owner).state);}

// Utility admission is per hand and release-rearmed. Losing a claim while a
// button stays held must not create a new Quick Talk/torch/visor press or release.
struct UtilityGate {
    bool quarantined[2]{};
    bool allow(int side,bool down,bool claimed) noexcept {
        if(side<0||side>1)return false;
        if(claimed)quarantined[side]=true;
        if(!down&&!claimed)quarantined[side]=false;
        return !claimed&&!quarantined[side];
    }
};
}
