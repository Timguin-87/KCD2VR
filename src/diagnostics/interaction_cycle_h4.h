#pragma once
// F5 copied evidence only. No native reads, hooks, callbacks, gameplay writes,
// allocation, formatting or I/O in producers. IDs/pointers are opaque values.
#include "capture168.h"
#include <array>
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cmath>
namespace kcd2vr::diagnostics::interaction_cycle_h4 {
enum class Stage:unsigned {Grip,Transport,Selector,Order,ActionRequest,ActionReturn,DoorAdmission,DoorProgress,DoorAck,Clip,Lifecycle,Motion};
inline const char* stageName(Stage s) noexcept {constexpr const char* n[]{"GRIP_ADMISSION","USE_TRANSPORT","SELECTOR_CANDIDATE","SORTED_ORDER","NATIVE_ACTION_REQUEST","NATIVE_ACTION_RETURN","DOOR_ADMISSION","DOOR_PROGRESS","DOOR_LUA_ACK","NATIVE_CLIP_START","ITEM_LIFECYCLE","DOOR_MOTION_ENDPOINT"};return n[unsigned(s)];}
inline void copy(char* dst,unsigned size,const char* src) noexcept {if(!size)return;unsigned i=0;if(src)for(;i+1<size&&src[i];++i)dst[i]=src[i];dst[i]=0;}
struct Event {
 Stage stage{};std::uint64_t session=0,tick=0,sourceTick=0,poseEpoch=0,trackingEpoch=0,generation=0,settings=0,press=0,otherPress=0,physicalUse=0,token=0,doorSerial=0,actionSeq=0,collector=0,call=0,key=0;
 std::uintptr_t player=0,actor=0,entity=0,action=0;std::uint32_t target=0,otherTarget=0;
 int side=-1,code=-1,detail=-1;unsigned flags=0;float value=0,value2=0,point[3]{};char reason[96]{},label[80]{};
};
struct ProducerExtra {bool contextKnown=false,enabled=false,menu=false,tracked=false,empty=false,rayPublished=false,sentPulse=false,held=false;unsigned reserved=0,gripBlocked=0,mode=0;float grip=0,trigger=0;};
class Recorder {
 std::atomic_flag lock=ATOMIC_FLAG_INIT;std::array<Event,256> queue{};unsigned head=0,tail=0;
 std::array<Event,6> previous{};std::array<Event,32> progressPrevious{};std::array<Event,64> detailPrevious{}; // input/transport, two actual sides; side=-1 has no stale hand identity
 std::uint64_t budgetSession=0,budget=0,edgeBudget=0;
public:
 std::atomic<std::uint64_t> pushed{0},dropped{0},contended{0},coalesced{0},budgetLost{0},off{0},badClock{0};
 std::atomic<std::uint64_t> collectorDetailBlocked{0},collectorLineBlocked{0},collectorCandidateTruncated{0},collectorOrderUnread{0};
 std::array<std::atomic<std::uint64_t>,12> calls{};
 bool offer(Event e,std::uint64_t now) noexcept {
  if(!capture168::active(capture168::Domain::Interaction,now)){++off;return false;}
  if(!e.tick||e.tick>now){++badClock;return false;}
  const auto session=capture168::gate.session[unsigned(capture168::Domain::Interaction)].load(std::memory_order_acquire);
  if(unsigned(e.stage)>=calls.size()){++badClock;return false;}++calls[unsigned(e.stage)];
  if(!session||(e.session&&e.session!=session)){++off;return false;}e.session=session;
  if(lock.test_and_set(std::memory_order_acquire)){++contended;return false;}
  if(budgetSession!=session){budgetSession=session;budget=0;edgeBudget=0;previous={};progressPrevious={};detailPrevious={};}
  bool ok=false;
  if((e.stage==Stage::Grip||e.stage==Stage::Transport)){
   auto& p=previous[unsigned(e.stage==Stage::Transport)*3+unsigned(e.side>=0&&e.side<2?e.side:2)];
   const bool same=p.session==session&&p.stage==e.stage&&p.press==e.press&&p.physicalUse==e.physicalUse&&p.token==e.token&&p.target==e.target&&p.otherTarget==e.otherTarget&&p.poseEpoch==e.poseEpoch&&p.trackingEpoch==e.trackingEpoch&&p.generation==e.generation&&p.flags==e.flags&&p.code==e.code&&p.detail==e.detail&&!std::strcmp(p.reason,e.reason);
   if(same&&now>=p.tick&&now-p.tick<250){++coalesced;lock.clear(std::memory_order_release);return false;}p=e;
  }
  const bool frequent=e.stage==Stage::DoorAck||e.stage==Stage::Motion||(e.stage==Stage::DoorProgress&&!std::strncmp(e.reason,"RESIDENT_CURVE_",15));
  if(frequent&&e.doorSerial){unsigned kind=e.stage==Stage::DoorAck?0:e.stage==Stage::DoorProgress?1:!std::strcmp(e.reason,"ENTITY_TRANSFORM")?3:2;auto& p=progressPrevious[(e.doorSerial%8)*4+kind];
   const bool same=p.session==session&&p.stage==e.stage&&p.doorSerial==e.doorSerial&&p.target==e.target&&p.player==e.player&&p.poseEpoch==e.poseEpoch&&p.press==e.press&&p.code==e.code&&p.detail==e.detail&&!std::strcmp(p.reason,e.reason)&&std::isfinite(p.value)&&std::isfinite(e.value)&&std::fabs(p.value-e.value)<.005f;
   if(same&&now>=p.tick&&now-p.tick<250){++coalesced;lock.clear(std::memory_order_release);return false;}p=e;
  }
  const bool lifeDetail=e.stage==Stage::Lifecycle&&(!std::strcmp(e.reason,"HAND_SAMPLE")||!std::strcmp(e.reason,"GRIP_SAMPLE")||!std::strcmp(e.reason,"BODY_SAMPLE")||!std::strcmp(e.reason,"OBJECT_POSE")||!std::strcmp(e.reason,"CARRY_SNAPSHOT")||!std::strcmp(e.label,"D53_CAMERA_CORE_NOT_FINAL_RENDER"));
  const bool detail=e.stage==Stage::Selector||e.stage==Stage::Order||e.stage==Stage::Clip||lifeDetail;
  if(detail){auto& p=detailPrevious[(e.target*17u+unsigned(e.stage)*7u+unsigned(e.detail+1))%detailPrevious.size()];
   const bool sameCameraOwner=std::strcmp(e.label,"D53_CAMERA_CORE_NOT_FINAL_RENDER")||
       (p.actor==e.actor&&p.poseEpoch==e.poseEpoch&&p.action==e.action&&p.token==e.token);
   const bool same=sameCameraOwner&&p.session==session&&p.stage==e.stage&&p.target==e.target&&p.entity==e.entity&&p.side==e.side&&p.actionSeq==e.actionSeq&&p.flags==e.flags&&p.code==e.code&&p.detail==e.detail&&p.press==e.press&&p.otherPress==e.otherPress&&!std::strcmp(p.reason,e.reason)&&!std::strcmp(p.label,e.label)&&std::isfinite(e.value)&&std::isfinite(p.value)&&std::fabs(e.value-p.value)<.01f&&std::fabs(e.value2-p.value2)<.01f&&std::fabs(e.point[0]-p.point[0])<.01f&&std::fabs(e.point[1]-p.point[1])<.01f&&std::fabs(e.point[2]-p.point[2])<.01f;
   if(same&&now>=p.tick&&now-p.tick<250){++coalesced;lock.clear(std::memory_order_release);return false;}p=e;
  }
  const bool edge=e.stage==Stage::ActionRequest||e.stage==Stage::ActionReturn||e.stage==Stage::DoorAdmission||(e.stage==Stage::DoorProgress&&!frequent)||(e.stage==Stage::Lifecycle&&!lifeDetail)||(e.stage==Stage::Transport&&(e.flags&256u));
  const unsigned used=unsigned((head+queue.size()-tail)%queue.size());
  if((edge?edgeBudget>=4096:budget>=4096)){++budgetLost;}
  else if(!edge&&used>=192){++dropped;}
  else {const auto next=(head+1)%queue.size();if(next==tail)++dropped;else{queue[head]=e;head=unsigned(next);if(edge)++edgeBudget;else ++budget;++pushed;ok=true;}}
  lock.clear(std::memory_order_release);return ok;
 }
 bool pop(Event& e) noexcept {if(lock.test_and_set(std::memory_order_acquire))return false;const bool ok=head!=tail;if(ok){e=queue[tail];tail=unsigned((tail+1)%queue.size());}lock.clear(std::memory_order_release);return ok;}
};
inline Recorder recorder;
inline bool observe(Event e,std::uint64_t now) noexcept {return recorder.offer(e,now);}
template<class Input,class Selection,class Plan,class Controller>
inline void observeProducer(const Input& i,const Selection& s,const Plan& p,const Controller& c,const ProducerExtra& x,bool transported) noexcept {
 Event e{};e.stage=transported?Stage::Transport:Stage::Grip;e.tick=i.now;e.sourceTick=i.poseTick;e.poseEpoch=i.poseEpoch;e.trackingEpoch=i.trackingEpoch;e.generation=i.sourceGeneration;e.settings=c.chosen.settingsEpoch;e.press=i.press;e.side=i.side;e.player=i.player;e.target=p.target?p.target:s.target;e.otherTarget=c.chosen.target;
 e.physicalUse=c.transportedSerial62;e.token=c.chosen.token;e.value=x.grip;e.value2=x.trigger;e.detail=int(x.mode);
 e.flags=(i.available?1u:0u)|(i.held?2u:0u)|(i.scoped54?4u:0u)|(i.waitingForRay49?8u:0u)|(i.waitingForPoint65?16u:0u)|(s.ordinary?32u:0u)|(p.owns?64u:0u)|(p.pulse?128u:0u)|(x.sentPulse?256u:0u)|(x.held?512u:0u)|(c.transportRetired64?1024u:0u)|(c.pointGapPendingH31?2048u:0u)|(c.cancelled?4096u:0u)|(c.transportedPress62==i.press&&i.press?8192u:0u)|(x.contextKnown?16384u:0u);
 const char* why="NATIVE_SELECTION_OR_ACTION_STILL_REQUIRED";
 if(x.contextKnown&&!x.enabled)why="INTERACTION_BACKEND_DISABLED";
 else if(x.contextKnown&&x.menu)why="MENU_OR_NATIVE_UI_OWNS_INPUT";
 else if(i.side<0||i.side>1)why="NO_SELECTED_PHYSICAL_HAND";
 else if(x.contextKnown&&(x.reserved&(1u<<i.side)))why="HAND_RESERVED_BY_CURRENT_OWNER";
 else if(x.contextKnown&&!x.tracked)why="CURRENT_HAND_TRACKING_UNAVAILABLE";
 else if(x.contextKnown&&!x.empty)why="HAND_OCCUPIED_OR_UNCLASSIFIED";
 else if(x.contextKnown&&(x.gripBlocked&(i.side?1u:2u)))why="UI_GRIP_BLOCKED";
 else if(!i.held||!i.press)why="NO_HELD_ORIGINAL_GRIP_PRESS";
 else if(!i.scoped54)why="INPUT_MODE_OR_CONTEXT_SCOPE_REFUSED";
 else if(i.waitingForPoint65)why="WAIT_CURRENT_PHYSICAL_POINT";
 else if(i.waitingForRay49)why="WAIT_CURRENT_HAND_RAY";
 else if(!i.available)why="CURRENT_POINT_OR_SOURCE_INPUT_UNAVAILABLE";
 else if(c.cancelled||c.transportRetired64)why="ORIGINAL_PRESS_RETIRED_OR_CANCELLED";
 else if(transported)why=x.sentPulse?"USE_PULSE_SENT_NOT_NATIVE_ACCEPTANCE":x.held?"USE_HELD_TRANSPORT_NOT_NATIVE_ACCEPTANCE":"NO_USE_TRANSPORT_THIS_FRAME";
 else if(p.pulse)why="EXACT_DOOR_USE_PLANNED_NOT_SENT";
 else if(c.pointGapPendingH31)why="EXISTING_POINT_GAP_QUARANTINE_NO_NEW_USE";
 else if(p.owns)why="DOOR_PLAN_OWNS_NO_NEW_PULSE";
 copy(e.reason,sizeof(e.reason),why);copy(e.label,sizeof(e.label),s.ordinary?"AnimDoor":"OTHER_OR_UNSELECTED_NATIVE_CANDIDATE");observe(e,i.now);
}
template<class Request>
inline Event doorEvent(const Request& r,Stage stage,std::uint64_t now,const char* why,float value,bool accepted) noexcept {
 Event e{};e.stage=stage;e.tick=now;e.sourceTick=r.tick;e.poseEpoch=r.epoch;e.player=r.player;e.entity=r.entity;e.action=r.action;e.target=r.id;e.side=r.side;e.generation=r.sourceGeneration;e.trackingEpoch=r.trackingEpoch46;e.settings=r.inputSettingsEpoch46;e.press=r.inputPress65;e.token=r.inputToken46;e.doorSerial=r.serial;e.detail=r.mode69;e.value=value;e.code=accepted?1:0;copy(e.reason,sizeof(e.reason),why);return e;
}
struct Link {std::uint64_t session=0,serial=0,epoch=0,action=0,tick=0;std::uintptr_t player=0;std::uint32_t target=0;int side=-1;};
struct Cycle {std::uint64_t session=0,action=0,tick=0,door=0;std::uint32_t target=0;int side=-1;bool returned=false,accepted=false,terminal=false;};
struct Consumer {
 std::uint64_t session=0,drained=0,expired=0,formatterClipped=0,linksLost=0,cyclesLost=0,ended=0;std::array<Link,32> links{};std::array<Cycle,64> cycles{};unsigned cursor=0;
};
inline Consumer consumer;
template<class Sink> inline void unresolved(Consumer& c,std::uint64_t now,const char* why,Sink sink,std::uint64_t onlySession=0) noexcept {
 for(auto& p:c.cycles)if(p.session&&!p.terminal&&(!onlySession||p.session==onlySession)){if(!std::strcmp(why,"TIMEOUT_OBSERVATION_WINDOW")&&(now<p.tick||now-p.tick<=15000))continue;
  const char* family=!std::strcmp(why,"RECORDER_ENDED_BEFORE_COMPLETION")?"H29R4 F5_END detail=PENDING":"H29R4 F5_UNRESOLVED";
  char line[520]{};std::snprintf(line,sizeof(line),"%s session=%llu actionSeq=%llu door=%llu target=%u side=%d elapsedMs=%llu reason=%s returned=%d acceptedObject=%d completion=NOT_OBSERVED notProofOfNativeFailure=1",
   family,(unsigned long long)p.session,(unsigned long long)p.action,(unsigned long long)p.door,p.target,p.side,(unsigned long long)(now>=p.tick?now-p.tick:0),why,p.returned,p.accepted);sink(line);p.terminal=true;
 }
}
template<class Sink> inline void service(std::uint64_t now,Sink sink) noexcept {
 auto& c=consumer;const bool active=capture168::active(capture168::Domain::Interaction,now);const auto current=capture168::gate.session[unsigned(capture168::Domain::Interaction)].load();
 Event e{};unsigned count=0;while(count++<256&&recorder.pop(e)){
  if(!c.session&&!active)c.session=e.session;
  if(now<e.tick||now-e.tick>3000){++c.expired;continue;}
  if(e.stage==Stage::DoorAdmission&&e.doorSerial&&e.actionSeq){auto& l=c.links[c.cursor++%c.links.size()];if(l.serial)++c.linksLost;l={e.session,e.doorSerial,e.poseEpoch,e.actionSeq,e.tick,e.player,e.target,e.side};}
  if(e.doorSerial&&!e.actionSeq)for(const auto& l:c.links)if(l.session==e.session&&l.serial==e.doorSerial&&l.epoch==e.poseEpoch&&l.player==e.player&&l.target==e.target&&l.side==e.side&&now>=l.tick&&now-l.tick<=15000){e.actionSeq=l.action;break;}
  Cycle* cycle=nullptr;if(e.actionSeq){for(auto& p:c.cycles)if(p.session==e.session&&p.action==e.actionSeq){cycle=&p;break;}
   if(!cycle&&e.stage==Stage::ActionRequest){for(auto& p:c.cycles)if(!p.session||p.terminal){p={e.session,e.actionSeq,e.tick,0,e.target,e.side};cycle=&p;break;}if(!cycle)++c.cyclesLost;}
  }
  if(cycle&&cycle->target==e.target){if(e.doorSerial)cycle->door=e.doorSerial;
   if(e.stage==Stage::ActionReturn){cycle->returned=true;cycle->accepted=e.action!=0;if(!e.action)cycle->terminal=true;}
   if(e.stage==Stage::DoorProgress&&e.code==1&&!std::strcmp(e.reason,"NATIVE_OPERATION_COMPLETED"))cycle->terminal=true;
  }
  const char* rowFamily=!active||e.session!=current?"H29R4 F5_END detail=QUEUED":"H29R4 F5_CYCLE";
  char line[1600]{};const auto written=std::snprintf(line,sizeof(line),"%s session=%llu stage=%s tick=%llu sourceTick=%llu sourceAgeMs=%llu press=%llu otherPress=%llu physicalUse=%llu token=%llu actionSeq=%llu door=%llu collector=%llu target=%u otherTarget=%u side=%d player=%p actor=%p entity=%p action=%p epoch=%llu trackingEpoch=%llu generation=%llu settings=%llu call=%llu key=%llX flags=0x%X code=%d detail=%d value=%.6g value2=%.6g point=(%.6g,%.6g,%.6g) reason=%s label=%s nativeCompletion=ONLY_EXACT_NATIVE_OPERATION_COMPLETED inventoryDelta=NOT_OBSERVED noGuessedJoin=1 writes=NONE",
   rowFamily,(unsigned long long)e.session,stageName(e.stage),(unsigned long long)e.tick,(unsigned long long)e.sourceTick,(unsigned long long)(e.sourceTick&&e.tick>=e.sourceTick?e.tick-e.sourceTick:~std::uint64_t(0)),(unsigned long long)e.press,(unsigned long long)e.otherPress,(unsigned long long)e.physicalUse,(unsigned long long)e.token,(unsigned long long)e.actionSeq,(unsigned long long)e.doorSerial,(unsigned long long)e.collector,e.target,e.otherTarget,e.side,(void*)e.player,(void*)e.actor,(void*)e.entity,(void*)e.action,(unsigned long long)e.poseEpoch,(unsigned long long)e.trackingEpoch,(unsigned long long)e.generation,(unsigned long long)e.settings,(unsigned long long)e.call,(unsigned long long)e.key,e.flags,e.code,e.detail,double(e.value),double(e.value2),double(e.point[0]),double(e.point[1]),double(e.point[2]),e.reason,e.label);if(written<0||std::size_t(written)>=sizeof(line))++c.formatterClipped;sink(line);++c.drained;
 }
 unresolved(c,now,"TIMEOUT_OBSERVATION_WINDOW",sink);
 if(c.session&&c.ended!=c.session&&(!active||current!=c.session)){unresolved(c,now,"RECORDER_ENDED_BEFORE_COMPLETION",sink,c.session);
  char line[1024]{};std::snprintf(line,sizeof(line),"H29R4 F5_END session=%llu drainedProcess=%llu enqueuedProcess=%llu droppedQueueProcess=%llu contendedProcess=%llu coalescedProcess=%llu budgetLostProcess=%llu expiredProcess=%llu formatterClippedProcess=%llu badClockProcess=%llu linksEvictedProcess=%llu cyclesLostProcess=%llu nativeWrites=NONE pendingDoesNotMeanNativeFailure=1",
   (unsigned long long)c.session,(unsigned long long)c.drained,(unsigned long long)recorder.pushed.load(),(unsigned long long)recorder.dropped.load(),(unsigned long long)recorder.contended.load(),(unsigned long long)recorder.coalesced.load(),(unsigned long long)recorder.budgetLost.load(),(unsigned long long)c.expired,(unsigned long long)c.formatterClipped,(unsigned long long)recorder.badClock.load(),(unsigned long long)c.linksLost,(unsigned long long)c.cyclesLost);sink(line);char counts[1024]{};std::snprintf(counts,sizeof(counts),"H29R4 F5_END session=%llu detail=CALL_COUNTS counters=PROCESS_BEFORE_COALESCING grip=%llu transport=%llu selector=%llu order=%llu actionRequest=%llu actionReturn=%llu doorAdmission=%llu progress=%llu ack=%llu clip=%llu lifecycle=%llu motion=%llu noHumanAttemptDenominator=1",(unsigned long long)c.session,(unsigned long long)recorder.calls[0].load(),(unsigned long long)recorder.calls[1].load(),(unsigned long long)recorder.calls[2].load(),(unsigned long long)recorder.calls[3].load(),(unsigned long long)recorder.calls[4].load(),(unsigned long long)recorder.calls[5].load(),(unsigned long long)recorder.calls[6].load(),(unsigned long long)recorder.calls[7].load(),(unsigned long long)recorder.calls[8].load(),(unsigned long long)recorder.calls[9].load(),(unsigned long long)recorder.calls[10].load(),(unsigned long long)recorder.calls[11].load());sink(counts);char limits[768]{};std::snprintf(limits,sizeof(limits),"H29R4 F5_END session=%llu detail=COLLECTOR_LIMITS counters=PROCESS detailBlocked=%llu lineBlocked=%llu candidateTruncated=%llu orderUnread=%llu oldLimits=512_DETAILED_PASSES_8192_LINES noExtraNativeReads=1 unavailableCandidates=UNKNOWN",(unsigned long long)c.session,(unsigned long long)recorder.collectorDetailBlocked.load(),(unsigned long long)recorder.collectorLineBlocked.load(),(unsigned long long)recorder.collectorCandidateTruncated.load(),(unsigned long long)recorder.collectorOrderUnread.load());sink(limits);c.ended=c.session;
 }
 if(active&&current!=c.session){c.session=current;char line[330]{};std::snprintf(line,sizeof(line),"H29R4 F5_BEGIN session=%llu capture=EXISTING_F5 selector=EXISTING_EXACT_9_SEAMS action=702288 clip=EXISTING_ORIGINAL_START doorPanel=EXISTING_LUA_ACK_AND_RESIDENT_CURVE completion=UNKNOWN_UNTIL_NATIVE_FINISH noNewHooks=1",(unsigned long long)current);sink(line);}
}
}
