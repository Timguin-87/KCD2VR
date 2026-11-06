#pragma once
#include "../subsystems/smithing/roomscale_station_ds.h"
#include <atomic>
#include <cmath>
#include <cstdint>

namespace kcd2vr::station_body_h41 {
// Mode value is shared with minigame_hands45::SixDofVrik. This owner moves only
// the already qualified completed-pose pelvis/fixed legs, never CPlayer or its
// native physics capsule. The camera and tracked hand endpoints are unchanged.
inline constexpr int kMode=3;
struct Owner {
    std::uintptr_t character=0;
    std::uint64_t pose=0,tracking=0,control=0,mode=0,lifetime=0;
    std::uintptr_t instance=0;
    unsigned kind=0;
};
inline bool valid(const Owner& o) noexcept {
    return o.character&&o.pose&&o.tracking&&o.control&&o.mode&&o.lifetime&&o.instance&&(o.kind==1||o.kind==2);
}
inline bool same(const Owner& a,const Owner& b) noexcept {
    return valid(a)&&valid(b)&&a.character==b.character&&a.pose==b.pose&&a.tracking==b.tracking&&
        a.control==b.control&&a.mode==b.mode&&a.lifetime==b.lifetime&&a.instance==b.instance&&a.kind==b.kind;
}
struct State {
    Owner owner{};
    roomscale_station_ds::State root{};
    std::uint64_t sample=0,tick=0;
};
struct Input {
    Owner owner{};
    std::uint64_t sample=0,tick=0,now=0;
    double x=0,y=0,yaw=0;
    bool active=false;
};
struct Output {double x=0,y=0,yaw=0;bool active=false;};
inline Output advance(State& s,const Input& in) noexcept {
    if(!in.active||!valid(in.owner)||!in.sample||!in.tick||in.now<in.tick||in.now-in.tick>80||
       !std::isfinite(in.x)||!std::isfinite(in.y)||!std::isfinite(in.yaw)) {s={};return {};}
    const bool continuity=same(s.owner,in.owner)&&s.sample&&in.sample>=s.sample&&
        in.tick>=s.tick&&in.tick-s.tick<=80;
    if(!continuity)s={};
    if(!continuity||in.sample!=s.sample){
        // The completed-pose pelvis is a continuous presentation target, not
        // a native movement-command queue. Applying the command deadzones here
        // holds slow tracked motion then jumps the visible body by 3 cm / 0.02 rad.
        // Keep the activation origin and bounds, but advance every distinct pose.
        // Duplicate XR samples still cannot advance this state a second time.
        auto presentation=roomscale_station_ds::Config{};
        presentation.deadZone=0;presentation.yawDeadZone=0;
        (void)roomscale_station_ds::step(s.root,in.x,in.y,in.yaw,true,presentation);
        s.owner=in.owner;s.sample=in.sample;s.tick=in.tick;
    }
    return {s.root.appliedX,s.root.appliedY,s.root.appliedYaw,true};
}

// R9C (Miki 25 Oct, R8d2: "FREE 6DOF VRIK alchemy was not working - like no VRIK, no nothing").
// log: every station frame of both visits (16:45:35-16:46:35, 16:46:45-16:48) was refused at the write gate
// (ACQUISITION_REVOKED_BEFORE_WRITE line 803, CoherentOwnerCurrent57 station branch), body sample frozen at
// 232290/257706 for the whole visit. the ONLY frames that committed in the visit (16:47:38, 16:47:4x) had the left
// hand held (trackingMask 39) = no station owner -> the station branch is what kills VRIK there.
// the gate below names the failed piece (VRIK_F7 POSE stage=STATION_REFUSED_<why>) and after a short refusal streak
// the visit falls back to the FREE (Gesture) body: same tracked frame + hands, only the 6DOF root step is dropped.
enum class Refuse:unsigned {None=0,FrameInvalid,OwnerGone,Mode,Identity,Epoch,Tracking,Held,Context,NativeBody,Count};
inline const char* refuseName(Refuse r) noexcept {
    static const char* n[]{"NONE","FRAME_INVALID","OWNER_GONE","MODE","IDENTITY","EPOCH_POLICY_CAL","TRACKING_EPOCH","HAND_HELD","CONTEXT","NATIVE_BODY"};
    return unsigned(r)<unsigned(Refuse::Count)?n[unsigned(r)]:"UNKNOWN";
}
inline const char* stageName(Refuse r) noexcept {
    static const char* n[]{"STATION_OK","STATION_REFUSED_FRAME_INVALID","STATION_REFUSED_OWNER_GONE","STATION_REFUSED_MODE",
        "STATION_REFUSED_IDENTITY","STATION_REFUSED_EPOCH_POLICY_CAL","STATION_REFUSED_TRACKING_EPOCH","STATION_REFUSED_HAND_HELD",
        "STATION_REFUSED_CONTEXT","STATION_REFUSED_NATIVE_BODY"};
    return unsigned(r)<unsigned(Refuse::Count)?n[unsigned(r)]:"STATION_REFUSED_UNKNOWN";
}
// pure first-failure order of the existing gate (same conditions, just named)
struct GateInput {
    bool frameValid=false,ownerNow=false,modeOk=false,identityOk=false,epochOk=false,trackingOk=false,
         notHeld=false,contextOk=false,bodyOk=false;
};
constexpr Refuse classify(const GateInput& g) noexcept {
    return !g.frameValid?Refuse::FrameInvalid:!g.ownerNow?Refuse::OwnerGone:!g.modeOk?Refuse::Mode:
        !g.identityOk?Refuse::Identity:!g.epochOk?Refuse::Epoch:!g.trackingOk?Refuse::Tracking:
        !g.notHeld?Refuse::Held:!g.contextOk?Refuse::Context:!g.bodyOk?Refuse::NativeBody:Refuse::None;
}
inline constexpr unsigned kFallbackFrames=12; // ~130 ms of distinct XR samples
struct Fallback {
    std::uintptr_t instance=0;std::uint64_t lifetime=0,lastSample=0;unsigned streak=0;
    bool active=false,announced=false;Refuse why=Refuse::None;
};
// one write-gate result for the CURRENT visit (instance+lifetime). counts distinct samples only (the gate is re-read
// several times per transaction). a held hand is not a station failure (the station owner is off then anyway).
// returns true exactly once: the moment this visit falls back.
inline bool note(Fallback& f,std::uintptr_t instance,std::uint64_t lifetime,std::uint64_t sample,Refuse why) noexcept {
    if(!instance)return false;
    if(f.instance!=instance||f.lifetime!=lifetime){f={};f.instance=instance;f.lifetime=lifetime;}
    if(f.active)return false;
    if(why==Refuse::None){f.streak=0;f.lastSample=sample;return false;}
    if(why==Refuse::Held)return false;
    if(sample&&sample==f.lastSample)return false;
    f.lastSample=sample;
    if(++f.streak<kFallbackFrames)return false;
    f.active=true;f.why=why;return true;
}
inline bool fallbackFor(const Fallback& f,std::uintptr_t instance,std::uint64_t lifetime) noexcept {
    return f.active&&instance&&f.instance==instance&&f.lifetime==lifetime;
}
inline std::atomic<bool> fallbackEnabled{true}; // INI VrStation6DofFallback
inline Fallback fallback{};                     // pose owner only (g_pose_mutex6689)
inline std::atomic<unsigned> lastRefuse{0},fallbackWhy{0};
inline std::atomic<std::uint64_t> refusals{0},fallbackVisits{0};
inline std::atomic<bool> fallbackNow{false};    // readers outside the pose owner (recorder/F5 lines)
}
