// Actual production finger adapters and completed physical-hand ownership query.
#include "../src/frik_pose_probe.cpp"
#include <cassert>
#include <cstdio>
namespace kcd2vr::diagnostic_trace {void Write(const char*) noexcept {}}
using namespace kcd2vr::frik_pose_probe;
int main(){
 namespace ft=kcd2vr::finger40;
 CoherentState6689 s{};s.rig.hand_r=0;s.rig.hand_l=1;
 for(int side=0;side<2;++side){
  s.fingerMap[side]=true;s.fingerCount[side]=15;s.open[side].ready=s.closed[side].ready=true;
  int n=0;
  for(int f=0;f<5;++f){auto& chain=s.fingers[side][f];chain.count=3;
   for(int j=0;j<3;++j,++n){const int id=2+side*15+n;chain.id[j]=id;s.fingerIds[side][n]=id;s.parent[id]=j?id-1:side;
    s.restLocal[id].q[3]=1;s.restLocal[id].t[0]=.02f;s.open[side].local[n]=s.restLocal[id];
    s.closed[side].local[n]=s.restLocal[id];QToQuatT(s.closed[side].local[n],QP(pc::from_rotation_vector({side?-.8:.8,0,0})));
    for(int k=0;k<4;++k)ft::set(side,f,k,k==1?1.f:0.f);
   }
  }
 }
 const auto oldBind=g_open_bind_enabled105.load();g_open_bind_enabled105=false;
 std::array<std::array<pc::Quat,15>,2> nativeOpen{};
 for(int side=0;side<2;++side){assert(ApplyFingerLocals6689(s,side,0,0));
  for(int n=0;n<15;++n)nativeOpen[side][n]=PQ(QFromQuatT(s.local[s.fingerIds[side][n]]));}
 // Independent freshly saved user closed settings, not raw-bone mirroring.
 ft::set(0,1,1,1.f);ft::set(1,1,1,.94f);
 ft::set(0,1,3,12);ft::set(1,1,3,-9);
 for(int side=0;side<2;++side){
  assert(ApplyFingerLocals6689(s,side,0,0));
  for(int n=0;n<15;++n)assert(pc::angle(PQ(QFromQuatT(s.local[s.fingerIds[side][n]])),nativeOpen[side][n])<1e-7);
  assert(ApplyFingerLocals6689(s,side,1,1));
  const int base=s.fingers[side][1].id[0],distal=s.fingers[side][1].id[1];
  const double expected=(side?.8*.94-9*pc::armPi/180:.8+12*pc::armPi/180)*(side?-1:1);
  assert(pc::angle(PQ(QFromQuatT(s.local[base])),pc::from_rotation_vector({expected,0,0}))<1e-6);
  assert(pc::angle(PQ(QFromQuatT(s.local[distal])),pc::from_rotation_vector({(side?-.8*.94:.8),0,0}))<1e-6);
  V3 out[6]{};FingerContactGeometry103 geometry{};assert(PalmGeometry103(s,side,{0,0,0,1},1,1,0,out,geometry));
  assert(pc::angle(geometry.curve[1].closed[0],PQ(QFromQuatT(s.local[base])))<1e-7);
  for(int n=0;n<15;++n){const int id=s.fingerIds[side][n];assert(s.local[id].t[0]==s.restLocal[id].t[0]);}
 }
 // Held native object fingers do NOT inherit free-fist tuning.
 for(int side=0;side<2;++side){const int hand=side;s.original[hand].q[3]=1;
  for(int n=0;n<15;++n){const int id=s.fingerIds[side][n];QuatT28 native=s.restLocal[id];QToQuatT(native,QP(pc::from_rotation_vector({0,.2,0})));
   s.original[id]=AbsoluteFromRelative(s.original[s.parent[id]],native);}
  assert(ApplyObjectFingerLocals6698(s,side,99+side,2,1,1000));
  assert(pc::angle(PQ(QFromQuatT(s.local[s.fingers[side][1].id[0]])),pc::from_rotation_vector({0,.2,0}))<1e-6);
 }
 g_open_bind_enabled105=oldBind;
 // Execute the exported production query with real PhysFrame/Equipment types.
 g_weapon_control_mode=7;g_phys_armed6698=true;g_phys_fault6698=false;g_gameplay_allowed=true;
 g_weapon_request_epoch6694=3;g_pose_epoch6689=7;g_committed_character6689=reinterpret_cast<void*>(0x1234);
 g_henry_item_attach_manager=reinterpret_cast<void*>(0x4567);
 PhysFrame6698 f{};PhysEquipment6698 e{};f.request=3;f.input.epoch=3;f.input.bodyEpoch=7;
 f.input.sample=10;f.input.tick=1000;f.input.allowed=true;f.input.tracked[0]=f.input.tracked[1]=true;
 f.player=e.player=reinterpret_cast<void*>(0x2345);f.manager=e.manager=g_henry_item_attach_manager.load();
 e.valid=true;e.request=3;e.epoch=7;e.tick=1000;e.count=1;e.heldIndex[0]=0;e.handKnown[0]=e.handKnown[1]=true;e.handKey[0]=77;
 auto& p=f.profiles[0];p.key=77;p.generation=2;p.entity=99;p.side=0;p.classId=7;p.primary=true;e.items[0].profile=p;e.items[0].held=true;
 auto& c=f.correlation126[0];c.socket.valid=true;c.socket.character=g_committed_character6689.load();c.socket.key=77;c.socket.generation=2;
 c.stamp={7,10,1000,0x1234};f.output.active[0]=f.output.owned[0]=true;
 g_phys_ready_sample6698=10;
 auto publish=[&](){g_phys_frame6698.publish(f);g_phys_equipment6698.publish(e);};publish();
 assert(ReadPhysicalHandHeld55(0,1000)==1&&ReadPhysicalHandHeld55(1,1000)==0);
 f.output.owned[1]=f.output.support[0]=true;publish();
 assert(e.handKey[1]==0&&ReadPhysicalHandHeld55(1,1080)==1); // actual support despite native empty slot
 assert(ReadPhysicalHandHeld55(1,1081)==-1); // tiny lifetime never extended
 ++e.items[0].profile.generation;publish();assert(ReadPhysicalHandHeld55(1,1000)==-1);--e.items[0].profile.generation;
 f.output.owned[1]=false;publish();assert(ReadPhysicalHandHeld55(1,1000)==-1);
 // Actual production publication helper: retained support out of anatomical
 // reach is a gesture veto even though it cannot own this frame's wrist.
 ph::WeaponState retained{};retained.key=77;retained.generation=2;retained.support=retained.supportGripReady169r2=true;
 f.profiles[0].twoHanded=true;e.items[0].profile=f.profiles[0];f.input.grip[1]=1;
 f.retainedSupport55[0]=kcd2vr::physical_hand_owner55::retained(f.input,f.output,f.profiles[0],retained,0);
 assert(f.retainedSupport55[0]);publish();assert(ReadPhysicalHandHeld55(1,1000)==1&&!f.output.owned[1]);
 f.input.grip[1]=0;f.retainedSupport55[0]=kcd2vr::physical_hand_owner55::retained(f.input,f.output,f.profiles[0],retained,0);
 f.output.support[0]=false;publish();assert(!f.retainedSupport55[0]&&ReadPhysicalHandHeld55(1,1000)==0);
 f.input.grip[1]=1;retained.generation=3;
 assert(!kcd2vr::physical_hand_owner55::retained(f.input,f.output,f.profiles[0],retained,0));
 retained.generation=2;f.input.tracked[1]=false;
 assert(!kcd2vr::physical_hand_owner55::retained(f.input,f.output,f.profiles[0],retained,0));
 f.input.tracked[1]=true;f.output.support[0]=true;
 f.output.owned[1]=true;f.output.support[0]=false;publish();assert(ReadPhysicalHandHeld55(1,1000)==-1);
 f.output.owned[1]=false;publish();assert(ReadPhysicalHandHeld55(1,1000)==0);
 ++g_pose_epoch6689;assert(ReadPhysicalHandHeld55(0,1000)==-1);--g_pose_epoch6689;
 ++g_weapon_request_epoch6694;assert(ReadPhysicalHandHeld55(0,1000)==-1);--g_weapon_request_epoch6694;
 ++g_phys_ready_sample6698;assert(ReadPhysicalHandHeld55(0,1000)==-1);g_phys_ready_sample6698=10;
 f.correlation126[0].socket.character=reinterpret_cast<void*>(0x9999);publish();assert(ReadPhysicalHandHeld55(0,1000)==-1);
 // Native bow belongs to LEFT slot while actual string hand is RIGHT.
 f.profiles[1]=f.profiles[0];f.profiles[1].side=1;f.profiles[1].classId=9;
 f.profiles[0]={};e.items[0].profile=f.profiles[1];e.handKey[0]=0;e.handKey[1]=77;e.heldIndex[0]=-1;e.heldIndex[1]=0;
 f.output.active[0]=false;f.output.active[1]=f.output.owned[0]=f.output.owned[1]=true;f.output.bowHand169r2=true;
 f.correlation126[1]=f.correlation126[0];f.correlation126[1].socket.character=g_committed_character6689.load();publish();
 assert(ReadPhysicalHandHeld55(0,1000)==1&&ReadPhysicalHandHeld55(1,1000)==1);
 f.input.tracked[0]=false;publish();assert(ReadPhysicalHandHeld55(0,1000)==-1);
 std::puts("PASS C55 actual both-hand native finger adapters/MCP/contact parity/native-held exclusion and actual primary/support occupancy query");
}
