#include "vrik/gesture_talk59c5.h"
#include <cstdio>
using namespace kcd2vr::gesture_talk59c5;
static int fails=0;static void ck(bool b,const char*m){std::printf("%s %s\n",b?"PASS":"FAIL",m);fails+=!b;}
int main(){
  {State s{};Hand hands[2]{};hands[0].valid=true;hands[0].empty=true;
   hands[0].grip=.9f;hands[0].trigger=.9f;hands[0].up=.3;hands[0].forward=.1;
   ck(!raised(hands[0],Config{}),"closed fist is not the grip-only raised gesture");
   s.pendingOption=2;s.waitUntil=5000;s.openUntil=5000;
   auto out=update(s,1000,hands,true,true,true);
   ck(!out.openChat&&!out.option&&!s.pendingOption,"combat chord cancels pending utility opener and option");
   for(int t=1010;t<1800;t+=10)out=update(s,t,hands,true,true,false);
   ck(s.fired==0,"held combat fist cannot fire a battle cry");
  }
  Hand h[2]{};auto point=[&](Hand& x){x.valid=true;x.grip=.8f;x.trigger=0;x.triggerTouch=false;x.forward=.45;x.up=-.2;};
  auto idle=[&](Hand& x){x=Hand{};x.valid=true;x.grip=.1f;x.triggerTouch=true;x.forward=.1;x.up=-.5;};
  {State s{};idle(h[0]);idle(h[1]);point(h[0]);Output o{};int opens=0;
   for(int t=0;t<=400;t+=10){o=update(s,1000+t,h,true,false,false);opens+=o.openChat;}
   ck(s.fired==1&&s.lastKind==Kind::Talk&&opens>=10&&opens<=13,"point out of combat: one quick-talk tap after 300 ms");
   for(int t=410;t<=3000;t+=10)o=update(s,1000+t,h,true,false,false);ck(s.fired==1,"holding the point does not repeat");
   idle(h[0]);update(s,4100,h,true,false,false);point(h[0]);for(int t=4110;t<=4500;t+=10)update(s,t,h,true,false,false);ck(s.fired==2,"lower and point again: second tap");}
  {State s{};idle(h[0]);idle(h[1]);point(h[1]);bool opt=false,blind=false;
   for(int t=0;t<=330;t+=10){auto o=update(s,1000+t,h,true,true,false);blind|=o.option!=0;}
   ck(s.pendingOption==3&&!blind,"point in combat: taunt pending, no blind option press while the chat is closed");
   int held=0;for(int t=340;t<=600;t+=10){auto o=update(s,1000+t,h,true,true,true);opt|=o.option==3;held+=o.openChat;}
   ck(opt&&held>10&&s.optionsSent==1,"chat open: option 3 (taunt) pressed while the opener is held");}
  {State s{};idle(h[0]);idle(h[1]);h[0].valid=true;h[0].grip=.9f;h[0].up=.3;h[0].forward=.1;
   for(int t=0;t<=450;t+=10)update(s,1000+t,h,true,true,false);ck(s.pendingOption==2,"hand raised over the head in combat: battle cry pending");
   for(int t=460;t<=1400;t+=10)update(s,1000+t,h,true,true,false);ck(s.chatTimeouts==1&&!s.pendingOption,"chat never opened: timeout, nothing pressed");}
  {State s{};idle(h[0]);idle(h[1]);h[0].valid=true;h[0].grip=.9f;h[0].up=.3;h[0].forward=.1;
   for(int t=0;t<=600;t+=10)update(s,1000+t,h,true,false,false);ck(s.fired==0,"raised hand outside combat does nothing");}
  {State s{};idle(h[0]);idle(h[1]);point(h[0]);h[0].triggerTouch=true;
   for(int t=0;t<=600;t+=10)update(s,1000+t,h,true,false,false);ck(s.fired==0,"finger resting on the trigger is not pointing");}
  {State s{};idle(h[0]);idle(h[1]);point(h[0]);
   for(int t=0;t<=600;t+=10)update(s,1000+t,h,false,false,false);ck(s.fired==0,"switched off: nothing");}
  ck(OutwardPointR26(0,-.1,-.5,0,0,-1,0,-1),"outward forward point qualifies");
  ck(!OutwardPointR26(0,-.1,-.5,0,0,1,0,-1),"inward aim must not claim shoulder withdrawal");
  ck(!OutwardPointR26(.5,0,0,-1,0,0,0,-1),"cross-body inward aim is not a point");
  ck(!OutwardPointR26(0,0,0,0,0,-1,0,-1),"degenerate relative position is not a point");
  {State s{};idle(h[0]);idle(h[1]);point(h[0]);h[0].outwardR26=false;
   for(int t=0;t<=600;t+=10){auto o=update(s,1000+t,h,true,false,false);ck(!o.claimMaskH4&&!o.openChat,"inward pose leaves holster ownership free");}
   h[0].outwardR26=true;update(s,2000,h,true,false,false);auto o=update(s,2401,h,true,false,false);ck(o.openChat,"outward point retains normal chooser");}
  std::printf("%d failures\n",fails);return fails;}
