#pragma once
// Closed hand is readiness, never an attack. Native dispatch is a separate owner.
#include "pose_math.h"
#include "motion_time.h"
#include "melee_tuning_c54.h"
#include "fist_stroke_d49.h"
#include "punch_travel_d65.h"
#include <array>
#include <cstdint>
namespace kcd2vr::fists {
namespace pc=pose_core;
struct Sample {
    std::uint64_t epoch=0,sequence=0,tick=0;
    std::uintptr_t actor=0;
    pc::Quat headOrientation{};pc::Vec3 head{},controller[2]{},allowed[2]{};
    double grip[2]{},trigger[2]{};
    bool valid=false,empty[2]{},blocked[2]{};
    bool claimedT24[2]{}; // mod claim is separate from BOTH-native-empty eligibility
    bool boxing167=false; // explicit F11=1; legacy detector thresholds remain the default
    bool freshContact174[2]{}; // qualified current plane, never an unknown/deferred stop
    bool contactStop175=false; // physical contact permission, independent of retired comparison thresholds
    bool openPalm44=false; // F4 only: fast deliberate open-hand stroke requires a fresh physical contact
    bool responsiveCombatH27=false; // native current unarmed/opponent owner: both analogs >60%; responsive motion thresholds
    std::uint64_t timeNs=0;
    bool strokeExperimentD49=false; // optional peak/reversal recognizer, default baseline
    std::uint64_t predictionPolicyD69=0;
};
struct Punch {bool valid=false;int side=-1;std::uint64_t sequence=0,tick=0,epoch=0;std::uintptr_t actor=0;double speed=0,travel=0;int motion167=0;bool kickH35=false;std::uint64_t kickSerialH35=0;}; // H35: an R3 kick travels the same once-only prepare/release lane as a punch, with zone 5
inline Punch retainUnenteredPendingD49(Punch pending,bool before,bool after) noexcept {
    return before!=after&&!pending.kickH35?Punch{}:pending;
}
enum class Phase {Release,Settling,Ready,Moving,Spent};
struct Hand {
    Phase phase=Phase::Release;
    bool released=false;
    bool gripClosedH27=false;
    std::uint64_t quiet=0,start=0;
    std::uint64_t contactTick174=0;
    pc::Vec3 xr{},relative{},allowed{},origin{},allowedOrigin{},direction{},strike{};
    pc::Vec3 translationRelativeD65{},translationOriginD65{},returnTranslationD65{};
    double peakSpeedD49=0,peakAlongD49=0;
    // R114: where THIS candidate first moved punch-fast (always tracked, also without the D49 experiment).
    bool fastSeenR114=false;pc::Vec3 fastOriginR114{},fastTranslationR114{};
    bool retractedD53=false;
    pc::Vec3 returnRelativeD53{},returnAllowedD53{},returnXrD53{};
};
struct State {std::uint64_t epoch=0,sequence=0,tick=0;std::uintptr_t actor=0;std::array<Hand,2> hand{};std::uint64_t timeNs=0;bool strokeExperimentD49=false;std::uint64_t predictionPolicyD69=0;};
// Optional F3 evidence observes the exact decisions below. It never changes state.
enum Reject : std::uint32_t { Invalid=1u,Duplicate=2u,ZeroDt=4u,Discontinuity=8u,Jump=16u,SpeedOutlier=32u,
    NeedRelease=64u,GripOpen=128u,GripMiddle=256u,Settling=512u,NoOnset=1024u,BlockedOnset=2048u,
    Reversed=4096u,TimedOut=8192u,Travel=16384u,AllowedTravel=32768u,Speed=65536u,AllowedSpeed=131072u,
    OpenPalmContact=262144u,SpentRearm=524288u,OtherHandWon=1048576u,TowardHeadD49=2097152u,GuardDriftR114=4194304u };
struct TraceHand {
    Phase before=Phase::Release,after=Phase::Release;std::uint32_t reject=0;
    double dt=0,speed=0,stillSpeed=0,travel=0,allowedTravel=0,allowedSpeed=0,directionDot=0;
    double onsetMin=0,speedMin=0,travelMin=0,allowedTravelMin=0,retract=0,retractMin=0;
    unsigned rearmQuietMin=0;
    bool strokeExperimentD49=false;double strokeSpeedD49=0,reversalDistanceD49=0;bool motionRearmD53=false;
    std::uint64_t quietMs=0,strokeMs=0;bool released=false,closed=false,openStrike=false,recentStop=false,qualified=false;
};
struct Trace {std::array<TraceHand,2> hand{};std::uint32_t invalidInputs=0;bool valid=false,continuity=false;};
inline bool held(const Sample& in,int side) noexcept {return side>=0&&side<2&&!in.claimedT24[side]&&in.grip[side]>double(.60f)&&in.trigger[side]>double(.60f);}
// Reciprocal combat ownership changes readiness, not the player's chosen
// sensitivity. The stronger F11 comparison is explicit; ordinary fists retain
// the chosen speed/travel floor in and out of combat.
inline double speedMinimum(const Sample& in) noexcept {return (in.boxing167?1.0:.50)/double(melee_tuning_c54::punch());}
inline double travelMinimum(const Sample& in) noexcept {return (in.boxing167?.10:punch_travel_d65::value())*double(melee_tuning_c54::bound(melee_tuning_c54::punchTravel.load(),.4f,2.f,1.f))/double(melee_tuning_c54::punch());}
inline double allowedTravelMinimum(const Sample& in) noexcept {return in.boxing167?.045:.015;}
// R114: a punch has a fast moment. R113 log (6 Nov 00:42:16-00:42:35): guard drift reached the 10 cm travel
// floor at 0.57-0.94 m/s peak over 125-406 ms and was sent as a native punch (enemy blocked/reacted while the
// fists only held guard); every real punch in the same fight peaked at 1.26-4.27 m/s within 47-94 ms. The floor
// follows the existing punch sensitivity slider (VrPunchSensitivityC54), like the other punch thresholds.
// A candidate that covers the full travel before any fast sample is retired (re-anchored), never sent.
// The fast part alone must also cover half the punch travel, so a slow drift plus a short quick twitch is not a punch.
inline double peakSpeedMinimumR114(const Sample&) noexcept {return 1.0/double(melee_tuning_c54::punch());}
inline double fastTravelMinimumR114(const Sample& in) noexcept {return .5*travelMinimum(in);}
// D64: recognize an attack leg before it owns a candidate. A return toward
// the face cannot seed a new axis. Hooks and uppercuts retain their existing
// motion classes; no radial-distance gate rejects an across-body hook.
inline bool attackLegD64(pc::Vec3 motion) noexcept {
    const double travel=pc::length(motion);if(!(travel>1e-9))return false;
    return -motion.z>=.30*travel ||
        (std::abs(motion.x)>.60*travel&&motion.z<=.30*travel) ||
        (motion.y>.60*travel&&motion.z<=.30*travel);
}
inline Punch step(State& s,const Sample& in,Trace* trace=nullptr) {
    Punch event{};
    if(trace){*trace={};for(int side=0;side<2;++side){auto& h=trace->hand[side];h.before=s.hand[side].phase;
        // Duplicate/zero-dt observations still describe the actual selected mode,
        // not formatter defaults that look like a live experiment toggle.
        h.strokeExperimentD49=in.strokeExperimentD49;h.released=s.hand[side].released;h.closed=held(in,side);
        h.onsetMin=.40/double(melee_tuning_c54::punch());h.speedMin=speedMinimum(in);
        h.travelMin=travelMinimum(in);h.allowedTravelMin=allowedTravelMinimum(in);
        h.retractMin=-(in.boxing167?.07:.055);h.rearmQuietMin=in.boxing167?60u:120u;}}
    // A live mode switch cannot borrow a prior onset, speed peak or readiness.
    // Native pending/in-flight actions remain the native bridge's separate owner.
    if(s.strokeExperimentD49!=in.strokeExperimentD49||s.predictionPolicyD69!=in.predictionPolicyD69){s={};s.strokeExperimentD49=in.strokeExperimentD49;s.predictionPolicyD69=in.predictionPolicyD69;}
    const bool valid=in.valid&&in.empty[0]&&in.empty[1]&&in.epoch&&in.sequence&&in.tick&&in.actor&&pc::finite(in.head)&&
        pc::finite(in.controller[0])&&pc::finite(in.controller[1])&&pc::finite(in.allowed[0])&&pc::finite(in.allowed[1])&&bool(pc::unit(in.headOrientation))&&
        std::isfinite(in.grip[0])&&std::isfinite(in.grip[1])&&std::isfinite(in.trigger[0])&&std::isfinite(in.trigger[1]);
    if(trace){trace->valid=valid;trace->invalidInputs=(!in.valid?1u:0u)|(!in.empty[0]?2u:0u)|(!in.empty[1]?4u:0u)|(!in.epoch?8u:0u)|(!in.sequence?16u:0u)|(!in.tick?32u:0u)|(!in.actor?64u:0u)|
        (!pc::finite(in.head)?128u:0u)|(!pc::finite(in.controller[0])?256u:0u)|(!pc::finite(in.controller[1])?512u:0u)|(!pc::finite(in.allowed[0])?1024u:0u)|(!pc::finite(in.allowed[1])?2048u:0u)|
        (!bool(pc::unit(in.headOrientation))?4096u:0u)|(!std::isfinite(in.grip[0])?8192u:0u)|(!std::isfinite(in.grip[1])?16384u:0u)|(!std::isfinite(in.trigger[0])?32768u:0u)|(!std::isfinite(in.trigger[1])?65536u:0u);}
    if(!valid){s={};if(trace)for(auto& h:trace->hand){h.reject=Invalid;h.after=Phase::Release;}return event;}
    for(int side=0;side<2;++side)if(in.claimedT24[side])s.hand[side]={};
    if(s.sequence==in.sequence&&s.epoch==in.epoch&&s.actor==in.actor){if(trace)for(auto& h:trace->hand){h.reject=Duplicate;h.after=h.before;}return event;}
    // Coalesce zero-elapsed samples. Keep the earlier positions so the next
    // positive dt covers the complete physical movement, not just its last bit.
    // Invalid owner/tracking/hand occupancy were rejected above, before reuse.
    const double elapsed=kcd2vr::motion_time::elapsed(s.tick,in.tick,s.timeNs,in.timeNs);
    if(s.epoch==in.epoch&&s.actor==in.actor&&in.sequence>s.sequence&&elapsed==0){
        for(int side=0;side<2;++side){auto& h=s.hand[side];
            if(in.blocked[side]){h.phase=Phase::Release;h.quiet=0;}
            if(in.responsiveCombatH27&&in.grip[side]<.35){h.gripClosedH27=false;h.released=true;h.phase=Phase::Release;h.quiet=0;}
            else if(in.responsiveCombatH27){h.gripClosedH27=held(in,side);if(!h.gripClosedH27){h.phase=Phase::Release;h.quiet=0;}}
            else if(in.grip[side]<.25&&in.trigger[side]<.25&&!in.openPalm44){h.released=true;h.phase=Phase::Release;h.quiet=0;}
            else if(in.openPalm44&&in.grip[side]<.25&&in.trigger[side]<.25){/* preserve readiness; zero dt cannot create motion */}
            else if(!held(in,side)){h.phase=Phase::Release;h.quiet=0;}
        }
        if(trace)for(int side=0;side<2;++side){trace->hand[side].reject=ZeroDt;trace->hand[side].after=s.hand[side].phase;}
        return event;
    }
    const bool continuity=s.epoch==in.epoch&&s.actor==in.actor&&in.sequence>s.sequence&&elapsed>0&&elapsed<=.100000001;
    const double dt=continuity?elapsed:0;
    if(trace)trace->continuity=continuity;
    if(!continuity){
        // Recover readiness, never a pose or pending punch. A short missed solve
        // with unchanged exact ownership used to demand opening both buttons.
        // Fresh quiet settling and a NEW stroke are still mandatory afterwards.
        const bool recover=s.epoch==in.epoch&&s.actor==in.actor&&in.sequence>s.sequence&&elapsed>0&&elapsed<=.250000001;
        const bool wasReleased[2]{s.hand[0].released,s.hand[1].released};
        s={};s.epoch=in.epoch;s.actor=in.actor;s.strokeExperimentD49=in.strokeExperimentD49;s.predictionPolicyD69=in.predictionPolicyD69;
        if(recover)for(int side=0;side<2;++side)s.hand[side].released=wasReleased[side];
    }
    for(int side=0;side<2;++side){
        if(in.claimedT24[side]){if(trace){trace->hand[side].reject=Invalid;trace->hand[side].after=Phase::Release;}continue;}
        auto& h=s.hand[side];const auto relative=pc::rotate(pc::conjugate(*pc::unit(in.headOrientation)),in.controller[side]-in.head);
        const auto translationRelativeD65=in.controller[side]-in.head;
        const auto dx=in.controller[side]-h.xr,dr=relative-h.relative,da=in.allowed[side]-h.allowed;
        const auto translationDeltaD65=translationRelativeD65-h.translationRelativeD65;
        const bool jump=continuity&&(pc::length(dx)>.25||pc::length(dr)>.25||pc::length(da)>.25);
        // D65: common translation plus HMD rotation changes both global hand
        // position and the rotating head-local coordinates without an arm stroke.
        // An independent translation-relative vector in tracking axes must also
        // move. Keep both old checks: a whole-body rigid rotation is not a punch.
        const double speed=dt>0?std::min({pc::length(dx),pc::length(dr),pc::length(translationDeltaD65)})/dt:0;
        // Independent global-controller and relative-to-head checks reject root
        // locomotion, head-only motion, and common-mode room translation.
        const double stillSpeed=dt>0?std::max(pc::length(dx),pc::length(dr))/dt:0;
        if(jump||speed>9){h={};}
        if(in.responsiveCombatH27){if(in.grip[side]>=.60)h.gripClosedH27=true;else if(in.grip[side]<.35)h.gripClosedH27=false;}
        else h.gripClosedH27=false;
        const bool released=in.responsiveCombatH27?in.grip[side]<.35:in.grip[side]<.25&&in.trigger[side]<.25;
        const bool closed=held(in,side);
        const bool openStrike=false; // H29R1: no open-palm attacks; both analogs must exceed60%.
        auto* d=trace?&trace->hand[side]:nullptr;
        if(d){d->strokeExperimentD49=in.strokeExperimentD49;d->strokeSpeedD49=speed;d->dt=dt;d->speed=speed;d->stillSpeed=stillSpeed;d->allowedSpeed=dt>0?pc::length(da)/dt:0;
            d->onsetMin=.40/double(melee_tuning_c54::punch());d->speedMin=speedMinimum(in);
            d->travelMin=travelMinimum(in);d->allowedTravelMin=allowedTravelMinimum(in);
            d->closed=closed;d->released=h.released;d->openStrike=openStrike;d->retractMin=-(in.boxing167?.07:.055);d->rearmQuietMin=in.boxing167?60u:120u;
            if(!continuity)d->reject|=Discontinuity;if(jump)d->reject|=Jump;if(speed>9)d->reject|=SpeedOutlier;
            if(!h.released&&!openStrike)d->reject|=NeedRelease;if(released&&!openStrike)d->reject|=GripOpen;
            else if(!closed&&!openStrike)d->reject|=GripMiddle;}
        if(openStrike)h.released=true;
        if(released&&!openStrike){h.released=true;h.phase=Phase::Release;h.quiet=0;}
        else if(!closed&&!openStrike){h.phase=Phase::Release;h.quiet=0;}
        else if(!jump){
            // Empty-hand ownership can begin while the fist is already closed
            // (for example after stowing a weapon). Fresh quiet establishes
            // readiness; it never contributes travel or requests an attack.
            // A later movement still follows the unchanged stroke detector.
            const bool firstClosure=in.responsiveCombatH27&&h.released&&h.phase==Phase::Release;
            if(h.phase==Phase::Release){h.phase=in.responsiveCombatH27&&h.released?Phase::Ready:Phase::Settling;h.quiet=in.tick;}
            if(h.phase==Phase::Settling){
                if(stillSpeed>.45)h.quiet=in.tick;
                if(d){d->reject|=Settling;d->quietMs=in.tick>=h.quiet?in.tick-h.quiet:0;}
                if(in.tick-h.quiet>=120){h.released=true;h.phase=Phase::Ready;h.origin=relative;h.translationOriginD65=translationRelativeD65;h.allowedOrigin=in.allowed[side];if(d)d->reject&=~(Settling|NeedRelease);}
            }else if(h.phase==Phase::Ready){
                if(!firstClosure&&speed>.40/double(melee_tuning_c54::punch())&&!in.blocked[side]&&attackLegD64(dr)){h.phase=Phase::Moving;h.start=in.tick;h.contactTick174=0;h.retractedD53=false;h.origin=h.relative;h.translationOriginD65=h.translationRelativeD65;h.allowedOrigin=h.allowed;h.direction=pc::unit(dr).value_or(pc::Vec3{0,0,0});h.fastSeenR114=false;if(in.strokeExperimentD49){h.peakSpeedD49=0;h.peakAlongD49=0;}}
                else {h.origin=relative;h.translationOriginD65=translationRelativeD65;h.allowedOrigin=in.allowed[side];if(d)d->reject|=in.blocked[side]?BlockedOnset:NoOnset;
                    if(d&&speed>.40/double(melee_tuning_c54::punch())&&!attackLegD64(dr))d->reject|=TowardHeadD49;}
            }
            if(h.phase==Phase::Moving){
                const auto motion=relative-h.origin;const double motionLengthD65=pc::length(motion);
                const double travel=std::min(motionLengthD65,pc::length(translationRelativeD65-h.translationOriginD65)),allowedTravel=pc::length(in.allowed[side]-h.allowedOrigin);
                const bool contactStop=in.boxing167||in.contactStop175;
                if(contactStop&&in.blocked[side]&&in.freshContact174[side]&&!h.contactTick174)h.contactTick174=in.tick;
                const bool recentStop174=h.contactTick174&&in.tick>=h.contactTick174&&in.tick-h.contactTick174<=30;
                // Claude's detector proposal, isolated from the default path.
                // Peak is bounded by this <=500ms stroke; common-mode/head motion,
                // allowed travel and fresh-contact tests still gate recognition.
                // h.relative / h.translationRelativeD65 still hold the PREVIOUS sample here (updated at the end).
                if(!h.fastSeenR114&&speed>=peakSpeedMinimumR114(in)){h.fastSeenR114=true;h.fastOriginR114=h.relative;h.fastTranslationR114=h.translationRelativeD65;}
                const double fastTravelR114=h.fastSeenR114?std::min(pc::length(relative-h.fastOriginR114),pc::length(translationRelativeD65-h.fastTranslationR114)):0.;
                const bool punchPeakR114=h.fastSeenR114&&fastTravelR114>=fastTravelMinimumR114(in);
                bool reversal=pc::dot(dr,h.direction)<-.002;
                bool outwardD49=attackLegD64(motion);double strokeSpeedD49=speed,reversalDistanceD49=0;
                if(in.strokeExperimentD49){
                    h.peakSpeedD49=std::max(h.peakSpeedD49,speed);
                    const double along=pc::dot(motion,h.direction);h.peakAlongD49=std::max(h.peakAlongD49,along);
                    reversalDistanceD49=h.peakAlongD49-along;reversal=reversalDistanceD49>.015;
                    strokeSpeedD49=h.peakSpeedD49;
                }
                // A past peak cannot turn later head-only or native-root motion
                // into a punch after the controller has stopped moving.
                const bool currentMotionD49=!in.strokeExperimentD49||speed>.10;
                if(d){d->strokeSpeedD49=strokeSpeedD49;d->reversalDistanceD49=reversalDistanceD49;d->travel=travel;d->allowedTravel=allowedTravel;d->recentStop=recentStop174;d->directionDot=pc::dot(dr,h.direction);d->strokeMs=in.tick-h.start;
                    if(reversal)d->reject|=Reversed;if(in.tick-h.start>500)d->reject|=TimedOut;
                    if(travel<d->travelMin)d->reject|=Travel;if(allowedTravel<d->allowedTravelMin)d->reject|=AllowedTravel;
                    if(strokeSpeedD49<d->speedMin||!currentMotionD49)d->reject|=Speed;if(!punchPeakR114)d->reject|=GuardDriftR114;if(!outwardD49)d->reject|=TowardHeadD49;if(dt<=0||(!(pc::length(da)/dt>.10)&&!(contactStop&&in.blocked[side]&&in.freshContact174[side]&&recentStop174)))d->reject|=AllowedSpeed;
                    if(openStrike&&!(in.blocked[side]&&in.freshContact174[side]&&recentStop174))d->reject|=OpenPalmContact;}
                // A failed candidate has no accepted stroke to recover from. Keeping
                // its arbitrary first axis in Spent caused the recorded late rearm.
                // Retire only this candidate; the next fresh attack leg may start.
                // R114: the full punch distance covered without a fast moment = guard repositioning. Retire
                // the candidate so a later quick twitch cannot borrow the slow drift's travel; a new punch starts here.
                const bool slowDriftR114=!h.fastSeenR114&&travel>=travelMinimum(in);
                if(reversal||slowDriftR114||in.tick-h.start>500){h.phase=Phase::Ready;h.origin=relative;h.translationOriginD65=translationRelativeD65;h.allowedOrigin=in.allowed[side];h.direction={};h.quiet=0;h.retractedD53=false;}
                else if((!openStrike||(in.blocked[side]&&in.freshContact174[side]&&recentStop174))&&
                        outwardD49&&currentMotionD49&&punchPeakR114&&travel>=travelMinimum(in)&&allowedTravel>=allowedTravelMinimum(in)&&strokeSpeedD49>=speedMinimum(in)&&dt>0&&
                        (pc::length(da)/dt>.10||(contactStop&&in.blocked[side]&&in.freshContact174[side]&&recentStop174))){
                    // A verified stop is allowed to stop the displayed fist. It
                    // must not erase an already deliberate tracked punch. Travel,
                    // speed, native action eligibility and one-request latch remain.
                    // At most one native request wins a simultaneous sample.
                    // Motion names describe the tracked gesture, not a forced native
                    // animation: 0 straight, 1 hook, 2 uppercut (OpenXR Y-up).
                    if(d){d->qualified=true;d->reject=0;}
                    const int motionKind=motion.y>motionLengthD65*.6?2:std::abs(motion.x)>motionLengthD65*.6?1:0;
                    if(!event.valid||strokeSpeedD49>event.speed)event={true,side,in.sequence,in.tick,in.epoch,in.actor,strokeSpeedD49,travel,motionKind};
                    h.direction=pc::unit(motion).value_or(h.direction);h.strike=relative;h.phase=Phase::Spent;h.quiet=0;
                }
            }else if(h.phase==Phase::Spent){
                if(pc::dot(relative-h.strike,h.direction)>0)h.strike=relative;
                const bool retracted=pc::dot(relative-h.strike,h.direction)<-(in.boxing167?.07:.055);
                if(d){d->reject|=SpentRearm;d->retract=pc::dot(relative-h.strike,h.direction);d->quietMs=h.quiet&&in.tick>=h.quiet?in.tick-h.quiet:0;}
                // D52 capture: repeated 10-21 cm returns stayed Spent while
                // boxing because the mandatory quiet pause kept resetting.
                // Both modes may begin a NEW attack leg after an accepted strike's
                // measured return trough. Returning alone never attacks;
                // a later sample still must pass the existing travel/speed,
                // allowed-wrist and native action eligibility checks.
                if(retracted||h.retractedD53){
                    if(!h.retractedD53||pc::dot(relative-h.returnRelativeD53,h.direction)<0){
                        h.retractedD53=true;h.returnRelativeD53=relative;h.returnTranslationD65=translationRelativeD65;
                        h.returnAllowedD53=in.allowed[side];h.returnXrD53=in.controller[side];
                    }
                    const auto renewed=relative-h.returnRelativeD53;
                    if(h.retractedD53&&pc::dot(renewed,h.direction)>.015&&
                       pc::length(in.controller[side]-h.returnXrD53)>.015&&
                       speed>.40/double(melee_tuning_c54::punch())&&!in.blocked[side]&&attackLegD64(renewed)){
                        h.phase=Phase::Moving;h.start=in.tick;h.contactTick174=0;
                        h.origin=h.returnRelativeD53;h.translationOriginD65=h.returnTranslationD65;h.allowedOrigin=h.returnAllowedD53;
                        h.direction=pc::unit(renewed).value_or(h.direction);
                        h.peakSpeedD49=speed;h.peakAlongD49=pc::length(renewed);
                        h.fastSeenR114=speed>=peakSpeedMinimumR114(in);h.fastOriginR114=h.returnRelativeD53;h.fastTranslationR114=h.returnTranslationD65;
                        h.retractedD53=false;h.quiet=0;
                        if(d){d->motionRearmD53=true;d->reject=0;}
                    }
                }
                if(h.phase==Phase::Spent){
                    if(retracted&&stillSpeed<.45){if(!h.quiet)h.quiet=in.tick;if(in.tick-h.quiet>=(in.boxing167?60u:120u)){h.phase=Phase::Ready;h.origin=relative;h.translationOriginD65=translationRelativeD65;h.allowedOrigin=in.allowed[side];h.retractedD53=false;}}
                    else h.quiet=0;
                }
            }
        }
        h.xr=in.controller[side];h.relative=relative;h.translationRelativeD65=translationRelativeD65;h.allowed=in.allowed[side];
        if(d){d->after=h.phase;d->released=h.released;}
    }
    if(trace&&event.valid)for(int side=0;side<2;++side)if(trace->hand[side].qualified&&side!=event.side)trace->hand[side].reject|=OtherHandWon;
    s.epoch=in.epoch;s.actor=in.actor;s.sequence=in.sequence;s.tick=in.tick;s.timeNs=in.timeNs;return event;
}
} // namespace
