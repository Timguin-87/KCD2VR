#pragma once
#include "npc_grab_policy60.h"
#include "../../vrik/npc_contact172.h"
#include <cstdint>
// Staged typed corpse-limb admission, distinct from the actual CG1/W5 velocity
// owner in vrik/lab58.inl. No generic rigid backend submits this request.
// Current native death, exact body/part and anatomical mapping are independent
// witnesses; a physical class or an integer part range cannot replace them.
namespace kcd2vr::corpse_drag60 {
namespace ng=kcd2vr::npc_grab60; // the policy's namespace (the file is npc_grab_policy60.h)
enum class Why : unsigned char {None,NotCorpse,Living,ImpaleOwns,NotLimb,NoBody,PartUnknown,GripTooShort,DeathUnknown,OwnerUnknown,Stale};
inline const char* name(Why w) noexcept {
    switch(w){case Why::None:return "OK";case Why::NotCorpse:return "NOT_A_CORPSE";case Why::Living:return "LIVING_NOT_DRAGGABLE";
    case Why::DeathUnknown:return "NATIVE_DEATH_UNKNOWN";case Why::OwnerUnknown:return "CURRENT_CORPSE_PART_OR_LIMB_MAPPING_UNKNOWN";case Why::Stale:return "ORIGINAL_CONTACT_STALE";case Why::ImpaleOwns:return "IMPALE_OWNS_THE_VICTIM";case Why::NotLimb:return "NOT_A_LIMB_PART";
    case Why::NoBody:return "NO_RAGDOLL_BODY";case Why::PartUnknown:return "PART_UNKNOWN";default:return "GRIP_TOO_SHORT";}
}
struct Evidence {
    std::uint32_t entity=0;int part=-1,bodyKind=0;
    std::uintptr_t body=0; // the articulated IPhysicalEntity (the caller's witness)
    bool impaleActive=false;bool held=false;std::uint64_t gripMs=0,press=0,tick=0,now=0;
    bool nativeDead=false,currentBodyPart=false,limbClassified=false;
};
struct Request {
    std::uint32_t entity=0;int part=-1;std::uintptr_t body=0;
    bool ok=false;std::uint64_t press=0,tick=0;const char* why="";
};
inline constexpr int kBodyArticulated=int(npc_contact172::Body::Articulated),kBodyLiving=int(npc_contact172::Body::Living);
inline constexpr std::uint64_t kMinGripMs=150;
inline Request admit(const Evidence& e) noexcept {
    Request r{};r.entity=e.entity;r.part=e.part;r.body=e.body;r.press=e.press;r.tick=e.tick;
    if(!e.entity){r.why=name(Why::NotCorpse);return r;}
    if(e.bodyKind==kBodyLiving){r.why=name(Why::Living);return r;}
    if(e.bodyKind!=kBodyArticulated){r.why=name(Why::NotCorpse);return r;}
    if(!e.nativeDead){r.why=name(Why::DeathUnknown);return r;}
    if(!e.currentBodyPart){r.why=name(Why::OwnerUnknown);return r;}
    if(!e.press||!e.tick||e.now<e.tick||e.now-e.tick>80){r.why=name(Why::Stale);return r;}
    if(e.impaleActive){r.why=name(Why::ImpaleOwns);return r;} // the impale owns its victim while it hangs
    if(!e.body){r.why=name(Why::NoBody);return r;}
    if(e.part<0){r.why=name(Why::PartUnknown);return r;}
    if(!e.limbClassified){r.why=name(Why::NotLimb);return r;}
    if(!e.held||e.gripMs<kMinGripMs){r.why=name(Why::GripTooShort);return r;}
    r.ok=true;r.why=name(Why::None);return r;
}
}
