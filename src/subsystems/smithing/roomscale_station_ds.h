// Station movement requests relative to the activation pose. No native writes.
// One existing root owner must apply each request and return any rejected part.
#pragma once
#include <cmath>
#include <cstdint>
#include <algorithm>
namespace kcd2vr::roomscale_station_ds {
struct Config {
    double halfExtent=1.0;   // Square: +/-1 m on each axis from the activation root.
    double deadZone=.03;
    double yawDeadZone=.02;
};
struct State {
    double appliedX=0,appliedY=0,appliedYaw=0;
    double originX=0,originY=0,originYaw=0;
    bool have=false;
};
inline void reset(State& s) noexcept {s=State{};}
inline bool finite3(double x,double y,double z) noexcept {
    return std::isfinite(x)&&std::isfinite(y)&&std::isfinite(z);
}
inline double signedYaw(double yaw) noexcept {
    return std::remainder(yaw,6.28318530717958647692);
}
struct Request {double dx=0,dy=0,dyaw=0;bool move=false,turn=false;};
// HMD coordinates/yaw are absolute samples in a stable station-local frame.
// Activation anchors the current native root to the first HMD sample: no jump
// and no claim that an unrequested HMD offset has already moved the actor.
inline Request step(State& s,double hmdX,double hmdY,double hmdYaw,bool active,const Config& c) noexcept {
    Request r{};
    if(!active||!finite3(hmdX,hmdY,hmdYaw)){reset(s);return r;}
    if(!s.have){s.originX=hmdX;s.originY=hmdY;s.originYaw=signedYaw(hmdYaw);s.have=true;return r;}
    const double extent=std::max(0.,c.halfExtent);
    const double tx=std::clamp(hmdX-s.originX,-extent,extent);
    const double ty=std::clamp(hmdY-s.originY,-extent,extent);
    const double dx=tx-s.appliedX,dy=ty-s.appliedY;
    // Keep unapplied sub-threshold motion pending so slow movement accumulates.
    if(std::hypot(dx,dy)>c.deadZone){
        r.dx=dx;r.dy=dy;r.move=true;s.appliedX=tx;s.appliedY=ty;
    }
    const double targetYaw=signedYaw(signedYaw(hmdYaw)-s.originYaw);
    const double dyaw=signedYaw(targetYaw-s.appliedYaw);
    if(std::abs(dyaw)>c.yawDeadZone){
        r.dyaw=dyaw;r.turn=true;s.appliedYaw=targetYaw;
    }
    return r;
}
// Return rejected movement, including a refused/partially applied body turn.
inline void rollback(State& s,double rejectedDx,double rejectedDy,double rejectedYaw=0) noexcept {
    s.appliedX-=rejectedDx;s.appliedY-=rejectedDy;
    s.appliedYaw=signedYaw(s.appliedYaw-rejectedYaw);
}
}
