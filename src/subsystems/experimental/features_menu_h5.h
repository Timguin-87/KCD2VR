#pragma once
#include <array>
#include "deepseek_options_h29.h"
#include "../../combat_menu49.h"
#include "../../frik_pose_probe.h"
#include "../../activity_eye59c19.h"
#include "../throw_drop/grip_stow_cb1.h"
#include "../doors/hand_only_h40.h"

// Presentation + existing revocable session controls only. Native actions and
// persistent configuration remain with their existing owners.
namespace kcd2vr::experimental_features_h5 {
using Rect=combat_menu49::Rect;
enum class Group : unsigned {Hands,Weapons,Tools,Combat,View,Count};
enum class Trial : unsigned {None,Wall,Fingers,Aperture,WeaponNpcContacts,F9,Kick,Stow,AlchemyEye,DoorHandDisplay};
struct Card {const wchar_t* title;const wchar_t* detail;Trial trial=Trial::None;};
constexpr Rect kHomeSave{708,554,950,578},kHomeStatus{620,580,1038,608};
constexpr Rect kHomeFeatures{620,610,1038,636},kHomeCombat{620,642,1038,668};
constexpr std::array<Rect,5> kGroups{{{620,116,698,148},{704,116,782,148},{788,116,866,148},{872,116,950,148},{956,116,1034,148}}};
constexpr Rect kHeading{620,158,900,186},kCombatLink{910,158,1038,186};
constexpr std::array<Rect,3> kCards{{{620,196,1038,228},{620,316,1038,348},{620,436,1038,468}}};
constexpr std::array<Rect,3> kDetails{{{620,232,1038,306},{620,352,1038,426},{620,472,1038,554}}};
constexpr Rect kAllOff{620,582,1038,614},kScope{620,620,1038,664};
constexpr Group group(unsigned value) noexcept {return value<unsigned(Group::Count)?Group(value):Group::Hands;}
constexpr const wchar_t* tab(Group value) noexcept {
 switch(value){case Group::Hands:return L"HANDS";case Group::Weapons:return L"PHYSICS";case Group::Tools:return L"TOOLS";case Group::Combat:return L"COMBAT";case Group::View:return L"VIEW / GEAR";default:return L"";}
}
constexpr const wchar_t* heading(Group value) noexcept {
 switch(value){case Group::Hands:return L"Hands and held objects";case Group::Weapons:return L"F9 physical ownership";case Group::Tools:return L"Complete interaction cycles";case Group::Combat:return L"Native combat and reactions";case Group::View:return L"View, map and equipment";default:return L"";}
}
constexpr std::array<Card,3> cards(Group value) noexcept {
 switch(value){
 case Group::Hands:return {{
  {L"Free-hand finger clearance trial",L"IMPLEMENTED / HOST CHECKED; combined headset not run. Extra curl for empty hands with precise contacts. No wrist movement, individual finger bodies or hit authority.",Trial::Fingers},
  {L"Grip-held item to inventory",L"Experimental: hold a loose item and press its hand button (default A/Y) to release and stow it through native inventory. Native rules apply. OFF by default; headset not tested.",Trial::Stow},
  {L"Closed fists and native hold poses",L"HOST CHECKED: full authored fists; held fingers paired to the same native grip and skeleton map. Missing reference may show open fingers after Finish. Palm/object anchor and headset appearance remain unproved."}
 }};
 case Group::Weapons:return {{
  {L"F9 physical mode",L"EXISTING OWNER; combined headset not run. Same F9 adapter as the key. Mode is not body readiness. Rung 2 uses achieved body; rung 3 uses tracked weapon/corpse lab. OFF retires the current owner.",Trial::F9},
  {L"F9 living NPC contact trial",L"EXPERIMENTAL: weapon collisions with living NPCs; visible stability unverified. Requires F9 physical weapon mode. Separate from paused living-touch/impact reactions. KCD2 still decides hits, stamina and damage.",Trial::WeaponNpcContacts},
  {L"Checked wall backout assist",L"IMPLEMENTED / HOST CHECKED; combined headset not run. Optional small outward assist. Normal repaired wall sliding and reacquisition do not require this trial.",Trial::Wall}
 }};
 case Group::Tools:return {{
  {L"Door: hand follows grabbed point",L"HOST CHECKED; headset not run. OFF by default. Hand follows the grabbed point at the native door angle. Grip doors also work with this OFF. Release finishes opening/closing. Exact handle and partial holding remain unverified.",Trial::DoorHandDisplay},
  {L"Alchemy: legacy F5 head trial",L"Saved FREE VRIK already owns tracked body/head. This optional switch retains the separate F5 alchemy head trial. Native actions and recipe costs remain authoritative; full cycle and headset unverified.",Trial::AlchemyEye},
  {L"Smithing: live contact controls",L"EXPERIMENTS > SMITH: work mode, world tool collision and anvil support. Real bounds uses the native anvil box, not a full mesh. Native stroke quality and results stay authoritative; headset validation remains."}
 }};
 case Group::Combat:return {{
  {L"Master strikes and combos",L"Cuts/stabs share native admission; phase32 continuation repaired. Combined headset not run. Victim reaction selection and native chain resets remain unresolved. NEXT: exact original action/target/callback joins."},
  {L"Native empty-hand R3 kick",L"Experimental: empty-hand R3 requests the native kick and authored lower-body animation. The game decides eligibility, hit and cost. Armed R3 unchanged. OFF by default; headset not tested.",Trial::Kick},
  {L"Gore and NPC limb grabbing",L"Native lethal result remains required. NPC limb-grab runtime provider is missing; corpse joint/selected reaction assets still need exact ownership. NEXT: current part/constraint to confirmed native Absent."}
 }};
 default:return {{
  {L"Both-eye comfort aperture",L"IMPLEMENTED / HOST CHECKED; headset not run. Uses your existing comfort-vignette strength without enabling comfort or changing the saved level. Session only.",Trial::Aperture},
  {L"VR map, reloads, torch and holsters",L"Bow/reload and holsters preserved. Native paper map and light/medium crossbow working-hand, tool and bolt code integrated; host checks only, headset not run. Heavy crank, rifle ramrod and throwing cycles remain incomplete; NEXT: prove their native owners."},
  {L"VRIK and HMD dodge ownership",L"VRIK elbow/finger acceptance remains open. HMD dodge requested for >15cm lateral/backward translation; concurrent-strike handoff is unproved. NEXT: exact current pose/action lifetime; no rotation-only dodge."}
 }};
 }
}
inline bool enabled(Trial trial) noexcept {
 switch(trial){case Trial::DoorHandDisplay:return kcd2vr::hand_only_door_h40::displayEnabled.load(std::memory_order_acquire);case Trial::AlchemyEye:return kcd2vr::alchemy_own_head_h51::enabled.load(std::memory_order_acquire);case Trial::Kick:return frik_pose_probe::FistKickEnabledH35();case Trial::Stow:return grip_stow_cb1::Enabled();case Trial::Wall:return deepseek_h29::wallBackoutSpringH31.load(std::memory_order_acquire);case Trial::Fingers:return deepseek_h29::fingerContactSpringH31.load(std::memory_order_acquire);case Trial::Aperture:return deepseek_h29::angularVignette.load(std::memory_order_acquire);case Trial::WeaponNpcContacts:return deepseek_h29::WeaponNpcContactsEnabledH5();default:return false;}
}
inline bool toggle(Trial trial) noexcept {
 switch(trial){
 case Trial::DoorHandDisplay:kcd2vr::hand_only_door_h40::displayEnabled.store(!enabled(trial),std::memory_order_release);return true;
 case Trial::AlchemyEye:kcd2vr::alchemy_own_head_h51::enabled.store(!enabled(trial),std::memory_order_release);return true;
 case Trial::Kick:frik_pose_probe::SetFistKickEnabledH35(!enabled(trial));return true;
 case Trial::Stow:grip_stow_cb1::SetEnabled(!enabled(trial));return true;
 case Trial::Wall:deepseek_h29::wallBackoutSpringH31.store(!enabled(trial),std::memory_order_release);return true;
 case Trial::Fingers:deepseek_h29::fingerContactSpringH31.store(!enabled(trial),std::memory_order_release);return true;
 case Trial::Aperture:deepseek_h29::angularVignette.store(!enabled(trial),std::memory_order_release);return true;
 case Trial::WeaponNpcContacts:deepseek_h29::SetWeaponNpcContactsH5(!enabled(trial));return true;
 default:return false; // F9 must pass through the existing VrMenuState adapter.
 }
}
constexpr const wchar_t* f9Label(int rung) noexcept {
 return rung==3?L"F9 MODE: TRACKED / CORPSE LAB":rung==2?L"F9 MODE: PHYSICAL WEAPON":rung==1?L"F9 MODE: OBSERVE ONLY":L"F9 MODE: OFF";
}
constexpr bool clean() noexcept {
 const auto inside=[](Rect r){return combat_menu49::inside(r,combat_menu49::kClip)&&r.t>=110;};
 const std::array<Rect,15> rows{{kGroups[0],kGroups[1],kGroups[2],kGroups[3],kGroups[4],kHeading,kCombatLink,kCards[0],kDetails[0],kCards[1],kDetails[1],kCards[2],kDetails[2],kAllOff,kScope}};
 for(unsigned i=0;i<rows.size();++i){if(!inside(rows[i]))return false;for(unsigned j=i+1;j<rows.size();++j)if(combat_menu49::overlap(rows[i],rows[j]))return false;}
 return kHomeSave.b<kHomeStatus.t&&kHomeStatus.b<kHomeFeatures.t&&kHomeFeatures.b<kHomeCombat.t&&kHomeCombat.b<=672;
}
static_assert(clean(),"Experimental navigation, status and actual controls must remain separate");
}
