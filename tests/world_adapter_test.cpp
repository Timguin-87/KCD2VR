// Executes current production adapter extracted by tools/extract_world_adapter_test.py.
// Memory reads and the native call are replaced. This is NOT Windows SEH/physics proof.
#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <chrono>
#include "../src/vrik/contact_budget.h"
#include "../src/vrik/station_hand_contact_d75.h"
#include "../src/vrik/held_motion_d67.h"
#include "../src/vrik/contact_projection44.h"
#include "../src/vrik/rigid_contact_d67r1.h"
#include "../src/experiments/contact_handover_d71.h"
#include "../src/vrik/contact_hold_r11.h"
#include "../src/vrik/contact_glide_w5.h"
#include "../src/vrik/contact_release_d45.h"
#include "../src/vrik/contact_balanced_d79.h"
#include "../src/vrik/smooth_lab_f2.h"
#include "../src/vrik/sweep_evidence169r2.h"
#include "../src/vrik/finger_surface.h"
#include "../src/vrik/knuckle_contact174.h"
#include "../src/dialogue_state59c10r2.h"
#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <map>
#include <limits>
#include <new>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>
#include "../src/vrik/world_query_contract.h"
#include "../src/diagnostics/passive_contact169r1.h"
#include "../src/diagnostics/pose_cost_d60.h"
#include "../src/diagnostics/lab_state.h"
#include "../src/subsystems/physics/touch_react_service60.h"
namespace diagnostics=kcd2vr::diagnostics;
static bool mockRetailLeaseD69=false;
namespace retail_build112 {static bool Ready(std::uintptr_t) noexcept {return mockRetailLeaseD69;}}
static std::atomic<int> g_weapon_control_mode{0};
static std::atomic<int> g_census_mode{0};
static int FocusedTestMode167() noexcept {return 0;}
#define __try try
#define __except(x) catch(...)
#define __fastcall
struct QuatT28 {float q[4];float t[3];};
#include "generated_adapter_types.h"
static constexpr std::uintptr_t base=0x100000000ull,world=0x180000000ull,character=0x180001000ull;
static std::map<std::uintptr_t,std::vector<unsigned char>> memory;
static std::vector<std::string> logs;
namespace diagnostic_trace {static void Write(const char*s){logs.emplace_back(s);}}
template<class T>static void Store(std::uintptr_t p,const T&v){auto& b=memory[p];b.resize(sizeof(v));std::memcpy(b.data(),&v,sizeof(v));}
template<class T>static bool Read(const void*p,T&v) noexcept {
 const auto a=reinterpret_cast<std::uintptr_t>(p);auto it=memory.upper_bound(a);if(it==memory.begin())return false;--it;
 if(a-it->first>it->second.size()||sizeof(T)>it->second.size()-(a-it->first))return false;
 std::memcpy(&v,it->second.data()+a-it->first,sizeof(T));return true;
}
static std::uintptr_t WhBase(){return base;}
static bool SupportedWhBuild(std::uintptr_t p){return p==base;}
static std::uint64_t mockMicros59c1=1000000,mockQueryUs59c1=0;
static int mockReplyAtH25=0,mockReplyModeH25=0;
static int mockCalls=0,mockMode=0;static bool argumentsOk=true;static float distance=.25f;
static bool mockBodies172=false;static void* mockCollider172=nullptr;
static bool mockPrepareNpcH9=false;static unsigned mockPrepareCallsH9=0;
static int mockPart172=0;static bool mockCornerParts55=false;static std::vector<V3> mockOrigins55;
static V3 wallPoint{.25f,0,0},wallNormal{-1,0,0};
static float spatialPatchYH25=.04761905f;
static V3 thinCenterH24{.1f,.1f,0};static float cornerXH24=.25f,cornerYH24=.25f;
static int MockRay(void*w,const V3* origin,const V3* sweep,int types,unsigned flags,void*hit,int maxHits,void*skip,int nskip,void*foreign,int foreignId,const char*){
 mockMicros59c1+=mockQueryUs59c1;
 ++mockCalls;argumentsOk=argumentsOk&&reinterpret_cast<std::uintptr_t>(w)==world&&maxHits==1&&!foreign&&!foreignId&&
   (mockBodies172?(types==0x11f&&flags==0x40f&&skip&&nskip==1&&*static_cast<void**>(skip)==reinterpret_cast<void*>(0x180003000ull)):
                  (types==0x101&&flags==0x8002040Fu&&!skip&&!nskip));
 const int replyH25=mockCalls==mockReplyAtH25?mockReplyModeH25:mockMode;
 std::memcpy(static_cast<unsigned char*>(hit)+8,&mockCollider172,sizeof(mockCollider172));
 mockOrigins55.push_back(*origin);std::memcpy(static_cast<unsigned char*>(hit)+0x14,&mockPart172,sizeof(mockPart172));
 if(replyH25==1)throw std::runtime_error("modeled native exception");
 if(replyH25==2){std::memcpy(hit,&distance,sizeof(distance));return 1;}
 if(replyH25==6){ // two independent measured planes: x=.25 and y=.25
  float best=2;V3 normal{};int part55=0;
  for(int axis=0;axis<2;++axis){const float from=axis?origin->y:origin->x,delta=axis?sweep->y:sweep->x;
   if(delta<=1e-7f)continue;const float t=((axis?cornerYH24:cornerXH24)-from)/delta;
   if(t>=0&&t<=1&&t<best){best=t;normal=axis?V3{0,-1,0}:V3{-1,0,0};part55=100+axis;}}
  if(best>1)return 0;const float dist=best*Len(*sweep);const auto p=Add(*origin,Mul(*sweep,best));
  std::memcpy(hit,&dist,4);std::memcpy(static_cast<unsigned char*>(hit)+0x24,&p,sizeof(p));
  std::memcpy(static_cast<unsigned char*>(hit)+0x30,&normal,sizeof(normal));
  if(mockCornerParts55)std::memcpy(static_cast<unsigned char*>(hit)+0x14,&part55,sizeof(part55));return 1;
 }
 if(mockMode==8){
  float nearest=2;V3 normal{-1,0,0};int chosen=-1;
  if(sweep->x<=1e-7f)return 0;
  for(int part=0;part<8;++part){const float x=part<7?.20f+.01f*part:.10f;
   const float parameter=(x-origin->x)/sweep->x;if(parameter<0||parameter>1||parameter>=nearest)continue;
   const V3 point=Add(*origin,Mul(*sweep,parameter));const float y=part<7?.02f*part:spatialPatchYH25;
   if(std::abs(point.y-y)>.0005f||std::abs(point.z)>.0005f)continue;
   nearest=parameter;chosen=part;
  }
  if(chosen<0)return 0;const float distance=nearest*Len(*sweep);const auto point=Add(*origin,Mul(*sweep,nearest));
  const int part=200+chosen;std::memcpy(static_cast<unsigned char*>(hit)+0x14,&part,4);
  if(chosen==7){void* collider=reinterpret_cast<void*>(0x180007000ull);std::memcpy(static_cast<unsigned char*>(hit)+8,&collider,sizeof(collider));}
  std::memcpy(hit,&distance,4);std::memcpy(static_cast<unsigned char*>(hit)+0x24,&point,sizeof(point));
  std::memcpy(static_cast<unsigned char*>(hit)+0x30,&normal,sizeof(normal));return 1;
 }
 if(replyH25==7){
  if(std::abs(sweep->x)<1e-7f)return 0;const float t=(thinCenterH24.x-origin->x)/sweep->x;
  if(t<0||t>1)return 0;const V3 p=Add(*origin,Mul(*sweep,t));
  if(std::abs(p.y-thinCenterH24.y)>.005f||std::abs(p.z-thinCenterH24.z)>.005f)return 0;
  const float distanceH24=t*Len(*sweep);V3 normalH24{-1,0,0};
  std::memcpy(hit,&distanceH24,4);std::memcpy(static_cast<unsigned char*>(hit)+0x24,&p,sizeof(p));
  std::memcpy(static_cast<unsigned char*>(hit)+0x30,&normalH24,sizeof(normalH24));return 1;
 }
 if(replyH25==4||replyH25==5){
  const float n=Len(*sweep),den=Dot(wallNormal,*sweep),a=Dot(wallNormal,Sub(*origin,wallPoint));
  if(std::abs(den)<1e-7f)return 0;const float t=-a/den;if(t<0||t>1)return 0;
  const float dist=t*n;V3 point=Add(*origin,Mul(*sweep,t));V3 normal=replyH25==5?V3{}:wallNormal;
  std::memcpy(hit,&dist,4);std::memcpy(static_cast<unsigned char*>(hit)+0x24,&point,sizeof(point));std::memcpy(static_cast<unsigned char*>(hit)+0x30,&normal,sizeof(normal));return 1;
 }
 if(replyH25==3)return 2;
 return 0;
}
#include "generated_world_adapter.inc"
// The entity/equipment owner is outside this extracted adapter. Its bounded
// proxy-map reader is tested in npc_contact172_test; never call game code here.
static bool PrepareNpcQuery172() noexcept {
 ++mockPrepareCallsH9;
 // Explicit entity/equipment native-owner leaf: the actual ContactHand query
 // owner and its full restore execute above without a copied scope stand-in.
 if(mockPrepareNpcH9&&!g_npc_query172.prepared){
  g_npc_query172.prepared=g_npc_query172.ready=true;g_npc_query172.gate=npc172::Gate::Ready;
  g_npc_query172.skip.add(reinterpret_cast<void*>(0x180005000ull),reinterpret_cast<void*>(0x180003000ull));
 }
 return g_npc_query172.ready;
}
static bool SameNpcQueryH9(const NpcQuery172& a,const NpcQuery172& b){
 return a.requested==b.requested&&a.prepared==b.prepared&&a.ready==b.ready&&a.gate==b.gate&&
  a.skip.bodyCount==b.skip.bodyCount&&a.skip.entityCount==b.skip.entityCount&&
  a.skip.bodies==b.skip.bodies&&a.skip.entities==b.skip.entities;
}
static int assertions=0,failures=0;
static void Check(bool v,const char*what){++assertions;if(!v)++failures;std::printf("%s %s\n",v?"PASS":"FAIL",what);}
static void Init(){
 g_census_mode=0; // each synthetic scene owns its own native-query selection
 kcd2vr::contact_handover_d71::SetContactContinuity(false);
 mockRetailLeaseD69=false;
 mockReplyAtH25=mockReplyModeH25=0;spatialPatchYH25=.04761905f;
 memory.clear();logs.clear();wallPoint={.25f,0,0};wallNormal={-1,0,0};mockCalls=0;mockMode=0;argumentsOk=true;distance=.25f;
 g_rigid_contact_contract_d67r1=0;g_hand_rigid_contact_d67r1=false;
 g_npc_query172={};mockBodies172=false;mockCollider172=nullptr;g_weapon_control_mode=0;
 mockPrepareNpcH9=false;mockPrepareCallsH9=0;
 diagnostics::lab::state.store(0);kcd2vr::collab_h21::setPhysicsMode(0);
 kcd2vr::lab58::rung.store(0);kcd2vr::lab58::livingHitImpact.store(false);
 mockPart172=0;mockCornerParts55=false;mockOrigins55.clear();
 g_contact_budget_active6695=false;g_contact_pass6695=false;g_contact_max_us6695=0;g_contact_budget_skips6695=0;g_collision_binding6692=0;g_collision_live6693=0;g_collision_last_status6692=0;g_collision_hits6692=0;
 g_contact_budget_sample65=0;g_contact_budget6695={}; // independent synthetic world/pass per case
 mockMicros59c1=1000000;mockQueryUs59c1=0;
 g_contact_latency59c1.~LatencyBackoff59c1();new(&g_contact_latency59c1)wc::LatencyBackoff59c1();
 g_collision_circuit6693.~Circuit();new(&g_collision_circuit6693)wc::Circuit();
 const unsigned char prefix[]{0x48,0x8B,0xC4,0x48,0x89,0x58,0x08,0x48,0x89,0x70,0x10,0x48,0x89,0x78,0x18,0x4C,0x89,0x70,0x20,0x55};Store(base+wc::kHelperRva,prefix);
 for(auto a:wc::kNativeWorldLoadRvas){unsigned char op[]{0x48,0x8B,0x0D,0,0,0,0};const auto d=static_cast<std::int32_t>(wc::kWorldSlotRva-a-7);std::memcpy(op+3,&d,4);Store(base+a,op);}
 Store(base+wc::kWorldSlotRva,world);Store(base+0x492B830ull,std::uintptr_t(0xDEAD));
 Store(world,base+wc::kWorldVtableRva);Store(base+wc::kWorldVtableRva+wc::kQueryVtableOffset,base+wc::kQueryMethodRva);
 const float transform[]{0,0,0,1,0,0,0,1};Store(character+0xA08,transform);
}
#ifndef KCD2VR_WORLD_ADAPTER_FIXTURE_ONLY_D53
// H24: the extractor supplies the actual final free-hand caller. Only palm
// geometry, current valid/owner flags and native query duration are test leaves.
// Stable costs below are modeled resource pressure, not measured engine timings.
static V3 schedulingOffsetsH24[6]{};
static FingerContactGeometry103 schedulingGeometryH24{};
struct SchedulingStateH24 {
 QuatT28 original[2]{};bool allowedReady[2]{},handBlocked[2]{},armBlocked[2]{},armSaturated[2]{};V3 allowedGoal[2]{};
};
struct SchedulingTrackingH24 {float left_grip=0,right_grip=0,left_trigger=0,right_trigger=0;std::uint64_t sequence=0,tick=0;};
static std::atomic<std::uint64_t> g_contact_control_epoch6695{1};
static Q4 DesiredHand6689(const SchedulingStateH24&,int,int,Q4 q){return q;}
static bool PalmGeometry103(const SchedulingStateH24&,int,Q4,float,float,std::uint64_t,V3* p,FingerContactGeometry103& f,float,bool){
 std::copy_n(schedulingOffsetsH24,6,p);f=schedulingGeometryH24;return true;
}
// Explicit native-tool solver leaf; the production caller's ownership/filter
// gates are executed below. The anvil solver has its own actual-header tests.
static unsigned smithDispatchRepair[2]{};
struct SmithContactHistoryR38 {};
static void SmithAnvilContactA2(void*,std::uint64_t,int side,const SchedulingStateH24&,int,Q4&,V3&,SmithContactHistoryR38* staged){if(staged)++smithDispatchRepair[side];}
static bool SimplePalmGeometryR24(const SchedulingStateH24&,int,Q4,V3* p){std::copy_n(schedulingOffsetsH24,6,p);return true;}
// Geometry is an explicit cached-rig leaf in this native-query fixture.
static bool ContactPalmGeometryD79(const SchedulingStateH24& s,int side,Q4 q,float grip,float trigger,
        std::uint64_t key,V3 (&p)[7],FingerContactGeometry103& f,bool precise,bool balanced,float bias,bool trial){
 V3 palm[6]{},tips[6]{};
 if(balanced){SimplePalmGeometryR24(s,side,q,palm);PalmGeometry103(s,side,q,grip,trigger,key,tips,f,bias,trial);
  kcd2vr::contact_balanced_d79::compose(palm,tips,p);return true;}
 return precise?PalmGeometry103(s,side,q,grip,trigger,key,p,f,bias,trial):SimplePalmGeometryR24(s,side,q,p);
}
static void SchedulingCallerH24(SchedulingStateH24& s,HandContact6695* stagedContact,V3* goals,SchedulingTrackingH24 ts,
                               bool collisionMode,const bool* valid,const bool* freeContact164R1,std::uint64_t epoch=1,
                               bool performance=false,bool nativeCapture=false,const unsigned* yieldsD56=nullptr,const bool* ownersD56=nullptr,bool stationContactD75=false){
 const bool balancedContactD79=performance&&!stationContactD75;
 const bool perfContactModeDs2=false,preciseContactModeR24=!stationContactD75&&!balancedContactD79,nativeFingerCapture=nativeCapture;
 const unsigned defaultYieldsD56[2]{};const bool defaultOwnersD56[2]{};
 const unsigned* contactYield164R1=yieldsD56?yieldsD56:defaultYieldsD56;const bool* otherOwnerH50=ownersD56?ownersD56:defaultOwnersD56;
 const auto character=reinterpret_cast<void*>(::character);const struct{int hand_r=0,hand_l=1;}rig;
 struct{std::uint64_t epoch;bool fingerTrialH7[2]{},fingerSpringH31=false;float fingerOpenBiasH7[2]{};SmithContactHistoryR38 smithContactR38{};}tx{epoch};
 const int frame=0;Q4 visQH50[2]{{0,0,0,1},{0,0,0,1}};const std::uint64_t heldFingerKey[2]{},contactControl=1;
 WORLD_ADAPTER_FREE_HAND_CALLER_H24
}
static void SchedulingReadyH24(){
 g_npc_query172.requested=g_npc_query172.prepared=g_npc_query172.ready=true;g_npc_query172.gate=npc172::Gate::Ready;
 g_npc_query172.skip.add(reinterpret_cast<void*>(0x180005000ull),reinterpret_cast<void*>(0x180003000ull));
}
#ifndef KCD2VR_WORLD_ADAPTER_CALLER_ONLY_D56
static void SmithDispatchRepair(){
 for(bool collision:{false,true})for(bool performance:{false,true}){
  Init();SchedulingStateH24 state{};HandContact6695 hands[2]{};V3 goals[2]{};SchedulingTrackingH24 tracking{};
  const bool valid[2]{true,true},held[2]{false,false};smithDispatchRepair[0]=smithDispatchRepair[1]=0;
  SchedulingCallerH24(state,hands,goals,tracking,collision,valid,held,1,performance);
  Check(smithDispatchRepair[0]==0&&smithDispatchRepair[1]==0,"free-hand stage never solves smith tools before fixture guidance");
  SchedulingCallerH24(state,hands,goals,tracking,collision,valid,held,1,performance,true);
  Check(smithDispatchRepair[0]==0&&smithDispatchRepair[1]==0,"native finger capture does not add early smith dispatch");
 }
}
static void ContactSchedulingH24(){
 for(bool seven:{false,true})for(unsigned stride:{1u,2u})for(unsigned cost:{1u,90u,130u,260u})for(int owner=0;owner<6;++owner){
  Init();mockMode=4;mockBodies172=true;mockQueryUs59c1=1;schedulingGeometryH24={};schedulingGeometryH24.orientation={0,0,0,1};
  const auto body=std::uintptr_t(0x180004000ull),entity=std::uintptr_t(0x180006000ull);
  Store(body,base+0x3a48b30);Store(base+0x3a48b30+0x50,base+0x5d6f44);Store(body+0x20,entity);
  Store(entity,base+0x4072c78);Store(entity+0xC,std::uint32_t(777));mockCollider172=reinterpret_cast<void*>(body);
  for(int f=0;f<6;++f)schedulingOffsetsH24[f]={.14f,.02f*(f-2),0};
  if(seven)for(int f=1;f<5;++f){auto& curve=schedulingGeometryH24.curve[f];curve.count=3;
   curve.offset[0]={.14,.02*(f-2),0};curve.offset[1]={0,.02,0};schedulingGeometryH24.input[f]=1;}
  wallPoint={.145f,0,0};wallNormal={-1,0,0};SchedulingStateH24 state{};SchedulingTrackingH24 tracking{};
  HandContact6695 hands[2]{};V3 goals[2]{};std::uint64_t previous=0;
  for(int h=0;h<2;++h){state.original[h].q[3]=1;ToT(state.original[h],{-.2f,h?.2f:-.2f,0});}
  bool valid[2]{owner!=3&&owner!=5,owner!=5},free[2]{owner!=1&&owner!=4,owner!=2&&owner!=4};
  tracking.tick=1000;tracking.sequence=1;goals[0]={.14f,-.2f,0};goals[1]={.14f,.2f,0};
  {ContactPass6695 pass(true,tracking.tick,previous,tracking.sequence);SchedulingReadyH24();
   SchedulingCallerH24(state,hands,goals,tracking,true,valid,free);}
  const QuatT28 original[2]{state.original[0],state.original[1]};const auto callsAtSeed=mockCalls;
  if(valid[0]&&free[0])Check(!mockOrigins55.empty()&&mockOrigins55[0].y<0,"H24 equal completion stamps retain deterministic original right-first order");
  mockQueryUs59c1=cost;unsigned completed[2]{},expired[2]{};std::uint64_t lastGood[2]{1000,1000},oldestAge[2]{};
  for(unsigned sample=1;sample<=120;++sample){tracking.tick=1000+11*sample;tracking.sequence=1+stride*sample;
   goals[0]={.14f,-.2f+.00246f*sample,0};goals[1]={.14f,.2f+.00246f*sample,0};const auto before=mockCalls;
   {ContactPass6695 pass(true,tracking.tick,previous,tracking.sequence);SchedulingReadyH24();
    SchedulingCallerH24(state,hands,goals,tracking,true,valid,free);
    Check(g_contact_budget6695.used<=18&&g_contact_budget6695.spent<=1000&&mockCalls-before<=18,
          "H24 scheduling retains the original shared18-ray/1000us cap");}
   for(int h=0;h<2;++h){if(hands[h].fresh169r1){++completed[h];lastGood[h]=tracking.tick;}
    if(!hands[h].have&&valid[h]&&free[h])++expired[h];oldestAge[h]=std::max(oldestAge[h],tracking.tick-lastGood[h]);
    if(valid[h]&&!free[h])Check(!hands[h].have&&!state.allowedReady[h]&&!state.handBlocked[h],"H24 weapon/native-owned hand keeps original free-contact refusal");}
   if(!valid[0]&&!valid[1])Check(mockCalls==before,"H24 both tracking-invalid sides perform no native contact query");
   const auto duplicateCalls=mockCalls;const auto stamp0=hands[0].lastCompleteTickH18,stamp1=hands[1].lastCompleteTickH18;
   {ContactPass6695 pass(true,tracking.tick,previous,tracking.sequence);SchedulingReadyH24();
    SchedulingCallerH24(state,hands,goals,tracking,true,valid,free);}
   Check(mockCalls==duplicateCalls&&hands[0].lastCompleteTickH18==stamp0&&hands[1].lastCompleteTickH18==stamp1,
         "H24 repeated eye keeps the original sample budget and completion provenance");
  }
  const unsigned eligible=unsigned(valid[0]&&free[0])+unsigned(valid[1]&&free[1]);
  const unsigned needed=eligible*(seven?7u:6u);
  if(needed&&std::uint64_t(needed-1)*cost<1000)
   Check((!valid[0]||!free[0]||completed[0]==120)&&(!valid[1]||!free[1]||completed[1]==120),"H24 sufficient budget completes every eligible hand every new sample");
  if(owner==0&&cost==130)Check(completed[0]>=59&&completed[1]>=59&&!expired[0]&&!expired[1]&&oldestAge[0]<=22&&oldestAge[1]<=22,
       "H24 constrained stable cost cannot permanently starve either free hand across120new samples or skipped sequence numbers");
  if(owner==0&&cost==260)Check(!completed[0]&&!completed[1]&&expired[0]>100&&expired[1]>100,
       "H24 insufficient budget cannot fabricate a completion or extend either original80ms deadline");
  Check(std::memcmp(original,state.original,sizeof(original))==0,"H24 caller resource ordering does not write native wrist endpoints");
  const auto beforeMenu=mockCalls;
  {ContactPass6695 pass(false,tracking.tick+11,previous,tracking.sequence+stride);
   SchedulingCallerH24(state,hands,goals,tracking,false,valid,free);}
  Check(mockCalls==beforeMenu,"H24 collision-disallowed caller performs no native query");
  std::printf("H24_SCHEDULER seven=%d stride=%u queryUs=%u owner=%d complete=%u,%u expired=%u,%u longestAgeMs=%llu,%llu nativeCalls=%d\n",
      seven,stride,cost,owner,completed[0],completed[1],expired[0],expired[1],(unsigned long long)oldestAge[0],(unsigned long long)oldestAge[1],mockCalls-callsAtSeed);
  // Reuse the same real caller after a backwards clock, contact-history
  // reset and menu gap. These do not create permission or extend a query lease.
  if(owner==0&&cost==130){
   for(int reset=0;reset<3;++reset){
    if(reset==1){ResetHandContact6695(hands[0]);ResetHandContact6695(hands[1]);}
    if(reset==2){hands[0].epoch=hands[1].epoch=1;}
    unsigned recovered[2]{};const auto tickBase=reset?std::uint64_t(500+reset*1000):std::uint64_t(500);
    for(unsigned sample=1;sample<=20;++sample){tracking.tick=tickBase+11*sample;tracking.sequence+=stride;
     goals[0]={.14f,-.2f+.00246f*sample,0};goals[1]={.14f,.2f+.00246f*sample,0};
     {ContactPass6695 pass(true,tracking.tick,previous,tracking.sequence);SchedulingReadyH24();
      SchedulingCallerH24(state,hands,goals,tracking,true,valid,free,reset==2?2:1);}
     for(int h=0;h<2;++h)if(hands[h].fresh169r1&&hands[h].lastCompleteTickH18==tracking.tick)++recovered[h];
    }
    Check(recovered[0]>=9&&recovered[1]>=9,"H24 clock rollback/reset/new body epoch cannot lock scheduling onto the same hand");
    std::printf("H24_RESET reset=%d seven=%d stride=%u complete=%u,%u\n",reset,seven,stride,recovered[0],recovered[1]);
   }
  }
 }
}
static void MovingContactSchedulingH25(){
 for(bool seven:{false,true})for(unsigned cost:{1u,60u,70u,75u,90u,130u,260u}){
  Init();mockMode=4;mockBodies172=true;mockQueryUs59c1=1;schedulingGeometryH24={};schedulingGeometryH24.orientation={0,0,0,1};
  const auto body=std::uintptr_t(0x180004000ull),entity=std::uintptr_t(0x180006000ull);
  Store(body,base+0x3a48b30);Store(base+0x3a48b30+0x50,base+0x5d6f44);Store(body+0x20,entity);
  Store(entity,base+0x4072c78);Store(entity+0xC,std::uint32_t(777));mockCollider172=reinterpret_cast<void*>(body);
  for(int f=0;f<6;++f)schedulingOffsetsH24[f]={.14f,.02f*(f-2),0};
  if(seven)for(int f=1;f<5;++f){auto& curve=schedulingGeometryH24.curve[f];curve.count=3;
   curve.offset[0]={.14,.02*(f-2),0};curve.offset[1]={0,.02,0};schedulingGeometryH24.input[f]=1;}
  SchedulingStateH24 state{};SchedulingTrackingH24 tracking{};HandContact6695 hands[2]{};V3 goals[2]{};
  std::uint64_t previous=0;bool valid[2]{true,true},free[2]{true,true};
  for(int h=0;h<2;++h){state.original[h].q[3]=1;ToT(state.original[h],{-.2f,h?.2f:-.2f,0});}
  wallPoint={.145f,0,0};wallNormal={-1,0,0};tracking.tick=1000;tracking.sequence=1;
  goals[0]={.14f,-.2f,0};goals[1]={.14f,.2f,0};
  {ContactPass6695 pass(true,tracking.tick,previous,tracking.sequence);SchedulingReadyH24();SchedulingCallerH24(state,hands,goals,tracking,true,valid,free);}
  mockQueryUs59c1=cost;unsigned completed[2]{},expired[2]{},penetrating[2]{};std::uint64_t lastGood[2]{1000,1000},oldest[2]{};
  for(unsigned sample=1;sample<=120;++sample){tracking.tick=1000+11*sample;tracking.sequence=1+sample;
   wallPoint.x=.145f-.009f*sample;goals[0]={.14f,-.2f+.00246f*(sample&1),0};goals[1]={.14f,.2f+.00246f*(sample&1),0};const auto calls=mockCalls;
   {ContactPass6695 pass(true,tracking.tick,previous,tracking.sequence);SchedulingReadyH24();SchedulingCallerH24(state,hands,goals,tracking,true,valid,free);
    Check(g_contact_budget6695.used<=18&&g_contact_budget6695.spent<=1000&&mockCalls-calls<=18,"moving scheduler retains global18/1000 cap");}
   if(sample<=8&&(cost==90||cost==130))std::printf("H25_MOVING_PASS seven=%d queryUs=%u sample=%u actualNativeCalls=%d used=%u spent=%llu fresh=%d,%d clock=%llu,%llu\n",
      seven,cost,sample,mockCalls-calls,g_contact_budget6695.used,(unsigned long long)g_contact_budget6695.spent,
      hands[0].fresh169r1,hands[1].fresh169r1,(unsigned long long)hands[0].lastCompleteTickH18,(unsigned long long)hands[1].lastCompleteTickH18);
   for(int h=0;h<2;++h){if(hands[h].fresh169r1){++completed[h];lastGood[h]=tracking.tick;}
    if(!hands[h].have)++expired[h];oldest[h]=std::max(oldest[h],tracking.tick-lastGood[h]);
    if(hands[h].fresh169r1&&goals[h].x+.14f>wallPoint.x-.0079f)++penetrating[h];}
  }
  const unsigned needed=(seven?7u:6u)+1u;
  if((needed-1)*cost<1000)Check(!expired[0]&&!expired[1]&&completed[0]>0&&completed[1]>0&&oldest[0]<=80&&oldest[1]<=80,
      "REQUIRED sufficient single-hand work cannot starve moving current recovery or extend original80ms");
  Check(!penetrating[0]&&!penetrating[1],"REQUIRED a fresh moving-contact completion is actually outside the current surface");
  std::printf("MOVING_SCHEDULER seven=%d queryUs=%u complete=%u,%u expired=%u,%u oldestMs=%llu,%llu penetratingFresh=%u,%u\n",
      seven,cost,completed[0],completed[1],expired[0],expired[1],(unsigned long long)oldest[0],(unsigned long long)oldest[1],penetrating[0],penetrating[1]);
 }
}
static void MovingContactFailureH25(){
 for(int failure=0;failure<5;++failure){
  Init();mockMode=4;mockBodies172=true;
  const auto body=std::uintptr_t(0x180004000ull),entity=std::uintptr_t(0x180006000ull);
  Store(body,base+0x3a48b30);Store(base+0x3a48b30+0x50,base+0x5d6f44);Store(body+0x20,entity);
  Store(entity,base+0x4072c78);Store(entity+0xC,std::uint32_t(777));mockCollider172=reinterpret_cast<void*>(body);
  V3 features[6]{};for(int i=0;i<6;++i)features[i]={.14f,.02f*(i-2),0};
  FingerContactGeometry103 geometry{};geometry.orientation={0,0,0,1};
  HandContact6695 contact{};std::uint64_t previous=0;V3 point{};bool ok=false;
  wallPoint={.145f,0,0};wallNormal={-1,0,0};
  const auto query=[&](std::uint64_t tick,std::uint64_t sample,V3 raw){
   ContactPass6695 pass(true,tick,previous,sample);SchedulingReadyH24();
   return ContactHand6695(contact,reinterpret_cast<void*>(character),1,{-.2f,0,0},raw,point,features,&geometry);
  };
  ok=query(1000,1,{.14f,0,0});const auto original=contact;const auto oldPoint=point;
  Check(ok&&contact.blocked&&contact.fresh169r1&&contact.lastCompleteTickH18==1000,"H25 failure control starts from an actual completed native surface query");
  wallPoint.x=.136f;
  if(failure<3){mockReplyAtH25=mockCalls+7;mockReplyModeH25=failure==0?1:failure==1?3:5;}
  if(failure==3)mockQueryUs59c1=180;
  if(failure==4)mockCollider172=reinterpret_cast<void*>(0x180003000ull);
  ok=query(1011,2,{.14f,.00246f,0});
  if(failure<2)Check(!ok&&!contact.have&&!contact.fresh169r1&&g_collision_circuit6693.reason()==failure+1,
      "H25 extra native fault/invalid result preserves existing circuit invalidation, not clearance");
  else{
   Check(ok&&contact.have&&!contact.fresh169r1&&contact.lastCompleteTickH18==original.lastCompleteTickH18&&Len(Sub(point,oldPoint))<1e-6,
      "H25 incomplete/no-plane/self recovery retains only original endpoint and completion clock");
   mockReplyAtH25=0;mockQueryUs59c1=180;mockCollider172=reinterpret_cast<void*>(body);
   ok=query(1090,3,{.14f,.00246f,0});
   Check(!ok&&!contact.have&&!contact.fresh169r1,"H25 recovery cannot renew or extend the original80ms incomplete-query deadline");
  }
  Check(g_contact_budget6695.used<=18&&g_contact_budget6695.spent<=1000,"H25 error recovery never replenishes shared count/time budget");
 }
}
static void FullContactCapacityH25(){
 Init();mockMode=8;mockBodies172=true;
 const auto body=std::uintptr_t(0x180004000ull),entity=std::uintptr_t(0x180006000ull);
 Store(body,base+0x3a48b30);Store(base+0x3a48b30+0x50,base+0x5d6f44);Store(body+0x20,entity);
 Store(entity,base+0x4072c78);Store(entity+0xC,std::uint32_t(777));mockCollider172=reinterpret_cast<void*>(body);
 V3 feature[6]{};for(int i=0;i<6;++i)feature[i]={.02f*i,.02f*i,0};
 FingerContactGeometry103 geometry{};geometry.orientation={0,0,0,1};
 for(int i=1;i<5;++i){geometry.curve[i].count=3;geometry.curve[i].offset[0]={.12,.12,0};geometry.input[i]=1;}
 HandContact6695 hand{};V3 allowed{};std::uint64_t previous=0;bool ok=false;
 {ContactPass6695 pass(true,1000,previous,1);SchedulingReadyH24();
  ok=ContactHand6695(hand,reinterpret_cast<void*>(character),1,{0,0,0},{.5f,0,0},allowed,feature,&geometry);}
 std::printf("SEVEN_SLOT_CONTACT current=%d planes=%d fresh=%d have=%d queries=%d allowed=(%.9f,%.9f,%.9f) clock=%llu\n",ok,hand.planeCount,hand.fresh169r1,hand.have,mockCalls,allowed.x,allowed.y,allowed.z,(unsigned long long)hand.lastCompleteTickH18);
 Check(ok&&hand.have&&hand.fresh169r1&&hand.planeCount==7&&allowed.x<=.1321f,
       "REQUIRED solvable original seven-plane temporal contact does not disappear on optional spatial capacity");
}
static void RecoverySeventhWitnessH25(){
 Init();mockMode=8;mockBodies172=true;spatialPatchYH25=.041322314f;
 for(int i=0;i<2;++i){const auto body=std::uintptr_t(i?0x180007000ull:0x180004000ull),entity=std::uintptr_t(i?0x180008000ull:0x180006000ull);
  Store(body,base+0x3a48b30);Store(base+0x3a48b30+0x50,base+0x5d6f44);Store(body+0x20,entity);
  Store(entity,base+0x4072c78);Store(entity+0xC,std::uint32_t(i?888:777));}
 mockCollider172=reinterpret_cast<void*>(0x180004000ull);
 V3 feature[6]{};for(int i=0;i<6;++i)feature[i]={.02f*i,.02f*i,0};
 FingerContactGeometry103 geometry{};geometry.orientation={0,0,0,1};
 HandContact6695 hand{};V3 allowed{};std::uint64_t previous=0;bool ok=false;
 {ContactPass6695 pass(true,1000,previous,1);SchedulingReadyH24();
  ok=ContactHand6695(hand,reinterpret_cast<void*>(character),1,{0,0,0},{.5f,0,0},allowed,feature,&geometry);}
 const auto& last=hand.planes[6];
 std::printf("SEVENTH_RECOVERY ok=%d count=%d feature=%d entity=%u part=%d queries=%d gap=%.9f\n",
  ok,hand.planeCount,last.feature,last.native172.entityId,last.native172.part,mockCalls,Dot(last.normal,Sub(Add(allowed,feature[5]),last.point)));
 Check(ok&&hand.fresh169r1&&hand.planeCount==7&&last.feature==5&&last.native172.entityId==888&&last.native172.part==207,
  "H25 actual six temporal witnesses plus recovery publish current finger5 witness at plane index6");
}

