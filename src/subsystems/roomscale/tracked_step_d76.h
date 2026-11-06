#pragma once
#include <cmath>
#include <cstdint>

namespace kcd2vr::roomscale_d76 {
// Samples remain in raw OpenXR metres. This emits displacement, never a speed
// demand, and never integrates the native joystick or a camera-base change.
struct TrackedStepSample {
    std::uint64_t actorLifetime=0,tracking=0,origin=0,sample=0,tick=0,now=0;
    double x=0,y=0,z=0,standingY=0,leanHeight=.10;
    bool allowed=false,live=false;
};
struct TrackedStepDelta { double x=0,z=0; bool submit=false,retirePending=false,lean=false; };
struct TrackedStepState { TrackedStepSample last{}; bool ready=false,lean=false; };
inline TrackedStepDelta observe(TrackedStepState& s,const TrackedStepSample& i) noexcept {
    TrackedStepDelta out{};
    const bool finite=std::isfinite(i.x)&&std::isfinite(i.y)&&std::isfinite(i.z)&&
        std::isfinite(i.standingY)&&std::isfinite(i.leanHeight);
    if(!finite||!i.allowed||!i.live||!i.actorLifetime||!i.tracking||!i.origin||!i.sample||
       !i.tick||i.now<i.tick||i.now-i.tick>60){
        s={};out.retirePending=true;return out;
    }
    const bool same=s.ready&&s.last.actorLifetime==i.actorLifetime&&s.last.tracking==i.tracking&&s.last.origin==i.origin;
    if(same&&i.sample==s.last.sample)return out;
    if(same&&(i.sample<s.last.sample||i.tick<s.last.tick)){s={};out.retirePending=true;return out;}
    const bool continuity=same&&i.tick-s.last.tick<=60;
    const bool previousLean=continuity&&s.lean;
    bool lean=i.leanHeight>0&&i.y-i.standingY<=-i.leanHeight;
    if(previousLean&&i.leanHeight>0&&i.y-i.standingY<-i.leanHeight*.5)lean=true;
    out.lean=lean;
    if(continuity&&!previousLean&&!lean){
        out.x=i.x-s.last.x;out.z=i.z-s.last.z;
        // A discontinuous runtime relocation is not a physical step. No low
        // speed threshold or positional deadzone: small real steps stay 1:1.
        if(std::hypot(out.x,out.z)<=.5)out.submit=out.x!=0||out.z!=0;
        else {out.x=out.z=0;out.retirePending=true;}
    }else out.retirePending=true;
    s.last=i;s.ready=true;s.lean=lean;
    return out;
}
}
