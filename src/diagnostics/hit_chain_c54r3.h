#pragma once
// F3 only. Completed production inputs/results; no native reads, gameplay writes,
// allocation, formatting or I/O in the pose producer. Missing geometry is UNKNOWN.
#include "../vrik/physical_weapon.h"
#include "capture168.h"
#include <array>
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstring>
namespace kcd2vr::diagnostics::hit_chain_c54r3 {
namespace ph=kcd2vr::physical;
enum Flag : unsigned {Allowed=1,Attacks=2,Tracked=4,IntentTracked=8,Active=16,Owned=32,
    HandlingOnly=64,Guard=128,Known=256,Blocked=512,Deferred=1024,Plane=2048,
    Candidate=4096,Prepared=8192,Spent=16384,World=32768,ContactEnabled=65536,
    GripRequired=131072,NativeBusy=262144,NativeKnown=524288,DrawHandlingD63=1048576,DrawHandoffD63=2097152};
struct Snapshot {
    std::uint64_t session=0,tick=0,sample=0,key=0,generation=0,epoch=0,request=0,gesture=0,impactGesture=0,impactTick=0,impactSample=0;
    unsigned side=0,flags=0,invalid=0,release=0,commands=0;int classId=-1,target=0,part=-1,body=-1,material=-1,surface=-1;
    ph::Vec3 origin{};int originZone=-1,requestedZone=-1,strokeKind=0;bool originKnown=false;
    ph::Pose requestedWeapon{},allowedWeapon{},world{};ph::Vec3 tip{};double scale=1,grip=0,trigger=0,reachExcess=0;
    ph::Contact103 contact{};ph::ImpactRecord178r30 impact{};melee118::Observation motion{};
    ph::Output::Strike178 strike{};double quiet=0,cooldown=0;bool rearmed=false,liveGesture=false;
    bool intentReady=false,relativeReady=false,gripRearm=false,heldMotion=false;
    bool normalHybridH31=false,legacyIntentR16=false,normalIntentEnabled=false,normalOwnerCurrent=false;
    char retirementReasonH29[96]{};unsigned retirementLineH29=0;
    ph::Output::TraceC54R3 kernel{};ph::Output::ResetH29 reset{};std::uint64_t committedGesture=0,committedTick=0;
};
inline Snapshot snapshot(const ph::Input& in,const ph::Output& out,const std::array<ph::Profile,2>& profiles,
        const ph::State& state,int side,std::uint64_t session) noexcept {
    Snapshot s{};if(side<0||side>1)return s;const auto& p=profiles[side];const auto& w=state.weapon[side];
    s.reset=out.resetH29[side];s.committedGesture=w.committedGestureH26.gesture;s.committedTick=w.committedGestureH26.tick;
    s.origin=w.origin167;s.originZone=w.originZone167;s.requestedZone=w.zone117;s.strokeKind=int(w.stroke117);s.originKnown=w.gesture59c3&&(w.stroke118.active||w.prepared||w.released44c1);
    s.session=session;s.tick=in.tick;s.sample=in.sample;s.key=p.key;s.generation=p.generation;
    s.epoch=in.bodyEpoch;s.request=in.epoch;s.gesture=w.gesture59c3;s.side=unsigned(side);s.classId=p.classId;
    s.flags=(in.allowed?Allowed:0)|(in.attacks?Attacks:0)|(in.tracked[side]?Tracked:0)|(in.intentTracked[side]?IntentTracked:0)|
        (out.active[side]?Active:0)|(out.owned[side]?Owned:0)|(out.handlingOnly55c1[side]?HandlingOnly:0)|
        (out.guardBySide30c1[side]?Guard:0)|(out.contactKnown[side]?Known:0)|(out.blocked[side]?Blocked:0)|
        (out.sweepDeferred115[side]?Deferred:0)|(out.contact119[side].plane?Plane:0)|(w.stroke118.active?Candidate:0)|
        (w.prepared?Prepared:0)|(w.stroke118.spent?Spent:0)|(in.haveWorld?World:0)|(in.contact?ContactEnabled:0)|
        (in.requirePrimaryGrip114?GripRequired:0)|(in.nativeAttackBusy178?NativeBusy:0)|(in.nativeStateKnown178?NativeKnown:0)|
        (state.drawDecisionD63[side].blocked?DrawHandlingD63:0)|(state.drawDecisionD63[side].released?DrawHandoffD63:0);
    s.invalid=out.invalidMask55c1[side];s.release=out.releaseReason121[side];s.grip=in.grip[side];s.trigger=in.trigger[side];
    s.reachExcess=out.acquisitionReachError124[side];s.quiet=w.stroke118.quiet;s.cooldown=w.stroke118.cooldown;s.rearmed=w.stroke118.ready;
    s.intentReady=w.intentReady;s.relativeReady=w.relativeIntentReady;s.gripRearm=w.gripRearm;s.heldMotion=in.heldMotionAttacks;
    s.normalHybridH31=ph::normal_hybrid_intent_h31(in);s.legacyIntentR16=in.intent59c16;
    s.normalIntentEnabled=ph::normal_intent_enabled_h30(in);
    s.normalOwnerCurrent=out.normalIntentH30[side].current&&ph::command_authority_h30(in,out,p,side);
    s.liveGesture=side==0&&out.primaryStrokeGesture19!=0;s.commands=out.commandCount;
    s.requestedWeapon=ph::compose(in.hands[side],ph::inverse(p.grip[side]));s.allowedWeapon=out.weapons[side];
    s.world=in.world;s.scale=in.worldScale;s.tip=p.tip;s.contact=out.contact119[side];s.impact=out.impactRecord178r30[side];
    const auto& c=s.contact;s.target=int(c.native172.entityId);s.part=c.native172.part;s.body=int(c.native172.body);
    s.surface=c.native172.surfaceIdx;s.material=c.native172.bodyMaterial;
    s.impactGesture=s.impact.gesture59c3;s.impactTick=s.impact.tick;s.impactSample=s.impact.sample;
    if(side==0){s.motion=out.melee118;s.strike=out.strike178;s.kernel=out.hitTraceC54R3;}return s;
}
inline const char* gate(const Snapshot& s) noexcept {
    if(!(s.flags&Allowed))return "GAMEPLAY_DISALLOWED";if(!(s.flags&Tracked))return "TRACKING_UNAVAILABLE";
    if(!s.key)return "NO_HELD_PROFILE";if(s.classId<0)return "PROFILE_UNAVAILABLE";
    if(!(s.flags&Active))return "PHYSICAL_OWNER_INACTIVE";if(!(s.flags&Owned))return "PHYSICAL_OWNER_UNOWNED";
    if(s.flags&DrawHandlingD63)return (s.flags&DrawHandoffD63)?"DRAW_HANDOFF_D63":"DRAW_EXTRACTION_HANDLING_D63";
    if(s.flags&HandlingOnly)return s.normalOwnerCurrent?"POSE_RECOVERY_INTENT_OWNED_NEW_CONTACT_DISABLED":"HANDLING_RECOVERY_NO_ATTACK_AUTHORITY";
    if(!(s.flags&Attacks))return "ATTACK_POLICY_DISABLED";
    if((s.flags&GripRequired)&&s.grip<.25)return "PRIMARY_GRIP_RELEASED";
    if(!(s.flags&IntentTracked))return "INTENT_FRAME_UNAVAILABLE";
    if(s.kernel.observed&&!s.kernel.attackIntent)return "ACTUAL_PRE_RECOGNIZER_ATTACK_INTENT_FALSE";
    if(s.kernel.observed&&!s.kernel.continuous)return "ACTUAL_PRE_RECOGNIZER_DISCONTINUOUS_OR_SPEED_OUTLIER";
    if(s.flags&Guard)return "POSITIONAL_GUARD";return "SEE_ACTUAL_RECOGNIZER_REASON";
}
struct Event {Snapshot sample{};std::uint64_t firstSample=0,steps=0,moving=0,onsets=0,contacts=0,noCandidate=0,suppressed=0,endedGesture=0;
    std::array<std::uint64_t,14> reasons{};bool summary=false,endedWithoutCandidate=false,retired=false;};
struct Lane {std::uint64_t session=0,sample=0,lastEvent=0,lastSummary=0,firstSample=0,steps=0,moving=0,onsets=0,contacts=0,noCandidate=0,suppressed=0;
    std::uint64_t gesture=0;bool live=false,hadContact=false;unsigned flags=0;int reason=-1;
    std::uint64_t resetGeneration=0,resetEpoch=0,resetRequest=0,resetGesture=0,resetCommit=0;
    unsigned resetReason=0,resetInvalid=0;bool emptyReset=false;
    std::array<std::uint64_t,14> reasons{};};
class Recorder {
    std::atomic_flag lock=ATOMIC_FLAG_INIT;std::array<Event,64> queue{};unsigned head=0,tail=0;
    std::array<Lane,2> lane{};
    bool push(const Event& e) noexcept {const auto next=(head+1)%queue.size();if(next==tail){dropped.fetch_add(1);return false;}
        queue[head]=e;head=unsigned(next);enqueued.fetch_add(1);return true;}
public:
    std::atomic<std::uint64_t> dropped{0},contention{0},observed{0},enqueued{0},drained{0},discarded{0},formatterClipped{0};
    bool stats(std::array<Lane,2>& out) noexcept {if(lock.test_and_set(std::memory_order_acquire))return false;out=lane;lock.clear(std::memory_order_release);return true;}
    // active=false takes no lock and changes no recorder state. A new session
    // resets only this observer, never the production recognizer or its gesture.
    void observe(const Snapshot& s,bool active) noexcept {
        if(!active||!s.tick||!s.sample||s.side>1)return;
        if(lock.test_and_set(std::memory_order_acquire)){contention.fetch_add(1);return;}
        struct End{std::atomic_flag& l;~End(){l.clear(std::memory_order_release);}} end{lock};
        auto& l=lane[s.side];if(l.session!=s.session){l={};l.session=s.session;l.lastSummary=s.tick;}
        if(l.sample==s.sample)return;observed.fetch_add(1);l.sample=s.sample;if(!l.firstSample)l.firstSample=s.sample;++l.steps;
        const auto reason=unsigned(s.motion.reason);if(reason<l.reasons.size())++l.reasons[reason];
        const bool moving=s.motion.handSpeed>.25||s.motion.tipSpeed>.5||s.motion.rotSpeed>.5||
            (s.kernel.observed&&(s.kernel.modelHandSpeed>.25||s.kernel.relativeHandSpeed>.25||s.kernel.modelTipSpeed>.5||s.kernel.relativeTipSpeed>.5));
        if(moving)++l.moving;if(moving&&!s.liveGesture)++l.noCandidate;
        const bool onset=s.liveGesture&&(!l.live||l.gesture!=s.gesture);if(onset)++l.onsets;
        const bool ended=l.live&&(!s.liveGesture||l.gesture!=s.gesture);
        if(s.impact.valid&&s.impact.gesture59c3==l.gesture&&l.gesture)l.hadContact=true;
        Event e{};e.sample=s;e.endedGesture=ended?l.gesture:0;e.endedWithoutCandidate=ended&&!l.hadContact;
        if(onset)l.hadContact=false;
        if(s.impact.valid&&s.impact.tick==s.tick&&s.impact.sample==s.sample){++l.contacts;l.hadContact=true;}
        const bool summary=s.tick<l.lastSummary||s.tick-l.lastSummary>=1000;
        // Every actual classifier result is counted, even when detail is
        // rate-limited. Commands/onsets/terminal edges retain full snapshots.
        // An empty hand has no physical profile on every sample. Count every
        // result, but report its unchanged reset as one observer occurrence.
        const bool repeatedEmptyReset=!s.key&&s.reset.reason&&l.emptyReset&&
            l.resetGeneration==s.generation&&l.resetEpoch==s.epoch&&l.resetRequest==s.request&&
            l.resetReason==s.reset.reason&&l.resetInvalid==s.invalid&&l.reason==int(reason)&&
            l.resetGesture==s.reset.priorGesture&&l.resetCommit==s.reset.priorCommit;
        const bool edge=onset||ended||s.commands||s.strike.tick==s.tick||(s.reset.reason&&!repeatedEmptyReset);
        const bool changed=l.reason!=int(reason)||l.flags!=s.flags;
        const bool due=!l.lastEvent||s.tick<l.lastEvent||s.tick-l.lastEvent>=(moving?100:1000);
        if(edge||summary||(due&&(changed||moving))){
            e.summary=summary;e.firstSample=l.firstSample;e.steps=l.steps;e.moving=l.moving;e.onsets=l.onsets;e.contacts=l.contacts;e.noCandidate=l.noCandidate;e.suppressed=l.suppressed;e.reasons=l.reasons;
            push(e);l.lastEvent=s.tick;if(summary)l.lastSummary=s.tick;
        }else ++l.suppressed;
        l.live=s.liveGesture;l.gesture=s.gesture;l.reason=int(reason);l.flags=s.flags;
        l.emptyReset=!s.key&&s.reset.reason;l.resetGeneration=s.generation;l.resetEpoch=s.epoch;l.resetRequest=s.request;
        l.resetReason=s.reset.reason;l.resetInvalid=s.invalid;l.resetGesture=s.reset.priorGesture;l.resetCommit=s.reset.priorCommit;
    }
    void retire(const ph::State& state,const char* reason,unsigned sourceLine,std::uint64_t now) noexcept {
        if(!capture168::active(capture168::Domain::Combat,now))return;
        if(lock.test_and_set(std::memory_order_acquire)){++contention;return;}
        for(unsigned side=0;side<2;++side){const auto& w=state.weapon[side];if(!w.gesture59c3&&!w.committedGestureH26.gesture)continue;
            Event e{};e.retired=true;auto& s=e.sample;s.session=capture168::gate.session[unsigned(capture168::Domain::Combat)].load();s.tick=now;s.sample=state.sample;s.epoch=state.bodyEpoch;s.request=state.epoch;s.side=side;s.key=w.key;s.generation=w.generation;s.gesture=w.gesture59c3;s.committedGesture=w.committedGestureH26.gesture;s.committedTick=w.committedGestureH26.tick;s.retirementLineH29=sourceLine;std::strncpy(s.retirementReasonH29,reason?reason:"UNKNOWN",sizeof(s.retirementReasonH29)-1);push(e);
        }lock.clear(std::memory_order_release);
    }
    bool pop(Event& e) noexcept {if(lock.test_and_set(std::memory_order_acquire))return false;
        const bool have=tail!=head;if(have){e=queue[tail];tail=unsigned((tail+1)%queue.size());drained.fetch_add(1);}lock.clear(std::memory_order_release);return have;}
};
inline Recorder recorder;
inline std::atomic_flag drainLock=ATOMIC_FLAG_INIT;
inline bool consumerWasActive=false;inline std::uint64_t consumerSession=0;
template<class Write> inline void drain(std::uint64_t now,Write write) noexcept {
    if(drainLock.test_and_set(std::memory_order_acquire))return;
    struct End{~End(){drainLock.clear(std::memory_order_release);}} end;
    const bool active=capture168::active(capture168::Domain::Combat,now);
    const auto session=capture168::gate.session[unsigned(capture168::Domain::Combat)].load();
    const bool ending=consumerWasActive&&(!active||consumerSession!=session);
    Event e{};unsigned budget=64;while(budget--&&recorder.pop(e)){
        if(!active||e.sample.session!=session){recorder.discarded.fetch_add(1);continue;}const auto& s=e.sample;const auto& d=s.motion;const auto& c=s.contact;
        if(e.retired){char line[700]{};std::snprintf(line,sizeof(line),"H29 STRIKE_RETIRE session=%llu tick=%llu sample=%llu side=%u gesture=%llu committedGesture=%llu committedTick=%llu key=%016llX generation=%llu epoch=%llu request=%llu reason=%s sourceLine=%u nativeContact=NONE authority=REVOKED",(unsigned long long)s.session,(unsigned long long)s.tick,(unsigned long long)s.sample,s.side,(unsigned long long)s.gesture,(unsigned long long)s.committedGesture,(unsigned long long)s.committedTick,(unsigned long long)s.key,(unsigned long long)s.generation,(unsigned long long)s.epoch,(unsigned long long)s.request,s.retirementReasonH29,s.retirementLineH29);write(line);continue;}
        char line[2048]{};int formatted=std::snprintf(line,sizeof(line),
            "C54R3 HIT_CHAIN stage=PRODUCER session=%llu side=%u tick=%llu sample=%llu gesture=%llu key=%016llX generation=%llu epoch=%llu request=%llu gate=%s flags=%X invalidMask=%u release=%u reason=%s kind=%u live=%d rearmed=%d quietMs=%.1f cooldownMs=%.1f commands=%u grip=%.3f trigger=%.3f handSpeed=%.3f tipSpeed=%.3f rotSpeed=%.3f peak=%.3f net=%.4f fastTravel=%.4f rotTravel=%.4f coherence=%.4f ageMs=%.1f axisKnown=%d axisCos=%.4f modelAxisCos=%.4f windup=%d strike{tick=%llu accepted=%d reason=%d speed=%.3f energy=%.3f scale=%.3f} query{reason=%s planned=%u complete=%u rays=%u/%u angle=%.4f/%.4f fraction=%.5f target=%d body=%d part=%d material=%d surface=%d witness=%llu} impact{valid=%d gesture=%llu sample=%llu tick=%llu speed=%.3f} endedWithoutSurfaceCandidate=%d evidence=COMPLETED_PRODUCTION_FRAME nativeAcceptance=NOT_INFERRED",
            (unsigned long long)s.session,s.side,(unsigned long long)s.tick,(unsigned long long)s.sample,(unsigned long long)s.gesture,(unsigned long long)s.key,(unsigned long long)s.generation,(unsigned long long)s.epoch,(unsigned long long)s.request,gate(s),s.flags,s.invalid,s.release,melee118::name(d.reason),unsigned(d.kind),int(s.liveGesture),int(s.rearmed),s.quiet*1000,s.cooldown*1000,s.commands,s.grip,s.trigger,d.handSpeed,d.tipSpeed,d.rotSpeed,d.peak,d.progress,d.fastTravel,d.rotTravel,d.coherence,d.age*1000,int(d.axisKnown),d.axisCos,d.modelAxisCos,int(d.windup),
            (unsigned long long)s.strike.tick,int(s.strike.accepted),s.strike.reason,s.strike.tipSpeed,s.strike.energy,s.strike.scale,sweep169r2::name(c.coverage169r2.reason),c.coverage169r2.planned,c.coverage169r2.completed,c.coverage169r2.completedRays,c.coverage169r2.attemptedRays,c.coverage169r2.checkedAngle,c.coverage169r2.requestedAngle,c.fraction,s.target,s.body,s.part,s.material,s.surface,(unsigned long long)c.witness169r1,int(s.impact.valid),(unsigned long long)s.impactGesture,(unsigned long long)s.impactSample,(unsigned long long)s.impactTick,s.impact.speed,int(e.endedWithoutCandidate));if(formatted<0||std::size_t(formatted)>=sizeof(line))++recorder.formatterClipped;write(line);
        if(s.reset.reason){formatted=std::snprintf(line,sizeof(line),"H29 STRIKE_RESET session=%llu sample=%llu side=%u reasonMask=%u priorGesture=%llu priorCommitTick=%llu dtMs=%.5f desiredJumpM=%.5f desiredJumpRad=%.5f retainedIdentity=%d committedGesture=%llu committedTick=%llu source=ACTUAL_KERNEL_RESET meaning=SOURCE_IDENTITY_ONLY nativeHit=NOT_INFERRED",(unsigned long long)s.session,(unsigned long long)s.sample,s.side,s.reset.reason,(unsigned long long)s.reset.priorGesture,(unsigned long long)s.reset.priorCommit,s.reset.seconds*1000,s.reset.positionJump,s.reset.angleJump,s.reset.retained,(unsigned long long)s.committedGesture,(unsigned long long)s.committedTick);write(line);}
        formatted=std::snprintf(line,sizeof(line),"H29 STRIKE_ORIGIN session=%llu sample=%llu gesture=%llu key=%016llX generation=%llu epoch=%llu request=%llu originKnown=%d originZone=%d requestedNativeZone=%d nativeCommandKind=%d bodyRelativeOrigin=(%.6g,%.6g,%.6g) motionKind=%u axisCos=%.5f modelAxisCos=%.5f windup=%d originSource=ACTUAL_RECOGNIZER_STATE nativeAttackAcceptance=NOT_INFERRED",(unsigned long long)s.session,(unsigned long long)s.sample,(unsigned long long)s.gesture,(unsigned long long)s.key,(unsigned long long)s.generation,(unsigned long long)s.epoch,(unsigned long long)s.request,s.originKnown,s.originZone,s.requestedZone,s.strokeKind,s.origin.x,s.origin.y,s.origin.z,unsigned(d.kind),d.axisCos,d.modelAxisCos,d.windup);if(formatted<0||std::size_t(formatted)>=sizeof(line))++recorder.formatterClipped;write(line);
        const auto modelToWorld=[&](ph::Vec3 p){return ph::point(s.world,p*s.scale);};
        formatted=std::snprintf(line,sizeof(line),"C54R3 HIT_CHAIN stage=BEFORE_RECOGNIZER session=%llu side=%u sample=%llu gesture=%llu measured=%d attackIntent=%d continuous=%d coherent=%d dtMs=%.5f modelTipSpeed=%.5f relativeTipSpeed=%.5f modelHandSpeed=%.5f relativeHandSpeed=%.5f intentReadyAfter=%d relativeReadyAfter=%d gripRearmAfter=%d heldMotionPolicy=%d normalHybridH31=%d legacyIntentR16=%d normalIntentEnabled=%d normalOwnerCurrent=%d gate=%s values=ACTUAL_KERNEL_LOCALS_not_recomputed source=F3_ONLY writes=NONE",
            (unsigned long long)s.session,s.side,(unsigned long long)s.sample,(unsigned long long)s.gesture,int(s.kernel.observed),int(s.kernel.attackIntent),int(s.kernel.continuous),int(s.kernel.coherent),s.kernel.dt*1000,s.kernel.modelTipSpeed,s.kernel.relativeTipSpeed,s.kernel.modelHandSpeed,s.kernel.relativeHandSpeed,int(s.intentReady),int(s.relativeReady),int(s.gripRearm),int(s.heldMotion),int(s.normalHybridH31),int(s.legacyIntentR16),int(s.normalIntentEnabled),int(s.normalOwnerCurrent),gate(s));if(formatted<0||std::size_t(formatted)>=sizeof(line))++recorder.formatterClipped;write(line);
        const auto requested=modelToWorld(s.requestedWeapon.p),allowed=modelToWorld(s.allowedWeapon.p),contact=modelToWorld(c.point);
        const auto requestedTip=modelToWorld(ph::point(s.requestedWeapon,s.tip)),allowedTip=modelToWorld(ph::point(s.allowedWeapon,s.tip));
        formatted=std::snprintf(line,sizeof(line),"C54R3 HIT_CHAIN stage=GEOMETRY session=%llu side=%u sample=%llu gesture=%llu key=%016llX worldKnown=%d scale=%.5f requestWorld=(%.5f,%.5f,%.5f) allowedWorld=(%.5f,%.5f,%.5f) requestModelQ=(%.6f,%.6f,%.6f,%.6f) allowedModelQ=(%.6f,%.6f,%.6f,%.6f) contactWorld=(%.5f,%.5f,%.5f) normalModel=(%.5f,%.5f,%.5f) featureLocal=(%.5f,%.5f,%.5f) seedReachExcess=%.5f pointValidity=REQUIRES_WORLD_AND_KNOWN_PLANE source=EXISTING_DATA nativePivotCorrespondence=UNPROVEN writes=NONE",
            (unsigned long long)s.session,s.side,(unsigned long long)s.sample,(unsigned long long)s.gesture,(unsigned long long)s.key,int(bool(s.flags&World)),s.scale,requested.x,requested.y,requested.z,allowed.x,allowed.y,allowed.z,s.requestedWeapon.q.x,s.requestedWeapon.q.y,s.requestedWeapon.q.z,s.requestedWeapon.q.w,s.allowedWeapon.q.x,s.allowedWeapon.q.y,s.allowedWeapon.q.z,s.allowedWeapon.q.w,contact.x,contact.y,contact.z,c.normal.x,c.normal.y,c.normal.z,c.feature.x,c.feature.y,c.feature.z,s.reachExcess);if(formatted<0||std::size_t(formatted)>=sizeof(line))++recorder.formatterClipped;write(line);
        formatted=std::snprintf(line,sizeof(line),"C54R3 HIT_CHAIN stage=BLADE_ENDPOINTS session=%llu side=%u sample=%llu gesture=%llu key=%016llX requestedTipWorld=(%.5f,%.5f,%.5f) allowedTipWorld=(%.5f,%.5f,%.5f) endpointDeltaM=%.6f collider=%p rayOwner=%p ownerRead=%d mask=%X source=EXISTING_PROFILE_AND_ALLOWED_POSE nativePivotCorrespondence=UNPROVEN",
            (unsigned long long)s.session,s.side,(unsigned long long)s.sample,(unsigned long long)s.gesture,(unsigned long long)s.key,requestedTip.x,requestedTip.y,requestedTip.z,allowedTip.x,allowedTip.y,allowedTip.z,ph::m::length(requestedTip-allowedTip),reinterpret_cast<void*>(c.native172.collider),reinterpret_cast<void*>(c.native172.owner),int(c.native172.ownerRead),c.native172.mask);if(formatted<0||std::size_t(formatted)>=sizeof(line))++recorder.formatterClipped;write(line);
        if(e.endedGesture){formatted=std::snprintf(line,sizeof(line),"C54R3 HIT_CHAIN stage=GESTURE_END session=%llu side=%u gesture=%llu nextGesture=%llu throughSample=%llu noSurfaceCandidateObserved=%d laterNominationMustBeJoined=1 nativeAcceptance=NOT_INFERRED",
            (unsigned long long)s.session,s.side,(unsigned long long)e.endedGesture,(unsigned long long)s.gesture,(unsigned long long)s.sample,int(e.endedWithoutCandidate));if(formatted<0||std::size_t(formatted)>=sizeof(line))++recorder.formatterClipped;write(line);}
        if(e.summary){formatted=std::snprintf(line,sizeof(line),"C54R3 HIT_CHAIN stage=COUNTS session=%llu side=%u firstSample=%llu lastSample=%llu steps=%llu movingFrames=%llu onsets=%llu surfaceCandidates=%llu movingWithoutLiveGesture=%llu suppressedDetails=%llu droppedEvents=%llu contendedSamples=%llu reasons{idle=%llu rearm=%llu slow=%llu candidate=%llu prepared=%llu committed=%llu withdrawn=%llu short=%llu incoherent=%llu timeout=%llu guard=%llu unknown=%llu blocked=%llu unavailable=%llu} counters=SESSION_CUMULATIVE queueCounters=PROCESS_TOTALS movingFramesAreNotHumanAttempts=1",
            (unsigned long long)s.session,s.side,(unsigned long long)e.firstSample,(unsigned long long)s.sample,(unsigned long long)e.steps,(unsigned long long)e.moving,(unsigned long long)e.onsets,(unsigned long long)e.contacts,(unsigned long long)e.noCandidate,(unsigned long long)e.suppressed,(unsigned long long)recorder.dropped.load(),(unsigned long long)recorder.contention.load(),(unsigned long long)e.reasons[0],(unsigned long long)e.reasons[1],(unsigned long long)e.reasons[2],(unsigned long long)e.reasons[3],(unsigned long long)e.reasons[4],(unsigned long long)e.reasons[5],(unsigned long long)e.reasons[6],(unsigned long long)e.reasons[7],(unsigned long long)e.reasons[8],(unsigned long long)e.reasons[9],(unsigned long long)e.reasons[10],(unsigned long long)e.reasons[11],(unsigned long long)e.reasons[12],(unsigned long long)e.reasons[13]);if(formatted<0||std::size_t(formatted)>=sizeof(line))++recorder.formatterClipped;write(line);}
    }
    if(ending){std::array<Lane,2> stats{};const bool copied=recorder.stats(stats);
        bool known=false;if(copied)for(const auto& l:stats)known|=l.session==consumerSession&&l.steps!=0;
        char line[1536]{};
        std::snprintf(line,sizeof(line),"V66.168 CAPTURE domain=COMBAT detail=HIT_CHAIN_HEALTH session=%llu state=RECORDER_END tick=%llu queueCounters=PROCESS_TOTALS observedSamples=%llu enqueued=%llu drained=%llu discardedOffOrOldSession=%llu droppedEvents=%llu contendedSamples=%llu formatterClipped=%llu finalStatsAvailable=%d endDoesNotMeanNativeHit=1",
            (unsigned long long)consumerSession,(unsigned long long)now,(unsigned long long)recorder.observed.load(),(unsigned long long)recorder.enqueued.load(),(unsigned long long)recorder.drained.load(),(unsigned long long)recorder.discarded.load(),(unsigned long long)recorder.dropped.load(),(unsigned long long)recorder.contention.load(),(unsigned long long)recorder.formatterClipped.load(),int(known));write(line);
        if(known)for(unsigned side=0;side<stats.size();++side){const auto& l=stats[side];if(l.session!=consumerSession||!l.steps)continue;
            std::snprintf(line,sizeof(line),"V66.168 CAPTURE domain=COMBAT detail=HIT_CHAIN_COUNTS side=%u session=%llu firstSample=%llu lastSample=%llu steps=%llu movingFrames=%llu onsets=%llu surfaceCandidates=%llu movingWithoutLiveGesture=%llu suppressedDetails=%llu reasons{idle=%llu rearm=%llu slow=%llu candidate=%llu prepared=%llu committed=%llu withdrawn=%llu short=%llu incoherent=%llu timeout=%llu guard=%llu unknown=%llu blocked=%llu unavailable=%llu} counters=SESSION_FINAL movingFramesAreNotHumanAttempts=1",
                side,(unsigned long long)l.session,(unsigned long long)l.firstSample,(unsigned long long)l.sample,(unsigned long long)l.steps,(unsigned long long)l.moving,(unsigned long long)l.onsets,(unsigned long long)l.contacts,(unsigned long long)l.noCandidate,(unsigned long long)l.suppressed,
                (unsigned long long)l.reasons[0],(unsigned long long)l.reasons[1],(unsigned long long)l.reasons[2],(unsigned long long)l.reasons[3],(unsigned long long)l.reasons[4],(unsigned long long)l.reasons[5],(unsigned long long)l.reasons[6],(unsigned long long)l.reasons[7],(unsigned long long)l.reasons[8],(unsigned long long)l.reasons[9],(unsigned long long)l.reasons[10],(unsigned long long)l.reasons[11],(unsigned long long)l.reasons[12],(unsigned long long)l.reasons[13]);write(line);}
    }
    consumerWasActive=active;consumerSession=session;
}
}
