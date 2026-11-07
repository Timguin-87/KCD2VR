#include "vrik/fist_motion.h"
#include "diagnostics/punch_path_c54r3.h"
#include <cstdio>
#include <cstring>
using namespace kcd2vr::fists;
static int checks=0,failed=0;
static void ck(bool v,const char*n){++checks;if(!v){++failed;std::printf("FAIL %s\n",n);}}
static Sample sample(bool trial){Sample x{};x.epoch=x.sequence=1;x.tick=1000;x.actor=7;x.valid=x.empty[0]=x.empty[1]=true;x.head={0,1.6,0};x.controller[0]=x.allowed[0]={.2,1.25,-.3};x.controller[1]=x.allowed[1]={-.2,1.25,-.3};x.strokeExperimentD49=trial;return x;}
static Punch stepNext(State&s,Sample&x,pc::Vec3 delta={},Trace*t=nullptr){++x.sequence;x.tick+=10;x.controller[0]=x.controller[0]+delta;x.allowed[0]=x.allowed[0]+delta;return step(s,x,t);}
static void ready(State&s,Sample&x){step(s,x);x.grip[0]=x.trigger[0]=1;for(int i=0;i<20;++i)stepNext(s,x);}
static int moves(State&s,Sample&x,pc::Vec3 d,int n){int hits=0;for(int i=0;i<n;++i)hits+=stepNext(s,x,d).valid;return hits;}
int main(){
 for(bool trial:{false,true}){
  {State s{};auto x=sample(trial);ready(s,x);int n=moves(s,x,{0,0,-.02},10);ck(n==1,"first forward punch nominates once");
   ck(!moves(s,x,{0,0,.02},10),"physical return alone never nominates");
   n=moves(s,x,{0,0,-.02},10);ck(n==1,"continuous second attack leg rearms after measured return in both modes");
   ck(!moves(s,x,{0,0,-.02},10),"follow-through does not duplicate request");}
  {State s{};auto x=sample(trial);ready(s,x);moves(s,x,{0,0,-.02},10);moves(s,x,{0,0,.02},10);
   for(int i=0;i<30;++i){x.head.z-=.02;x.controller[0].z-=.02;x.allowed[0].z-=.02;ck(!stepNext(s,x).valid,"common mode translation after return cannot punch");}}
  {State s{};auto x=sample(trial);ready(s,x);moves(s,x,{0,0,-.02},10);moves(s,x,{0,0,.02},10);
   for(int i=0;i<30;++i){x.head.z+=.02;ck(!stepNext(s,x).valid,"head-only return/re-advance cannot spend a punch");}}
  {State s{};auto x=sample(trial);ready(s,x);moves(s,x,{0,0,-.02},10);moves(s,x,{0,0,.02},10);
   for(int i=0;i<30;++i){x.allowed[0].z-=.02;ck(!stepNext(s,x).valid,"native allowed-only motion cannot rearm into attack");}}
  {State s{};auto x=sample(trial);ready(s,x);moves(s,x,{0,0,-.02},10);moves(s,x,{0,0,.02},10);x.blocked[0]=true;ck(!moves(s,x,{0,0,-.02},10),"blocked onset remains blocked");}
  {State s{};auto x=sample(trial);ready(s,x);moves(s,x,{0,0,-.02},10);moves(s,x,{0,0,.02},10);x.trigger[0]=0;ck(!moves(s,x,{0,0,-.02},10),"trigger release revokes fist ownership");}
  {State s{};auto x=sample(trial);ready(s,x);moves(s,x,{0,0,-.02},10);moves(s,x,{0,0,.02},10);x.empty[1]=false;ck(!moves(s,x,{0,0,-.02},10),"native other-hand weapon prevents unarmed request");}
  {State s{};auto x=sample(trial);ready(s,x);moves(s,x,{0,0,-.02},10);moves(s,x,{0,0,.02},10);x.tick+=300;ck(!stepNext(s,x,{0,0,-.02}).valid&&s.hand[0].phase==Phase::Settling,"tracking gap discards return history");}
 }
 {State s{};auto x=sample(true);ready(s,x);moves(s,x,{0,0,-.02},10);moves(s,x,{0,0,.02},4);
  // Only8cm return: rearm must survive crossing back within5.5cm of old peak.
  ck(moves(s,x,{0,0,-.02},8)==1,"short deliberate return can begin a fresh jab without quiet pause");}
 {State s{};auto x=sample(true);ready(s,x);moves(s,x,{0,0,-.02},10);moves(s,x,{0,0,.02},10);
  Trace t{};ck(!stepNext(s,x,{0,0,-.02},&t).valid&&t.hand[0].motionRearmD53&&t.hand[0].after==Phase::Moving,"reversal creates onset, never instantaneous hit");
  x.allowed[0]=s.hand[0].allowed;for(int i=0;i<12;++i){x.controller[0].z-=.02;++x.sequence;x.tick+=10;ck(!step(s,x).valid,"new stroke still needs allowed wrist travel");}}
 {State s{};auto x=sample(true);ready(s,x);Trace t{};step(s,x,&t);ck(t.hand[0].reject==Duplicate&&t.hand[0].strokeExperimentD49&&t.hand[0].closed&&t.hand[0].released,"duplicate record preserves mode and squeeze facts");
  kcd2vr::diagnostics::punch_c54r3::Record r{};r.hand=t.hand[0];char out[3072]{};auto n=kcd2vr::diagnostics::punch_c54r3::format(r,out,sizeof(out));ck(n>0&&n<int(sizeof(out))&&std::strstr(out,"strokeExperimentD49=1")&&std::strstr(out,"motionRearmD53=0"),"bounded formatter reports exact trial/rearm");}
 std::printf("fist_rearm_d53=%d/%d; nomination only, native damage and headset unclaimed\n",checks-failed,checks);return failed?1:0;
}
