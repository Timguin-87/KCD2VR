#pragma once
#include <algorithm>
#include <cmath>
namespace kcd2vr::impact_haptics133 {
struct Feedback { float primary=0.0f,support=0.0f; int durationMs=0; };
// Presentation-only impact cue. Inventory weight remains a tuning proxy, never a
// claim about the weapon's true rigid-body mass. Native KCD2 owns damage/material
// acceptance; this function only maps an already-accepted physical contact speed
// to bounded controller feedback.
inline Feedback map(double speed,double weight,bool twoHandSupport) noexcept {
    Feedback f{};
    if(!std::isfinite(speed)||speed<=0.25)return f;
    const double w=std::isfinite(weight)?std::clamp(weight,0.15,8.0):1.0;
    const double massScale=std::clamp(std::sqrt(w/2.0),0.65,1.80);
    f.primary=float(std::clamp(0.08+speed*0.055*massScale,0.08,0.55));
    if(twoHandSupport)f.support=float(std::clamp(double(f.primary)*0.68,0.06,0.38));
    f.durationMs=std::clamp(int(std::lround(18.0+speed*5.0+w*1.4)),22,45);
    return f;
}
}
