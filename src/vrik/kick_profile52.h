#pragma once
// C52 (Miki 18 Oct): "the VR kick is not realistic, look at the game kick animation".
// Pure leg-pose profile for the R3 kick (no native call, no ABI). This is a
// hand-authored visual curve referenced to ONE normal-kick table row, not a
// decoded retail ankle curve or a universal native hit clock. Other free/dog
// kick rows have different timing and remain adjudicated by the native game.
// The NATIVE TIMING reference profile uses the normal game
// kick clip (Codex asset discovery 18 Oct): combat_action_attack row CombatAttackGen aZ5 kick, duration 1.1667 s,
// attack_time_to_start 0.3 s (chambered), attack_time_to_hit 0.7333 s; male.animevents of the blendspace
// 1d-combat_rg_sz5_az5_natk_kick_nwpn: footstep 0.06, footstep (plant) 0.23, foley foleyam 0.485 + c_kick_whoosh
// 0.42..0.50 (= the extension), foleyaq 0.77 (retract), footstep 0.82 (foot down). Times here are ms after the clip start.
// The OLD profile is the C34b hand-made 110/200/300/520 ms knee/snap/hold/recover, kept for comparison.
// The kick aims at the opponent (belly point handed in at the click): the foot travels forward at hip height toward
// him, scaled by distance (the blendspace's short/medium/long), clamped to leg reach. The knee comes from the existing
// two-bone leg IK (SolveLeg128) - this header only says where the ankle is at each moment. No root/camera move.
#include <algorithm>
#include <cmath>
#include <cstdint>
#include "pose_math.h"
namespace kcd2vr::kick52 {
using Vec3=pose_core::Vec3;
enum class Profile : int {NativeTiming=0, Old=1};
inline Profile clampProfile(int p) noexcept {return p==1?Profile::Old:Profile::NativeTiming;}
inline const char* name(Profile p) noexcept {return p==Profile::Old?"OLD_110_200_300":"NATIVE_ROW_REFERENCE_1167";}
struct Timing {std::uint64_t chamberEnd=0,extendStart=0,hit=0,holdEnd=0,footDown=0,complete=0;};
inline constexpr Timing kNative{300,500,733,830,980,1167};
inline constexpr Timing kOld{110,110,200,300,520,520};
inline constexpr Timing timing(Profile p) noexcept {return p==Profile::Old?kOld:kNative;}
inline constexpr std::uint64_t hitMs(Profile p) noexcept {return timing(p).hit;}           // foot on the target
inline constexpr std::uint64_t completeMs(Profile p) noexcept {return timing(p).complete;}
inline constexpr double kMinReachScale=.60,kMaxReachScale=1.0,kDefaultReachScale=.92;      // menu slider (VrKickReach52 %)
inline double clampReachScale(double s) noexcept {return std::isfinite(s)?std::clamp(s,kMinReachScale,kMaxReachScale):kDefaultReachScale;}
inline constexpr double kYawLimitRadians=50.0*3.14159265358979323846/180.0;                // a front kick, not a side kick
inline constexpr double kSoleAheadMeters=.06;                                               // ankle sits this far short of the target
inline constexpr double kLeanRadians=5.0*3.14159265358979323846/180.0;
enum class Phase : unsigned {Chamber, Extend, Hold, Recover, Settle, Done};
struct Geometry {
    Vec3 hip{},restAnkle{},forward{},target{};  // model space; forward = horizontal unit; target = opponent belly
    bool haveTarget=false;
    double reach=0,reachScale=kDefaultReachScale; // reach = thigh + shin
};
struct Pose {Vec3 ankle{},pole{},dir{};Phase phase=Phase::Done;double lean=0;bool contactWindow=false,clamped=false;};
inline double smooth(double t) noexcept {t=std::clamp(t,0.0,1.0);return t*t*(3-2*t);}
inline double snap(double t) noexcept {t=std::clamp(t,0.0,1.0);const double u=1-t;return 1-u*u*u;} // fast out, decelerates into the target
inline Vec3 mix(Vec3 a,Vec3 b,double e) noexcept {return a+(b-a)*e;}
inline bool good(Vec3 v) noexcept {return pose_core::finite(v);}
inline double len2d(Vec3 v) noexcept {return std::sqrt(v.x*v.x+v.y*v.y);}
// Horizontal kick direction: toward the target, limited to +-50 deg from the body forward; forward without a target.
inline Vec3 kickDirection(const Geometry& g,bool& clampedYaw) noexcept {
    clampedYaw=false;Vec3 f=g.forward;f.z=0;const double fl=len2d(f);if(!(fl>1e-6))return {0,1,0};f=f*(1/fl);
    if(!g.haveTarget||!good(g.target))return f;
    Vec3 d=g.target-g.hip;d.z=0;const double dl=len2d(d);if(!(dl>.05))return f;d=d*(1/dl);
    const double c=std::clamp(f.x*d.x+f.y*d.y,-1.0,1.0);const double ang=std::acos(c);
    if(ang<=kYawLimitRadians)return d;
    clampedYaw=true;const double side=(f.x*d.y-f.y*d.x)>=0?1:-1;const double a=side*kYawLimitRadians;
    const double cs=std::cos(a),sn=std::sin(a);return {f.x*cs-f.y*sn,f.x*sn+f.y*cs,0};
}
// The ankle at full extension: toward the target's belly (ankle a sole short of it), clamped into leg reach, never
// below the knee-height floor or above the hip (a push kick, foot at belly/hip height).
inline Vec3 extendedAnkle(const Geometry& g,Vec3 dir,bool& clamped) noexcept {
    clamped=false;const double r=std::max(g.reach,.05),maxLen=clampReachScale(g.reachScale)*r;
    double dz=-.10*r,along=std::sqrt(std::max(maxLen*maxLen-dz*dz,0.0))*.995; // forward kick: just inside the reach setting
    if(g.haveTarget&&good(g.target)){
        const Vec3 v=g.target-g.hip;const double horizontal=dir.x*v.x+dir.y*v.y;
        along=std::max(horizontal-kSoleAheadMeters,.45*r);
        dz=std::clamp(v.z,-.30*r,.12*r);
    }
    Vec3 leg{dir.x*along,dir.y*along,dz};const double l=pose_core::length(leg);
    if(l>maxLen){clamped=true;leg=leg*(maxLen/l);}
    return g.hip+leg;
}
// The OLD C34b kernel, byte-for-byte the same points as kick176 had (forward only, 88 % reach, 4 deg lean).
inline Pose evaluateOld(const Geometry& g,std::uint64_t elapsed,Vec3 liveAnkle) noexcept {
    Pose p{};const Vec3 up{0,0,1};const double r=g.reach;bool cy=false;
    const Vec3 fwd=kickDirection(Geometry{g.hip,g.restAnkle,g.forward,{},false,g.reach,g.reachScale},cy);
    const Vec3 base=g.restAnkle;
    const Vec3 chamber=base+fwd*(.30*r)+up*(.50*r);
    const Vec3 extended=g.hip+fwd*(.88*r)-up*(.10*r);
    const auto& t=kOld;p.dir=fwd;p.pole=fwd+up*.2;
    if(elapsed>=t.complete){p.phase=Phase::Done;p.ankle=liveAnkle;return p;}
    if(elapsed<t.chamberEnd){p.ankle=mix(base,chamber,smooth(double(elapsed)/double(t.chamberEnd)));p.phase=Phase::Chamber;}
    else if(elapsed<t.hit){p.ankle=mix(chamber,extended,smooth(double(elapsed-t.chamberEnd)/double(t.hit-t.chamberEnd)));p.phase=Phase::Extend;p.contactWindow=true;}
    else if(elapsed<t.holdEnd){p.ankle=extended;p.phase=Phase::Hold;p.contactWindow=true;}
    else {p.ankle=mix(extended,liveAnkle,smooth(double(elapsed-t.holdEnd)/double(t.complete-t.holdEnd)));p.phase=Phase::Recover;}
    p.lean=(4.0*3.14159265358979323846/180.0)*std::sin(3.14159265358979323846*double(elapsed)/double(t.complete));
    return p;
}
// NATIVE TIMING: chamber 0..300 (knee up to hip height, shin hanging), coil 300..500, snap 500..733 (whoosh), hold
// on the target 733..830, retract 830..980 (foot down a step ahead, native footstep .82), settle to the live native
// ankle by 1167. contactWindow = the foot is out (EXTEND for the contact ledger). lean peaks at the hit (5 deg).
inline Pose evaluate(Profile profile,const Geometry& g,std::uint64_t elapsed,Vec3 liveAnkle) noexcept {
    if(profile==Profile::Old)return evaluateOld(g,elapsed,liveAnkle);
    Pose p{};const Vec3 up{0,0,1};const double r=std::max(g.reach,.05);bool cy=false,cl=false;
    const Vec3 dir=kickDirection(g,cy);
    const Vec3 extended=extendedAnkle(g,dir,cl);p.clamped=cy||cl;
    const Vec3 base=g.restAnkle;
    const Vec3 chamber=g.hip+dir*(.30*r)-up*(.45*r);      // knee at hip height, foot under the knee
    const Vec3 coiled=g.hip+dir*(.26*r)-up*(.40*r);       // pulled in a touch more before the snap
    const Vec3 footDown=base+dir*(.12*r);                 // the foot lands a short step ahead
    const auto& t=kNative;p.dir=dir;p.pole=dir+up*.3;
    if(elapsed>=t.complete){p.phase=Phase::Done;p.ankle=liveAnkle;return p;}
    if(elapsed<t.chamberEnd){p.ankle=mix(base,chamber,smooth(double(elapsed)/double(t.chamberEnd)));p.phase=Phase::Chamber;}
    else if(elapsed<t.extendStart){p.ankle=mix(chamber,coiled,smooth(double(elapsed-t.chamberEnd)/double(t.extendStart-t.chamberEnd)));p.phase=Phase::Chamber;}
    else if(elapsed<t.hit){p.ankle=mix(coiled,extended,snap(double(elapsed-t.extendStart)/double(t.hit-t.extendStart)));p.phase=Phase::Extend;p.contactWindow=true;}
    else if(elapsed<t.holdEnd){p.ankle=extended;p.phase=Phase::Hold;p.contactWindow=true;}
    else if(elapsed<t.footDown){p.ankle=mix(extended,footDown,smooth(double(elapsed-t.holdEnd)/double(t.footDown-t.holdEnd)));p.phase=Phase::Recover;}
    else {p.ankle=mix(footDown,liveAnkle,smooth(double(elapsed-t.footDown)/double(t.complete-t.footDown)));p.phase=Phase::Settle;}
    // counter-lean: nothing while chambering, a bell from the coil to the foot-down that peaks at the hit
    if(elapsed>t.chamberEnd&&elapsed<t.footDown){
        const double a=elapsed<=t.hit?.5*double(elapsed-t.chamberEnd)/double(t.hit-t.chamberEnd):.5+.5*double(elapsed-t.hit)/double(t.footDown-t.hit);
        p.lean=kLeanRadians*std::sin(3.14159265358979323846*std::clamp(a,0.0,1.0));
    }
    return p;
}
}
