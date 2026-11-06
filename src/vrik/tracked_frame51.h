#pragma once
#include "pose_math.h"
#include "station_reposition_d53.h"
#include "native_crouch_r93.h"
#include <algorithm>
#include <cstdint>

namespace kcd2vr::tracked_frame51 {
namespace m=pose_core;

// A presentation frame, not another tracking/gameplay writer. All positions are
// model-local until the successful pose publication supplies its current world.
// A native activity may use the tracked frame without becoming an ordinary
// neutral-anatomy donor. This is mod-owned presentation identity, not game ABI.
enum class ReferenceDomainD86 : unsigned { Ordinary, Dice, Mounted, Workstation, Other };
struct Input {
    m::Stamp stamp{};
    std::uint64_t control=0,timeNs=0,bowKey=0,bowGeneration=0,bowRequest=0,acquisitionSerial=0,acquisitionAction=0,acquisitionScope=0;
    std::uint32_t acquisitionToken=0;
    std::uint64_t nativeAnchorPolicyR14=0; // F2 rung5 temporal policy; zero keeps the established frame unchanged
    std::uint64_t workstationModeEpochH51=0,workstationLifetimeH51=0;
    std::uintptr_t workstationInstanceH51=0;
    m::Vec3 nativeHead{},nativePelvis{},restHead{},restPelvis{},calibratedHead{},acquisitionAnchor{};
    double scale=1;
    bool allowed=false,smooth=false,bow=false,drawing=false,eyeNeutral=false,hmdHeight=false;
    double acquisitionCrouchOffsetR93=0; // exact pre-queue ordinary reference; no new stance authority
    bool nativeStanceR93=false,nativeCrouchedR93=false; // fresh exact native predicate, never an input request
    // R98 body height COMBINED: HMD height everywhere, native height while the
    // native crouch stance is current or Henry is on/at a horse (nativeRiderR98).
    // nativeStanceR93/nativeCrouchedR93 carry the same exact crouch receipt.
    bool combinedHeightR98=false,nativeRiderR98=false;
    unsigned workstationKindH51=0; // Exact saved FREE VRIK activity kind; same ordinary anatomy
    ReferenceDomainD86 referenceDomainD86=ReferenceDomainD86::Ordinary;
    std::uintptr_t referenceInstanceD86=0;std::uint64_t referenceLifetimeD86=0;
};
struct Frame {
    m::Stamp stamp{};
    std::uint64_t control=0,publication=0,acquisitionSerial=0,acquisitionAction=0,acquisitionScope=0;
    std::uint32_t acquisitionToken=0;
    std::uint64_t workstationModeEpochH51=0,workstationLifetimeH51=0;
    std::uintptr_t workstationInstanceH51=0;
    m::Vec3 nativeHead{},rawAnchor{},anchor{},bodyOffset{},root{},calibratedHead{},restOffset{},stationOffsetD53{};
    m::Quat rotation{};
    double scale=1;
    bool valid=false,smooth=false,bow=false,neutral=false,hmdHeight=false;
    bool nativeStanceKnownR93=false;
    double nativeCrouchOffsetR93=0; // separate from filter/bow history
    // R98: anchor Z that came from native pelvis height (NATIVE mode, or the
    // COMBINED blend). Hands, weapon and camera all live in this same anchor,
    // so the weapon motor/intent history must move by it like the hands do.
    double heightOffsetR98=0,nativeHeightWeightR98=0;
    bool heightDrivenR98=false,combinedHeightR98=false;
    unsigned workstationKindH51=0; // Current typed workstation owner while native gameplay controls remain held
    ReferenceDomainD86 referenceDomainD86=ReferenceDomainD86::Ordinary;
    std::uintptr_t referenceInstanceD86=0;std::uint64_t referenceLifetimeD86=0;
};
inline bool ordinary_reference_d86(const Frame& f) noexcept {
    return f.valid&&f.referenceDomainD86==ReferenceDomainD86::Ordinary&&
        !f.workstationKindH51&&!f.acquisitionSerial&&!f.bow;
}
struct State {
    m::Stamp stamp{};
    std::uint64_t control=0,timeNs=0,bowKey=0,bowGeneration=0,bowRequest=0;
    m::Vec3 calibratedHead{},restOffset{},filtered{},bowIdle{},mountedAnchorR96{};
    double scale=1,bowWeight=0,nativeAnchorZ100=0;
    bool nativeAnchorReady100=false,nativeStandReleaseR93=false;
    // R98 COMBINED blend (0 = HMD height, 1 = native height). Reset with the
    // frame; never carried into another mode, owner or session.
    double nativeWeightR98=0;std::uint64_t wantedTickR98=0;
    bool weightReadyR98=false,wantedR98=false;
    bool ready=false,bowReady=false,smooth=false,hmdHeight=false,mountedReadyR96=false;
    Frame frame{};Input last{};
    station_reposition_d53::State stationMoveD53{}; // copied/staged with this pose transaction
};
inline bool same_stamp(m::Stamp a,m::Stamp b) noexcept {
    return a.actor==b.actor&&a.epoch==b.epoch&&a.sample==b.sample&&a.tick==b.tick;
}
inline bool valid(const Input& i) noexcept {
    const auto spine=i.restHead-i.restPelvis;
    return i.allowed&&i.stamp.actor&&i.stamp.epoch&&i.stamp.sample&&i.stamp.tick&&i.control&&
        m::finite(i.nativeHead)&&m::finite(i.nativePelvis)&&m::finite(i.restHead)&&m::finite(i.restPelvis)&&
        m::finite(i.calibratedHead)&&std::isfinite(i.scale)&&i.scale>=.1&&i.scale<=10&&
        m::length(spine)>=.20&&m::length(spine)<=1.25&&
        (!i.bow||(i.bowKey&&i.bowGeneration&&i.bowRequest))&&
        ((!i.acquisitionSerial&&!i.acquisitionAction)||(i.acquisitionSerial&&i.acquisitionAction&&i.acquisitionScope&&m::finite(i.acquisitionAnchor)));
}
// The ordinary tracked owner supplies pitch/roll through the HMD, not through
// an animated pelvis. Keep native yaw and translation; use the validated rest
// foundation for the other two axes. No stance, root-height or bone-length
// inference occurs here. The caller must select/solve the pelvis subtree and
// must never apply this to a protected native owner.
inline std::optional<m::Quat> pelvis_foundation54(m::Quat native,m::Quat rest,bool trackedStation=false) noexcept {
    const auto n=m::unit(native),r=m::unit(rest);
    if(!n||!r)return std::nullopt;
    // A qualified Free/6DOF workstation owns the visible body foundation.
    // Authored station pelvis yaw must not rotate that foundation underneath
    // HMD/controller targets. Real entity-world turn remains applied outside.
    if(trackedStation)return r;
    const auto delta=m::multiply(*n,m::conjugate(*r));
    const double norm=std::hypot(delta.z,delta.w);
    if(!std::isfinite(norm)||norm<1e-6)return std::nullopt;
    const m::Quat yaw{0,0,delta.z/norm,delta.w/norm};
    return m::unit(m::multiply(yaw,*r));
}
inline void reset(State& s) noexcept {s={};}
// R98: ~0.25 s from HMD height to native height and back; linear, no latch.
inline constexpr double kHeightBlendSecondsR98=.25;
inline constexpr std::uint64_t kHeightHoldMsR98=500; // a briefly missing crouch receipt keeps the last answer
inline double step_height_weight_r98(double weight,bool wanted,double dt) noexcept {
    const double target=wanted?1.:0.;
    if(!std::isfinite(weight))return target;
    const double step=std::clamp(dt,0.,.08)/kHeightBlendSecondsR98;
    return weight<target?std::min(target,weight+step):std::max(target,weight-step);
}
inline bool advance(State& s,const Input& source,Frame& out) noexcept {
    Input i=source;
    out={};
    if(!valid(i)){reset(s);return false;}
    const auto offset=i.restHead-i.restPelvis;
    const bool same=s.ready&&s.stamp.actor==i.stamp.actor&&s.stamp.epoch==i.stamp.epoch&&s.control==i.control&&
        s.smooth==i.smooth&&s.hmdHeight==i.hmdHeight&&s.last.workstationKindH51==i.workstationKindH51&&
        s.last.referenceDomainD86==i.referenceDomainD86&&s.last.referenceInstanceD86==i.referenceInstanceD86&&
        s.last.referenceLifetimeD86==i.referenceLifetimeD86&&
        s.last.workstationModeEpochH51==i.workstationModeEpochH51&&
        s.last.workstationLifetimeH51==i.workstationLifetimeH51&&s.last.workstationInstanceH51==i.workstationInstanceH51&&
        s.last.acquisitionSerial==i.acquisitionSerial&&s.last.acquisitionAction==i.acquisitionAction&&
        s.last.acquisitionScope==i.acquisitionScope&&s.last.acquisitionToken==i.acquisitionToken&&std::fabs(s.scale-i.scale)<1e-6&&
        m::length(s.calibratedHead-i.calibratedHead)<1e-6&&m::length(s.restOffset-offset)<1e-6;
    if(same&&(i.stamp.sample<s.stamp.sample||i.stamp.tick<s.stamp.tick||
        (i.timeNs&&s.timeNs&&i.timeNs<s.timeNs))){reset(s);return false;}
    // R96: a completed native seat owns one model-local upper-body foundation
    // for this visit. Authored gait/attack pelvis sway must not translate the
    // headset/controller frame after handover. Presentation options may retire
    // smoothing without silently recapturing the current gait as a new seat.
    const bool mountedR96=i.referenceDomainD86==ReferenceDomainD86::Mounted&&
        i.referenceLifetimeD86&&!i.acquisitionSerial&&!i.workstationKindH51;
    const bool sameMountedR96=mountedR96&&s.ready&&s.mountedReadyR96&&
        s.stamp.actor==i.stamp.actor&&s.stamp.epoch==i.stamp.epoch&&
        s.last.referenceDomainD86==ReferenceDomainD86::Mounted&&
        s.last.referenceInstanceD86==i.referenceInstanceD86&&
        s.last.referenceLifetimeD86==i.referenceLifetimeD86&&
        std::fabs(s.scale-i.scale)<1e-6&&m::length(s.calibratedHead-i.calibratedHead)<1e-6&&
        m::length(s.restOffset-offset)<1e-6;
    const auto priorMountedAnchorR96=s.mountedAnchorR96;
    const bool duplicate=same&&same_stamp(i.stamp,s.stamp);
    // Physical hands/weapons reuse their completed output for a repeated XR
    // source. Admit an accepted stance edge with the next fresh controller
    // sample so the shared body cannot move ahead of those cached endpoints.
    // A lost/foreign receipt is never retained by this alignment rule.
    if(duplicate&&i.nativeStanceR93){
        i.nativeStanceR93=s.last.nativeStanceR93;
        i.nativeCrouchedR93=s.last.nativeStanceR93&&s.last.nativeCrouchedR93;
    }
    const bool sameBow=s.last.bow==i.bow&&s.last.drawing==i.drawing&&
        s.bowKey==i.bowKey&&s.bowGeneration==i.bowGeneration&&s.bowRequest==i.bowRequest;
    const bool sameInput=duplicate&&sameBow&&s.last.eyeNeutral==i.eyeNeutral&&s.timeNs==i.timeNs&&
        s.last.nativeAnchorPolicyR14==i.nativeAnchorPolicyR14&&
        s.last.nativeStanceR93==i.nativeStanceR93&&s.last.nativeCrouchedR93==i.nativeCrouchedR93&&
        s.last.acquisitionCrouchOffsetR93==i.acquisitionCrouchOffsetR93&&
        m::length(s.last.nativeHead-i.nativeHead)<1e-9&&m::length(s.last.nativePelvis-i.nativePelvis)<1e-9;
    if(sameInput){out=s.frame;return out.valid;}
    // Same-owner native callbacks can update their untouched head/pelvis inside
    // one XR sample. Retain the filter at dt=0; an upper-body animation alone
    // must not snap the shared frame back to the raw pelvis. Item/draw transitions
    // invalidate their cached bow correction even when the XR stamp is unchanged.
    const bool sameBowOwner=s.last.bow==i.bow&&s.bowKey==i.bowKey&&s.bowGeneration==i.bowGeneration&&s.bowRequest==i.bowRequest;
    const bool continuous=same&&(!duplicate||sameBowOwner)&&i.stamp.tick-s.stamp.tick<=80;
    double dt=continuous&&!duplicate?double(i.stamp.tick-s.stamp.tick)*.001:0;
    if(continuous&&!duplicate&&i.timeNs&&s.timeNs)dt=double(i.timeNs-s.timeNs)*1e-9;
    dt=std::clamp(dt,0.0,.08);
    // A native callback can equip/draw within the SAME XR sample. Changing
    // item/draw state retires temporal integration, but a positively neutral
    // ordinary frame from this exact body/stamp may still donate its anatomy.
    const bool precedingNeutral=(continuous||(same&&duplicate))&&s.last.eyeNeutral&&
        s.last.referenceDomainD86==ReferenceDomainD86::Ordinary&&
        !s.last.acquisitionSerial&&!s.last.bow;
    const auto precedingAnchor=s.filtered;
    if(!continuous)reset(s);

    // Standing anatomy is derived from the SAME validated rest skeleton used
    // by the fixed-length solver, never from a bow/attack/slumped live head.
    // The calibration-relative expression below is exactly nativePelvis plus
    // the validated rest head-to-pelvis offset: calibration still keys lifetime
    // and tracking origin, but a saved slumped native head is not an eye-height
    // authority. Native crouch/step changes the real pelvis; upper-body animation
    // does not acquire the headset. There is no height-amplitude "crouch" guess.
    const auto calibratedPelvis=i.calibratedHead-offset;
    const auto nativeAnchor=i.calibratedHead+(i.nativePelvis-calibratedPelvis);
    auto raw=i.acquisitionSerial?i.acquisitionAnchor:nativeAnchor;
    if(mountedR96){
        s.mountedAnchorR96=sameMountedR96?priorMountedAnchorR96:nativeAnchor;
        s.mountedReadyR96=true;raw=s.mountedAnchorR96;
    }else s.mountedReadyR96=false;
    // Explicit HMD-owned height: the current real model-world root still
    // carries stairs/translation. Animation-local pelvis lowering must not
    // become standing eye height or reseed a lower frame after a native hit.
    // The consumer applies this SAME correction to pelvis/legs and head/hands.
    // Button-only native crouch is retained by opting OUT, not guessed by cm.
    // R98 COMBINED: the same HMD frame, but native pelvis height is blended in
    // while Henry is crouched (exact native receipt) or riding/mounting/dismounting.
    // Only ordinary on-foot frames blend; seat/station/pickup keep their owners.
    const bool combinedR98=i.combinedHeightR98&&i.hmdHeight&&!mountedR96&&!i.acquisitionSerial&&
        !i.workstationKindH51&&i.referenceDomainD86==ReferenceDomainD86::Ordinary;
    double weightR98=0;
    if(combinedR98){
        bool wantedR98=i.nativeRiderR98;
        if(i.nativeStanceR93){
            wantedR98=wantedR98||i.nativeCrouchedR93;
            s.wantedR98=i.nativeCrouchedR93;s.wantedTickR98=i.stamp.tick;
        }else if(continuous&&s.weightReadyR98&&s.wantedTickR98&&i.stamp.tick>=s.wantedTickR98&&
                 i.stamp.tick-s.wantedTickR98<=kHeightHoldMsR98)
            wantedR98=wantedR98||s.wantedR98;
        else{s.wantedR98=false;s.wantedTickR98=0;}
        // First frame of a fresh owner takes the wanted height directly (the
        // native transfer clip already moved the view); later edges blend.
        weightR98=continuous&&s.weightReadyR98?step_height_weight_r98(s.nativeWeightR98,wantedR98,dt):(wantedR98?1.:0.);
        s.nativeWeightR98=weightR98;s.weightReadyR98=true;
        raw.z=i.restHead.z+(nativeAnchor.z-i.restHead.z)*weightR98;
    }else{
        s.nativeWeightR98=0;s.weightReadyR98=false;s.wantedR98=false;s.wantedTickR98=0;
        // COMBINED pickup: the frozen pre-queue reference already holds the
        // height it was taken at (crouched = native height). Keep it.
        const bool combinedPickupR98=i.combinedHeightR98&&i.hmdHeight&&i.acquisitionSerial&&!mountedR96;
        if(i.hmdHeight&&!mountedR96&&!combinedPickupR98)raw.z=i.restHead.z;
    }
    auto wanted=raw;
    const bool bowSame=continuous&&s.bowKey==i.bowKey&&s.bowGeneration==i.bowGeneration&&s.bowRequest==i.bowRequest;
    // The first draw can arrive before an idle sample of the newly equipped bow.
    // Only this body's immediately preceding positively neutral ordinary frame
    // may seed it; a drawn/stale/replaced body cannot donate a posture.
    if(!i.bow||!bowSame){s.bowReady=false;s.bowWeight=0;}
    if(i.bow){
        if(!s.bowReady&&precedingNeutral){s.bowIdle=precedingAnchor;s.bowReady=true;}
        if(!s.bowReady&&!i.drawing&&i.eyeNeutral&&i.referenceDomainD86==ReferenceDomainD86::Ordinary){s.bowIdle=raw;s.bowReady=true;}
        // C63R2 user-selected free bow view: idle, quiver, nock, draw and release
        // share ONE model-local foundation for this bow lifetime. Release must
        // not blend the native aim animation back into the headset. Current
        // world travel/turn and independent HMD position/orientation stay live.
        // C55: native draw/aim moves pelvis Z and camera yaw as well as XY.
        // The view must not take those authored movements. Pin the SAME body,
        // hands and eye foundation immediately on draw; current entity world
        // travel/turn and physical HMD crouch are still composed afterwards.
        // Native-button-only crouch height is intentionally not replayed during
        // a qualified draw, even when the ordinary native-Z option is selected.
        s.bowWeight=s.bowReady?1.0:0.0;
        if(s.bowReady&&s.bowWeight>0){
            const auto correction=(s.bowIdle-raw)*s.bowWeight;
            if(m::length(correction)<.40)wanted=wanted+correction;
            else{s.bowReady=false;s.bowWeight=0;}
        }
    }
    auto anchor=wanted;
    if(i.smooth&&continuous){
        // Filter the native frame only. The HMD/controllers are added later
        // without filtering. A large stance change gets the faster response.
        const auto d=wanted-s.filtered;
        const double tau=m::length(d)>.20?.08:.30;
        anchor=s.filtered+d*(1-std::exp(-dt/tau));
    }
    // Preserve the native-height/filter/bow calculation, including its original
    // state. Only the completed presentation's XY registration is anatomical:
    // real travel/turn comes from entity world, not an animation-local pelvis
    // lunge when Picking/Attach ends. HMD/controller offsets stay current.
    const auto nativeFiltered=anchor;
    // F2 rung5 compares only residual ordinary native height. XY is already
    // anatomical; protected bow/acquisition/station/HMD-height owners retain
    // their established foundation. The HMD/controllers and world root are
    // composed after this model-local value and are never time-filtered here.
    // Keep the baseline filter/bow donor intact: this independent staged scalar
    // cannot alter a later bowIdle seed or stack the baseline .30/.08s response.
    const bool nativeAnchor100=i.nativeAnchorPolicyR14&&!mountedR96&&!i.hmdHeight&&!i.bow&&
        !i.acquisitionSerial&&!i.workstationKindH51;
    if(nativeAnchor100){
        if(!continuous||!s.nativeAnchorReady100||s.last.nativeAnchorPolicyR14!=i.nativeAnchorPolicyR14)
            s.nativeAnchorZ100=wanted.z;
        else s.nativeAnchorZ100+=(wanted.z-s.nativeAnchorZ100)*(1-std::exp(-dt/.100));
        anchor.z=s.nativeAnchorZ100;
    }
    s.nativeAnchorReady100=nativeAnchor100;
    anchor.x=i.restHead.x;anchor.y=i.restHead.y;
    // Compare residual correction in the same anatomical XY basis. The removed
    // authored pelvis lunge is not another reach/error budget; native Z policy
    // and the existing finite/45cm correction limit remain unchanged.
    const m::Vec3 basisAnchor{i.restHead.x,i.restHead.y,nativeAnchor.z};
    const bool nativeStanceR93=i.nativeStanceR93&&i.hmdHeight&&!i.combinedHeightR98&&!i.acquisitionSerial&&!i.workstationKindH51&&
        i.referenceDomainD86==ReferenceDomainD86::Ordinary;
    auto residualR93=anchor-basisAnchor;
    // The accepted native crouch animation may still be between the authored
    // standing/crouched pelvis heights on either state edge. Remove only that
    // known Z interval before applying the unchanged 45cm residual guard.
    // This is NOT an amplitude-based stance detector: only the exact native
    // predicate decides the presentation offset below. Legacy paths are exact.
    const double stanceHeightR93=native_crouch_r93::heightMeters/i.scale;
    const bool acquisitionStanceR93=i.acquisitionSerial&&i.hmdHeight&&
        std::fabs(i.acquisitionCrouchOffsetR93+stanceHeightR93)<1e-9;
    // Only an observed same-owner crouch->stand edge may retain the authored
    // interval while its native pelvis returns. Ordinary standing never gets
    // this extra allowance, and it retires as soon as the old guard admits.
    const bool releaseR93=nativeStanceR93&&!i.nativeCrouchedR93&&same&&
        ((s.last.nativeStanceR93&&s.last.nativeCrouchedR93)||s.nativeStandReleaseR93)&&
        m::length(residualR93)>.45;
    s.nativeStandReleaseR93=releaseR93;
    if((nativeStanceR93&&i.nativeCrouchedR93)||releaseR93||acquisitionStanceR93)
        residualR93.z-=std::clamp(residualR93.z,0.0,stanceHeightR93);
    // R98 COMBINED: while blending between HMD and native height the known
    // crouch interval is expected, exactly like the R93 stance allowance.
    if(combinedR98&&(weightR98>0||(i.nativeStanceR93&&i.nativeCrouchedR93)||i.nativeRiderR98||s.wantedR98))
        residualR93.z-=std::clamp(residualR93.z,0.0,stanceHeightR93);
    if(!m::finite(nativeFiltered)||!m::finite(anchor)||m::length(residualR93)>.45){reset(s);return false;}
    const double crouchOffsetR93=((nativeStanceR93&&i.nativeCrouchedR93)||acquisitionStanceR93)?-stanceHeightR93:0;
    anchor.z+=crouchOffsetR93; // once, AFTER the bow foundation and native filter
    out.stamp=i.stamp;out.control=i.control;out.nativeHead=i.nativeHead;
    out.referenceDomainD86=i.referenceDomainD86;out.referenceInstanceD86=i.referenceInstanceD86;out.referenceLifetimeD86=i.referenceLifetimeD86;
    out.acquisitionSerial=i.acquisitionSerial;out.acquisitionAction=i.acquisitionAction;
    out.acquisitionScope=i.acquisitionScope;out.acquisitionToken=i.acquisitionToken;
    out.nativeStanceKnownR93=nativeStanceR93;out.nativeCrouchOffsetR93=crouchOffsetR93;
    // R98: native-height share of this anchor (NATIVE mode or COMBINED blend).
    // HMD mode keeps exactly the R93 stance-only coordinate change.
    out.combinedHeightR98=combinedR98;out.nativeHeightWeightR98=combinedR98?weightR98:(!i.hmdHeight?1.:0.);
    out.heightDrivenR98=(combinedR98||(!i.hmdHeight&&!mountedR96))&&!i.acquisitionSerial&&!i.workstationKindH51&&
        i.referenceDomainD86==ReferenceDomainD86::Ordinary;
    out.heightOffsetR98=out.heightDrivenR98?anchor.z-crouchOffsetR93-i.restHead.z:0;
    out.rawAnchor=nativeAnchor;out.anchor=anchor;out.bodyOffset=anchor-nativeAnchor;out.scale=i.scale;
    out.calibratedHead=i.calibratedHead;out.restOffset=offset;
    out.valid=true;out.workstationKindH51=i.workstationKindH51;out.workstationModeEpochH51=i.workstationModeEpochH51;out.workstationLifetimeH51=i.workstationLifetimeH51;out.workstationInstanceH51=i.workstationInstanceH51;out.smooth=i.smooth||nativeAnchor100;out.hmdHeight=i.hmdHeight;out.bow=i.bow;out.neutral=i.eyeNeutral&&i.referenceDomainD86==ReferenceDomainD86::Ordinary&&!i.acquisitionSerial&&(!i.bow||(!i.drawing&&s.bowReady&&m::length(s.bowIdle-raw)<.01));
    s.ready=true;s.stamp=i.stamp;s.control=i.control;s.timeNs=i.timeNs;s.scale=i.scale;s.smooth=i.smooth;
    s.calibratedHead=i.calibratedHead;s.restOffset=offset;s.filtered=nativeFiltered;s.hmdHeight=i.hmdHeight;
    s.bowKey=i.bowKey;s.bowGeneration=i.bowGeneration;s.bowRequest=i.bowRequest;s.frame=out;s.last=i;
    return true;
}

// The production station adapter stages its motion with this same copied
// frame state. A rejected paired-pose transaction discards both together.
// Preserve a station visit across the nested book's global activity changes,
// while its independent actor/tracking/mode/station epoch still gates motion.
inline bool advance_station_d53(State& s,const Input& i,station_reposition_d53::Input move,Frame& out,
                                station_reposition_d53::Output* moved=nullptr) noexcept {
    if(moved)*moved={};
    auto prior=s.stationMoveD53;
    if(!advance(s,i,out))return false;
    move.active=move.active&&!i.bow&&!i.acquisitionSerial&&i.workstationKindH51&&
        move.owner.character==i.stamp.actor&&move.owner.pose==i.stamp.epoch&&move.owner.control==i.control&&
        move.owner.instance==i.workstationInstanceH51&&move.owner.mode==i.workstationModeEpochH51&&
        move.owner.kind==i.workstationKindH51&&move.sample==i.stamp.sample&&move.tick==i.stamp.tick&&
        std::fabs(move.scale-i.scale)<1.e-6;
    const auto result=station_reposition_d53::advance(prior,move);
    s.stationMoveD53=prior;
    station_reposition_d53::apply(out,result);
    // s.frame remains the unshifted native foundation: a duplicate original
    // callback can never stack this virtual displacement a second time.
    if(moved)*moved=result;
    return true;
}

// Only the proven native stance component is a shared coordinate change.
// Actual HMD movement and other native Z motion remain physical inputs.
inline m::Vec3 native_stance_delta_r93(const Frame& a,const Frame& b) noexcept {
    if(!a.valid||!b.valid||!a.nativeStanceKnownR93||!b.nativeStanceKnownR93||!a.hmdHeight||!b.hmdHeight||
       a.referenceDomainD86!=ReferenceDomainD86::Ordinary||b.referenceDomainD86!=ReferenceDomainD86::Ordinary||
       a.acquisitionSerial||b.acquisitionSerial||a.workstationKindH51||b.workstationKindH51||
       !a.stamp.actor||a.stamp.actor!=b.stamp.actor||a.stamp.epoch!=b.stamp.epoch||
       !a.control||a.control!=b.control||a.stamp.sample<=b.stamp.sample||a.stamp.tick<b.stamp.tick||
       a.stamp.tick-b.stamp.tick>80||!std::isfinite(a.scale)||a.scale<.1||a.scale>10||std::fabs(a.scale-b.scale)>1e-6)return {};
    const double height=native_crouch_r93::heightMeters/a.scale;
    const auto exact=[&](double v){return std::isfinite(v)&&(v==0||std::fabs(v+height)<1e-9);};
    return exact(a.nativeCrouchOffsetR93)&&exact(b.nativeCrouchOffsetR93)?m::Vec3{0,0,a.nativeCrouchOffsetR93-b.nativeCrouchOffsetR93}:m::Vec3{};
}
// R98: the complete shared-frame Z change between two consecutive frames of the
// same owner. HMD height keeps the exact R93 stance step. NATIVE height (and
// the COMBINED blend) move the whole anchor - camera, head and hand goals -
// with the native pelvis every frame (walk bob, crouch). The held weapon's
// motor/intent history must take the SAME change, or the weapon body is
// dragged after the hands by its drive and trails behind/below them.
inline double shared_height_offset_r98(const Frame& f) noexcept {
    const double v=f.nativeCrouchOffsetR93+(f.heightDrivenR98?f.heightOffsetR98:0.);
    return std::isfinite(v)?v:0.;
}
inline m::Vec3 shared_frame_delta_r98(const Frame& a,const Frame& b) noexcept {
    if(!a.heightDrivenR98&&!b.heightDrivenR98)return native_stance_delta_r93(a,b);
    if(!a.valid||!b.valid||a.hmdHeight!=b.hmdHeight||a.combinedHeightR98!=b.combinedHeightR98||
       a.referenceDomainD86!=ReferenceDomainD86::Ordinary||b.referenceDomainD86!=ReferenceDomainD86::Ordinary||
       a.acquisitionSerial||b.acquisitionSerial||a.workstationKindH51||b.workstationKindH51||
       !a.stamp.actor||a.stamp.actor!=b.stamp.actor||a.stamp.epoch!=b.stamp.epoch||
       !a.control||a.control!=b.control||a.stamp.sample<=b.stamp.sample||a.stamp.tick<b.stamp.tick||
       a.stamp.tick-b.stamp.tick>80||!std::isfinite(a.scale)||a.scale<.1||a.scale>10||std::fabs(a.scale-b.scale)>1e-6)return {};
    const double dz=shared_height_offset_r98(a)-shared_height_offset_r98(b);
    return std::isfinite(dz)&&std::fabs(dz)<=.60?m::Vec3{0,0,dz}:m::Vec3{};
}
struct EyeState {std::uintptr_t actor=0;std::uint64_t epoch=0,control=0;double scale=1;m::Vec3 offset{},calibratedHead{},restOffset{},forward{};bool ready=false,headingReady=false;};
inline bool eye(EyeState& s,const Frame& f,m::Stamp committed,std::uint64_t control,std::uint64_t now,
                m::Vec3 nativeEye,m::Vec3& result,m::Vec3 nativeForward={},m::Vec3* bowForward=nullptr) noexcept {
    result={};if(bowForward)*bowForward={};
    const double qn=f.rotation.x*f.rotation.x+f.rotation.y*f.rotation.y+f.rotation.z*f.rotation.z+f.rotation.w*f.rotation.w;
    if(!f.valid||!same_stamp(f.stamp,committed)||f.control!=control||!m::fresh(f.stamp,committed.epoch,committed.actor,now,80)||
       !m::finite(f.root)||!m::finite(f.anchor)||!m::finite(f.nativeHead)||!m::finite(nativeEye)||
       !std::isfinite(f.scale)||f.scale<.1||f.scale>10||!std::isfinite(qn)||std::fabs(qn-1)>.01)return false;
    if(s.actor!=f.stamp.actor||s.epoch!=f.stamp.epoch||std::fabs(s.scale-f.scale)>1e-6||
       m::length(s.calibratedHead-f.calibratedHead)>1e-6||m::length(s.restOffset-f.restOffset)>1e-6){
        s={};s.actor=f.stamp.actor;s.epoch=f.stamp.epoch;s.control=f.control;s.scale=f.scale;
        s.calibratedHead=f.calibratedHead;s.restOffset=f.restOffset;
    }
    s.control=f.control; // A mode toggle retires temporal motion, not this same-body anatomical eye offset.
    if(!s.ready){
        if(!f.neutral)return false;
        const m::Quat inv{-f.rotation.x,-f.rotation.y,-f.rotation.z,f.rotation.w};
        const auto offset=m::rotate(inv,nativeEye-f.root)*(1/f.scale)-f.nativeHead;
        if(!m::finite(offset)||m::length(offset)>.30)return false;
        s.offset=offset;s.ready=true;
    }
    result=f.root+m::rotate(f.rotation,(f.anchor+s.offset)*f.scale);
    if(!m::finite(result)||m::length(result-nativeEye)>1.25)return false;
    // Measure a neutral CAMERA heading relative to the actual character-world
    // root, not a torso/spine bone or the drawn native AimPose. Retain only that
    // local direction; every application uses the CURRENT joined world root.
    // The HMD quaternion is still applied once by camera_hook after this leaf.
    if(!s.headingReady&&f.neutral&&m::finite(nativeForward)){
        const m::Quat inv{-f.rotation.x,-f.rotation.y,-f.rotation.z,f.rotation.w};
        auto local=m::rotate(inv,nativeForward);local.z=0;
        const double n=m::length(local);
        if(std::isfinite(n)&&n>1e-5){s.forward=local*(1/n);s.headingReady=true;}
    }
    if((f.bow||f.workstationKindH51)&&s.headingReady&&bowForward){ // FREE VRIK workstations retain the neutral root-relative heading like a drawn bow
        auto world=m::rotate(f.rotation,s.forward);world.z=0;
        const double n=m::length(world);
        if(std::isfinite(n)&&n>1e-5)*bowForward=world*(1/n);
    }
    return true;
}
}
#include "camera_hold_e1.h" // E1: camera base hold across a short body-frame gap
