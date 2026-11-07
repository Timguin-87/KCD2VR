// Executes the production callbacks with synthetic memory and a host register bank.
// Does not execute a retail DLL instruction or claim in-game hook acceptance.
#include "../src/frik_pose_probe.cpp"
#include <cassert>
#include <cstdio>
#include <string>
static std::string traceH31;
namespace kcd2vr::diagnostic_trace {void Write(const char* line) noexcept {traceH31=line?line:"";}}
// Mode and native-frame pump leaves are explicit activation witnesses here;
// all remaining absent services are linked once from the external fixture.
namespace kcd2vr::animation_filter {
void SetMode(SuppressionMode) noexcept {} bool Install() noexcept{return false;} bool Ready() noexcept{return false;}
}
namespace kcd2vr::native_stereo_outer_lab {bool EnsurePhysicalFramePump6698() noexcept{return true;}}
namespace kcd2vr::retail_build112 {extern bool hostRetailVerified112;}
using namespace kcd2vr::frik_pose_probe;
namespace dg=kcd2vr::diagnostics;
template<class T>static void put168(unsigned char* p,T v){std::memcpy(p,&v,sizeof(v));}
static unsigned assertions=0;
static void check168At(bool b,int line){++assertions;if(!b)std::fprintf(stderr,"FAIL check=%u fixtureLine=%d\n",assertions,line);assert(b);}
#define check168(b) check168At((b),__LINE__)
static unsigned nativeCalls=0;static int edges[4]{};static std::string namesH24[4]{};
static unsigned char* kickStateR19=nullptr;static unsigned blockReleasesR19=0;
static std::array<unsigned char,0x40> downControllerH24{};
static std::vector<int> heavyEdgesH24;
static bool guardFencedH24=true;static bool prepareBoundaryH31=true,releaseBoundaryH31=true;
static void __fastcall downLeafH24(void*,int input){heavyEdgesH24.push_back(input);}
static void __fastcall zoneR19(void*,int zone,std::int64_t,bool){put168(kickStateR19+0xF8,zone);}
static void __fastcall action168(void*,const std::uint64_t* name,int mode,float){
 if(name&&std::string(reinterpret_cast<const char*>(*name))=="block"){assert(mode==2);++blockReleasesR19;return;}
 assert(name);
 const std::string actualH24(reinterpret_cast<const char*>(*name));
 assert(actualH24=="attack_special"||actualH24=="prepare_to_attack"||actualH24=="attack_primary");
 assert(dg::intent168::current.source==dg::intent168::Source::R3&&dg::intent168::current.serial==111);
 if(actualH24=="prepare_to_attack"||actualH24=="attack_primary"){
  const auto old=hostReturnAddressValue;hostReturnAddressValue=reinterpret_cast<void*>(WhBase()+
      (actualH24=="prepare_to_attack"?0x27C08E6:0x27C0916));
  guardFencedH24&=NativeKickGuardFenceR19(g_input_scope117->owner.player,GetTickCount64());
  if(actualH24=="prepare_to_attack"){if(prepareBoundaryH31)NativeInputBegin117(downControllerH24.data(),18);}
  else if(releaseBoundaryH31)NativeInputRelease117(downControllerH24.data(),18);
  hostReturnAddressValue=old;
 }
 namesH24[nativeCalls]=actualH24;edges[nativeCalls++]=mode;
}
int main(){
 kcd2vr::clinch_intent_d40::setMode(0); // Retained legacy/distance cases; D40 modes have their own adapter fixture.
 std::vector<unsigned char> image(0x5400000);hostModuleHandleW=image.data();g_wh_pinned104=0;
 const auto base=WhBase();hostTickCount64=10000;kcd2vr::retail_build112::hostRetailVerified112=true;
 std::array<unsigned char,0xE00> player{},state{},enemyState{};
 std::array<unsigned char,0x500> actor{},enemy{};
 std::array<unsigned char,0x100> manager{},entity{},input{},factory{},query{};
 std::array<unsigned char,0x300> hands{};
 put168(player.data(),base+0x3AA4B98);put168(player.data()+0x38,entity.data());
 put168(entity.data(),base+0x4072C78);put168(entity.data()+0xC,std::uint32_t(123));
 put168(player.data()+0x278,actor.data());put168(actor.data(),base+0x3A80610);
 put168(actor.data()+0x1F8,player.data());put168(actor.data()+0x210,state.data());put168(state.data()+0xCE0,actor.data());
 put168(player.data()+0x9D0,manager.data());put168(manager.data(),base+0x3EC9958);put168(manager.data()+0x38,player.data());
 put168(player.data()+0x990,hands.data());put168(hands.data()+0x260,std::uint64_t(77));
 put168(player.data()+0xB88,input.data());put168(input.data(),base+0x3EC99A8);put168(input.data()+0x10,player.data());
 put168(state.data()+0xCF8,enemy.data());put168(enemy.data(),base+0x3E3FAC8);put168(enemy.data()+0x210,enemyState.data());put168(enemyState.data()+0xCE0,enemy.data());
 put168(enemyState.data()+0x38,8);put168(enemyState.data()+0x2D8,std::uint8_t(0));put168(enemyState.data()+0x218,1);put168(enemyState.data()+0x278,2);
 put168(factory.data()+8,actor.data());put168(query.data(),55);put168(query.data()+0x18,enemy.data());put168(image.data()+0x53204B4,55);
 put168(state.data()+0x38,1);put168(state.data()+0x218,-1);put168(state.data()+0x248,-1);put168(state.data()+0x278,-1);put168(state.data()+0x1B8,0);
 g_last_player_entity=entity.data();g_henry_item_attach_manager=manager.data();g_pose_epoch6689=2;g_weapon_request_epoch6694=5;
 g_gameplay_allowed=g_head_valid=g_right_valid=g_left_valid=true;kcd2vr::animation_filter::nativeBodyOwned112=false;
 dg::lab::Set(dg::lab::Key::F3,3);dg::lab::Set(dg::lab::Key::F5,0);g_census_mode=3;g_weapon_control_mode=7;g_f11_test167=0;
 g_combat_experiment175=true;dg::capture168::gate.mask=0;
 PhysFrame6698 frame{};frame.player=player.data();frame.manager=manager.data();frame.request=5;frame.input.tick=10000;frame.input.bodyEpoch=2;
 frame.profiles[0].key=77;frame.profiles[0].classId=1;frame.profiles[0].generation=4;frame.output.active[0]=frame.output.owned[0]=true;g_phys_frame6698.publish(frame);
 TrackingSnapshot tracked{};tracked.sequence=9;tracked.tick=10000;tracked.head_good_tick=tracked.right_good_tick=tracked.left_good_tick=10000;
 tracked.allowed=tracked.head_valid=tracked.right_valid=tracked.left_valid=true;g_tracking_frames6695.publish(tracked);
 g_master_gate_ready168=true;g_master_policy168=3;g_master_used168=0;g_master_veto59c14=true; // R59.16C10R14: the V168 ticket veto under test (INI VrMasterStrikeVeto=1)
 b168::Ticket ticket{1,10000,2,3,77,4,5,9,reinterpret_cast<std::uintptr_t>(player.data()),reinterpret_cast<std::uintptr_t>(actor.data()),reinterpret_cast<std::uintptr_t>(state.data()),reinterpret_cast<std::uintptr_t>(enemy.data()),1,true};
 using namespace DetourModKit::hook;MidContext ctx{};
 for(unsigned i=0;i<10;++i)ctx.registers[i]=0x123400+i;
 gpr(ctx,Gpr::Rsi)=reinterpret_cast<std::uintptr_t>(factory.data());gpr(ctx,Gpr::Rdi)=reinterpret_cast<std::uintptr_t>(query.data());
 auto run=[&](bool expectPass){g_master_ticket168.publish(ticket);gpr(ctx,Gpr::Rax)=0xABCDEF01;auto before=ctx;MasterFactoryGate168(ctx);
   check168(gpr(ctx,Gpr::Rax)==(expectPass?0xABCDEF01:0xABCDEF00));
   for(unsigned i=0;i<10;++i)if(i!=unsigned(Gpr::Rax))check168(ctx.registers[i]==before.registers[i]);check168(ctx.stack==before.stack);};
 run(true);check168(g_master_used168==1);run(false); // consumed once
 ticket.serial=2;ticket.thrust=false;run(false);ticket.thrust=true;
 ticket.origin=2;run(false);ticket.origin=1;
 ticket.tick=9800;run(false);ticket.tick=10000;
 ticket.key=999;run(false);ticket.key=77;
 ticket.policy=2;run(false);ticket.policy=3;
 tracked.right_held=true;g_tracking_frames6695.publish(tracked);run(false);tracked.right_held=false;g_tracking_frames6695.publish(tracked);
 put168(enemyState.data()+0x38,16);put168(enemyState.data()+0x2D8,std::uint8_t(1));run(false);
 put168(enemyState.data()+0x38,8);put168(enemyState.data()+0x2D8,std::uint8_t(0));run(true);++ticket.serial;
 put168(enemyState.data()+0x278,3);run(false);put168(enemyState.data()+0x278,2);
 // Only explicit F4 gameplay enables the rejection. F3/F11 recorders never do.
 g_combat_experiment175=false;run(true);g_f11_test167=2;run(true);g_f11_test167=0;
 dg::capture168::gate.select(dg::capture168::Domain::Combat,true,hostTickCount64);run(true);
 dg::capture168::gate.mask=0;g_combat_experiment175=true;
 put168(factory.data()+8,enemy.data());run(true);put168(factory.data()+8,actor.data());
 put168(query.data(),3);run(true);put168(query.data(),55);
 put168(image.data()+0x53204A8,55);run(true);put168(image.data()+0x53204A8,0); // ambiguous fallback must not be redirected
 kcd2vr::retail_build112::hostRetailVerified112=false;run(true);kcd2vr::retail_build112::hostRetailVerified112=true;
 gpr(ctx,Gpr::Rax)=0xABCDEF00;g_master_ticket168.publish(ticket);MasterFactoryGate168(ctx);check168(gpr(ctx,Gpr::Rax)==0xABCDEF00);
 // R59.16C10R14 veto OFF (the default): the game's own factory result passes unchanged for every VR stroke - a slash, a late
 // thrust, a same-side thrust, a consumed ticket; a native false is never turned into a master strike; registers untouched.
 g_master_veto59c14=false;
 run(true);run(true); // consumed ticket still passes
 ticket.thrust=false;run(true);ticket.thrust=true;
 ticket.tick=9000;run(true);ticket.tick=10000;
 ticket.origin=2;run(true);ticket.origin=1;
 gpr(ctx,Gpr::Rax)=0xABCDEF00;g_master_ticket168.publish(ticket);MasterFactoryGate168(ctx);check168(gpr(ctx,Gpr::Rax)==0xABCDEF00);
 g_master_veto59c14=true;
 // V176: these retained assertions verify the native route outside F4.
 // F4 now owns procedural R3; do not inherit the prior master-test mode here.
 g_combat_experiment175=false;
 // Actual native-frame semantic consumer: balanced edges; no keyboard injection.
 g_combat_hook_base101=base;g_combat_bypass101=&action168;g_combat_input_original101=&action168;g_combat_own_patch101.fill(0);
 g_phys_native6698={};g_phys_native6698.actionNames[4]=reinterpret_cast<std::uint64_t>("attack_special");
 g_phys_native6698.actionNames[0]=reinterpret_cast<std::uint64_t>("prepare_to_attack");
 g_phys_native6698.actionNames[1]=reinterpret_cast<std::uint64_t>("attack_primary");
 g_phys_native6698.actionNames[3]=reinterpret_cast<std::uint64_t>("block");
 kickStateR19=state.data();g_native_contract104=base;g_native_zone104=&zoneR19;
 put168(downControllerH24.data()+0x18,actor.data());
 put168(image.data()+0x5321014,18);put168(image.data()+0x5321010,17);put168(image.data()+0x5321018,19);
 g_input_contract117=base;g_input_begin117=&downLeafH24;g_input_release117=&downLeafH24;
 g_special_allowed168=true;g_special_context168=8;
 b168::SpecialRequest special{111,10000,8,2,reinterpret_cast<std::uintptr_t>(player.data()),reinterpret_cast<std::uintptr_t>(actor.data()),reinterpret_cast<std::uintptr_t>(state.data())};
 g_special_request168.publish(special);g_special_pending168=true;NativeSpecialTick168();check168(nativeCalls==2&&edges[0]==1&&edges[1]==2&&!g_special_pending168);
 for(int metadataPhaseH14:{128,256}){
  put168(state.data()+0x38,metadataPhaseH14);put168(state.data()+0x278,7);put168(state.data()+0x1B8,1);
  NativeOwner104 ownerH14{};nc104::Observed observedH14{};
  check168(NativeResolve104(player.data(),ownerH14)&&NativeObserve104(ownerH14,observedH14)&&
      nc104::blockMetadataH14(observedH14)&&!nc104::valid(observedH14));
  g_special_request168.publish(special);g_special_pending168=true;NativeSpecialTick168();
  check168(nativeCalls==2&&!g_special_pending168); // readable context is not R3 write authority
  // U16: only our exact live physical guard may forward the original special.
  // The fake native leaf does NOT choose a kick; this checks input ownership only.
  const auto oldTransportU16=kcd2vr::combat_rules178r10::kickTransport.exchange(3);
  g_native_guard112={};g_native_guard112.owner=ownerH14;g_native_guard112.held=true;
  g_guard_physical112=true;g_guard_physical_owner112=player.data();g_guard_manual112=false;
  put168(state.data()+0x630,base+0x3B0A968);put168(image.data()+0x3B0A970,base+0x1A7DDE0);
  const unsigned char getterR19[]{0x0F,0xB6,0xC2,0x0F,0xB6,0x44,0x08,0x18,0xC3};
  std::memcpy(image.data()+0x1A7DDE0,getterR19,sizeof(getterR19));put168(state.data()+0x648,std::uint8_t(1));
  g_kick_guard_wait_r19={};nativeCalls=0;g_special_request168.publish(special);g_special_pending168=true;NativeSpecialTick168();
  check168(nativeCalls==0&&g_special_pending168&&NativeKickGuardFenceR19(player.data(),10000));
  // Actual guard consumer between original R3 release and native LT-up ACK.
  // This posture would otherwise press a new block from writable phase128.
  const auto oldCensusR19=g_census_mode.load(),oldWeaponModeR19=g_weapon_control_mode.load();
  g_census_mode=5;g_weapon_control_mode=0;put168(hands.data()+0x260,std::uint64_t(0));
  FistFrame101 guardR19{};guardR19.player=player.data();guardR19.request=g_fist_epoch101.load();
  auto& gs=guardR19.sample;gs.valid=gs.empty[0]=gs.empty[1]=true;gs.epoch=2;gs.actor=reinterpret_cast<std::uintptr_t>(entity.data());
  gs.sequence=42;gs.tick=10000;gs.headOrientation={0,0,0,1};gs.controller[0]={.15,-.2,-.2};gs.controller[1]={-.15,-.2,-.2};
  gs.grip[0]=gs.grip[1]=gs.trigger[0]=gs.trigger[1]=1;g_fist_frame101.publish(guardR19);
  g_native_guard112.fists175.actor=gs.actor;g_native_guard112.fists175.epoch=2;
  g_native_guard112.fists175.sequence=41;g_native_guard112.fists175.tick=9990;
  g_native_guard112.fists175.raisedSince=9900;g_native_guard112.fists175.guarding=true;
  put168(state.data()+0x38,128);put168(state.data()+0x278,0);
  const auto releasedR19=blockReleasesR19;NativeFistGuardTick175();
  check168(!g_native_guard112.held&&blockReleasesR19==releasedR19&&g_special_pending168);
  g_census_mode=oldCensusR19;g_weapon_control_mode=oldWeaponModeR19;put168(hands.data()+0x260,std::uint64_t(77));
  put168(state.data()+0x38,metadataPhaseH14);put168(state.data()+0x278,7);
  NativeSpecialTick168();check168(nativeCalls==0&&g_special_pending168); // native LT-up not acknowledged
  put168(state.data()+0x648,std::uint8_t(0));put168(state.data()+0x38,1);put168(state.data()+0x278,0);
  NativeSpecialTick168();
  check168(nativeCalls==2&&edges[0]==1&&edges[1]==2&&!g_special_pending168);
  check168(namesH24[0]=="prepare_to_attack"&&namesH24[1]=="attack_primary"&&
      !g_down_kick_dispatch_h24.load());
  check168(heavyEdgesH24.size()>=2&&heavyEdgesH24[heavyEdgesH24.size()-2]==18&&
      heavyEdgesH24.back()==18&&guardFencedH24&&!g_input_scope117);
  int nativeZoneR19=-1;Read(state.data()+0xF8,nativeZoneR19);check168(nativeZoneR19==5);
  g_native_guard112={};g_native_guard112.owner=ownerH14;g_native_guard112.owner.epoch++;
  g_native_guard112.held=true;g_guard_physical112=true;g_guard_physical_owner112=player.data();
  put168(state.data()+0x38,metadataPhaseH14);put168(state.data()+0x278,7);nativeCalls=0;
  g_special_request168.publish(special);g_special_pending168=true;NativeSpecialTick168();
  check168(nativeCalls==0&&!g_special_pending168); // old guard cannot acquire R3 authority
  g_native_guard112={};g_guard_physical112=false;g_guard_physical_owner112=nullptr;
  kcd2vr::combat_rules178r10::kickTransport=oldTransportU16;nativeCalls=2;

 }
 // R19: exact guard handoff cannot renew the original R3 or cross authority.
 {
  const auto oldTransportR19=kcd2vr::combat_rules178r10::kickTransport.exchange(3);
  const auto original=special;const auto oldCalls=nativeCalls;const auto oldWeapon=g_weapon_request_epoch6694.load();
  auto waiting=[&](){
   special=original;hostTickCount64=10000;g_special_allowed168=true;g_special_context168=8;
   g_native_guard112={};g_guard_physical112=false;g_guard_physical_owner112=nullptr;g_guard_manual112=false;
   put168(state.data()+0x38,256);put168(state.data()+0x278,7);put168(state.data()+0x1B8,1);
   put168(state.data()+0x218,-1);put168(state.data()+0x2A8,std::uint8_t(0));put168(state.data()+0x2D8,std::uint8_t(0));
   put168(state.data()+0x648,std::uint8_t(1));
   g_kick_guard_wait_r19={special,oldWeapon,g_fist_epoch101.load()};
   g_special_request168.publish(special);g_special_pending168=true;
  };
  waiting();NativeSpecialTick168();check168(nativeCalls==oldCalls&&g_special_pending168);
  b168::SpecialRequest unchanged{};g_special_request168.read(unchanged);check168(unchanged.tick==original.tick);
  hostTickCount64=10121;NativeSpecialTick168();check168(nativeCalls==oldCalls&&!g_special_pending168);
  waiting();g_guard_manual_owner112=player.data();g_guard_manual112=true;NativeSpecialTick168();
  check168(nativeCalls==oldCalls&&!g_special_pending168);
  waiting();++g_weapon_request_epoch6694;NativeSpecialTick168();check168(nativeCalls==oldCalls&&!g_special_pending168);
  g_weapon_request_epoch6694=oldWeapon;
  waiting();++g_special_context168;NativeSpecialTick168();check168(nativeCalls==oldCalls&&!g_special_pending168);
  waiting();put168(state.data()+0x38,32);put168(state.data()+0x278,0);NativeSpecialTick168();
  check168(nativeCalls==oldCalls&&!g_special_pending168);
  g_kick_guard_wait_r19={};g_special_context168=8;g_guard_manual112=false;
  special=original;hostTickCount64=10000;kcd2vr::combat_rules178r10::kickTransport=oldTransportR19;
 }
 put168(state.data()+0x38,1);put168(state.data()+0x278,-1);put168(state.data()+0x1B8,0);
 NativeSpecialTick168();check168(nativeCalls==2&&dg::intent168::current.serial==0);
 g_special_request168.publish(special);g_special_pending168=true;g_special_allowed168=false;NativeSpecialTick168();check168(nativeCalls==2);
 g_special_allowed168=true;special.tick=9800;g_special_request168.publish(special);g_special_pending168=true;NativeSpecialTick168();check168(nativeCalls==2);
 special.tick=10000;special.context=7;g_special_request168.publish(special);g_special_pending168=true;NativeSpecialTick168();check168(nativeCalls==2);
 // A previously queued native special cannot leak across entry into F4.
 special.tick=10000;special.context=8;g_combat_experiment175=true;
 g_special_request168.publish(special);g_special_pending168=true;NativeSpecialTick168();
 check168(nativeCalls==2&&!g_special_pending168);
 // Exact auto-sync caller is rejection-only and only protects a current boxing
 // pair. Test both initiating actors with all recorders off, and no native code.
 g_combat_experiment175=false;g_close_combat_ready175=true;g_census_mode=5;g_weapon_control_mode=0;
 dg::capture168::gate.mask=0;g_fist_fault101=false;g_fist_epoch101=9;
 put168(hands.data()+0x260,std::uint64_t(0));put168(state.data()+8,std::uint8_t(1));put168(enemyState.data()+8,std::uint8_t(1));
 put168(enemyState.data()+0xCF8,actor.data());put168(actor.data()+0x3F0,factory.data());put168(enemy.data()+0x3F0,factory.data());
 put168(image.data()+0x53204E4,35);put168(factory.data()+0x1A,std::uint8_t(0x5A));
 FistFrame101 boxing{};boxing.player=player.data();boxing.request=9;boxing.sample.actor=reinterpret_cast<std::uintptr_t>(entity.data());
 boxing.sample.valid=boxing.sample.empty[0]=boxing.sample.empty[1]=true;boxing.sample.epoch=2;boxing.sample.sequence=42;boxing.sample.tick=10000;
 boxing.sample.grip[0]=boxing.sample.trigger[0]=1;
 auto close=[&](bool rejected){g_fist_frame101.publish(boxing);gpr(ctx,Gpr::Rbx)=reinterpret_cast<std::uintptr_t>(factory.data());
     gpr(ctx,Gpr::Rax)=0x1122334455667701;const auto before=ctx;CloseCombatAutoGate175(ctx);
     check168(gpr(ctx,Gpr::Rax)==(rejected?0x1122334455667700ull:0x1122334455667701ull));
     for(unsigned i=0;i<10;++i)if(i!=unsigned(Gpr::Rax))check168(ctx.registers[i]==before.registers[i]);
     check168(ctx.stack==before.stack&&factory[0x1A]==0x5A);};
 put168(factory.data()+8,actor.data());close(true);check168(dg::capture168::gate.mask==0);
 put168(factory.data()+8,enemy.data());close(true);
 g_close_combat_ready175=false;close(false);g_close_combat_ready175=true;
 g_census_mode=3;close(false);g_census_mode=5;
 g_weapon_control_mode=7;close(false);g_weapon_control_mode=0;
 boxing.sample.grip[0]=0;close(false);boxing.sample.grip[0]=1;
 boxing.sample.tick=9919;close(false);boxing.sample.tick=10000;
 boxing.request=8;close(false);boxing.request=9;
 boxing.sample.epoch=1;close(false);boxing.sample.epoch=2;
 boxing.sample.valid=false;close(false);boxing.sample.valid=true;
 put168(hands.data()+0x260,std::uint64_t(77));close(false);put168(hands.data()+0x260,std::uint64_t(0));
 put168(enemyState.data()+0xCF8,static_cast<void*>(nullptr));close(false);put168(enemyState.data()+0xCF8,actor.data());
 put168(actor.data()+8,std::uint8_t(0)); // unrelated actor bytes cannot serve as combat state
 put168(state.data()+8,std::uint8_t(0));close(false);put168(state.data()+8,std::uint8_t(1));
 put168(image.data()+0x53204E4,20);close(false);put168(image.data()+0x53204E4,35);
 kcd2vr::animation_filter::nativeBodyOwned112=true;close(false);kcd2vr::animation_filter::nativeBodyOwned112=false;
 kcd2vr::animation_filter::nativeKickActive175=true;close(false);kcd2vr::animation_filter::nativeKickActive175=false;
 put168(state.data()+0x218,4);close(false);put168(state.data()+0x218,-1);
 kcd2vr::retail_build112::hostRetailVerified112=false;close(false);kcd2vr::retail_build112::hostRetailVerified112=true;
 gpr(ctx,Gpr::Rax)=0x1122334455667700;const auto nativeFalse=ctx;CloseCombatAutoGate175(ctx);
 check168(std::memcmp(&nativeFalse,&ctx,sizeof(ctx))==0);
 // H24 restoration uses the actual public ingress and native-frame callback.
 // No fixture fabricates selected attack type, damage or stamina.
 g_combat_experiment175=false;g_special_allowed168=true;g_special_context168=8;
 g_guard_manual112=false;g_guard_physical112=false;g_native_guard112={};g_kick_guard_wait_r19={};
 kcd2vr::combat_rules178r10::kickTransport=3;g_pose_epoch6689=2;g_weapon_request_epoch6694=5;
 hostTickCount64=10000;put168(hands.data()+0x260,std::uint64_t(77));
 put168(state.data()+0x38,1);put168(state.data()+0x218,-1);put168(state.data()+0x248,-1);
 put168(state.data()+0x278,-1);put168(state.data()+0x1B8,0);put168(state.data()+0x2A8,std::uint8_t(0));
 tracked.tick=10000;tracked.allowed=tracked.head_valid=tracked.left_valid=tracked.right_valid=true;
 g_tracking_frames6695.publish(tracked);g_gameplay_allowed=true;
 for(int clsH24:{1,2,3,4,5,6,7,16}){
  frame.profiles[0].classId=clsH24;g_phys_frame6698.publish(frame);nativeCalls=0;
  special={111,10000,8,2,reinterpret_cast<std::uintptr_t>(player.data()),reinterpret_cast<std::uintptr_t>(actor.data()),reinterpret_cast<std::uintptr_t>(state.data())};
  g_special_request168.publish(special);g_special_pending168=true;NativeSpecialTick168();
  check168(nativeCalls==2&&namesH24[0]=="prepare_to_attack"&&namesH24[1]=="attack_primary");
 }
 for(int rangedH24:{0,9,10,13,14,15,-1}){
  frame.profiles[0].classId=rangedH24;g_phys_frame6698.publish(frame);nativeCalls=0;
  g_phys_native6698.attackPrepared=g_phys_native6698.heldPrepare57=true;
  put168(state.data()+0xF8,2);const auto blockBeforeH24=blockReleasesR19;
  g_special_request168.publish(special);g_special_pending168=true;NativeSpecialTick168();
  check168(nativeCalls==0&&!g_special_pending168);
  int unchangedZoneH24=-1;Read(state.data()+0xF8,unchangedZoneH24);
  check168(g_phys_native6698.attackPrepared&&unchangedZoneH24==2&&blockBeforeH24==blockReleasesR19);
  g_phys_native6698.attackPrepared=g_phys_native6698.heldPrepare57=false;
 }
 frame.profiles[0].classId=1;g_phys_frame6698.publish(frame);nativeCalls=0;
 // Physical weapon mode may gate ordinary swings, never this qualified R3.
 // Releasing weapon Grip does not revoke the native foot action.
 frame.output.active[0]=frame.output.owned[0]=false;frame.input.grip[0]=0;
 g_phys_frame6698.publish(frame);g_combat_experiment175=true;
 kcd2vr::native_weapon_body_t4::SetEnabled(true);
 g_special_request168.publish(special);g_special_pending168=true;NativeSpecialTick168();
 check168(nativeCalls==2&&namesH24[0]=="prepare_to_attack"&&namesH24[1]=="attack_primary");
 kcd2vr::native_weapon_body_t4::SetEnabled(false);g_combat_experiment175=false;
 // H31: actual R3 callback identifies the first refusal and reaches exact
 // heavy boundaries when allowed. Recorders do not grant input authority.
 {
  using GateH31=b168::KickGateH31;
  const auto savedCaptureH31=dg::capture168::gate.mask.load();
  dg::capture168::gate.select(dg::capture168::Domain::Combat,true,hostTickCount64);
  const auto original=special;
  auto eventH31=[&](){
   b168::Event discard{};while(g_bridge_events168.pop(discard)){}
   nativeCalls=0;g_special_request168.publish(special);g_special_pending168=true;NativeSpecialTick168();
   b168::Event result{};check168(g_bridge_events168.pop(result)&&result.kind==1&&result.serial==111);
   check168(!g_bridge_events168.pop(discard));return result;
  };
  g_special_allowed168=false;g_gameplay_allowed=false;
  auto evH31=eventH31();check168(evH31.reason==unsigned(GateH31::IngressDisabled)&&nativeCalls==0&&evH31.beforePhaseH31==-1);
  g_special_allowed168=true;special.tick=9800;
  evH31=eventH31();check168(evH31.reason==unsigned(GateH31::RequestExpired)&&nativeCalls==0);
  special=original;evH31=eventH31();check168(evH31.reason==unsigned(GateH31::Gameplay)&&nativeCalls==0);
  g_gameplay_allowed=true;kcd2vr::animation_filter::nativeBodyOwned112=true;
  evH31=eventH31();check168(evH31.reason==unsigned(GateH31::NativeBody)&&nativeCalls==0);
  kcd2vr::animation_filter::nativeBodyOwned112=false;
  tracked.head_held=true;g_tracking_frames6695.publish(tracked);
  evH31=eventH31();check168(evH31.reason==unsigned(GateH31::Tracking)&&nativeCalls==0);
  tracked.head_held=false;g_tracking_frames6695.publish(tracked);
  special.actor=std::uintptr_t(1);evH31=eventH31();check168(evH31.reason==unsigned(GateH31::ActorChanged)&&nativeCalls==0);
  special=original;special.state=std::uintptr_t(1);evH31=eventH31();check168(evH31.reason==unsigned(GateH31::StateChanged)&&nativeCalls==0);
  special=original;put168(state.data()+0x2A8,std::uint8_t(1));
  evH31=eventH31();check168(evH31.reason==unsigned(GateH31::NativeRequestPending)&&nativeCalls==0&&evH31.beforePhaseH31==1);
  put168(state.data()+0x2A8,std::uint8_t(0));g_guard_manual_owner112=player.data();g_guard_manual112=true;
  evH31=eventH31();check168(evH31.reason==unsigned(GateH31::ManualGuard)&&nativeCalls==0);
  // Check the production formatter, rather than a duplicate test string.
  g_bridge_events168.push(evH31);ServiceBridges168();
  check168(traceH31.find("gate=MANUAL_GUARD")!=std::string::npos&&traceH31.find("beforePhase=1")!=std::string::npos);
  g_guard_manual112=false;evH31=eventH31();
  check168(nativeCalls==2&&evH31.reason==0&&evH31.inputWitnessH31==3&&evH31.hold==-2);
  prepareBoundaryH31=false;evH31=eventH31();
  check168(nativeCalls==2&&evH31.nativeResult&&evH31.allowed&&evH31.inputWitnessH31==2&&evH31.reason==unsigned(GateH31::BeginNotObserved));
  prepareBoundaryH31=true;releaseBoundaryH31=false;evH31=eventH31();
  check168(nativeCalls==2&&evH31.nativeResult&&evH31.allowed&&evH31.inputWitnessH31==1&&evH31.reason==unsigned(GateH31::ReleaseNotObserved));
  releaseBoundaryH31=true;
  check168(std::string(b168::kickGateNameH31(unsigned(GateH31::BeginNotObserved)))=="HEAVY_BEGIN_BOUNDARY_NOT_OBSERVED");
  dg::capture168::gate.mask=0;nativeCalls=0;
  g_special_request168.publish(special);g_special_pending168=true;NativeSpecialTick168();
  b168::Event absentH31{};check168(nativeCalls==2&&!g_bridge_events168.pop(absentH31));
  dg::capture168::gate.mask=savedCaptureH31;special=original;
 }
 g_r3_last_ingress_h24=0;g_special_pending168=false;
 g_combat_contract101=base;g_fist_fault101=false;
 check168(RequestNativeSpecial168());
 check168(!RequestNativeSpecial168());g_special_pending168=false;
 hostTickCount64=10599;check168(!RequestNativeSpecial168());
 hostTickCount64=10600;check168(RequestNativeSpecial168());g_special_pending168=false;
 hostTickCount64=10000;g_r3_last_ingress_h24=0;
 #include "native_clinch32_cases.inl"
 std::printf("PASS %u production-callback checks: master/auto-clinch radius register preservation/identity/timing/off/native-false and R3 frame-context edges. Retail instructions NOT executed.\n",assertions);
}
