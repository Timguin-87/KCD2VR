#include <array>
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <map>
#include <mutex>
#include <cassert>
#include <limits>
#include <string>
#include <thread>
#include "vrik/motion_owned59c4.h"
#include "vrik/contact_authority176.h"
static std::uint64_t hostNow=1000;
static bool hostVerified=true,hostOwner=true,hostEffortOK=true;
static unsigned hostFailProp=~0u;
static unsigned hostExceptionProp=~0u,hostMismatchProp=~0u;
static int hostMismatchInt=0;static float hostMismatchFloat=0.f;
static bool hostMismatchOnce=false;
static unsigned hostPhasePublications=0;
static unsigned hostEffortCalls=0;
static unsigned hostOfferReads=0,hostOfferReadMask=7;
static bool hostCombatCapture=false;
std::uint64_t GetTickCount64(){return hostNow;}
std::map<std::pair<const void*,unsigned>,int> hostInts;
std::map<std::pair<const void*,unsigned>,float> hostFloats;
static const void* actor=reinterpret_cast<void*>(0x2000);
static const void* player=reinterpret_cast<void*>(0x1000);
static int I(const void* s,unsigned p){return hostInts[{s,p}];}
static unsigned failClockReads=0;
static float F(const void* s,unsigned p){if(p==0x450 && failClockReads){--failClockReads;return -99.f;}return hostFloats[{s,p}];}
template<class T>T Rd(const void*,std::size_t p,T d){if(p==0xCE0)return reinterpret_cast<T>(actor);if(p==0x1F8)return reinterpret_cast<T>(player);return d;}
constexpr unsigned kActionPhaseProp39=0x30,kAttackType=0x210,kAttackStrength=0x1e0,kPendingZone=0x120,kAttackZone=0xf0,kCommittedZone=0x180,kExecutedZone=0x270,kAttackingHand=0x1b0,kDefenseZone=0x150;
static bool hostRowAvailable=true,hostQueryTakeover=false,hostRowMinusOne=false;
static unsigned hostQueries=0;
static bool HostResolveDefense(const void* hs,int committed,const void*& row,int& value){
 ++hostQueries;assert(I(hs,kAttackZone)==committed&&I(hs,kCommittedZone)==committed);
 if(hostQueryTakeover){hostInts[{hs,kActionPhaseProp39}]=32;hostInts[{hs,kDefenseZone}]=42;}
 row=hostRowAvailable?reinterpret_cast<void*>(std::uintptr_t(0x6000+committed*0x100)):nullptr;
 value=hostRowMinusOne?-1:(committed==5?0:1);return hostRowAvailable;
}
static void HostWatchPublication(const void* hs,unsigned prop,int value){
 if(prop==kActionPhaseProp39&&(value==4||value==8)){
  ++hostPhasePublications;
  assert(I(hs,kDefenseZone)==(hostRowMinusOne?-1:(I(hs,kCommittedZone)==5?0:1)));
  assert(F(hs,0x450)>=0.f&&F(hs,0x450)<=.600001f);
 }
}
namespace frik_pose_probe {static bool GuardOwnedByUs59c7e(const void*){return false;}static int GeometryOwnerStage49(){return 0;}static bool GeometryOwnerValid178r30(std::uint64_t,std::uint64_t,std::uint64_t,std::uint64_t){return hostOwner;}}
namespace diagnostics::capture168 {enum class Domain {Combat};static bool active(Domain,std::uint64_t){return hostCombatCapture;}}
namespace diagnostic_trace {static std::string last,offerRetired;static void Write(const char* s){last=s;if(last.find("FORWARD_OFFER_RETIRED")!=std::string::npos)offerRetired=last;}}
// This extracted state-controller fixture has no live native actor graph.
// Fail closed at these external leaves; quiet_hit_feedback168 exercises the complete
// production follow-up ownership helper with an explicit actor/opponent fixture.
static bool CombatActor(const void*,const void*&,const void*&,bool) noexcept {return false;}
static bool IsPlayer(const void*) noexcept {return false;}
static const void* hostNativeAction=nullptr;
static const void* LiveAttackAction178r35(const void*) noexcept {return hostNativeAction;}
#include "state_driver5912.inc"

