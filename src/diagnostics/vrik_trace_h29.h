#pragma once
#include "capture168.h"
#include "contact_links_d77.h"
#include "../vrik/pose_math.h"
#include "../vrik/elbow_r89.h"
#include <array>
#include <atomic>
#include <cstdio>
#include <cstring>
namespace kcd2vr::diagnostics::vrik_h29 {
namespace pc=pose_core;
// Copied at existing boundaries; no native observer or pose authority here.
enum class Kind : unsigned { Cycle, Skip, Render, Physics, Count };
enum class FingerOwner : unsigned { Unreached, NativeCapture, NativeOrObject, FreeRecipe, NativeMissingReference };
enum class ElbowSolverR94 : unsigned {Unreached,Continuous,FrikLab,ShoulderD45,HandR89};
inline const char* solverR94(ElbowSolverR94 s) noexcept {
 return s==ElbowSolverR94::HandR89?"HAND_R89":s==ElbowSolverR94::ShoulderD45?"SHOULDER_D45":
  s==ElbowSolverR94::FrikLab?"FRIK_LAB":s==ElbowSolverR94::Continuous?"CONTINUOUS":"UNREACHED";
}
inline const char* fingerOwner(FingerOwner v) noexcept {
 return v==FingerOwner::NativeCapture?"NATIVE_CAPTURE":v==FingerOwner::NativeOrObject?"NATIVE_OR_OBJECT_LOCALS":
  v==FingerOwner::FreeRecipe?"FREE_RECIPE":v==FingerOwner::NativeMissingReference?"NATIVE_MISSING_REFERENCE":
  "UNREACHED";
}
// D86 corrective audit: retain the actual hand-reference and chest-basis
// inputs. A smooth pole alone cannot prove those inputs are anatomical.
struct ArmBasisD86 {
 bool available=false,handConversionKnown=false;
 int upper=-1,forearm=-1,hand=-1;
 std::uint64_t elbowPolicy=0;
 pc::Vec3 restUpper{},restForearm{},restHand{},chestRightLocal{},chestUpLocal{},hingeNormalLocal{};
 pc::Quat restHandQ{},desiredHandQ{},boneToAnatomy{},chestQ{};
};
struct ArmContext {
 ElbowSolverR94 selectedSolverR94=ElbowSolverR94::Unreached;
 elbow_r89::Input inputR89{};elbow_r89::TraceR94 traceR89{};
 ArmBasisD86 basisD86{};
 contact_d77::Evidence contactD77{};unsigned contactEdgesD77=0;
 pc::Vec3 requested{},goal{},shoulder{},rawPole{},torsoPole{},conditioned{},fallback{},priorAxis{},priorBend{};
 pc::Vec3 torsoA{},torsoB{},torsoSeed{},torsoResult{},bodyBackward{},filteredPole{},axisInput{},solvePole{};
 pc::Vec3 wristCueD86{},headD86{},outwardD86{},upD86{},bowAwayD86{};
 double drawWeightD86=0,intrusionD86=0,filteredIntrusionD86=0,costD86=0;unsigned evaluationsD86=0;
 bool anatomicalD86=false,wristCueValidD86=false;
 double torsoRadius=0;std::uint64_t priorEpoch=0;
 bool torsoInput=false,torsoSeedPresent=false,torsoSearchLab=false,continuousTorsoD45=false,priorReady=false;
 std::uint64_t priorSample=0,priorTick=0,priorNs=0,objectKey=0,objectGeneration=0,heldKey=0,doorSerial=0,contactTick=0;
 double reach=0,conditioning=0,handover=0,hover=0,grip=0,trigger=0;
 unsigned ownerMask=0,contactYield=0,fingerContact=0;int itemState=0,planes=0;
 FingerOwner finger=FingerOwner::Unreached;
 bool entered=false,bowPole=false,frikLab=false,elbowShoulderD45=false,historyCurrent=false,conditionAttemptedD44=false,conditionedOk=false,continuityOk=false,solveOk=false;
 bool fingerCall=false,fingerOk=false,contactFresh=false,curveTrialRequestedH7=false,curveTrialAvailableH7=false;
 bool d71Trial=false,d71LocalSeed=false,d71BodyRelease=false;unsigned d71PartRequeryMask=0;
};
// A default false result is not an attempted failure (not reached or bow policy).
inline const char* conditionStatusD44(const ArmContext& c) noexcept {
 if(!c.entered)return "NOT_ENTERED";
 if(c.selectedSolverR94==ElbowSolverR94::HandR89)return "HAND_R89";
 if(c.anatomicalD86)return "ANATOMICAL_D86";
 if(!c.conditionAttemptedD44)return c.bowPole?"BOW_BYPASS":"NOT_ATTEMPTED";
 return c.conditionedOk?"PASS":"FAIL";
}
struct Joint {int id=-1,parent=-1,digit=-1,segment=-1;pc::Vec3 position{},localPosition{};pc::Quat rotation{},localRotation{};bool valid=false,selected=false,attempted=false;};
struct Arm {Joint upper{},elbow{},wrist{};pc::Vec3 bend{},axis{};double upperLength=0,foreLength=0;unsigned fingers=0;bool mapped=false,open=false,closed=false,history=false;std::uint64_t historyTick=0;ArmContext context{};std::array<Joint,32> finger{};};
struct Record {
 std::uint64_t tick=0,sample=0,epoch=0,session=0,trackingEpoch=0,publication=0,suppressed=0;
 // Observer deltas only; original source clocks are never relabelled.
 std::uint64_t observerOrdinal=0,priorSample=0,priorSourceTick=0,sourceAdvance=0,sourceDelta=UINT64_MAX,sourceNsDelta=UINT64_MAX,observerDelta=UINT64_MAX;
 unsigned observationEdges=0;bool sourceComparable=false,sourceDuplicate=false,sourceBackwards=false;
 std::uint64_t sourceTick=0,inputTick=0,timeNs=0,goodHead=0,goodRight=0,goodLeft=0,currentTrackingEpoch=0,policy=0,control=0;
 std::int64_t locateTargetXrNs=0,heldInputTargetXrNs[2]{};unsigned heldMotorPath=0;
 std::uintptr_t character=0;char stage[72]{},end[48]{};int line=0,reason=0;unsigned tracking=0,context=0,restoreWrites=0;
 Kind kind=Kind::Physics;
 bool committed=false,poseData=false,allowed=false,started=false,nativeCalled=false,nativeReturned=false;
 bool restored=false,restoreOk=false,publishAttempted=false,packetPublished=false,published=false,envelopePublished=false;
 std::uint64_t proposedPublicationD49=0;unsigned consumersD49=0;bool pairedPublishedD49=false;
 bool renderData=false;pc::Vec3 nativeMin{},nativeMax{},allowedMin{},allowedMax{};
 pc::Vec3 rawPosition[3]{},frameRoot{},frameAnchor{},frameOffset{};pc::Quat rawRotation[3]{},frameRotation{};
 std::uint64_t frameSample=0,frameTick=0,framePublication=0;double frameScale=1;bool frameValid=false,worldKnown=false;
 std::array<float,8> modelWorld{};
 bool pelvisKnown=false;pc::Quat nativePelvis{},solvedPelvis{}; // D45: existing original/working MODEL poses
 std::array<Arm,2> arms{};
};
inline bool selected() noexcept {return (capture168::gate.mask.load(std::memory_order_relaxed)&(1u<<unsigned(capture168::Domain::Vrik)))!=0;}
inline bool active(std::uint64_t now) noexcept {return capture168::active(capture168::Domain::Vrik,now);}
inline std::uint64_t age(std::uint64_t now,std::uint64_t original) noexcept {return original&&now>=original?now-original:UINT64_MAX;}
inline std::array<Record,32> queue{};inline unsigned head=0,tail=0;inline std::atomic_flag lock=ATOMIC_FLAG_INIT;
inline std::atomic<std::uint64_t> dropped{0},copied{0},drained{0};
#include "vrik_account_h6.inl"
inline bool push(Record r) noexcept {
 if(!active(r.tick))return false;const unsigned source=unsigned(r.kind);if(source>=unsigned(Kind::Count))return false;
 r.observerOrdinal=metricH6(source,MetricH6::Observed).fetch_add(1,std::memory_order_relaxed)+1;
 const auto session=capture168::gate.session[unsigned(capture168::Domain::Vrik)].load(std::memory_order_acquire);
 if(r.session&&r.session!=session){++metricH6(source,MetricH6::StaleSession);return false;}r.session=session;
 outcomeH6(r,source);
 static thread_local std::array<Record,unsigned(Kind::Count)> previous{};static thread_local std::array<std::uint64_t,unsigned(Kind::Count)> emittedAt{},suppressed{};
 auto& prior=previous[source];auto& emitted=emittedAt[source];auto& skipped=suppressed[source];
 bool changed=prior.session!=r.session||prior.committed!=r.committed||prior.line!=r.line||prior.reason!=r.reason||prior.tracking!=r.tracking||prior.context!=r.context||prior.published!=r.published||prior.restoreOk!=r.restoreOk||prior.pairedPublishedD49!=r.pairedPublishedD49||prior.consumersD49!=r.consumersD49||std::strcmp(prior.stage,r.stage)||std::strcmp(prior.end,r.end);
 bool jump=false;if(r.poseData&&prior.poseData)for(unsigned h=0;h<2;++h){const auto& a=r.arms[h];const auto& b=prior.arms[h];
  changed=changed||a.context.finger!=b.context.finger||a.context.selectedSolverR94!=b.context.selectedSolverR94||a.context.frikLab!=b.context.frikLab||a.context.elbowShoulderD45!=b.context.elbowShoulderD45||a.context.ownerMask!=b.context.ownerMask||a.context.contactYield!=b.context.contactYield||a.context.historyCurrent!=b.context.historyCurrent||a.context.curveTrialRequestedH7!=b.context.curveTrialRequestedH7||a.context.curveTrialAvailableH7!=b.context.curveTrialAvailableH7||a.context.d71Trial!=b.context.d71Trial||a.context.d71LocalSeed!=b.context.d71LocalSeed||a.context.d71PartRequeryMask!=b.context.d71PartRequeryMask;
  r.arms[h].context.contactEdgesD77=contact_d77::edges(a.context.contactD77,b.context.contactD77);
  changed=changed||r.arms[h].context.contactEdgesD77!=0;
  jump=jump||pc::dot(a.bend,b.bend)<0.||pc::length(a.elbow.position-b.elbow.position)>.04;
  for(unsigned i=0;i<a.fingers&&i<32;++i)if(a.finger[i].valid&&b.finger[i].valid)jump=jump||pc::angle(a.finger[i].rotation,b.finger[i].rotation)>.45;}
 observeH6(r,prior,jump,source);const bool sameSession=prior.session==r.session;prior=r;if(emitted&&sameSession&&r.tick>=emitted&&(r.tick-emitted<25||(!changed&&!jump&&r.tick-emitted<250))){++skipped;++metricH6(source,MetricH6::Throttled);if(r.observationEdges)++metricH6(source,MetricH6::SuppressedEdges);return false;}
 r.suppressed=skipped;skipped=0;emitted=r.tick;
 if(lock.test_and_set(std::memory_order_acquire)){++dropped;++metricH6(source,MetricH6::QueueBusy);return false;}const auto next=(head+1)%queue.size();if(next==tail){++dropped;++metricH6(source,MetricH6::QueueFull);lock.clear(std::memory_order_release);return false;}
 queue[head]=r;head=unsigned(next);++copied;lock.clear(std::memory_order_release);return true;
}
// R9 (lane D): FINGER lines were 73% of the F7 recorder (1.64 M lines / 653 MB of 893 MB in 25 min, up to 4k lines/s
// on the game thread). a finger block is written when the hand's fingers really moved (any local joint > 0.1 rad or
// 5 mm since the last written block), the map changed, a new session, or every 250 ms. ARM/ELBOW/RAW stay per record.
struct FingerMemoR9 {std::uint64_t session=0,tick=0;unsigned fingers=0;std::array<pc::Quat,32> q{};std::array<pc::Vec3,32> p{};bool have=false;};
inline bool fingerDueR9(FingerMemoR9& m,const Arm& a,std::uint64_t session,std::uint64_t tick) noexcept {
 const unsigned n=a.fingers<32?a.fingers:32u;bool due=!m.have||m.session!=session||m.fingers!=n||tick<m.tick||tick-m.tick>=250;
 for(unsigned i=0;!due&&i<n;++i){const auto& j=a.finger[i];if(!j.valid)continue;
  due=pc::angle(j.localRotation,m.q[i])>.1||pc::length(j.localPosition-m.p[i])>.005;}
 if(!due)return false;
 m.have=true;m.session=session;m.tick=tick;m.fingers=n;for(unsigned i=0;i<n;++i){m.q[i]=a.finger[i].localRotation;m.p[i]=a.finger[i].localPosition;}
 return true;
}
inline FingerMemoR9 fingerMemoR9[2]{};inline std::atomic<std::uint64_t> fingerBlocksSkippedR9{0};
inline bool pop(Record& r) noexcept {if(lock.test_and_set(std::memory_order_acquire))return false;const bool yes=tail!=head;if(yes){r=queue[tail];tail=(tail+1)%queue.size();++drained;}lock.clear(std::memory_order_release);return yes;}
template<class Writer> inline void drain(std::uint64_t now,Writer&& write) noexcept {
 Record r{};unsigned budget=8;while(budget--&&pop(r)){if(!active(now)||r.session!=capture168::gate.session[unsigned(capture168::Domain::Vrik)].load()){if(unsigned(r.kind)<unsigned(Kind::Count))++metricH6(unsigned(r.kind),MetricH6::Discarded);continue;}char text[1500]{};
 std::snprintf(text,sizeof(text),"VRIK_F7 POSE session=%llu tick=%llu sample=%llu epoch=%llu trackingEpoch=%llu character=%llX publication=%llu committed=%d committedMeaning=SCOPED_WRITE_ACCEPTED finalOwnerPublished=%d stage=%s line=%d reason=%d trackingMask=%u poseData=%d suppressed=%llu queueDropped=%llu renderedVisibility=UNPROVEN writes=NONE",(unsigned long long)r.session,(unsigned long long)r.tick,(unsigned long long)r.sample,(unsigned long long)r.epoch,(unsigned long long)r.trackingEpoch,(unsigned long long)r.character,(unsigned long long)r.publication,r.committed,r.published,r.stage,r.line,r.reason,r.tracking,r.poseData,(unsigned long long)r.suppressed,(unsigned long long)dropped.load());write(text);
 std::snprintf(text,sizeof(text),"VRIK_F7 OBSERVATION session=%llu kind=%s observerOrdinal=%llu sample=%llu sourceTick=%llu sourceComparable=%d priorSample=%llu priorSourceTick=%llu sourceSequenceAdvance=%llu sourceDeltaMs=%llu sourceDeltaNs=%llu observerDeltaMs=%llu duplicateSource=%d sourceBackwards=%d edges=%u originalClocks=UNCHANGED sequenceGapMeaning=NOT_INFERRED unknownDelta=%llu",(unsigned long long)r.session,kindH6(unsigned(r.kind)),(unsigned long long)r.observerOrdinal,(unsigned long long)r.sample,(unsigned long long)r.sourceTick,r.sourceComparable,(unsigned long long)r.priorSample,(unsigned long long)r.priorSourceTick,(unsigned long long)r.sourceAdvance,(unsigned long long)r.sourceDelta,(unsigned long long)r.sourceNsDelta,(unsigned long long)r.observerDelta,r.sourceDuplicate,r.sourceBackwards,r.observationEdges,(unsigned long long)UINT64_MAX);write(text);
 std::snprintf(text,sizeof(text),"VRIK_F7 SOURCE session=%llu sample=%llu sourceTick=%llu inputTick=%llu timeNs=%llu goodTickHRL=%llu/%llu/%llu observedAt=%llu agesSourceInputHRL=%llu/%llu/%llu/%llu/%llu trackingEpoch=%llu currentTrackingEpoch=%llu allowed=%d contextMask=%u policy=%llu control=%llu clocks=ORIGINAL_NOT_REFRESHED ageUnknown=%llu locateTargetXrNs=%lld heldInputTargetXrNsRL=%lld/%lld heldMotorPath=%u motorPaths=NONE:0,KERNEL_DYNAMICS:1,NATIVE_T4:2,KERNEL_DIRECT:3 inputTarget0=DELIVERY_FALLBACK targetDomain=XR",(unsigned long long)r.session,(unsigned long long)r.sample,(unsigned long long)r.sourceTick,(unsigned long long)r.inputTick,(unsigned long long)r.timeNs,(unsigned long long)r.goodHead,(unsigned long long)r.goodRight,(unsigned long long)r.goodLeft,(unsigned long long)r.tick,(unsigned long long)age(r.tick,r.sourceTick),(unsigned long long)age(r.tick,r.inputTick),(unsigned long long)age(r.tick,r.goodHead),(unsigned long long)age(r.tick,r.goodRight),(unsigned long long)age(r.tick,r.goodLeft),(unsigned long long)r.trackingEpoch,(unsigned long long)r.currentTrackingEpoch,r.allowed,r.context,(unsigned long long)r.policy,(unsigned long long)r.control,(unsigned long long)UINT64_MAX,(long long)r.locateTargetXrNs,(long long)r.heldInputTargetXrNs[0],(long long)r.heldInputTargetXrNs[1],r.heldMotorPath);write(text);
 std::snprintf(text,sizeof(text),"VRIK_F7 CYCLE session=%llu sample=%llu sourceTick=%llu end=%s started=%d scopedWriteAccepted=%d nativeOriginalCalled=%d nativeOriginalReturned=%d restoreFinished=%d restoreOk=%d restoreWrites=%u publicationAttempted=%d bodyPacketPublished=%d finalOwnerPublished=%d renderEnvelopePublished=%d proposedPublication=%llu pairedFramePublished=%d consumerEntryMask=%X consumerBits=PRESENCE:1,TOOLS:2,GRASP:4,PASSIVE:8,PHYSICS:10,CONTACT:20,FIST:40,DOOR:80,RELOAD:100 consumerAcceptance=UNOBSERVED originalCall=UNCHANGED meshSubmission=UNOBSERVED",(unsigned long long)r.session,(unsigned long long)r.sample,(unsigned long long)r.sourceTick,r.end,r.started,r.committed,r.nativeCalled,r.nativeReturned,r.restored,r.restoreOk,r.restoreWrites,r.publishAttempted,r.packetPublished,r.published,r.envelopePublished,(unsigned long long)r.proposedPublicationD49,r.pairedPublishedD49,r.consumersD49);write(text);
 if((r.kind==Kind::Cycle||r.kind==Kind::Skip)&&r.sample&&r.sourceTick){for(unsigned source=0;source<3;++source){const auto& p=r.rawPosition[source];const auto& q=r.rawRotation[source];
  std::snprintf(text,sizeof(text),"VRIK_F7 RAW session=%llu sample=%llu sourceTick=%llu source=%s position=(%.6f,%.6f,%.6f) rotation=(%.7f,%.7f,%.7f,%.7f) space=ORIGINAL_XR_SNAPSHOT trackedFlag=NOT_IN_PACKET heldAndValid=SEE_SOURCE",(unsigned long long)r.session,(unsigned long long)r.sample,(unsigned long long)r.sourceTick,source==0?"HEAD":source==1?"RIGHT":"LEFT",p.x,p.y,p.z,q.x,q.y,q.z,q.w);write(text);}
 std::snprintf(text,sizeof(text),"VRIK_F7 FRAME session=%llu sample=%llu sourceTick=%llu frameValid=%d frameSample=%llu frameTick=%llu framePublication=%llu root=(%.6f,%.6f,%.6f) rotation=(%.7f,%.7f,%.7f,%.7f) scale=%.6f anchor=(%.6f,%.6f,%.6f) bodyOffset=(%.6f,%.6f,%.6f) publishWorldKnown=%d publishWorld=(%.7f,%.7f,%.7f,%.7f;%.6f,%.6f,%.6f;%.6f) pelvisKnown=%d nativePelvisQuat=(%.7f,%.7f,%.7f,%.7f) solvedPelvisQuat=(%.7f,%.7f,%.7f,%.7f) pelvisSpace=MODEL frameAndWorld=EXISTING_COPIES_ONLY",(unsigned long long)r.session,(unsigned long long)r.sample,(unsigned long long)r.sourceTick,r.frameValid,(unsigned long long)r.frameSample,(unsigned long long)r.frameTick,(unsigned long long)r.framePublication,r.frameRoot.x,r.frameRoot.y,r.frameRoot.z,r.frameRotation.x,r.frameRotation.y,r.frameRotation.z,r.frameRotation.w,r.frameScale,r.frameAnchor.x,r.frameAnchor.y,r.frameAnchor.z,r.frameOffset.x,r.frameOffset.y,r.frameOffset.z,r.worldKnown,double(r.modelWorld[0]),double(r.modelWorld[1]),double(r.modelWorld[2]),double(r.modelWorld[3]),double(r.modelWorld[4]),double(r.modelWorld[5]),double(r.modelWorld[6]),double(r.modelWorld[7]),r.pelvisKnown,r.nativePelvis.x,r.nativePelvis.y,r.nativePelvis.z,r.nativePelvis.w,r.solvedPelvis.x,r.solvedPelvis.y,r.solvedPelvis.z,r.solvedPelvis.w);write(text);}
 if(r.renderData){std::snprintf(text,sizeof(text),"VRIK_F7 RENDER session=%llu sample=%llu sourceTick=%llu publication=%llu result=%s reason=%d native=(%.4f,%.4f,%.4f)/(%.4f,%.4f,%.4f) returned=(%.4f,%.4f,%.4f)/(%.4f,%.4f,%.4f) exactCaller=HENRY_RENDER_PROXY_GETTER frustumDecision=UNOBSERVED meshVisibility=UNPROVEN",(unsigned long long)r.session,(unsigned long long)r.sample,(unsigned long long)r.sourceTick,(unsigned long long)r.publication,r.reason<0?"ALLOWED_UNION":"NATIVE_FALLBACK",r.reason,r.nativeMin.x,r.nativeMin.y,r.nativeMin.z,r.nativeMax.x,r.nativeMax.y,r.nativeMax.z,r.allowedMin.x,r.allowedMin.y,r.allowedMin.z,r.allowedMax.x,r.allowedMax.y,r.allowedMax.z);write(text);}
 if(!r.poseData)continue;for(unsigned side=0;side<2;++side){const auto& a=r.arms[side];const auto& c=a.context;
 std::snprintf(text,sizeof(text),"VRIK_F7 ARM session=%llu sample=%llu side=%u committed=%d upper=(%.5f,%.5f,%.5f) elbow=(%.5f,%.5f,%.5f) wrist=(%.5f,%.5f,%.5f) lengths=(%.5f,%.5f) bend=(%.5f,%.5f,%.5f) axis=(%.5f,%.5f,%.5f) upperQuat=(%.7f,%.7f,%.7f,%.7f) elbowQuat=(%.7f,%.7f,%.7f,%.7f) wristQuat=(%.7f,%.7f,%.7f,%.7f) bendAxis=COMMITTED_HISTORY elbowHistory=%d historyTick=%llu fingerMap=%d openReady=%d closedReady=%d fingers=%u kind=%s observerOrdinal=%llu",(unsigned long long)r.session,(unsigned long long)r.sample,side,r.committed,a.upper.position.x,a.upper.position.y,a.upper.position.z,a.elbow.position.x,a.elbow.position.y,a.elbow.position.z,a.wrist.position.x,a.wrist.position.y,a.wrist.position.z,a.upperLength,a.foreLength,a.bend.x,a.bend.y,a.bend.z,a.axis.x,a.axis.y,a.axis.z,a.upper.rotation.x,a.upper.rotation.y,a.upper.rotation.z,a.upper.rotation.w,a.elbow.rotation.x,a.elbow.rotation.y,a.elbow.rotation.z,a.elbow.rotation.w,a.wrist.rotation.x,a.wrist.rotation.y,a.wrist.rotation.z,a.wrist.rotation.w,a.history,(unsigned long long)a.historyTick,a.mapped,a.open,a.closed,a.fingers,kindH6(unsigned(r.kind)),(unsigned long long)r.observerOrdinal);write(text);
 std::snprintf(text,sizeof(text),"VRIK_F7 ELBOW session=%llu sample=%llu side=%u entered=%d bowPolicy=%d frikLab=%d elbowShoulderD45=%d historyCurrent=%d priorSample=%llu priorTick=%llu priorTimeNs=%llu conditionAttempted=%d conditionStatus=%s conditionOk=%d continuityOk=%d solveOk=%d conditioning=%.6f reach=%.6f requested=(%.5f,%.5f,%.5f) finalGoal=(%.5f,%.5f,%.5f) shoulder=(%.5f,%.5f,%.5f) rawPole=(%.5f,%.5f,%.5f) torsoPole=(%.5f,%.5f,%.5f) conditioned=(%.5f,%.5f,%.5f) fallback=(%.5f,%.5f,%.5f) priorAxis=(%.5f,%.5f,%.5f) priorBend=(%.5f,%.5f,%.5f) handover=%.4f hover=%.4f",(unsigned long long)r.session,(unsigned long long)r.sample,side,c.entered,c.bowPole,c.frikLab,c.elbowShoulderD45,c.historyCurrent,(unsigned long long)c.priorSample,(unsigned long long)c.priorTick,(unsigned long long)c.priorNs,c.conditionAttemptedD44,conditionStatusD44(c),c.conditionedOk,c.continuityOk,c.solveOk,c.conditioning,c.reach,c.requested.x,c.requested.y,c.requested.z,c.goal.x,c.goal.y,c.goal.z,c.shoulder.x,c.shoulder.y,c.shoulder.z,c.rawPole.x,c.rawPole.y,c.rawPole.z,c.torsoPole.x,c.torsoPole.y,c.torsoPole.z,c.conditioned.x,c.conditioned.y,c.conditioned.z,c.fallback.x,c.fallback.y,c.fallback.z,c.priorAxis.x,c.priorAxis.y,c.priorAxis.z,c.priorBend.x,c.priorBend.y,c.priorBend.z,c.handover,c.hover);write(text);
 // D45: complete numeric replay inputs. These are existing transaction
 // copies, emitted under the same bounded F7 record; no additional native reads.
 std::snprintf(text,sizeof(text),"VRIK_F7 TORSO session=%llu kind=%s observerOrdinal=%llu sample=%llu side=%u inputValid=%d continuousD45=%d searchLab=%d shoulder=(%.17g,%.17g,%.17g) goal=(%.17g,%.17g,%.17g) lengths=(%.17g,%.17g) rawPole=(%.17g,%.17g,%.17g) torsoA=(%.17g,%.17g,%.17g) torsoB=(%.17g,%.17g,%.17g) radius=%.17g seedPresent=%d seed=(%.17g,%.17g,%.17g) searchResult=(%.17g,%.17g,%.17g) finalPreference=(%.17g,%.17g,%.17g) bowPolicy=%d space=MODEL",
  (unsigned long long)r.session,kindH6(unsigned(r.kind)),(unsigned long long)r.observerOrdinal,(unsigned long long)r.sample,side,c.torsoInput,c.continuousTorsoD45,c.torsoSearchLab,
  c.shoulder.x,c.shoulder.y,c.shoulder.z,c.goal.x,c.goal.y,c.goal.z,a.upperLength,a.foreLength,
  c.rawPole.x,c.rawPole.y,c.rawPole.z,c.torsoA.x,c.torsoA.y,c.torsoA.z,c.torsoB.x,c.torsoB.y,c.torsoB.z,c.torsoRadius,
  c.torsoSeedPresent,c.torsoSeed.x,c.torsoSeed.y,c.torsoSeed.z,c.torsoResult.x,c.torsoResult.y,c.torsoResult.z,c.torsoPole.x,c.torsoPole.y,c.torsoPole.z,c.bowPole);write(text);
 // D86 adds no timer/file/observer: these inputs share the existing finite
 // F7 record and are absent when it is off. CPU proxy clearance is not mesh proof.
 if(c.anatomicalD86){
 std::snprintf(text,sizeof(text),"VRIK_F7 ANATOMY_D86 session=%llu kind=%s observerOrdinal=%llu sample=%llu side=%u wristCueKnown=%d wristCue=(%.17g,%.17g,%.17g) head=(%.17g,%.17g,%.17g) outward=(%.17g,%.17g,%.17g) up=(%.17g,%.17g,%.17g) bowAway=(%.17g,%.17g,%.17g) drawWeight=%.17g intrusion=%.17g filteredIntrusion=%.17g cost=%.17g objectiveEvaluations=%u join=TORSO_ELBOW_INPUT_SAME_ID clearance=CAPSULE_SPHERE_PROXY mesh=UNOBSERVED",
  (unsigned long long)r.session,kindH6(unsigned(r.kind)),(unsigned long long)r.observerOrdinal,(unsigned long long)r.sample,side,c.wristCueValidD86,
  c.wristCueD86.x,c.wristCueD86.y,c.wristCueD86.z,c.headD86.x,c.headD86.y,c.headD86.z,c.outwardD86.x,c.outwardD86.y,c.outwardD86.z,
  c.upD86.x,c.upD86.y,c.upD86.z,c.bowAwayD86.x,c.bowAwayD86.y,c.bowAwayD86.z,c.drawWeightD86,c.intrusionD86,c.filteredIntrusionD86,c.costD86,c.evaluationsD86);write(text);
 }
 if(c.basisD86.available){const auto& b=c.basisD86;
 std::snprintf(text,sizeof(text),"VRIK_F7 BASIS_D86 session=%llu kind=%s observerOrdinal=%llu sample=%llu side=%u upperBone=%d foreBone=%d handBone=%d elbowPolicy=%llu handConversionKnown=%d restUpper=(%.17g,%.17g,%.17g) restFore=(%.17g,%.17g,%.17g) restHand=(%.17g,%.17g,%.17g) restHandQ=(%.17g,%.17g,%.17g,%.17g) desiredHandQ=(%.17g,%.17g,%.17g,%.17g) boneToAnatomy=(%.17g,%.17g,%.17g,%.17g) chestQ=(%.17g,%.17g,%.17g,%.17g) chestRightLocal=(%.17g,%.17g,%.17g) chestUpLocal=(%.17g,%.17g,%.17g) hingeNormalLocal=(%.17g,%.17g,%.17g) vectors=MODEL_UNLESS_LOCAL quaternionOrder=XYZW handCue=REFERENCE_INFERENCE finalQuaternions=ARM_SAME_ID mesh=UNOBSERVED",
  (unsigned long long)r.session,kindH6(unsigned(r.kind)),(unsigned long long)r.observerOrdinal,(unsigned long long)r.sample,side,b.upper,b.forearm,b.hand,(unsigned long long)b.elbowPolicy,b.handConversionKnown,
  b.restUpper.x,b.restUpper.y,b.restUpper.z,b.restForearm.x,b.restForearm.y,b.restForearm.z,b.restHand.x,b.restHand.y,b.restHand.z,
  b.restHandQ.x,b.restHandQ.y,b.restHandQ.z,b.restHandQ.w,b.desiredHandQ.x,b.desiredHandQ.y,b.desiredHandQ.z,b.desiredHandQ.w,
  b.boneToAnatomy.x,b.boneToAnatomy.y,b.boneToAnatomy.z,b.boneToAnatomy.w,b.chestQ.x,b.chestQ.y,b.chestQ.z,b.chestQ.w,
  b.chestRightLocal.x,b.chestRightLocal.y,b.chestRightLocal.z,b.chestUpLocal.x,b.chestUpLocal.y,b.chestUpLocal.z,b.hingeNormalLocal.x,b.hingeNormalLocal.y,b.hingeNormalLocal.z);write(text);
 }
 std::snprintf(text,sizeof(text),"VRIK_F7 ELBOW_INPUT session=%llu kind=%s observerOrdinal=%llu sample=%llu side=%u epoch=%llu sourceTick=%llu timeNs=%llu frikLab=%d elbowShoulderD45=%d bowPolicy=%d priorReady=%d priorEpoch=%llu priorSample=%llu priorTick=%llu priorTimeNs=%llu priorAxis=(%.17g,%.17g,%.17g) priorBend=(%.17g,%.17g,%.17g) axisInput=(%.17g,%.17g,%.17g) bodyBackward=(%.17g,%.17g,%.17g) anatomical=(%.17g,%.17g,%.17g) fallback=(%.17g,%.17g,%.17g) reach=%.17g conditionOk=%d continuityOk=%d filteredPole=(%.17g,%.17g,%.17g) solvePole=(%.17g,%.17g,%.17g) finalElbow=(%.9g,%.9g,%.9g) finalWrist=(%.9g,%.9g,%.9g) solveOk=%d",
  (unsigned long long)r.session,kindH6(unsigned(r.kind)),(unsigned long long)r.observerOrdinal,(unsigned long long)r.sample,side,(unsigned long long)r.epoch,(unsigned long long)r.sourceTick,
  (unsigned long long)r.timeNs,c.frikLab,c.elbowShoulderD45,c.bowPole,c.priorReady,(unsigned long long)c.priorEpoch,
  (unsigned long long)c.priorSample,(unsigned long long)c.priorTick,(unsigned long long)c.priorNs,
  c.priorAxis.x,c.priorAxis.y,c.priorAxis.z,c.priorBend.x,c.priorBend.y,c.priorBend.z,c.axisInput.x,c.axisInput.y,c.axisInput.z,
  c.bodyBackward.x,c.bodyBackward.y,c.bodyBackward.z,c.conditioned.x,c.conditioned.y,c.conditioned.z,
  c.fallback.x,c.fallback.y,c.fallback.z,c.reach,c.conditionedOk,c.continuityOk,c.filteredPole.x,c.filteredPole.y,c.filteredPole.z,c.solvePole.x,c.solvePole.y,c.solvePole.z,
  a.elbow.position.x,a.elbow.position.y,a.elbow.position.z,a.wrist.position.x,a.wrist.position.y,a.wrist.position.z,c.solveOk);write(text);
 std::snprintf(text,sizeof(text),"VRIK_F7 ELBOW_SOLVER session=%llu kind=%s observerOrdinal=%llu sample=%llu side=%u solver=%s publication=%llu finalOwnerPublished=%d sourceTick=%llu sourceNs=%llu trackingEpoch=%llu bodyEpoch=%llu",
  (unsigned long long)r.session,kindH6(unsigned(r.kind)),(unsigned long long)r.observerOrdinal,(unsigned long long)r.sample,side,solverR94(c.selectedSolverR94),
  (unsigned long long)r.publication,r.published,(unsigned long long)r.sourceTick,(unsigned long long)r.timeNs,(unsigned long long)r.trackingEpoch,(unsigned long long)r.epoch);write(text);
 if(c.selectedSolverR94==ElbowSolverR94::HandR89){const auto& in=c.inputR89;const auto& t=c.traceR89;const auto seed=in.seedElbow.value_or(pc::Vec3{});
 std::snprintf(text,sizeof(text),"VRIK_F7 R89_INPUT session=%llu kind=%s observerOrdinal=%llu sample=%llu side=%u shoulder=(%.17g,%.17g,%.17g) wrist=(%.17g,%.17g,%.17g) lengths=(%.17g,%.17g) body=(%.17g,%.17g,%.17g) forward=(%.17g,%.17g,%.17g) thumb=(%.17g,%.17g,%.17g) torsoA=(%.17g,%.17g,%.17g) torsoB=(%.17g,%.17g,%.17g) radius=%.17g seedPresent=%d seed=(%.17g,%.17g,%.17g) weights=(%.17g,%.17g,%.17g,%.17g) tau=%.17g rate=%.17g confidenceScale=%.17g torsoMargin=%.17g torsoBand=%.17g reference=ACTUAL_CALIBRATED_INPUT prior=ELBOW_INPUT_SAME_ID",
  (unsigned long long)r.session,kindH6(unsigned(r.kind)),(unsigned long long)r.observerOrdinal,(unsigned long long)r.sample,side,
  in.shoulder.x,in.shoulder.y,in.shoulder.z,in.wrist.x,in.wrist.y,in.wrist.z,in.upper,in.fore,in.body.x,in.body.y,in.body.z,in.forward.x,in.forward.y,in.forward.z,
  in.thumb.x,in.thumb.y,in.thumb.z,in.torsoA.x,in.torsoA.y,in.torsoA.z,in.torsoB.x,in.torsoB.y,in.torsoB.z,in.torsoRadius,bool(in.seedElbow),seed.x,seed.y,seed.z,
  elbow_r89::kBody,elbow_r89::kForward,elbow_r89::kThumb,elbow_r89::kTorso,elbow_r89::kTau,elbow_r89::kRate,elbow_r89::kConfidence,elbow_r89::kTorsoMargin,elbow_r89::kTorsoBand);write(text);
 std::snprintf(text,sizeof(text),"VRIK_F7 R89_OUTPUT session=%llu kind=%s observerOrdinal=%llu sample=%llu side=%u reset=%u previousValid=%d targetValid=%d seedUsed=%d rateClamped=%d timeClamped=%d ok=%d previous=(%.17g,%.17g,%.17g) target=(%.17g,%.17g,%.17g) output=(%.17g,%.17g,%.17g) strength=%.17g confidence=%.17g dt=%.17g change=%.17g step=%.17g resets=NONE:0,COLD:1,EPOCH:2,EXPIRED:3,DUPLICATE:4,INVALID:5,BACKWARDS:6,NO_TARGET:7",
  (unsigned long long)r.session,kindH6(unsigned(r.kind)),(unsigned long long)r.observerOrdinal,(unsigned long long)r.sample,side,unsigned(t.reset),t.previousValid,t.targetValid,t.seedUsed,t.rateClamped,t.timeClamped,t.ok,
  t.previous.x,t.previous.y,t.previous.z,t.target.x,t.target.y,t.target.z,t.output.x,t.output.y,t.output.z,t.strength,t.confidence,t.dt,t.change,t.step);write(text);
 }
 std::snprintf(text,sizeof(text),"VRIK_F7 HAND_OWNER session=%llu sample=%llu side=%u fingerOwner=%s ownerMask=%u heldKey=%llu objectKey=%llu objectGeneration=%llu doorSerial=%llu itemState=%d contactYield=%u contactFresh=%d contactTick=%llu planes=%d fingerContactMask=%u freeRecipeCalled=%d freeRecipeOk=%d grip=%.4f trigger=%.4f d71Trial=%d d71LocalSeed=%d d71PartRequeryMask=%u d71BodyRelease=%d ownershipBits=PHYSICAL:1,PRESENCE:2,PROP:4,DOOR:8,PASSENGER:16,READING:32,BOW_STRING:64,NATIVE_CAPTURE:128 rawSqueezePolicy=UNCHANGED nativeCurveTrial=%s trialReference=%s nativeFinalBlend=UNPROVEN",(unsigned long long)r.session,(unsigned long long)r.sample,side,fingerOwner(c.finger),c.ownerMask,(unsigned long long)c.heldKey,(unsigned long long)c.objectKey,(unsigned long long)c.objectGeneration,(unsigned long long)c.doorSerial,c.itemState,c.contactYield,c.contactFresh,(unsigned long long)c.contactTick,c.planes,c.fingerContact,c.fingerCall,c.fingerOk,c.grip,c.trigger,c.d71Trial,c.d71LocalSeed,c.d71PartRequeryMask,c.d71BodyRelease,c.curveTrialRequestedH7?(!c.curveTrialAvailableH7?"REFERENCE_UNAVAILABLE_DEFAULT":c.fingerCall?"FREE_CURVE_APPLIED":"OTHER_OWNER"):"OFF",c.curveTrialRequestedH7?(c.curveTrialAvailableH7?"CAPTURED_NATIVE_PUNCH_H45":"UNAVAILABLE_BIN_NOT_ACCEPTED"):"NOT_SELECTED");write(text);
 contact_d77::format(c.contactD77,c.contactEdgesD77,r.session,r.sample,side,r.observerOrdinal,unsigned(r.kind),write);
 if(!fingerDueR9(fingerMemoR9[side],a,r.session,r.tick)){++fingerBlocksSkippedR9;continue;}
 for(unsigned i=0;i<a.fingers&&i<32;++i){const auto& j=a.finger[i];std::snprintf(text,sizeof(text),"VRIK_F7 FINGER session=%llu sample=%llu side=%u ordinal=%u bone=%d parent=%d digit=%d joint=%d valid=%d selected=%d writeAttempted=%d committed=%d committedMeaning=SCOPED_WRITE_ACCEPTED finalOwnerPublished=%d modelPos=(%.5f,%.5f,%.5f) modelQuat=(%.6f,%.6f,%.6f,%.6f) localPos=(%.5f,%.5f,%.5f) localQuat=(%.6f,%.6f,%.6f,%.6f)",(unsigned long long)r.session,(unsigned long long)r.sample,side,i,j.id,j.parent,j.digit,j.segment,j.valid,j.selected,j.attempted,r.committed,r.published,j.position.x,j.position.y,j.position.z,j.rotation.x,j.rotation.y,j.rotation.z,j.rotation.w,j.localPosition.x,j.localPosition.y,j.localPosition.z,j.localRotation.x,j.localRotation.y,j.localRotation.z,j.localRotation.w);write(text);}
 }}
 // A finite heartbeat distinguishes armed-but-silent producers from recorder OFF.
 static std::uint64_t last=0,session=0;const auto current=capture168::gate.session[unsigned(capture168::Domain::Vrik)].load();
 if(active(now)&&(session!=current||!last||now<last||now-last>=1000)){session=current;last=now;char text[448]{};
  std::snprintf(text,sizeof(text),"VRIK_F7 HEALTH session=%llu tick=%llu copied=%llu drained=%llu queueDropped=%llu minEventIntervalMs=25 stableIntervalMs=250 drainRecordBudget=8 fingerBlocksUnchangedR9=%llu fingerRule=MOVED_0.1RAD_5MM_OR_250MS renderedVisibility=UNPROVEN",(unsigned long long)session,(unsigned long long)now,(unsigned long long)copied.load(),(unsigned long long)drained.load(),(unsigned long long)dropped.load(),(unsigned long long)fingerBlocksSkippedR9.load());write(text);summariesH6(session,now,write);}
}
}

