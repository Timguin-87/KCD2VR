#include "../src/vrik/physical_weapon.h"
#include <cstdio>
#include <limits>
using namespace kcd2vr::physical;
namespace m=kcd2vr::pose_core;
static unsigned checks=0,failures=0;
static void check(bool b,const char* name){++checks;if(!b)++failures;std::printf("%s %s\n",b?"PASS":"FAIL",name);}
static auto clear=[](const Profile&,Pose,Pose,bool){return 1.0;};
static Input input(){Input i{};i.allowed=true;i.epoch=i.bodyEpoch=1;i.sample=1;i.tick=1000;i.tracked[0]=i.tracked[1]=true;
 i.hands[0].p={.2,.3,1.3};i.hands[1].p={.05,.3,1.3};i.shoulders[0]={.2,0,1.3};i.shoulders[1]={-.2,0,1.3};return i;}
static std::array<Profile,2> profiles(){std::array<Profile,2> p{};auto& a=p[0];a.key=1;a.entity=42;a.generation=1;a.side=0;a.classId=4;a.primary=a.support=a.geometry=a.twoHanded=true;a.tip={0,.9,0};a.axis={0,1,0};a.extent=1;a.weight=2;a.grip[1].p={-.15,0,0};p[1].side=1;return p;}
static void advance(Input& i,unsigned ms=11){++i.sample;i.tick+=ms;}
static bool has(const Output& o,CommandKind k){for(unsigned i=0;i<o.commandCount;++i)if(o.commands[i].kind==k)return true;return false;}
static double drag(double weight,int rate,bool two=false){auto p=profiles();p[0].weight=weight;auto i=input();State s{};step(s,i,p,clear);double ms=1000;Output o{};
 for(int n=1;n<=rate/10;++n){i.sample++;i.tick=std::uint64_t(ms+1000.0*n/rate);double t=double(n)/rate;i.hands[0].q=m::from_rotation_vector({0,0,t*1.6});if(two)i.grip[1]=1;o=step(s,i,p,clear);}return m::angle(o.weapons[0].q,i.hands[0].q);}
