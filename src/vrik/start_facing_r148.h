#pragma once
// R148 (Miki 8 Nov: "with SteamVR I still need to recenter after the start, because the game starts behind me").
// the mod's base space is OpenXR LOCAL (xr_manager.cpp CreateReferenceSpace). the camera origin (yaw + position) is
// taken from the FIRST head pose after the XR session begins (camera_hook.cpp V66.125 kind=INITIAL), i.e. during the
// intro / main menu, wherever the headset is at that moment (on the desk, untracked, or you not yet in place).
// a save load only re-anchors through the R112 automatic recenter, and that waits for the head to sit steady at the
// runtime center: within 15 cm of the LOCAL origin and within 25 deg of LOCAL forward.
//  - VDXR: LOCAL is recentred to you when the session starts, so the head IS at the runtime center -> R112 recenters.
//  - SteamVR: LOCAL is the SteamVR seated zero (kept across sessions, can be anywhere in the room and face anywhere),
//    so the head is never "at the runtime center" -> R112 skips after 10 s ("origin kept") and the start origin from
//    the main menu stays: the game forward can be behind you until you recenter by hand.
// now: while that automatic recenter waits (same switch VrStartAtHenryEyesR111, same episode, same F8 calibration
// guard), a head that is TRACKED, steady 0.5 s and faces more than 90 deg away from the current camera forward also
// recenters (= the manual recenter). 90 deg: a seated neck turn does not reach it, so on VDXR (origin already right)
// nothing changes. no latch: every save load / game start decides again; no pose yet = keep waiting (10 s window).
#include "start_r111.h"
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstring>

namespace kcd2vr::start_facing_r148 {
inline std::atomic<bool> headTracked{false}; // xr_manager: latest head locate had ORIENTATION_TRACKED (false while stopped)
inline char runtimeName[128]{};              // xr_manager: written once at instance creation, before the session
inline std::atomic<bool> runtimeKnown{false};
constexpr float kAwayYawDeg=90.f;            // head yaw vs camera forward beyond this = the origin points away from you

inline void setRuntime(const char* name) noexcept {
    if(!name)name="<unknown>";
    std::strncpy(runtimeName,name,sizeof(runtimeName)-1);runtimeName[sizeof(runtimeName)-1]=0;
    runtimeKnown.store(true,std::memory_order_release);
}
inline const char* runtime() noexcept {return runtimeKnown.load(std::memory_order_acquire)?runtimeName:"<unknown>";}
// yaw (deg) of the camera origin quaternion (yaw-only, see tracking_origin125.h), same axis as start_r111::yawDeg
inline float originYawDeg(float qx,float qy,float qz,float qw) noexcept {return start_r111::yawDeg(qx,qy,qz,qw);}
// the head faces away from the camera forward the start/load kept: recenter like the manual recenter
inline bool facingAway(bool tracked,bool steady,float headYaw,float originYaw) noexcept {
    if(!tracked||!steady||!std::isfinite(headYaw)||!std::isfinite(originYaw))return false;
    return start_r111::yawDelta(headYaw,originYaw)>kAwayYawDeg;
}

// R148 part 2 (Miki 8 Nov: "the game is starting wrong there... the start intro and menu... like before flipped, now
// not to be seen without recenter"). the intro / main menu / pause screen is the automatic Theater quad in LOCAL space,
// placed in front of the head pose captured when it opens (xr_manager.cpp SetFlatUiMode, flat_ui_anchor_pose_; the
// mod sets it at mod start, mod.cpp SetNativeMenuPlaneMode(true)). it is captured from the FIRST usable pose and then
// kept. on SteamVR the first poses come before you look into the headset (upside down ones are dropped since R108 =
// "before flipped"; the next upright one can be the headset on the desk / not tracked yet) -> the screen sits where
// the headset pointed then = "not to be seen". only a runtime recenter (reference space change) re-captured it.
// now: while the automatic Theater quad is up, a TRACKED head that looks roughly level (pitch within 35 deg) and more
// than 60 deg away from the screen for 1.5 s re-places the screen in front of you (same capture as on open). looking
// down at the controllers/lap does not count. an anchor taken right after YOUR runtime recenter is never moved
// (until the next screen opens). no latch: every screen open / session start decides again.
constexpr float kUiAwayDeg=60.f;
constexpr float kUiLevelPitchDeg=35.f;
constexpr std::uint64_t kUiAwayMs=1500;
struct Fwd {float x=0,y=0,z=-1;};
// OpenXR forward (-Z) of an orientation, normalized
inline Fwd forwardOf(float qx,float qy,float qz,float qw) noexcept {
    Fwd f{-2.f*(qx*qz+qw*qy),-2.f*(qy*qz-qw*qx),-(1.f-2.f*(qx*qx+qy*qy))};
    const float n=std::sqrt(f.x*f.x+f.y*f.y+f.z*f.z);
    if(!(n>1e-4f)||!std::isfinite(n))return Fwd{NAN,NAN,NAN};
    f.x/=n;f.y/=n;f.z/=n;return f;
}
inline float angleDeg(Fwd a,Fwd b) noexcept {
    const float d=a.x*b.x+a.y*b.y+a.z*b.z;
    if(!std::isfinite(d))return NAN;
    return std::acos(d>1.f?1.f:d<-1.f?-1.f:d)*57.2957795f;
}
inline float pitchDeg(Fwd f) noexcept {return std::isfinite(f.y)?std::asin(f.y>1.f?1.f:f.y<-1.f?-1.f:f.y)*57.2957795f:NAN;}
// one call per mod tick while the screen is up. awaySince = 0 when not looking away. true = re-place the screen now.
inline bool uiLookAway(std::uint64_t& awaySince,bool on,bool userAnchor,bool tracked,float angle,float headPitch,std::uint64_t now) noexcept {
    if(!on||userAnchor||!tracked||!std::isfinite(angle)||!std::isfinite(headPitch)||angle<=kUiAwayDeg||std::fabs(headPitch)>kUiLevelPitchDeg){
        awaySince=0;return false;}
    if(!awaySince||now<awaySince){awaySince=now?now:1;return false;}
    return now-awaySince>=kUiAwayMs;
}
} // namespace kcd2vr::start_facing_r148
