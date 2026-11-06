// Regression for D52 log882357 ->885238: a right-pestle source becomes the
// native LEFT mortar / RIGHT pestle pair. This must not retire its own owner.
#define KCD2VR_INPUT67_NO_MAIN
#define main retainedProximityHandoffD55
#include "subsystems/alchemy/proximity_craft1_test.cpp"
#undef main
static unsigned total53=0,failed53=0;
static void expect53(bool ok,const char* name){++total53;if(!ok){++failed53;std::fprintf(stderr,"FAIL %u: %s\n",total53,name);}}
template<class T>static void hands53(T& in){if constexpr(requires(T v){v.handsOnlyD53;})in.handsOnlyD53=true;}
static void boundPair53(Fixture& q){
 q.grip(0,1);q.grip(1,1);q.tool(0,3);q.tool(1,2);
 for(int h=0;h<2;++h){auto& t=q.in.tool[h];t.parent=999;
  t.binding={1,2,888,std::uintptr_t(800+h),t.entity,h,t.type};
  t.binding.object=900+h;t.binding.lifetime=3;t.binding.action=999;t.binding.actionClass=0x3F60770;
  q.in.palm[h].point={0,0,0};q.in.world[h].point[0]=q.in.world[h].point[1]=q.in.world[h].point[2]=0;}
 q.in.action.parent=999;q.in.work={1,999,555,3,q.in.now,44,s::Mortar,3,3,true,false};
 for(int slot:{2,3}){auto& o=q.in.nativeFrame.offers[slot];o={{slot,unsigned(100+slot)},{},q.in.now,true};o.boundsValid=true;
  auto& b=o.bounds;b.world[0]=b.world[5]=b.world[10]=1;
  if(slot==2){b.local[0]=b.local[1]=-.08f;b.local[2]=-.04f;b.local[3]=b.local[4]=.08f;b.local[5]=.04f;}
  else {b.local[0]=b.local[1]=-.02f;b.local[3]=b.local[4]=.02f;b.local[5]=.20f;b.world[11]=.03f;}}
}
int main(){
 for(bool mode3:{false,true})for(int source:{2,3})for(int side:{0,1})for(int destination:{0,6}){
  Fixture q;policy1(q);q.in.proximityCraft1=true;kcd2vr::minigame_hands45::alchemyMode67=mode3?3:2;if(mode3)hands53(q.in);q.admit(source,side);q.advance();boundPair53(q);auto out=q.step();
  expect53(out.admitted&&q.state.mortarPair69&&out.control.held[0]&&out.control.held[1],"actual pair arrival preserves admission from either central source hand");
  q.advance(16);q.f.hand[0].rotation[2]=std::sin(.12f/2);q.f.hand[0].rotation[3]=std::cos(.12f/2);out=q.step();
  expect53(out.admitted&&out.gesture.totalSeconds>0&&out.gesture.status==g::Status::Grinding,"working exact pair still earns motion-owned native work after handoff");
  q.advance(81);q.in.palm[1].point={.95,0,0};q.in.palm[0].point={0,0,0};box1(q,destination,1.);q.selected(destination);out=q.step();
  expect53(out.nomination.decision.source==n::Source::Left&&out.nomination.decision.ref.slot==destination,"left carrier owns destination while original right source Grip remains closed");
  q.advance();q.grip(1,0);out=q.step();
  expect53(out.mortarStop69.serial&&!out.use&&out.semiControl.manualUse,"left release at native confirmed destination requests real native handoff once");
  if(mode3){publish1(q,out);std::uint32_t entity55=0;
   expect53(out.nomination.decision.source==n::Source::Left&&!out.nomination.decision.gripSelectionOwnedCraft1,
    "released paired mortar keeps the left destination as open hands-only ownership");
   expect53(b1::selectionNear68(q.in.nativeFrame.table,entity55,q.in.now)&&entity55==unsigned(100+destination),
    "actual native selection bridge accepts the open left mortar destination instead of restoring gaze");}

  const auto serial=out.mortarStop69.serial;q.advance();q.in.action.parent=0;q.in.work={};for(auto& t:q.in.tool){t.parent=0;t.binding.action=0;t.binding.actionClass=0;}
  q.in.mortarResult69={serial,q.in.now,1,3,kcd2vr::subsystems::alchemy::mortar69::Status::ReadyForNativePlace};out=q.step();
  expect53(out.use&&q.state.mortarReturning69,"only acknowledged native stop and retained pair permit contextual Place");
 }
 {Fixture q;q.in.proximityCraft1=true;hands53(q.in);q.admit(3,0);q.advance();boundPair53(q);q.step();q.advance();q.in.tool[0].binding.object++;auto out=q.step();
  expect53(!out.admitted&&!out.use&&!out.mortarStop69.serial,"a later replacement still retires the exact pair; no blanket reattachment acceptance");}
 std::printf("D53 mortar source-to-carrier handoff: %u checks, %u failures; native action/attachment observations are explicit leaves\n",total53,failed53);
 return failed53?1:0;
}
