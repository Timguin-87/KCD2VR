#include "vrik/semi_workstations46.h"
#include <cassert>
#include <cstdio>
namespace kcd2vr::diagnostic_trace {static unsigned lines=0;void Write(const char*) noexcept {++lines;}}
using namespace kcd2vr;
struct PadD47 {bool a=false,b=false,x=false;float lx=0,ly=0,rt=0,lt=0;};
static workstation_sample46::Frame frameD47(std::uint64_t tick,std::uint64_t seq){
 workstation_sample46::Frame f{};f.sequence=seq;f.tick=tick;f.timeNs=tick*1000000;f.poseEpoch=7;f.sourceEpoch=9;
 f.allowed=f.headValid=true;f.head[1]=1.7f;for(auto& h:f.hand){h.valid=true;h.position[1]=1.3f;}return f;
}
int main(){
 namespace a=alchemy_semi46;namespace sw=semi_workstations46;
 minigame_hands45::mode=minigame_hands45::SemiPassenger;a::hooksReady=true;a::producer.reset();
 sw::State state;sw::Context c{};c.enabled=c.qualified=c.alchemyTracked=true;c.alchemy=10;c.lifetime=20;
 a::WorkWitness w{10,30,40,20,1000,1,a::Bellows,1,2,false,true};a::work.publish(w);
 auto f=frameD47(1000,1);PadD47 pad;state.apply(c,f,1000,pad);assert(!pad.a&&pad.lt==0);
 f=frameD47(1020,2);f.hand[1].position[1]-=.03f;w.tick=1020;a::work.publish(w);state.apply(c,f,1020,pad);
 assert(pad.lt==1&&!pad.a);a::Control control{};assert(a::control.read(control)&&control.use);
 c.qualified=false;pad={};state.apply(c,f,1020,pad);assert(!pad.a&&!pad.lt&&a::control.read(control)&&!control.enabled);
 c={};f=frameD47(1030,3);state.apply(c,f,1030,pad);assert(a::control.read(control)&&!control.enabled);
 const auto revoked=control.sequence;f=frameD47(1040,4);state.apply(c,f,1040,pad);
 assert(a::control.read(control)&&control.sequence==revoked&&!control.enabled); // idle path skips table/adapter republishes
 c.enabled=c.qualified=c.alchemyTracked=true;c.alchemy=10;c.lifetime=20;w.tick=1050;a::work.publish(w);
 f=frameD47(1050,5);state.apply(c,f,1050,pad);assert(a::control.read(control)&&control.enabled&&control.sequence==5);
 assert(diagnostic_trace::lines==0);
 std::puts("D47 workstation routing PASS: native bellows LT, same-frame revoke, idle fast path, station re-entry, diagnostics off");
}
