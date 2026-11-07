// V66.169 retained bow-distance request comparison, not projectile-spawn proof.
#include "vrik/physical_weapon.h"
#include <cstdio>
using namespace kcd2vr::physical;
static unsigned checks=0,failures=0;static void ck(bool b,const char*n){++checks;if(!b)++failures;std::printf("%s %s\n",b?"PASS":"FAIL",n);}
static int count(const Output& o,CommandKind k){int n=0;for(unsigned j=0;j<o.commandCount;++j)if(o.commands[j].kind==k)++n;return n;}
// Manual bow, Trigger on the string, pull diagonally (lateral ~0.8 x along as in the V164R4 log).
static int bowRun(bool exp,int& fires){
 Input i{};i.epoch=i.bodyEpoch=i.sample=1;i.tick=1000;
 i.allowed=i.tracked[0]=i.tracked[1]=i.intentTracked[0]=i.intentTracked[1]=true;
 i.requirePrimaryGrip114=i.bowActions=true;i.contact=i.dynamics=i.attacks=false;
 i.quiverGesture114=i.bowRegrab116=i.bowNativeGrab146=i.bowRetention=i.bowStringTrigger160=true;i.bowTrigger=i.nativeBowTrigger126=false;i.quiverRadius146=.28;
 i.experiments169=exp;i.grip[1]=1;i.reach[0]=i.reach[1]=1.0;i.shoulders[0]={.2,0,1.3};i.shoulders[1]={-.2,0,1.3};
 i.hands[1].p={0,.4,1.3};i.hands[0].p={.35,.1,1.0};
 Profile bow{};bow.key=bow.generation=7;bow.side=1;bow.classId=9;bow.primary=bow.support=true;bow.grip[0].p={0,-.05,0};bow.axis={0,-1,0};
 std::array<Profile,2> ps{Profile{},bow};State st{};
 auto sw=[](const Profile&,Pose,Pose,bool){Contact103 c{};c.known=true;c.fraction=1;return c;};
 Output o{};int cancels=0;fires=0;const Vec3 nock{0,.35,1.3};
 auto step1=[&](){++i.sample;i.tick+=10;o=step(st,i,ps,sw);fires+=count(o,CommandKind::BowFire);cancels+=count(o,CommandKind::BowCancel);};
 i.trigger[0]=0;step1();i.hands[0].p=nock;i.trigger[0]=1;for(int n=0;n<5;++n)step1();
 for(int n=1;n<=20;++n){i.hands[0].p=nock+Vec3{.03*n,-.02*n,0};step1();} // pulled back AND sideways
 i.trigger[0]=0;step1();return cancels;}
int main(){
 int fires=0;const int cOff=bowRun(false,fires);const int firesOff=fires;
 const int cOn=bowRun(true,fires);
 ck(cOff>=1&&firesOff==0,"OFF (V168R1): a diagonal real-world pull is cancelled as DEVIATION (log behavior)");
 ck(cOn==0&&fires==1,"EXPERIMENT: the same pull draws by distance and Trigger release requests fire once");
 std::printf("experiments169 %u checks, %u failures\n",checks,failures);return failures?1:0;}
