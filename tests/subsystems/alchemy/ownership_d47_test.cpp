// Actual State::step; copied native observations are explicit leaves.
#define KCD2VR_INPUT67_NO_MAIN
#include "input67_test.cpp"
static void ingredient(int type){
 Fixture q;q.admit(type,0);q.advance();
 auto& table=q.in.nativeFrame.table;table.handTypesKnown68=true;table.handType68[0]=unsigned(type);
 q.in.action.parent=900;auto o=q.step();
 check(o.logicalIngredientHeldD47&&!o.ordinaryHeld&&!o.control.held[0]&&q.state.ordinaryCarried);
 q.advance(90);q.in.action.parent=0;q.in.nativeFrame.offers[6].point={.05,0,0};q.selected(6);o=q.step();
 check(o.admitted&&o.logicalIngredientHeldD47&&!o.use);
 q.advance();q.grip(0,0);o=q.step();
 check(o.ordinaryReturnRequested&&o.use&&o.passenger&&!o.control.held[0]);
 q.advance();q.in.action.parent=901;o=q.step();check(o.passenger&&!o.ordinaryReturnObserved);
 q.advance(90);q.in.action.parent=0;table.handType68[0]=21;o=q.step();
 check(o.ordinaryReturnObserved&&!o.admitted&&!o.use);
 q.advance();o=q.step();check(!o.use&&!o.ordinaryReturnRequested);
}
int main(){
 for(int type=7;type<=9;++type)ingredient(type);
 for(int fault=0;fault<4;++fault){
  Fixture q;q.admit(8,0);q.advance(90);auto& t=q.in.nativeFrame.table;
  t.handTypesKnown68=true;t.handType68[0]=8;
  if(fault==0)t.handTypesKnown68=false;if(fault==1)t.handType68[0]=7;
  if(fault==2)t.handType68[0]=21;if(fault==3){t.handType68[0]=21;t.handType68[1]=8;}
  auto o=q.step();check(!o.logicalIngredientHeldD47&&!q.state.ordinaryCarried);
  q.grip(0,0);q.selected(6);o=q.step();check(!o.use&&!o.ordinaryReturnRequested);
 }
 {Fixture q;q.admit(18,1);q.advance();q.in.action.parent=999;
  q.in.work={1,999,555,3,q.in.now,44,s::Bellows,0,2,false,false};
  auto o=q.step();check(o.control.parent==999&&o.control.clockAdmissionD47&&o.control.admissionTickR22);
  check(o.semiControl.clockStop&&o.semiControl.totalSeconds==0&&!o.control.held[0]&&!o.control.held[1]);
  q.advance();q.grip(1,0);o=q.step();check(!o.control.clockAdmissionD47&&o.semiControl.clockStop&&!o.use);
 }
 {Fixture q;q.in.nativeBellowsD47=true;const auto o=q.step();check(o.passenger&&o.semiControl.manualUse&&!o.admitted);}
 {Fixture q;q.admit(2,1);q.advance();q.grip(0,.8f);q.in.palm[0].point={0,0,0};q.in.world[0].point[0]=0;
  q.in.action.parent=999;q.in.work={1,999,555,3,q.in.now,44,s::Mortar,3,3,true,false};
  for(int h=0;h<2;++h){q.tool(h,h?2:3);auto& t=q.in.tool[h];t.parent=999;
   t.binding={1,2,888,std::uintptr_t(700+h),t.entity,h,t.type,999,0x3F60770,std::uintptr_t(600+h),3};}
  q.step();q.advance();q.f.hand[0].position[0]=.01f;auto o=q.step();check(o.semiControl.totalSeconds>0&&!o.semiControl.clockStop);
  const auto total=o.semiControl.totalSeconds;q.advance();q.f.hand[0].position[0]=.02f;
  q.in.pestlePresentationD47={q.in.tool[0].binding,q.in.now,4,5,6,true};o=q.step();
  check(o.semiControl.totalSeconds==total&&o.semiControl.clockStop&&!o.control.held[0]&&o.control.held[1]);
  q.advance();q.in.pestlePresentationD47.parked=false;o=q.step();check(o.semiControl.totalSeconds==total);
  q.advance();q.f.hand[0].position[0]=.03f;o=q.step();check(o.semiControl.totalSeconds>total&&!o.semiControl.clockStop);
 }
 // Parked presentation may suppress physical work, never fabricate a binding.
 {a::bridge::NativeTool t{};t.joined=true;t.side=0;t.type=3;t.poseEpoch=4;
  t.binding={1,2,888,777,103,0,3,999,0x3F60770,666,3};
  a::bridge::PestlePresentationD47 p{t.binding,1000,4,5,6,true};
  check(a::bridge::pestleParkedD47(p,t,1000,5,6));
  p.binding.object++;check(!a::bridge::pestleParkedD47(p,t,1000,5,6));
  p.binding=t.binding;p.tick=919;check(!a::bridge::pestleParkedD47(p,t,1000,5,6));
 }
 std::printf("D47 crafting ownership: %u checks PASS; native membership and input requests, recipe effects unobserved\n",checks);
}
