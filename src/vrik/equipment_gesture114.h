#pragma once
#include "pose_math.h"
#include <cstdint>
#include <cmath>
#include <algorithm>

namespace kcd2vr::equipment114 {

// Only this empty hand's qualified combat chord owns its holster gesture.
// Opposite-hand Grip/fist input is independent. Held equipment still releases
// normally; after a fist cancels acquisition, the existing neutral Grip rearm
// is required before another draw.
inline bool CombatFistOwnsHandH39(bool knownEmpty,double grip,double trigger) noexcept {
    return knownEmpty&&std::isfinite(grip)&&std::isfinite(trigger)&&
        grip>double(.60f)&&trigger>double(.60f);
}
enum class Request { None, Draw, Holster };
struct State {
    std::uint64_t key=0,generation=0,epoch=0,tick=0,started=0,sent=0;
    bool initialized=false,neutral=false,latched=false,pending=false,wantDrawn=false;
    bool releasedDuringDraw=false,blocked=false;
    bool requireReleaseR15=false; // consumed/cancelled modern Grip cannot reacquire until release
    bool heldWhileDrawn136=false;std::uint64_t releaseSince136=0,outsideSince136=0;
    pose_core::Vec3 origin{};
};
struct Input {
    std::uint64_t key=0,generation=0,epoch=0,tick=0;
    pose_core::Vec3 hand{},mount{};
    double grip=0;
    bool eligible=false,drawn=false,mountKnown=false;
    bool zoneGripR15=false; // current controller holsters; retained comparison keeps its original cycle
    bool instantZoneDrawD44=false; // explicitly selected modern haptic-zone Grip policy
    bool dispatchReadyD44=true; // transient coordination/read contention is not an ownership cancellation
    // Current zone radius and release policy. An accepted Draw remembers a
    // release before native acknowledgement, so an open hand never needs a
    // second squeeze merely to dispose of the item when it finally arrives.
    double radius=.20;bool releaseAnywhere=false;std::uint64_t releaseDebounceMs=120;
    std::uint64_t stickyOutsideMs=250;
};
// Cross-item acquisition exclusion must use the same current identity/age and
// native acknowledgement as update(). A removed/reordered/expired item must
// not retain a global draw lock just because its update() was skipped.
inline bool PendingDrawBlocks(const State& s,std::uint64_t key,std::uint64_t generation,
                              std::uint64_t epoch,std::uint64_t now,bool drawn) noexcept {
    return s.initialized&&s.pending&&s.wantDrawn&&!drawn&&key&&
        s.key==key&&s.generation==generation&&s.epoch==epoch&&
        now>=s.tick&&s.tick&&now-s.tick<=100&&now>=s.sent&&now-s.sent<2000;
}
// Current same-item held acquisition is a gesture owner only. It never grants
// tracking, native equipment eligibility or a contact/pose lease.
inline bool HeldAcquisitionR15(const State& s,std::uint64_t key,std::uint64_t generation,
                              std::uint64_t epoch,std::uint64_t now,double grip) noexcept {
    return s.initialized&&s.key==key&&s.generation==generation&&s.epoch==epoch&&
        key&&s.tick&&now>=s.tick&&now-s.tick<=100&&std::isfinite(grip)&&grip>.55&&
        !s.blocked&&((s.latched&&!s.wantDrawn)||PendingDrawBlocks(s,key,generation,epoch,now,false));
}
inline void CancelAcquisitionR15(State& s,double grip) noexcept {
    s={};s.requireReleaseR15=std::isfinite(grip)&&grip>=.25;
}
// A ring/consumer rejection proves THIS request never entered native code.
// It may retire the local pending bit; entered jobs never use this adapter.
// Preserve a continuously held, current draw latch for next-sample admission,
// while an observed release ends it immediately. No native action is replayed.
inline bool RejectUnenteredD48(State& s,std::uint64_t key,std::uint64_t generation,
                              std::uint64_t epoch,std::uint64_t sent,bool draw,double grip) noexcept {
    if(!s.initialized||!s.pending||s.key!=key||s.generation!=generation||s.epoch!=epoch||
       s.sent!=sent||s.wantDrawn!=draw)return false;
    const bool held=std::isfinite(grip)&&grip>.55;
    const bool released=std::isfinite(grip)&&grip<.25;
    s.pending=false;s.blocked=false;s.wantDrawn=false;s.releasedDuringDraw=false;s.sent=0;
    s.latched=draw&&held;s.neutral=released;s.requireReleaseR15=!held&&!released;
    if(!draw)s.heldWhileDrawn136=true; // release disposition was refused before entry
    return true;
}
// Modern zone Grip acquires immediately when explicitly selected by its producer.
// Legacy comparison retains its 12 cm pull and 80 ms dwell unchanged.
// One request, then native hand-key acknowledgement. A returned native call is
// never an acknowledgement. Failed requests require a fresh grip cycle.
inline Request update(State& s,const Input& i) noexcept {
    if(!i.eligible||!i.key||!i.tick||!std::isfinite(i.grip)||
       !pose_core::finite(i.hand)||(i.mountKnown&&!pose_core::finite(i.mount))){
        // A refused candidate has not consumed a Grip. Preserve rearm only
        // for an actual acquired/requested/cancelled cycle, not idle eligibility.
        if(i.zoneGripR15&&(s.latched||s.pending||s.blocked||s.requireReleaseR15))
            CancelAcquisitionR15(s,i.grip);
        else s={};
        return Request::None;}
    if(!s.initialized||s.key!=i.key||s.generation!=i.generation||s.epoch!=i.epoch||
       i.tick<s.tick||(s.tick&&i.tick-s.tick>100)) {
        // A publication gap or idle item refresh has not consumed a Grip.
        // Retire stale acquisition, but require release only for a cycle that
        // actually latched, issued, failed, or was explicitly cancelled.
        const bool releaseRequired=i.zoneGripR15&&
            (s.requireReleaseR15||s.latched||s.pending||s.blocked);
        // First native instantiation may change the item's generation. A
        // current same-key native drawn fact can still complete this pending
        // Draw's release disposition; it grants no physical/contact ownership.
        const bool releasedDraw=s.initialized&&s.pending&&s.wantDrawn&&s.key==i.key&&s.epoch==i.epoch&&
            i.drawn&&(s.releasedDuringDraw||i.grip<.25)&&i.tick>=s.sent&&i.tick-s.sent<2000;
        s={};s.requireReleaseR15=releaseRequired;s.initialized=true;s.key=i.key;s.generation=i.generation;s.epoch=i.epoch;
        s.heldWhileDrawn136=releasedDraw;
    }
    if(s.tick==i.tick)return Request::None;
    s.tick=i.tick;
    const bool released=i.grip<.25,held=i.grip>.55;
    if(released)s.requireReleaseR15=false;
    if(s.pending){
        // D48: Grip intent and an entered native obligation are separate.
        // Keep the release disposition until a real drawn fact arrives. Native
        // consumers cancel only requests that have not entered the game.
        if(s.wantDrawn&&released)s.releasedDuringDraw=true;
        if(i.drawn==s.wantDrawn){
            if(s.wantDrawn&&s.releasedDuringDraw)s.heldWhileDrawn136=true;
            s.pending=false;s.blocked=false;s.releasedDuringDraw=false;s.latched=false;
        }
        else if(i.tick-s.sent<2000)return Request::None;
        else {s.pending=false;s.blocked=true;s.neutral=false;s.latched=false;s.releasedDuringDraw=false;return Request::None;}
    }
    if(s.blocked){
        if(released)s.neutral=true;
        if(held&&s.neutral){s.blocked=false;}
        else return Request::None;
    }
    auto request=[&](Request r){s.pending=true;s.wantDrawn=r==Request::Draw;s.sent=i.tick;
        s.latched=false;s.neutral=false;s.releasedDuringDraw=false;
        if(i.zoneGripR15)s.requireReleaseR15=true;return r;};
    if(i.drawn){
        // V66.131 Skyrim-style stow: ordinary Grip release keeps the native item
        // drawn. Holster only after the held hand deliberately enters its own
        // current belt/shoulder zone, dwells briefly, and releases there.
        const double R=std::isfinite(i.radius)?std::clamp(i.radius,.10,.45):.20;
        if(held){s.heldWhileDrawn136=true;s.releaseSince136=0;}
        // The producer supplies the selected release policy. A pending Draw
        // that was released uses this same disposition after native acknowledgement;
        // it never manufactures an acknowledgement or retries a native request.
        if(i.releaseAnywhere&&released&&s.heldWhileDrawn136){
            if(!s.releaseSince136)s.releaseSince136=i.tick;
            if(i.tick>=s.releaseSince136+i.releaseDebounceMs){s.heldWhileDrawn136=false;s.releaseSince136=0;return request(Request::Holster);}
            return Request::None;
        }
        if(!released)s.releaseSince136=0;
        // Sticky Skyrim-style stow. Once the held weapon spends a short dwell in
        // its actual body-space zone, keep the candidate through normal hand/body
        // motion and through a transient mount-read miss. Cancel only after the
        // held hand stays clearly outside the zone for a bounded interval. This
        // directly matches the headset evidence: 13 cm latched, then 27 cm 96 ms
        // later before the Grip release edge arrived.
        double gap=1.0e9;
        if(i.mountKnown)gap=pose_core::length(i.hand-i.mount);
        if(held&&i.mountKnown&&gap<R){
            if(!s.latched){s.latched=true;s.started=i.tick;s.origin=i.hand-i.mount;}
            s.outsideSince136=0;
        } else if(held&&s.latched&&i.mountKnown&&gap>R+.12){
            if(!s.outsideSince136)s.outsideSince136=i.tick;
            if(i.tick>=s.outsideSince136&&i.tick-s.outsideSince136>i.stickyOutsideMs){s.latched=false;s.outsideSince136=0;}
        } else if(held&&s.latched&&i.mountKnown) s.outsideSince136=0;
        if(released){
            const bool dwell=s.latched&&i.tick>=s.started+50;
            const bool recentlyOutside=!s.outsideSince136||i.tick<s.outsideSince136||i.tick-s.outsideSince136<=i.stickyOutsideMs;
            const bool stow=dwell&&recentlyOutside;
            s.latched=false;s.outsideSince136=0;s.neutral=true;s.heldWhileDrawn136=false;
            if(stow)return request(Request::Holster);
        }
        return Request::None;
    }
    // A neutral hand may close Grip before reaching the mount. The producer
    // separately excludes this hand's qualified Trigger+Grip combat fist.
    if(!i.mountKnown){
        if(released){s.neutral=true;s.latched=false;s.outsideSince136=0;}
        else if(i.zoneGripR15&&s.latched){
            if(!s.outsideSince136)s.outsideSince136=i.tick;
            if(i.tick-s.outsideSince136>i.stickyOutsideMs){s.latched=false;s.neutral=false;s.requireReleaseR15=true;}
        }
        return Request::None;
    }
    const auto relative=i.hand-i.mount;
    const double gap=pose_core::length(relative);
    const double R=std::isfinite(i.radius)?std::clamp(i.radius,.10,.45):.20;
    if(released){s.neutral=true;s.latched=false;return Request::None;}
    // A confirmed free holster can be grasped with Grip already closed. Native
    // eligibility/ownership and the acquisition volume remain producer-owned.
    // A consumed or cancelled cycle still requires actual release.
    if(held&&(s.neutral||(i.zoneGripR15&&!s.requireReleaseR15))&&!s.latched&&gap<R){
        s.latched=true;s.started=i.tick;s.origin=relative;s.outsideSince136=0;
    }
    if(i.zoneGripR15&&s.latched){
        if(gap>R+.12){if(!s.outsideSince136)s.outsideSince136=i.tick;}
        else s.outsideSince136=0;
        if(!held||(s.outsideSince136&&i.tick-s.outsideSince136>i.stickyOutsideMs)){
            s.latched=false;s.neutral=false;s.requireReleaseR15=!released;
        }
    }else if(s.latched&&(!held||i.tick-s.started>2000)){s.latched=false;s.neutral=false;}
    // C10R2: lifting the mace can travel 12 cm while still inside the configured
    // 28 cm sphere. Requiring exit beyond radius+6 cm made that grasp a no-op.
    // Keep neutral Grip, dwell and pull, and require outward progress so
    // moving a closed hand inward or across the body is not a draw. Both vectors
    // are body-relative, so ordinary world locomotion supplies no pull distance.
    const double startGap=pose_core::length(s.origin);
    // Modern haptic-ready holsters acquire on Grip in the confirmed zone. The
    // native equipment consumer still validates/attaches the item; this emits
    // only one request and never manufactures the acknowledgement. Dispatch
    // contention waits with current input rather than consuming a failed draw.
    if(s.latched&&i.dispatchReadyD44&&
       ((i.zoneGripR15&&i.instantZoneDrawD44)||
        (i.tick-s.started>=80&&gap>startGap+.02&&pose_core::length(relative-s.origin)>.12)))
        return request(Request::Draw);
    return Request::None;
}
} // namespace kcd2vr::equipment114
