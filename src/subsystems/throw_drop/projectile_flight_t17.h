#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>

// Dedicated throwing ammunition only. The native arrow owns gravity, sweeps,
// collision, damage and embedding. This policy returns a velocity correction;
// it never advances position, chooses a victim, awards damage or adds gravity.
namespace kcd2vr::throw_projectile_t17 {
inline constexpr float kEffectiveRange=20.f;
inline constexpr float kRangeTransition=1.f;
inline constexpr float kTailDragPerSecond=8.f;
inline constexpr float kMaximumStepSeconds=.05f;
inline constexpr float kMaximumSpeed=24.f;
inline constexpr float kMinimumThrowSpeed=1.5f;
struct Vec3 {float x=0,y=0,z=0;};
inline bool finite(Vec3 v) noexcept {
    return std::isfinite(v.x)&&std::isfinite(v.y)&&std::isfinite(v.z);
}
struct Flight {
    Vec3 origin{};
    float furthestHorizontal=0,intervalFrom=0;
    bool started=false,priorKnown=false,intervalKnown=false;
    bool begin(Vec3 p) noexcept {
        *this={};if(!finite(p))return false;origin=p;started=priorKnown=true;return true;
    }
    // Tracking/status/time gaps discard only interval authority. The original
    // release origin and furthest range budget never reset or gain time.
    void breakInterval() noexcept {priorKnown=intervalKnown=false;intervalFrom=furthestHorizontal;}
    // Range is horizontal displacement from release, not total arc length.
    // Latch the furthest point: a rebound cannot recover a fresh 20 m budget.
    bool observe(Vec3 p) noexcept {
        if(!started||!finite(p)){breakInterval();return false;}
        const double x=double(p.x)-origin.x,y=double(p.y)-origin.y;
        const double r=std::hypot(x,y);
        if(!std::isfinite(r)||r>1000){breakInterval();return false;}
        intervalFrom=furthestHorizontal;furthestHorizontal=std::max(furthestHorizontal,float(r));
        intervalKnown=priorKnown;priorKnown=true;return true;
    }
    bool tailVelocity(Vec3 measured,float seconds,Vec3& corrected) const noexcept {
        corrected=measured;
        if(!started||!intervalKnown||!finite(measured)||!std::isfinite(seconds)||seconds<=0||
           seconds>kMaximumStepSeconds||furthestHorizontal<=kEffectiveRange)return false;
        // Smoothly engage over the next metre. Damping horizontal / upward
        // velocity, while preserving native downward velocity, gives a steeper
        // tail without a wall, teleport, second gravity or an upward brake.
        const auto integral=[](double range) noexcept {
            const double u=(range-kEffectiveRange)/kRangeTransition;
            if(u<=0)return 0.;if(u>=1)return kRangeTransition*(u-.5);
            return kRangeTransition*(u*u*u-.5*u*u*u*u);
        };
        const double span=double(furthestHorizontal)-intervalFrom;
        double weight=0;
        if(span>0)weight=(integral(furthestHorizontal)-integral(intervalFrom))/span;
        else{const double u=std::clamp((double(furthestHorizontal)-kEffectiveRange)/kRangeTransition,0.,1.);weight=u*u*(3-2*u);}
        // Average the native-observed monotonic range segment, including its
        // portion below20m. Never charge the entire step at its endpoint weight.
        const float decay=std::exp(-kTailDragPerSecond*float(std::clamp(weight,0.,1.))*seconds);
        corrected.x*=decay;corrected.y*=decay;
        if(corrected.z>0)corrected.z*=decay;
        return finite(corrected);
    }
};
// Preserve the player's measured release direction, including either hand.
// The native launch adapter must supply the speed scale; no target steering.
inline bool launchVelocity(Vec3 measured,float gain,Vec3& velocity) noexcept {
    velocity={};
    if(!finite(measured)||!std::isfinite(gain)||gain<=0||gain>4)return false;
    const double speed=std::hypot(std::hypot(double(measured.x),double(measured.y)),double(measured.z));
    if(!std::isfinite(speed)||speed<kMinimumThrowSpeed)return false;
    const double scale=std::min(double(gain),double(kMaximumSpeed)/speed);
    velocity={float(measured.x*scale),float(measured.y*scale),float(measured.z*scale)};
    return finite(velocity);
}
}
