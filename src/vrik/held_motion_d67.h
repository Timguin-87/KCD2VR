#pragma once
#include <atomic>
// D67 changes only the existing checked held-weapon candidate. No native
// semantic state, damage, camera, input, or additional pose writer.
namespace kcd2vr::held_motion_d67 {
inline std::atomic<bool> gripAnchorSetting{true}, arcCoverageSetting{true}, supportReleaseSetting{true}, contactFairnessSetting{true};
inline bool gripAnchored() noexcept { return gripAnchorSetting.load(std::memory_order_relaxed); }
inline void setGripAnchored(bool value) noexcept { gripAnchorSetting.store(value,std::memory_order_relaxed); }
inline bool currentArcCoverage() noexcept { return arcCoverageSetting.load(std::memory_order_relaxed); }
inline void setCurrentArcCoverage(bool value) noexcept { arcCoverageSetting.store(value,std::memory_order_relaxed); }
inline bool supportRelease() noexcept { return supportReleaseSetting.load(std::memory_order_relaxed); }
inline void setSupportRelease(bool value) noexcept { supportReleaseSetting.store(value,std::memory_order_relaxed); }
inline bool contactFairness() noexcept { return contactFairnessSetting.load(std::memory_order_relaxed); }
inline void setContactFairness(bool value) noexcept { contactFairnessSetting.store(value,std::memory_order_relaxed); }
// Existing motor inertia remains, but a basis change cannot bypass it through
// the compliance projection. This limit applies only to a released support
// grasp, around the primary grip; the offhand has already become free.
inline constexpr double supportReleaseRate=8.0; // rad/s; 5.0 degrees at90Hz
}
