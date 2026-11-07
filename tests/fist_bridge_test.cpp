// Entire current frik TU; only OS/game API leaves are explicitly replaced.
#include "../src/frik_pose_probe.cpp"
#include <cstdio>
#include <string>
#include <vector>
namespace kcd2vr::diagnostic_trace {void Write(const char*) noexcept {}}
namespace kcd2vr::animation_filter {void SetInteractionAnimationTarget(void*,std::uint32_t,std::uint64_t) noexcept {} void SetMode(SuppressionMode) noexcept {} bool Install() noexcept {return false;} bool Ready() noexcept {return false;}}
namespace kcd2vr::native_stereo_outer_lab {bool EnsurePhysicalFramePump6698() noexcept {return true;}}
// Physical-hand solver is an unrelated external leaf in this native-input
// fixture. No hand-body readiness/contact is fabricated by these refusals.
namespace kcd2vr::physics::hand_proxy60 {
bool SubmitPresenceH24(int,const native_hand_proxy_t4::Input&) noexcept{return false;}
void CancelPresenceH24(int) noexcept{}
bool PresenceReadyH24() noexcept{return false;}
bool SetPresencePredicateH24(bool(*)(const native_hand_proxy_t4::Input&) noexcept) noexcept{return false;}
bool ReadPresenceFeedbackH24(int,PresenceFeedbackH24& out) noexcept{out={};return false;}
}
using namespace kcd2vr::frik_pose_probe;
static std::vector<std::string> calls;static int failures=0,total=0;
static void Check(bool b,const char*s){++total;failures+=!b;std::printf("%s %s\n",b?"PASS":"FAIL",s);}
namespace kcd2vr::retail_build112 {extern bool hostRetailVerified112;}
static unsigned latch119=0,accepted119=0;
static void (*inputReentry176)(const std::string&,int)=nullptr;
static const char* inputFailure176=nullptr;
static std::array<unsigned char,0x30> punchControlR19{};
static std::vector<int> punchClassesR19;
static void __fastcall PunchLeafR19(void*,int input){punchClassesR19.push_back(input);}
static void __fastcall NativeInput(void*,const std::uint64_t* action,int mode,float){
 const std::string name(reinterpret_cast<const char*>(*action));
 if(inputFailure176&&name==inputFailure176)throw 176; // host exception exercises production failed native leaf
 calls.push_back(name+":"+std::to_string(mode));
 if(name=="attack_abort"&&mode==1)latch119=2;
 if(name=="prepare_to_attack"&&mode==1&&latch119==0){
  const auto old=hostReturnAddressValue;hostReturnAddressValue=reinterpret_cast<void*>(WhBase()+0x27C08E6);
  NativeInputBegin117(punchControlR19.data(),18);hostReturnAddressValue=old;latch119=1;
 }
 if((name=="attack_primary"||name=="attack_primary_mouse")&&mode==2){
  if(latch119==1){const auto old=hostReturnAddressValue;hostReturnAddressValue=reinterpret_cast<void*>(WhBase()+0x27C0916);
   NativeInputRelease117(punchControlR19.data(),18);hostReturnAddressValue=old;}
  accepted119+=latch119==1;latch119=0;
 }
 if(inputReentry176)inputReentry176(name,mode);
}
template<class T>void put(unsigned char*p,T x){std::memcpy(p,&x,sizeof(x));}
static int Count(const char* key){return int(std::count(calls.begin(),calls.end(),key));}
static unsigned char* state104=nullptr;
static void __fastcall NativeRequest104Test(void*,int id,bool held){
 calls.push_back("semantic:"+std::to_string(id)+":"+(held?"1":"0"));
 put(state104+0x248,id);put(state104+0x2A8,std::uint8_t(held));
}
static void __fastcall NativeZone104Test(void*,int zone,std::int64_t,bool){put(state104+0xF8,zone);}
int main(){
 kcd2vr::retail_build112::hostRetailVerified112=true;
 std::vector<unsigned char> image(0x5400000);hostModuleHandleW=image.data();g_wh_pinned104=0;const auto base=WhBase();hostTickCount64=10000;
 std::array<unsigned char,0x100> entity{};std::array<unsigned char,0x500> actor{};std::array<unsigned char,0xE00> state{};state104=state.data();
 std::array<unsigned char,0x100> manager{};std::array<unsigned char,0xE00> player{};std::array<unsigned char,0x300> slots{};std::array<unsigned char,0x30> input{};
 put(manager.data(),base+0x3EC9958);put(manager.data()+0x38,static_cast<void*>(player.data()));put(player.data(),base+0x3AA4B98);
 put(player.data()+0x9D0,static_cast<void*>(manager.data()));put(player.data()+0x990,static_cast<void*>(slots.data()));put(player.data()+0xB88,static_cast<void*>(input.data()));
 put(player.data()+0x38,entity.data());put(entity.data(),base+0x4072C78);g_last_player_entity=entity.data();
 put(player.data()+0x278,actor.data());put(actor.data(),base+0x3A80610);put(actor.data()+0x1F8,player.data());put(actor.data()+0x210,state.data());put(state.data()+0xCE0,actor.data());
 put(image.data()+0x5321010,17);put(image.data()+0x5321014,18);put(image.data()+0x5321018,19);
 g_native_contract104=base;g_native_request104=&NativeRequest104Test;g_native_zone104=&NativeZone104Test;
 g_input_contract117=base;g_input_begin117=g_input_release117=&PunchLeafR19;
 put(punchControlR19.data()+0x18,actor.data());
 put(input.data(),base+0x3EC99A8);put(input.data()+0x10,static_cast<void*>(player.data()));g_henry_item_attach_manager=manager.data();
 g_combat_contract101=base;g_combat_input_original101=&NativeInput;g_combat_bypass101=&NativeInput;
 const char* names[]{"prepare_to_attack","attack_primary","attack_abort"};for(int i=0;i<3;++i)g_fist_action_names101[i]=reinterpret_cast<std::uintptr_t>(names[i]);
 g_census_mode=5;g_weapon_control_mode=0;g_gameplay_allowed=g_head_valid=g_right_valid=g_left_valid=true;g_pose_epoch6689=1;g_fist_epoch101=2;
 FistOwner101 owner{};Check(FistReadOwner101(owner)&&owner.empty&&owner.player==player.data(),"real ownership reader qualifies exact empty native hands");
 auto dispatch=[&](const char* n,int mode){auto a=reinterpret_cast<std::uint64_t>(n);CombatInputDetour101(input.data(),&a,mode,mode==2?0.f:1.f);};
 dispatch("prepare_to_attack",1);dispatch("attack_primary_mouse",1);dispatch("block",1);dispatch("attack_special",1);
 Check(calls.size()==2&&Count("attack_special:1")==1&&Count("block:1")==1,"physical fists retain native R3 and manual block while consuming raw punch input");
 dispatch("attack_special",4);Check(Count("attack_special:4")==1,"native special hold retains its original phase");calls.clear();
 g_census_mode=0;dispatch("attack_primary",2);dispatch("block",2);dispatch("attack_special",2);
 Check(calls.size()==2&&Count("attack_special:2")==1&&Count("block:2")==1,"exit balances native block/special releases and consumes owned raw punch release");calls.clear();
 dispatch("jump",1);Check(calls.size()==1&&calls.back()=="jump:1","unrelated native actions pass through");calls.clear();
 kcd2vr::diagnostics::capture168::gate.select(kcd2vr::diagnostics::capture168::Domain::Combat,true,hostTickCount64);
 cc::select(true,hostTickCount64);ObserveR3Combat101(true);dispatch("prepare_to_attack",1);cc::Event event{};
 Check(cc::pop(event)&&event.kind==cc::Kind::R3&&event.flags==1,"reference captures R3 edge even independently of native action delivery");
 Check(cc::pop(event)&&event.kind==cc::Kind::Input&&!std::strcmp(event.name,"prepare_to_attack")&&(event.flags&2)&&event.key[0]==0,"passive recorder captures actual native action, R3 level and hand keys");
 ObserveR3Combat101(true);Check(!cc::pop(event),"held R3 does not flood diagnostic queue");
 ObserveR3Combat101(false);Check(cc::pop(event)&&event.kind==cc::Kind::R3&&event.flags==0,"reference captures R3 release separately from native input");
 cc::select(false,hostTickCount64);kcd2vr::diagnostics::capture168::gate.mask=0;
 dispatch("attack_primary",2);Check(Count("attack_primary:2")==1,"reference mode does not alter native release");calls.clear();
 g_census_mode=5;const auto beforeEntry=accepted119;FistNativeTick101();Check(Count("attack_abort:1")==1&&Count("attack_abort:2")==1&&Count("attack_primary:2")==1&&latch119==0&&accepted119==beforeEntry,"entry aborts then clears cancelled latch without committing a punch");calls.clear();
 TrackingSnapshot t{};t.allowed=t.head_valid=t.right_valid=t.left_valid=true;t.head={0,1.6f,0};t.hq={0,0,0,1};t.right={.2f,1.25f,-.2f};t.left={-.2f,1.25f,-.2f};
 CommittedFrame6695 body{};body.stamp.epoch=1;body.stamp.actor=0x1234;body.valid[0]=body.valid[1]=true;body.allowed[0]={{0,0,0,1},{.2f,.2f,1.25f}};body.allowed[1]={{0,0,0,1},{-.2f,.2f,1.25f}};bool blocked[2]{};
 auto pose=[&](float movement){hostTickCount64+=10;t.tick=hostTickCount64;++t.sequence;body.stamp.tick=t.tick;body.stamp.sample=t.sequence;t.right.z-=movement;body.allowed[0].t[1]+=movement;NativeFrameBegin104();FistNativeTick101();NativeFrameEnd104();FistPublish101(t,body,blocked);};
 pose(0);t.right_grip=t.right_trigger=1;for(int i=0;i<30;++i)pose(0);Check(calls.empty(),"real committed-pose bridge does not attack from closing/holding fist");
 for(int i=0;i<12;++i)pose(.018f);Check(Count("prepare_to_attack:1")==1&&Count("attack_primary:2")==1,"actual committed motion stages prepare press then primary release across distinct original frames");
 Check(accepted119==beforeEntry+1&&latch119==0,"punch reaches native prepared latch instead of being ignored after cancellation");
 Check(punchClassesR19.size()>=2&&punchClassesR19[punchClassesR19.size()-2]==18&&punchClassesR19.back()==18,
       "H19 actual native Begin and Release preserve the same live HEAVY input used by authored unarmed rows");
 const auto once=calls.size();for(int i=0;i<10;++i){FistPublish101(t,body,blocked);FistNativeTick101();}Check(calls.size()==once,"repeated eye/pose consumption cannot repeat native punch");
 // Explicit packet: context loss may release only our native request, never mouse attack.
 hostTickCount64+=300;FistFrame101 frame{};frame.request=g_fist_epoch101;frame.player=player.data();frame.sample.valid=true;frame.sample.epoch=1;frame.sample.tick=hostTickCount64;frame.sample.empty[0]=frame.sample.empty[1]=true;frame.sample.grip[0]=frame.sample.trigger[0]=1;frame.punch={true,0,999,hostTickCount64,1,0x1234,2,.2};g_fist_frame101.publish(frame);
 calls.clear();NativeFrameBegin104();FistNativeTick101();Check(Count("prepare_to_attack:1")==1&&Count("attack_primary:2")==0,"pending punch asserts only prepare before a later native-frame commit");
 g_right_valid=false;++hostTickCount64;NativeFrameBegin104();FistNativeTick101();Check(Count("attack_abort:1")==1&&Count("attack_abort:2")==1&&Count("attack_primary:2")==1&&latch119==0&&accepted119==beforeEntry+1,"tracking loss aborts then clears only the cancelled latch without firing");g_right_valid=true;
 // Native positive item ID is deliberately not dereferenced as an empty sentinel.
 put(player.data()+0x9D0,static_cast<void*>(nullptr));Check(!FistReadOwner101(owner),"broken owner chain cannot authorize unarmed requests");put(player.data()+0x9D0,static_cast<void*>(manager.data()));
 g_census_mode=5;Check(FistTransportTrigger101(0,1,true,false)==0,"transport trigger consumed independently of finger snapshot");
 Check(FistTransportTrigger101(1,.5f,true,false,.3f)==0,"H4 left trigger cannot arm native kick before fist Grip crosses .70");
 Check(FistTransportTrigger101(1,1,true,false,0)==0,"H4 physical boxing owns left trigger even with an open Grip");
 Check(FistTransportTrigger101(1,1,true,true,0)==1,"H4 menu owner still receives left trigger");
 g_census_mode=0;Check(FistTransportTrigger101(0,1,true,false)==0,"transport quarantine survives exit");Check(FistTransportTrigger101(0,1,true,true)==1,"menu trigger is not stolen by gameplay test");Check(FistTransportTrigger101(0,0,false,false)==0&&FistTransportTrigger101(0,1,true,false)==0,"tracking loss is not physical release");FistTransportTrigger101(0,0,true,false);Check(FistTransportTrigger101(0,1,true,false)==1,"real tracked release restores ordinary trigger transport");
 FistTransportTrigger101(1,0,true,false);
 // The actual public ladder, with an already installed mock input trampoline.
 SetCombatCensusMode(4);CycleCombatCensusMode();Check(CombatCensusMode()==5&&WeaponControlMode()==0,"actual F10 collision rung advances to fist test exclusively");CycleCombatCensusMode();Check(CombatCensusMode()==0,"next press exits fist test to OFF");
 // Permission-cache efficiency is measured on the actual complete input hook.
 g_census_mode=5;g_weapon_control_mode=0;g_fist_quarantine101=0;
 const auto beforeQ=g_read_queries101R2.load(),beforeReuse=g_read_hits101R2.load();
 dispatch("attack_primary_mouse",1);
 Check(g_read_queries101R2.load()-beforeQ<8&&g_read_hits101R2.load()-beforeReuse>=15,
       "actual input callback shares page checks across action characters and owner fields");
 g_census_mode=0;g_fist_quarantine101=0;cc::select(false,hostTickCount64);
 // Press/release edges intentionally inspect current native input ownership,
 // even with F4 off (external prepare cancellation). Only an unselected hold
 // takes the zero-read fast path. Start cold: literal/page layout must not
 // decide whether this assertion passes through the 150ms page memo.
 g_page_memo169={};InvalidateReadCache101R2();
 const auto quietQueries=g_read_queries101R2.load();dispatch("jump",4);
 Check(g_read_queries101R2.load()==quietQueries,"unselected hold callback performs no guarded owner reads even with a cold page memo");
 dispatch("jump",1);
 Check(g_read_queries101R2.load()>quietQueries&&Count("jump:1")==1,"native press edge still reads current ownership and passes unrelated action through");
 calls.clear();SetHolsterCombatDiagMode(1);
 Check(!cc::active(hostTickCount64)&&!FistSelected101(),"internal native pose comparison cannot start a recorder");
 kcd2vr::diagnostics::capture168::gate.select(kcd2vr::diagnostics::capture168::Domain::Combat,true,hostTickCount64);
 dispatch("attack_primary_mouse",1);dispatch("attack_primary_mouse",2);
 Check(Count("attack_primary_mouse:1")==1&&Count("attack_primary_mouse:2")==1,"F3 input capture preserves native mouse attack pair");
 std::array<unsigned char,0x20> collision{};
 put(collision.data()+8,actor.data());put(actor.data()+0x218,collision.data());
 Check(ReferenceSweepOwner113(collision.data()),"reference sweep resolves exact Henry independent of F12 metadata");
 put(actor.data()+0x218,static_cast<void*>(nullptr));
 Check(!ReferenceSweepOwner113(collision.data()),"reference sweep rejects replaced collision owner");
 put(actor.data()+0x218,collision.data());put(manager.data()+0x38,static_cast<void*>(nullptr));
 Check(!ReferenceSweepOwner113(collision.data()),"reference sweep rejects broken player backlink");
 put(manager.data()+0x38,player.data());
 // Reference must remain native even when F12 physical-control selection persists.
 g_weapon_control_mode=6;g_fist_quarantine101=0;calls.clear();
 dispatch("prepare_to_attack",1);dispatch("attack_primary",2);
 Check(Count("prepare_to_attack:1")==1&&Count("attack_primary:2")==1,"F3 reference with F12=6 does not swallow native attacks");
 kcd2vr::diagnostics::capture168::gate.select(kcd2vr::diagnostics::capture168::Domain::Interaction,true,hostTickCount64);
 cc::selectSources(cc::InteractionReference,hostTickCount64,true);cc::Event discard{};while(cc::pop(discard)){}
 dispatch("contextual_superaction",1);dispatch("contextual_superaction",2);
 int contextual=0;while(cc::pop(discard))if(!std::strcmp(discard.name,"contextual_superaction"))++contextual;
 Check(contextual==2&&Count("contextual_superaction:1")==1&&Count("contextual_superaction:2")==1,"F5 recorder observes contextual press/release without replacing action");
 cc::selectSources(cc::CombatReference,hostTickCount64,true);g_weapon_control_mode=0;
 SetHolsterCombatDiagMode(0);Check(cc::active(hostTickCount64),"internal gameplay change cannot stop explicitly selected recorder");
 kcd2vr::diagnostics::capture168::gate.mask=0;
 calls.clear();g_census_mode=5;g_weapon_control_mode=0;g_fist_quarantine101=0;
 dispatch("attack_abort",1);dispatch("attack_abort",2);
 Check(Count("attack_abort:1")==0&&Count("attack_abort:2")==0,"physical fists consume unrelated raw attack_abort edges");
 g_census_mode=0;g_weapon_control_mode=6;g_fist_quarantine101=0;calls.clear();
 dispatch("attack_special",1);dispatch("attack_special",4);dispatch("attack_special",2);
 Check(calls.size()==3&&Count("attack_special:1")==1&&Count("attack_special:4")==1&&Count("attack_special:2")==1,"F12 physical weapon input leaves native R3 lifecycle intact");calls.clear();
 dispatch("attack_abort",1);dispatch("attack_abort",2);
 Check(Count("attack_abort:1")==1&&Count("attack_abort:2")==1,"F12 selection without a fresh eligible weapon owner preserves native abort");
 g_weapon_control_mode=0;g_fist_quarantine101=0;calls.clear();dispatch("attack_abort",1);dispatch("attack_abort",2);
 Check(Count("attack_abort:1")==1&&Count("attack_abort:2")==1,"native OFF mode retains ordinary abort input");
 g_weapon_control_mode=7;g_phys_input6698=false;g_phys_fault6698=false;g_gameplay_allowed=true;
 FistTransportTrigger101(0,0,true,false);
 Check(FistTransportTrigger101(0,1,true,false)==1,"F12=7 selection without a weapon input owner preserves gun/crossbow trigger");
 g_phys_input6698=true;g_phys_input_tick6698=hostTickCount64;
 Check(FistTransportTrigger101(0,1,true,false)==0,"F12=7 fresh melee/bow ownership consumes competing raw trigger");
 kcd2vr::diagnostics::lab::Set(kcd2vr::diagnostics::lab::Key::F3,1);FistTransportTrigger101(0,0,true,false);
 Check(FistTransportTrigger101(0,1,true,false)==1,"F12=7 native reference releases raw trigger ownership after tracked release");
 kcd2vr::diagnostics::lab::Set(kcd2vr::diagnostics::lab::Key::F3,0);hostTickCount64+=101;
 Check(FistTransportTrigger101(0,1,true,false)==1,"F12=7 stale owner cannot steal native trigger");
 g_census_mode=0;g_weapon_control_mode=6;g_phys_input6698=false;
 FistTransportTrigger101(0,0,true,false);
 Check(FistTransportTrigger101(0,1,true,false)==1,"F12=6 without current melee/bow ownership preserves gun/crossbow trigger");
 g_phys_input6698=true;g_phys_input_tick6698=hostTickCount64;
 Check(FistTransportTrigger101(0,1,true,false)==0,"F12=6 fresh physical owner consumes competing stock attack");
 kcd2vr::diagnostics::lab::Set(kcd2vr::diagnostics::lab::Key::F3,1);FistTransportTrigger101(0,0,true,false);
 Check(FistTransportTrigger101(0,1,true,false)==1,"F12=6 reference keeps stock trigger after release");
 g_census_mode=5;g_weapon_control_mode=0;FistTransportTrigger101(1,0,true,false);
 Check(FistTransportTrigger101(1,1,true,false)==1,"native reference also preserves fist-mode stock block");
 kcd2vr::diagnostics::lab::Set(kcd2vr::diagnostics::lab::Key::F3,0);
 // F10 disallows boxing through authored hit/clinch ownership and explains it
 // with a bounded POD event, without an extra speculative native dispatch.
 g_census_mode=5;g_weapon_control_mode=0;g_f11_test167=0;
 kcd2vr::diagnostics::lab::Set(kcd2vr::diagnostics::lab::Key::F3,2);
 kcd2vr::diagnostics::lab::Set(kcd2vr::diagnostics::lab::Key::F5,0);
 kcd2vr::diagnostics::capture168::gate.select(kcd2vr::diagnostics::capture168::Domain::Combat,true,hostTickCount64);
 cc::Event gate167{};while(cc::pop(gate167)){}
 kcd2vr::animation_filter::nativeBodyOwned112=true;const auto acceptedBefore167=accepted119;
 FistNativeTick101();bool sawGate167=false;
 while(cc::pop(gate167))if(std::strstr(gate167.name,"F10_FIST_GATE_CLOSED"))sawGate167=(gate167.flags&2)!=0;
 Check(sawGate167&&accepted119==acceptedBefore167,"F10 authored-body handoff reports gate only when F3 recorder selected and never punches through it");
 FistNativeTick101();Check(!cc::pop(gate167),"F10 blocked-owner diagnostic does not spam per native frame");
 kcd2vr::diagnostics::capture168::gate.mask=0;hostTickCount64+=2000;
 FistNativeTick101();Check(!cc::pop(gate167)&&accepted119==acceptedBefore167,"native body protection remains effective with recorder OFF");
 kcd2vr::animation_filter::nativeBodyOwned112=false;g_f11_test167=0;
 // V176: exercise the actual copied-equipment predicate and complete input
 // detour. This is not a substitute transport policy or a native gameplay run.
 g_census_mode=3;g_weapon_control_mode=7;g_combat_experiment175=true;
 g_phys_input6698=false;g_phys_fault6698=false;g_gameplay_allowed=true;
 g_fist_quarantine101=0;g_guard_manual112=false;g_guard_physical112=false;
 kcd2vr::diagnostics::lab::Set(kcd2vr::diagnostics::lab::Key::F3,0);
 g_phys_equipment6698.publish(PhysEquipment6698{});
 const auto noPoll176=g_read_queries101R2.load();
 Check(F4GestureInputOwned176()&&g_read_queries101R2.load()==noPoll176,
       "F4 cold equipment excludes stock melee without polling a native object");
 Check(FistTransportTrigger101(0,1,true,false,0)==0&&FistTransportTrigger101(1,1,true,false,0)==0,
       "F4 raw LT and RT are excluded without Grip or a fresh weapon pose");
 PhysEquipment6698 equipment176{};equipment176.valid=true;equipment176.count=1;
 equipment176.player=player.data();equipment176.manager=manager.data();
 equipment176.tick=hostTickCount64;equipment176.epoch=g_pose_epoch6689.load();
 equipment176.request=g_weapon_request_epoch6694.load();
 equipment176.handKnown[0]=equipment176.handKnown[1]=true;
 equipment176.handKey[0]=77;equipment176.heldIndex[0]=0;
 for(int cls:{0,1,4,6,8,11,12,16,17}){
   equipment176.items[0].profile.classId=cls;g_phys_equipment6698.publish(equipment176);
   Check(F4GestureInputOwned176(),"F4 melee/shield/empty-context class retains gesture ownership");
 }
 for(int cls:{9,10,13,14,15}){
   equipment176.items[0].profile.classId=cls;g_phys_equipment6698.publish(equipment176);
   Check(!F4GestureInputOwned176(),"verified bow/crossbow/rifle class retains separate ranged controls");
   FistTransportTrigger101(0,0,true,false);FistTransportTrigger101(1,0,true,false);
   Check(FistTransportTrigger101(0,1,true,false)==1&&FistTransportTrigger101(1,1,true,false)==1,
         "ranged trigger transport is available after an actual neutral release");
 }
 equipment176.tick=hostTickCount64-81;g_phys_equipment6698.publish(equipment176);
 Check(F4GestureInputOwned176(),"stale ranged exemption cannot reopen native melee input");
 equipment176.tick=hostTickCount64;equipment176.count=9;g_phys_equipment6698.publish(equipment176);
 Check(F4GestureInputOwned176(),"invalid copied inventory count cannot index outside the bounded equipment array");
 equipment176.count=1;equipment176.items[0].profile.classId=1;
 equipment176.manager=nullptr;g_phys_equipment6698.publish(equipment176);
 Check(F4GestureInputOwned176(),"replaced equipment manager closes ranged exemption");
 equipment176.manager=manager.data();g_phys_equipment6698.publish(equipment176);
 calls.clear();g_fist_quarantine101=0;g_guard_manual_owner112=player.data();g_guard_manual112=true;
 for(const char* n:{"prepare_to_attack","attack_primary","attack_special","attack_abort","block"})
   for(int mode:{1,4,2})dispatch(n,mode);
 Check(calls.empty(),"F4 actual OnAction detour excludes external canned melee/block/special lifecycle");
 Check(g_guard_manual112.load(),"suppressed F4 release preserves inherited native block flag for the one-shot guard-pump drain");
 g_guard_manual112=false;

 g_phys_native6698.actionNames[3]=reinterpret_cast<std::uintptr_t>("block");
 Check(PhysCallAction6698(player.data(),3,1)&&PhysCallAction6698(player.data(),3,2)&&
       Count("block:1")==1&&Count("block:2")==1,
       "our positional guard still reaches the original native input through its existing bypass");
 calls.clear();dispatch("jump",1);
 Check(Count("jump:1")==1,"F4 combat-only exclusion preserves unrelated native actions");
 calls.clear();g_gameplay_allowed=false;g_fist_quarantine101=15;g_guard_physical112=true;g_guard_physical_owner112=player.data();
 dispatch("prepare_to_attack",1);dispatch("attack_primary",2);dispatch("block",1);dispatch("block",2);
 Check(calls.size()==4,"outside gameplay the F4 detour preserves authored native input phases");
 g_gameplay_allowed=true;g_guard_manual112=false;g_guard_physical112=false;g_combat_experiment175=false;
 FistTransportTrigger101(0,0,true,false);FistTransportTrigger101(1,0,true,false);
 Check(!F4GestureInputOwned176()&&FistTransportTrigger101(1,1,true,false)==1,
       "F4 exit and real neutral release restore baseline native LT");
 // Actual external-input lease and native-pump drain. No inferred native hit.
 auto externalBase176=[&](){
   inputReentry176=nullptr;inputFailure176=nullptr;ExternalPrepareForget176();g_fist_quarantine101=0;
   g_census_mode=0;g_weapon_control_mode=0;g_combat_experiment175=false;
   g_gameplay_allowed=true;kcd2vr::animation_filter::nativeBodyOwned112=false;
   kcd2vr::animation_filter::nativeKickActive175=false;g_phys_native6698.attackPrepared=false;
   g_fist_native101.prepared=false;g_native_pending104.held=false;g_phys_armed6698=false;
   g_guard_manual112=false;g_guard_physical112=false;g_phys_fault6698=false;
   put(slots.data()+0x260,std::uint64_t(0));put(slots.data()+0x268,std::uint64_t(0));
   g_phys_equipment6698.publish(PhysEquipment6698{});
   g_phys_native6698.actionNames[0]=reinterpret_cast<std::uintptr_t>("prepare_to_attack");
   g_phys_native6698.actionNames[1]=reinterpret_cast<std::uintptr_t>("attack_primary");
   g_phys_native6698.actionNames[2]=reinterpret_cast<std::uintptr_t>("attack_abort");
   g_phys_native6698.actionNames[4]=reinterpret_cast<std::uintptr_t>("attack_special");
   latch119=0;calls.clear();
 };
 auto enterF4176=[&](){ExternalPrepareTakeover176();g_census_mode=4;g_weapon_control_mode=7;g_combat_experiment175=true;};
 externalBase176();const auto acceptedStart176=accepted119;dispatch("prepare_to_attack",1);
 Check(latch119==1&&g_external_prepare176.serial,"all-off actual delivered native prepare is recorded with exact owner");
 enterF4176();dispatch("attack_primary",2);Check(latch119==1&&accepted119==acceptedStart176,"F4 suppresses release before a usable pose without committing prepared input");
 calls.clear();PhysicalNativeFrameTick6698();
 // V66.178R28: with empty hands every F4 rung auto-selects boxing (R22C2), whose first ownership adds its own native
 // phase cancellation (abort/primary/abort) after the external-prepare drain - two identical, idempotent sequences.
 Check(latch119==0&&accepted119==acceptedStart176&&Count("attack_abort:1")>=1&&Count("attack_abort:1")<=2&&Count("attack_primary:2")==Count("attack_abort:1")&&Count("attack_abort:2")==Count("attack_abort:1"),
       "actual native pump aborts before clearing prior external RT preparation without accepted attack");
 Check(!g_external_prepare176.serial,"drained external preparation cannot replay");calls.clear();PhysicalNativeFrameTick6698();
 Check(Count("attack_abort:1")==0,"repeated native pump never repeats external cancellation");
 g_combat_experiment175=false;g_weapon_control_mode=0;g_census_mode=0;dispatch("attack_primary",2);
 Check(latch119==0&&accepted119==acceptedStart176,"exit release cannot fire the drained native preparation");
 externalBase176();dispatch("prepare_to_attack",1);enterF4176();g_combat_experiment175=false;g_weapon_control_mode=0;g_census_mode=0;
 dispatch("attack_primary",2);Check(latch119==1&&accepted119==acceptedStart176,"exit before native pump retains pending cancellation and suppresses unsafe release");
 PhysicalNativeFrameTick6698();Check(latch119==0&&accepted119==acceptedStart176,"delayed native pump completes cancellation after F4 exit");
 externalBase176();dispatch("prepare_to_attack",1);enterF4176();g_gameplay_allowed=false;calls.clear();PhysicalNativeFrameTick6698();
 Check(latch119==1&&g_external_prepare176.serial&&calls.empty(),"UI/context loss defers pending cancel without native calls");
 g_gameplay_allowed=true;PhysicalNativeFrameTick6698();Check(latch119==0&&accepted119==acceptedStart176,"returning gameplay drains retained external preparation");
 externalBase176();enterF4176();calls.clear();PhysicalNativeFrameTick6698();Check(calls.empty(),"no observed external prepare means no cancellation");
 externalBase176();dispatch("prepare_to_attack",1);enterF4176();g_phys_native6698.attackPrepared=true;calls.clear();ExternalPrepareDrain176();
 Check(calls.empty()&&!g_external_prepare176.serial&&g_phys_native6698.attackPrepared,"old external lease cannot abort a current physical-owned preparation");
 externalBase176();dispatch("prepare_to_attack",1);enterF4176();++g_pose_epoch6689;calls.clear();ExternalPrepareDrain176();
 Check(calls.empty()&&!g_external_prepare176.serial,"changed native pose epoch discards old cancel authority");--g_pose_epoch6689;
 externalBase176();dispatch("prepare_to_attack",1);enterF4176();put(state.data()+0xCE0,static_cast<void*>(nullptr));calls.clear();ExternalPrepareDrain176();
 Check(calls.empty()&&!g_external_prepare176.serial,"broken reciprocal actor identity cannot authorize cancellation");put(state.data()+0xCE0,actor.data());
 for(int cls:{9,10,13,14,15}){
   externalBase176();put(slots.data()+0x260,std::uint64_t(77));
   PhysEquipment6698 ranged{};ranged.valid=true;ranged.tick=hostTickCount64;ranged.epoch=g_pose_epoch6689;ranged.request=g_weapon_request_epoch6694;
   ranged.manager=manager.data();ranged.player=player.data();ranged.count=1;ranged.handKnown[0]=ranged.handKnown[1]=true;
   ranged.handKey[0]=77;ranged.heldIndex[0]=0;ranged.items[0].profile.key=77;ranged.items[0].profile.side=0;ranged.items[0].profile.classId=cls;
   g_phys_equipment6698.publish(ranged);dispatch("prepare_to_attack",1);enterF4176();calls.clear();ExternalPrepareDrain176();
   Check(calls.empty()&&!g_external_prepare176.serial&&latch119==1,"verified ranged preparation remains native and is never canceled as melee");
 }
 externalBase176();g_gameplay_allowed=false;dispatch("prepare_to_attack",1);enterF4176();calls.clear();ExternalPrepareDrain176();
 Check(calls.empty()&&!g_external_prepare176.serial&&latch119==1,"authored native UI prepare never creates gameplay cancellation authority");
 externalBase176();
 inputReentry176=[](const std::string& name,int mode){if(name!="prepare_to_attack"||mode!=1)return;inputReentry176=nullptr;
   void* p=nullptr;Read(static_cast<unsigned char*>(g_henry_item_attach_manager.load())+0x38,p);void* in=nullptr;Read(static_cast<unsigned char*>(p)+0xB88,in);
   const auto abort=reinterpret_cast<std::uint64_t>("attack_abort");CombatInputDetour101(in,&abort,1,1.f);
   const auto release=reinterpret_cast<std::uint64_t>("attack_primary");CombatInputDetour101(in,&release,2,0.f);};
 dispatch("prepare_to_attack",1);Check(latch119==0&&!g_external_prepare176.serial,"nested release/abort prevents old prepare callback from reviving its lease");
 externalBase176();dispatch("prepare_to_attack",1);dispatch("attack_primary",2);
 Check(!g_external_prepare176.serial&&latch119==0,"ordinary delivered native release retires external lease without changing native semantics");
 externalBase176();dispatch("prepare_to_attack",1);calls.clear();const auto idleReads176=g_read_queries101R2.load();
 for(int i=0;i<20;++i)ExternalPrepareDrain176();
 Check(calls.empty()&&g_read_queries101R2.load()==idleReads176&&g_external_prepare176.serial,
       "normal all-off native hold performs no added native owner polling or cancellation");
 externalBase176();g_combat_input_original101=nullptr;dispatch("prepare_to_attack",1);g_combat_input_original101=&NativeInput;
 Check(!g_external_prepare176.serial&&latch119==0,"undelivered input cannot publish external preparation authority");
 externalBase176();put(slots.data()+0x260,std::uint64_t(77));
 PhysEquipment6698 melee176{};melee176.valid=true;melee176.tick=hostTickCount64;melee176.epoch=g_pose_epoch6689;melee176.request=g_weapon_request_epoch6694;
 melee176.manager=manager.data();melee176.player=player.data();melee176.count=1;melee176.handKnown[0]=melee176.handKnown[1]=true;
 melee176.handKey[0]=77;melee176.heldIndex[0]=0;melee176.items[0].profile.key=77;melee176.items[0].profile.side=0;melee176.items[0].profile.classId=1;
 g_phys_equipment6698.publish(melee176);dispatch("prepare_to_attack",1);enterF4176();
 g_combat_experiment175=false;g_weapon_control_mode=0;g_census_mode=0;++g_weapon_request_epoch6694;hostTickCount64+=200;
 const auto acceptedArmed176=accepted119;dispatch("attack_primary",2);calls.clear();PhysicalNativeFrameTick6698();
 Check(latch119==0&&accepted119==acceptedArmed176&&Count("attack_abort:1")==1,
       "qualified armed preparation cancels after Off changes request epoch and expires equipment publication");
 externalBase176();put(slots.data()+0x260,std::uint64_t(88));dispatch("prepare_to_attack",1);enterF4176();calls.clear();ExternalPrepareDrain176();
 Check(calls.empty()&&latch119==1&&g_external_prepare176.serial,
       "unclassified occupied hand retains quarantine without guessing melee or issuing abort");
 externalBase176();dispatch("prepare_to_attack",1);enterF4176();inputFailure176="attack_abort";calls.clear();ExternalPrepareDrain176();
 const auto failedSerial176=g_external_prepare176.serial;
 Check(failedSerial176&&g_external_prepare176.attempted&&latch119==1&&calls.empty(),
       "failed abort invocation retains exact preparation lease and never sends an unsafe primary release");
 inputFailure176=nullptr;g_phys_fault6698=false;g_combat_experiment175=false;g_weapon_control_mode=0;g_census_mode=0;
 for(int i=0;i<20;++i)ExternalPrepareDrain176();dispatch("attack_primary",2);
 Check(g_external_prepare176.serial==failedSerial176&&latch119==1&&calls.empty(),
       "failed cancel remains quarantined after exit without per-frame retry or later raw release");
 dispatch("attack_abort",1);dispatch("attack_primary",2);
 Check(!g_external_prepare176.serial&&latch119==0,
       "later actually delivered native abort retires failed cancellation and permits balanced native release");
 externalBase176();dispatch("prepare_to_attack",1);enterF4176();inputFailure176="attack_primary";calls.clear();ExternalPrepareDrain176();
 Check(g_external_prepare176.serial&&g_external_prepare176.attempted&&latch119==2&&Count("attack_abort:1")==1&&Count("attack_abort:2")==1,
       "primary-release failure retains lease after one successful abort and balances abort phase");
 inputFailure176=nullptr;g_phys_fault6698=false;ExternalPrepareDrain176();
 Check(Count("attack_abort:1")==1&&latch119==2,"partial cancellation cannot repeat its successful native abort");
 externalBase176();dispatch("prepare_to_attack",1);enterF4176();inputFailure176="attack_abort";ExternalPrepareDrain176();
 inputFailure176=nullptr;g_phys_fault6698=false;++g_pose_epoch6689;calls.clear();ExternalPrepareDrain176();
 Check(calls.empty()&&!g_external_prepare176.serial&&!g_external_prepare_serial176.load(),
       "failed cancellation retires stale pose epoch without another native call");--g_pose_epoch6689;
 externalBase176();dispatch("prepare_to_attack",1);enterF4176();inputFailure176="attack_abort";ExternalPrepareDrain176();
 inputFailure176=nullptr;g_phys_fault6698=false;std::array<unsigned char,0xE00> replacementState176{};
 put(replacementState176.data()+0xCE0,actor.data());put(actor.data()+0x210,replacementState176.data());calls.clear();ExternalPrepareDrain176();
 Check(calls.empty()&&!g_external_prepare176.serial&&!g_external_prepare_serial176.load(),
       "failed cancellation cannot quarantine replacement native state under the same player");put(actor.data()+0x210,state.data());
 externalBase176();dispatch("prepare_to_attack",1);enterF4176();calls.clear();
 inputReentry176=[](const std::string& name,int mode){if(name!="attack_abort"||mode!=1)return;inputReentry176=nullptr;
   ExternalPrepareForget176();};
 ExternalPrepareDrain176();
 Check(Count("attack_abort:1")==1&&Count("attack_primary:2")==0&&!g_external_prepare176.serial,
       "callback invalidation prevents old cancel sequence from continuing into a new publication");
 externalBase176();
 // H11 exact punch-sector selection and its native ACK; no call if either hand equips.
 g_census_mode=5;g_weapon_control_mode=0;g_fist_fault101=false;g_gameplay_allowed=g_head_valid=g_left_valid=g_right_valid=true;
 put(slots.data()+0x260,std::uint64_t(0));put(slots.data()+0x268,std::uint64_t(0));
 put(state.data()+0xF8,5);
 Check(FistSelectPunchH11(player.data(),0,g_pose_epoch6689.load())&&*reinterpret_cast<int*>(state.data()+0xF8)==1,
       "H19 right fist replaces leaked lower kick stance with authored right-hand hook sector1");
 Check(FistSelectPunchH11(player.data(),1,g_pose_epoch6689.load())&&*reinterpret_cast<int*>(state.data()+0xF8)==2,
       "H19 left fist requests authored left-hand hook sector2, never lower kick sector5");
 put(slots.data()+0x260,std::uint64_t(77));put(state.data()+0xF8,5);
 Check(!FistSelectPunchH11(player.data(),1,g_pose_epoch6689.load())&&*reinterpret_cast<int*>(state.data()+0xF8)==5,
       "H11 no weapon-plus-left-fist selection or native zone mutation");
 put(slots.data()+0x260,std::uint64_t(0));
 TrackingSnapshot h11{};h11.allowed=h11.head_valid=h11.left_valid=h11.right_valid=true;
 h11.tick=h11.head_good_tick=h11.left_good_tick=h11.right_good_tick=hostTickCount64;h11.sequence=900000;
 kcd2vr::tracking59c6::enabled=false;g_tracking_frames6695.publish(h11);
 Check(!FistHandsClosedH11()&&PhysicalHandReservations114()==0,"H11 selected boxing mode with open hands leaves selection/grabbing unreserved");
 h11.right_grip=1;g_tracking_frames6695.publish(h11);
 Check(!FistHandsClosedH11()&&PhysicalHandReservations114()==0,"H11 Grip-only pointing does not reserve hands for boxing");
 h11.right_trigger=1;g_tracking_frames6695.publish(h11);
 Check(FistHandsClosedH11()&&PhysicalHandReservations114()==3,"H11 Trigger plus Grip reserves boxing instead of interaction");
 FistContactFrameH11 contactH11{};contactH11.player=player.data();contactH11.request=g_fist_epoch101.load();contactH11.completedH31=true;
 contactH11.epoch=g_pose_epoch6689.load();contactH11.tick=hostTickCount64;contactH11.valid[0]=true;
 contactH11.sample=900000;contactH11.character=0x1234;
 CommittedFrame6695 committedH11{};committedH11.stamp.epoch=contactH11.epoch;committedH11.stamp.sample=contactH11.sample;
 committedH11.stamp.actor=contactH11.character;committedH11.valid[0]=true;g_committed_frame6695.publish(committedH11);
 contactH11.palm[0]={2,3,4};contactH11.previous[0]={2,2.9,4};g_fist_contact_h11.publish(contactH11);
 float closeH11[3]{2,3.05f,4},farH11[3]{2,3.5f,4};
 Check(FistContactAdmissionH11(actor.data(),closeH11,0)==1&&FistContactAdmissionH11(actor.data(),farH11,0)==0,
       "H11 native punch needs contact on current tracked palm sweep, not a distant native fist");
 h11.right_held=true;g_tracking_frames6695.publish(h11);
 Check(FistContactAdmissionH11(actor.data(),closeH11,0)==0,"H11 tracking grace cannot authorize a punch contact");
 h11.right_held=false;g_tracking_frames6695.publish(h11);hostTickCount64+=61;
 Check(FistContactAdmissionH11(actor.data(),closeH11,0)==0,"H11 stale palm cannot authorize delayed native punch damage");
 // U16: exercise the production fist consumer across a delayed native LT-up ACK.
 g_census_mode=5;g_weapon_control_mode=0;g_combat_experiment175=false;
 g_fist_fault101=false;g_phys_fault6698=false;g_gameplay_allowed=g_head_valid=g_right_valid=g_left_valid=true;
 kcd2vr::animation_filter::nativeBodyOwned112=false;
 kcd2vr::diagnostics::lab::Set(kcd2vr::diagnostics::lab::Key::F3,0);
 slots.fill(0);g_guard_manual112=false;g_guard_physical112=false;g_native_guard112={};
 put(state.data()+0x38,1);put(state.data()+0x218,-1);put(state.data()+0x248,-1);put(state.data()+0x278,0);
 put(state.data()+0x1B8,1);put(state.data()+0x2A8,std::uint8_t(0));put(state.data()+0x2D8,std::uint8_t(0));put(state.data()+0x308,std::uint8_t(0));
 put(state.data()+0x630,base+0x3B0A968);put(image.data()+0x3B0A970,base+0x1A7DDE0);
 const unsigned char ackPinU16[]{0x0F,0xB6,0xC2,0x0F,0xB6,0x44,0x08,0x18,0xC3};
 std::memcpy(image.data()+0x1A7DDE0,ackPinU16,sizeof(ackPinU16));
 g_phys_native6698.actionNames[3]=reinterpret_cast<std::uint64_t>("block");
 g_combat_bypass101=&NativeInput;g_combat_input_original101=&NativeInput;g_combat_hook_base101=base;g_combat_own_patch101.fill(0);
 NativeOwner104 guardOwnerU16{};Check(NativeResolve104(player.data(),guardOwnerU16),"U16 exact fist owner resolves");
 auto seedU16=[&](std::uint64_t serial){
   g_fist_native101={};g_fist_native101.owned=true;g_fist_native101.player=player.data();
   g_fist_native101.request=g_fist_epoch101.load();g_fist_native101.epoch=g_pose_epoch6689.load();
   FistFrame101 f{};f.player=player.data();f.request=g_fist_epoch101.load();f.sample.valid=true;
   f.sample.empty[0]=f.sample.empty[1]=true;f.sample.epoch=g_pose_epoch6689.load();f.sample.tick=hostTickCount64;
   f.sample.sequence=serial;f.sample.grip[0]=f.sample.grip[1]=f.sample.trigger[0]=f.sample.trigger[1]=1;
   f.punch={true,0,serial,hostTickCount64,g_pose_epoch6689.load(),1,1.5,.12,0};g_fist_frame101.publish(f);
   g_native_guard112={};g_native_guard112.owner=guardOwnerU16;g_native_guard112.held=true;
   g_guard_physical112=true;g_guard_physical_owner112=player.data();
   put(state.data()+0x648,std::uint8_t(1));calls.clear();latch119=0;
 };
 auto freshU16=[&](){FistFrame101 f{};g_fist_frame101.read(f);f.sample.tick=hostTickCount64;++f.sample.sequence;f.punch={};g_fist_frame101.publish(f);};
 seedU16(9001);FistNativeTick101();
 Check(g_fist_native101.waitingGuardU16&&!g_fist_native101.prepared&&Count("prepare_to_attack:1")==0,
       "U16 owned guard release queues original punch without preparing in stale guard context");
 ++hostTickCount64;freshU16();FistNativeTick101();
 Check(g_fist_native101.waitingGuardU16&&Count("prepare_to_attack:1")==0,"U16 no prepare before real context0 release ACK");
 put(state.data()+0x648,std::uint8_t(0));++hostTickCount64;freshU16();FistNativeTick101();
 Check(!g_fist_native101.waitingGuardU16&&g_fist_native101.prepared&&Count("prepare_to_attack:1")==1,
       "U16 exact original punch prepares once after native Update ACK");
 ++hostTickCount64;freshU16();FistNativeTick101();
 Check(!g_fist_native101.prepared&&Count("attack_primary:2")==1,"U16 punch releases once on later native frame");
 seedU16(9002);FistNativeTick101();hostTickCount64+=101;freshU16();FistNativeTick101();
 Check(!g_fist_native101.waitingGuardU16&&Count("prepare_to_attack:1")==0,"U16 original100ms gesture expires without stale replay");
 seedU16(9003);FistNativeTick101();g_guard_manual_owner112=player.data();g_guard_manual112=true;
 ++hostTickCount64;freshU16();FistNativeTick101();
 Check(!g_fist_native101.waitingGuardU16&&Count("prepare_to_attack:1")==0,"U16 manual LT takeover cancels queued physical punch");
 g_guard_manual112=false;
 // H19R2: an owned residual block is INPUT context, not a neutral-state
 // prerequisite. The original selector may replace it after context0 LT-up.
 for(int residualH19:{128,256}){
  seedU16(9101+residualH19);put(state.data()+0x38,residualH19);put(state.data()+0x278,7);put(state.data()+0xF8,5);FistNativeTick101();
  Check(g_fist_native101.waitingGuardU16&&Count("prepare_to_attack:1")==0,"H19 residual owned block still waits for actual context0 LT-up");
  put(state.data()+0x648,std::uint8_t(0));++hostTickCount64;freshU16();FistNativeTick101();
  Check(!g_fist_native101.waitingGuardU16&&g_fist_native101.prepared&&Count("prepare_to_attack:1")==1,
        "H19R2 native prepare is offered after LT-up ACK without waiting for quiet block metadata to disappear");
  int desiredH19=-1,phaseH19=0,metadataH19=0;Read(state.data()+0xF8,desiredH19);Read(state.data()+0x38,phaseH19);Read(state.data()+0x278,metadataH19);
  Check(desiredH19==1&&phaseH19==residualH19&&metadataH19==7,"H19R2 original direction setter changes desired sector only; current native phase/block metadata stay untouched");
  ++hostTickCount64;freshU16();FistNativeTick101();
  Check(!g_fist_native101.prepared&&Count("attack_primary:2")==1,"H19R2 original punch release stays on a later native frame");
 }
 seedU16(9102);put(state.data()+0x38,256);put(state.data()+0x278,7);FistNativeTick101();
 put(state.data()+0x648,std::uint8_t(0));hostTickCount64+=101;freshU16();FistNativeTick101();
 Check(!g_fist_native101.waitingGuardU16&&Count("prepare_to_attack:1")==0,
       "H19 guard handoff does not renew original100ms gesture deadline");
 put(state.data()+0x38,1);put(state.data()+0x278,0);++hostTickCount64;freshU16();FistNativeTick101();
 Check(Count("prepare_to_attack:1")==0,"H19 late native ACK cannot revive an expired punch");
 seedU16(9103);FistNativeTick101();put(state.data()+0x648,std::uint8_t(0));put(state.data()+0x38,64);
 ++hostTickCount64;freshU16();FistNativeTick101();
 Check(!g_fist_native101.waitingGuardU16&&Count("prepare_to_attack:1")==0,
       "H19 hit reaction during guard handoff retires punch without a direction write or prepare");
 put(state.data()+0x38,1);put(state.data()+0x278,0);
 seedU16(9104);FistNativeTick101();put(state.data()+0x648,std::uint8_t(0));
 auto replacementH19=state;put(actor.data()+0x210,replacementH19.data());
 ++hostTickCount64;freshU16();FistNativeTick101();
 Check(!g_fist_native101.waitingGuardU16&&Count("prepare_to_attack:1")==0,
       "H19 same player and pose epoch cannot transfer old punch into a replacement native state");
 put(actor.data()+0x210,state.data());
 seedU16(9105);FistNativeTick101();put(state.data()+0x648,std::uint8_t(0));
 put(state.data()+0x2A8,std::uint8_t(1));++hostTickCount64;freshU16();FistNativeTick101();
 Check(!g_fist_native101.waitingGuardU16&&Count("prepare_to_attack:1")==0,
       "H19 another native attack request cannot be stolen by waiting fist");
 put(state.data()+0x2A8,std::uint8_t(0));
 seedU16(9106);put(state.data()+0x38,256);put(state.data()+0x218,1);FistNativeTick101();
 put(state.data()+0x648,std::uint8_t(0));++hostTickCount64;freshU16();FistNativeTick101();
 Check(!g_fist_native101.waitingGuardU16&&g_fist_native101.prepared&&Count("prepare_to_attack:1")==1,
       "H19R2 original prepare may replace our released free-block type1 without waiting for neutral");
 put(state.data()+0x38,1);put(state.data()+0x218,-1);++hostTickCount64;freshU16();FistNativeTick101();
 Check(!g_fist_native101.prepared&&Count("attack_primary:2")==1,
       "H19R2 free-block handoff retains original later-frame release");
 ++hostTickCount64;freshU16();FistNativeTick101();
 {
  NativeOwner104 ownerR19{};Check(NativeResolve104(player.data(),ownerR19),"R19 exact native punch owner resolves");
  NativeInputScope117 scope{};scope.owner=ownerR19;scope.prepare=true;scope.stroke=ph::CommandKind::Slash;scope.fistPunchR19=true;
  auto* prior=g_input_scope117;g_input_scope117=&scope;
  Check(NativeInputClass117(punchControlR19.data(),18,base+0x27C08E6,true)==18,"H19 scoped empty-hand punch preserves authored HEAVY input");
  scope.fistPunchR19=false;
  Check(NativeInputClass117(punchControlR19.data(),18,base+0x27C08E6,true)==18,"R19 ordinary weapon swing keeps HEAVY input");
  scope.fistPunchR19=true;
  Check(NativeInputClass117(punchControlR19.data(),18,base+0x27C08E7,true)==18,"R19 different native caller remains unchanged");
  put(punchControlR19.data()+0x18,static_cast<void*>(nullptr));
  Check(NativeInputClass117(punchControlR19.data(),18,base+0x27C08E6,true)==18,"R19 foreign control cannot acquire punch semantics");
  put(punchControlR19.data()+0x18,actor.data());
  g_input_scope117=prior;
 }
 // H27 actual native owner -> Grip-only publisher -> staged native consumer.
 {
  std::array<unsigned char,0x240> enemyH27{};std::array<unsigned char,0xD10> enemyStateH27{};
  put(enemyH27.data(),base+0x3E3FAC8);put(enemyH27.data()+0x210,enemyStateH27.data());
  put(enemyStateH27.data()+0xCE0,enemyH27.data());put(enemyStateH27.data()+0xCF8,actor.data());put(state.data()+0xCF8,enemyH27.data());
  g_census_mode=5;g_weapon_control_mode=0;g_fist_fault101=false;g_gameplay_allowed=g_head_valid=g_right_valid=g_left_valid=true;
  g_ctx_menu=g_ctx_dialogue=g_ctx_cutscene=g_ctx_scripted=g_ctx_reading=false;kcd2vr::animation_filter::nativeBodyOwned112=false;
  kcd2vr::combat_rules178r10::useHeld=false;kcd2vr::workstation178r20c1::experimentArmed36=false;kcd2vr::hands59c7f::clear();
  kcd2vr::diagnostics::lab::Set(kcd2vr::diagnostics::lab::Key::F3,0);
  ++g_fist_epoch101;g_fist_native101={};g_fist_seen101=0;g_fist_motion_request101=0;g_fist_motion101={};g_fist_pending101={};
  t.right_grip=t.right_trigger=t.left_grip=t.left_trigger=0;t.allowed=t.head_valid=t.right_valid=t.left_valid=true;
  pose(0);pose(0);calls.clear();const auto acceptedH27=accepted119;
  FistOwner101 combatH27{};Check(FistReadOwner101(combatH27)&&combatH27.empty&&combatH27.responsiveCombatH27,"actual reciprocal current unarmedcombat grants responsive motion readiness with qualified squeeze");
  t.right_grip=.65f;t.right_trigger=.65f;pose(.012f);
  for(unsigned i=0;i<8;++i)pose(.012f);pose(0);pose(0);
  Check(Count("prepare_to_attack:1")==1&&Count("attack_primary:2")==1&&accepted119==acceptedH27+1,
    "actual Trigger-and-Grip qualified noquiet short jab crosses publisher and native prepare/release exactlyonce");
  g_ctx_dialogue=true;Check(FistReadOwner101(combatH27)&&!combatH27.responsiveCombatH27,"dialogue revokes Grip-only combat admission");g_ctx_dialogue=false;
  kcd2vr::combat_rules178r10::useHeld=true;Check(FistReadOwner101(combatH27)&&!combatH27.responsiveCombatH27,"native use ownsGrip before punching");kcd2vr::combat_rules178r10::useHeld=false;
  put(enemyStateH27.data()+0xCF8,enemyH27.data());Check(FistReadOwner101(combatH27)&&!combatH27.responsiveCombatH27,"nonreciprocal opponent cannot promote interactionGrip to punch");
  put(state.data()+0xCF8,static_cast<void*>(nullptr)); // retire the fixture opponent before its stack lifetime ends
 }
 { // Two different deliberate hands/strokes may arrive inside the old mod220ms timer.
  hostTickCount64+=300;seedU16(20001);
  g_native_guard112={};g_guard_physical112=false;g_guard_manual112=false;
  put(state.data()+0x38,1);put(state.data()+0x218,-1);put(state.data()+0x278,0);
  put(state.data()+0x648,std::uint8_t(0));put(state.data()+0x2A8,std::uint8_t(0));
  g_fist_native101.lastAttack=hostTickCount64-100;
  FistNativeTick101();
  Check(g_fist_native101.prepared&&Count("prepare_to_attack:1")==1,
    "fresh different punch inside220ms is offered once to the original native input selector");
  const auto firstOffer=calls.size();FistNativeTick101();
  Check(calls.size()==firstOffer,"same native frame cannot repeat prepare or release");
  ++hostTickCount64;freshU16();FistNativeTick101();
  Check(!g_fist_native101.prepared&&Count("attack_primary:2")==1,
    "short-interval punch retains original later-frame native release");
  FistFrame101 repeated{};g_fist_frame101.read(repeated);
  repeated.punch={true,0,20001,hostTickCount64,g_pose_epoch6689.load(),1,1.5,.12,0};
  g_fist_frame101.publish(repeated);FistNativeTick101();
  Check(Count("prepare_to_attack:1")==1&&Count("attack_primary:2")==1,
    "removing mod timer does not replay an already-consumed gesture");
 }
 std::printf("fist_bridge=%d/%d native_GAME_calls=REPLACED original_adapter_executed=1\n",total-failures,total);return failures?1:0;
}
