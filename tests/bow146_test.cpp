// V66.146R1 true bow reload: quiver grab (native load at once), carry, nock,
// physical draw, release-to-shoot; cancel keeps the arrow; fire spends it.
#include "vrik/physical_weapon.h"
#include <cstdio>
#include "diagnostics/reload_milestones136r3.h"
using namespace kcd2vr::physical;
static unsigned checks=0,failures=0;static void ck(bool b,const char*n){++checks;if(!b)++failures;std::printf("%s %s\n",b?"PASS":"FAIL",n);}
static int count(const Output& o,CommandKind k){int n=0;for(unsigned j=0;j<o.commandCount;++j)if(o.commands[j].kind==k)++n;return n;}
int main(){
 ck(kcd2vr::diagnostics::reload136r3::NameMatches("bow_load_loop_over_player","bow_load_loop_over_player"),"exact reload alias matches");
 ck(!kcd2vr::diagnostics::reload136r3::NameMatches("bow_load_loop_over_player","bow_load_loop_over"),"player variant cannot inherit base events");
 ck(!kcd2vr::diagnostics::reload136r3::NameMatches("rifle_charging_phase2_to_idle_aim","rifle_charging_phase2"),"transition cannot inherit phase2 reload events");
 Input fallback{};Profile noString{};noString.classId=9;
 fallback.quiverGesture114=fallback.bowRegrab116=fallback.bowNativeGrab146=true;
 resolve_bow_mode146(fallback,noString);
 ck(fallback.nativeBowTrigger126&&fallback.bowTrigger&&!fallback.quiverGesture114&&!fallback.bowNativeGrab146,
    "missing string geometry selects RT for BOTH native dispatch and gesture producer");
 State prior{};prior.epoch=prior.bodyEpoch=3;prior.weapon[1].key=prior.weapon[1].generation=7;
 prior.weapon[1].bowHeld=prior.weapon[1].nativeBowMode146=true;
 Input arrived{};arrived.epoch=arrived.bodyEpoch=3;arrived.quiverGesture114=true;
 Profile learned{};learned.key=learned.generation=7;learned.classId=9;learned.support=true;
 resolve_bow_mode146(arrived,learned,&prior);
 ck(arrived.nativeBowTrigger126&&!arrived.quiverGesture114,"geometry arriving during RT draw cannot switch release ownership to Grip");
 Input i{};i.epoch=i.bodyEpoch=i.sample=1;i.tick=1000;
 i.allowed=i.tracked[0]=i.tracked[1]=i.intentTracked[0]=i.intentTracked[1]=true;
 i.requirePrimaryGrip114=i.bowActions=true;i.contact=i.dynamics=i.attacks=false;
 i.quiverGesture114=i.bowRegrab116=i.bowNativeGrab146=i.bowRetention=true;i.bowTrigger=i.nativeBowTrigger126=false;i.quiverRadius146=.28;
 i.grip[1]=1;i.reach[0]=i.reach[1]=1.0;
 i.shoulders[0]={.2,0,1.3};i.shoulders[1]={-.2,0,1.3};
 i.hands[1].p={0,.4,1.3};i.hands[0].p={.35,.1,1.0};
 Profile bow{};bow.key=bow.generation=7;bow.side=1;bow.classId=9;bow.primary=bow.support=true;
 bow.grip[0].p={0,-.05,0};bow.axis={0,-1,0};
 std::array<Profile,2> profiles{Profile{},bow};State state{};
 auto sweep=[](const Profile&,Pose,Pose,bool){Contact103 c{};c.known=true;c.fraction=1;return c;};
 Output o{};int begins=0,fires=0,cancels=0;
 auto stepNext=[&](){++i.sample;i.tick+=10;o=step(state,i,profiles,sweep);begins+=count(o,CommandKind::BowBegin);fires+=count(o,CommandKind::BowFire);cancels+=count(o,CommandKind::BowCancel);return o;};
 const Vec3 quiver=right_shoulder_zone(i),nock{0,.35,1.3};
 i.grip[0]=0;stepNext();
 // First draw of a session: the arrow already on the bow can be taken at the string.
 i.hands[0].p=nock;i.grip[0]=1;for(int n=0;n<5;++n)stepNext();
 for(int n=1;n<=15;++n){i.hands[0].p=nock+Vec3{0,-.02*n,0};stepNext();}
 i.grip[0]=0;stepNext();
 ck(begins==1&&fires==1,"first draw of a session needs no quiver (arrow already nocked)");
 begins=fires=cancels=0;
 i.hands[0].p=nock;stepNext();i.grip[0]=1;for(int n=0;n<5;++n)stepNext();
 ck(begins==0,"no arrow: gripping the string alone does not draw after a shot");
 i.grip[0]=0;stepNext();
 i.hands[0].p=quiver+Vec3{.05,0,.05};stepNext();i.grip[0]=1;for(int n=0;n<6;++n)stepNext();
 ck(begins==1&&o.bowHeld&&fires==0,"Grip over the right shoulder (28 cm zone) takes the arrow and starts the native load at once");
 for(int n=1;n<=10;++n){i.hands[0].p=quiver+(nock-quiver)*(n/10.0);stepNext();}
 ck(cancels==0&&fires==0&&o.bowHeld,"carrying the arrow from the shoulder to the string never cancels or fires");
 for(int n=1;n<=20;++n){i.hands[0].p=nock+Vec3{0,-.02*n,0};stepNext();}
 ck(o.bowHeld&&o.bowFraction>.5,"pulling back with Grip held draws physically");
 i.grip[0]=0;stepNext();
 ck(fires==1&&cancels==0&&!o.bowHeld,"releasing Grip shoots exactly once");
 // after a shot the quiver is required again
 i.hands[0].p=nock;stepNext();i.grip[0]=1;for(int n=0;n<5;++n)stepNext();
 ck(begins==1,"after a shot a new arrow must come from the quiver");
 // cancel during carry keeps the arrow: regrab at the string works without the quiver
 i.grip[0]=0;stepNext();i.hands[0].p=quiver;stepNext();i.grip[0]=1;for(int n=0;n<6;++n)stepNext();
 ck(begins==2,"second quiver grab starts a second native load");
 i.hands[0].p=quiver+(nock-quiver)*.5;stepNext();i.grip[0]=0;for(int n=0;n<3;++n)stepNext();
 ck(cancels==1&&fires==1,"letting go before the string cancels, never shoots");
 i.hands[0].p=nock;stepNext();i.grip[0]=1;for(int n=0;n<5;++n)stepNext();
 ck(begins==3&&o.bowHeld,"a cancelled arrow can be re-taken at the string without another quiver reach");
 std::printf("bow146 %u checks, %u failures\n",checks,failures);return failures?1:0;}
