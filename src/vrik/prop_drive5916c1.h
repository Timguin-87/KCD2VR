#pragma once
// R59.16C1 CP02. Bounded native-body velocity servo; not a created hand constraint.
// The engine resolves contacts. This policy never writes an object transform,
// spawns a copy, deducts inventory or supplies a combat hit.
#include "physical_weapon.h"
#include "../core/snapshot_mailbox.h"
namespace kcd2vr::prop5916c1 {
namespace m=pose_core;namespace p=physical;
inline std::atomic<bool> enabled{true};
struct Dynamics {
    m::Vec3 linear{},angular{},centre{};double mass=0;
    bool valid() const noexcept {return m::finite(linear)&&m::finite(angular)&&m::finite(centre)&&
        std::isfinite(mass)&&mass>=.01&&mass<=20&&m::length(linear)<=12&&m::length(angular)<=40;}
};
struct Limits {
    // Design limits, not measurements of Henry's strength or a native inertia tensor.
    // R59.16C2: linearSpeed 3 -> 8 m/s so a released object can leave the hand at
    // throwing speed (a 3 m/s cap turned every throw into a lob). The per-mass
    // acceleration budget still makes heavy objects lag.
    double force=40,angularAcceleration=80,linearSpeed=8,angularSpeed=12;
    double positionGain=14,rotationGain=12,maxSeparation=.35;
    // R59.16C2: support of the object's weight is NOT charged to the force budget.
    // Native gravity removes ~g*dt of vertical velocity between our commands; without
    // this, any body above force/g (~4.1 kg) sagged out of the hand and was dropped.
    double gravity=9.81; // CryEngine world is Z-up
};
inline m::Vec3 rotationError(m::Quat target,m::Quat current) noexcept {
    auto q=m::unit(m::multiply(target,m::conjugate(current)));if(!q)return {};
    if(q->w<0)*q={-q->x,-q->y,-q->z,-q->w};
    const m::Vec3 v{q->x,q->y,q->z};const double n=m::length(v);
    return n>1e-9?v*(2*std::atan2(n,std::clamp(q->w,0.,1.))/n):m::Vec3{};
}
// D85 interact: the legacy kinematic hold (VrPropPhysicsDrive=false) teleports
// the item to the hand each sample and zeroed the body velocity on every
// release, so it could never be thrown. On an intentional open-hand release
// the last two placed positions give the hand-carried velocity; same 8 m/s
// limit as Limits::linearSpeed and the native hand drop. Stale/duplicate
// samples keep the old zero-velocity release.
struct KinematicTrailD85 {
    m::Vec3 p[2]{};std::uint64_t t[2]{};
    void add(m::Vec3 x,std::uint64_t tick) noexcept {
        if(!m::finite(x)||!tick||(t[1]&&tick<=t[1]))return;
        p[0]=p[1];t[0]=t[1];p[1]=x;t[1]=tick;
    }
    bool release(std::uint64_t now,m::Vec3& v) const noexcept {
        v={};if(!t[0]||!t[1]||t[1]<=t[0]||now<t[1]||now-t[1]>100)return false;
        const auto ms=t[1]-t[0];if(ms<2||ms>100)return false;
        v=(p[1]-p[0])*(1000./double(ms));if(!m::finite(v)){v={};return false;}
        const double n=m::length(v);if(n>8)v=v*(8/n);
        return true;
    }
};
enum class Decision { Drive, Duplicate, End };
struct Command {Decision decision=Decision::End;m::Vec3 linear{},angular{};double separation=0,dt=0;};
struct Controller {
    p::Pose goal{};std::uint64_t sequence=0,tick=0;bool ready=false;
    void seed(p::Pose target,std::uint64_t sample,std::uint64_t sourceTick) noexcept {
        goal=target;sequence=sample;tick=sourceTick;ready=p::finite(target)&&sample&&sourceTick;
    }
    Command update(p::Pose target,p::Pose actual,const Dynamics& d,std::uint64_t sample,
                   std::uint64_t sourceTick,std::uint64_t now,const Limits& l={}) noexcept {
        Command c{};
        if(!ready||!sample||!sourceTick||now<sourceTick||now-sourceTick>80||!p::finite(target)||!p::finite(actual)||!d.valid())return c;
        c.separation=m::length(target.p-actual.p);if(c.separation>l.maxSeparation)return c;
        if(sample==sequence){if(sourceTick==tick)c.decision=Decision::Duplicate;return c;}
        if(sample<sequence||sourceTick<tick||sourceTick-tick>80)return c;
        // GetTickCount64 can stamp two distinct controller samples with the
        // same tick. No elapsed time means no new velocity impulse, not a lost
        // hold. Keep the last integrated goal/time: the next advancing source
        // tick consumes the current target without renewing stale tracking.
        if(sourceTick==tick){c.decision=Decision::Duplicate;return c;}
        const double elapsed=(sourceTick-tick)*.001;
        // A stall never purchases a large catch-up impulse. Source time, not
        // number of game/stereo reads, controls this step.
        c.dt=std::min(elapsed,.033);
        const auto feed=p::limited((target.p-goal.p)*(1./elapsed),l.linearSpeed);
        const auto feedAngular=p::limited(rotationError(target.q,goal.q)*(1./elapsed),l.angularSpeed);
        const auto wantedAngular=p::limited(feedAngular+rotationError(target.q,actual.q)*l.rotationGain,l.angularSpeed);
        const double alpha=l.angularAcceleration/std::max(1.,d.mass); // conservative design proxy
        c.angular=d.angular+p::limited(wantedAngular-d.angular,alpha*c.dt);
        // Native linear velocity is COM velocity. Account for the rotating
        // entity-origin offset rather than treating COM and visual pivot alike.
        const auto wantedLinear=p::limited(feed+(target.p-actual.p)*l.positionGain+
            m::cross(c.angular,d.centre-actual.p),l.linearSpeed);
        const m::Vec3 support{0,0,l.gravity*elapsed}; // restore what gravity took since the last command
        const m::Vec3 base=d.linear+support;
        c.linear=base+p::limited(wantedLinear-base,(l.force/d.mass)*c.dt);
        if(!m::finite(c.linear)||!m::finite(c.angular))return {};
        goal=target;sequence=sample;tick=sourceTick;c.decision=Decision::Drive;return c;
    }
};
// R116 "firm grip" (Experiments > BODY page 2 "Held small props: firm grip", INI VrPropFirmHoldR116, ON).
// His R115 report: carried props "too heavy and draggy, lagging behind my hand when I turn".
// R114 log (01:17:57-01:26:27, 9 holds): 62 of 87 sampled DRIVE rows were DUPLICATE_SOURCE_NO_ACTION -
// the Controller above only writes a velocity when the 1 ms GetTickCount64 stamp (15.6 ms steps) of a NEW
// pose sample advanced, so at 110 game fps most frames got no command and the body coasted; plus a force
// budget of 80 N (53 m/s^2 for a 1.5 kg jug), position gain 14/s (13 % of the gap per frame) and a 3.2 rad/s
// per-frame spin budget -> gaps of 4-8 cm and 13-71 deg (DRIVE value/value2) and one drop at 35 cm.
// This hold commands EVERY game frame with a real clock: velocity = hand velocity (finite difference of the
// pose samples, extrapolated to now) + half the remaining gap per frame, no force/mass budget (the prop is
// as light as the hand), caps 11 m/s / 38 rad/s (Dynamics::valid reads 12 / 40). The engine still resolves
// contacts (table, walls, other props). Same 35 cm leash. Release = the hand velocity (throws).
inline std::atomic<bool> firmHoldR116{true};
// R118 "very jittery in my hand, not smooth like the hand" (R117 log, SteamVR): three noise sources in R116:
// (1) the hand velocity was the pose difference divided by the time the GAME THREAD first SAW the sample.
//     samples come at the headset rate, the game thread looks at ~110 Hz, so that seen-interval is the real
//     ~10-11 ms +/- one game frame (2..20 ms) -> velocity spikes up to 5x and dips to half, frame to frame.
//     now: divided by the sample's own headset time (XrTime the pose was located for, CommittedFrame6695
//     handTimeNsR118), which advances exactly with the pose. no usable time -> keep the last velocity.
// (2) "extrapolate to now" added hand velocity x (age since the game thread SAW the sample, 0..9 ms random)
//     - a random 0-9 mm saw at 1 m/s. now the age is on the pose clock: game clock minus a smoothed
//     (pose clock -> game clock) offset, so between two samples the target slides evenly instead of
//     standing still and then jumping (the sample-and-hold saw).
// (3) the gap was measured from the entity transform (written back from physics after the step, can be a
//     step old) while the velocity is for the centre of mass. now the gap is centre of mass (physics status,
//     same thing the velocity moves) against the hand-held centre of mass. rotation still from the entity.
// (4) first frame of a hold: commandAt = seed time, so dt clamped to 5 ms -> k = 100/s; the R117 log has
//     first-frame gaps of 3-11 cm (DRIVE value .0325/.0448/.1153) -> a 3-11 m/s kick at the grab; the 11.5 cm
//     one (jug 1576197, 09:46:31.306) read back over the 12 m/s body check 10 ms later and was dropped
//     (NATIVE_BODY_REPLACED_OR_UNAVAILABLE). now dt >= 8 ms and the gap part of the velocity is capped at
//     1.5 m/s (hand motion itself still comes through the hand velocity, uncapped up to 11 m/s).
// gain 0.5 of the gap per frame, no force budget, caps and release unchanged.
struct FirmLimitsR116 {
    double gain=.5;          // share of the remaining gap closed per frame (stable with 2 frames of physics delay)
    double linearSpeed=11,angularSpeed=38,throwSpeed=8,throwSpin=20;
    double feedBlend=.6;     // new hand-velocity sample weight (light jitter filter)
    double correctionSpeed=1.5,correctionSpin=8; // the gap-closing part only (R118)
    double maxSeparation=.35,gravity=9.81;
};
struct FirmHoldR116 {
    p::Pose last{};m::Vec3 v{},w{},comLocal{};std::uint64_t sample=0;double sampleTime=0,commandAt=0,clockOffset=0;
    bool primed=false,comKnown=false,offsetKnown=false;
    // sampleTime = the pose's own clock in seconds (headset time); 0 = unknown
    void seed(p::Pose target,std::uint64_t s,double now,double poseTime=0) noexcept {
        last=target;v={};w={};comLocal={};comKnown=false;sample=s;sampleTime=poseTime>0?poseTime:0;commandAt=now;
        offsetKnown=sampleTime>0;clockOffset=offsetKnown?now-sampleTime:0;
        primed=p::finite(target)&&s&&std::isfinite(now);
    }
    // now = seconds of a monotonic clock read on the game thread for every call
    Command update(p::Pose target,p::Pose actual,const Dynamics& d,std::uint64_t s,double now,double poseTime,
                   const FirmLimitsR116& l={}) noexcept {
        Command c{};
        if(!primed||!s||s<sample||!std::isfinite(now)||now<commandAt||!p::finite(target)||!p::finite(actual)||!d.valid())return c;
        c.separation=m::length(target.p-actual.p);if(c.separation>l.maxSeparation)return c;
        if(!comKnown){comLocal=m::rotate(m::conjugate(actual.q),d.centre-actual.p);comKnown=m::finite(comLocal)&&m::length(comLocal)<1;}
        if(s!=sample){
            const double dt=(std::isfinite(poseTime)&&poseTime>0&&sampleTime>0)?poseTime-sampleTime:0;
            if(dt>=.004&&dt<=.1){
                const auto nv=p::limited((target.p-last.p)*(1./dt),l.linearSpeed);
                const auto nw=p::limited(rotationError(target.q,last.q)*(1./dt),l.angularSpeed);
                v=v*(1-l.feedBlend)+nv*l.feedBlend;w=w*(1-l.feedBlend)+nw*l.feedBlend;
            }else if(dt>.1){v={};w={};}
            // dt<4 ms or unknown time: keep the last velocity (never a spike from a guessed interval)
            last=target;sample=s;sampleTime=(std::isfinite(poseTime)&&poseTime>0)?poseTime:0;
            if(sampleTime>0){const double o=now-sampleTime; // seen late = bigger o; follow drops at once, rises slowly
                if(!offsetKnown||o<clockOffset||std::fabs(o-clockOffset)>.1)clockOffset=o;else clockOffset+=.02*(o-clockOffset);
                offsetKnown=true;}else offsetKnown=false;
        }
        const double ahead=offsetKnown?std::clamp(now-(sampleTime+clockOffset),0.,.03):0.;
        p::Pose aim=last;aim.p=last.p+v*ahead;
        if(const auto q=m::unit(m::multiply(m::from_rotation_vector(w*ahead),last.q)))aim.q=*q;
        const double dt=std::clamp(now-commandAt,.008,.033);const double k=l.gain/dt;
        c.dt=dt;
        const m::Vec3 lever=comKnown?m::rotate(aim.q,comLocal):m::Vec3{};
        const m::Vec3 goalCom=aim.p+lever,bodyCom=comKnown?d.centre:actual.p;
        c.angular=p::limited(w+p::limited(rotationError(aim.q,actual.q)*k,l.correctionSpin),l.angularSpeed);
        c.linear=p::limited(v+m::cross(w,lever)+p::limited((goalCom-bodyCom)*k,l.correctionSpeed),l.linearSpeed)+m::Vec3{0,0,l.gravity*dt};
        if(!m::finite(c.linear)||!m::finite(c.angular))return {};
        commandAt=now;c.decision=Decision::Drive;return c;
    }
    // open hand: the prop leaves with the hand's own motion (throw), not with the gap correction
    void release(m::Vec3& linear,m::Vec3& angular,const FirmLimitsR116& l={}) const noexcept {
        linear=p::limited(v+m::cross(w,comKnown?m::rotate(last.q,comLocal):m::Vec3{}),l.throwSpeed);angular=p::limited(w,l.throwSpin);
        if(!m::finite(linear)||!m::finite(angular)){linear={};angular={};}
    }
};
struct Presentation {
    std::uintptr_t character=0,body=0;std::uint64_t bodyEpoch=0,experimentEpoch=0,key=0;
    std::uint64_t observationTick=0,sourceTick=0,sequence=0,instantTokenD78=0;int side=-1;
    p::Pose wristWorld{};
};
inline core::SnapshotMailbox<Presentation> presentation;
inline void clearPresentation() noexcept {(void)presentation.publish({});}
inline bool select(std::uintptr_t character,std::uint64_t bodyEpoch,std::uint64_t experiment,
                   std::uint64_t now,int side,p::Pose& wrist,std::uint64_t instantTokenD78=0) noexcept {
    Presentation s{};
    if(!enabled.load(std::memory_order_relaxed)||!presentation.read(s)||s.side!=side||!s.body||!s.key||
       s.character!=character||s.bodyEpoch!=bodyEpoch||
       (s.instantTokenD78?(s.instantTokenD78!=instantTokenD78):(instantTokenD78||s.experimentEpoch!=experiment))||
       !s.observationTick||!s.sourceTick||now<s.observationTick||now<s.sourceTick||
       now-s.observationTick>80||now-s.sourceTick>80||!p::finite(s.wristWorld))return false;
    wrist=s.wristWorld;return true;
}
// Native layouts are checked against this retail CRigidEntity implementation,
// not accepted solely because the corresponding SDK declarations exist.
struct Status {
    std::uint32_t type=8,part=0x80000000u,index=0x80000000u;
    float linear[3]{},angular[3]{},acceleration[3]{},angularAcceleration[3]{},centre[3]{};
    float submerged=0,mass=0,energy=0;std::int32_t contacts=0;float interval=0;
};
struct Velocity {
    std::uint32_t type=10,part=0x80000000u,index=0x80000000u;
    float linear[3]{},angular[3]{};std::int32_t rotateAroundPivot=0;
};
static_assert(sizeof(Status)==92&&offsetof(Status,linear)==0x0c&&offsetof(Status,angular)==0x18&&
              offsetof(Status,centre)==0x3c&&offsetof(Status,mass)==0x4c);
static_assert(sizeof(Velocity)==40&&offsetof(Velocity,linear)==0x0c&&offsetof(Velocity,angular)==0x18);
}
