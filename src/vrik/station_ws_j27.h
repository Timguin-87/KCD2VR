#pragma once
// J27 (Miki 26 Oct: "the chest is obstructing the view - in alchemy and smithing henry's head is nearer to the things
// than his body - we have no real VRIK there"; "the iron and the hammer have no collisions").
// pure rules, host tested (tests/station_ws_j27_test.cpp). the pose stage feeds them, nothing here reads the game.
//
// TORSO: the 6DOF station root moved the pelvis by the FULL HMD delta (roomscale_station_ds, deadzone 0), and any head
// drop lowered the pelvis 1:1 (crouch). so leaning over the table carried hips and chest along with the head and the
// chest stayed right under the eyes. real VRIK: the hips stay, the spine pivots, the head goes over the table.
//  - upright steps follow after .10 m; measured head drop permits a real forward
//    lean up to .45 m (back .10 m, side .15 m), 6DOF mode only
//  - head drop that the lean explains (straight spine pivoting about the pelvis) is NOT crouch, only the rest is
//  - the chest bone stays >= clearCm behind the eye along the body forward; if not, the pelvis goes back (max 25 cm)
// the camera/HMD and the hands are never touched, only where the hips sit under them.
//
// TOOLS: the hammer / tongs / workpiece hand gets a world sweep like a free hand: 3 probe points along the tool's own
// native local bounds (long axis ends + middle) move from the last allowed wrist to the new one, the wrist stops at
// the first hit. hand and tool stop together. the native forge stroke still comes from the controller motion path.
#include <algorithm>
#include <atomic>
#include <cmath>
#include "../diagnostics/capture168.h"

namespace kcd2vr::station_ws_j27 {
inline bool captureD47(std::uint64_t now) noexcept {
    return diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,now);
}
inline std::atomic<bool> torsoEnabled{true};   // INI VrStationTorsoFollowHead
inline std::atomic<bool> toolCollision{true};  // INI VrSmithToolCollision
inline void setToolCollision(bool value) noexcept {toolCollision.store(value,std::memory_order_release);}
inline std::atomic<float> clearCm{12.f};       // INI VrStationChestClearCm (0..30)

struct TorsoIn {
    double fx=0,fy=1;        // body forward, horizontal unit (model)
    double hx=0,hy=0;        // HMD horizontal delta that the old rule gave the pelvis (6DOF) or head vs anchor (FREE)
    double drop=0;           // head below its anchor, >=0
    double spine=0;          // rest pelvis->head length
    double restHeadFwd=0;    // rest head ahead of the rest pelvis along forward
    double restChestFwd=0;   // rest chest (spine3) ahead of the rest pelvis along forward
    double chestFraction=.6; // |rest spine3 - rest pelvis| / spine
    double eyeFwd=.09;       // eye ahead of the head bone
    double clear=.12;        // wanted chest-behind-eye distance (m)
    bool follow=false;       // 6DOF station: hips may step after the head past the lean radius
    double leanFwd=.45,leanBack=.10,leanSide=.15,maxBackShift=.25,uprightLean=.10;
};
struct TorsoOut {double px=0,py=0,pz=0,eyeAheadOfPelvis=0,chestClear=0,backShift=0;bool valid=false;};