static void TemporalCoverageH25(){
 for(bool seven:{false,true})for(int selected=0;selected<(seven?7:6);++selected){
  Init();mockMode=7;mockBodies172=true;thinCenterH24={.1f,.02f*selected,0};
  const auto body=std::uintptr_t(0x180004000ull),entity=std::uintptr_t(0x180006000ull);
  Store(body,base+0x3a48b30);Store(base+0x3a48b30+0x50,base+0x5d6f44);Store(body+0x20,entity);
  Store(entity,base+0x4072c78);Store(entity+0xC,std::uint32_t(777));mockCollider172=reinterpret_cast<void*>(body);
  V3 feature[6]{};for(int i=0;i<6;++i)feature[i]={0,.02f*i,0};
  FingerContactGeometry103 geometry{};geometry.orientation={0,0,0,1};
  if(seven)for(int i=1;i<5;++i){geometry.curve[i].count=3;geometry.curve[i].offset[0]={0,.12,0};geometry.input[i]=1;}
  HandContact6695 contact{};V3 allowed{};std::uint64_t previous=0;bool ok=false;
  {ContactPass6695 pass(true,1000,previous,1);SchedulingReadyH24();
   ok=ContactHand6695(contact,reinterpret_cast<void*>(character),1,{0,0,0},{0,0,0},allowed,feature,&geometry);}
  const auto calls=mockCalls;
  {ContactPass6695 pass(true,1011,previous,2);SchedulingReadyH24();
   ok=ContactHand6695(contact,reinterpret_cast<void*>(character),1,{0,0,0},{.2f,0,0},allowed,feature,&geometry);}
  bool witness=false;for(int j=0;j<contact.planeCount;++j)witness|=contact.planes[j].feature==selected&&contact.planes[j].native172.entityId==777;
  Check(ok&&contact.fresh169r1&&contact.blocked&&witness&&allowed.x<=.0921f,
    "H25 each original temporal palm/finger/knuckle ray retains its own isolated5mm obstacle witness");
  Check(mockCalls-calls==(seven?7:6),"H25 isolated temporal obstacle needs no replacement or extra query");
 }
}
static void MovingWorldTransformH25(){
 for(float scale:{.8f,1.2f})for(float angle:{-.4f,.4f}){
  Init();mockMode=4;mockBodies172=true;mockQueryUs59c1=1;
  const auto body=std::uintptr_t(0x180004000ull),entity=std::uintptr_t(0x180006000ull);
  Store(body,base+0x3a48b30);Store(base+0x3a48b30+0x50,base+0x5d6f44);Store(body+0x20,entity);
  Store(entity,base+0x4072c78);Store(entity+0xC,std::uint32_t(777));mockCollider172=reinterpret_cast<void*>(body);
  const float transform[]{0,0,std::sin(angle*.5f),std::cos(angle*.5f),.35f,-.22f,.60f,scale};Store(character+0xA08,transform);
  ModelWorld6692 model{};Check(ReadModelWorld6692(reinterpret_cast<void*>(character),model),"H25 transformed contact uses the original native model transform reader");
  schedulingGeometryH24={};schedulingGeometryH24.orientation={0,0,0,1};
  for(int f=0;f<6;++f)schedulingOffsetsH24[f]={.14f,.02f*(f-2),0};
  SchedulingStateH24 state{};SchedulingTrackingH24 tracking{};HandContact6695 hands[2]{};V3 goals[2]{};
  std::uint64_t previous=0;bool valid[2]{true,true},free[2]{true,true};
  for(int h=0;h<2;++h){state.original[h].q[3]=1;ToT(state.original[h],{-.2f,h?.2f:-.2f,0});}
  wallNormal=QRotateV(model.q,{-1,0,0});
  for(unsigned sample=0;sample<2;++sample){tracking.tick=1000+11*sample;tracking.sequence=sample+1;
   wallPoint=ModelToWorld6692(model,{sample?.136f:.145f,0,0});
   goals[0]={.14f,-.2f+.00246f*sample,0};goals[1]={.14f,.2f+.00246f*sample,0};
   {ContactPass6695 pass(true,tracking.tick,previous,tracking.sequence);SchedulingReadyH24();SchedulingCallerH24(state,hands,goals,tracking,true,valid,free);}
  }
  for(int h=0;h<2;++h){const auto end=ModelToWorld6692(model,Add(goals[h],schedulingOffsetsH24[0]));
   const float gap=Dot(wallNormal,Sub(end,wallPoint));
   Check(hands[h].fresh169r1&&hands[h].blocked&&hands[h].lastCompleteTickH18==tracking.tick&&
      std::abs(goals[h].x+.012f)<.00001f&&gap>=.008f*scale-.000001f,
     "H25 deferred completion retains captured rotation/translation/scale and original model-space wrist mapping");
  }
 }
}
int main(){
 Init();mockQueryUs59c1=9000;
 auto slow59=WorldRayDistance6692({0,0,0},{1,0,0});
 Check(slow59.state==wc::Result::Busy&&g_collision_circuit6693.enabled()&&g_collision_circuit6693.faults()==0,"slow valid call enters recoverable cooldown, not permanent fault");
 for(int i=0;i<1000;++i)(void)WorldRayDistance6692({0,0,0},{1,0,0});
 Check(mockCalls==1,"cooldown performs no repeated native queries");
 mockMicros59c1+=500000;mockQueryUs59c1=0;
 slow59=WorldRayDistance6692({0,0,0},{1,0,0});
 Check(slow59.state==wc::Result::Clear&&mockCalls==2&&!g_contact_latency59c1.pending(),"fast revalidated call recovers without save reload or restart");
 Init();mockQueryUs59c1=9000;mockMode=1;
 slow59=WorldRayDistance6692({0,0,0},{1,0,0});mockMicros59c1+=9000000;
 (void)WorldRayDistance6692({0,0,0},{1,0,0});
 Check(slow59.state==wc::Result::Fault&&mockCalls==1&&g_collision_circuit6693.reason()==1,"slow real fault remains permanently closed");
 Init();mockQueryUs59c1=9000;mockMode=3;
 slow59=WorldRayDistance6692({0,0,0},{1,0,0});
 Check(slow59.state==wc::Result::Invalid&&!g_collision_circuit6693.enabled(),"slow invalid result is not concealed by latency handling");
 Init();mockQueryUs59c1=9000;
 for(int attempt=0;attempt<5;++attempt){
   slow59=WorldRayDistance6692({0,0,0},{1,0,0});const int calls=mockCalls;
   const auto delay=500000ull<<std::min(attempt,3);mockMicros59c1+=delay-1;
   (void)WorldRayDistance6692({0,0,0},{1,0,0});
   Check(mockCalls==calls,"repeated slow calls back off 0.5/1/2/4 seconds without query storm");++mockMicros59c1;
 }
 Init();mockQueryUs59c1=9000;(void)WorldRayDistance6692({0,0,0},{1,0,0});
 mockMicros59c1+=500000;Store(world,base+0x1234);
 (void)WorldRayDistance6692({0,0,0},{1,0,0});
 Check(mockCalls==1&&!g_collision_circuit6693.enabled(),"recovery revalidates world type before another native call");
 Init();Check(CollisionBinding6692(base),"native caller-load contract matches");
 auto r=WorldRayDistance6692({0,0,0},{1,0,0});Check(r.state==wc::Result::Clear&&mockCalls==1&&argumentsOk,"correct world pointer used, poisoned old slot ignored");
 Init();memory.erase(base+wc::kWorldSlotRva);r=WorldRayDistance6692({0,0,0},{1,0,0});Check(r.state==wc::Result::Disabled&&mockCalls==0,"absent correct world is unavailable, not a clear query");
 Init();Store(world,base+0x1234);r=WorldRayDistance6692({0,0,0},{1,0,0});Check(mockCalls==0&&!g_collision_circuit6693.enabled(),"wrong live world type blocks without native invocation");
 Init();unsigned char wrongLoad[]{0x48,0x8B,0x0D,0,0,0,0};auto bad=std::int32_t(0x492B830-wc::kNativeWorldLoadRvas[0]-7);std::memcpy(wrongLoad+3,&bad,4);Store(base+wc::kNativeWorldLoadRvas[0],wrongLoad);
 Check(!CollisionBinding6692(base)&&mockCalls==0,"old pointer source in native fingerprint rejected");
 Init();mockMode=1;
 for(int i=0;i<12570;++i)(void)WorldRayFan6692({0,0,0},{1,0,0},.05f);
 Check(mockCalls==1&&g_collision_circuit6693.calls()==1&&g_collision_circuit6693.faults()==1,"12570 retries make only one native call after a modeled fault");
 Check(!g_collision_fan_valid6693&&!CollisionBinding6692(base),"fault cannot be reported as a valid empty ray fan");
 CollisionStatus6692(2000,"ACTIVE_OR_WAITING_WORLD");Check(std::any_of(logs.begin(),logs.end(),[](const std::string&x){return x.find("BLOCKED_NATIVE_FAULT")!=std::string::npos;}),"status reports blocked fault, never active");
 Init();mockMode=1;V3 allowed{};float hit=0;
 bool ok=ConstrainHandWorld6692(reinterpret_cast<void*>(character),{0,0,0},{1,0,0},allowed,hit);
 Check(!ok&&mockCalls==1&&hit==0,"failed hand query returns unavailable backend, not successful collision");
 Init();mockMode=2;
 ok=ConstrainHandWorld6692(reinterpret_cast<void*>(character),{0,0,0},{1,0,0},allowed,hit);
 Check(ok&&mockCalls==5&&std::fabs(allowed.x-.238f)<.00001&&hit==.25f,"valid modeled hit constrains hand with skin margin");
 Init();ok=ConstrainHandWorld6692(reinterpret_cast<void*>(character),{0,0,0},{1,0,0},allowed,hit);
 Check(ok&&mockCalls==5&&allowed.x==1&&hit==0,"valid modeled empty fan preserves request");
 Init();mockMode=3;r=WorldRayDistance6692({0,0,0},{1,0,0});Check(r.state==wc::Result::Invalid&&!g_collision_circuit6693.enabled(),"impossible native count blocks");
 Init();mockMode=2;distance=std::numeric_limits<float>::quiet_NaN();r=WorldRayDistance6692({0,0,0},{1,0,0});Check(r.state==wc::Result::Invalid&&!g_collision_circuit6693.enabled(),"nonfinite distance blocks instead of unclamping");
 Init();Store(base+wc::kWorldVtableRva+wc::kQueryVtableOffset,base+0x123456);Check(!CollisionBinding6692(base)&&mockCalls==0,"query virtual-slot mismatch is blocked");

 Init();std::uint64_t previous=0;HandContact6695 handState;V3 point{};
 {ContactPass6695 pass(true,1000,previous,101);mockMode=4;distance=.25f;
  ok=ContactHand6695(handState,reinterpret_cast<void*>(character),1,{0,0,0},{1,0,0},point);
  Check(ok&&mockCalls==5&&handState.blocked&&std::abs(point.x-.242f)<.00001f,"actual contact adapter stops modeled hand before static surface");}
 const auto callsAtContact=mockCalls;
 {ContactPass6695 pass(true,1008,previous,101);ok=ContactHand6695(handState,reinterpret_cast<void*>(character),1,{0,0,0},{1,0,0},point);}
 Check(ok&&mockCalls==callsAtContact&&std::abs(point.x-.242f)<.00001f,"same tracking sample at a later callback reuses world-space constraint without duplicate physics");
 const float shifted[]{0,0,0,1,.1f,0,0,1};Store(character+0xA08,shifted);
 {ContactPass6695 pass(true,1009,previous,101);ok=ContactHand6695(handState,reinterpret_cast<void*>(character),1,{0,0,0},{1,0,0},point);}
 Check(ok&&std::abs(point.x-.142f)<.00001f,"moving avatar frame does not drag cached contact through the wall");
 // H18: recoverable latency backoff must not alternate a just-checked
 // contact with the raw tracked goal/native reseed. Its original receipt
 // expires after80ms; budget/backoff presentation cannot renew it.
 Init();previous=0;ResetHandContact6695(handState);mockMode=4;
 {ContactPass6695 pass(true,1000,previous,1);ok=ContactHand6695(handState,reinterpret_cast<void*>(character),1,{0,0,0},{1,0,0},point);}
 mockQueryUs59c1=9000;
 {ContactPass6695 pass(true,1010,previous,2);ok=ContactHand6695(handState,reinterpret_cast<void*>(character),1,{.8f,0,0},{1,0,0},point);}
 Check(ok&&handState.have&&!handState.fresh169r1&&std::abs(point.x-.242f)<.00001f,
     "H18 first slow query conservatively retains the last complete endpoint");
 mockQueryUs59c1=0;const auto slowContactCallsH18=mockCalls;
 handState.fingerContact[1]=true;handState.curl[1]=.7; // an existing contact override must retire with its receipt
 for(auto tickH18:{1020u,1040u,1080u}){
  ContactPass6695 pass(true,tickH18,previous,tickH18);
  ok=ContactHand6695(handState,reinterpret_cast<void*>(character),1,{.8f,0,0},{1,0,0},point);
  Check(ok&&handState.have&&!handState.fresh169r1&&std::abs(point.x-.242f)<.00001f&&mockCalls==slowContactCallsH18,
      "H18 recoverable backoff holds original same-owner contact for at most80ms without queries or fresh evidence");
 }
 {ContactPass6695 pass(true,1081,previous,1081);ok=ContactHand6695(handState,reinterpret_cast<void*>(character),1,{.8f,0,0},{1,0,0},point);}
 Check(!ok&&!handState.have&&!handState.fingerContact[1]&&!handState.planeCount&&point.x==1&&mockCalls==slowContactCallsH18,
     "H18 held backoff frames never renew the original80ms contact deadline");
 Init();previous=0;ResetHandContact6695(handState);mockQueryUs59c1=9000;mockMode=4;
 {ContactPass6695 pass(true,1000,previous,1);ok=ContactHand6695(handState,reinterpret_cast<void*>(character),1,{0,0,0},{1,0,0},point);}
 Check(!ok&&!handState.have&&point.x==1,"H18 a first incomplete query cannot freeze a hand at the native seed");
 Init();previous=0;ResetHandContact6695(handState);mockMode=4;
 {ContactPass6695 pass(true,1000,previous,1);ContactHand6695(handState,reinterpret_cast<void*>(character),1,{0,0,0},{1,0,0},point);}
 (void)g_contact_latency59c1.observe(wc::Result::Hit,9000,mockMicros59c1);
 {ContactPass6695 pass(true,1010,previous,2);ok=ContactHand6695(handState,reinterpret_cast<void*>(character),2,{0,0,0},{1,0,0},point);}
 Check(!ok&&!handState.have&&point.x==1,"H18 replacement pose epoch never inherits cached hand contact");
 Init();previous=0;ResetHandContact6695(handState);mockMode=4;
 {ContactPass6695 pass(true,1000,previous,1);ContactHand6695(handState,reinterpret_cast<void*>(character),1,{0,0,0},{1,0,0},point);}
 (void)g_contact_latency59c1.observe(wc::Result::Hit,9000,mockMicros59c1);
 const float replacementH18[]{0,0,0,1,0,0,0,1};Store(character+0x1000+0xA08,replacementH18);
 {ContactPass6695 pass(true,1010,previous,2);ok=ContactHand6695(handState,reinterpret_cast<void*>(character+0x1000),1,{0,0,0},{1,0,0},point);}
 Check(!ok&&!handState.have&&point.x==1,"H18 replacement character cannot borrow another hand's cached contact");
 Init();previous=0;ResetHandContact6695(handState);mockMode=4;
 {ContactPass6695 pass(true,1000,previous,1);ContactHand6695(handState,reinterpret_cast<void*>(character),1,{0,0,0},{1,0,0},point);}
 const auto beforeBudgetHoldH18=mockCalls;
 {ContactPass6695 pass(true,1020,previous,2);g_contact_budget6695.remaining=0;
  ok=ContactHand6695(handState,reinterpret_cast<void*>(character),1,{0,0,0},{1,0,0},point);}
 Check(ok&&!handState.fresh169r1&&std::abs(point.x-.242f)<.00001f&&mockCalls==beforeBudgetHoldH18,
     "H18 exhausted shared budget retains a recent checked endpoint without an extra native query");
 {ContactPass6695 pass(true,1081,previous,3);g_contact_budget6695.remaining=0;
  ok=ContactHand6695(handState,reinterpret_cast<void*>(character),1,{0,0,0},{1,0,0},point);}
 Check(!ok&&!handState.have&&point.x==1&&mockCalls==beforeBudgetHoldH18,
     "H18 repeated budget failures cannot extend original hand contact beyond80ms");
 Init();previous=0;ResetHandContact6695(handState);mockMode=4;
 {ContactPass6695 pass(true,1000,previous,1);ContactHand6695(handState,reinterpret_cast<void*>(character),1,{0,0,0},{1,0,0},point);}
 handState.fingerContact[1]=true;handState.curl[1]=.7;
 g_collision_circuit6693.invalidate();
 {ContactPass6695 pass(true,1010,previous,2);ok=ContactHand6695(handState,reinterpret_cast<void*>(character),1,{0,0,0},{1,0,0},point);}
 Check(!ok&&!handState.have&&!handState.fingerContact[1]&&!handState.planeCount&&point.x==1,"H18 permanent invalid/fault circuit always releases cached contact immediately");
 Init();previous=0;ResetHandContact6695(handState);memory.erase(base+wc::kWorldSlotRva);
 {ContactPass6695 pass(true,1000,previous);ok=ContactHand6695(handState,reinterpret_cast<void*>(character),1,{0,0,0},{1,0,0},point);}
 Check(!ok&&!handState.have&&point.x==1&&mockCalls==0,"unavailable world cannot freeze hands in the initial native position");
 Init();previous=0;ResetHandContact6695(handState);
 {ContactPass6695 pass(true,1000,previous);mockMode=0;ok=ContactHand6695(handState,reinterpret_cast<void*>(character),1,{0,0,0},{1,0,0},point);}
 Check(ok&&point.x==1&&!handState.blocked,"actual contact adapter follows controller after a clear modeled sweep");
 {ContactPass6695 pass(true,1020,previous);mockMode=1;ok=ContactHand6695(handState,reinterpret_cast<void*>(character),1,{1,0,0},{1.2f,0,0},point);}
 Check(!ok&&!g_collision_circuit6693.enabled(),"first contact fault disables backend rather than reporting a collision-free scene");
 const auto afterFault=mockCalls;for(unsigned i=0;i<100;++i){ContactPass6695 pass(true,1040+i*20,previous);ContactHand6695(handState,reinterpret_cast<void*>(character),1,{1,0,0},{1.2f,0,0},point);}
 Check(mockCalls==afterFault,"later contact passes cannot rearm fault circuit");
 Init();previous=0;{ContactPass6695 pass(true,1000,previous);for(int i=0;i<100;++i)(void)WorldRayDistance6692({0,0,0},{1,0,0});}
 Check(mockCalls<=18&&g_contact_budget_skips6695.load()>0,"actual native adapter enforces shared pass budget");
 Init();(void)WorldRayFan6692({0,0,0},{1,0,0},0.f);Check(mockCalls==1,"center-line arm test no longer duplicates the identical ray five times");

 // Regular free-space callbacks must not hold/catch up every 16ms.
 Init();previous=0;ResetHandContact6695(handState);
 {ContactPass6695 pass(true,1000,previous,1);ContactHand6695(handState,reinterpret_cast<void*>(character),1,{0,0,0},{.1f,0,0},point);}
 {ContactPass6695 pass(true,1008,previous,2);ContactHand6695(handState,reinterpret_cast<void*>(character),1,{0,0,0},{.12f,0,0},point);}
 Check(std::abs(point.x-.12f)<.0001,"fresh free-space sample advances at 8ms instead of freezing until 16ms");
 const auto callsBeforeDuplicate=mockCalls;
 {ContactPass6695 pass(true,1009,previous,2);ContactHand6695(handState,reinterpret_cast<void*>(character),1,{0,0,0},{.12f,0,0},point);}
 Check(mockCalls==callsBeforeDuplicate,"duplicated eye/callback sample performs zero additional native rays");
 // R49C1: repeat evaluation of a CLEAR sample follows the current body frame.
 const float clearMoved49[]{0,0,0,1,.1f,0,0,1};Store(character+0xA08,clearMoved49);
 {ContactPass6695 pass(true,1009,previous,2);ContactHand6695(handState,reinterpret_cast<void*>(character),1,{0,0,0},{.12f,0,0},point);}
 Check(std::abs(point.x-.12f)<.0001&&mockCalls==callsBeforeDuplicate,"clear duplicate rebases with Henry instead of replaying old world wrist");
 const auto previousWorld49=handState.worldAllowed;
 ContactCommit6695(handState,reinterpret_cast<void*>(character),point);
 Check(handState.worldAllowed.x==previousWorld49.x,"duplicate presentation does not advance the last physics-tested world origin");
 const float clearTurn49[]{0,0,.70710678f,.70710678f,.1f,0,0,1};Store(character+0xA08,clearTurn49);
 {ContactPass6695 pass(true,1010,previous,2);ContactHand6695(handState,reinterpret_cast<void*>(character),1,{0,0,0},{.12f,0,0},point);}
 Check(std::abs(point.x-.12f)<.0001&&std::abs(point.y)<.0001&&mockCalls==callsBeforeDuplicate,"clear duplicate follows body rotation without additional rays");
 // Forward fingertip extension, not a larger wrist-centered fan, reaches wall.
 Init();previous=0;ResetHandContact6695(handState);mockMode=4;
 V3 palm[5]{{.06f,0,0},{.05f,.03f,0},{.13f,.02f,0},{.14f,0,0},{.11f,-.03f,0}};
 {ContactPass6695 pass(true,1000,previous,1);ok=ContactHand6695(handState,reinterpret_cast<void*>(character),1,{0,0,0},{.23f,0,0},point,palm);}
 bool outside=true;for(auto off:palm)outside=outside&&Dot(Sub(Add(point,off),wallPoint),wallNormal)>=.0079f;
 Check(ok&&outside&&std::abs(point.x-.102f)<.0001,"farthest fingertip stays outside plane with 8mm skin; wrist is not the palm center");
 {ContactPass6695 pass(true,1008,previous,2);ok=ContactHand6695(handState,reinterpret_cast<void*>(character),1,{0,0,0},{.23f,.04f,0},point,palm);}
 Check(ok&&std::abs(point.y-.04f)<.0001&&std::abs(point.x-.102f)<.0001,"fresh constrained hand can slide tangentially instead of holding last point");
 // Rotation-only extension changes point sweeps even with a fixed wrist.
 Init();previous=0;ResetHandContact6695(handState);mockMode=4;
 V3 shortPalm[5]{};for(auto& q:shortPalm)q={.04f,0,0};
 {ContactPass6695 pass(true,1000,previous,1);ContactHand6695(handState,reinterpret_cast<void*>(character),1,{0,0,0},{.14f,0,0},point,shortPalm);}
 {ContactPass6695 pass(true,1008,previous,2);ContactHand6695(handState,reinterpret_cast<void*>(character),1,{0,0,0},{.14f,0,0},point,palm);}
 Check(point.x<.103f,"rotating/extending palm features is detected even without wrist translation");
 Init();mockMode=5;auto noNormal=WorldRayDistance6692({0,0,0},{1,0,0});
 Check(noNormal.state==wc::Result::Hit&&!noNormal.plane,"absent/invalid normal cannot become a fabricated contact plane");
 Init();previous=0;ResetHandContact6695(handState);mockMode=4;wallPoint={.4f,0,0};wallNormal=Normalize(V3{-1,.3f,0});
 {ContactPass6695 pass(true,1000,previous,1);ContactHand6695(handState,reinterpret_cast<void*>(character),1,{0,0,0},{.5f,.05f,0},point,palm);}
 outside=true;for(auto off:palm)outside=outside&&Dot(Sub(Add(point,off),wallPoint),wallNormal)>.0078f;
 Check(outside,"oblique native normal constrains the complete sampled palm, not merely its travel axis");
 // C55: exercise the actual simultaneous-plane adapter, not only the
 // mathematical projection helper. Rotating feature paths and a corner must
 // not become order-dependent extra pushes or an old plane lifetime cache.
 Init();previous=0;ResetHandContact6695(handState);mockMode=6;V3 corner55[5]{{.15f,0,0},{0,.15f,0},{.10f,.10f,0},{.05f,0,0},{0,.05f,0}};
 bool cornerSafe55=true;float cornerWorst55=0;V3 previous55{};
 for(unsigned sample55=1;sample55<=120;++sample55){
  const float phase55=float(sample55)/120;const V3 want55{.40f,.08f+.32f*phase55,0};
  const auto before55=mockCalls;
  {ContactPass6695 pass(true,1000+sample55*8,previous,sample55);ok=ContactHand6695(handState,reinterpret_cast<void*>(character),1,{0,0,0},want55,point,corner55);}
  cornerSafe55=cornerSafe55&&ok&&mockCalls-before55<=5&&point.x+.15f<=.2421f&&point.y+.15f<=.2501f;
  if(sample55>1)cornerWorst55=std::max(cornerWorst55,Len(Sub(point,previous55)));
  previous55=point;
 }
 std::printf("C55_CORNER steps=120 safe=%d worstStepM=%.9f totalRays=%d\n",cornerSafe55,cornerWorst55,mockCalls);
 Check(cornerSafe55&&cornerWorst55<.01f,"actual measured corner planes keep every palm feature outside without per-frame jumping");
 {ContactPass6695 pass(true,2000,previous,121);ok=ContactHand6695(handState,reinterpret_cast<void*>(character),1,{0,0,0},{0,0,0},point,corner55);}
 Check(ok&&Len(point)<.0001f&&!handState.blocked,"backing away retires actual corner constraints instead of retaining a frozen wrist");
 // V66.103 executes the actual six-feature contact adapter plus the portable
 // curve fitting it calls. Geometry below is explicitly synthetic, not Henry data.
 Init();previous=0;ResetHandContact6695(handState);mockMode=4;
 FingerContactGeometry103 geometry{};geometry.orientation={0,0,0,1};geometry.enabled[1]=true;
 auto& curve=geometry.curve[1];curve.count=4;
 for(int k=0;k<4;++k){curve.offset[k]={.04,0,0};curve.closed[k]=kcd2vr::pose_core::from_rotation_vector({0,0,1});}
 const auto savedCurve=curve;V3 six[6]{};for(auto& p:six)p={.04f,0,0};six[2]={.16f,0,0};
 {ContactPass6695 pass(true,1000,previous,1);ok=ContactHand6695(handState,reinterpret_cast<void*>(character),1,{0,0,0},{.13f,0,0},point,six,&geometry);}
 Check(ok&&std::abs(point.x-.13f)<.0001&&handState.fingerContact[1]&&handState.curl[1]>.1,
       "actual hand contact curls only the touching finger instead of retracting whole wrist");
 bool onlyIndex=true;for(int finger=0;finger<5;++finger)if(finger!=1)onlyIndex=onlyIndex&&!handState.fingerContact[finger];
 Check(onlyIndex,"one finger surface does not change unrelated finger curls");
 Check(curve.closed[0].z==savedCurve.closed[0].z&&curve.open[0].w==savedCurve.open[0].w,"native contact adapter leaves endpoint data untouched");
 const int beforeRepeat=mockCalls;
 {ContactPass6695 pass(true,1000,previous,1);ok=ContactHand6695(handState,reinterpret_cast<void*>(character),1,{0,0,0},{.13f,0,0},point,six,&geometry);}
 Check(ok&&mockCalls==beforeRepeat&&std::abs(point.x-.13f)<.0001,"duplicate contact reuses fitted curve and performs no additional ray calls");
 {ContactPass6695 pass(true,1008,previous,2);ok=ContactHand6695(handState,reinterpret_cast<void*>(character),1,{0,0,0},{0,0,0},point,six,&geometry);}
 Check(ok&&!handState.fingerContact[1]&&std::abs(point.x)<.0001,"moving away releases surface override without saving a new finger pose");
 Init();previous=0;ResetHandContact6695(handState);mockMode=5;
 {ContactPass6695 pass(true,1000,previous,1);ok=ContactHand6695(handState,reinterpret_cast<void*>(character),1,{0,0,0},{.13f,0,0},point,six,&geometry);}
 Check(ok&&!handState.fingerContact[1]&&point.x<.13f,"a hit without a validated normal uses conservative wrist contact, not an invented finger fit");
 Init();previous=0;ResetHandContact6695(handState);mockMode=4;geometry={};geometry.orientation={0,0,0,1};
 for(int f=0;f<5;++f){auto& c=geometry.curve[f];c.count=3;c.offset[0]={.04,0,0};c.offset[1]={.05,0,0};c.offset[2]={.04,0,0};
  c.closed[1]=kcd2vr::pose_core::from_rotation_vector({0,0,3.141592653589793});geometry.input[f]=1;}
 for(auto& p:six)p={.05f,0,0};six[0]={.04f,0,0};
 {ContactPass6695 pass(true,1000,previous,1);ok=ContactHand6695(handState,reinterpret_cast<void*>(character),1,{0,0,0},{.18f,0,0},point,six,&geometry);}
 Check(ok&&handState.featureCount==7&&handState.blocked&&point.x<.16f&&mockCalls==7,
       "closed-fist knuckles hit a wall even when curled fingertips and palm are clear");
 // A moving body entering a stationary open hand still intersects the existing
 // per-feature spatial ray. No additional rays or guessed overlap ABI.
 Init();previous=0;ResetHandContact6695(handState);mockMode=4;wallPoint={.6f,0,0};
 {ContactPass6695 pass(true,1000,previous,1);ContactHand6695(handState,reinterpret_cast<void*>(character),1,{.14f,0,0},{.14f,0,0},point,palm);}
 {ContactPass6695 pass(true,1008,previous,2);
  g_npc_query172.requested=g_npc_query172.ready=true;mockBodies172=true;
  g_npc_query172.skip.add(reinterpret_cast<void*>(0x180005000ull),reinterpret_cast<void*>(0x180003000ull));
  wallPoint={.25f,0,0};const auto before=mockCalls;
  ok=ContactHand6695(handState,reinterpret_cast<void*>(character),1,{.14f,0,0},{.14f,0,0},point,palm);
  Check(ok&&handState.blocked&&point.x<.103f&&mockCalls-before==5,
        "stationary hand detects an entering body within the unchanged feature-ray budget");}
 // New body-query route: exact skip argument transport, owner self rejection,
 // and no native call at all when the exclusion owner is unavailable.
 Init();g_npc_query172.requested=true;
 r=WorldRayDistance6692({0,0,0},{1,0,0});
 Check(r.state==wc::Result::Busy&&mockCalls==0,"missing NPC exclusions are UNKNOWN, never clearance");
 Init();g_npc_query172.requested=g_npc_query172.ready=true;mockBodies172=true;mockMode=4;
 constexpr std::uintptr_t selfBody=0x180003000ull,otherBody=0x180004000ull,selfEntity=0x180005000ull;
 g_npc_query172.skip.add(reinterpret_cast<void*>(selfEntity),reinterpret_cast<void*>(selfBody));
 mockCollider172=reinterpret_cast<void*>(otherBody);Store(otherBody,base+0x3a82570);
 r=WorldRayDistance6692({0,0,0},{1,0,0});
 Check(argumentsOk&&r.state==wc::Result::Hit&&r.plane&&g_npc_hits172[unsigned(npc172::Body::Living)].load()>0,
       "NPC query uses retail body mask/flags/skip list and classifies a living hit");
 mockCollider172=reinterpret_cast<void*>(selfBody);r=WorldRayDistance6692({0,0,0},{1,0,0});
 Check(r.state==wc::Result::Busy&&!r.plane,"self body returned despite skip is not a usable contact");
 mockCollider172=reinterpret_cast<void*>(otherBody);
 Store(base+0x3a82570+0x50,base+0x5d6f44);Store(otherBody+0x20,selfEntity);
 Store(selfEntity,base+0x4072c78);Store(selfEntity+0xC,std::uint32_t(42));
 r=WorldRayDistance6692({0,0,0},{1,0,0});
 Check(r.state==wc::Result::Busy&&!r.plane,"secondary physics body with Henry's exact entity owner is rejected");
 {previous=0;g_weapon_control_mode=7;ContactPass6695 pass(true,1000,previous,1);
  Check(g_npc_query172.requested&&!g_npc_query172.ready,"a new pass never reuses borrowed body exclusions");}
 Check(!g_npc_query172.requested&&!g_npc_query172.skip.bodyCount,"pass exit drops every borrowed physics body");
 // H9: execute the current extracted ContactHand and its ACTUAL production
 // scoped owner. F9 selects NPCs only inside these existing free-hand rays;
 // the surrounding held-weapon query context is never borrowed or changed.
 Init();previous=0;ResetHandContact6695(handState);mockMode=4;mockBodies172=true;mockPrepareNpcH9=true;
 mockCollider172=reinterpret_cast<void*>(otherBody);Store(otherBody,base+0x3a48b30);
 Store(base+0x3a48b30+0x50,base+0x5d6f44);Store(otherBody+0x20,std::uintptr_t(0x180006000ull));
 Store(0x180006000ull,base+0x4072c78);Store(0x18000600Cull,std::uint32_t(777));
 kcd2vr::collab_h21::setPhysicsMode(3);kcd2vr::lab58::rung.store(3);kcd2vr::lab58::livingHitImpact.store(true);
 {ContactPass6695 pass(true,1000,previous,1);
  g_npc_query172.prepared=g_npc_query172.ready=true;g_npc_query172.gate=npc172::Gate::Equipment;
  g_npc_query172.skip.add(reinterpret_cast<void*>(0x180009000ull),reinterpret_cast<void*>(0x18000a000ull));
  const auto outer=g_npc_query172;
  Check(!NpcQuerySelected172(),"F9 alone does not change the outer held-weapon body query selector");
  ok=ContactHand6695(handState,reinterpret_cast<void*>(character),1,{0,0,0},{1,0,0},point);
  Check(ok&&mockCalls==5&&argumentsOk&&mockPrepareCallsH9>0&&handState.planeCount&&
        handState.planes[0].native172.body==npc172::Body::Articulated&&handState.planes[0].native172.entityId==777,
        "F9 ON actual free-hand adapter uses existing five body rays and current articulated witness in world-only mode");
  Check(SameNpcQueryH9(g_npc_query172,outer),"F9 free-hand exit restores every outer query flag and exclusion entry");
  mockBodies172=false;const auto before=mockCalls;r=WorldRayDistance6692({0,0,0},{1,0,0});
  Check(r.state==wc::Result::Hit&&mockCalls==before+1&&argumentsOk,
        "held/world ray after F9 free-hand call retains its original world-only mask and no borrowed exclusions");
 }
 Init();previous=0;ResetHandContact6695(handState);mockMode=4;mockPrepareNpcH9=true;
 {ContactPass6695 pass(true,1000,previous,1);const auto outer=g_npc_query172;
  ok=ContactHand6695(handState,reinterpret_cast<void*>(character),1,{0,0,0},{1,0,0},point);
  Check(ok&&mockCalls==5&&argumentsOk&&mockPrepareCallsH9==0&&SameNpcQueryH9(g_npc_query172,outer),
        "F9 OFF current free-hand adapter keeps normal world query and does not prepare NPC exclusions");
 }
 Init();previous=0;ResetHandContact6695(handState);mockMode=4;mockPrepareNpcH9=true;
 kcd2vr::lab58::rung.store(3);kcd2vr::lab58::livingHitImpact.store(true);
 diagnostics::lab::Set(diagnostics::lab::Key::F3,1);
 {ContactPass6695 pass(true,1000,previous,1);const auto outer=g_npc_query172;
  ok=ContactHand6695(handState,reinterpret_cast<void*>(character),1,{0,0,0},{1,0,0},point);
  Check(ok&&mockCalls==5&&argumentsOk&&mockPrepareCallsH9==0&&SameNpcQueryH9(g_npc_query172,outer),
        "native reference retains normal query policy even when F9 subtest remains selected");
 }
 Init();previous=0;ResetHandContact6695(handState);mockMode=4;mockBodies172=true;
 kcd2vr::lab58::rung.store(3);kcd2vr::lab58::livingHitImpact.store(true);
 {ContactPass6695 pass(true,1000,previous,1);
  g_npc_query172.requested=g_npc_query172.prepared=g_npc_query172.ready=true;g_npc_query172.gate=npc172::Gate::Ready;
  g_npc_query172.skip.add(reinterpret_cast<void*>(selfEntity),reinterpret_cast<void*>(selfBody));
  const auto outer=g_npc_query172;
  ok=ContactHand6695(handState,reinterpret_cast<void*>(character),1,{0,0,0},{1,0,0},point);
  Check(ok&&mockCalls==5&&argumentsOk&&SameNpcQueryH9(g_npc_query172,outer),
        "already prepared body-query owner remains unchanged across the F9 free-hand call");
 }
 Init();previous=0;ResetHandContact6695(handState);kcd2vr::lab58::rung.store(3);kcd2vr::lab58::livingHitImpact.store(true);
 {ContactPass6695 pass(true,1000,previous,1);
  g_npc_query172.prepared=true;g_npc_query172.gate=npc172::Gate::HenryBody;
  g_npc_query172.skip.add(reinterpret_cast<void*>(selfEntity),reinterpret_cast<void*>(selfBody));
  const auto outer=g_npc_query172;memory.erase(character+0xA08);
  ok=ContactHand6695(handState,reinterpret_cast<void*>(character),1,{0,0,0},{1,0,0},point);
  Check(!ok&&mockCalls==0&&SameNpcQueryH9(g_npc_query172,outer),
        "actual adapter early return also restores the complete prior query context");
 }
 // C55 raw intent: a moving body can overtake the previous achieved fingertip
 // while the tracked hand remains still. A contact correction is not input
 // movement. Reuse each feature's existing spatial ray, with no extra queries.
 for(const bool seven55:{false,true})for(const float tangent55:{0.f,.00246f}){
 Init();previous=0;ResetHandContact6695(handState);mockMode=4;
 V3 movingFeatures55[6]{};for(auto& p:movingFeatures55)p={.14f,0,0};movingFeatures55[0]={.04f,0,0};
 FingerContactGeometry103 movingGeometry55{};movingGeometry55.orientation={0,0,0,1};
 if(seven55)for(int f=1;f<5;++f){movingGeometry55.curve[f].count=3;
   movingGeometry55.curve[f].offset[0]={.07,0,0};movingGeometry55.curve[f].offset[1]={.07,0,0};movingGeometry55.input[f]=1;}
 V3 fixedRequest55{.14f,0,0};V3 priorPoint55{};
 mockCollider172=reinterpret_cast<void*>(otherBody);Store(otherBody,base+0x3a48b30);
 Store(base+0x3a48b30+0x50,base+0x5d6f44);Store(otherBody+0x20,std::uintptr_t(0x180006000ull));
 Store(0x180006000ull,base+0x4072c78);Store(0x18000600Cull,std::uint32_t(777));
 wallPoint={.145f,0,0};wallNormal={-1,0,0};
 {ContactPass6695 pass(true,1000,previous,1);
  g_npc_query172.requested=g_npc_query172.ready=true;mockBodies172=true;
  g_npc_query172.skip.add(reinterpret_cast<void*>(selfEntity),reinterpret_cast<void*>(selfBody));
  ok=ContactHand6695(handState,reinterpret_cast<void*>(character),1,{-.2f,0,0},fixedRequest55,point,movingFeatures55,&movingGeometry55);}
 priorPoint55=point;const auto priorPlanes55=handState.planeCount;
 Check(ok&&handState.featureCount==(seven55?7:6)&&priorPlanes55==2&&handState.planes[0].native172.body==npc172::Body::Articulated&&
   handState.planes[0].native172.entityId==777,"moving-surface fixture starts with two current qualified articulated-body feature planes");
 wallPoint={.136f,0,0};fixedRequest55.y+=tangent55;const auto beforeMoving55=mockCalls;
 {ContactPass6695 pass(true,1011,previous,2);
  g_npc_query172.requested=g_npc_query172.ready=true;mockBodies172=true;
  g_npc_query172.skip.add(reinterpret_cast<void*>(selfEntity),reinterpret_cast<void*>(selfBody));
  ok=ContactHand6695(handState,reinterpret_cast<void*>(character),1,{-.2f,0,0},fixedRequest55,point,movingFeatures55,&movingGeometry55);}
 std::printf("C55_MOVING_SURFACE rawTangentM=%.5f wallDeltaM=.009 dtMs=11 priorAllowedX=%.9f allowedX=%.9f outputStepM=%.9f priorPlanes=%d planes=%d rays=%d\n",
   tangent55,priorPoint55.x,point.x,Len(Sub(point,priorPoint55)),priorPlanes55,handState.planeCount,mockCalls-beforeMoving55);
 Check(ok&&handState.fresh169r1&&point.x+.14f<=wallPoint.x-.0079f&&Len(Sub(point,priorPoint55))<.010f,
   "stationary or tangential raw hand stays outside a9mm moving body without a91mm achieved-target jump");
 Check(mockCalls-beforeMoving55<=(seven55?7:6),"moving-body recovery uses only the existing feature-ray count");
 const auto afterMoving55=mockCalls;const auto allowedMoving55=point;
 {ContactPass6695 pass(true,1012,previous,2);
  g_npc_query172.requested=g_npc_query172.ready=true;mockBodies172=true;
  g_npc_query172.skip.add(reinterpret_cast<void*>(selfEntity),reinterpret_cast<void*>(selfBody));
  ok=ContactHand6695(handState,reinterpret_cast<void*>(character),1,{-.2f,0,0},fixedRequest55,point,movingFeatures55,&movingGeometry55);}
 Check(ok&&mockCalls==afterMoving55&&Len(Sub(point,allowedMoving55))<.0001f,
   "same tracking sample reuses the completed contact without advancing raw-intent history or querying again");
 {ContactPass6695 pass(true,1022,previous,3);
  g_npc_query172.requested=g_npc_query172.ready=true;mockBodies172=true;
  g_npc_query172.skip.add(reinterpret_cast<void*>(selfEntity),reinterpret_cast<void*>(selfBody));
  ok=ContactHand6695(handState,reinterpret_cast<void*>(character),1,{-.2f,0,0},{-.2f,0,0},point,movingFeatures55,&movingGeometry55);}
 Check(ok&&!handState.blocked&&std::abs(point.x+.2f)<.0001f,"backing away still retires current contact without a retained-plane timeout");
 }
 // A different current body/part is not continuity of the old palm surface.
 // Preserve its temporal feature cast; copied identity is not a retained lease.
 for(const bool replacementBody55:{false,true}){
 Init();previous=0;ResetHandContact6695(handState);mockMode=4;
 V3 bodyFeatures55[6]{};for(auto& p:bodyFeatures55)p={.14f,0,0};bodyFeatures55[0]={.04f,0,0};
 FingerContactGeometry103 bodyGeometry55{};bodyGeometry55.orientation={0,0,0,1};
 Store(otherBody,base+0x3a48b30);Store(base+0x3a48b30+0x50,base+0x5d6f44);
 Store(otherBody+0x20,std::uintptr_t(0x180006000ull));Store(0x180006000ull,base+0x4072c78);Store(0x18000600Cull,std::uint32_t(777));
 mockCollider172=reinterpret_cast<void*>(otherBody);wallPoint={.145f,0,0};
 {ContactPass6695 pass(true,1000,previous,1);g_npc_query172.requested=g_npc_query172.ready=true;mockBodies172=true;
  g_npc_query172.skip.add(reinterpret_cast<void*>(selfEntity),reinterpret_cast<void*>(selfBody));
  ContactHand6695(handState,reinterpret_cast<void*>(character),1,{-.2f,0,0},{.14f,0,0},point,bodyFeatures55,&bodyGeometry55);}
 const auto temporalStart55=Add(point,bodyFeatures55[1]);const auto rayStart55=mockOrigins55.size();wallPoint.x=.136f;
 if(replacementBody55){mockCollider172=reinterpret_cast<void*>(0x180007000ull);Store(0x180007000ull,base+0x3a48b30);
  Store(0x180007020ull,std::uintptr_t(0x180006000ull));}else mockPart172=422;
 {ContactPass6695 pass(true,1011,previous,2);g_npc_query172.requested=g_npc_query172.ready=true;mockBodies172=true;
  g_npc_query172.skip.add(reinterpret_cast<void*>(selfEntity),reinterpret_cast<void*>(selfBody));
  ContactHand6695(handState,reinterpret_cast<void*>(character),1,{-.2f,0,0},{.14f,.00246f,0},point,bodyFeatures55,&bodyGeometry55);}
 Check(mockOrigins55.size()>rayStart55+1&&Len(Sub(mockOrigins55[rayStart55+1],temporalStart55))<.0001f,
       "different native body or part does not reuse old surface continuity for a spatial ray");
 }
 // Keep independent surfaces on one articulated body distinct. This uses the
 // actual six-feature adapter over120 moving requests, not projection alone.
 Init();previous=0;ResetHandContact6695(handState);mockMode=6;mockCornerParts55=true;
 Store(otherBody,base+0x3a48b30);Store(base+0x3a48b30+0x50,base+0x5d6f44);
 Store(otherBody+0x20,std::uintptr_t(0x180006000ull));Store(0x180006000ull,base+0x4072c78);Store(0x18000600Cull,std::uint32_t(777));
 mockCollider172=reinterpret_cast<void*>(otherBody);
 V3 bodyCorner55[6]{{.04f,.04f,0},{.15f,0,0},{0,.15f,0},{.1f,.1f,0},{.05f,0,0},{0,.05f,0}};
 FingerContactGeometry103 cornerGeometry55{};cornerGeometry55.orientation={0,0,0,1};
 bool bodyCornerSafe55=true;float bodyCornerWorst55=0;V3 lastBodyCorner55{};
 for(unsigned s=1;s<=120;++s){const auto before55=mockCalls;
  {ContactPass6695 pass(true,1000+s*8,previous,s);g_npc_query172.requested=g_npc_query172.ready=true;mockBodies172=true;
   g_npc_query172.skip.add(reinterpret_cast<void*>(selfEntity),reinterpret_cast<void*>(selfBody));
   ok=ContactHand6695(handState,reinterpret_cast<void*>(character),1,{0,0,0},{.4f,.08f+.32f*float(s)/120,0},point,bodyCorner55,&cornerGeometry55);}
  bodyCornerSafe55=bodyCornerSafe55&&ok&&mockCalls-before55<=6;
  // A not-yet-hit surface has no measured8mm skin constraint; require actual
  // nonpenetration, as in the retained static corner case above.
  for(auto feature55:bodyCorner55)bodyCornerSafe55=bodyCornerSafe55&&point.x+feature55.x<=.2501f&&point.y+feature55.y<=.2501f;
  if(s>1)bodyCornerWorst55=std::max(bodyCornerWorst55,Len(Sub(point,lastBodyCorner55)));lastBodyCorner55=point;
 }
 std::printf("C55_BODY_CORNER safe=%d worstStepM=%.9f final=(%.9f,%.9f) planes=%d rays=%d\n",bodyCornerSafe55,bodyCornerWorst55,point.x,point.y,handState.planeCount,mockCalls);
 Check(bodyCornerSafe55&&bodyCornerWorst55<.01f,"two distinct current articulated-body parts retain corner clearance and bounded target movement");
 {ContactPass6695 pass(true,2000,previous,121);g_npc_query172.requested=g_npc_query172.ready=true;mockBodies172=true;
  g_npc_query172.skip.add(reinterpret_cast<void*>(selfEntity),reinterpret_cast<void*>(selfBody));
  ok=ContactHand6695(handState,reinterpret_cast<void*>(character),1,{0,0,0},{0,0,0},point,bodyCorner55,&cornerGeometry55);}
 Check(ok&&!handState.blocked&&Len(point)<.0001f,"moving away from two body parts immediately clears both current contacts");

 // H23: current first-surface queries, not retained planes, must recover a
 // moving body that has overtaken EVERY stationary feature's temporal origin.
 // Coplanar features are synthetic; this is not the logged62610/62612 geometry.
 for(int signH23:{-1,1})for(bool livingH23:{false,true})for(bool sevenH23:{false,true})for(int replacementH23=0;replacementH23<3;++replacementH23){
  Init();previous=0;ResetHandContact6695(handState);mockMode=4;mockBodies172=true;
  V3 featuresH23[6]{};for(int f=0;f<6;++f)featuresH23[f]={signH23*.14f,.02f*(f-2),0};
  FingerContactGeometry103 geometryH23{};geometryH23.orientation={0,0,0,1};
  if(sevenH23)for(int f=1;f<5;++f){auto& curveH23=geometryH23.curve[f];curveH23.count=3;
   curveH23.offset[0]={signH23*.14,.02*(f-2),0};curveH23.offset[1]={0,.02,0};geometryH23.input[f]=1;}
  const int featureCountH23=sevenH23?7:6;
  const V3 rawH23{signH23*.14f,0,0},nativeH23{signH23*-.2f,0,0};
  const auto bodyAH23=std::uintptr_t(0x180004000ull),bodyBH23=std::uintptr_t(0x180007000ull);
  const auto vftH23=base+(livingH23?0x3a82570:0x3a48b30);
  for(auto bodyH23:{bodyAH23,bodyBH23}){Store(bodyH23,vftH23);Store(vftH23+0x50,base+0x5d6f44);
   Store(bodyH23+0x20,std::uintptr_t(0x180006000ull));}
  Store(0x180006000ull,base+0x4072c78);Store(0x18000600Cull,std::uint32_t(777));
  mockCollider172=reinterpret_cast<void*>(bodyAH23);wallPoint={signH23*.145f,0,0};wallNormal={float(-signH23),0,0};
  const auto queryH23=[&](std::uint64_t tick,std::uint64_t sample,V3 raw,std::uint64_t epoch=1){
   const auto outerH23=g_npc_query172;const auto raysH23=mockCalls;
   {ContactPass6695 pass(true,tick,previous,sample);
    g_npc_query172.requested=g_npc_query172.prepared=g_npc_query172.ready=true;g_npc_query172.gate=npc172::Gate::Ready;
    g_npc_query172.skip.add(reinterpret_cast<void*>(selfEntity),reinterpret_cast<void*>(selfBody));
    ok=ContactHand6695(handState,reinterpret_cast<void*>(character),epoch,nativeH23,raw,point,featuresH23,&geometryH23);}
   Check(mockCalls-raysH23<=18,"H25 complete current recovery stays within the shared18 query cap");
   Check(SameNpcQueryH9(g_npc_query172,outerH23),"H23 current body query restores the outer physics owner");
  };
  queryH23(1000,1,rawH23);const auto beforeH23=point;const auto firstRayH23=mockOrigins55.size();
  Check(ok&&handState.fresh169r1&&handState.blocked&&handState.planeCount>0&&handState.featureCount==featureCountH23&&
       handState.planes[0].native172.collider==bodyAH23&&handState.planes[0].native172.entityId==777,
       "H23 initial coplanar hand contact has a fresh qualified current body witness");
  if(replacementH23==1)mockCollider172=reinterpret_cast<void*>(bodyBH23);
  if(replacementH23==2)mockPart172=422;
  wallPoint.x=signH23*.136f;queryH23(1011,2,rawH23);
  bool currentWitnessH23=handState.planeCount>0;
  for(int p=0;p<handState.planeCount;++p)currentWitnessH23=currentWitnessH23&&
    handState.planes[p].native172.collider==(replacementH23==1?bodyBH23:bodyAH23)&&
    handState.planes[p].native172.part==mockPart172;
  const auto outsideH23=Dot(wallNormal,Sub(Add(point,featuresH23[0]),wallPoint));
  std::printf("H23_STATIONARY side=%d body=%d seven=%d replacement=%d rawDeltaM=0 wallDeltaM=.009 outputStepM=%.9f clearanceM=%.9f planes=%d\n",
    signH23,livingH23,sevenH23,replacementH23,Len(Sub(point,beforeH23)),outsideH23,handState.planeCount);
  Check(ok&&handState.fresh169r1&&currentWitnessH23&&outsideH23>=.0079f&&Len(Sub(point,beforeH23))<.01f,
        "H23 a9mm moving body cannot release stationary hand features143mm through its surface");
  Check(mockOrigins55.size()>firstRayH23&&Len(Sub(mockOrigins55[firstRayH23],beforeH23))<1e-6,
        "H23 palm spatial query uses the previous achieved wrist rather than an inside feature or raw wrist");
  const auto duplicateRaysH23=mockCalls;const auto completedH23=point;queryH23(1012,2,rawH23);
  Check(ok&&mockCalls==duplicateRaysH23&&Len(Sub(point,completedH23))<1e-6,
        "H23 same-sample callback does not repeat queries or advance the completed contact");
  queryH23(1022,3,nativeH23);
  Check(ok&&!handState.blocked&&Len(Sub(point,nativeH23))<1e-6,"H23 retreat from current body contact immediately clears measured constraints");
  queryH23(1033,4,rawH23);Check(ok&&handState.fresh169r1&&handState.blocked,"H23 forward motion freshly reacquires the surface after retreat");
  mockMode=0;queryH23(1044,5,rawH23);
  Check(ok&&handState.fresh169r1&&!handState.blocked&&!handState.planeCount&&Len(Sub(point,rawH23))<1e-6,
        "H23 no current hit clears old body contact immediately with no retained-plane lifetime");
  const auto clearRaysH23=mockCalls;queryH23(1055,6,rawH23);
  Check(ok&&mockCalls-clearRaysH23==featureCountH23&&!handState.planeCount&&Len(Sub(point,rawH23))<1e-6,
        "H23 completed no-contact history does not resurrect a cached NPC constraint");
  queryH23(1066,7,nativeH23);
  Check(ok&&!handState.blocked&&Len(Sub(point,nativeH23))<1e-6,"H23 retreat remains current free motion without sticking");
  mockMode=4;queryH23(1077,8,rawH23);Check(ok&&handState.blocked&&handState.fresh169r1,"H23 fresh forward reacquisition keeps its native query path");
  const auto teleportRaysH23=mockCalls;queryH23(1088,9,{signH23*3.f,0,0});
  Check(!ok&&mockCalls==teleportRaysH23&&!handState.have&&!handState.rawFeatureReady55,
        "H23 original large-motion rejection retires history without querying a borrowed body");
  queryH23(1099,10,rawH23,2);Check(ok&&handState.epoch==2&&handState.fresh169r1&&handState.blocked,
        "H23 a new body epoch reseeds from native pose and accepts only its current measured geometry");
 }

 // The NPC-enabled native mask includes world geometry too. Surface recovery
 // does not turn a current world hit into an NPC/body witness.
 for(int signWorldH23:{-1,1}){
  Init();previous=0;ResetHandContact6695(handState);mockMode=4;mockBodies172=true;mockCollider172=nullptr;
  V3 worldFeaturesH23[6]{};for(auto& f:worldFeaturesH23)f={signWorldH23*.14f,0,0};
  FingerContactGeometry103 worldGeometryH23{};worldGeometryH23.orientation={0,0,0,1};
  V3 firstWorldH23{};wallNormal={float(-signWorldH23),0,0};
  for(int nWorldH23=0;nWorldH23<2;++nWorldH23){wallPoint={signWorldH23*(nWorldH23?.136f:.145f),0,0};
   ContactPass6695 pass(true,1000+11*nWorldH23,previous,nWorldH23+1);
   g_npc_query172.requested=g_npc_query172.ready=true;
   g_npc_query172.skip.add(reinterpret_cast<void*>(selfEntity),reinterpret_cast<void*>(selfBody));
   const auto raysWorldH23=mockCalls;
   ok=ContactHand6695(handState,reinterpret_cast<void*>(character),1,{signWorldH23*-.2f,0,0},{signWorldH23*.14f,0,0},point,worldFeaturesH23,&worldGeometryH23);
   Check(mockCalls-raysWorldH23<=18,"H23 world surface in NPC-enabled mask keeps the same feature query budget");
   if(!nWorldH23)firstWorldH23=point;
  }
  bool noNpcWorldH23=handState.planeCount>0;
  for(int pWorldH23=0;pWorldH23<handState.planeCount;++pWorldH23){const auto& witnessWorldH23=handState.planes[pWorldH23].native172;
   noNpcWorldH23=noNpcWorldH23&&!witnessWorldH23.collider&&!witnessWorldH23.owner&&!witnessWorldH23.entityId;}
  Check(ok&&handState.fresh169r1&&noNpcWorldH23&&Len(Sub(point,firstWorldH23))<.01f&&
    Dot(wallNormal,Sub(Add(point,worldFeaturesH23[0]),wallPoint))>=.0079f,
    "H23 current world surface recovery keeps its actual non-NPC witness without fabricated body ownership");
 }
 SmithDispatchRepair();
 ContactSchedulingH24();

 // Independent missing-coverage control. Only the original finger1 temporal
 // ray crosses the finite patch; the palm and other five paths remain clear.
 Init();previous=0;ResetHandContact6695(handState);mockMode=7;mockBodies172=true;
 V3 thinFeaturesH24[6]{};thinFeaturesH24[1]={0,.1f,0};
 for(int i=2;i<6;++i)thinFeaturesH24[i]={0,-.01f*i,0};
 FingerContactGeometry103 thinGeometryH24{};thinGeometryH24.orientation={0,0,0,1};
 const auto prepareBodyH24=[&](std::uintptr_t body,unsigned id){Store(body,base+0x3a48b30);Store(base+0x3a48b30+0x50,base+0x5d6f44);
  const auto ownerH24=body==otherBody?std::uintptr_t(0x180006000ull):std::uintptr_t(0x180008000ull);
  Store(body+0x20,ownerH24);Store(ownerH24,base+0x4072c78);Store(ownerH24+0xC,std::uint32_t(id));};
 prepareBodyH24(otherBody,777);mockCollider172=reinterpret_cast<void*>(otherBody);
 for(int sampleThinH24=0;sampleThinH24<2;++sampleThinH24){ContactPass6695 pass(true,1000+11*sampleThinH24,previous,sampleThinH24+1);
  g_npc_query172.requested=g_npc_query172.ready=true;g_npc_query172.skip.add(reinterpret_cast<void*>(selfEntity),reinterpret_cast<void*>(selfBody));
  ok=ContactHand6695(handState,reinterpret_cast<void*>(character),1,{0,0,0},{sampleThinH24?.2f:0,0,0},point,thinFeaturesH24,&thinGeometryH24);
 }
 bool thinWitnessH24=false;for(int p=0;p<handState.planeCount;++p)thinWitnessH24|=handState.planes[p].feature==1&&
  handState.planes[p].native172.collider==otherBody&&std::abs(handState.planes[p].point.y-.1f)<1e-6;
 std::printf("THIN_FINGER1 recorded=NO outputX=%.9f blocked=%d planes=%d actualFinger1Witness=%d rays=%d\n",point.x,handState.blocked,handState.planeCount,thinWitnessH24,mockCalls);
 Check(ok&&handState.blocked&&thinWitnessH24&&point.x<=.0921f,"REQUIRED displaced finger1 temporal ray retains its own finite-obstacle native query witness");
 for(int sideH24:{-1,1})for(int changedH24=0;changedH24<6;++changedH24){
  Init();previous=0;ResetHandContact6695(handState);mockMode=changedH24==5?6:4;mockBodies172=true;mockCornerParts55=changedH24==5;
  cornerXH24=cornerYH24=.25f;prepareBodyH24(otherBody,777);prepareBodyH24(0x180007000ull,888);
  mockCollider172=changedH24==3?nullptr:reinterpret_cast<void*>(otherBody);
  V3 featuresH24[6]{};for(int i=0;i<6;++i)featuresH24[i]={sideH24*.14f,.02f*(i-2),0};
  FingerContactGeometry103 geometryH24{};geometryH24.orientation={0,0,0,1};
  V3 rawH24{sideH24*.14f,0,0},nativeH24{sideH24*-.2f,0,0};
  if(changedH24==5){const V3 corner[6]{{.04f,.04f,0},{.15f,0,0},{0,.15f,0},{.1f,.1f,0},{.05f,0,0},{0,.05f,0}};
   std::copy_n(corner,6,featuresH24);rawH24={.4f,.4f,0};nativeH24={0,0,0};}
  wallPoint={sideH24*.145f,0,0};wallNormal={float(-sideH24),0,0};
  const auto queryH24=[&](unsigned sample,V3 raw){const auto rays=mockCalls;ContactPass6695 pass(true,1000+11*sample,previous,sample);
   g_npc_query172.requested=g_npc_query172.ready=true;g_npc_query172.skip.add(reinterpret_cast<void*>(selfEntity),reinterpret_cast<void*>(selfBody));
   ok=ContactHand6695(handState,reinterpret_cast<void*>(character),1,nativeH24,raw,point,featuresH24,&geometryH24);
   Check(mockCalls-rays<=18,"CONTROL original temporal features plus current recovery share18-query cap");};
  queryH24(1,rawH24);queryH24(2,nativeH24);
  Check(ok&&!handState.blocked&&Len(Sub(point,nativeH24))<1e-6,"CONTROL retreat before moving-body test clears all original current planes");
  queryH24(3,rawH24);const auto beforeH24=point;
  if(changedH24==1)mockCollider172=reinterpret_cast<void*>(0x180007000ull);
  if(changedH24==2)mockPart172=422;
  if(changedH24==4)wallNormal=Normalize(V3{float(-sideH24),.15f,0});
  if(changedH24==5){cornerXH24=cornerYH24=.241f;rawH24.z=.00246f;}
  else{wallPoint.x=sideH24*.136f;rawH24.y=.00246f;}
  queryH24(4,rawH24);float minimumClearanceH24=1e6f;
  for(auto feature:featuresH24)if(changedH24==5)minimumClearanceH24=std::min({minimumClearanceH24,cornerXH24-point.x-feature.x,cornerYH24-point.y-feature.y});
   else minimumClearanceH24=std::min(minimumClearanceH24,Dot(wallNormal,Sub(Add(point,feature),wallPoint)));
  const bool safeH24=ok&&minimumClearanceH24>=-.0001f;
  bool measuredSkinH24=true;for(int p=0;p<handState.planeCount;++p){const auto& plane=handState.planes[p];
   for(int f=0;f<6;++f)if(plane.feature<0||plane.feature==f)measuredSkinH24=measuredSkinH24&&Dot(plane.normal,Sub(Add(point,featuresH24[f]),plane.point))>=.0079f;}

  bool witnessCurrentH24=handState.planeCount>0;
  for(int p=0;p<handState.planeCount;++p)witnessCurrentH24=witnessCurrentH24&&
   handState.planes[p].native172.collider==(changedH24==3?0:changedH24==1?0x180007000ull:otherBody)&&
   (changedH24==5||handState.planes[p].native172.part==mockPart172);
  std::printf("TANGENT_CONTROL recorded=NO side=%d scenario=%d safe=%d measuredSkin=%d minClearanceM=%.9f currentWitness=%d stepM=%.9f allowed=(%.9f,%.9f,%.9f) planes=%d\n",sideH24,changedH24,safeH24,measuredSkinH24,minimumClearanceH24,witnessCurrentH24,Len(Sub(point,beforeH24)),point.x,point.y,point.z,handState.planeCount);
  Check(safeH24&&measuredSkinH24&&witnessCurrentH24,"REQUIRED tangent current surface/collider/part/normal coverage remains outside measured geometry");
  mockMode=0;queryH24(5,rawH24);Check(ok&&!handState.blocked&&!handState.planeCount&&Len(Sub(point,rawH24))<1e-6,"CONTROL no current hit revokes all prior planes without lifetime extension");
 }
 MovingContactSchedulingH25();
 MovingContactFailureH25();
 FullContactCapacityH25();
 RecoverySeventhWitnessH25();
 TemporalCoverageH25();
 MovingWorldTransformH25();
 std::printf("world_adapter=%d/%d\n",assertions-failures,assertions);return failures?1:0;
}

#endif // KCD2VR_WORLD_ADAPTER_CALLER_ONLY_D56
#endif // KCD2VR_WORLD_ADAPTER_FIXTURE_ONLY_D53
