#define __fastcall
#include "core/instant_pickup_d72.h"
#include "core/haptics.h"
#include "subsystems/physics/load_fault_recovery_r96.h"
#include <cstring>
#include "core/mutual_admission_t25.h"
#include "vrik/prop_drive5916c1.h"
#include "vrik/staged_hold596c2.h"
#include "vrik/workstation178r20c1.h"
#include "diagnostics/capture168.h"
#include <cassert>
#include <cstdio>
#include <string>
#include <vector>
using DWORD=unsigned long;
namespace pm=kcd2vr::pose_core;
// The source's ph::Pose notation needs its real production namespace, not an alias type.

namespace ph=kcd2vr::physical;
namespace stage596=kcd2vr::staged596c2;namespace prop5916=kcd2vr::prop5916c1;
struct V3{double x{},y{},z{};};
struct CommittedFrame6695{
    float pickupTriggerD80[2]{};
    struct Stamp{std::uint64_t epoch=1,sample=1,tick=1000;std::uintptr_t actor=1;}stamp;
    ph::Pose allowed[2]{},propRequested5916c1[2]{};bool valid[2]{true,true},propRequestedValid5916c1[2]{true,true};
    V3 solvedHead122{0,0,.4};bool headEvidence122=true,mapFoldTrackedM39=true;float mapGripM39[2]{1,1};
};
struct TrackingSnapshot{bool allowed=true,head_valid=true,head_held=false,left_held=false,right_held=false;
    std::uint64_t tick=1000,source_epoch=1;double right_grip=1,left_grip=1;};
struct StagedProp596;
static bool PropIdentity596(const StagedProp596&) noexcept;
static void PropNote596(const char*,const StagedProp596&,bool,const char* = "NONE") noexcept;
static bool PropPlace596(const StagedProp596&,ph::Pose) noexcept;
static bool PropObjectPose5916C1(const StagedProp596&,ph::Pose&) noexcept;
static void PropPublish5916C1(const StagedProp596&,const CommittedFrame6695&,ph::Pose,std::uint64_t) noexcept;
static void PropSelfCollisionR155(const StagedProp596&,std::uint64_t) noexcept; // R155: R103 lease request per held tick (spy below)
static unsigned selfCollisionCallsR155=0,selfCollisionLegacyCallsR155=0;
#include "../src/vrik/instant_drop_r158.h" // R158: g_instant_drop_r158 lives in the extracted preamble
static void PropDropR158(const StagedProp596&,pm::Vec3,bool,std::uint64_t) noexcept; // R158: drop request (spy below)
static unsigned dropCallsR158=0,dropHandVelocityR158=0;
static std::uint64_t now=1000;static CommittedFrame6695 frame;static TrackingSnapshot tracking;
static std::atomic<int> g_cal_state{2};
static std::atomic<bool> g_gameplay_allowed{true},g_ctx_menu{false},g_ctx_dialogue{false},g_ctx_cutscene{false},g_ctx_reading{false},g_ctx_scripted{false};
static bool nativeReference=false,contextAllowed=true;
namespace diagnostics {namespace lab {static bool NativeReference(){return nativeReference;}}}
namespace animation_filter {static std::atomic<bool> nativeBodyOwned112{false};static bool kick=false;static bool NativeKickActive175(){return kick;}}
static const char* WorkstationContextRefusal19c4r2(std::uint64_t,bool){return contextAllowed?nullptr:"CONTEXT";}

