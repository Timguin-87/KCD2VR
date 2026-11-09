// C4R3: the FOLLOWUP_C evaluation must survive synchronous phase publication.
// Only an application request is subject to the existing exact owner barrier.
// Exercise the production state driver for forward, late and borrowed contact.
#include <array>
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <map>
#include <mutex>
#undef NDEBUG
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
static const void* rpgObj=reinterpret_cast<void*>(0x3000);
static const void* npc=reinterpret_cast<void*>(0x4000);
static const void* requestObj=reinterpret_cast<void*>(0x5000);
static int I(const void* s,unsigned p){return hostInts[{s,p}];}
static float F(const void* s,unsigned p){return hostFloats[{s,p}];}
// Rd model of the retail links used by the refusal: actor+0x388=rpg, rpg+8=actor, state+0xCF8=npc, request+0x10=npc
template<class T>T Rd(const void* b,std::size_t p,T d){
 if(p==0xCE0)return reinterpret_cast<T>(actor);if(p==0x1F8)return reinterpret_cast<T>(player);
 if(b==actor&&p==0x388)return reinterpret_cast<T>(rpgObj);if(b==rpgObj&&p==8)return reinterpret_cast<T>(actor);
 if(p==0xCF8)return reinterpret_cast<T>(npc);if(b==requestObj&&p==0x10)return reinterpret_cast<T>(npc);return d;}
constexpr unsigned kActionPhaseProp39=0x30,kAttackType=0x210,kAttackStrength=0x1e0,kPendingZone=0x120,kAttackZone=0xf0,kCommittedZone=0x180,kExecutedZone=0x270,kAttackingHand=0x1b0,kDefenseZone=0x150;
static bool hostRowAvailable=true,hostQueryTakeover=false,hostRowMinusOne=false,hostRowFallback=false;
static unsigned hostQueries=0;
static bool HostResolveDefense(const void* hs,int committed,const void*& row,int& value){
 ++hostQueries;row=reinterpret_cast<void*>(std::uintptr_t(0x6000+committed*0x100));value=committed==5?0:1;return true;}
