#include "vrik/semi_workstations46.h"
#include <cassert>
#include <cstdio>
namespace kcd2vr::diagnostic_trace { void Write(const char*) noexcept {} }
namespace sw=kcd2vr::semi_workstations46;
namespace bridge=kcd2vr::subsystems::smithing::bridge67;
namespace mh=kcd2vr::minigame_hands45;
namespace ss=kcd2vr::semi_smith46;
static unsigned checks=0;
static void check(bool b){++checks;if(!b){std::fprintf(stderr,"bound tool assertion %u failed\n",checks);std::abort();}}
struct Pad {bool a=false,b=false,x=false;float lx=0,ly=0,rt=0,lt=0;};
struct Run {
    sw::State state{};sw::Context context{};kcd2vr::workstation_sample46::Frame frame{};
    Run(bool gesture=true){
        context.enabled=context.qualified=context.smithTracked=context.smithGestures=true;
        context.smith=0x100;context.lifetime=7;context.smithPhase=11;context.gestureSmith67=gesture;
        frame.allowed=frame.headValid=true;frame.tick=1000;frame.sequence=1;frame.timeNs=1000000000;
        frame.poseEpoch=8;frame.sourceEpoch=9;frame.head[1]=1.7f;
        for(auto& h:frame.hand){h.valid=true;h.position[1]=1.1f;}
        bridge::tools[0].publish({});bridge::tools[1].publish({});ss::clockReady=true;
    }
    bridge::NativeTool tool(int side){bridge::NativeTool t{};t.joined=true;t.tick=frame.tick;
        t.poseEpoch=frame.poseEpoch;t.modeEpoch=mh::modeEpoch67.load();auto& b=t.binding;
        b.instance=context.smith;b.lifetime=context.lifetime;b.player=1;b.character=2;b.attachment=3;
        b.model=4;b.holder=5;b.object=6;b.entity=7;b.sourceEntity=7;b.side=side;b.phase=context.smithPhase;
        b.tool=side?1:0;b.key=9;return t;}
    void next(){frame.tick+=10;frame.timeNs+=10000000;++frame.sequence;}
    Pad step(bool manualX=false){Pad p{};p.x=manualX;state.apply(context,frame,frame.tick,p);return p;}
    void publish(int side){bridge::tools[side].publish(tool(side));}
    void roll(float radians){frame.hand[1].rotation[2]=std::sin(radians*.5f);frame.hand[1].rotation[3]=std::cos(radians*.5f);}
};
int main(){
    Run empty;empty.step();for(int i=0;i<5;++i){empty.next();empty.frame.hand[0].position[1]+=.04f;check(!empty.step().rt);}
    check(!empty.state.lastSmith.axesOwned);
    Run right;right.publish(0);right.step();right.next();right.publish(0);right.step();right.next();right.publish(0);right.step();
    right.next();right.publish(0);right.frame.hand[0].position[1]+=.04f;
    check(right.step().rt>0&&right.state.lastSmith.clock.enabled&&!right.state.lastSmith.axesOwned);
    right.publish(1);right.next();right.publish(0);right.publish(1);right.step();
    right.next();right.publish(0);right.publish(1);right.frame.hand[1].position[2]-=.0028f;right.step();
    check(right.state.lastSmith.axesOwned&&right.state.lastSmith.y>0);
    right.next();right.publish(0);right.publish(1);const auto stillH21=right.step();
    check(right.state.lastSmith.axesOwned&&stillH21.lx==0&&stillH21.ly==0);
    right.next();right.publish(0);right.publish(1);right.frame.hand[1].position[2]+=.0028f;
    const auto reverseH21=right.step();check(reverseH21.ly<0&&reverseH21.ly>=-1&&reverseH21.lx==0);
    // Each tool proof is independently current; a valid left tool does not
    // authorize an empty/foreign/stale right hand to start/continue a stroke.
    for(int fault=0;fault<9;++fault){Run r;auto t=r.tool(0);
        if(fault==0)t.binding.tool=1;if(fault==1)t.binding.side=1;if(fault==2)++t.binding.lifetime;
        if(fault==3)++t.binding.instance;if(fault==4)++t.poseEpoch;if(fault==5)++t.modeEpoch;
        if(fault==6)t.tick=r.frame.tick-81;if(fault==7)t.binding.phase=-1;if(fault==8)t.joined=false;
        check(!bridge::qualified(t,r.frame.tick,0,r.context.smith,r.context.lifetime,r.frame.poseEpoch,mh::modeEpoch67.load()));
        bridge::tools[0].publish(t);r.step();r.next();r.frame.hand[0].position[1]+=.04f;check(!r.step().rt);
    }
    auto proof=right.tool(0);proof.binding.phase=14;check(bridge::qualified(proof,right.frame.tick,0,right.context.smith,right.context.lifetime,right.frame.poseEpoch,mh::modeEpoch67.load()));
    // Saved Semi behavior is deliberately unaffected by Gesture-only gating.
    Run semi(false);semi.step();semi.next();semi.step();semi.next();semi.step();semi.next();semi.frame.hand[0].position[1]+=.04f;
    check(semi.step().rt>0);
    // H12: the ACTUAL final-pad adapter must supply the existing native flip
    // input in Gesture Smithing without F5=3/guided Dock publication. Current
    // native tool membership substitutes only that already-held tool witness;
    // it does not create a second axis, hammer or animation-clock producer.
    kcd2vr::guided_crafting55::enabled=false;
    for(auto& d:kcd2vr::guided_crafting55::dock)d.publish({});
    {Run r;r.frame.hand[1].grip=1;r.publish(1);check(!r.step().x);
        for(int i=0;i<=10;++i){r.next();r.roll(.8f);r.publish(1);const auto p=r.step();
            check(p.x==(i==10)&&!p.a&&!p.rt&&!p.lx&&!p.ly);}
        check(r.state.guidedSmith.flipConsumed&&!r.state.guidedSmith.flipAcknowledged);
        for(int i=0;i<15;++i){r.next();r.publish(1);const auto p=r.step();check(!p.a&&!p.rt);
            if(i>=7)check(!p.x);}
        r.context.smithPhase=12;r.next();r.publish(1);check(!r.step().x&&r.state.guidedSmith.flipAcknowledged);
        r.context.smithPhase=11;r.next();r.publish(1);check(!r.step().x&&r.state.guidedSmith.flipConsumed);
        r.next();r.roll(0);r.publish(1);check(!r.step().x&&!r.state.guidedSmith.flipConsumed);
        for(int i=0;i<=10;++i){r.next();r.roll(.8f);r.publish(1);check(r.step().x==(i==10));}
    }
    // No gesture flip from stale/wrong native tool proof, revoked native/UI
    // authority, untracked smith mode or an explicitly disabled gesture setting.
    for(int fault=0;fault<6;++fault){Run r;r.frame.hand[1].grip=1;
        for(int i=0;i<18;++i){if(i)r.next();r.roll(i?.8f:0);auto t=r.tool(1);
            if(fault==0)t.tick=r.frame.tick-81;if(fault==1)++t.binding.lifetime;
            if(fault==2)++t.modeEpoch;if(fault==3)r.context.qualified=false;
            if(fault==4)r.context.smithTracked=false;if(fault==5)r.context.smithGestures=false;
            bridge::tools[1].publish(t);check(!r.step().x&&!r.state.guidedSmith.flipConsumed);}
    }
    // Real X stays real X; switching the saved mode while twisted rebases and
    // cannot reuse the old dwell to generate a delayed extra request.
    {Run r;r.frame.hand[1].grip=1;r.publish(1);r.step();
        for(int i=0;i<5;++i){r.next();r.roll(.8f);r.publish(1);check(!r.step().x);}
        r.next();r.publish(1);check(r.step(true).x&&!r.state.guidedSmith.flipConsumed);
        mh::setMode(mh::Smithing,mh::SemiPassenger);r.context.gestureSmith67=false;
        r.next();r.publish(1);check(!r.step().x);
        mh::setMode(mh::Smithing,mh::Gesture);r.context.gestureSmith67=true;
        for(int i=0;i<18;++i){r.next();r.publish(1);check(!r.step().x);}
    }
    std::printf("PASS G1 bound smith tools: %u assertions; actual input adapter with explicit native proof leaves\n",checks);
}
