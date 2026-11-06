#pragma once
#include "pose_math.h"
#include <array>
namespace kcd2vr::holster_body_d70 {
// Acquisition zones only. Keep the existing wrist/torso/combat solve untouched.
// The fixed rest-body offset follows calibrated HMD translation and yaw; native
// lean, animated chest pitch and reaching-arm accommodation cannot move it.
inline bool shoulders(pose_core::Vec3 restHead,pose_core::Vec3 restRight,
    pose_core::Vec3 restLeft,pose_core::Vec3 trackedHead,pose_core::Quat headDelta,
    std::array<pose_core::Vec3,2>& out) noexcept {
    namespace m=pose_core;
    if(!m::finite(restHead)||!m::finite(restRight)||!m::finite(restLeft)||!m::finite(trackedHead))return false;
    const auto q=m::unit(headDelta);if(!q)return false;
    const auto yaw=m::unit(m::Quat{0,0,q->z,q->w});if(!yaw)return false;
    const double span=m::length(restRight-restLeft);
    if(span<.15||span>.8||m::length(restRight-restHead)>.8||m::length(restLeft-restHead)>.8)return false;
    out={trackedHead+m::rotate(*yaw,restRight-restHead),trackedHead+m::rotate(*yaw,restLeft-restHead)};
    return m::finite(out[0])&&m::finite(out[1]);
}
}