int main(){
 namespace mo=kcd2vr::motion59c4;namespace vs=vrstate596;
 int memory=0;const void* hs=&memory;
 auto reset=[&](){hostFailProp=hostExceptionProp=hostMismatchProp=~0u;hostVerified=hostOwner=hostEffortOK=true;
   vs::g_fault=false;vs::g_setter_exception=false;vs::g_reset=false;vs::Forget();mo::attempts.clear();
   hostInts.clear();hostFloats.clear();hostInts[{hs,kActionPhaseProp39}]=1;
   hostRowAvailable=true;hostQueryTakeover=hostRowMinusOne=false;failClockReads=0;
   mo::selected=true;vs::g_consequence=true;vs::g_eta_prepare59c10=.25f;vs::g_eta_strike59c10=.12f;
   vs::g_npc_block_speed18=1.f; // compatibility slider is not a direct-mode clock switch
   vs::g_prediction_history19c3={};vs::g_telegraph_strike59c10=false;hostNativeAction=nullptr;
   {std::lock_guard<std::mutex> lock(vs::g_ev_mutex);vs::g_ev_count=0;}
 };
 reset();
 mo::Key a{101,77,4,1,1,kcd2vr::contact176::recoveryEpoch55c1.load(),std::uintptr_t(player)},b=a;b.gesture=102;
 auto event=[&](mo::Key key,mo::Event e,int kind,int zone,int technique,std::uint64_t now){hostNow=now;
   assert(mo::attempts.event(key,e,now,now,zone,technique));assert(vs::Event(key,now,kind,zone,technique));vs::Tick(hs,now);};
 event(a,mo::Event::Prepare,1,2,0,1000);event(a,mo::Event::Commit,2,2,0,1010);
 event(b,mo::Event::Prepare,1,5,1,1050);event(b,mo::Event::Commit,2,5,1,1060);
 const auto bPrediction=vs::g_prediction19c3;const float bEta=F(hs,0x450);
 hostNow=1100;mo::Attempt claim;assert(mo::attempts.claim(a,1040,hostNow,55,claim)==mo::Claim::Ready);
 {vs::ContactScope5912 scope;assert(scope.begin(hs,claim));assert(scope.current());scope.afterHit();}
 const float restored=F(hs,0x450);const auto restoredPrediction=vs::g_prediction19c3;
 assert(vs::g_gesture==b.gesture);
 hostNow=1110;vs::Tick(hs,hostNow);const float next=F(hs,0x450);
 std::printf("A/B prediction: B began1050 deadline=%llu commit1060=%.3f; A contact1100 restoresB=%.3f next1110=%.3f\n",
   (unsigned long long)bPrediction.deadline,bEta,restored,next);
 assert(bPrediction.onset==1050&&bPrediction.deadline==1300);
 assert(mo::same(restoredPrediction.key,b)&&restoredPrediction.onset==bPrediction.onset&&restoredPrediction.deadline==bPrediction.deadline);
 assert(std::fabs(restored-.200f)<.0001f&&std::fabs(next-.190f)<.0001f);
 // The original native clock has a valid value, but its FIRST baseline read fails.
 reset();hostNow=5000;hostFloats[{hs,0x450}]=.42f;failClockReads=1;
 vs::g_key5912=a;vs::g_gesture=a.gesture;
 const auto publications=hostPhasePublications;
 const bool opened=vs::Publish(hs,4,2,0,5600);assert(!opened);
 const float guessedBaseline=vs::g_base_clock;
 hostNow=5010;const bool closed=vs::Publish(hs,0,-1,0);
 std::printf("Unreadable clock baseline: actualBefore=.420 firstRead=UNREADABLE sentinel=-99 opened=%d savedBaseline=%.3f closed=%d final=%.3f fault=%d\n",opened,guessedBaseline,closed,F(hs,0x450),bool(vs::g_fault));
 assert(!closed&&F(hs,0x450)==.42f&&!vs::g_fault&&hostPhasePublications==publications);
 // Demonstrate separate lifetime and ETA fields, not a simulated native outcome.
 reset();hostNow=7000;vs::g_key5912=a;vs::g_gesture=a.gesture;
 assert(mo::attempts.event(a,mo::Event::Prepare,hostNow,hostNow,2,0));
 assert(vs::Publish(hs,4,2,0,7600));hostNow=7100;assert(vs::Publish(hs,4,5,1,7600));
 std::printf("Same-attempt rezone: attemptDeadline=%llu predictionDeadline=%llu remaining=%.3f; measured=0\n",
   (unsigned long long)vs::g_deadline,(unsigned long long)vs::g_prediction19c3.deadline,F(hs,0x450));
 assert(vs::g_deadline==7600&&vs::g_prediction19c3.deadline==7250&&std::fabs(F(hs,0x450)-.150f)<.0001f);
 // Original countdown reaches zero without a floor; a future onset is never valid.
 assert(std::fabs(vs::PredictionRemaining19c3(vs::g_prediction19c3,7249)-.001f)<.0001f);
 assert(vs::PredictionRemaining19c3(vs::g_prediction19c3,7250)==0.f);
 assert(vs::PredictionRemaining19c3(vs::g_prediction19c3,6999)==0.f);
 hostNow=7250;vs::Tick(hs,hostNow);assert(F(hs,0x450)==0.f&&vs::g_stage==4);
 hostNow=7601;vs::Tick(hs,hostNow);assert(vs::g_stage==0&&!vs::g_clock_owned);
 // Retired stage/speed sliders cannot restore the old constant clock or disable
 // the new direct-only prediction. Changes during a gesture do not re-seed it.
 for(const float k:{0.f,.5f,1.f,1.5f,9.f}){
  reset();vs::g_npc_block_speed18=k;hostNow+=1000;const auto onset=hostNow;
  event(a,mo::Event::Prepare,1,2,0,onset);assert(std::fabs(F(hs,0x450)-.25f)<.0001f);
  vs::g_eta_prepare59c10=.60f;vs::g_eta_strike59c10=.60f;vs::g_npc_block_speed18=3.f;
  event(a,mo::Event::Commit,2,2,0,onset+20);
  assert(vs::g_prediction19c3.deadline==onset+250&&std::fabs(F(hs,0x450)-.23f)<.0001f);
  hostNow=onset+249;vs::Tick(hs,hostNow);assert(std::fabs(F(hs,0x450)-.001f)<.0001f);
  hostNow=onset+400;vs::Tick(hs,hostNow);assert(F(hs,0x450)==0.f);
  assert(vs::Publish(hs,0,-1,0)&&F(hs,0x450)==0.f); // original baseline was zero
 }
 // Native comparison never enters this service: preserve its native time/phase.
 reset();mo::selected=false;hostNow+=1000;hostFloats[{hs,0x450}]=.42f;
 const auto beforeComparison=hostPhasePublications;
 assert(vs::Event(a,hostNow,1,2,0));vs::Tick(hs,hostNow);
 assert(vs::g_stage==0&&!vs::g_clock_owned&&F(hs,0x450)==.42f&&hostPhasePublications==beforeComparison);
 // A real admitted ordinary action owns its native ETA even with direct selected.
 reset();hostNow+=1000;hostNativeAction=actor;hostFloats[{hs,0x450}]=.42f;
 const auto beforeNative=hostPhasePublications;
 event(a,mo::Event::Prepare,1,2,0,hostNow);
 assert(vs::g_stage==0&&!vs::g_clock_owned&&F(hs,0x450)==.42f&&hostPhasePublications==beforeNative);
 // Native timing takeover of a current lease is not adopted or overwritten.
 reset();hostNow+=1000;event(a,mo::Event::Prepare,1,2,0,hostNow);
 hostFloats[{hs,0x450}]=.37f;hostNow+=10;vs::Tick(hs,hostNow);
 assert(vs::g_stage==0&&!vs::g_clock_owned&&F(hs,0x450)==.37f&&mo::attempts.beginForKey(a,hostNow)==0);
 std::puts("Actual-driver ETA regression: A/B restore, unreadable baseline, onset-only countdown, no floor/re-seed, comparison/native-action isolation and native takeover passed; native leaves substituted.");return 0;
}
