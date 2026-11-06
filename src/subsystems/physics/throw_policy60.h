#pragma once
#include "core/snapshot_mailbox.h"
#include <cmath>
#include <cstdint>
// DS-F92: the THROW - a grip release with a high hand speed becomes a throw
// (the prop gets the release velocity, capped) instead of a plain drop. Pure
// policy + the release-velocity capture; the native SetVelocity on the prop
// lease is the backend's retire seam (Codex). Host-tested.
namespace kcd2vr::throw60 {
inline constexpr float kMinThrowSpeed=1.5f; // m/s: below this a release is a drop
inline constexpr float kMaxThrowSpeed=10.f; // m/s cap
inline constexpr std::uint64_t kMaxDeltaMs=100; // the release velocity window
// Velocity from the last two evidence positions (world metres).
inline void velocity(const float prev[3],const float cur[3],std::uint64_t prevTick,std::uint64_t curTick,float out[3]) noexcept {
    out[0]=out[1]=out[2]=0;
    if(!prevTick||!curTick||curTick<=prevTick||curTick-prevTick>kMaxDeltaMs)return;
    const double dt=double(curTick-prevTick)/1000.0;
    if(dt<=0)return;
    out[0]=float((double(cur[0])-prev[0])/dt);
    out[1]=float((double(cur[1])-prev[1])/dt);
    out[2]=float((double(cur[2])-prev[2])/dt);
}
inline float speed(const float v[3]) noexcept {
    return std::isfinite(v[0])&&std::isfinite(v[1])&&std::isfinite(v[2])?
        std::sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]):0.f;
}
// A release whose hand speed clears the threshold is a throw; the capped
// velocity is what the prop should get.
inline bool shouldThrow(const float v[3]) noexcept {return speed(v)>=kMinThrowSpeed;}
inline void throwVelocity(const float v[3],float out[3]) noexcept {
    out[0]=out[1]=out[2]=0;
    const float s=speed(v);
    if(s<=1e-4f)return;
    const float k=s>kMaxThrowSpeed?kMaxThrowSpeed/s:1.f;
    out[0]=v[0]*k;out[1]=v[1]*k;out[2]=v[2]*k;
}
// DS-F92: the release-with-velocity request the input helper publishes; the
// producer/backend's retire seam applies SetVelocity on the prop lease.
struct ThrowRequest {
    std::uint64_t press=0;int side=-1;float v[3]{};
    std::uintptr_t body=0,entityPointer=0,world=0;
    std::uint64_t worldGeneration=0,tick=0,throwEpoch=0,physicsEpoch=0,poseEpoch=0,trackingEpoch=0;
    std::uint32_t entity=0;
};
// D40: simultaneous releases must retain both exact owners independently.
inline core::SnapshotMailbox<ThrowRequest> mailbox[2];
inline bool publish(const ThrowRequest& r) noexcept {return r.side>=0&&r.side<2&&mailbox[r.side].publish(r);}
inline bool read(int side,ThrowRequest& r) noexcept {return side>=0&&side<2&&mailbox[side].read(r);}
}
