#pragma once
// Render-only envelope. Every point is copied from the successfully committed
// fixed-anatomy MODEL pose; never raw tracking, guessed world pivots or a new IK.
#include <algorithm>
#include <cmath>
#include <cstdint>

namespace kcd2vr::render_bounds63 {
inline constexpr std::uintptr_t kGetter=0x1AAFCE0, kCaller=0x63F90E, kCharacterVft=0x40602F8;
struct Box { float v[6]{}; };
struct Envelope {
    Box box{}; float world[8]{};
    std::uintptr_t character=0;
    std::uint64_t epoch=0,sample=0,tick=0,publication=0,policy=0,control=0;
    unsigned points=0; bool freshTracked=false;
};
inline bool valid(const Box& b) noexcept {
    for(int k=0;k<3;++k)if(!std::isfinite(b.v[k])||!std::isfinite(b.v[k+3])||
        b.v[k]>b.v[k+3]||b.v[k+3]-b.v[k]>6.f)return false;
    return true;
}
inline void add(Envelope& e,float x,float y,float z) noexcept {
    const float p[3]{x,y,z};
    for(float f:p)if(!std::isfinite(f))return;
    for(int k=0;k<3;++k){
        if(!e.points)e.box.v[k]=e.box.v[k+3]=p[k];
        else{e.box.v[k]=std::min(e.box.v[k],p[k]);e.box.v[k+3]=std::max(e.box.v[k+3],p[k]);}
    }
    ++e.points;
}
inline bool current(const Envelope& e,std::uintptr_t character,std::uint64_t epoch,
        std::uint64_t publication,std::uint64_t policy,std::uint64_t control,std::uint64_t now) noexcept {
    return e.freshTracked&&e.points>=4&&e.character==character&&character&&e.epoch==epoch&&epoch&&
        e.sample&&e.tick&&now>=e.tick&&now-e.tick<=80&&
        e.publication==publication&&publication&&!(publication&1)&&e.policy==policy&&!(policy&1)&&
        e.control==control&&std::isfinite(e.world[7])&&e.world[7]>=.1f&&e.world[7]<=10.f&&valid(e.box);
}
inline bool unite(const Box& native,const Envelope& e,Box& result) noexcept {
    if(!valid(native)||!valid(e.box)||!e.points||!std::isfinite(e.world[7])||e.world[7]<.1f||e.world[7]>10.f)return false;
    // 8cm skin/render margin in world metres. This changes neither tracking
    // expiry nor physical geometry; no native box or cache is written here.
    const float margin=.08f/e.world[7];
    for(int k=0;k<3;++k){
        result.v[k]=std::min(native.v[k],e.box.v[k]-margin);
        result.v[k+3]=std::max(native.v[k+3],e.box.v[k+3]+margin);
        // Reject an implausible envelope instead of disabling culling globally.
        if((native.v[k]-result.v[k])*e.world[7]>1.25f||
           (result.v[k+3]-native.v[k+3])*e.world[7]>1.25f)return false;
    }
    return valid(result);
}
}
