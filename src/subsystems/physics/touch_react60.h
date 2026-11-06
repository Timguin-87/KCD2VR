#pragma once
#include <cmath>
#include <cstdint>
// DS-F96: the TOUCH-REACTION policy (Miki's "skin reacts" request, Route A).
// A hand contact on a living NPC's SOFT part (chest/butt - the procedural
// pendulum bones the bounce mod tunes) triggers the game's own flesh contact
// effect at the touch point + a haptic - the visible press response WITHOUT
// writing physics to a living actor. Route B (a real pendulum impulse) needs
// the CryAnimation pendulum-runtime audit (Claude's lane). Pure logic,
// host-tested. The contact witness (npc_contact172) is the input.
namespace kcd2vr::touch_react60 {
// The soft-parts list (chest/butt attachments). The caller maps the witness
// part id to these via the character setup; here the policy just classifies.
inline constexpr unsigned kChest=0x10u,kButt=0x20u;
struct Contact {
    std::uint32_t entity=0;int side=-1,part=-1;
    unsigned softMask=0; // which soft parts this contact touches (kChest/kButt)
    bool living=false,fistActive=false;
    float point[3]{},normal[3]{};
    std::uint64_t tick=0,worldGen=0; // DS-F102: the world generation clears the cooldown on map change
};
enum class Why : unsigned char {None,NotLiving,NotSoft,NoPoint,TooSoon,Haunted};
inline const char* name(Why w) noexcept {
    switch(w){case Why::None:return "OK";case Why::NotLiving:return "NOT_LIVING";case Why::NotSoft:return "NOT_A_SOFT_PART";
    case Why::NoPoint:return "NO_CONTACT_POINT";case Why::TooSoon:return "TOO_SOON";default:return "HAUNTED";}
}
struct Plan {
    Why why=Why::None;bool ok=false;
    float point[3]{},normal[3]{};int side=-1;unsigned softMask=0;std::uint32_t entity=0;
    bool flinch=false; // DS-F98 (Claude B3): the short additive flinch request - the parent-bone motion excites the pendulum jiggle
};
// DS-F98 (Claude review): a bounded per-entity cooldown map (16 slots) - two
// NPCs alternating can no longer bypass the 300 ms one-shot; a zero tick still
// honours the cooldown. DS-F102 (Codex review): the map clears on a world
// generation change (stale entity ids never pin a fresh world); under churn the
// round-robin eviction is DELIBERATE - the OLDEST slot is reused, which can
// drop an active cooldown. Overflow behavior: eviction wins over retention
// (bounded memory; a re-touch simply re-admits after the eviction).
struct Cooldown {std::uint32_t entity[16]{};std::uint64_t tick[16]{};unsigned next=0;std::uint64_t worldGen=0;
    bool admit(std::uint32_t e,std::uint64_t now,std::uint64_t world) noexcept {
        if(world!=worldGen){for(unsigned i=0;i<16;++i){entity[i]=0;tick[i]=0;}next=0;worldGen=world;}
        for(unsigned i=0;i<16;++i)if(entity[i]==e){if(now>=tick[i]&&now-tick[i]<300)return false;tick[i]=now;return true;}
        entity[next]=e;tick[next]=now;next=(next+1)&15u;return true;
    }};
inline Cooldown cooldown;
inline Plan plan(const Contact& c,std::uint64_t now) noexcept {
    Plan p{};p.why=Why::None;
    if(!c.living){p.why=Why::NotLiving;return p;}
    if(!c.softMask){p.why=Why::NotSoft;return p;}
    if(!std::isfinite(c.point[0])||!std::isfinite(c.point[1])||!std::isfinite(c.point[2])){p.why=Why::NoPoint;return p;}
    if(!std::isfinite(c.normal[0])||!std::isfinite(c.normal[1])||!std::isfinite(c.normal[2])){p.why=Why::NoPoint;return p;} // DS-F102: the normal must be finite before dispatch
    if(c.fistActive){p.why=Why::Haunted;return p;} // a deliberate fist is combat, not a touch
    if(!cooldown.admit(c.entity,now,c.worldGen)){p.why=Why::TooSoon;return p;}
    p.ok=true;p.side=c.side;p.softMask=c.softMask;p.entity=c.entity;
    p.point[0]=c.point[0];p.point[1]=c.point[1];p.point[2]=c.point[2];
    p.normal[0]=c.normal[0];p.normal[1]=c.normal[1];p.normal[2]=c.normal[2];
    p.flinch=true; // the visible jiggle comes from the additive flinch moving the parent bone
    return p;
}
}
