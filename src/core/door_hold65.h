#pragma once
#include "native_door19c4.h"
#include "snapshot_mailbox.h"
#include <cmath>

// Copied Mod/pose/native-worker messages only. These do not authorize a native
// Use, resolve entities or write animation state. Lua owns its existing door.
namespace kcd2vr::door_hold65 {
struct Hand {
    bool available=false;int side=-1;float grip=0;
    std::uint64_t tick=0,poseTick=0,press=0,poseEpoch=0,trackingEpoch=0;
    std::uint64_t sourceGeneration=0,settingsEpoch=0,experimentEpoch=0;
    std::uintptr_t player=0;door19c4::Point point{};
};
struct Frame {Hand hand[2]{};};
struct Operation {
    door19c4::Request request{};Hand source{};std::uint64_t tick=0;
    bool paused=false; // requested control mode; Lua rate readback is the achieved pause proof
};
struct Resume {
    std::uint64_t serial=0;std::uint32_t target=0;std::uintptr_t entity=0;
    Hand hand{};
};
inline core::SnapshotMailbox<Frame> frame;
inline core::SnapshotMailbox<Operation> operation;
inline core::SnapshotMailbox<Resume> resume;
inline bool fresh(std::uint64_t tick,std::uint64_t now) noexcept {
    return tick&&now>=tick&&now-tick<=80;
}
inline bool current(const Hand& h,std::uint64_t now) noexcept {
    return h.available&&h.side>=0&&h.side<2&&h.player&&h.poseEpoch&&h.trackingEpoch&&
        h.sourceGeneration&&h.settingsEpoch&&fresh(h.tick,now)&&fresh(h.poseTick,now)&&
        std::isfinite(h.grip)&&h.grip>=0&&h.grip<=1&&door19c4::finite(h.point);
}
inline bool sameSource(const Hand& a,const Hand& b) noexcept {
    return a.side==b.side&&a.player==b.player&&a.poseEpoch==b.poseEpoch&&
        a.trackingEpoch==b.trackingEpoch&&a.sourceGeneration==b.sourceGeneration&&
        a.settingsEpoch==b.settingsEpoch&&a.experimentEpoch==b.experimentEpoch;
}
inline bool current(const Operation& o,std::uint64_t now) noexcept {
    return o.request.serial&&o.request.id&&o.request.id<=16777215u&&o.request.entity&&fresh(o.tick,now)&&
        current(o.source,now)&&o.request.side==o.source.side&&o.request.player==o.source.player&&
        o.request.epoch==o.source.poseEpoch&&o.request.sourceGeneration==o.source.sourceGeneration&&
        o.request.trackingEpoch46==o.source.trackingEpoch&&o.request.inputSettingsEpoch46==o.source.settingsEpoch&&
        o.request.inputExperimentEpoch65==o.source.experimentEpoch;
}
inline bool resumeMatches(const Resume& r,const Operation& o,const Hand& h,std::uint64_t now) noexcept {
    return o.paused&&current(o,now)&&current(h,now)&&current(r.hand,now)&&
        r.serial==o.request.serial&&r.target==o.request.id&&r.entity==o.request.entity&&
        sameSource(r.hand,o.source)&&sameSource(r.hand,h)&&r.hand.press&&
        r.hand.press==h.press&&r.hand.grip>=.30f&&h.grip>=.30f;
}
}
