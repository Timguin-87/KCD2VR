#pragma once
#include "holster_body_d70.h"
#include "pose_math.h"
#include <array>
#include <cmath>
// D85 holster: back / over-shoulder holster zones on the PLAYER's body.
// Play-test 2 Nov: "I pull the bow from my chest".
// Cause: calibration maps the headset EYE point onto Henry's head BONE
// (requested hands = head anchor + frame*(xrHand - xrHeadAtCal), pose_tracking_goals.inl).
// D70 then hung Henry's rest head->upper arm offset (male.chr bind: +18.6 cm right,
// -1.9 cm forward, -16.8 cm down) from that point, i.e. from the player's EYES. The
// skull pivot that offset belongs to sits EyePivotForward/Up (default 9/8 cm) behind
// and below the eyes, the same pivot the torso stage already uses. So every shoulder
// zone was ~9 cm too far forward and ~8 cm too high on the player's body.
// D80 21:52:19.190: bow drawn with the left wrist at the old shoulder plane
// (0.3 cm behind it, 21 cm left of the right shoulder, 13 cm below it) = upper chest.
// Fix at the source: hang the same rest offset from the player's head pivot
// (headset eye point minus the calibrated eye offset turned with the head).
// The draw volume / radius rules are unchanged; only their anchor moves.
namespace kcd2vr::holster_body_d85 {
namespace m=pose_core;
// hmdEye: headset eye point in Henry model space, built exactly like the requested hands
// (head anchor + calibration frame * XR head motion), so hands and zones share one frame.
// headDelta: head rotation since calibration (model space). eyeOffsetNeutral: the calibrated
// eye offset (EyePivotForwardMeters*forward + EyePivotUpMeters*up) in the neutral chest axes.
inline bool pivot(m::Vec3 hmdEye,m::Quat headDelta,m::Vec3 eyeOffsetNeutral,m::Vec3& out) noexcept {
    if(!m::finite(hmdEye)||!m::finite(eyeOffsetNeutral)||m::length(eyeOffsetNeutral)>.25)return false;
    const auto q=m::unit(headDelta);if(!q)return false;
    out=hmdEye-m::rotate(*q,eyeOffsetNeutral);
    return m::finite(out);
}
inline bool shoulders(m::Vec3 restHead,m::Vec3 restRight,m::Vec3 restLeft,m::Vec3 hmdEye,
    m::Quat headDelta,m::Vec3 eyeOffsetNeutral,std::array<m::Vec3,2>& out) noexcept {
    m::Vec3 p{};if(!pivot(hmdEye,headDelta,eyeOffsetNeutral,p))return false;
    return holster_body_d70::shoulders(restHead,restRight,restLeft,p,headDelta,out);
}
}
