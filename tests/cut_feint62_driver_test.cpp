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
#include "subsystems/direct/tutorial_native_r159.h" // R159: SuppressNativeFollowup18 in a regenerated state_driver5912.inc reads the tutorial gate
#include "subsystems/direct/training_vanilla_r163.h" // R163: a regenerated state driver reads the training-fight gate (DIRBLOCK / defence timing / NR1)
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
static float F(const void* s,unsigned p){return hostFloats[{s,p}];}
template<class T>T Rd(const void*,std::size_t p,T d){if(p==0xCE0)return reinterpret_cast<T>(actor);if(p==0x1F8)return reinterpret_cast<T>(player);return d;}
constexpr unsigned kActionPhaseProp39=0x30,kAttackType=0x210,kAttackStrength=0x1e0,kPendingZone=0x120,kAttackZone=0xf0,kCommittedZone=0x180,kExecutedZone=0x270,kAttackingHand=0x1b0,kDefenseZone=0x150;
static bool hostRowAvailable=true,hostQueryTakeover=false,hostRowMinusOne=false,hostRowFallback=false; // R12: fallback case publishes without a defense write
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
  if(!hostRowFallback)assert(I(hs,kDefenseZone)==(hostRowMinusOne?-1:(I(hs,kCommittedZone)==5?0:1)));
  assert(F(hs,0x450)>=0.f&&F(hs,0x450)<=.600001f); // expired/missing predictions stay zero
 }
}
static bool hostGuardOwned=false; // R59.16C10R16: our posture guard holds the block input
namespace frik_pose_probe {static bool GuardOwnedByUs59c7e(const void*){return hostGuardOwned;}static int GeometryOwnerStage49(){return 0;}static bool GeometryOwnerValid178r30(std::uint64_t,std::uint64_t,std::uint64_t,std::uint64_t){return hostOwner;}}
namespace diagnostics::capture168 {enum class Domain {Combat};static bool active(Domain,std::uint64_t){return hostCombatCapture;}}
namespace diagnostic_trace {static std::string last,offerRetired;static void Write(const char* s){last=s;if(last.find("FORWARD_OFFER_RETIRED")!=std::string::npos)offerRetired=last;}}
// This extracted state-controller fixture has no live native actor graph.
// Fail closed at these external leaves; quiet_hit_feedback168 exercises the complete
// production follow-up ownership helper with an explicit actor/opponent fixture.
static bool CombatActor(const void*,const void*&,const void*&,bool) noexcept {return false;}
static bool IsPlayer(const void*) noexcept {return false;}
static const void* hostNativeAction=nullptr;
static const void* LiveAttackAction178r35(const void*) noexcept {return hostNativeAction;}
#define KCD2VR_CUT_FEINT62_TEST
static unsigned phaseWrites62=0,clockWrites62=0,strengthWrites62=0,typeWrites62=0;
static void HostObserveWrite62(unsigned prop,bool){phaseWrites62+=prop==kActionPhaseProp39;clockWrites62+=prop==0x450;
 strengthWrites62+=prop==kAttackStrength;typeWrites62+=prop==kAttackType;}
