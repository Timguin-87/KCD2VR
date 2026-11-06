#include "../src/vrik/elbow_continuity.h"
#include <cstdio>
#include <limits>
using namespace kcd2vr::pose_core;
int total=0,pass=0;void Check(bool b,const char*n){++total;pass+=b;std::printf("%s %s\n",b?"PASS":"FAIL",n);}
int main(){
 ElbowContinuity s{};Vec3 axis{1,0,0},anatomy{.2,-.3,-1},down{.35,-.25,-1};
 auto b=continuous_elbow(s,axis,anatomy,down,1,1,1,1000);
 Check(b&&s.updates==1&&dot(*b,down)>0,"initial pole remains in calibrated side/down half-space");
 auto old=s;for(int i=0;i<100;++i)continuous_elbow(s,axis,anatomy,down,1,1,1,1000);
 Check(s.updates==old.updates&&length(s.bend-old.bend)<1e-12,"100 repeated eye callbacks do not integrate continuity");
 auto anti=continuous_elbow(s,axis,anatomy*(-1),down,1,1,2,1010);
 Check(anti&&dot(*anti,old.bend)>std::cos(.041),"opposite anatomical destination changes only by bounded temporal turn");
 Check(!continuous_elbow(s,axis,anatomy,down,1,1,1,1000)&&s.sample==2,"out-of-order sample cannot roll continuity back");
 Check(!continuous_elbow(s,{0,0,0},anatomy,down,1,1,3,1020),"zero reach direction is rejected without mutating state");
 s={};auto singular=continuous_elbow(s,axis,axis,down,1,1,1,1000);
 Check(singular&&std::abs(dot(*singular,axis))<1e-9,"pole parallel to full reach uses secondary anatomical preference");
 bool lengths=true,stable=true,valid=true;Vec3 last{};s={};
 for(int k=0;k<1200;++k){double angle=-1.3+2.6*k/1199.0;Vec3 a{std::cos(angle),std::sin(angle),.1*std::sin(3*angle)};a=*unit(a);
   double reach=.85+.15*std::cos(k*.02);auto p=continuous_elbow(s,a,anatomy,down,reach,1,k+1,1000+k*11);
   auto arm=p?solve_arm({},a*(.5278*reach),.2663,.2615,*p,*p):std::optional<ArmSolution>{};
   valid=valid&&bool(arm);if(!arm)continue;
   lengths=lengths&&std::abs(length(arm->elbow)-.2663)<1e-9&&std::abs(length(arm->hand-arm->elbow)-.2615)<1e-9;
   if(k)stable=stable&&dot(arm->bend,last)>.99;last=arm->bend;
 }
 Check(valid&&lengths,"1200 side/reach samples preserve both exact arm lengths including 2-degree reserve");
 Check(stable,"side arc through 70-100 percent extension has no bend-plane flip");
 auto run=[](int hz){ElbowContinuity c{};continuous_elbow(c,{1,0,0},{0,0,-1},{0,0,-1},.8,1,1,1000);
   for(int k=1;k<=hz;++k)continuous_elbow(c,{1,0,0},{0,-1,-.3},{0,-1,-1},.8,1,k+1,1000+(1000*k)/hz);return c.bend;};
 Check(length(run(60)-run(120))<.003,"time-aware pole filter gives consistent one-second 60/120Hz convergence");
 // A bone aimed in the right direction can still have the wrong hinge roll.
 const Vec3 oldAxis{1,0,0},oldNormal{0,0,1},newAxis=*unit(Vec3{.4,.7,-.2});
 const Vec3 newNormal=*unit(cross(newAxis,Vec3{0,0,-1}));
 const auto oq=orient_upper_arm({},oldAxis,oldNormal,newAxis,newNormal);
 Check(oq&&length(rotate(*oq,oldAxis)-newAxis)<1e-9&&length(rotate(*oq,oldNormal)-newNormal)<1e-9,
       "upper-arm orientation satisfies both aim and elbow hinge plane");
 bool rollSmooth=true;Quat lastQ{};
 for(int k=0;k<721;++k){const double a=-2.5+5.0*k/720;const Vec3 x=*unit(Vec3{std::cos(a),std::sin(a),-.1});
   const Vec3 n=*unit(cross(x,Vec3{0,0,-1}));auto q=orient_upper_arm({},oldAxis,oldNormal,x,n);
   rollSmooth=rollSmooth&&bool(q);if(q){if(k)rollSmooth=rollSmooth&&angle(*q,lastQ)<.03;lastQ=*q;}}
 Check(rollSmooth,"upper-arm roll stays continuous throughout wide horizontal reach arc");
 ReachAssist a{};Check(stable_reach_excess(a,.003,1,1)==0&&!a.active,"3mm shoulder jitter stays in dead zone");
 Check(stable_reach_excess(a,.005,1,2)==.005&&a.active,"5mm excess activates existing reach accommodation");
 Check(stable_reach_excess(a,.002,1,3)==.002&&a.active,"hysteresis retains accommodation below activation threshold");
 Check(stable_reach_excess(a,0,1,4)==0&&!a.active,"retraction releases accommodation");
 Check(stable_reach_excess(a,.5,1,4)==0,"duplicate tracking sample does not re-evaluate shoulder state");
 // V66.98 regression: an infinitesimal target-plane change crossed the
 // sign test, then the pre-filter 80-degree cone snapped the committed elbow.
 s={};auto p0=continuous_elbow(s,{1,0,0},{0,1,-.001},{0,0,-1},.95,1,1,1000);
 auto p1=continuous_elbow(s,{1,0,0},{0,1,.001},{0,0,-1},.95,1,2,1010);
 const double jump=p0&&p1?std::acos(std::clamp(dot(*p0,*p1),-1.0,1.0)):10;
 std::printf("hemisphere_crossing_jump_degrees=%.6f\n",jump*180/3.14159265358979323846);
 Check(p0&&p1&&jump<=.041,"hemisphere crossing is time-limited, not an instantaneous cone snap");
 s={};p0=continuous_elbow(s,{1,0,0},{0,0,-1},{0,0,-1},.95,1,1,1000);const auto before=s;
 p1=continuous_elbow(s,{1,0,0},{0,-1,0},{0,-1,0},.95,1,1,1000);
 Check(p0&&p1&&length(*p1-*p0)<1e-10&&s.updates==before.updates,
       "duplicate sample cannot reclamp an already committed elbow to a new anchor");
 p1=continuous_elbow(s,{1,0,0},{0,-1,0},{0,-1,0},.95,1,2,1125);
 const double stallJump=p0&&p1?std::acos(std::clamp(dot(*p0,*p1),-1.0,1.0)):10;
 Check(p0&&p1&&stallJump<=.201,
       "125ms gap preserves geometric elbow history without extending pose or weapon freshness");
 p1=continuous_elbow(s,{1,0,0},{0,-1,0},{0,-1,0},.95,2,1,1126);
 Check(p1&&s.epoch==2&&length(*p1-Vec3{0,-1,0})<1e-10,
       "origin epoch change still replaces geometric history immediately");
 // Anatomical target is independent of transported history in the low-arm cone.
 {ElbowContinuity a{},b{};a.ready=b.ready=true;a.epoch=b.epoch=3;a.sample=b.sample=1;a.tick=b.tick=1000;
  a.axis=b.axis={0,0,-1};a.bend={1,0,0};b.bend={-1,0,0};
  const auto x=condition_elbow_preference(a,{0,0,-1},{0,1,0},{.01,0,-1},{0,-1,0},3,2,1010);
  const auto y=condition_elbow_preference(b,{0,0,-1},{0,1,0},{.01,0,-1},{0,-1,0},3,2,1010);
  Check(x&&y&&length(x->anatomical-y->anatomical)<1e-12&&length(x->fallback-y->fallback)<1e-12,
        "opposite elbow histories cannot change the anatomical destination");
  Check(x&&dot(x->anatomical,Vec3{0,-1,0})>0,"low-arm forward pole resolves down/out/back");
 }

 // The torso-clearance destination must not jump at its anatomical60deg limit.
 // This isolates the real signed target, before temporal rate limiting can hide it.
 {const Vec3 axis{0,1,0},raw=*unit(Vec3{.35,-.25,-1}),back{0,-1,0};
  const auto fallback=*unit(perpendicular(raw+back*.6,axis));ElbowContinuity e{};
  auto condition=[&](double degrees){const auto p=rotate(from_rotation_vector(axis*(degrees*3.14159265358979323846/180)),fallback);
   return condition_elbow_preference(e,axis,p,raw,back,3,1,1000);};
  const auto before=condition(59.999),after=condition(60.001);
  Check(before&&after&&angle(from_to(before->anatomical,after->anatomical),Quat{})<.0001,
        "0.002deg torso threshold crossing cannot reverse the target60degrees");
  bool continuous=true;Vec3 prior{};
  for(int k=0;k<=6000;++k){const auto q=condition(35.+30.*k/6000.);continuous=continuous&&bool(q);
   if(q){if(k)continuous=continuous&&std::acos(std::clamp(dot(prior,*unit(q->anatomical)),-1.,1.))<.005;prior=*unit(q->anatomical);}}
  Check(continuous,"continuous target sweep through torso acceptance band has no binary switch");
  const auto close=condition(20.);const auto expected=rotate(from_rotation_vector(axis*(20.*3.14159265358979323846/180)),fallback);
  Check(close&&length(close->anatomical-expected)<1e-12&&length(close->fallback-fallback)<1e-12,
        "nearby well-conditioned torso destination and original anatomical fallback stay exact");
  auto a=e,b=e;a.ready=b.ready=true;a.epoch=b.epoch=3;a.sample=b.sample=1;a.tick=b.tick=990;a.axis=b.axis=axis;
  a.bend={1,0,0};b.bend={-1,0,0};const auto p=rotate(from_rotation_vector(axis*(50.*3.14159265358979323846/180)),fallback);
  const auto c=condition_elbow_preference(a,axis,p,raw,back,3,2,1000),d=condition_elbow_preference(b,axis,p,raw,back,3,2,1000);
  Check(c&&d&&length(c->anatomical-d->anatomical)<1e-12&&length(c->fallback-d->fallback)<1e-12,
        "smooth anatomical blend never uses opposite committed histories as its destination");
 }
 std::printf("elbow_continuity=%d/%d\n",pass,total);return pass!=total;
}
