// Complete production BuildStagedPickup596, with explicit native identity,
// physics and current-pose leaves. No native ABI/physics execution is claimed.
#define main PropFixtureMainD78
#include "prop_adapter5916c1_test.cpp"
#undef main
#include "diagnostics/interaction_receipt59c10.h"
#include <array>
#include <cstring>
#include <limits>
namespace diagnostics {
namespace capture168=kcd2vr::diagnostics::capture168;
namespace interaction_receipt59c10=kcd2vr::diagnostics::interaction_receipt59c10;
}
struct Matrix34V6550 {float m[12]{1,0,0,0,0,1,0,0,0,0,1,0};};
struct Q4 {float x=0,y=0,z=0,w=1;};
static double Len(V3 v){return std::sqrt(v.x*v.x+v.y*v.y+v.z*v.z);}
static V3 Normalize(V3 v){const auto n=Len(v);return {v.x/n,v.y/n,v.z/n};}
static Q4 QFromBasis(V3,V3,V3){return {};}
static std::array<unsigned char,0x200> nativeController{},nativeEntity{},nativeItem{};
static bool nativePinsD78=true,nativeBuildD78=true;
static bool (*g_graspEnabled13)()=+[](){return true;};
static std::atomic<int> g_grab_mode60{2};
static unsigned long GetCurrentThreadId(){return 1;}
static std::uintptr_t WhBase(){return 0x10000000;}
namespace retail_build112 {static bool Ready(std::uintptr_t){return nativeBuildD78;}}
template<class T>static bool Read(const void* p,T& value){if(!p)return false;std::memcpy(&value,p,sizeof(value));return true;}
static bool PhysVft6698(void* p,std::uintptr_t rva){return (p==nativeController.data()&&rva==0x3EC85E0)||(p==nativeItem.data()&&rva==0x3A2DA88);}
static bool LifecycleHenry165(std::uint32_t id){return id==11;}
static bool LifecycleEntity165(std::uintptr_t p,std::uint32_t& id){id=10;return p==reinterpret_cast<std::uintptr_t>(nativeEntity.data());}
static bool PhysEmpty6698(std::uint64_t key){return key==0;}
static bool GetEntityWorldTmV6552(void* p,std::uint32_t id,Matrix34V6550& out){out={};return p==nativeEntity.data()&&id==10;}
static bool PropPhysicsPins5916C1(){return nativePinsD78;}
static int kPropLeaves596[1]{};
template<class T>static bool StageLeafPins596(const T&){return nativePinsD78;}
namespace action_time_owner59c11 {
static int requestedSideR96=-1;
static bool RequestedPickupSideD85(std::uint32_t entity,int& side){side=requestedSideR96;return entity==10&&side>=0&&side<2;}
}
#ifndef PICKUP_BUILD_D78_INC
#define PICKUP_BUILD_D78_INC "pickup_build_d78.inc"
#endif
#include PICKUP_BUILD_D78_INC
static void prepare(){
 setup();action_time_owner59c11::requestedSideR96=-1;g_prop596={};g_stage_tick596=now;g_stage_thread596=1;
 kcd2vr::combat_rules178r10::f5Rung=0;kcd2vr::workstation178r20c1::experimentArmed36=false;
 frame.allowed[0]={};frame.allowed[1].p={1,0,0};
 frame.propRequested5916c1[0]=frame.allowed[0];frame.propRequested5916c1[1]=frame.allowed[1];
 nativePinsD78=nativeBodyD78=nativeBuildD78=true;
 void* entity=nativeEntity.data();void* item=nativeItem.data();std::uint64_t key=12;
 std::memcpy(nativeController.data()+0x38,&entity,sizeof(entity));std::memcpy(nativeController.data()+0x58,&item,sizeof(item));
 std::memcpy(nativeItem.data()+0x30,&key,sizeof(key));
}
#ifndef KCD2VR_PICKUP_BUILD_FIXTURE_ONLY_D81
int main(){
 StagedProp596 p{};prepare();
 assert(BuildStagedPickup596(nativeController.data(),11,true,&p));
 assert(p.side==0&&p.bodyEpoch==frame.stamp.epoch&&p.epoch==p.bodyEpoch&&!g_prop596.id&&!p.instantD72);
 // The D79 regression: selected fist mode with Grip1/Trigger0 is a free pickup hand.
 for(int side=0;side<2;++side){prepare();selectedFistsD80=true;
  frame.allowed[side]={};frame.allowed[1-side].p={1,0,0};frame.pickupTriggerD80[1-side]=1;
  frame.propRequested5916c1[side]=frame.allowed[side];frame.propRequested5916c1[1-side]=frame.allowed[1-side];
  assert(BuildStagedPickup596(nativeController.data(),11,true,&p)&&p.side==side);
  frame.pickupTriggerD80[side]=1;assert(!BuildStagedPickup596(nativeController.data(),11,true,&p));
  frame.pickupTriggerD80[side]=std::numeric_limits<float>::quiet_NaN();assert(!BuildStagedPickup596(nativeController.data(),11,true,&p));
 }
 // Preference admission lives at InstantPickupRequestD72, while an already
 // accepted request may rebuild this exact candidate after preference changes.
 for(int fault=0;fault<12;++fault){prepare();switch(fault){
  case 0:g_ctx_menu=true;break;case 1:animation_filter::nativeBodyOwned112=true;break;
  case 2:frame.mapFoldTrackedM39=false;break;case 3:frame.headEvidence122=false;break;
  case 4:frame.stamp.tick=now-81;break;case 5:frame.mapGripM39[0]=0;break;
  case 6:handFree=false;break;case 7:nativeBodyD78=false;break;
  case 8:nativePinsD78=false;break;case 9:nativeBuildD78=false;break;
  case 10:frame.propRequested5916c1[0].p={.3,0,0};break;case 11:frame.propRequestedValid5916c1[0]=false;break;
 }
 assert(!BuildStagedPickup596(nativeController.data(),11,true,&p)&&!g_prop596.id);
 }
 prepare();assert(!BuildStagedPickup596(nativeController.data(),99,true,&p));
 assert(!BuildStagedPickup596(nativeController.data(),11,false)); // legacy F5 stays opt-in
 // Contact may correct the displayed hand by 18cm. Acquisition must seed the
 // drive at the actual item, with no correction-sized target jump, either hand.
 for(int side=0;side<2;++side){prepare();frame.mapGripM39[1-side]=0;
  frame.propRequested5916c1[side].p={.05,0,0};frame.allowed[side].p={.23,0,0};
  assert(BuildStagedPickup596(nativeController.data(),11,true,&p)&&p.side==side);
  const auto initial=ph::compose(frame.propRequested5916c1[side],p.offset);
  assert(pm::length(initial.p)<1e-8&&pm::length(p.drive.goal.p)<1e-8);
  frame.propRequested5916c1[side].p.x+=.01;
  assert(std::fabs(ph::compose(frame.propRequested5916c1[side],p.offset).p.x-.01)<1e-8);
 }
 prepare();frame.allowed[0].p={0,0,0};frame.propRequested5916c1[0].p={.3,0,0};
 assert(!BuildStagedPickup596(nativeController.data(),11,true,&p)); // visible correction cannot grant distant reach
 prepare();prop5916::enabled=false;frame.allowed[0].p={.05,0,0};frame.propRequested5916c1[0].p={.3,0,0};
 assert(BuildStagedPickup596(nativeController.data(),11,true,&p)&&!p.physics);
 assert(pm::length(ph::compose(frame.allowed[0],p.offset).p)<1e-8); // explicit kinematic mode still uses its actual driver
 prop5916::enabled=true;
 std::puts("PASS actual pickup build: explicit F5-OFF admission, exact body epoch, 13 identity/input/context refusals, legacy experiment preserved");
}
#endif
