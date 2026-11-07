// Actual production input/gesture and smith RT producers. Native effects are
// not simulated as success; these checks stop at their documented input edge.
#define main retained_alchemy_input_regressions_d56
#include "subsystems/alchemy/input67_test.cpp"
#undef main
#include "../src/vrik/semi_smith46.h"

static void mortarPairD56(Fixture& q){
 q.admit(2,1);q.advance();q.in.handsOnlyD53=true;
 q.in.action.parent=999;q.in.work={1,999,555,3,q.in.now,44,s::Mortar,3,3,true,false};
 for(int h=0;h<2;++h){q.tool(h,h?2:3);q.in.tool[h].parent=999;
  q.in.tool[h].binding={1,2,888,std::uintptr_t(777+h),unsigned(100+(h?2:3)),h,h?2:3,999,0x3F60770,std::uintptr_t(666+h),3};
  q.in.palm[h].point={0,0,0};q.in.world[h].point[0]=0;
 }
 q.grip(0,0);q.grip(1,1);
}
static void mortarContractD56(){
 Fixture q;mortarPairD56(q);auto o=q.step();check(o.control.held[0]&&o.control.held[1]&&o.gesture.enabled);
 const auto initial=o.gesture.totalSeconds;
 for(int i=0;i<10;++i){q.advance();o=q.step();check(o.gesture.totalSeconds==initial&&o.gesture.clockStop);}
 q.advance();q.f.hand[0].position[0]=.02f;q.in.world[0].point[0]=.02f;o=q.step();
 check(o.gesture.totalSeconds>initial&&!o.gesture.clockStop&&!o.gesture.releaseRequested);
 const auto earned=o.gesture.totalSeconds;o=q.step();check(o.gesture.totalSeconds==earned);
 q.advance();q.grip(1,0);o=q.step();check(o.gesture.totalSeconds==earned&&o.gesture.clockStop&&!o.control.held[1]);
 for(int fault=0;fault<7;++fault){Fixture r;mortarPairD56(r);r.step();r.advance();r.f.hand[0].position[0]=.02f;
  switch(fault){case 0:r.in.tool[0].joined=false;break;case 1:r.in.tool[0].entity++;break;
   case 2:r.in.tool[0].binding.object++;break;case 3:r.in.tool[0].tick=r.in.now-81;break;
   case 4:r.in.tool[0].parent++;break;case 5:r.in.handsOnlyD53=false;break;case 6:r.f.hand[0].valid=false;break;}
  o=r.step();check(o.gesture.totalSeconds==0&&o.gesture.clockStop);
 }
}
static void retortContractD56(){
 check(kcd2vr::alchemy_tool18::nativeFixtureD56(4)&&!kcd2vr::alchemy_tool18::nativeFixtureD56(5));
 Fixture q;q.admit(4,0,true);q.tool(0,4);auto o=q.step();
 check(!o.use&&!o.control.held[0]&&!o.control.held[1]&&o.semiControl.manualUse);
 q.advance();q.in.nativeA=true;q.in.action.parent=700;
 auto& t=q.in.nativeFrame.table;t.actionKnown=true;t.action=700;t.nativeRetortD56=true;
 o=q.step();check(o.passenger&&!o.control.trackedPresentation71&&o.semiControl.manualUse&&!o.use);
 q.advance();q.in.nativeA=false;o=q.step();check(o.passenger&&!o.control.trackedPresentation71&&!o.use);
 q.advance();q.in.action.parent=0;t.action=0;t.nativeRetortD56=false;
 o=q.step();check(!o.passenger&&o.control.trackedPresentation71);
 // Slot5 keeps the carried native phial path.
 Fixture phial;phial.admit(5,0);phial.tool(0,5);o=phial.step();check(o.ordinaryHeld&&o.control.held[0]);
}
static void smithReversalD56(){
 namespace smith=kcd2vr::semi_smith46;
 smith::Controller c;smith::Context context{true,true,false,123,456,11,true};
 Frame f{};f.sequence=1;f.tick=1000;f.timeNs=1000000000;f.poseEpoch=4;f.sourceEpoch=5;f.allowed=f.headValid=true;
 f.head[1]=1.7f;for(auto& h:f.hand){h.valid=true;h.position[1]=1.f;}
 const auto step=[&](float y,unsigned ms=10){f.tick+=ms;f.timeNs+=std::uint64_t(ms)*1000000;++f.sequence;f.hand[0].position[1]=y;return c.step(context,f,f.tick,0,0,0);};
 c.step(context,f,f.tick,0,0,0);step(1);step(1);
 auto o=step(1.04f,20);check(o.requestRT&&o.strokeStarted&&!o.clock.enabled);
 context.phase=14;o=step(1.14f,30);check(o.requestRT&&!o.strokeReleased);
 o=step(1.14f);check(o.requestRT&&!o.strokeReleased); // hold = native charge duration
 o=step(1.13f);check(o.requestRT&&!o.strokeReleased); // sub-threshold tremor
 o=step(1.12f);check(!o.requestRT&&o.strokeReleased&&f.hand[0].position[1]>1.018f); // release before impact height
 o=c.step(context,f,f.tick,0,0,0);check(!o.requestRT&&!o.strokeReleased); // exactly one release edge
 context.phase=16;o=step(1.07f,20);check(!o.requestRT&&!o.clock.enabled);
 context.phase=17;o=step(1.0f,20);check(!o.requestRT&&!o.clock.enabled);
 context.phase=11;step(1);step(1);o=step(1.04f,20);check(o.strokeStarted);
 o=c.step(context,f,f.tick,0,0,.8f);check(!o.requestRT&&o.strokeReleased&&o.reason==smith::Reason::ManualStroke);
}
int main(){mortarContractD56();retortContractD56();smithReversalD56();
 std::printf("D56 crafting production contracts: %u checks PASS; native scoring/effects not inferred\n",checks);}
