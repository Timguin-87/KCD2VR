#pragma once
// One monotonic producer timestamp per tracking sample. Milliseconds remain the
// legacy/native freshness clock; never compare them directly with nanoseconds.
#include <cstdint>
#include <algorithm>
namespace kcd2vr::motion_time {
inline double elapsed(std::uint64_t oldMs,std::uint64_t newMs,
                      std::uint64_t oldNs,std::uint64_t newNs) noexcept {
    if (oldNs || newNs) {
        if (!oldNs || !newNs || newNs < oldNs) return -1.0;
        return double(newNs-oldNs)*1e-9;
    }
    return newMs >= oldMs ? double(newMs-oldMs)*.001 : -1.0;
}
inline bool discontinuity(std::uint64_t oldMs,std::uint64_t newMs,
                          std::uint64_t oldNs,std::uint64_t newNs) noexcept {
    const double dt=elapsed(oldMs,newMs,oldNs,newNs);
    return dt < 0.0 || dt > .100000001;
}
// Held-body integration may use the time for which XR located a pose. Existing
// achieved-motion consumers use its resulting velocities. Input, action,
// contact occurrence and freshness owners retain elapsed() above.
// Unavailable or discontinuous target clocks preserve the delivery-time path.
inline double target_elapsed(std::int64_t oldTarget,std::int64_t newTarget,
                             std::uint64_t oldEpoch,std::uint64_t newEpoch) noexcept {
    if(oldTarget<=0||newTarget<=oldTarget||!oldEpoch||oldEpoch!=newEpoch)return -1.;
    const auto delta=newTarget-oldTarget;
    return delta<=100000001?double(delta)*1e-9:-1.;
}
}
