#pragma once
#include "hand_push_policy60.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
// Staged push-intent classifier, not implemented native NPC reactions.
// These labels describe requested outcomes for a future native provider:
//   - a firm SUSTAINED press -> WALK (she steps back away from the hand);
//   - a firm quick push     -> STUMBLE (the native stagger, faced to the push);
//   - a hard shove          -> KNOCKDOWN (the native ragdoll knock);
//   - a low sweep lifting both feet -> SWEEP (the knockdown via the legs).
// C52DF4 is a minor/major reaction-kind lookup within an already admitted hit;
// it cannot request Walk/Stumble/Knockdown/Sweep. No such provider is connected.
namespace kcd2vr::npc_push60 {
inline constexpr bool kNativeReactionProviderAvailable=false;
inline constexpr float kWalkSpeedMin=.5f,kStumbleSpeedMin=1.5f,kKnockSpeedMin=4.f;
inline constexpr std::uint64_t kHoldMs=250; // a sustained press = walk, not a poke
struct Evidence {
    float motion[3]{};std::uint64_t gripMs=0,tick=0;bool lowSweep=false; // the contact swept near the feet
};
enum class Outcome : unsigned char {None,Walk,Stumble,Knockdown,Sweep};
inline const char* name(Outcome o) noexcept {
    switch(o){case Outcome::Walk:return "PUSH_WALK";case Outcome::Stumble:return "PUSH_STUMBLE";
    case Outcome::Knockdown:return "SHOVE_KNOCKDOWN";case Outcome::Sweep:return "LEG_SWEEP";default:return "NONE";}
}
struct Plan {
    Outcome outcome=Outcome::None;float strength=0; // proposed 0..1 intent, not a native reaction receipt
};
inline float knockSpeedClamp() noexcept {return 8.f;}
inline Plan plan(const Evidence& e) noexcept {
    Plan p{};
    const float s=hand_push60::speed(e.motion);
    if(s<0||s<kWalkSpeedMin){p.outcome=Outcome::None;return p;}
    if(e.lowSweep&&s>=kWalkSpeedMin){p.outcome=Outcome::Sweep;p.strength=std::min(1.f,s/knockSpeedClamp());return p;}
    if(s>=kKnockSpeedMin&&e.gripMs<kHoldMs){p.outcome=Outcome::Knockdown;p.strength=std::min(1.f,s/8.f);return p;} // a hard quick shove
    if(s>=kStumbleSpeedMin&&e.gripMs<kHoldMs){p.outcome=Outcome::Stumble;p.strength=std::min(1.f,(s-kStumbleSpeedMin)/(kKnockSpeedMin-kStumbleSpeedMin));return p;}
    if(e.gripMs>=kHoldMs){p.outcome=Outcome::Walk;p.strength=std::min(1.f,s/kStumbleSpeedMin);return p;} // a sustained press steps her back
    p.outcome=Outcome::None;return p;
}
}
