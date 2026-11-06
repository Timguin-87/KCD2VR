#pragma once
#include "../../core/snapshot_mailbox.h"
#include "../../core/mutual_admission_t25.h"
#include "../../vrik/visible_failure.h"
#include "door_press_why_r9.h" // R9: why a squeeze did not become a hand door claim
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <limits>

namespace kcd2vr::hand_only_door_h40 {
constexpr std::uint64_t kMaxInteger=9007199254740991ull;
constexpr const char* kCallback="kcd2vr_door_hand_h40_t2";
struct Frame {
 std::uint64_t tick=0,body=0,request=0,sample=0,tracking=0,policy=0;
 std::uintptr_t actor=0,character=0,nativePlayer=0,manager=0;std::uint32_t player=0;
 float grip[2]{},point[2][3]{};bool permitted=false,available[2]{};
};
inline bool Fresh(std::uint64_t tick,std::uint64_t now,std::uint64_t limit=80) noexcept {return tick&&now>=tick&&now-tick<=limit;}
inline bool Identity(const Frame&a,const Frame&b) noexcept {
 return a.player&&a.actor&&a.character&&a.body&&a.request&&a.tracking&&a.policy&&
  a.player==b.player&&a.actor==b.actor&&a.character==b.character&&a.nativePlayer==b.nativePlayer&&a.manager==b.manager&&a.body==b.body&&a.request==b.request&&a.tracking==b.tracking&&a.policy==b.policy;
}
inline bool Valid(const Frame&f,std::uint64_t now) noexcept {
 if(!f.player||!f.actor||!f.character||!f.body||!f.request||!f.sample||!f.tracking||!f.policy||
    f.body>kMaxInteger||f.request>kMaxInteger||f.tick>kMaxInteger||!Fresh(f.tick,now))return false;
 for(int s=0;s<2;++s){if(!std::isfinite(f.grip[s])||f.grip[s]<0||f.grip[s]>1)return false;
  for(float v:f.point[s])if(!std::isfinite(v))return false;}
 return true;
}
// Session test switch; does not change H40 input, native admission or release.
inline std::atomic<bool> displayEnabled{false};
struct Display {
 Frame source{};std::uint64_t tick=0,serial=0,token[2]{};std::uint32_t door[2]{};
 unsigned mask=0;float point[2][3]{},phase[2]{};
};
inline core::SnapshotMailbox<Display> display;
inline core::SnapshotMailbox<Frame> completed;
inline std::atomic<bool> ready{false},enabled{true},vmQuarantine{false},transportInstalled{false};
// R96: the VM quarantine was a session latch (hand doors off until restart
// after a Lua VM replacement during a held door). It now lifts when the
// completed frame shows a different player body (save load / actor change):
// the old door clip belongs to a world that no longer exists.
inline std::atomic<std::uint32_t> quarantinePlayerR96{0};
inline std::atomic<std::uint64_t> quarantineBodyR96{0};
using ClockR96=std::uint64_t(*)() noexcept;
inline std::atomic<ClockR96> clockR96{nullptr};
inline std::uint64_t ObserveClockR96(std::uint64_t entry) noexcept {const auto fn=clockR96.load(std::memory_order_acquire);return fn?fn():entry;}
inline std::atomic<std::uint64_t> nearTick{0};
inline std::atomic<unsigned> admissionBusy{0};
inline std::atomic<std::uint64_t> claims[2]{},entered[2]{};
// D49: entered is uncertain native debt, even before Lua finds a door.
// Only a validated same-token ACK with a concrete door publishes acquisition.
inline std::atomic<std::uint64_t> acquiredD49[2]{};
inline std::atomic<unsigned> currentWhy[2]{}; // W4: last HandOnlyDoorCurrentH40 refusal code per hand (0 = current)
// Actual Lua-owned paused operations; not a hand reservation or permission.
inline std::atomic<unsigned> resting{0};
inline std::atomic<std::uint64_t> nextClaim{1};
struct Candidate {Frame original{};std::uint64_t token=0,tick=0;std::uint32_t target=0;};
inline core::SnapshotMailbox<Candidate> candidates[2];
struct Fallback {Candidate candidate{};};
inline core::SnapshotMailbox<Fallback> fallback[2];
inline std::atomic<std::uint64_t> fallbackToken[2]{};
inline void ObserveNear(std::uint64_t now) noexcept {nearTick.store(now,std::memory_order_release);}
inline std::atomic<bool> neutral[2]{true,true};
using CurrentReader=bool(*)(const Frame&,int,std::uint64_t) noexcept;
inline std::atomic<CurrentReader> currentReader{nullptr};
using InputIdentityReader=bool(*)(Frame&,std::uint64_t) noexcept;
inline std::atomic<InputIdentityReader> inputIdentityReader{nullptr};
// Sole mod-input producer owns this bounded, not-yet-native press record.
// It grants no script/clip ownership and is never read by the script callback.
struct PendingInput {Frame identity{};std::uint64_t tick=0;std::uint32_t target=0;bool active=false;};
inline PendingInput pendingInput[2];
struct NativeFallback {Candidate original{};bool active=false,armed=false;};
inline NativeFallback nativeFallback[2];
inline bool Pending(int side) noexcept {return side>=0&&side<2&&pendingInput[side].active;}
inline unsigned Reserved() noexcept {return (claims[0].load(std::memory_order_acquire)?1u:0u)|(claims[1].load(std::memory_order_acquire)?2u:0u);}
// May-own debt defers dispatch and survives an unknown callback outcome.
// It is NOT evidence of a door target and must not cancel holster Grip.
inline unsigned Entered() noexcept {
 unsigned mask=0;
 for(int side=0;side<2;++side){const auto claim=claims[side].load(std::memory_order_acquire);
  if(claim&&entered[side].load(std::memory_order_acquire)==claim&&
     claims[side].load(std::memory_order_acquire)==claim)mask|=1u<<side;
 }
 return mask;}
inline unsigned Acquired() noexcept {
 unsigned mask=0;
 for(int side=0;side<2;++side){const auto claim=claims[side].load(std::memory_order_acquire);
  if(claim&&acquiredD49[side].load(std::memory_order_acquire)==claim&&
     entered[side].load(std::memory_order_acquire)==claim&&
     claims[side].load(std::memory_order_acquire)==claim)mask|=1u<<side;
 }
 return mask;}
inline bool RetireClaimH41(int side,std::uint64_t original) noexcept {
 if(side<0||side>1||!original)return false;auto expected=original;
 if(!claims[side].compare_exchange_strong(expected,0,std::memory_order_acq_rel))return false;
 expected=original;(void)acquiredD49[side].compare_exchange_strong(expected,0,std::memory_order_acq_rel);
 (void)mutual_admission_t25::RetireDoorH41(side,original);return true;
}
// Transient cross-feature admission only; H40's existing local lease still
// protects its script callback. Never carry a T25 transaction into native/Lua.
struct DoorAdmissionH41 {
 mutual_admission_t25::Token token{};
 bool take(int side) noexcept {token=mutual_admission_t25::TryBegin(side,mutual_admission_t25::Kind::Door);return mutual_admission_t25::valid(token);}
 static void publish(void*) noexcept {}
 ~DoorAdmissionH41(){if(mutual_admission_t25::valid(token))(void)mutual_admission_t25::Finish(token,&publish,nullptr);}
};
// One bounded CAS. This is admission exclusion, never an engine lock or wait.
// Native cleanup does not acquire this lease. Other-hand admission is independent.
struct Admission {
 unsigned mask=0;
 Admission()=default;Admission(const Admission&)=delete;Admission&operator=(const Admission&)=delete;
 bool take(unsigned bits,bool own=false) noexcept {
  bits&=3;if(!bits)return false;
  if(!own&&(Reserved()&bits))return false;
  auto before=admissionBusy.load(std::memory_order_acquire);
  if((before&bits)||!admissionBusy.compare_exchange_strong(before,before|bits,std::memory_order_acq_rel))return false;
  mask=bits;
  if(!own&&(Reserved()&bits)){release();return false;}return true;
 }
 void release() noexcept {if(mask){admissionBusy.fetch_and(~mask,std::memory_order_release);mask=0;}}
 ~Admission(){release();}
};
// Staged DRAW may cross gesture/native sides (bow/quiver/whole-loadout).
// Keep BOTH leases for any actual DRAW until queue publication. With no DRAW,
// release immediately instead of parking unrelated gestures through skinning.
template<class Output,class Kind> inline void ReleaseUnstagedAdmissions(const Output&out,Kind draw,Admission(&lease)[2]) noexcept {
 if(out.commandCount>out.commands.size())return; // unknown staged data cannot relinquish exclusion
 for(unsigned i=0;i<out.commandCount;++i)if(out.commands[i].kind==draw)return;
 lease[0].release();lease[1].release();
}
inline bool Current(const Frame&f,int side,std::uint64_t now) noexcept {
 const auto fn=currentReader.load(std::memory_order_acquire);now=ObserveClockR96(now);
 return side>=0&&side<2&&Valid(f,now)&&f.permitted&&f.available[side]&&fn&&fn(f,side,now);
}
inline bool ReadDisplay(int side,std::uintptr_t character,std::uint64_t body,std::uint64_t request,
 std::uint64_t tracking,std::uint64_t policy,std::uint64_t now,Display&out) noexcept {
 out={};Display d{};
 if(!displayEnabled.load(std::memory_order_acquire)||side<0||side>1||!display.read(d)||
    !(d.mask&(1u<<side))||!Fresh(d.tick,now)||d.source.character!=character||d.source.body!=body||
    d.source.request!=request||d.source.tracking!=tracking||d.source.policy!=policy||
    !d.token[side]||claims[side].load(std::memory_order_acquire)!=d.token[side]||
    entered[side].load(std::memory_order_acquire)!=d.token[side]||!Current(d.source,side,now))return false;
 out=d;return true;
}
inline void PublishCompleted(const Frame&f) noexcept {(void)completed.publish(f);}
inline void RevokeCompleted() noexcept {(void)completed.publish({});}
// Called before the original interaction producer emits Use. A physical press
// is consumed here even if readiness is absent; it cannot become a new H40
// acquisition halfway through a native Use after a delayed readiness reply.
// R9: one refusal code per press (door_press_why_r9.h). the input thread is the only caller.
inline door_press_why_r9::Press pressWhy[2];
inline unsigned lastBusyR9[2]{}; // last per-frame admission failure of the open press (input thread)
inline std::atomic<std::uint64_t> pressSerial{0};
inline bool PressRefusedR9(int side,unsigned code,std::uint64_t now) noexcept {
 namespace pw=door_press_why_r9;
 if(side<0||side>1||!pressWhy[side].note(code))return false;
 pw::last[side].store(code,std::memory_order_relaxed);pw::lastTick[side].store(now,std::memory_order_relaxed);
 pw::refused[side].fetch_add(1,std::memory_order_relaxed);
 if(const char* text=pw::say(code))(void)visible_failure::publish(text,now);
 return false;
}
inline bool CandidatePress(int side,std::uint32_t target,std::uint64_t now,bool eligible,float grip,float trigger,unsigned ineligibleWhy=door_press_why_r9::HandBusy) noexcept {
 namespace pw=door_press_why_r9;
 if(side<0||side>1)return false;auto&pending=pendingInput[side];
 if(std::isfinite(grip)&&grip<.25f){neutral[side].store(true,std::memory_order_release);fallbackToken[side].store(0,std::memory_order_release);nativeFallback[side]={};pending={};
  if(lastBusyR9[side]){(void)PressRefusedR9(side,lastBusyR9[side],now);lastBusyR9[side]=0;} // released while still refused
  pressWhy[side].end();return false;}
 if(!std::isfinite(grip)||!std::isfinite(trigger)){pending={};return false;}
 const bool edge=grip>.55f&&neutral[side].exchange(false,std::memory_order_acq_rel);
 if(edge){pressWhy[side].begin(pressSerial.fetch_add(1,std::memory_order_relaxed)+1);lastBusyR9[side]=0;}
 const auto no=[&](unsigned code){return PressRefusedR9(side,code,now);};
 if(!eligible){pending={};return edge?no(ineligibleWhy?ineligibleWhy:unsigned(pw::HandBusy)):false;}
 if(!transportInstalled.load(std::memory_order_acquire)||!enabled.load(std::memory_order_acquire)){pending={};return edge?no(pw::Off):false;}
 if(trigger>=.10f){pending={};return no(pw::Trigger);}
 if(edge){
  Frame identity{};const auto capture=inputIdentityReader.load(std::memory_order_acquire);
  const bool captured=capture?capture(identity,now):completed.read(identity);
  if(!captured||!Valid(identity,ObserveClockR96(now))){pending={};return no(pw::NoIdentity);}
  pending={identity,now,target,true};
 }
 if(!pending.active||grip<=.55f)return false;
 if(!Fresh(pending.tick,now,100)){pending={};const unsigned why=lastBusyR9[side];lastBusyR9[side]=pw::None;return no(why?why:unsigned(pw::PressExpired));}
 nearTick.store(now,std::memory_order_release);
 // the leases below are retried every frame for 100 ms; only the code of the LAST try matters, so it is
 // noted when the press expires (above) - noting here would report a one-frame contention as the reason.
 Admission lease;if(!lease.take(1u<<side)){lastBusyR9[side]=pw::LeaseBusy;return false;}
 DoorAdmissionH41 shared;if(!shared.take(side)){lastBusyR9[side]=pw::SharedBusy;return false;}
 Frame f{};
 const bool copiedR96=completed.read(f);now=ObserveClockR96(now);
 if(!copiedR96||!Valid(f,now)){lastBusyR9[side]=pw::PoseMissing;return false;}
 if(!Fresh(pending.tick,now,100)){pending={};return no(pw::PressExpired);}
 if(!f.permitted){lastBusyR9[side]=pw::PoseNotPermitted;return false;}
 if(!f.available[side]){lastBusyR9[side]=pw::HandNotAvailable;return false;}
 if(!Current(f,side,now)){lastBusyR9[side]=pw::PoseNotCurrent;return false;}
 if(!Identity(pending.identity,f)){pending={};return no(pw::IdentityChanged);}
 auto serial=nextClaim.load(std::memory_order_acquire);
 if(!serial||serial>=kMaxInteger||!nextClaim.compare_exchange_strong(serial,serial+1,std::memory_order_acq_rel)){lastBusyR9[side]=pw::ClaimRace;return false;}
 std::uint64_t empty=0;if(!claims[side].compare_exchange_strong(empty,serial,std::memory_order_acq_rel)){lastBusyR9[side]=pw::ClaimRace;return false;}
 if(!mutual_admission_t25::RetainDoorH41(shared.token,serial)){(void)RetireClaimH41(side,serial);lastBusyR9[side]=pw::ClaimRace;return false;}
 if(!candidates[side].publish({f,serial,pending.tick,pending.target})){
  (void)RetireClaimH41(side,serial);lastBusyR9[side]=pw::ClaimRace;return false;
 }
 pending={};lastBusyR9[side]=pw::None;pressWhy[side].claimed();
 pw::last[side].store(pw::None,std::memory_order_relaxed);pw::lastTick[side].store(now,std::memory_order_relaxed);
 pw::claimedCount[side].fetch_add(1,std::memory_order_relaxed);
 return true;
}

struct Ack {std::uint64_t serial=0,player=0,body=0,request=0,ackSeq=0;unsigned mask=0,retiring=0,resting=0;std::uint32_t door[2]{};bool restProtocol=false;};
inline bool Integer(const char*&p,std::uint64_t&out,char delimiter) noexcept {
 if(*p<'0'||*p>'9')return false;std::uint64_t v=0;
 do{const unsigned n=unsigned(*p-'0');if(v>(kMaxInteger-n)/10)return false;v=v*10+n;++p;}while(*p>='0'&&*p<='9');
 if(*p!=delimiter)return false;if(delimiter)++p;out=v;return true;
}
inline bool Parse(const char*text,Ack&out) noexcept {
 out={};if(!text)return false;
 const bool rest=!std::strncmp(text,"H40R1|",6);
 if(!rest&&std::strncmp(text,"OK|",3))return false;
 const char*p=text+(rest?6:3);std::uint64_t mask=0,retiring=0,d0=0,d1=0,count=0;
 if(!Integer(p,out.serial,'|')||!Integer(p,out.player,'|')||!Integer(p,out.body,'|')||!Integer(p,out.request,'|')||
    !Integer(p,mask,'|')||!Integer(p,retiring,'|')||!Integer(p,out.ackSeq,'|')||!Integer(p,d0,'|')||!Integer(p,d1,rest?'|':0)||(rest&&!Integer(p,count,0))||
    !out.serial||!out.player||out.player>UINT32_MAX||!out.body||!out.request||mask>3||retiring>3||(retiring&~mask)||
    out.ackSeq>out.serial||d0>UINT32_MAX||d1>UINT32_MAX||count>2||bool(mask&1)!=bool(d0)||bool(mask&2)!=bool(d1))return false;
 out.mask=unsigned(mask);out.retiring=unsigned(retiring);out.resting=unsigned(count);out.restProtocol=rest;out.door[0]=std::uint32_t(d0);out.door[1]=std::uint32_t(d1);return true;
}
inline bool NumberDisplay(const char*&p,float&out,char delimiter) noexcept {
 char*end=nullptr;const double value=std::strtod(p,&end);
 if(end==p||*end!=delimiter||!std::isfinite(value)||std::fabs(value)>double(std::numeric_limits<float>::max()))return false;
 out=float(value);p=end+(delimiter?1:0);return true;
}
inline bool ParseDisplay(const char*text,Display&out) noexcept {
 out={};if(!text||std::strncmp(text,"H40D1|",6))return false;const char*p=text+6;
 std::uint64_t mask=0,player=0;
 if(!Integer(p,out.serial,'|')||!Integer(p,player,'|')||!Integer(p,out.source.body,'|')||
    !Integer(p,out.source.request,'|')||!Integer(p,mask,'|')||!out.serial||!player||player>UINT32_MAX||mask>3)return false;
 out.source.player=std::uint32_t(player);out.mask=unsigned(mask);
 for(int s=0;s<2;++s){std::uint64_t door=0;
  if(!Integer(p,out.token[s],'|')||!Integer(p,door,'|')||door>UINT32_MAX||
     !NumberDisplay(p,out.point[s][0],'|')||!NumberDisplay(p,out.point[s][1],'|')||
     !NumberDisplay(p,out.point[s][2],'|')||!NumberDisplay(p,out.phase[s],s==1?0:'|')||
     bool(mask&(1u<<s))!=bool(door)||bool(door)!=bool(out.token[s])||out.phase[s]<0||out.phase[s]>1)return false;
  out.door[s]=std::uint32_t(door);
 }
 return true;
}
inline bool Packet(const Frame&f,std::uint64_t serial,const char*action,const float*g,char*out,std::size_t cap,const std::uint64_t*tokens=nullptr,const std::uint32_t*targets=nullptr) noexcept {
 const int n=std::snprintf(out,cap,"%llu|%s|%u|%llu|%llu|%llu|%.9g|%.9g|%.9g|%.9g|%.9g|%.9g|%.9g|%.9g|%llu|%llu|%u|%u",
  (unsigned long long)serial,action,f.player,(unsigned long long)f.body,(unsigned long long)f.request,(unsigned long long)f.tick,
  double(g[0]),double(f.point[0][0]),double(f.point[0][1]),double(f.point[0][2]),
  double(g[1]),double(f.point[1][0]),double(f.point[1][1]),double(f.point[1][2]),
  (unsigned long long)(tokens?tokens[0]:0),(unsigned long long)(tokens?tokens[1]:0),targets?targets[0]:0u,targets?targets[1]:0u);
 return n>0&&std::size_t(n)<cap;
}
// Actual service state, used unchanged by script callback and host tests. All
// outputs are one synchronous typed CallPacket; no native ABI lives in this class.
enum class Stage : unsigned {Idle,Probe,AwaitCompletedPose,Feed,Release,Retained,Retiring,Cleared,Fault,VmQuarantine,Rested};
enum class Reason : unsigned {None,AdmissionBusy,LocalMailbox,PoseNotQualified,ProbeRejected,PacketFormat,CallFailed,MalformedReply,IdentityMismatch,UnexpectedSide,MissingTerminalAck,VmReplaced,SerialExhausted,LuaPendingOrOwned,Terminal,PartialRest};
inline const char* Name(Stage s) noexcept {constexpr const char*n[]{"IDLE","PROBE","WAIT_COMPLETED_POSE","FEED","RELEASE","LUA_PENDING_OR_OWNED","RETIRING","CLEARED","FAULT","VM_QUARANTINE","RELEASED_PARTIAL_REST"};const auto i=unsigned(s);return i<11?n[i]:"UNKNOWN";}
inline const char* Name(Reason s) noexcept {constexpr const char*n[]{"NONE","ADMISSION_BUSY","LOCAL_MAILBOX_UNAVAILABLE","POSE_NOT_QUALIFIED","PROBE_REJECTED","PACKET_FORMAT","CALL_FAILED","MALFORMED_REPLY","IDENTITY_MISMATCH","UNEXPECTED_SIDE","MISSING_TERMINAL_ACK","VM_REPLACED","SERIAL_EXHAUSTED","LUA_PENDING_OR_OWNED","MATCHED_LUA_TERMINAL","ORIGINAL_NATIVE_OPERATION_RETAINED"};const auto i=unsigned(s);return i<16?n[i]:"UNKNOWN";}
struct Receipt {std::uint64_t tick=0,body=0,request=0,sample=0,sourceTick=0,command=0;std::uint32_t player=0,doors[2]{};
 unsigned reserved=0,mayOwn=0,acquired=0,retiring=0,resting=0;Stage stage=Stage::Idle;Reason reason=Reason::None,fault=Reason::None;};
inline core::SnapshotMailbox<Receipt> receipts;
struct ServiceState {
 Frame original{};bool identity=false,restProtocol=false;std::uint64_t serial=0,vm=0,lastProbe=0;
 std::uint32_t doors[2]{};std::uint64_t probeToken[2]{};bool fault=false;Frame diagnosticOriginal{};Stage stage=Stage::Idle;Reason reason=Reason::None,faultReason=Reason::None;unsigned retiring=0;
 template<class IO> void tick(IO&io,std::uint64_t now,std::uint64_t generation) noexcept {
  stage=Stage::Idle;reason=Reason::None;
  bool displayPublished=false;
  struct DisplayOnExit {bool&published;~DisplayOnExit(){if(!published&&displayEnabled.load(std::memory_order_acquire))(void)display.publish({});}} displayExit{displayPublished};
  struct ReceiptOnExit {ServiceState&s;std::uint64_t& now;~ReceiptOnExit(){Receipt r{};r.tick=now;r.body=s.diagnosticOriginal.body;r.request=s.diagnosticOriginal.request;
   r.player=s.diagnosticOriginal.player;r.sample=s.diagnosticOriginal.sample;r.sourceTick=s.diagnosticOriginal.tick;r.command=s.serial;r.reserved=Reserved();
   r.mayOwn=Entered();r.acquired=Acquired();r.retiring=s.retiring;r.resting=resting.load(std::memory_order_acquire);r.doors[0]=s.doors[0];r.doors[1]=s.doors[1];
   r.stage=s.stage;r.reason=s.reason;r.fault=s.faultReason;(void)receipts.publish(r);}} receipt{*this,now};
  if(vm!=generation){
   if(vm&&((Reserved()&&(entered[0].load()||entered[1].load()))||resting.load(std::memory_order_acquire))){
    const Frame& qid=original.player?original:diagnosticOriginal; // resting-only debt has no live claim identity
    quarantinePlayerR96.store(qid.player,std::memory_order_release);quarantineBodyR96.store(qid.body,std::memory_order_release);
    vmQuarantine.store(true,std::memory_order_release);fault=true;faultReason=Reason::VmReplaced;}
   vm=generation;ready.store(false,std::memory_order_release);lastProbe=0;
   // R96R1: an unentered claim must prove readiness in this VM, even with the same token.
   probeToken[0]=probeToken[1]=0;
  }
  // A replacement VM cannot acknowledge the old script instance's obligations.
  if(vmQuarantine.load(std::memory_order_acquire)){
   Frame q{};
   const bool copiedQR97=completed.read(q);const std::uint64_t qNow=ObserveClockR96(now);
   if(copiedQR97&&Valid(q,qNow)&&(q.player!=quarantinePlayerR96.load(std::memory_order_acquire)||
      q.body!=quarantineBodyR96.load(std::memory_order_acquire))){
    for(int s=0;s<2;++s){const auto c=claims[s].load(std::memory_order_acquire);if(c)(void)RetireClaimH41(s,c);entered[s].store(0,std::memory_order_release);}
    resting.store(0,std::memory_order_release);vmQuarantine.store(false,std::memory_order_release);
    fault=false;faultReason=Reason::None;identity=false;original={};doors[0]=doors[1]=0;ready.store(false,std::memory_order_release);lastProbe=0;probeToken[0]=probeToken[1]=0;
    stage=Stage::Cleared;reason=Reason::VmReplaced;return; // receipt row: CLEARED/VM_REPLACED = quarantine lifted
   }
   stage=Stage::VmQuarantine;reason=Reason::VmReplaced;return;}
  const unsigned reserved=Reserved(),nativeRest=resting.load(std::memory_order_acquire);Frame latest{};
  const bool copiedR96=completed.read(latest);now=ObserveClockR96(now);const bool have=copiedR96&&Valid(latest,now);
  if(!reserved&&!nativeRest){
   identity=false;original={};doors[0]=doors[1]=0;
   if(!enabled.load()||!have||!latest.permitted||!Fresh(nearTick.load(),now,120)||ready.load()||
      (lastProbe&&now>=lastProbe&&now-lastProbe<1000))return;
   stage=Stage::Probe;diagnosticOriginal=latest;lastProbe=now;if(serial>=kMaxInteger){reason=Reason::SerialExhausted;return;}const auto sent=++serial;const float no[2]{-1,-1};char in[512]{},out[512]{};Ack ack{};
   if(Packet(latest,sent,"probe",no,in,sizeof(in))&&io.call(in,out,sizeof(out))&&Parse(out,ack)&&
      ack.serial==sent&&ack.player==latest.player&&ack.body==latest.body&&ack.request==latest.request&&ack.mask==0&&ack.retiring==0&&ack.resting==0){
    restProtocol=ack.restProtocol;ready.store(true,std::memory_order_release);
    // A failed readiness-only call never entered a native door operation.
    // Only an exact successful empty probe can clear that local probe fault.
    if(faultReason==Reason::ProbeRejected&&!entered[0].load()&&!entered[1].load()){fault=false;faultReason=Reason::None;}
   }
   return;
  }
  Admission lease;if(!lease.take(reserved?reserved:3u,true)){stage=Stage::AwaitCompletedPose;reason=Reason::AdmissionBusy;return;}
  std::uint64_t tokens[2]{(reserved&1u)?claims[0].load():0,(reserved&2u)?claims[1].load():0};Candidate candidate[2]{};
  for(int s=0;s<2;++s)if(tokens[s]&&candidates[s].read(candidate[s])&&candidate[s].token==tokens[s]&&!identity){original=candidate[s].original;diagnosticOriginal=original;identity=true;}
  if(!identity){stage=Stage::AwaitCompletedPose;reason=Reason::LocalMailbox;return;} // no native ownership
  if(!nativeRest&&!ready.load(std::memory_order_acquire)&&!entered[0].load()&&!entered[1].load()&&
     (!fault||faultReason==Reason::ProbeRejected)&&
     (probeToken[0]!=tokens[0]||probeToken[1]!=tokens[1])){
   probeToken[0]=tokens[0];probeToken[1]=tokens[1];
   stage=Stage::Probe;if(serial>=kMaxInteger){reason=Reason::SerialExhausted;return;}const auto sent=++serial;const float no[2]{-1,-1};char in[512]{},out[512]{};Ack ack{};
   if(!Packet(original,sent,"probe",no,in,sizeof(in))||!io.call(in,out,sizeof(out))||!Parse(out,ack)||
      ack.serial!=sent||ack.player!=original.player||ack.body!=original.body||ack.request!=original.request||ack.mask||ack.retiring||ack.resting){
    fault=true;stage=Stage::Fault;reason=faultReason=Reason::ProbeRejected;return; // no replay
   }
   restProtocol=ack.restProtocol;ready.store(true,std::memory_order_release);
   if(faultReason==Reason::ProbeRejected){fault=false;faultReason=Reason::None;}
  }
  float grip[2]{-1,-1};bool feed=enabled.load()&&have&&Identity(original,latest)&&!fault;
  for(int s=0;s<2;++s)if(tokens[s]){
   const bool current=feed&&Current(latest,s,now)&&Identity(candidate[s].original,original);
   if(current&&(entered[s].load()?(restProtocol||latest.grip[s]>=.30f):latest.grip[s]>.55f))grip[s]=latest.grip[s];
   if(!entered[s].load()&&grip[s]<0&&(!Fresh(candidate[s].tick,now,100)||!feed)){
    (void)RetireClaimH41(s,tokens[s]);tokens[s]=0;
   }
  }
  if(!tokens[0]&&!tokens[1]&&!nativeRest){
   // The last unentered claim was retired above. Wanted() can now stop service
   // callbacks entirely, so the idle branch may never clear this identity.
   // A later press must take its own original body/request/tracking identity.
   identity=false;original={};doors[0]=doors[1]=0;
   stage=Stage::Cleared;return;
  }
  const bool drive=grip[0]>=0||grip[1]>=0;
  const bool keepRest=nativeRest&&feed&&latest.permitted;
  if(!drive&&!nativeRest&&!entered[0].load()&&!entered[1].load()){
   stage=Stage::AwaitCompletedPose;reason=Reason::PoseNotQualified;return;
  }
  stage=(drive||keepRest)?Stage::Feed:Stage::Release;
  Frame packetFrame=feed?latest:original;
  if(serial>=kMaxInteger){fault=true;ready.store(false);stage=Stage::Fault;reason=faultReason=Reason::SerialExhausted;return;}
  const auto sent=++serial;char in[512]{},out[512]{};Ack ack{};
  const std::uint32_t offeredD48[2]{candidate[0].target,candidate[1].target};
  if(!Packet(packetFrame,sent,(drive||keepRest)?"feed":"clear",grip,in,sizeof(in),tokens,offeredD48)){fault=true;stage=Stage::Fault;reason=faultReason=Reason::PacketFormat;return;}
  // May-own BEFORE entry. A lost/invalid response can never forgive it.
  for(int s=0;s<2;++s)if(tokens[s])entered[s].store(tokens[s],std::memory_order_release);
  Reason bad=Reason::None;
  if(!io.call(in,out,sizeof(out)))bad=Reason::CallFailed;
  else if(!Parse(out,ack))bad=Reason::MalformedReply;
  else if(ack.serial!=sent||ack.player!=original.player||ack.body!=original.body||ack.request!=original.request)bad=Reason::IdentityMismatch;
  else if(ack.mask&~((tokens[0]?1u:0u)|(tokens[1]?2u:0u)))bad=Reason::UnexpectedSide;
  else if(((nativeRest||restProtocol)&&!ack.restProtocol)||(!ack.mask&&ack.ackSeq!=sent))bad=Reason::MissingTerminalAck;
  if(bad!=Reason::None){fault=true;ready.store(false,std::memory_order_release);stage=Stage::Fault;reason=faultReason=bad;return;}
  // Publish only proven acquisition, before any optional presentation callback.
  // A failed/unknown reply above preserves may-own debt without claiming a door.
  for(int s=0;s<2;++s)if(tokens[s]&&(ack.mask&(1u<<s)))
   acquiredD49[s].store(tokens[s],std::memory_order_release);
  // Only the versioned synchronous reply can transfer a released hand to
  // bounded Lua native-operation debt. Old OK replies cannot forgive a rest.
  restProtocol=ack.restProtocol;resting.store(ack.resting,std::memory_order_release);
  // A separate read-only reply describes this same native operation. Its
  // failure retires only presentation; it never changes the accepted t2 ACK.
  if constexpr(requires{io.display(in,out,sizeof(out));}){
   if(displayEnabled.load(std::memory_order_acquire)&&drive&&(ack.mask&~ack.retiring)){
    Display d{};
    if(io.display(in,out,sizeof(out))&&ParseDisplay(out,d)&&d.serial==sent&&d.source.player==original.player&&
       d.source.body==original.body&&d.source.request==original.request&&!(d.mask&~(ack.mask&~ack.retiring))){
     bool exact=true;for(int s=0;s<2;++s)if(d.mask&(1u<<s))exact=exact&&d.token[s]==tokens[s]&&d.door[s]==ack.door[s];
     if(exact){d.source=latest;d.tick=now;displayPublished=display.publish(d);}
    }
   }
  }
  retiring=ack.retiring;stage=ack.mask?(ack.retiring?Stage::Retiring:Stage::Retained):(ack.resting?Stage::Rested:Stage::Cleared);
  reason=ack.mask?Reason::LuaPendingOrOwned:(ack.resting?Reason::PartialRest:Reason::Terminal);
  // H40R1 includes all uncertain/retiring HAND sides. A missing hand with
  // rest debt is an explicit detached-rest receipt, never native completion.
  // Legacy OK has no rest support and retains its original terminal contract.
  for(int s=0;s<2;++s)if(tokens[s]){
   if(!(ack.mask&(1u<<s))){
    // Only an initial exact successful feed which acquired no door may hand
    // this SAME original press back to the existing native interaction FSM.
    if(drive&&grip[s]>=0&&!doors[s]&&candidate[s].token==tokens[s]&&Identity(candidate[s].original,original)&&
       fallback[s].publish({candidate[s]}))fallbackToken[s].store(tokens[s],std::memory_order_release);
    entered[s].store(0,std::memory_order_release);
    (void)RetireClaimH41(s,tokens[s]);doors[s]=0;
   }else doors[s]=ack.door[s];
  }
  if(!ack.mask&&!ack.resting){fault=false;faultReason=Reason::None;identity=false;original={};}
 }
};
inline bool TakeFallback(int side,const Frame&current,std::uint64_t now,Candidate&out) noexcept {
 if(side<0||side>1)return false;const auto token=fallbackToken[side].exchange(0,std::memory_order_acq_rel);Fallback value{};
 if(!token||!fallback[side].read(value)||value.candidate.token!=token||
    !Identity(value.candidate.original,current)||!Current(current,side,now)||current.grip[side]<.30f||
    !Fresh(value.candidate.tick,now,300))return false;
 out=value.candidate;return true;
}
inline ServiceState service;
inline bool QuarantineLiftableR96(std::uint64_t now) noexcept {
 Frame q{};const bool copied=completed.read(q);now=ObserveClockR96(now);return copied&&Valid(q,now)&&(q.player!=quarantinePlayerR96.load(std::memory_order_acquire)||
  q.body!=quarantineBodyR96.load(std::memory_order_acquire));
}
inline bool Wanted(std::uint64_t now) noexcept {if(vmQuarantine.load(std::memory_order_acquire))return QuarantineLiftableR96(now);return (Reserved()||resting.load(std::memory_order_acquire)||(enabled.load(std::memory_order_acquire)&&!ready.load(std::memory_order_acquire)&&Fresh(nearTick.load(std::memory_order_acquire),now,120)));}
bool Install() noexcept;
}
