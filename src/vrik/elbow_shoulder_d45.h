#pragma once
// Explicit live anatomy experiment. The current controller/contact endpoint is immutable.
// Smooth the shoulder's small reach accommodation, then project ONLY that
// shoulder back into the fixed-length feasible shell for this exact endpoint.
#include "upper_body.h"
#include "arm_constraints.h"
namespace kcd2vr::elbow_shoulder_d45 {
namespace pc=kcd2vr::pose_core;
namespace fl=kcd2vr::frik_elbow_lab;
// Compatibility entry point; all production arm policies now share the exact
// finite-angle swing. D45 shoulder/pole policy remains an explicit experiment.
inline pc::Quat from_to(pc::Vec3 a,pc::Vec3 b,pc::Vec3 antipodeHint) {
    return pc::arm_from_to(a,b,antipodeHint);
}
inline std::optional<pc::Vec3> transport(const pc::ElbowContinuity& state,pc::Vec3 axis) {
    return pc::unit(pc::perpendicular(pc::rotate(from_to(state.axis,axis,state.bend),state.bend),axis));
}
inline std::optional<pc::Quat> orient_upper(pc::Quat current,pc::Vec3 currentAxis,
        pc::Vec3 currentPlane,pc::Vec3 desiredAxis,pc::Vec3 desiredPlane) {
    auto q=pc::unit(current);auto axis=pc::unit(desiredAxis);if(!q||!axis)return std::nullopt;
    const auto swing=from_to(currentAxis,*axis,currentPlane);
    const auto n0=pc::unit(pc::perpendicular(pc::rotate(swing,currentPlane),*axis));
    const auto n1=pc::unit(pc::perpendicular(desiredPlane,*axis));if(!n0||!n1)return std::nullopt;
    const double roll=std::atan2(pc::dot(*axis,pc::cross(*n0,*n1)),std::clamp(pc::dot(*n0,*n1),-1.0,1.0));
    return pc::unit(pc::multiply(pc::from_rotation_vector(*axis*roll),pc::multiply(swing,*q)));
}
struct ShoulderState {
    pc::Vec3 offsetChest{};
    std::uint64_t epoch=0,policy=0,sample=0,tick=0,timeNs=0,updates=0;
    bool ready=false;
};
inline double reserve(double upper,double fore) noexcept {
    return upper+fore-fl::reach_at_flexion(upper,fore,10.0);
}
inline std::optional<pc::ControllerShoulder103> shoulder(
        ShoulderState& state,pc::Vec3 natural,pc::Vec3 endpoint,pc::Quat chest,
        double upper,double fore,pc::Vec3 fallback,std::uint64_t epoch,
        std::uint64_t policy,std::uint64_t sample,std::uint64_t tick,std::uint64_t timeNs) {
    const auto q=pc::unit(chest);
    if(!q||!policy||!sample||!tick||!pc::finite(natural)||!pc::finite(endpoint)||
       !std::isfinite(upper)||!std::isfinite(fore)||upper<=0||fore<=0)return std::nullopt;
    const bool same=state.ready&&state.epoch==epoch&&state.policy==policy;
    if(same&&(sample<state.sample||tick<state.tick))return std::nullopt;
    const double dt=same?kcd2vr::motion_time::elapsed(state.tick,tick,state.timeNs,timeNs):0;
    if(same&&dt<0)return std::nullopt;
    const bool history=same&&dt<=.5;
    const double keep=reserve(upper,fore);
    const double lo=pc::minimum_arm_reach103(upper,fore),hi=upper+fore-keep-.0001;
    // 12 mm C1 transition before a reach boundary, rather than a hard change
    // from the natural shoulder to a moving root. Maximum interior bias 3 mm.
    const double d=pc::length(endpoint-natural);
    const double wanted=fl::smooth_min(fl::smooth_max(d,lo,.012),hi,.012);
    const auto target=pc::controller_shoulder103(natural,endpoint,upper,fore,fallback,wanted,keep);
    if(!target)return std::nullopt;
    const auto toChest=pc::conjugate(*q);
    const pc::Vec3 desired=pc::rotate(toChest,target->shoulder-natural);
    pc::Vec3 offset=desired;
    if(history){
        // Chest-local offset follows the body instantly. Only anatomical
        // participation relaxes; there is no world-space shoulder/hand lag.
        const double blend=sample==state.sample?0.:-std::expm1(-std::min(dt,.05)/.055);
        offset=state.offsetChest+(desired-state.offsetChest)*blend;
    }
    const auto allowed=pc::controller_shoulder103(natural+pc::rotate(*q,offset),endpoint,
        upper,fore,fallback,-1.,keep);
    if(!allowed)return std::nullopt;
    if(!history||sample!=state.sample){
        // Do not feed the instantaneous feasibility projection back into the
        // temporal preference: contact can move the endpoint in this very frame.
        state.offsetChest=offset;state.epoch=epoch;state.policy=policy;
        state.sample=sample;state.tick=tick;state.timeNs=timeNs;state.ready=true;++state.updates;
    }
    return allowed;
}
inline std::optional<pc::Vec3> elbow(pc::ElbowContinuity& state,pc::Vec3 axis,
        pc::Vec3 anatomical,pc::Vec3 fallback,double reach,std::uint64_t epoch,
        std::uint64_t sample,std::uint64_t tick,std::uint64_t timeNs) {
    const auto a=pc::unit(axis);if(!a)return std::nullopt;
    const bool history=pc::elbow_history_current(state,epoch,sample,tick);
    auto staged=state;
    if(history){
        // Preserve the existing rate-limited anatomical destination; improve
        // only transport at an axis reversal, where a world-axis choice flips.
        const auto prior=transport(state,*a);
        if(!prior)return std::nullopt;
        staged.axis=*a;staged.bend=*prior;
    }
    const auto result=pc::continuous_elbow(staged,*a,anatomical,fallback,reach,epoch,sample,tick,timeNs);
    if(result&&!(history&&sample==state.sample))state=staged;
    return result;
}
}
