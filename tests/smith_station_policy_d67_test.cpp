#define main priorBellowsMain
#include "bellows_pull_d51_test.cpp"
#undef main
#include "../src/subsystems/smithing/bridge67.h"
int main(){
 using namespace kcd2vr;kcd2vr::crafting_d67::setSmithRecovery(true);
 {Forge51 q;expect51(!q.move(.8f,0),"closed entry rebases source");expect51(!q.move(.8f,0)&&q.trace.acquired,"current closed Grip may acquire handle");expect51(q.move(.8f,-.04f),"only fresh relative downstroke pumps");expect51(!q.move(0,-.04f),"release stops");}
 {Forge51 q;q.move(.8f,0);q.move(.8f,0);q.f.head[1]=-.04f;expect51(!q.move(.8f,-.04f),"head/body translation cannot pump");}
 {Forge51 q;q.move(.8f,0);q.in.frame.source[2].entity++;q.in.frame.source[2].geometryEntity=q.in.frame.source[2].entity;expect51(!q.move(.8f,-.04f),"new source never reuses previous travel");expect51(!q.move(.8f,-.04f)&&q.trace.acquired,"new exact source gets only baseline");}
 {Forge51 q;crafting_d67::setSmithRecovery(false);q.move(.8f,0);expect51(!q.move(.8f,0)&&!q.trace.acquired,"optional old policy retained");crafting_d67::setSmithRecovery(true);}
 namespace sb=kcd2vr::subsystems::smithing::bridge67;smith_tool36::Binding b{};b.instance=1;b.holder=2;b.entity=3;b.side=1;b.tool=1;b.phase=11;b.lifetime=4;const double p[3]{0,1,0};
 minigame_hands45::setMode(minigame_hands45::Smithing,3);crafting_d67::setSmithSemiAnchors(false);expect51(!sb::fixtureStep({},b,1,2,3,4,5,.5,p).valid,"default mode3 remains tracked");crafting_d67::setSmithSemiAnchors(true);
 for(int phase:{11,14,16,17}){b.phase=phase;auto a=sb::fixtureStep({},b,1,2,3,4,5,.5,p);expect51(!a.valid&&!a.engaged&&a.weight==0,"mode3 semi uses workpiece surface support without any native wrist takeover");expect51(!sb::fixtureJoined(a,b,1,3,4,5),"surface support cannot publish a native wrist-pose lease");}
 b.phase=8;expect51(!sb::fixtureStep({},b,1,2,3,4,5,.5,p).valid,"forge phase never anchors left hand");b.phase=11;b.side=0;b.tool=0;expect51(!sb::fixtureStep({},b,1,2,3,4,5,.5,p).valid,"right hammer never anchored");
 std::printf("D67 station policy %u checks PASS\n",checks51);
}
