#pragma once
#include "pose_math.h"
#include "npc_contact172.h"
#include <algorithm>
#include <cmath>
#include <limits>

namespace kcd2vr::weapon_exit_d63 {
namespace m=pose_core;
struct Surface {
    m::Vec3 point{},nativeNormal{};
    npc_contact172::Witness witness{};
    double distance=0;
    bool plane=false;
};
// A BASE proxy with an entity owner can be an NPC's secondary body. Only a
// successful pinned foreign-data read returning NULL qualifies this repair.
inline bool world(const npc_contact172::Witness& w) noexcept {
    using namespace npc_contact172;
    return w.mask==BodiesAndWorld&&w.body==Body::Physical&&w.vft==0x3a578c0&&
        w.collider&&w.foreignNullD62&&!w.ownerRead&&!w.owner&&!w.entityId&&w.part>=0;
}
inline double coordinateTolerance(double a,double b) noexcept {
    const float magnitude=static_cast<float>(std::max(std::abs(a),std::abs(b)));
    const double ulp=double(std::nextafter(magnitude,std::numeric_limits<float>::infinity()))-magnitude;
    return std::min(.0005,std::max(.000001,ulp+.000001));
}
inline bool samePoint(m::Vec3 a,m::Vec3 b) noexcept {
    return m::finite(a)&&m::finite(b)&&
        std::abs(a.x-b.x)<=coordinateTolerance(a.x,b.x)&&
        std::abs(a.y-b.y)<=coordinateTolerance(a.y,b.y)&&
        std::abs(a.z-b.z)<=coordinateTolerance(a.z,b.z);
}
inline bool onRay(const Surface& h,m::Vec3 from,m::Vec3 to) noexcept {
    const auto motion=to-from;const double length=m::length(motion);
    return h.plane&&world(h.witness)&&m::finite(h.nativeNormal)&&
        std::abs(m::length(h.nativeNormal)-1.)<.001&&std::isfinite(length)&&length>.0001&&
        std::isfinite(h.distance)&&h.distance>=0&&h.distance<=length&&
        samePoint(h.point,from+motion*(h.distance/length));
}
inline bool candidate(const Surface& h,m::Vec3 from,m::Vec3 to) noexcept {
    if(!onRay(h,from,to))return false;
    const double behind=m::dot(from-h.point,h.nativeNormal);
    const double outward=m::dot(to-from,h.nativeNormal);
    // Logged exits897..906 started1.47..1.61mm behind their actual plane.
    // This is only a shallow outward recovery, never general depenetration.
    return behind<-.00001&&behind>=-.003&&outward>.00001&&
        m::dot(to-h.point,h.nativeNormal)>.0001;
}
inline bool confirmed(const Surface& forward,const Surface& reverse,m::Vec3 from,m::Vec3 to) noexcept {
    if(!candidate(forward,from,to)||!onRay(reverse,to,from))return false;
    const auto& a=forward.witness;const auto& b=reverse.witness;
    return a.collider==b.collider&&a.vft==b.vft&&a.part==b.part&&
        a.surfaceIdx==b.surfaceIdx&&samePoint(forward.point,reverse.point)&&
        m::dot(forward.nativeNormal,reverse.nativeNormal)>.99999;
}
}
