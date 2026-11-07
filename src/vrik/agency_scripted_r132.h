#pragma once
#include <cstdint>

// R132 (Miki 7 Nov, new game: "after arriving to Trosky I lost it (it begins on the horse) - so no calibration or VRIK").
// the "scripted" gate (V64.19C) was: 4 or more of the 31 native action filters on = the game took control, VRIK off.
// the prologue ride to Trosky keeps 5 STORY filters on the whole time (KCD2VR.log 01:33:13.895
// "V64.19C-AGENCY count=5 mask=0x50820002 active=no_inventory|no_dog|no_distract|no_companion_bond|no_horse_saddle_bond")
// while Henry is fully controlled by the player. so VRIK tracking was "not allowed" (gameplay=0 in every R47C1 row),
// the saved calibration never re-seated on the new Henry (POSE-REJECT reason=CALIBRATION_NOT_READY at each mount) and
// the body stayed native the whole ride (D61_PERF poses=0 committed=0 from 01:30:55 to 01:33:23).
// now: scripted = the old count rule AND a real control filter (move / camera, or 2 of the body-control ones).
// story-only restrictions (inventory, dog, fast travel, torch, companion bond, horse bond, ...) keep VRIK on.
// never stricter than before: every mask that is scripted now was scripted before.
namespace kcd2vr::agency_r132 {
// bit = index into kAgencyFiltersV6419C (mod.cpp)
enum Filter : unsigned { NoMenu=0,NoInventory=1,NoInteraction=2,NoApse=3,NoAttack=4,NoCamera=5,NoHorizontalCamera=6,NoFocus=7,
    NoJump=8,NoCrouch=9,NoMove=10,NoHorse=11,NoStandUp=21 };
inline constexpr std::uint64_t bit(unsigned i) noexcept {return 1ull<<i;}
// any one of these: Henry's body or view is driven by the game
inline constexpr std::uint64_t kHardControl=bit(NoMove)|bit(NoCamera)|bit(NoHorizontalCamera);
// two of these together: the game holds the body (attack + interaction + jump ... all off)
inline constexpr std::uint64_t kBodyControl=bit(NoInteraction)|bit(NoAttack)|bit(NoFocus)|bit(NoJump)|bit(NoCrouch)|bit(NoHorse)|bit(NoStandUp);
inline constexpr int bits(std::uint64_t m) noexcept {int n=0;while(m){m&=m-1;++n;}return n;}
inline constexpr bool oldScripted(bool known,int count) noexcept {return known&&count>=4;}
inline constexpr bool scripted(bool known,int count,std::uint64_t mask) noexcept {
    return oldScripted(known,count)&&((mask&kHardControl)!=0||bits(mask&kBodyControl)>=2);
}
// only story filters on (old rule said scripted, new one does not): VRIK stays on
inline constexpr bool storyOnly(bool known,int count,std::uint64_t mask) noexcept {
    return oldScripted(known,count)&&!scripted(known,count,mask);
}
}
