// Actual production contact adapter, native leaves supplied by its existing fixture.
#define KCD2VR_SMITH_CONTACT_FIXTURE_ONLY_D52
#include "smith_tool_contact_repair_test.cpp"
static int revokeD60=0;
static void revokeOwnerD60(){
 using namespace kcd2vr::activity_input163;
 switch(revokeD60){
  case 0:++witness175.lifetime175;break;
  case 1:witness175.active[unsigned(Kind175::Smithing)]=0;break;
  case 2:witness175.transitionDepth175=1;break;
  case 3:++kcd2vr::workstation178r20c1::smithHoldEpoch36;break;
  case 4:++kcd2vr::minigame_hands45::modeEpoch67;break;
  case 5:kcd2vr::station_ws_j27::toolCollision=false;break;
  case 6:smithLiveRepair=false;break;
  case 7:g_tool_socket59c10[0].value.smith36.object++;break;
  case 8:g_tool_socket59c10[0].value.id++;break;
  case 9:nativeOwnerRepair=false;break;
 }
}
int main(){
 for(revokeD60=0;revokeD60<10;++revokeD60){
  resetRepair();V3 goal{};Q4 q{};solveRepair(goal,q);advanceRepair();
  queryStateRepair=wc::Result::Hit;onRayRepair=revokeOwnerD60;
  goal.x=.2f;q={0,.5f,0,.8660254f};solveRepair(goal,q);
  checkRepair(goal.x==.2f&&q.y==.5f&&!g_smith_tool_contact_j27[0].have,
   "owner loss inside world query leaves controller pose and retires stopped history");
 }
 for(revokeD60=0;revokeD60<9;++revokeD60){
  resetRepair();V3 goal{.2f,0,0};Q4 q{};onBoundsRepair=revokeOwnerD60;solveRepair(goal,q);
  checkRepair(goal.x==.2f&&originsRepair.empty()&&!g_smith_tool_contact_j27[0].have,
   "owner loss inside native bounds cannot seed collision history");
 }
 resetRepair();V3 goal{};Q4 q{};solveRepair(goal,q);advanceRepair();queryStateRepair=wc::Result::Clear;goal.x=.2f;solveRepair(goal,q);
 checkRepair(goal.x==.2f&&nativeOwnerReadsRepair==0,"clear world spends no new native binding reads");
 resetRepair();goal={};q={};solveRepair(goal,q);advanceRepair();queryStateRepair=wc::Result::Hit;goal.x=.2f;solveRepair(goal,q);
 checkRepair(std::fabs(goal.x-.18f)<1e-6&&nativeOwnerReadsRepair==1,"stable owner preserves contact with one final binding rejoin");
 resetRepair();goal={};q={};solveRepair(goal,q);advanceRepair();queryStateRepair=wc::Result::Unknown;goal.x=.2f;
 revokeD60=1;onRayRepair=revokeOwnerD60;solveRepair(goal,q);
 checkRepair(goal.x==.2f&&!g_smith_tool_contact_j27[0].have,"unknown query also retires revoked owner without freezing");
 // A refreshed same-binding mailbox must not renew this already-started
 // solve after a slow external query expired its original samples.
 resetRepair();goal={};q={};solveRepair(goal,q);advanceRepair();queryStateRepair=wc::Result::Hit;goal.x=.2f;
 onRayRepair=+[](){nowRepair+=81;g_tool_socket59c10[0].value.tick=nowRepair;};solveRepair(goal,q);
 checkRepair(goal.x==.2f&&!g_smith_tool_contact_j27[0].have,"new same-owner publication cannot renew original socket deadline");
 resetRepair();goal={};q={};solveRepair(goal,q);advanceRepair();g_committed_frame6695.value.stamp.tick=nowRepair-70;
 queryStateRepair=wc::Result::Hit;goal.x=.2f;onRayRepair=+[](){nowRepair+=15;g_tool_socket59c10[0].value.tick=nowRepair;};solveRepair(goal,q);
 checkRepair(goal.x==.2f&&!g_smith_tool_contact_j27[0].have,"new socket cannot renew the original committed contact-pose deadline");
 std::printf("D60 smith contact owner: %u checks PASS\n",checksRepair);
}
