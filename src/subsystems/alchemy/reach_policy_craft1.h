#pragma once
#include <cmath>
#include <atomic>
#include <cstdint>

// Crafting-only hand nomination policy. Native AlchemyItem table indices, not
// world left/right guesses: rotating a table does not exchange hand roles.
// The source reader still has to prove the current registered entity/bounds.
// These ranges authorize SELECTION, never an attach, recipe result or transfer.
#define KCD2VR_CRAFT_PROXIMITY_V1 1
#define KCD2VR_CLOSE_PICKUP_D39 1
namespace kcd2vr::subsystems::alchemy::reach_craft1 {
inline std::atomic<bool> enabled{false}; // Optional CRAFT1 trial; baseline OFF.
struct Rule { unsigned hands; double metres; };
inline constexpr unsigned Right=1,Left=2,Both=3;
inline constexpr double Tolerance=1.e-6;
inline constexpr Rule rule(int slot) noexcept {
    if(slot<0||slot>=21)return {0,0.};
    if((slot>=13&&slot<=16)||slot==18)return {Left,.20}; // resource carafes and bellows
    if(slot==1||(slot>=7&&slot<=12))return {Right,.20}; // hourglass and native ingredient slots
    return {Both,.10}; // dish, mortar/pestle, pot and other registered central tools
}
inline constexpr bool allowed(int slot,int side) noexcept {
    return side>=0&&side<2&&(rule(slot).hands&(1u<<unsigned(side)))!=0;
}
inline constexpr double radius(int slot,int side) noexcept {
    return allowed(slot,side)?rule(slot).metres:0.;
}
inline bool contains(int slot,int side,double gap) noexcept {
    return allowed(slot,side)&&std::isfinite(gap)&&gap>=0.&&gap<=radius(slot,side)+Tolerance;
}
// No wider outer radius: leaving the requested 10/20 cm zone restores gaze.
// Identity/tie retention is handled INSIDE the region, not by increasing reach.
// D39: source pickup alone may require the tracked palm to approach the
// current native object bounds. Three centimetres represents a small palm
// contact allowance; it is a bounds proxy, not a mesh collision claim. Slot
// roles stay native, and admitted tool/destination/work radii stay unchanged.
enum class PickupReachD39 : int {CloseBounds=0,Assisted=1};
inline std::atomic<int> pickupReachD39{int(PickupReachD39::CloseBounds)};
inline constexpr int clampPickupReachD39(int value) noexcept {
 return value==int(PickupReachD39::Assisted)?value:int(PickupReachD39::CloseBounds);
}
inline int getPickupReachD39() noexcept {return pickupReachD39.load(std::memory_order_acquire);}
inline void setPickupReachD39(int value) noexcept {
 pickupReachD39.store(clampPickupReachD39(value),std::memory_order_release);
}
inline constexpr double pickupRadiusD39(int slot,int side,int mode) noexcept {
 return !allowed(slot,side)?0.0:clampPickupReachD39(mode)==int(PickupReachD39::Assisted)?radius(slot,side):.03;
}
inline bool pickupContainsD39(int slot,int side,double gap,int mode) noexcept {
 return allowed(slot,side)&&std::isfinite(gap)&&gap>=0&&gap<=pickupRadiusD39(slot,side,mode)+Tolerance;
}
// D53: mode3 uses a single20cm palm-to-native-bounds selection region.
// This is only nomination; hand roles, native usability and action admission
// are still checked independently. It is not solid collision geometry.
inline constexpr double handsRadiusD53(int slot,int side) noexcept {
 return allowed(slot,side)?.20:0.;
}
inline bool handsContainsD53(int slot,int side,double gap) noexcept {
 return allowed(slot,side)&&std::isfinite(gap)&&gap>=0&&gap<=handsRadiusD53(slot,side)+Tolerance;
}
inline constexpr double SwitchMargin=.01;
}
