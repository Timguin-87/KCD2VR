// V66.163: socket-centered support, 20 ms dwell, known collision approach required.
#include "vrik/physical_weapon.h"
#include <cstdio>
using namespace kcd2vr::physical;
static unsigned checks=0,failures=0;static void ck(bool b,const char*n){++checks;if(!b)++failures;std::printf("%s %s\n",b?"PASS":"FAIL",n);}
static bool run(Vec3 left,bool known,int frames){
 Input i{};i.epoch=i.bodyEpoch=i.sample=1;i.tick=1000;
 i.allowed=i.tracked[0]=i.tracked[1]=i.intentTracked[0]=i.intentTracked[1]=true;
 i.requirePrimaryGrip114=true;i.contact=true;i.dynamics=i.attacks=false;i.grip[0]=1;i.grip[1]=0;
 i.reach[0]=i.reach[1]=1.0;i.shoulders[0]={.2,0,1.3};i.shoulders[1]={-.2,0,1.3};
 i.hands[0].p={0,.4,1.1};
 Profile p{};p.key=p.generation=3;p.side=0;p.classId=4;p.primary=p.geometry=p.support=p.twoHanded=true;
 p.grip[0].p={0,0,0};p.grip[1].p={0,-.25,0};p.axis={0,1,0};p.tip={0,1,0};
 std::array<Profile,2> ps{p,Profile{}};State s{};
 auto sweep=[known](const Profile&,Pose,Pose,bool){Contact103 c{};c.known=known;c.fraction=known?1:0;return c;};
 Output o{};
 for(int n=0;n<3;++n){++i.sample;i.tick+=10;o=step(s,i,ps,sweep);}
 const Pose w=o.weapons[0];i.hands[1].p=point(w,left);
 ++i.sample;i.tick+=10;o=step(s,i,ps,sweep);i.grip[1]=1;
 for(int n=0;n<frames;++n){++i.sample;i.tick+=10;i.hands[1].p=point(o.weapons[0],left);o=step(s,i,ps,sweep);}
 return s.weapon[0].support;}
int main(){
 ck(run({0,-.25,0},true,4),"grip at the native support socket latches within 40 ms");
 ck(!run({.13,-.15,0},true,4),"grip outside the 14 cm support-socket zone does not latch");
 ck(run({0,-.32,0},true,4),"grip on the pommel, 7 cm past the support socket, latches");
 ck(!run({.30,-.15,0},true,6),"a hand 30 cm away from the handle does not latch");
 ck(!run({0,-.25,0},false,4),"an UNKNOWN approach cannot authorize a support hand through an obstacle");
 std::printf("support146 %u checks, %u failures\n",checks,failures);return failures?1:0;}
