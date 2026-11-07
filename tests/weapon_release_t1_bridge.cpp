// Complete production frik TU: actual release selector and allowed-object
// history producer. Native boundary is exercised separately in owner13 cases.
#include "../src/frik_pose_probe.cpp"
#include "native_held_backend_t4_stub.h"
#include <cassert>
#include <cstdio>
namespace kcd2vr::diagnostic_trace {void Write(const char*) noexcept {}}
using namespace kcd2vr::frik_pose_probe;
namespace kcd2vr::action_time_owner59c11 {extern supply_t2::Input lastSupplyInputT9;}
int main(){
 kcd2vr::collab_h21::setThrowing(true);
 namespace w=kcd2vr::weapon_release_t1;
 PhysEquipment6698 equipment{};equipment.count=1;equipment.valid=true;equipment.handKnown[0]=equipment.handKnown[1]=true;
 auto& item=equipment.items[0];item.profile.key=55;item.profile.generation=7;item.profile.side=0;
 item.profile.classId=3;item.profile.weight=1.5;item.set=0;item.held=true;item.dropUnitT1=true;
 equipment.handKey[0]=55;equipment.heldIndex[0]=0;
 ph::Input in{};in.allowed=true;in.requirePrimaryGrip114=true;in.epoch=10;in.bodyEpoch=9;
 in.tracked[0]=in.intentTracked[0]=true;in.tracked[1]=in.intentTracked[1]=true;
 in.shoulders[0]={.2,0,1.4};in.shoulders[1]={-.2,0,1.4};in.hands[0].p={.2,.5,1.3};
 auto release=[&](int mode,bool eligible,bool tracked,bool allowed,bool inZone,bool supportOnly){
  w::SetMode(mode);item.dropUnitT1=eligible;std::array<ph::HolsterState,8> state{};ph::Output first{},second{};
  in.allowed=true;in.tracked[0]=in.intentTracked[0]=true;in.tick=1000;in.sample=100;in.grip[0]=1;in.grip[1]=1;
  pm::Vec3 mount{};assert(ph::holster_mount126(in,item.profile,mount));
  in.hands[0].p=inZone?mount:mount+pm::Vec3{0,.6,0};
  PhysHolsterGestures6698(in,equipment,first,state);assert(first.commandCount==0);
  in.tick=1011;in.sample=101;in.allowed=allowed;in.tracked[0]=in.intentTracked[0]=tracked;
  in.grip[supportOnly?1:0]=0;PhysHolsterGestures6698(in,equipment,second,state);return second;
 };
 kcd2vr::collab_h21::setThrowing(false);
 auto out=release(1,true,true,true,false,false);assert(out.commandCount==1&&out.commands[0].kind==ph::CommandKind::Holster);
 kcd2vr::collab_h21::setThrowing(true);
 out=release(1,true,true,true,false,false);assert(out.commandCount==1&&out.commands[0].kind==ph::CommandKind::DropT1);
 assert(out.commands[0].throwEpochH21==kcd2vr::collab_h21::throwEpoch.load());
 const auto queuedThrowEpoch=out.commands[0].throwEpochH21;
 kcd2vr::collab_h21::setThrowing(false);kcd2vr::collab_h21::setThrowing(true);
 assert(queuedThrowEpoch!=kcd2vr::collab_h21::throwEpoch.load()); // consumer must refuse, never retag this command
 out=release(0,true,true,true,false,false);assert(out.commandCount==1&&out.commands[0].kind==ph::CommandKind::Holster);
 out=release(1,true,true,true,true,false);assert(out.commandCount==1&&out.commands[0].kind==ph::CommandKind::Holster);
 out=release(1,false,true,true,false,false);assert(out.commandCount==1&&out.commands[0].kind==ph::CommandKind::Holster);
 out=release(1,true,true,true,false,true);assert(out.commandCount==0); // support release is not primary drop
 out=release(1,true,false,true,false,false);assert(out.commandCount==0);
 out=release(1,true,true,false,false,false);assert(out.commandCount==0);
 assert(!w::SupportedClass(9)&&!w::SupportedClass(10)&&!w::SupportedClass(13)&&w::SupportedClass(3)&&w::SupportedClass(8));
 // T9: enabling finite throwing stock must not disable an ordinary equipped
 // axe/mace/hammer at the same hip. Native equipment has one gesture owner.
 equipment.handKey[0]=equipment.handKey[1]=0;equipment.selected=0;item.held=false;item.slot126=0;
 in.allowed=true;in.tracked[0]=in.intentTracked[0]=true;in.grip[1]=0;
 kcd2vr::action_time_owner59c11::SetThrowingSupplyT2(true);
 kcd2vr::inventory_lifecycle_t8::daggers=5;kcd2vr::inventory_lifecycle_t8::axes=3;
 for(int cls:{3,5,6}){
  item.profile.classId=cls;
  std::array<ph::HolsterState,8> states{};ph::Output output{};pm::Vec3 mount{};
  assert(ph::holster_mount126(in,item.profile,mount));in.hands[0].p=mount;in.grip[0]=0;
  auto gesture=[&](){output={};in.tick+=20;++in.sample;PhysHolsterGestures6698(in,equipment,output,states);};
  gesture();in.grip[0]=1;gesture();in.hands[0].p.y+=.20;
  unsigned draws=0;for(int n=0;n<6;++n){gesture();for(unsigned j=0;j<output.commandCount;++j)draws+=output.commands[j].kind==ph::CommandKind::Draw;}
  assert(draws==1);gesture();assert(output.commandCount==0);
  PhysFrame6698 hipFrame{};hipFrame.input=in;hipFrame.request=in.epoch;hipFrame.output=output;const auto axes=ph::body_axes(in);
  PhysPublishSupplyT2(hipFrame,equipment,false,in.tick);
  const auto reserved=kcd2vr::action_time_owner59c11::lastSupplyInputT9;
  assert(!reserved.pouchNear[0]&&!reserved.pouchNear[1]);
  auto noHip=equipment;noHip.count=0;
  std::array<ph::HolsterState,8> emptyStates{};hipFrame.input.hands[0].p=mount+axes.forward*.13;hipFrame.input.grip[0]=0;
  ++hipFrame.input.sample;hipFrame.input.tick+=20;hipFrame.output={};
  PhysHolsterGestures6698(hipFrame.input,noHip,hipFrame.output,emptyStates);
  PhysPublishSupplyT2(hipFrame,noHip,false,in.tick);
  assert(kcd2vr::action_time_owner59c11::lastSupplyInputT9.pouchNear[0]); // stock remains selectable when hip is free
 }
 // T15: ordinary hip, front dagger and separate upper-rear axe remain distinct.
 // Native qualification IO is substituted only after actual publication;
 // both the ordinary draw and existing supply Gesture consume that selection.
 for(int cls:{3,5,6})for(int region=0;region<3;++region){
  item.profile.classId=cls;item.held=false;equipment.handKey[0]=0;
  std::array<ph::HolsterState,8> states{};ph::Output output{};pm::Vec3 mount{};
  assert(ph::holster_mount126(in,item.profile,mount));const auto axes=ph::body_axes(in);
  const auto origin=region==0?mount:region==1?mount+axes.forward*.13:
   in.shoulders[0]-axes.up*.25-axes.forward*.25+axes.right*.06;
  in.hands[0].p=origin;in.grip[0]=0;
  kcd2vr::supply_t2::Gesture supplyGesture{};unsigned draws=0,stock=0;int family=-1;
  auto step=[&](){output={};in.tick+=20;++in.sample;PhysHolsterGestures6698(in,equipment,output,states);
   for(unsigned j=0;j<output.commandCount;++j)draws+=output.commands[j].kind==ph::CommandKind::Draw;
   PhysFrame6698 f{};f.input=in;f.output=output;f.request=in.epoch;PhysPublishSupplyT2(f,equipment,false,in.tick);
   auto witness=kcd2vr::action_time_owner59c11::lastSupplyInputT9;
   assert(!witness.bone&&!witness.beltShow); // no renderer or pelvis dependency
   witness.allowed=witness.tracked=true;witness.manager=1;witness.player=2;witness.character=3;witness.entity=4;
   const int picked=supplyGesture.next(witness,in.tick,true,false);if(picked>=0){++stock;family=picked;}
  };
  step();in.grip[0]=1;step();assert(stock==0);in.hands[0].p=origin+axes.forward*.20;
  for(int n=0;n<8;++n){step();if(region&&n<3)assert(stock==0);} // no supply before80ms pull
  assert(region==0?(draws==1&&stock==0):(draws==0&&stock==1&&family==region-1));
  // An old or foreign completed-pose grant cannot light a native stock pouch.
  PhysFrame6698 stale{};stale.input=in;stale.request=in.epoch;stale.output=output;--stale.output.hipGrantT10.sample;
  PhysPublishSupplyT2(stale,equipment,false,in.tick);
  assert(!kcd2vr::action_time_owner59c11::lastSupplyInputT9.pouchNear[0]&&!kcd2vr::action_time_owner59c11::lastSupplyInputT9.pouchNear[1]);
 }
 // Rejected pose publication cannot commit a grab latch. Exercise the actual
 // publisher's early refusal after operating only on its copied holster bank.
 {const auto saved=g_phys_holsters6698;g_phys_holsters6698[0].hipStateT10.owner=kcd2vr::hip_t10::Owner::Axe;
  g_phys_holsters6698[0].hipStateT10.sample=999;auto local=g_phys_holsters6698;
  item.profile.classId=3;pm::Vec3 mount{};assert(ph::holster_mount126(in,item.profile,mount));
  in.hands[0].p=mount;in.grip[0]=0;in.tick+=20;++in.sample;ph::Output output{};
  PhysHolsterGestures6698(in,equipment,output,local);assert(local[0].hipStateT10.owner==kcd2vr::hip_t10::Owner::Equipment);
  const int oldMode=g_weapon_control_mode.exchange(0);float world[8]{0,0,0,1,0,0,0,1};ph::State state{};std::array<ph::Profile,2> profiles{};
  PhysPublish6698(in,output,profiles,equipment,world,state,local);g_weapon_control_mode=oldMode;
  assert(g_phys_holsters6698[0].hipStateT10.owner==kcd2vr::hip_t10::Owner::Axe&&g_phys_holsters6698[0].hipStateT10.sample==999);
  g_phys_holsters6698=saved;
 }
 // Sword/hunting-sword scabbards retain their captured native mount. Test
 // draw, native-hand ACK, fixed return point, stow and a held offhand shield.
 for(int cls:{1,2,4,16}){
  item.profile.classId=cls;item.held=false;equipment.handKey[0]=0;equipment.handKey[1]=777;in.grip[1]=1;
  PhysGripBank6698 bank{};auto& mount=bank.grip[0];mount.key=item.profile.key;mount.generation=item.profile.generation;
  mount.mountReady=true;mount.mount.p={.28,.02,.90};mount.mountStamp126.epoch=in.bodyEpoch;mount.mountStamp126.tick=GetTickCount64();
  assert(g_phys_grips6698.publish(bank));const auto point=mount.mount.p;
  std::array<ph::HolsterState,8> states{};ph::Output output{};in.hands[0].p=point;in.grip[0]=0;
  auto step=[&](){output={};in.tick+=20;++in.sample;PhysHolsterGestures6698(in,equipment,output,states);};
  step();in.grip[0]=1;step();in.hands[0].p.y+=.20;unsigned draws=0;
  for(int n=0;n<6;++n){step();for(unsigned j=0;j<output.commandCount;++j)draws+=output.commands[j].kind==ph::CommandKind::Draw;}
  assert(draws==1);item.held=true;equipment.handKey[0]=item.profile.key;step();
  mount.mount.p={1,1,1};assert(g_phys_grips6698.publish(bank)); // held blade cannot move the cached scabbard
  in.hands[0].p=point;for(int n=0;n<4;++n)step();in.grip[0]=0;step();
  assert(output.commandCount==1&&output.commands[0].kind==ph::CommandKind::Holster&&output.commands[0].side==0);
  assert(equipment.handKey[1]==777);step();assert(output.commandCount==0);
  item.held=false;equipment.handKey[0]=0;mount.mountReady=false;assert(g_phys_grips6698.publish(bank));
  step();in.grip[0]=1;for(int n=0;n<6;++n){step();assert(output.commandCount==0);} // no invented stale native scabbard
 }
 g_phys_grips6698.publish(PhysGripBank6698{});equipment.handKey[1]=0;in.grip[1]=0;
 kcd2vr::action_time_owner59c11::SetThrowingSupplyT2(false);item.held=true;item.profile.classId=3;equipment.handKey[0]=55;
 // T15: actual publication carries only occupied-left exclusion metadata. The
 // equipment/loadout never becomes a visible belt or attachment request.
 kcd2vr::action_time_owner59c11::SetThrowingSupplyT2(true);
 equipment.count=1;equipment.valid=true;equipment.selected=0;equipment.handKnown[0]=equipment.handKnown[1]=true;
 equipment.handKey[0]=equipment.handKey[1]=0;item.held=false;item.set=0;item.slot126=0;item.profile.twoHanded=false;
 item.profile.key=0x0200000000019a5bull;item.profile.classId=3;
 PhysFrame6698 belt{};belt.input=in;belt.input.grip[0]=0;belt.request=in.epoch;equipment.epoch=in.bodyEpoch;equipment.request=in.epoch;
 auto beltInput=[&](bool allow=true){PhysPublishSupplyT2(belt,equipment,allow,in.tick);return kcd2vr::action_time_owner59c11::lastSupplyInputT9;};
 auto display=beltInput();assert(!display.beltKey&&!display.beltClass&&!display.beltShow&&!display.bone);
 // Holding an unrelated bow or shield never changes either native key/slot.
 equipment.handKey[1]=777;display=beltInput();assert(display.beltKey==777&&!display.beltShow&&equipment.handKey[1]==777&&item.slot126==0);
 equipment.handKey[0]=888;display=beltInput();assert(display.beltKey==777&&!display.beltShow&&equipment.handKey[0]==888);
 equipment.handKey[0]=item.profile.key;assert(!beltInput().beltShow);equipment.handKey[0]=0;
 item.held=true;assert(!beltInput().beltShow);item.held=false;
 belt.output.hipGrantT10={};belt.output.hipGrantT10.owner=kcd2vr::hip_t10::Owner::Equipment;
 belt.output.hipGrantT10.key=item.profile.key;belt.output.hipGrantT10.body=in.bodyEpoch;
 belt.output.hipGrantT10.request=belt.request;belt.output.hipGrantT10.sample=in.sample;belt.input.grip[0]=1;
 assert(!beltInput().beltShow);belt.output={};belt.input.grip[0]=0;
 assert(!beltInput(false).beltShow);
 ++equipment.epoch;assert(!beltInput().beltShow);--equipment.epoch;
 ++equipment.request;assert(!beltInput().beltShow);--equipment.request;
 item.profile.classId=9;assert(!beltInput().beltShow);item.profile.classId=3;
 item.profile.twoHanded=true;assert(!beltInput().beltShow);item.profile.twoHanded=false;
 equipment.items[1]=item;equipment.items[1].profile.key=0x0200000000019a5cull;equipment.items[1].set=1;equipment.count=2;
 assert(beltInput().beltKey==777&&!beltInput().beltShow);equipment.selected=2;assert(!beltInput().beltShow);
 assert(equipment.handKey[1]==777&&item.slot126==0&&equipment.items[1].slot126==0);
 equipment.count=1;equipment.selected=0;equipment.handKey[0]=55;equipment.handKey[1]=0;item.profile.key=55;item.held=true;
 kcd2vr::action_time_owner59c11::SetThrowingSupplyT2(false);
 // Actual world-object sample publisher, including its identity and source-time gates.
 w::SetMode(1);equipment.manager=reinterpret_cast<void*>(0x1000);equipment.epoch=9;equipment.request=10;
 item.item=reinterpret_cast<void*>(0x2000);item.entity=reinterpret_cast<void*>(0x3000);item.profile.entity=72;
 PhysFrame6698 f{};f.input.tracked[0]=f.input.intentTracked[0]=true;f.input.grip[0]=1;
 f.input.sample=100;f.input.tick=1000;f.output.owned[0]=true;f.correlation126[0].stamp.actor=77;
 auto* attachment=reinterpret_cast<void*>(0x4000);float placed[8]{0,0,0,1,30,40,50,1};
 PhysRecordReleaseT1(equipment,item,f,attachment,placed);
 f.input.sample=101;f.input.tick=1010;placed[4]+=.02f;PhysRecordReleaseT1(equipment,item,f,attachment,placed);
 w::Sample sample{};assert(g_weapon_release_sample_t1[0].read(sample)&&sample.motion.pair);
 float v[3]{},omega[3]{};sample.motion.velocity(1011,true,v,omega);assert(v[0]>1.9f&&v[0]<2.1f);
 f.input.sample=102;f.input.tick=1020;f.input.grip[0]=0;placed[4]=100;PhysRecordReleaseT1(equipment,item,f,attachment,placed);
 assert(g_weapon_release_sample_t1[0].read(sample)&&sample.motion.newer.sample==101); // release does not learn native sheath motion
 f.input.grip[0]=1;f.output.displayLatched[0]=true;PhysRecordReleaseT1(equipment,item,f,attachment,placed);
 assert(g_weapon_release_sample_t1[0].read(sample)&&sample.motion.newer.sample==101); // no impale/display-only velocity
 w::Request r{};r.key=55;r.manager=0x1000;r.character=77;r.entity=72;r.bodyEpoch=9;r.side=0;r.generation=7;
 r.requestEpoch=10;r.sample=102;r.tick=1020;r.attachment=0x4000;
 r.throwEpochH21=kcd2vr::collab_h21::throwEpoch.load();r.physicsEpochH21=kcd2vr::collab_h21::epoch.load();
 assert(w::matches(sample,r,1020));r.bodyEpoch=99;assert(!w::matches(sample,r,1020));r.bodyEpoch=9;
 assert(!w::matches(sample,r,1100));r.requestEpoch=99;assert(!w::matches(sample,r,1020));
 r.requestEpoch=10;kcd2vr::collab_h21::setThrowing(false);kcd2vr::collab_h21::setThrowing(true);r.throwEpochH21=kcd2vr::collab_h21::throwEpoch.load();
 assert(!w::matches(sample,r,1020)); // old held motion cannot be retagged to a new F4 test
 kcd2vr::collab_h21::setPhysicsMode(2);kcd2vr::collab_h21::setPhysicsMode(0);r.physicsEpochH21=kcd2vr::collab_h21::epoch.load();
 assert(!w::matches(sample,r,1020)); // same for old F9 profile motion
 // F9's handling-only frame may record release motion ONLY when it is the
 // exact achieved native collider pose, never the unsolved controller target.
 namespace nf=kcd2vr::native_weapon_body_t4::fixture;
 f.output.displayLatched[0]=false;f.output.handlingOnly55c1[0]=f.output.solverOwnedT4[0]=true;
 f.input.sample=103;f.input.tick=1030;f.input.grip[0]=1;g_weapon_release_history_t1[0]={};
 nf::achieved={};nf::achieved.poseValid=true;nf::achieved.colliderRetired=false;nf::achieved.sourceTick=GetTickCount64();
 nf::achieved.owner={55,7,10,9,reinterpret_cast<std::uintptr_t>(equipment.player),0x1000,77,0x3000,0x4000,6,72,0};
 nf::achieved.weaponWorld={{0,0,0,1},{placed[4],placed[5],placed[6]}};
 PhysRecordReleaseT1(equipment,item,f,attachment,placed);assert(g_weapon_release_sample_t1[0].read(sample)&&sample.motion.newer.sample==103);
 ++f.input.sample;f.input.tick+=10;placed[4]+=.02f;nf::achieved.weaponWorld.p.x=placed[4];
 PhysRecordReleaseT1(equipment,item,f,attachment,placed);assert(g_weapon_release_sample_t1[0].read(sample)&&sample.motion.newer.sample==104&&sample.motion.pair);
 ++f.input.sample;f.input.tick+=10;nf::achieved.weaponWorld.p.x+=.10;
 PhysRecordReleaseT1(equipment,item,f,attachment,placed);assert(g_weapon_release_sample_t1[0].read(sample)&&sample.motion.newer.sample==104);
 nf::achieved.weaponWorld.p.x=placed[4];++nf::achieved.owner.key;
 PhysRecordReleaseT1(equipment,item,f,attachment,placed);assert(g_weapon_release_sample_t1[0].read(sample)&&sample.motion.newer.sample==104);
 // A live F9 solver stages the exact original release; no native Drop is
 // invoked from the pose, and changing the retained owner refuses admission.
 w::SetMode(1);nf::pending=true;nf::achieved.owner={55,7,10,9,1,2,3,4,5,6,72,0};
 equipment.items[0].profile.entity=72;in.epoch=10;in.bodyEpoch=9;in.allowed=true;in.grip[0]=1;in.tick=2000;in.sample=200;
 std::array<ph::HolsterState,8> solverStates{};ph::Output solverFirst{},solverRelease{};
 pm::Vec3 solverMount{};assert(ph::holster_mount126(in,item.profile,solverMount));in.hands[0].p=solverMount+pm::Vec3{0,.7,0};
 PhysHolsterGestures6698(in,equipment,solverFirst,solverStates);in.grip[0]=0;in.tick+=10;++in.sample;
 PhysHolsterGestures6698(in,equipment,solverRelease,solverStates);assert(solverRelease.commandCount==1&&solverRelease.commands[0].kind==ph::CommandKind::DropT1);
 ++nf::achieved.owner.generation;solverStates={};solverRelease={};in.grip[0]=1;in.tick+=10;++in.sample;
 PhysHolsterGestures6698(in,equipment,solverRelease,solverStates);assert(solverRelease.commandCount==0);nf::pending=false;
 std::puts("PASS T1 complete production release/F4 gate/solver release staging, native stow and allowed WORLD motion");
}
