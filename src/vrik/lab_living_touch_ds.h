// F9 rung 3 contact experiment: bounded policy math, without native writes.
// The runtime adapter proves the current living capsule or mapped skeletal joint
// before submission. Direct secondary-pendulum ownership remains unimplemented.
// No living-ragdoll activation, constraint, damage, stamina or accepted-hit writes.
#pragma once
#include <cmath>
#include <cstdint>
#include <algorithm>
#include "npc_contact172.h"
namespace kcd2vr::lab_living_touch_ds {
// The admission contract (the caller supplies the proven evidence; this header decides).
struct Evidence {
    std::uint32_t entity=0;      // the living NPC entity
    int part=-1;                // the touched part id (-1 = unknown)
    int bodyKind=0;             // npc_contact172::Body int (must be Living)
    double speed=0;             // the hand speed at the touch, m/s
    double dist=0;              // committed hand feature to contact point, m (not NPC origin)
    bool gripHeld=false;        // the hand Grip is held (the push/pull mode)
    bool softPart=false;        // reserved policy hint; no native soft-part binding is supplied
    bool henry=false;           // the entity is Henry (the refuse)
    std::uint64_t tick=0,now=0;
};
struct Request {
    std::uint32_t entity=0;int part=-1;
    bool push=false;            // the push/pull impulse on the touched part
    bool poke=false;            // the soft-part poke impulse
    double k=0;                 // push 0.1 to 0.6; half-strength poke 0.05 to 0.3
    const char* why="";
};
inline constexpr double kMinSpeed=.4,kMaxSpeed=6.;   // the m/s window for a deliberate touch
inline constexpr double kMaxDist=.5;                  // committed hand feature to contact point
inline constexpr double kKPerSpeed=.15,kMinK=.1,kMaxK=.6; // native submission strength; visible response unverified
inline constexpr int kBodyLiving=int(kcd2vr::npc_contact172::Body::Living);
inline double impulseK(double speed) noexcept {
    return std::isfinite(speed)?std::clamp(kKPerSpeed*speed,kMinK,kMaxK):kMinK;
}
inline Request admit(const Evidence& e) noexcept {
    Request r{};r.entity=e.entity;r.part=e.part;
    if(!e.entity){r.why="NO_ENTITY";return r;}
    if(e.henry){r.why="HENRY";return r;}
    if(e.bodyKind!=kBodyLiving){r.why="NOT_LIVING";return r;} // the corpses go the rung-2/3 corpse-drag way
    // A living capsule has no limb part. Only a separately proven soft-part
    // poke may require a joint; never reinterpret its capsule part as a joint.
    if(e.softPart&&e.part<0){r.why="NO_PART";return r;}
    if(!std::isfinite(e.speed)||e.speed<kMinSpeed){r.why="TOO_SLOW";return r;}
    if(e.speed>kMaxSpeed){r.why="TOO_FAST";return r;}
    if(!std::isfinite(e.dist)||e.dist<0||e.dist>kMaxDist){r.why="TOO_FAR";return r;}
    if(!e.tick||!e.now||e.now<e.tick||e.now-e.tick>80){r.why="STALE";return r;}
    if(e.gripHeld&&!e.softPart){r.push=true;r.k=impulseK(e.speed);r.why="GRIP_PUSH";return r;} // the deliberate push/pull
    if(!e.gripHeld&&e.softPart){r.poke=true;r.k=impulseK(e.speed)*.5;return r;} // the soft poke, gentler
    // W5 (25 Oct, Miki: "npc showing or reacting to our hands"): an OPEN moving hand touching a living capsule was
    // NO_TOUCH_MODE = nothing, and in every logged rung-3 session the grip was 0.00. an open-hand touch is a half
    // strength shove now (same bounded capsule impulse, same budget); a held grip stays the full push.
    if(!e.gripHeld&&!e.softPart){r.push=true;r.k=impulseK(e.speed)*.5;r.why="OPEN_HAND_TOUCH";return r;}
    r.why="GRIP_POKE_NOT_SUPPORTED"; // the grip on a soft part = the no constraint by design
    return r;
}
// The rate limiter mirrors the lab's: one touch per entity per cooldown, a global window.
struct Budget {
    static constexpr std::uint64_t kEntityCooldownMs=300,kWindowMs=1000;
    static constexpr unsigned kPerWindow=6;
    struct Ent {std::uint32_t entity=0;std::uint64_t tick=0;};
    Ent ent[12]{};std::uint64_t global[kPerWindow]{};unsigned at=0;
    const char* refuse(std::uint32_t entity,std::uint64_t now) const noexcept {
        if(!entity)return "NO_ENTITY";
        for(const auto& x:ent)if(x.entity==entity&&x.tick&&now>=x.tick&&now-x.tick<kEntityCooldownMs)return "ENTITY_COOLDOWN";
        unsigned n=0;for(auto t:global)if(t&&now>=t&&now-t<kWindowMs)++n;
        return n>=kPerWindow?"GLOBAL_BUDGET":nullptr;
    }
    bool admit(std::uint32_t entity,std::uint64_t now) noexcept {
        if(refuse(entity,now))return false;
        Ent* slot=nullptr;for(auto& x:ent){if(x.entity==entity){slot=&x;break;}if(!slot&&(!x.entity||!x.tick||now<x.tick||now-x.tick>=kEntityCooldownMs))slot=&x;}
        if(!slot)slot=&ent[0];
        *slot={entity,now};
        global[at++%kPerWindow]=now;return true;
    }
};
}
