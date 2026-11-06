// Executes the actual physical kernel. Source of the initial lever replay:
// independent overnight-review-20261019/heavy_bow.cpp; no retail execution.
#include "vrik/physical_weapon.h"
#include <cstdio>
namespace ph=kcd2vr::physical;namespace m=kcd2vr::pose_core;
static unsigned checks=0,failed=0;
static void ck(bool yes,const char* name){++checks;if(!yes)++failed;std::printf("%s %s\n",yes?"PASS":"FAIL",name);}
struct Lever63 {
 ph::Input i{};ph::State s{};std::array<ph::Profile,2> p{};ph::Output o{};
 unsigned prepares=0,commits=0;double travel=0,visiblePeak=0,intentPeak=0;bool relativeFixed=false;
 Lever63(){
  i.epoch=i.bodyEpoch=1;i.tick=1000;i.allowed=i.tracked[0]=i.tracked[1]=i.intentTracked[0]=i.intentTracked[1]=true;
  i.requirePrimaryGrip114=i.heldMotionAttacks=i.coherentMelee119=i.controllerAnchor119=true;
  i.naturalCombat117=i.physicalCombat177=i.forwardOnly19=true;i.dynamics=i.contact=false;i.supportAxis59c16=true;
  i.reach[0]=i.reach[1]=1.4;i.shoulders[0]={.2,0,1.3};i.shoulders[1]={-.2,0,1.3};
  i.hands[0].p={.2,.25,1.3};i.hands[0].q=m::from_rotation_vector({0,0,-.96});i.grip[0]=1;
  auto&w=p[0];w.key=w.generation=1;w.classId=7;w.side=0;w.primary=w.support=w.geometry=w.twoHanded=true;
  w.grip[1].p={0,-.3,0};w.axis={0,1,0};w.tip={0,1.4,0};w.extent=1.8;w.weight=3;
  i.hands[1].p=ph::point(ph::compose(i.hands[0],ph::inverse(w.grip[0])),w.grip[1].p);
  for(int n=0;n<15;++n)next();i.grip[1]=1;for(int n=0;n<10;++n)next();
  ck(o.support[0],"tracked native-profile support is latched before the replay");
  prepares=commits=0;travel=visiblePeak=intentPeak=0;
 }
 void next(){++i.sample;i.tick+=10;i.timeNs=i.tick*1000000;
  if(!relativeFixed){i.intentHands[0]=i.hands[0];i.intentHands[1]=i.hands[1];}
  const auto old=o.weapons[0];o=ph::step(s,i,p,[](const ph::Profile&,ph::Pose,ph::Pose,bool){return ph::Contact103{};});
  const double delta=m::length(ph::point(o.weapons[0],p[0].tip)-ph::point(old,p[0].tip));
  travel+=delta;visiblePeak=std::max(visiblePeak,delta/.01);intentPeak=std::max(intentPeak,o.melee118.tipSpeed);
  for(unsigned n=0;n<o.commandCount;++n){const auto k=o.commands[n].kind;
   prepares+=k==ph::CommandKind::Prepare;commits+=k==ph::CommandKind::Slash||k==ph::CommandKind::Thrust;}
 }
 void arc(bool primaryFollows=false,double radians=.96,unsigned samples=24){
  for(unsigned n=1;n<=samples;++n){const double angle=-.96+radians*n/samples;
   if(primaryFollows)i.hands[0].q=m::from_rotation_vector({0,0,angle});
   i.hands[1].p=i.hands[0].p+m::rotate(m::from_rotation_vector({0,0,angle}),p[0].grip[1].p);next();}
 }
};
int main(){
 {Lever63 r;r.arc();
  std::printf("support-only travel=%.9f visiblePeak=%.9f intentPeak=%.9f prepare=%u commit=%u\n",r.travel,r.visiblePeak,r.intentPeak,r.prepares,r.commits);
  ck(r.o.support[0]&&r.travel>1.3&&r.visiblePeak>5.5,"same measured fast blade path remains physically supported");
  ck(r.intentPeak>5.5&&r.prepares==1&&r.commits==1,"support-hand lever contributes deliberate tip motion to both intent families");}
 {Lever63 r;r.arc(true);ck(r.prepares==1&&r.commits==1,"primary-following positive control remains one attempt");}
 {Lever63 r;r.s.weapon[0].guardExitTick32=r.i.tick;r.arc();
  ck(r.prepares==1&&r.commits==1,"postguard lever cut earns the same translated 7 cm/.8 m/s hand proof through its support hand");}
 {Lever63 r;r.s.weapon[0].guardExitTick32=r.i.tick;r.arc(false,.08,4);
  ck(r.prepares==0&&r.commits==0,"short high-tip-speed guard lever adjustment is not a cut");}
 {Lever63 r;r.s.weapon[0].guardExitTick32=r.i.tick;r.arc(true,.08,4);
  ck(r.prepares==0&&r.commits==0,"short primary-wrist guard flick remains excluded");}
 {Lever63 r;r.relativeFixed=true;r.arc();ck(r.prepares==0&&r.commits==0,"model-only support rotation cannot borrow a missing HMD-relative stroke");}
 {Lever63 r;r.i.intentTracked[1]=false;r.arc();ck(r.prepares==0&&r.commits==0,"missing support intent tracking cannot create a lever attack");}
 {Lever63 r;r.relativeFixed=true;
  for(unsigned n=0;n<24;++n){const ph::Pose common{m::from_rotation_vector({0,0,.04}),{}};
   r.i.hands[0]=ph::compose(common,r.i.hands[0]);r.i.hands[1]=ph::compose(common,r.i.hands[1]);r.next();}
  ck(r.prepares==0&&r.commits==0,"common model/body rotation remains insufficient for attack intent");}
 {Lever63 r;r.i.grip[1]=0;r.next();r.next();ck(r.prepares==0&&r.commits==0,"support release cannot inject its changed intent pose as a swing");}
 {Lever63 r;r.i.allowed=false;r.next();ck(r.prepares==0&&r.commits==0&&!r.o.owned[0],"gameplay ownership loss still releases rather than attacking");}
 // Current arrival data must join the achieved pose whose velocities were
 // integrated in this sample, rather than the preceding completed orientation.
 for(bool direct:{false,true})for(bool two:{false,true}){Lever63 r;
  r.i.motionOwned59c5=direct;r.i.dynamics=true;r.i.constraintDrive129=true;
  if(!two){r.i.grip[1]=0;for(unsigned n=0;n<20;++n)r.next();}
  unsigned arrivals=0,mismatches=0;bool prepareWitness=false;
  for(unsigned n=0;n<30;++n){
   r.i.hands[0].q=m::multiply(m::from_rotation_vector({0,.1*n,0}),m::from_rotation_vector({0,0,-.96}));
   r.i.hands[0].p={.2,.25+.002*n,1.3};
   r.i.hands[1].p=ph::point(ph::compose(r.i.hands[0],ph::inverse(r.p[0].grip[0])),r.p[0].grip[1].p);const auto beforeFrame=r.s;r.next();
   if(r.o.arrival39.gesture){++arrivals;const auto& body=r.s.weapon[0];
    const auto lever=ph::point(r.o.weapons[0],r.p[0].tip)-ph::point(r.o.weapons[0],r.p[0].grip[0].p);
    const auto v=ph::body_vector117(r.i,body.linearVelocity+m::cross(body.angularVelocity,lever));
    const auto w=ph::body_vector117(r.i,body.angularVelocity);
    const auto current=kcd2vr::dirblock39::predictCut({v.x,v.y,v.z},{w.x,w.y,w.z});
    mismatches+=!current.valid||current.zone!=r.o.arrival39.zone||current.angle!=r.o.arrival39.angle||current.nearBoundary!=r.o.arrival39.nearBoundary;
   }
   if(!direct&&!two&&n==8){for(unsigned j=0;j<r.o.commandCount;++j){const auto& cmd=r.o.commands[j];
    if(cmd.kind==ph::CommandKind::Prepare){prepareWitness=true;
     ck(cmd.zone117==0&&cmd.sample==r.i.sample&&cmd.tick==r.i.tick&&cmd.gesture59c3==r.o.arrival39.gesture,"native origin and original Prepare identity remain independent of measured arrival");
     ck(r.o.arrival39.zone==2&&r.o.arrival39.angle==34&&!r.o.arrival39.nearBoundary,"qualified Prepare supplies current confident blade sector instead of previous-pose boundary fallback");}
   }}
   if(!direct&&!two&&n==8){
    const auto input=r.i;
    const auto intent=ph::compose(input.hands[0],ph::inverse(r.p[0].grip[0]));
    auto first=beforeFrame,second=beforeFrame;ph::Output a{},b{};
    auto requested=first.weapon[0].weapon;requested.q=m::multiply(m::from_rotation_vector({0,1.2,0}),requested.q);
    ph::normal_intent_phase_h30(first,input,r.p[0],first.weapon[0],a,0,.01,intent,intent,intent,first.weapon[0].weapon,false,{},{},true);
    ph::normal_intent_phase_h30(second,input,r.p[0],second.weapon[0],b,0,.01,intent,intent,intent,requested,false,{},{},true);
    ck(a.arrival39.gesture&&b.arrival39.gesture,"soft-path control reaches the actual arrival producer");
    ck(a.arrival39.zone==b.arrival39.zone&&a.arrival39.angle==b.arrival39.angle&&a.arrival39.nearBoundary==b.arrival39.nearBoundary,"soft recovery cannot announce a requested fallback instead of its stored checked pose");
   }
  }
  ck(arrivals>0&&mismatches==0,"one/two-hand Direct/Hybrid arrival uses the same current achieved pose as its velocities");
  if(!direct&&!two)ck(prepareWitness,"current-sector regression reaches the actual Prepare producer");
 }
 std::printf("two_hand_intent63=%u/%u native_execution=NONE\n",checks-failed,checks);return failed?1:0;
}
