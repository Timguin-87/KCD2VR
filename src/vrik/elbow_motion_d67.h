#pragma once
#include "arm_constraints.h"
namespace kcd2vr::elbow_motion_d67 {
namespace pc=pose_core;
// INVESTIGATE_D67R1[ELBOW_POSTURE]: the default continuity repair fixes the
// reproduced target-branch jump, not every anatomical posture. The outward /
// backward reach pole below is still selected only by the elbow trial. Review
// free arms and boxing separately with the same shoulder/wrist endpoints and
// segment lengths; a smooth wrist does not prove a natural elbow. See the
// D67 motion audit and OPEN_SOURCE_POINTS.txt in repair_evidence/d67r1.
inline pc::Vec3 preference(pc::Vec3 shoulder,pc::Vec3 hand,double upper,double fore,
        pc::Vec3 raw,pc::Vec3 torsoA,pc::Vec3 torsoB,double radius,
        const pc::ElbowContinuity& history,std::uint64_t epoch,std::uint64_t sample,std::uint64_t tick,
        bool continuous,bool legacyLab) {
    std::optional<pc::Vec3> prior;
    if(continuous&&pc::elbow_history_current(history,epoch,sample,tick))
        if(const auto axis=pc::unit(hand-shoulder))prior=pc::transported_elbow_bend(history,*axis);
    return pc::torso_elbow_preference(shoulder,hand,upper,fore,raw,torsoA,torsoB,radius,prior,continuous,legacyLab);
}
// The historical .35-out/.25-back reach preference is tried only as the hand
// rises. Hanging arms retain the saved W1 pole, avoiding the old sideways elbow.
// This changes a pole destination, never a wrist, shoulder or anatomical length.
inline pc::Vec3 reachPole(pc::Vec3 saved,pc::Vec3 right,pc::Vec3 up,
        pc::Vec3 shoulder,pc::Vec3 hand,int side,double outward=.35,double backward=.25) {
    const auto r=pc::unit(right),u=pc::unit(up);if(!r||!u)return saved;
    const auto forward=pc::unit(pc::cross(*u,*r));if(!forward)return saved;
    const auto trial=pc::unit(*r*(side?-outward:outward)-*forward*backward-*u);
    if(!trial)return saved;
    const double raised=pc::smooth01((pc::dot(hand-shoulder,*u)+.25)/.30);
    return pc::unit(saved*(1-raised)+*trial*raised).value_or(saved);
}
}
