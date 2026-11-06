#pragma once
#include "pose_math.h"

namespace kcd2vr::tracking125 {
namespace m=pose_core;
// OpenXR coordinates: +Y up, -Z forward. Pitch/roll remain physical, including
// after recenter. At a near-vertical look retain the previous horizontal yaw.
inline m::Quat yaw_only(m::Quat orientation,m::Quat previous={}) noexcept {
    previous=m::unit(previous).value_or(m::Quat{});
    const auto q=m::unit(orientation);
    if(!q)return previous;
    const auto forward=m::rotate(*q,{0,0,-1});
    if(std::hypot(forward.x,forward.z)<.10)return previous;
    const double yaw=std::atan2(-forward.x,-forward.z);
    return {0,std::sin(yaw*.5),0,std::cos(yaw*.5)};
}
struct Origin {m::Vec3 position{};m::Quat yaw{};};
using OriginVisitorH46=bool(*)(const Origin&,void*) noexcept;
using OriginReaderH46=bool(*)(OriginVisitorH46,void*) noexcept;
inline Origin recenter(m::Vec3 position,m::Quat orientation,m::Quat previous={}) noexcept {
    return {position,yaw_only(orientation,previous)};
}
inline m::Vec3 model_vector(m::Vec3 xr,m::Quat yaw) noexcept {
    const auto v=m::rotate({-yaw.x,-yaw.y,-yaw.z,yaw.w},xr);
    return {v.x,-v.z,v.y};
}
inline m::Vec3 displacement(const Origin& origin,m::Vec3 position) noexcept {
    return model_vector(position-origin.position,origin.yaw);
}
}
