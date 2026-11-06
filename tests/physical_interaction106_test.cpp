#include "vrik/physical_interaction.h"
#include <cstdio>
#include <cstdlib>
using namespace kcd2vr::physical_interaction106;
static int n=0;
static void ck(bool v,const char* m){++n;if(!v){std::fprintf(stderr,"FAIL %s\n",m);std::exit(1);}}
int main(){
 ck(EffectiveReach(20.0f,true,.40f)==.40f,"hand scope limits native reach");
 ck(EffectiveReach(.25f,true,.40f)==.25f,"hand scope never extends native reach");
 ck(EffectiveRadius(.21f,true,.14f)==.14f,"hand scope uses physical acquisition radius");
 ck(EffectiveRadius(.21f,false,.14f)==.21f,"ordinary interaction radius preserved");
 State s{};Input in{};in.enabled=true;in.now_ms=100;in.hand[0]={true,true,true,0.0f};in.hand[1]={true,true,true,0.0f};
 auto o=Step(s,in);ck(!o.fire_use&&!o.ray_active,"idle");
 in.hover_age_ms[0]=150; // V66.178R7: an established hover keeps the 20 ms pulse
 in.hand[0].grip=.7f;in.now_ms=110;o=Step(s,in);ck(!o.fire_use&&o.ray_active&&o.ray_owner==0,"right grip edge arms ray before use");
 in.now_ms=120;o=Step(s,in);ck(!o.fire_use&&o.ray_active,"short pre-use delay");
 in.now_ms=131;o=Step(s,in);ck(o.fire_use&&o.ray_active,"use fires once after ray publication");
 CompleteUse116(s,o,false);in.now_ms=132;o=Step(s,in);ck(o.fire_use,"failed ray publication preserves pending use inside the original lease");
 CompleteUse116(s,o,true);in.now_ms=133;o=Step(s,in);ck(!o.fire_use,"successful transport consumes the pulse once");
 in.hand[0].grip=0.0f;in.now_ms=140;o=Step(s,in);ck(!o.fire_use&&o.ray_active,"release keeps short native selection window");
 in.now_ms=461;o=Step(s,in);ck(!o.ray_active&&o.ray_owner<0,"ray expires");
 in.hand[0].neutral=false;in.hand[0].grip=.7f;in.now_ms=470;o=Step(s,in);ck(!o.fire_use,"modifier direction blocks use");
 in.hand[0].grip=0;in.hand[0].neutral=true;in.now_ms=480;Step(s,in);
 in.hand[0].empty=false;in.hand[0].grip=.7f;in.now_ms=490;o=Step(s,in);ck(!o.fire_use,"occupied hand blocks use");
 in.hand[0].grip=0;in.hand[0].empty=true;in.now_ms=500;Step(s,in);
 in.hand[0].grip=.7f;in.hand[1].grip=.7f;in.now_ms=510;o=Step(s,in);ck(!o.fire_use&&o.simultaneous_reject,"simultaneous grips fail closed");
 in.enabled=false;in.now_ms=520;o=Step(s,in);ck(!o.ray_active,"mode off cancels ray");
 in.enabled=true;in.hand[0].grip=.7f;in.hand[1].grip=0;in.now_ms=530;o=Step(s,in);ck(!o.fire_use,"enter held requires release");
 in.hand[0].grip=0;in.now_ms=540;Step(s,in);in.hand[0].grip=.7f;in.now_ms=550;o=Step(s,in);ck(o.ray_active&&!o.fire_use,"release rearms new ray");in.now_ms=571;o=Step(s,in);ck(o.fire_use,"rearmed use fires after delay");
 CompleteUse116(s,o,false);in.now_ms=901;o=Step(s,in);ck(!o.fire_use&&!o.ray_active&&!s.use_pending,"unpublished pending use expires instead of extending stale ownership");
 { // V66.178R7: a brand-new hand ray (no hover yet) waits 120 ms for the native target query before Use
  State s2{};Input j{};j.enabled=true;j.hand[0]={true,true,true,0.0f,false};j.now_ms=1000;Step(s2,j);
  j.hand[0].grip=.7f;j.now_ms=1010;auto r=Step(s2,j);ck(r.ray_active&&!r.fire_use,"new ray armed");
  j.now_ms=1100;r=Step(s2,j);ck(r.ray_active&&!r.fire_use,"no use 90 ms after a brand-new ray");
  j.now_ms=1131;r=Step(s2,j);ck(r.ray_active&&r.fire_use,"use fires at 120 ms once the native query had time to select the hand target");}
 std::printf("PASS %d physical interaction checks\n",n);return 0;
}
