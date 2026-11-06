// Actual held-source session -> existing native selection bridge -> native
// Stop/Place/return input. Native action/attachment outcomes remain explicit leaves.
#define KCD2VR_MORTAR69_NO_MAIN
#include "mortar_input69_test.cpp"
namespace b=kcd2vr::subsystems::alchemy::bridge67;
namespace geo=kcd2vr::subsystems::alchemy::geometry68;
namespace kcd2vr::frik_pose_probe {
static workstation_sample46::Frame currentH21{};
bool WorkstationFrame46(workstation_sample46::Frame& out) noexcept {out=currentH21;return true;}
}
static geo::Bounds boxH21(double x,double y=0){
 geo::Bounds b{};for(int j=0;j<3;++j){b.local[j]=-.04f;b.local[j+3]=.04f;b.world[j*4+j]=1;}
 b.world[3]=float(x);b.world[7]=float(y);return b;
}
static void aimH21(Fixture& q,int target=6){
 q.in.context.tableGeneration=q.in.context.lifetime;
 q.in.palm[1].point={.2,0,0};
 q.in.nativeFrame.offers[0]={{0,100},{1,1,0},q.in.now,true,boxH21(1,1),true};
 q.in.nativeFrame.offers[6]={{6,106},{1,0,0},q.in.now,true,boxH21(1),true};
 q.in.aimH21[1]={{1,0,0},q.in.palm[1].tick,q.f.poseEpoch,q.f.sourceEpoch,q.in.modeEpoch,1,true};
 if(target==0){q.in.nativeFrame.offers[0].bounds=boxH21(1);q.in.nativeFrame.offers[6].bounds=boxH21(1,1);}
}
static void publishH21(Fixture& q,const a::Output& o){
 b::enabled=true;kcd2vr::minigame_hands45::modeEpoch67=q.in.modeEpoch;kcd2vr::tracking59c6::epoch=q.f.sourceEpoch;
 kcd2vr::frik_pose_probe::currentH21=q.f;
 q.in.nativeFrame.table.action=q.in.action.parent;q.in.nativeFrame.table.actionKnown=q.in.action.valid;
 b::finish(q.in.nativeFrame.table);b::nomination.publish(o.nomination);b::control.publish(o.control);
 for(int j=0;j<2;++j)b::tools[j].publish(q.in.tool[j]);
 for(const auto& offer:q.in.nativeFrame.offers)if(offer.valid)b::offers[offer.ref.slot].publish({1,2,3,offer});
}
static void controllerPairH21(Fixture& q){
 q.in.controllerMortar70=true;q.in.context.tableGeneration=3;q.in.nativeFrame.offers[14].valid=false;
 q.in.nativeFrame.offers[2]={{2,102},{0,0,0},q.in.now,true,boxH21(0),true};
 q.in.nativeFrame.offers[6]={{6,106},{1,0,0},q.in.now,true,boxH21(1),true};
 q.in.palm[1].point={0,0,0};q.grip(1,1);q.step();q.advance();q.selected(2);
 check(q.step().admitted&&q.state.mortarHome70.valid);
 q.grip(0,1);q.tool(0,3);q.tool(1,2);
 for(int h=0;h<2;++h){auto& t=q.in.tool[h];t.binding={1,2,888,std::uintptr_t(800+h),t.entity,h,t.type};t.binding.object=900+h;t.binding.lifetime=3;}
 q.step();q.advance(81);q.step();
}
int main(){
 Fixture remote;aimH21(remote);remote.grip(1,1);remote.selected(6);auto remoteOut=remote.step();
 check(!remoteOut.admitted&&!remoteOut.use&&!remoteOut.nomination.decision.aimedDestinationH21); // never remote source acquisition
 Fixture q;q.in.context.tableGeneration=3;pair69(q);q.advance();aimH21(q);q.in.nativeSelectedKnown=false;
 auto o=q.step();check(o.admitted&&o.nomination.decision.aimedDestinationH21&&o.nomination.decision.ref.slot==6);
 check(!o.use&&!o.mortarStop69.serial); // nomination is never native acceptance/effect
 publishH21(q,o);unsigned id=0;check(b::selectionNear68(q.in.nativeFrame.table,id,q.in.now)&&id==106);
 // The actual native final selector must accept the same destination first.
 q.advance();aimH21(q);q.selected(6);o=q.step();check(!o.mortarStop69.serial);
 q.advance();aimH21(q);q.grip(1,0);o=q.step();check(o.mortarStop69.serial&&!o.use&&q.state.mortarStopping69);
 publishH21(q,o);check(b::selectionNear68(q.in.nativeFrame.table,id,q.in.now)&&id==106);
 const auto stop=o.mortarStop69.serial;
 q.advance();aimH21(q);q.in.work={};q.in.action.parent=0;for(auto& t:q.in.tool){t.parent=0;t.binding.action=0;}
 q.in.mortarResult69={stop,q.in.now,1,3,kcd2vr::subsystems::alchemy::mortar69::Status::ReadyForNativePlace};
 o=q.step();check(o.use&&o.passenger&&q.state.mortarReturning69); // ONE native place request
 q.advance();aimH21(q);q.in.action.parent=777;q.in.action.placeMortar69=true;o=q.step();check(!o.use&&q.state.mortarReturnParent69==777);
 q.advance();q.in.action.parent=0;q.in.nativeFrame.table.handTypesKnown68=true;q.in.nativeFrame.table.handType68={21,21};
 o=q.step();check(o.mortarPlaced69&&!o.admitted&&!o.use); // only actual native detach/end completes
 // Neither action end nor a temporary destination gap discards a held pair.
 Fixture carry;carry.in.context.tableGeneration=3;pair69(carry,false);carry.advance();aimH21(carry);carry.in.aimH21[1].valid=false;
 carry.in.nativeSelectedKnown=false;o=carry.step();check(o.admitted&&o.control.held[1]&&!o.use&&!o.nomination.decision.aimedDestinationH21);
 // Current near contact wins over a competing far aimed recipient.
 carry.advance();aimH21(carry);carry.in.nativeFrame.offers[0].bounds=boxH21(.21);o=carry.step();
 check(o.nomination.decision.ref.slot==0&&!o.nomination.decision.aimedDestinationH21);
 for(int fault=0;fault<10;++fault){Fixture v;v.in.context.tableGeneration=3;pair69(v);v.advance();aimH21(v);v.selected(6);
  if(fault==0)v.in.aimH21[1].tick-=81;if(fault==1)++v.in.aimH21[1].sourceEpoch;
  if(fault==2)v.in.aimH21[1].side=0;if(fault==3)v.in.aimH21[1].direction={-1,0,0};
  if(fault==4)v.in.aimH21[1].direction={100,0,0};if(fault==5)v.in.nativeFrame.offers[6].bounds=boxH21(3);
  if(fault==6)v.in.tool[1].joined=false;if(fault==7)v.in.nativeFrame.offers[6].boundsValid=false;
  if(fault==8)v.in.nativeFrame.offers[6].ref={1,101};if(fault==9)++v.in.aimH21[1].poseEpoch;
  o=v.step();check(!o.nomination.decision.aimedDestinationH21&&!o.use&&!o.mortarStop69.serial);
 }
 // Revalidate the actual binding, destination bounds and controller frame at
 // the native facing/filter consumer, not only in the input producer.
 Fixture v;v.in.context.tableGeneration=3;pair69(v);v.advance();aimH21(v);o=v.step();publishH21(v,o);
 check(b::selectionNear68(v.in.nativeFrame.table,id,v.in.now)&&id==106);
 auto tool=v.in.tool[1];tool.binding.player++;b::tools[1].publish(tool);check(!b::selectionNear68(v.in.nativeFrame.table,id,v.in.now));
 publishH21(v,o);auto pestle=v.in.tool[0];pestle.joined=false;b::tools[0].publish(pestle);check(!b::selectionNear68(v.in.nativeFrame.table,id,v.in.now));
 publishH21(v,o);auto differentParent=v.in.tool[0];differentParent.parent++;differentParent.binding.action=differentParent.parent;
 b::tools[0].publish(differentParent);check(!b::selectionNear68(v.in.nativeFrame.table,id,v.in.now));
 publishH21(v,o);auto leftChanged=v.in.tool[1];leftChanged.binding.action++;b::tools[1].publish(leftChanged);check(!b::selectionNear68(v.in.nativeFrame.table,id,v.in.now));
 publishH21(v,o);auto otherAction=v.in.nativeFrame.table;otherAction.action++;b::finish(otherAction);check(!b::selectionNear68(otherAction,id,v.in.now));
 publishH21(v,o);auto changed=o.nomination;changed.decision.ref={1,101};b::nomination.publish(changed);check(!b::selectionNear68(v.in.nativeFrame.table,id,v.in.now));
 publishH21(v,o);kcd2vr::frik_pose_probe::currentH21.hand[1].grip=0;check(!b::selectionNear68(v.in.nativeFrame.table,id,v.in.now));
 publishH21(v,o);auto bad=v.in.nativeFrame.offers[6];bad.bounds=boxH21(1,2);b::offers[6].publish({1,2,3,bad});check(!b::selectionNear68(v.in.nativeFrame.table,id,v.in.now));
 const auto hit=geo::ray(boxH21(1),{.2,0,0},{1,0,0});check(hit.valid&&std::abs(hit.distance-.76)<1e-5);
 check(!geo::ray(boxH21(-1),{.2,0,0},{1,0,0}).valid);
 // Aim nominates the destination but cannot remotely earn pouring/return work.
 // The existing native placement child still needs the real near-pot tilt and
 // later upright return to the original home to advance its exact clock.
 Fixture physical;controllerPairH21(physical);physical.advance();aimH21(physical);physical.selected(6);
 physical.f.hand[1].rotation[0]=std::sin(.3f);physical.f.hand[1].rotation[3]=std::cos(.3f);
 o=physical.step();check(o.use&&physical.state.mortarReturning69&&!o.passenger&&o.control.controllerMortar70);
 physical.advance(20);aimH21(physical);physical.in.action.parent=777;physical.in.action.placeMortar69=true;
 for(auto& t:physical.in.tool){t.parent=777;t.binding.action=777;}
 physical.in.work={1,777,888,3,physical.in.now,66,s::MortarPlace70,0,3,true,false};
 physical.step();physical.advance(20);aimH21(physical);o=physical.step();check(o.semiControl.totalSeconds==0);
 physical.advance(20);aimH21(physical);physical.in.palm[1].point={.98,0,0};o=physical.step();
 check(o.semiControl.totalSeconds>.019&&o.gesture.status==g::Status::Pouring);
 const auto poured=o.semiControl.totalSeconds;physical.advance(20);physical.in.work.phase=1;
 physical.f.hand[1].rotation[0]=0;physical.f.hand[1].rotation[3]=1;o=physical.step();check(o.semiControl.totalSeconds==poured);
 physical.advance(20);physical.in.palm[1].point={0,0,0};o=physical.step();check(o.semiControl.totalSeconds>poured);
 physical.advance(20);physical.in.work.phase=2;o=physical.step();check(o.semiControl.manualUse);
 std::printf("mortar aim H21: %u checks PASS; existing native selection/Stop/Place/end owns effects\n",checks);
}
