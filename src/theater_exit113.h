#pragma once
#include <cstdint>

namespace kcd2vr::theater_exit113 {
// R59C2: projection and Theater are exclusive (since V118R1). The caller now
// qualifies APSE/Shop with observed native camera selection. This adapter only
// clears the retired producer-count wait; it does not promise a fresh pixel
// boundary. Drain only while Theater itself owns the flat menu.
inline bool drain(bool nativeTheater, bool waiting) { (void)waiting; return nativeTheater; }

inline bool update(bool enabled, bool nativeTheater, int applied,
                   std::uint64_t completed, bool& waiting, std::uint64_t& baseline) {
    (void)enabled; (void)applied; (void)completed;
    waiting=false;
    baseline=0;
    return nativeTheater;
}
}
