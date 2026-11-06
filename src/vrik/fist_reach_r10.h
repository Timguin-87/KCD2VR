#pragma once
// R10 fists (Claude lead, Miki 25 Oct: "the fists should be Henry's reach, with collisions like a weapon").
// no stretching. the native punch still owns the attack (prepare/release, damage, block, stagger, reaction), but the
// ONLY thing its hit sweep may test is where the VR fist really was:
//  - while the VR fist is valid a native sweep we can't put on the VR fist does not run (R8d2: the animated bone hit
//    the NPC from 40-60 cm in front of the real fist = the "invisible punch"). VrFistVrOnlyHits.
//  - the native hit window opens later than a fast VR punch reaches its end (refusals FIST_RETRACTION / old pair /
//    other gesture while the window was open). the farthest segment of THIS punch is remembered and replayed once
//    into that window, so a real touch at real reach still becomes the native hit. VrFistPeakReplay.
// kSkinM: our own hand collision stops the display fist ON the skin; the replayed segment may go that far past it.
#include "pose_math.h"
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstring>

namespace kcd2vr::fist_reach_r10 {
namespace m=pose_core;
inline std::atomic<bool> vrOnlyHits{true};  // VrFistVrOnlyHits
inline std::atomic<bool> peakReplay{true};   // VrFistPeakReplay (H2H 27 Oct: on, narrowed - see below)
inline constexpr double kSkinM=.03;
// H2H (Miki 27 Oct, J27 log 00:15-00:18: 34 native punches, 21 started a native attack, only 9 of those hit). with R27 the
// native window runs on its own clock again (no pre-peak hold); a fast jab can still be past its farthest point when the
// window opens, then the only segment the native sweep sees is the fist going BACK. the replay is now narrow: the peak
// must be of THIS admitted punch, at most 150 ms old (was 600), used once, and only when the live pair is unusable or
// clearly retracting behind that peak. a touch at real reach becomes the native hit; nothing longer than the real arm.
inline constexpr std::uint64_t kPeakMaxAgeMs=150;
inline constexpr double kRetractM=.02;          // live fist at least 2 cm behind the peak along the punch direction
inline std::atomic<unsigned long long> suppressed{0},replayed{0},current{0},nativeAllowed{0};

inline bool finite3(m::Vec3 v) noexcept {return std::isfinite(v.x)&&std::isfinite(v.y)&&std::isfinite(v.z);}

// the farthest point THIS punch reached along its own direction (one per side, keyed by the punch's gesture sample).
struct Peak {std::uint64_t gesture=0,tick=0,sample=0;m::Vec3 origin{},from{},to{};double along=-1;bool valid=false;};
// one call per published fist sample. returns true when this sample is the new peak.
inline bool track(Peak& p,std::uint64_t gesture,bool pair,m::Vec3 from,m::Vec3 to,m::Vec3 dir,std::uint64_t sample,std::uint64_t tick) noexcept {
    if(!gesture){p={};return false;}
    if(p.gesture!=gesture){p={};p.gesture=gesture;}
    if(!pair||!finite3(from)||!finite3(to)||!finite3(dir))return false;
    const double n=m::length(dir);if(!(n>1e-6))return false;
    const m::Vec3 d{dir.x/n,dir.y/n,dir.z/n};
    if(p.along<0&&!p.valid)p.origin=from;
    if(!(m::dot(to-from,d)>.001))return false;          // advancing only: the way back is not a strike
    const double along=m::dot(to-p.origin,d);
    if(p.valid&&!(along>p.along))return false;
    p.along=along;p.from=from;p.to=to;p.sample=sample;p.tick=tick;p.valid=true;return true;
}
// replay is allowed once per punch, while the peak is young.
inline bool usable(const Peak& p,std::uint64_t gesture,std::uint64_t now,std::uint64_t spentGesture) noexcept {
    return p.valid&&gesture&&p.gesture==gesture&&spentGesture!=gesture&&now>=p.tick&&now-p.tick<=kPeakMaxAgeMs;
}
// H2H: the live segment goes back (toward the body) and already sits behind the punch's farthest point.
inline bool retracting(const Peak& p,m::Vec3 from,m::Vec3 to,m::Vec3 dir) noexcept {
    if(!p.valid||!finite3(from)||!finite3(to)||!finite3(dir))return false;
    const double n=m::length(dir);if(!(n>1e-6))return false;
    const m::Vec3 d{dir.x/n,dir.y/n,dir.z/n};
    return m::dot(to-from,d)<0&&m::dot(to-p.origin,d)<p.along-kRetractM;
}
// the replayed end point: the real fist position plus at most the skin allowance along its own motion.
inline m::Vec3 replayEnd(m::Vec3 from,m::Vec3 to) noexcept {
    const auto d=to-from;const double n=m::length(d);
    if(!(n>1e-6))return to;
    return {to.x+d.x/n*kSkinM,to.y+d.y/n*kSkinM,to.z+d.z/n*kSkinM};
}
// prediction refusals that mean "the game/system owns this sweep" (not a VR punch, VR fist not tracked, rig not
// ready): native runs as before. every other refusal happens while the VR fist IS valid -> the animated bone must
// not hit in its place.
inline bool nativeMayHit(const char* why) noexcept {
    if(!why)return true;
    for(const char* k:{"FIST_NATIVE_CALLER_DT","FIST_RETAIL_CONTRACT","FIST_CURRENT_GAMEPLAY_OWNER","FIST_ADMITTED_ACTION",
                       "FIST_HAND_IDENTITY","FIST_EMPTY_NATIVE_OWNER","FIST_CURRENT_TRACKING"})
        if(!std::strcmp(why,k))return true;
    return false;
}
// what the prediction hook does with a refused fist sweep.
enum class Sweep:int {Native=0,Suppress=1};
inline Sweep refused(const char* why,bool vrOnly) noexcept {return vrOnly&&!nativeMayHit(why)?Sweep::Suppress:Sweep::Native;}
// the contact admission accepts the replayed (older) fist frame up to the peak age, the live one up to 60 ms.
inline std::uint64_t palmAgeLimitMs(bool replay) noexcept {return replay?kPeakMaxAgeMs:60;}
}
