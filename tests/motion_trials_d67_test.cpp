#include "vrik/motion_trials_d67.h"
#include "vrik/elbow_motion_d67.h"
#include "core/publication_wait_d67.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <limits>
#include "diagnostics/capture_control168.h"
namespace kcd2vr::diagnostic_trace {void Write(const char*) noexcept {} void WriteFor(diagnostics::capture168::Domain,const char*) noexcept {}}
using namespace kcd2vr;
namespace pc=pose_core;
static unsigned checks=0,failed=0;
static void ck(bool ok,const char* name){++checks;if(!ok){++failed;std::cerr<<"FAIL "<<name<<'\n';}}
struct Row {unsigned session=0,side=0;std::uint64_t sample=0,epoch=0,tick=0,ns=0;pc::ElbowContinuity prior{};pc::Vec3 shoulder,goal,raw,torsoA,torsoB,backward,axis;double upper=0,fore=0,radius=0,reach=0;pc::Vec3 torso,conditioned,filtered;};
static void vec(pc::Vec3& v){std::cin>>v.x>>v.y>>v.z;}
static Row read(){Row r;int ready;std::cin>>r.session>>r.side>>r.sample>>r.epoch>>r.tick>>r.ns>>ready>>r.prior.epoch>>r.prior.sample>>r.prior.tick>>r.prior.timeNs;r.prior.ready=ready!=0;vec(r.prior.axis);vec(r.prior.bend);vec(r.shoulder);vec(r.goal);vec(r.raw);vec(r.torsoA);vec(r.torsoB);vec(r.backward);vec(r.axis);std::cin>>r.upper>>r.fore>>r.radius>>r.reach;vec(r.torso);vec(r.conditioned);vec(r.filtered);return r;}
static double angle(pc::Vec3 a,pc::Vec3 b){return std::acos(std::clamp(pc::dot(*pc::unit(a),*pc::unit(b)),-1.,1.))*180/pc::armPi;}
static pc::Vec3 solve(const Row& r,bool repaired){return elbow_motion_d67::preference(r.shoulder,r.goal,r.upper,r.fore,r.raw,r.torsoA,r.torsoB,r.radius,r.prior,r.epoch,r.sample,r.tick,repaired,false);}
int main(int argc,char** argv){
 std::ifstream captured;if(argc>1){captured.open(argv[1]);std::cin.rdbuf(captured.rdbuf());}
 namespace m=motion_trials_d67;namespace c=diagnostics::capture168;
 ck(m::ContinuityRepair(),"proven continuity repair starts enabled");
 ck(m::MenuMode()==2&&m::ModeAt(1000)==2,"D70 accepted fresh-publication timing starts by default without enabling capture");
 ck(!c::gate.active(c::Domain::Vrik,1000),"default timing does not arm diagnostics");
 const auto baselineArmPolicy=m::Policy(0);ck(m::Policy(1)==baselineArmPolicy&&m::Policy(2)==baselineArmPolicy,"timing and capture do not reset arm history");
 m::SetMenuMode(2);ck(m::ModeAt(1000)==2,"menu timing independent of capture");
 ck(c::cycleVrik(1000,"TEST")==1,"F7 starts baseline diagnostics");m::SetMode(1);
 ck(m::ModeAt(1001)==1&&m::TimingSelectedAt(1001)&&!m::ElbowSelectedAt(1001),"F7 recorder preserves accepted timing");
 const auto end=c::gate.until[unsigned(c::Domain::Vrik)].load(),serial=c::gate.session[unsigned(c::Domain::Vrik)].load();
 ck(c::cycleVrik(1010,"TEST")==2,"F7 timing second rung");m::SetMode(2);ck(m::TimingSelectedAt(1010)&&!m::ElbowSelectedAt(1010),"timing independent of elbows");
 ck(c::cycleVrik(1020,"TEST")==3,"F7 third rung");m::SetMode(3);ck(m::ElbowSelectedAt(1020)&&m::TimingSelectedAt(1020),"elbow comparison preserves independent accepted timing");
 ck(c::gate.until[unsigned(c::Domain::Vrik)].load()==end&&c::gate.session[unsigned(c::Domain::Vrik)].load()==serial,"rungs do not extend finite capture");
 ck(m::ModeAt(end)==2,"expiry restores saved preference without control polling");
 ck(c::cycleVrik(1030,"TEST")==0,"F7 off");m::SetMode(0);ck(m::ModeAt(1030)==2,"F7 off restores menu preference");
 m::SetMenuMode(0);ck(m::ModeAt(1030)==0,"menu off baseline");
 m::SetMenuMode(1);ck(m::MenuMode()==0,"diagnostic-only rung is not saved trial");
 m::SetElbowOutward(2);m::SetElbowBack(-1);ck(m::Policy(0)==baselineArmPolicy&&m::Policy(2)==baselineArmPolicy,"inactive pole tuning does not reset baseline arms");ck(m::ElbowOutward()==.6f&&m::ElbowBack()==.1f,"bounded live anatomy weights");
 m::SetElbowOutward(std::numeric_limits<float>::quiet_NaN());m::SetElbowBack(std::numeric_limits<float>::infinity());ck(m::ElbowOutward()==.35f&&m::ElbowBack()==.25f,"nonfinite anatomy resets trial defaults");
 m::SetContinuityRepair(false);ck(!m::ContinuityRepair(),"baseline repair can compare off");m::SetContinuityRepair(true);
 namespace w=publication_wait_d67;
 ck(w::floor(90,100,false)==90&&w::ready(100,90),"legacy timing admits already published unused token");
 ck(w::floor(90,100,true)==100&&!w::ready(100,100)&&w::ready(101,100),"trial requires publication newer than consumer arrival");
 ck(w::floor(110,100,true)==110&&!w::ready(109,110),"trial does not consume backwards token");
 const auto a=read(),b=read();ck(bool(std::cin)&&a.sample==302666&&b.sample==302670,"exact captured elbow pair loaded");
 const auto mix=[](pc::Vec3 x,pc::Vec3 y,double t){return x+(y-x)*t;};
 double oldJump=0,newJump=0,wristError=0,lengthError=0;auto old=solve(a,false),fixed=solve(a,true);
 for(unsigned k=1;k<=10000;++k){Row r=a;const double t=k/10000.;r.shoulder=mix(a.shoulder,b.shoulder,t);r.goal=mix(a.goal,b.goal,t);r.raw=mix(a.raw,b.raw,t);r.torsoA=mix(a.torsoA,b.torsoA,t);r.torsoB=mix(a.torsoB,b.torsoB,t);r.radius=a.radius+(b.radius-a.radius)*t;
  const auto nextOld=solve(r,false),nextFixed=solve(r,true);oldJump=std::max(oldJump,angle(old,nextOld));newJump=std::max(newJump,angle(fixed,nextFixed));old=nextOld;fixed=nextFixed;
  const auto arm=pc::solve_arm(r.shoulder,r.goal,r.upper,r.fore,nextFixed,nextFixed);if(!arm){ck(false,"fixed arm solve valid");break;}
  wristError=std::max(wristError,pc::length(arm->hand-r.goal));lengthError=std::max({lengthError,std::abs(pc::length(arm->elbow-r.shoulder)-r.upper),std::abs(pc::length(arm->hand-arm->elbow)-r.fore)});
 }
 ck(oldJump>50.,"negative control reproduces default discrete branch flip");ck(newJump<.2,"continuous branch removes captured target discontinuity");ck(wristError<1e-8&&lengthError<1e-8,"controller endpoint and hard segment lengths preserved");
 const auto hanging=elbow_motion_d67::reachPole(a.raw,{1,0,0},{0,0,1},{0,0,0},{0,0,-.4},1);
 ck(pc::length(hanging-*pc::unit(a.raw))<1e-12,"trial retains relaxed hanging pole");
 const auto r=elbow_motion_d67::reachPole({.18,-.40,-.9},{1,0,0},{0,0,1},{0,0,0},{0,.4,.1},0);
 const auto l=elbow_motion_d67::reachPole({-.18,-.40,-.9},{1,0,0},{0,0,1},{0,0,0},{0,.4,.1},1);
 ck(std::abs(r.x+l.x)<1e-12&&std::abs(r.y-l.y)<1e-12&&std::abs(r.z-l.z)<1e-12,"trial raised poles mirror across body");
 std::cout<<std::setprecision(12)<<"oldTargetStepDeg="<<oldJump<<" repairedTargetStepDeg="<<newJump<<" endpointErrorM="<<wristError<<" segmentErrorM="<<lengthError<<"\nD67 motion "<<(checks-failed)<<'/'<<checks<<'\n';return failed?1:0;
}
