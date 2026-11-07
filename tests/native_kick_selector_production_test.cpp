#define main RetainedBridges168Main
#include "host_retained_bridges168.cpp"
#undef main
#ifndef MEM_RESERVE
#define MEM_RESERVE 0x2000
#endif
#ifndef MEM_RELEASE
#define MEM_RELEASE 0x8000
#endif
extern "C" __declspec(dllimport) void* __stdcall VirtualAlloc(void*,std::size_t,unsigned long,unsigned long);
extern "C" __declspec(dllimport) int __stdcall VirtualFree(void*,std::size_t,unsigned long);
#define KICK_PRODUCTION_COMPOSED
#include "native_kick_selector_native_test.cpp"
namespace kcd2vr::frik_pose_probe {namespace {
#include "candidate_consumer_fixture.inl"
}}
namespace kf=kick_fixture;
static unsigned actualEdges=0;
static std::vector<std::string> actualNames;
static void __fastcall productionAction(void*,const std::uint64_t* name,int mode,float value){
 ++actualEdges;const std::string text(reinterpret_cast<const char*>(*name));actualNames.push_back(text+":"+std::to_string(mode));
 auto& n=*kf::active;
 if(text=="attack_special"){
  n.fn<kf::Native::Handler>(0x27C0928)(n.inner.data(),nullptr,nullptr,mode,value);return;
 }
 const auto old=hostReturnAddressValue;
 if(text=="prepare_to_attack"){
  hostReturnAddressValue=reinterpret_cast<void*>(WhBase()+0x27C08E6);NativeInputBegin117(n.control.data(),1);
 }else if(text=="attack_primary"){
  hostReturnAddressValue=reinterpret_cast<void*>(WhBase()+0x27C0916);NativeInputRelease117(n.control.data(),1);
 }else assert(false);
 hostReturnAddressValue=old;
}
struct ProductionRig {
 kf::Native native;
 std::array<unsigned char,0x100> manager{},entity{};
 b168::SpecialRequest original{};
 explicit ProductionRig(const char* path):native(path){
  auto& n=native;hostModuleHandleW=n.image;g_wh_pinned104=0;const auto base=WhBase();hostTickCount64=10000;
  kcd2vr::retail_build112::hostRetailVerified112=true;
  kf::put(n.player.data(),0,base+0x3AA4B98);kf::put(n.player.data(),0x38,entity.data());kf::put(entity.data(),0,base+0x4072C78);kf::put(entity.data(),0xC,std::uint32_t(123));
  kf::put(n.player.data(),0x278,n.actor.data());kf::put(n.player.data(),0x9D0,manager.data());kf::put(manager.data(),0,base+0x3EC9958);kf::put(manager.data(),0x38,n.player.data());
  kf::put(n.player.data(),0xB88,n.input.data());kf::put(n.input.data(),0,base+0x3EC99A8);kf::put(n.input.data(),0x10,n.player.data());
  kf::put(n.state.data(),0xCE0,n.actor.data());kf::put(n.enemy.data(),0,base+0x3E3FAC8);kf::put(n.enemyState.data(),0xCE0,n.enemy.data());kf::put(n.enemyState.data(),0xCF8,n.actor.data());
  kf::put(n.state.data(),0x38,1);kf::put(n.state.data(),0x218,-1);kf::put(n.state.data(),0x248,-1);kf::put(n.state.data(),0x278,-1);kf::put(n.state.data(),0x1B8,0);
  g_last_player_entity=entity.data();g_henry_item_attach_manager=manager.data();g_pose_epoch6689=2;g_weapon_request_epoch6694=5;g_fist_epoch101=7;
  g_gameplay_allowed=g_head_valid=g_right_valid=g_left_valid=true;kcd2vr::animation_filter::nativeBodyOwned112=false;
  dg::lab::Set(dg::lab::Key::F3,0);dg::lab::Set(dg::lab::Key::F5,0);g_census_mode=5;g_weapon_control_mode=7;g_combat_experiment175=false;
  g_ctx_menu=g_ctx_dialogue=g_ctx_cutscene=g_ctx_scripted=g_ctx_reading=false;kcd2vr::combat_rules178r10::useHeld=false;
  g_combat_hook_base101=base;g_combat_bypass101=&productionAction;g_combat_input_original101=&productionAction;g_combat_own_patch101.fill(0);
  g_combat_contract101=base;g_native_contract104=base;g_native_zone104=n.fn<decltype(g_native_zone104)>(0x90F824);g_native_fault104=false;
  g_input_contract117=base;g_input_begin117=n.fn<decltype(g_input_begin117)>(0xD4AAC4);g_input_release117=n.fn<decltype(g_input_release117)>(0xD4A99C);g_input_scope117=nullptr;
  g_phys_native6698={};const char* names[]{"prepare_to_attack","attack_primary","attack_abort","block","attack_special"};for(int i=0;i<5;++i)g_phys_native6698.actionNames[i]=reinterpret_cast<std::uint64_t>(names[i]);
  kcd2vr::attack_xi178r8::enabled=false;kcd2vr::combat_rules178r10::kickTransport=3;
  g_guard_manual112=g_guard_physical112=false;g_guard_manual_owner112=g_guard_physical_owner112=nullptr;g_native_guard112={};g_kick_guard_wait_r19={};g_down_kick_dispatch_h24=nullptr;
  TrackingSnapshot t{};t.tick=10000;t.sequence=9;t.allowed=t.head_valid=t.right_valid=t.left_valid=true;t.head_good_tick=t.left_good_tick=t.right_good_tick=10000;g_tracking_frames6695.publish(t);
  g_special_allowed168=true;g_special_context168=8;g_special_pending168=false;
  original={111,10000,8,2,reinterpret_cast<std::uintptr_t>(n.player.data()),reinterpret_cast<std::uintptr_t>(n.actor.data()),reinterpret_cast<std::uintptr_t>(n.state.data())};
 }
 void run(bool candidate){
  auto& n=native;actualEdges=0;actualNames.clear();n.selectedIndex=~0u;n.slots[0]=nullptr;n.queueCalls=n.rpgCalls=0;n.eligible.clear();
  std::memset(n.inner.data()+0x38,0,12);kf::put(n.state.data(),0x2A8,std::uint8_t(0));kf::put(n.state.data(),0x248,-1);kf::put(n.state.data(),0xF8,0);kf::put(n.state.data(),0x188,0);
  g_special_request168.publish(original);g_special_pending168=true;
  if(candidate)NativeSpecialParentFixture();else NativeSpecialTick168();
  if(n.slots[0]){auto row=kf::get<void*>(n.slots[0],0x58);for(unsigned i=0;i<n.rows.size();++i)if(row==n.rows[i].data.data())n.selectedIndex=i;}
 }
 void arm(int cls,int left){
  auto& n=native;n.rclass=cls;n.lclass=left;kf::put(n.held.data(),0x260,std::uint64_t(77));kf::put(n.held.data(),0x268,left==12?std::uint64_t(0):std::uint64_t(88));
  PhysFrame6698 frame{};frame.player=n.player.data();frame.manager=manager.data();frame.request=5;frame.input.tick=10000;frame.input.bodyEpoch=2;frame.input.sample=9;
  frame.profiles[0].key=77;frame.profiles[0].classId=cls;frame.profiles[0].generation=1;g_phys_frame6698.publish(frame);g_phys_ready_sample6698=9;
 }
};
int main(int argc,char** argv){
 const char* path=argc>1?argv[1]:".";
 {ProductionRig r(path);r.run(true);kf::check(actualNames==std::vector<std::string>({"attack_special:1","attack_special:2"})&&r.native.queryInput==2&&!r.native.slots[0],"exact parent empty-hand R3 reaches SPECIAL selector, emits no kick carrier in controlled native ordinary context");}
 {ProductionRig r(path);r.run(false);kf::check(actualNames==std::vector<std::string>({"prepare_to_attack:1","attack_primary:2"})&&r.native.selectedIndex<233&&rowMetaFixture[r.native.selectedIndex].atk==4,"actual source sends original HEAVY prepare/release and native selector chooses actual kick, not heavy punch");const auto edges=actualEdges;NativeSpecialTick168();kf::check(actualEdges==edges&&r.native.queueCalls==1,"same production R3 serial delivers one native queue transaction without retry");}
 {ProductionRig r(path);r.native.onlyPunch=true;r.run(false);kf::check(!r.native.slots[0]&&r.native.rpgCalls==0,"actual source with punch-only native candidates cannot fabricate kick selection");}
 {ProductionRig r(path);r.native.nativeRpgAllowed=false;r.run(false);kf::check(!r.native.slots[0]&&r.native.rpgCalls>0,"actual source preserves native eligibility refusal");}
 {ProductionRig r(path);r.native.nativeQueueAllowed=false;r.run(false);kf::check(!r.native.slots[0]&&r.native.queueCalls>0,"actual source preserves native manager refusal");}
 for(int cls:{1,2,3,4,5,6,7,16}){
  ProductionRig r(path);r.arm(cls,cls==4||cls==7?cls:12);r.run(false);kf::check(r.native.selectedIndex<233&&rowMetaFixture[r.native.selectedIndex].atk==4,"actual armed production path retains current class/group native kick selection");
 }
 {ProductionRig r(path);r.arm(9,12);r.run(false);kf::check(!r.native.slots[0]&&actualEdges==0,"existing ranged-loadout path remains untouched and cannot fire ranged primary as a kick");}
 for(int legacy:{1,2}){ProductionRig r(path);kcd2vr::combat_rules178r10::kickTransport=legacy;r.run(false);kf::check(actualNames==std::vector<std::string>({"attack_special:1","attack_special:2"}),"advanced legacy SPECIAL transport remains selectable unchanged");}
 std::printf("KICK_PRODUCTION_NATIVE %u/%u PASS; controlled native leaves, headset NOT_RUN\n",kf::checks-kf::failures,kf::checks);return kf::failures?1:0;
}