// Shift+F10 observes existing pose/attachment owners. The finite Hands lease
// owns only these copied diagnostics, never a pose, endpoint or native action.
namespace kcd2vr::diagnostics::hands28 {
namespace pc=pose_core;
struct Local {pc::Quat q{};pc::Vec3 p{};};
struct Joint {
 int id=-1,parent=-1,digit=-1,segment=-1;std::uint32_t crc=0;
 Local native{},final{},open{},closed{},trial{},curveOpen{},curveClosed{};
 bool nativeKnown=false,finalKnown=false;
};
struct Clip {int id=-1,layer=-1,fifo=-1,segment=-1;float phase=0;char name[160]{};};
struct Hand {
 int wrist=-1,count=0;bool map=false,open=false,closed=false,trialReady=false,referenceOwner=false,writer=false;
 bool trialRequested=false,trialActive=false,contact[5]{};
 bool objectWriter=false,objectReady=false,objectFollow=false,peakCandidate=false,heldKnown=false;
 std::uint64_t objectSince=0,objectLastSample=0,objectSequence=0;double nativeClosureScore=0;
 double grip=0,trigger=0,inputCurl[5]{},appliedCurl[5]{};
 std::uint64_t heldKey=0,objectKey=0,generation=0,openSequence=0,closedSequence=0,trialSequence=0;
 std::uint32_t curveFlags[3]{};int curveId[3]{-1,-1,-1},selectedPhase=-1;bool cacheOnly=false;
 Local nativeWrist{},finalWrist{};Joint joints[32]{};
};
struct Record {
 enum class Kind {Pose,Item};Kind kind=Kind::Pose;
 std::uint64_t tick=0,session=0,epoch=0,request=0,sample=0,sourceTick=0,timeNs=0,trackingEpoch=0,rig=0,clipTick=0,clipSerial=0;
 std::uintptr_t character=0,skeleton=0,pose=0,anim=0,item=0,attachment=0,nativeItemPointer=0;
 unsigned clipCount=0;bool clipKnown=false,clipTruncated=false,committed=false,restored=false,published=false,originalReturned=false;
 unsigned reason=0;Hand hand[2]{};Clip clips[12]{};
 int side=-1,joint=-1;std::uint32_t entity=0;std::uint64_t key=0,generation=0,token=0;
 Local nativeItem{},finalItem{},nativeWrist{},finalWrist{},actualItem{},actualWrist{};
 std::array<float,12> actualMatrix{};bool actualMatrixKnown=false,actualItemKnown=false;unsigned itemRoute=0;bool itemPair=false,placed=false,readbackKnown=false,readback=false,keyKnown=false,generationKnown=false;
};
struct Gate {
 std::uint64_t session=0,last=0,lastEdge=0,signature=0;unsigned edges=0,packets=0;
 unsigned due(std::uint64_t serial,std::uint64_t now,std::uint64_t current) noexcept {
  if(!serial||!now)return 0;if(session!=serial){*this={};session=serial;}
  if(packets>=4096)return 0;
  const bool edge=last&&current!=signature;signature=current;
  unsigned reason=0;
  if(!last||now<last||now-last>=100)reason=1;
  else if(edge&&edges<256&&(!lastEdge||now<lastEdge||now-lastEdge>=25)){reason=2;++edges;lastEdge=now;}
  if(reason){last=now;++packets;}return reason;
 }
};
inline bool selected() noexcept {return (capture168::gate.mask.load(std::memory_order_relaxed)&(1u<<unsigned(capture168::Domain::HandReference)))!=0;}
inline bool active(std::uint64_t now) noexcept {return capture168::active(capture168::Domain::HandReference,now);}
inline bool current(const Record& r,std::uint64_t now) noexcept {
 return active(now)&&r.session==capture168::gate.session[unsigned(capture168::Domain::HandReference)].load(std::memory_order_acquire);
}
inline std::array<Record,8> queue{};inline unsigned head=0,tail=0;inline std::atomic_flag lock=ATOMIC_FLAG_INIT;
inline std::atomic<std::uint64_t> dropped{0};
inline bool push(const Record& r,std::uint64_t now) noexcept {
 if(!current(r,now))return false;
 if(lock.test_and_set(std::memory_order_acquire)){++dropped;return false;}
 const auto next=(head+1)%queue.size();bool ok=next!=tail;
 if(ok){queue[head]=r;head=unsigned(next);}else ++dropped;
 lock.clear(std::memory_order_release);return ok;
}
// Worker-only exact reference memo. Dynamic native/final joints keep the full
// existing cadence; reference geometry is emitted once per identity/change.
struct ReferenceMemoD53 {
 std::uint64_t id=0,session=0,epoch=0,request=0,rig=0,generation=0,openSequence=0,closedSequence=0,trialSequence=0;
 std::uintptr_t character=0,skeleton=0;Joint joint{};
};
inline std::array<std::array<ReferenceMemoD53,32>,2> referenceMemoD53{};
inline std::uint64_t referenceSerialD53=0;
inline bool localBitsEqualD53(const Local&a,const Local&b) noexcept {
 const double av[]{a.p.x,a.p.y,a.p.z,a.q.x,a.q.y,a.q.z,a.q.w};
 const double bv[]{b.p.x,b.p.y,b.p.z,b.q.x,b.q.y,b.q.z,b.q.w};
 return std::memcmp(av,bv,sizeof(av))==0;
}
inline bool referenceDueD53(ReferenceMemoD53&m,const Record&r,const Hand&h,const Joint&j) noexcept {
 const bool same=m.id&&m.session==r.session&&m.epoch==r.epoch&&m.request==r.request&&m.rig==r.rig&&
  m.character==r.character&&m.skeleton==r.skeleton&&m.generation==h.generation&&
  m.openSequence==h.openSequence&&m.closedSequence==h.closedSequence&&m.trialSequence==h.trialSequence&&
  m.joint.id==j.id&&m.joint.parent==j.parent&&m.joint.crc==j.crc&&m.joint.digit==j.digit&&m.joint.segment==j.segment&&
  localBitsEqualD53(m.joint.open,j.open)&&localBitsEqualD53(m.joint.closed,j.closed)&&localBitsEqualD53(m.joint.trial,j.trial)&&
  localBitsEqualD53(m.joint.curveOpen,j.curveOpen)&&localBitsEqualD53(m.joint.curveClosed,j.curveClosed);
 if(same)return false;
 m={};m.id=++referenceSerialD53;if(!m.id)m.id=++referenceSerialD53;
 m.session=r.session;m.epoch=r.epoch;m.request=r.request;m.rig=r.rig;m.generation=h.generation;
 m.character=r.character;m.skeleton=r.skeleton;m.openSequence=h.openSequence;m.closedSequence=h.closedSequence;m.trialSequence=h.trialSequence;m.joint=j;
 return true;
}
template<class Write> inline void drain(std::uint64_t now,Write write) noexcept {
 for(unsigned packets=0;packets<8;++packets){
  if(lock.test_and_set(std::memory_order_acquire))return;
  if(head==tail){lock.clear(std::memory_order_release);return;}
  const Record r=queue[tail];tail=unsigned((tail+1)%queue.size());lock.clear(std::memory_order_release);
  if(!current(r,now))continue;
  const auto queueDropped=dropped.load(std::memory_order_relaxed);
  char line[2400]{};
  auto local=[&](const Local& v,char* out,std::size_t n){std::snprintf(out,n,"(%.9g,%.9g,%.9g;%.9g,%.9g,%.9g,%.9g)",v.p.x,v.p.y,v.p.z,v.q.x,v.q.y,v.q.z,v.q.w);};
  if(r.kind==Record::Kind::Item){
   char ni[200]{},fi[200]{},nw[200]{},fw[200]{};local(r.nativeItem,ni,sizeof(ni));local(r.finalItem,fi,sizeof(fi));local(r.nativeWrist,nw,sizeof(nw));local(r.finalWrist,fw,sizeof(fw));
   std::snprintf(line,sizeof(line),"HAND_F10 ITEM tick=%llu session=%llu CI=%p epoch=%llu request=%llu sourceSample=%llu sourceTick=%llu side=%d entity=%u keyKnown=%d key=%016llX generationKnown=%d generation=%llu token=%llu attachment=%p entityPtr=%p nativeItemPtr=%p socketJoint=%d sameItemPair=%d placeCallReturned=%d existingReadbackKnown=%d existingReadback=%d nativeWristRelativeWorldM=%s allowedWristRelativeWorldM=%s nativeItemModel=%s submittedItemWorld=%s metric=WRIST_RELATIVE_NOT_PALM_OR_MESH surfaceGap=UNKNOWN GPU=UNOBSERVED",
    (unsigned long long)r.tick,(unsigned long long)r.session,reinterpret_cast<void*>(r.character),(unsigned long long)r.epoch,(unsigned long long)r.request,(unsigned long long)r.sample,(unsigned long long)r.sourceTick,r.side,r.entity,r.keyKnown,(unsigned long long)r.key,r.generationKnown,(unsigned long long)r.generation,(unsigned long long)r.token,reinterpret_cast<void*>(r.attachment),reinterpret_cast<void*>(r.item),reinterpret_cast<void*>(r.nativeItemPointer),r.joint,r.itemPair,r.placed,r.readbackKnown,r.readback,nw,fw,ni,fi);write(line);
   char actual[200]{},relative[200]{};local(r.actualItem,actual,sizeof(actual));local(r.actualWrist,relative,sizeof(relative));
   std::snprintf(line,sizeof(line),"HAND_F10 ITEM_MEASURED queueDroppedTotal=%llu tick=%llu session=%llu CI=%p side=%d entity=%u route=%u cachedWorldMatrixKnown=%d uniformQuatTSKnown=%d measuredWorld=%s measuredWristRelativeWorldM=%s measuredWristRelativeKnown=%d exactBodyPair=%d observation=POST_CALLBACK_PERIODIC100_OR_BOUNDED_EDGE25 atomicRender=UNPROVEN matrix34=(%.9g,%.9g,%.9g,%.9g;%.9g,%.9g,%.9g,%.9g;%.9g,%.9g,%.9g,%.9g)",(unsigned long long)queueDropped,(unsigned long long)r.tick,(unsigned long long)r.session,reinterpret_cast<void*>(r.character),r.side,r.entity,r.itemRoute,r.actualMatrixKnown,r.actualItemKnown,actual,relative,r.actualItemKnown&&r.itemPair,r.itemPair,r.actualMatrix[0],r.actualMatrix[1],r.actualMatrix[2],r.actualMatrix[3],r.actualMatrix[4],r.actualMatrix[5],r.actualMatrix[6],r.actualMatrix[7],r.actualMatrix[8],r.actualMatrix[9],r.actualMatrix[10],r.actualMatrix[11]);write(line);continue;
  }
  std::snprintf(line,sizeof(line),"HAND_F10 POSE queueDroppedTotal=%llu tick=%llu session=%llu CI=%p skeleton=%p pose=%p anim=%p epoch=%llu request=%llu trackingEpoch=%llu sample=%llu sourceTick=%llu timeNs=%llu mappingHash=%016llX reason=%u committed=%d restored=%d published=%d originalReturned=%d finalStage=%s nativeStage=INCOMING_PRE_VR_B1 fullyBlendedIncoming=ENGINE_B1 clipKnown=%d clipTick=%llu clipSeq=%llu clipCount=%u truncated=%d clipAssociation=ASYNCHRONOUS_CACHE_NOT_BLEND_WEIGHT endpointMutation=NONE GPU=UNOBSERVED",
   (unsigned long long)queueDropped,(unsigned long long)r.tick,(unsigned long long)r.session,reinterpret_cast<void*>(r.character),reinterpret_cast<void*>(r.skeleton),reinterpret_cast<void*>(r.pose),reinterpret_cast<void*>(r.anim),(unsigned long long)r.epoch,(unsigned long long)r.request,(unsigned long long)r.trackingEpoch,(unsigned long long)r.sample,(unsigned long long)r.sourceTick,(unsigned long long)r.timeNs,(unsigned long long)r.rig,r.reason,r.committed,r.restored,r.published,r.originalReturned,r.published?"COMMITTED_ALLOWED_LOCALS":r.committed?"REJECTED_PUBLICATION_FINAL_UNKNOWN":"NATIVE_NO_VR_FINAL_UNKNOWN",r.clipKnown,(unsigned long long)r.clipTick,(unsigned long long)r.clipSerial,r.clipCount,r.clipTruncated);write(line);
  for(unsigned c=0;c<r.clipCount&&c<12;++c){const auto& v=r.clips[c];std::snprintf(line,sizeof(line),"HAND_F10 CLIP tick=%llu session=%llu CI=%p clipTick=%llu clipSeq=%llu id=%d layer=%d fifo=%d segment=%d phase=%.6f alias=\"%s\" association=CACHED_NATIVE_FIFO weight=UNKNOWN",(unsigned long long)r.tick,(unsigned long long)r.session,reinterpret_cast<void*>(r.character),(unsigned long long)r.clipTick,(unsigned long long)r.clipSerial,v.id,v.layer,v.fifo,v.segment,double(v.phase),v.name);write(line);}
  for(int side=0;side<2;++side){const auto& h=r.hand[side];
   std::snprintf(line,sizeof(line),"HAND_F10 HAND tick=%llu session=%llu side=%d wrist=%d mapped=%d count=%d heldKnown=%d heldKey=%016llX objectKey=%016llX generation=%llu grip=%.6g trigger=%.6g referenceOwner=%d openReady=%d closedReady=%d nativeTrialReady=%d openSource=%llu closedSource=%llu trialSource=%llu cacheOnly=%d curveIds=%d/%d/%d curveFlags=%X/%X/%X selectedPhase=%d trialRequested=%d actualFreeWriter=%d trialActuallyActive=%d fallback=%s objectWriter=%d objectCacheReady=%d objectFollowNativeAction=%d objectSince=%llu objectLastSample=%llu objectSequence=%llu actualSource=%s nativeWholeHandScore=%.9g observedWholeHandPeakCandidate=%d peakMetric=MEAN_NATIVE_TO_OPEN_ANGLE extremaGuaranteed=NO",(unsigned long long)r.tick,(unsigned long long)r.session,side,h.wrist,h.map,h.count,h.heldKnown,(unsigned long long)h.heldKey,(unsigned long long)h.objectKey,(unsigned long long)h.generation,h.grip,h.trigger,h.referenceOwner,h.open,h.closed,h.trialReady,(unsigned long long)h.openSequence,(unsigned long long)h.closedSequence,(unsigned long long)h.trialSequence,h.cacheOnly,h.curveId[0],h.curveId[1],h.curveId[2],h.curveFlags[0],h.curveFlags[1],h.curveFlags[2],h.selectedPhase,h.trialRequested,h.writer,h.trialActive,!h.referenceOwner?"REFERENCE_OWNER_UNKNOWN":!h.writer?"NATIVE_OBJECT_OR_UNCOMMITTED":h.trialRequested&&!h.trialReady?"NATIVE_TRIAL_UNAVAILABLE_DEFAULT":!h.open||!h.closed?"DEFAULT_REFERENCE_UNAVAILABLE_NATIVE":"NONE",h.objectWriter,h.objectReady,h.objectFollow,(unsigned long long)h.objectSince,(unsigned long long)h.objectLastSample,(unsigned long long)h.objectSequence,h.objectWriter?(h.objectReady?"CACHED_OBJECT_LOCALS":"CURRENT_EVALUATED_NATIVE_OBJECT"):h.writer?(h.trialActive?"NATIVE_CURVE_TRIAL":"DEFAULT_FREE_CURVE"):"NATIVE_OR_OTHER_OWNER",h.nativeClosureScore,h.peakCandidate);write(line);
   char nw[200]{},fw[200]{};local(h.nativeWrist,nw,sizeof(nw));local(h.finalWrist,fw,sizeof(fw));
   std::snprintf(line,sizeof(line),"HAND_F10 WRIST tick=%llu side=%d id=%d nativeModelKnown=%d finalModelKnown=%d nativeModel=%s committedModel=%s",(unsigned long long)r.tick,side,h.wrist,h.map,r.published,nw,fw);write(line);
   for(int f=0;f<5;++f){std::snprintf(line,sizeof(line),"HAND_F10 CURL tick=%llu side=%d finger=%d input=%.9g applied=%.9g appliedKnown=%d contactClamp=%d",(unsigned long long)r.tick,side,f,h.inputCurl[f],h.appliedCurl[f],h.writer&&r.published,h.contact[f]);write(line);}
   for(int j=0;j<h.count&&j<32;++j){if(!current(r,now))return;const auto& v=h.joints[j];auto& memo=referenceMemoD53[side][j];
    if(referenceDueD53(memo,r,h,v)){
     char c[200]{},d[200]{},e[200]{},f[200]{},g[200]{};local(v.open,c,sizeof(c));local(v.closed,d,sizeof(d));local(v.trial,e,sizeof(e));local(v.curveOpen,f,sizeof(f));local(v.curveClosed,g,sizeof(g));
     std::snprintf(line,sizeof(line),"HAND_F10 REFERENCE_D53 referenceId=%llu tick=%llu session=%llu CI=%p mappingHash=%016llX epoch=%llu request=%llu generation=%llu side=%d ordinal=%d id=%d parent=%d finger=%d segment=%d crc=%08X openSource=%llu closedSource=%llu trialSource=%llu rawOpen=%s defaultClosed=%s nativeTrialClosed=%s selectedFreeCurveOpen=%s selectedFreeCurveClosed=%s coordinates=PARENT_LOCAL reuse=EXACT_BITS_AND_IDENTITY_UNTIL_NEXT_REFERENCE",
      (unsigned long long)memo.id,(unsigned long long)r.tick,(unsigned long long)r.session,reinterpret_cast<void*>(r.character),(unsigned long long)r.rig,(unsigned long long)r.epoch,(unsigned long long)r.request,(unsigned long long)h.generation,side,j,v.id,v.parent,v.digit,v.segment,v.crc,(unsigned long long)h.openSequence,(unsigned long long)h.closedSequence,(unsigned long long)h.trialSequence,c,d,e,f,g);write(line);
    }
    char a[200]{},b[200]{};local(v.native,a,sizeof(a));local(v.final,b,sizeof(b));
    std::snprintf(line,sizeof(line),"HAND_F10 JOINT tick=%llu session=%llu side=%d ordinal=%d id=%d parent=%d finger=%d segment=%d crc=%08X nativeKnown=%d finalKnown=%d nativeLocal=%s committedLocal=%s referenceId=%llu coordinates=PARENT_LOCAL endpointAuthority=UNCHANGED",(unsigned long long)r.tick,(unsigned long long)r.session,side,j,v.id,v.parent,v.digit,v.segment,v.crc,v.nativeKnown,v.finalKnown,a,b,(unsigned long long)memo.id);write(line);
   }
  }
 }
}
}
