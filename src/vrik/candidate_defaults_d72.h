#pragma once
#include <array>
namespace kcd2vr::defaults_d72 {
struct Setting {const char* key;const char* value;};
// The D72 user-requested candidate profile. Applied once to an older INI;
// subsequent menu choices are persistent and never reapplied on each startup.
inline constexpr std::array<Setting,12> settings{{
 {"VrInstantActivitiesD72","true"},{"VrInstantInteractionsD72","false"} /* R124: Miki's INI */,
 {"VrInstantCraftingD72","false"},{"VrInstantReloadsD72","false"},{"VrInstantCombatD72","true"},
 {"VrSmithingMode","3"},{"VrSmithSemiAnchorsD67","true"} /* D85 craft: LT assist ON (was OFF, ran after the D81 repair) */,
 {"VrLeftChestKnives","false"},{"VrTwoHandPrimaryBiasExperimentD65","true"},
 {"VrTwoHandPrimaryBiasD65","0.40"},{"VrCrossbowReloadD65C","true"},
 {"VrBowPhysicalAimD65C","true"}
}};
}
