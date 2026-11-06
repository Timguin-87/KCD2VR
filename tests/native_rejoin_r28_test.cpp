#include "vrik/native_rejoin_r28.h"
#include "vrik/weapon_sweep112.h"
#include "vrik/contact_budget.h"
#include <cstdio>
using namespace kcd2vr::physical;
namespace nr=kcd2vr::native_rejoin_r28;
namespace sweep169r2=kcd2vr::sweep169r2;
unsigned checks=0,failed=0;void ck(bool x,const char* n){++checks;if(!x){++failed;printf("FAIL %s\n",n);}}
struct Socket{uint64_t key=0,generation=0;void*character=nullptr;int joint=0;struct{float q[4]{0,0,0,1},t[3]{};}relative;float scale[3]{1,1,1};bool valid=false;};
struct Fixture{
 Profile p{};Input in{};Pose desired;Vec3 previous{.08549211,.2162164,1.129798};
 Fixture(){const double fraction=.487602;
 const Pose allowedWeapon{*m::unit(Quat{-.0674874,-.01175833,-.07117776,-.9951085}),{-.1073528,.3076561,1.294026}};
 const Pose allowedHand{*m::unit(Quat{.7539781,-.6533179,-.05603515,-.03940642}),{-.0738776,.2440751,1.300691}};
 const Quat q=*m::unit(Quat{.103278,-.138355,-.241831,-.954834});const Vec3 hand{.08399466,.2175052,1.123458};
 p.key=0x0200000000018DD5ULL;p.generation=1;p.entity=42;p.classId=4;p.primary=p.geometry=p.nativeSeed=true;
 p.grip[0]=compose(inverse(allowedWeapon),allowedHand);p.tip={0,1,0};
 desired={q,hand-m::rotate(q,p.grip[0].p)};const auto relative=m::multiply(m::conjugate(q),allowedWeapon.q);
 p.nativeWeapon.q=m::multiply(q,m::from_rotation_vector(m::rotation_vector(relative)*(1/(1-fraction))));
 p.nativeWeapon.p=(allowedHand.p-hand*fraction)*(1/(1-fraction))-m::rotate(p.nativeWeapon.q,p.grip[0].p);
 in.allowed=in.contact=in.controllerAnchor119=in.physicalCombat177=true;in.attacks=in.naturalCombat117=in.coherentMelee119=true;in.tracked[0]=in.intentTracked[0]=true;in.grip[0]=1;
 in.reach[0]=reach_max(.2663,.2615);in.shoulders[0]={.1763,-.0468,1.2900};in.hands[0]=compose(desired,p.grip[0]);in.intentHands[0]=in.hands[0];
 in.epoch=in.bodyEpoch=6;in.sample=282286;in.tick=1109203453;in.timeNs=in.tick*1000000;
 }
};
struct Rays{
 kcd2vr::contact::Budget budget;unsigned called=0,hitAt=0,unknownAt=0;uint64_t cost=1,clock=0;
 Rays(unsigned cap=18,uint64_t us=1000){budget.begin_work(cap,us);}
 Contact103 operator()(Vec3,Vec3,double){Contact103 c;
  if(!budget.take(clock)){c.known=false;c.fraction=0;c.coverage169r2.reason=budget.remaining?sweep169r2::Reason::TimeBudget:sweep169r2::Reason::RayCap;return c;}
  ++called;clock+=cost;budget.finish(clock);
  if(called==hitAt){c.fraction=.5;c.plane=true;c.normal={0,1,0};c.point={0,0,0};}
  if(called==unknownAt){c.known=false;c.fraction=0;c.coverage169r2.reason=sweep169r2::Reason::QueryBusy;}
  return c;
 }
};
void trackerTests(){Fixture f;nr::Context c{1,0x1234,6,3};Socket sock{};sock.key=f.p.key;sock.generation=1;sock.character=(void*)0x1234;sock.joint=27;sock.valid=true;
 nr::Binding<Socket> b{f.p,sock,true};std::array<nr::Binding<Socket>,2> bindings{};bindings[0]=b;bool raw[2]{true,true};
 auto armed=[&]{nr::Tracker<Socket> t;t.observe(c,true,bindings);t.observe(c,false);t.complete(c,100,1000,7,1000,raw);return t;};
 auto claim=[&](nr::Tracker<Socket>&t,nr::Context x,const Profile&p,const Socket&s,uint64_t sample=101,uint64_t tick=1010,uint64_t now=1010,bool valid=true){return t.claim(x,0,p,s,sample,tick,now,valid);};
 auto t=armed();ck(claim(t,c,f.p,sock),"exact native retirement then completed raw pose qualifies");ck(!claim(t,c,f.p,sock),"receipt consumed once");
 t={};t.observe(c,true,bindings);t.observe(c,false);ck(!claim(t,c,f.p,sock),"native exit alone is not successful completed-body evidence");
 t=armed();t.observe(c,true,bindings);ck(!claim(t,c,f.p,sock),"still-native cannot claim");
 for(int bad=0;bad<4;++bad){t=armed();auto x=c;switch(bad){case 0:x.policy++;break;case 1:x.actor++;break;case 2:x.bodyEpoch++;break;case 3:x.trackingEpoch++;break;}ck(!claim(t,x,f.p,sock),"policy actor body tracking mismatch rejected");}
 t=armed();ck(!claim(t,{},f.p,sock),"experiment off clears receipt");ck(!claim(t,c,f.p,sock),"off/on cannot revive receipt");
 for(int bad=0;bad<7;++bad){t=armed();auto p=f.p;switch(bad){case 0:p.key++;break;case 1:p.generation++;break;case 2:p.entity++;break;case 3:p.classId++;break;case 4:p.grip[0].p.x+=.001;break;case 5:p.tip.z+=.001;break;case 6:p.nativeSeed=false;break;}ck(!claim(t,c,p,sock),"changed item or grip/geometry rejected");}
 for(int bad=0;bad<6;++bad){t=armed();auto s=sock;switch(bad){case 0:s.joint++;break;case 1:s.relative.t[0]=.001f;break;case 2:s.scale[1]=1.01f;break;case 3:s.character=(void*)0x5678;break;case 4:s.valid=false;break;case 5:s.generation++;break;}ck(!claim(t,c,f.p,s),"changed wrist binding rejected");}
 t=armed();ck(!claim(t,c,f.p,sock,100),"same sample cannot acquire");t=armed();ck(!claim(t,c,f.p,sock,101,999,1010),"source before completed receipt rejected");
 t=armed();ck(!claim(t,c,f.p,sock,101,1010,1081),"old completed pose expires at80ms");t=armed();ck(!claim(t,c,f.p,sock,101,1100,1010),"future source rejected");
 t=armed();ck(!claim(t,c,f.p,sock,101,1010,1010,false),"unknown source/real owner rejects");ck(!claim(t,c,f.p,sock),"rejected source consumes receipt");
 t=armed();t.complete(c,105,1070,8,1070,raw);ck(!claim(t,c,f.p,sock,106,1081,1081),"repeated callback does not renew first return age");
 t=armed();bool motor[2]{false,true};t.complete(c,101,1010,8,1010,motor);ck(!claim(t,c,f.p,sock),"already-owned/nonraw completed hand cancels receipt");
 t={};t.observe(c,true,bindings);t.observe(c,false);t.complete(c,100,1000,0,1000,raw);ck(!claim(t,c,f.p,sock),"failed publication cannot arm");
 auto left=f.p;left.key++;left.side=1;left.grip[1]=left.grip[0];auto ls=sock;ls.key=left.key;ls.joint=41;bindings[1]={left,ls,true};t={};t.observe(c,true,bindings);t.observe(c,false);t.complete(c,100,1000,7,1000,raw);
 ck(!t.claim(c,1,f.p,sock,101,1010,1010,true),"opposite side cannot steal receipt");ck(claim(t,c,f.p,sock),"right identity survives failed opposite claim");
}
void sweepTests(){Fixture f;g_sweep_coverage_scale178r6=1.;
 auto p=f.p;Rays old;auto before=sweep_weapon112(p,p.nativeWeapon,f.desired,false,old);
 ck(before.limited115&&before.fraction<.6&&before.coverage169r2.reason==sweep169r2::Reason::AngularPrefix,"baseline61.8degree native gap is limited prefix");
 p.nativeRejoinSweepR28=true;Rays clear;auto after=sweep_weapon112(p,p.nativeWeapon,f.desired,false,clear);
 ck(after.known&&!after.limited115&&after.fraction==1&&after.coverage169r2.reason==sweep169r2::Reason::FullClear,"rung7 actually checks full reconstructed gap");
 ck(clear.called<=18&&after.coverage169r2.completed==after.coverage169r2.planned&&after.coverage169r2.completedRays==clear.called,"all returned full-clear segments/rays actually completed within18");
 printf("SWEEP before fraction=%.6f rays=%u after fraction=%.6f rays=%u segments=%u angleDeg=%.6f\n",before.fraction,old.called,after.fraction,clear.called,after.coverage169r2.completed,after.coverage169r2.checkedAngle*180/3.141592653589793);
 Rays wall;wall.hitAt=12;auto hit=sweep_weapon112(p,p.nativeWeapon,f.desired,false,wall);ck(hit.plane&&hit.fraction<1&&hit.coverage169r2.reason==sweep169r2::Reason::SurfaceHit,"real wall in extended tail stops motion");
 Rays unknown;unknown.unknownAt=8;auto u=sweep_weapon112(p,p.nativeWeapon,f.desired,false,unknown);ck(u.limited115&&u.fraction<1&&u.coverage169r2.completed==2&&u.coverage169r2.completedRays==7,"unknown partway retains only2 fully checked segments");
 Rays cap(7);auto q=sweep_weapon112(p,p.nativeWeapon,f.desired,false,cap);ck(q.fraction<1&&cap.called==7&&q.coverage169r2.completed==2&&q.coverage169r2.reason==sweep169r2::Reason::RayCap,"no replenishment of partially spent shared ray budget");
 Rays timeout;timeout.cost=200;auto x=sweep_weapon112(p,p.nativeWeapon,f.desired,false,timeout);ck(x.fraction<1&&timeout.called==5&&x.coverage169r2.completed==1&&x.coverage169r2.reason==sweep169r2::Reason::TimeBudget,"1000us ceiling cannot mark partial segment checked");
 Rays zero(0);auto z=sweep_weapon112(p,p.nativeWeapon,f.desired,false,zero);ck(!z.known&&z.fraction==0&&zero.called==0,"fully spent budget grants nothing");
 p.nativeRejoinSweepR28=false;Rays off;auto disabled=sweep_weapon112(p,p.nativeWeapon,f.desired,false,off);ck(disabled.fraction==before.fraction&&off.called==old.called,"off is exact prior query behavior");
 p.nativeRejoinSweepR28=true;Rays a,b;auto support=sweep_weapon112(p,p.nativeWeapon,f.desired,true,a);p.nativeRejoinSweepR28=false;auto normalSupport=sweep_weapon112(p,p.nativeWeapon,f.desired,true,b);ck(support.fraction==normalSupport.fraction&&a.called==b.called,"support acquisition coverage unchanged");
}
void kernelTests(){Fixture f;std::array<Profile,2> profiles{};profiles[0]=f.p;
 auto run=[&](bool on,Rays& rays,State& state){f.in.nativeRejoinR28[0]=on;return step(state,f.in,profiles,[&](const Profile&p,Pose a,Pose b,bool acquire){kcd2vr::contact::AcquisitionBurstD45 burst(rays.budget,p.initialAcquireSweepD44&&!acquire);return sweep_weapon112(p,a,b,acquire,rays);});};
 State baseline{};Rays b;auto old=run(false,b,baseline);State repaired{};Rays a;auto now=run(true,a,repaired);
 printf("KERNEL beforeJumpMm=%.6f afterJumpMm=%.6f baselineRays=%u experimentRays=%u\n",m::length(old.hands[0].p-f.previous)*1000,m::length(now.hands[0].p-f.previous)*1000,b.called,a.called);
 ck(old.initialAcquireSweepD44[0]&&m::length(old.hands[0].p-f.in.hands[0].p)<1e-7,"D44 first acquisition also completes checked path without native-return receipt");
 ck(now.nativeRejoinSweepR28[0]&&m::length(now.hands[0].p-f.in.hands[0].p)<1e-7,"actual experiment output reaches current controller after checked sweep");
 ck(!now.displayRejoinR13[0]&&!now.displayLatched[0]&&now.contactKnown[0],"no unchecked display tail or fake contact status used");
 ck(now.commandCount==0&&!now.impactRecord178r30[0].valid&&old.commandCount==0,"return generates no damage/hit/attack command");
 ck(m::length(compose(now.weapons[0],profiles[0].grip[0]).p-now.hands[0].p)<1e-8,"primary wrist remains exactly on unchanged learned grip");
 const auto count=a.called;auto repeat=run(true,a,repaired);ck(a.called==count&&repeat.commandCount==0,"same XR sample replay cannot run query twice or emit command");
 State collision{};Rays wall;wall.hitAt=12;auto blocked=run(true,wall,collision);ck(blocked.hands[0].p.x!=now.hands[0].p.x&&blocked.contact119[0].plane,"kernel retains real collision when new coverage reaches obstacle");
 State scarce{};Rays cap(7);auto limited=run(true,cap,scarce);ck(limited.sweepDeferred115[0]&&!limited.contactKnown[0]&&!limited.displayRejoinR13[0],"partial queried return is still limited, never controller display bypass");
 f.in.contactFaultH30=true;State fault{};Rays faultR;auto faultOut=run(true,faultR,fault);ck(!faultOut.nativeRejoinSweepR28[0],"contact fault cannot enable special coverage");f.in.contactFaultH30=false;
 f.in.intentTracked[0]=false;State held{};Rays heldR;auto heldOut=run(true,heldR,held);ck(!heldOut.nativeRejoinSweepR28[0],"held/lost raw tracking cannot enable special coverage");
}
void twoHandTests(){
 for(bool v120:{false,true}){
  Profile p{};p.key=1;p.entity=42;p.generation=1;p.classId=4;p.primary=p.support=p.geometry=p.twoHanded=p.nativeSeed=true;
  p.tip={0,.9,0};p.axis={0,1,0};p.extent=1;p.weight=2;p.grip[1].p={-.15,0,0};p.supportV120_178r27=v120;
  p.nativeWeapon={m::from_rotation_vector({0,0,-1.05}),{.2,.3,1.3}};
  Input in{};in.allowed=in.contact=in.controllerAnchor119=true;in.epoch=in.bodyEpoch=1;in.sample=1;in.tick=1000;
  in.tracked[0]=in.tracked[1]=in.intentTracked[0]=in.intentTracked[1]=true;
  in.hands[0].p={.2,.3,1.3};in.hands[1].p={.05,.3,1.3};in.intentHands[0]=in.hands[0];in.intentHands[1]=in.hands[1];
  in.shoulders[0]={.2,0,1.3};in.shoulders[1]={-.2,0,1.3};in.supportV120_178r27=v120;in.nativeRejoinR28[0]=true;
  std::array<Profile,2> profiles{};profiles[0]=p;profiles[1].side=1;State state{};
  unsigned laterFlags=0;auto advance=[&]{Rays rays;return step(state,in,profiles,[&](const Profile& query,Pose a,Pose b,bool acquire){if(in.sample>1&&query.nativeRejoinSweepR28)++laterFlags;return sweep_weapon112(query,a,b,acquire,rays);});};
  auto out=advance();ck(out.nativeRejoinSweepR28[0]&&!out.support[0],"post-native acquisition does not invent support latch");
  for(int i=0;i<10;++i){++in.sample;in.tick+=10;in.grip[1]=1;out=advance();}
  ck(out.support[0]&&out.owned[1],"normal support dwell still attaches after qualified native return");
  ck(m::length(compose(out.weapons[0],p.grip[0]).p-out.hands[0].p)<1e-8&&m::length(compose(out.weapons[0],p.grip[1]).p-out.hands[1].p)<1e-8,"both wrists share unchanged exact rigid weapon grip");
  ck(laterFlags==0,"one-shot acquisition coverage does not leak to later movement or support query");
  ++in.sample;in.tick+=10;in.grip[1]=0;out=advance();ck(!out.support[0]&&!out.owned[1]&&out.owned[0],"support release leaves primary ownership intact");
 }
}
int main(){trackerTests();sweepTests();kernelTests();twoHandTests();printf("RESULT %u/%u pass; deterministic actual sweep/kernel and receipt tests, not headset validation.\n",checks-failed,checks);return failed?1:0;}
