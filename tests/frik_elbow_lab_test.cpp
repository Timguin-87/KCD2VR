// R8c FRIK elbow lab: pure rules (src/vrik/frik_elbow_lab.h) + the lab torso clearance branch.
#include "vrik/arm_constraints.h"
#include <cstdio>
using namespace kcd2vr::pose_core;
namespace fl=kcd2vr::frik_elbow_lab;
int total=0,pass=0;void Check(bool b,const char*n){++total;pass+=b;std::printf("%s %s\n",b?"PASS":"FAIL",n);}
static double deg(double r){return r*180/3.141592653589793;}
static double flexion(const ArmSolution& a,Vec3 shoulder){ // 0 = straight
    const auto u=unit(a.elbow-shoulder),f=unit(a.hand-a.elbow);return deg(std::acos(std::clamp(dot(*u,*f),-1.0,1.0)));}
int main(){
 const double U=.2663,F=.2615;const Vec3 S{0,0,0},pole{0,-.4,-1};
 // 1 min/max flexion with fixed lengths
 bool lengths=true,minOk=true,maxOk=true,solved=true;double minSeen=1e9,maxSeen=0;
 for(int k=0;k<=2000;++k){
   const double d=.001+.8*k/2000.0;const auto c=fl::clamp_triangle(d,U,F);
   auto a=solve_arm(S,Vec3{d,0,0}*(c.distance/d),U,F,pole,pole,c.reserve);
   if(!a){solved=false;continue;}
   lengths=lengths&&std::abs(length(a->elbow-S)-U)<1e-9&&std::abs(length(a->hand-a->elbow)-F)<1e-9;
   const double fx=flexion(*a,S);minSeen=std::min(minSeen,fx);maxSeen=std::max(maxSeen,fx);
   minOk=minOk&&fx>=10.0-1e-6;maxOk=maxOk&&fx<=145.0+1e-6;
 }
 std::printf("  flexion range seen %.3f..%.3f deg\n",minSeen,maxSeen);
 Check(solved&&lengths,"2001 distances 1 mm..80 cm solve with both exact bone lengths (no stretch, no shortening)");
 Check(minOk&&minSeen<10.5,"never straighter than 10 deg flexion; full reach sits at ~10 deg");
 Check(maxOk&&maxSeen>144.5,"never more than 145 deg flexion; at the shoulder it sits at ~145 deg");
 {const auto far=fl::clamp_triangle(2.0,U,F);Check(far.limited&&std::abs(far.distance-fl::reach_at_flexion(U,F,10))<1e-9,
   "far controller: solve distance = reach(10 deg), shortfall stays the reach cap");
  const auto near=fl::clamp_triangle(.01,U,F);Check(near.limited&&std::abs(near.distance-fl::reach_at_flexion(U,F,145))<1e-9,
   "controller at the shoulder: solve distance pushed OUT to reach(145 deg)");
  const auto mid=fl::clamp_triangle(.35,U,F);Check(!mid.limited&&mid.distance==.35,"mid range distance untouched");
  Check(std::abs(far.reserve-(U+F-fl::reach_at_flexion(U,F,10)))<1e-12&&far.reserve<.003,"min-flex reserve < 3 mm for a 27/26 cm arm");}
 // 2 blend continuity: no jump across the switch points (distance clamp, window, level weight, smooth max/min)
 {double worst=0,prev=fl::clamp_triangle(.0,U,F).distance;
  for(int k=1;k<=200000;++k){const double d=.6*k/200000.0;const double x=fl::clamp_triangle(d,U,F).distance;worst=std::max(worst,std::abs(x-prev));prev=x;}
  Check(worst<=.6/200000.0*1.0000001,"clamped distance is continuous and never moves faster than the controller (Lipschitz 1)");
  double slopeJump=0;const double rMax=fl::reach_at_flexion(U,F,145),rMin=fl::reach_at_flexion(U,F,10);
  for(double p:{rMax-.01,rMax+.01,rMin-.01,rMin+.01,rMax,rMin}){const double h=1e-6;
    const double s0=(fl::clamp_triangle(p,U,F).distance-fl::clamp_triangle(p-h,U,F).distance)/h,s1=(fl::clamp_triangle(p+h,U,F).distance-fl::clamp_triangle(p,U,F).distance)/h;
    slopeJump=std::max(slopeJump,std::abs(s1-s0));}
  Check(slopeJump<1e-3,"C1 at both band edges and at the 145/10 deg switch points (no velocity step)");
  double wj=0,lj=0;double pw=fl::window_half_deg(-.5),pl=fl::level_weight(-.5);
  for(int k=1;k<=100000;++k){const double h=-.5+.6*k/100000.0;const double w=fl::window_half_deg(h),l=fl::level_weight(h);wj=std::max(wj,std::abs(w-pw));lj=std::max(lj,std::abs(l-pl));pw=w;pl=l;}
  std::printf("  window step %.6f level step %.6f\n",wj,lj);Check(wj<.01&&lj<.001,"hanging window (30<->60 deg) and level-reach weight change continuously with hand height");
  bool floor15=true;for(int k=0;k<=1000;++k)floor15=floor15&&fl::window_half_deg(-2+2.2*k/1000)>=15.0;
  Check(floor15,"swivel window never closes below 15 deg");
  double mj=0;for(double b:{0.0,.3,1.0}){double pv=fl::smooth_max(-1,b,.1);for(int k=1;k<=20000;++k){const double a=-1+2.0*k/20000;const double v=fl::smooth_max(a,b,.1);mj=std::max(mj,std::abs(v-pv));pv=v;}}
  Check(mj<=2.0/20000*1.0000001&&fl::smooth_max(.3,.3,.1)==.3+.025&&fl::smooth_min(.3,.3,.1)==.3-.025,"smoothMax/smoothMin match FRIK (k/4 bump, Lipschitz 1)");}
 // 3 one, two or three solves per sample = the same result (duplicate samples never advance)
 {Vec3 r[3];bool same=true;std::uint64_t upd[3]{};
  for(int n=1;n<=3;++n){ElbowContinuity c{};fl::continuous_elbow_lab(c,{1,0,0},{0,0,-1},{0,0,-1},.8,1,1,1000,1000000000ull);
    Vec3 last{};for(int k=1;k<=90;++k){const auto t=1000000000ull+std::uint64_t(k)*11111111ull;
      for(int rep=0;rep<n;++rep){auto p=fl::continuous_elbow_lab(c,{1,0,.02*k},{0,-1,-.3},{0,-1,-1},.8,1,k+1,1000+k*11,t);
        if(!p)same=false;else if(rep&&length(*p-last)>1e-12)same=false;else last=*p;}}
    r[n-1]=c.bend;upd[n-1]=c.updates;}
  Check(same&&length(r[0]-r[1])<1e-12&&length(r[0]-r[2])<1e-12&&upd[0]==91&&upd[1]==91&&upd[2]==91,"1/2/3 solves per frame give the identical pole");}
 // 4 45/90/180 Hz independence (retention .75 per 90 Hz frame)
 {auto run=[](int hz){ElbowContinuity c{};fl::continuous_elbow_lab(c,{1,0,0},{0,0,-1},{0,0,-1},.8,1,1,1000,1000000000ull);
    for(int k=1;k<=hz/5;++k)fl::continuous_elbow_lab(c,{1,0,0},{0,-1,0},{0,-1,-1},.8,1,k+1,1000+(200*k)/hz,1000000000ull+std::uint64_t(200000000.0*k/hz+.5));
    return c.bend;};
  // the same 90 Hz count of frames toward a 90 deg-away target: angle left = 90*.75^(t*90)
  auto ang=[](Vec3 b){return deg(std::acos(std::clamp(dot(b,Vec3{0,-1,0}),-1.0,1.0)));};
  const double a45=ang(run(45)),a90=ang(run(90)),a180=ang(run(180));
  std::printf("  remaining after 0.2 s: 45Hz %.6f  90Hz %.6f  180Hz %.6f deg\n",a45,a90,a180);
  Check(std::abs(a45-a90)<1e-6&&std::abs(a180-a90)<1e-6,"45/90/180 Hz end in the same pole after 0.2 s");
  ElbowContinuity c{};fl::continuous_elbow_lab(c,{1,0,0},{0,0,-1},{0,0,-1},.8,1,1,1000,1000000000ull);
  fl::continuous_elbow_lab(c,{1,0,0},{0,-1,0},{0,-1,-1},.8,1,2,1011,1000000000ull+11111111ull);
  Check(std::abs(ang(c.bend)-90*.75)<1e-6,"one 90 Hz frame keeps exactly 75 % of the swivel error");
  ElbowContinuity h{};fl::continuous_elbow_lab(h,{1,0,0},{0,0,-1},{0,0,-1},.8,1,1,1000,1000000000ull);
  fl::continuous_elbow_lab(h,{1,0,0},{0,-1,0},{0,-1,-1},.8,1,2,1300,1300000000ull);
  Check(std::abs(ang(h.bend)-90*std::pow(.75,3))<1e-6,"a 300 ms hitch moves at most 3 frames worth");
  Check(std::abs(fl::move_fraction(.75,1.0/90)-.25)<1e-12&&fl::move_fraction(.75,0)==0&&fl::move_fraction(.75,-1)==0,"move fraction: .25 per 90 Hz frame, 0 for no time");
  ElbowContinuity e{};fl::continuous_elbow_lab(e,{1,0,0},{0,0,-1},{0,0,-1},.8,1,1,1000,1000000000ull);
  auto re=fl::continuous_elbow_lab(e,{1,0,0},{0,-1,0},{0,-1,-1},.8,2,2,1011,1000000000ull+11111111ull);
  Check(re&&ang(*re)<1e-6,"new epoch (skeleton/owner/load) resets: no history, pole = anchor");}
 // 5 the 180 deg aim case
 {ElbowContinuity c{};c.ready=true;c.axis={1,0,0};c.bend={0,0,-1};
  auto t=fl::transported_bend_stable(c,{-1,0,0});
  Check(t&&length(*t-Vec3{0,0,-1})<1e-9,"arm axis reversal transports the bend about itself (stays down)");
  auto q=fl::from_to_stable({1,0,0},{-1,0,0},{0,.3,-1});const auto v=rotate(q,Vec3{1,0,0});
  Check(length(v-Vec3{-1,0,0})<1e-9&&length(rotate(q,*unit(Vec3{0,.3,-1}))-*unit(Vec3{0,.3,-1}))<1e-9,"from_to_stable 180 deg: maps a->b and keeps the hint axis");
  auto q2=fl::from_to_stable({1,0,0},{0,1,0},{0,0,1});Check(angle(q2,from_to({1,0,0},{0,1,0}))<1e-12,"non-opposite case is exactly from_to");
  auto o=fl::orient_upper_arm_lab(Quat{0,0,0,1},{1,0,0},{0,0,1},{-1,0,0},{0,0,1});
  Check(o&&length(rotate(*o,Vec3{1,0,0})-Vec3{-1,0,0})<1e-9&&length(rotate(*o,Vec3{0,0,1})-Vec3{0,0,1})<1e-9,"upper arm 180 deg aim keeps its hinge normal (no roll pop)");}
 // lab switch + torso branch
 {Check(!fl::Active(),"lab is OFF by default (R8b elbows)");
  const Vec3 sh{.18,0,1.4},hand{.05,.12,1.12},torsoA{0,0,1.0},torsoB{0,0,1.45};
  const auto off=torso_elbow_preference(sh,hand,U,F,{.2,-.45,-1},torsoA,torsoB,.14);
  fl::rung2.store(true);Check(!fl::Active(),"retained F7 rung cannot select a gameplay solver");
  const auto captureOnly=torso_elbow_preference(sh,hand,U,F,{.2,-.45,-1},torsoA,torsoB,.14);
  Check(length(captureOnly-off)==0,"capture rung leaves current production torso policy unchanged");
  fl::enabled.store(true);Check(fl::Active(),"live experiment preference selects FRIK trial");
  const auto on=torso_elbow_preference(sh,hand,U,F,{.2,-.45,-1},torsoA,torsoB,.14);
  fl::rung2.store(false);fl::enabled.store(false);const auto off2=torso_elbow_preference(sh,hand,U,F,{.2,-.45,-1},torsoA,torsoB,.14);
  Check(finite(on)&&std::abs(length(on)-1)<1e-9,"lab torso clearance returns a unit bend");
  Check(length(off-off2)==0,"rung 0/1 torso clearance is bit-identical before/after a lab session");
  fl::enabled.store(true);Check(fl::Active(),"VrFrikElbows forces the lab on at every rung");fl::enabled.store(false);
  // continuity of the lab torso choice along a slow hand path (no >25 deg pick jumps from the window/level walls)
  fl::enabled.store(true);Vec3 prev=torso_elbow_preference(sh,{.05,.12,.80},U,F,{.2,-.45,-1},torsoA,torsoB,.14);double jump=0;
  for(int k=1;k<=400;++k){const Vec3 hp{.05,.12,.80+.55*k/400};const auto b=torso_elbow_preference(sh,hp,U,F,{.2,-.45,-1},torsoA,torsoB,.14);
    const auto ax=*unit(hp-sh);const auto p0=unit(perpendicular(prev,ax)),p1=unit(perpendicular(b,ax));
    if(p0&&p1)jump=std::max(jump,deg(std::acos(std::clamp(dot(*p0,*p1),-1.0,1.0))));prev=b;}
  fl::enabled.store(false);std::printf("  lab torso pick: largest step along a 55 cm vertical sweep %.2f deg\n",jump);
  Check(jump<=25.0,"lab torso clearance pick has no large jumps along a slow vertical sweep");}
 std::printf("%d/%d\n",pass,total);return pass==total?0:1;
}
