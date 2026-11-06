#pragma once
#include <atomic>
namespace kcd2vr::knife_pouch_d66 {
// Location only: both choices use the same finite LEFT-hand native service.
// OFF preserves the accepted left-chest route and all ordinary holsters.
inline std::atomic<bool> rightHip{false};
inline bool rightHipSetting() noexcept { return rightHip.load(std::memory_order_acquire); }
inline void setRightHip(bool value) noexcept { rightHip.store(value,std::memory_order_release); }
inline const char* label(bool value) noexcept { return value?"RIGHT_HIP":"LEFT_CHEST"; }
// R109: VrKnifeChestZoneR109. the draw zone was moved to the left hip in D85R5 while the
// visible dagger (VrChestDagger) still sits on the chest, so you reach where the knife is
// and nothing happens. ON = the empty left hand also draws at the chest pouch (where the
// dagger shows). the left hip zone always stays. OFF = left hip only.
inline std::atomic<bool> chestZoneR109{true};
inline bool chestZoneSetting() noexcept { return chestZoneR109.load(std::memory_order_acquire); }
inline void setChestZone(bool value) noexcept { chestZoneR109.store(value,std::memory_order_release); }
}
