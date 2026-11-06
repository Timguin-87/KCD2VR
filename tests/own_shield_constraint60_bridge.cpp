// Execute the actual qualified coherent-pose adapter and physical publication.
#include "../src/frik_pose_probe.cpp"
#include <cassert>
#include <cstdio>
namespace kcd2vr::diagnostic_trace {void Write(const char*) noexcept {}}
using namespace kcd2vr::frik_pose_probe;
static void transaction60(CoherentTransaction6689& tx){
 tx.epoch=1;tx.physicalActive=true;
 tx.tracking.allowed=tx.tracking.head_valid=tx.tracking.left_valid=tx.tracking.right_valid=true;
 tx.tracking.sequence=20;tx.tracking.tick=2000;tx.tracking.timeNs=2000000000;
 tx.tracking.source_epoch=kcd2vr::tracking59c6::epoch.load();
 auto& in=tx.physicalInput;in.allowed=in.contact=true;in.sample=20;in.tick=2000;in.timeNs=2000000000;in.epoch=2;in.bodyEpoch=1;
 auto& e=tx.physicalEquipment;e.valid=true;e.epoch=1;e.request=2;e.tick=2000;
 e.player=reinterpret_cast<void*>(0x2345);e.manager=reinterpret_cast<void*>(0x3456);e.count=2;
 for(int side=0;side<2;++side){
  in.tracked[side]=in.intentTracked[side]=true;in.grip[side]=1;in.reach[side]=1.8;
  auto& p=tx.physicalProfiles[side];p.side=side;p.key=77+side;p.generation=88+side;p.entity=99+side;
  p.primary=p.geometry=p.diagnosticBounds59c6=true;p.classId=side?8:4;p.tip={0,1,0};
  p.diagnosticBoundsMin59c6={-.01,.1,-.4};p.diagnosticBoundsMax59c6={.01,.9,.4};
  e.handKnown[side]=true;e.handKey[side]=p.key;e.heldIndex[side]=side;e.items[side].held=true;e.items[side].profile=p;
  auto& c=tx.physicalCorrelation126[side];c.socket.valid=true;c.socket.key=p.key;c.socket.generation=p.generation;
  c.socket.character=g_pose6689.character;c.stamp={1,19,1990,0x1234};
  auto& o=tx.physicalOutput;o.active[side]=o.owned[side]=true;
  ph::Pose start{},end{};if(side==0){start.p.x=.2;end.p.x=-.2;}
  o.checkedPath60[side].begin(start,end,true);o.weapons[side]=o.displayWeapons[side]=end;
  o.hands[side]=o.displayHands[side]=ph::compose(end,p.grip[side]);
  auto& s=tx.physicalState.weapon[side];s.key=p.key;s.generation=p.generation;s.ready=true;s.weapon=end;s.lastTick=2000;s.lastSample=20;
 }
 tx.physicalState.epoch=2;tx.physicalState.bodyEpoch=1;tx.physicalState.sample=20;
 tx.physicalState.weapon[0].prepared=true;tx.physicalOutput.add(ph::CommandKind::Slash,in,tx.physicalProfiles[0]);
 tx.physicalOutput.impactRecord178r30[0].valid=true;tx.physicalOutput.contact119[0].plane=true;
 tx.physicalOutput.contact119[0].native172.entityId=123;tx.physicalOutput.contactKnown[0]=true;
}
int main(){
 g_pose6689.character=reinterpret_cast<void*>(0x1234);g_pose_epoch6689=1;g_weapon_request_epoch6694=2;
 g_committed_character6689=g_pose6689.character;g_gameplay_allowed=true;g_ctx_menu=false;g_ctx_dialogue=false;g_ctx_cutscene=false;g_ctx_reading=false;
 kcd2vr::animation_filter::nativeBodyOwned112=false;g_weapon_control_mode=4;g_phys_armed6698=true;g_phys_fault6698=false;
 g_tool_context19c4r2.store(2000ull<<8);ph::preciseContacts44=false;
 for(int mode=0;mode<3;++mode){CoherentTransaction6689 tx{};transaction60(tx);tx.physicalInput.positionGuard176=mode!=0;
  tx.physicalInput.attacks=mode!=0;tx.physicalInput.physicalCombat177=mode!=0; // native outcomes remain absent
  const auto result=ConstrainOwnShield60(tx);assert(result.usable&&result.limited);
  assert(tx.physicalOutput.weapons[0].p.x>.01799&&tx.physicalOutput.weapons[0].p.x<.0181);
  assert(!tx.physicalOutput.contact119[0].known&&!tx.physicalOutput.contactKnown[0]&&!tx.physicalOutput.impactRecord178r30[0].valid);
  assert(tx.shieldContact54.eligible&&tx.shieldContact54.blade.a.x>.01799&&tx.shieldContact54.blade.a.x<.0181);
  assert(tx.physicalOutput.checkedPath60[0].point[tx.physicalOutput.checkedPath60[0].count-1].p.x==tx.physicalOutput.weapons[0].p.x);
  bool abort=false;for(unsigned i=0;i<tx.physicalOutput.commandCount;++i){assert(tx.physicalOutput.commands[i].kind!=ph::CommandKind::Slash);abort|=tx.physicalOutput.commands[i].kind==ph::CommandKind::Abort;}assert(abort);
  // Exact production publisher receives the constrained pose/state/cache. No
  // second renderer-specific mesh correction can disagree with the hand packet.
  const float world[8]{0,0,0,1,0,0,0,1};
  PhysFrame6698 previous{};previous.input=tx.physicalInput;previous.input.sample=19;previous.input.tick=1980;
  previous.input.timeNs=1980000000;previous.output=tx.physicalOutput;
  previous.output.ownShieldBlocked60[0]=previous.output.ownShieldBlocked60[1]=false;
  previous.player=tx.physicalEquipment.player;previous.manager=tx.physicalEquipment.manager;previous.request=2;
  assert(g_phys_frame6698.publish(previous));g_phys_ready_sample6698=19;
  const auto oldPolicy=kcd2vr::contact176::policy.load(),oldRecovery=kcd2vr::contact176::recoveryEpoch55c1.load();
  PhysPublish6698(tx.physicalInput,tx.physicalOutput,tx.physicalProfiles,tx.physicalEquipment,world,
      tx.physicalState,tx.physicalHolsters,&tx.physicalCorrelation126);
  PhysFrame6698 f{};assert(g_phys_frame6698.read(f)&&f.input.sample==20&&f.output.ownShieldBlocked60[0]);
  assert(f.output.weapons[0].p.x==tx.physicalOutput.weapons[0].p.x&&g_phys_state6698.weapon[0].weapon.p.x==f.output.weapons[0].p.x);
  assert(!ph::attack_authority55c1(f.input,f.output,0));
  assert(kcd2vr::contact176::policy.load()==oldPolicy+(mode!=0));
  assert(kcd2vr::contact176::recoveryEpoch55c1.load()==oldRecovery+(mode!=0));
  // Re-publishing a stopped packet cannot repeatedly advance policy. The old
  // token remains retired after a later clear publication/withdrawal.
  const auto stoppedPolicy=kcd2vr::contact176::policy.load();
  PhysPublish6698(tx.physicalInput,tx.physicalOutput,tx.physicalProfiles,tx.physicalEquipment,world,
      tx.physicalState,tx.physicalHolsters,&tx.physicalCorrelation126);
  assert(kcd2vr::contact176::policy.load()==stoppedPolicy);
  tx.physicalOutput.ownShieldBlocked60[0]=tx.physicalOutput.ownShieldBlocked60[1]=false;
  PhysPublish6698(tx.physicalInput,tx.physicalOutput,tx.physicalProfiles,tx.physicalEquipment,world,
      tx.physicalState,tx.physicalHolsters,&tx.physicalCorrelation126);
  assert(kcd2vr::contact176::policy.load()==stoppedPolicy);
 }
 // H31: public HYBRID emits one primary intent proof. A display-only recovery
 // of the left torch must not retire that already-paid right cut.
 // Every publication below still executes the actual production publisher.
 for(int toolClass:{11,8}){
  CoherentTransaction6689 tx{};transaction60(tx);auto& input=tx.physicalInput;auto& output=tx.physicalOutput;
  input.attacks=input.physicalCombat177=input.coherentMelee119=input.forwardOnly19=true;
  input.intent59c16=false;
  for(int side=0;side<2;++side){input.modelIntentSourceH30[side]=input.modelIntentTrackedH30[side]=true;
   input.modelIntentHandsH30[side]=input.hands[side];}
  tx.physicalProfiles[1].primary=true;tx.physicalProfiles[1].classId=toolClass;
  tx.physicalEquipment.items[1].profile=tx.physicalProfiles[1];output.commandCount=0;output.impactRecord178r30[0]={};
  output.normalIntentH30[0]={77,88,1,2,input.sample,input.tick,true};
  const float world[8]{0,0,0,1,0,0,0,1};g_phys_ready_sample6698=0;
  auto publish=[&](){tx.physicalEquipment.tick=input.tick;
   PhysPublish6698(input,output,tx.physicalProfiles,tx.physicalEquipment,world,
       tx.physicalState,tx.physicalHolsters,&tx.physicalCorrelation126);};
  auto advance=[&](){++input.sample;input.tick+=10;input.timeNs+=10000000;
   output.normalIntentH30[0].sample=input.sample;output.normalIntentH30[0].tick=input.tick;};
  publish();const auto originalRecovery=kcd2vr::contact176::recoveryEpoch55c1.load();
  const auto originalPolicy=kcd2vr::contact176::policy.load();
  kcd2vr::swing_effort59c14::Ledger paid;
  const kcd2vr::motion59c4::Key cut{900,77,88,1,2,originalRecovery,reinterpret_cast<std::uintptr_t>(tx.physicalEquipment.player)};
  assert(paid.publish(cut,1.,1.1,false,input.tick)&&paid.nativeComplete54(cut,input.tick));
  advance();output.handlingOnly55c1[1]=true;output.contactKnown[1]=false;output.contact119[1]={};
  assert(ph::command_authority_h30(input,output,tx.physicalProfiles[0],0));
  assert(!ph::attack_authority55c1(input,output,0)&&!ph::attack_authority55c1(input,output,1));
  publish();
  assert(kcd2vr::contact176::recoveryEpoch55c1.load()==originalRecovery);
  assert(kcd2vr::contact176::policy.load()==originalPolicy&&paid.paidFor54(cut,input.tick));
  // Re-run the same offhand edge with each hard dependency invalidated. The
  // primary proof alone never launders a lost tracking/item/context owner.
  for(bool alreadyRecovering:{false,true})for(unsigned cause=0;cause<14;++cause){
   CoherentTransaction6689 hard{};transaction60(hard);
   hard.physicalInput=tx.physicalInput;hard.physicalOutput=tx.physicalOutput;
   hard.physicalProfiles=tx.physicalProfiles;hard.physicalEquipment=tx.physicalEquipment;
   hard.physicalState=tx.physicalState;hard.physicalHolsters=tx.physicalHolsters;
   hard.physicalCorrelation126=tx.physicalCorrelation126;hard.physicalOutput.handlingOnly55c1[1]=alreadyRecovering;
   PhysFrame6698 previous{};previous.input=hard.physicalInput;previous.output=hard.physicalOutput;
   previous.profiles=hard.physicalProfiles;previous.player=hard.physicalEquipment.player;
   previous.manager=hard.physicalEquipment.manager;previous.request=hard.physicalEquipment.request;
   assert(g_phys_frame6698.publish(previous));g_phys_ready_sample6698=previous.input.sample;
   auto& hin=hard.physicalInput;auto& hout=hard.physicalOutput;
   ++hin.sample;hin.tick+=10;hin.timeNs+=10000000;hard.physicalEquipment.tick=hin.tick;
   hout.normalIntentH30[0].sample=hin.sample;hout.normalIntentH30[0].tick=hin.tick;
   hout.handlingOnly55c1[1]=true;
   switch(cause){
    case 0:hin.tracked[1]=false;break;
    case 1:hin.intentTracked[1]=false;break;
    case 2:hin.modelIntentTrackedH30[1]=false;break;
    case 3:++hard.physicalProfiles[1].key;break;
    case 4:++hard.physicalProfiles[1].generation;break;
    case 5:hout.normalIntentH30[0]={};break;
    case 6:hin.allowed=false;hin.physicalCombat177=false;break;
    case 7:hout.ownShieldBlocked60[1]=true;break;
    case 8:hout.solverOwnedT4[1]=true;break;
    case 9:++hout.normalIntentH30[0].request;break;
    case 10:hard.physicalProfiles[1].classId=4;break; // melee offhand has no independent tool exception
    case 11:hin.modelIntentSourceH30[1]=false;break;
    case 12:hard.physicalProfiles[1].side=0;break;
    case 13:hard.physicalProfiles[0].primary=false;break;
   }
   const auto beforeRecovery=kcd2vr::contact176::recoveryEpoch55c1.load();
   PhysPublish6698(hin,hout,hard.physicalProfiles,hard.physicalEquipment,world,
       hard.physicalState,hard.physicalHolsters,&hard.physicalCorrelation126);
   const auto retired=kcd2vr::contact176::recoveryEpoch55c1.load();assert(retired==beforeRecovery+1);
   if(cause<=4||cause==10||cause==11||cause==12){
    PhysPublish6698(hin,hout,hard.physicalProfiles,hard.physicalEquipment,world,
        hard.physicalState,hard.physicalHolsters,&hard.physicalCorrelation126);
    assert(kcd2vr::contact176::recoveryEpoch55c1.load()==retired);
   }
  }
  std::printf("H31 actual publisher: paid right cut + left class%d primary=true soft recovery preserves original epoch; both contacts stay strict; 28 hard cases retire including loss after recovery PASS\n",toolClass);
 }
 // H30: execute the complete production publisher across a real physical-mode
 // loss. Its current physical flag is false precisely when tracking/context
 // removes attack permission, so retirement must inspect the prior frame too.
 {
  CoherentTransaction6689 tx{};transaction60(tx);auto& input=tx.physicalInput;auto& output=tx.physicalOutput;
  input.attacks=input.physicalCombat177=input.coherentMelee119=input.intent59c16=input.forwardOnly19=true;
  output.commandCount=0;output.impactRecord178r30[0]={};
  output.normalIntentH30[0]={77,88,1,2,input.sample,input.tick,true};
  const float world[8]{0,0,0,1,0,0,0,1};g_phys_ready_sample6698=0;
  auto publish=[&](){tx.physicalEquipment.tick=input.tick;
   PhysPublish6698(input,output,tx.physicalProfiles,tx.physicalEquipment,world,
       tx.physicalState,tx.physicalHolsters,&tx.physicalCorrelation126);};
  publish();const auto originalRecovery=kcd2vr::contact176::recoveryEpoch55c1.load();
  const auto originalPolicy=kcd2vr::contact176::policy.load();
  ++input.sample;input.tick+=10;input.timeNs+=10000000;
  output.handlingOnly55c1[0]=true;output.contactKnown[0]=false;output.contact119[0]={};
  output.normalIntentH30[0].sample=input.sample;output.normalIntentH30[0].tick=input.tick;
  assert(ph::command_authority_h30(input,output,tx.physicalProfiles[0],0)&&!ph::attack_authority55c1(input,output,0));
  publish();
  assert(kcd2vr::contact176::recoveryEpoch55c1.load()==originalRecovery&&kcd2vr::contact176::policy.load()==originalPolicy);
  ++input.sample;input.tick+=10;input.timeNs+=10000000;
  input.physicalCombat177=input.attacks=input.coherentMelee119=false;input.intentTracked[0]=false;
  output.normalIntentH30[0]={};
  publish();const auto retired=kcd2vr::contact176::recoveryEpoch55c1.load();
  assert(retired==originalRecovery+1&&kcd2vr::contact176::policy.load()==originalPolicy+1);
  publish();assert(kcd2vr::contact176::recoveryEpoch55c1.load()==retired);
  ++input.sample;input.tick+=10;input.timeNs+=10000000;
  input.physicalCombat177=input.attacks=input.coherentMelee119=input.intentTracked[0]=true;
  output.handlingOnly55c1[0]=false;output.contactKnown[0]=true;
  output.normalIntentH30[0]={77,88,1,2,input.sample,input.tick,true};
  publish();
  assert(kcd2vr::contact176::recoveryEpoch55c1.load()==retired&&retired!=originalRecovery);
  std::puts("H30 actual publisher: soft recovery retains epoch; current physical=false hard loss retires once and cannot revive on re-entry PASS");
 }
 // All refusal cases execute the production ownership/context gate. They must
 // leave both existing world-solved weapon poses untouched.
 for(unsigned cause=0;cause<14;++cause){CoherentTransaction6689 tx{};transaction60(tx);
  switch(cause){case 0:tx.tracking.left_held=true;break;case 1:tx.tracking.head_held=true;break;
   case 2:tx.tracking.source_epoch+=1;break;case 3:tx.physicalEquipment.handKey[1]+=1;break;
   case 4:tx.physicalCorrelation126[1].socket.generation+=1;break;case 5:tx.physicalInput.epoch+=1;break;
   case 6:tx.physicalOutput.support[0]=true;break;case 7:tx.physicalOutput.displayLatched[0]=true;break;
   case 8:tx.physicalOutput.checkedPath60[1].valid=false;break;case 9:tx.physicalProfiles[0].classId=3;break;
   case 10:tx.physicalInput.intentTracked[1]=false;break;case 11:tx.physicalCorrelation126[1].stamp.tick=1000;break;
   case 12:g_ctx_menu=true;break;case 13:tx.physicalInput.reach[0]=.01;break;}
  const auto before=tx.physicalOutput;assert(!ConstrainOwnShield60(tx).usable);
  assert(std::memcmp(&before,&tx.physicalOutput,sizeof(before))==0);g_ctx_menu=false;
 }
 std::puts("own_shield_constraint60_bridge: actual coherent qualification/publication, full-pose cache/hand agreement, precise OFF, native-contact retirement and owner refusals PASS");
}
