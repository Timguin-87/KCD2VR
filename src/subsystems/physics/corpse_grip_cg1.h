#pragma once
#include "core/snapshot_mailbox.h"
#include "corpse_body_owner_d40.h"
#include <cstdint>
#include <cmath>
#include <limits>
// Input consumption is not physics permission. Native qualification owns the
// candidate; the worker owns ONE squeeze; the drag rechecks its native body.
namespace kcd2vr::corpse_grip_cg1 {
inline constexpr float kReleaseGrip=.30f,kAcquireGrip=.58f;
enum class Route : unsigned { ArticulatedPart, EntityVelocityD72, SavedArticulatedPartR95 };
struct Key {
 std::uintptr_t actor=0,identity=0,root=0;
 std::uint64_t epoch=0,serial=0;std::uint32_t entity=0;int part=-1;
 std::uint64_t physicsEpoch=0; // actual native publisher captures the F9 generation, not renewed by a worker
 Route route=Route::ArticulatedPart; std::uint64_t scriptToken=0; // D72 explicit whole-entity script owner; never a fabricated physics part
};
inline bool same(const Key&a,const Key&b) noexcept {
 return a.actor==b.actor&&a.identity==b.identity&&a.root==b.root&&a.epoch==b.epoch&&
  a.serial==b.serial&&a.entity==b.entity&&a.part==b.part&&a.physicsEpoch==b.physicsEpoch&&a.route==b.route&&a.scriptToken==b.scriptToken;
}
inline bool valid(const Key&k) noexcept {return k.actor&&k.identity&&k.root&&k.epoch&&k.serial&&k.entity&&
 (k.route==Route::ArticulatedPart?(k.part>=0&&!k.scriptToken):((k.route==Route::EntityVelocityD72&&k.part==-1&&k.scriptToken)||(k.route==Route::SavedArticulatedPartR95&&k.part>=0&&k.scriptToken)));}
inline bool fresh(std::uint64_t tick,std::uint64_t now) noexcept {return tick&&now>=tick&&now-tick<=80;}
struct Candidate {Key key{};std::uint64_t tick=0;bool active=false;
 // A current native-dead entity/bone within 45 cm is a nomination, not a grab.
 // It can reserve the ORIGINAL squeeze for 120 ms while the same target reaches
 // 25 cm. Body/request/tracking changes cancel it; arbitrary empty space cannot.
 bool pendingOnlyD78=false;std::uint64_t requestEpochD78=0,trackingEpochD78=0;
};
struct Request {Key key{};std::uint64_t tick=0,press=0,claimGeneration=0;};
// W5 (25 Oct, Miki: "grabbing and pulling their corpses with our HANDS"): one mailbox per hand, index 0 RIGHT, 1 LEFT
// (pi.hand / ReadInteractionHandWorld123R1 order). candidate/request stay the LEFT names the CG1 worker block uses.
inline core::SnapshotMailbox<Candidate> handCandidates[2];
inline core::SnapshotMailbox<Request> handRequests[2];
inline core::SnapshotMailbox<Candidate>& candidate=handCandidates[1];
inline core::SnapshotMailbox<Request>& request=handRequests[1];
struct Publisher {
 Candidate current{};std::uint64_t next=0;
 Candidate publish(Key k,std::uint64_t now,bool active,bool pendingOnly=false,std::uint64_t requestEpoch=0,std::uint64_t trackingEpoch=0) noexcept {
  k.serial=current.key.serial;
  if(!current.tick||!same(k,current.key)){k.serial=++next;if(!k.serial)k.serial=++next;}
  current={k,now,active,pendingOnly,requestEpoch,trackingEpoch};(void)handCandidates[side].publish(current);return current;
 }
 void clear() noexcept {current={};(void)handCandidates[side].publish({});}
 int side=1; // W5: which hand's mailbox this publisher owns
};
inline bool permits(const Request&r,const Candidate&c,std::uint64_t now) noexcept {
 return r.press&&fresh(r.tick,now)&&fresh(c.tick,now)&&valid(r.key)&&same(r.key,c.key)&&!c.pendingOnlyD78;
}
// CG1 owns consumed-input and actual native-drag lifetime separately. Only
// new consumption takes shared admission; release/revocation never waits.
// Generations survive release/context changes so an old queued Draw cannot ABA.
struct DrawOccurrence {std::uint64_t generation[2]{};unsigned mask=0;};
struct Claims {
 std::atomic<std::uint64_t> generation[2]{{1},{1}},livePress[2]{};
 std::atomic<bool> consumed[2]{},nativeHeld[2]{},fault[2]{};
 unsigned reserved() const noexcept {
  unsigned bits=0;for(int s=0;s<2;++s)if(consumed[s].load(std::memory_order_acquire)||
   nativeHeld[s].load(std::memory_order_acquire)||fault[s].load(std::memory_order_acquire))bits|=1u<<s;
  return bits;
 }
 bool capture(unsigned admitted,DrawOccurrence& out) const noexcept {
  out={};if((admitted&3u)!=3u||reserved())return false;
  // Draw has native cross-item reevaluation/listeners; both hands are admitted
  // for every branch, including later mode/loadout changes. No side-only ABI.
  out={{generation[0].load(std::memory_order_acquire),generation[1].load(std::memory_order_acquire)},3u};
  return out.generation[0]&&out.generation[1];
 }
 bool current(const DrawOccurrence& original) const noexcept {
  return original.mask==3u&&original.generation[0]&&original.generation[1]&&!reserved()&&
   original.generation[0]==generation[0].load(std::memory_order_acquire)&&
   original.generation[1]==generation[1].load(std::memory_order_acquire);
 }
 bool authority(int side,const Request& r) const noexcept {
  return side>=0&&side<2&&r.press&&r.claimGeneration&&!fault[side].load(std::memory_order_acquire)&&
   consumed[side].load(std::memory_order_acquire)&&r.press==livePress[side].load(std::memory_order_acquire)&&
   r.claimGeneration==generation[side].load(std::memory_order_acquire);
 }
 void held(int side,bool value) noexcept {if(side>=0&&side<2)nativeHeld[side].store(value,std::memory_order_release);}
};
inline Claims claims;
template<class Output> inline bool stagedAcquisition(const Output& out) noexcept {
 if(out.commandCount>out.commands.size())return true;
 for(unsigned i=0;i<out.commandCount;++i)if(out.commands[i].corpseClaimCg1.mask)return true;
 return false;
}
// R89 corpse (R90: route ONLY ON SQUEEZE, lab58::corpseSqueezeSearchR90). The
// dead-body search (native part scan and the Lua sphere query of the saved-corpse
// route) runs only while a squeeze is STARTING: from grip >= kSearchGripR89 until
// kSearchWindowMsR89 later. An open hand, a hand that keeps the grip closed, or
// stale input does no search at all. D85R5 log 3 Nov: frameMsP95 ~20 ms while the
// always-on search ran, 12.5 ms while the same session refused it.
inline constexpr float kSearchGripR89=.15f;
inline constexpr std::uint64_t kSearchWindowMsR89=700;
inline bool closingR89(bool fresh,float grip) noexcept {
 return fresh&&std::isfinite(grip)&&grip>=kSearchGripR89&&grip<=1.f;
}
struct SearchWindowR89 {
 std::uint64_t start=0;bool open=false;
 // Call update() every tick the owner runs: an open hand must be seen to rearm.
 bool update(bool fresh,float grip,std::uint64_t now) noexcept {
  if(!closingR89(fresh,grip)){start=0;open=false;return false;}
  if(!open){open=true;start=now;}
  return now>=start&&now-start<=kSearchWindowMsR89;
 }
};
struct Output {bool suppressLeftUse=false;Request native{};std::uint64_t returnedPressD78=0;};
struct Gate {
 bool armed=false,down=false,consumed=false,permission=false;std::uint64_t next=0,press=0;Key owner{};
 bool waitingD78=false,returnAllowedD78=false;std::uint64_t pendingTickD78=0,pendingLastD78=0,requestEpochD78=0,trackingEpochD78=0;
 Output update(std::uint64_t now,bool inputCurrent,bool context,float grip,const Candidate&c,bool ordinaryNeutral=true) noexcept {
  Output out{};
  const bool raw=inputCurrent&&std::isfinite(grip)&&grip>=0&&grip<=1;
  const bool released=raw&&grip<kReleaseGrip;
  if(consumed){
   // Losing qualification retires native authority, never releases this old
   // squeeze into a delayed A. Only a real current opened hand consumes release.
   out.suppressLeftUse=true;
   if(!raw||!context||!fresh(c.tick,now)||!same(owner,c.key)){
    permission=false;waitingD78=false;returnAllowedD78=false;
   }
   if(released){
    consumed=permission=down=waitingD78=returnAllowedD78=false;armed=true;owner={};press=0;
    // This positively current open hand is a real release. Let ordinary pulse
    // and held policies see it so the next squeeze can rearm in one sample.
    // The claimed press already retired their pending Use/tap on acquisition;
    // the empty native request below releases drag without producing an A edge.
    out.suppressLeftUse=false;return out;
   }
   if(waitingD78){
    returnAllowedD78=returnAllowedD78&&ordinaryNeutral;
    const bool sameInput=requestEpochD78&&trackingEpochD78&&requestEpochD78==c.requestEpochD78&&
     trackingEpochD78==c.trackingEpochD78&&fresh(pendingLastD78,now)&&now>=pendingTickD78;
    if(!sameInput){waitingD78=returnAllowedD78=false;return out;}
    pendingLastD78=now;
    if(now-pendingTickD78>120){
     waitingD78=false;
     // Return an unspent current occurrence only. The caller re-admits that
     // same original press to ordinary producers; no release or new Grip is
     // manufactured, and a cancelled/changed occurrence stays consumed.
     if(returnAllowedD78){out.returnedPressD78=pendingTickD78;out.suppressLeftUse=false;
      consumed=permission=false;owner={};press=0;}
     returnAllowedD78=false;return out;
    }
    if(!c.pendingOnlyD78){waitingD78=returnAllowedD78=false;permission=true;}
   }
   if(!waitingD78&&c.pendingOnlyD78)permission=false;
   if(permission)out.native={owner,now,press};
   return out;
  }
  if(!raw){armed=false;return out;}
  if(released){armed=true;down=false;return out;}
  if(grip>kAcquireGrip&&!down){
   down=true;
   if(armed&&context&&fresh(c.tick,now)&&valid(c.key)&&
      (!c.pendingOnlyD78||(c.requestEpochD78&&c.trackingEpochD78))){
    consumed=true;permission=!c.pendingOnlyD78;owner=c.key;press=++next;if(!press)press=++next;
    waitingD78=c.pendingOnlyD78;returnAllowedD78=waitingD78&&ordinaryNeutral;
    pendingTickD78=pendingLastD78=now;requestEpochD78=c.requestEpochD78;trackingEpochD78=c.trackingEpochD78;
    out.suppressLeftUse=true;if(permission)out.native={owner,now,press};
   }
   armed=false;
  }
  return out;
 }
};
// Sole worker owner of Gate; mailbox is an explicit publication leaf. A
// failed clear cannot revive an old request: livePress is revoked first.
template<class Admission,class Mailbox> inline Output updateShared(Gate& gate,int side,
 std::uint64_t now,bool inputCurrent,bool context,float grip,const Candidate& c,Mailbox& mailbox,bool ordinaryNeutral=true) noexcept {
 if(side<0||side>1)return {};
 Admission admission;
 const bool was=gate.consumed;
 // Do not contend with unrelated features for idle/open/unknown input.
 const bool acquiring=!was&&gate.armed&&!gate.down&&inputCurrent&&context&&
  std::isfinite(grip)&&grip>kAcquireGrip&&grip<=1.f&&fresh(c.tick,now)&&valid(c.key);
 const bool entry=was||!acquiring||admission.take(1u<<side);
 auto out=gate.update(now,inputCurrent,context&&entry&&!claims.fault[side].load(std::memory_order_acquire),grip,c,ordinaryNeutral);
 if(!was&&gate.consumed){
  claims.consumed[side].store(true,std::memory_order_release);
  const auto before=claims.generation[side].load(std::memory_order_relaxed);
  if(before==std::numeric_limits<std::uint64_t>::max()){
   claims.fault[side].store(true,std::memory_order_release);gate.permission=false;out.native={};
  }else claims.generation[side].store(before+1,std::memory_order_release);
 }
 if(out.native.press)out.native.claimGeneration=claims.generation[side].load(std::memory_order_acquire);
 claims.livePress[side].store(out.native.press,std::memory_order_release);
 // A real release may clear consumption while nativeHeld still fences Draw.
 claims.consumed[side].store(gate.consumed,std::memory_order_release);
 (void)mailbox.publish(out.native);
 return out;
}
}
