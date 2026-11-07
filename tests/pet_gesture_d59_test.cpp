#include "../src/subsystems/interactions/pet_gesture_d59.h"
#include <cassert>
#include <limits>
using namespace kcd2vr::petting_d59;
static Frame frame(std::uint64_t tick,float x=0.f){
 Frame f{};f.tick=tick;f.body=2;f.tracking=3;f.policy=4;f.player=5;f.playerIdentity=6;f.permitted=true;
 for(int s=0;s<2;++s){auto&h=f.hand[s];h.tracked=h.empty=h.free=true;h.palm[0]=s?1.f:x;h.target=7;
  h.dogCandidate=!s;h.distance=.1f;h.candidateTick=tick;h.candidateGeneration=8;h.candidateContext=9;}
 return f;
}
int main(){
 Recognizer r;
 // One open-hand caress, no Grip press, is enough; no periodic replay.
 assert(r.update(frame(1000))<0);assert(r.update(frame(1060,.04f))<0);
 assert(r.update(frame(1120,.081f))==0);
 for(unsigned i=1;i<=30;++i)assert(r.update(frame(1120+40*i,.081f))<0);
 // A selector loss alone cannot rearm, nor can a tracking interruption.
 auto f=frame(2400,.081f);f.hand[0].target=0;f.hand[0].dogCandidate=false;
 assert(r.update(f)<0);f.tick=2600;assert(r.update(f)<0);
 f=frame(2700,.081f);f.hand[0].tracked=false;assert(r.update(f)<0);
 assert(r.update(frame(2800,.081f))<0);
 // Positive physical withdrawal then a new caress re-arms the gesture.
 for(unsigned t=3000;t<=3200;t+=40)assert(r.update(frame(t,.6f))<0);
 assert(r.update(frame(3300))<0);assert(r.update(frame(3360,.04f))<0);assert(r.update(frame(3420,.081f))==0);
 // Fingers closed/trigger raised, another owner, stale candidates, invalid
 // coordinates, remote dog, candidate switch and jumps cannot request Bond.
 for(unsigned mode=0;mode<11;++mode){r.reset();for(unsigned i=0;i<4;++i){
   auto q=frame(1000+40*i,.03f*i);
   if(mode==0)q.hand[0].grip=.7f;
   if(mode==1)q.hand[0].trigger=.7f;
   if(mode==2)q.hand[0].empty=false;
   if(mode==3)q.hand[0].free=false;
   if(mode==4)q.hand[0].candidateTick=q.tick-81;
   if(mode==5)q.hand[0].palm[0]=std::numeric_limits<float>::quiet_NaN();
   if(mode==6)q.hand[0].distance=.21f;
   if(mode==7)q.hand[0].target+=i;
   if(mode==8)q.hand[0].palm[0]=.5f*i;
   if(mode==9)q.permitted=false;
   if(mode==10)q.body=0;
   assert(r.update(q)<0);
 }}
 // Jitter cannot accumulate its way through a caress threshold.
 r.reset();for(unsigned i=0;i<30;++i)assert(r.update(frame(1000+20*i,(i%2)*.003f))<0);
 // Same tick can be processed repeatedly without integrating bogus travel.
 r.reset();assert(r.update(frame(1000))<0);for(int i=0;i<30;++i)assert(r.update(frame(1000,.1f))<0);
 // Two simultaneously eligible hands have one globally consumed episode.
 r.reset();for(unsigned i=0;i<3;++i){auto q=frame(1000+60*i,.041f*i);q.hand[1]=q.hand[0];
  const int hit=r.update(q);assert(hit==(i==2?0:-1));}
 assert(r.update(frame(1180,.1f))<0);
 // R1: a candidate source change cannot join travel from an earlier source.
 for(unsigned change=0;change<2;++change){
  r.reset();assert(r.update(frame(1000))<0);assert(r.update(frame(1060,.04f))<0);
  auto source=frame(1120,.081f);
  if(change==0)++source.hand[0].candidateGeneration;else ++source.hand[0].candidateContext;
  assert(r.update(source)<0);assert(!r.latched);
 }
 // A zero-body interruption also discards an UNCOMMITTED partial stroke.
 r.reset();assert(r.update(frame(1000))<0);assert(r.update(frame(1060,.04f))<0);
 Frame gap{};gap.tick=1090;assert(r.update(gap)<0);
 assert(r.update(frame(1120,.081f))<0&&!r.latched);
 // R3: the production body reader returns {}, not just tracked=false. A spent
 // gesture survives unavailable identities and does not count the gap as away.
 r.reset();assert(r.update(frame(1000))<0);assert(r.update(frame(1060,.04f))<0);
 assert(r.update(frame(1120,.081f))==0);assert(r.latched);
 Frame unavailable{};unavailable.tick=1180;unavailable.policy=4;
 assert(r.update(unavailable)<0&&r.latched);
 assert(r.update(frame(1240,.081f))<0);assert(r.update(frame(1300,.12f))<0);
 assert(r.update(frame(1360,.17f))<0&&r.latched); // no withdrawal
 assert(r.update(frame(1420,.6f))<0);assert(r.awaySince==1420);
 unavailable.tick=1480;assert(r.update(unavailable)<0&&r.latched&&r.awaySince==0);
 assert(r.update(frame(1600,.6f))<0&&r.latched); // starts a NEW positive interval
 assert(r.update(frame(1660,.6f))<0&&r.latched);
 assert(r.update(frame(1720,.6f))<0&&r.latched);
 assert(r.update(frame(1780,.6f))<0&&!r.latched);
 assert(r.update(frame(1840))<0);assert(r.update(frame(1900,.04f))<0);
 assert(r.update(frame(1960,.081f))==0);
 // A positively valid new body/lifetime is an intentional coordinate reset.
 auto changed=frame(2020);++changed.body;assert(r.update(changed)<0&&!r.latched);
 changed=frame(2080,.04f);++changed.body;assert(r.update(changed)<0);
 changed=frame(2140,.081f);++changed.body;assert(r.update(changed)==0);
 // A refused/policy-interrupted sample cannot itself rearm a consumed stroke;
 // a later positively valid different policy is an explicit new episode.
 auto refused=changed;refused.tick=2200;refused.permitted=false;++refused.policy;
 assert(r.update(refused)<0&&r.latched);
 refused.permitted=true;assert(r.update(refused)<0&&!r.latched);
}
