#pragma once
#include "pose_math.h"
#include <cstdint>
namespace kcd2vr::bow_camera35 {
namespace m=pose_core;
struct Owner {std::uintptr_t player=0,manager=0;std::uint64_t key=0,generation=0,request=0,epoch=0;};
inline bool same(const Owner&a,const Owner&b) noexcept {return a.player==b.player&&a.manager==b.manager&&a.key==b.key&&a.generation==b.generation&&a.request==b.request&&a.epoch==b.epoch;}
struct Input {Owner owner{};std::uint64_t tick=0,sample=0; m::Vec3 origin{};m::Quat rotation{};double scale=1;bool qualified=false,drawing=false;};
inline bool fresh(const Input& in,std::uint64_t now) noexcept {
    const double n=in.rotation.x*in.rotation.x+in.rotation.y*in.rotation.y+in.rotation.z*in.rotation.z+in.rotation.w*in.rotation.w;
    return in.qualified&&in.owner.player&&in.owner.manager&&in.owner.key&&in.owner.generation&&in.owner.request&&in.owner.epoch&&
        in.sample&&in.tick&&now>=in.tick&&now-in.tick<=80&&m::finite(in.origin)&&std::isfinite(n)&&std::fabs(n-1)<.01&&
        std::isfinite(in.scale)&&in.scale>=.1&&in.scale<=10;
}
struct State {Owner owner{};m::Vec3 idleModel{};std::uint64_t sourceTick=0,sample=0;bool ready=false;};
// Model-local NEUTRAL eye only: current body translation/rotation/scale remains
// live. No camera-derived HMD delta, native pointer, smoothing or world replay.
inline bool correct(State& state,const Input& in,m::Vec3 native,std::uint64_t now,m::Vec3& correction) noexcept {
    correction={};
    if(!fresh(in,now)||!m::finite(native)){state={};return false;}
    if(!same(state.owner,in.owner)){state={};state.owner=in.owner;}
    if(state.ready&&(in.sample<state.sample||in.tick<state.sourceTick)){state={};return false;}
    const m::Quat inverse{-in.rotation.x,-in.rotation.y,-in.rotation.z,in.rotation.w};
    const auto local=m::rotate(inverse,native-in.origin)*(1/in.scale);
    if(!m::finite(local)){state={};return false;}
    // A release/automatic nock is still an authored bow pose. Never train it
    // as the next neutral eye (the old code relearned every non-drawing frame).
    if(!state.ready&&!in.drawing){state.idleModel=local;state.sourceTick=in.tick;state.sample=in.sample;state.ready=true;return false;}
    if(!state.ready)return false; // first observed pose may already be authored draw
    const auto desired=in.origin+m::rotate(in.rotation,state.idleModel*in.scale);
    const auto delta=desired-native;
    if(!m::finite(delta)||m::length(delta)>=.40){state={};return false;}
    state.sourceTick=in.tick;state.sample=in.sample;correction=delta;return true;
}
}
