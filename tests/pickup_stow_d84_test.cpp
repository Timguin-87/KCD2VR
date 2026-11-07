// Executes the extracted, unmodified production StagedPropTick596 with native
// leaves from the existing adapter fixture and the real haptic event queue.
#define main RetainedPropFixtureD84
#include "prop_adapter5916c1_test.cpp"
#undef main
#include <limits>

namespace hp=kcd2vr::haptics101;
namespace cap=kcd2vr::diagnostics::capture168;
static unsigned checkedD84=0,failedD84=0;
static void checkD84(bool ok,const char* label){
    ++checkedD84;if(!ok){++failedD84;std::printf("FAIL %s\n",label);}
}
static void seedD84(int side,bool physics=true,bool record=false){
    setup();prop5916::enabled=physics;g_prop596.physics=physics;
    kcd2vr::combat_rules178r10::f5Rung=0;
    kcd2vr::workstation178r20c1::experimentArmed36=false;
    g_prop596.side=side;g_prop596.instantD72=true;g_prop596.instantTokenD72=101+side;
    g_prop596.epoch=g_prop596.bodyEpoch;g_prop596.hold.begin(g_prop596.bodyEpoch,now);
    kcd2vr::mutual_admission_t25::pickupClaimsD72[side]=g_prop596.instantTokenD72;
    hp::enabled=true;hp::allowed=true;hp::epoch.fetch_add(1);hp::testPending=false;
    for(auto& q:hp::queue)(void)q.publish({});
    cap::gate.select(cap::Domain::Interaction,record,now);
}
static hp::Pulse pulseD84(int side){hp::Pulse p{};(void)hp::queue[side].read(p);return p;}
static bool receiptD84(){
    for(const auto& line:notes)if(line.find("D84 F5_STOW_CONFIRM")!=std::string::npos)return true;
    return false;
}
static void enterD84(int side,double displayed){
    frame.allowed[side].p={displayed,0,0};
    frame.propRequested5916c1[side].p={.34,0,0};step();
    frame.propRequested5916c1[side].p={.05,0,0};
}
static void finishD84(){for(unsigned n=0;n<17;++n)step();}

int main(){
    // Same requested gesture in multiple corrected presentations, both hands,
    // both body-drive modes, capture on/off. No displayed wrist grants stow.
    for(int side=0;side<2;++side)for(bool physics:{false,true})for(bool capture:{false,true})
    for(double visible:{0.,.25,.60}){
        seedD84(side,physics,capture);enterD84(side,visible);finishD84();
        const auto p=pulseD84(side),other=pulseD84(1-side);
        checkD84(!g_prop596.id&&stowCalls==1,"one qualified native stow");
        checkD84(p.key==12&&p.amplitude==.65f&&p.durationMs==65&&hp::eligible(p,now),"successful stow queues acquired-hand cue");
        checkD84(other.key==0,"opposite hand is not buzzed");
        checkD84(receiptD84()==capture,"F5 alone owns stow receipt");
        step();checkD84(pulseD84(side).sequence==p.sequence&&stowCalls==1,"retired occurrence cannot replay cue or transfer");
    }
    for(int side=0;side<2;++side){
        seedD84(side);stowResultD84=false;enterD84(side,.5);finishD84();
        checkD84(!g_prop596.id&&stowCalls==1&&pulseD84(side).key==0,"failed native stow has no success cue");
        seedD84(side);enterD84(side,.5);frame.mapGripM39[side]=0;step();
        checkD84(!g_prop596.id&&stowCalls==0&&pulseD84(side).key==0,"loose release does not confirm inventory");
        seedD84(side);enterD84(side,.5);g_ctx_menu=true;step();
        checkD84(!g_prop596.id&&stowCalls==0&&pulseD84(side).key==0,"cancelled owner has no success cue");
        seedD84(side);enterD84(side,.5);identity=false;step();
        checkD84(!g_prop596.id&&stowCalls==0&&pulseD84(side).key==0,"retired native identity has no success cue");
        seedD84(side);enterD84(side,.5);hp::enabled=false;finishD84();
        checkD84(stowCalls==1&&pulseD84(side).key==0,"haptics preference does not gate native stow");
        seedD84(side);enterD84(side,.5);hp::allowed=false;finishD84();
        checkD84(stowCalls==1&&pulseD84(side).key==0,"XR haptic admission remains authoritative");
        seedD84(side);frame.allowed[side].p={.4,0,0};frame.propRequested5916c1[side].p={.05,0,0};step();
        frame.allowed[side].p={0,0,0};finishD84();
        checkD84(g_prop596.id&&stowCalls==0&&pulseD84(side).key==0,"visible-only chest excursion cannot manufacture tracked gesture");
        seedD84(side);enterD84(side,.5);frame.propRequestedValid5916c1[side]=false;step();
        checkD84(!g_prop596.id&&stowCalls==0&&pulseD84(side).key==0,"missing requested physics pose cannot grant stow");
        seedD84(side);enterD84(side,.5);frame.propRequested5916c1[side].p.x=std::numeric_limits<double>::quiet_NaN();step();
        checkD84(!g_prop596.id&&stowCalls==0&&pulseD84(side).key==0,"nonfinite requested gesture retires without cue");
    }
    // An old animated pickup acknowledgement is not an inventory receipt.
    setup();hp::enabled=hp::allowed=true;for(auto& q:hp::queue)(void)q.publish({});
    frame.allowed[0].p=frame.propRequested5916c1[0].p={.34,0,0};step();
    frame.allowed[0].p=frame.propRequested5916c1[0].p={0,0,0};finishD84();
    checkD84(stowCalls==1&&pulseD84(0).key==0,"animated admission is not falsely called inventory success");
    std::printf("D84 actual stow consumer: %u/%u checks; native leaves substituted, headset NOT_RUN\n",checkedD84-failedD84,checkedD84);
    return failedD84?1:0;
}
