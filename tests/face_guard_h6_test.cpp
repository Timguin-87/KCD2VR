// Actual production header. This is input/posture intent, not native block acceptance.
#include "../src/vrik/fist_guard175.h"
#include <array>
#include <cmath>
#include <cstdio>
#include <limits>
namespace fg=kcd2vr::fist_guard175;
namespace f=kcd2vr::fists;
namespace p=kcd2vr::pose_core;
namespace tuning=kcd2vr::melee_tuning_c54;
static unsigned checks=0,failures=0;
static void ck(bool b,const char* why){++checks;if(!b){++failures;std::printf("FAIL %s\n",why);}}
static f::Sample sample(){f::Sample x{};x.actor=42;x.epoch=x.sequence=1;x.tick=1000;
 x.valid=x.empty[0]=x.empty[1]=true;x.head={0,1.7,0};
 x.controller[0]={.16,1.57,-.20};x.controller[1]={-.16,1.57,-.20};
 for(int h=0;h<2;++h){x.allowed[h]=x.controller[h];x.grip[h]=x.trigger[h]=.8;}return x;}
static fg::Decision advance(fg::State& s,f::Sample& x,unsigned ms=1,bool attacking=false){++x.sequence;x.tick+=ms;return fg::update(s,x,attacking);}
static bool settle(fg::State& s,f::Sample& x){fg::update(s,x,false);fg::Decision out{};for(unsigned i=0;i<200;++i)out=advance(s,x);return out.block;}
static p::Quat rotation(p::Vec3 axis,double angle){const auto a=p::unit(axis).value();const double q=std::sin(angle/2);return {a.x*q,a.y*q,a.z*q,std::cos(angle/2)};}
static f::Sample transformed(f::Sample x,p::Quat q,p::Vec3 shift){for(int h=0;h<2;++h){x.controller[h]=p::rotate(q,x.controller[h])+shift;x.allowed[h]=p::rotate(q,x.allowed[h])+shift;}x.head=p::rotate(q,x.head)+shift;x.headOrientation=q;return x;}
int main(){
 for(float sensitivity:{.5f,1.f,1.5f}){
  tuning::blockSensitivityH29=sensitivity;
  for(double down:{.55,.60}){
   fg::State s{};auto x=sample();for(auto& h:x.controller)h.y=x.head.y-down;
   ck(!settle(s,x),"55/60cm lowered posture never enters at any sensitivity");
   s={};x=sample();ck(settle(s,x),"face posture enters at every sensitivity");
   for(auto& h:x.controller)h.y=x.head.y-down;
   ck(!advance(s,x).block&&!s.guarding,"55/60cm lowered posture never retains an existing guard");
  }
  for(int side=0;side<2;++side){
   fg::State s{};auto x=sample();x.controller[side].y=x.head.y-.55;
   ck(!settle(s,x),"one low hand refuses bilateral guard even when other is raised");
   for(int analog=0;analog<2;++analog){
    x=sample();(analog?x.trigger[side]:x.grip[side])=double(.60f);s={};
    ck(!settle(s,x),"exact native float0.60 on either analog/hand is refused");
    (analog?x.trigger[side]:x.grip[side])=std::nextafter(.60f,1.f);s={};
    ck(settle(s,x),"next native float strictly above0.60 qualifies with other analogs closed");
    (analog?x.trigger[side]:x.grip[side])=0.;
    ck(!advance(s,x).block,"opening either analog immediately releases bilateral guard");
   }
  }
  // Same current source dwell: sensitivity changes only the time to entry.
  fg::State s{};auto x=sample();ck(!fg::update(s,x,false).block,"entry has no immediate block");
  const unsigned dwell=unsigned(std::ceil(80./double(tuning::blockH29())));
  for(unsigned i=1;i<dwell;++i)ck(!advance(s,x).block,"before slider-specific dwell block stays off");
  ck(advance(s,x).block,"slider-specific dwell enters exactly after required time");
  const auto originalTick=s.tick;for(unsigned n=0;n<20;++n)ck(fg::update(s,x,false).block&&s.tick==originalTick,"repeated original sample cannot renew a clock");
  ck(!advance(s,x,1,true).block,"qualified punch releases guard immediately");
  ck(!advance(s,x).block,"after attack guard needs fresh dwell rather than old timing");
  ck(settle(s,x),"fresh valid dwell rearms without moving rendered hands");
  x.valid=false;ck(!advance(s,x).block&&s.raisedSince==0,"tracking invalidation clears retained dwell");x.valid=true;
  ck(!advance(s,x).block&&settle(s,x),"tracking reacquisition starts fresh dwell");
  ++x.epoch;ck(!advance(s,x).block,"calibration/body epoch change cannot borrow dwell");
  ck(settle(s,x),"new calibration source can establish its own guard");
  ++x.actor;ck(!advance(s,x).block,"character change cannot borrow guard");
  ck(settle(s,x)&&!advance(s,x,81).block,"source gap beyond80ms resets guard");
  x=sample();s={};ck(settle(s,x),"neutral-rearm baseline");x.controller[1].z=.1;
  ck(!advance(s,x).block,"one hand behind head releases");x.controller[1].z=-.2;
  ck(!advance(s,x).block,"reentering neutral face region does not reuse old dwell");
  ck(settle(s,x),"reentered valid posture can rearm");x.empty[0]=false;
  ck(!advance(s,x).block,"equipped hand revokes unarmed posture");
 }
 // Property replay: identical spatial decisions at min/default/max. This does
 // not mirror policy inequalities; default's actual header output is the oracle.
 for(double y:{-.60,-.55,-.44,-.39,-.37,-.25,-.13,.14,.16,.25})
 for(double z:{-.70,-.50,-.47,-.20,-.07,-.03,.10})
 for(double halfWidth:{.03,.08,.16,.32,.40,.55}){
  std::array<bool,3> entered{};std::array<bool,3> retained{};unsigned n=0;
  for(float sensitivity:{.5f,1.f,1.5f}){tuning::blockSensitivityH29=sensitivity;
   auto x=sample();x.controller[0]={halfWidth,x.head.y+y,z};x.controller[1]={-halfWidth,x.head.y+y,z};fg::State s{};entered[n]=settle(s,x);
   x=sample();s={};settle(s,x);x.controller[0]={halfWidth,x.head.y+y,z};x.controller[1]={-halfWidth,x.head.y+y,z};retained[n]=advance(s,x).block;++n;
  }
  ck(entered[0]==entered[1]&&entered[1]==entered[2],"entry geometry invariant across slider");
  ck(retained[0]==retained[1]&&retained[1]==retained[2],"held hysteresis geometry invariant across slider");
 }
 for(float sensitivity:{.5f,1.f,1.5f})for(auto q:{rotation({0,1,0},1.2),rotation({1,0,0},.6),rotation({0,0,1},-.4),rotation({1,2,3},.8)})
 for(auto root:{p::Vec3{3,0,8},p::Vec3{-5,-.6,2},p::Vec3{1,.3,-9}}){
  tuning::blockSensitivityH29=sensitivity;auto x=transformed(sample(),q,root);fg::State s{};
  ck(settle(s,x),"original XR head/hand semantic frame is rotation/root/standing-seated translation invariant");
  auto low=sample();low.controller[1].y=low.head.y-.55;low=transformed(low,q,root);s={};
  ck(!settle(s,low),"same low-hand negative survives head rotations and calibration origin/height");
  // Native/model allowed wrist/ITEM presentation is a separate domain. It must
  // not become the source for this original XR intent policy.
  x.allowed[0]={100,200,300};x.allowed[1]={-100,-200,-300};s={};
  ck(settle(s,x),"guard evaluates original tracked hand sources, not item/allowed model transforms");
  x.headOrientation={-q.x,-q.y,-q.z,-q.w};s={};ck(settle(s,x),"quaternion hemisphere does not alter posture");
 }
 tuning::blockSensitivityH29=1;
 {auto x=sample();fg::State s{};x.headOrientation={0,0,0,0};ck(!settle(s,x),"invalid HMD frame refuses");
  x=sample();x.controller[0].x=std::numeric_limits<double>::quiet_NaN();s={};ck(!settle(s,x),"nonfinite source refuses");}
 std::printf("face_guard_h6 %u/%u actual_header; geometry_frozen_default timing_only; native_and_headset_NOT_RUN\n",checks-failures,checks);return failures?1:0;
}
