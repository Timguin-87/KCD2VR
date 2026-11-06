// D43 sample221038->221042: measured hand/weapon/world poses. Radius is FITTED
// from the recorded prefix, not an independently captured size at this instant.
// Synthetic rays exercise the actual kernel and sweep; no headset-success claim.
#include "vrik/weapon_sweep112.h"
#include "vrik/world_turn_coverage_d44.h" // direct historical helper checks, no production dependency
#include "vrik/contact_budget.h"
#include <cstdio>
using namespace kcd2vr::physical;
namespace se=kcd2vr::sweep169r2;
unsigned checks=0,failed=0;void ck(bool b,const char*n){++checks;if(!b){++failed;printf("FAIL %s\n",n);}}
constexpr double radians(double d){return d*3.141592653589793/180.;}
struct Fixture{
 Profile p{};Input in{};State state{};
 Pose previousWeapon{*m::unit(Quat{-.1512001,.01180379,-.07752265,.985388}),{.1402384,.4160007,1.289218}};
 Pose previousHand{*m::unit(Quat{.1717462,.09476194,.5914431,.7821243}),{.09492579,.3584362,1.293758}};
 Pose desiredHand{*m::unit(Quat{.1818165,.09310453,.5872417,.7832122}),{.09593326,.3586,1.295328}};
 Pose observedWeapon{*m::unit(Quat{-.1460043,-.009863451,.08904465,.985219}),{-.0004227591,.4021177,1.290668}};
 Pose oldWorld{*m::unit(Quat{0,0,.7068586,.7073548}),{3186.596436,490.251801,41.057587}};
 Pose newWorld{*m::unit(Quat{0,0,.3091314,.9510193}),oldWorld.p};
 double radius=0;
 Fixture(){
  p.key=0x02000000000195A2ull;p.generation=2;p.entity=10;p.classId=9;p.side=1;p.primary=p.geometry=p.nativeSeed=true;
  p.grip[1]=compose(inverse(previousWeapon),previousHand);p.nativeWeapon=previousWeapon;
  const auto from=compose(inverse(newWorld),compose(oldWorld,previousWeapon));
  const double checked=m::angle(from.q,observedWeapon.q);radius=.004/(1-std::cos(checked/6));
  p.tip=p.grip[1].p+Vec3{0,0,radius};p.extent=radius*2.;
  in.allowed=in.contact=in.controllerAnchor119=in.haveWorld=true;
  in.tracked[1]=in.intentTracked[1]=true;in.hands[1]=in.intentHands[1]=previousHand;in.grip[1]=1;
  in.shoulders[1]={.00860,.15546,1.42656};in.reach[1]=reach_max(.26628,.26153);
  in.epoch=in.bodyEpoch=8;in.sample=221038;in.tick=1271361500;in.timeNs=1271361533576100ull;in.world=oldWorld;
 }
 void turn(){in.sample=221042;in.tick=1271361515;in.timeNs=1271361544593300ull;in.hands[1]=in.intentHands[1]=desiredHand;in.shoulders[1]={.00639,.15556,1.42689};in.world=newWorld;}
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
template<class Ray>Output run(Fixture&f,Ray&r){std::array<Profile,2> profiles{};profiles[1]=f.p;return step(f.state,f.in,profiles,[&](const Profile&p,Pose a,Pose b,bool acquire){return sweep_weapon112(p,a,b,acquire,r);});}
void prime(Fixture&f){Rays rays;auto o=run(f,rays);ck(o.owned[1]&&m::length(o.hands[1].p-f.previousHand.p)<1e-7,"existing bow owner primed at exact prior controller");}
int main(){
 Fixture f;prime(f);f.turn();Rays rays;auto out=run(f,rays);
 const double error=m::length(out.hands[1].p-f.desiredHand.p)*1000;
 printf("MEASURED_POSE_KERNEL fittedRadius=%.9f requestedTurnDeg=%.9f allowedFraction=%.9f handErrorMm=%.9f rays=%u\n",f.radius,m::angle(f.oldWorld.q,f.newWorld.q)*180/3.141592653589793,out.contact119[1].fraction,error,rays.called);
 ck(error<1e-4,"first root turn no longer creates122.844mm clear-prefix hand jump");
#ifdef D44_ROOT_COVERAGE
 ck(out.contactKnown[1]&&!out.sweepDeferred115[1]&&rays.called<=18&&out.contact119[1].coverage169r2.completedRays==rays.called,"full allowed path genuinely queried within bounded budget");
 ck(out.commandCount==0&&!out.impactRecord178r30[1].valid,"no invented attack or impact");
 ck(m::length(compose(out.weapons[1],f.p.grip[1]).p-out.hands[1].p)<1e-8,"unchanged rigid primary grip endpoint");
 auto calls=rays.called;auto repeat=run(f,rays);ck(rays.called==calls&&repeat.commandCount==0,"duplicate publication does not requery");
 for(int scenario=0;scenario<4;++scenario){Fixture blocked;prime(blocked);blocked.turn();Rays r(scenario==1?7:18);if(scenario==0)r.hitAt=10;if(scenario==2)r.unknownAt=1;if(scenario==3)r.cost=200;
  auto b=run(blocked,r);ck(m::length(b.hands[1].p-blocked.desiredHand.p)>.001,"obstruction unknown partial-budget or timeout cannot grant endpoint");
  if(scenario==0)ck(b.contact119[1].plane&&b.contact119[1].coverage169r2.reason==se::Reason::SurfaceHit,"real wall preserved");
  if(scenario==1)ck(r.called==7&&b.sweepDeferred115[1]&&!b.contactKnown[1],"partly spent budget not renewed");
  if(scenario==2)ck(!b.contact119[1].known&&b.contact119[1].fraction==0,"unknown remains unknown");
  if(scenario==3)ck(r.called==5&&b.contact119[1].coverage169r2.reason==se::Reason::TimeBudget,"existing time ceiling retained");
 }
 namespace wc=kcd2vr::world_turn_coverage_d44;
 ck(wc::scale(true,.8,radians(2),1)==1,"small root turn exact old policy");
 ck(wc::scale(false,.8,radians(90),1)==1,"non-bow unchanged");
 ck(wc::scale(true,.8,0,1)==1,"hand-only movement unchanged");
 ck(wc::scale(true,.8,radians(90),4)==4,"existing backlog coverage preserved");
 ck(wc::scale(true,std::nan(""),radians(90),1)==1,"invalid geometry cannot enable policy");
 Fixture small;prime(small);small.turn();small.in.world.q=m::multiply(small.oldWorld.q,m::from_rotation_vector({0,0,radians(2)}));Rays sr;auto so=run(small,sr);
 ck(g_sweep_coverage_scale178r6.load()==1&&so.contactKnown[1],"actual small-turn kernel retains original subdivision");
#endif
 printf("world_turn_coverage_d44 %u checks %u failures\n",checks,failed);return failed?1:0;
}
