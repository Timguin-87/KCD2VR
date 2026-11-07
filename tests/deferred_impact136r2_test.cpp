#include "../src/vrik/physical_weapon.h"
#include <cstdio>
using namespace kcd2vr::physical; namespace m=kcd2vr::pose_core;
static int n=0,f=0;static void ck(bool b,const char*s){++n;if(!b)++f;std::printf("%s %s\n",b?"PASS":"FAIL",s);}
int main(){
 std::array<Profile,2> p{};auto& w=p[0];w.key=1;w.generation=1;w.side=0;w.classId=4;w.primary=w.geometry=true;w.tip={0,.9,0};w.axis={0,1,0};w.extent=1;w.weight=2;
 Input i{};i.allowed=true;i.epoch=i.bodyEpoch=1;i.sample=1;i.tick=1000;i.tracked[0]=true;i.hands[0].p={.2,.3,1.3};i.shoulders[0]={.2,0,1.3};i.controllerAnchor119=true;i.contact=true;i.dynamics=true;i.attacks=false;
 State s{};auto clear=[](const Profile&,Pose,Pose,bool){return Contact103{};};(void)step(s,i,p,clear);
 ++i.sample;i.tick+=10;i.hands[0].p.y+=.15;
 auto deferred=[](const Profile& p,Pose,Pose b,bool){Contact103 c{};c.known=true;c.limited115=true;c.fraction=.4;c.feature=point(b,p.tip);return c;};
 auto o=step(s,i,p,deferred);
 ck(o.blocked[0]&&o.sweepDeferred115[0]&&!o.contactKnown[0],"partial bounded sweep remains deferred, never promoted to a plane/contact");
 ck(o.impactSpeed[0]==0,"budget-limited motion is not evidence of an impact");
 ck(!o.commandCount,"deferred obstruction haptic does not manufacture attack/damage commands");
 std::printf("deferred_impact136r2=%d/%d\n",n-f,n);return f?1:0;
}
