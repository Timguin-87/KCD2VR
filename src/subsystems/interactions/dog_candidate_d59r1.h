#pragma once
#include "../../vrik/palm_selection59c9.h"
#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>

namespace kcd2vr::dog_candidate_d59r1 {
// A separate geometry witness for an already enumerated native Dog. This is
// deliberately not HandCandidate::object: native actors keep their head policy.
// The copied point is the native interaction point, not a proven head/fur hit.
struct Source {std::uint64_t generation=0,context=0;};
inline bool same(Source a,Source b) noexcept {
    return a.generation&&a.context&&a.generation==b.generation&&a.context==b.context;
}
struct Candidate {
    std::uint64_t tick=0,sourceTick=0,sourceEpoch=0;
    Source source{};std::uintptr_t sourcePlayer=0;
    std::uint32_t entity=0;int owner=-1;
    float point[3]{},distance=0;
};
inline bool fresh(std::uint64_t tick,std::uint64_t now) noexcept {
    return tick&&now>=tick&&now-tick<=80;
}
inline bool palm(const palm59c9::Pair& pair,int side,std::uint32_t target,std::uint64_t now) noexcept {
    if(side<0||side>1||!target||!pair.eligible[side]||
       (pair.locked>=0&&pair.locked!=side)||(pair.lockedEntity&&pair.lockedEntity!=target))return false;
    const auto& r=pair.ray[side];
    return r.hand&&r.owner==side&&r.sourcePlayer&&r.sourceEpoch&&interaction112::Fresh(r,now)&&
        std::isfinite(r.ox)&&std::isfinite(r.oy)&&std::isfinite(r.oz);
}
inline bool distance(const float* point,const interaction112::Ray& r,float& out) noexcept {
    if(!point)return false;
    const float dx=point[0]-r.ox,dy=point[1]-r.oy,dz=point[2]-r.oz;
    out=std::sqrt(dx*dx+dy*dy+dz*dz);
    return std::isfinite(out)&&out<=.20f;
}
// Small bounded copied cache; no native pointer is retained for dereference.
// Readers choose the closest current same-hand witness, not an old cached range.
struct Store {
    std::array<Candidate,16> entries{};unsigned next=0;
    void clear() noexcept {entries={};next=0;}
    void withdraw(std::uint32_t target) noexcept {
        for(auto& e:entries)if(e.entity==target)e=Candidate{};
    }
    void observe(std::uint32_t target,const char* cls,std::uintptr_t nativePlayerEntity,
                 const float* point,const palm59c9::Pair& pair,Source source,
                 std::uint64_t nativeTick,std::uint64_t now) noexcept {
        if(!target)return;
        withdraw(target); // a new outside/invalid observation retires this Dog
        if(!cls||std::strcmp(cls,"Dog")||!nativePlayerEntity||!same(source,source)||!fresh(nativeTick,now))return;
        for(int side=0;side<2;++side){
            if(!palm(pair,side,target,now))continue;
            const auto& ray=pair.ray[side];float separationM=0;
            if(ray.sourcePlayer!=nativePlayerEntity||!distance(point,ray,separationM))continue;
            Candidate c{};c.tick=now;c.sourceTick=ray.tick;c.sourceEpoch=ray.sourceEpoch;
            c.source=source;c.sourcePlayer=ray.sourcePlayer;c.entity=target;c.owner=side;c.distance=separationM;
            for(unsigned n=0;n<3;++n)c.point[n]=point[n];
            entries[next++%entries.size()]=c;
        }
    }
    bool read(int side,const palm59c9::Pair& pair,Source source,std::uint64_t now,Candidate& out) const noexcept {
        out=Candidate{};float best=.201f;
        for(const auto& c:entries){
            if(c.owner!=side||!same(c.source,source)||!fresh(c.tick,now)||!fresh(c.sourceTick,now)||
               !palm(pair,side,c.entity,now))continue;
            const auto& ray=pair.ray[side];float separationM=0;
            if(ray.sourcePlayer!=c.sourcePlayer||ray.sourceEpoch!=c.sourceEpoch||
               ray.tick<c.sourceTick||!distance(c.point,ray,separationM))continue;
            if(separationM<best||(separationM==best&&c.entity<out.entity)){out=c;out.distance=separationM;best=separationM;}
        }
        return out.entity!=0;
    }
};
}
