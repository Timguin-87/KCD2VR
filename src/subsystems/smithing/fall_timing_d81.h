#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>

namespace kcd2vr::subsystems::smithing::fall_d81 {
// Permission to shorten one native fall, earned only by our acknowledged lift
// and deliberate reversal. This never supplies charge, a collision or a hit.
struct Receipt {
    std::uintptr_t instance=0;
    std::uint64_t lifetime=0,request=0,poseEpoch=0,sourceEpoch=0,modeEpoch=0;
    std::uint64_t sequence=0,tick=0,released=0;
    int phase=-1;
};
struct Input {
    Receipt identity{};
    bool current=false,start=false,held=false,release=false,abort=false;
};
inline bool same(const Receipt& a,const Receipt& b) noexcept {
    return a.instance&&a.instance==b.instance&&a.lifetime==b.lifetime&&
        a.poseEpoch==b.poseEpoch&&a.sourceEpoch==b.sourceEpoch&&a.modeEpoch==b.modeEpoch;
}
inline bool fresh(const Receipt& r,std::uint64_t now) noexcept {
    return r.instance&&r.lifetime&&r.request&&r.poseEpoch&&r.sourceEpoch&&r.modeEpoch&&
        r.sequence&&r.tick&&now>=r.tick&&now-r.tick<=32&&r.released&&
        now>=r.released&&now-r.released<=700&&(r.phase==14||r.phase==16);
}
inline float rate(float dt,float controllerScale) noexcept {
    if(!std::isfinite(dt)||dt<=0.f||dt>.08f||!std::isfinite(controllerScale)||
       controllerScale<=0.f||controllerScale>8.f)return 1.f;
    return std::max(1.f,std::min(4.f,.08f/(dt*controllerScale)));
}
class Tracker {
    Receipt owner_{};
    std::uint64_t next_=0,started_=0;
    bool raised_=false,fell_=false;
    void clear() noexcept {owner_={};started_=0;raised_=fell_=false;}
public:
    Receipt step(const Input& in,std::uint64_t now) noexcept {
        const auto& v=in.identity;
        if(!in.current||in.abort||!v.instance||!v.lifetime||!v.poseEpoch||!v.sourceEpoch||!v.modeEpoch||
           !v.sequence||!v.tick||now<v.tick||now-v.tick>32){clear();return {};}
        if(owner_.instance&&(!same(owner_,v)||v.sequence<owner_.sequence||v.tick<owner_.tick))clear();
        if(in.start&&v.phase==11){clear();owner_=v;owner_.request=++next_;if(!next_)owner_.request=++next_;started_=now;}
        if(!owner_.instance)return {};
        if(owner_.released){
            if(now<owner_.released||now-owner_.released>700||v.phase==11||v.phase==17||
               (v.phase!=14&&v.phase!=16)||(fell_&&v.phase==14)){clear();return {};}
            fell_=fell_||v.phase==16;
        }else{
            if((v.phase!=11&&v.phase!=14)||(raised_&&v.phase==11)||
               (!raised_&&(now<started_||now-started_>450))){clear();return {};}
            // The native phase observation and our release can arrive in the
            // same input packet. The producer still owns this exact requested
            // lift; requiring an extra held packet loses its real phase14 ACK
            // when the player has already begun the downward reversal.
            if(v.phase==14&&(in.held||in.release))raised_=true;
            if(in.release){if(!raised_||v.phase!=14){clear();return {};}owner_.released=now;}
        }
        owner_.sequence=v.sequence;owner_.tick=v.tick;owner_.phase=v.phase;
        return owner_.released?owner_:Receipt{};
    }
};
}
