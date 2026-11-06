#pragma once
// F5-only copied-state receipts. No native reads, writes, time queries or
// allocations. Capturing an input request never certifies a native result.
#include "input67.h"
#include "native_return67.h"
#include <array>
#include <cstdio>

namespace kcd2vr::subsystems::alchemy::capture_d42 {
struct Key {
 std::array<std::uint64_t,54> value{};
 bool operator==(const Key& b)const noexcept{return value==b.value;}
};
struct Gate {
 std::uint64_t session=0,first=0,last=0,seen=0,changes=0,emitted=0,throttled=0,dropped=0;
 unsigned limit=4096;bool have=false,exhausted=false;Key previous{};
 // Bit 1: emit state; bit 2: first budget-exhaustion notice. Transitions have
 // NO minimum spacing: a short valid grip/Attach/release must survive capture.
 unsigned observe(std::uint64_t s,std::uint64_t now,const Key& key) noexcept {
  if(!s||!now)return 0;
  if(session!=s){const auto cap=limit;*this={};limit=cap;session=s;first=now;}
  const bool change=!have||!(previous==key);have=true;previous=key;++seen;if(change)++changes;
  const bool due=change||!last||now<last||now-last>=1000;
  if(!due){++throttled;return 0;}
  if(emitted>=limit){++dropped;if(!exhausted){exhausted=true;return 2;}return 0;}
  last=now;++emitted;return 1;
 }
 bool close(std::uint64_t now,const char* kind,const char* reason,char* line,std::size_t size) noexcept {
  if(!session)return false;
  std::snprintf(line,size,"D42_F5_END kind=%s session=%llu reason=%s firstTick=%llu lastStateTick=%llu endTick=%llu observed=%llu changed=%llu emitted=%llu throttled=%llu dropped=%llu budget=%u exhausted=%d",
   kind,(unsigned long long)session,reason,(unsigned long long)first,(unsigned long long)last,(unsigned long long)now,
   (unsigned long long)seen,(unsigned long long)changes,(unsigned long long)emitted,(unsigned long long)throttled,(unsigned long long)dropped,limit,exhausted);
  const auto cap=limit;*this={};limit=cap;return true;
 }
};
inline std::uint64_t age(std::uint64_t now,std::uint64_t tick) noexcept{return tick&&now>=tick?now-tick:UINT64_MAX;}
inline Key key(const input67::Input& in,const input67::Output& out,const input67::State& s,const workstation_sample46::Frame& f,const return67::Result& ret={},unsigned selector=0) noexcept {
 Key k{};unsigned i=0;const auto add=[&](std::uint64_t v){k.value[i++]=v;};
 add(in.context.instance);add(in.context.lifetime);add(in.modeEpoch);add(f.poseEpoch);add(f.sourceEpoch);
 add(s.admittedTick);add(s.admittedRef.entity);add(unsigned(s.admittedRef.slot));add(unsigned(s.admittedSide));
 add(unsigned(out.gesture.status));add(in.action.parent);add(in.work.parent);add(in.work.child);add(in.work.token);add(unsigned(in.work.phase));
 add(out.nomination.decision.ref.entity);add(unsigned(out.nomination.decision.ref.slot));add(unsigned(out.nomination.decision.source));
 add(in.nativeSelected.entity);add(in.nativeSelected.type);add(in.nativeSelectedTypeOnly);
 add((in.enabled?1:0)|(in.qualified?2:0)|(in.cancel?4:0)|(in.nativeA?8:0)|(f.allowed?16:0)|(f.headValid?32:0)|
     (in.nativeSelectedKnown?64:0)|(in.action.valid?128:0)|(in.work.working?256:0)|(in.work.ready?512:0));
 add((out.admitted?1:0)|(out.use?2:0)|(out.passenger?4:0)|(out.control.held[0]?8:0)|(out.control.held[1]?16:0)|
     (out.ordinaryHeld?32:0)|(out.ordinaryReturnRequested?64:0)|(out.ordinaryReturnObserved?128:0)|(out.gesture.releaseRequested?256:0)|
     (s.releasedAdmission?512:0)|(s.ordinaryReleasePending?1024:0)|(s.mortarStopping69?2048:0)|(s.mortarReturning69?4096:0)|(out.nomination.decision.selectedPickupD44?8192:0));
 for(unsigned h=0;h<2;++h){const auto& t=in.tool[h];add(t.entity);add(t.binding.object);add(t.binding.attachment);add(t.binding.action);
  add((t.joined?1:0)|(f.hand[h].valid?2:0)|(f.hand[h].grip>=.55f?4:0)|(f.hand[h].grip<.35f?8:0)|
      (near67::fresh(t.tick,in.now)?16:0)|(s.selection.quarantined[h]?32:0));}
 add(in.mortarResult69.serial);add(unsigned(in.mortarResult69.status));add(unsigned(in.mortarResult69.stopEntry72));
 add(s.mortarSerial69);add(s.ordinaryReturnParent);add(in.nativeReturnAuthorized);add(out.gesture.clockStop);
 add(ret.serial);add(unsigned(ret.status));add((ret.cancelReturned?1:0)|(ret.detachReturned?2:0)|(ret.homeReturned?4:0));
 add(selector);add(out.nomination.decision.nativeAgreeW4);add(in.nativeSelected.hold);add(out.mortarPlaced69);
 add(in.gestureTransferD59);add(out.transferStateD59);add(s.ordinaryIntentTickD59);
 add((s.ordinaryTransferD59?1:0)|(s.ordinaryTransferTiltD59?2:0));
 add(s.unacquiredReleaseTickD61);add(out.unacquiredRetiredD61);
 return k;
}
struct Alchemy {
 Gate gate{};
 template<class Writer> void stop(std::uint64_t now,const char* reason,Writer&& write) noexcept {
  char line[512]{};if(gate.close(now,"ALCHEMY",reason,line,sizeof(line)))write(line);
 }
 template<class Writer> void record(std::uint64_t session,const input67::Input& in,const input67::Output& out,
     const input67::State& state,const workstation_sample46::Frame& frame,const return67::Result& ret,Writer&& write,unsigned selector=0) noexcept {
  if(!session||!in.now)return;
  if(gate.session&&gate.session!=session)stop(in.now,"SESSION_CHANGED",write);
  const unsigned event=gate.observe(session,in.now,key(in,out,state,frame,ret,selector));if(!event)return;
  char line[2400]{};
  if(event==2){std::snprintf(line,sizeof(line),"D42_F5 kind=ALCHEMY event=BUDGET_EXHAUSTED session=%llu tick=%llu emitted=%llu budget=%u furtherStates=COUNTED_NOT_RECORDED",
   (unsigned long long)session,(unsigned long long)in.now,(unsigned long long)gate.emitted,gate.limit);write(line);return;}
  const auto& g=out.gesture;const auto& t=in.nativeFrame.table;unsigned bounds=0;
  for(const auto& offer:in.nativeFrame.offers)if(offer.valid&&offer.boundsValid&&near67::fresh(offer.tick,in.now))++bounds;
  std::snprintf(line,sizeof(line),"D42_F5 kind=ALCHEMY event=STAGE session=%llu serial=%llu tick=%llu instance=%llX life=%llu sample=%llu pose=%llu sourceEpoch=%llu modeEpoch=%llu request=%llu admitted=%u/%d side=%d status=%s qualified=%d enabled=%d inputUse=%d manualA=%d cancel=%d passenger=%d selected=%u/%u known=%d selectedTypeOnly=%u action=%llX actionKnown=%d work=%llX/%llX/%u phase=%d running=%d ready=%d credit=%.6f clockStop=%d release=%d ordinaryPending=%d ordinaryReturning=%d toolR=%u/%d/%d objectR=%llX toolL=%u/%d/%d objectL=%llX nativeReturnAuthorized=%d mortarRequest=%llu mortarResult=%llu/%u stopEntry=%u returnResult=%llu/%u cancelReturned=%d detachReturned=%d homeReturned=%d tableAge=%llu actionAge=%llu workAge=%llu selectorWhy=%u nativeAgree=%d nativeSelectedAssist=%d nativeHoldHint=%d bounds=%u/21 mortarPlaced=%d controlHeld=%d,%d admittedActive=%d transferExperimentD59=%d transferStateD59=%u transferIntentTickD59=%llu transferDestinationD59=%u/%d unacquiredReleaseTickD61=%llu unacquiredRetiredD61=%d nativeHandsKnown=%d nativeHands=%u,%u tableActionKnown=%d tableAction=%llX nativeCompletion=NOT_INFERRED",
   (unsigned long long)session,(unsigned long long)gate.emitted,(unsigned long long)in.now,(unsigned long long)in.context.instance,
   (unsigned long long)in.context.lifetime,(unsigned long long)frame.sequence,(unsigned long long)frame.poseEpoch,(unsigned long long)frame.sourceEpoch,(unsigned long long)in.modeEpoch,
   (unsigned long long)state.admittedTick,state.admittedRef.entity,state.admittedRef.slot,state.admittedSide,gesture67::name(g.status),in.qualified,in.enabled,out.use,in.nativeA,in.cancel,out.passenger,
   in.nativeSelected.entity,in.nativeSelected.type,in.nativeSelectedKnown,in.nativeSelectedTypeOnly,(unsigned long long)in.action.parent,in.action.valid,
   (unsigned long long)in.work.parent,(unsigned long long)in.work.child,in.work.token,in.work.phase,in.work.working,in.work.ready,g.totalSeconds,g.clockStop,g.releaseRequested,
   state.ordinaryReleasePending,state.ordinaryReturning,in.tool[0].entity,in.tool[0].type,in.tool[0].joined,(unsigned long long)in.tool[0].binding.object,
   in.tool[1].entity,in.tool[1].type,in.tool[1].joined,(unsigned long long)in.tool[1].binding.object,in.nativeReturnAuthorized,(unsigned long long)state.mortarSerial69,
   (unsigned long long)in.mortarResult69.serial,unsigned(in.mortarResult69.status),unsigned(in.mortarResult69.stopEntry72),
   (unsigned long long)ret.serial,unsigned(ret.status),ret.cancelReturned,ret.detachReturned,ret.homeReturned,
   (unsigned long long)age(in.now,t.tick),(unsigned long long)age(in.now,in.action.tick),(unsigned long long)age(in.now,in.work.tick),selector,out.nomination.decision.nativeAgreeW4,out.nomination.decision.selectedPickupD44,in.nativeSelected.hold,bounds,out.mortarPlaced69,out.control.held[0],out.control.held[1],out.admitted,in.gestureTransferD59,out.transferStateD59,
   (unsigned long long)state.ordinaryIntentTickD59,state.ordinaryDestination.entity,state.ordinaryDestination.slot,(unsigned long long)state.unacquiredReleaseTickD61,out.unacquiredRetiredD61,t.handTypesKnown68,t.handType68[0],t.handType68[1],t.actionKnown,(unsigned long long)t.action);
  write(line);
 }
};
// Full station source tables are unchanged in most samples. Emit per-slot
// geometry on identity/validity changes and every five seconds, while the
// existing INPUT/JOIN/TOOL rows keep their original short capture cadence.
struct Sources {
 std::uint64_t session=0,owner=0,life=0,last[21]{},identity[21]{};unsigned flags[21]{};
 bool due(std::uint64_t s,std::uint64_t now,unsigned slot,std::uint64_t id,unsigned bits,std::uint64_t instance,std::uint64_t lifetime) noexcept {
  if(!s||!now||slot>=21)return false;if(session!=s||owner!=instance||life!=lifetime){*this={};session=s;owner=instance;life=lifetime;}
  if(!last[slot]||identity[slot]!=id||flags[slot]!=bits||now<last[slot]||now-last[slot]>=5000){
   last[slot]=now;identity[slot]=id;flags[slot]=bits;return true;}return false;
 }
};
}
