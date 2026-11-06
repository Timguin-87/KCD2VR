#pragma once
#include <cmath>
#include <cstdint>
namespace kcd2vr::interaction112 {
struct Ray {
    float x=0,y=1,z=0,tan_x=0,tan_y=0;
    float ox=0,oy=0,oz=0,limit=0,radius=0;
    std::uint64_t tick=0;
    bool hand=false;
    int owner=-1;
    std::uint64_t sourceEpoch=0;std::uintptr_t sourcePlayer=0; // optional committed-palm identity, used only by door admission
};
inline bool Normalize(Ray& r) noexcept {
    const float length=std::sqrt(r.x*r.x+r.y*r.y+r.z*r.z);
    if(!std::isfinite(r.ox)||!std::isfinite(r.oy)||!std::isfinite(r.oz)||
       !std::isfinite(length)||length<1e-4f||!std::isfinite(r.limit)||
       !std::isfinite(r.radius)||r.limit<0||r.radius<0)return false;
    r.x/=length;r.y/=length;r.z/=length;return true;
}
inline bool Fresh(const Ray& r,std::uint64_t now) noexcept {
    return r.tick&&now>=r.tick&&now-r.tick<=(r.hand?80u:100u);
}
}
