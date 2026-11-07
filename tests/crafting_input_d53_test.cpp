// D53 regression at the actual near/input/current-selection/final-pad boundary.
// Retail selection, geometry and attachment callbacks remain explicit leaves.
#define main retainedProximityMainD53
#include "subsystems/alchemy/proximity_craft1_test.cpp"
#undef main
namespace mh53=kcd2vr::minigame_hands45;
namespace reach53=kcd2vr::subsystems::alchemy::reach_craft1;
template<class T> static void enableHands53(T& in){
 // The same test compiles against untouched D52 to demonstrate the regression.
 if constexpr(requires(T v){v.handsOnlyD53;})in.handsOnlyD53=true;
}
static void policy53(Fixture& q,int saved=0){
 policy1(q);q.in.proximityCraft1=true;enableHands53(q.in);q.in.pickupReachD39=saved;
 reach53::setPickupReachD39(saved);mh53::alchemyMode67=mh53::SixDofVrik;
}
static bool selection53(Fixture& q,const a::Output& out,std::uint32_t& entity){
 publish1(q,out);return b1::selectionNear68(q.in.nativeFrame.table,entity,q.in.now);
}
static void matrix53(){
 for(int saved:{0,1})for(int type=0;type<21;++type)for(int side=0;side<2;++side)for(double gap:{.019,.15,.199,.201}){
  Fixture q;policy53(q,saved);box1(q,type,0);palm1(q,side,gap+.005);palm1(q,1-side,3);
  const bool within=reach53::allowed(type,side)&&gap<.20;auto out=q.step();std::uint32_t entity=999;
  expect1(selection53(q,out,entity)&&entity==(within?unsigned(100+type):0u),"mode3 open hands own exact20cm bounds or empty selection under either saved pickup policy");
  expect1(!out.use&&!out.admitted,"mode3 hover cannot request native Use");
  q.grip(side,1);q.step();refresh1(q);q.selected(type);out=q.step();
  expect1(out.use==(within&&type!=4)&&out.admitted==within,"eligible native fixture is nominated but only carried tools synthesize Grip Use");
 }
}
static void recheck53(){
 for(int fault=0;fault<8;++fault){Fixture q;policy53(q);box1(q,19,0);palm1(q,0,.155);palm1(q,1,3);auto out=q.step();publish1(q,out);
  std::uint32_t entity=999;
  if(fault==0){auto moved=q.in.nativeFrame.offers[19];moved.bounds.world[3]=2;b1::offers[19].publish({1,2,3,moved});}
  if(fault==1)b1::offers[19].publish({});
  if(fault==2)liveFrame1.hand[0].valid=false;
  if(fault==3)++liveFrame1.poseEpoch;
  if(fault==4){auto t=q.in.nativeFrame.table;t.refs[19].entity++;b1::table.publish(t);}
  if(fault==5)mh53::alchemyMode67=mh53::Gesture;
  if(fault==6)++mh53::modeEpoch67;
  if(fault==7)liveFrame1.tick=q.in.now-81;
  const bool owns=b1::selectionNear68(q.in.nativeFrame.table,entity,q.in.now);
  expect1(fault<2?(owns&&entity==0):!owns,"current geometry can clear an open-hand nominee; stale context/mode/tracking revokes it");
 }
 // D44 assisted mode must not revive a distant gaze target while the hand is empty.
 {Fixture q;policy53(q,1);box1(q,19,0);palm1(q,0,2);palm1(q,1,3);q.grip(0,1);q.step();
  refresh1(q);q.selected(19);auto out=q.step();std::uint32_t entity=999;
  expect1(selection53(q,out,entity)&&entity==0&&!out.use&&!out.admitted,"mode3 empty Grip never admits a remote native gaze target");}
 // The saved close/assisted control remains meaningful outside mode3.
 {Fixture q;policy1(q);mh53::alchemyMode67=mh53::Gesture;q.in.pickupReachD39=0;reach53::setPickupReachD39(0);
  box1(q,19,0);palm1(q,0,.155);palm1(q,1,3);auto out=q.step();std::uint32_t entity=999;
  expect1(!selection53(q,out,entity),"existing Gesture close-bounds mode is unchanged");}
}
static void adapter53(){
 struct Pad{bool a=false,b=false,x=false;float lx=0,ly=0,rt=0,lt=0;};
 Fixture q;policy53(q);box1(q,19,0);palm1(q,0,.155);palm1(q,1,3);sw1::State state;sw1::Context c{};
 c.enabled=c.qualified=c.alchemyTracked=c.gestureAlchemy67=true;c.alchemy=1;c.lifetime=3;
 auto step=[&](bool manual=false){c.world67=q.in.world;c.worldPlayer67[0]=c.worldPlayer67[1]=9;
  b1::completed.publish(q.in.nativeFrame);b1::table.publish(q.in.nativeFrame.table);s::work.publish({});
  kcd2vr::alchemy_input19c4::clear();
  if(q.in.nativeSelectedKnown){kcd2vr::alchemy_input19c4::observed[q.in.nativeSelected.type].publish(q.in.nativeSelected);
   kcd2vr::alchemy_input19c4::selected.publish({1,3,q.in.now,q.in.nativeSelected.entity,q.in.nativeSelected.type});}
  Pad p;p.a=manual;state.apply(c,q.f,q.in.now,p);return p;};
 auto p=step();expect1(!p.a&&state.lastAlchemy67.nomination.decision.ref.slot==19,"public pad adapter selects book at15cm with savedCloseBounds under mode3");
 q.grip(0,1);refresh1(q);step();refresh1(q);q.selected(19);p=step();
 expect1(p.a&&state.lastAlchemy67.admitted,"public adapter sends one qualified native book-use request");
 refresh1(q,81);p=step();expect1(!p.a,"native use pulse does not repeat with Grip held");
 p=step(true);expect1(p.a&&state.lastAlchemy67.passenger,"manual native action path retains priority");
}
int main(){matrix53();recheck53();adapter53();
 std::printf("D53 crafting selection: %u checks, %u failures; actual production headers, explicit native leaves\n",total1,bad1);
 return bad1?1:0;
}
