// The same final pad adapter included by Mod: current native input authority,
// manual controls, source revocation, and the retired legacy pulse producer.
#include "vrik/semi_workstations46.h"
#include "vrik/workstation178r20c1.h"
#include <cassert>
#include <cmath>
#include <cstdio>
using namespace kcd2vr;
namespace kcd2vr::diagnostic_trace {void Write(const char*) noexcept {}}
namespace sa=alchemy_semi46;
namespace ss=semi_smith46;
namespace sw=semi_workstations46;
static unsigned checks=0;
static void ck(bool value){++checks;if(!value)std::fprintf(stderr,"check %u failed\n",checks);assert(value);}
struct Pad {bool a=false,b=false,x=false,lb=false,rb=false;float lx=0,ly=0,rt=0,lt=0;};
static workstation_sample46::Frame frame(std::uint64_t tick,std::uint64_t seq){
    workstation_sample46::Frame f{};f.sequence=seq;f.tick=tick;f.timeNs=tick*1000000;
    f.poseEpoch=7;f.sourceEpoch=9;f.allowed=f.headValid=true;f.head[1]=1.7f;
    for(auto& h:f.hand){h.valid=true;h.position[1]=1.2f;h.position[2]=-.3f;}
    f.hand[0].position[0]=.2f;f.hand[1].position[0]=-.2f;return f;
}
static void reset(){
    minigame_hands45::mode=minigame_hands45::SemiPassenger;
    sa::producer.reset();sa::publishedSequence=sa::publishedTick=0;
    sa::work.publish({});sa::control.publish({});sa::hooksReady=true;
    ss::clockReady=false;ss::clockInterested=false;ss::clockControl.publish({});
}

static void toolsNow(const sw::Context& c,const workstation_sample46::Frame& f){
 namespace b=kcd2vr::subsystems::smithing::bridge67;
 for(int side=0;side<2;++side){b::NativeTool t{};t.joined=true;t.tick=f.tick;t.poseEpoch=f.poseEpoch;t.modeEpoch=minigame_hands45::modeEpoch67.load();
  auto& x=t.binding;x.instance=c.smith;x.lifetime=c.lifetime;x.player=10;x.character=20;x.attachment=30+side;x.model=40;x.object=50+side;x.sourceEntity=x.entity=3+side;x.holder=60+side;x.side=side;x.phase=c.smithPhase;x.tool=side;x.key=80+side;
  b::tools[side].publish(t);
 }
}
int main(){
 for(int phase:{8,10,11,12,14,16,17,18})for(bool enabled:{false,true})for(float lt:{0.f,1.f}){
  reset();minigame_hands45::setMode(minigame_hands45::Smithing,3);crafting_d67::setSmithSemiAnchors(enabled);
  sw::State s;sw::Context c{};c.enabled=c.qualified=c.smithTracked=c.gestureSmith67=true;c.smith=11;c.lifetime=21;c.smithPhase=phase;
  auto f=frame(2000,1);f.hand[1].trigger=lt;toolsNow(c,f);Pad p;p.lt=lt;p.rt=.7f;s.apply(c,f,2000,p);
  const bool held=enabled&&lt>0&&(phase==11||phase==14||phase==16||phase==17);
  ck(p.lt==(held?0.f:lt));ck(p.rt==.7f);if(held)ck(!s.lastSmith.axesOwned);
  f=frame(2010,2);f.hand[1].trigger=0;toolsNow(c,f);p={};s.apply(c,f,2010,p);ck(p.lt==0);
 }
 for(int invalid=0;invalid<5;++invalid){
  reset();minigame_hands45::setMode(minigame_hands45::Smithing,3);crafting_d67::setSmithSemiAnchors(true);
  sw::State s;sw::Context c{};c.enabled=c.qualified=c.smithTracked=c.gestureSmith67=true;c.smith=11;c.lifetime=21;c.smithPhase=11;
  auto f=frame(3000,1);f.hand[1].trigger=1;toolsNow(c,f);
  switch(invalid){case 0:f.hand[1].valid=false;break;case 1:f.allowed=false;break;case 2:f.tick=2919;break;case 3:c.qualified=false;break;case 4:subsystems::smithing::bridge67::tools[1].publish({});break;}
  Pad p;p.lt=1;s.apply(c,f,3000,p);ck(p.lt==1);
 }
 std::printf("D80 LT actual station input: %u checks PASS; heating/manual hammer/release/native LT preserved\n",checks);
}
