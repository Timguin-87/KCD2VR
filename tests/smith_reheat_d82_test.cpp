// Real station-to-pad adapter; only trace output and copied native tool leaves
// are substituted. No collision, charge, quality or hit evaluator is simulated.
#include "vrik/semi_workstations46.h"
#include <cstdio>
#include <cstdlib>
#include <limits>

namespace k=kcd2vr;
namespace kcd2vr::diagnostic_trace {void Write(const char*) noexcept {}}
static unsigned checks=0;
static void ck(bool ok,const char* why){++checks;if(!ok){std::fprintf(stderr,"FAIL %u: %s\n",checks,why);std::exit(1);}}
struct Pad {bool a=false,b=false,x=false,lb=false,rb=false;float lx=0,ly=0,rt=0,lt=0;};
struct Driver {
    k::semi_workstations46::State state;
    k::semi_workstations46::Context context{};
    k::workstation_sample46::Frame frame{};
    Pad pad{};unsigned dt=11;
    bool hammer=true;
    Driver(unsigned stepMs=11):dt(stepMs){
        k::minigame_hands45::setMode(k::minigame_hands45::Smithing,3);
        k::crafting_d67::setSmithRecovery(true);
        k::semi_smith46::clockReady=false;
        k::semi_smith46::clockControl.publish({});
        context.enabled=context.qualified=context.smithTracked=context.smithGestures=context.gestureSmith67=true;
        context.smith=0x100;context.lifetime=7;context.smithPhase=11;
        frame.allowed=frame.headValid=true;frame.poseEpoch=8;frame.sourceEpoch=9;
        frame.sequence=1;frame.tick=1000;frame.timeNs=1000000000;frame.head[1]=1.65f;
        for(auto& h:frame.hand){h.valid=true;h.position[1]=1.2f;h.position[2]=-.3f;}
    }
    void tools(){
        namespace b=k::subsystems::smithing::bridge67;
        for(int side=0;side<2;++side){b::NativeTool t{};
            t.joined=side||hammer;t.tick=frame.tick;t.poseEpoch=frame.poseEpoch;
            t.modeEpoch=k::minigame_hands45::modeEpoch67.load();
            auto& x=t.binding;x.instance=context.smith;x.lifetime=context.lifetime;
            x.player=10;x.character=20;x.attachment=30+side;x.model=40;x.object=50+side;
            x.sourceEntity=x.entity=3+side;x.holder=60+side;x.side=side;x.phase=context.smithPhase;
            x.tool=side;x.key=80+side;b::tools[side].publish(t);
        }
    }
    k::semi_smith46::Output step(int phase,float y,float manual=0.f){
        context.smithPhase=phase;frame.hand[0].position[1]=y;tools();pad={};pad.rt=manual;
        state.apply(context,frame,frame.tick,pad);
        const auto result=state.lastSmith;
        ck(!result.clock.enabled&&!result.clock.request,"saved 6DOF never enters legacy work clock");
        ck(pad.rt==(result.requestRT?1.f:manual),"actual final pad matches exactly one RT producer");
        frame.tick+=dt;frame.timeNs+=std::uint64_t(dt)*1000000;++frame.sequence;
        return result;
    }
    void heat(){
        step(13,1.15f);hammer=false;step(8,1.12f);step(8,1.10f);
        step(10,1.08f);hammer=true;step(11,1.06f);
    }
    void lower(){
        for(int i=1;i<=7;++i){const auto o=step(11,1.06f-float(i)*.012f);
            ck(!o.requestRT&&!o.strokeStarted,"lowering only arms, never requests a native hit");}
    }
    void liftAndFall(){
        bool raised=false;float y=.976f;
        for(int i=1;i<=4;++i){y=.976f+float(i)*.012f;const auto o=step(11,y);
            raised=raised||o.strokeStarted;
        }
        ck(raised&&pad.rt==1.f,"continuous post-reheat lift reaches native RT without a stop");
        ck(step(14,y+.012f).requestRT,"native acknowledgement retains real charge/raise");
        const auto release=step(14,y-.020f);
        ck(release.strokeReleased&&!release.requestRT,"measured downward reversal releases once");
        ck(release.clock.fallD81.request!=0,"only acknowledged raise/reversal earns native fall pacing");
        ck(!step(16,y-.032f).requestRT,"native fall cannot generate a second RT request");
        ck(!step(17,y-.044f).requestRT,"native completion cannot generate a second RT request");
    }
};
static void movingReturn(){
    for(unsigned dt:{8u,9u,11u,16u}){Driver d(dt);
        for(int visit=0;visit<3;++visit){d.heat();d.lower();d.liftAndFall();}
    }
}
static void noPassiveOrForeignPress(){
    Driver rise;rise.heat();
    for(int i=1;i<12;++i)ck(!rise.step(11,1.06f+.012f*i).requestRT,"moving entry alone cannot start a stroke");
    Driver down;down.heat();down.lower();
    for(int i=0;i<8;++i)ck(!down.step(11,.976f+(i%2)*.004f).requestRT,"small hand tremor cannot start a stroke");
    for(int phase:{8,10,12,13,14,16,17,18}){Driver d;d.heat();
        for(int i=0;i<10;++i)ck(!d.step(phase,1.06f+(i<5?-.012f*i:.012f*(i-10))).requestRT,
            "non-idle motion never earns a new hammer stroke");
    }
}
static void boundaries(){
    for(int boundary=0;boundary<9;++boundary){Driver d;d.heat();d.lower();
        switch(boundary){
            case 0:++d.frame.poseEpoch;break;
            case 1:++d.frame.sourceEpoch;break;
            case 2:++d.context.lifetime;break;
            case 3:++d.context.smith;break;
            case 4:++k::minigame_hands45::modeEpoch67;break;
            case 5:d.hammer=false;d.step(11,.976f);d.hammer=true;break;
            case 6:d.step(10,.976f);break;
            case 7:d.frame.tick+=81;d.frame.timeNs+=81000000;break;
            case 8:d.step(11,.976f,.8f);break;
        }
        for(int i=1;i<=4;++i)ck(!d.step(11,.976f+.012f*i).requestRT,
            "owner, tool, phase, timing or manual boundary cannot reuse the previous lowering");
    }
    Driver manual;manual.heat();manual.lower();
    const auto o=manual.step(11,1.02f,.8f);
    ck(!o.requestRT&&!o.strokeStarted&&manual.pad.rt==.8f,"real manual trigger remains unchanged");
    Driver duplicate;duplicate.heat();duplicate.lower();
    auto copied=duplicate.frame;copied.sequence-=1;copied.tick-=duplicate.dt;copied.timeNs-=duplicate.dt*1000000;
    copied.hand[0].position[1]=1.02f;
    Pad p{};duplicate.tools();duplicate.state.apply(duplicate.context,copied,copied.tick,p);
    ck(!p.rt&&!duplicate.state.lastSmith.strokeStarted,"duplicate tracking sample cannot replay a lift");
}
#ifndef KCD2VR_SMITH_REHEAT_FIXTURE_ONLY_D83R1
int main(){movingReturn();noPassiveOrForeignPress();boundaries();
    std::printf("D82 actual smith station adapter: %u checks PASS; repeat reheat and boundary cases\n",checks);
}

#endif
