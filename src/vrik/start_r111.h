#pragma once
// R111 (Miki 5 Nov): "the game should always start in VRIK mode once calibrated once" and "the calibration head
// height should always pass - sitting or standing, the start is Henry's eyes, not the HMD height".
// pure state + the per-load decision, host tested (tests/start_vrik_r111_test.cpp). the mod loop (mod.cpp) runs it.
//  - vrikOnStart (VrVrikOnStartR111, default ON): a saved F8 calibration ([VRIK.ArmCalibration]) switches VRIK on at
//    every game start and after every save load. there was NO saved VRIK on/off key before R111: the mode lived only
//    in memory (g_census_mode=0 at start), so every start was native until the menu VRIK button was pressed.
//    switching VRIK off in the menu/F10 is respected until the next save load (one decision per load episode).
//  - henryEyes (VrStartAtHenryEyesR111, default ON): every recenter (game start, save load, menu/OS recenter) takes
//    the headset where it is NOW as Henry's eye point. the C43/V66.107 standing-eye correction (current eye height -
//    F8 standing eye height) is not applied, so a seated player does not start 50 cm below Henry's eyes. the head
//    still moves Henry relative to that start (crouch, lean, stand up). a save load also recenters once.
// no latch: a refused start retries every 2 s while the world is up; a new load is a new decision.
//
// R112 (Miki 5 Nov R111 log: "VRIK was stranger - the back weapon was seen by every swing coming over my head
// forward, henry was also stepping forward... like we lost something with HMD height"):
// the R111 automatic recenter fired 1.5 s after the world came up and took the headset WHEREVER it was as Henry's
// eyes. 21:36:26.009 it took a head 42 cm forward / 8 cm up (he fixed it with the Oculus recenter 1.25 s later,
// 21:36:27.260), 21:41:09.803 (save load) a head 18 cm to the side: for the rest of the log Henry's pelvis stood
// 13-18 cm beside the camera and the VRIK head missed its target by 5-9 cm instead of 1-2 (V66.122 CAMERA_BODY).
// a camera that is not in Henry's head shows his shoulders/back, the stowed weapon and the native swing + attack
// step from outside. now the automatic recenter (game start VRIK, save load) waits until the headset is where YOU
// put your runtime center (OpenXR LOCAL space, set by the Oculus/SteamVR recenter): steady 0.5 s, within 15 cm
// sideways/forward and 12 cm up/down of it, facing its forward within 25 deg. if that does not happen within 10 s
// of world time, the origin you already have is kept (no jump). the menu VRIK button, RESTORE and the headset
// recenter still recenter at once. no latch: every load / game start decides again.
#include <atomic>
#include <cmath>
#include <cstdint>

