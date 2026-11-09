// Execute the COMPLETE production TU with explicit replacement native leaves.
#include "../src/subsystems/masterstrike_combos/preopen_stance_h14.h"
#include "../src/frik_pose_probe.cpp"
#include <cstdio>
#include <thread> // V66.133R1: g++/libstdc++ does not provide std::thread transitively
#include <string>
#include <functional>
static std::vector<std::string> parryTraceLines176,predictionTraceLines176,kickTraceLines59c10,guardHandoffTrace19,masterTrace23,alignTraceLines25;
namespace kcd2vr::diagnostic_trace {void Write(const char* text) noexcept {if(text&&(std::strstr(text,"C23 ")||std::strstr(text,"MELEE_INPUT")||std::strstr(text,"XI_RECEIPT")||std::strstr(text,"MS_WINDOW_OPEN")||std::strstr(text,"PREOPEN_STANCE"))&&masterTrace23.size()<4096)masterTrace23.emplace_back(text);if(text&&std::strstr(text,"V66.176 F4_BLOCK_EDGE"))parryTraceLines176.emplace_back(text);if(text&&(std::strstr(text,"PREDICTION")||std::strstr(text,"F4_ATTACK_STAGE")))predictionTraceLines176.emplace_back(text);if(text&&std::strstr(text,"R3_NATIVE_RECEIPT"))kickTraceLines59c10.emplace_back(text);if(text&&std::strstr(text,"C19C2 GUARD_STRIKE_HANDOFF"))guardHandoffTrace19.emplace_back(text);if(text&&std::strstr(text,"C24 ALIGN_VETO_COVERAGE"))alignTraceLines25.emplace_back(text);}}
using namespace kcd2vr::frik_pose_probe;
namespace diagnostics=kcd2vr::diagnostics;
namespace animation_filter=kcd2vr::animation_filter;
namespace combat_diag_probe=kcd2vr::combat_diag_probe;
namespace kcd2vr::retail_build112 {extern bool hostRetailVerified112;}
namespace kcd2vr::combat_diag_probe {extern bool hostGeometry59c3,hostMotionGuardOwned59c10,hostMotionGuardUnknown59c10;extern kcd2vr::motion59c4::Key hostGuardQuery59c10;extern void (*hostVrStateEventObserver19c4r2)();}
static bool admissionReturnH12=false;static unsigned admissionCallsH12=0;
static bool __fastcall AdmissionLeafH12(void*,int){++admissionCallsH12;return admissionReturnH12;}
static int total=0,failed=0;static unsigned onCalls=0,offCalls=0,zoneCalls=0;static int lastInput=-1,lastZone=-1;
static unsigned actionCalls=0;static int lastActionMode=0;static std::string lastAction;
static unsigned char* nativeState=nullptr;static bool zoneAck62=true;
static std::vector<int> blockModes176;
static std::function<void(int)> blockCallback176;
static bool blockReleaseUpdateH14=true; // explicit host model: native Update ACKs LT-up
template<class T>void put(unsigned char*p,T v){std::memcpy(p,&v,sizeof(v));}
static void ck(bool b,const char*s){++total;failed+=!b;std::printf("%s %s\n",b?"PASS":"FAIL",s);}
static void __fastcall request(void*,int id,bool held){lastInput=id;held?++onCalls:++offCalls;put(nativeState+0x248,id);put(nativeState+0x2A8,std::uint8_t(held));}
static void __fastcall zone(void*,int z,std::int64_t reserved,bool commit){++zoneCalls;lastZone=z;if(zoneAck62){put(nativeState+0xF8,z);put(nativeState+0x188,z);}ck(reserved==0&&commit,"native zone ABI preserves reserved argument and commit Boolean");}
static unsigned cutFeintCallsH20=0;static int cutFeintGuardH20=-1,cutFeintOldZoneH20=-1;
static std::function<void()> cutFeintCallbackH20;
static bool __fastcall cutFeintH20(void*,int z,int guard){
 ++cutFeintCallsH20;cutFeintGuardH20=guard;std::memcpy(&cutFeintOldZoneH20,nativeState+0x188,4);
 lastZone=z;if(zoneAck62){put(nativeState+0xF8,z);put(nativeState+0x188,z);}
 auto callback=std::move(cutFeintCallbackH20);cutFeintCallbackH20={};if(callback)callback();return zoneAck62;
}
#include "native_input117_model.inl"
#include "native_prediction120_model.inl"
static void __fastcall action(void*,const std::uint64_t* p,int mode,float){++actionCalls;lastActionMode=mode;lastAction=p&&*p?reinterpret_cast<const char*>(*p):"";if(lastAction=="block"){if(mode==1||mode==2)put(nativeState+0x308,std::uint8_t(mode==1));if(mode==1)put(nativeState+0x648,std::uint8_t(1));else if(mode==2&&blockReleaseUpdateH14)put(nativeState+0x648,std::uint8_t(0));blockModes176.push_back(mode);auto callback=std::move(blockCallback176);blockCallback176={};if(callback)callback(mode);}ModelAction117(lastAction,mode);}
static bool comboGateObservedH14=false;static unsigned comboGateCallsH14=0;
static bool releaseRetainsPendingH14=true,releaseNestedForeignH14=false,releaseSawPriorSourceH14=false;
static void __fastcall ComboReleaseLeafH14(void* self,int input){
 releaseSawPriorSourceH14=g_released_input_h14.tick!=0;
 put(nativeState+0x2A8,std::uint8_t(releaseRetainsPendingH14));put(nativeState+0x248,input);
 if(releaseNestedForeignH14){auto* previous=g_input_scope117;g_input_scope117=nullptr;
  NativeInputBegin117(self,input);g_input_scope117=previous;}
}
static void __fastcall ComboAdmissionActionH14(void* self,const std::uint64_t* p,int mode,float value){
 action(self,p,mode,value);
 if(lastAction=="prepare_to_attack"&&g_input_scope117&&g_input_scope117->visited){
  ++comboGateCallsH14;comboGateObservedH14=AdmissionGateHook51(control117.data(),beginInput117);
 }
}
namespace directComboH14 {
// Actual production lineage and post-submit includes. Retail reads use the
// complete frik TU, including its real GeometryAdmittedRelease51; only native
// submit/property-set leaves are explicit spies at this same callback seam.
namespace frik_pose_probe=kcd2vr::frik_pose_probe;
using kcd2vr::frik_pose_probe::Read;using kcd2vr::frik_pose_probe::PhysVft6698;
static std::atomic<bool> g_enabled{true},g_sync59c10r9{true},g_intent59c16{false},g_live178r35{true};
struct Pending {std::uint64_t gesture59c3=0,key=0,generation=0,epoch=0,request=0,recovery55c1=0;
 std::uint64_t tick=0;std::uint32_t entityId=0,serial=0;const void* action=nullptr;kcd2vr::contact176::Ticket release51{};
 struct {std::uint64_t serial=0;}lease47c2;};
template<class T>static T Rd(const void* p,unsigned off,T fallback){T value{};return p&&Read(static_cast<const unsigned char*>(p)+off,value)?value:fallback;}
static unsigned EntityId(const void* e){return Rd<std::uint32_t>(e,0xC,0);}
static bool CombatActor(const void* actor,const void*& entity,const void*& state,bool){
 entity=state=nullptr;if(!actor)return false;state=Rd<const void*>(actor,0x210,nullptr);const auto* owner=Rd<const void*>(actor,0x1F8,nullptr);
 if(!state||Rd<const void*>(state,0xCE0,nullptr)!=actor)return false;
 if(PhysVft6698(const_cast<void*>(owner),0x4072C78))entity=owner;else if(PhysVft6698(const_cast<void*>(owner),0x3AA4B98)&&Rd<const void*>(owner,0x278,nullptr)==actor)entity=Rd<const void*>(owner,0x38,nullptr);
 return PhysVft6698(const_cast<void*>(entity),0x4072C78)&&EntityId(entity);
}
#include "../src/subsystems/masterstrike_combos/lineage_owner.inl"
static unsigned setterCalls=0,submitCalls=0;
namespace vrstate596 {static bool Verified(){return true;}
 static bool SetBool(const void* state,unsigned prop,bool value){++setterCalls;put(const_cast<unsigned char*>(static_cast<const unsigned char*>(state))+prop+8,std::uint8_t(value));return true;}}
static constexpr unsigned kActionPhaseProp39=0x30;
static int I(const void* state,unsigned prop){return Rd<int>(state,prop+8,-1);}
static std::uint8_t B(const void* state,unsigned prop){return Rd<std::uint8_t>(state,prop+8,255);}
static bool Allow(int,unsigned){return false;}
namespace diagnostic_trace=kcd2vr::diagnostic_trace;
#ifdef KCD2VR_R17_ROUTE_EXTRACT
// The runner extracts the literal production route prefix through the normal
// live-action branch, preserving its returns/gotos. Only live receiver discovery
// is a controlled native leaf; admitted release and lineage use the full TU.
static std::atomic<bool> g_live_fault178r35{false},g_apply178r30{true},g_sync_overdue_any59c12{true};
static std::atomic<bool> g_intent_live59c17{true};
static std::atomic<unsigned> g_sync_grace_ms59c10r9{70},g_intent_live_ms59c17{70};
static std::atomic<int> g_strike_phase178r39{8};
static std::atomic<std::uint64_t> g_stats[8]{},g_intent_hits59c16{0},g_intent_direct_hits59c17{0},g_intent_live_hits59c17{0};
static const void* currentActionR17=nullptr;static bool callableR17=true,directR17=false,waitR17=false;
static int checksR17=0,failuresR17=0;
static bool FistReceiverCallableH12(){return callableR17;}
static const void* LiveAttackAction178r35(const void*){return currentActionR17;}
static void routeR17(Pending& p,const void* henryActor,const void* hs,std::uint64_t now){
 const bool vrDirect596=false;const bool directOkR159=true;directR17=waitR17=false; // R159: no tutorial in this fixture
 struct PendingLease {void wait(const Pending&){waitR17=true;}} pendingLease;
#include KCD2VR_R17_ROUTE_EXTRACT
 }
sync_direct59c10r9:
 directR17=syncDirect59c10r9;
}
#endif
static bool run(Pending p,const void* henryActor,const void* hs,const void* target,std::uint64_t now,
 const std::function<void()>& nativeCallback={},bool nativeOk=true,bool nativeApply=true){
 const bool syncDirect59c10r9=true;bool ok=false;const bool apply=nativeApply;
#include "../src/subsystems/masterstrike_combos/direct_lineage_owner.inl"
 ++submitCalls;ok=nativeOk;if(nativeCallback)nativeCallback(); // original submit leaf once, before the real post-submit include
#include "../src/subsystems/masterstrike_combos/direct_combo_flag.inl"
 const auto completedNowC2=GetTickCount64();
#include "../src/subsystems/masterstrike_combos/direct_lineage_completed.inl"
 return comboEdge19;
}
}
static std::int64_t __fastcall damage(void*p){put(static_cast<unsigned char*>(p)+0x13C,12.5f);put(static_cast<unsigned char*>(p)+0x138,8.f);return 73;}
static const Matrix34V6550* __fastcall WorldMatrix50(void* e){return reinterpret_cast<const Matrix34V6550*>(static_cast<unsigned char*>(e)+0x58);}
static std::uint32_t __fastcall EntityId50(void* e){std::uint32_t id=0;std::memcpy(&id,static_cast<unsigned char*>(e)+0xC,4);return id;}
static void* materialSelf116=nullptr;static void* materialParams116=nullptr;
static unsigned materialCalls116=0;
static std::uint16_t __fastcall materialLookup116(void*,const char*,int){++materialCalls116;return 37;}
static bool __fastcall materialExecute116(void*,std::uint16_t id,const void*){++materialCalls116;ck(id==37,"native material effect handle preserved");return false;}
static bool __fastcall materialDispatch116(void*,const void*){
 ++materialCalls116;auto id=MaterialLookupDetour116(materialSelf116,"hit_passed_sword_slash_player",12);
 return MaterialExecuteDetour116(materialSelf116,id,materialParams116);
}
static bool reenterBeginR18=false;
static unsigned realBeginCallsR18=0;
static void __fastcall BeginLeafR18(void*,int){
 ++realBeginCallsR18;
 if(reenterBeginR18)NativeInvalidateReleasedInputH14(); // explicit foreign-input/reentry leaf
}
int main(int argc,char** argv){
 diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Combat,true,GetTickCount64());
 hostModuleHandleW=nullptr;g_wh_pinned104=0;ck(WhBase()==0,"unloaded WHGame is not cached permanently");
 std::vector<unsigned char> image(0x5400000);hostModuleHandleW=image.data();const auto base=WhBase();const auto pins=hostModulePinCalls;
 for(int i=0;i<1000;++i)(void)WhBase();ck(hostModulePinCalls==pins,"one successful module pin replaces repeated loader lookups");
 // Actual Windows/retail contracts are separately byte-checked against the DLL.
 auto dos=reinterpret_cast<IMAGE_DOS_HEADER*>(image.data());dos->e_magic=IMAGE_DOS_SIGNATURE;dos->e_lfanew=0x100;
 auto nt=reinterpret_cast<IMAGE_NT_HEADERS64*>(image.data()+0x100);nt->Signature=IMAGE_NT_SIGNATURE;nt->FileHeader.TimeDateStamp=kSupportedWhTimestamp;
 const unsigned char req[]{0x48,0x89,0x5c,0x24,0x08,0x57,0x48,0x83,0xec,0x20,0x48,0x8b,0xd9,0x41,0x8a,0xf8,0x48,0x8b,0x89,0x10,0x02,0x00,0x00,0x48};
 const unsigned char zo[]{0x83,0xfa,0xff,0x74,0x60,0x48,0x89,0x5c,0x24,0x08,0x48,0x89,0x74,0x24,0x10,0x57,0x48,0x83,0xec,0x30,0x8b,0xda,0x41,0x8a};
 std::memcpy(image.data()+0x93A28C,req,sizeof(req));std::memcpy(image.data()+0x90F824,zo,sizeof(zo));put(image.data()+0x3A80610+74*8,base+0x93A28C);put(image.data()+0x3A80610+73*8,base+0x2756D58);
 ck(NativeContract104(base),"exact request/zone bytes and derived-player vtable slots required");image[0x93A28C]^=1;ck(!NativeContract104(base),"altered native request bytes fail closed");image[0x93A28C]^=1;
 for(const auto& pin:kcd2vr::native_parry55c2::pins)std::memcpy(image.data()+pin.rva,pin.bytes,pin.size);
 auto pbContract55c2=[&](){return kcd2vr::native_parry55c2::contract(base,[](std::uintptr_t at,unsigned char* out,std::size_t n){std::memcpy(out,reinterpret_cast<void*>(at),n);return true;});};
 ck(pbContract55c2(),"perfect-block reader accepts verified open/close/count/consumer pins");
 for(const auto& pin:kcd2vr::native_parry55c2::pins){image[pin.rva]^=1;ck(!pbContract55c2(),"each changed PB boundary pin fails closed");image[pin.rva]^=1;}
 std::array<unsigned char,0xE00> player{},state{};std::array<unsigned char,0x500> actor{};std::array<unsigned char,0x100> manager{},entity{};std::array<unsigned char,0x300> hands{};std::array<unsigned char,0x30> input{};
 put(player.data(),base+0x3AA4B98);put(player.data()+0x38,entity.data());put(entity.data(),base+0x4072C78);put(entity.data()+0xC,std::uint32_t(123));
 put(player.data()+0x278,actor.data());put(actor.data(),base+0x3A80610);put(actor.data()+0x1F8,player.data());put(actor.data()+0x210,state.data());put(state.data()+0xCE0,actor.data());
 put(player.data()+0x9D0,manager.data());put(manager.data(),base+0x3EC9958);put(manager.data()+0x38,player.data());put(player.data()+0x990,hands.data());
 put(player.data()+0xB88,input.data());put(input.data(),base+0x3EC99A8);put(input.data()+0x10,player.data());
 put(state.data()+0x630,base+0x3B0A968);put(image.data()+0x3B0A970,base+0x1A7DDE0);
 const unsigned char contextGetterH14[]{0x0F,0xB6,0xC2,0x0F,0xB6,0x44,0x08,0x18,0xC3};
 std::memcpy(image.data()+0x1A7DDE0,contextGetterH14,sizeof(contextGetterH14));
 put(player.data()+0x668,reinterpret_cast<void*>(0x12345678));g_last_player_entity=entity.data();g_henry_item_attach_manager=manager.data();
 g_pose_epoch6689=1;g_census_mode=5;g_weapon_control_mode=0;g_gameplay_allowed=g_head_valid=g_right_valid=g_left_valid=true;
 g_native_contract104=base;g_native_request104=&request;g_native_zone104=&zone;nativeState=state.data();
 g_combat_bypass101=&action;g_combat_input_original101=&action;
 // Deliberately non-default live IDs prove that input-class globals are read.
 put(image.data()+0x5321010,17);put(image.data()+0x5321014,18);put(image.data()+0x5321018,19);
 NativeOwner104 o{};ck(NativeResolve104(player.data(),o)&&o.actor==actor.data()&&o.state==state.data(),"runtime-proven C_Player/combat-state backlinks identify Henry exactly");
 put(state.data()+0xCE0,static_cast<void*>(nullptr));ck(!NativeResolve104(player.data(),o),"wrong state owner blocks all semantic requests");put(state.data()+0xCE0,actor.data());
 NativeFrameBegin104();ck(NativeSubmit104(player.data(),nc104::Gesture::Punch,0,0,{}),"actual adapter calls native RequestAttack for physical punch");
 ck(onCalls==1&&lastInput==17&&lastZone==0,"live light input ID, not attack-type/punch number, reaches native leaf");
 ck(!NativeSubmit104(player.data(),nc104::Gesture::Punch,0,0,{}),"same pending request cannot be issued twice");
 NativeFrameEnd104();ck(!g_native_pending104.entered,"returned request alone is not labelled action entry");
 // A simulated original game update enters its real state property.
 put(state.data()+0x38,4);put(state.data()+0x2D8,std::uint8_t(1));put(state.data()+0x218,5);NativeFrameEnd104();
 ck(g_native_pending104.entered,"native post-frame reads independent action-state transition");
 NativeFrameBegin104();ck(offCalls==1&&!g_native_pending104.held,"request released only after crossing original frame boundary");
 NativeFrameBegin104();ck(offCalls==1,"additional frames cannot repeat native release");
 hostTickCount64+=600;put(state.data()+0x2D8,std::uint8_t(0));put(state.data()+0x38,1);NativeFrameEnd104();
 g_right_valid=false;ck(!NativeSubmit104(player.data(),nc104::Gesture::Punch,0,0,{}),"lost controller tracking cannot dispatch a queued punch");g_right_valid=true;
 put(hands.data()+0x260,std::uint64_t(77));ck(!NativeSubmit104(player.data(),nc104::Gesture::Punch,0,0,{}),"new held item revokes empty-fist eligibility immediately");
 g_census_mode=3;g_weapon_control_mode=6;ck(NativeSubmit104(player.data(),nc104::Gesture::Slash,0,77,{1,0,0}),"qualified held sword sends native directional attack request");ck(lastInput==18&&lastZone==1,"slash chooses separate heavy input and directional zone");NativeFrameBegin104();
 hostTickCount64+=600;g_native_pending104={};ck(NativeSubmit104(player.data(),nc104::Gesture::Thrust,0,77,{0,1,0}),"thrust uses native direct-attack route");ck(lastInput==17&&lastZone==5,"thrust does not reuse directional slash input");NativeFrameBegin104();
 hostTickCount64+=600;g_native_pending104={};const auto zones=zoneCalls;ck(NativeSubmit104(player.data(),nc104::Gesture::Pommel,0,77,{0,-1,0}),"pommel intent reaches native special request");ck(lastInput==19&&zoneCalls==zones,"special preserves native context instead of forcing a fabricated zone/type");
 // Do not clear another owner's changed request on release.
 put(state.data()+0x248,88);const auto oldOff=offCalls;NativeFrameBegin104();ck(offCalls==oldOff,"release cannot cancel a different native input owner");put(state.data()+0x2A8,std::uint8_t(0));
 // Exact damage observer runs after original, does not alter its return value.
 NativeResolve104(player.data(),o);g_native_owner104.publish(o);std::array<unsigned char,0x160> ctx{};
 put(ctx.data()+0x10,o.soul);put(ctx.data()+0x18,entity.data());put(ctx.data()+0x20,std::uint64_t(77));g_native_damage_original104=&damage;
 ck(NativeDamageDetour104(ctx.data())==73,"post-damage hook preserves original native return");
 bool sawDamage=false;for(unsigned j=0;j<g_native_head104;++j)if(g_native_events104[j].kind==4){const auto&e=g_native_events104[j];sawDamage=e.health==12.5f&&e.stamina==8.f&&e.serial==0;}
 ck(sawDamage,"post-mitigation values observed without inventing VR hit attribution or stamina cost");
 const auto priorHead=g_native_head104;put(ctx.data()+0x10,reinterpret_cast<void*>(std::uintptr_t(0x87654321)));NativeDamageDetour104(ctx.data());ck(g_native_head104==priorHead,"unrelated NPC combat is filtered before recording results");
 ResetPageMemo169();unsigned char data[32]{};const auto q0=g_read_queries101R2.load(); // V66.169 memo starts empty
 {ReadScope101R2 outer;int v=0;(void)Read(data,v);const auto q=g_read_cache101R2.queries;{ReadScope101R2 nested;(void)Read(data,v);ck(g_read_cache101R2.queries==q,"nested scopes preserve page permission cache");}put(data,42);Read(data,v);ck(v==42&&g_read_cache101R2.queries==q,"cached permission never freezes live identity values");}
 ck(g_read_queries101R2.load()>q0&&g_read_cache101R2.depth==0,"outer callback clears its permission cache and aggregates counters");
 // Execute actual query admission with a missing mock world, never a native ray.
 std::uint64_t previous=0;g_collision_binding6692=1;{ContactPass6695 pass(true,1000,previous,1);g_contact_budget6695.started=ContactMicros6695()-1000000;
  auto result=WorldRayDistance6692({0,0,0},{0,1,0});ck(result.state!=wc::Result::Busy&&g_contact_budget6695.used==1,"actual adapter admits first query despite preceding non-contact work");}
 ck(!g_contact_budget_active6695,"contact pass ends without leaking a query lease");
 // Execute the real physical queue consumer without emulating unknown native item factories.
 // Only immutable definitions are seeded; all live ownership reads still execute.
 g_native_pending104={};hostTickCount64+=1000;g_census_mode=3;g_weapon_control_mode=6;
 put(state.data()+0x2A8,std::uint8_t(0));put(manager.data()+0x40,0);
 put(manager.data()+0x48,std::uint64_t(77));g_weapon_request_epoch6694=1;
 g_phys_armed6698=true;g_phys_fault6698=false;g_phys_input6698=true;g_phys_abort6698=false;g_phys_native6698={};
 static const char prep[]="prepare_to_attack",primary[]="attack_primary",abort[]="attack_abort";
 g_phys_native6698.actionNames[0]=reinterpret_cast<std::uint64_t>(prep);g_phys_native6698.actionNames[1]=reinterpret_cast<std::uint64_t>(primary);g_phys_native6698.actionNames[2]=reinterpret_cast<std::uint64_t>(abort);
 g_phys_commands6698.head=0;g_phys_commands6698.tail=0;
 PhysLive103 live{};ck(PhysReadLive103(live)&&live.hand[0]==77,"actual live equipment reader identifies the held command owner");
 auto& meta=g_phys_metadata104;meta={};meta.last=live;meta.metadataAt=hostTickCount64;
 auto& equipment=meta.profiles;equipment.valid=true;equipment.player=player.data();equipment.manager=manager.data();equipment.request=1;equipment.epoch=1;equipment.tick=hostTickCount64;equipment.selected=0;
 equipment.handKey[0]=77;equipment.handKnown[0]=equipment.handKnown[1]=true;equipment.heldIndex[0]=0;equipment.count=1;
 auto& item=equipment.items[0];item.held=true;item.set=0;item.profile.key=77;item.profile.generation=4;item.profile.side=0;item.profile.classId=4;item.profile.primary=true;
 PhysFrame6698 frame{};frame.request=1;frame.player=player.data();frame.manager=manager.data();frame.input.allowed=true;frame.input.bodyEpoch=1;frame.input.epoch=1;frame.input.sample=100;frame.input.tick=hostTickCount64;frame.input.tracked[0]=frame.input.tracked[1]=true;frame.input.heldMotionAttacks=true;frame.profiles[0]=item.profile;frame.output.active[0]=true;
 g_phys_frame6698.publish(frame);g_phys_ready_sample6698=100;
 frame.input.attacks=true;frame.output.owned[0]=true;g_phys_frame6698.publish(frame);
 ph::Command command{};command.kind=ph::CommandKind::Prepare;command.side=0;command.key=77;command.epoch=command.bodyEpoch=1;command.sample=100;command.generation=4;command.tick=hostTickCount64;command.direction={1,0,0};
 g_phys_commands6698.push(command);command.kind=ph::CommandKind::Slash;g_phys_commands6698.push(command);
 const auto queueActions=actionCalls;PhysicalNativeFrameTick6698();
 ck(actionCalls==queueActions+1&&lastAction=="prepare_to_attack"&&lastActionMode==1&&g_phys_native6698.attackPrepared,"same-frame queue emits prepare press only and defers primary release");
 PhysicalNativeFrameEnd104();++hostTickCount64;PhysicalNativeFrameTick6698();
 ck(actionCalls==queueActions+2&&lastAction=="attack_primary"&&lastActionMode==2&&!g_phys_native6698.attackPrepared,"actual F12 queue commits with controller-style primary release on the next outer frame");
 PhysicalNativeFrameEnd104();++hostTickCount64;PhysicalNativeFrameTick6698();
 ck(actionCalls==queueActions+2,"real frame pump never repeats a consumed physical stroke");
 command.generation=3;command.tick=hostTickCount64;g_phys_commands6698.push(command);PhysicalNativeFrameTick6698();
 ck(actionCalls==queueActions+2,"stale generation in actual queue cannot request an attack");
 command.generation=4;command.tick=hostTickCount64;g_phys_commands6698.push(command);const auto missingPrepare=actionCalls;
 PhysicalNativeFrameTick6698();ck(actionCalls==missingPrepare+1&&lastAction=="prepare_to_attack"&&g_phys_commands6698.head!=g_phys_commands6698.tail,"orphaned fresh commit restores prepare without consuming the swing");
 ++hostTickCount64;PhysicalNativeFrameTick6698();ck(actionCalls==missingPrepare+2&&lastAction=="attack_primary"&&lastActionMode==2&&g_phys_commands6698.head==g_phys_commands6698.tail,"restored prepare commits once on the following native frame");
 command.tick=hostTickCount64;g_phys_commands6698.push(command);frame.output.active[0]=false;g_phys_frame6698.publish(frame);
 const auto releasedCommit=actionCalls;PhysicalNativeFrameTick6698();ck(actionCalls==releasedCommit,"release of weapon ownership rejects a still-fresh queued stroke");frame.output.active[0]=true;
 // Actual guard bridge, including its native action source balancing.
 frame.output.handlingOnly55c1[0]=true;g_phys_frame6698.publish(frame);
 PhysicalNativeFrameTick6698(); // takeover may cancel previous stock input; settle before queueing
 command.kind=ph::CommandKind::Prepare;command.tick=hostTickCount64;g_phys_commands6698.push(command);
 const auto handlingActions=actionCalls;PhysicalNativeFrameTick6698();
 ck(actionCalls==handlingActions,"visible handling recovery cannot drain a queued attack preparation");
 frame.output.handlingOnly55c1[0]=false;frame.input.attacks=false;g_phys_frame6698.publish(frame);
 command.tick=hostTickCount64;g_phys_commands6698.push(command);PhysicalNativeFrameTick6698();
 ck(actionCalls==handlingActions,"a display-only frame cannot prepare an attack even with an active weapon");
 // H30: the actual native pump may consume an independently qualified tracked
 // gesture during pose-only recovery. The old no-proof negative above stays.
 {
  const auto savedFrameH30=frame;TrackingSnapshot savedTrackingH30{};g_tracking_frames6695.read(savedTrackingH30);
  const auto savedCommandH30=command;const auto savedStateH30=state;
  const auto savedContractH30=g_input_contract117.load();const bool savedRetailH30=kcd2vr::retail_build112::hostRetailVerified112;
  const bool savedModelH30=model117,savedRouteH30=route117,savedOpponentH30=g_attack_require_opponent59c10.load();
  const auto savedPoseTickH30=animation_filter::combatPoseTick112.load();
  const auto savedHeadTickH30=g_head_valid_tick.load();const bool savedHeadValidH30=g_head_valid.load();
  model117=route117=true;kcd2vr::retail_build112::hostRetailVerified112=true;g_input_contract117=base;
  g_attack_require_opponent59c10=false;animation_filter::combatPoseTick112=hostTickCount64;
  put(control117.data()+0x18,actor.data());put(state.data()+0x38,1);put(state.data()+0x218,-1);
  put(state.data()+0x2A8,std::uint8_t(0));put(state.data()+0x2D8,std::uint8_t(0));put(state.data()+0x308,std::uint8_t(0));
  frame.input.attacks=frame.input.physicalCombat177=frame.input.coherentMelee119=frame.input.intentTracked[0]=true;
  frame.input.naturalCombat117=true;frame.input.grip[0]=1;
  frame.input.tick=hostTickCount64;frame.output.handlingOnly55c1[0]=true;
  frame.output.normalIntentH30[0]={77,4,1,1,frame.input.sample,frame.input.tick,true};
  TrackingSnapshot trackingH30{};trackingH30.sequence=100;trackingH30.allowed=true;
  trackingH30.head_valid=trackingH30.right_valid=true;
  trackingH30.tick=trackingH30.head_good_tick=trackingH30.right_good_tick=hostTickCount64;
  g_tracking_frames6695.publish(trackingH30);g_phys_frame6698.publish(frame);
  ck(!GeometryOwnerValid178r30(77,4,1,1)&&GeometryIntentOwnerH30(77,4,1,1),
     "actual current tracked proof keeps native intent owner while new contact owner stays closed in recovery");
  kcd2vr::swing_effort59c14::Input54 costH30{};
  costH30.key={900,77,4,1,1,kcd2vr::contact176::recoveryEpoch55c1.load(),reinterpret_cast<std::uintptr_t>(player.data())};
  costH30.actor=reinterpret_cast<std::uintptr_t>(actor.data());
  ck(kcd2vr::solver_rpg_h29::costOwned(costH30,GeometryIntentOwnerH30,GeometrySolverCostOwnerValidH28),
     "actual normal cost callback retains original owned native ancestry through soft recovery");
  ck(OriginalGeometryContactOwnerH30(77,4,1,1,frame.input.sample-1,900,hostTickCount64-10,hostTickCount64),
     "actual original-contact callback retains a10ms old event during qualified soft recovery");
  ck(!OriginalGeometryContactOwnerH30(77,4,1,1,frame.input.sample-1,900,hostTickCount64-161,hostTickCount64)&&
     !OriginalGeometryContactOwnerH30(77,4,1,1,frame.input.sample+1,900,hostTickCount64-10,hostTickCount64)&&
     !OriginalGeometryContactOwnerH30(77,4,1,1,frame.input.sample,900,hostTickCount64+1,hostTickCount64),
     "actual original-contact callback rejects expired event, future sample and future impact");
  g_phys_commands6698.head=g_phys_commands6698.tail=0;command.kind=ph::CommandKind::Prepare;
  command.tick=hostTickCount64;command.gesture59c3=900;command.stroke117=ph::CommandKind::Slash;command.zone117=2;
  command.recoveryEpochH30=kcd2vr::contact176::recoveryEpoch55c1.load();
  const auto beforeH30=actionCalls;g_phys_commands6698.push(command);PhysicalNativeFrameTick6698();
  ck(actionCalls==beforeH30+1&&g_phys_native6698.attackPrepared,
     "actual native input pump drains qualified preparation exactly once during pose recovery");
  auto badH30=frame;++badH30.output.normalIntentH30[0].sample;g_phys_frame6698.publish(badH30);
  ck(!GeometryIntentOwnerH30(77,4,1,1)&&!kcd2vr::solver_rpg_h29::costOwned(costH30,GeometryIntentOwnerH30,GeometrySolverCostOwnerValidH28),
     "malformed or stale tracked proof cannot authorize normal native cost");
  g_phys_frame6698.publish(frame);trackingH30.head_valid=false;trackingH30.head_held=true;
  trackingH30.head_good_tick=hostTickCount64-61;g_tracking_frames6695.publish(trackingH30);
  g_head_valid=false;g_head_valid_tick=hostTickCount64-61;
  ck(!GeometryIntentOwnerH30(77,4,1,1)&&
     !OriginalGeometryContactOwnerH30(77,4,1,1,frame.input.sample-1,900,hostTickCount64-10,hostTickCount64),
     "current intent and original-contact callbacks never extend the real HMD60ms tracking grace");
  g_head_valid=true;g_head_valid_tick=hostTickCount64;trackingH30.head_valid=true;trackingH30.head_held=false;
  trackingH30.head_good_tick=hostTickCount64;g_tracking_frames6695.publish(trackingH30);
  PhysCancelAttack117();g_phys_commands6698.head=g_phys_commands6698.tail=0;
  put(state.data()+0x38,1);put(state.data()+0x218,-1);put(state.data()+0x2A8,std::uint8_t(0));
  // Queue through the real producer, then deliberately omit the native pump
  // throughout hard loss/re-entry. Old commands may never acquire the new key.
  const auto sourceRecoveryH30=kcd2vr::contact176::recoveryEpoch55c1.load();
  command.gesture59c3=901;command.kind=ph::CommandKind::Prepare;command.recoveryEpochH30=0;
  frame.output.commandCount=2;frame.output.commands[0]=command;frame.output.commands[1]=command;
  frame.output.commands[1].kind=ph::CommandKind::Slash;
  ph::State queueStateH30{};std::array<ph::HolsterState,8> queueHolstersH30{};const float queueWorldH30[8]{0,0,0,1,0,0,0,1};
  auto publishH30=[&](){equipment.tick=hostTickCount64;
   PhysPublish6698(frame.input,frame.output,frame.profiles,equipment,queueWorldH30,queueStateH30,queueHolstersH30);};
  publishH30();ph::Command queuedH30{};
  ck(g_phys_commands6698.peek(queuedH30)&&queuedH30.recoveryEpochH30==sourceRecoveryH30,
     "actual producer stamps original recovery epoch on queued native command");
  frame.output.commandCount=0;++hostTickCount64;++frame.input.sample;frame.input.tick=hostTickCount64;
  frame.input.attacks=frame.input.physicalCombat177=frame.input.coherentMelee119=frame.input.intentTracked[0]=false;
  frame.output.normalIntentH30[0]={};publishH30();
  ++hostTickCount64;++frame.input.sample;frame.input.tick=hostTickCount64;
  frame.input.attacks=frame.input.physicalCombat177=frame.input.coherentMelee119=frame.input.intentTracked[0]=true;
  frame.output.normalIntentH30[0]={77,4,1,1,frame.input.sample,frame.input.tick,true};publishH30();
  const auto staleCallsH30=actionCalls;PhysicalNativeFrameTick6698();
  ck(kcd2vr::contact176::recoveryEpoch55c1.load()!=sourceRecoveryH30&&actionCalls==staleCallsH30&&
     !g_phys_native6698.attackPrepared&&g_phys_commands6698.head==g_phys_commands6698.tail,
     "actual pump refuses pre-loss preparation/release after re-entry without retagging or native calls");
  PhysCancelAttack117();g_phys_commands6698.head=g_phys_commands6698.tail=0;
  g_head_valid=savedHeadValidH30;g_head_valid_tick=savedHeadTickH30;
  g_tracking_frames6695.publish(savedTrackingH30);frame=savedFrameH30;frame.input.tick=hostTickCount64;g_phys_frame6698.publish(frame);
  g_phys_ready_sample6698=frame.input.sample;command=savedCommandH30;state=savedStateH30;
  g_input_contract117=savedContractH30;kcd2vr::retail_build112::hostRetailVerified112=savedRetailH30;
  model117=savedModelH30;route117=savedRouteH30;g_attack_require_opponent59c10=savedOpponentH30;
  animation_filter::combatPoseTick112=savedPoseTickH30;
 }
 // A focused invocation executes the same complete TU, setup, original input
 // transport checks and H30 cases, without the unrelated historical guard lab.
 // The unfiltered native104_bridge remains registered and unchanged in scope.
 if(argc>1&&std::strcmp(argv[1],"--gesture-recovery-only-h30")==0){
  std::printf("gesture_native_h30=%d/%d native_leaves=EXPLICIT_TEST_REPLACEMENTS retail_execution=NONE\n",total-failed,total);
  return failed?1:0;
 }
 // XR32 focused return-value check: same actual TU and established owner
 // setup; retain the historical unfiltered guard fixture unchanged below.
 if(argc>1&&std::strcmp(argv[1],"--xr32-camera-only")==0){
  const int before=total;
  using kcd2vr::retail_build112::hostRetailVerified112;
  #include "bow_zoom121_cases.inl"
  std::printf("xr32_camera=%d scopedChecks=%d total=%d failures=%d native_leaves=EXPLICIT_TEST_REPLACEMENTS\n",failed==0,total-before,total,failed);
  return failed?1:0;
 }
 frame.input.attacks=true;g_phys_frame6698.publish(frame);
 // Begin in observed native idle, not the preceding fixture's stale punch type.
 put(state.data()+0x38,1);put(state.data()+0x218,-1);
 diagnostics::lab::Set(diagnostics::lab::Key::F3,2);animation_filter::nativeBodyOwned112=false;
 static const char block[]="block";std::uint64_t blockName=reinterpret_cast<std::uint64_t>(block);
 g_phys_native6698.actionNames[3]=blockName;g_guard_manual112=false;g_guard_physical112=false;
 frame.profiles[0].geometry=true;frame.profiles[0].tip={.6,0,0};frame.output.owned[0]=true;
 frame.output.weapons[0].p={-.3,.4,.3};frame.input.shoulders[0]=frame.input.shoulders[1]={};
 put(state.data()+0xF8,2);put(state.data()+0x278,0);put(state.data()+0x308,std::uint8_t(0));
 auto guardFrame=[&](){hostTickCount64+=10;++frame.input.sample;frame.input.tick=hostTickCount64;
   animation_filter::combatPoseTick112=hostTickCount64;g_phys_frame6698.publish(frame);g_phys_ready_sample6698=frame.input.sample;NativeGuardTick112();};
 const auto beforeGuard=actionCalls;g_phys_armed6698=false;guardFrame();
 ck(actionCalls==beforeGuard&&!g_guard_physical112,"unarmed native fingerprint contract prevents all guard writes");
 g_phys_armed6698=true;guardFrame();
 ck(actionCalls==beforeGuard&&!g_guard_physical112,"unverified SHA prevents physical guard writes");
 kcd2vr::retail_build112::hostRetailVerified112=true;for(int i=0;i<12;++i)guardFrame();
 ck(g_native_guard112.held&&g_guard_physical112&&lastAction=="block"&&(lastActionMode==1||lastActionMode==4),"actual guard bridge acquires promptly and sustains native block");
 ck(lastZone==0&&zoneCalls>zones,"guard compares current +F8 even when attack snapshot +278 already equals desired sector");
 // V168 production guard re-press, not just the gesture policy. Logging OFF
 // must not change native input/feedback behavior.
 diagnostics::capture168::gate.mask=0;diagnostics::lab::Set(diagnostics::lab::Key::F3,3);
 TrackingSnapshot guardTracking168{};guardTracking168.sequence=1000;guardTracking168.allowed=true;
 guardTracking168.head_valid=guardTracking168.right_valid=guardTracking168.left_valid=true;
 auto rawGuard168=[&](float x){guardTracking168.tick=hostTickCount64+10;++guardTracking168.sequence;
   guardTracking168.head_good_tick=guardTracking168.right_good_tick=guardTracking168.left_good_tick=guardTracking168.tick;
   guardTracking168.right.x=x;g_tracking_frames6695.publish(guardTracking168);guardFrame();};
 for(int i=0;i<15;++i)rawGuard168(0.f);
 ck(g_native_guard112.motion168.armed,"quiet guard gesture arms only after settling in the actual native guard bridge");
 rawGuard168(.03f);rawGuard168(.06f);const auto motionBefore168=actionCalls;rawGuard168(.09f);
 ck(actionCalls==motionBefore168+2&&lastAction=="block"&&lastActionMode==1&&g_native_guard112.held,
    "actual quiet guard bridge emits one balanced release/press for deliberate tracked motion");
 g_guard_manual_owner112=player.data();g_guard_manual112=true;
 for(int i=0;i<35;++i)rawGuard168(.09f);const auto manualMotion168=actionCalls;
 rawGuard168(.12f);rawGuard168(.15f);rawGuard168(.18f);
 ck(actionCalls==manualMotion168,"manual LT ownership prevents motion re-press through the actual bridge");
 g_guard_manual112=false;g_guard_manual_owner112=nullptr;
 diagnostics::lab::Set(diagnostics::lab::Key::F3,2);
 diagnostics::capture168::gate.select(diagnostics::capture168::Domain::Combat,true,GetTickCount64());
 const auto heldActions=actionCalls;g_right_valid=false;NativeGuardTick112();
 ck(!g_native_guard112.held&&actionCalls==heldActions+1&&lastActionMode==2,"tracking loss releases exactly the owned physical block");
 NativeGuardTick112();ck(actionCalls==heldActions+1,"tracking loss cannot repeat release");g_right_valid=true;
 CombatInputDetour101(input.data(),&blockName,1,1.f);const auto manualPress=actionCalls;
 for(int i=0;i<12;++i)guardFrame();
 ck(g_native_guard112.held&&actionCalls==manualPress,"physical source joins manual block without duplicate press");
 CombatInputDetour101(input.data(),&blockName,2,0.f);
 ck(actionCalls==manualPress&&!g_guard_manual112,"manual release cannot cancel physical block lease");
 animation_filter::nativeBodyOwned112=true;NativeGuardTick112();
 ck(actionCalls==manualPress+1&&lastActionMode==2&&!g_guard_physical112,"native hit/clinch handoff releases physical block");
 animation_filter::nativeBodyOwned112=false;CombatInputDetour101(input.data(),&blockName,1,1.f);
 for(int i=0;i<12;++i)guardFrame();const auto shared=actionCalls;
 diagnostics::lab::Set(diagnostics::lab::Key::F3,0);NativeGuardTick112();
 ck(!g_guard_physical112&&actionCalls==shared,"mode exit leaves still-held manual LT untouched");
 CombatInputDetour101(input.data(),&blockName,2,0.f);ck(actionCalls==shared+1,"last manual release reaches native input once");
 diagnostics::lab::Set(diagnostics::lab::Key::F3,2);for(int i=0;i<12;++i)guardFrame();
 const auto fresh=actionCalls;hostTickCount64+=101;NativeGuardTick112();
 ck(!g_guard_physical112&&actionCalls==fresh+1,"stalled renderer cannot retain physical block");
 for(int i=0;i<12;++i)guardFrame();const auto sameOwner=actionCalls;
 put(state.data()+0xCE0,static_cast<void*>(nullptr));NativeGuardTick112();
 ck(!g_guard_physical112&&actionCalls==sameOwner,"broken native owner backlink cannot receive a release");
 put(state.data()+0xCE0,actor.data());put(state.data()+0x308,std::uint8_t(0));
 // R44C2: replay the observed native free-block transition through the ACTUAL
 // input bridge. No native phase writes or calls are performed in the game.
 {const auto oldExperiment=g_combat_experiment175.load();g_combat_experiment175=true;
  frame.input.positionGuard176=frame.input.coherentMelee119=true;
  frame.output.guardBySide30c1[0]=true;frame.output.strokeActive119=false;
  put(state.data()+0x38,1);put(state.data()+0x218,-1);put(state.data()+0x2A8,std::uint8_t(0));put(state.data()+0x2D8,std::uint8_t(0));
  guardFrame();ck(g_native_guard112.held,"R44C2 replay begins with owned positional guard");
  const auto entered=actionCalls;put(state.data()+0x38,256);
  // +308 can still be zero in the observed combat_free_blk transition.
  for(int i=0;i<20;++i)guardFrame();
  ck(g_native_guard112.held&&g_guard_physical112&&actionCalls<=entered+4&&lastActionMode==4,
     "native free-block phase keeps one owned press plus bounded holds; no release/press loop");
  const auto holding=actionCalls;frame.output.guardBySide30c1[0]=false;guardFrame();
  ck(!g_native_guard112.held&&actionCalls==holding+1&&lastActionMode==2,
     "lowering physical guard still releases once from phase 256");
  frame.output.guardBySide30c1[0]=true;const auto foreign=actionCalls;guardFrame();
  ck(!g_native_guard112.held&&actionCalls==foreign,"phase 256 alone cannot acquire somebody else's guard input");
  g_combat_experiment175=oldExperiment;frame.input.positionGuard176=frame.input.coherentMelee119=false;
  put(state.data()+0x38,1);}
 #include "native_parry176_cases.inl"
 #include "native_fist_guard175_cases.inl"
 #include "native_motion112_cases.inl"
 #include "native_prediction120_cases.inl"
 #include "bow_zoom121_cases.inl"
 #include "contact114_cases.inl"
 #include "material116_cases.inl"
 #include "native_input117_cases.inl"
 #include "diagnostics122_cases.inl"
 #include "interaction_body122_cases.inl"
 { // C54: actual completed physical publication, independent of native Prepare consumption.
  namespace mo=kcd2vr::motion59c4;
  const bool oldSync=g_synchronized59c10r9.load(),oldIntegrated=g_combat_integrated178r22c2.load(),oldExperiment54=g_combat_experiment175.load();
  g_synchronized59c10r9=true;g_combat_integrated178r22c2=true;g_combat_experiment175=true;mo::selected=false;mo::hybridAttempts54.clear();
  auto hf=frame;hf.player=player.data();hf.request=hf.input.epoch=74;hf.input.bodyEpoch=73;
  hf.input.tick=hostTickCount64;hf.input.sample=9000;hf.input.physicalCombat177=true;hf.input.motionOwned59c5=false;
  hf.input.allowed=hf.input.attacks=hf.input.tracked[0]=true;hf.output.active[0]=hf.output.owned[0]=true;
  hf.output.handlingOnly55c1[0]=hf.output.handlingOnly55c1[1]=false;hf.output.invalidMask55c1[0]=0;
  hf.profiles[0].key=71;hf.profiles[0].generation=72;hf.profiles[0].side=0;
  hf.output.commandCount=1;auto& c=hf.output.commands[0];c={};c.kind=ph::CommandKind::Prepare;c.side=0;
  c.gesture59c3=992;c.key=71;c.generation=72;c.epoch=74;c.bodyEpoch=73;c.sample=9000;c.tick=hostTickCount64;
  c.stroke117=ph::CommandKind::Thrust;c.zone117=2;c.originZone167=2;
  PhysPublishHybrid54(hf);mo::Attempt attempt{};
  mo::Key key{992,71,72,73,74,kcd2vr::contact176::recoveryEpoch55c1.load(),reinterpret_cast<std::uintptr_t>(player.data())};
  ck(mo::hybridAttempts54.contact(key,hostTickCount64,hostTickCount64,456,1,attempt)==mo::Claim::Ready,
     "C54 completed physical Prepare owns first contact before any native release/input call");
  mo::hybridAttempts54.clear();c.gesture59c3=993;c.held57=true;PhysPublishHybrid54(hf);key.gesture=993;
  ck(mo::hybridAttempts54.contact(key,hostTickCount64,hostTickCount64,456,1,attempt)==mo::Claim::Identity,
     "C54 held rear wind-up does not mint physical contact authority");
  g_synchronized59c10r9=oldSync;g_combat_integrated178r22c2=oldIntegrated;g_combat_experiment175=oldExperiment54;mo::hybridAttempts54.clear();
 }
 { // H12: actual installed input observer retains coverage with F3 OFF.
  namespace h12=kcd2vr::diagnostics::flowH12;
  const auto oldMask=diagnostics::capture168::gate.mask.load();diagnostics::capture168::gate.mask=0;
  const auto savedGate=g_admission_gate51;auto* savedScope=g_input_scope117;
  alignas(16) unsigned char selfH12[0x30]{},stateH12[0x900]{},actorH12[0x40]{};
  put(selfH12+0x18,static_cast<void*>(actorH12));put(stateH12+0x7D8,0);put(stateH12+0x868,std::uint8_t(1));
  NativeInputScope117 inputH12{};inputH12.visited=true;inputH12.prepare=true;inputH12.owner.actor=actorH12;inputH12.owner.state=stateH12;
  g_input_scope117=&inputH12;g_admission_gate51=&AdmissionLeafH12;admissionReturnH12=true;
  const auto before=h12::audit.admission.load(),slots=h12::audit.slotAtInput.load(),nativeTrue=h12::audit.nativeTrue.load();
  admissionCallsH12=0;ck(AdmissionGateHook51(selfH12,1),"H12 recorder-OFF observation preserves true native result");
  ck(admissionCallsH12==1&&h12::audit.admission==before+1&&h12::audit.slotAtInput==slots+1&&h12::audit.nativeTrue==nativeTrue+1,
     "H12 genuine slot-at-input counted with F3 OFF; native original called exactly once");
  admissionReturnH12=false;ck(!AdmissionGateHook51(selfH12,1),"H12 observer preserves native ordinary false result");
  ck(h12::audit.nativeTrue==nativeTrue+1,"H12 false native result never counted as true or as hit acceptance");
  g_input_scope117=nullptr;const auto observed=h12::audit.admission.load();
  (void)AdmissionGateHook51(selfH12,1);ck(h12::audit.admission==observed&&admissionCallsH12==3,"H12 unrelated native input forwards once without VR counts");
  h12::Audit copy{};copy.attempt(true,2,0,1,5);copy.attempt(false,1,1,0,2);copy.input(false,-1,false,true);
  copy.window(false,-1,false);copy.window(true,1,false);copy.window(true,2,true);
  char receipt[640]{};h12::format(copy,receipt,sizeof(receipt));
  ck(copy.prepare==1&&copy.commit==1&&copy.zero==1&&copy.positive==1&&copy.unknown==1&&copy.perkUnknown==0&&
     std::strstr(receipt,"detailScope=F3_ONLY"),"H12 copied eligibility aggregates distinguish recorder scope, no window and native learned perk");
  ck(copy.msSamples==3&&copy.msUnknown==1&&copy.msPositive==2&&copy.msMax==2&&copy.msObservedOpens==1,
     "H12 raw positive samples distinguish a mid-window observation from a proven new open edge");
  copy.admission=copy.nativeTrue=copy.unknown=copy.slotAtInput=copy.masterAtInput=UINT64_MAX;
  copy.prepare=copy.commit=copy.cut=copy.thrust=copy.positive=copy.zero=copy.windowUnknown=UINT64_MAX;
  copy.learned=copy.unlearned=copy.perkUnknown=UINT64_MAX;
  copy.msSamples=copy.msUnknown=copy.msPositive=copy.msMax=copy.msObservedOpens=UINT64_MAX;
  for(auto& count:copy.eligibilityReasons)count=UINT64_MAX;
  char largest[1536]{};h12::format(copy,largest,sizeof(largest));
  ck(std::strstr(largest,"ownedWeaponInputs{")&&std::strstr(largest,"detailScope=F3_ONLY"),
     "H12 separate bounded eligibility record retains scope even at maximum counters");
  g_input_scope117=savedScope;g_admission_gate51=savedGate;diagnostics::capture168::gate.mask=oldMask;
 }
 #include "pending_block_release_r18_cases.inl"
 // Menu/controller grace never becomes an input source.
 diagnostics::lab::Set(diagnostics::lab::Key::F3,0);
#ifdef KCD2VR_R17_ROUTE_EXTRACT
 if(argc>1&&std::strcmp(argv[1],"--admitted-direct-lineage-only-r17")==0){
  std::printf("admitted_direct_lineage_r17=%d/%d inherited_other_failures=%d full_TU=YES engine_leaves=CONTROLLED\n",
      directComboH14::checksR17-directComboH14::failuresR17,directComboH14::checksR17,failed-directComboH14::failuresR17);
  return directComboH14::failuresR17?1:0;
 }
#endif
 std::printf("native104_bridge=%d/%d native_leaves=EXPLICIT_TEST_REPLACEMENTS retail_execution=NONE\n",total-failed,total);return failed?1:0;
}
