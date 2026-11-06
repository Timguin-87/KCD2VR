// V66.107 regression: generic delayed-tap policy remains available; quiver with right grip in RT mode.
#include "vrik/physical_interaction.h"
#include "vrik/physical_weapon.h"
#include <cstdio>
static unsigned checks=0,failures=0;
static void check(bool b,const char* n){++checks;if(!b)++failures;std::printf("%s %s\n",b?"PASS":"FAIL",n);}
int main(){
 using namespace kcd2vr;
 {namespace pi=physical_interaction106;pi::State s{};pi::Input in{};in.enabled=true;in.now_ms=1000;
  for(int i=0;i<2;++i){in.hand[i].tracked=true;in.hand[i].empty=true;in.hand[i].neutral=true;}
  in.hand[1].tap=true;
  in.hover_age_ms[0]=in.hover_age_ms[1]=200; // V66.178R7: hover already owns the interactor (20 ms pulse); timing is not what this test asserts
  // left grip pressed: nothing yet
  in.hand[1].grip=1;auto o=pi::Step(s,in);check(!o.fire_use&&!o.ray_active,"generic tap-mode PRESS does not use");
  in.now_ms+=150;in.hand[1].grip=0;o=pi::Step(s,in);in.now_ms+=25;o=pi::Step(s,in);
  check(o.fire_use&&o.ray_owner==1,"generic short tap with neutral stick uses");
  // grip + stick = d-pad: no use
  in.now_ms+=1000;in.hand[1].grip=1;o=pi::Step(s,in);in.now_ms+=50;in.hand[1].neutral=false;o=pi::Step(s,in);
  in.now_ms+=50;in.hand[1].neutral=true;in.hand[1].grip=0;o=pi::Step(s,in);in.now_ms+=25;o=pi::Step(s,in);
  check(!o.fire_use,"generic tap invalidated by non-neutral stick");
  // long hold: no use
  in.now_ms+=1000;in.hand[1].grip=1;o=pi::Step(s,in);in.now_ms+=600;o=pi::Step(s,in);in.hand[1].grip=0;o=pi::Step(s,in);in.now_ms+=25;o=pi::Step(s,in);
  check(!o.fire_use,"generic long tap-mode hold never uses");
  // right hand keeps the edge semantics
  in.now_ms+=1000;in.hand[0].grip=1;o=pi::Step(s,in);in.now_ms+=25;o=pi::Step(s,in);
  check(o.fire_use&&o.ray_owner==0,"right grip press still uses (edge)");}
 {namespace ph=physical;namespace m=pose_core;std::array<ph::Profile,2> p{};p[0].side=0;auto& bow=p[1];bow.key=3;bow.generation=1;bow.side=1;bow.classId=9;
  bow.primary=bow.support=bow.geometry=true;bow.grip[0].p={0,-.15,0};bow.axis={0,-1,0};bow.tip={0,0,.6};bow.extent=1.2;
  ph::Input i{};i.allowed=true;i.epoch=i.bodyEpoch=1;i.sample=1;i.tick=1000;i.tracked[0]=i.tracked[1]=true;
  i.shoulders[0]={.2,0,1.3};i.shoulders[1]={-.2,0,1.3};i.reach[0]=i.reach[1]=.65;i.bowActions=true;i.bowTrigger=true;i.arrowFromShoulder=true;i.bowRetention=true;
  i.hands[1].p={0,.4,1.3};i.hands[0].p={0,.25,1.3};ph::State s{};auto clear=[](const ph::Profile&,ph::Pose,ph::Pose,bool){return 1.0;};
  auto adv=[&](unsigned ms=11){++i.sample;i.tick+=ms;};
  ph::step(s,i,p,clear);
  adv();i.trigger[0]=1;auto o=ph::step(s,i,p,clear);check(!o.bowHeld,"RT at the string without an arrow does nothing (quiver mode)");
  adv();i.trigger[0]=0;ph::step(s,i,p,clear);
  i.hands[0].p=ph::right_shoulder_zone(i);for(int k=0;k<6;++k){adv();i.grip[0]=1;ph::step(s,i,p,clear);}
  check(s.weapon[1].arrowInHand,"right grip over the right shoulder takes an arrow");
  i.hands[0].p={0,.25,1.3};adv();o=ph::step(s,i,p,clear);check(o.bowHeld,"holding grip, the arrow nocks at the string");
  adv();i.hands[0].p.y-=.30;o=ph::step(s,i,p,clear);const double f=o.bowFraction;
  adv();i.grip[0]=0;o=ph::step(s,i,p,clear);bool fired=false;for(unsigned k=0;k<o.commandCount;++k)fired=fired||o.commands[k].kind==ph::CommandKind::BowFire;
  check(fired&&f>.4,"releasing the right grip shoots");}
 std::printf("%u checks, %u failures\n",checks,failures);return failures?1:0;
}
