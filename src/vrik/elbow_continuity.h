#pragma once
// V66.97. Geometry-only elbow continuity. Never reads animation or tracking APIs.
// The caller stages this value until native restoration and accepted paired publication.
#include "pose_math.h"
#include "motion_time.h"
namespace kcd2vr::pose_core {
struct ElbowContinuity {
    Vec3 axis{}, bend{};
    std::uint64_t epoch=0,sample=0,tick=0,updates=0,timeNs=0;
    bool ready=false;
    Vec3 destAxisD67C{},destinationD67C{};
    std::uint64_t destEpochD67C=0,destSampleD67C=0,destTickD67C=0,destNsD67C=0,alternativeSinceD67C=0;
    int alternativeSideD69=0;bool destinationReadyD67C=false;

};
inline Vec3 perpendicular(Vec3 v,Vec3 axis) { return v-axis*dot(v,axis); }
// D47: exact finite-angle arm swing. The generic pose helper has an identity
// deadband at dot > .999999 (~0.081 degrees) and a near-antipode snap. Those
// shortcuts leave visible sub-mm wrist steps when an arm is composed from
// fixed lengths. Keep them out of arms without changing legs, spine or camera.
inline Quat arm_from_to(Vec3 a,Vec3 b,Vec3 antipodeHint) {
    const auto ua=unit(a),ub=unit(b);if(!ua||!ub)return {};
    const double d=std::clamp(dot(*ua,*ub),-1.0,1.0);
    const auto c=cross(*ua,*ub);
    if(d>=0)return unit(Quat{c.x,c.y,c.z,1+d}).value_or(Quat{});
    // The common acute-angle path needs no additional square root.
    // Using sin(theta/2) here avoids the cancellation of 1+dot near pi.
    const double sine=length(c);
    if(sine>1e-12){
        const double sn=std::sqrt(std::max(0.0,(1-d)*.5));
        return unit(Quat{c.x*(sn/sine),c.y*(sn/sine),c.z*(sn/sine),sine/(2*sn)}).value_or(Quat{});
    }
    if(const auto hint=unit(perpendicular(antipodeHint,*ua)))return {hint->x,hint->y,hint->z,0};
    return from_to(a,b); // Exactly opposite and no usable anatomical hint.
}
inline double smooth01(double t) { t=std::clamp(t,0.0,1.0);return t*t*(3-2*t); }
inline bool elbow_history_current(const ElbowContinuity& s,std::uint64_t epoch,
        std::uint64_t sample,std::uint64_t tick) {
    return s.ready&&s.epoch==epoch&&tick>=s.tick&&tick-s.tick<=500&&sample>=s.sample;
}
inline std::optional<Vec3> transported_elbow_bend(const ElbowContinuity& s,Vec3 axis) {
    return unit(perpendicular(rotate(arm_from_to(s.axis,axis,s.bend),s.bend),axis));
}
struct ConditionedElbowPreference { Vec3 anatomical{},fallback{}; };
// A torso-clearance search returns a unit bend and thereby erases how close
// its original body-space preference was to the reach axis. Restore that
// conditioning before continuity sees it. A committed bend supplies geometric
// memory in the ambiguous cone; cold starts use the calibrated chest backward
// direction, independently of the mostly-down original preference.
inline std::optional<ConditionedElbowPreference> condition_elbow_preference(
        const ElbowContinuity& s,Vec3 axis,Vec3 preferred,Vec3 raw_preference,
        Vec3 body_backward,std::uint64_t epoch,std::uint64_t sample,std::uint64_t tick,
        Vec3 outward_floor={}) {
    // R5 (Miki 23 oct: "elbows corkscrewy" since H15). His 12:35-12:51 F7 record (29007 elbow samples) shows why:
    //  * H15 made the PREVIOUS committed bend the fallback destination and scaled the anatomical target by the raw-pole
    //    conditioning, so in the ill-conditioned cone (hand hanging, arm along side-down: 8 % right, 23 % left) the elbow
    //    steered toward its own history. continuous_elbow transports that history along the moving arm every frame, and a
    //    transported direction around a loop of arm motion comes back rotated - the corkscrew. committed bend vs torso
    //    target: p90 37 deg right / 76 deg left, p99 119 / 155 deg; ~9 % of frames the elbow pointed across the body.
    //  * the torso-clearance target itself sometimes picks the forward hemisphere for a low hand (80+ flips > 60 deg/side).
    // R5: the destination is always anatomical, never history. default pole = side-down + 0.6 x body backward (down, out,
    // back), projected off the arm. the torso-clearance target is used only on the same side of the arm as that default;
    // more than 60 deg away from it (or on the opposite side) it is replaced by the default (replay: 0.5 cut p99 145->58 right). history stays only what it should be: the rate-limited start
    // point inside continuous_elbow (smooth, never a target).
    const auto a=unit(axis);
    if(!a||!finite(preferred)||!finite(raw_preference)||!finite(body_backward)||!sample||!tick)return std::nullopt;
    if(s.ready&&s.epoch==epoch&&(sample<s.sample||tick<s.tick))return std::nullopt;
    auto anatomicalPlane=perpendicular(raw_preference+body_backward*.6,*a);
    // Raised arms can align with the opposite of the down/back preference.
    // Normalizing that nearly-zero plane turned millimetres into large swivel
    // changes. A calibrated outward component supplies a well-conditioned
    // anatomical direction; it is not the previous bend or a wrist filter.
    if(finite(outward_floor))if(const auto outward=unit(perpendicular(outward_floor,*a))){
        const double floor=length(outward_floor);
        anatomicalPlane=anatomicalPlane+*outward*std::max(0.,floor-dot(anatomicalPlane,*outward));
    }
    auto anatomicalDefault=unit(anatomicalPlane);
    if(!anatomicalDefault)anatomicalDefault=unit(perpendicular(body_backward,*a));
    if(!anatomicalDefault)anatomicalDefault=unit(perpendicular(raw_preference,*a));
    if(!anatomicalDefault)return std::nullopt;
    Vec3 target=preferred;
    if(const auto preferredPlane=unit(perpendicular(preferred,*a))){
        // D64: the narrow .50..55 cosine blend amplified the recorded ~3deg
        // preference changes into ~52deg targets. Preserve the exact preferred
        // vector within acos(.55), but fade to anatomy over the whole remaining
        // forward hemisphere. For unit plane preferences the resulting swivel
        // stays below60deg; an opposite hemisphere still chooses anatomy.
        // No history-as-destination, wrist filter, bone or temporal-policy change.
        const double weight=smooth01(dot(*preferredPlane,*anatomicalDefault)/.55);
        target=*anatomicalDefault*(1-weight)+preferred*weight;
    }else target=*anatomicalDefault;
    return ConditionedElbowPreference{target,*anatomicalDefault};
}
// Rotate within the arm's perpendicular plane, not by lerping opposite vectors.
inline Vec3 turn_pole(Vec3 from,Vec3 to,Vec3 axis,double max_angle) {
    const double a=std::atan2(dot(axis,cross(from,to)),std::clamp(dot(from,to),-1.0,1.0));
    const double step=std::clamp(a,-max_angle,max_angle);
    return *unit(from*std::cos(step)+cross(axis,from)*std::sin(step));
}
inline std::optional<Vec3> continuous_elbow(ElbowContinuity& s,Vec3 axis,
        Vec3 anatomical,Vec3 side_down,double reach,std::uint64_t epoch,
        std::uint64_t sample,std::uint64_t tick,std::uint64_t timeNs=0) {
    const auto a=unit(axis);
    if(!a||!finite(anatomical)||!finite(side_down)||!std::isfinite(reach)||!sample||!tick)return std::nullopt;
    axis=*a;
    const auto neutral=unit(perpendicular(anatomical,axis));
    const auto down=unit(perpendicular(side_down,axis));
    // Temporal state is a prior COMMITTED VR bend, never another animation frame.
    // 500ms is geometric-memory retention only. The caller's tracking and
    // weapon freshness checks remain 100ms; no stale pose/command is authorized.
    const bool history=elbow_history_current(s,epoch,sample,tick);
    if(s.ready&&s.epoch==epoch&&(sample<s.sample||tick<s.tick))return std::nullopt;
    auto previous=history?transported_elbow_bend(s,axis):std::optional<Vec3>{};
    // A neutral pole close to the reach axis is ill-conditioned. The second
    // calibrated body-space preference supplies a legal half-space, not new anatomy.
    const double conditioning=length(perpendicular(anatomical,axis));
    auto anchor=neutral;
    if(down) {
        const double w=smooth01((conditioning-.08)/.25);
        anchor=unit(*down*(1-w)+(anchor?*anchor:*down)*w);
        if(!anchor)anchor=down;
    }
    if(!anchor)anchor=previous;
    if(!anchor)return std::nullopt;
    if(!previous)previous=anchor;
    // V66.99: do not clamp the PREVIOUS committed pole against a moving
    // anchor before rate-limiting. That step snapped up to 100 degrees at an
    // anchor hemisphere crossing and even changed duplicate-sample output.
    // The anatomical anchor remains the destination; all correction is timed.
    Vec3 legalPrevious=*previous;
    Vec3 desired=*anchor;
    // Signed elbow directions are not equivalent quaternion hemispheres.
    // Recover toward the anatomical side even if committed history is opposite.
    const double straight=smooth01((reach-.85)/.15);
    // Exponential time constant plus an angular rate cap: no frame-count blend.
    // Filtering the POLE does not delay or change the controller wrist endpoint.
    const double elapsed=history?motion_time::elapsed(s.tick,tick,s.timeNs,timeNs):0;
    if(history&&elapsed<0)return std::nullopt;
    const double dt=history?std::min(.05,elapsed):0;
    const double tau=.065+.095*straight, rate=4.0-2.0*straight;
    double change=std::acos(std::clamp(dot(legalPrevious,desired),-1.0,1.0));
    const double step=std::min(change*(-std::expm1(-dt/tau)),rate*dt);
    Vec3 bend=history?turn_pole(legalPrevious,desired,axis,step):desired;
    // Duplicate eye/skinning consumption can reproject a committed bend for the
    // current shoulder geometry, but MUST NOT advance the temporal integrator.
    if(history&&sample==s.sample)return legalPrevious;
    s.axis=axis;s.bend=bend;s.epoch=epoch;s.sample=sample;s.tick=tick;s.timeNs=timeNs;s.ready=true;++s.updates;
    return bend;
}
// Aim fixes bone direction but does not fix axial roll. Align the upper-arm
// hinge plane as a second, independent constraint. This affects skin orientation,
// not the elbow/wrist target or either segment length. All vectors share a frame.
inline std::optional<Quat> orient_upper_arm(Quat current, Vec3 current_axis,
        Vec3 current_plane_normal, Vec3 desired_axis, Vec3 desired_plane_normal) {
    auto q=unit(current); auto axis=unit(desired_axis);
    if(!q||!axis)return std::nullopt;
    auto swing=arm_from_to(current_axis,*axis,current_plane_normal);
    auto n0=unit(perpendicular(rotate(swing,current_plane_normal),*axis));
    auto n1=unit(perpendicular(desired_plane_normal,*axis));
    if(!n0||!n1)return std::nullopt;
    const double roll=std::atan2(dot(*axis,cross(*n0,*n1)),std::clamp(dot(*n0,*n1),-1.0,1.0));
    return unit(multiply(from_rotation_vector(*axis*roll),multiply(swing,*q)));
}
struct ReachAssist {
    double excess=0; bool active=false;
    std::uint64_t epoch=0,sample=0;
};
// A 4mm activation / 1mm release band prevents shoulder/chest jitter. Above
// the band the established V66.95 accommodation magnitude remains unchanged.
inline double stable_reach_excess(ReachAssist& s,double excess,std::uint64_t epoch,std::uint64_t sample) {
    if(!std::isfinite(excess))return 0;
    if(s.epoch!=epoch)s={};
    if(s.sample==sample&&s.epoch==epoch)return s.excess;
    if(s.active){if(excess<=.001)s.active=false;}else if(excess>=.004)s.active=true;
    s.excess=s.active?std::max(0.0,excess):0;s.epoch=epoch;s.sample=sample;
    return s.excess;
}
}
