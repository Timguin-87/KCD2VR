#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>

// Mod policy, not a claim about retail metres/second or RPG balance. Native
// TimeAlignment retains its own min/max bias, recovery and opponent response.
namespace kcd2vr::attack_timing178r22c2 {
enum class Kind : unsigned { None, Slash, Thrust, Punch };
struct Intent {
    std::uintptr_t player=0,actor=0,state=0,manager=0;
    std::uint64_t serial=0,policy=0,epoch=0,request=0,sample=0,tick=0,key=0,generation=0;
    Kind kind=Kind::None;int side=0;double speed=0;
    std::uint64_t gesture59c3=0;
    std::uint64_t recovery54=0;
};
inline bool fresh(std::uint64_t tick,std::uint64_t now) noexcept {
    return tick&&now>=tick&&now-tick<=100;
}
inline float multiplier(Kind kind,double speed) noexcept {
    if(kind==Kind::None||!std::isfinite(speed)||speed<.35||speed>30)return 1.f;
    const double reference=kind==Kind::Slash?4.5:2.5;
    // Bounded, continuous and monotonic; no wrist-jitter threshold is weakened.
    return float(std::clamp(std::sqrt(speed/reference),.75,1.6));
}
inline bool eligible(const Intent& i,std::uint64_t now,std::uint64_t policy,
                     std::uint64_t epoch,std::uint64_t consumed) noexcept {
    return i.serial&&i.serial>consumed&&i.policy==policy&&i.epoch==epoch&&i.player&&i.actor&&i.state&&
        i.manager&&i.sample&&fresh(i.tick,now)&&i.kind!=Kind::None&&i.side>=0&&i.side<2&&
        std::isfinite(i.speed)&&i.speed>=.35&&i.speed<=30;
}
inline float apply(float native,float scale) noexcept {
    if(!std::isfinite(native)||native<.1f||native>8.f||!std::isfinite(scale)||scale<.75f||scale>1.6f)return native;
    return native*scale;
}
}
