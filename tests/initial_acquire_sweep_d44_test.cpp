// Actual production kernel + ray sweep. Uses the recorded D43 first-draw hand
// and learned grip; the initial native pose is a declared synthetic80deg gap,
// because the first seed itself was not included in that runtime receipt.
#include "vrik/weapon_sweep112.h"
#include "vrik/contact_budget.h"
#include <cstdio>
using namespace kcd2vr::physical;
namespace se=kcd2vr::sweep169r2;
unsigned checks=0,failed=0;void ck(bool b,const char*n){++checks;if(!b){++failed;printf("FAIL %s\n",n);}}
struct Fixture{
 Profile p{};Input in{};State state{};
 Pose requested{*m::unit(Quat{.5166928,.05994649,.5118814,-.6836758}),{-.1237072,.2190489,1.217}};
 Fixture(){
  const Pose allowedWeapon{*m::unit(Quat{.4631892,.4548711,-.2565432,-.7160542}),{.1608507,.08411254,.8697268}};
  const Pose allowedHand{*m::unit(Quat{.702813,-.3231546,.6331374,-.02759496}),{.1558596,.07671754,.9396257}};
  p.key=0x0200000000018DDBull;p.generation=1;p.entity=101851;p.classId=4;p.primary=p.geometry=p.nativeSeed=true;
  p.grip[0]=compose(inverse(allowedWeapon),allowedHand);p.tip=p.grip[0].p+Vec3{0,.8,0};
  const Pose desired=compose(requested,inverse(p.grip[0]));
  p.nativeWeapon.q=m::multiply(desired.q,m::from_rotation_vector({0,0,80*3.141592653589793/180}));
  p.nativeWeapon.p=Vec3{.22,.10,.95}-m::rotate(p.nativeWeapon.q,p.grip[0].p);
  in.allowed=in.contact=in.controllerAnchor119=in.constraintDrive129=in.physicalCombat177=true;
  in.tracked[0]=in.intentTracked[0]=true;in.grip[0]=1;in.requirePrimaryGrip114=true;
  in.shoulders[0]={.12219,.11650,1.37426};in.reach[0]=reach_max(.26627,.26153);
  in.hands[0]=in.intentHands[0]=requested;
  in.epoch=in.bodyEpoch=8;in.sample=205662;in.tick=1271318203;in.timeNs=1271318237261900ull;
 }
};
struct Rays{
 kcd2vr::contact::Budget budget;unsigned called=0,hitAt=0,unknownAt=0;std::uint64_t clock=0,cost=1;
 Rays(unsigned cap=18,std::uint64_t us=1000){budget.begin_work(cap,us);}
 Contact103 operator()(Vec3,Vec3,double){Contact103 c;
  if(!budget.take(clock)){c.known=false;c.fraction=0;c.coverage169r2.reason=budget.remaining?se::Reason::TimeBudget:se::Reason::RayCap;return c;}
  ++called;clock+=cost;budget.finish(clock);
  if(called==hitAt){c.fraction=.5;c.plane=true;c.normal={1,0,0};c.point={0,0,0};}
  if(called==unknownAt){c.known=false;c.fraction=0;c.coverage169r2.reason=se::Reason::QueryBusy;}
  return c;
 }
};
template<class Ray>Output run(Fixture&f,Ray&r){std::array<Profile,2> profiles{};profiles[0]=f.p;return step(f.state,f.in,profiles,[&](const Profile&p,Pose a,Pose b,bool acquire){kcd2vr::contact::AcquisitionBurstD45 burst(r.budget,p.initialAcquireSweepD44&&!acquire);return sweep_weapon112(p,a,b,acquire,r);});}
int main(){
 g_sweep_coverage_scale178r6=1;
 Fixture f;Rays rays;auto out=run(f,rays);
 printf("INITIAL_MELEE requested80deg gap allowedFraction=%.9f controllerErrorMm=%.9f rays=%u checkedDegrees=%.9f\n",out.contact119[0].fraction,m::length(out.hands[0].p-f.requested.p)*1000,rays.called,out.contact119[0].coverage169r2.checkedAngle*180/3.141592653589793);
 ck(out.owned[0]&&m::length(out.hands[0].p-f.requested.p)<1e-7,"clear first draw reaches controller through actual checked path");
 ck(out.contactKnown[0]&&!out.sweepDeferred115[0]&&out.contact119[0].fraction==1,"no untested prefix promoted as full clear");
 ck(rays.called<=72&&out.contact119[0].coverage169r2.completedRays==rays.called,"every admitted segment uses shared bounded rays");
 ck(!out.displayLatched[0]&&!out.displayRejoinR13[0]&&!out.handlingOnly55c1[0],"repair uses no unchecked display bypass");
 ck(m::length(compose(out.weapons[0],f.p.grip[0]).p-out.hands[0].p)<1e-8,"learned primary rigid grip is unchanged");
 ck(out.commandCount==0&&!out.impactRecord178r30[0].valid,"acquisition fabricates no hit or attack");
 const auto calls=rays.called;auto repeat=run(f,rays);ck(rays.called==calls&&repeat.commandCount==0,"same sample cannot run acquisition twice");
#ifdef D44_INITIAL_SWEEP
 ck(out.initialAcquireSweepD44[0]&&!out.nativeRejoinSweepR28[0],"first draw distinct from native-return receipt");
 ++f.in.sample;f.in.tick+=11;f.in.timeNs+=11000000;Rays later;auto next=run(f,later);
 ck(!next.initialAcquireSweepD44[0],"later owned motion does not inherit acquisition flag");
 for(int scenario=0;scenario<5;++scenario){Fixture blocked;Rays r(scenario==1?7:scenario==4?0:18);if(scenario==1)r.budget.acquisitionBurstUsedD45=true;if(scenario==0)r.hitAt=12;if(scenario==2)r.unknownAt=8;if(scenario==3)r.cost=200;
  auto b=run(blocked,r);
  ck(!b.contactKnown[0]||b.contact119[0].plane,"wall unknown cap or timeout never becomes clear");
  ck(b.commandCount==0&&!b.impactRecord178r30[0].valid,"failed acquisition creates no attack or contact hit");
  if(scenario==0)ck(m::length(b.hands[0].p-blocked.requested.p)>.001,"real observed wall still constrains the acquired grip");
  else {
   ck(b.displayRejoinR13[0]&&b.handlingOnly55c1[0]&&m::length(b.displayHands[0].p-blocked.requested.p)<1e-7,"budget or unknown acquisition never drags the visible wrist toward native animation");
   ck(m::length(compose(b.weapons[0],blocked.p.grip[0]).p-blocked.requested.p)>.001,"collision seed retains only its actually checked prefix");
   ck(!attack_authority55c1(blocked.in,b,0),"presentation-only acquisition cannot grant native combat authority");
  }
  // Independent radius subdivisions may need other features after the ray12
  // hit before that hit prefix has common coverage; never exceed original cap.
  if(scenario==0)ck(b.contact119[0].plane&&b.contact119[0].coverage169r2.reason==se::Reason::SurfaceHit&&r.called>=12&&r.called<=72,"real wall in extra checked tail constrains the weapon after all features cover its prefix");
  if(scenario==1)ck(r.called==7&&b.sweepDeferred115[0]&&b.contact119[0].coverage169r2.completed==2,"shared partial ray budget never replenished");
  if(scenario==2)ck(b.sweepDeferred115[0]&&b.contact119[0].coverage169r2.completed==2,"unknown feature grants only complete previous segments");
  if(scenario==3)ck(r.called==5&&b.contact119[0].coverage169r2.reason==se::Reason::TimeBudget,"existing1000us time ceiling remains effective");
  if(scenario==4)ck(r.called==0&&!b.contactKnown[0]&&!b.impactRecord178r30[0].valid,"zero ray budget grants no physical contact authority");
 }
 for(int bad=0;bad<10;++bad){Fixture x;switch(bad){case 0:x.p.entity=0;break;case 1:x.p.generation=0;break;case 2:x.p.nativeSeed=false;break;case 3:x.in.intentTracked[0]=false;break;case 4:x.in.controllerAnchor119=false;break;case 5:x.in.contactFaultH30=true;break;case 6:x.in.allowed=false;break;case 7:x.in.tracked[0]=false;break;case 8:x.p.classId=8;break;case 9:x.in.grip[0]=0;break;}Rays r;auto a=run(x,r);ck(!a.initialAcquireSweepD44[0],"missing identity native seed tracking context or main-grip cannot request added coverage");}
 Fixture native;native.in.nativeRejoinR28[0]=true;Rays n;auto nr=run(native,n);ck(nr.nativeRejoinSweepR28[0]&&!nr.initialAcquireSweepD44[0]&&nr.contactKnown[0],"qualified R28 native return retains distinct original route");
 Fixture noItem;noItem.p.key=0;Rays empty;auto e=run(noItem,empty);ck(!e.initialAcquireSweepD44[0]&&!e.owned[0]&&empty.called==0,"free hands unaffected");
 Fixture support;support.p.initialAcquireSweepD44=true;support.p.grip[1].p=support.p.grip[0].p+Vec3{0,.3,0};const auto target=compose(support.requested,inverse(support.p.grip[0]));Rays a,b;
 auto extra=sweep_weapon112(support.p,support.p.nativeWeapon,target,true,a);support.p.initialAcquireSweepD44=false;auto original=sweep_weapon112(support.p,support.p.nativeWeapon,target,true,b);
 ck(extra.fraction==original.fraction&&a.called==b.called,"support acquisition ray subdivision unchanged");
#endif
 printf("initial_acquire_sweep_d44 %u/%u passed\n",checks-failed,checks);return failed?1:0;
}
