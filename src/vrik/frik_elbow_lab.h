#pragma once
// Explicit FRIK elbow experiment, controlled by the live Experiments menu or
// saved VrFrikElbows preference. F7 selects observation only; even a retained
// legacy rung latch cannot silently change the solver while recording.
//
// What is ported from FRIK (src/skeleton/Skeleton.cpp, arm IK) - the IDEAS, not its Fallout nodes:
//  1 min flexion 10 deg at full reach: the arm is never solved straight (a straight arm is where the swivel loses its
//    reference and spins). FRIK stretches the bones there; we DON'T: lengths stay fixed, the shoulder accommodates
//    the admitted endpoint; the wrist remains at that controller/contact target.
//  2 max flexion 145 deg near the shoulder: the triangle is solved for at least reach(145 deg). FRIK shortens the upper arm
//    to keep the wrist; we keep lengths and move the shoulder into the feasible shell around the same wrist.
//  3 smooth blends (polynomial smoothMax/smoothMin, C1 across the switch) instead of hard thresholds in the torso clearance
//    pole search; its swivel window never closes below 15 deg.
//  4 swivel smoothing: retention 0.75 per 90 Hz frame, frame-rate independent (move = 1-0.75^(dt*90), dt <= 3 frames),
//    advanced once per pose sample, reset with the elbow history (epoch = skeleton/owner/load, 500 ms age).
//    D86 keeps that small-motion response but caps swivel speed at 2-4 rad/s; destination inference is shared.
//  5 a stable axis for the 180 deg case of from_to (arm axis reversal): rotate about the known bend/hinge normal instead
//    of an arbitrary world axis.
// Kept: fixed bone lengths, W1 pole 0.20/0.45 + hanging cap, controller wrist rotation, captured fist, two-hand grips,
// bow wrists and grips. D86 shares the anatomical destination; bow retains its established pole clock.
#include <atomic>
#include "elbow_continuity.h"
#include "elbow_r89.h"
namespace kcd2vr::frik_elbow_lab {
inline std::atomic<bool> enabled{false}; // Experiments preference; persisted as VrFrikElbows
inline std::atomic<bool> rung2{false};   // Retired compatibility storage, never a solver selector
// R90: the R89 hand-guided option owns the swivel when selected; FRIK trial is another choice of the same
// Experiments slot (D86 DEFAULT -> R89 HAND-GUIDED -> FRIK TRIAL), so both never run at once.
inline bool Active() noexcept {
    return enabled.load(std::memory_order_relaxed)&&!kcd2vr::elbow_r89::Active();
}
namespace pc=kcd2vr::pose_core;
constexpr double kPi=3.141592653589793;
constexpr double kMinFlexionDeg=10.0, kMaxFlexionDeg=145.0, kWindowMinDeg=15.0;
constexpr double kRetention=.75;        // per 90 Hz frame
constexpr double kDistanceBand=.02;     // m, smooth band of the flexion clamp
constexpr double kAmountBand=.10;       // FRIK kAmountBand
constexpr double kAngleBandDeg=9.0;     // FRIK kAngleBand

// FRIK smoothMax: max(a,b) outside a band of width k around a==b, C1 across it.
inline double smooth_max(double a,double b,double k) noexcept {
    if(!(k>0))return std::max(a,b);
    const double h=std::max(k-std::abs(a-b),0.0)/k;
    return std::max(a,b)+h*h*k*.25;
}
inline double smooth_min(double a,double b,double k) noexcept { return -smooth_max(-a,-b,k); }
// FRIK smoothingMoveFraction, frame-rate independent branch.
inline double move_fraction(double retentionAt90,double dtSeconds) noexcept {
    const double r=std::clamp(std::isfinite(retentionAt90)?retentionAt90:kRetention,0.0,.999);
    const double dt=std::isfinite(dtSeconds)?std::clamp(dtSeconds,0.0,3.0/90.0):0.0;
    return 1.0-std::pow(r,dt*90.0);
}
// shoulder->wrist distance of a fixed-length arm with the elbow flexed by flexDeg (0 = straight).
inline double reach_at_flexion(double upper,double fore,double flexDeg) noexcept {
    const double c=std::cos(flexDeg*kPi/180.0);
    return std::sqrt(std::max(0.0,upper*upper+fore*fore+2*upper*fore*c));
}
struct TriangleClamp { double distance=0,reserve=0; bool limited=false; };
// Solve distance for a controller at d: never closer than reach(145) (pushed OUT along the same line), never further
// than reach(10) (the shortfall stays the reach cap). Both ends blend with smooth_max/smooth_min (C1). reserve is the
// solve_arm parameter that makes its own upper clamp equal reach(10).
inline TriangleClamp clamp_triangle(double d,double upper,double fore,
        double minFlexDeg=kMinFlexionDeg,double maxFlexDeg=kMaxFlexionDeg,double band=kDistanceBand) noexcept {
    TriangleClamp r;
    if(!std::isfinite(d)||!(upper>0)||!(fore>0))return r;
    const double rMin=reach_at_flexion(upper,fore,minFlexDeg), rMax=reach_at_flexion(upper,fore,maxFlexDeg);
    r.reserve=upper+fore-rMin;
    double x=smooth_max(d,rMax,band);
    x=smooth_min(x,rMin,band);
    // smooth_min may dip k/4 under rMin's neighbourhood but never above it; keep strictly inside the solvable range.
    r.distance=std::clamp(x,std::abs(upper-fore)+2e-6,rMin);
    r.limited=std::abs(r.distance-d)>1e-6;
    return r;
}
// from_to with a stable 180 deg axis: for opposite vectors rotate about the hint (made perpendicular to a), not about a
// world axis that has nothing to do with the arm.
inline pc::Quat from_to_stable(pc::Vec3 a,pc::Vec3 b,pc::Vec3 hint) {
    return pc::arm_from_to(a,b,hint);
}
inline std::optional<pc::Vec3> transported_bend_stable(const pc::ElbowContinuity& s,pc::Vec3 axis) {
    return pc::unit(pc::perpendicular(pc::rotate(from_to_stable(s.axis,axis,s.bend),s.bend),axis));
}
// continuous_elbow with the FRIK swivel step. Everything else is the production function line for line: the same
// anchor/conditioning, the same history rules (epoch, 500 ms, monotonic sample/tick), the same duplicate-sample
// no-advance and caller staging. D86 bounds the fractional step with the established reach-dependent rate.
inline std::optional<pc::Vec3> continuous_elbow_lab(pc::ElbowContinuity& s,pc::Vec3 axis,
        pc::Vec3 anatomical,pc::Vec3 side_down,double reach,std::uint64_t epoch,
        std::uint64_t sample,std::uint64_t tick,std::uint64_t timeNs=0) {
    using namespace pc;
    const auto a=unit(axis);
    if(!a||!finite(anatomical)||!finite(side_down)||!std::isfinite(reach)||!sample||!tick)return std::nullopt;
    axis=*a;
    const auto neutral=unit(perpendicular(anatomical,axis));
    const auto down=unit(perpendicular(side_down,axis));
    const bool history=elbow_history_current(s,epoch,sample,tick);
    if(s.ready&&s.epoch==epoch&&(sample<s.sample||tick<s.tick))return std::nullopt;
    auto previous=history?transported_bend_stable(s,axis):std::optional<Vec3>{};
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
    const Vec3 legalPrevious=*previous, desired=*anchor;
    const double elapsed=history?motion_time::elapsed(s.tick,tick,s.timeNs,timeNs):0;
    if(history&&elapsed<0)return std::nullopt;
    const double change=std::acos(std::clamp(dot(legalPrevious,desired),-1.0,1.0));
    // D86: the recorded FRIK path stepped 43.867 deg in 11.047 ms. A
    // fraction of an almost-opposite destination is not a speed bound.
    // Retain its response for small changes and cap only pole swivel; wrists
    // and fixed arm lengths remain untouched.
    const double dt=history?std::clamp(elapsed,0.,.05):0.;
    const double rate=4.-2.*smooth01((reach-.85)/.15);
    const double step=std::min(change*move_fraction(kRetention,dt),rate*dt);
    const Vec3 bend=history?turn_pole(legalPrevious,desired,axis,step):desired;
    if(history&&sample==s.sample)return legalPrevious;
    s.axis=axis;s.bend=bend;s.epoch=epoch;s.sample=sample;s.tick=tick;s.timeNs=timeNs;s.ready=true;++s.updates;
    return bend;
}
// orient_upper_arm with the stable 180 deg swing (hint = the current hinge normal, perpendicular to the current axis).
inline std::optional<pc::Quat> orient_upper_arm_lab(pc::Quat current,pc::Vec3 current_axis,
        pc::Vec3 current_plane_normal,pc::Vec3 desired_axis,pc::Vec3 desired_plane_normal) {
    using namespace pc;
    auto q=unit(current); auto axis=unit(desired_axis);
    if(!q||!axis)return std::nullopt;
    auto swing=from_to_stable(current_axis,*axis,current_plane_normal);
    auto n0=unit(perpendicular(rotate(swing,current_plane_normal),*axis));
    auto n1=unit(perpendicular(desired_plane_normal,*axis));
    if(!n0||!n1)return std::nullopt;
    const double roll=std::atan2(dot(*axis,cross(*n0,*n1)),std::clamp(dot(*n0,*n1),-1.0,1.0));
    return unit(multiply(from_rotation_vector(*axis*roll),multiply(swing,*q)));
}
// Torso clearance window, smooth version of the R8b hard rules:
//  half window = 60 deg, W1 hanging cap 30 deg when the hand is >20 cm under the shoulder -> smooth over 15..25 cm,
//  never below 15 deg (FRIK twist window minimum). candidates outside the window are not removed but priced (soft wall).
inline double window_half_deg(double handHeight) noexcept {
    const double hang=pc::smooth01((-handHeight-.15)/.10);
    return std::max(kWindowMinDeg,60.0-30.0*hang);
}
inline double window_cost(double angleRad,double handHeight) noexcept {
    const double w=window_half_deg(handHeight)*kPi/180.0;
    const double over=smooth_max(std::abs(angleRad)-w,0.0,kAngleBandDeg*kPi/180.0);
    return 4.0*over*over; // 10 deg past the window ~ .12, comparable to the .15 angular preference
}
//  level reach (|hand height| < 10 cm: the elbow must not rise above the hand) -> weight smooth over 8..12 cm, priced
//  by how far the elbow rises (smooth_max, 1 cm band) instead of a hard skip.
inline double level_weight(double handHeight) noexcept { return 1.0-pc::smooth01((std::abs(handHeight)-.08)/.04); }
inline double level_cost(double elbowHeight,double handHeight) noexcept {
    const double rise=smooth_max(elbowHeight-(std::max(0.0,handHeight)+.002),0.0,.01);
    return level_weight(handHeight)*50.0*rise; // 2 cm rise ~ 1.0, dominates the angular preference like the old skip
}
}
