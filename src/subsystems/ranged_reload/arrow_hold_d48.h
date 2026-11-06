#pragma once
#include <atomic>

namespace kcd2vr::arrow_hold_d48 {
// D56 adopts the user's tested native orientation once. Subsequent explicit
// stable-grip choices remain saved. This does not change arrow/ammo ownership.
// Native creation, lifetime, attachment identity and slt_0 nock anchoring remain
// with the existing manual-arrow owner.
inline constexpr bool LoadNativeTurnD56(int adopted,bool saved) noexcept {
    return adopted>=1?saved:true;
}
inline std::atomic<bool> nativeTurn{true};
}
