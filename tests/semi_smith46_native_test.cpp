// The complete production owner translation unit, with only native leaves and
// memory/tracking I/O replaced. This does not execute the supplied game DLL.
#include "../src/tracking_sample59c6.h"
#define main OriginalGraspMainSmith46
#include "native_grasp_owner13_test.cpp"
#undef main
namespace kcd2vr::action_time_owner59c11 {namespace {
static std::array<unsigned char,0x700> smithInstance46{},smithParent46{},smithModel46{};
static std::array<std::uintptr_t,1> smithQueue46{};
static unsigned smithChecks46=0,smithQueueCalls46=0,smithHelperCalls46=0,smithNativeHits46=0,smithFullCharge46=0;
static bool smithNested46=false;
static smith46::ClockControl smithInput46{};
static void sc46(bool ok){++smithChecks46;if(!ok){std::fprintf(stderr,"smith native check %u failed\n",smithChecks46);std::abort();}}
static void NativeSmithQueue46(void* ctrl,void* child,float time){
    ++smithQueueCalls46;put(child,kActQueueTime,time);put(child,kActStatus,int(1));put(child,kActActiveTime,0.f);
    put(child,kActContext,Rd<void*>(ctrl,kCtrlContext,nullptr));put(child,kActRootScope,static_cast<void*>(nullptr));
    smithQueue46[0]=reinterpret_cast<std::uintptr_t>(child);put(ctrl,0x48,reinterpret_cast<std::uintptr_t>(smithQueue46.data()));put(ctrl,0x50,reinterpret_cast<std::uintptr_t>(smithQueue46.data()+1));
}
static void NativeSmithBuilder46(void* parent,void* smart){
    ++smithHelperCalls46;
    if(smithNested46){smithNested46=false;SemiSmithBuilderDetour46(nullptr,smart);return;}
    void* child=Rd<void*>(smart,0,nullptr);QueueDetour(controller.data(),child,0);
    // This ordering is the retained instruction witness399BF0 ->399BF3.
    if(parent)put(parent,0x88,reinterpret_cast<std::uintptr_t>(child));
}
static void NativeSmithUpdate46(void* ctrl,float dt){
    ++updateOriginalCalls;
    const float heat=Rd<float>(smithModel46.data(),0x400,1.f);put(smithModel46.data(),0x400,heat-dt);
    if(Rd<void*>(scope.data(),kScopeAction,nullptr)==action.data()&&Rd<void*>(action.data(),kActRootScope,nullptr)==scope.data()){
        const float bias=Rd<float>(action.data(),kActSpeedBias,1.f),scale=Rd<float>(ctrl,0x9C,1.f);
        const float before=Rd<float>(action.data(),kActActiveTime,0.f),after=before+dt*scale*bias;
        put(action.data(),kActActiveTime,after);put(scope.data(),kScopeSpeedBias,bias);
        const auto code=Rd<std::uint8_t>(smithParent46.data(),0x50,0);
        // Native event delivery itself is an explicitly substituted leaf. Only
        // the production ownership/clock decides whether its time can cross.
        if(code==10&&before<.08f&&after>=.08f)++smithFullCharge46;
        if(code==11&&before<.04f&&after>=.04f)++smithNativeHits46;
    }
}
static void SmithSample46(std::uint64_t tick,double raise,double finish=0){
    hostTickCount64=tick;smithInput46.tick=tick;++smithInput46.sequence;smithInput46.raiseWork=raise;smithInput46.finishWork=finish;
    smith46::clockControl.publish(smithInput46);
    testWorkstationFrame46.tick=tick;testWorkstationFrame46.timeNs=tick*1000000;testWorkstationFrame46.sequence=smithInput46.sequence;
    kcd2vr::minigame_hands45::publish(kcd2vr::minigame_hands45::Smithing,tick);
}
static void PrepareSmith46(int code=10){
    smith46::clockReady=false;smith46::clockInterested=false;smith46::clockControl.publish({});smith46::clockStatus.publish({});
    g_semi_smith46={};g_semi_smith_pending46=false;g_semi_smith_executing46={};g_semi_smith_abandoned46={};g_semi_smith_stop46=false;
    g_semi_pending46=false;g_semi_alchemy46={};semi46::hooksReady=false;reset();ResetOwner();g_mode=Off;
    smithInstance46.fill(0);smithParent46.fill(0);smithModel46.fill(0);smithQueueCalls46=smithHelperCalls46=smithNativeHits46=smithFullCharge46=0;smithNested46=false;
    const auto b=g_base.load(),a=reinterpret_cast<std::uintptr_t>(smithInstance46.data()),p=reinterpret_cast<std::uintptr_t>(smithParent46.data()),m=reinterpret_cast<std::uintptr_t>(smithModel46.data());
    for(const auto& pin:smith46::pins::all)std::memcpy(image.data()+pin.rva,pin.bytes,pin.size);
    for(const auto& slot:smith46::pins::slots)put(image.data(),slot.rva,b+slot.target);
    put(smithInstance46.data(),0,b+smithNative46::kInstanceVft);put(smithInstance46.data(),0x2C,std::uint8_t(1));put(smithInstance46.data(),0x18,reinterpret_cast<std::uintptr_t>(player.data()));
    put(smithInstance46.data(),0x70,b+smithNative46::kListenerVft);put(smithInstance46.data(),0x98,m);
    put(smithModel46.data(),0,b+smithNative46::kModelVft);put(smithModel46.data(),0x88,std::uint8_t(code==10?14:16));put(smithModel46.data(),0x400,1.f);
    put(smithParent46.data(),0,b+smithNative46::kParentVft);put(smithParent46.data(),0x10,b+smithNative46::kParentActionVft);put(smithParent46.data(),0x28,std::uint8_t(1));
    put(smithParent46.data(),0x50,std::uint8_t(code));put(smithParent46.data(),0x78,reinterpret_cast<std::uintptr_t>(player.data()));put(smithParent46.data(),0x80,a+0x70);
    put(action.data(),0,b+smithNative46::kChildVft);put(action.data(),kActStatus,int(0));put(action.data(),kActActiveTime,0.f);put(action.data(),kActSpeedBias,1.f);
    put(action.data(),kActToken,55u);put(action.data(),kActContext,static_cast<void*>(nullptr));put(action.data(),kActRootScope,static_cast<void*>(nullptr));
    put(scope.data(),kScopeAction,static_cast<void*>(nullptr));put(controller.data(),0x9C,1.f);
    auto& w=kcd2vr::activity_input163::witness175;w.Clear();w.ready=true;w.Begin(kcd2vr::activity_input163::Kind175::Smithing,a);w.transitionDepth175=0;
    kcd2vr::activity_input163::trackedHands59c7f[1]=true;kcd2vr::minigame_hands45::mode=kcd2vr::minigame_hands45::SemiPassenger;
    kcd2vr::combat_rules178r10::workstationEnabled=true;kcd2vr::workstation178r20c1::smithAutoHold36=true;kcd2vr::combat_rules178r10::f5Rung=0;kcd2vr::workstation178r20c1::setExperiment36(false);
    smithInput46={};smithInput46.enabled=true;smithInput46.instance=a;smithInput46.lifetime=w.lifetime175.load();smithInput46.request=1;smithInput46.sequence=1;
    smithInput46.poseEpoch=8;smithInput46.sourceEpoch=9;smithInput46.modeEpoch67=kcd2vr::minigame_hands45::modeEpoch67.load();smithInput46.requestedPhase=code==10?14:16;
    testWorkstationFrame46=kcd2vr::workstation_sample46::Frame{};testWorkstationFrame46.allowed=testWorkstationFrame46.headValid=true;testWorkstationFrame46.head[1]=1.7f;
    testWorkstationFrame46.poseEpoch=8;testWorkstationFrame46.sourceEpoch=9;testWorkstationFrame46.hand[0].valid=true;testWorkstationFrame46.hand[1].valid=true;
    smith46::clockReady=true;smith46::clockInterested=true;SmithSample46(1000,0);
    g_queue_original=NativeSmithQueue46;g_update_original=NativeSmithUpdate46;g_semi_smith_builder_original46=NativeSmithBuilder46;
}
static void SmithBuilder46(){
    std::uintptr_t child=reinterpret_cast<std::uintptr_t>(action.data());SemiSmithBuilderDetour46(smithParent46.data(),&child);
    // Retail856FE6 starts the parent first; current+C8 is stored only afterward.
    put(smithInstance46.data(),0xC8,reinterpret_cast<std::uintptr_t>(smithParent46.data()));
}
static void SmithInstallQueued46(){
    put(controller.data(),0x50,reinterpret_cast<std::uintptr_t>(smithQueue46.data()));
    put(scope.data(),kScopeAction,static_cast<void*>(action.data()));put(action.data(),kActRootScope,static_cast<void*>(scope.data()));put(action.data(),kActStatus,int(2));
}
int SmithNativeScenario46(){
    PrepareSmith46();sc46(SemiSmithPins46());SmithBuilder46();
    sc46(smithQueueCalls46==1&&smithHelperCalls46==1&&g_semi_smith_pending46&&Rd<float>(action.data(),kActSpeedBias,-1)==0.f);
    sc46(!g_semi_smith_executing46.valid&&g_semi_smith_inflight46==0&&Rd<int>(action.data(),kActStatus,-1)==1);
    smith46::ClockStatus status{};sc46(smith46::clockStatus.read(status)&&status.owned&&status.phase==14&&status.request==1);
    // Native queue delay does not spend hand credit; the counter remains zero.
    SmithSample46(1010,.01);UpdateDetour(controller.data(),.01f);sc46(Rd<float>(action.data(),kActActiveTime,-1)==0.f);
    SmithInstallQueued46();SmithSample46(1020,.01);UpdateDetour(controller.data(),.01f);sc46(std::fabs(Rd<float>(action.data(),kActActiveTime,-1)-.01f)<1e-6f);
    // Stopping the hand pauses exact work while the mocked native heat ages.
    const float heat=Rd<float>(smithModel46.data(),0x400,-1);SmithSample46(1030,.01);UpdateDetour(controller.data(),.01f);
    sc46(Rd<float>(action.data(),kActSpeedBias,-1)<.000001f&&std::fabs(Rd<float>(action.data(),kActActiveTime,-1)-.01f)<1e-6f);
    sc46(Rd<float>(smithModel46.data(),0x400,-1)<heat&&smithNativeHits46==0&&smithFullCharge46==0);
    SmithSample46(1040,.017);SmithSample46(1050,.03);UpdateDetour(controller.data(),.02f);sc46(std::fabs(Rd<float>(action.data(),kActActiveTime,-1)-.03f)<1e-6f);
    UpdateDetour(controller.data(),.02f);sc46(std::fabs(Rd<float>(action.data(),kActActiveTime,-1)-.03f)<1e-6f);
    // Time-scale is read, never overwritten. Same authored hand work is spent.
    put(controller.data(),0x9C,.5f);SmithSample46(1060,.04);UpdateDetour(controller.data(),.01f);
    sc46(std::fabs(Rd<float>(action.data(),kActActiveTime,-1)-.04f)<1e-6f&&Rd<float>(controller.data(),0x9C,-1)==.5f);
    put(controller.data(),0x9C,1.f);SmithSample46(1070,.09);UpdateDetour(controller.data(),.01f);sc46(smithFullCharge46==1&&smithNativeHits46==0);
    // Native mode change is enforced immediately, before any worker republish.
    kcd2vr::minigame_hands45::mode=kcd2vr::minigame_hands45::Native;UpdateDetour(controller.data(),.01f);sc46(!g_semi_smith_pending46&&Rd<float>(action.data(),kActSpeedBias,-1)==1.f);
    PrepareSmith46();SmithBuilder46();SmithBuilder46();sc46(!g_semi_smith_pending46&&g_semi_smith_abandoned46.request==1&&Rd<float>(action.data(),kActSpeedBias,-1)==1.f);
    // Child11 has its own work budget; its native hit is crossed only by work.
    PrepareSmith46(11);SmithBuilder46();SmithInstallQueued46();SmithSample46(1010,0,0);UpdateDetour(controller.data(),.01f);sc46(smithNativeHits46==0);
    SmithSample46(1020,0,.025);UpdateDetour(controller.data(),.01f);sc46(smithNativeHits46==0);SmithSample46(1030,0,.05);UpdateDetour(controller.data(),.01f);sc46(smithNativeHits46==1);
    SmithSample46(1040,0,.05);UpdateDetour(controller.data(),.01f);sc46(smithNativeHits46==1&&Rd<float>(action.data(),kActSpeedBias,-1)<.000001f);
    // Slow native time-scale never allows a backlog to continue after a stop.
    PrepareSmith46();SmithBuilder46();SmithInstallQueued46();put(controller.data(),0x9C,.01f);
    SmithSample46(1010,.2);UpdateDetour(controller.data(),.01f);const float little=Rd<float>(action.data(),kActActiveTime,-1);
    SmithSample46(1020,.2);UpdateDetour(controller.data(),.01f);sc46(little>0&&Rd<float>(action.data(),kActActiveTime,-1)==little&&Rd<float>(action.data(),kActSpeedBias,-1)==0);
    // A new native code/token at the same allocation is not a duplicate queue.
    PrepareSmith46();SmithBuilder46();put(smithParent46.data(),0x50,std::uint8_t(11));put(smithParent46.data(),0x88,std::uintptr_t(0));
    put(smithInstance46.data(),0xC8,std::uintptr_t(0));put(smithModel46.data(),0x88,std::uint8_t(16));put(action.data(),kActToken,56u);put(action.data(),kActSpeedBias,1.f);
    smithInput46.requestedPhase=16;SmithSample46(1010,0,.01);SmithBuilder46();sc46(g_semi_smith_pending46&&g_semi_smith46.owner.code==11&&g_semi_smith46.token==56&&Rd<float>(action.data(),kActSpeedBias,-1)==0);
    // Release waiting for the replacement parent must pause code10, then allow
    // the same gesture transaction to seed code11 (no whole-request retirement).
    PrepareSmith46();SmithBuilder46();SmithInstallQueued46();smithInput46.requestedPhase=16;SmithSample46(1010,.1,.01);UpdateDetour(controller.data(),.01f);sc46(Rd<float>(action.data(),kActActiveTime,-1)==0.f);
    put(smithModel46.data(),0x88,std::uint8_t(16));SmithSample46(1020,.1,.01);UpdateDetour(controller.data(),.01f);sc46(!g_semi_smith_pending46&&g_semi_smith_abandoned46.request==0);
    put(scope.data(),kScopeAction,static_cast<void*>(nullptr));put(action.data(),kActRootScope,static_cast<void*>(nullptr));put(action.data(),kActToken,56u);put(action.data(),kActActiveTime,0.f);
    put(smithParent46.data(),0x50,std::uint8_t(11));put(smithParent46.data(),0x88,std::uintptr_t(0));put(smithInstance46.data(),0xC8,std::uintptr_t(0));SmithBuilder46();sc46(g_semi_smith_pending46&&g_semi_smith46.owner.code==11&&g_semi_smith46.token==56);
    // Manual/mode/tracking revocation restores queued as well as installed work.
    for(unsigned fault=0;fault<6;++fault){PrepareSmith46();SmithBuilder46();
        if(fault==0)smithInput46.enabled=false;if(fault==1)testWorkstationFrame46.hand[0].valid=false;
        if(fault==2)++testWorkstationFrame46.poseEpoch;if(fault==3)kcd2vr::activity_input163::trackedHands59c7f[1]=false;
        if(fault==4)kcd2vr::activity_input163::witness175.transitionDepth175=1;
        if(fault==5)kcd2vr::workstation178r20c1::smithAutoHold36=false;
        smith46::clockControl.publish(smithInput46);UpdateDetour(controller.data(),.01f);sc46(!g_semi_smith_pending46&&Rd<float>(action.data(),kActSpeedBias,-1)==1.f);}
    PrepareSmith46();SmithBuilder46();g_owner_thread13=99;sc46(!StopSemiSmithClock46()&&g_semi_smith_pending46);g_owner_thread13=1;sc46(StopSemiSmithClock46()&&!g_semi_smith_pending46);
    // Native writer wins; this adapter never overwrites a foreign playback rate.
    PrepareSmith46();SmithBuilder46();SmithInstallQueued46();put(action.data(),kActSpeedBias,.6f);UpdateDetour(controller.data(),.01f);sc46(!g_semi_smith_pending46&&Rd<float>(action.data(),kActSpeedBias,-1)==.6f);
    // Unreadable membership retains cleanup and release waits for a native
    // owner callback. A proven empty queue then retires without old child reads.
    PrepareSmith46();SmithBuilder46();put(controller.data(),0x50,reinterpret_cast<std::uintptr_t>(smithQueue46.data())+9);smithInput46.enabled=false;smith46::clockControl.publish(smithInput46);UpdateDetour(controller.data(),.01f);sc46(g_semi_smith_pending46);
    sc46(smith46::clockStatus.read(status)&&!status.owned);
    put(controller.data(),0x50,reinterpret_cast<std::uintptr_t>(smithQueue46.data()));forbidRetiredRead19c4=true;UpdateDetour(controller.data(),.01f);sc46(!g_semi_smith_pending46&&retiredReads19c4==0);forbidRetiredRead19c4=false;
    PrepareSmith46();SmithBuilder46();smithInput46.enabled=false;smith46::clockControl.publish(smithInput46);refusedWrite19=action.data()+kActSpeedBias;UpdateDetour(controller.data(),.01f);sc46(g_semi_smith_pending46);refusedWrite19=nullptr;UpdateDetour(controller.data(),.01f);sc46(!g_semi_smith_pending46);
    // The class is generic. A direct queue without the exact executing smith
    // parent is never enough; nor is a reused child/old owner identity.
    PrepareSmith46();QueueDetour(controller.data(),action.data(),0);sc46(!g_semi_smith_pending46&&Rd<float>(action.data(),kActSpeedBias,-1)==1.f);
    for(unsigned fault=0;fault<12;++fault){PrepareSmith46();
        if(fault==0)put(smithParent46.data(),0x80,std::uintptr_t(1));if(fault==1)put(smithParent46.data(),0x78,std::uintptr_t(1));
        if(fault==2)put(smithParent46.data(),0x50,std::uint8_t(12));if(fault==3)put(smithParent46.data(),0x28,std::uint8_t(0));
        if(fault==4)put(smithInstance46.data(),0x2C,std::uint8_t(0));if(fault==5)put(smithModel46.data(),0x88,std::uint8_t(11));
        if(fault==6)smithInput46.lifetime++;if(fault==7)smithInput46.tick=919;if(fault==8)smithInput46.instance++;
        if(fault==9)put(action.data(),0,g_base.load()+0x3A47948);if(fault==10)smithInput46.requestedPhase=16;if(fault==11)testWorkstationFrame46.allowed=false;
        smith46::clockControl.publish(smithInput46);SmithBuilder46();sc46(!g_semi_smith_pending46&&smithQueueCalls46==1&&Rd<float>(action.data(),kActSpeedBias,-1)==1.f);}
    PrepareSmith46();smithNested46=true;SmithBuilder46();sc46(smithHelperCalls46==2&&!g_semi_smith_executing46.valid&&g_semi_smith_inflight46==0&&!g_semi_smith_pending46);
    // No late acquisition of a child whose first native events may have run.
    PrepareSmith46();NativeSmithQueue46(controller.data(),action.data(),0);put(smithParent46.data(),0x88,reinterpret_cast<std::uintptr_t>(action.data()));put(smithInstance46.data(),0xC8,reinterpret_cast<std::uintptr_t>(smithParent46.data()));SmithInstallQueued46();put(action.data(),kActActiveTime,.2f);UpdateDetour(controller.data(),.01f);sc46(!g_semi_smith_pending46);
    smith46::clockReady=false;smith46::clockInterested=false;kcd2vr::activity_input163::witness175.Clear();
    std::printf("PASS C46 exact smith native adapter: %u checks; full production TU/helper/Queue/Update/cleanup, mocked native leaves only\n",smithChecks46);return 0;
}
}}
#ifndef KCD2VR_SMITH_NATIVE_NO_MAIN_G1
int main(){return kcd2vr::action_time_owner59c11::SmithNativeScenario46();}
#endif
