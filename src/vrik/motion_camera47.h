#pragma once
#include "pose_math.h"
#include <cstdint>
namespace kcd2vr::motion_camera47 {
namespace m=pose_core;
struct Input {
    std::uintptr_t character=0;std::uint64_t epoch=0,tick=0;
    m::Vec3 root{},nativeHead{},calibratedHead{},nativeEye{},nativeForward{0,1,0};
    m::Quat rotation{};double scale=1;bool allowed=false,neutral=false;
};
struct State {std::uintptr_t character=0;std::uint64_t epoch=0;bool ready=false;m::Vec3 eyeOffset{},forward{0,1,0};};
struct Result {m::Vec3 eye{},forward{};};
// Camera-local eye offset is learned once from a neutral, original-body packet.
// Never learn an authored draw/release/hit pose as another neutral, or retain a
// world-space camera. Fresh root motion and HMD composition remain independent.
inline bool solve(State& s,const Input& i,std::uint64_t now,Result& out) noexcept {
    out={};
    if(!i.allowed||!i.character||!i.epoch||!i.tick||now<i.tick||now-i.tick>80||
       !m::finite(i.root)||!m::finite(i.nativeHead)||!m::finite(i.calibratedHead)||
       !m::finite(i.nativeEye)||!m::finite(i.nativeForward)||!std::isfinite(i.scale)||i.scale<.1||i.scale>10)return false;
    const double norm=i.rotation.x*i.rotation.x+i.rotation.y*i.rotation.y+i.rotation.z*i.rotation.z+i.rotation.w*i.rotation.w;
    if(!std::isfinite(norm)||std::fabs(norm-1)>.01)return false;
    if(s.character!=i.character||s.epoch!=i.epoch){s={};s.character=i.character;s.epoch=i.epoch;}
    const m::Quat inv{-i.rotation.x,-i.rotation.y,-i.rotation.z,i.rotation.w};
    if(!s.ready){
        if(!i.neutral)return false;
        const auto offset=m::rotate(inv,i.nativeEye-i.root)*(1/i.scale)-i.nativeHead;
        auto forward=m::rotate(inv,i.nativeForward);forward.z=0;const auto f=m::unit(forward);
        if(!f||!m::finite(offset)||m::length(offset)>.30)return false;
        s.eyeOffset=offset;s.forward=*f;s.ready=true;
    }
    out.eye=i.root+m::rotate(i.rotation,(i.calibratedHead+s.eyeOffset)*i.scale);
    auto forward=m::rotate(i.rotation,s.forward);forward.z=0;const auto f=m::unit(forward);
    if(!f||!m::finite(out.eye)||m::length(out.eye-i.nativeEye)>1.)return false;
    out.forward=*f;return true;
}
}
