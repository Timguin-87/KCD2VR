#pragma once
// H18 VRIK elbow lane (0092): bounded pole-continuity policy. Pure pose-pipeline
// math, host-testable, no engine ABI and no native calls. The caller owns all
// state: it supplies the previous committed pole and the current raw pole.
//
// Contract:
//  * condition_pole blends previous -> raw inside the plane perpendicular to the
//    reach axis, clamped to max_step_radians per call (default 40 degrees). The
//    blend is an in-plane rotation, never a lerp through the axis: no length
//    loss, no cancellation at antipodal inputs.
//  * Neutral rearm: when the hand moved farther than rearm_distance since the
//    previous pole's frame, the previous pole is stale geometry. bounded_pole
//    then returns the raw pole unchanged with rearmed=true so the caller reseeds
//    continuity instead of dragging a stale pole into a new posture.
//  * Fail closed: any non-finite vector or scalar, a zero reach axis, a max step
//    outside [0, pi], or a pole parallel to the reach axis returns nullopt.
//
// Relation to the production pipeline: coherent_pose feeds the raw per-frame
// torso-clearance pole through condition_elbow_preference (H15) and
// continuous_elbow (V66.97/V66.99, a time-based bounded turn with a 4 rad/s
// rate cap) in src/vrik/elbow_continuity.h. This header is the standalone
// per-call bound; it is intentionally NOT wired into that chain at H18 because
// the existing filter already applies a tighter bound (<= 0.2 rad per commit)
// and adding a second series filter would be redundant.
#include <algorithm>
#include <cmath>
#include <optional>
#include "pose_math.h"
namespace kcd2vr::elbow60 {
namespace pc = kcd2vr::pose_core;
inline constexpr double kPi = 3.14159265358979323846;
inline constexpr double kDefaultMaxStepRadians = kPi * 40.0 / 180.0; // 40 degrees per call

// Angle-clamped blend of the previous pole toward the raw pole, inside the
// plane perpendicular to the reach axis. Output is unit length and
// perpendicular to the axis. Nullopt fails closed.
inline std::optional<pc::Vec3> condition_pole(pc::Vec3 previous_pole, pc::Vec3 raw_pole,
        pc::Vec3 reach_axis, double max_step_radians = kDefaultMaxStepRadians) {
    if(!pc::finite(previous_pole)||!pc::finite(raw_pole)||!pc::finite(reach_axis)||
       !std::isfinite(max_step_radians)||max_step_radians<0.0||max_step_radians>kPi)
        return std::nullopt;
    const auto axis=pc::unit(reach_axis);
    if(!axis) return std::nullopt;
    const pc::Vec3 p0=previous_pole-*axis*pc::dot(previous_pole,*axis);
    const pc::Vec3 p1=raw_pole-*axis*pc::dot(raw_pole,*axis);
    const auto from=pc::unit(p0), to=pc::unit(p1);
    if(!from||!to) return std::nullopt; // pole on the reach axis: ill-conditioned
    const double full=std::atan2(pc::dot(*axis,pc::cross(*from,*to)),std::clamp(pc::dot(*from,*to),-1.0,1.0));
    const double step=std::clamp(full,-max_step_radians,max_step_radians);
    return pc::unit(*from*std::cos(step)+pc::cross(*axis,*from)*std::sin(step));
}

// Neutral-rearm predicate: the hand moved far enough that the previous pole's
// frame is stale. Non-finite inputs are NOT "moved far"; bounded_pole refuses
// them first.
inline bool hand_moved_far(pc::Vec3 previous_hand, pc::Vec3 hand, double rearm_distance) {
    if(!pc::finite(previous_hand)||!pc::finite(hand)||!std::isfinite(rearm_distance)||rearm_distance<0.0)
        return false;
    return pc::length(hand-previous_hand)>rearm_distance;
}

struct BoundedPole { pc::Vec3 pole{}; bool rearmed=false; };

// One-call bounded pole continuity: finite-input refusal, neutral rearm on a far
// hand move (returns the raw pole unchanged), otherwise the angle-clamped blend.
inline std::optional<BoundedPole> bounded_pole(pc::Vec3 previous_pole, pc::Vec3 raw_pole,
        pc::Vec3 reach_axis, pc::Vec3 previous_hand, pc::Vec3 hand, double rearm_distance,
        double max_step_radians = kDefaultMaxStepRadians) {
    if(!pc::finite(previous_pole)||!pc::finite(raw_pole)||!pc::finite(reach_axis)||
       !pc::finite(previous_hand)||!pc::finite(hand)||
       !std::isfinite(rearm_distance)||rearm_distance<0.0||
       !std::isfinite(max_step_radians)||max_step_radians<0.0||max_step_radians>kPi)
        return std::nullopt;
    const auto axis=pc::unit(reach_axis);
    if(!axis) return std::nullopt;
    const auto raw=pc::unit(raw_pole-*axis*pc::dot(raw_pole,*axis));
    if(!raw) return std::nullopt;
    if(pc::length(hand-previous_hand)>rearm_distance) return BoundedPole{*raw,true};
    const auto blended=condition_pole(previous_pole,raw_pole,reach_axis,max_step_radians);
    if(!blended) return std::nullopt;
    return BoundedPole{*blended,false};
}
} // namespace kcd2vr::elbow60
