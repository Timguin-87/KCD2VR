#pragma once
#include <atomic>
#include <cstdint>
namespace kcd2vr::weapon_trials_d69 {
// Explicit experiments: OFF retains the accepted D69 producer/consumer paths.
inline std::atomic<bool> sameFrameSetting{false}, rigidGripSetting{false};
inline std::atomic<std::uint64_t> sameFrameRevision{1};
inline bool sameFrame() noexcept { return sameFrameSetting.load(std::memory_order_acquire); }
inline bool rigidGrip() noexcept { return rigidGripSetting.load(std::memory_order_acquire); }
inline void setSameFrame(bool value) noexcept {
 if(sameFrameSetting.exchange(value,std::memory_order_acq_rel)!=value)
  sameFrameRevision.fetch_add(1,std::memory_order_acq_rel);
}
inline void setRigidGrip(bool value) noexcept { rigidGripSetting.store(value,std::memory_order_release); }
}
