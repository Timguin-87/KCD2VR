#define KCD2VR_SMITH_FLOOR_FIXTURE_ONLY_D62
#include "smith_native_floor_d56_test.cpp"
static bool assist(SmithContactHistoryR38& tx,V3& goal,Q4& q,float lt=1.f,int side=1){
 return SmithNativeFloorD56(reinterpret_cast<void*>(character),4,side,pose,sample,5,now,goal,q,tx,lt,&q);
}
static void prepare(bool nested=false){setup(nested);kcd2vr::crafting_d67::setSmithSemiAnchors(true);kcd2vr::station_ws_j27::toolCollision=false;}
int main(){
 namespace a=kcd2vr::subsystems::smithing::assist_d80;
 for(int phase=0;phase<21;++phase)check(a::held(true,3,phase,true,1)==(phase==11||phase==14||phase==15||phase==16||phase==17),"only native anvil phases admit LT");
 for(bool nested:{false,true})for(int phase:{11,14,16,17}){
  prepare(nested);current.phase=phase;refresh();auto& sock=g_tool_socket59c10[1].value;
  QToQuatT(pose.original[0],Q4{0,0,.38268343f,.92387953f});ToT(pose.original[0],{.2f,.3f,1.f});
  QToQuatT(sock.relative,Q4{.258819f,0,0,.9659258f});ToT(sock.relative,{.01f,.02f,.03f});
  QToQuatT(sock.grip67,Q4{0,.1305262f,0,.9914449f});ToT(sock.grip67,{.03f,-.04f,.08f});
  SmithContactHistoryR38 tx{};V3 goal{.4f,.5f,1.8f};Q4 q{};
  check(assist(tx,goal,q),"current attached direct/nested item gets native pose only while held LT");
  QuatT28 wrist{};ToT(wrist,goal);QToQuatT(wrist,q);
  const auto placed=AbsoluteFromRelative(wrist,sock.grip67),expected=AbsoluteFromRelative(pose.original[0],sock.relative);
  for(int i=0;i<3;++i)check(near(placed.t[i],expected.t[i]),"canonical grip places native tool translation exactly");
  for(int i=0;i<4;++i)check(near(placed.q[i],expected.q[i]),"canonical grip places native tool rotation exactly");
  check(boundsReads==0,"assist independent of collision bounds and collision toggle");
  goal={.8f,.7f,1.9f};q={0,0,0,1};check(!assist(tx,goal,q,0)&&near(goal.x,.8f)&&near(goal.y,.7f)&&near(goal.z,1.9f)&&near(q.w,1),"release returns same-frame controller goal without any tail");
  check(!tx.nativeFloorD56.have,"no retained assist pose history after release");
 }
 for(int fault=0;fault<19;++fault){prepare(true);SmithContactHistoryR38 tx{};V3 goal{.4f,.5f,1.8f};Q4 q{};float trigger=1;int side=1;
  switch(fault){
  case 0:kcd2vr::crafting_d67::setSmithSemiAnchors(false);break;
  case 1:kcd2vr::minigame_hands45::smithingMode67=0;break;
  case 2:kcd2vr::minigame_hands45::smithingMode67=1;break;
  case 3:kcd2vr::minigame_hands45::smithingMode67=2;break;
  case 4:current.phase=8;refresh();break;
  case 5:current.phase=10;refresh();break;
  case 6:current.phase=12;refresh();break;
  case 7:trigger=.55f;break;
  case 8:trigger=1.01f;break;
  case 9:trigger=std::numeric_limits<float>::quiet_NaN();break;
  case 10:g_tool_socket59c10[1].value.tick=now-81;break;
  case 11:kcd2vr::activity_input163::witness175.transitionDepth175=1;break;
  case 12:kcd2vr::activity_input163::witness175.lifetime175=6;break;
  case 13:put(holder+st::kAttached,std::uint8_t(0));break;
  case 14:g_tool_socket59c10[1].value.smith36.entity++;break;
  case 15:side=0;break;
  case 16:kcd2vr::activity_input163::witness175.phase=8;break;
  case 17:sample=0;break;
  case 18:pins=false;break;
  }
  check(!assist(tx,goal,q,trigger,side)&&near(goal.x,.4f)&&near(goal.y,.5f)&&near(goal.z,1.8f)&&near(q.w,1),"unqualified input/phase/owner never borrows a native wrist");
 }
 std::printf("D80 LT native anvil assist: %u checks PASS; actual adapter, native reads substituted, headset NOT_RUN\n",checks);
}
