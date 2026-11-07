#include "vrik/fist_motion.h"
#include "vrik/fist_guard175.h"
#include <cassert>
#include <cstdio>
namespace f=kcd2vr::fists;namespace m=kcd2vr::melee_tuning_c54;
static unsigned checks=0;static void check(bool b){++checks;assert(b);}
static f::Sample sample(){f::Sample x{};x.valid=x.empty[0]=x.empty[1]=true;x.epoch=x.sequence=1;x.tick=1000;x.timeNs=1000000000;x.actor=42;x.responsiveCombatH27=true;x.boxing167=false;x.head={0,1.7,0};x.controller[0]=x.allowed[0]={.16,1.57,-.2};x.controller[1]=x.allowed[1]={-.16,1.57,-.2};return x;}
static f::Punch step(f::State& s,f::Sample& x,double z=0){++x.sequence;x.tick+=10;x.timeNs+=10000000;x.controller[0].z-=z;x.allowed[0].z-=z;return f::step(s,x);}
static unsigned jab(double trigger,double grip,float sensitivity=1){m::punchSensitivity=sensitivity;f::State s{};auto x=sample();f::step(s,x);x.grip[0]=grip;x.trigger[0]=trigger;step(s,x);unsigned hits=0;for(unsigned i=0;i<6;++i)hits+=step(s,x,.012).valid;return hits;}
int main(){
 check(jab(.61,.61)==1);check(jab(.60,.61)==0);check(jab(.61,.60)==0);check(jab(0,1)==0);check(jab(1,0)==0);
 check(jab(1,1,.5f)==0);check(jab(1,1,2.f)==1);m::punchSensitivity=1;
 // Normal recognition is identical before and during reciprocal combat;
 // H27 only changes readiness. F11 explicitly owns the stronger comparison.
 for(bool combat:{false,true}){f::State s{};auto normal=sample();normal.responsiveCombatH27=combat;
  f::step(s,normal);normal.grip[0]=normal.trigger[0]=.61;for(unsigned i=0;i<20;++i)step(s,normal);
  check(f::speedMinimum(normal)==.50&&f::travelMinimum(normal)==.045&&f::allowedTravelMinimum(normal)==.015);
  unsigned hits=0;for(unsigned i=0;i<12;++i)hits+=step(s,normal,.006).valid;check(hits==1);
  for(unsigned i=0;i<40;++i)hits+=step(s,normal,.006).valid;check(hits==1);
 }
 {auto comparison=sample();comparison.boxing167=true;check(f::speedMinimum(comparison)==1.&&f::travelMinimum(comparison)==.10&&f::allowedTravelMinimum(comparison)==.045);}
 auto x=sample();x.grip[0]=x.grip[1]=x.trigger[0]=x.trigger[1]=.61;
 kcd2vr::fist_guard175::State guard{};for(unsigned i=0;i<10;++i){++x.sequence;x.tick+=10;kcd2vr::fist_guard175::update(guard,x,false);}check(guard.guarding);
 x.trigger[0]=.60;check(!kcd2vr::fist_guard175::update(guard,x,false).block);x.trigger[0]=.61;x.grip[1]=.60;check(!kcd2vr::fist_guard175::update(guard,x,false).block);
 x.grip[1]=.61;x.controller[0].y=x.controller[1].y=1.15;guard={};m::blockSensitivityH29=1;for(unsigned i=0;i<20;++i){++x.sequence;x.tick+=10;kcd2vr::fist_guard175::update(guard,x,false);}check(!guard.guarding);
 guard={};m::blockSensitivityH29=1.5f;for(unsigned i=0;i<20;++i){++x.sequence;x.tick+=10;kcd2vr::fist_guard175::update(guard,x,false);}check(guard.guarding);
 x.trigger[1]=.60;check(!kcd2vr::fist_guard175::update(guard,x,false).block);m::blockSensitivityH29=1;
 std::printf("fist_qualification_h29 %u checks PASS; motion and guard only; native damage unclaimed\n",checks);
}
