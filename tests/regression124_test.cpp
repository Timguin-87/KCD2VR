#include "vrik/weapon_sweep112.h"
#include <cstdio>
#include <cstdlib>
using namespace kcd2vr::physical;
static unsigned checks=0;
static void ck(bool ok,const char* name){++checks;if(!ok){std::fprintf(stderr,"FAIL %s\n",name);std::exit(1);}}
int main(){
 Input i{};i.allowed=i.tracked[0]=i.intentTracked[0]=true;i.epoch=i.bodyEpoch=i.sample=1;i.tick=1000;
 i.haveWorld=i.controllerAnchor119=i.contact=i.requirePrimaryGrip114=true;i.grip[0]=1;i.dynamics=false;
 i.shoulders[0]={0,0,1.3};i.hands[0].p={0,.45,1.3};i.reach[0]=.53;
 Profile p{};p.key=p.generation=1;p.primary=p.geometry=p.nativeSeed=true;p.side=0;p.classId=4;
 p.tip={0,.8,0};p.nativeWeapon.p={0,.75,1.3};p.nativeWeapon.q=m::from_rotation_vector({0,0,2.6});
 std::array<Profile,2> ps{p,Profile{}};State s{};bool reached=false;unsigned waits=0;
 auto clear=[](const Profile& p,Pose a,Pose b,bool acquire){return sweep_weapon112(p,a,b,acquire,[](Vec3,Vec3,double){return Contact103{};});};
 for(int n=0;n<20;++n){auto o=step(s,i,ps,clear);
  ck(o.commandCount==0,"acquisition cannot produce combat commands");
  if(o.active[0])ck(m::length(o.hands[0].p-i.shoulders[0])<=i.reach[0]+.001,"acquisition never stretches an arm");else ++waits;
  if(o.active[0]&&o.contactKnown[0]&&m::angle(o.weapons[0].q,i.hands[0].q)<1.e-6){reached=true;break;}
  ++i.sample;i.tick+=10;
 }
 ck(waits>0&&reached,"native seed outside solved shoulder reach converges through checked prefixes");
 s={};++i.sample;i.tick+=10;
 auto unknown=[](const Profile&,Pose,Pose,bool){Contact103 c{};c.known=false;c.fraction=0;return c;};
 for(int n=0;n<8;++n){auto o=step(s,i,ps,unknown);
  ck(o.active[0]&&o.handlingOnly55c1[0]&&!o.commandCount&&!attack_authority55c1(i,o,0)&&
     m::length(s.weapon[0].weapon.p-p.nativeWeapon.p)<1e-9&&m::length(o.hands[0].p-i.hands[0].p)<1e-9,
     "unknown acquisition permits current display only, never checked-seed motion or attacks");++i.sample;i.tick+=10;}
 auto blocked=[](const Profile&,Pose,Pose,bool){Contact103 c{};c.fraction=0;c.plane=true;c.normal={0,-1,0};return c;};
 for(int n=0;n<8;++n){auto o=step(s,i,ps,blocked);ck(!o.active[0]&&!o.commandCount,"wall outside reach cannot grant an impossible pose");++i.sample;i.tick+=10;}
 s={};step(s,i,ps,clear);++i.sample;i.tick+=10;i.grip[0]=0;
 auto o=step(s,i,ps,clear);ck(!o.active[0]&&!s.weapon[0].ready,"release clears pending acquisition");
 i.grip[0]=1;s={};++i.sample;i.tick+=10;step(s,i,ps,clear);
 const auto progressed=s.weapon[0].weapon;
 ++i.sample;i.tick+=90;Pose restarted{};
 step(s,i,ps,[&](const Profile&,Pose a,Pose,bool){restarted=a;Contact103 c{};c.known=false;c.fraction=0;return c;});
 ck(m::length(restarted.p-p.nativeWeapon.p)<1.e-8&&m::length(restarted.p-progressed.p)>.001,
    "pending seed is not reused after the 80 ms freshness limit");
 std::printf("PASS %u regression124 checks\n",checks);
}
