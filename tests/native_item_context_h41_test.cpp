// Actual current-reader bodies; owned packet/tracking leaves modeled.
#include "subsystems/alchemy/finish_observer_cb1.h"
#include "subsystems/throw_drop/grip_stow_cb1.h"
#include "activity_input163.h"
#include <atomic>
#include <cstring>
#include <cassert>
#include <cstdio>
static std::uintptr_t base=0x10000000;static bool ready=true,packetKnown=true,trackKnown=true,handUsable=true;
static std::atomic<void*> g_henry_item_attach_manager{},g_last_player_entity{},g_committed_character6689{};
static std::atomic<std::uintptr_t> g_player_char37{};
static std::atomic<std::uint64_t> g_pose_epoch6689{7},g_weapon_request_epoch6694{8},g_pose_policy_generation57{2};
static std::atomic<bool> g_gameplay_allowed{true},g_ctx_menu{false},g_ctx_dialogue{false},g_ctx_cutscene{false},g_ctx_reading{false},g_ctx_scripted{false};
static std::atomic<int> g_cal_state{2};
namespace kcd2vr::retail_build112{bool Ready(std::uintptr_t b)noexcept{return ready&&b==base;}bool Verify(std::uintptr_t b)noexcept{return Ready(b);}}
namespace kcd2vr::diagnostic_trace{void Write(const char*)noexcept{}}
struct CommittedFrame6695{struct Stamp{std::uintptr_t actor=0;std::uint64_t epoch=0,tick=0;}stamp;bool valid[2]{true,true};std::uintptr_t doorPlayer38=0;std::uint64_t doorTrackingEpoch66=9;};
struct TrackingSnapshot{bool allowed=true,head_valid=true,head_held=false,left_valid=true,right_valid=true,left_held=false,right_held=false;std::uint64_t tick=1000,source_epoch=9;};
static CommittedFrame6695 packet;static TrackingSnapshot tracking;
static std::uintptr_t WhBase()noexcept{return base;}
template<class T>bool Read(const void*p,T&out)noexcept{if(!p)return false;std::memcpy(&out,p,sizeof(out));return true;}
static bool PhysVft6698(void*p,std::uintptr_t rva)noexcept{std::uintptr_t v=0;return Read(p,v)&&v==base+rva;}
static bool ReadCommittedPacket6695(CommittedFrame6695&out)noexcept{out=packet;return packetKnown;}
static bool ReadTrackingSnapshot(TrackingSnapshot&out)noexcept{out=tracking;return trackKnown;}
static bool TrackingHandUsable111(int,std::uint64_t)noexcept{return handUsable;}
using namespace kcd2vr;
#include "../src/subsystems/interactions/native_item_context_h41.inl"
#include "../src/subsystems/alchemy/finish_observer_cb1.cpp"
static unsigned char manager[0x40]{},playerObject[0x9D8]{},actor[0x20]{},alchemy[0x30]{};static int checks=0,nativeCalls=0;
template<class T>static void put(unsigned char*p,int off,T v){std::memcpy(p+off,&v,sizeof(v));}
#define CK(x) do{++checks;assert(x);}while(0)
static bool __fastcall nativeFinish(void*,void*,const unsigned char*,unsigned char*,int*){++nativeCalls;return true;}
static void reset(){
 ready=packetKnown=trackKnown=handUsable=true;g_gameplay_allowed=true;g_cal_state=2;g_ctx_menu=g_ctx_dialogue=g_ctx_cutscene=g_ctx_reading=g_ctx_scripted=false;
 put(manager,0,base+0x3EC9958);put(manager,0x38,(void*)playerObject);put(playerObject,0,base+0x3AA4B98);put(playerObject,0x9D0,(void*)manager);put(playerObject,0x38,(void*)actor);put(playerObject,0x30,std::uint32_t(10));put(actor,0,base+0x4072C78);put(actor,0xC,std::uint32_t(10));put(playerObject,0x668,std::uintptr_t(0xBEEF));put(alchemy,0,base+0x3F60B08);put(alchemy,0x18,(std::uintptr_t)playerObject);put(alchemy,0x2C,(unsigned char)1);
 g_henry_item_attach_manager=manager;g_last_player_entity=actor;g_committed_character6689=(void*)0xABC;g_player_char37=0xABC;g_pose_epoch6689=7;g_weapon_request_epoch6694=8;g_pose_policy_generation57=2;kcd2vr::tracking59c6::epoch=9;
 auto&w=kcd2vr::activity_input163::witness175;w.ready=true;w.transitionDepth175=0;w.lifetime175=11;w.active[0]=(std::uintptr_t)alchemy;
 packet={};packet.stamp={0xABC,7,1000};packet.doorPlayer38=(std::uintptr_t)actor;packet.doorTrackingEpoch66=9;tracking={};
}
int main(){namespace af=kcd2vr::alchemy_finish_cb1;namespace st=kcd2vr::grip_stow_cb1;namespace gp=kcd2vr::physics::grab_producer60;
 reset();gp::Owner o{};o.side=0;o.player=(std::uintptr_t)actor;o.poseEpoch=7;o.trackingEpoch=9;st::Identity i{};af::Context c{};
 HenryItemContextH41 h{};CK(ReadHenryItemContextH41(h));
 CK(GripStowCurrentH41(o,1000,i)&&i.player==10&&i.actor==(std::uintptr_t)actor&&i.nativePlayer==(std::uintptr_t)playerObject&&i.body==7&&i.tracking==9&&i.request==8);
 CK(AlchemyFinishCurrentH41(c)&&c.player==(std::uintptr_t)playerObject&&c.actor==(std::uintptr_t)actor&&c.soul==0xBEEF&&c.lifetime==11);
 af::SetCurrentReader(&AlchemyFinishCurrentH41);af::SetEnabled(true);af::g_original=nativeFinish;
 CK(af::Detour(nullptr,(void*)0xBEEF,nullptr,nullptr,nullptr));af::Result result{};CK(af::Last(result)&&result.valid&&result.context.instance==(std::uintptr_t)alchemy&&nativeCalls==1);
 CK(af::Detour(nullptr,(void*)0xBADD,nullptr,nullptr,nullptr));CK(af::Count()==1&&nativeCalls==2);
 // Recipes remain native while minigame gameplay-input ownership is false.
 g_gameplay_allowed=false;CK(AlchemyFinishCurrentH41(c));CK(!GripStowCurrentH41(o,1000,i));g_gameplay_allowed=true;
 ready=false;CK(!AlchemyFinishCurrentH41(c)&&!GripStowCurrentH41(o,1000,i));reset();
 put(playerObject,0x9D0,(void*)nullptr);CK(!AlchemyFinishCurrentH41(c)&&!GripStowCurrentH41(o,1000,i));reset();
 put(playerObject,0x30,std::uint32_t(11));CK(!AlchemyFinishCurrentH41(c)&&!GripStowCurrentH41(o,1000,i));reset();
 put(playerObject,0x38,(void*)manager);CK(!AlchemyFinishCurrentH41(c)&&!GripStowCurrentH41(o,1000,i));reset();
 put(alchemy,0x18,(std::uintptr_t)manager);CK(!AlchemyFinishCurrentH41(c));reset();put(alchemy,0x2C,(unsigned char)0);CK(!AlchemyFinishCurrentH41(c));reset();
 kcd2vr::activity_input163::witness175.transitionDepth175=1;CK(!AlchemyFinishCurrentH41(c));reset();put(playerObject,0x668,std::uintptr_t(0));CK(!AlchemyFinishCurrentH41(c));reset();
 auto old=o;o.player=(std::uintptr_t)manager;CK(!GripStowCurrentH41(o,1000,i));o=old;o.poseEpoch++;CK(!GripStowCurrentH41(o,1000,i));o=old;o.trackingEpoch++;CK(!GripStowCurrentH41(o,1000,i));o=old;
 packet.stamp.epoch++;CK(!GripStowCurrentH41(o,1000,i));reset();packet.doorPlayer38=(std::uintptr_t)manager;CK(!GripStowCurrentH41(o,1000,i));reset();
 packet.stamp.tick=919;CK(!GripStowCurrentH41(o,1000,i));reset();packet.stamp.tick=920;CK(GripStowCurrentH41(o,1000,i));reset();tracking.tick=919;CK(!GripStowCurrentH41(o,1000,i));reset();
 tracking.head_held=true;CK(!GripStowCurrentH41(o,1000,i));reset();tracking.right_held=true;CK(!GripStowCurrentH41(o,1000,i));reset();g_ctx_cutscene=true;CK(!GripStowCurrentH41(o,1000,i));reset();
 g_player_char37=0xBAD;CK(!GripStowCurrentH41(o,1000,i));reset();g_pose_policy_generation57=3;CK(!GripStowCurrentH41(o,1000,i));reset();
 std::printf("H41 actual current readers+recipe detour checks=%d PASS\n",checks);
}