int main(){
 {auto p=profiles();p[1]=p[0];p[1].side=1;p[1].key=99;p[1].classId=11;p[1].twoHanded=false;p[1].support=false;p[0]={};
  auto i=input();i.grip[0]=i.grip[1]=0;i.requirePrimaryGrip114=true;i.dynamics=false;i.contact=false;State s{};
  auto o=step(s,i,p,clear);advance(i,16);o=step(s,i,p,clear);
  check(o.active[1]&&o.owned[1],"native-held torch follows the left wrist without held Grip");
 }

 {Command generic{};check(!generic.recognizerAbort18,"generic Abort provenance defaults hard; reach/tracking/ownership cannot authorize late contact");}

 {auto p=profiles();auto i=input();i.dynamics=false;i.contact=false;State state{};step(state,i,p,clear);
  advance(i,16);i.grip[1]=1;auto out=step(state,i,p,clear);check(!out.support[0],"manual support requires stable Grip dwell, not an instant latch");
  for(int n=0;n<8;++n){advance(i,10);out=step(state,i,p,clear);}const auto tick=state.weapon[0].lastTick;
  ++i.sample;out=step(state,i,p,clear);check(out.support[0]&&out.owned[1]&&out.active[0]&&state.weapon[0].lastTick==tick,"new tracking sequence at equal timer value preserves latched support and primary ownership");
  ++i.sample;p[0]={};out=step(state,i,p,clear);check(!out.active[0]&&!out.owned[0],"zero elapsed time never hides actual item revocation");
 }
 {auto p=profiles();auto i=input();i.haveWorld=true;State state{};step(state,i,p,clear);++i.sample;i.world.p.x=.5;
  auto out=step(state,i,p,[&](const Profile& p,Pose a,Pose b,bool){double from=point(a,p.tip).x+i.world.p.x,to=point(b,p.tip).x+i.world.p.x;return to>.45&&to>from?std::clamp((.45-from)/(to-from),0.,1.):1.;});
  check(out.blocked[0]&&std::abs(out.hands[0].p.x+i.world.p.x-.45)<1e-8,"equal-tick root movement still uses world collision, not stale model-space contact");
 }

 { // R59.16C10 support-latch probe: observation only, reasons in the order the manual latch rules reject.
  auto p=profiles();auto i=input();State s{};auto o=step(s,i,p,clear);advance(i);o=step(s,i,p,clear); // first frame seeds ownership only
  check(o.supportProbe59c10.side==0&&o.supportProbe59c10.reason==5&&!o.supportProbe59c10.otherKeyed&&o.supportProbe59c10.profileSupport,"probe: open off hand near the hilt reports GRIP_BELOW_0.55");
  advance(i);i.grip[1]=1;o=step(s,i,p,clear);check(o.supportProbe59c10.reason==8,"probe: squeezing inside 14 cm reports DWELL until the latch");
  for(int n=0;n<8;++n){advance(i,10);o=step(s,i,p,clear);}check(o.support[0]&&o.supportProbe59c10.reason==0,"probe: latched = reason 0");
  advance(i);i.hands[1].p.x-=.30;o=step(s,i,p,clear);check(!o.support[0]&&o.supportProbe59c10.reason==4&&o.supportProbe59c10.rearm,"probe: release by distance leaves REARM pending deliberate return or Grip open");
  advance(i);i.hands[1].p.x+=.30;o=step(s,i,p,clear);check(!o.support[0]&&s.weapon[0].supportRearm,"C10R2 return while squeezing does not relatch immediately");
  for(int n=0;n<14;++n){advance(i,10);o=step(s,i,p,clear);}
  check(!o.support[0]&&s.weapon[0].supportRearm,"C10R2 sub150ms return keeps support rearm");
  for(int n=0;n<8;++n){advance(i,10);o=step(s,i,p,clear);}
  check(o.support[0],"C10R2 distance-only return rearms after150ms plus normal clear approach and dwell");
  advance(i);i.hands[1].p.x-=.30;o=step(s,i,p,clear);
  advance(i);i.grip[1]=0;o=step(s,i,p,clear);advance(i);i.grip[1]=1;o=step(s,i,p,clear);check(o.supportProbe59c10.reason==6&&o.supportProbe59c10.gap>.14,"probe: far squeeze reports GAP_OVER_14CM");
  advance(i);p[1].key=9;p[1].classId=8;p[1].side=1;o=step(s,i,p,clear);check(o.supportProbe59c10.reason==2&&o.supportProbe59c10.otherKeyed,"probe: shield in the off hand reports OTHER_HAND_HOLDS_ITEM");
  advance(i);p[1]={};i.tracked[1]=false;o=step(s,i,p,clear);check(o.supportProbe59c10.reason==3,"probe: untracked off hand reports OTHER_HAND_UNTRACKED");
  advance(i);i.tracked[1]=true;p[0].support=false;o=step(s,i,p,clear);check(o.supportProbe59c10.reason==1,"probe: no support socket reports NO_SUPPORT_SOCKET");
  advance(i);p[0].support=true;i.autoSupport=true;o=step(s,i,p,clear);check(o.supportProbe59c10.reason==9,"probe: auto-support path is reported as such");
 }
 for(bool occupied:{false,true}){
  auto p=profiles();auto i=input();State s{};step(s,i,p,clear);advance(i);i.grip[1]=1;
  Output o{};for(int n=0;n<9;++n){advance(i,10);o=step(s,i,p,clear);}check(o.support[0],"C10R2 loss fixture begins latched");
  // A simultaneous far hand and tracking/item loss must not be labelled a
  // distance-only release. Recovering its tracking/item is not a new grasp.
  advance(i);i.hands[1].p.x-=.3;
  if(occupied)p[1].key=9;else i.tracked[1]=false;
  o=step(s,i,p,clear);
  check(!o.support[0]&&!s.weapon[0].supportDistanceRelease59c10,"C10R2 tracked ownership loss cannot mint distance-return permission");
  advance(i);p[1]={};i.tracked[1]=true;i.hands[1].p.x+=.3;
  for(int n=0;n<25;++n){advance(i,10);o=step(s,i,p,clear);}
  check(!o.support[0]&&s.weapon[0].supportRearm,"C10R2 lost tracking or occupied offhand still needs an open Grip");
  advance(i);i.grip[1]=0;step(s,i,p,clear);i.grip[1]=1;
  for(int n=0;n<9;++n){advance(i,10);o=step(s,i,p,clear);}check(o.support[0],"C10R2 explicit Grip release recovers ownership after tracking/item loss");
  advance(i);i.hands[1].p.x-=.3;step(s,i,p,clear);
  check(s.weapon[0].supportDistanceRelease59c10,"C10R2 genuine distance release creates bounded return permission");
  advance(i);if(occupied)p[1].key=9;else i.tracked[1]=false;step(s,i,p,clear);
  advance(i);p[1]={};i.tracked[1]=true;i.hands[1].p.x+=.3;
  for(int n=0;n<25;++n){advance(i,10);o=step(s,i,p,clear);}
  check(!o.support[0]&&!s.weapon[0].supportDistanceRelease59c10,"C10R2 later tracking/item loss revokes an unused distance-return permission");
 }
 {auto p=profiles();auto i=input();State s{};auto o=step(s,i,p,clear);check(o.active[0]&&!o.active[1],"one owned native item, empty other hand");check(m::length(o.hands[0].p-i.hands[0].p)<1e-9,"primary mapping unchanged at initial ownership");check(!o.commandCount,"initialization never emits attack");
  advance(i);i.grip[1]=1;o=step(s,i,p,clear);check(!o.support[0],"held grip near hilt does not latch before dwell");
  for(int n=0;n<8;++n){advance(i,10);o=step(s,i,p,clear);}check(o.support[0]&&o.owned[1],"support acquired after stable held-grip dwell at native relation");
  const auto first=s;auto repeat=step(s,i,p,clear);check(repeat.commandCount==0&&s.weapon[0].lastTick==first.weapon[0].lastTick,"same sample has no repeated integration or commands");
  const auto supportBefore=compose(o.weapons[0],p[0].grip[1]).p;i.hands[1].p.y+=.08;advance(i);o=step(s,i,p,clear);check(m::length(compose(o.weapons[0],p[0].grip[0]).p-o.hands[0].p)<1e-9,"primary derives from one weapon");
  const auto supportAfter=compose(o.weapons[0],p[0].grip[1]).p;check(m::length(supportAfter-i.hands[1].p)<m::length(supportBefore-i.hands[1].p)&&m::angle(o.weapons[0].q,compose(o.hands[0],inverse(p[0].grip[0])).q)<=i.supportMaxAngle+.001,"support hand only nudges the same rigid weapon within bounded angular authority");
  advance(i);i.grip[1]=0;o=step(s,i,p,clear);check(!o.support[0]&&!o.owned[1],"support release preserves primary");
  advance(i);i.grip[1]=1;i.hands[1].p=compose(o.weapons[0],p[0].grip[1]).p;o=step(s,i,p,clear);check(!o.support[0],"rearmed press still observes support dwell");
  for(int n=0;n<8;++n){advance(i,10);o=step(s,i,p,clear);}check(o.support[0],"one deliberate release is enough to rearm a later press after dwell");
  advance(i);i.grip[1]=0;step(s,i,p,clear);advance(i);i.grip[1]=1;o=step(s,i,p,clear);for(int n=0;n<8;++n){advance(i,10);o=step(s,i,p,clear);}check(o.support[0],"explicit release rearms support with stable reacquire dwell");
  advance(i);p[1].key=2;o=step(s,i,p,clear);check(!o.support[0],"occupied opposite hand cannot support");
  advance(i);p[0]={};o=step(s,i,p,clear);check(!o.active[0]&&!o.owned[0],"sheath removes physical ownership");}
 {auto p=profiles();auto i=input();State s{};step(s,i,p,clear);advance(i);i.grip[1]=1;auto query=[](auto&,auto,auto,bool){return 0.0;};auto o=step(s,i,p,query);for(int n=0;n<8;++n){advance(i,10);o=step(s,i,p,query);}check(!o.support[0],"obstructed support acquire rejected after dwell");}
 {auto p=profiles();auto i=input();State s{};step(s,i,p,clear);advance(i);i.hands[0].p.y+=.05;auto before=s.weapon[0].weapon;auto o=step(s,i,p,[](auto&,auto,auto,bool){return 0.0;});check(o.blocked[0]&&m::length(o.weapons[0].p-before.p)<1e-9,"blocked sweep freezes actual weapon, not raw controller");check(!has(o,CommandKind::Slash)&&!has(o,CommandKind::Thrust),"blocked weapon cannot request strike");
  advance(i);o=step(s,i,p,[](auto&,auto,auto,bool){return std::numeric_limits<double>::quiet_NaN();});check(o.blocked[0],"invalid query is not clear");}
 {auto p=profiles();auto i=input();State s{};step(s,i,p,clear);advance(i);i.hands[0].p={3,4,5};auto o=step(s,i,p,clear);check(m::length(o.hands[0].p-i.shoulders[0])<=i.reach[0]+1e-8,"extreme reach clamps without stretching anatomy");check(o.commandCount==0,"tracking discontinuity does not swing");advance(i,200);o=step(s,i,p,clear);check(!o.commandCount,"long tracking gap does not swing");}
 {auto p=profiles();auto i=input();State s{};i.hands[0].q.x=std::numeric_limits<double>::quiet_NaN();auto o=step(s,i,p,clear);check(!o.active[0],"nonfinite controller rejected");i=input();p[0].weight=std::numeric_limits<double>::quiet_NaN();o=step(s,i,p,clear);check(!o.active[0],"nonfinite mass proxy rejected");}
 check(drag(8,90)>drag(.4,90),"heavier proxy increases rotational lag");
 check(std::abs(drag(2,60)-drag(2,120))<.06,"dt-aware inertia comparable at 60 and 120 samples/sec");
 {auto p=profiles();auto i=input();State s{};step(s,i,p,clear);Output o{};for(int n=0;n<200;++n){advance(i,10);i.hands[0].p.x+=.001;o=step(s,i,p,clear);}check(m::length(o.hands[0].p-i.hands[0].p)<.012,"slow deliberate translation remains precise");}
 {std::array<Profile,2> p{};p[0].side=0;auto& bow=p[1];bow.key=3;bow.generation=1;bow.side=1;bow.classId=9;bow.primary=bow.support=bow.geometry=true;bow.grip[0].p={0,-.15,0};bow.axis={0,-1,0};bow.tip={0,0,.6};bow.extent=1.2;
  auto i=input();i.hands[1].p={0,.4,1.3};i.hands[0].p={0,.25,1.3};State s{};auto o=step(s,i,p,clear);advance(i);i.grip[0]=1;o=step(s,i,p,clear);check(o.bowHeld&&has(o,CommandKind::BowBegin),"right grip acquires left-hand bow nock");
  advance(i);i.hands[0].p.y-=.25;o=step(s,i,p,clear);check(o.bowHeld&&o.bowFraction>.4,"projected separation draws bow");double before=o.bowFraction;advance(i);i.hands[0].p.x+=.1;o=step(s,i,p,clear);check(std::abs(o.bowFraction-before)<1e-9,"lateral motion is not bow draw progress");advance(i);i.grip[0]=0;o=step(s,i,p,clear);check(has(o,CommandKind::BowFire)&&!o.bowHeld,"deliberate release emits one native fire request");o=step(s,i,p,clear);check(!has(o,CommandKind::BowFire),"duplicate sample never fires twice");
  advance(i);i.hands[0].p={0,.25,1.3};step(s,i,p,clear);advance(i);i.grip[0]=1;o=step(s,i,p,clear);check(o.bowHeld,"bow rearm after physical release");advance(i);i.tracked[0]=false;o=step(s,i,p,clear);check(has(o,CommandKind::BowCancel)&&!has(o,CommandKind::BowFire),"tracking loss cancels, never fires");}
 {HolsterState s{};Vec3 mount{0,0,1},hand=mount;check(holster(s,1,hand,mount,0,false,true,1000)==CommandKind::None,"empty-hand holster initial rearm");holster(s,1,hand,mount,1,false,true,1010);hand.x=.25;check(holster(s,1,hand,mount,1,false,true,1150)==CommandKind::Draw,"grip and pull from actual mount requests draw");
 check(holster(s,1,hand,mount,1,false,true,1200)==CommandKind::None,"draw gesture one shot");holster(s,1,hand,mount,1,true,true,2000);check(holster(s,1,hand,mount,0,true,true,2010)==CommandKind::None,"ordinary release away does not sheath");holster(s,1,mount,mount,1,true,true,3000);check(holster(s,1,mount,mount,0,true,true,3010)==CommandKind::Holster,"return and release at own mount holsters");check(holster(s,0,mount,mount,1,false,false,4000)==CommandKind::None,"empty mount never creates phantom weapon");}
 {auto p=profiles();auto i=input();State s{};step(s,i,p,clear);advance(i);i.grip[0]=1;step(s,i,p,clear);bool attack=false,prepared=false;
  for(int n=0;n<35;++n){advance(i,10);i.hands[0].p.y+=.011;auto o=step(s,i,p,clear);prepared=prepared||has(o,CommandKind::Prepare);attack=attack||has(o,CommandKind::Slash)||has(o,CommandKind::Thrust);}check(prepared,"deliberate travel prepares native attack");
  bool abort=false;for(int n=0;n<100;++n){advance(i,10);auto o=step(s,i,p,clear);abort=abort||has(o,CommandKind::Abort);}check(attack||abort,"prepared gesture eventually attacks or aborts");}
 {auto p=profiles();auto i=input();i.haveWorld=true;State s{};step(s,i,p,clear);
  advance(i);i.world.p.x=.5;
  auto o=step(s,i,p,[&](const Profile& p,Pose a,Pose b,bool){const double from=point(a,p.tip).x+i.world.p.x,to=point(b,p.tip).x+i.world.p.x;return to>.45&&to>from?std::clamp((.45-from)/(to-from),0.0,1.0):1.0;});
  check(o.blocked[0]&&std::abs(o.hands[0].p.x+i.world.p.x-.45)<1e-8,"root locomotion sweep stops actual world weapon at wall");
  check(!has(o,CommandKind::Slash)&&!has(o,CommandKind::Thrust),"root motion into obstacle cannot generate attack");
 }
 {auto p=profiles();p[0].weight=.4;auto i=input();State s{};step(s,i,p,clear);advance(i);i.grip[0]=1;step(s,i,p,clear);int prep=0,slash=0;
  for(int n=1;n<50;++n){advance(i,10);i.hands[0].q=m::from_rotation_vector({0,0,.04*n});auto o=step(s,i,p,clear);prep+=has(o,CommandKind::Prepare);slash+=has(o,CommandKind::Slash);}
  check(prep==1&&slash==1,"continuous physical slash emits one prepare and one attack request");
 }
 {auto p=profiles();p[0].weight=.3;auto i=input();i.hands[0].p.y=.05;State s{};step(s,i,p,clear);advance(i);i.grip[0]=1;step(s,i,p,clear);int thrust=0;
  for(int n=1;n<20;++n){advance(i,10);i.hands[0].p.y+=.028;auto o=step(s,i,p,clear);thrust+=has(o,CommandKind::Thrust);}
  check(thrust==1,"forward tip motion recognizes deliberate thrust once");
 }
 {HolsterState s{};Vec3 mount{0,0,1};holster(s,1,mount,mount,1,true,true,1000);holster(s,1,mount,mount,.4,true,true,1010);
  check(holster(s,1,mount,mount,0,true,true,1020)==CommandKind::Holster,"analog grip release retains hysteresis through midrange");
 }
 {auto p=profiles();auto i=input();State s{};i.hands[0].q.w=2;
  check(!step(s,i,p,clear).active[0],"non-unit controller quaternion is rejected, never scales grip geometry");}
 {auto p=profiles();auto i=input();i.dynamics=i.contact=i.attacks=i.bowActions=false;State s{};int sweeps=0,commands=0;
  auto query=[&](const Profile&,Pose,Pose,bool){++sweeps;return 0.0;};step(s,i,p,query);Output o{};
  for(int n=0;n<30;++n){advance(i,10);i.hands[0].q=m::from_rotation_vector({0,0,.03*n});i.grip[0]=1;o=step(s,i,p,query);commands+=o.commandCount;}
  check(sweeps==0&&commands==0,"F12 handling gate performs no static queries or attack requests");
  check(m::angle(o.weapons[0].q,i.hands[0].q)<1e-5,"handling-only mode does not secretly apply inertia");
  i.contact=i.dynamics=true;advance(i);step(s,i,p,query);check(sweeps>0,"weight/contact rung enables bounded query path separately");
  check(!has(o,CommandKind::Slash)&&!has(o,CommandKind::Thrust),"handling rung cannot emit gesture attacks");
 }
 {std::array<Profile,2> p{};p[0].side=0;auto& bow=p[1];bow.key=3;bow.generation=1;bow.side=1;bow.classId=9;bow.primary=bow.support=bow.geometry=true;bow.grip[0].p={0,-.15,0};bow.axis={0,-1,0};bow.tip={0,0,.6};bow.extent=1.2;
  auto i=input();i.hands[1].p={0,.4,1.3};i.hands[0].p={0,.25,1.3};i.bowActions=false;State s{};step(s,i,p,clear);advance(i);i.grip[0]=1;auto o=step(s,i,p,clear);
  check(!o.bowHeld&&!has(o,CommandKind::BowBegin)&&o.active[1],"native left-bow placement works below bow-action rung without nocking");
 }
 std::printf("%u checks, %u failures\n",checks,failures);return failures?1:0;
}