inline double radiusFollow(double v,double pos,double neg) noexcept {
    return v>pos?v-pos:v<-neg?v+neg:0.0;
}
inline TorsoOut torso(const TorsoIn& in) noexcept {
    TorsoOut o{};
    const double fl=std::hypot(in.fx,in.fy);
    if(!(fl>.2)||!std::isfinite(in.hx)||!std::isfinite(in.hy)||!std::isfinite(in.drop)||!(in.spine>.2)||in.spine>1.5)return o;
    const double fx=in.fx/fl,fy=in.fy/fl,rx=fy,ry=-fx;            // right = forward x up
    const double f=in.hx*fx+in.hy*fy,s=in.hx*rx+in.hy*ry;
    // R38: a constant .45 m allowance treats an upright roomscale step as a
    // deep table lean and leaves the hips behind. A spine of length L can lean
    // horizontally sqrt(2*L*drop-drop^2) for the measured vertical head drop.
    // Keep a small upright allowance, and let an actual bend earn the rest.
    const double measuredDrop=std::clamp(in.drop,0.0,in.spine);
    const double impliedLean=std::sqrt(std::max(0.0,2*in.spine*measuredDrop-measuredDrop*measuredDrop));
    const double allowance=std::max(std::clamp(in.uprightLean,0.0,.20),impliedLean);
    const double forward=std::min(std::max(0.0,in.leanFwd),allowance);
    const double backRadius=std::min(std::max(0.0,in.leanBack),allowance);
    const double sideRadius=std::min(std::max(0.0,in.leanSide),allowance);
    double pf=in.follow?radiusFollow(f,forward,backRadius):0.0;
    const double ps=in.follow?radiusFollow(s,sideRadius,sideRadius):0.0;
    const double t=std::clamp(in.chestFraction,0.0,.95);
    const double base=in.restHeadFwd+in.eyeFwd-in.restChestFwd;    // upright chest-behind-eye
    double clear=base+(1.0-t)*(f-pf);
    double back=0;
    if(clear<in.clear){back=std::min((in.clear-clear)/(1.0-t),in.maxBackShift);pf-=back;clear=base+(1.0-t)*(f-pf);}
    const double df=f-pf,ds=s-ps,d=std::hypot(df,ds);
    const double L=in.spine,leanDrop=L-std::sqrt(std::max(0.0,L*L-std::min(d*d,L*L)));
    const double crouch=std::max(0.0,std::max(0.0,in.drop)-leanDrop);
    o.px=pf*fx+ps*rx;o.py=pf*fy+ps*ry;o.pz=-crouch;
    o.eyeAheadOfPelvis=in.restHeadFwd+in.eyeFwd+df;o.chestClear=clear;o.backShift=back;o.valid=true;
    return o;
}

// ---- tool probes: 3 points along the longest axis of the native local bounds ----
struct Probes {double p[3][3]{};double radius=0;bool valid=false;};
inline Probes probes(const float* b) noexcept { // b = min xyz, max xyz (entity local)
    Probes out{};if(!b)return out;
    double ext[3]{};
    for(int k=0;k<3;++k){if(!std::isfinite(b[k])||!std::isfinite(b[k+3])||b[k+3]<b[k])return out;ext[k]=double(b[k+3])-b[k];}
    int a=0;for(int k=1;k<3;++k)if(ext[k]>ext[a])a=k;
    if(ext[a]<.03||ext[a]>2.5)return out;
    double c[3]{};for(int k=0;k<3;++k)c[k]=.5*(double(b[k])+b[k+3]);
    for(int i=0;i<3;++i){for(int k=0;k<3;++k)out.p[i][k]=c[k];}
    out.p[0][a]=b[a];out.p[2][a]=b[a+3];
    double small=1e9;for(int k=0;k<3;++k)if(k!=a)small=std::min(small,ext[k]);
    out.radius=std::clamp(.5*small,.01,.05);out.valid=true;
    return out;
}
// one probe segment result: fraction of the motion allowed before the hit (1 = clear)
inline double hitFraction(double distance,double length,double margin) noexcept {
    if(!(length>1e-6)||!std::isfinite(distance))return 1.0;
    return std::clamp((distance-margin)/length,0.0,1.0);
}
// admission: only a live smith tool socket of this hand, feature on, fresh wrist
constexpr bool admitTool(bool enabled,bool smithGateOpen,bool socketLive,bool sameTool,bool wristValid) noexcept {
    return enabled&&smithGateOpen&&socketLive&&sameTool&&wristValid;
}
inline const char* toolName(int tool) noexcept {return tool==0?"HAMMER":tool==1?"TONGS":tool==2?"WORKPIECE":"NONE";}
}
