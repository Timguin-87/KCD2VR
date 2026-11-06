#define KCD2VR_MORTAR69_NO_MAIN
#include "mortar_input69_test.cpp"
namespace i75=kcd2vr::subsystems::alchemy::instant_d72;
static a::Output beginPour75(Fixture& q){
 pair69(q,false);q.advance();q.in.palm[1].point={.5,0,0};q.in.nativeFrame.offers[6].point={.52,0,0};q.selected(6);
 q.f.hand[1].rotation[0]=std::sin(.3f);q.f.hand[1].rotation[3]=std::cos(.3f);
 return q.step();
}
int main(){
 for(int fault=-1;fault<13;++fault){
  i75::receipt.publish({});Fixture q;auto o=beginPour75(q);
  check(o.use&&o.control.heldMortarPourD75&&q.state.mortarReturning69);
  i75::Receipt r{};r.instance=q.in.context.instance;r.parent=777;r.serial=1;r.lifetime=q.in.context.lifetime;
  r.admission=q.state.admittedTick;r.poseEpoch=q.f.poseEpoch;r.sourceEpoch=q.f.sourceEpoch;r.modeEpoch=q.in.modeEpoch;
  r.tick=q.in.now;r.source=2;r.destination=6;r.side=1;r.action=i75::Action::PourMortarD75;r.entered=r.effect=r.completed=true;
  switch(fault){case 0:++r.instance;break;case 1:++r.lifetime;break;case 2:++r.admission;break;
   case 3:++r.poseEpoch;break;case 4:++r.sourceEpoch;break;case 5:++r.modeEpoch;break;
   case 6:r.source=3;break;case 7:r.destination=0;break;case 8:r.side=0;break;
   case 9:r.entered=false;break;case 10:r.effect=false;break;case 11:r.completed=false;break;
   case 12:r.action=i75::Action::PlaceBowl;break;}
  i75::receipt.publish(r);q.advance();o=q.step();
  check(q.state.mortarContentsTransferredD75==(fault<0));
  if(fault<0){check(o.admitted&&!o.use&&!q.state.mortarReturning69&&o.control.held[0]&&o.control.held[1]);
   q.advance(40);o=q.step();check(!o.use&&!o.mortarStop69.serial&&q.state.mortarPair69); // sustained tilt cannot empty twice
   q.advance();q.grip(1,0);o=q.step();check(o.use&&!o.control.heldMortarPourD75); // real release remains native return
  }
 }
 i75::receipt.publish({});Fixture released;pair69(released,false);released.advance();released.in.palm[1].point={.5,0,0};
 released.in.nativeFrame.offers[6].point={.52,0,0};released.selected(6);released.step();released.advance();released.grip(1,0);
 check(!released.step().control.heldMortarPourD75); // no instant held-container semantics for open Grip
 i75::receipt.publish({});
 std::printf("D75 mortar retained-contents input: %u checks PASS\n",checks);
}
