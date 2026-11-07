// Actual input state machine; native semantics are explicitly supplied receipts.
// This verifies a parent completed between observations, not game execution.
#define KCD2VR_INPUT67_NO_MAIN
#include "input67_test.cpp"
namespace instant72=kcd2vr::subsystems::alchemy::instant_d72;
int main(){
 for(int fault=-1;fault<12;++fault){
  instant72::receipt.publish({});Fixture q;q.admit(8,0);q.advance();
  auto& table=q.in.nativeFrame.table;table.handTypesKnown68=true;table.handType68[0]=8;
  q.step();q.advance(90);q.in.nativeFrame.offers[6].point={.05,0,0};q.selected(6);q.step();
  q.advance();q.grip(0,0);auto requested=q.step();
  check(requested.ordinaryReturnRequested&&requested.use&&q.state.ordinaryReturning);
  // No parent pointer ever appeared in a producer observation. Completion
  // belongs to this physical source, recipient, side and admission lifetime.
  instant72::Receipt r{};r.instance=q.in.context.instance;r.parent=999;r.serial=1;
  r.lifetime=q.in.context.lifetime;r.admission=q.state.admittedTick;r.poseEpoch=q.in.context.poseEpoch;
  r.sourceEpoch=q.in.context.sourceEpoch;r.modeEpoch=q.in.modeEpoch;r.tick=q.in.now;
  r.source=8;r.destination=6;r.side=0;r.action=instant72::Action::PlaceIngredient;
  r.entered=r.effect=r.completed=true;
  switch(fault){case 0:++r.instance;break;case 1:++r.lifetime;break;case 2:++r.admission;break;
   case 3:++r.poseEpoch;break;case 4:++r.sourceEpoch;break;case 5:++r.modeEpoch;break;
   case 6:--r.source;break;case 7:r.destination=2;break;case 8:r.side=1;break;
   case 9:r.effect=false;break;case 10:r.completed=false;break;case 11:r.action=instant72::Action::TakeIngredient;break;}
  instant72::receipt.publish(r);q.advance();table.handType68[0]=21;
  auto done=q.step();check(done.ordinaryReturnObserved==(fault<0));
  if(fault<0){check(!done.admitted&&!done.use&&!done.control.passenger);
   q.advance();check(!q.step().ordinaryReturnRequested);}
 }
 instant72::receipt.publish({});
 std::printf("D72 direct alchemy completion: %u checks PASS; native effects not executed in host\n",checks);
}
