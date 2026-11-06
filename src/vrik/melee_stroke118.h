#pragma once
#include "pose_math.h"
#include "thrust_tuning_d67.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
namespace kcd2vr::melee118 {
namespace m=pose_core;
using m::Vec3;
enum class Kind { None, Cut, Thrust, Pommel };
enum class Reason { Idle, Rearm, Slow, Candidate, Prepared, Committed, Withdrawn, Short, Incoherent, Timeout, Guard, UnknownContact, Blocked, Unavailable };
inline const char* name(Reason r) {
    constexpr const char* names[]{"IDLE","REARM","BELOW_SPEED","CANDIDATE","PREPARED","COMMITTED","WITHDRAWN","SHORT_STROKE","INCOHERENT","TIMEOUT","GUARD","UNKNOWN_CONTACT","BLOCKED","UNAVAILABLE"};
    return names[unsigned(r)];
}
struct Observation {
    bool windup=false; // model-frame retreat hint only; producer must qualify both frames before advertising it
    Reason reason=Reason::Idle;Kind kind=Kind::None;
    double handSpeed=0,tipSpeed=0,peak=0,progress=0,coherence=0,age=0,fastTravel=0;
    double rotSpeed=0,rotTravel=0; // V66.178R46: tip motion RELATIVE to the hand (wrist/forearm rotation), both frames
    // Signed hand-velocity / blade-axis cosines at the existing classifier.
    // Zero while axisKnown=false is unavailable, not a sideways gesture.
    double axisCos=0,modelAxisCos=0;bool axisKnown=false;
    double guardHandTravel59=0;bool guardedOrigin59=false,guardCutPass59=false;
};
// Preserve the V118 replay policy for comparison; every live F12=6 producer
// selects deliberate119. Distance is gesture intent, never hit/damage authority.
struct Policy { bool deliberate119=false;
    // V66.178R1 (F4 rung 2): re-arm on the controller HAND, not on wrist/tip rotation.
    // The V178 headset log showed 70 REARM results vs 2 commits: the user's wrist kept
    // the tip moving (1.5 m/s at 0.3 m/s hand) so the strict quiet/opposite-recovery
    // test almost never passed. Native rhythm + strike quality already stop spamming.
    bool handRearm178=false;
    // R46C1: F4's already-qualified complete stroke need not wait for a speed drop.
    bool responsive44c2=false;
    bool motionOwned59c5=false; // qualified contact may occur in the first intent sample
    bool telegraphStart59c10=false; // selected forward-onset intent, never early hit authority
    bool forwardOnly19=false; // selected synchronized/direct gesture policy only
    bool deliberateCut42=false; // C42 hybrid: small fixed-wrist guard flicks cannot prepare/release a cut
    bool purposefulCuts52=false; // selected live physical modes: slow path cannot accumulate fast-cut distance
    bool cutFeint62=false; // one purposeful new cut segment, same prepared attempt and original clock
    bool heavy60=false,twoHand60=false; // H1: native class7 polearm credential, two hands latched
    bool pommelForward69=false; // C71: forward-only policies may accept a deliberate FORWARD end-first push (pommel), never a retreat
    double cutSensitivityH29=1.,stabSensitivityH29=1.,intentSensitivityH29=1.,travelR8=1.;
    bool c54PostGuardH27=false; // live producer only AFTER current guard releases: C54 wrist-cut admission
    double bladeAxisCosineH41=.80; // same alignment for both frames and the existing 60 ms refinement
    bool independentThrustD67=false;
    double thrustOnsetD67=1.25,thrustTravelD67=.15; // effective forward thrust only; cuts/pommel unchanged
};
struct Input {
    double dt=0;Vec3 modelTip{},tip{},modelHand{},hand{},modelAxis{},axis{};
    double outward=0;bool eligible=false,known=false,guard=false,blocked=false;
    Vec3 forward{},modelForward{}; // V66.178R51: body forward in the model frame (zero = unknown; no wind-up test)
    std::uint32_t contactEntity175=0;double contactFraction175=0;
    bool guardedOrigin59=false; // current held/recent guard, only selected forward motion policy
    Vec3 supportModelHand63{},supportHand63{};bool supportMotion63=false;
};
struct State {
    bool ready=false,active=false,prepared=false,spent=false;
    bool guardedOrigin59=false; // sticky for this candidate; rotational guard adjustments are not cuts
    double cutSensitivityH29=1.,stabSensitivityH29=1.,intentSensitivityH29=1.,travelR8=1.;
    bool c54PostGuardH27=false;
    bool independentThrustD67=false;double thrustOnsetD67=1.25,thrustTravelD67=.15;
    bool heavy60=false,twoHand60=false; // H1: two-hand credential must survive the whole candidate
    bool earlyTelegraph59c10=false; // allow the existing short cut->thrust refinement after onset intent
    bool cutSegment62=false,cutPending62=false;
    double cutSegmentAge62=0;
    Vec3 previousTip62{},previousModelTip62{},previousHand62{},previousModelHand62{};
    Kind kind=Kind::None,lastKind=Kind::None;
    double quiet=0,age=0,low=0,cooldown=0,peak=0,handPeak=0;
    double tipPath=0,modelTipPath=0,handPath=0,modelHandPath=0;
    double fastTipPath=0,fastModelTipPath=0;
    double rotPath=0,modelRotPath=0; // R46: accumulated |tip-hand| relative travel (a wrist cut moves the edge, not the hand)
    Vec3 tip{},modelTip{},hand{},modelHand{},axis{},modelAxis{};
    Vec3 supportHand63{},supportModelHand63{};double supportHandPeak63=0;bool supportMotion63=false;
    Vec3 lastDirection{},lastModelDirection{},recovery{},modelRecovery{};
    double recoveryTime=0;
    Vec3 lastTipDirectionH33{},lastModelTipDirectionH33{},tipRecoveryH33{},modelTipRecoveryH33{};
    double tipRecoveryTimeH33=0;
};
struct Result { bool prepare=false,commit=false,abort=false,refine59c10=false,refineCut62=false,cutSegment62=false;Kind kind=Kind::None;Vec3 modelDirection{};Observation observation{}; };
inline void end_candidate(State& s) {
    s.active=s.prepared=s.earlyTelegraph59c10=s.guardedOrigin59=false;s.age=s.low=s.peak=s.handPeak=0;
    s.tipPath=s.modelTipPath=s.handPath=s.modelHandPath=0;
    s.fastTipPath=s.fastModelTipPath=0;s.rotPath=s.modelRotPath=0;
    s.tip=s.modelTip=s.hand=s.modelHand={};s.kind=Kind::None;
    s.supportHand63=s.supportModelHand63={};s.supportHandPeak63=0;s.supportMotion63=false;
    s.cutSegment62=s.cutPending62=false;s.cutSegmentAge62=0;
    s.previousTip62=s.previousModelTip62=s.previousHand62=s.previousModelHand62={};
}
// An early native release is still a complete deliberate stroke. PREPARE alone
// (6cm thrust / 12cm cut) is cancellable intent, never sufficient commitment.
// V66.178R45 (permissive attacks): the 2026-10-10 15:07 log had 31 PREPARE / 11 commits / 20 aborts
// (13 WITHDRAWN, 3 SHORT, 4 UNAVAILABLE) and 121/170 geometry contacts with NO native action. A VR cut
// at arm's length rarely travels 50 cm of fast tip path before it reaches the opponent. Commitment is
// now 30 cm fast tip travel / 22 cm net (cut) and 12 cm (thrust); reversal tolerance is wider.
inline constexpr double kCutCommitFast45=.30,kCutCommitNet45=.22,kCutCommitHand45=.07,kThrustCommit45=.15;
// V66.178R46 (ChatGPT R45 review, section 5): a 0.6 m blade turned 90 degrees in 150 ms about a FIXED wrist has a
// 6.3 m/s tip and zero hand translation; the hand-travel requirement is satisfied by relative (wrist) travel too.
inline constexpr double kCutRotHand46=.20,kCutRotSpeed46=2.5;
inline constexpr double kCutHandMin119=.8; // established deliberate arm-start floor, also required by a guarded cut
// Heavy classes require longer deliberate travel. Only usable two-hand support
// lowers the guarded hand-speed floor: a polearm lever can move its tip quickly
// while either hand moves more slowly. Each hand must earn its own net travel.
inline constexpr double kCutHandMinHeavy2H60=.45,kCutHandTravelHeavy60=.10,kCutTravelHeavyScale60=1.6,kThrustTravelHeavyScale60=1.3;
inline double cutFast60(bool heavy,double travel=1.) noexcept {return (kCutCommitFast45*(heavy?kCutTravelHeavyScale60:1.0))*travel;}
inline double cutNet60(bool heavy,double travel=1.) noexcept {return (kCutCommitNet45*(heavy?kCutTravelHeavyScale60:1.0))*travel;}
inline double thrustNet60(bool heavy,double travel=1.) noexcept {return (kThrustCommit45*(heavy?kThrustTravelHeavyScale60:1.0))*travel;}
inline constexpr double kCutWindupInward51=.45; // V66.178R51/R55: hand speed BACKWARD (against body forward) above which a cut candidate is a wind-up (R55: .60 let the wind-up through)
inline constexpr double kCutWindupTip55=1.5;    // V66.178R55: TIP speed backward above which the motion is a wind-up (the hand barely moves when the blade is cocked over the shoulder)
inline constexpr double kCutWindupNet55=-.50;   // V66.178R55: cosine; a cut whose NET tip travel points more than 120 deg away from body forward (behind) is never prepared/committed - the wind-up is not the attack; a lateral sweep (~90 deg) is
inline bool cut_hand_travel46(const State& s) noexcept {
    return std::min(m::length(s.hand),m::length(s.modelHand))>=kCutCommitHand45/s.intentSensitivityH29||std::min(s.rotPath,s.modelRotPath)>=kCutRotHand46/s.intentSensitivityH29;
}
// Net travel in BOTH independent frames, not rotational path alone. A long
// deliberate wrist cut remains possible; a short guard adjustment needs arm travel.
inline bool deliberate_cut42(const State& s,const Observation& d,Vec3 axis,Vec3 modelAxis) noexcept {
    if(s.kind!=Kind::Cut)return true;
    if(d.progress<.10/s.intentSensitivityH29||d.fastTravel<.10/s.intentSensitivityH29)return false;
    if(std::min(m::length(s.hand),m::length(s.modelHand))>=.08/s.intentSensitivityH29)return true;
    if(s.supportMotion63&&std::min(m::length(s.supportHand63),m::length(s.supportModelHand63))>=.08/s.intentSensitivityH29)return true;
    const auto a=m::unit(axis),b=m::unit(modelAxis);
    return a&&b&&d.progress>=.25/s.intentSensitivityH29&&d.fastTravel>=.35/s.intentSensitivityH29&&
        m::dot(*a,s.axis)<=std::cos(.5235987756/s.intentSensitivityH29)&&m::dot(*b,s.modelAxis)<=std::cos(.5235987756/s.intentSensitivityH29);
}
// C59 headset: rotating a guarded sword through a short tip arc is a parry /
// guard adjustment, even when its tip is fast. Reuse the existing 7cm cut HAND
// travel in BOTH families and the established deliberate .8m/s hand peak;
// rotational tip distance plus a slow drifting hand cannot substitute here.
// This is intent only. Free wrist cuts and axial thrust thresholds stay separate.
inline bool guarded_cut59(const State& s) noexcept {
    const double handMin=s.heavy60&&s.twoHand60?kCutHandMinHeavy2H60:kCutHandMin119; // H1
    const double handTravel=(s.heavy60?kCutHandTravelHeavy60:kCutCommitHand45)/s.intentSensitivityH29;
    return s.c54PostGuardH27||s.kind!=Kind::Cut||!s.guardedOrigin59||
        (std::min(m::length(s.hand),m::length(s.modelHand))>=handTravel&&s.handPeak>=handMin)||
        (s.supportMotion63&&std::min(m::length(s.supportHand63),m::length(s.supportModelHand63))>=handTravel&&s.supportHandPeak63>=handMin);
}
inline bool early_release44c1(const State& s,const Observation& d) noexcept {
    if(!s.active||!s.prepared||s.cutPending62||(s.cutSegment62?s.cutSegmentAge62:s.age)<.065/s.intentSensitivityH29||d.coherence<.75||!guarded_cut59(s))return false;
    if(s.kind==Kind::Thrust)return d.progress>=((s.independentThrustD67?s.thrustTravelD67:thrustNet60(s.heavy60,s.travelR8)/s.intentSensitivityH29));
    return s.kind==Kind::Cut&&d.fastTravel>=(cutFast60(s.heavy60,s.travelR8)/s.intentSensitivityH29)&&d.progress>=(cutNet60(s.heavy60,s.travelR8)/s.intentSensitivityH29)&&cut_hand_travel46(s);
}
// Input-intent policy only. These thresholds do not qualify a native hit, set
// damage, or stand in for the contact-point velocities used by PLANCK.
inline Result update(State& s,const Input& in,Policy policy={}) {
    Result o{};auto& d=o.observation;
    s.c54PostGuardH27=policy.c54PostGuardH27;
    s.heavy60=policy.heavy60;s.travelR8=std::isfinite(policy.travelR8)?std::clamp(policy.travelR8,.4,2.):1.;
    // Acquiring/recovering support cannot retroactively lower the floor for
    // motion already banked without it. Loss retires only that credential;
    // the same candidate may still earn the ordinary one-hand proof.
    s.twoHand60=policy.twoHand60&&(!s.active||s.twoHand60);
    if(!in.eligible||!std::isfinite(in.dt)||in.dt<=0||
       !m::finite(in.tip)||!m::finite(in.modelTip)||!m::finite(in.hand)||!m::finite(in.modelHand)||
       (in.supportMotion63&&(!m::finite(in.supportHand63)||!m::finite(in.supportModelHand63)))||
       !m::finite(in.axis)||!m::finite(in.modelAxis)||!std::isfinite(in.outward)){
        o.abort=s.prepared;s={};d.reason=Reason::Unavailable;return o;
    }
    // V66.178R6: the 50 ms sample guard assumed a 72-90 Hz step. The 2026-10-08 23:14 headset
    // run stepped at 18-30 fps (36-55 ms), so every other sample reset the whole machine
    // (81x UNAVAILABLE, then an 80 ms quiet re-arm) and no stroke could ever commit.
    // Samples up to 120 ms are ordinary; a longer gap ends only the current candidate and
    // keeps the machine armed (no hidden re-arm wait after one hitch).
    if(in.dt>.120000001){o.abort=s.prepared;end_candidate(s);d.reason=Reason::Unavailable;return o;}
    // V66.136: KCD2VR's optional world-contact query returning UNKNOWN is not a
    // reason to cancel a swing (26 strokes were killed mid-swing in the V135R1
    // headset log). Contact uncertainty stays visible in the weapon contact logs
    // and still gates the physical blade pose; it no longer vetoes native intent.
    s.cooldown=std::max(0.0,s.cooldown-in.dt);
    if(in.guard){o.abort=s.prepared;const double cooldown=s.cooldown;s={};s.ready=true;s.cooldown=cooldown;d.reason=Reason::Guard;return o;}
    const double cutSensitivityH29=std::isfinite(policy.cutSensitivityH29)?std::clamp(policy.cutSensitivityH29,.25,2.):1.;
    const double stabSensitivityH29=std::isfinite(policy.stabSensitivityH29)?std::clamp(policy.stabSensitivityH29,.25,2.):1.;
    const bool independentThrustD67=policy.independentThrustD67;
    const auto thrustD67=thrust_tuning_d67::bounded({float(policy.thrustOnsetD67),float(policy.thrustTravelD67)});
    if(s.active&&s.kind==Kind::Thrust&&(s.independentThrustD67!=independentThrustD67 ||
       (independentThrustD67&&(s.thrustOnsetD67!=thrustD67.speed||s.thrustTravelD67!=thrustD67.travel)))){
        o.abort=s.prepared;end_candidate(s);s.ready=false;d.reason=Reason::Unavailable;return o;
    }
    s.independentThrustD67=independentThrustD67;s.thrustOnsetD67=thrustD67.speed;s.thrustTravelD67=thrustD67.travel;
    const double thrustSensitivityD67=independentThrustD67?1.25/double(thrustD67.speed):stabSensitivityH29;
    const double intentH29=std::isfinite(policy.intentSensitivityH29)?std::clamp(policy.intentSensitivityH29,.5,2.):1.;
    const double bladeAxisCosineH41=std::isfinite(policy.bladeAxisCosineH41)&&policy.bladeAxisCosineH41>=.60&&policy.bladeAxisCosineH41<=.95?policy.bladeAxisCosineH41:.80;
    s.intentSensitivityH29=intentH29;
    const double tip=m::length(in.tip)/in.dt,mtip=m::length(in.modelTip)/in.dt;
    const double hand=m::length(in.hand)/in.dt,mhand=m::length(in.modelHand)/in.dt;
    d.tipSpeed=std::min(tip,mtip);d.handSpeed=std::min(hand,mhand);
    const double rot=m::length(in.tip-in.hand)/in.dt,mrot=m::length(in.modelTip-in.modelHand)/in.dt;d.rotSpeed=std::min(rot,mrot); // R46
    // Fresh finite controller samples already passed the source clock/identity
    // checks. Tip angular travel is not a second maximum-speed permission gate.
    const bool quiet=policy.handRearm178?(std::max(hand,mhand)<.30&&std::max(tip,mtip)<1.8):
        (std::max(tip,mtip)<.25&&std::max(hand,mhand)<.25);
    s.quiet=quiet?s.quiet+in.dt:0;
    if(!s.ready||s.spent){
        bool recovered=s.quiet>=(policy.handRearm178?.05:.08)-1e-9;
        if(s.spent&&!in.blocked&&!quiet){
            const Vec3 r=(s.lastKind==Kind::Cut&&!policy.handRearm178)?in.tip:in.hand;
            const Vec3 mr=(s.lastKind==Kind::Cut&&!policy.handRearm178)?in.modelTip:in.modelHand;
            const auto rd=m::unit(r),md=m::unit(mr);
            const double opposite=policy.handRearm178?-.30:-.6; // hand back-swing, not a strict reversal
            if(rd&&md&&m::dot(*rd,s.lastDirection)<opposite&&m::dot(*md,s.lastModelDirection)<opposite){
                s.recovery=s.recovery+r;s.modelRecovery=s.modelRecovery+mr;s.recoveryTime+=in.dt;
                const double recovery=policy.handRearm178?.08:(policy.deliberate119?(s.lastKind==Kind::Cut?.15:.08):.06);
                recovered=s.recoveryTime>=.035&&std::min(m::length(s.recovery),m::length(s.modelRecovery))>=recovery;
            }else{s.recovery=s.modelRecovery={};s.recoveryTime=0;}
        }
        // A cut may qualify by wrist rotation with almost no hand travel.
        // Its deliberate backward tip arc must also be able to rearm it.
        // This is recovery only: return below before classifying a new attack.
        const auto tipBackH33=m::unit(in.tip),modelTipBackH33=m::unit(in.modelTip);
        if(policy.forwardOnly19&&policy.handRearm178&&s.spent&&s.lastKind==Kind::Cut&&!in.blocked&&!quiet&&
           m::dot(in.hand,in.forward)<=0&&m::dot(in.modelHand,in.modelForward)<=0&&
           tipBackH33&&modelTipBackH33&&
           m::dot(*tipBackH33,s.lastTipDirectionH33)<-.6&&
           m::dot(*modelTipBackH33,s.lastModelTipDirectionH33)<-.6){
            s.tipRecoveryH33=s.tipRecoveryH33+in.tip;s.modelTipRecoveryH33=s.modelTipRecoveryH33+in.modelTip;
            s.tipRecoveryTimeH33+=in.dt;
            recovered=recovered||(s.tipRecoveryTimeH33>=.035&&
                std::min(m::length(s.tipRecoveryH33),m::length(s.modelTipRecoveryH33))>=.15);
        }else{s.tipRecoveryH33=s.modelTipRecoveryH33={};s.tipRecoveryTimeH33=0;}
        // The same deliberate backward motion already classified as windup
        // can ready an unarmed recognizer. Do not demand a motionless pause
        // after a reset during continuous fighting. This step fires no attack.
        if(!s.ready&&policy.forwardOnly19&&!in.blocked){
            const auto forward=m::unit(in.forward),modelForward=m::unit(in.modelForward);
            if(forward&&modelForward){
                const bool handWindup=m::dot(in.hand,*forward)/in.dt<-kCutWindupInward51&&
                    m::dot(in.modelHand,*modelForward)/in.dt<-kCutWindupInward51;
                const bool tipWindup=m::dot(in.tip,*forward)/in.dt<-kCutWindupTip55&&
                    m::dot(in.modelTip,*modelForward)/in.dt<-kCutWindupTip55;
                recovered=recovered||handWindup||tipWindup;
            }
        }
        if(!recovered||s.cooldown>0){d.reason=Reason::Rearm;return o;}
        s.ready=true;s.spent=false;s.recovery=s.modelRecovery={};s.recoveryTime=0;
        s.tipRecoveryH33=s.modelTipRecoveryH33={};s.tipRecoveryTimeH33=0;
        // Recovery motion itself is not the next attack; take a new stroke origin.
        d.reason=Reason::Rearm;return o;
    }
    // V66.178R45: a fast candidate (peak >= 2 m/s) that reaches the opponent before its 30 ms/8 cm
    // preparation is a close-range strike, not a touch: it is prepared at contact instead of BLOCKED.
    // V66.178R54 (the user, four logs in a row: "the swings that landed made their sound BEFORE the collision; the
    // others were stopped BY the collision"): an active stroke that reaches a living actor IS the strike. No travel,
    // fraction, owner/part or approach-direction evidence is demanded here any more - the candidate already needed a
    // deliberate speed to exist, and the producer's energy gate (strikeMinScale on the pre-impact velocity) is the
    // one physical filter. Only a WORLD obstruction (no actor entity) still cancels an uncommitted stroke.
    // R54, the user's rule in full: THE COLLISION IS THE ATTACK, IT NEVER STOPS IT. An active stroke that reaches a living
    // actor commits; a stroke that was already prepared commits on ANY obstruction (the native attack runs, hits or not);
    // an unprepared candidate stopped by the world is simply not an attack (no abort - there is nothing to abort).
    const bool firstContact59c5=policy.motionOwned59c5&&policy.deliberate119&&in.blocked&&in.known&&
        in.contactEntity175&&std::isfinite(in.contactFraction175)&&in.contactFraction175>=0&&in.contactFraction175<1;
    // The old early test ran before classification and required s.active. Thus
    // a fresh deliberate short thrust could never begin at its first contact.
    // The motion owner supplies a checked actor contact, not a body label alone.
    const bool actorImpact=policy.motionOwned59c5?firstContact59c5:
        policy.deliberate119&&in.blocked&&in.known&&s.active&&(in.contactEntity175||s.prepared);
    if(in.blocked&&!actorImpact){if(!policy.deliberate119){o.abort=s.prepared;end_candidate(s);s.ready=false;d.reason=Reason::Blocked;return o;}
        // An onset telegraph may already exist before the first world stop.
        // Retire that same attempt rather than leave native preparation alive.
        o.abort=policy.motionOwned59c5&&s.prepared;
        end_candidate(s);s.ready=false;d.reason=Reason::Blocked;return o;}
    const auto axis=m::unit(in.axis),modelAxis=m::unit(in.modelAxis);
    if(!axis||!modelAxis){o.abort=s.prepared;s={};d.reason=Reason::Unavailable;return o;}
    const double along=m::dot(in.hand,*axis)/in.dt,modelAlong=m::dot(in.modelHand,*modelAxis)/in.dt;
    if(hand>1e-9&&mhand>1e-9){d.axisKnown=true;
        d.axisCos=std::clamp(along/hand,-1.0,1.0);d.modelAxisCos=std::clamp(modelAlong/mhand,-1.0,1.0);}
    // V125: tip speed includes wrist rotation and must not veto an otherwise
    // axial hand trajectory. Signed distance, both frames and stable axis still qualify it.
    const bool axial=d.handSpeed>=1.0/((independentThrustD67&&along>0&&modelAlong>0)?thrustSensitivityD67:stabSensitivityH29)&&std::abs(along)>bladeAxisCosineH41*hand&&std::abs(modelAlong)>bladeAxisCosineH41*mhand&&(policy.deliberate119||d.handSpeed>d.tipSpeed*.60)&&along*modelAlong>0;
    Kind kind=axial?(along>0?Kind::Thrust:Kind::Pommel):Kind::Cut;
    // R51C1: reject withdrawal throughout a cut, not only its first sample.
    const auto forward=m::unit(in.modelForward);
    const auto relativeForward=m::unit(in.forward);
    // A cocked blade can point behind the hand while the hand AND its cutting
    // edge already travel toward the opponent. Negative axial hand velocity
    // alone calls that a pommel/withdrawal and discards the first half of a real
    // forehand. Require deliberate transverse edge rotation and forward motion
    // in BOTH independent families before treating it as a cut. End-first
    // translation, wrist roll, retreat and frame disagreement stay excluded.
    if(policy.forwardOnly19&&kind==Kind::Pommel&&forward&&relativeForward&&d.rotSpeed>=kCutRotSpeed46&&
       m::dot(in.modelHand,*forward)>.25*in.dt&&m::dot(in.hand,*relativeForward)>.25*in.dt&&
       m::dot(in.modelTip,*forward)>.25*in.dt&&m::dot(in.tip,*relativeForward)>.25*in.dt&&
       m::length(m::cross(in.modelTip,*modelAxis))/in.dt>=2.0&&m::length(m::cross(in.tip,*axis))/in.dt>=2.0)
        kind=Kind::Cut;
    // The selected forward policy has no automatic pommel/backstroke input. Both independent frames
    // must observe a forward-facing thrust; a retreat is recovery, even when
    // its blade axis points backward and would otherwise classify as a stab.
    // C71 (Miki: pommel strikes worked once, never again): R19 made HYBRID/DIRECT forward-only and dropped EVERY
    // Pommel classification. A real pommel strike is the opposite of the retreat this guard exists for: the hand
    // travels TOWARD the opponent in BOTH independent frames, the blade axis points back (end-first), and there is no
    // edge rotation. A retreat, a wind-up or a cocked cut still fails these tests and stays Withdrawn.
    const bool pommelPush69=policy.pommelForward69&&kind==Kind::Pommel&&forward&&relativeForward&&d.rotSpeed<kCutRotSpeed46&&
       d.handSpeed>=1.25&&m::dot(in.modelHand,*forward)>=.80*mhand*in.dt&&m::dot(in.hand,*relativeForward)>=.80*hand*in.dt;
    if(policy.forwardOnly19&&!pommelPush69&&(!forward||!relativeForward||kind==Kind::Pommel||
       (kind==Kind::Thrust&&(m::dot(*modelAxis,*forward)<.25||m::dot(*axis,*relativeForward)<.25)))){
        d.windup=policy.motionOwned59c5&&!s.active&&s.cooldown<=0&&kind==Kind::Pommel&&forward&&relativeForward&&
            m::dot(in.modelHand,*forward)/in.dt<-kCutWindupInward51&&m::dot(in.hand,*relativeForward)/in.dt<-kCutWindupInward51;
        o.abort=s.prepared;end_candidate(s);d.reason=Reason::Withdrawn;return o;
    }
    const bool retreat=policy.deliberate119&&forward&&
        (m::dot(in.modelHand,*forward)/in.dt<-kCutWindupInward51||m::dot(in.modelTip,*forward)/in.dt<-kCutWindupTip55||
         (policy.forwardOnly19&&relativeForward&&
          (m::dot(in.hand,*relativeForward)/in.dt<-kCutWindupInward51||m::dot(in.tip,*relativeForward)/in.dt<-kCutWindupTip55))); // R55: tip too
    if(retreat&&(policy.forwardOnly19||(s.active&&s.kind==Kind::Cut)||(!s.active&&kind==Kind::Cut))){
        d.windup=policy.motionOwned59c5&&!s.active&&s.cooldown<=0;
        o.abort=s.prepared;end_candidate(s);d.reason=Reason::Withdrawn;return o;
    }
    bool started59c10=false;
    if(!s.active){
        // V66.178R51 ("backward strokes are activating the swing"): the wind-up - the hand pulled back toward the body or
        // behind the shoulder faster than 0.6 m/s - is not a cut candidate. The reversal that follows it is the cut.
        const auto fwd51=m::unit(in.modelForward);
        const bool windup51=policy.deliberate119&&kind==Kind::Cut&&fwd51&&(m::dot(in.modelHand,*fwd51)/in.dt<-kCutWindupInward51||m::dot(in.modelTip,*fwd51)/in.dt<-kCutWindupTip55);
        const bool deliberate=kind==Kind::Cut?(!windup51&&d.tipSpeed>=(policy.purposefulCuts52?2.5:policy.deliberate119?2.0:1.5)/cutSensitivityH29&&(d.handSpeed>=(policy.deliberate119?kCutHandMin119:.65)||(policy.deliberate119&&d.rotSpeed>=kCutRotSpeed46))): // R46: a wrist cut
            (d.handSpeed>=(policy.deliberate119?1.25:1.1)/(kind==Kind::Thrust?thrustSensitivityD67:stabSensitivityH29)&&
             ((policy.deliberate119&&kind==Kind::Thrust)||in.outward>(kind==Kind::Pommel?.18:.10)));
        if(!deliberate||s.cooldown>0){d.reason=s.cooldown>0?Reason::Rearm:Reason::Slow;return o;}
        end_candidate(s);s.active=true;s.kind=kind;s.axis=*axis;s.modelAxis=*modelAxis;
        s.supportMotion63=in.supportMotion63;
        s.guardedOrigin59=policy.forwardOnly19&&in.guardedOrigin59;started59c10=true;
    }
    if(policy.deliberate119&&s.active&&!s.cutSegment62&&(!s.prepared||s.earlyTelegraph59c10)&&s.kind==Kind::Cut&&
       kind==Kind::Thrust&&s.age<=.060000001){
        const auto handSum=s.hand+in.hand,modelSum=s.modelHand+in.modelHand;
        const auto h=m::unit(handSum),mh=m::unit(modelSum);
        if(h&&mh&&m::dot(*h,*axis)>bladeAxisCosineH41&&m::dot(*mh,*modelAxis)>bladeAxisCosineH41){
            const bool announced=s.earlyTelegraph59c10;
            end_candidate(s);s.active=true;s.kind=Kind::Thrust;s.axis=*axis;s.modelAxis=*modelAxis;
            s.prepared=s.earlyTelegraph59c10=announced;o.refine59c10=announced;
        }
    }
    // C62: only an abrupt, translated change in BOTH motion families can
    // refine an announced, uncommitted cut. Ordinary curved tip arcs, wrist
    // rotations and a held rear wind-up do not meet these four direction tests.
    // Clear segment evidence, not its original age/preparation/attempt. The
    // new segment must earn its own intent and later full commit travel.
    if(policy.cutFeint62&&policy.forwardOnly19&&policy.deliberate119&&s.prepared&&
       s.kind==Kind::Cut&&kind==Kind::Cut&&in.known&&!in.blocked&&d.tipSpeed>=2.5&&d.handSpeed>=kCutHandMin119){
        const auto turns=[](Vec3 a,Vec3 b){const auto x=m::unit(a),y=m::unit(b);return x&&y&&m::dot(*x,*y)<.5;};
        const auto translated=[](Vec3 hand,Vec3 tip){const auto h=m::unit(hand),t=m::unit(tip);return h&&t&&m::dot(*h,*t)>=.75;};
        if(turns(in.tip,s.previousTip62)&&turns(in.modelTip,s.previousModelTip62)&&
           turns(in.hand,s.previousHand62)&&turns(in.modelHand,s.previousModelHand62)&&
           translated(in.hand,in.tip)&&translated(in.modelHand,in.modelTip)){
            // The native zone-only boundary retains its existing original
            // 100ms preparation deadline. No repeated turn renews that lease.
            if(s.cutSegment62||s.age+in.dt>.100000001){o.abort=true;end_candidate(s);d.reason=Reason::Withdrawn;return o;}
            s.cutSegment62=s.cutPending62=true;s.cutSegmentAge62=0;
            s.tip=s.modelTip=s.hand=s.modelHand={};
            s.tipPath=s.modelTipPath=s.handPath=s.modelHandPath=0;
            s.fastTipPath=s.fastModelTipPath=s.rotPath=s.modelRotPath=0;
            s.peak=s.handPeak=s.low=0;s.axis=*axis;s.modelAxis=*modelAxis;
            s.supportHand63=s.supportModelHand63={};s.supportHandPeak63=0;
        }
    }
    d.kind=o.kind=s.kind;o.cutSegment62=s.cutSegment62;s.age+=in.dt;
    if(s.cutSegment62)s.cutSegmentAge62+=in.dt;
    s.previousTip62=in.tip;s.previousModelTip62=in.modelTip;
    s.previousHand62=in.hand;s.previousModelHand62=in.modelHand;
    const bool axialStroke=s.kind!=Kind::Cut;const double sign=s.kind==Kind::Pommel?-1:1;
    const double current=axialStroke?d.handSpeed:d.tipSpeed;
    // Signed travel against the axis captured at stroke start. Retraction cannot
    // bank forward distance for a later jab; changing the weapon axis cannot
    // convert unrelated movement into the old prepared thrust.
    const bool reversed=axialStroke?
        (m::dot(in.hand,s.axis)*sign<-(policy.deliberate119?.010:.002)||m::dot(in.modelHand,s.modelAxis)*sign<-(policy.deliberate119?.010:.002)||
         m::dot(*axis,s.axis)<(policy.deliberate119?.6:.8)||m::dot(*modelAxis,s.modelAxis)<(policy.deliberate119?.6:.8)):
        (m::length(s.tip)>.015&&m::dot(in.tip,s.tip)<-(policy.deliberate119?.60:.25)*m::length(in.tip)*m::length(s.tip)); // R45: an arc is not a withdrawal
    if(reversed){o.abort=s.prepared;end_candidate(s);d.reason=Reason::Withdrawn;return o;}
    // At first contact count only the checked fraction of this final increment.
    // Earlier clear motion remains intent evidence; pressure behind the stop does
    // not bank distance and the visible weapon is never moved through the target.
    const double travelFraction=actorImpact?in.contactFraction175:1.0;
    // A real latched support hand may supply the SAME deliberate translated
    // cut credential, independently in both frames. Never mix one hand's net
    // travel with the other's peak, and never carry it across support loss.
    if(!in.supportMotion63)s.supportMotion63=false;
    if(s.supportMotion63){
        s.supportHand63=s.supportHand63+in.supportHand63*travelFraction;
        s.supportModelHand63=s.supportModelHand63+in.supportModelHand63*travelFraction;
        s.supportHandPeak63=std::max(s.supportHandPeak63,
            std::min(m::length(in.supportHand63),m::length(in.supportModelHand63))/in.dt);
    }
    s.tip=s.tip+in.tip*travelFraction;s.modelTip=s.modelTip+in.modelTip*travelFraction;
    s.hand=s.hand+in.hand*travelFraction;s.modelHand=s.modelHand+in.modelHand*travelFraction;
    s.tipPath+=m::length(in.tip)*travelFraction;s.modelTipPath+=m::length(in.modelTip)*travelFraction;
    s.handPath+=m::length(in.hand)*travelFraction;s.modelHandPath+=m::length(in.modelHand)*travelFraction;
    s.rotPath+=m::length(in.tip-in.hand)*travelFraction;s.modelRotPath+=m::length(in.modelTip-in.modelHand)*travelFraction;d.rotTravel=std::min(s.rotPath,s.modelRotPath); // R46
    // Slow reach, jitter and pauses cannot buy the requested half-metre swing.
    if(d.tipSpeed>=(policy.purposefulCuts52&&s.kind==Kind::Cut?2.5:1.2)/(s.kind==Kind::Cut?cutSensitivityH29:s.kind==Kind::Thrust?thrustSensitivityD67:stabSensitivityH29)&&(d.handSpeed>=.35||(policy.deliberate119&&d.rotSpeed>=kCutRotSpeed46))){s.fastTipPath+=m::length(in.tip)*travelFraction;s.fastModelTipPath+=m::length(in.modelTip)*travelFraction;}
    s.peak=std::max(s.peak,current);s.handPeak=std::max(s.handPeak,d.handSpeed);
    d.age=s.age;d.peak=s.peak;
    const double a=axialStroke?m::dot(s.hand,s.axis)*sign:m::length(s.tip);
    const double b=axialStroke?m::dot(s.modelHand,s.modelAxis)*sign:m::length(s.modelTip);
    d.progress=std::min(a,b);
    d.fastTravel=std::min(s.fastTipPath,s.fastModelTipPath);
    d.coherence=std::min(a/std::max(1e-9,axialStroke?s.handPath:s.tipPath),b/std::max(1e-9,axialStroke?s.modelHandPath:s.modelTipPath));
    o.modelDirection=axialStroke?s.modelHand:s.modelTip;
    if(s.cutPending62){
        if(s.age>.100000001||kind!=Kind::Cut){o.abort=true;end_candidate(s);d.reason=Reason::Withdrawn;return o;}
        if(s.cutSegmentAge62>=.030-1e-9&&d.fastTravel>=.12&&d.progress>=.08&&d.coherence>=.75&&
           std::min(m::length(s.hand),m::length(s.modelHand))>=.08&&guarded_cut59(s)){
            s.cutPending62=false;o.refineCut62=true;
        }
    }
    const double timeout=policy.deliberate119?.60:.40;
    if(s.age>timeout|| (s.age>(policy.deliberate119?.07:.035)&&d.coherence<(policy.deliberate119?.60:.70))){
        o.abort=s.prepared;end_candidate(s);s.ready=false;d.reason=d.age>timeout?Reason::Timeout:Reason::Incoherent;return o;
    }
    const double coherence=policy.deliberate119?.75:.80;
    // V147: start the cancellable native preparation before the last 10 cm of
    // a cut. Commitment remains 50 cm / 18 cm; preparation is not hit acceptance.
    const bool prepareTravel=policy.deliberate119?
        (axialStroke?d.progress>=.06/intentH29:(d.fastTravel>=.12/intentH29&&d.progress>=.08/intentH29&&(std::min(m::length(s.hand),m::length(s.modelHand))>=.03||std::min(s.rotPath,s.modelRotPath)>=.08))): // R46
        d.progress>=.025;
    // R55 ("who swings backward? it is winding up, no?"): the net tip travel of a cut must not point behind the body.
    const bool netForward55=policy.forwardOnly19?
        (axialStroke||((m::length(s.modelTip)<1e-6||m::dot(s.modelTip,*forward)>=-.15*m::length(s.modelTip))&&
                       (m::length(s.tip)<1e-6||m::dot(s.tip,*relativeForward)>=-.15*m::length(s.tip)))):
        (!policy.deliberate119||axialStroke||!forward||m::length(s.modelTip)<.06||m::dot(s.modelTip,*forward)>=kCutWindupNet55*m::length(s.modelTip));
    if(!netForward55&&(policy.forwardOnly19||!s.prepared||s.earlyTelegraph59c10)){o.abort=s.prepared;end_candidate(s);d.reason=Reason::Withdrawn;return o;}
    d.guardHandTravel59=std::max(std::min(m::length(s.hand),m::length(s.modelHand)),
        s.supportMotion63?std::min(m::length(s.supportHand63),m::length(s.supportModelHand63)):0.0);
    d.guardedOrigin59=s.guardedOrigin59;d.guardCutPass59=guarded_cut59(s);
    const bool cutProof42=d.guardCutPass59&&(!policy.deliberateCut42||deliberate_cut42(s,d,in.axis,in.modelAxis));
    const bool preparedTravel=cutProof42&&s.age>=(policy.deliberate119?.030:.020)/intentH29-1e-9&&prepareTravel&&d.coherence>=coherence;
    if(preparedTravel)s.earlyTelegraph59c10=false;
    // Only the explicitly selected onset policy advertises a first deliberate
    // sample. SYNC and DIRECT may use it; legacy comparison keeps it off.
    // Commitment, contact and native RPG acceptance are unchanged.
    const bool onset=started59c10&&policy.deliberate119&&policy.telegraphStart59c10&&s.kind!=Kind::Pommel&&cutProof42;
    if(!s.prepared&&(preparedTravel||onset)){s.prepared=true;o.prepare=true;s.earlyTelegraph59c10=!preparedTravel;}
    bool preparedAtContact45=false;
    if(!s.prepared&&actorImpact&&in.contactEntity175&&cutProof42){s.prepared=true;o.prepare=preparedAtContact45=true;} // R45/R54: prepared at the opponent
    const bool travel=policy.deliberate119?
        (axialStroke?d.progress>=((s.kind==Kind::Thrust&&s.independentThrustD67)?s.thrustTravelD67:thrustNet60(s.heavy60,s.travelR8)/s.intentSensitivityH29):(d.fastTravel>=(cutFast60(s.heavy60,s.travelR8)/s.intentSensitivityH29)&&d.progress>=(cutNet60(s.heavy60,s.travelR8)/s.intentSensitivityH29)&&cut_hand_travel46(s))): // H1; D67 explicit thrust distance, pommel unchanged
        d.progress>=(axialStroke?.08:.10);
    // A native actor physically reached before the free-space swing length may
    // admit the already-prepared intent. This is an input request only: native
    // attack eligibility, trajectory, parry, damage and reaction still decide.
    // The same angular evidence admitted at preparation must survive first contact.
    // Axial thrust/pommel still require hand translation; wrist roll has no tip travel.
    const bool contactTravel=actorImpact&&cutProof42&&(!policy.motionOwned59c5||
        (s.age>=.025-1e-9&&d.coherence>=coherence&&netForward55&&
         (s.kind==Kind::Thrust?(d.progress>=.03&&d.handSpeed>=1.25/thrustSensitivityD67):
          s.kind==Kind::Cut?(d.progress>=.04&&d.fastTravel>=.04&&d.tipSpeed>=2.0/cutSensitivityH29):false)));
    // First contact replaces free-space travel only, not direction, deliberate
    // motion or a current physical contact. Resting pressure cannot rearm it.
    // V66.178R30 (R28 review F05): a fast close stab reached the body at 50 ms with 13.5 cm of coherent travel and was
    // aborted as BLOCKED only because the 80 ms free-space age gate had not elapsed. The age gate belongs to the
    // free-space swing; an actor contact with the contact-travel evidence (speed, hand speed, progress, coherence)
    // is already a complete deliberate stroke.
    const double segmentAge62=s.cutSegment62?s.cutSegmentAge62:s.age;
    const bool qualified=!s.cutPending62&&cutProof42&&s.prepared&&((d.coherence>=coherence&&travel&&segmentAge62>=(policy.deliberate119&&!policy.responsive44c2?.050:.040)/s.intentSensitivityH29-1e-9)||contactTravel); // R54: contact needs no coherence // R46: 50 ms (was 80) for a complete fast stroke
    // Commit near the first clear speed peak. A sustained stroke need not wait
    // until the arm stops: after 65 ms of coherent motion it can commit too.
    if(qualified&&(contactTravel||policy.responsive44c2||current<=s.peak*.85||s.age>=.065-1e-9)){ // R30C1: first checked contact cannot wait for a later free-space peak
        o.commit=true;s.lastKind=s.kind;s.lastDirection=m::unit((axialStroke||policy.handRearm178)?s.hand:s.tip).value_or(Vec3{});
        s.lastModelDirection=m::unit((axialStroke||policy.handRearm178)?s.modelHand:s.modelTip).value_or(Vec3{});
        s.lastTipDirectionH33=m::unit(s.tip).value_or(Vec3{});
        s.lastModelTipDirectionH33=m::unit(s.modelTip).value_or(Vec3{});
        s.tipRecoveryH33=s.modelTipRecoveryH33={};s.tipRecoveryTimeH33=0;
        // H5: selected VR attacks already require an actual recovery/quiet
        // rearm and an independently qualified next stroke. Do not add a
        // second fixed attack clock after those conditions. Reference stays.
        end_candidate(s);s.spent=true;s.cooldown=policy.forwardOnly19?0.:.220;s.quiet=0;d.reason=Reason::Committed;return o;
    }
    if(actorImpact){o.abort=s.prepared&&!preparedAtContact45;o.prepare=false;end_candidate(s);s.ready=false;d.reason=Reason::Blocked;return o;} // R45: a contact-only preparation that does not qualify was never published
    s.low=current<(policy.deliberate119?(axialStroke?.60:policy.purposefulCuts52?2.5:1.2):.35)?s.low+in.dt:0;
    if(s.low>=.025){o.abort=s.prepared;end_candidate(s);d.reason=Reason::Short;return o;}
    d.reason=o.prepare?Reason::Prepared:Reason::Candidate;return o;
}
}