static bool identity=true,handFree=true,actionOK=true,statusOK=true,stowResultD84=true;
static unsigned velocityCalls=0,placeCalls=0,stowCalls=0,statusCalls=0;static prop5916::Dynamics dynamics;
static ph::Pose object;static std::vector<std::string> notes;
static unsigned publishCalls=0;static std::uint64_t publishedSourceTick=0,publishedSample=0,publishedAt=0;
namespace diagnostic_trace{void Write(const char*s) noexcept{notes.emplace_back(s);}}
static std::uint64_t GetTickCount64(){return now;}
static bool selectedFistsD80=false;
static bool FistInputOwned101(){return selectedFistsD80;}
#include "../src/vrik/pickup_input_d80.h"
static bool committedAvailableR96=true;static std::uint64_t readClockR96=0;static unsigned committedReadsR96=0;
static bool ReadCommittedPacket6695(CommittedFrame6695& o){++committedReadsR96;if(readClockR96)now=readClockR96;o=frame;return committedAvailableR96;}
static bool TrackingHandUsable111(int){return tracking.allowed&&!tracking.right_held;}
static bool ReadTrackingSnapshot(TrackingSnapshot&o){o=tracking;return true;}
static bool PropHandFree596(int){return handFree;}
static bool nativeBodyD78=true;
static void* PropBody596(void*,std::uintptr_t* observed=nullptr,std::uintptr_t* vft=nullptr){
 if(observed)*observed=13;if(vft)*vft=0x10000000+0x3A48DC0;return nativeBodyD78?reinterpret_cast<void*>(13):nullptr;
}
static ph::Pose PhysPose6698(ph::Pose p){return p;}
static ph::Pose PropWorldHand596(const CommittedFrame6695&f,int side){return f.allowed[side];}
static ph::Pose PropRequestedWorld5916C1(const CommittedFrame6695&f,int side){return f.propRequested5916c1[side];}
static bool PropCommit596(void*,std::uint32_t){++stowCalls;return stowResultD84;}
static bool PropSetVelocity5916C1(void*,pm::Vec3 v={},pm::Vec3 a={}) noexcept{++velocityCalls;if(actionOK){dynamics.linear=v;dynamics.angular=a;}return actionOK;}
static bool PropVelocity596(void*p){return PropSetVelocity5916C1(p);}
static bool PropDynamics5916C1(void*,prop5916::Dynamics&d){++statusCalls;d=dynamics;return statusOK;}
#ifndef PROP_ADAPTER_D78_INC
#define PROP_ADAPTER_D78_INC "prop_adapter5916c1.inc"
// the checked-in copy predates R96 (tools/extract_prop5916c1.py was not re-run since; tools/check_interactions_d84.py
// says so). the R155 lease-call checks below need the current consumer, which tools/check_interactions_r96.py extracts.
#define PROP_ADAPTER_CHECKED_IN_PRE_R96 1
#endif
#include PROP_ADAPTER_D78_INC
static bool PropCommitInstantD72(const StagedProp596&) noexcept{++stowCalls;return stowResultD84;}
static void PropRetireInstantD72(const StagedProp596& p) noexcept {
 if(p.instantD72)(void)kcd2vr::mutual_admission_t25::RetirePickupD72(p.side,p.instantTokenD72);
}
static bool PropIdentity596(const StagedProp596&) noexcept{return identity;}
static void PropNote596(const char*s,const StagedProp596&,bool,const char* reason) noexcept{notes.emplace_back(s);notes.emplace_back(reason);}
static bool PropPlace596(const StagedProp596&,ph::Pose) noexcept{++placeCalls;return true;}
// R155: the production helper only asks the lease for an instant hold with a body; the legacy F5 hold is unchanged.
// The spy keeps the same guard so the tick's call pattern is what is checked here (lease table: instant_pickup_d72_test).
static void PropSelfCollisionR155(const StagedProp596& p,std::uint64_t) noexcept{
 if(!p.instantD72){++selfCollisionLegacyCallsR155;return;}
 if(!p.id||!p.entity||!p.body||p.side<0||p.side>1)return;++selfCollisionCallsR155;}
// R158: same guard as production (only an instant hold with an entity posts a drop)
static void PropDropR158(const StagedProp596& p,pm::Vec3,bool hand,std::uint64_t) noexcept{
 if(!p.instantD72||!p.id||!p.entity)return;++dropCallsR158;if(hand)++dropHandVelocityR158;}
