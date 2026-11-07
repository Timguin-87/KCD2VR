#include "vrik/elbow_shoulder_d45.h"
#include <cstdio>
#include <cstring>
namespace pc=kcd2vr::pose_core;namespace ex=kcd2vr::elbow_shoulder_d45;
constexpr double U=.26628,F=.26153;
static int checks=0,failures=0;
static void ck(bool v,const char* s){++checks;if(!v){++failures;std::printf("FAIL %s\n",s);}}
static bool near(pc::Vec3 a,pc::Vec3 b,double e=1e-8){return pc::length(a-b)<e;}
static void endpointSweep(){
 for(int side:{0,1})for(double distance:{0.,.001,.03,.1,.3,.519,.527,.7,1.2}){
  ex::ShoulderState state{};pc::ElbowContinuity elbow{};
  const pc::Vec3 natural{side?-.18:.18,0,1.4};
  for(unsigned n=1;n<=72;++n){
   const double a=n*pc::armPi/36;const auto axis=*pc::unit(pc::Vec3{std::cos(a),std::sin(a),.3*std::sin(2*a)});
   const auto wrist=natural+axis*distance;const auto ns=1000000000ull+n*11111111ull;
   const auto fit=ex::shoulder(state,natural,wrist,{},U,F,axis,1,2,n,ns/1000000,ns);
   ck(bool(fit),"all near/far/fast shoulder fits valid");if(!fit)continue;
   const auto currentAxis=*pc::unit(wrist-fit->shoulder);
   const auto raw=*pc::unit(pc::Vec3{side?-.2:.2,-.45,-1});
   const auto torso=pc::torso_elbow_preference(fit->shoulder,wrist,U,F,raw,{0,0,.85},{0,0,1.4},.1152,
      elbow.ready?ex::transport(elbow,currentAxis):std::nullopt,true,false);
   const auto condition=pc::condition_elbow_preference(elbow,currentAxis,torso,raw,{0,-1,0},1,n,ns/1000000);
   const auto bend=condition?ex::elbow(elbow,currentAxis,condition->anatomical,condition->fallback,
      pc::length(wrist-fit->shoulder)/(U+F),1,n,ns/1000000,ns):std::nullopt;
   const auto arm=bend?pc::solve_arm(fit->shoulder,wrist,U,F,*bend,*bend,ex::reserve(U,F)):std::nullopt;
   ck(arm&&near(arm->hand,wrist)&&!arm->reach_limited,"experiment never delays or retargets wrist");
   ck(arm&&std::abs(pc::length(arm->elbow-fit->shoulder)-U)<1e-8&&std::abs(pc::length(arm->hand-arm->elbow)-F)<1e-8,"hard upper/forearm lengths");
  }
 }
}
static void composition(){
 const pc::Vec3 restE{U,0,0},restW{U+F*.6,F*.8,0},plane{0,0,1};
 for(double a:{0.,1e-8,.0014142,.0014143,pc::armPi-.0014143,pc::armPi-.0014142,pc::armPi}){
  const auto q=pc::from_rotation_vector({0,0,a});const auto desiredE=pc::rotate(q,restE),desiredW=pc::rotate(q,restW);
  const auto upper=ex::orient_upper({},restE,plane,desiredE,plane);
  ck(bool(upper),"upper orientation accepts tiny and antipodal angles");if(!upper)continue;
  const auto e=pc::rotate(*upper,restE),before=e+pc::rotate(*upper,restW-restE);
  const auto fore=pc::multiply(ex::from_to(before-e,desiredW-e,plane),*upper);
  const auto w=e+pc::rotate(fore,restW-restE);
  ck(near(e,desiredE)&&near(w,desiredW),"composed bones match desired endpoint across former angle dead zones");
 }
}
static void history(){
 ex::ShoulderState s{};const pc::Vec3 shoulder{},first{.52,0,0},fast{0,.9,0};
 const auto a=ex::shoulder(s,shoulder,first,{},U,F,{1,0,0},1,3,1,1000,1000000000);
 const auto b=ex::shoulder(s,shoulder,fast,{},U,F,{1,0,0},1,3,2,1011,1011111111);
 ck(a&&b&&pc::length(fast-b->shoulder)<=U+F-ex::reserve(U,F),"fast current wrist forces immediate feasible shoulder");
 const auto saved=s;const auto duplicate=ex::shoulder(s,shoulder,fast,{},U,F,{1,0,0},1,3,2,1011,1011111111);
 ck(duplicate&&b&&near(duplicate->shoulder,b->shoulder)&&s.updates==saved.updates&&near(s.offsetChest,saved.offsetChest),"duplicate input does not advance shoulder history");
 ck(!ex::shoulder(s,shoulder,first,{},U,F,{1,0,0},1,3,1,1000,1000000000),"old sample cannot rewind history");
 const auto reset=ex::shoulder(s,shoulder,first,{},U,F,{1,0,0},1,4,1,1000,1000000000);
 ck(reset&&s.policy==4&&s.sample==1,"comparison epoch retires history");
 pc::ElbowContinuity pole{};const auto start=ex::elbow(pole,{1,0,0},{0,0,-1},{0,0,-1},.7,1,1,1000,1000000000);
 const auto opposite=ex::elbow(pole,{-1,0,0},{0,0,-1},{0,0,-1},.7,1,2,1011,1011111111);
 ck(start&&opposite&&near(*start,*opposite),"180 degree axis reversal keeps known bend side");
 const auto count=pole.updates;const auto prior=pole.bend;
 const auto same=ex::elbow(pole,{-1,0,0},{0,1,0},{0,1,0},.7,1,2,1011,1011111111);
 ck(same&&near(*same,prior)&&pole.updates==count&&near(pole.bend,prior),"duplicate pole does not integrate changed destination");
 // Natural body translation/rotation is not filtered along with reach offset.
 ex::ShoulderState t{};const auto base=ex::shoulder(t,{0,0,0},{.6,0,0},{},U,F,{1,0,0},1,1,1,1000,1000000000);
 const auto rot=pc::from_rotation_vector({0,0,1.5});const pc::Vec3 shift{10,20,30};
 const auto moved=ex::shoulder(t,shift,pc::rotate(rot,{.6,0,0})+shift,rot,U,F,pc::rotate(rot,{1,0,0}),1,1,2,1011,1011111111);
 ck(base&&moved&&near(moved->shoulder,pc::rotate(rot,base->shoulder)+shift),"chest/world transport has no temporal lag");
}
int main(){endpointSweep();composition();history();std::printf("D45 arm experiment %d/%d PASS\n",checks-failures,checks);return failures?1:0;}
