// Actual display-focus adapter and actual bridge consumer; camera/native-memory
// leaves are controlled. No native action or headset rendering is simulated.
#define KCD2VR_INPUT67_NO_MAIN
#include "subsystems/alchemy/input67_test.cpp"
#include "interaction_alignment.h"
#include "vrik/visual_ray_d75.h"
#include "subsystems/alchemy/native_selection68.h"
#include "subsystems/hud_ui_controls/runtime/alchemy_head_name_d75.h"
#include <cstring>

static Frame headFrame75{};
static bool camera75=true,frameReadable75=true;
static bool cameraRecenterPending75=false,cameraRecenterRequested75=false;
static float eye75[3]{0,0,0},forward75[3]{0,1,0};
static void(*afterVisualRead75)()=nullptr;
static void(*afterSelectedRead75)()=nullptr;
static std::uint64_t cameraGeneration75=9,rayGeneration75=9,rayEpoch75=5;
namespace kcd2vr {
class CameraHook {
public:
 static std::uint64_t CurrentFamilyGenerationR24() noexcept {return cameraGeneration75;}
 static bool GetLatestVisualRayD75(visual_ray_d75::Sample& out,std::uint64_t age){
  float matrix[3][4]{};
  for(int i=0;i<3;++i){matrix[i][3]=eye75[i];matrix[i][1]=forward75[i];}
  out=visual_ray_d75::make(matrix,headFrame75.tick,rayEpoch75,rayGeneration75,headFrame75.sequence);
  if(afterVisualRead75)afterVisualRead75();
  return camera75&&!cameraRecenterPending75&&!cameraRecenterRequested75&&
   visual_ray_d75::current(out,headFrame75.tick,tracking59c6::epoch.load(),cameraGeneration75,age);
 }
};
namespace frik_pose_probe {
bool WorkstationFrame46(workstation_sample46::Frame& out) noexcept {out=headFrame75;return frameReadable75;}
}
namespace interaction_alignment {
namespace selection68=subsystems::alchemy::native_selection68;
namespace craft68=subsystems::alchemy::bridge67;
static std::atomic<bool> g_enabled{true},g_selection_ready68{true};
static std::uintptr_t currentInstance75=0;
static std::uint64_t currentLifetime75=0;
static std::uintptr_t NomineeAlchemyInstance68() noexcept {return currentInstance75;}
static bool NomineeActivityCurrent68(const selection68::Table& table) noexcept {
 return currentInstance75==table.instance&&currentLifetime75==table.lifetime;
}
template<class T>static bool SelectionRead68(std::uintptr_t p,T& out) noexcept {
 if(!p)return false;std::memcpy(&out,reinterpret_cast<void*>(p),sizeof(out));
 if(afterSelectedRead75){auto callback=afterSelectedRead75;afterSelectedRead75=nullptr;callback();}
 return true;
}
static bool SelectionVft68(std::uintptr_t p,std::uintptr_t value) noexcept {
 std::uintptr_t observed=0;return SelectionRead68(p,observed)&&observed==value;
}
#ifndef KCD2VR_NAME_ADAPTER_D75
#define KCD2VR_NAME_ADAPTER_D75 "../src/interaction_alchemy_names_d75.inl"
#endif
#include KCD2VR_NAME_ADAPTER_D75
}
}
namespace ia75=kcd2vr::interaction_alignment;
namespace b75=kcd2vr::subsystems::alchemy::bridge67;
static std::array<unsigned char,0x400> station75{};
static std::array<unsigned char,0x100> player75{};
static unsigned checks75=0,failures75=0;
static void ck75(bool yes,const char* why){++checks75;if(!yes){++failures75;std::printf("FAIL %u %s\n",checks75,why);}}
template<class T>static void put75(auto& memory,std::size_t offset,T value){std::memcpy(memory.data()+offset,&value,sizeof(value));}
static void publish75(Fixture& q){
 auto& t=q.in.nativeFrame.table;b75::table.publish(t);
 for(unsigned i=0;i<n::Capacity;++i)b75::offers[i].publish({t.instance,t.player,t.lifetime,q.in.nativeFrame.offers[i]});
 headFrame75=q.f;
}
static void box75(Fixture& q,int type,float x,float y,float z,float extent=.05f){
 auto& offer=q.in.nativeFrame.offers[type];offer={};offer.ref=q.in.nativeFrame.table.refs[type];
 offer.tick=q.in.now;offer.valid=offer.boundsValid=true;
 for(unsigned j=0;j<3;++j){offer.bounds.local[j]=-extent;offer.bounds.local[j+3]=extent;offer.bounds.world[4*j+j]=1;}
 offer.bounds.world[3]=x;offer.bounds.world[7]=y;offer.bounds.world[11]=z;
}
static void reset75(Fixture& q){
 q=Fixture{};station75={};player75={};camera75=frameReadable75=true;afterVisualRead75=nullptr;
 cameraRecenterPending75=cameraRecenterRequested75=false;afterSelectedRead75=nullptr;
 cameraGeneration75=rayGeneration75=9;rayEpoch75=5;
 eye75[0]=eye75[1]=eye75[2]=0;forward75[0]=forward75[2]=0;forward75[1]=1;
 b75::enabled=true;ia75::g_enabled=ia75::g_selection_ready68=true;
 kcd2vr::minigame_hands45::alchemyMode67=kcd2vr::minigame_hands45::SixDofVrik;
 kcd2vr::minigame_hands45::modeEpoch67=q.in.modeEpoch;kcd2vr::tracking59c6::epoch=q.f.sourceEpoch;
 auto& t=q.in.nativeFrame.table;t.instance=reinterpret_cast<std::uintptr_t>(station75.data());
 t.playerEntity=reinterpret_cast<std::uintptr_t>(player75.data());
 q.in.context.instance=t.instance;q.in.context.tableGeneration=t.lifetime;
 q.in.action.instance=t.instance;
 ia75::currentInstance75=t.instance;ia75::currentLifetime75=t.lifetime;
 put75(player75,0,std::uintptr_t(0x4072C78));put75(player75,0xC,unsigned(42));
 for(unsigned i=0;i<n::Capacity;++i)put75(station75,0x150+4*i,t.refs[i].entity);
 for(auto& offer:q.in.nativeFrame.offers)offer={};
 b75::nomination.publish({});publish75(q);
}
static bool read75(Fixture& q,int slot,int side=-1){
 ia75::AlchemyHandNameFocusD72 out{};
 return ia75::ReadAlchemyHandNameFocusD72(q.in.now,out)&&out.entity==unsigned(100+slot)&&out.type==slot&&out.side==side&&out.player==42;
}
static bool empty75(Fixture& q){ia75::AlchemyHandNameFocusD72 out{};return !ia75::ReadAlchemyHandNameFocusD72(q.in.now,out)&&out.entity==0;}
int main(){
 Fixture q;reset75(q);box75(q,8,0,1,0);publish75(q);
 ck75(read75(q,8),"head names exact native ingredient with no hand nomination");
 // Neither an empty-hand state nor the previous +304 action is hover authority.
 put75(station75,0x304,unsigned(2));q.f.hand[0].valid=q.f.hand[1].valid=false;publish75(q);
 ck75(read75(q,8),"head names survive unavailable hands and unrelated previous action type");
 for(int slot:{0,5,7,8,9,13,14,15,16,19}){
  reset75(q);box75(q,slot,1,0,0);forward75[0]=1;forward75[1]=0;publish75(q);
  ck75(read75(q,slot),"head turning to far-right native station bounds shows that object's name");
  q.in.nativeFrame.offers[slot].bounds.world[3]=-1;forward75[0]=-1;publish75(q);
  ck75(read75(q,slot),"head turning to far-left native station bounds shows that object's name");
 }
 reset75(q);box75(q,7,0,1.4f,0);box75(q,8,0,.8f,0);publish75(q);
 ck75(read75(q,8),"nearest intersected native box wins regardless of table order");
 q.in.nativeFrame.offers[8].bounds.world[3]=.2f;publish75(q);
 ck75(read75(q,7),"off-axis object does not steal head focus");
 reset75(q);box75(q,8,1,1,0);eye75[0]=1;publish75(q);
 ck75(read75(q,8),"visual HMD translation is used instead of native Henry head origin");
 for(int fault=0;fault<22;++fault){
  reset75(q);box75(q,8,0,1,0);publish75(q);
  switch(fault){
   case 0:q.in.nativeFrame.offers[8].tick=q.in.now-81;publish75(q);break;
   case 1:q.in.nativeFrame.offers[8].tick=q.in.now+1;publish75(q);break;
   case 2:q.in.nativeFrame.offers[8].boundsValid=false;publish75(q);break;
   case 3:q.in.nativeFrame.offers[8].bounds.world[7]=2.2f;publish75(q);break;
   case 4:q.in.nativeFrame.offers[8].bounds.world[7]=-1;publish75(q);break;
   case 5:q.in.nativeFrame.offers[8].bounds.world[0]=std::numeric_limits<float>::quiet_NaN();publish75(q);break;
   case 6:{auto t=q.in.nativeFrame.table;++t.lifetime;b75::table.publish(t);break;}
   case 7:put75(station75,0x150+4*8,unsigned(999));break;
   case 8:++ia75::currentLifetime75;break;
   case 9:ia75::currentInstance75=0;break;
   case 10:kcd2vr::minigame_hands45::alchemyMode67=kcd2vr::minigame_hands45::Native;break;
   case 11:camera75=false;break;
   case 12:headFrame75.allowed=false;break;
   case 13:headFrame75.tick=q.in.now-81;break;
   case 14:++kcd2vr::tracking59c6::epoch;break;
   case 15:afterVisualRead75=[](){++kcd2vr::tracking59c6::epoch;};break;
   case 16:afterVisualRead75=[](){++kcd2vr::minigame_hands45::modeEpoch67;};break;
   case 17:afterVisualRead75=[](){++headFrame75.poseEpoch;};break;
   case 18:ia75::g_enabled=false;break;
   case 19:++cameraGeneration75;break;
   case 20:afterVisualRead75=[](){++cameraGeneration75;};break;
   case 21:++rayEpoch75;break;
  }
  ck75(empty75(q),"stale/foreign/invalid/context-switched visual focus clears without action");
 }
 reset75(q);q.in.handsOnlyD53=q.in.proximityCraft1=true;
 box75(q,7,.08f,0,0);box75(q,8,0,1,0);q.grip(0,1);q.in.palm[0].point={.08,0,0};q.in.palm[1].point={2,0,0};
 auto action=q.step();b75::nomination.publish(action.nomination);publish75(q);
 b75::Nomination observed{};const auto& table=q.in.nativeFrame.table;
 ck75(b75::selectionOwner68(table,observed,q.in.now)==0&&observed.decision.ref.slot==7,"actual hand consumer owns herb7 before label read");
 ck75(read75(q,8),"head names herb8 while real hand nominee remains herb7");
 ck75(b75::selectionOwner68(table,observed,q.in.now)==0&&observed.decision.ref.slot==7&&!action.use,"name read neither redirects hand selection nor emits Use");
 forward75[1]=-1;
 ck75(read75(q,7,0),"qualified hand name remains fallback when head misses table bounds");
 for(int request=0;request<2;++request){
  reset75(q);box75(q,8,0,1,0);publish75(q);
  afterSelectedRead75=request?+[](){cameraRecenterRequested75=true;}:+[](){cameraRecenterPending75=true;};
  ck75(empty75(q),"recenter pending/requested during final native read revokes old head-name focus");
 }
 reset75(q);box75(q,8,0,1,0);publish75(q);
 afterSelectedRead75=[](){++headFrame75.sequence;};
 ck75(read75(q,8),"next rendered frame in the same source/camera lifetime does not revoke head-name focus");
 std::printf("D75 head names: %u checks, %u failures; actual adapter/bridge, native and camera leaves controlled; headset NOT_RUN\n",checks75,failures75);
 return failures75?1:0;
}
