#pragma once
// V66.178R58 ROOMSCALE FOLLOW + COMBAT DODGE (pure logic, host-tested; no native state).
//
// The headset position relative to the recentred origin is H (camera-local metres: x right, y forward). The entity
// (Henry) stays where native locomotion put him. Roomscale follow = push the native movement input toward the point
// the player is standing on, and - as the entity catches up - subtract the distance it walked from the camera
// offset (the "anchor"), so the view never slides. R = H + anchor is what Henry still has to walk; R -> 0 as he
// arrives. Walking back to the origin makes R negative and Henry walks back. Nothing is teleported; the game's own
// locomotion, collision and animation move him, so a wall stops him and R simply stays.
//
// Dodge (combat only, opponent locked): fast sideways/backward HMD travel (> 10 cm, at least 0.4 m/s by default) sends the
// native dodge (jump button + stick direction while locked) once; the game decides whether it was a perfect dodge.
// A requested dodge never resets the view anchor: input is not proof of native movement.
#include <cmath>
#include <cstdint>
#include <algorithm>
#include <atomic>

namespace kcd2vr::roomscale58 {
// D70/D75: this controls automatic forward FOLLOW and rejects lowered-head
// lean as a dodge gesture, never the tracked view/anchor or manual stick. Zero
// restores translation-only classification. Height is relative to recenter, not a
// claim that HMD-only tracking can identify footsteps or an anatomical waist.
inline std::atomic<float> leanHeightD70{.10f};
inline float LeanHeightD70() noexcept {return leanHeightD70.load(std::memory_order_acquire);}
inline void SetLeanHeightD70(float v) noexcept {leanHeightD70.store(std::isfinite(v)?std::clamp(v,0.f,.12f):.10f,std::memory_order_release);}

// D81: body tracking, physical walking and headset dodging are independent.
// Toggling BODY must preserve both explicit input choices, including dodge-only.
inline int bodySelection(bool wasBody,bool body,int requestedRoomscale) noexcept {
    (void)wasBody;(void)body;
    return std::clamp(requestedRoomscale,0,3);
}
struct Config {
    double consumedPhysicalX_D76=0,consumedPhysicalY_D76=0; // follow only; dodge history remains raw
    double forwardLeanHeightD70=.10; // m of tracked height loss; 0 disables, release at half threshold
    double deadZone=.12;        // m: R below this is leaning, not walking
    double gainMeters=.35;      // m of R that asks for the full follow stick
    double maxStick=.65;        // walk, never sprint, unless R is far
    double farMeters=.90;       // beyond this the full stick is allowed
    double dodgeDisplacement=.10; // C44: fast lateral/backward HMD travel
    double slowDodgeDisplacement=.10;
    std::uint64_t slowDodgeWindowMs=1000;
    std::uint64_t dodgeWindowMs=250,dodgeHoldMs=120,dodgeCooldownMs=2000;
    std::uint64_t catchupGraceMs=300;  // R59: locomotion blend-out after the follow stick drops is still ours
};
inline constexpr unsigned kHistoryCapacity54=256; // >=1 s of fresh poses even at 180 Hz
struct Sample {std::uint64_t tick=0;double x=0,y=0;
    // Translation stays in origin axes; direction is classified in the latest
    // horizontal HMD heading. Turning must never become translation itself.
    double forwardX=0,forwardY=1;
    double heightDeltaD70=0;bool heightKnownD70=false;
};
struct State {
    double anchorX=0,anchorY=0;
    std::uint64_t catchupUntil=0,lastHmdTickU16=0;Sample hist[kHistoryCapacity54]{};unsigned histCount=0,histHead=0;
    std::uint64_t dodgeUntil=0,lastDodge=0;double dodgeX=0,dodgeY=0;
    bool returning54=false;double returnX54=0,returnY54=0;std::uint64_t neutralSince54=0;
    Sample rearmSample54{};
    Sample followPoseD70{}; // independent of combat-only dodge history retirement
    bool forwardLeanBlockedD70=false;
    bool dodgeLeanBlockedD75=false; // retire lean travel, including the recovery edge
    std::uint64_t consumed=0,dodges=0,injectedFrames=0; // diagnostics
};
struct Drive {float lx=0,ly=0;bool dodge=false;float dodgeLx=0,dodgeLy=0;bool following=false;bool forwardLeanHeldD70=false;};

inline void reset(State& s) noexcept {s=State{};}
// The entity moved by (entX,entY) in the camera-local frame this frame. Only the part of that movement that goes
// toward the player's position (along R) is roomscale catch-up; the stick, a push, a dodge beyond R are not.
inline void consume(State& s,std::uint64_t now,double hmdX,double hmdY,double entX,double entY,const Config& c) noexcept {
    // R59: owned while the follow stick is injected and for catchupGraceMs after it drops (Henry's locomotion
    // blend-out keeps moving him 10-20 cm; without the grace every stop slid the view forward by that much).
    if(!s.catchupUntil||now>s.catchupUntil)return;
    const double rx=hmdX+s.anchorX,ry=hmdY+s.anchorY,rl=std::sqrt(rx*rx+ry*ry);
    if(rl<1e-6)return;
    const double ux=rx/rl,uy=ry/rl;
    double along=entX*ux+entY*uy;
    if(along<=0)return;                 // moved away from the player: not ours
    if(!std::isfinite(along)||along>1.0)return; // a teleport/reload: ignore, the recenter zeroes the anchor
    along=std::min(along,rl);           // never past the player
    s.anchorX-=ux*along;s.anchorY-=uy*along;++s.consumed;
}
inline void observeHmd(State& s,std::uint64_t tick,double hmdX,double hmdY,double forwardX=0,double forwardY=1,double heightDeltaD70=0,bool heightKnownD70=false) noexcept {
    if(!tick||!std::isfinite(hmdX)||!std::isfinite(hmdY))return;
    if(s.lastHmdTickU16&&tick<=s.lastHmdTickU16)return;
    // The history is cleared after each request, so freshness cannot depend
    // on histCount. A lost tracking interval also retires the held input/rearm.
    if(s.lastHmdTickU16&&tick-s.lastHmdTickU16>80){
        s.histCount=0;s.histHead=0;s.dodgeUntil=0;s.returning54=false;s.neutralSince54=0;s.rearmSample54={};s.dodgeLeanBlockedD75=false;
    }
    s.lastHmdTickU16=tick;
    const double headingLength=std::hypot(forwardX,forwardY);
    if(!std::isfinite(headingLength)||headingLength<1e-4){forwardX=forwardY=0;}
    else {forwardX/=headingLength;forwardY/=headingLength;}
    const Sample observedD70{tick,hmdX,hmdY,forwardX,forwardY,heightDeltaD70,heightKnownD70&&std::isfinite(heightDeltaD70)};
    // RoomscaleDrive may re-observe an already accepted tick without vertical
    // metadata after dodge-history retirement. Preserve that exact tracked row.
    if(tick>s.followPoseD70.tick||(tick==s.followPoseD70.tick&&observedD70.heightKnownD70))s.followPoseD70=observedD70;
    s.hist[s.histHead]=observedD70;s.histHead=(s.histHead+1)%kHistoryCapacity54;if(s.histCount<kHistoryCapacity54)++s.histCount;
}
inline bool dodgeTrigger(const State& s,std::uint64_t now,double& dx,double& dy,const Config& c,Sample* origin54=nullptr) noexcept {
    if(s.histCount<2)return false;
    const auto& latest=s.hist[(s.histHead+kHistoryCapacity54-1)%kHistoryCapacity54];
    if(now<latest.tick||now-latest.tick>60||std::hypot(latest.forwardX,latest.forwardY)<.5)return false;
    // D75: distance alone over the long window admitted ordinary walking and
    // replaced MainAttack with native Dodge. Keep the existing fast-window
    // intent in both windows: default 10cm / 250ms = 0.4m/s. The long history
    // tolerates a longer genuine fast step, not a slower walk. No native attack
    // veto or action cancellation is introduced here.
    if(!c.dodgeWindowMs||!std::isfinite(c.dodgeDisplacement)||c.dodgeDisplacement<=0)return false;
    const double minSpeed=c.dodgeDisplacement/(double(c.dodgeWindowMs)*.001);
    const std::uint64_t windows[]{c.dodgeWindowMs,c.slowDodgeWindowMs};
    const double distances[]{c.dodgeDisplacement,c.slowDodgeDisplacement};
    for(int window=0;window<2;++window){
        const Sample* oldest=nullptr;
        for(unsigned i=0;i<s.histCount;++i){const auto& h=s.hist[(s.histHead+kHistoryCapacity54-1-i)%kHistoryCapacity54];
            if(now>=h.tick&&now-h.tick<=windows[window])oldest=&h;else break;}
        if(!oldest||oldest==&latest)continue;
        const double originX=latest.x-oldest->x,originY=latest.y-oldest->y;
        dx=originX*latest.forwardY-originY*latest.forwardX;
        dy=originX*latest.forwardX+originY*latest.forwardY;
        // A current forward lean remains forward after physical turning.
        // Sideways intent may contain a small forward component, but native
        // dodge output must never ask Henry to lunge toward the opponent.
        if(dy>0&&dy>=std::abs(dx))continue;
        dy=std::min(dy,0.0);
        const double l=std::hypot(dx,dy);if(l<=distances[window])continue;
        const double elapsed=double(latest.tick-oldest->tick)*.001;
        if(elapsed<=0||l/elapsed<minSpeed)continue;
        if(origin54)*origin54=*oldest;
        dx/=l;dy/=l;return true;
    }
    return false;
}
inline Drive drive(State& s,std::uint64_t now,double hmdX,double hmdY,bool follow,bool combatLocked,bool dodgeEnabled,const Config& c) noexcept {
    Drive d{};
    // R59.16C5: the HMD dodge no longer needs FOLLOW (walking). Only invalid tracking resets everything.
    if(!std::isfinite(hmdX)||!std::isfinite(hmdY)||(!follow&&!dodgeEnabled)){s.catchupUntil=0;s.dodgeUntil=0;s.histCount=0;s.histHead=0;s.lastHmdTickU16=0;s.returning54=false;s.neutralSince54=0;return d;}
    if(!follow)s.catchupUntil=0;
    if(!dodgeEnabled||!combatLocked){s.dodgeUntil=0;s.histCount=0;s.histHead=0;s.lastHmdTickU16=0;s.returning54=false;s.neutralSince54=0;}
    // D75: known lowered-head travel is a lean, not a new side/back dodge.
    // Retire that travel and the first upright sample so standing up cannot
    // replay a queued displacement. Preserve the cooldown and any existing
    // post-dodge quiet rearm. This changes input admission only, not the game
    // action already accepted or any camera/anchor transform.
    const auto& leanPose=s.followPoseD70;
    const bool dodgeLeanNow=c.forwardLeanHeightD70>0&&std::isfinite(c.forwardLeanHeightD70)&&
        leanPose.tick&&leanPose.heightKnownD70&&now>=leanPose.tick&&now-leanPose.tick<=60&&
        leanPose.heightDeltaD70<=-c.forwardLeanHeightD70;
    if(dodgeLeanNow||s.dodgeLeanBlockedD75){
        s.histCount=0;s.histHead=0;s.dodgeUntil=0;s.neutralSince54=0;
        if(s.returning54)s.rearmSample54={now,hmdX,hmdY};
    }
    s.dodgeLeanBlockedD75=dodgeLeanNow;
    if(s.dodgeUntil&&now<s.dodgeUntil){d.dodge=true;d.dodgeLx=float(s.dodgeX);d.dodgeLy=float(s.dodgeY);return d;}
    // U16: returning/settling is recovery, never another dodge. A physical
    // step can settle at a NEW origin; requiring the old origin traps every
    // later left/back gesture until the user walks back to the first spot.
    // Fresh quiet tracking for 200ms starts a new displacement history.
    // This does not move the view anchor or infer native dodge acceptance.
    if(s.returning54){
        const auto& latest=s.hist[(s.histHead+kHistoryCapacity54-1)%kHistoryCapacity54];
        const double dt=s.rearmSample54.tick&&now>s.rearmSample54.tick?double(now-s.rearmSample54.tick)*.001:0;
        const double speed=dt>0?std::hypot(hmdX-s.rearmSample54.x,hmdY-s.rearmSample54.y)/dt:0;
        const bool fresh=s.histCount&&latest.tick&&now>=latest.tick&&now-latest.tick<=60;
        if(fresh&&dt>0&&speed<=.12){
            if(!s.neutralSince54)s.neutralSince54=now;
            if(now>=s.neutralSince54&&now-s.neutralSince54>=200){
                s.returning54=false;s.neutralSince54=0;s.histCount=0;s.histHead=0;
                observeHmd(s,latest.tick,hmdX,hmdY,latest.forwardX,latest.forwardY,latest.heightDeltaD70,latest.heightKnownD70);
            }
        }else s.neutralSince54=0;
        s.rearmSample54={now,hmdX,hmdY};
    }
    if(dodgeEnabled&&combatLocked&&!s.returning54&&(!s.lastDodge||(now>=s.lastDodge&&now-s.lastDodge>=c.dodgeCooldownMs))){
        double dx=0,dy=0;Sample origin54{};
        if(dodgeTrigger(s,now,dx,dy,c,&origin54)){
            s.returning54=true;s.returnX54=origin54.x;s.returnY54=origin54.y;
            s.neutralSince54=0;s.rearmSample54={now,hmdX,hmdY};
            s.dodgeX=dx;s.dodgeY=dy;s.dodgeUntil=now+c.dodgeHoldMs;s.lastDodge=now;++s.dodges;
            s.histCount=0;s.histHead=0; // request is not an accepted displacement
            d.dodge=true;d.dodgeLx=float(dx);d.dodgeLy=float(dy);return d;
        }
    }
    if(!follow){s.forwardLeanBlockedD70=false;return d;}
    double rx=hmdX+s.anchorX-c.consumedPhysicalX_D76,ry=hmdY+s.anchorY-c.consumedPhysicalY_D76;
    // A forward reach with a lowered headset must not become an automatic
    // walk toward the target. The latest accepted sample supplies both height
    // and heading; no camera transform, time smoothing or inferred foot state.
    const auto* latest=s.followPoseD70.tick?&s.followPoseD70:nullptr;
    const bool leanKnown=c.forwardLeanHeightD70>0&&std::isfinite(c.forwardLeanHeightD70)&&latest&&latest->heightKnownD70&&
        now>=latest->tick&&now-latest->tick<=60&&std::hypot(latest->forwardX,latest->forwardY)>.5;
    if(!leanKnown)s.forwardLeanBlockedD70=false;
    else if(latest->heightDeltaD70<=-c.forwardLeanHeightD70)s.forwardLeanBlockedD70=true;
    else if(latest->heightDeltaD70>=-c.forwardLeanHeightD70*.5)s.forwardLeanBlockedD70=false;
    if(s.forwardLeanBlockedD70){
        const double forward=rx*latest->forwardX+ry*latest->forwardY;
        if(forward>1e-6){rx-=forward*latest->forwardX;ry-=forward*latest->forwardY;d.forwardLeanHeldD70=true;}
    }
    const double rl=std::sqrt(rx*rx+ry*ry);
    if(rl<=c.deadZone)return d;
    const double over=rl-c.deadZone;
    const double mag=std::min(rl>=c.farMeters?1.0:c.maxStick,over/c.gainMeters);
    d.lx=float(rx/rl*mag);d.ly=float(ry/rl*mag);d.following=true;s.catchupUntil=now+c.catchupGraceMs;++s.injectedFrames;return d;
}
}
