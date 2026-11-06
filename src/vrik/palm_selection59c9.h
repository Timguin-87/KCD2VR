#pragma once
#include "interaction_cone57c1.h"
#include "../core/door_surface65.h"
#include <limits>

namespace kcd2vr::palm59c9 {
// A pair of fresh, empty-hand palm rays. Closing Grip latches the owner;
// native candidate/action validation still decides whether anything is usable.
struct Pair { interaction112::Ray ray[2]{}; bool eligible[2]{}; int locked=-1; std::uint32_t lockedEntity=0; };
// Mod-thread copied state only. Grip owns a HAND before a native Use has
// actually been transported; an arbitrary hover is not an object transaction.
// This prevents a nearby Pickable from excluding a direct Door first offered
// after the squeeze. Once transported, keep that exact object until release or
// source retirement; candidate enumeration cannot silently retarget it.
struct TransportLock64 {
    int side=-1;std::uint32_t target=0;bool transported=false;
    std::uint64_t tick=0,epoch=0,generation=0,press=0,tracking=0,settings=0,experiment=0,acceptedSerial=0;
    unsigned mode=0;std::uintptr_t player=0;
    std::uint32_t observe(const Pair& pair,int owner,std::uint64_t sourcePress,std::uint64_t sourceGeneration,
                          std::uint64_t trackingEpoch,std::uint64_t settingsEpoch,std::uint64_t experimentEpoch,
                          unsigned f5Mode,std::uint64_t now) noexcept {
        if(owner<0||owner>1||pair.locked!=owner||!pair.eligible[owner]||!now||
           !interaction112::Fresh(pair.ray[owner],now)||!pair.ray[owner].sourceEpoch||!pair.ray[owner].sourcePlayer||
           !sourcePress||now<sourcePress||!sourceGeneration||!trackingEpoch||!settingsEpoch){*this={};return 0;}
        const auto& ray=pair.ray[owner];
        if(side!=owner||epoch!=ray.sourceEpoch||player!=ray.sourcePlayer||generation!=sourceGeneration||
           press!=sourcePress||tracking!=trackingEpoch||settings!=settingsEpoch||experiment!=experimentEpoch||mode!=f5Mode||
           !tick||now<tick||now-tick>80){*this={};side=owner;epoch=ray.sourceEpoch;player=ray.sourcePlayer;generation=sourceGeneration;
            press=sourcePress;tracking=trackingEpoch;settings=settingsEpoch;experiment=experimentEpoch;mode=f5Mode;}
        tick=now;return target;
    }
    bool current(const Pair& pair,int owner,std::uint64_t sourcePress,std::uint64_t sourceGeneration,
                 std::uint64_t trackingEpoch,std::uint64_t settingsEpoch,std::uint64_t experimentEpoch,
                 unsigned f5Mode,std::uint64_t now) const noexcept {
        return sourcePress&&press==sourcePress&&now>=sourcePress&&side==owner&&owner>=0&&owner<2&&pair.locked==owner&&
           pair.eligible[owner]&&tick&&now>=tick&&now-tick<=80&&generation==sourceGeneration&&
           tracking==trackingEpoch&&settings==settingsEpoch&&experiment==experimentEpoch&&mode==f5Mode&&
           pair.ray[owner].sourceEpoch==epoch&&pair.ray[owner].sourcePlayer==player&&
           interaction112::Fresh(pair.ray[owner],now);
    }
    void claim(const Pair& pair,int owner,std::uint32_t entity,std::uint64_t sourcePress,
               std::uint64_t sourceGeneration,std::uint64_t trackingEpoch,std::uint64_t settingsEpoch,
               std::uint64_t experimentEpoch,unsigned f5Mode,std::uint64_t now) noexcept {
        if(transported||!current(pair,owner,sourcePress,sourceGeneration,trackingEpoch,settingsEpoch,experimentEpoch,f5Mode,now))return;
        transported=true;target=entity;
    }
    // A native-accepted Door can resolve an earlier generic physical Use to
    // its actual target. This is ONE adoption of that same transported press,
    // not a new pulse or candidate-driven retarget.
    void adoptAccepted(std::uint32_t entity,std::uint64_t sourcePress,std::uint64_t serial) noexcept {
        if(!transported||!entity||!serial||acceptedSerial||press!=sourcePress)return;
        target=entity;acceptedSerial=serial;
    }
};
inline TransportLock64 ordinaryTransportLock64{};
inline bool accepts(const Pair& pair,std::uint32_t entity,int side) noexcept {
    return side>=0&&side<2&&(!pair.lockedEntity||entity==pair.lockedEntity);
}
struct Choice { int side=-1; float distance=0,score=std::numeric_limits<float>::infinity(); };
inline Choice choose(const Pair& pair,float x,float y,float z,std::uint64_t now,
                     int previous=-1) noexcept {
    Choice best{};
    for(int side=0;side<2;++side){
        const auto& ray=pair.ray[side];
        if(!pair.eligible[side]||(pair.locked>=0&&pair.locked!=side)||
           !ray.hand||!interaction112::Fresh(ray,now))continue;
        const float dx=x-ray.ox,dy=y-ray.oy,dz=z-ray.oz;
        const float distance=std::sqrt(dx*dx+dy*dy+dz*dz);
        if(!std::isfinite(distance)||distance>interaction57c1::HandReach)continue;
        // A hand already over an object need not aim the finger axis through it.
        // This is candidate acquisition only, not collision radius or a forced Use.
        const auto p=interaction57c1::Project(ray,x,y,z,interaction57c1::HandReach,
            interaction57c1::HandRadius,interaction57c1::HandHalfAngle);
        if(distance>.16f&&!p.inside)continue;
        const float score=std::max(0.f,distance-(side==(previous>=0?previous:0)?.025f:0.f));
        if(score<best.score)best={side,distance,score};
    }
    return best;
}
// Nearest candidate within one native enumeration, independent of enumeration
// order. A 2 cm preference stabilizes adjacent objects; stale IDs never win.
struct Ranking {
    std::uint64_t pass=0,tick=0; std::uint32_t id=0,preferred=0;
    float score=std::numeric_limits<float>::infinity();
    float evaluate(std::uint64_t next,std::uint64_t now,std::uint32_t candidate,float distance) noexcept {
        if(next!=pass||!tick||now<tick||now-tick>80){preferred=(tick&&now>=tick&&now-tick<=80)?id:0;pass=next;id=0;score=std::numeric_limits<float>::infinity();}
        return std::max(0.f,distance-(candidate&&candidate==preferred?.02f:0.f));
    }
    bool offer(std::uint64_t now,std::uint32_t candidate,float value) noexcept {
        if(!std::isfinite(value)||value>=score)return false;
        score=value;id=candidate;tick=now;return true;
    }
};
}