static bool PropObjectPose5916C1(const StagedProp596&,ph::Pose&p) noexcept{p=object;return true;}
static void PropPublish5916C1(const StagedProp596&,const CommittedFrame6695& f,ph::Pose p,std::uint64_t at) noexcept{
 assert(p.p.x==object.p.x);++publishCalls;publishedSourceTick=f.stamp.tick;publishedSample=f.stamp.sample;publishedAt=at;
}
static void setup(){
    now=1000;nativeBodyD78=true;committedAvailableR96=true;readClockR96=0;committedReadsR96=0;
    kcd2vr::inventory_lifecycle_t8::loadEpisode18=1;kcd2vr::inventory_lifecycle_t8::loadingObserved18=false;kcd2vr::inventory_lifecycle_t8::state=2;
    selectedFistsD80=false;frame=CommittedFrame6695{};tracking={};object={};dynamics={};dynamics.mass=1;
    publishCalls=0;publishedSourceTick=publishedSample=publishedAt=0;
    g_cal_state=2;g_gameplay_allowed=true;g_ctx_menu=g_ctx_dialogue=g_ctx_cutscene=g_ctx_reading=g_ctx_scripted=false;
    nativeReference=false;contextAllowed=true;animation_filter::nativeBodyOwned112=false;animation_filter::kick=false;
    kcd2vr::instant_pickup_d72::stopping=false;for(auto& x:kcd2vr::mutual_admission_t25::pickupClaimsD72)x=0;
    notes.clear();identity=handFree=actionOK=statusOK=stowResultD84=true;velocityCalls=placeCalls=stowCalls=statusCalls=0;
    selfCollisionCallsR155=selfCollisionLegacyCallsR155=0;dropCallsR158=dropHandVelocityR158=0;
    kcd2vr::combat_rules178r10::f5Rung=2;kcd2vr::combat_rules178r10::workstationEnabled=true;
    kcd2vr::workstation178r20c1::experimentArmed36=true;kcd2vr::workstation178r20c1::experimentEpoch36=5;
    prop5916::enabled=true;g_prop596={};g_prop596.id=10;g_prop596.actor=11;g_prop596.key=12;
    g_prop596.body=reinterpret_cast<void*>(13);g_prop596.character=1;g_prop596.bodyEpoch=1;
    g_prop596.epoch=5;g_prop596.side=0;g_prop596.physics=true;g_prop596.sample=1;
    g_prop596.hold.begin(5,1000);g_prop596.drive.seed({},1,1000);g_prop596.sourceTickR96=1000;g_prop596.loadEpisodeR96=1;g_prop_fault5916c1.clear();
}
static void step(){now+=20;tracking.tick=frame.stamp.tick=now;++frame.stamp.sample;StagedPropTick596();}
int main(){
 setup();frame.propRequested5916c1[0].p.x=.1;step();
 assert(g_prop596.id&&velocityCalls==1&&placeCalls==0&&stowCalls==0&&object.p.x==0);
 StagedPropTick596();assert(velocityCalls==1); // same completed sample cannot apply twice
 step();assert(velocityCalls==2&&placeCalls==0&&g_prop596.id);
 tracking.right_grip=0;const auto achieved=dynamics.linear.x;step();assert(!g_prop596.id&&velocityCalls==2&&stowCalls==0&&dynamics.linear.x==achieved);
 setup();frame.propRequested5916c1[0].p.x=.1;step();tracking.allowed=false;step();assert(!g_prop596.id&&velocityCalls==2&&pm::length(dynamics.linear)==0);
 setup();identity=false;step();assert(!g_prop596.id&&velocityCalls==0&&stowCalls==0);
 setup();frame.stamp.epoch=2;step();assert(!g_prop596.id&&stowCalls==0);
 setup();actionOK=false;frame.propRequested5916c1[0].p.x=.1;step();assert(!g_prop596.id&&g_prop_fault5916c1.load()&&velocityCalls==1);step();assert(velocityCalls==1);
 setup();statusOK=false;step();assert(!g_prop596.id&&stowCalls==0);
 setup();frame.allowed[0].p=frame.propRequested5916c1[0].p={.34,0,0};step();frame.allowed[0].p=frame.propRequested5916c1[0].p={0,0,0};
 for(unsigned n=0;n<16;++n)step();assert(!g_prop596.id&&stowCalls==1);step();assert(stowCalls==1);
 setup();kcd2vr::diagnostics::capture168::gate.select(kcd2vr::diagnostics::capture168::Domain::Interaction,false,1000);
 frame.propRequested5916c1[0].p.x=.1;step();assert(g_prop596.id&&velocityCalls==1&&notes.empty());
 // The admitted instant occurrence has its own body/hand claim, independent
 // of legacy F5 and of subsequent changes to that experiment's epoch.
 const auto instant=[](){setup();kcd2vr::combat_rules178r10::f5Rung=0;kcd2vr::workstation178r20c1::experimentArmed36=false;
   g_prop596.instantD72=true;g_prop596.instantTokenD72=101;g_prop596.epoch=g_prop596.bodyEpoch;
   g_prop596.hold.begin(g_prop596.bodyEpoch,now);kcd2vr::mutual_admission_t25::pickupClaimsD72[0]=101;};
 instant();frame.propRequested5916c1[0].p.x=.1;step();assert(g_prop596.id&&velocityCalls==1);
 kcd2vr::workstation178r20c1::experimentEpoch36=77;step();assert(g_prop596.id&&velocityCalls==2);
#ifndef PROP_ADAPTER_CHECKED_IN_PRE_R96
 // R155: every held tick of an instant hold asks the R103 lease (the fixture's instant hold has no entity pointer, the
 // guard refuses it like production would; with one it is asked once per tick); a hold that ended (instant grip source
 // = the committed frame's mapGripM39, open hand) asks nothing more
 assert(selfCollisionCallsR155==0&&selfCollisionLegacyCallsR155==0);
 g_prop596.entity=reinterpret_cast<void*>(0x1234);step();assert(g_prop596.id&&selfCollisionCallsR155==1);step();assert(selfCollisionCallsR155==2);
 assert(dropCallsR158==0);
 frame.mapGripM39[0]=0;step();assert(!g_prop596.id&&selfCollisionCallsR155==2);step();assert(selfCollisionCallsR155==2);
 // R158: the open hand (not stowed) posts exactly one drop request; the servo hold carries no trail velocity
 assert(dropCallsR158==1&&dropHandVelocityR158==0);step();assert(dropCallsR158==1);
 // the legacy F5 hold (not instant) reaches the helper too but it is a no-op there (same guard in production)
 setup();frame.propRequested5916c1[0].p.x=.1;step();assert(g_prop596.id&&selfCollisionCallsR155==0&&selfCollisionLegacyCallsR155==1);
#endif
 for(int fault=0;fault<9;++fault){instant();switch(fault){
  case 0:g_ctx_menu=true;break;case 1:animation_filter::nativeBodyOwned112=true;break;case 2:animation_filter::kick=true;break;
  case 3:g_cal_state=0;break;case 4:nativeReference=true;break;case 5:contextAllowed=false;break;
  case 6:kcd2vr::mutual_admission_t25::pickupClaimsD72[0]=102;break;case 7:frame.mapFoldTrackedM39=false;break;
  case 8:frame.stamp.epoch=2;break;}
  step();if(fault==7){assert(g_prop596.id&&velocityCalls==0&&stowCalls==0);now=1081;StagedPropTick596();}
  assert(!g_prop596.id&&stowCalls==0&&placeCalls==0);}
 instant();selectedFistsD80=true;frame.pickupTriggerD80[1]=1;step();assert(g_prop596.id&&stowCalls==0);
 frame.pickupTriggerD80[0]=1;step();assert(!g_prop596.id&&stowCalls==0); // exact held hand can become a real fist
 instant();frame.allowed[0].p=frame.propRequested5916c1[0].p={.34,0,0};step();frame.allowed[0].p=frame.propRequested5916c1[0].p={0,0,0};
 for(unsigned n=0;n<16;++n)step();assert(!g_prop596.id&&stowCalls==1);step();assert(stowCalls==1);
#ifndef PROP_ADAPTER_CHECKED_IN_PRE_R96
 // R158: a chest stow never posts a drop (the item is in the inventory)
 instant();g_prop596.entity=reinterpret_cast<void*>(0x1234);frame.allowed[0].p=frame.propRequested5916c1[0].p={.34,0,0};step();
 frame.allowed[0].p=frame.propRequested5916c1[0].p={0,0,0};
 for(unsigned n=0;n<16;++n)step();assert(!g_prop596.id&&stowCalls==1&&dropCallsR158==0);
#endif
 std::puts("PASS verbatim staged consumer: native drive, one-shot stow, loss, legacy F5, instant F5 OFF, independent epoch, exact claim and current context");
 return 0; // Also included as a named fixture function by pickup admission.
}
