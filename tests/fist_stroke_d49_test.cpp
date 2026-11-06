#include "vrik/fist_motion.h"
#include "diagnostics/punch_path_c54r3.h"
#include <cstdio>
#include <cstring>
using namespace kcd2vr::fists;
static unsigned checks=0,failed=0;
static void ck(bool value,const char* name){++checks;if(!value){++failed;std::printf("FAIL %s\n",name);}}
static Sample sample(bool experiment=true){Sample x{};x.epoch=x.sequence=1;x.tick=1000;x.actor=123;x.valid=x.empty[0]=x.empty[1]=true;x.head={0,1.6,0};x.controller[0]=x.allowed[0]={.2,1.25,-.3};x.controller[1]=x.allowed[1]={-.2,1.25,-.3};x.strokeExperimentD49=experiment;return x;}
static Punch advance(State&s,Sample&x,Trace* trace=nullptr){++x.sequence;x.tick+=10;return step(s,x,trace);}
static void ready(State&s,Sample&x){step(s,x);x.grip[0]=x.trigger[0]=1;for(int i=0;i<20;++i)advance(s,x);}
static Punch move(State&s,Sample&x,pc::Vec3 delta,Trace* trace=nullptr){x.controller[0]=x.controller[0]+delta;x.allowed[0]=x.allowed[0]+delta;return advance(s,x,trace);}
static unsigned stroke(State&s,Sample&x,pc::Vec3 delta,int count=12){unsigned n=0;for(int i=0;i<count;++i)n+=move(s,x,delta).valid;return n;}
int main(){
    kcd2vr::punch_travel_d65::minimumMeters.store(.045f); // explicit D64 reference; slider defaults covered separately
 ck(!kcd2vr::fist_stroke_d49::enabled.load()&&!Sample{}.strokeExperimentD49,"trial defaults off in both UI flag and sample");
 for(bool on:{false,true}){
  {State s{};auto x=sample(on);ready(s,x);unsigned n=0;for(int i=0;i<12;++i)n+=move(s,x,{0,0,i==2?.003:-.020}).valid;ck(n==1u,"failed short candidate can recover for a fresh extension in both modes");}
  {State s{};auto x=sample(on);ready(s,x);unsigned n=move(s,x,{0,0,-.020}).valid;for(int i=0;i<8;++i)n+=move(s,x,{0,0,-.004}).valid;ck(n==(on?1u:0u),"peak permits slower genuine finish only in experiment");}
  {State s{};auto x=sample(on);ready(s,x);unsigned n=stroke(s,x,{0,0,-.012},3);n+=stroke(s,x,{0,0,.020});ck(!n,"real reversal before travel is not a punch");}
  {State s{};auto x=sample(on);x.controller[0].z=x.allowed[0].z=-.45;ready(s,x);ck(stroke(s,x,{0,0,.018})==0u,"inward straight return cannot seed either detector");}
  {State s{};auto x=sample(on);ready(s,x);ck(stroke(s,x,{0,.020,0})==1,"upward gesture remains an uppercut");}
  {State s{};auto x=sample(on);ready(s,x);Trace trace{};Punch p{};for(int i=0;i<12;++i){auto q=move(s,x,{.018,0,-.008},&trace);if(q.valid)p=q;}ck(p.valid&&p.motion167==1,"outward/forward hook retains hook nomination");}
  {State s{};auto x=sample(on);ready(s,x);unsigned n=0;for(int i=0;i<80;++i){x.head.z-=.018;x.controller[0].z-=.018;x.allowed[0].z-=.018;n+=advance(s,x).valid;}ck(!n,"common-mode walking cannot punch");}
  {State s{};auto x=sample(on);ready(s,x);unsigned n=0;for(int i=0;i<20;++i){x.head.z+=.018;n+=advance(s,x).valid;}ck(!n,"head-only motion cannot punch");}
  {State s{};auto x=sample(on);ready(s,x);unsigned n=0;for(int i=0;i<20;++i){x.allowed[0].z-=.018;n+=advance(s,x).valid;}ck(!n,"native root-only allowed-hand movement cannot punch");}
  {State s{};auto x=sample(on);ready(s,x);unsigned n=move(s,x,{0,0,-.020}).valid;for(int i=0;i<12;++i){x.head.z+=.018;x.allowed[0].z-=.018;n+=advance(s,x).valid;}ck(!n,"short real onset plus head/root movement cannot spend old peak");}
  {State s{};auto x=sample(on);ready(s,x);x.empty[1]=false;ck(!stroke(s,x,{0,0,-.018}),"other-hand held weapon cancels unarmed nomination");}
  {State s{};auto x=sample(on);ready(s,x);x.claimedT24[0]=true;ck(!stroke(s,x,{0,0,-.018}),"mod-held object owns its hand");}
  {State s{};auto x=sample(on);ready(s,x);x.trigger[0]=0;ck(!stroke(s,x,{0,0,-.018}),"Grip alone never nominates a punch");}
  {State s{};auto x=sample(on);ready(s,x);ck(stroke(s,x,{0,0,-.018})==1,"ordinary punch nominates once");ck(!stroke(s,x,{0,0,-.018})&&!step(s,x).valid,"follow-through and duplicate sample cannot renominate");}
  for(bool fresh:{false,true}){State s{};auto x=sample(on);ready(s,x);x.contactStop175=true;unsigned n=stroke(s,x,{0,0,-.018},2);x.blocked[0]=true;x.freshContact174[0]=fresh;x.controller[0].z-=.018;n+=advance(s,x).valid;ck(n==(fresh?1u:0u),"stopped allowed hand requires real fresh contact");}
 }
 for(bool before:{false,true}){State s{};auto x=sample(before);ready(s,x);move(s,x,{0,0,-.020});x.strokeExperimentD49=!before;ck(!move(s,x,{0,0,-.030}).valid&&s.hand[0].phase==Phase::Settling,"mode change discards current onset and restarts quiet");
  for(int i=0;i<20;++i)advance(s,x);ck(stroke(s,x,{0,0,-.018})==1,"new settled stroke after live change works");}
 {Punch p{true,0,10,1100,1,123,1.,.06};ck(retainUnenteredPendingD49(p,false,false).valid,"unchanged trial preserves unentered nomination");ck(!retainUnenteredPendingD49(p,false,true).valid,"changed trial retires unentered punch");p.kickH35=true;p.kickSerialH35=99;auto k=retainUnenteredPendingD49(p,false,true);ck(k.valid&&k.kickSerialH35==99,"mode change preserves existing R3 kick request");}
 {namespace fp=kcd2vr::diagnostics::punch_c54r3;fp::Record r{};r.hand.strokeExperimentD49=true;r.hand.strokeSpeedD49=1.5;r.hand.reversalDistanceD49=.004;char text[3072]{};int n=fp::format(r,text,sizeof(text));ck(n>0&&n<int(sizeof(text))&&std::strstr(text,"PEAK_RETRACT_GT_0.015")&&std::strstr(text,"strokeExperimentD49=1"),"existing F3 formatter records exact experimental decision");ck(!fp::active(1000),"F3 observer remains off by default");}
 std::printf("fist_stroke_d49=%u/%u; recognizer/nomination only, native damage unclaimed\n",checks-failed,checks);return failed?1:0;
}
