#include "../src/vrik/fist_motion.h"
#include "../src/vrik/fist_guard175.h"
#include <cstdio>
#include <cstdlib>
using namespace kcd2vr::fists;
static unsigned checks=0;
static void ck(bool b,const char* why){++checks;if(!b){std::fprintf(stderr,"FAIL %u %s\n",checks,why);std::exit(2);}}
static Sample sample(bool responsive=true){Sample x{};x.epoch=x.sequence=1;x.tick=1000;x.actor=123;x.valid=x.empty[0]=x.empty[1]=true;
 x.head={0,1.6,0};x.controller[0]=x.allowed[0]={.2,1.25,-.2};x.controller[1]=x.allowed[1]={-.2,1.25,-.2};x.boxing167=true;x.responsiveCombatH27=responsive;return x;}
static Punch advance(State& s,Sample& x,double dz=0,int side=0){x.tick+=10;++x.sequence;x.controller[side].z-=dz;x.allowed[side].z-=dz;return step(s,x);}
static unsigned jab(State& s,Sample& x,int side=0){unsigned n=0;for(unsigned i=0;i<8;++i)n+=advance(s,x,.012,side).valid;return n;}
int main(){
 for(int side:{0,1})for(double grip:{.60,.64,1.0}){State s{};auto x=sample();step(s,x);x.grip[side]=grip;
  ck(!advance(s,x,.012,side).valid,"Grip closure + first moving baseline is not an attack");
  ck(s.hand[side].phase==Phase::Ready,"actual combat Grip arms without120ms quiet or Trigger");
  ck(jab(s,x,side)==1,"deliberate short Grip-only jab recognizes exactly once");
  ck(jab(s,x,side)==0,"followthrough cannot repeat punch");
  for(unsigned i=0;i<8;++i)advance(s,x,-.014,side);for(unsigned i=0;i<8;++i)advance(s,x,0,side);
  ck(jab(s,x,side)==1,"real retraction and quiet rearms new jab whileGrip staysclosed");
 }
 {State s{};auto x=sample(false);step(s,x);x.grip[0]=1;advance(s,x);ck(jab(s,x)==0,"same H25 Grip-only trajectory refused by legacy Trigger+Grip policy");}
 {State s{};auto x=sample();step(s,x);x.grip[0]=.59;advance(s,x);ck(jab(s,x)==0,"subthreshold Grip cannot arm");
  x.grip[0]=.65;advance(s,x);x.grip[0]=.44;ck(jab(s,x)==1,"Grip hysteresis tolerates squeeze noise after real closure");}
 {State s{};auto x=sample();step(s,x);x.grip[0]=1;advance(s,x);unsigned n=0;
  for(unsigned i=0;i<1000;++i){x.controller[0].x=x.allowed[0].x=.2+(i%2?.002:-.002);n+=advance(s,x).valid;}
  ck(n==0,"holding/jitter never punches");}
 {State s{};auto x=sample();step(s,x);x.grip[0]=1;advance(s,x);unsigned n=0;
  for(unsigned i=0;i<50;++i){x.head.z-=.02;x.controller[0].z-=.02;x.allowed[0].z-=.02;n+=advance(s,x).valid;}
  ck(n==0,"roomwalking common-mode translation is not punch");}
 {State s{};auto x=sample();step(s,x);x.grip[0]=1;advance(s,x);unsigned n=0;
  for(unsigned i=0;i<50;++i){x.head.z-=.02;n+=advance(s,x).valid;}ck(n==0,"head-only dodge never punches");}
 {State s{};auto x=sample();step(s,x);x.grip[0]=1;advance(s,x);x.controller[0].z-=1;x.allowed[0].z-=1;
  ck(!advance(s,x).valid&&jab(s,x)==0,"tracking jump and held reacquisition cannot punch");}
 {State s{};auto x=sample();x.grip[0]=1;step(s,x);advance(s,x);ck(jab(s,x)==0,"entering owner whileGripalreadyheld requiresrelease");}
 {State s{};auto x=sample();step(s,x);x.grip[0]=1;advance(s,x);x.empty[1]=false;ck(jab(s,x)==0,"opposite equipped item excludes unarmedrecognition");}
 {State s{};auto x=sample();step(s,x);x.grip[0]=1;advance(s,x);x.valid=false;advance(s,x);x.valid=true;ck(jab(s,x)==0,"revokedcurrentowner cannot borrow readiness");}
 {State s{};auto x=sample();step(s,x);x.grip[0]=1;advance(s,x);x.blocked[0]=true;ck(jab(s,x)==0,"alreadyblocked hand cannot seed a stroke");}
 {auto x=sample();x.grip[0]=.6;ck(held(x,0),"actual native stagedconsumer retains Grip-only source");x.grip[0]=.34;ck(!held(x,0),"actual source release cancels stagedconsumer");
  x.responsiveCombatH27=false;x.grip[0]=1;ck(!held(x,0),"legacy/outsidecombat stillrequiresTrigger for stagedconsumer");}
 {auto x=sample();x.grip[0]=x.grip[1]=1;kcd2vr::fist_guard175::State guard{};
  for(unsigned i=0;i<10;++i){x.tick+=10;++x.sequence;kcd2vr::fist_guard175::update(guard,x,false);}
  ck(guard.guarding,"twoactuallyraised Gripclosedhands retain spatialguard");x.controller[0].z=x.controller[1].z=.3;
  ck(!kcd2vr::fist_guard175::update(guard,x,false).block,"handsbehindbody do notguard");}
 std::printf("fist_responsive_h27: %u checks PASS; recognition/inputintent only nativeacceptedresults UNCLAIMED\n",checks);
}
