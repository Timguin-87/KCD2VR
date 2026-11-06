#pragma once
// R59.16C10R6 - the user's damage rule (13 Oct, final): the mod never invents damage. It only SCALES the game's own
// normal swing by swing energy and two-handed grip, within 30..120 % of a normal swing, so weapons, armour, skills and
// perks keep mattering. Every bonus/penalty (two-handed, riposte window, repeated hits) is applied first; this bounds
// the result. Pure function, host-tested (strike_power59c10r6_test).
#include <algorithm>
#include <cmath>
namespace kcd2vr::strike_power59c10r6 {
inline constexpr float kRuleMin=.30f,kRuleMax=1.20f;
inline float Final(float raw,float minPower,float maxPower) noexcept {
    const float lo=std::isfinite(minPower)?std::clamp(minPower,kRuleMin,kRuleMax):kRuleMin;
    const float hi=std::isfinite(maxPower)?std::clamp(maxPower,lo,kRuleMax):kRuleMax;
    if(!std::isfinite(raw))raw=1.f;
    return std::clamp(raw,lo,hi);
}
}
