// Real input recognizer and immutable request admission; native property calls,
// dt heat integration and the headset are explicitly outside this host fixture.
#define main existing_bellows_d51_tests
#include "bellows_pull_d51_test.cpp"
#undef main
#include "../src/subsystems/smithing/instant_native_d72.h"
namespace inst72=kcd2vr::subsystems::smithing::instant_d72;
static inst72::Pump request72(const Forge51& q,bool pressed){
 inst72::Pump p{};p.handle=q.in.frame.source[2];p.handle.model=99;
 p.lifetime=q.in.lifetime;p.poseEpoch=q.f.poseEpoch;p.sourceEpoch=q.f.sourceEpoch;
 p.modeEpoch=q.in.modeEpoch;p.sample=q.f.sequence;p.tick=q.in.now;
 p.admitted=q.trace.current&&q.trace.sampled&&q.trace.acquired;p.pressed=pressed;return p;
}
int main(){
 if(existing_bellows_d51_tests())return 1;
 Forge51 q;q.acquire();auto p=request72(q,q.move(.8f,-.04f));
 expect51(p.pressed&&inst72::current(p,1,2,3,q.in.now),"actual qualified downstroke admits native semantic request");
 for(int field=0;field<16;++field){auto wrong=p;
  switch(field){case 0:wrong.admitted=false;break;case 1:wrong.manual=true;break;
  case 2:++wrong.handle.instance;break;case 3:++wrong.lifetime;break;case 4:++wrong.modeEpoch;break;
  case 5:wrong.poseEpoch=0;break;case 6:wrong.sourceEpoch=0;break;case 7:wrong.sample=0;break;
  case 8:wrong.tick=0;break;case 9:wrong.tick=q.in.now-81;break;case 10:wrong.tick=q.in.now+1;break;
  case 11:wrong.handle.slot=0;break;case 12:wrong.handle.phase=11;break;case 13:wrong.handle.geometryKnown=false;break;
  case 14:wrong.handle.geometryType=pump51::Geometry::Bounds;break;case 15:++wrong.handle.geometryEntity;break;}
  expect51(!inst72::current(wrong,1,2,3,q.in.now),"changed identity/tracking/source cannot borrow bellows admission");
 }
 p=request72(q,q.move(.8f,-.04f,0,81));
 expect51(!p.pressed,"stationary lowered hand does not retain pumping after motion expiry");
 p=request72(q,q.move(.8f,-.08f,.8f));p.manual=true;
 expect51(!p.pressed&&!inst72::current(p,1,2,3,q.in.now),"real trigger takes native ownership without synthetic pump");
 kcd2vr::instant_d72::activities.store(true);kcd2vr::instant_d72::crafting.store(true);
 inst72::ready.store(true);kcd2vr::minigame_hands45::setMode(kcd2vr::minigame_hands45::Smithing,kcd2vr::minigame_hands45::SixDofVrik);
 expect51(inst72::route(),"ready requested FREE smith routes physical pump to semantic owner");
 inst72::ready.store(false);expect51(!inst72::route(),"missing hook preserves native RT fallback");
 std::printf("smith_instant_bellows_d72: %u checks PASS; native effects unobserved\n",checks51);return 0;
}
