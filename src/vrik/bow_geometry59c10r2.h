#pragma once
#include "pose_math.h"
#include <cmath>
#include <cstdint>

namespace kcd2vr::bow_geometry59c10r2 {
namespace pm = kcd2vr::pose_core;
// These identify LOCAL rig geometry. No world pose or controller pose is cached.
struct Identity {
    std::uint64_t key=0,generation=0,request=0,epoch=0;
    void* entity=nullptr;void* character=nullptr;void* pose=nullptr;void* owner=nullptr;
    std::uintptr_t poseType=0;
    double scale=0;
};
inline bool valid(const Identity& i) noexcept {
    return i.key&&i.generation&&i.request&&i.epoch&&i.entity&&i.character&&i.pose&&i.owner&&i.poseType&&
        std::isfinite(i.scale)&&i.scale>.1&&i.scale<10;
}
inline bool same(const Identity& a,const Identity& b) noexcept {
    return valid(a)&&valid(b)&&a.key==b.key&&a.generation==b.generation&&a.request==b.request&&a.epoch==b.epoch&&
        a.entity==b.entity&&a.character==b.character&&a.pose==b.pose&&a.owner==b.owner&&a.poseType==b.poseType&&
        std::abs(a.scale-b.scale)<=1e-5;
}
struct Placement {
    Identity identity{};
    std::uint64_t lifetime=0,tick=0,sourceTick=0,sample=0;
};
inline bool fresh(const Placement& p,std::uint64_t now) noexcept {
    return valid(p.identity)&&p.lifetime&&p.sample&&p.tick&&p.sourceTick&&now>=p.tick&&now>=p.sourceTick&&
        now-p.tick<=80&&now-p.sourceTick<=80;
}
// Native attachment owner only. A failed identity read retires the lifetime;
// a later matching pointer cannot resurrect geometry from before that failure.
struct Lifetimes {
    Identity previous{};std::uint64_t serial=0;
    Placement observe(const Identity& i,std::uint64_t now,std::uint64_t sourceTick,std::uint64_t sample) noexcept {
        if(!valid(i)||!now||!sourceTick||now<sourceTick||now-sourceTick>80||!sample){previous={};return {};}
        if(!same(previous,i)){if(++serial==0)++serial;previous=i;}
        return {i,serial,now,sourceTick,sample};
    }
};
struct Geometry : Identity {
    std::uint64_t tick=0,lifetime=0;
    pm::Vec3 nock{},axis{};double drawLength=0;bool restStructural175=false;bool partialAxis48=false;
};
inline bool usable(const Geometry& g,const Placement& p,std::uint64_t now) noexcept {
    // The measurement timestamp remains its original capture time. Only current
    // successful placement can authorize reuse; elapsed measurement age cannot
    // revoke immutable local geometry or rejuvenate a stale placement.
    return g.tick&&now>=g.tick&&fresh(p,now)&&g.lifetime==p.lifetime&&same(g,p.identity)&&
        pm::finite(g.nock)&&pm::unit(g.axis)&&std::isfinite(g.drawLength)&&
        ((g.restStructural175&&g.drawLength==0)||(g.drawLength>=.12&&g.drawLength<=.95));
}
} // namespace kcd2vr::bow_geometry59c10r2
