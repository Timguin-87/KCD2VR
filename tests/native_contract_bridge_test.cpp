// Execute actual current frik private contracts, using only in-memory native data.
#include "../src/frik_pose_probe.cpp"
#include <cstdio>
namespace kcd2vr::diagnostic_trace { void Write(const char*) noexcept {} }
using namespace kcd2vr::frik_pose_probe;
static int count=0,passed=0;
void check(bool x,const char*n){++count;passed+=x;std::printf("%s %s\n",x?"PASS":"FAIL",n);}
template<class T>void put(unsigned char*p,T value){std::memcpy(p,&value,sizeof(value));}
void __fastcall original(void*,const std::uint64_t*,int,float){}
int main(){
 std::vector<unsigned char> image(0x5488000);const auto base=reinterpret_cast<std::uintptr_t>(image.data());hostModuleHandleW=image.data();hostTickCount64=1000;
 check(!OwnCombatInputHook101(base),"unregistered input patch is not our verified hook");
 const std::array<unsigned char,16> patch{0xFF,0x25,0,0,0,0,0x34,0x12,0,0,0,0,0,0,0x90,0x90};std::memcpy(image.data()+0x8E5324,patch.data(),16);
 g_combat_own_patch101=patch;g_combat_bypass101=&original;g_combat_hook_base101=base;
 check(OwnCombatInputHook101(base),"physical gate recognizes exact live bytes of our previously installed input hook");
 image[0x8E5324+10]^=1;check(!OwnCombatInputHook101(base),"foreign replacement of our input hook fails exact byte ownership");image[0x8E5324+10]^=1;
 g_combat_bypass101=nullptr;check(!OwnCombatInputHook101(base),"matching bytes without original trampoline never satisfy arming gate");g_combat_bypass101=&original;
 check(!OwnCombatInputHook101(base+8),"same bytes at wrong module identity are not accepted");
 put(image.data()+0x3A657C0+0x60,base+0x1A72860);put(image.data()+0x3A657C0+0x80,base+0x1069A88);put(image.data()+0x3A657C0+0xC8,base+0x1AAF3C0);
 const unsigned char project[16]{0x48,0x8B,0xC4,0x48,0x89,0x58,0x08,0x48,0x89,0x78,0x10,0x55,0x48,0x8D,0x68,0xA1};std::memcpy(image.data()+0x1069A88,project,16);
 check(PhysSocketContract101(base),"exact native BONE slots and project routine qualify the socket layout");
 image[0x1069A88]^=1;check(!PhysSocketContract101(base),"modified socket projection routine fails closed");image[0x1069A88]^=1;
 put(image.data()+0x3A657C0+0xC8,base+0x1AAF3C1);check(!PhysSocketContract101(base),"wrong joint accessor invalidates layout contract");put(image.data()+0x3A657C0+0xC8,base+0x1AAF3C0);
 std::array<unsigned char,0x100> character{},attachment{};std::array<unsigned char,0x80> manager{};std::array<unsigned char,0x30> self{};
 put(character.data(),base+0x40602F8);put(attachment.data(),base+0x3A657C0);put(attachment.data()+0x28,manager.data());put(manager.data()+0x70,character.data());put(attachment.data()+0x78,std::int16_t(2));
 const QuatT28 relative{{0,0,0,1},{.01f,.02f,0}};put(attachment.data()+0x9C,relative);for(int i=0;i<3;++i)put(self.data()+0x0C+i*4,1.f);
 PhysEquipment6698 e{};e.valid=true;e.epoch=1;e.request=7;e.count=1;e.tick=1000;auto& item=e.items[0];item.held=true;item.profile.key=0xABC;item.profile.generation=1;item.profile.side=0;item.profile.twoHanded=true;item.boundsValid=true;for(int k=0;k<3;++k){item.bounds[k]=-.5f;item.bounds[k+3]=.5f;}
 PhysRememberSocket101(item,self.data(),attachment.data(),1000);PhysSocketBank101 bank{};g_phys_sockets101.read(bank);
 check(bank.sockets[0].valid&&bank.sockets[0].joint==2&&bank.sockets[0].character==character.data(),"exact native BONE layout yields joint/default-relative descriptor");
 g_weapon_control_mode=4;g_phys_armed6698=true;g_weapon_request_epoch6694=7;g_pose_epoch6689=1;
 RigMap rig{};rig.hand_r=0;rig.hand_l=1;std::array<QuatT28,4> native{};TrackingSnapshot t{};
 for(int n=0;n<13;++n){hostTickCount64=1000+n*10;e.tick=hostTickCount64;g_phys_equipment6698.publish(e);t.tick=hostTickCount64;t.sequence=n+1;
  const auto q=pm::from_rotation_vector({0,0,.13*n});native[2]=PhysQuat6698({q,{.05*n,.3,1.3}});
  auto weapon=AbsoluteFromRelative(native[2],relative);
  native[0]=AbsoluteFromRelative(weapon,QuatT28{{0,0,0,1},{0,0,0}});
  native[1]=AbsoluteFromRelative(weapon,QuatT28{{0,0,0,1},{0,-.12f,0}});
  PhysCaptureSamePose101(character.data(),native.data(),native.size(),rig,t);
 }
 auto& g=g_phys_grip_bank6698.grip[0];
 check(g.ready[0]&&g.ready[1],"changing native animation with rigid hand/socket relation qualifies BOTH grips from same pose");
 check(pm::length(g.hand[0].p)<1e-5&&pm::length(g.hand[1].p-pm::Vec3{0,-.12,0})<1e-5,"same-pose grip uses native relation rather than mixed-frame average");
 const auto oldSample=g.lastSample;PhysCaptureSamePose101(character.data(),native.data(),native.size(),rig,t);check(g.lastSample==oldSample,"duplicate pose does not advance grip acquisition");
 item.profile.generation=2;e.tick=hostTickCount64;g_phys_equipment6698.publish(e);++t.sequence;PhysCaptureSamePose101(character.data(),native.data(),native.size(),rig,t);check(g.generation==1,"stale socket generation cannot authorize replacement item grip");
 // Bad descriptor publication leaves preceding good data; producer never advertises it as new.
 const auto before=bank.sockets[0].tick;put(attachment.data()+0x7A,std::uint8_t(1));PhysRememberSocket101(item,self.data(),attachment.data(),1500);g_phys_sockets101.read(bank);check(bank.sockets[0].generation==1&&bank.sockets[0].tick==before,"unprojected/deferred native socket does not become a fresh valid descriptor");
 put(attachment.data()+0x7A,std::uint8_t(0));put(attachment.data(),base+0x123);PhysRememberSocket101(item,self.data(),attachment.data(),1500);g_phys_sockets101.read(bank);check(bank.sockets[0].generation==1,"unsupported attachment class cannot replace exact bone socket");
 // Execute the physical leaf that previously used true for both restore and placement.
 put(self.data()+8,std::uint32_t(42));e.items[0].profile.entity=42;e.valid=false;g_phys_equipment6698.publish(e);g_phys_relevant_id101[0]=42;
 static int nativeRestores=0;g_entity_attachment_update_original=+[](void*,void*){++nativeRestores;};g_phys_pending6698=0;
 auto result=PhysAttachmentResultImpl101(self.data(),attachment.data());check(result==PhysicalAttachmentResult101::Waiting&&nativeRestores==0,"actual unready physical callback returns WAITING, not PLACED or native restore");
 g_phys_placement6698[0]={self.data(),attachment.data(),0xABC,true};g_phys_pending6698=1;
 result=PhysAttachmentResultImpl101(self.data(),attachment.data());check(result==PhysicalAttachmentResult101::Restored&&nativeRestores==1,"actual stale owned physical callback returns RESTORED, never successful takeover");
 // V66.103 fresh live ownership: no item getters or native function addresses executed.
 std::array<unsigned char,0x100> liveManager{};std::array<unsigned char,0xA00> livePlayer{};
 std::array<unsigned char,0x300> handInterface{};
 put(liveManager.data(),base+0x3EC9958);put(liveManager.data()+0x38,static_cast<void*>(livePlayer.data()));
 put(liveManager.data()+0x40,1);put(livePlayer.data(),base+0x3AA4B98);
 put(livePlayer.data()+0x9D0,static_cast<void*>(liveManager.data()));put(livePlayer.data()+0x990,static_cast<void*>(handInterface.data()));
 put(liveManager.data()+0x58,std::uint64_t(0x456));put(handInterface.data()+0x260,std::uint64_t(0x456));
 put(image.data()+0x5325B60,std::uint64_t(0xE));put(handInterface.data()+0x268,std::uint64_t(0xE));
 g_henry_item_attach_manager=liveManager.data();g_weapon_request_epoch6694=55;g_pose_epoch6689=9;
 PhysLive103 a{},b{};check(PhysReadLive103(a)&&a.hand[0]==0x456&&a.hand[1]==0&&a.selected==1,"fresh reader follows exact hand chain and normalizes native empty sentinel");
 check(PhysReadLive103(b)&&PhysSameLive103(a,b),"unchanged fresh hand roles permit metadata reuse");
 put(handInterface.data()+0x268,std::uint64_t(0x999));check(PhysReadLive103(b)&&!PhysSameLive103(a,b),"new occupied offhand invalidates support eligibility immediately, not after cached 100 ms");
 put(handInterface.data()+0x268,std::uint64_t(0xE));put(liveManager.data()+0x60,std::uint64_t(0x777));
 check(PhysReadLive103(b)&&!PhysSameLive103(a,b),"different equipped slot invalidates definition cache selection");
 put(liveManager.data()+0x60,std::uint64_t(0));++g_pose_epoch6689;
 check(PhysReadLive103(b)&&!PhysSameLive103(a,b),"new pose generation cannot reuse prior live eligibility");
 put(livePlayer.data()+0x9D0,static_cast<void*>(nullptr));
 check(!PhysReadLive103(b),"broken live actor-manager backlink rejects cached identity");
 // Execute the exact queue boundary predicates consumed by the native frame pump.
 PhysNativeOwner6698 owner{};owner.attackPrepared=true;owner.prepareFrame=77;
 ph::Command c{};c.kind=ph::CommandKind::Slash;
 check(PhysDeferRelease103(owner,c,77)&&!PhysDeferRelease103(owner,c,78),"prepare and slash cannot drain in the same native callback");
 c.kind=ph::CommandKind::Abort;check(!PhysDeferRelease103(owner,c,77),"owned cancellation is never held behind an attack release barrier");
 owner.bowHeld=true;owner.bowBeginFrame=77;owner.key=0xB;owner.request=4;owner.epoch=8;
 c.kind=ph::CommandKind::BowFire;c.key=0xB;c.epoch=4;c.bodyEpoch=8;c.sample=3;c.tick=2000;
 PhysFrame6698 frame{};frame.input.sample=3;frame.output.bowHeld=false;
 check(PhysDeferRelease103(owner,c,77)&&!PhysDeferRelease103(owner,c,78),"bow begin is observed by native frame before deliberate release");
 check(PhysPendingBowRelease103(owner,c,frame,2020),"queued valid release preserves bow owner while producer already marks draw released");
 c.key=0xC;check(!PhysPendingBowRelease103(owner,c,frame,2020),"another bow key cannot preserve the old nock");c.key=0xB;
 c.bodyEpoch=9;check(!PhysPendingBowRelease103(owner,c,frame,2020),"recentered draw does not resurrect an old queued fire");c.bodyEpoch=8;
 check(!PhysPendingBowRelease103(owner,c,frame,2101),"stale release is cancellation, not postponed fire");
 // B4 executes the real socket->reference->six-sample producer, not duplicate math.
 {
  g_phys_grip_bank6698={};g_weapon_control_mode=4;g_phys_armed6698=true;
  g_weapon_request_epoch6694=7;g_pose_epoch6689=1;
  PhysEquipment6698 shieldEq{};shieldEq.valid=true;shieldEq.request=7;shieldEq.epoch=1;shieldEq.count=1;
  auto& shield=shieldEq.items[0];shield.held=true;shield.profile.key=0xB4;shield.profile.generation=4;
  shield.profile.side=1;shield.profile.classId=8;
  PhysSocketBank101 sb{};auto& socket=sb.sockets[0];socket.valid=true;socket.key=0xB4;socket.generation=4;
  socket.character=character.data();socket.joint=2;
  socket.relative={{-.2362f,-.3535f,.6627f,.6165f},{-.120f,0,0}};
  socket.scale[0]=socket.scale[1]=socket.scale[2]=1;
  std::array<QuatT28,4> reference{};for(auto& x:reference)x.q[3]=1;reference[2].t[0]=.06614168f;
  reference[2].t[1]=-.02392785f;reference[2].t[2]=-.01614014f;
  int parents[4]{-1,-1,1,-1};
  ph::Pose expected{};check(ShieldPrimaryReferenceB4(reference.data(),parents,4,1,2,socket.relative,expected),"actual left-wrist descendant admits authored reference");
  check(pm::length(expected.p-pm::Vec3{.01862131,-.04703572,-.03427272})<1e-5,"retail socket reproduces computed neutral grip");
  auto rotatedReference=reference;
  const auto referenceRoot=PhysQuat6698({pm::from_rotation_vector({.3,-.5,.7}),{1.2,-.6,.9}});
  for(auto& joint:rotatedReference)joint=AbsoluteFromRelative(referenceRoot,joint);
  ph::Pose invariant{};
  check(ShieldPrimaryReferenceB4(rotatedReference.data(),parents,4,1,2,socket.relative,invariant)&&
        pm::length(invariant.p-expected.p)<1e-6&&pm::angle(invariant.q,expected.q)<1e-6,
        "authored shield grip is invariant under reference root translation and rotation");
  for(int n=0;n<6;++n){
   hostTickCount64=3000+n*10;shieldEq.tick=socket.tick=hostTickCount64;g_phys_equipment6698.publish(shieldEq);g_phys_sockets101.publish(sb);
   t.sequence=100+n;t.tick=hostTickCount64;
   native[1]=PhysQuat6698({pm::from_rotation_vector({.2*n,.1*n,-.15*n}),{.1*n,-.2*n,1}});
   native[2]=PhysQuat6698({pm::from_rotation_vector({-.1*n,.25*n,.3*n}),{-.15*n,.18*n,1.2}});
   PhysCaptureSamePose101(character.data(),native.data(),4,rig,t,reference.data(),parents);
  }
  const auto& learned=g_phys_grip_bank6698.grip[0];
  check(learned.ready[1]&&!learned.ready[0]&&pm::length(learned.hand[1].p-expected.p)<1e-6&&pm::angle(learned.hand[1].q,expected.q)<1e-6,
        "six changing block/idle poses retain authored shield primary position AND orientation");
  const auto animated=PhysPose6698(AbsoluteFromRelative(native[2],socket.relative));
  check(pm::length(learned.nativeWeapon.p-animated.p)<1e-6&&pm::angle(learned.nativeWeapon.q,animated.q)<1e-6,
        "shield acquisition seed remains the current animated weapon, never the bind reference");
  // Wrong ancestry/cycle/missing reference cannot turn into an animated fallback.
  parents[2]=2;ph::Pose refused{};
  check(!ShieldPrimaryReferenceB4(reference.data(),parents,4,1,2,socket.relative,refused),"cyclic or unrelated socket refuses reference");
  ++shield.profile.generation;++socket.generation;hostTickCount64+=10;shieldEq.tick=socket.tick=hostTickCount64;
  g_phys_equipment6698.publish(shieldEq);g_phys_sockets101.publish(sb);++t.sequence;t.tick=hostTickCount64;
  PhysCaptureSamePose101(character.data(),native.data(),4,rig,t,reference.data(),parents);
  check(!g_phys_grip_bank6698.grip[0].ready[1],"unqualified replacement shield never learns animated bad grip");
 }

 // First current attachment, not merely a held key, may establish the primary.
 // Real BONE reader and same-evaluation capture feed the actual profile/kernel.
 {
  g_weapon_control_mode=4;g_phys_armed6698=true;g_weapon_request_epoch6694=7;g_pose_epoch6689=1;
  put(attachment.data(),base+0x3A657C0);put(attachment.data()+0x7A,std::uint8_t(0));
  RigMap primaryRig{};primaryRig.hand_r=0;primaryRig.hand_l=1;
  std::array<QuatT28,4> rest{},current{};for(auto& q:rest)q.q[3]=1;for(auto& q:current)q.q[3]=1;
  const QuatT28 handSlot{{0,0,0,1},{.066f,-.024f,-.016f}},objectLocal{{0,0,0,1},{-.07f,.02f,.03f}};
  std::uint64_t sequence=1000;PhysEquipment6698 eq{};eq.valid=true;eq.request=7;eq.epoch=1;eq.count=1;
  auto& it=eq.items[0];it.held=true;it.boundsValid=true;it.profile.weight=2;
  for(int k=0;k<3;++k){it.bounds[k]=-.5f;it.bounds[k+3]=.5f;}
  int hierarchy[4]{-1,-1,0,-1};
  const auto reset=[&](int side){
   g_phys_grip_bank6698={};g_phys_socket_bank101={};g_phys_grips6698.publish({});g_phys_sockets101.publish({});
   it.profile.key=0x1000+sequence;it.profile.generation=3;it.profile.side=side;
   hierarchy[2]=side;rest[2]=AbsoluteFromRelative(rest[side],handSlot);
   put(attachment.data()+0x78,std::int16_t(2));put(attachment.data()+0x9C,objectLocal);
  };
  const auto capture=[&](unsigned sampleAdvance=1){
   sequence+=sampleAdvance;hostTickCount64+=11;eq.tick=hostTickCount64;g_phys_equipment6698.publish(eq);
   PhysRememberSocket101(it,self.data(),attachment.data(),hostTickCount64);
   TrackingSnapshot fresh{};fresh.sequence=sequence;fresh.tick=hostTickCount64;
   PhysCaptureSamePose101(character.data(),current.data(),4,primaryRig,fresh,rest.data(),hierarchy);
  };
  for(int cls:{0,1,2,3,4,5,6,7,8,9,10,11,13,14,15,16,17}){
   ph::Profile role{};role.classId=cls;const int side=cls==11?1:ph::holster_hand126(role);it.profile.classId=cls;it.profile.twoHanded=cls==4||cls==7||cls==9;reset(side);
   current[side]=PhysQuat6698({pm::from_rotation_vector({.2,-.1,.3}),{side?-.2:.2,.3,1.3}});
   current[2]=AbsoluteFromRelative(current[side],handSlot);capture();
   const auto& captured=g_phys_grip_bank6698.grip[0];ph::Pose reference{};
   check(ShieldPrimaryReferenceB4(rest.data(),hierarchy,4,side,2,objectLocal,reference),"primary has exact authored same-wrist attachment reference");
   check(captured.ready[side]&&captured.stable[side]==1,"current matching native wrist primary is ready on first pose");
   const auto actual=ph::compose(ph::inverse(PhysPose6698(AbsoluteFromRelative(current[2],objectLocal))),PhysPose6698(current[side]));
   check(pm::length(actual.p-reference.p)<1e-6&&pm::angle(actual.q,reference.q)<1e-6,"animated wrist still reproduces authored per-item relation");
   std::array<ph::Profile,2> profile{};profile[1].side=1;PhysFillProfile6698(it,captured,profile[side]);
   ph::Input in{};in.allowed=true;in.sample=sequence;in.tick=hostTickCount64;in.timeNs=in.tick*1000000ull;in.epoch=7;in.bodyEpoch=1;in.tracked[side]=true;in.grip[side]=1;
   in.hands[side]=PhysPose6698(current[side]);in.shoulders[side]={side?-.2:.2,0,1.3};in.reach[side]=.65;in.dynamics=false;in.contact=false;in.attacks=false;
   ph::State physical{};const auto out=ph::step(physical,in,profile,[](const ph::Profile&,ph::Pose,ph::Pose,bool){return 1.;});
   check(out.active[side]&&pm::length(ph::compose(out.weapons[side],reference).p-in.hands[side].p)<1e-6,"first native profile/kernel output is correctly seated");
   const auto before=captured.stable[side];const auto originalStamp=captured.nativeStamp;capture(0);
   check(captured.ready[side]&&captured.stable[side]==before&&captured.nativeStamp.tick==originalStamp.tick,"duplicate completed pose does not advance primary acquisition");
  }
  it.profile.classId=4;it.profile.twoHanded=false;reset(0);
  // held metadata arrives while native attachment still points at a stow bone.
  put(attachment.data()+0x78,std::int16_t(3));current[3]=PhysQuat6698({{},{.1,.1,1.1}});
  capture();check(!g_phys_grip_bank6698.grip[0].ready[0],"held key on non-wrist stow bone cannot use first-pose path");
  put(attachment.data()+0x78,std::int16_t(2));current[2]=AbsoluteFromRelative(current[0],handSlot);capture();
  check(g_phys_grip_bank6698.grip[0].ready[0]&&g_phys_grip_bank6698.grip[0].stable[0]==1,"actual switch to wrist binding resets stow candidate and acquires exact primary");
  // Same descendant, but its animated local offset has not reached reference.
  reset(0);auto displaced=handSlot;displaced.t[0]+=.04f;current[2]=AbsoluteFromRelative(current[0],displaced);
  capture();check(!g_phys_grip_bank6698.grip[0].ready[0]&&g_phys_grip_bank6698.grip[0].stable[0]==1,"animated descendant mismatch keeps original acquisition");
  for(int n=1;n<6;++n)capture();check(g_phys_grip_bank6698.grip[0].ready[0]&&g_phys_grip_bank6698.grip[0].stable[0]==6,"unproved animated relation retains original six-sample fallback");
  // Six actual eligible support observations remain necessary after fast primary.
  reset(0);it.profile.twoHanded=true;it.bounds[0]=-.3f;it.bounds[3]=.8f;current[2]=AbsoluteFromRelative(current[0],handSlot);const auto weapon=AbsoluteFromRelative(current[2],objectLocal);
  auto support=ph::compose(ph::inverse(PhysPose6698(weapon)),PhysPose6698(current[0]));support.p.x-=.12;
  current[1]=PhysQuat6698(ph::compose(PhysPose6698(weapon),support));
  for(int n=1;n<=6;++n){capture();const auto& g=g_phys_grip_bank6698.grip[0];check(g.ready[0]&&g.ready[1]==(n==6),"fast primary preserves six current eligible support observations");}
  const auto expectedSupport=ph::compose(ph::inverse(PhysPose6698(weapon)),PhysPose6698(current[1]));
  check(pm::length(g_phys_grip_bank6698.grip[0].hand[1].p-expectedSupport.p)<1e-6,"native support socket geometry is unchanged");
  // Existing equipment/request/pose boundaries still precede any fast capture.
  const auto acceptedSample=g_phys_grip_bank6698.grip[0].nativeStamp.sample;
  eq.request=8;capture();check(g_phys_grip_bank6698.grip[0].nativeStamp.sample==acceptedSample,"request replacement cannot refresh old capture");
  ph::Input denied{};std::array<ph::Profile,2> deniedProfiles{};PhysEquipment6698 deniedEquipment{};
  check(!PhysPrepare6698(denied,deniedProfiles,deniedEquipment),"actual profile ingress rejects mismatched request");
  eq.request=7;eq.epoch=2;capture();check(g_phys_grip_bank6698.grip[0].nativeStamp.sample==acceptedSample,"different pose owner cannot refresh old capture");
  eq.epoch=1;

 }
 std::printf("native_contract_bridge=%d/%d native_function_calls=NONE\n",passed,count);return count!=passed;
}
