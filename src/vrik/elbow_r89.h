#pragma once
// R89 elbow (Claude, 3 Nov 2026). Miki: "elbows have never been good" - flicker when pulling the bow at the face /
// reaching over the shoulder, strange rotation with the hands up, and no elbow setting made it better.
//
// why every older elbow failed (full table in REVIEW89_elbow.md):
//  - all of them took the pole from the BODY only (fixed down/back/out vector, torso clearance search, native
//    animation elbow). a body pole is singular where the arm points along it (hand in front of the face /
//    hand up and back over the shoulder), so a few mm of hand motion swung the target around the arm.
//  - each fix added one more filter or branch rule on that ill-conditioned target (H15 conditioning, D45/D67
//    continuous search, D69 hysteresis, D85Rx outward floor) instead of a better input.
//  - the FRIK lab swivel (frik_elbow_lab) has no rate cap: 25 % of the gap per frame, so a target jump became a
//    20-45 deg/frame swing (F7 3 Nov: 112 jumps >15 deg on 43 s of raised-hand time).
//  - policy/context changes (bow, native capture, handover, F2/F7 comparison rungs) reset the elbow history and
//    snapped the elbow 10-29 cm in one frame.
//  - the forearm ignored the displayed hand: wrist bend p95 135 deg with hands up = "elbows rotate strangely".
//
// this solver (one function for every arm, bow included, no context switch):
//  target = sum of UNNORMALISED parts perpendicular to the shoulder->wrist axis
//      body    (W1 side-down + .6 backward; bow arms: their archer/torso preference)   weight .75
//      -hand forward (the elbow sits behind the wrist along the displayed hand)        weight 1
//      -thumb  (the elbow sits on the little-finger side)                               weight .5
//      + torso push when that elbow would be inside the chest capsule.
//  every part fades out by itself where it is ambiguous, so the sum is only weak where ALL parts are weak
//  (no hard thresholds, no branch choice). weak sum = low confidence = the swivel holds still instead of spinning.
//  continuity in angle space: the committed bend is carried along the arm, then turned toward the target with
//  tau 60 ms and at most 5 rad/s (<= 3.2 deg per 90 Hz frame). duplicate samples never advance it.
//  cold start (no history) begins at the current native elbow, so leaving a native/other owner never snaps.
// fixed bone lengths, wrist position and hand rotation are untouched (pole only).
#include <atomic>
#include <optional>
#include "elbow_continuity.h"
namespace kcd2vr::elbow_r89 {
namespace pc=kcd2vr::pose_core;
inline std::atomic<bool> enabled{false}; // R90: option, INI VrElbowSolverR89 (default false = R2's D86 destination); Experiments > Elbow preference "R89 HAND-GUIDED"
inline bool Active() noexcept { return enabled.load(std::memory_order_relaxed); }
constexpr double kBody=.75,kForward=1.0,kThumb=.5;      // replay-tuned (VRIK-like weights)
constexpr double kTorso=2.0,kTorsoMargin=.05,kTorsoBand=.08;
constexpr double kTau=.06,kRate=5.0,kConfidence=.35;
struct Input {
    pc::Vec3 shoulder{},wrist{};
    double upper=0,fore=0;
    pc::Vec3 body{};                 // body pole (any length)
    pc::Vec3 forward{},thumb{};      // world directions of the displayed hand; zero = not known
    pc::Vec3 torsoA{},torsoB{};double torsoRadius=0;
    std::optional<pc::Vec3> seedElbow{}; // current native elbow position, cold start only
};
// R94: copied only when bounded F7 capture asks for it. Values come from the
// actual selected solve, including seed and confidence, never inferred axes.
enum class ResetR94 : unsigned {None,Cold,Epoch,Expired,Duplicate,Invalid,Backwards,NoTarget};
struct TraceR94 {
    pc::Vec3 axis{},previous{},target{},output{};
    double strength=0,confidence=0,dt=0,change=0,step=0;
    ResetR94 reset=ResetR94::None;
    bool previousValid=false,targetValid=false,seedUsed=false,rateClamped=false,timeClamped=false,ok=false;
};
inline std::optional<pc::Vec3> target(const Input& in,pc::Vec3 axis,double& strength) {
    using namespace pc;
    strength=0;
    Vec3 b{};
    if(const auto body=unit(in.body))b=b+perpendicular(*body,axis)*kBody;
    if(const auto f=unit(in.forward))b=b-perpendicular(*f,axis)*kForward;
    if(const auto t=unit(in.thumb))b=b-perpendicular(*t,axis)*kThumb;
    const double m=length(b);
    if(!(m>1e-9)||!finite(b))return std::nullopt;
    if(in.torsoRadius>0&&std::isfinite(in.torsoRadius)&&finite(in.torsoA)&&finite(in.torsoB))
        if(const auto arm=solve_arm(in.shoulder,in.wrist,in.upper,in.fore,b,b)){
            const Vec3 ab=in.torsoB-in.torsoA;const double d2=dot(ab,ab);
            const double t=d2>1e-12?std::clamp(dot(arm->elbow-in.torsoA,ab)/d2,0.0,1.0):0.0;
            const Vec3 away=arm->elbow-(in.torsoA+ab*t);
            if(const auto push=unit(perpendicular(away,axis)))
                b=b+*push*(kTorso*m*smooth01((in.torsoRadius+kTorsoMargin-length(away))/kTorsoBand));
        }
    strength=length(b);
    return unit(b);
}
inline std::optional<pc::Vec3> elbow(pc::ElbowContinuity& s,const Input& in,std::uint64_t epoch,
        std::uint64_t sample,std::uint64_t tick,std::uint64_t timeNs,TraceR94* trace=nullptr) {
    using namespace pc;
    if(trace)*trace={};
    const auto a=unit(in.wrist-in.shoulder);
    if(!a||!finite(in.shoulder)||!finite(in.wrist)||!finite(in.body)||!std::isfinite(in.upper)||!std::isfinite(in.fore)||
       in.upper<=0||in.fore<=0||!sample||!tick){if(trace)trace->reset=ResetR94::Invalid;return std::nullopt;}
    if(s.ready&&s.epoch==epoch&&(sample<s.sample||tick<s.tick)){if(trace)trace->reset=ResetR94::Backwards;return std::nullopt;}
    const bool history=elbow_history_current(s,epoch,sample,tick);
    auto previous=history?transported_elbow_bend(s,*a):std::optional<Vec3>{};
    if(trace){trace->axis=*a;trace->reset=history?ResetR94::None:!s.ready?ResetR94::Cold:s.epoch!=epoch?ResetR94::Epoch:ResetR94::Expired;}
    if(history&&sample==s.sample&&previous){
        if(trace){trace->reset=ResetR94::Duplicate;trace->previous=trace->output=*previous;trace->previousValid=trace->ok=true;}
        return previous; // duplicate consumer: no temporal advance
    }
    double dt=0;
    if(history){
        const double e=motion_time::elapsed(s.tick,tick,s.timeNs,timeNs);
        if(e<0){if(trace)trace->reset=ResetR94::Backwards;return std::nullopt;}
        dt=std::min(.05,e);
        if(trace)trace->timeClamped=e>.05;
    }
    if(!previous&&in.seedElbow&&finite(*in.seedElbow)){
        previous=unit(perpendicular(*in.seedElbow-in.shoulder,*a));if(trace)trace->seedUsed=bool(previous);
    }
    double strength=0;
    auto wanted=target(in,*a,strength);
    if(!wanted)wanted=previous;
    if(!wanted)wanted=unit(perpendicular(in.body,*a));
    if(!wanted){if(trace)trace->reset=ResetR94::NoTarget;return std::nullopt;}
    if(trace){trace->dt=dt;trace->strength=strength;trace->confidence=smooth01(strength/kConfidence);
        trace->target=*wanted;trace->targetValid=true;trace->previousValid=bool(previous);if(previous)trace->previous=*previous;}
    Vec3 bend=*wanted;
    if(previous){
        const double change=std::acos(std::clamp(dot(*previous,*wanted),-1.0,1.0));
        const double step=std::min(change*(-std::expm1(-dt/kTau)),kRate*dt)*smooth01(strength/kConfidence);
        if(trace){trace->change=change;trace->step=step;trace->rateClamped=change*(-std::expm1(-dt/kTau))>kRate*dt;}
        bend=turn_pole(*previous,*wanted,*a,step);
    }
    s.axis=*a;s.bend=bend;s.epoch=epoch;s.sample=sample;s.tick=tick;s.timeNs=timeNs;s.ready=true;++s.updates;
    if(trace){trace->output=bend;trace->ok=true;}
    return bend;
}
}
