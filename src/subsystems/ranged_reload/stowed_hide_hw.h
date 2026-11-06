#pragma once
// D33 STOWED WEAPON HIDING (Miki 27 Oct: "hide all weapons on Henry but only in alchemy and smithing ... in game only
// the frontal weapons should be visible"). Pure policy + exactly-once ownership, host tested (tests/stowed_hide_hw_test.cpp).
// Henry's stowed items are character BONE attachments (same native route as the R59.16C10R15 stowed bow:
// GetInterfaceByName 0x971B2C + IAttachment::HideAttachment 0x5AF5E8). Names and bones from the retail Henry cdf
// (IPL_Characters-part0.pak child_henry.cdf, same skeleton attachment set as adult Henry, J27 log probe weapon_bow/
// weapon_quiver/weapon_quiver_ammo/weapon_crossbow/weapon_shield = 1):
//   weapon_holster_right -> rightweaponslot   FRONT (hip)      weapon_holster_left -> leftweaponslot  FRONT (hip)
//   weapon_dagger        -> DaggerSlot        FRONT (belt)
//   weapon_bow           -> BackSlot_bow      BACK             weapon_crossbow     -> BackSlot_crossbow BACK
//   weapon_cannon        -> BackSlot_cannon   BACK             weapon_shield       -> BackSlot_shield   BACK
//   weapon_quiver / weapon_quiver_ammo -> BackSlot_quiver BACK
// Only these bone attachments are touched: never hand sockets, station tools or anything on another character.
#include <array>
#include <atomic>
#include <cstdint>
#include <cstring>

