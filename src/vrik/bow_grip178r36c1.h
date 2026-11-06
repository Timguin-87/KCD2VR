#pragma once
#include "physical_weapon.h"

// Presentation only. No input edges, native charge, ammunition or shot writes.
namespace kcd2vr::bow_grip178r36c1 {
namespace ph = physical;
namespace m = pose_core;
struct Identity {
    std::uint64_t key=0,generation=0,request=0,epoch=0;
    bool operator==(const Identity& b) const noexcept {
        return key==b.key&&generation==b.generation&&request==b.request&&epoch==b.epoch;
    }
};
struct Latch {
    Identity identity{};
    ph::Pose stringInHand{};m::Quat stringInBow{};
    std::uint64_t tick=0,sample=0;
    bool held=false;
};
// All poses are in the SAME bow skeleton space. The physical kernel couples
// the bow root and drawing wrist. This leaf translates the string to that
// completed wrist; its captured bow-local attitude remains fixed. Giving the
// string independent wrist roll makes its child arrow rotate off the bow rest.
inline bool follow(Latch& s,Identity id,std::uint64_t sample,std::uint64_t tick,
        double grip,bool eligible,ph::Pose hand,ph::Pose string,ph::Pose& result) noexcept {
    if(!eligible||!id.key||!id.generation||!id.request||!id.epoch||!sample||!tick||
       !std::isfinite(grip)||!ph::finite(hand)||!ph::finite(string)||grip<.25){s={};return false;}
    if(s.held&&(!(s.identity==id)||tick<s.tick||tick-s.tick>80||sample<s.sample)){
        s={};return false;
    }
    if(!s.held){
        if(grip<.55||m::length(string.p-hand.p)>.18)return false;
        s.identity=id;s.stringInHand=ph::compose(ph::inverse(hand),string);s.stringInBow=string.q;s.held=true;
    }
    result=ph::compose(hand,s.stringInHand);result.q=s.stringInBow;
    if(!ph::finite(result)){s={};return false;}
    s.tick=tick;s.sample=sample;
    return true;
}
// Body model -> world -> actual bow skeleton; account for BOTH object scales.
inline bool hand_in_bow(ph::Pose hand,ph::Pose body,double bodyScale,
        ph::Pose bow,double bowScale,ph::Pose& result) noexcept {
    if(!ph::finite(hand)||!ph::finite(body)||!ph::finite(bow)||
       !std::isfinite(bodyScale)||!std::isfinite(bowScale)||
       bodyScale<=.1||bodyScale>=10||bowScale<=.1||bowScale>=10)return false;
    hand.p=hand.p*bodyScale;
    result=ph::compose(ph::inverse(bow),ph::compose(body,hand));
    result.p=result.p*(1.0/bowScale);
    return ph::finite(result);
}
} // namespace kcd2vr::bow_grip178r36c1
