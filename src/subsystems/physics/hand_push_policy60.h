#pragma once
#include "core/snapshot_mailbox.h"
#include "vrik/npc_contact172.h"
#include <cmath>
#include <cstdint>
// DS-F106: the HAND-PUSH (HIGGS-style) policy - a hand contact on a RIGID
// prop (the npc_contact172 witness: body=Rigid, entityId, the hand's world
// motion) plans a bounded physical impulse on the prop. FAIL-CLOSED: a living
// actor is NEVER pushed (the H18 rule), an articulated corpse is the impale's,
// an unknown body is refused, a slow/stationary hand is refused, and one push
// per entity per cooldown. Pure, host-tested. The native side is the pinned
// rigid Action (vft 3A48DC0 -> Action AC0D18, the PartVelocity pattern).
namespace kcd2vr::hand_push60 {
// npc_contact172::Body values (kept as ints so this header stays standalone)
inline constexpr int kBodyRigid=int(npc_contact172::Body::Rigid),kBodyLiving=int(npc_contact172::Body::Living),kBodyArticulated=int(npc_contact172::Body::Articulated);
inline constexpr float kMinPushSpeed=.5f;   // m/s: below this the hand is resting
inline constexpr float kMaxPushSpeed=6.f;   // m/s cap on the impulse
inline constexpr unsigned kCooldownMs=350;  // one push per entity per 350 ms
struct Evidence {
    std::uint32_t entity=0;int part=-1,bodyKind=0;
    float motion[3]{};float point[3]{},normal[3]{};
    int side=-1;std::uint64_t tick=0;
};
enum class Why : unsigned char {None,NotRigid,LivingOrCorpse,NoEntity,Slow,NonFinite,TooSoon};
inline const char* name(Why w) noexcept {
    switch(w){case Why::None:return "OK";case Why::NotRigid:return "NOT_A_RIGID_PROP";
    case Why::LivingOrCorpse:return "LIVING_OR_CORPSE_NOT_PUSHED";case Why::NoEntity:return "NO_PROP_ENTITY";
    case Why::Slow:return "HAND_TOO_SLOW";case Why::NonFinite:return "NONFINITE_MOTION";default:return "TOO_SOON";}
}
struct Plan {
    Why why=Why::None;bool ok=false;std::uint32_t entity=0;int side=-1;
    float impulse[3]{};
};
inline float speed(const float v[3]) noexcept {
    return std::isfinite(v[0])&&std::isfinite(v[1])&&std::isfinite(v[2])?
        std::sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]):-1.f;
}
struct Cooldown {std::uint32_t entity[16]{};std::uint64_t tick[16]{};unsigned next=0;
    bool admit(std::uint32_t e,std::uint64_t now) noexcept {
        for(unsigned i=0;i<16;++i)if(entity[i]==e){if(now>=tick[i]&&now-tick[i]<kCooldownMs)return false;tick[i]=now;return true;}
        entity[next]=e;tick[next]=now;next=(next+1)&15u;return true; // eviction-under-churn wins (documented, like touch_react60)
    }};
inline Cooldown cooldown;
inline Plan plan(const Evidence& e,std::uint64_t now) noexcept {
    Plan p{};p.why=Why::None;p.entity=e.entity;p.side=e.side;
    if(!e.entity){p.why=Why::NoEntity;return p;}
    if(e.bodyKind!=kBodyRigid){p.why=(e.bodyKind==kBodyLiving||e.bodyKind==kBodyArticulated)?Why::LivingOrCorpse:Why::NotRigid;return p;}
    const float s=speed(e.motion);
    if(s<0){p.why=Why::NonFinite;return p;}
    if(s<kMinPushSpeed){p.why=Why::Slow;return p;}
    if(!cooldown.admit(e.entity,now)){p.why=Why::TooSoon;return p;}
    const float k=s>kMaxPushSpeed?kMaxPushSpeed/s:1.f;
    p.impulse[0]=e.motion[0]*k;p.impulse[1]=e.motion[1]*k;p.impulse[2]=e.motion[2]*k;
    p.ok=true;return p;
}
// DS-F106: the pose side publishes the push evidence; the game-thread drain
// plans + fires the impulse on the proven rigid route (PropSetVelocity5916C1).
inline core::SnapshotMailbox<Evidence> mailbox;
}
