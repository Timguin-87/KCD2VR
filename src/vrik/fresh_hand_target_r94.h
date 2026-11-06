#pragma once
#include "core/snapshot_mailbox.h"
#include <cstdint>
#include <limits>

namespace kcd2vr::fresh_hands_r94 {
// Read together: an independent atomic target/time pair can straddle xrWaitFrame.
// This observes its prediction horizon; it does not change the camera target.
struct Anchor {std::int64_t target=0,observed=0,period=0;std::uint64_t epoch=0;};
inline core::SnapshotMailbox<Anchor,4> anchors;
inline bool Target(const Anchor& a,std::int64_t now,std::uint64_t epoch,std::int64_t& target) noexcept {
    target=0;
    if(!epoch||a.epoch!=epoch||a.target<=0||a.observed<=0||a.period<=0||
       a.period>100000000||now<a.observed)return false;
    const auto phase=now-a.observed;
    // predictedDisplayTime is the runtime's forecast, not an observation age.
    // A valid 50ms forecast at90Hz exceeds four periods; rejecting it starved
    // the selected independent hand family even while every locate was fresh.
    // Keep the existing anchor-age bound and still obtain NEW tracked poses.
    if(phase>4*a.period||a.target<a.observed-a.period||
       a.target>std::numeric_limits<std::int64_t>::max()-phase)return false;
    // Advance continuously between worker frames, using a new runtime locate.
    // No integer frame-step prediction, averaging or controller low-pass delay.
    target=a.target+phase;
    return target>0;
}
}