static void (*g_followupProbe)(const void*)=nullptr;
static void HostWatchPublication(const void* hs,unsigned prop,int value){
 // retail 93A0C8 stores the new value, THEN notifies synchronously (910E9C); the NPC's reaction
 // reaches 1483580 -> C55BB4 -> 173BD74 -> D568B0 inside this call (R14B log ordering).
 if(prop==kActionPhaseProp39&&value==8&&I(hs,kActionPhaseProp39)!=8&&g_followupProbe){hostInts[{hs,prop}]=value;g_followupProbe(hs);}
}
static bool hostGuardOwned=false;
namespace frik_pose_probe {static bool GuardOwnedByUs59c7e(const void*){return hostGuardOwned;}static int GeometryOwnerStage49(){return 0;}static bool GeometryOwnerValid178r30(std::uint64_t,std::uint64_t,std::uint64_t,std::uint64_t){return hostOwner;}}
namespace diagnostics::capture168 {enum class Domain {Combat};static bool active(Domain,std::uint64_t){return hostCombatCapture;}}
namespace diagnostic_trace {static std::string last,offerRetired;static void Write(const char* s){last=s;if(last.find("FORWARD_OFFER_RETIRED")!=std::string::npos)offerRetired=last;}}
static bool CombatActor(const void* a,const void*& e,const void*& s,bool) noexcept;
static bool IsPlayer(const void* e) noexcept {return e==player;}
static const void* LiveAttackAction178r35(const void*) noexcept {return nullptr;}
static const void* g_hs=nullptr;
static bool CombatActor(const void* a,const void*& e,const void*& s,bool) noexcept {if(a!=actor)return false;e=player;s=g_hs;return true;}
#include "state_driver5912.inc"
static int lastRefused=-1;static const char* lastWhere="";
static void Probe(const void* hs){
 namespace vs=vrstate596;
 const int type=I(hs,kAttackType); // C55C74..C55C8A: edx = Henry state +0x210 getter
 const auto own=vs::QueryAttackOwnership(vs::g_key5912,hostNow);
 assert(!vs::SuppressNativeFollowup18(rpgObj,0x173BF58,requestObj,false,type));
 lastRefused=vs::SuppressNativeFollowup18(rpgObj,0x173BF58,requestObj,true,type)?1:0;
 std::printf("  [%s] phase-8 edge inside SetInt: snapshotStage=%d gTechnique=%d nativeType=%d ownership=%s -> evaluation=FORWARDED application=%s\n",
   lastWhere,vs::g_attack_snapshot.stage.load(),vs::g_technique.load(),type,
   own==kcd2vr::motion59c4::AttackOwnership::Owned?"Owned":own==kcd2vr::motion59c4::AttackOwnership::None?"None":"Unknown",
   lastRefused?"REFUSED":"FORWARDED (native parry-in-place runs)");
}
int main(){
 namespace mo=kcd2vr::motion59c4;namespace vs=vrstate596;
 int dummy=0;const void* hs=&dummy;g_hs=hs;mo::selected=true;vs::g_consequence=true;vs::g_own_hits18=true;vs::g_henry_actor=actor;
 vs::g_npc_block_speed18=1.f;
 g_followupProbe=Probe;
 hostInts[{hs,kActionPhaseProp39}]=1;
 mo::Key a{11,77,4,1,1,kcd2vr::contact176::recoveryEpoch55c1.load(),std::uintptr_t(player)};
 auto event=[&](const mo::Key& k,mo::Event e,int kind,int z,int tech,std::uint64_t t,bool rec=false){hostNow=t;
  assert(mo::attempts.event(k,e,t,t,z,tech,rec));assert(vs::Event(k,t,kind,z,tech));vs::Tick(hs,t);};
 // H21: preparation and the next Tick no longer manufacture a strike edge.
 // Exercise the synchronous exact-owner barrier at the real Commit instead.
 vs::g_telegraph_strike59c10=true;lastRefused=-1;
 lastWhere="prepare";event(a,mo::Event::Prepare,1,2,0,1000);hostNow=1016;vs::Tick(hs,1016);
 assert(lastRefused==-1&&vs::g_stage==4);
 lastWhere="real Commit";event(a,mo::Event::Commit,2,2,0,1020);assert(lastRefused==1);
 // S2 late contact after the recognizer's own abort publishes its own strike
 lastRefused=-1;event(a,mo::Event::Cancel,4,-1,0,1100,true);
 mo::Attempt claimed{};hostNow=1140;
 auto r=mo::attempts.claim(a,1140,1140,55,claimed);if(r==mo::Claim::Wait)r=mo::attempts.contact(a,1140,1140,55,0,claimed);
 assert(r==mo::Claim::Ready);
 lastWhere="late contact";{vs::ContactScope5912 scope;assert(scope.begin(hs,claimed));}assert(lastRefused==1);
 // Evaluation remains allowed after contact; an unexpected application still
 // cannot independently apply our exact same owned direct announcement.
 {vs::g_hit_at5910c1=hostNow;const int type=I(hs,kAttackType);
  assert(I(hs,kActionPhaseProp39)==8&&vs::g_stage.load()==8);
  assert(!vs::SuppressNativeFollowup18(rpgObj,0x173BF58,requestObj,false,type));
  assert(vs::SuppressNativeFollowup18(rpgObj,0x173BF58,requestObj,true,type)&&vs::g_followup_after_hit19.load()==1);
  hostNow+=250;assert(!vs::SuppressNativeFollowup18(rpgObj,0x173BF58,requestObj,true,type));
  vs::g_hit_at5910c1=0;}
 // S3 contact-led strike (preparation never published)
 for(int i=0;i<50;++i){hostNow=1400+i*20;vs::Tick(hs,hostNow);}
 mo::Key c=a;c.gesture=12;hostNow=2000;assert(mo::attempts.event(c,mo::Event::Prepare,2000,2000,1,0));
 hostNow=2050;r=mo::attempts.contact(c,2050,2050,55,0,claimed);assert(r==mo::Claim::Ready);
 lastRefused=-1;lastWhere="contact-led";{vs::ContactScope5912 scope;assert(scope.begin(hs,claimed));}assert(lastRefused==1);
 // A live exact owner inside our synchronous phase write is authority only on
 // this thread. Publish a stable no-lease snapshot to isolate the TLS path;
 // the positive on this thread must still refuse the application request.
 {const int oldStage=vs::g_stage.load(),nativeType=I(hs,kAttackType);vs::g_stage=0;vs::SnapshotAttack(hostNow);
  vs::OwnPhaseWriteScope19 own(hs,nativeType);
  assert(vs::SuppressNativeFollowup18(rpgObj,0x173BF58,requestObj,true,nativeType));
  assert(!vs::SuppressNativeFollowup18(rpgObj,0x173BF58,requestObj,false,nativeType));
  assert(!vs::SuppressNativeFollowup18(rpgObj,0x173BF58,requestObj,true,2)); // native kick
  assert(!vs::SuppressNativeFollowup18(rpgObj,0x173BF60,requestObj,true,nativeType)); // foreign caller
  bool other=true;std::thread t([&]{other=vs::SuppressNativeFollowup18(rpgObj,0x173BF58,requestObj,true,nativeType);});t.join();
  assert(!other);vs::g_stage=oldStage;vs::SnapshotAttack(hostNow);}
 // Outside any attempt even an application request is forwarded.
 for(int i=0;i<50;++i){hostNow=2100+i*20;vs::Tick(hs,hostNow);}
 assert(vs::g_stage.load()==0);
 hostInts[{hs,kActionPhaseProp39}]=8;hostInts[{hs,kAttackType}]=0;
 assert(!vs::SuppressNativeFollowup18(rpgObj,0x173BF58,requestObj,true,0));
 {vs::OwnPhaseWriteScope19 own(hs,0);
  assert(!vs::SuppressNativeFollowup18(rpgObj,0x173BF58,requestObj,true,0));} // no current owned attempt
 hostInts[{hs,kActionPhaseProp39}]=1;hostInts[{hs,kAttackType}]=-1;
 // S4 contact refresh: the published attempt lost its phase to Henry's own guard
 mo::Key d=a;d.gesture=13;lastWhere="refresh-prepare";event(d,mo::Event::Prepare,1,2,0,3000);
 hostNow=3005;assert(mo::attempts.event(d,mo::Event::Commit,3005,3005,2,0));
 hostInts[{hs,kActionPhaseProp39}]=2;
 hostNow=3010;r=mo::attempts.claim(d,3010,3010,55,claimed);
 lastRefused=-1;lastWhere="contact refresh";{vs::ContactScope5912 scope;assert(scope.begin(hs,claimed));}
 assert(lastRefused==1&&vs::g_contact_refreshed18.load()>=1);
 // S5 commit edge with the technique reclassified (prepare CUT, commit THRUST) without the early strike phase
 for(int i=0;i<50;++i){hostNow=3100+i*20;vs::Tick(hs,hostNow);}
 vs::g_telegraph_strike59c10=false;hostInts[{hs,kActionPhaseProp39}]=1;
 mo::Key e=a;e.gesture=14;lastWhere="prepare";event(e,mo::Event::Prepare,1,2,0,5000);
 lastRefused=-1;lastWhere="commit tech change";event(e,mo::Event::Commit,2,5,1,5030);assert(lastRefused==1);
 assert(vs::g_followup_in_write19.load()>=4);
 std::puts("C4R3 FOLLOWUP_C: evaluation forwarded inside phase writes and after contact; exact-owner application barrier retained; kicks/foreign callers/threads unchanged.");
 return 0;
}
