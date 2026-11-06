// Complete production TU; native held backend/memory leaves are explicit host services.
#include "../src/frik_pose_probe.cpp"
#include "native_held_backend_t4_stub.h"
#include <cstdio>
#include <array>
namespace kcd2vr::diagnostic_trace {void Write(const char*) noexcept {}}
using namespace kcd2vr::frik_pose_probe;
namespace f=kcd2vr::native_weapon_body_t4::fixture;
#include "generated_holster_side_retirement_h36.inc"
static unsigned tests=0,failed=0;
static void ck(bool b,const char* s){++tests;if(!b){++failed;std::printf("FAIL %s\n",s);}}
struct RigH36 {
 std::array<unsigned char,0x100> manager{};PhysEquipment6698 gear{};ph::Input in{};ph::Output out{};std::array<ph::HolsterState,8> states{};
 RigH36(){
  hostTickCount64=1000;g_pose_epoch6689=9;g_weapon_request_epoch6694=10;g_player_char37=0x3000;
  g_gameplay_allowed=true;g_ctx_menu=g_ctx_dialogue=g_ctx_cutscene=g_ctx_scripted=false;g_bow_manual_reload146=false;
  g_left_equipment_obligation_h36={};g_left_equipment_successors_h36={};g_left_equipment_established_h36={};g_left_equipment_overflow_h36=false;g_native_held_equipment_pending_h36=false;g_native_held_valid_t4=false;f::equipmentSuspendedH36=false;
  g_phys_commands6698.head=g_phys_commands6698.tail=0;g_phys_native6698={};g_phys_fault6698=false;g_weapon_release_pending_t1[0]=g_weapon_release_pending_t1[1]=0;
  f::enabled=true;f::pending=false;f::read=f::ownerRead=true;f::admissions=7;f::cancels=f::retires=f::submits=0;
  f::achieved={};f::achieved.state=heldT4::State::Waiting;f::achieved.colliderRetired=true;f::achieved.owner={77,8,10,9,0x2000,reinterpret_cast<std::uintptr_t>(manager.data()),0x3000,0x4000,0x5000,0x6000,70,0};
  gear.valid=true;gear.count=2;gear.selected=0;gear.request=10;gear.epoch=9;gear.tick=1000;gear.player=reinterpret_cast<void*>(0x2000);gear.manager=manager.data();
  gear.handKnown[0]=gear.handKnown[1]=true;gear.handKey[0]=77;gear.heldIndex[0]=0;
  auto&r=gear.items[0];r.held=true;r.set=0;r.slot126=0;r.profile.side=0;r.profile.classId=4;r.profile.key=77;r.profile.generation=8;r.profile.entity=70;
  auto&l=gear.items[1];l.set=0;l.slot126=1;l.profile.side=1;l.profile.classId=8;l.profile.key=88;l.profile.generation=18;l.profile.entity=80;
  std::memcpy(manager.data()+0x48,&r.profile.key,8);std::memcpy(manager.data()+0x50,&l.profile.key,8);
  in.allowed=in.requirePrimaryGrip114=true;in.epoch=10;in.bodyEpoch=9;in.tick=1000;in.sample=20;in.grip[0]=1;
  in.tracked[0]=in.tracked[1]=in.intentTracked[0]=in.intentTracked[1]=true;
  in.shoulders[0]={.2,0,1.4};in.shoulders[1]={-.2,0,1.4};ph::Vec3 m{};ph::holster_mount126(in,l.profile,m);in.hands[1].p=m;
  kcd2vr::quick_talk_owner_h4::mailbox.publish({1000,99,100,0xdead,0}); // independent fixture owner
  kcd2vr::quick_talk_owner_h4::mailbox.publish({1000,9,10,0x2000,0});
  g_native_held_release_inhibit_t4=true;
 }
 // An explicitly queued Draw is issued while the original live Grip is held.
 ph::Command drawCommand() {in.grip[1]=1;return {ph::CommandKind::Draw,1,0,88,10,in.sample,in.tick,0,{},9,18};}
 PhysFrame6698 frame() const {PhysFrame6698 f{};f.input=in;f.player=gear.player;f.manager=gear.manager;f.request=gear.request;return f;}
 void ackLeft(){gear.items[1].held=true;gear.handKey[1]=88;gear.heldIndex[1]=1;}
 bool stow(){ackLeft();in.grip[1]=1;tick();in.grip[1]=0;tick();return out.commandCount==1&&out.commands[0].kind==ph::CommandKind::Holster&&out.commands[0].side==1;}
 void currentSource(){
  g_pose_policy_generation57=2;g_weapon_control_mode=4;g_cal_state=2;g_ctx_reading=false;
  g_head_valid=g_right_valid=g_left_valid=true;g_head_valid_tick=g_right_valid_tick=g_left_valid_tick=in.tick;
  g_right_grip=1;g_left_grip=0;g_tool_context19c4r2=in.tick<<8;
  kcd2vr::animation_filter::nativeBodyOwned112=false;
  NativeHeldSourceT4 s{};s.valid=true;s.tick=in.tick;s.leftNativeMembershipH36=g_left_native_membership_h36.load();s.input.owner=f::achieved.owner;
  s.input.colliderLease=5;s.input.trackingEpoch=kcd2vr::tracking59c6::epoch.load();s.input.cancelGeneration=2;
  g_native_held_lease_t4=5;g_native_held_source_t4.publish(s);g_native_held_valid_t4=true;
 }
 void sourceTransaction(CoherentTransaction6689& tx){tx.epoch=9;tx.tracking.source_epoch=kcd2vr::tracking59c6::epoch.load();
  tx.tracking.head_valid=tx.tracking.right_valid=tx.tracking.left_valid=tx.tracking.allowed=true;
  tx.physicalInput=in;tx.physicalInput.haveWorld=true;tx.physicalInput.worldScale=1;
  tx.physicalProfiles[0]=gear.items[0].profile;tx.physicalProfiles[0].primary=tx.physicalProfiles[0].geometry=true;
  tx.physicalCorrelation126[0].stamp.actor=0x3000;tx.physicalEquipment=gear;
 }
 void tick(){in.tick+=20;++in.sample;hostTickCount64=in.tick;gear.tick=in.tick;
  kcd2vr::quick_talk_owner_h4::mailbox.publish({in.tick,9,10,0x2000,0});out={};PhysHolsterGestures6698(in,gear,out,states);}
 bool draw(){tick();in.grip[1]=1;tick();in.hands[1].p.y+=.20;for(int i=0;i<4;++i)tick();return out.commandCount==1&&out.commands[0].kind==ph::CommandKind::Draw&&out.commands[0].side==1&&out.commands[0].key==88;}
};
int main(){
 {RigH36 r;g_native_held_release_inhibit_t4=false;ck(r.draw(),"CONTROL native eligible left shield draws when no F9 inhibit");}
 {RigH36 r;ck(r.draw(),"retired same original right collider cannot let lingering RIGHT inhibit park LEFT shield draw");}
 {RigH36 r;f::pending=true;ck(!r.draw(),"active or retiring shared collider still parks both hands");}
 {RigH36 r;f::achieved.colliderRetired=false;ck(!r.draw(),"no current retired receipt cannot release left fence");}
 {RigH36 r;f::read=false;ck(!r.draw(),"contended backend receipt cannot imply retirement");}
 {RigH36 r;++f::achieved.owner.generation;ck(!r.draw(),"another original right item generation cannot authorize recovery");}
 {RigH36 r;r.gear.handKnown[1]=false;ck(!r.draw(),"unknown native left hand cannot draw");}
 {RigH36 r;r.gear.handKey[1]=99;ck(!r.draw(),"occupied native left hand cannot draw");}
 {RigH36 r;r.in.allowed=false;ck(!r.draw(),"menu or gameplay disallow cannot draw");}
 {RigH36 r;r.in.tracked[1]=false;ck(!r.draw(),"lost left tracking cannot draw");}
 {RigH36 r;r.gear.items[1].set=1;ck(!r.draw(),"another loadout cannot switch beside held right weapon");}
 // Original immutable retirement proof rejects changed actor/item/context receipts.
 {RigH36 r;++f::achieved.owner.player;ck(!r.draw(),"retired different player cannot release opposite hand");}
 {RigH36 r;++f::achieved.owner.manager;ck(!r.draw(),"retired different manager cannot release opposite hand");}
 {RigH36 r;++f::achieved.owner.character;ck(!r.draw(),"retired different skeleton cannot release opposite hand");}
 {RigH36 r;++f::achieved.owner.epoch;ck(!r.draw(),"retired different body epoch cannot release opposite hand");}
 {RigH36 r;++f::achieved.owner.request;ck(!r.draw(),"retired different request cannot release opposite hand");}
 {RigH36 r;++f::achieved.owner.key;ck(!r.draw(),"retired different right WUID cannot release opposite hand");}
 {RigH36 r;++f::achieved.owner.entityId;ck(!r.draw(),"retired different right entity cannot release opposite hand");}
 {RigH36 r;f::achieved.owner.side=1;ck(!r.draw(),"left receipt cannot be substituted for original right body");}
 {RigH36 r;ck(r.stow(),"retired original right owner allows deliberate existing left native stow gesture");}
 {RigH36 r;r.tick();r.in.grip[0]=0;r.tick();ck(!r.out.commandCount,"right still cannot stow through its retained release rearm");ck(r.out.hipGrantT10.owner==kcd2vr::hip_t10::Owner::None,"right hip stock grant remains excluded");}
 {RigH36 r;r.in.tick-=101;ck(!PhysLeftHolsterRetiredH36(r.in,r.gear),"stale copied input cannot prove retired join");}
 {RigH36 r;r.gear.tick-=101;ck(!PhysLeftHolsterRetiredH36(r.in,r.gear),"stale native equipment cannot prove retired join");}
 {RigH36 r;r.tick();r.in.grip[1]=1;r.tick();r.in.hands[1].p.y+=.2;
  for(int i=0;i<4;++i){r.in.tick+=20;++r.in.sample;hostTickCount64=r.in.tick;r.gear.tick=r.in.tick;
   kcd2vr::quick_talk_owner_h4::mailbox.publish({r.in.tick,9,10,0x2000,2});r.out={};PhysHolsterGestures6698(r.in,r.gear,r.out,r.states);}
  ck(!r.out.commandCount&&!r.states[1].comparison114.latched,"quick-talk cancels left draw ownership even after right retirement");
  r.tick();ck(!r.out.commandCount,"closing quick-talk cannot replay consumed left draw");}
 // Actual consumer-prefix replay: original deadlines/queue precede H36.
 {RigH36 r;auto c=r.drawCommand();g_phys_commands6698.push(c);auto frame=r.frame();
  ck(ActualEquipmentPrefixH36(frame,r.gear,r.in.tick)==1,"current eligible native left draw reaches unchanged native include boundary");
  ck(g_left_equipment_obligation_h36.active&&!g_left_equipment_obligation_h36.entered&&g_native_held_equipment_pending_h36&&f::equipmentSuspendedH36,"original authorized native transition revokes future T4 source before dispatch");
  (void)(PhysLeftEquipmentBeginH36(r.gear,c)&&PhysLeftEquipmentEnterH36(r.gear,c));ck(g_left_equipment_obligation_h36.entered,"exact source native-entry marker records unknown only immediately before original call");
  ck(!g_native_held_valid_t4&&f::cancels>0,"source invalidated and protected backend cancel requested before native dispatch");
  r.currentSource();NativeHeldSourceT4 refreshed{};g_native_held_source_t4.read(refreshed);ck(!NativeHeldSourceCurrentT4(refreshed.input),"left Grip release plus refreshed old right source cannot re-admit before ACK");
  CoherentTransaction6689 pendingTx{};r.sourceTransaction(pendingTx);const auto submits=f::submits;StageNativeHeldT4(pendingTx);ck(f::submits==submits&&!pendingTx.nativeHeldActiveT4,"actual coherent T4 stage cannot submit during original native Draw obligation");
  NativeHeldCaptureT4(r.gear,r.in.tick);ck(g_native_held_reason_t4==HeldPhysicsSourceT4::Context&&!g_native_held_valid_t4,"actual native source capture stops at obligation before retail attachment calls");
  auto original=g_left_equipment_obligation_h36.command;r.in.tick+=30;hostTickCount64=r.in.tick;r.gear.tick=r.in.tick;
  PhysLeftEquipmentRefreshH36(r.gear,r.in.tick);ck(g_native_held_equipment_pending_h36,"left still empty is no draw acknowledgement");
  r.gear.handKey[1]=99;PhysLeftEquipmentRefreshH36(r.gear,r.in.tick);ck(g_native_held_equipment_pending_h36,"wrong occupied WUID cannot acknowledge original draw");
  r.ackLeft();r.gear.items[1].profile.generation+=1;PhysLeftEquipmentRefreshH36(r.gear,r.in.tick);
  ck(g_native_held_equipment_pending_h36&&g_left_equipment_obligation_h36.active&&f::equipmentSuspendedH36,"occupied-left WUID is not attachment completion: old key-only Draw ACK is superseded by actual original callback/binding receipt");
  ck(!g_native_held_valid_t4,"ACK cannot resurrect the original pre-mutation right source");
  ck(original.tick==1000&&original.sample==20,"consumer retains original command clock and sample");}
 {RigH36 r;f::pending=true;auto c=r.drawCommand();g_phys_commands6698.push(c);auto frame=r.frame();
  ck(ActualEquipmentPrefixH36(frame,r.gear,r.in.tick)==0,"late right admission before consumer waits actual retirement");
  ph::Command kept{};ck(g_phys_commands6698.peek(kept)&&kept.tick==c.tick&&kept.sample==c.sample&&!g_left_equipment_obligation_h36.entered,"WAIT neither consumes nor retags original equipment command");
  hostTickCount64=1100;r.gear.tick=1100;PhysLeftEquipmentRefreshH36(r.gear,1100);ck(g_native_held_equipment_pending_h36,"100ms boundary is not expiry or native ACK");
  hostTickCount64=1101;r.gear.tick=1101;PhysLeftEquipmentRefreshH36(r.gear,1101);ck(g_native_held_equipment_pending_h36,"expired NOT_INVOKED still cannot release a live collider");
  f::pending=false;PhysLeftEquipmentRefreshH36(r.gear,1101);ck(!g_native_held_equipment_pending_h36&&!g_left_equipment_obligation_h36.active&&!f::equipmentSuspendedH36,"known never-dispatched original command may retire after deadline and authoritative collider absence");
  ck(ActualEquipmentPrefixH36(frame,r.gear,1101)==0&&!g_phys_commands6698.peek(kept),"actual unchanged100ms native gate discards expired request without dispatch");}
 {RigH36 r;auto c=r.drawCommand();c.tick+=1;g_phys_commands6698.push(c);auto frame=r.frame();
  ck(!ActualEquipmentPrefixH36(frame,r.gear,r.in.tick)&&!g_native_held_equipment_pending_h36,"future command rejected before any new equipment obligation");}
 {RigH36 r;auto c=r.drawCommand();++c.epoch;g_phys_commands6698.push(c);auto frame=r.frame();
  ck(!ActualEquipmentPrefixH36(frame,r.gear,r.in.tick)&&!g_native_held_equipment_pending_h36,"changed request rejected before any new equipment obligation");}
 {RigH36 r;auto c=r.drawCommand();g_phys_commands6698.push(c);auto frame=r.frame();
  kcd2vr::quick_talk_owner_h4::mailbox.publish({r.in.tick,9,10,0x2000,2});
  ck(ActualEquipmentPrefixH36(frame,r.gear,r.in.tick)==1&&g_native_held_equipment_pending_h36,"later quick-talk cannot discard actual queued equipment before native include");
  r.in.tick+=20;hostTickCount64=r.in.tick;kcd2vr::quick_talk_owner_h4::mailbox.publish({r.in.tick,9,10,0x2000,0});
  ck(ActualEquipmentPrefixH36(frame,r.gear,r.in.tick)==1,"prefix-only fixture leaves original equipment queued until actual consumer pop");}
 {RigH36 r;r.currentSource();NativeHeldSourceT4 s{};g_native_held_source_t4.read(s);ck(NativeHeldSourceCurrentT4(s.input),"CONTROL same current right source is normally eligible");
  CoherentTransaction6689 controlTx{};r.sourceTransaction(controlTx);StageNativeHeldT4(controlTx);ck(f::submits==1,"CONTROL actual coherent T4 stage normally submits this same current source");
  r.ackLeft();auto c=r.drawCommand();c.kind=ph::CommandKind::Holster;auto frame=r.frame();
  ck(PhysLeftEquipmentH36(frame,r.gear,c)==HolsterNativeH36::Ready,"eligible left stow still reaches original native operation after right retirement");
  (void)(PhysLeftEquipmentBeginH36(r.gear,c)&&PhysLeftEquipmentEnterH36(r.gear,c));r.gear.handKey[1]=0;r.gear.heldIndex[1]=-1;r.gear.items[1].held=false;
  hostTickCount64=3000;r.gear.tick=3000;PhysLeftEquipmentRefreshH36(r.gear,3000);
  ck(g_native_held_equipment_pending_h36,"STOW empty hand key and elapsed time are NOT native attachment completion");
  f::enabled=false;PhysLeftEquipmentRefreshH36(r.gear,3000);ck(g_native_held_equipment_pending_h36,"OFF does not erase unknown entered native obligation");
  ++r.gear.request;PhysLeftEquipmentRefreshH36(r.gear,3000);ck(g_native_held_equipment_pending_h36,"new global request does not imply original action cleanup");
  ck(!NativeHeldSourceCurrentT4(s.input),"unknown stow blocks only T4 source re-admission");}
 {RigH36 r;r.gear.handKey[1]=99;auto c=r.drawCommand();auto frame=r.frame();
  ck(PhysLeftEquipmentH36(frame,r.gear,c)==HolsterNativeH36::Discard&&!g_native_held_equipment_pending_h36&&f::cancels==0,"current native left occupancy refuses new Draw before any physical revoke");}
 {RigH36 r;auto c=r.drawCommand();auto frame=r.frame();PhysLeftEquipmentH36(frame,r.gear,c);(void)(PhysLeftEquipmentBeginH36(r.gear,c)&&PhysLeftEquipmentEnterH36(r.gear,c));
  r.ackLeft();++r.gear.request;PhysLeftEquipmentRefreshH36(r.gear,1000);ck(g_native_held_equipment_pending_h36,"new load/request plus matching WUID cannot acknowledge old original draw");}
 {RigH36 r;auto c=r.drawCommand();auto frame=r.frame();PhysLeftEquipmentH36(frame,r.gear,c);
  auto newer=c;++newer.sample;++newer.tick;hostTickCount64=newer.tick;r.in.tick=newer.tick;r.gear.tick=newer.tick;
  PhysLeftEquipmentH36(r.frame(),r.gear,newer);(void)(PhysLeftEquipmentBeginH36(r.gear,newer)&&PhysLeftEquipmentEnterH36(r.gear,newer));r.ackLeft();PhysLeftEquipmentRefreshH36(r.gear,newer.tick);
  ck(g_left_equipment_obligation_h36.active&&g_left_equipment_obligation_h36.command.sample==c.sample&&g_left_equipment_successors_h36[0].entered&&g_native_held_equipment_pending_h36,"new explicit request retains the never-invoked original record and its own separate entered obligation");}
 std::printf("holster_side_retirement_h36=%u/%u actualProduction=HOLSTER_SELECTOR_NATIVE_PREFIX_AND_SOURCE_PREDICATE nativeCalls=NONE\n",tests-failed,tests);return failed?1:0;
}
