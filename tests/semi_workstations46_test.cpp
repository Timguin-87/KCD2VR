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
int main(){
    reset();
    {
        sw::State s;sw::Context c{};c.enabled=c.qualified=c.alchemyTracked=true;c.alchemy=10;c.lifetime=20;
        sa::WorkWitness w{};w.instance=10;w.lifetime=20;w.parent=30;w.child=40;w.token=1;
        w.kind=sa::Bellows;w.phase=1;w.ready=true;w.tick=1000;w.hands=1;sa::work.publish(w);
        auto f=frame(1000,1);Pad p;s.apply(c,f,1000,p);ck(!p.a&&!p.rt&&!ss::clockInterested);
        f=frame(1020,2);f.hand[0].position[1]-=.03f;w.tick=1020;sa::work.publish(w);s.apply(c,f,1020,p);
        ck(!p.a&&p.lt==1.f); // alch_use_bag is native LT, not contextual A
        sa::Control command{};ck(sa::control.read(command)&&command.use&&command.useUntil==1100);
        c.qualified=false;p={};s.apply(c,f,1020,p);ck(!p.a); // same XR frame, native input revoked
        ck(sa::control.read(command)&&!command.enabled);
        c.qualified=true;c.enabled=false;p={};p.a=true;p.lx=.4f;p.rt=.7f;
        minigame_hands45::mode=minigame_hands45::Native;s.apply(c,f,1020,p);
        ck(p.a&&p.lx==.4f&&p.rt==.7f); // Native selection preserves the actual native pad
        ck(sa::control.read(command)&&!command.enabled&&!ss::clockInterested);
        minigame_hands45::mode=minigame_hands45::SemiPassenger;c.enabled=true;c.smith=11;
        p={};s.apply(c,f,1020,p);ck(!p.a&&!p.rt); // contradictory native activity witness owns nothing
        s.revoke(1030);ck(sa::control.read(command)&&!command.enabled&&!command.use);
    }
    reset();
    {
        sw::State s;sw::Context c{};c.enabled=c.qualified=c.smithTracked=true;
        c.smith=11;c.lifetime=21;c.smithPhase=11;
        auto f=frame(2000,1);Pad p;s.apply(c,f,2000,p);ck(!p.rt&&ss::clockInterested);
        f=frame(2020,2);f.hand[1].position[0]+=.004f;f.hand[1].position[2]-=.004f;
        p={};s.apply(c,f,2020,p);ck(p.lx>.6f&&p.ly>.6f&&!p.rt);
        ck(s.lastSmith.axesOwned&&s.lastSmith.reason==ss::Reason::ClockUnavailable);
        // The native clock is required for the hammer. Native iron input rates
        // remain useful when that optional exact child hook cannot be installed.
        f.tick=2040;f.timeNs=2040000000;f.sequence=3;p={};s.apply(c,f,2040,p);
        ck(p.lx==0&&p.ly==0&&!p.rt); // zero hand motion means zero joystick rate
        f.head[0]+=.2f;f.head[1]-=.2f;f.tick=2060;f.timeNs=2060000000;f.sequence=4;
        p={};s.apply(c,f,2060,p);ck(p.lx==0&&p.ly==0); // leaning alone does not move iron
        f.hand[1].position[0]+=.02f;f.tick=2080;f.timeNs=2080000000;f.sequence=5;
        p={};p.lx=-.5f;p.ly=.3f;s.apply(c,f,2080,p);ck(p.lx==-.5f&&p.ly==.3f&&!s.lastSmith.axesOwned);
        f.tick=2100;f.timeNs=2100000000;f.sequence=6;p={};s.apply(c,f,2100,p);ck(p.lx==0&&p.ly==0);
        f.tick=2120;f.timeNs=2120000000;f.sequence=7;p={};s.apply(c,f,2120,p);ck(p.lx==0&&p.ly==0);
        c.qualified=false;f.hand[1].position[0]+=.02f;p={};s.apply(c,f,2120,p);
        ss::ClockControl clock{};ck(!p.lx&&!p.ly&&!p.rt&&!ss::clockInterested);
        ck(ss::clockControl.read(clock)&&!clock.enabled&&!clock.request);
        c.qualified=true;c.smithTracked=false;p={};p.rt=.65f;s.apply(c,f,2120,p);ck(p.rt==.65f&&!ss::clockInterested);
        c.smithTracked=true;c.smithGestures=false;p={};s.apply(c,f,2120,p);ck(!p.lx&&!p.ly&&!p.rt&&!ss::clockInterested);
        c.smithGestures=true;f.allowed=false;p={};s.apply(c,f,2120,p);ck(!p.lx&&!p.ly&&!p.rt);
        s.revoke(2140);ck(ss::clockControl.read(clock)&&!clock.enabled&&!clock.request&&!ss::clockInterested);
    }
    {
        namespace ws=workstation178r20c1;
        combat_rules178r10::workstationEnabled=true;combat_rules178r10::f5Rung=2;ws::setExperiment36(true);
        minigame_hands45::publish(minigame_hands45::Smithing,3000);
        minigame_hands45::mode=minigame_hands45::Native;
        ck(ws::ownsWrist(3000)&&!ws::ownsActivityWrist45(3000)); // explicit Native beats a leftover experiment
        ws::Controller old;std::array<ws::Sample,2> h{};
        for(auto& v:h){v.valid=true;v.height=1.1f;v.grip=1;v.tick=3000;v.sequence=1;v.timeNs=3000000000;}
        auto r=old.update(ws::Kind::Anvil,11,11,h,3000);ck(!r.use&&!r.stroke&&!old.ownsGrip(0,3000));
        h[0].height=.4f;h[0].tick=3020;h[0].sequence=2;h[0].timeNs=3020000000;
        r=old.update(ws::Kind::Anvil,11,14,h,3020);ck(!r.use&&!r.stroke&&old.strokeEnd==0);
        Pad p;old.apply(3020,p.a,p.lb,p.rb,p.rt);ck(!p.a&&!p.rt); // old 140 ms pulse is gone
        minigame_hands45::mode=minigame_hands45::SemiPassenger;
        ck(ws::ownsActivityWrist45(3000));
        ws::gesturesEverywhere59c2=false;combat_rules178r10::f5Rung=0;ws::setExperiment36(false);
        ck(ws::gesturesOn(3000)); // the selected Semi setting cannot be starved by the old INI experiment switch
    }
    std::printf("semi_workstations46: %u connected pad/mode/source/legacy checks PASS\n",checks);
}
