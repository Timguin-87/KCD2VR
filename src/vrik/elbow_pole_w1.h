#pragma once
// W1 elbow pole (Claude, 25 Oct 2026). Miki: "the elbows are still not good".
// 25 Oct log (8318 VRIK_F7 ELBOW samples): measured perpendicular to the arm, the torso clearance search moved the
// pole >20 deg in only 94 left / 19 right samples - so no torso flipping. what stays wrong is the body-fixed pole
// itself: (0.35 out, -0.25 back, -1 down) per side. for a hanging arm the down part is along the arm and drops
// out, leaving out > back = elbow sideways ("chicken wing"). FRIK-like relaxed arm: elbow mostly BACK, a bit out.
// forward/sideways reaches keep the elbow down (down still dominates there). values are INI tunable.
#include <atomic>
#include <cmath>
namespace kcd2vr::elbow_pole_w1 {
inline std::atomic<float> outward{.20f}; // INI VrElbowPoleOutW1  (old .35)
inline std::atomic<float> backward{.45f};// INI VrElbowPoleBackW1 (old .25)
struct V {double x=0,y=0,z=0;};
// body frame: x right, y forward, z up. side 0 right, 1 left.
inline V preference(int side,double out,double back) noexcept {
    V p{side?-out:out,-back,-1.0};const double l=std::sqrt(p.x*p.x+p.y*p.y+p.z*p.z);
    if(!(l>1e-9))return {0,0,-1};return {p.x/l,p.y/l,p.z/l};
}
// pole as seen by the arm: preference minus its part along the shoulder->hand axis (unit axis).
inline V perpendicular(V p,V axis) noexcept {
    const double d=p.x*axis.x+p.y*axis.y+p.z*axis.z;V r{p.x-axis.x*d,p.y-axis.y*d,p.z-axis.z*d};
    const double l=std::sqrt(r.x*r.x+r.y*r.y+r.z*r.z);if(!(l>1e-9))return {0,0,0};return {r.x/l,r.y/l,r.z/l};
}
inline float clampOut(float v) noexcept {return std::isfinite(v)?(v<0.f?0.f:v>1.f?1.f:v):.20f;}
inline float clampBack(float v) noexcept {return std::isfinite(v)?(v<0.f?0.f:v>1.f?1.f:v):.45f;}
}
