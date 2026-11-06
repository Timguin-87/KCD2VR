#pragma once
// C43 (Miki 17 Oct): "first calibration is the king". The stored calibration (F8 T-pose arm profile, standing eye height,
// Henry's head anchor at calibration) is the single source of truth. Everything here is pure (host-tested); the runtime
// wiring lives in frik_pose_probe.cpp / camera_hook.cpp / mod.cpp.
//  a) every re-seat (recenter, session start, owner exit) corrects the vertical by (current eye height - calibrated
//     standing eye height) instead of taking the current headset as "Henry's neutral head" -> the view height stays the
//     calibrated one even when you recenter slouched or after a knockdown.
//  b) F4 OFF leaves the combat experiment only; the VRIK selection that was active before F4 comes back.
//  c) switching the menu to VRIK with a stored calibration applies it, no new T-pose.
#include <cmath>
#include <cstdint>
#include "user_modes175.h"

namespace kcd2vr::calibration_restore43 {
constexpr const char* kIniKey="VrCalibrationAutoRestore"; // [Settings], default 1
constexpr float kDriftReportM=.03f;      // receipt threshold: effective head anchor vs calibrated
constexpr float kMaxVerticalShiftM=1.2f; // sanity bound for the standing-eye correction (same as V66.107)
constexpr std::uint64_t kDebounceMs=500; // one restore per owner-exit burst

// Vertical correction for a re-seat: how far the headset is ABOVE the calibrated standing eye height right now.
// 0 when nothing is known; the caller subtracts it from the headset height it is about to adopt as neutral.
inline float verticalShift(bool autoRestore,bool floorTracking,float currentEyeM,float standingEyeM) noexcept {
    if(!autoRestore||!floorTracking||!std::isfinite(currentEyeM)||!std::isfinite(standingEyeM))return 0.f;
    if(standingEyeM<=.5f||currentEyeM<=.3f)return 0.f;
    const float shift=currentEyeM-standingEyeM;
    return std::fabs(shift)<kMaxVerticalShiftM?shift:0.f;
}
// Henry's head anchor (model-local) at calibration vs the one a re-seat would latch from the live pose.
inline float anchorDriftM(const float calibrated[3],const float live[3]) noexcept {
    const float dx=live[0]-calibrated[0],dy=live[1]-calibrated[1],dz=live[2]-calibrated[2];
    const float d=std::sqrt(dx*dx+dy*dy+dz*dz);return std::isfinite(d)?d:0.f;
}
// A stored head anchor is plausible model-local anatomy (Henry standing: head ~1.4-1.7 m above the model origin).
inline bool anchorPlausible(const float a[3]) noexcept {
    return std::isfinite(a[0])&&std::isfinite(a[1])&&std::isfinite(a[2])&&std::fabs(a[0])<.5f&&std::fabs(a[1])<.5f&&a[2]>1.f&&a[2]<2.f;
}
// Which anchor a re-seat keeps: the calibrated one whenever it exists and the live one is not wildly different (a
// different character/scale -> the live one, reported). Returns true when the calibrated anchor is kept.
inline bool keepCalibratedAnchor(bool haveCalibrated,const float calibrated[3],const float live[3]) noexcept {
    if(!haveCalibrated||!anchorPlausible(calibrated))return false;
    return anchorDriftM(calibrated,live)<.75f;
}
struct Debounce {std::uint64_t last=0;};
inline bool admit(Debounce& d,std::uint64_t now) noexcept {
    if(d.last&&now>=d.last&&now-d.last<kDebounceMs)return false;
    d.last=now;return true;
}

struct OwnerExit {unsigned prev=0;bool activityPending=false;};
struct ExitAction {bool restore=false,recenter=false;unsigned fell=0;};
inline ExitAction ownerExit(OwnerExit& s,unsigned owners) noexcept {
    ExitAction a{};const unsigned fell=s.prev&~owners,rose=owners&~s.prev;s.prev=owners;
    if(rose&2u)s.activityPending=false;
    if(fell&2u)s.activityPending=true;
    if(owners||(!fell&&!s.activityPending))return a;
    a.restore=true;a.recenter=s.activityPending;a.fell=fell|(s.activityPending?2u:0u);s.activityPending=false;
    return a;
}
inline const char* exitOwner(unsigned fell) noexcept {return fell&1u?"DIALOGUE_EXIT":fell&2u?"ACTIVITY_EXIT":"NATIVE_BODY_EXIT";}

// b) F4 cycle exit: back to the VRIK selection that was active before the first F4 press (Calibrated when that was a
// combat mode or OFF - F4 needs the calibrated body, so leaving the experiment never means "no body").
inline constexpr bool combatMode(user175::Mode m) noexcept {return user175::configuration(m).experiment;}
inline constexpr user175::Mode f4Exit(user175::Mode remembered) noexcept {
    return remembered==user175::Mode::Off||combatMode(remembered)?user175::Mode::Calibrated:remembered;
}
// The public (non-combat) selection behind the current runtime tuple; Off when none matches.
inline constexpr user175::Mode selectionFromTuple(int pose,int weapon,bool experiment) noexcept {
    if(experiment)return user175::Mode::Off;
    if((pose==3||pose==4)&&weapon==6)return user175::Mode::Calibrated;
    if(pose==5&&weapon==0)return user175::Mode::Hands;
    if(pose==4&&weapon==7)return user175::Mode::Weapons;
    if(pose==5&&weapon==7)return user175::Mode::WeaponsHands40;
    return user175::Mode::Off;
}
// C4R2 cycle with the C43 exit rule: OFF -> 1 SYNCHRONIZED -> 2 VR DIRECT -> (remembered selection).
inline constexpr user175::Mode f4Next(bool experiment,bool synchronized,bool direct,user175::Mode remembered) noexcept {
    const auto next=user175::hybridCurrent19c4r2(experiment,synchronized,direct);
    return next==user175::Mode::Off||next==user175::Mode::Weapons?f4Exit(remembered):next; // C44: Codex exit = Weapons; the mod.cpp handler uses hybridCurrent19c4r2 directly, this helper only keeps the pure rule for tests
}
// c) menu VRIK: a stored calibration (INI arm profile) is applied; only without one a T-pose is asked for.
inline constexpr bool needTpose(bool calibrationStored) noexcept {return !calibrationStored;}
}
