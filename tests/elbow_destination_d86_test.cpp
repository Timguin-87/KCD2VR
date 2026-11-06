#include "../src/vrik/elbow_destination_d86.h"
#include <cstdio>
using namespace kcd2vr::elbow_destination_d86;
static int checks=0,fail=0;
void ck(bool ok,const char* n){++checks;if(!ok){++fail;std::printf("FAIL %s\n",n);}}
double deg(m::Vec3 a,m::Vec3 b){return std::acos(std::clamp(m::dot(*m::unit(a),*m::unit(b)),-1.,1.))*180/m::armPi;}
Input fixture(int side=0){Input in{};double sign=side?-1.:1.;in.shoulder={sign*.19,0,1.40};in.hand={sign*.15,.15,1.60};
 in.upper=.28;in.fore=.27;in.relaxed={sign*.20,-.45,-1};in.outward={sign,0,0};in.up={0,0,1};in.backward={0,-1,0};
 in.torsoA={0,0,.85};in.torsoB={0,0,1.40};in.torsoRadius=.115;in.head={0,0,1.66};in.haveHead=true;
 in.forearmDirection=*m::unit(m::Vec3{-sign*.4,.5,.7});in.haveForearm=true;return in;}
int main(){
 auto in=fixture();auto r=choose(in);ck(r.valid,"near-face destination valid");
 double maxWrist=0,maxLength=0,maxMirror=0,maxIntrusion=0;unsigned maxEval=0;
 for(int side=0;side<2;++side)for(int i=0;i<180;++i){
  auto a=fixture(side);double sign=side?-1.:1.,t=i*m::armPi/179;
  a.hand={sign*(.19+.14*std::sin(t)),.12+.08*std::sin(t),1.22+.64*i/179};
  auto result=choose(a);ck(result.valid,"face-to-overhead arm valid");if(!result.valid)continue;
  auto arm=m::solve_arm(a.shoulder,a.hand,a.upper,a.fore,result.pole,result.pole);
  ck(bool(arm),"exact arm reconstruction");if(!arm)continue;
  maxWrist=std::max(maxWrist,m::length(arm->hand-a.hand));
  maxLength=std::max({maxLength,std::abs(m::length(arm->elbow-a.shoulder)-a.upper),std::abs(m::length(arm->hand-arm->elbow)-a.fore)});
  maxIntrusion=std::max(maxIntrusion,result.intrusion);maxEval=std::max(maxEval,result.evaluations);
  auto b=a;b.shoulder.x*=-1;b.hand.x*=-1;b.relaxed.x*=-1;b.outward.x*=-1;b.forearmDirection.x*=-1;
  auto mirrored=choose(b);auto expected=result.pole;expected.x*=-1;
  if(mirrored.valid)maxMirror=std::max(maxMirror,deg(expected,mirrored.pole));
 }
 ck(maxWrist<1e-9&&maxLength<1e-9,"controllers and native segment lengths remain exact");
 ck(maxIntrusion<1e-10,"reachable face/overhead fixture clears torso and head proxies");
 ck(maxMirror<.01,"anatomy mirrors without scan-order preference");
 const m::Quat rest=m::from_rotation_vector({.3,.7,-.1}),now=m::from_rotation_vector({-.6,.2,.4});
 const m::Vec3 restFore{.2,0,1.4},restHand{.2,.25,1.5};
 const auto base=forearm_direction(restFore,restHand,rest,now);ck(bool(base),"neutral forearm direction usable");
 for(int k=-180;k<=180;k+=5){const auto q=m::multiply(m::from_rotation_vector(*base*(k*m::armPi/180)),now);
  auto cue=forearm_direction(restFore,restHand,rest,q);ck(cue&&m::length(*cue-*base)<1e-12,"pure forearm pronation cannot steer elbow cue");}
 ck(!forearm_direction(restFore,restFore,rest,now),"invalid neutral anatomy supplies no invented axis");
 // D85's normalized outward floor changes about57 degrees across this axis.
 // The D86 costs become weak rather than amplifying a near-zero projection.
 auto left=fixture();left.haveHead=false;left.haveForearm=false;left.hand=left.shoulder+m::Vec3{.4,-1e-6,0};
 auto one=choose(left);left.prior=one.pole;left.hand.y=1e-6;
 auto two=choose(left);ck(one.valid&&two.valid&&deg(one.pole,two.pole)<.02,"millimetre-scale outward-axis crossing has no pole hemisphere flip");
 // The anatomical destination must not chase its transported starting bend.
 auto loop=fixture();loop.haveForearm=false;loop.haveHead=false;loop.hand={.2,.35,1.4};
 auto start=choose(loop);auto current=start;
 for(int turn=0;turn<2;++turn)for(int i=0;i<=360;++i){double t=i*m::armPi/180;loop.hand={.2+.08*std::sin(t),.35,1.4+.08*std::cos(t)};
  const auto ax=*m::unit(loop.hand-loop.shoulder);m::ElbowContinuity s{};s.ready=true;s.axis=*m::unit(loop.hand-loop.shoulder);s.bend=current.pole;
  loop.prior=m::unit(m::perpendicular(current.pole,ax));current=choose(loop);ck(current.valid,"continuous closed arm loop valid");}
 loop.hand={.2,.35,1.4};for(int i=0;i<10;++i){loop.prior=current.pole;current=choose(loop);}
 ck(deg(start.pole,current.pole)<.1,"closed arm motion returns to anatomy without corkscrew memory");
 auto steady=fixture();auto dest=choose(steady);m::ElbowContinuity state{};
 auto first=m::continuous_elbow(state,steady.hand-steady.shoulder,dest.pole,dest.pole,.6,3,1,1000,1000000000);
 auto count=state.updates;auto again=m::continuous_elbow(state,steady.hand-steady.shoulder,dest.pole,dest.pole,.6,3,1,1000,1000000000);
 ck(first&&again&&state.updates==count&&m::length(*first-*again)<1e-12,"second eye cannot advance anatomical bend");
 // Near-folded and almost-straight wrists stay finite without stretching
 // or a special pole overwrite after the fixed-length solve.
 for(double reach:{.010002,.03,.5498}){auto a=fixture();a.haveHead=false;
  a.hand=a.shoulder+*m::unit(m::Vec3{.2,.8,.4})*reach;auto d=choose(a);
  auto arm=d.valid?m::solve_arm(a.shoulder,a.hand,a.upper,a.fore,d.pole,d.pole):std::optional<m::ArmSolution>{};
  ck(arm&&m::length(arm->hand-a.hand)<1e-9,"near-folded/straight geometry keeps exact reachable wrist");}
 m::Vec3 samples[3][91]{};double filteredIntrusion=0;
 const int rates[3]{60,90,120};
 for(int rateIndex=0;rateIndex<3;++rateIndex){const int hz=rates[rateIndex];m::ElbowContinuity history{};
  for(int i=0;i<=3*hz;++i){double t=double(i)/hz;auto a=fixture();
   a.hand={.24,.18,1.25+.52*std::min(1.,t/2.)};
   if(history.ready)a.prior=m::transported_elbow_bend(history,*m::unit(a.hand-a.shoulder));
   const auto d=choose(a);const auto ns=std::uint64_t(1000000000+std::llround(t*1e9));
   const double reach=m::length(a.hand-a.shoulder)/(a.upper+a.fore);
   auto bend=d.valid?m::continuous_elbow(history,a.hand-a.shoulder,d.pole,d.pole,reach,9,i+1,ns/1000000,ns):std::optional<m::Vec3>{};
   ck(bool(bend),"continuous raising trajectory valid at60/90/120Hz");
   if(bend){auto c=circle(a);filteredIntrusion=std::max(filteredIntrusion,intrusion(a,*c,*bend));
    if(i%(hz/30)==0)samples[rateIndex][i/(hz/30)]=*bend;}
  }
 }
 double rateDifference=0;for(int i=0;i<=90;++i)for(int hz=1;hz<3;++hz)rateDifference=std::max(rateDifference,deg(samples[0][i],samples[hz][i]));
 ck(rateDifference<2.,"anatomical trajectory does not depend materially on frame rate");
 ck(filteredIntrusion<1e-10,"pole filtering stays outside proxies on clear raising path");
 std::printf("rate_difference_deg=%.9g filtered_proxy_intrusion=%.9g\n",rateDifference,filteredIntrusion);
 std::printf("elbow_destination_d86=%d/%d wrist_error_m=%.12g length_error_m=%.12g mirror_deg=%.8g proxy_intrusion=%.9g max_objective_evaluations=%u\n",checks-fail,checks,maxWrist,maxLength,maxMirror,maxIntrusion,maxEval);
 return fail?1:0;
}