static int npcPhase62=1,npcZone62=-1;static bool npcAvailable62=true;
static const void* npc62=reinterpret_cast<void*>(0x6200);
static const void* HostReciprocal62(const void*& state,bool& reciprocal){state=npc62;reciprocal=npcAvailable62;return npcAvailable62?npc62:nullptr;}
static bool HostNpcDefense62(const void* target,int& phase,int& zone,bool& henry){phase=npcPhase62;zone=npcZone62;henry=npcAvailable62&&target==npc62;return henry;}
#include "state_driver5912.inc"
int main(){
 namespace mo=kcd2vr::motion59c4;namespace vs=vrstate596;
 int value62=0;const void* hs=&value62;mo::selected=true;vs::g_consequence=true;
 vs::g_telegraph_strike59c10=false; // a current same-stage prepared offer; no forced phase8
 auto clean62=[&](std::uint64_t now){hostNow=now;vs::Forget();mo::attempts.clear();vs::g_fault=false;
  hostInts.clear();hostFloats.clear();hostInts[{hs,kActionPhaseProp39}]=1;
  hostFloats[{hs,0x450}]=-1;hostFailProp=~0u;hostRowAvailable=true;hostRowMinusOne=false;
  npcAvailable62=true;npcPhase62=1;npcZone62=-1;
 };
 auto event62=[&](const mo::Key& k,mo::Event event,int kind,int zone,std::uint64_t now){hostNow=now;
  assert(mo::attempts.event(k,event,now,now,zone,0));assert(vs::Event(k,now,kind,zone,0));vs::Tick(hs,now);
 };
 mo::Key key{620,77,4,1,1,kcd2vr::contact176::recoveryEpoch55c1.load(),std::uintptr_t(player)};
 clean62(1000);event62(key,mo::Event::Prepare,1,2,1000);
 assert(vs::g_stage==4&&vs::g_dirblock39.plan.active);const auto deadline62=vs::g_deadline;
 const auto prediction62=vs::g_prediction19c3;const auto phases62=hostPhasePublications,effort62=hostEffortCalls;
 const auto pw62=phaseWrites62,cw62=clockWrites62,sw62=strengthWrites62,tw62=typeWrites62;
 const auto strength62=F(hs,kAttackStrength);
 hostNow=1060;assert(mo::attempts.event(key,mo::Event::Feint,hostNow,hostNow,0,0));
 assert(vs::DirectCutFeint62({key,hostNow,6,0,0},hs,hostNow));
 assert(vs::g_stage==4&&I(hs,kAttackZone)==0&&I(hs,kCommittedZone)==0&&I(hs,kDefenseZone)==1&&
  vs::g_dirblock39.plan.zone==0&&vs::g_dirblock39.plan.cutRefined62&&vs::g_dirblock39.plan.frozen);
 assert(hostPhasePublications==phases62&&hostEffortCalls==effort62&&F(hs,kAttackStrength)==strength62&&
  phaseWrites62==pw62&&clockWrites62==cw62&&strengthWrites62==sw62&&typeWrites62==tw62&&
  vs::g_deadline==deadline62&&vs::g_prediction19c3.onset==prediction62.onset&&vs::g_prediction19c3.deadline==prediction62.deadline);
 event62(key,mo::Event::Commit,2,0,1080);assert(vs::g_stage==8&&I(hs,kExecutedZone)==0);
 std::puts("C62 actual DIRECT Tick/Publish native row ACK retains original prediction/phase/effort/strength and commit keeps refined sector");
 ++key.gesture;clean62(1500);event62(key,mo::Event::Prepare,1,2,1500);
 const auto tickDeadline62=vs::g_deadline,tickOnset62=vs::g_prediction19c3.onset;
 const auto tickPhases62=phaseWrites62,tickStrength62=strengthWrites62,tickTypes62=typeWrites62,tickClock62=clockWrites62;
 event62(key,mo::Event::Feint,6,0,1560);
 assert(vs::g_stage==4&&vs::g_zone==0&&vs::g_dirblock39.plan.zone==0&&
  phaseWrites62==tickPhases62&&strengthWrites62==tickStrength62&&typeWrites62==tickTypes62&&
  clockWrites62==tickClock62+1&&vs::g_deadline==tickDeadline62&&vs::g_prediction19c3.onset==tickOnset62);
 // The ONE clock write is the pre-existing end-of-Tick countdown servicing,
 // not Publish's zone-only branch (the direct helper above proves zero there).
 std::puts("C62 actual queued kind6 Tick updates sector; only ordinary countdown servicing writes the clock");
 // Block already committed, unread NPC, an attacking NPC, a pending native offer,
 // duplicate refinement, expired original preparation and missing row each cancel
 // this exact attempt; a queued Commit cannot recover it under the old zone.
 for(int bad=0;bad<7;++bad){++key.gesture;const std::uint64_t start=2000+1000*bad;clean62(start);
  event62(key,mo::Event::Prepare,1,2,start);assert(vs::g_stage==4);
  if(bad==0){npcPhase62=128;npcZone62=2;}
  if(bad==1)npcAvailable62=false;
  if(bad==2)npcPhase62=8;
  if(bad==3)vs::g_telegraph_offer.pending=true;
  if(bad==4)vs::g_dirblock39.plan.cutRefined62=true;
  if(bad==5)hostRowAvailable=false;
  const auto turn=start+(bad==6?101:60);event62(key,mo::Event::Feint,6,0,turn);
  assert(vs::g_stage==0&&!mo::attempts.publicationAllowed(key,mo::Event::Feint,turn,turn));
  hostNow=turn+1;assert(!mo::attempts.event(key,mo::Event::Commit,hostNow,hostNow,0,0));
  assert(vs::Event(key,hostNow,2,0,0));vs::Tick(hs,hostNow);assert(vs::g_stage==0);
  mo::Attempt attempt{};assert(mo::attempts.claim(key,turn,hostNow,777,attempt)==mo::Claim::Cancelled);
 }
 // Existing retired rear-cue event must remain inadmissible.
 assert(!vs::Event(key,hostNow,5,2,0));
 std::puts("C62 actual DIRECT refused native rows/committed NPC defense/unknown/pending/repeated/expired feints stay cancelled; kind5 retired PASS");
}
