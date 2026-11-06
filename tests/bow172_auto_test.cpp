#include "vrik/physical_weapon.h"
#include <array>
#include <cstdio>
using namespace kcd2vr::physical;
static int commands(const Output&o,CommandKind k){int n=0;for(unsigned j=0;j<o.commandCount;++j)n+=o.commands[j].kind==k;return n;}
int main(){
 Input i{};i.epoch=i.bodyEpoch=1;i.allowed=i.tracked[0]=i.tracked[1]=i.intentTracked[0]=i.intentTracked[1]=true;
 i.bowActions=i.bowStringTrigger160=i.bowRegrab116=true;i.bowTrigger=i.nativeBowTrigger126=i.quiverGesture114=i.bowNativeGrab146=false;
 i.bowRetention=true;i.grip[1]=1;i.reach[0]=i.reach[1]=1.0;i.shoulders[0]={.2,0,1.3};i.shoulders[1]={-.2,0,1.3};i.hands[1].p={0,.4,1.3};
 Profile bow{};bow.key=bow.generation=7;bow.side=1;bow.classId=9;bow.primary=bow.support=true;bow.grip[0].p={0,-.05,0};bow.axis={0,-1,0};
 std::array<Profile,2> profiles{Profile{},bow};State state{};
 auto sweep=[](const Profile&,Pose,Pose,bool){Contact103 c{};c.known=true;c.fraction=1;return c;};
 Output o{};int begins=0,fires=0,cancels=0;
 auto next=[&](){++i.sample;i.tick+=10;o=step(state,i,profiles,sweep);begins+=commands(o,CommandKind::BowBegin);fires+=commands(o,CommandKind::BowFire);cancels+=commands(o,CommandKind::BowCancel);};
 const Vec3 nock{0,.35,1.3};i.hands[0].p=nock;next();i.trigger[0]=1;next();
 for(int n=1;n<=16;++n){i.hands[0].p=nock+Vec3{0,-.02*n,0};next();}
 bool held=o.bowHeld&&o.bowFraction>.3;
 i.trigger[0]=0;next();bool fired=fires==1&&begins==1&&cancels==0;
 i.hands[0].p=nock;next();i.trigger[0]=1;next();for(int n=1;n<=16;++n){i.hands[0].p=nock+Vec3{0,-.02*n,0};next();}i.trigger[0]=0;next();
 bool repeat=fires==2&&begins==2&&cancels==0;
 std::printf("auto-string candidate held=%d first=%d repeat=%d beginnings=%d fires=%d cancels=%d\n",held,fired,repeat,begins,fires,cancels);
 return held&&fired&&repeat?0:1;
}