namespace kcd2vr::start_r111 {
inline std::atomic<bool> vrikOnStart{true};
inline std::atomic<bool> henryEyes{true};
inline std::atomic<bool> worldReady{false}; // mod loop: ordinary gameplay frame, no menu/dialogue/cutscene/UI owner
constexpr std::uint64_t kSettleMs=1500; // world must be up this long after a load before we act
constexpr std::uint64_t kRetryMs=2000;  // a refused VRIK start tries again this often
// R112: automatic recenter gate (LOCAL space = the player's own runtime center)
constexpr float kCenterSideM=.15f;      // head within this horizontal distance of the runtime center
constexpr float kCenterUpM=.12f;        // and within this height of it
constexpr float kCenterYawDeg=25.f;     // and looking along its forward
constexpr float kSteadyMoveM=.02f;      // steady = moved less than this ...
constexpr float kSteadyYawDeg=5.f;      // ... and turned less than this ...
constexpr std::uint64_t kSteadyMs=500;  // ... for this long
constexpr std::uint64_t kRecenterWindowMs=10000; // no qualifying head in this much world time: keep the origin

enum class Act : unsigned {None,Wait,StartVrik,Recenter,KeepOrigin};
struct Episode {std::uint64_t done=0,readySince=0,nextTry=0,recenterSince=0,seen=0;unsigned tries=0;bool recenterDue=false;};

// head yaw (deg) around +Y of an OpenXR orientation; 0 = the runtime's forward (-Z)
inline float yawDeg(float qx,float qy,float qz,float qw) noexcept {
    const float s=2.f*(qw*qy+qx*qz),c=1.f-2.f*(qx*qx+qy*qy);
    return std::atan2(s,c)*57.2957795f;
}
inline float yawDelta(float a,float b) noexcept {float d=std::fmod(a-b+540.f,360.f)-180.f;return std::fabs(d);}
// the headset is where the player set his runtime center (LOCAL space origin), facing its forward
inline bool atRuntimeCenter(float x,float y,float z,float yaw) noexcept {
    if(!std::isfinite(x)||!std::isfinite(y)||!std::isfinite(z)||!std::isfinite(yaw))return false;
    return std::sqrt(x*x+z*z)<=kCenterSideM&&std::fabs(y)<=kCenterUpM&&yawDelta(yaw,0.f)<=kCenterYawDeg;
}
// steady for kSteadyMs (any bigger motion restarts the window)
struct Steady {float x=0,y=0,z=0,yaw=0;std::uint64_t since=0;bool have=false;};
inline bool steady(Steady& s,float x,float y,float z,float yaw,std::uint64_t now) noexcept {
    if(!std::isfinite(x)||!std::isfinite(y)||!std::isfinite(z)||!std::isfinite(yaw)){s.have=false;return false;}
    const float dx=x-s.x,dy=y-s.y,dz=z-s.z;
    if(!s.have||now<s.since||std::sqrt(dx*dx+dy*dy+dz*dz)>kSteadyMoveM||yawDelta(yaw,s.yaw)>kSteadyYawDeg){
        s.x=x;s.y=y;s.z=z;s.yaw=yaw;s.since=now;s.have=true;return false;}
    return now-s.since>=kSteadyMs;
}

// one call per mod loop tick. episode = observed load episode (inventory_lifecycle_t8::loadEpisode18).
// the caller sets e.done=episode after the episode's work is over (finished()), and when the player picked a
// VRIK mode himself in this episode (manual()). after a successful StartVrik the caller calls started(): VRIK is on,
// the recenter that goes with it waits for headReady (headset steady at the runtime center) like the load recenter.
inline Act decide(Episode& e,std::uint64_t episode,bool loading,bool world,std::uint64_t now,bool vrikSetting,
                  bool eyesSetting,bool calibrated,bool vrikOn,bool calibrating,bool headReady=true) noexcept {
    if(loading||!world){e.readySince=0;e.recenterSince=0;return Act::None;}
    if(e.seen!=episode){e.seen=episode;e.recenterDue=false;e.recenterSince=0;} // a new load decides from scratch
    if(e.done==episode)return Act::None;
    if(!e.readySince||now<e.readySince){e.readySince=now?now:1;return Act::Wait;}
    if(now-e.readySince<kSettleMs||calibrating)return Act::Wait;
    if(vrikSetting&&calibrated&&!vrikOn&&!e.recenterDue){
        if(e.nextTry&&now<e.nextTry)return Act::Wait;
        e.nextTry=now+kRetryMs;++e.tries;return Act::StartVrik;
    }
    if(eyesSetting||e.recenterDue){
        if(!e.recenterSince||now<e.recenterSince)e.recenterSince=now?now:1;
        if(headReady)return Act::Recenter;
        return now-e.recenterSince>=kRecenterWindowMs?Act::KeepOrigin:Act::Wait;
    }
    e.done=episode;return Act::None;
}
inline void started(Episode& e) noexcept {e.recenterDue=true;e.tries=0;} // VRIK on; only its recenter is left this episode
inline void finished(Episode& e,std::uint64_t episode) noexcept {e.done=episode;e.nextTry=0;e.tries=0;e.recenterDue=false;e.recenterSince=0;}
inline void manual(Episode& e,std::uint64_t episode) noexcept {finished(e,episode);}

// the vertical correction a recenter applies (C43 / V66.107): none when "start at Henry's eyes" is on.
inline float recenterShift(bool eyesSetting,float standingShift) noexcept {return eyesSetting?0.f:standingShift;}
}
