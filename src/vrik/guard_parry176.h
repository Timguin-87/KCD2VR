#pragma once
#include "guard_motion168.h"
#include "melee_tuning_c54.h"
#include "combat_r9.h"
#include "release_combat_r92.h"
#include "shield_block_r120.h"
namespace kcd2vr::guard_parry176 {
namespace m=pose_core;
using V=m::Vec3;
// Body-relative guard geometry shared by the pose producer and native input
// consumer. The profile adapter supplies a real blade segment, never a shield
// face normal inferred from a bounding-box axis. Angles are position bearings.
struct Position {V grip{},tip{},chest{},bodyRight{1,0,0};bool shield=false;V faceNormal{};bool requireFace=false;V head{};bool faceKnown=false;};
// R59.16C10R17 strict (Miki: "blocking = no swing/stab movement against the enemy and proper position"; rung-3 log analysis:
// 53 % of guard holds lasted < 100 ms, 38 % ended just before Henry's own prepare): a sword guard is ENTERED only with the
// blade's midpoint at least 12 cm in front of the chest and its tip no more than 10 cm behind it (a sword cocked over the
// shoulder for a wind-up is not a guard). A held guard keeps a looser hysteresis (6 cm / 20 cm). Shields are unchanged.
inline bool position(Position p,bool held,bool strict=false) noexcept {
    if(!m::finite(p.grip)||!m::finite(p.tip)||!m::finite(p.chest)||!m::finite(p.bodyRight))return false;
    p.bodyRight.z=0;const auto right=m::unit(p.bodyRight),blade=m::unit(p.tip-p.grip);
    if(!right||(!p.shield&&!blade))return false;
    const V forward=m::cross(V{0,0,1},*right);
    // R40C1: defend the frontal torso/face, rather than a narrow ray from the
    // chest. A shield is normally held to one side; its grip need not cross the
    // centre line. These are INPUT tolerances, not a fabricated shield collider.
    // V66.178R45 (permissive guard): a close-range block with the weapon held near the body, the
    // blade pointed more toward the opponent, or held wider/lower/higher still counts as a guard.
    // The native block system (timing, zone, perfect block) still decides every outcome.
    const double bodyDistanceD56=melee_tuning_c54::guardDistanceD56(held);
    if(m::length(p.grip-p.chest)>=bodyDistanceD56||!(p.shield||std::abs(m::dot(*blade,forward))<(held?.93:.88)))return false;
    // D72: a shield covers the torso as well as the face. Its profile tip is
    // not a blade segment, so head tracking must not redirect it through the
    // sword-only face-box test. Keep the existing shield chest/reach rule below
    // whether the head is known or not; native defense still owns the outcome.
    if(p.faceKnown&&!p.shield){
        if(!m::finite(p.head))return false;
        const V a{m::dot(p.grip-p.head,*right),m::dot(p.grip-p.head,forward),p.grip.z-p.head.z};
        // R9: a resting ready stance (hilt at the belly, blade up across the face box) is not a guard; the hilt must be raised.
        if(!p.shield&&!kcd2vr::combat_r9::hiltRaised(a.z,held,double(kcd2vr::combat_r9::guardHiltBelowHeadM.load(std::memory_order_relaxed))))return false;
        const V b{m::dot(p.tip-p.head,*right),m::dot(p.tip-p.head,forward),p.tip.z-p.head.z};
        if(!p.shield&&std::abs(m::dot(*blade,forward))>(held?.70:.60))return false;
        // Clip the real blade against one face-height frontal plane envelope.
        // Two-hand support position and weapon class add no alternate guard box.
        const double w=melee_tuning_c54::bound(melee_tuning_c54::guardHalfWidth.load(),.25f,.75f,.50f),h=melee_tuning_c54::bound(melee_tuning_c54::guardHalfHeight.load(),.10f,.35f,.20f);
        // D61: adjustable depth of this existing face-level box only. The radial
        // hand reach cap and missing-head fallback retain their D60 behavior.
        const double depth=melee_tuning_c54::bound(melee_tuning_c54::guardBoxDepthD61.load(),.10f,.80f,.50f);
        const V d=b-a,lo{-w,.10,-h},hi{w,std::min(.10+depth,bodyDistanceD56),h};double enter=0,leave=1;
        const auto clip=[&](double x,double dx,double low,double high){
            if(std::abs(dx)<1e-9)return x>=low&&x<=high;
            double u=(low-x)/dx,v=(high-x)/dx;if(u>v)std::swap(u,v);
            enter=std::max(enter,u);leave=std::min(leave,v);return enter<=leave;};
        return clip(a.x,d.x,lo.x,hi.x)&&clip(a.y,d.y,lo.y,hi.y)&&clip(a.z,d.z,lo.z,hi.z);
    }
    // R120: the shield block box (VrShieldBlockBoxR120, ON). Only the shield grip is tested, in the body frame of
    // the shoulder centre; the chest-distance limit above still applies. OFF = the broad R40C1 shield rule below.
    if(p.shield&&kcd2vr::shield_block_r120::on()){
        const V g=p.grip-p.chest;
        return kcd2vr::shield_block_r120::inside(m::dot(g,*right),m::dot(g,forward),g.z,held,bodyDistanceD56);
    }
    const double halfWidth=p.shield?(held?.70:.60):(held?.50:.42);
    const V lo{-halfWidth,held?-.05:.0,held?-.50:-.42};
    const V hi{halfWidth,std::min(held?1.05:1.0,bodyDistanceD56),held?.65:.58};
    const auto body=[&](V point){const V v=point-p.chest;return V{m::dot(v,*right),m::dot(v,forward),v.z};};
    const V a=body(p.grip),b=body(p.shield?p.grip:p.tip),d=b-a;
    if(!p.shield){
        // H24: proximity/segment-box intersection alone included lowered,
        // side and overhead carrying. The central blade covers FRONT torso/face.
        const V centre=(a+b)*.5;
        if(centre.y<(held?.06:.12)||centre.y>(held?.85:.80)||
           std::abs(centre.x)>(held?.55:.48)||centre.z<-.30||centre.z>.58||
           b.y<(held?-.20:-.10))return false;
        if(p.requireFace){
            const auto normal=m::unit(p.faceNormal);
            if(!normal||std::abs(m::dot(*normal,forward))<(held?.65:.75))return false;
        }
    }
    (void)strict; // basic frontal coverage is mandatory; shields retain their own rule. No new mode.
    // Exact segment/box clipping avoids gaps between the old five blade probes.
    double first=0,last=1;
    const auto clip=[&](double origin,double delta,double low,double high){
        if(std::abs(delta)<1.e-9)return origin>=low&&origin<=high;
        double enter=(low-origin)/delta,leave=(high-origin)/delta;
        if(enter>leave)std::swap(enter,leave);
        first=std::max(first,enter);last=std::min(last,leave);return first<=last;
    };
    return clip(a.x,d.x,lo.x,hi.x)&&clip(a.y,d.y,lo.y,hi.y)&&clip(a.z,d.z,lo.z,hi.z);
}
// R30C1: same entry dwell for gesture arbitration and native block delivery.
// No new edge from an unavailable/duplicate sample, no nonfinite speed bypass.
struct Entry {std::uint64_t since=0;};
// R49C1: a deliberate forward stab must be able to leave the broad guard box
// below the cut's tip-speed threshold. Both motion frames must agree; sideways
// parry motion, head-only movement and a single noisy sample cannot qualify.
struct ThrustExit49 {double time=0,travel=0;};
inline bool thrustExit49(ThrustExit49& s,bool held,double dt,V hand,V modelHand,V axis,V modelAxis,V forward) noexcept {
    const auto a=m::unit(axis),b=m::unit(modelAxis),f=m::unit(forward);
    const double h=m::length(hand),mh=m::length(modelHand);
    if(!held||!std::isfinite(dt)||dt<=0||dt>.12||!m::finite(hand)||!m::finite(modelHand)||!a||!b||!f||
       m::dot(*b,*f)<.60||h<=0||mh<=0||m::dot(hand,*a)<.80*h||m::dot(modelHand,*b)<.80*mh||
       std::min(h,mh)/dt<1.0){s={};return false;}
    s.time+=dt;s.travel+=std::min(m::dot(hand,*a),m::dot(modelHand,*b));
    return s.time>=.025&&s.travel>=.035;
}
// V66.178R45: a HELD guard releases when the blade centre moves faster than exitSpeed (0 = never):
// a strike that starts from inside the (now wider) guard box is an attack, not a block. The entry
// ceiling (maxSpeed) then keeps the returning blade from re-pressing block mid-swing.
// R59.16C10R17: forwardSpeed >= 0 makes the exit direction-aware - a held guard leaves when the blade TIP moves toward the
// opponent faster than exitSpeed. H5 removes the conflicting any-direction speed
// escape: fast sideways/upward parries retain guard. forwardSpeed < 0 = the
// reference R45 rule (any direction).
inline bool enter(Entry& s,bool posture,bool held,std::uint64_t tick,double speed,double maxSpeed,unsigned dwell,double exitSpeed=0,double forwardSpeed=-1) noexcept {
    if(!posture){s={};return false;}
    if(held){
        if(exitSpeed>0&&std::isfinite(speed)){
            // H5: speed alone cannot turn a lateral/upward parry into an
            // attack. The directional owner must see forward approach; the
            // recognizer separately validates deliberate cuts and thrusts.
            const bool leave=forwardSpeed>=0?forwardSpeed>exitSpeed:speed>exitSpeed;
            if(leave){s={};return false;}
        }
        return true;
    }
    if(!tick||!std::isfinite(speed)||speed<0||(maxSpeed>0&&speed>maxSpeed)){s={};return false;}
    if(!s.since||tick<s.since)s.since=tick;
    return tick-s.since>=dwell;
}
struct Movement {V raw{},relative{};bool valid=false;};
// V66.178R59.4: displacement over a short trailing window. The R57C1 counter-motion
// rule fed towardSector only from the guard_motion168 edge detector, which needs a
// 120 ms quiet settle before it can arm; mid-combo the guard hand is never still
// that long, so the supplied 16:12 log shows windowOpenings57=111, motionCandidates57=4,
// towardSector57=0 - not one parry qualified while the opponent chained attacks. A
// window sample is the same original XR hand/head translations over the last
// <=160 ms (span >=40 ms), with the same key/epoch/sequence/tick discipline. It never
// decides a parry: towardSector still requires sector-aligned travel, and the
// native trigger window/cooldown still own the delivered block press.
struct Window {
    static constexpr int kMax=24;
    struct Entry {std::uint64_t tick=0,sequence=0,epoch=0,key=0;V hand{},relative{};};
    Entry ring[kMax]{};int head=0,count=0;
    void reset() noexcept {head=0;count=0;}
    void push(const guard_motion168::Sample& s) noexcept {
        if(!s.eligible||!s.tick||!s.sequence||!s.key||!m::finite(s.hand)||!m::finite(s.head)){reset();return;}
        if(count){const Entry& last=ring[(head+kMax-1)%kMax];
            // The native pump can visit the same original XR publication more than once.
            // That observation is idempotent; it must not erase the preceding beat history.
            if(last.epoch==s.epoch&&last.key==s.key&&last.sequence==s.sequence&&last.tick==s.tick){
                const V relative=s.hand-s.head;
                if(last.hand.x!=s.hand.x||last.hand.y!=s.hand.y||last.hand.z!=s.hand.z||
                   last.relative.x!=relative.x||last.relative.y!=relative.y||last.relative.z!=relative.z)reset();
                return;
            }
            if(last.epoch!=s.epoch||last.key!=s.key||s.sequence<=last.sequence||s.tick<last.tick||s.tick-last.tick>80)reset();
            else if(s.tick==last.tick)return;}
        ring[head]={s.tick,s.sequence,s.epoch,s.key,s.hand,s.hand-s.head};head=(head+1)%kMax;if(count<kMax)++count;
    }
    Movement recent(std::uint64_t spanMs=160,std::uint64_t minSpanMs=40,std::uint64_t earliest=0) const noexcept {
        if(count<2)return {};
        const Entry& last=ring[(head+kMax-1)%kMax];const Entry* oldest=nullptr;
        for(int i=1;i<count;++i){const Entry& e=ring[(head+kMax-1-i)%kMax];if(last.tick-e.tick>spanMs||e.tick<earliest)break;oldest=&e;}
        if(!oldest||last.tick-oldest->tick<minSpanMs)return {};
        const double dt=double(last.tick-oldest->tick)*.001;
        const V raw=last.hand-oldest->hand,relative=last.relative-oldest->relative;
        // Same sanity ceiling as the edge detector: tracking jumps are not a gesture.
        if(m::length(raw)/dt>8.||m::length(relative)/dt>8.)return {};
        return {raw,relative,true};
    }
};
struct State {
    guard_motion168::State motion{};
    Window window{};
    void reset() noexcept {motion.reset();window.reset();}
    Movement update(guard_motion168::Sample sample) noexcept {
        window.push(sample);
        const auto anchor=motion.anchorHand,relativeAnchor=motion.anchorRelative;
        if(!motion.update(sample))return {};
        // The original XR translations own speed/distance/rearm. A new camera
        // heading, model transform or locomotion cannot create this candidate.
        return {sample.hand-anchor,(sample.hand-sample.head)-relativeAnchor,true};
    }
    // Edge candidate when the detector fired, otherwise the trailing-window displacement.
    Movement recent() const noexcept {return window.recent();}
};
inline bool sector(int value) noexcept {return value==0||value==1||value==2||value==5;}
inline int defensiveSector(int incoming) noexcept {
    // Supplied combat_zone.xml symetric_combat_zone_id, NOT the different
    // master_strike_combat_zone_id relation: head/lower stay, left/right mirror.
    return incoming==1?2:incoming==2?1:(incoming==0||incoming==5?incoming:-1);
}
// R59.16C10R17 (Miki: "parrying = from block against the weapon means perfect block"). Source zone: the ATTACKER's +0x158
// (Henry's frame, NOT mirrored - Henry's perceived +0x5D8 equals it in every native block/PB of the PROCESS_HIT census). R16
// compared Henry's +0x5D8 with the attacker's executed +0x278, which reads upper-left for almost every attack, so
// towardSector57 stayed 0 in every log and the press-1 parry could never fire. Only HMD-relative guard-hand travel counts
// (walking, turning or a head move is not a parry): >= minAlong along the counter direction and >= minShare of the travel.
// Side attack -> toward that side + .6 up + .4 forward (BOTH lateral signs are accepted until a headset log proves the frame
// sign; `lateral` reports which one matched), overhead -> up + .4 forward, thrust -> forward (beat the point down/aside).
// Pulling the guard back or dropping it never qualifies. This only decides the INPUT edge; native timing decides the PB.
struct Against {bool ok=false;int lateral=0;double along=0,travel=0;};
inline Against againstAttack(const Movement& motion,int zone,m::Quat trackingToModel,V bodyRight,double minAlong=.03,double minShare=.50) noexcept {
    Against r{};
    if(!motion.valid||!sector(zone)||!m::finite(motion.raw)||!m::finite(motion.relative)||!std::isfinite(minAlong)||!std::isfinite(minShare))return r;
    const auto rotation=m::unit(trackingToModel);
    bodyRight.z=0;const auto right=m::unit(bodyRight);
    if(!rotation||!right)return r;
    const V up{0,0,1},forward=m::cross(up,*right);
    const auto mapped=[&](V v){return m::rotate(*rotation,V{v.x,-v.z,v.y});};
    const V raw=mapped(motion.raw),relative=mapped(motion.relative);
    const double rawTravel=m::length(raw),relativeTravel=m::length(relative);
    r.travel=std::max(rawTravel,relativeTravel);
    if(!std::isfinite(r.travel)||std::min(rawTravel,relativeTravel)<=1e-6)return r;
    // A forward beat meets an incoming weapon from any known sector. Require the SAME
    // direction in actual hand and HMD-relative motion; head movement cannot invent it.
    const auto consider=[&](V direction,int lateral){const auto d=m::unit(direction);if(!d)return;
        const double a=m::dot(raw,*d),b=m::dot(relative,*d),both=std::min(a,b);
        const bool ok=std::isfinite(both)&&a>=minAlong&&b>=minAlong&&a>=rawTravel*minShare&&b>=relativeTravel*minShare;
        if((ok&&!r.ok)||(ok==r.ok&&both>r.along)){r.ok=ok;r.along=both;r.lateral=lateral;}};
    consider(forward,0);
    if(zone==0)consider(up+forward*.4,0);
    else if(zone!=5){const double side=zone==2?1.:-1.;
        consider(*right*side+up*.6+forward*.4,1);consider(*right*-side+up*.6+forward*.4,-1);}
    return r;
}
// A recovery before the final beat must not cancel the beat by netting the entire
// 160ms history. Examine bounded trailing spans ending at the SAME original sample.
// Call only for a known incoming sector; earliest can fence the current attack
// and consumed motion. This grants no PB result or physical-contact proof.
inline Against againstRecent(const State& state,int zone,m::Quat trackingToModel,V bodyRight,double minAlong=.03,double minShare=.50,std::uint64_t earliest=0) noexcept {
    Against best{};
    for(std::uint64_t span:{40u,60u,80u,100u,120u,140u,160u}){
        const auto candidate=againstAttack(state.window.recent(span,40,earliest),zone,trackingToModel,bodyRight,minAlong,minShare);
        if((candidate.ok&&!best.ok)||(candidate.ok==best.ok&&candidate.along>best.along))best=candidate;
    }
    return best;
}
// R59.16C10R16: minAlong/minShare default to the R57C1 values; F4 press 1 (VR INTENT) passes 4 cm / 50 % ("sensitive").
inline bool towardSector(const Movement& motion,int incoming,m::Quat trackingToModel,V bodyRight,double minAlong=.06,double minShare=.65) noexcept {
    if(!motion.valid||!sector(incoming)||!m::finite(motion.raw)||!m::finite(motion.relative))return false;
    const auto rotation=m::unit(trackingToModel);
    bodyRight.z=0;const auto right=m::unit(bodyRight);
    if(!rotation||!right)return false;
    // XR Y-up/-Z-forward -> the same Cry axes and calibrated frame used by
    // the production hand producer. Never rotate intent by live HMD orientation.
    const auto mapped=[&](V v){return m::rotate(*rotation,V{v.x,-v.z,v.y});};
    const int zone=defensiveSector(incoming);
    const V toward=zone==0?V{0,0,1}:zone==5?V{0,0,-1}:*right*(zone==2?1.:-1.);
    const V raw=mapped(motion.raw),relative=mapped(motion.relative);
    const auto matches=[&](V v){const double along=m::dot(v,toward),travel=m::length(v);
        return std::isfinite(travel)&&along>=minAlong&&along>=travel*minShare;};
    // Coarse native attack-sector qualification only. This is not an enemy
    // blade trajectory, collision, accepted block or perfect-block result.
    return matches(raw)&&matches(relative);
}
}
