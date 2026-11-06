#pragma once
#include <cmath>
#include <cstdint>
// Staged living-grip admission only. No production living constraint,
// animation/motor ownership or NPC locomotion provider is connected here.
// A living NPC (never a corpse, which has separate CG1/impale ownership, never
// a PickableItem - native inventory owns those), a limb part (arm/leg, never
// the torso/head for the first stage), and a deliberate grip (not a fist
// punch - the closed-fist boxing owner must not be stolen). Pure, host-tested.
namespace kcd2vr::npc_grab60 {
enum class Why : unsigned char {None,NotLiving,NotNpc,Corpse,InventoryItem,TorsoOrHead,PartUnknown,FistActive,GripTooShort,NotHeld};
inline const char* name(Why w) noexcept {
    switch(w){case Why::None:return "OK";case Why::NotLiving:return "NOT_LIVING";case Why::NotNpc:return "NOT_NPC";
    case Why::Corpse:return "CORPSE_NOT_GRABBABLE";case Why::InventoryItem:return "INVENTORY_ITEM_NOT_GRABBABLE";
    case Why::TorsoOrHead:return "TORSO_OR_HEAD_REFUSED";case Why::PartUnknown:return "PART_UNKNOWN";
    case Why::FistActive:return "FIST_OWNER_ACTIVE";case Why::GripTooShort:return "GRIP_TOO_SHORT";default:return "NOT_HELD";}
}
struct Admission {
    bool npc=false,alive=false,corpse=false,inventoryItem=false,limbPart=false,partKnown=false;
    bool fistActive=false,held=false;std::uint64_t gripMs=0;
};
inline constexpr std::uint64_t kMinGripMs=120; // a deliberate grip, not a tap
inline constexpr std::uint64_t kMaxGripMs=4000; // bounded admission window
// A native raw part ID is not a skeleton joint index or an anatomical class.
// The caller supplies current native part and explicit limb-mapping evidence.
// In particular, [0,4095] does not distinguish a head/torso from an arm/leg.
inline Why qualify(const Admission& a) noexcept {
    if(!a.npc)return Why::NotNpc;
    if(a.corpse)return Why::Corpse; // a corpse is more specific than !alive
    if(!a.alive)return Why::NotLiving;
    if(a.inventoryItem)return Why::InventoryItem;
    if(!a.partKnown)return Why::PartUnknown;
    if(!a.limbPart)return Why::TorsoOrHead;
    if(a.fistActive)return Why::FistActive; // never steal a qualified deliberate fist
    if(!a.held)return Why::NotHeld;
    if(a.gripMs<kMinGripMs||a.gripMs>kMaxGripMs)return Why::GripTooShort;
    return Why::None;
}
}
