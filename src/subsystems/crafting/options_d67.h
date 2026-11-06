#pragma once
#include <atomic>
namespace kcd2vr::crafting_d67 {
// Recovery repairs input lifecycle only. Native actions/effects stay native.
inline std::atomic<bool> smithRecovery{true};
// LT anvil assist (VrSmithSemiAnchorsD67). D85 craft: ON, the one default
// shared with the INI reader, the D81 repair and the D72 profile.
inline std::atomic<bool> smithSemiAnchors{true};
inline void setSmithRecovery(bool v) noexcept {smithRecovery.store(v,std::memory_order_release);}
inline void setSmithSemiAnchors(bool v) noexcept {smithSemiAnchors.store(v,std::memory_order_release);}
}
