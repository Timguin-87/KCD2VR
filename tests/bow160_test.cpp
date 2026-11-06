// V66.160 manual bow, Trigger on the string: Grip takes/carries the arrow,
// Trigger at the nock holds the string, Trigger release shoots exactly once.
#include "vrik/physical_weapon.h"
#include <cstdio>
using namespace kcd2vr::physical;
static unsigned checks=0,failures=0;static void ck(bool b,const char*n){++checks;if(!b)++failures;std::printf("%s %s\n",b?"PASS":"FAIL",n);}
static int count(const Output& o,CommandKind k){int n=0;for(unsigned j=0;j<o.commandCount;++j)if(o.commands[j].kind==k)++n;return n;}
int main(){
 Input i{};i.epoch=i.bodyEpoch=i.sample=1;i.tick=1000;
 i.allowed=i.tracked[0]=i.tracked[1]=i.intentTracked[0]=i.intentTracked[1]=true;
 i.requirePrimaryGrip114=i.bowActions=true;i.contact=i.dynamics=i.attacks=false;
 i.quiverGesture114=i.bowRegrab116=i.bowNativeGrab146=i.bowRetention=i.bowStringTrigger160=true;i.bowTrigger=i.nativeBowTrigger126=false;i.quiverRadius146=.28;
 i.grip[1]=1;i.reach[0]=i.reach[1]=1.0;i.shoulders[0]={.2,0,1.3};i.shoulders[1]={-.2,0,1.3};
 i.hands[1].p={0,.4,1.3};i.hands[0].p={.35,.1,1.0};
 Profile bow{};bow.key=bow.generation=7;bow.side=1;bow.classId=9;bow.primary=bow.support=true;bow.grip[0].p={0,-.05,0};bow.axis={0,-1,0};
 std::array<Profile,2> profiles{Profile{},bow};State state{};
 auto sweep=[](const Profile&,Pose,Pose,bool){Contact103 c{};c.known=true;c.fraction=1;return c;};
 Output o{};int begins=0,fires=0,cancels=0;
 auto stepNext=[&](){++i.sample;i.tick+=10;o=step(state,i,profiles,sweep);begins+=count(o,CommandKind::BowBegin);fires+=count(o,CommandKind::BowFire);cancels+=count(o,CommandKind::BowCancel);};
 const Vec3 quiver=right_shoulder_zone(i),nock{0,.35,1.3};
 // spend the session's free first arrow with a trigger draw at the string
 i.grip[0]=0;i.trigger[0]=0;stepNext();i.hands[0].p=nock;i.trigger[0]=1;for(int n=0;n<5;++n)stepNext();
 for(int n=1;n<=15;++n){i.hands[0].p=nock+Vec3{0,-.02*n,0};stepNext();}
 i.trigger[0]=0;stepNext();
 ck(begins==1&&fires==1,"first arrow: Trigger at the string draws, Trigger release shoots");
 begins=fires=cancels=0;
 i.hands[0].p=nock;stepNext();i.grip[0]=1;for(int n=0;n<5;++n)stepNext();
 ck(begins==0,"after a shot, Grip at the string without a quiver arrow does nothing");
 i.grip[0]=0;stepNext();i.hands[0].p=quiver;stepNext();i.grip[0]=1;for(int n=0;n<6;++n)stepNext();
 ck(begins==1&&o.bowHeld,"Grip over the right shoulder takes the arrow (native load starts)");
 for(int n=1;n<=10;++n){i.hands[0].p=quiver+(nock-quiver)*(n/10.0);stepNext();}
 ck(cancels==0&&fires==0,"carrying with Grip never cancels or fires");
 for(int n=0;n<3;++n){i.hands[0].p=nock+Vec3{0,-.005*n,0};stepNext();}
 ck(o.bowFraction<.05,"at the nock without Trigger the string is not taken yet");
 i.trigger[0]=1;stepNext();i.grip[0]=0;
 for(int n=1;n<=20;++n){i.hands[0].p=nock+Vec3{0,-.02*n,0};stepNext();}
 ck(o.bowHeld&&o.bowFraction>.5&&cancels==0,"Trigger holds the string; Grip may be released; pull draws physically");
 i.trigger[0]=0;stepNext();
 ck(fires==1&&!o.bowHeld,"releasing the Trigger shoots exactly once");
 const auto release=state.weapon[1].lastBowRelease166;
 ck(release&&state.weapon[1].quiverSpent116,"a requested shot records its exact release identity");
 ck(!restore_rejected_bow166(state,999,7,i.epoch,i.bodyEpoch,release),"another item cannot restore the quiver ticket");
 ck(!restore_rejected_bow166(state,7,7,i.epoch,i.bodyEpoch,release-1),"an earlier rejection cannot undo a later release");
 ck(restore_rejected_bow166(state,7,7,i.epoch,i.bodyEpoch,release)&&!state.weapon[1].quiverSpent116,"explicit native dispatch rejection permits retry at the string");
 ck(!restore_rejected_bow166(state,7,7,i.epoch,i.bodyEpoch,release),"rejection is consumed once");
 i.hands[0].p=nock+Vec3{.08,0,.07};stepNext();i.trigger[0]=1;stepNext();
 ck(o.bowHeld&&o.bowFraction==0,"off-center Trigger latch starts at zero physical draw");
 for(int n=1;n<=15;++n){i.hands[0].p=nock+Vec3{.08,-.02*n,.07};stepNext();}
 ck(o.bowHeld&&o.bow116.lateral<1e-8&&o.bowFraction>.4,"latch offset is not treated as draw deviation");
 std::printf("bow160 %u checks, %u failures\n",checks,failures);return failures?1:0;}
