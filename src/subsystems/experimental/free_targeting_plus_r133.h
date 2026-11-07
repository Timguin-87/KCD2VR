#pragma once
// R133 free targeting mode (Miki 7 Nov: "free targeting is still a lot restrictive, camera moving too much
// circling the enemy ... so the target can be more to side, not necessary in the middle").
//
//  0 NATIVE      the game's own lock and combat camera (the old VrFreeTargeting=false)
//  1 FREE        the D42/R103 free targeting: combat camera leased off, the queued "turn self toward
//                opponent" command vetoed in the ordinary phases, plus a wider target cone (R133)
//  2 FREE PLUS   1 + the view and the tracked body stay where they are in the world when the game turns
//                Henry toward the target (the yaw is absorbed into a view offset, the stick is turned by
//                the same offset so walking does not become a circle), widest target cone.
//
// Why mode 2 is needed (R125 F3 log, 17:59:48-18:02:48): in 3 minutes of fighting the queued turn command
// ran only 45 times for Henry, 42 were vetoed (henryVetoed=42, ORDINARY_MOTION:3). the rest of the turning
// toward the enemy is the game's locked locomotion (Henry's root keeps facing the opponent while he walks,
// stick left/right = strafe around him). that route has no hook; mode 2 does not fight it, it takes it out
// of the view instead: the root may turn, your head, hands and the stick do not turn with it.
//
// Everything here is pure (no native reads); the owners publish into these atomics:
//  camera thread  -> offsetView / offsetBody / bleed (the hold below runs inside the camera hook)
//  pad thread     -> userTurnMs (right stick turn deflection), reads offsetView / bleed for the stick
//  C_CombatPlayer -> opponentMs (Henry's S_CombatActorState+0xCF8 opponent is set)
//  main loop      -> contextReady (free_targeting_d42::contextReady), the one-row receipts
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
namespace kcd2vr::free_targeting_r133 {
enum : int {Native=0,Free=1,FreePlus=2};
constexpr int kDefaultMode=1; // FREE PLUS is not headset proven yet (R133), so the proven mode 1 stays default
inline int clampMode(int v) noexcept {return v<Native?Native:v>FreePlus?FreePlus:v;}
inline int cycle(int v) noexcept {return (clampMode(v)+1)%3;}
inline const char* name(int v) noexcept {
    switch(clampMode(v)){case Native:return "NATIVE";case Free:return "FREE";default:return "FREE_PLUS";}
}
// INI: VrFreeTargetingModeR133 wins when it holds 0..2. Otherwise the old bool VrFreeTargeting is read as
// before (true -> 1, false -> 0). neither key -> kDefaultMode. oldPresent is found by reading the bool
// twice with opposite defaults (the caller does that).
inline int resolveIni(int newValue,bool oldPresent,bool oldValue) noexcept {
    if(newValue>=Native&&newValue<=FreePlus)return newValue;
    if(oldPresent)return oldValue?Free:Native;
    return kDefaultMode;
}

// Target cone per mode (wh_cs_Player* lease values, R52/R57 owner in mod.cpp CombatCvarLease52).
// lockArea = wh_cs_PlayerInputLockAreaWidth: lateral distance (m, X axis of the view) from the opponent
// within which looking at him selects him; unlock distance = lockArea + unlockHyst (the game's own help
// text: "PlayerInputLockAreaWidth + PlayerInputUnlockAreaWidthHyst gives unlock distance"). native 0.4/0.2,
// R57 0.6. angles = wh_cs_PlayerMaxOpponentAngleToLock/Unlock (rad).
struct Cone {float lockAngle,unlockAngle,lockArea,unlockHyst;bool hyst;};
inline Cone cone(int mode) noexcept {
    switch(clampMode(mode)){
    case Native:return {1.5708f,2.4f,.6f,.2f,false};   // today's R52/R57 values, hysteresis stays native
    case Free:return {1.5708f,2.4f,1.0f,.3f,true};     // ~2x the R57 area: the target may sit off centre
    default:return {1.5708f,1.75f,2.0f,.5f,true};      // FREE PLUS: ~65-70 deg to the side at 2 m still selects,
    }                                                  // lost past ~2.5 m lateral or 100 deg
}

inline double wrap(double a) noexcept {
    if(!std::isfinite(a))return 0;
    a=std::remainder(a,6.283185307179586);
    return a;
}
// Cry yaw of a horizontal forward vector (forward = (-sin yaw, cos yaw)), the camera_hook convention.
inline double yawOf(double fx,double fy) noexcept {return std::atan2(-fx,fy);}

// ---- view/body hold (camera thread only) ----
// offsetView rotates the final camera basis; offsetBody rotates the tracking->model frame of the VR body
// (head, hands, weapon, room walking). Each absorbs its own reference: the native camera yaw for the view,
// Henry's root yaw for the body, so they stay right even if the native camera and the root turn apart.
struct Hold {
    double view=0,body=0;          // offsets (rad)
    double lastCamera=0,lastRoot=0;
    bool haveCamera=false,haveRoot=false;
    double absorbedDeg=0;          // this fight, for the receipt
    double maxOffsetDeg=0;
    std::uint64_t bleedSince=0,lastEngaged=0;
    double bleedStartAbs=0;int bleedSign=-1;bool flipped=false;
    std::uint32_t fights=0,clears=0,bleeds=0;
};
struct Input {
    bool modePlus=false;      // mode == 2
    bool context=false;       // plain tracked gameplay (no menu/dialogue/cutscene/native body), tracked camera base
    bool engaged=false;       // Henry has a combat opponent (recent)
    bool userTurn=false;      // the player is turning with the stick (or a snap pulse) right now
    bool reset=false;         // recenter / load / pose epoch change
    bool haveRoot=false;
    double cameraYaw=0,rootYaw=0;
    std::uint64_t now=0;
};
enum class Step : unsigned {Off,Pass,Absorb,Bleed,Cleared};
constexpr double kJumpRad=.35;          // > 20 deg in one frame = teleport/scripted, never absorbed
constexpr double kDoneRad=.0175;        // 1 deg: offset is gone
constexpr std::uint64_t kBleedMaxMs=4000;
inline void clear(Hold& h) noexcept {
    h.view=h.body=0;h.bleedSince=0;h.flipped=false;h.bleedSign=-1;
}
// One call per rendered camera apply. Calling it twice with the same yaws is a no-op (delta 0).
inline Step step(Hold& h,const Input& in) noexcept {
    if(!in.modePlus||in.reset){
        const bool had=h.view!=0||h.body!=0;clear(h);h.haveCamera=h.haveRoot=false;
        return had?Step::Cleared:Step::Off;
    }
    const double dc=h.haveCamera?wrap(in.cameraYaw-h.lastCamera):0;
    const double dr=(h.haveRoot&&in.haveRoot)?wrap(in.rootYaw-h.lastRoot):0;
    h.lastCamera=in.cameraYaw;h.haveCamera=true;
    if(in.haveRoot){h.lastRoot=in.rootYaw;h.haveRoot=true;} // a frame without a root read keeps the last one: the next delta covers the gap
    if(in.engaged)h.lastEngaged=in.now;
    const bool offset=std::fabs(h.view)>kDoneRad||std::fabs(h.body)>kDoneRad;
    if(!in.context){ // keep the offset through a menu/clinch in the fight, never absorb outside plain play
        h.bleedSince=0;
        if(offset&&!in.engaged&&h.lastEngaged&&in.now>=h.lastEngaged&&in.now-h.lastEngaged>kBleedMaxMs){clear(h);++h.clears;return Step::Cleared;}
        return Step::Pass;
    }
    const bool jump=std::fabs(dc)>kJumpRad||std::fabs(dr)>kJumpRad;
    if(in.engaged){
        h.bleedSince=0;
        if(in.userTurn||jump)return Step::Pass;
        h.view=wrap(h.view-dc);h.body=wrap(h.body-dr);
        h.absorbedDeg+=std::fabs(dc)*57.29577951308232;
        h.maxOffsetDeg=std::max(h.maxOffsetDeg,std::fabs(h.view)*57.29577951308232);
        return Step::Absorb;
    }
    if(!offset){if(h.view!=0||h.body!=0)clear(h);return Step::Pass;}
    // after the fight: our stick input turns Henry back under the view (absorbed, so the view stays),
    // the offset runs out. a bleed that does not shrink within 4 s ends with one clear (one snap).
    if(!h.bleedSince){h.bleedSince=in.now?in.now:1;h.bleedStartAbs=std::fabs(h.view);++h.bleeds;}
    if(in.now>=h.bleedSince&&in.now-h.bleedSince>kBleedMaxMs){clear(h);++h.clears;return Step::Cleared;}
    if(!in.userTurn&&!jump){h.view=wrap(h.view-dc);h.body=wrap(h.body-dr);}
    // wrong turning sign on this machine: |offset| grew by 5 deg -> flip once for this bleed
    if(!h.flipped&&std::fabs(h.view)>h.bleedStartAbs+.087){h.bleedSign=-h.bleedSign;h.flipped=true;h.bleedStartAbs=std::fabs(h.view);}
    // done when the VIEW offset is gone; a body rest (camera and root turned a little apart) goes with it
    if(std::fabs(h.view)<=kDoneRad){clear(h);return Step::Pass;}
    return Step::Bleed;
}
// right stick X the pad sends while bleeding (rx<0 = turn left = yaw up in Cry). 0 when not bleeding.
inline float bleedRx(double view,int sign) noexcept {
    if(std::fabs(view)<=kDoneRad)return 0;
    const double mag=std::clamp(std::fabs(view)*1.1,.18,.55);
    return float((view>0?1.:-1.)*sign*mag);
}
// The game moves Henry relative to its own camera yaw N; the player sees N+offset. Turning the stick by the
// offset makes "forward" the forward he sees, so a locked strafe is a straight sidestep, not a circle.
inline void rotateStick(float& x,float& y,double offset) noexcept {
    if(std::fabs(offset)<=kDoneRad)return;
    const double c=std::cos(offset),s=std::sin(offset);
    const double nx=c*x-s*y,ny=s*x+c*y;
    x=float(std::clamp(nx,-1.,1.));y=float(std::clamp(ny,-1.,1.));
}
// z-yaw quaternion multiply: out = Rz(phi) * q  (x,y,z,w)
inline void yawFrame(float q[4],double phi) noexcept {
    if(std::fabs(phi)<=1e-6)return;
    const double s=std::sin(phi*.5),c=std::cos(phi*.5);
    const double x=q[0],y=q[1],z=q[2],w=q[3];
    // (0,0,s,c) * (x,y,z,w)
    q[0]=float(c*x-s*y);q[1]=float(c*y+s*x);q[2]=float(c*z+s*w);q[3]=float(c*w-s*z);
}

// ---- cross-thread publication ----
inline std::atomic<int> mode{kDefaultMode};
inline std::atomic<std::uint64_t> modeGeneration{1};
inline std::atomic<float> offsetView{0.f},offsetBody{0.f};
inline std::atomic<int> bleedSign{0};          // 0 = no bleed request, else +-1 (see bleedRx)
inline std::atomic<std::uint64_t> userTurnMs{0},opponentMs{0};
inline std::atomic<std::uint64_t> rootYawTick{0};inline std::atomic<float> rootYaw{0.f};
inline std::atomic<std::uint64_t> absorbFrames{0},stickFrames{0},bleedFrames{0};
inline std::atomic<float> absorbedDeg{0.f},maxOffsetDeg{0.f};
inline std::atomic<std::uint32_t> fights{0},clears{0};
inline bool engaged(std::uint64_t now) noexcept {
    const auto t=opponentMs.load(std::memory_order_acquire);return t&&now>=t&&now-t<=1500;
}
inline bool userTurning(std::uint64_t now) noexcept {
    const auto t=userTurnMs.load(std::memory_order_acquire);return t&&now>=t&&now-t<=250;
}
}