namespace kcd2vr::stowed_hide_hw {

// R121 settings audit: the two D33 switches were INI only, so "Stowed bow visibility: NATIVE" in Experiments did
// nothing while VrHideBackWeapons (default ON) kept hiding every back model. Both are Experiments GEAR page 2 switches
// now (live_settings_d39.h), read + saved by live_runtime_d40.inl, and the stowed tick reads them every frame.
inline constexpr const char* kHideBackKey="VrHideBackWeapons";
inline constexpr const char* kHideAtStationsKey="VrHideStowedAtStations";
inline std::atomic<bool> hideBackSetting{true};       // VrHideBackWeapons: bow, crossbow, shield, quiver hidden in normal play
inline std::atomic<bool> hideAtStationsSetting{true}; // VrHideStowedAtStations: every stowed weapon hidden at alchemy / smithing

enum class Place : unsigned char { Front, Back };
struct Slot { const char* name; const char* bone; Place place; };
// Claude M57: retail weapon_class.xml pairs holster_slot_id8 with the
// LeftWeaponSheath scabbard. Hide it at stations; restore only our own hide.
inline constexpr std::array<Slot,10> kSlots{{
    {"weapon_holster_right","rightweaponslot",Place::Front},{"weapon_holster_left","leftweaponslot",Place::Front},
    {"weapon_dagger","DaggerSlot",Place::Front},
    {"weapon_bow","BackSlot_bow",Place::Back},{"weapon_crossbow","BackSlot_crossbow",Place::Back},
    {"weapon_cannon","BackSlot_cannon",Place::Back},{"weapon_shield","BackSlot_shield",Place::Back},
    {"weapon_quiver","BackSlot_quiver",Place::Back},{"weapon_quiver_ammo","BackSlot_quiver",Place::Back},
    {"scabbard_left","LeftWeaponSheath",Place::Front}}};
inline constexpr std::size_t kCount=kSlots.size();

struct Inputs {
    bool immersive=false;      // calibrated VR gameplay (StowedBowImmersive54)
    bool stationVisual=false;  // calibrated, no cutscene (menus are allowed: the minigame UI is open at a station)
    bool atStation=false;      // native alchemy or smithing minigame active (activity witness)
    bool hideAtStations=true;  // VrHideStowedAtStations
    bool hideBack=true;        // VrHideBackWeapons
    bool legacyBowWant=false;  // R59.16C10R14 stowed bow mode 1/2 decision for the bow attachment list
    bool backVisibility=false; // calibrated non-cutscene/non-reference view, independent of pose/menu ownership
};
// is `name` one of the legacy VrHideStowedBowAttachments names?
inline bool wantSlot(const Slot& s,const Inputs& in,bool inLegacyList) noexcept {
    if(in.atStation&&in.hideAtStations&&in.stationVisual)return true;      // A: everything stowed, at the station only
    // A UI or ordinary native activity can release the upper-body pose without
    // releasing an explicit stowed-back visibility preference. Held sockets
    // are not in this table; actual attachment consumers still qualify Henry.
    if(in.hideBack&&in.backVisibility&&s.place==Place::Back)return true;
    if(!in.immersive)return false;
    return inLegacyList&&in.legacyBowWant;                                  // old stowed-bow rule stays valid
}

// Exactly-once ownership: we restore only what WE hid on THIS character, and never adopt something the game hid.
enum class Act : unsigned char { None, Hide, Show, Forget };
struct Owner {
    std::uintptr_t character=0;
    std::uint64_t lifetimeD44=0;
    std::array<std::uintptr_t,kCount> hidden{}; // attachment pointer we hid, per slot
    unsigned hides=0,shows=0,reshows=0;
    // character changed (save/load, death respawn, body swap): the old pointers are not ours to touch any more
    bool onCharacter(std::uintptr_t ch) noexcept {
        if(ch==character)return false;
        character=ch;hidden.fill(0);return true;
    }
    bool onOwnerD44(std::uintptr_t ch,std::uint64_t lifetime) noexcept {
        if(ch==character&&lifetime==lifetimeD44)return false;
        character=ch;lifetimeD44=lifetime;hidden.fill(0);return true;
    }
    // observed: attachment pointer (0 = missing) and its current hidden bit
    Act step(std::size_t i,std::uintptr_t att,bool isHidden,bool want) noexcept {
        if(i>=kCount)return Act::None;
        if(!att){hidden[i]=0;return Act::None;}
        if(hidden[i]&&hidden[i]!=att){hidden[i]=0;} // a different attachment object now: the old receipt is void
        if(want){
            if(isHidden){return Act::None;}                       // already hidden (by us earlier or by the game)
            if(hidden[i]==att)++reshows;                          // the game showed it again after our hide
            return Act::Hide;
        }
        if(hidden[i]==att&&isHidden)return Act::Show;             // ours and still hidden: restore exactly once
        if(hidden[i]&&!isHidden){hidden[i]=0;return Act::Forget;} // the game already showed it: nothing to restore
        return Act::None;                                         // never unhide what the game itself hid
    }
    // Exact retail HideAttachment writes/clears all three visibility bits.
    // The game may own shadow/recursion visibility even with the main bit clear.
    // Never adopt that partial state, or clear it through a later Show(false).
    Act stepFlagsD44(std::size_t i,std::uintptr_t att,unsigned flags,bool want) noexcept {
        if(i>=kCount)return Act::None;
        const unsigned bits=flags&0x70000u;
        if(hidden[i]==att&&bits&&bits!=0x70000u)hidden[i]=0;
        return step(i,att,bits!=0,want);
    }
    void applied(std::size_t i,Act a,std::uintptr_t att,bool ok) noexcept {
        if(i>=kCount||!ok)return;
        if(a==Act::Hide){hidden[i]=att;++hides;}
        else if(a==Act::Show){hidden[i]=0;++shows;}
    }
    bool anyHidden() const noexcept {for(auto h:hidden)if(h)return true;return false;}
};

// Draw intent cannot change this stowed-slot policy. A held item moves to its
// native hand binding; its entity visibility is retired only by that exact
// held-item presentation owner. Unrelated back slots never receive a grace.

} // namespace kcd2vr::stowed_hide_hw
