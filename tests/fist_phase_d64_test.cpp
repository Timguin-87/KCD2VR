#include "vrik/fist_motion.h"
#include "vrik/fist_guard175.h"
#include <cstdio>
using namespace kcd2vr::fists;
static int checks=0,failures=0;
static void ck(bool b,const char* n){++checks;if(!b){++failures;std::printf("FAIL %s\n",n);}}
static Sample initial(int side,bool trial){Sample x{};x.epoch=x.sequence=1;x.tick=1000;x.actor=7;x.valid=x.empty[0]=x.empty[1]=true;x.head={0,1.6,0};x.controller[0]=x.allowed[0]={.22,1.3,-.30};x.controller[1]=x.allowed[1]={-.22,1.3,-.30};x.strokeExperimentD49=trial;(void)side;return x;}
static Punch advance(State&s,Sample&x,int side,pc::Vec3 d={},Trace* t=nullptr){++x.sequence;x.tick+=10;x.controller[side]=x.controller[side]+d;x.allowed[side]=x.allowed[side]+d;return step(s,x,t);}
static void ready(State&s,Sample&x,int side){step(s,x);x.grip[side]=x.trigger[side]=1;for(int i=0;i<20;++i)advance(s,x,side);}
static int moves(State&s,Sample&x,int side,pc::Vec3 d,int n){int count=0;for(int i=0;i<n;++i)count+=advance(s,x,side,d).valid;return count;}
int main(){
 for(bool trial:{false,true})for(int side:{0,1}){
  {State s{};auto x=initial(side,trial);ready(s,x,side);
   ck(moves(s,x,side,{0,0,.02},9)==0,"fresh 18cm retraction cannot create attack");
   ck(s.hand[side].phase==Phase::Ready,"unaccepted return does not own Spent recovery");
   ck(moves(s,x,side,{0,0,-.02},10)==1,"next 20cm extension remains immediately available");
   ck(moves(s,x,side,{0,0,-.02},10)==0,"accepted extension followthrough stays once-only");
   ck(moves(s,x,side,{0,0,.02},8)==0,"accepted strike return never attacks");
   ck(moves(s,x,side,{0,0,-.02},8)==1,"continuous next jab needs no 120ms stop");}
  {State s{};auto x=initial(side,trial);ready(s,x,side);
   ck(!advance(s,x,side,{0,0,-.01}).valid,"short candidate below travel");
   ck(!advance(s,x,side,{0,0,.025}).valid,"candidate reverses without qualification");
   ck(s.hand[side].phase==Phase::Ready,"failed candidate does not retain stale accepted-stroke axis");
   ck(moves(s,x,side,{0,0,-.02},10)==1,"recorded failure structure: next forward20cm not spent as return");}
  {State s{};auto x=initial(side,trial);ready(s,x,side);const double dx=side?.02:-.02;
   ck(moves(s,x,side,{dx,0,0},9)==1,"across-body hook stays valid while radial head distance decreases");}
  {State s{};auto x=initial(side,trial);ready(s,x,side);ck(moves(s,x,side,{0,.02,0},9)==1,"uppercut remains available");}
  {State s{};auto x=initial(side,trial);ready(s,x,side);int n=0;for(int i=0;i<20;++i){x.head.z-=.02;n+=advance(s,x,side,{0,0,-.02}).valid;}ck(!n,"common head/controller motion cannot supply strike");}
 }
 // Actual captured head-relative displacement while old state was Spent:
 // source352474->352518 =(-.0816456,+.1152689,-.2062542)m. This is an
 // independent directional counterexample, not a replay of unrecorded frames.
 ck(attackLegD64({-.0816456,.1152689,-.2062542}),"recorded forward leg is recognized as attack direction");
 ck(!attackLegD64({0,0,.18}),"straight return is not an attack direction");
 namespace g=kcd2vr::fist_guard175;
 for(double y:{-.22,-.30,-.46}){g::State s{};auto x=initial(0,false);x.grip[0]=x.grip[1]=x.trigger[0]=x.trigger[1]=1;x.controller[0]={.16,1.6+y,-.20};x.controller[1]={-.16,1.6+y,-.20};g::Decision d{};for(int i=0;i<20;++i){++x.sequence;x.tick+=10;d=g::update(s,x,false);}ck(d.block&&d.zone==5,"compact body envelope reaches native body sector");}
 for(pc::Vec3 bad:{pc::Vec3{.4,-.3,-.2},pc::Vec3{.16,-.65,-.2},pc::Vec3{.16,-.3,.1},pc::Vec3{.16,-.3,-.7}}){g::State s{};auto x=initial(0,false);x.grip[0]=x.grip[1]=x.trigger[0]=x.trigger[1]=1;x.controller[0]=x.head+bad;x.controller[1]=x.head+pc::Vec3{-bad.x,bad.y,bad.z};g::Decision d{};for(int i=0;i<20;++i){++x.sequence;x.tick+=10;d=g::update(s,x,false);}ck(!d.block,"side/hip/behind/extended poses cannot become compact body guard");}
 std::printf("fist_phase_d64=%d/%d (host policy; native outcomes and headset unclaimed)\n",checks-failures,checks);return failures?1:0;
}
