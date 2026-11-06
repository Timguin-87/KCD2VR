// Complete production floor adapter + nested Assetslot reader; only the native
// query leaves are doubled. Revocation happens inside a bounds query.
#define KCD2VR_SMITH_FLOOR_FIXTURE_ONLY_D62
#include "smith_native_floor_d56_test.cpp"
static int faultD62=0;
static void revokeD62(){
 auto& witness=kcd2vr::activity_input163::witness175;auto& sock=g_tool_socket59c10[1].value;
 switch(faultD62){
  case 0:++witness.lifetime175;break;case 1:witness.active[0]=0;break;
  case 2:witness.transitionDepth175=1;break;case 3:witness.ready=false;break;
  case 4:++witness.smithEpochC1;break;case 5:witness.phase=14;break;
  case 6:++kcd2vr::workstation178r20c1::smithHoldEpoch36;break;
  case 7:++kcd2vr::minigame_hands45::modeEpoch67;break;
  case 8:kcd2vr::minigame_hands45::smithingMode67=2;break;
  case 9:kcd2vr::station_ws_j27::toolCollision=false;break;
  case 10:now+=81;sock.tick=now;break;case 11:++sock.id;break;
  case 12:++sock.smith36.object;break;case 13:sock.grip67.t[0]+=.02f;break;
  case 14:sock.relative.t[0]+=.02f;break;case 15:sock.gripReady67=false;break;
  case 16:sock.nativeGrasp13=true;break;case 17:sock.tick=now+1;break;
 }
}
int main(){
 for(bool nested:{false,true})for(faultD62=0;faultD62<18;++faultD62){
  setup(nested);auto tx=ReadSmithContactHistoryR38();V3 goal{.2f,.3f,.5f};
  check(solve(tx,goal),"valid original owner can acquire native idle floor");
  CommitSmithContactHistoryR38(tx);refresh();goal={.2f,.3f,.4f};onBoundsD62=revokeD62;
  check(!solve(tx,goal)&&near(goal.z,.4f)&&near(goal.x,.2f)&&near(goal.y,.3f)&&!tx.nativeFloorD56.have,
   "revoked or expired query cannot clamp the new wrist or retain old floor history");
 }
 for(bool nested:{false,true}){
  setup(nested);auto tx=ReadSmithContactHistoryR38();V3 goal{.2f,.3f,.5f};
  onBoundsD62=+[](){now+=30;g_tool_socket59c10[1].value.tick=now;};
  check(solve(tx,goal)&&near(goal.z,1),"same owner/socket geometry within original source deadline still acquires floor");
  CommitSmithContactHistoryR38(tx);refresh();current.phase=14;kcd2vr::activity_input163::witness175.phase=14;
  goal.z=.4f;check(solve(tx,goal)&&near(goal.z,1),"current same-tool raise retains previously captured native idle floor");
 }
 std::printf("D62 smith floor owner: %u checks PASS; no scoring/mesh physics or headset claim\n",checks);
}
