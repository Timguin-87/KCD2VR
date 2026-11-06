#include "../src/vrik/physical_weapon.h"
#include "../src/diagnostics/capture168.h"
#include <cstdio>
#include <cstdlib>
namespace ph=kcd2vr::physical;
namespace m=kcd2vr::pose_core;
namespace cap=kcd2vr::diagnostics::capture168;
static int checks=0;
static void ck(bool ok,const char* msg){++checks;if(!ok){std::fprintf(stderr,"FAIL %s\n",msg);std::exit(1);}}
static int commands(const ph::Output& o,ph::CommandKind k){int n=0;for(unsigned j=0;j<o.commandCount;++j)n+=o.commands[j].kind==k;return n;}
struct Rig {
 ph::Input i{};ph::State s{};std::array<ph::Profile,2> p{};ph::Output o{};bool wall=false;
 int begins=0,fires=0,cancels=0;ph::Pose swept{};
 Rig(){
  i.epoch=i.bodyEpoch=1;i.allowed=i.tracked[0]=i.tracked[1]=i.intentTracked[0]=i.intentTracked[1]=true;
  i.bowActions=i.bowStringTrigger160=i.bowRegrab116=i.bowRetention=true;
  i.bowDrawPolicy175=ph::BowDrawPolicy175::PhysicalString;i.bowGeometryReady175=true;
  i.grip[0]=i.grip[1]=1;i.reach[0]=i.reach[1]=1.1;
  i.shoulders[0]={.2,0,1.3};i.shoulders[1]={-.2,0,1.3};i.hands[1].p={0,.5,1.3};
  p[1].key=p[1].generation=7;p[1].side=1;p[1].classId=9;p[1].primary=p[1].support=p[1].geometry=true;
  p[1].grip[0].p={0,-.15,0};p[1].axis={0,-1,0};p[1].tip={0,0,.6};p[1].bowDrawLength166=.55;
  i.hands[0].p={0,.35,1.3};next();i.trigger[0]=1;next();next();
 }
 void next(){++i.sample;i.tick=1000+10*i.sample;i.timeNs=i.tick*1000000;
  o=ph::step(s,i,p,[&](const ph::Profile&,ph::Pose a,ph::Pose b,bool){
   swept=b;ph::Contact103 c{};c.known=true;c.fraction=wall?0:1;return c;
  });begins+=commands(o,ph::CommandKind::BowBegin);fires+=commands(o,ph::CommandKind::BowFire);cancels+=commands(o,ph::CommandKind::BowCancel);
 }
 void pull(){for(int n=0;n<15;++n){i.hands[0].p.y-=.02;next();}}
};
int main(){
 Rig r;r.pull();ck(r.o.bowHeld&&r.s.weapon[1].bowGrip38,"right Grip latches the physical drawing hand");
 ck(std::abs(r.o.bowFraction-.30/.55)<1e-8,"draw length measures hand separation, not elapsed trigger time");
 const auto localQ=m::multiply(m::conjugate(r.o.weapons[1].q),r.o.hands[0].q);
 const auto left=r.o.hands[1].p;
 for(int n=0;n<20;++n){r.i.hands[0].p.x+=.006;r.next();}
 ck(m::angle(r.o.weapons[1].q,{})>.1,"right-hand lateral motion steers the entire bow");
 ck(m::length(r.o.hands[1].p-left)<1e-8,"bow rotates around the left grip, never a camera/body pivot");
 ck(m::length(r.o.hands[0].p-r.i.hands[0].p)<1e-7,"settled two-hand solution reaches the tracked right wrist");
 ck(m::angle(m::multiply(m::conjugate(r.o.weapons[1].q),r.o.hands[0].q),localQ)<1e-7,"grasp orientation stays glued in the bow frame");
 r.i.hands[0].q=m::from_rotation_vector({0,.8,0});r.next();
 ck(m::angle(m::multiply(m::conjugate(r.o.weapons[1].q),r.o.hands[0].q),localQ)<1e-7,"right wrist twist cannot independently rotate the arrow off the bow");
 const auto draw=r.o.bowFraction;for(int n=0;n<40;++n)r.next();
 ck(std::abs(r.o.bowFraction-draw)<1e-9,"stationary Trigger cannot accumulate physical draw");
 const auto aimAtRelease=r.o.weapons[1].q;
 r.i.trigger[0]=0;r.next();ck(m::angle(r.o.weapons[1].q,aimAtRelease)<1e-7,"release frame retains coupled aim for native shooting");
 ck(r.fires==1&&!r.o.bowHeld&&r.cancels==0,"intentional Trigger release still requests exactly one native shot");
 r.next();ck(r.fires==1,"held release cannot replay the shot");
 Rig w;w.pull();const auto stopped=w.o.weapons[1];w.wall=true;w.i.hands[0].p.x+=.03;w.next();
 ck(m::length(w.o.weapons[1].p-stopped.p)<1e-8&&m::angle(w.o.weapons[1].q,stopped.q)<1e-7,"bow steering cannot bypass a blocked sweep");
 ck(m::angle(m::multiply(m::conjugate(w.o.weapons[1].q),w.o.hands[0].q),w.s.weapon[1].bowHandLocal38)<1e-7,"drawing wrist reconstructs from allowed root after contact");
 w.i.tracked[0]=false;w.next();ck(!w.o.bowHeld&&w.cancels==1&&w.fires==0,"tracking loss cancels without firing");
 Rig g;g.pull();g.i.grip[0]=0;g.next();
 ck(g.o.bowHeld&&!g.s.weapon[1].bowGrip38&&g.fires==0,"Grip release detaches the visual grasp; Trigger remains the sole shot owner");
 // A moving left wrist at the first drawing-hand latch must not contaminate
 // the right wrist grasp with the previous frame's bow orientation.
 Rig first;first.s.weapon[1].bowGrip38=false;
 first.i.hands[1].q=m::from_rotation_vector({0,.30,0});
 first.i.hands[0].q=m::from_rotation_vector({.12,0,.08});first.next();
 ck(first.o.bowHeld&&first.s.weapon[1].bowGrip38,"first moving-root latch remains physically held");
 ck(m::angle(first.o.hands[0].q,first.i.hands[0].q)<1e-7,"first latch preserves current right wrist against the CURRENT allowed bow root");
 Rig firstWall;firstWall.s.weapon[1].bowGrip38=false;firstWall.wall=true;
 firstWall.i.hands[1].q=m::from_rotation_vector({0,.30,0});
 firstWall.i.hands[0].q=m::from_rotation_vector({.12,0,.08});firstWall.next();
 ck(firstWall.o.bowHeld&&firstWall.s.weapon[1].bowGrip38,"blocked first latch remains held");
 ck(m::angle(firstWall.o.hands[0].q,firstWall.i.hands[0].q)<1e-7,"first blocked latch captures allowed root, not desired or previous root");
 Rig e;e.pull();++e.i.bodyEpoch;e.next();ck(!e.o.bowHeld&&!e.s.weapon[1].bowGrip38&&e.fires==0,"recenter clears grasp and pending draw");
 cap::gate.select(cap::Domain::Reload,true,1000);
 ck(cap::accept("V66.178R36C2 RELOAD_NATIVE session=1 stage=HAND_ATTACH",1001),"F11 alone retains native reload lifecycle events");
 ck(cap::accept("V66.128-BOW_STRING FOLLOW",1001),"F11 alone retains bow/string pose evidence");
 ck(!cap::accept("V66.117 MELEE_INPUT edge=COMMIT",1001),"F11 does not enable unrelated combat recording");
 cap::gate.select(cap::Domain::Reload,false,1002);
 ck(!cap::accept("V66.178R36C2 RELOAD_NATIVE session=1",1003),"F11 off stops per-event recording");
 ck(cap::accept("V66.178R36C2 RELOAD_NATIVE_END session=1",1003)&&cap::accept("V66.178R36C2 RELOAD_NATIVE_COVERAGE session=1",1003),"coverage and end/drop counters survive recorder shutdown");
 std::printf("PASS bow_coupling39c1 %d checks; native execution NONE\n",checks);return 0;
}
