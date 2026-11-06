// V66.102: weapon feel, auto two-hand grip, polearm slide, rebound, bow quiver, sweep gate.
#include "../src/vrik/physical_weapon.h"
#include <cstdio>
using namespace kcd2vr::physical;
namespace m=kcd2vr::pose_core;
static unsigned checks=0,failures=0;
static void check(bool b,const char* n){++checks;if(!b)++failures;std::printf("%s %s\n",b?"PASS":"FAIL",n);}
static auto clear=[](const Profile&,Pose,Pose,bool){return 1.0;};
static Input input(){Input i{};i.allowed=true;i.epoch=i.bodyEpoch=1;i.sample=1;i.tick=1000;i.tracked[0]=i.tracked[1]=true;
 i.hands[0].p={.2,.3,1.3};i.hands[1].p={.05,.3,1.3};i.shoulders[0]={.2,0,1.3};i.shoulders[1]={-.2,0,1.3};i.reach[0]=i.reach[1]=.65;return i;}
static std::array<Profile,2> sword(double weight=2,double extent=1){std::array<Profile,2> p{};auto& a=p[0];a.key=1;a.entity=42;a.generation=1;a.side=0;a.classId=4;
 a.primary=a.support=a.geometry=a.twoHanded=true;a.tip={0,.9,0};a.axis={0,1,0};a.extent=extent;a.weight=weight;a.grip[1].p={0,-.15,0};p[1].side=1;return p;}
