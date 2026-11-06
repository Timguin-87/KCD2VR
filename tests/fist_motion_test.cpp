#include "../src/vrik/fist_motion.h"
#include <cstdio>
using namespace kcd2vr::fists;
int total=0,pass=0;void check(bool v,const char*n){++total;pass+=v;std::printf("%s %s\n",v?"PASS":"FAIL",n);}
Sample sample(){Sample x{};x.epoch=x.sequence=1;x.tick=1000;x.actor=123;x.valid=x.empty[0]=x.empty[1]=true;x.head={0,1.6,0};x.controller[0]=x.allowed[0]={.2,1.25,-.2};x.controller[1]=x.allowed[1]={-.2,1.25,-.2};return x;}
Punch advance(State&s,Sample&x,int dt=10){++x.sequence;x.tick+=dt;return step(s,x);}
void ready(State&s,Sample&x,int side=0){step(s,x);x.grip[side]=x.trigger[side]=1;for(int i=0;i<20;++i)advance(s,x);}
int punch(State&s,Sample&x,int side=0){int n=0;for(int i=0;i<12;++i){x.controller[side].z-=.018;x.allowed[side].z-=.018;n+=advance(s,x).valid;}return n;}
int main(){State s{};auto x=sample();ready(s,x);int n=0;for(int i=0;i<1000;++i)n+=advance(s,x).valid;check(n==0,"ten seconds holding closed fist causes no attacks");
 check(punch(s,x)==1,"deliberate closed-fist stroke produces one request");check(punch(s,x)==0,"follow-through does not repeat request");
 for(int i=0;i<10;++i){x.controller[0].z+=.025;x.allowed[0].z+=.025;advance(s,x);}for(int i=0;i<20;++i)advance(s,x);check(punch(s,x)==1,"retract and settle rearms while fist remains closed");
 x.grip[0]=x.trigger[0]=0;check(!advance(s,x).valid,"opening or releasing is not an attack");s={};x=sample();check(punch(s,x)==0,"moving open hand never punches");
 s={};x=sample();x.grip[0]=x.trigger[0]=1;ready(s,x);check(punch(s,x)==1,"mode entered closed quietly readies a NEW deliberate stroke");
 s={};x=sample();ready(s,x);n=0;for(int i=0;i<20;++i){x.head.z-=.02;x.controller[0].z-=.02;x.allowed[0].z-=.02;n+=advance(s,x).valid;}check(n==0,"common head and controller room movement is not a punch");
 s={};x=sample();ready(s,x);n=0;for(int i=0;i<20;++i){x.allowed[0].z-=.02;n+=advance(s,x).valid;}check(n==0,"native root locomotion without controller motion is not a punch");
 s={};x=sample();ready(s,x);x.blocked[0]=true;check(punch(s,x)==0,"already blocked fist cannot seed a new strike");
 s={};x=sample();ready(s,x);x.allowed[0].z=0;for(int i=0;i<20;++i){x.controller[0].z-=.02;check(!advance(s,x).valid,"controller tunneling past stationary allowed hand is ignored");}
 s={};x=sample();ready(s,x);x.controller[0].z-=1;x.allowed[0].z-=1;check(!advance(s,x).valid,"tracking jump is not a punch");check(punch(s,x)==0,"tracking recovery while held requires release");
 s={};x=sample();ready(s,x);x.empty[1]=false;check(punch(s,x)==0,"equipped opposite hand excludes unarmed-only prototype");
 s={};x=sample();ready(s,x);x.epoch++;check(punch(s,x)==0,"recenter cannot become punch");
 s={};x=sample();ready(s,x,1);check(punch(s,x,1)==1,"left controller also recognizes one physical punch");check(!step(s,x).valid,"duplicate eye consumption is inert");
 s={};x=sample();ready(s,x);advance(s,x,200);check(punch(s,x)==0,"short tracking gap rejects movement until fresh quiet settling");
 n=0;for(int i=0;i<20;++i)n+=advance(s,x).valid;
 check(n==0&&punch(s,x)==1,"short same-owner gap can settle and accept a NEW deliberate punch without opening fist");
 s={};x=sample();ready(s,x);punch(s,x);advance(s,x,150);n=0;
 for(int i=0;i<20;++i)n+=advance(s,x).valid;
 check(n==0,"spent punch is never replayed by recovery and settling");
 check(punch(s,x)==1,"spent fist requires new motion after recovery settling");
 s={};x=sample();ready(s,x);advance(s,x,251);for(int i=0;i<20;++i)advance(s,x);
 check(punch(s,x)==1,"long tracking loss needs fresh quiet then a NEW deliberate stroke");
 s={};x=sample();ready(s,x);x.valid=false;advance(s,x);x.valid=true;for(int i=0;i<20;++i)advance(s,x);
 check(punch(s,x)==1,"invalid ownership cannot borrow readiness but fresh quiet admits a NEW stroke");
 s={};x=sample();ready(s,x);const auto local=x.controller[0]-x.head;n=0;
 for(int i=1;i<=80;++i){x.headOrientation=pc::from_rotation_vector({0,.02*i,0});x.controller[0]=x.head+pc::rotate(x.headOrientation,local);x.allowed[0]=x.controller[0];n+=advance(s,x).valid;}
 check(n==0,"rigid body turn with hand fixed relative to torso is not a punch");
 for(bool responsive:{false,true}){
  s={};x=sample();x.responsiveCombatH27=responsive;x.grip[0]=x.trigger[0]=1;
  step(s,x);n=0;for(int i=0;i<13;++i)n+=advance(s,x).valid;
  check(n==0&&s.hand[0].phase==Phase::Ready,"closed-at-entry quiet is readiness only in normal and responsive combat");
  check(punch(s,x)==1,"post-quiet NEW stroke is offered once without opening the closed fist");
  check(punch(s,x)==0,"closed entry follow-through never replays the original stroke");
  s={};x=sample();x.responsiveCombatH27=responsive;x.grip[0]=x.trigger[0]=1;step(s,x);
  check(punch(s,x)==0&&punch(s,x)==0,"motion before closed-entry quiet cannot become an attack");
  n=0;for(int i=0;i<13;++i)n+=advance(s,x).valid;
  check(n==0&&punch(s,x)==1,"old pre-quiet travel is discarded and only the later NEW stroke counts");
  s={};x=sample();x.responsiveCombatH27=responsive;x.grip[0]=x.trigger[0]=.60f;step(s,x);
  for(int i=0;i<20;++i)advance(s,x);
  check(punch(s,x)==0,"exact60percent cannot use quiet readiness to bypass strict squeeze");
  s={};x=sample();x.responsiveCombatH27=responsive;x.grip[0]=1;x.trigger[0]=0;step(s,x);
  for(int i=0;i<20;++i)advance(s,x);
  check(punch(s,x)==0,"Grip-only closed entry cannot punch");
  s={};x=sample();x.responsiveCombatH27=responsive;x.grip[0]=0;x.trigger[0]=1;step(s,x);
  for(int i=0;i<20;++i)advance(s,x);
  check(punch(s,x)==0,"Trigger-only closed entry cannot punch");
  s={};x=sample();x.responsiveCombatH27=responsive;x.grip[0]=x.trigger[0]=1;step(s,x);
  for(int i=0;i<10;++i)advance(s,x);x.epoch++;advance(s,x);
  check(punch(s,x)==0,"owner change restarts quiet and cannot consume old readiness or travel");
 }
 // R114 (R113 log 00:42:16-00:42:35): slow guard drift that covers the 10 cm floor is not a punch; real punches still are.
 for(bool responsive:{false,true})for(double mps:{.55,.75,.95}){
  s={};x=sample();x.responsiveCombatH27=responsive;ready(s,x);n=0;
  for(int i=0;i<40;++i){x.controller[0].z-=mps*.01;x.allowed[0].z-=mps*.01;n+=advance(s,x).valid;}
  check(n==0,"R114 guard drift below 1 m/s over 10+ cm never punches");
  for(int i=0;i<3;++i){x.controller[0].z-=.012;x.allowed[0].z-=.012;n+=advance(s,x).valid;}
  check(n==0,"R114 a short quick twitch cannot borrow the slow drift's travel");
  for(int i=0;i<10;++i){x.controller[0].z+=.02;x.allowed[0].z+=.02;advance(s,x);}for(int i=0;i<20;++i)advance(s,x);
  check(punch(s,x)==1,"R114 a real 1.8 m/s punch after guard drift still produces one request");}
 s={};x=sample();ready(s,x);n=0;for(int i=0;i<9;++i){x.controller[0].z-=.0125;x.allowed[0].z-=.0125;n+=advance(s,x).valid;}
 check(n==1,"R114 a short 1.25 m/s jab (R113 slowest real punch) is still a punch");
 std::printf("fist_motion=%d/%d\n",pass,total);return pass!=total;
}
