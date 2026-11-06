#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>
#include "pose_math.h"
#include "kick_profile52.h" // C52 (Miki 18 Oct): native-timing / old leg profile, target aim, reach clamp

// Procedural presentation/contact probe only. No native input, actor properties,
// target selection, damage, stamina, or attack admission lives in this kernel.
namespace kcd2vr::kick176 {
using Vec3=pose_core::Vec3;
struct Request {std::uint64_t serial=0,tick=0,cancel=0;
    bool haveTarget=false;Vec3 targetWorld{};float targetDistance=0; // C52: opponent belly (world) read once at the R3 click
};
enum class Status : unsigned {Idle, Lift, Extend, Recover, Complete, Cancelled,
    StaleRequest, Unavailable, InvalidPose, PoseRejected};
inline const char* name(Status s) noexcept {
    switch(s){case Status::Lift:return "KNEE_UP";case Status::Extend:return "EXTEND";
    case Status::Recover:return "RECOVER";case Status::Complete:return "COMPLETE";
    case Status::Cancelled:return "CANCELLED";case Status::StaleRequest:return "STALE_REQUEST";
    case Status::Unavailable:return "LEGS_OR_CONTEXT_UNAVAILABLE";
    case Status::InvalidPose:return "INVALID_POSE";case Status::PoseRejected:return "POSE_REJECTED";
    default:return "IDLE";}
}
enum class ContactKind : unsigned {Unknown, Clear, Hit};
struct Contact {
    ContactKind kind=ContactKind::Unknown;
    Vec3 point{},normal{};
    std::uint32_t entity=0;
    int part=-1;
    unsigned body=0,queries=0;
    bool bodyOwner=false;
};
struct State {
    bool active=false,havePreviousWorld=false,contactObserved=false;
    int side=0,nextSide=0;
    std::uint64_t seenRequest=0,serial=0,cancel=0,epoch=0,started=0,lastTick=0,lastSample=0;
    Vec3 forward{},startOffset{},previousWorld{};
    double reach=0;
    kick52::Profile profile=kick52::Profile::NativeTiming; // C52: fixed at the start of the kick
    bool haveTarget=false;Vec3 target{};double reachScale=kick52::kDefaultReachScale;
};
struct Input {
    Request request{};
    std::uint64_t tick=0,now=0,sample=0,cancel=0,epoch=0;
    bool eligible=false,legsReady=false;
    int forceSide=-1; // C26: 0 right, 1 left, -1 alternate (KICKS page)
    Vec3 hip[2]{},ankle[2]{},forward{};
    double reach[2]{};
    // C52: profile + aim (KICKS page). target = opponent belly in MODEL space (converted by the pose callback at the
    // start of the kick); without it the kick goes straight forward as before.
    kick52::Profile profile=kick52::Profile::NativeTiming;
    bool haveTarget=false;Vec3 target{};double reachScale=kick52::kDefaultReachScale;
};
struct Frame {
    bool active=false,started=false,terminal=false,newSample=false;
    int side=0;
    std::uint64_t serial=0;
    Status status=Status::Idle;
    Vec3 target{},pole{},forward{};
    double leanRadians=0;
    std::uint64_t elapsedMs=0;bool aimed=false,clamped=false; // C52: receipt data (target used / reach or yaw clamped)
};
// C34b (Miki: "only a knee strike, no reach - make it look like a real kick"): a front push kick. The knee chambers
// up and forward, the foot SNAPS out almost straight at belly height (88 % of the leg length), holds a beat on the
// target, then comes back. Old pose: foot .75 forward / .72 up from the resting ankle = a bent knee strike.
// C52 (Miki 18 Oct): the keyframes moved to kick_profile52.h. Default = NATIVE TIMING (the game's 1.1667 s kick clip:
// chamber 0.3 s, snap to the hit at 0.733 s, foot down at 0.98 s), aimed at the opponent; OLD = the C34b 110/200/300/520.
inline constexpr std::uint64_t kLiftEndMs=110,kSnapEndMs=200,kHoldEndMs=300,kCompleteMs=520; // OLD profile (kick52::kOld)
inline double ease(double t) noexcept {t=std::clamp(t,0.0,1.0);return t*t*(3-2*t);}
inline Vec3 lerp(Vec3 a,Vec3 b,double t) noexcept {return a+(b-a)*ease(t);}
inline bool good(Vec3 v) noexcept {return pose_core::finite(v);}
inline Status statusOf(kick52::Phase p) noexcept {
    switch(p){case kick52::Phase::Chamber:return Status::Lift;case kick52::Phase::Extend:case kick52::Phase::Hold:return Status::Extend;
    case kick52::Phase::Recover:case kick52::Phase::Settle:return Status::Recover;default:return Status::Complete;}
}
inline Frame advance(State& s,const Input& in) noexcept {
    Frame out{};out.serial=s.active?s.serial:0;out.side=s.side;
    const bool changed=s.cancel!=in.cancel||s.epoch!=in.epoch;
    if(changed){s.active=false;s.havePreviousWorld=false;s.cancel=in.cancel;s.epoch=in.epoch;}
    const bool freshRequest=in.request.serial>s.seenRequest;
    const auto reject=[&](Status status){out.status=status;out.terminal=out.serial!=0;
        s.active=false;s.havePreviousWorld=false;return out;};
    if(!in.eligible||!in.sample||!in.tick){
        if(freshRequest){out.serial=in.request.serial;s.seenRequest=in.request.serial;}
        return reject(Status::Cancelled);
    }
    if(s.active&&freshRequest)s.seenRequest=in.request.serial; // active edges are dropped, never queued
    if(s.active&&(in.tick<s.lastTick||in.tick-s.lastTick>100||in.sample<s.lastSample))return reject(Status::Cancelled);
    if(!s.active&&freshRequest){
        out.serial=in.request.serial;
        if(in.request.cancel!=in.cancel||!in.request.tick||!in.now||in.now<in.request.tick||
           in.now-in.request.tick>100||in.tick>in.now){
            s.seenRequest=in.request.serial;return reject(Status::StaleRequest);
        }
        // R3 may arrive between tracking publication and this pose callback.
        // Retain the original edge until a sample reaches it, bounded by wall
        // time (never by a stale/repeated callback's timestamp).
        if(in.tick<in.request.tick){out.serial=0;return out;}
        s.seenRequest=in.request.serial;
        if(!in.legsReady)return reject(Status::Unavailable);
        const int side=in.forceSide==0||in.forceSide==1?in.forceSide:s.nextSide; // C26
        Vec3 f=in.forward;f.z=0;const double length=pose_core::length(f);
        if(!good(f)||length<.25||!good(in.hip[side])||!good(in.ankle[side])||
           !std::isfinite(in.reach[side])||in.reach[side]<.5||in.reach[side]>1.4)
            return reject(Status::InvalidPose);
        s.serial=in.request.serial;s.side=side;s.started=in.tick;s.lastSample=0;
        s.reach=in.reach[side];s.forward=f*(1/length);s.startOffset=in.ankle[side]-in.hip[side];
        s.profile=in.profile;s.reachScale=kick52::clampReachScale(in.reachScale); // C52: fixed for this kick
        s.haveTarget=in.haveTarget&&good(in.target);s.target=s.haveTarget?in.target:Vec3{};
        s.active=true;s.havePreviousWorld=false;s.contactObserved=false;out.started=true;
    }
    if(!s.active)return out;
    out.serial=s.serial;out.side=s.side;
    if(!in.legsReady||!good(in.hip[s.side])||!good(in.ankle[s.side]))return reject(Status::InvalidPose);
    const bool duplicate=in.sample==s.lastSample;
    const auto elapsed=(duplicate?s.lastTick:in.tick)-s.started;
    if(elapsed>=kick52::completeMs(s.profile)){s.nextSide=1-s.side;return reject(Status::Complete);}
    out.newSample=!duplicate;
    if(!duplicate){s.lastSample=in.sample;s.lastTick=in.tick;}
    // C52: the ankle keyframes come from the profile (kick_profile52.h). The trajectory is anchored to the CURRENT
    // native hip (locomotion moves it), the direction/target were fixed at the start (looking aside does not steer).
    const Vec3 hip=in.hip[s.side];
    kick52::Geometry g{};g.hip=hip;g.restAnkle=hip+s.startOffset;g.forward=s.forward;g.reach=s.reach;g.reachScale=s.reachScale;
    g.haveTarget=s.haveTarget;g.target=s.target;
    const auto pose=kick52::evaluate(s.profile,g,elapsed,in.ankle[s.side]);
    if(!good(pose.ankle)||!good(pose.pole)||!std::isfinite(pose.lean))return reject(Status::InvalidPose);
    out.target=pose.ankle;out.status=statusOf(pose.phase);out.elapsedMs=elapsed;out.aimed=s.haveTarget;out.clamped=pose.clamped;
    // A bounded visual counter-lean. This never translates pelvis/camera or changes
    // the tracked hand targets. The production arm solve runs after this rotation.
    out.leanRadians=pose.lean;
    out.pole=pose.pole;out.forward=pose.dir;out.active=true;
    return out;
}
}