static void adv(Input& i,unsigned ms=11){++i.sample;i.tick+=ms;}
// fast 2 m/s sideways swing for 150 ms, then stop; returns {max lag during move, overshoot after stop}
static void swing(double weight,bool two,double& maxLag,double& overshoot){
 auto p=sword(weight);auto i=input();i.autoSupport=true;i.dynamics=true;State s{};step(s,i,p,clear);
 if(two){i.hands[1].p=compose(s.weapon[0].weapon,p[0].grip[1]).p;for(int k=0;k<12;++k){adv(i);step(s,i,p,clear);}}
 maxLag=0;overshoot=0;
 for(int k=0;k<14;++k){adv(i);i.hands[0].p.z+=.022;if(two)i.hands[1].p.z+=.022;auto o=step(s,i,p,clear);maxLag=std::max(maxLag,m::length(o.hands[0].p-i.hands[0].p));}
 for(int k=0;k<40;++k){adv(i);auto o=step(s,i,p,clear);overshoot=std::max(overshoot,o.hands[0].p.z-i.hands[0].p.z);}
}
int main(){
 {double lag1=0,ov1=0,lag2=0,ov2=0,lagH=0,ovH=0;swing(2,false,lag1,ov1);swing(2,true,lag2,ov2);swing(6,false,lagH,ovH);
  std::printf("   one-hand lag %.1f mm, two-hand lag %.1f mm, heavy one-hand lag %.1f mm, overshoot %.2f mm\n",lag1*1000,lag2*1000,lagH*1000,ov1*1000);
  check(lag1>.004&&lag1<.045,"one-handed weapon trails the hand a little (4-45 mm at 2 m/s)");
  check(lag2<lag1,"two-handed grip trails less than one-handed");
  check(lagH>lag1,"heavier weapon trails more");
  check(ov1<.002&&ov2<.002,"critically damped: no overshoot after the hand stops");}
 {auto p=sword();auto i=input();i.dynamics=true;State s{};step(s,i,p,clear);double worst=0;
  for(int k=1;k<40;++k){adv(i);i.hands[0].q=m::from_rotation_vector({0,0,.12*k});auto o=step(s,i,p,clear);worst=std::max(worst,m::angle(o.weapons[0].q,compose(i.hands[0],inverse(p[0].grip[0])).q));}
  std::printf("   fast wrist rotation (11 rad/s): max weapon rotation lag %.2f deg\n",worst*57.2958);
  check(worst<=4.0/57.2958+1e-6,"rotation lag never exceeds 4 degrees (no board-on-a-pole)");}
 {auto p=sword();auto i=input();i.dynamics=i.contact=true;State s{};step(s,i,p,clear);
  int calls=0;auto wall=[&](const Profile& pr,Pose a,Pose b,bool){++calls;const double from=point(a,pr.tip).x,to=point(b,pr.tip).x;return to>.30&&to>from?std::clamp((.30-from)/(to-from),0.0,1.0):1.0;};
  bool blocked=false;for(int k=0;k<10;++k){adv(i);i.hands[0].p.x+=.03;auto o=step(s,i,p,wall);blocked=blocked||o.blocked[0];}
  i.hands[0].p.x=.2;double back=0;Output o{};for(int k=0;k<60;++k){adv(i);o=step(s,i,p,wall);}
  back=m::length(o.hands[0].p-i.hands[0].p);
  check(blocked,"blade stops at the wall");check(back<.01,"after impact the blade springs back and settles on the hand");}
 {auto p=sword();auto i=input();i.autoSupport=true;State s{};step(s,i,p,clear);
  const Vec3 grip2=compose(s.weapon[0].weapon,p[0].grip[1]).p;
  i.hands[1].p=grip2+Vec3{.05,0,0};i.grip[1]=0;Output o{};
  adv(i,30);o=step(s,i,p,clear);check(!o.support[0],"no instant latch on a pass-through (dwell)");
  for(int k=0;k<10;++k){adv(i);o=step(s,i,p,clear);}
  check(o.support[0]&&o.owned[1],"support latches by proximity (<10 cm) without the grip button");
  i.hands[1].p=grip2+Vec3{.14,0,0};adv(i);o=step(s,i,p,clear);check(o.support[0],"hysteresis: still held at 14 cm");
  i.hands[1].p=grip2+Vec3{.25,0,0};adv(i);o=step(s,i,p,clear);check(!o.support[0]&&o.active[0],"moving the hands apart releases support, primary kept");
  for(int k=0;k<5;++k){adv(i);i.hands[1].p=grip2+Vec3{.05,.02*k,0};o=step(s,i,p,clear);} // quick sweep past
  check(!o.support[0],"a hand sweeping quickly past the hilt does not grab");}
 {auto p=sword(4,2.2);p[0].tip={0,1.8,0};auto i=input();i.autoSupport=true;State s{};step(s,i,p,clear);
  Vec3 g=compose(s.weapon[0].weapon,p[0].grip[1]).p;i.hands[1].p=g;Output o{};for(int k=0;k<12;++k){adv(i);o=step(s,i,p,clear);}
  check(o.support[0],"long item support latches at its actual helper");
  for(int k=0;k<40;++k){adv(i);i.hands[1].p=compose(s.weapon[0].weapon,Pose{{},{0,.10,0}}).p;o=step(s,i,p,clear);}
  const Vec3 local=point(inverse(o.weapons[0]),o.hands[1].p);
  std::printf("   released free-hand local position %.3f m (authored helper -0.150)\n",local.y);
  check(!o.support[0]&&!o.owned[1],"length alone does not authorize a sliding shaft; moving off helper releases");}
 {std::array<Profile,2> p{};p[0].side=0;auto& bow=p[1];bow.key=3;bow.generation=1;bow.side=1;bow.classId=9;bow.primary=bow.support=bow.geometry=true;
  bow.grip[0].p={0,-.15,0};bow.axis={0,-1,0};bow.tip={0,0,.6};bow.extent=1.2;
  auto i=input();i.arrowFromShoulder=true;i.bowRetention=true;i.hands[1].p={0,.4,1.3};i.hands[0].p={0,.25,1.3};State s{};step(s,i,p,clear);
  adv(i);i.grip[0]=1;auto o=step(s,i,p,clear);check(!o.bowHeld,"no nock without an arrow from the quiver");
  adv(i);i.grip[0]=0;step(s,i,p,clear);
  i.hands[0].p=right_shoulder_zone(i);for(int k=0;k<6;++k){adv(i);i.grip[0]=1;step(s,i,p,clear);}
  check(s.weapon[1].arrowInHand,"arrow taken over the right shoulder");
  i.hands[0].p={0,.25,1.3};adv(i);o=step(s,i,p,clear);check(o.bowHeld,"arrow nocks at the string");
  adv(i);i.hands[0].p.y-=.30;o=step(s,i,p,clear);const double f=o.bowFraction;
  adv(i,11);i.hands[0].p.x+=.30;o=step(s,i,p,clear);check(o.bowHeld,"a brief lateral wobble does not drop the arrow");
  adv(i,11);i.hands[0].p.x-=.30;o=step(s,i,p,clear);
  adv(i);i.grip[0]=0;o=step(s,i,p,clear);bool fired=false;for(unsigned k=0;k<o.commandCount;++k)fired=fired||o.commands[k].kind==CommandKind::BowFire;
  check(fired&&f>.4,"releasing the grip fires the drawn arrow");}
 {auto p=sword();auto i=input();i.contact=true;i.sweepGate=true;State s{};int calls=0;auto q=[&](const Profile&,Pose,Pose,bool){++calls;return 1.0;};
  step(s,i,p,q);for(int k=0;k<5;++k){adv(i);step(s,i,p,q);}const int still=calls;
  for(int k=0;k<5;++k){adv(i);i.hands[0].p.x+=.02;step(s,i,p,q);}
  std::printf("   static queries: %d while still (6 frames), %d more while moving (5 frames)\n",still,calls-still);
  check(still<=2,"a still, clear weapon does not re-query every frame");check(calls-still>=5,"a moving weapon is swept every frame");}
 {auto p=sword();auto i=input();i.autoSupport=true;State s{};step(s,i,p,clear);i.hands[1].p=compose(s.weapon[0].weapon,p[0].grip[1]).p;
  for(int k=0;k<12;++k){adv(i);step(s,i,p,clear);}double prev=-1,maxJump=0;
  for(int k=0;k<20;++k){adv(i);i.hands[0].p.z+=.015;i.hands[1].p.z+=.015;auto o=step(s,i,p,clear);const double lag=m::length(o.hands[0].p-i.hands[0].p);if(k>4&&prev>=0)maxJump=std::max(maxJump,std::abs(lag-prev));prev=lag;}
  std::printf("   two-handed steady walk/swing: frame-to-frame lag change %.2f mm\n",maxJump*1000);
  check(maxJump<.002,"no frame-to-frame jitter while moving at constant speed (two-handed)");}
 std::printf("%u checks, %u failures\n",checks,failures);return failures?1:0;
}
