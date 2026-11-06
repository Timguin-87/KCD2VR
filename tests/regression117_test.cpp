#include "vrik/physical_guard112.h"
#include <cstdio>
#include <cstdlib>
using namespace kcd2vr::physical;
namespace g=kcd2vr::guard112;
static int count=0;
static void ck(bool v,const char* name){++count;if(!v){std::fprintf(stderr,"FAIL %s\n",name);std::exit(1);}}
static void next(Input& i){++i.sample;i.tick+=10;}
static Input input(){Input i{};i.allowed=i.tracked[0]=i.tracked[1]=true;i.epoch=i.bodyEpoch=i.sample=1;i.tick=1000;
 i.dynamics=i.contact=false;i.naturalCombat117=i.heldMotionAttacks=true;i.shoulders[0]={.2,0,1.3};i.shoulders[1]={-.2,0,1.3};i.reach[0]=.85;i.grip[0]=1;return i;}
static Profile profile(){Profile p{};p.key=p.generation=1;p.classId=4;p.primary=p.geometry=true;p.tip={.6,0,0};p.axis={1,0,0};return p;}
static auto clear=[](const Profile&,Pose,Pose,bool){return Contact103{};};
int main(){
 {auto i=input();auto p=profile();g::State s{};Pose weapon{{},{-.3,.4,1.3}};
  ck(!g::update(s,i,p,weapon,true).block,"new owner seeds continuity");next(i);
  ck(g::update(s,i,p,weapon,true).block,"centered transverse weapon blocks on first continuous sample without sector dwell");
  for(int n=0;n<100;++n){next(i);weapon.p.z+=n%2?.005:-.005;ck(g::update(s,i,p,weapon,true).block,"held guard survives small motion without re-press");}
  next(i);weapon.q=m::from_rotation_vector({0,0,1.57079632679});ck(!g::update(s,i,p,weapon,true).block,"pointing forward releases guard");
  next(i);weapon.q={};weapon.p.z=1.6;ck(g::update(s,i,p,weapon,true).block,"fast raise can enter guard immediately");
  next(i);i.requirePrimaryGrip114=true;i.grip[0]=0;ck(!g::update(s,i,p,weapon,true).block,"released primary grip cannot own block");
 }
 {auto i=input();auto p=profile();Pose weapon{{},{-.3,.4,1.5}};
  const auto q=m::from_rotation_vector({0,0,1.57079632679});
  for(auto& shoulder:i.shoulders)shoulder=m::rotate(q,shoulder);
  weapon=compose(Pose{q,{}},weapon);
  ck(natural_guard117(i,p,weapon,false),"guard follows Henry shoulder axes after a 90 degree body turn");
  weapon.p={0,0,1.5};ck(!natural_guard117(i,p,weapon,false),"weapon behind chest is not guard");
 }
 {auto i=input();auto p=profile();State s{};std::array<Profile,2> ps{p,Profile{}};i.hands[0].p={-.3,.4,1.5};
  auto o=step(s,i,ps,clear);ck(o.guard117,"initial posture reserves guard before queued input");
  for(int n=0;n<15;++n){next(i);i.hands[0].p.z+=n%2?.025:-.025;o=step(s,i,ps,clear);ck(o.guard117&&o.commandCount==0,"moving held guard cannot prepare or fire a swing");}
  // Turn the blade forward, then thrust from the guard without an extra quiet rearm.
  for(int n=1;n<=4;++n){next(i);i.hands[0].q=m::from_rotation_vector({0,0,1.57079632679*n/4});o=step(s,i,ps,clear);}
  bool prepare=false,commit=false;int zone=-1;
  for(int n=0;n<10;++n){next(i);i.hands[0].p.y+=.018;o=step(s,i,ps,clear);
   for(unsigned j=0;j<o.commandCount;++j){auto c=o.commands[j];
    if(c.kind==CommandKind::Prepare){prepare=true;zone=c.zone117;ck(c.stroke117==CommandKind::Thrust,"prepare already carries thrust classification");}
    if(c.kind==CommandKind::Thrust){commit=true;ck(c.zone117==zone&&zone==5,"commit retains its prepared native zone");}}
  }
  ck(prepare&&commit,"guard exit can become one deliberate thrust");
  const auto duplicate=step(s,i,ps,clear);ck(!duplicate.commandCount,"repeated eye/sample cannot repeat attack");
 }
 {auto i=input();auto p=profile();p.tip={0,.6,0};p.axis={0,1,0};i.hands[0].p={.65,.25,1.5};i.shoulders[0]={.3,0,1.3};State s{};std::array<Profile,2> ps{p,Profile{}};
  step(s,i,ps,clear);for(int n=0;n<10;++n){next(i);step(s,i,ps,clear);}
  int prepares=0,cuts=0;
  for(int n=0;n<12;++n){next(i);i.hands[0].p.x-=.025;auto o=step(s,i,ps,clear);
   for(unsigned j=0;j<o.commandCount;++j){const auto c=o.commands[j];prepares+=c.kind==CommandKind::Prepare;
    if(c.kind==CommandKind::Slash){++cuts;ck(c.zone117==2,"right-to-left physical cut requests right-origin native zone");}}}
  ck(prepares==1&&cuts==1,"a deliberate side cut is not swallowed by guard acquisition");
 }
 std::printf("regression117=%d/%d\n",count,count);
}
