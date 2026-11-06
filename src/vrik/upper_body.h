#pragma once
// V66.103: inexpensive joint-limited shoulder girdle. This is a tracking
// reconstruction, not an estimate of measured scapular/upper-arm orientation.
#include "pose_math.h"
#include "motion_time.h"
namespace kcd2vr::pose_core {
struct ChestAxes103 {Vec3 right{},forward{},up{};};
inline std::optional<ChestAxes103> chest_axes103(Vec3 right,Vec3 up) {
    auto u=unit(up);if(!u)return std::nullopt;
    auto r=unit(right-*u*dot(right,*u));if(!r)return std::nullopt;
    auto f=unit(cross(*u,*r));if(!f)return std::nullopt;
    return ChestAxes103{*r,*f,*u};
}
struct ShoulderAngles103 {double elevation=0,protraction=0;};
struct ShoulderState103 {ShoulderAngles103 angle{};std::uint64_t epoch=0,sample=0,tick=0,timeNs=0;bool ready=false;};
inline double smooth_range103(double a,double b,double value) {
    const double t=std::clamp((value-a)/(b-a),0.0,1.0);return t*t*(3-2*t);
}
inline ShoulderAngles103 shoulder_goal103(const ChestAxes103& body,int side,Vec3 shoulder,Vec3 hand,double length) {
    const Vec3 v=hand-shoulder;const double d=pose_core::length(v);const auto direction=unit(v);
    if(!direction||!std::isfinite(length)||length<=0)return {};
    const double sign=side?-1.0:1.0;
    const double high=dot(*direction,body.up),forward=dot(*direction,body.forward),across=-sign*dot(*direction,body.right);
    const double reach=d/length;
    constexpr double rad=3.14159265358979323846/180.0;
    // Explicit axes prevent the clavicle flipping when a controller crosses it.
    // Elevation cannot turn into arbitrary axial twist; behind-body motion has
    // a small separate retraction allowance rather than a look-at quaternion.
    const double elevation=20*rad*smooth_range103(.15,.90,high)*smooth_range103(.35,.90,reach);
    const double protraction=12*rad*smooth_range103(.25,.95,forward)*smooth_range103(.50,1.02,reach)+
        6*rad*smooth_range103(.05,.70,across)*smooth_range103(.25,.75,reach)-
        6*rad*smooth_range103(.15,.70,-forward)*smooth_range103(.50,1.0,reach);
    return {elevation,std::clamp(protraction,-6*rad,16*rad)};
}
// A small fixed candidate set adjusts shoulder participation only when the
// preferred posture leaves a genuine endpoint reach error. No bone stretching,
// wrist remap, translation of the clavicle pivot, or arbitrary shoulder look-at.
inline ShoulderAngles103 shoulder_goal_reach104(const ChestAxes103& body,int side,
        Vec3 pivot,Vec3 shoulder,Vec3 hand,double armLength) {
    auto best=shoulder_goal103(body,side,shoulder,hand,armLength);
    if(!finite(pivot)||!finite(shoulder)||!finite(hand)||!std::isfinite(armLength)||armLength<=0)return best;
    const Vec3 lever=shoulder-pivot;const double sign=side?-1.0:1.0;
    const auto distance=[&](ShoulderAngles103 a){
        const auto q=multiply(from_rotation_vector(body.up*(sign*a.protraction)),from_rotation_vector(body.forward*(-sign*a.elevation)));
        return length(hand-(pivot+rotate(q,lever)));
    };
    const auto initial=best;double excess=std::max(0.0,distance(best)-armLength);
    if(excess<.005||length(lever)<.015||length(lever)>.35)return best;
    constexpr double rad=3.14159265358979323846/180.0;
    // At most 12 candidates. Prefer the smallest additional rotation among
    // equally reachable poses. Limits remain body-relative and bilateral.
    double bestCost=excess*excess;
    for(int e=0;e<=2;++e)for(int p=0;p<=3;++p){
        ShoulderAngles103 a{std::min(30*rad,initial.elevation+e*5*rad),std::min(30*rad,initial.protraction+p*5*rad)};
        const double residual=std::max(0.0,distance(a)-armLength);
        const double de=a.elevation-initial.elevation,dp=a.protraction-initial.protraction;
        const double cost=residual*residual+.0001*(de*de+dp*dp);
        if(cost+1e-8<bestCost){bestCost=cost;best=a;}
    }
    return best;
}
inline std::optional<Quat> shoulder_delta103(ShoulderState103& state,const ChestAxes103& body,int side,
        Vec3 shoulder,Vec3 hand,double length,std::uint64_t epoch,std::uint64_t sample,std::uint64_t tick,std::uint64_t timeNs=0,std::optional<Vec3> clavicle=std::nullopt) {
    if(!finite(hand)||!finite(shoulder)||!sample||!tick||side<0||side>1)return std::nullopt;
    const bool same=state.ready&&state.epoch==epoch;
    if(same&&sample<state.sample)return std::nullopt;
    const auto goal=clavicle?shoulder_goal_reach104(body,side,*clavicle,shoulder,hand,length):shoulder_goal103(body,side,shoulder,hand,length);
    const double dt=same?motion_time::elapsed(state.tick,tick,state.timeNs,timeNs):0;
    const bool history=same&&dt>=0&&dt<=.5;
    if(!history){state={};state.ready=true;state.angle=goal;state.epoch=epoch;}
    else if(sample!=state.sample){
        const double step=std::min(dt,.05),blend=-std::expm1(-step/.055);
        auto approach=[&](double old,double wanted){return old+std::clamp((wanted-old)*blend,-1.5*step,1.5*step);};
        state.angle.elevation=approach(state.angle.elevation,goal.elevation);
        state.angle.protraction=approach(state.angle.protraction,goal.protraction);
    }
    state.sample=sample;state.tick=tick;state.timeNs=timeNs;
    const double sign=side?-1.0:1.0;
    const Quat elevate=from_rotation_vector(body.forward*(-sign*state.angle.elevation));
    const Quat protract=from_rotation_vector(body.up*(sign*state.angle.protraction));
    return unit(multiply(protract,elevate));
}
// A 155-degree close-reach limit is a conservative rig policy, not a universal
// anatomical measurement. Straight-reach margin remains the existing 2 degrees.
inline double minimum_arm_reach103(double upper,double fore) {
    constexpr double angle=155*3.14159265358979323846/180.0;
    return std::sqrt(std::max(0.0,upper*upper+fore*fore+2*upper*fore*std::cos(angle)));
}
inline Vec3 limit_close_reach103(Vec3 shoulder,Vec3 hand,double upper,double fore) {
    const auto direction=unit(hand-shoulder);if(!direction)return hand;
    const double d=length(hand-shoulder),minimum=minimum_arm_reach103(upper,fore);
    return d<minimum?shoulder+*direction*minimum:hand;
}
// V66.124 soft reach knee, moved verbatim out of coherent_pose.inl (C38) so the executable test runs the production
// math: above 99.5% of Henry's arm the goal is compressed smoothly toward 99.95%, ALWAYS along shoulder->controller.
// An over-reach never drops, hides or re-aims the goal; solve_arm then holds the hand at full extension (hard limit).
inline Vec3 soft_reach_cap124(Vec3 shoulder,Vec3 goal,double arm) {
    const Vec3 d=goal-shoulder;const double len=length(d);
    if(!(len>1e-4)||!(arm>.1)||!std::isfinite(len))return goal;
    const double r=len/arm,knee=.995,cap=.9995;
    if(r<=knee)return goal;
    const double soft=knee+(cap-knee)*(1.0-std::exp(-(r-knee)/(cap-knee)));
    return shoulder+d*(soft/r);
}
// Body contact uses torso-derived shoulders captured before per-arm reach fit.
inline double torso_radius103(Vec3 rightShoulder,Vec3 leftShoulder) {
    return std::clamp(length(rightShoulder-leftShoulder)*.32,.08,.16);
}
// R38 controller-authoritative wrist. The endpoint has already been selected by
// tracking or its legitimate contact/native owner. Anatomy may move the shoulder
// root to fit it; it may never retarget that endpoint. Humerus/forearm lengths
// stay fixed. This is scapular/root accommodation, not arm-bone stretching.
struct ControllerShoulder103 { Vec3 shoulder{}; double distance=0; bool adjusted=false; };
inline std::optional<ControllerShoulder103> controller_shoulder103(
        Vec3 shoulder,Vec3 endpoint,double upper,double fore,Vec3 fallback,
        double desiredDistance=-1.0,double reserve=-1.0) {
    if(!finite(shoulder)||!finite(endpoint)||!finite(fallback)||
       !std::isfinite(upper)||!std::isfinite(fore)||upper<=0||fore<=0||
       !std::isfinite(desiredDistance)||desiredDistance < -1.0||
       !std::isfinite(reserve)||reserve < -1.0)return std::nullopt;
    if(reserve<0)reserve=upper+fore-std::sqrt(upper*upper+fore*fore+
        2*upper*fore*std::cos(2.0*3.141592653589793/180.0));
    // Keep a small float-composition margin inside solve_arm's limits. It is
    // applied to the shoulder, not subtracted from the controller target.
    // Retain the established 155-degree close-flexion policy, but move its
    // shoulder instead of pushing a close/coincident controller away.
    const double lo=std::max(std::abs(upper-fore)+.0001,minimum_arm_reach103(upper,fore)),hi=upper+fore-reserve-.0001;
    if(!(hi>lo))return std::nullopt;
    const Vec3 delta=endpoint-shoulder;const double d=length(delta);
    if(!std::isfinite(d))return std::nullopt;
    const double fit=std::clamp(desiredDistance>=0?desiredDistance:d,lo,hi);
    if(std::abs(fit-d)<1e-10)return ControllerShoulder103{shoulder,d,false};
    auto axis=unit(delta);if(!axis)axis=unit(fallback);
    if(!axis)return std::nullopt;
    return ControllerShoulder103{endpoint-*axis*fit,fit,true};
}

}
