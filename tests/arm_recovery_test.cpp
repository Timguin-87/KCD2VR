#include "../src/vrik/arm_constraints.h"
#include <cstdio>
#include <limits>
using namespace kcd2vr::pose_core;
unsigned count=0,passed=0;void check(bool v,const char* n){++count;passed+=v;std::printf("%s %s\n",v?"PASS":"FAIL",n);}
int main(){
 ForearmPronation s{};Quat prev{};double sum=0,maxStep=0;bool finiteLoop=true;
 for(int i=0;i<=720;++i){const double t=2*armPi*i/720;Quat q{std::sqrt(1-.000001),.001*std::sin(t),0,.001*std::cos(t)};
  auto r=continuous_pronation(s,q,{0,1,0},1,i+1,1000+11*i);finiteLoop=finiteLoop&&bool(r);if(r){if(i){sum+=angle(prev,*r);maxStep=std::max(maxStep,angle(prev,*r));}prev=*r;}}
 check(finiteLoop,"720 near-singular projected twist samples stay finite");
 std::printf("singular_cumulative_deg=%.9f max_step_deg=%.9f\n",sum*180/armPi,maxStep*180/armPi);check(sum<1e-6,"tiny singular loop does not wind forearm");
 s={};Quat a{};for(int i=0;i<100;++i)a=*continuous_pronation(s,from_rotation_vector({0,179.9*armPi/180,0}),{0,1,0},1,i+1,1000+i*11);
 auto b=continuous_pronation(s,from_rotation_vector({0,180.1*armPi/180,0}),{0,1,0},1,101,2100);
 check(b&&angle(a,*b)<.01,"179.9 to 180.1 degrees does not switch anatomical clamp branches");
 auto old=s;auto same=continuous_pronation(s,from_rotation_vector({0,-1,0}),{0,1,0},1,101,2100);check(same&&s.unwrapped==old.unwrapped&&s.allowed==old.allowed,"duplicate consumption never integrates twist");
 check(!continuous_pronation(s,{}, {},1,102,2110),"zero axis fails closed");check(!continuous_pronation(s,{}, {0,1,0},1,100,2090),"out of order cannot rewind pronation");
 b=continuous_pronation(s,from_rotation_vector({0,1,0}),{0,1,0},2,1,3000);check(b&&s.allowed==0&&s.epoch==2,"new origin uses stable initial pronation");
 ElbowContinuity e{};continuous_elbow(e,{1,0,0},{0,0,1},{0,0,1},.8,1,1,1000);
 for(int i=1;i<=2000;++i)continuous_elbow(e,{1,0,0},{0,0,-1},{0,0,-1},.8,1,i+1,1000+11*i);
 check(dot(e.bend,Vec3{0,0,-1})>.999,"wrong-side elbow recovers to actual anatomical side");
 bool finite=true,lengths=true;for(int side:{-1,1})for(int h=0;h<200;++h){Vec3 shoulder{side*.18,0,1.4},hand{side*(.02+.5*std::cos(h*.03)),.2*std::sin(h*.05),1.1+.45*std::sin(h*.025)};
 auto p=torso_elbow_preference(shoulder,hand,.2663,.2615,{double(side),0,-1},{0,0,.85},{0,0,1.55},.12);auto arm=solve_arm(shoulder,hand,.2663,.2615,p,p);
 finite=finite&&bool(arm);if(arm)lengths=lengths&&std::abs(length(arm->elbow-shoulder)-.2663)<1e-8&&std::abs(length(arm->hand-arm->elbow)-.2615)<1e-8;}
 check(finite&&lengths,"mirrored torso/face/reach cases retain exact lengths");
 std::printf("arm_recovery=%u/%u\n",passed,count);return passed!=count;
}
