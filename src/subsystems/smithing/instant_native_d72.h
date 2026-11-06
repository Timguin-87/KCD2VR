#pragma once
#include "../../core/snapshot_mailbox.h"
#include "../../vrik/instant_activities_d72.h"
#include "../../vrik/minigame_hands45.h"
#include "source_grip_h42.h"
#include <atomic>
namespace kcd2vr::subsystems::smithing::instant_d72 {
// The already-qualified ForgePump publishes its physical downstroke, not a
// native action or heat value. The exact native process callback consumes it.
struct Pump {
    source_h42::Source handle{};
    std::uint64_t lifetime=0,poseEpoch=0,sourceEpoch=0,modeEpoch=0,sample=0,tick=0;
    bool admitted=false,pressed=false,manual=false;
};
inline core::SnapshotMailbox<Pump> pump;
inline std::atomic<bool> ready{false};
inline bool route() noexcept {
    return ready.load(std::memory_order_acquire)&&kcd2vr::instant_d72::Crafting()&&
        minigame_hands45::modeFor(minigame_hands45::Smithing)==minigame_hands45::SixDofVrik;
}
inline bool current(const Pump& p,std::uintptr_t instance,std::uint64_t life,
    std::uint64_t mode,std::uint64_t now) noexcept {
    return p.admitted&&!p.manual&&p.handle.instance==instance&&p.lifetime==life&&
        p.modeEpoch==mode&&p.poseEpoch&&p.sourceEpoch&&p.sample&&p.tick&&now>=p.tick&&now-p.tick<=80&&
        p.handle.slot==2&&p.handle.side==0&&p.handle.phase==8&&p.handle.entity&&
        p.handle.holder&&p.handle.player&&p.handle.model&&p.handle.geometryKnown&&
        p.handle.geometryEntity==p.handle.entity&&p.handle.geometryType==source_h42::Geometry::ForgeHandle;
}
}
