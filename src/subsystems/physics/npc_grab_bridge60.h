#pragma once
#include "npc_grab_policy60.h" // DS-F95: the admission (ng::)
#include "../../vrik/npc_contact172.h"
#include <cstdint>
// Staged admission contract. There is no connected living-NPC constraint or
// animation/locomotion adapter. A rigid prop backend cannot own an NPC limb.
namespace kcd2vr::npc_grab_bridge60 {
namespace ng=kcd2vr::npc_grab60; // the policy's namespace (the file is npc_grab_policy60.h)
inline constexpr int kBodyLiving=int(npc_contact172::Body::Living),kBodyArticulated=int(npc_contact172::Body::Articulated);
struct Evidence {
    std::uint32_t entity=0;int side=-1,part=-1,bodyKind=0;
    bool alive=false,fistActive=false,held=false;
    std::uint64_t gripMs=0,press=0,tick=0;
    std::uintptr_t identity=0,body=0;
    std::uint64_t now=0;
    bool nativeActorCurrent=false,currentBodyPart=false,limbClassified=false,inventoryItem=false;
};
struct Request {
    std::uint32_t entity=0;int side=-1,part=-1,bodyKind=0;
    bool ok=false;std::uint64_t press=0,tick=0;
    const char* why="";
};
inline Request bridge(const Evidence& e) noexcept {
    Request r{};r.entity=e.entity;r.side=e.side;r.part=e.part;r.bodyKind=e.bodyKind;r.press=e.press;r.tick=e.tick;
    ng::Admission a{};
    a.npc=(e.bodyKind==kBodyLiving||e.bodyKind==kBodyArticulated);
    if(!a.npc){r.why=ng::name(ng::Why::NotNpc);return r;}
    if(!e.entity||e.side<0||e.side>1||!e.press){r.why="INVALID_GRIP_REQUEST";return r;}
    if(!e.tick||e.now<e.tick||e.now-e.tick>80){r.why="ORIGINAL_CONTACT_STALE";return r;}
    if(!e.identity||!e.body||!e.nativeActorCurrent){r.why="CURRENT_NATIVE_ACTOR_BODY_UNKNOWN";return r;}
    a.alive=e.alive;
    a.corpse=(e.bodyKind==kBodyArticulated&&!e.alive);
    a.inventoryItem=e.inventoryItem;
    a.partKnown=(e.part>=0)&&e.currentBodyPart;
    a.limbPart=e.limbClassified;
    a.fistActive=e.fistActive;
    a.held=e.held;
    a.gripMs=e.gripMs;
    const auto w=ng::qualify(a);
    r.ok=(w==ng::Why::None);
    r.why=ng::name(w);
    return r;
}
}
