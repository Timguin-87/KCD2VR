// Reuse native leaves/memory fixtures; compile the COMPLETE production owner TU.
// The existing scenario is available but this executable tests only C46 seams.
#include "../src/tracking_sample59c6.h"
#define main OriginalGraspMain46
#include "native_grasp_owner13_test.cpp"
#undef main
#include "../src/vrik/workstation_sample46.h"
namespace kcd2vr::action_time_owner59c11 { namespace {
static std::array<unsigned char,0x700> alchemy46{},parent46{};
static std::array<std::uintptr_t,2> director46{};
static std::array<std::uintptr_t,1> queue46{};
static unsigned checks46=0;
static void ck46(bool ok){++checks46;if(!ok)std::fprintf(stderr,"failed native check %u\n",checks46);assert(ok);}
static void SeedPins46(){
    for(const auto& p:semi46::pins::all)std::memcpy(image.data()+p.rva,p.bytes,p.size);
    auto putPin=[](unsigned at,std::initializer_list<unsigned char> bytes){std::copy(bytes.begin(),bytes.end(),image.begin()+at);};
    putPin(0x6C6D33,{0xE8,0x70,0xC0,0xFB,0xFF,0x48,0x8D,0x05,0x09,0x0C,0x38,0x03,0x48,0x89,0x9F,0x68,0x02,0x00,0x00});
    putPin(0x682DD3,{0xB8,0x00,0x00,0x80,0x3F,0x89,0x43,0x5C,0x89,0x43,0x60});
    putPin(0x7012EF,{0x48,0x8D,0x7B,0x78});
    putPin(0x4B3A76,{0xF3,0x0F,0x10,0x7D,0x5C,0xF3,0x0F,0x10,0x43,0x58,0x0F,0x2E,0xC7});
    putPin(0x4B3F12,{0x0F,0x28,0xCF,0xF3,0x0F,0x59,0x4B,0x5C,0x48,0x8B,0xCB,0xFF,0x50,0x30});
    put(image.data(),0x3F60260+0x110,g_base.load()+0x17BAD08);
    put(image.data(),0x3A5BD40+0xF8,g_base.load()+0xA95AEC);
    put(image.data(),0x3A7D9D0+8,g_base.load()+0x1AAF020);put(image.data(),0x3A7D9D0+0x18,g_base.load()+0x1AAFB60);
    put(image.data(),kSkelVft+0x170,g_base.load()+kSkelDispatch);put(image.data(),kCtrlVft+0xE0,g_base.load()+0x1A73130);
}
static void NativeQueue46(void* ctrl,void* a,float time){
    // Verified Queue establishes context/status1/time0 and enqueues; it does not
    // advance native time or emit an animation event.
    put(a,kActQueueTime,time);put(a,kActStatus,int(1));put(a,kActActiveTime,0.f);
    put(a,kActContext,Rd<void*>(ctrl,kCtrlContext,nullptr));put(a,kActRootScope,static_cast<void*>(nullptr));
    queue46[0]=reinterpret_cast<std::uintptr_t>(a);
    put(ctrl,0x48,reinterpret_cast<std::uintptr_t>(queue46.data()));put(ctrl,0x50,reinterpret_cast<std::uintptr_t>(queue46.data()+1));
}
static float lastBellowsDt46=-1;
static int NativeBellowsTick46(void* self,float dt){
    lastBellowsDt46=dt;
    if(Rd<int>(self,0x78,-1)==1)put(self,0x90,Rd<float>(self,0x90,0.f)+dt);
    return 0;
}
static bool nestedBookDisable46=false;
static void NativeBookEnable46(void* self,bool enabled){if(enabled&&nestedBookDisable46){nestedBookDisable46=false;SemiBookEnableDetour46(self,false);}}
static semi46::Control input46;
static float originalDt46=-1.f;
static void NativeUpdate46(void* ctrl,float dt){
    // Native consumer double for exact 4B3E2F GetTimeScale multiplication.
    // The production detour must preserve the caller's original dt argument.
    originalDt46=dt;UpdateActionLeaf(ctrl,dt*Rd<float>(ctrl,kCtrlTimeScale,0.f));
}
static void Prepare46(unsigned kind=semi46::Bellows,int phase=2){
    semi46::hooksReady=false;semi46::control.publish({});semi46::work.publish({});g_semi_pending46=false;g_semi_alchemy46={};
    reset();(void)RestoreSpeed13();ResetOwner();SeedPins46();g_semi_retired46={};g_semi_resource46=SemiResourceState46{};g_semi_stop46=false;
    kcd2vr::minigame_hands45::mode=kcd2vr::minigame_hands45::SemiPassenger;
    kcd2vr::combat_rules178r10::f5Rung=0;kcd2vr::workstation178r20c1::setExperiment36(false);
    alchemy46.fill(0);parent46.fill(0);director46={};put(controller.data(),kCtrlTimeScale,1.f);
    const auto b=g_base.load(),a=reinterpret_cast<std::uintptr_t>(alchemy46.data()),p=reinterpret_cast<std::uintptr_t>(parent46.data());
    put(alchemy46.data(),0,b+0x3F60B08);put(alchemy46.data(),0x18,reinterpret_cast<std::uintptr_t>(player.data()));put(alchemy46.data(),0x2C,std::uint8_t(1));
    put(player.data(),0x38,std::uintptr_t(123));
    put(alchemy46.data(),0x78,b+0x3BFA5B0);director46[1]=p;
    put(alchemy46.data(),0xB0,reinterpret_cast<std::uintptr_t>(director46.data()));put(alchemy46.data(),0xB8,reinterpret_cast<std::uintptr_t>(director46.data()+2));
    const auto vft=kind==semi46::Bellows?0x3F60260:kind==semi46::Mortar?0x3F60770:kind==semi46::Resource?0x3A5C670:0x3A5BD40;
    const auto offset=kind==semi46::Bellows?0x88:kind==semi46::Mortar?0x90:kind==semi46::Resource?0x78:0x98;
    put(parent46.data(),0,b+vft);put(parent46.data(),0x58,a);put(parent46.data(),0x40,a+0x78);put(parent46.data(),0x3C,int(1));put(parent46.data(),0x28,std::uint8_t(1));
    if(kind==semi46::Bellows)put(parent46.data(),0x78,phase);if(kind==semi46::Mortar)put(parent46.data(),0x70,phase);
    put(parent46.data(),offset,reinterpret_cast<std::uintptr_t>(action.data()));
    put(action.data(),0,b+0x3A47948);put(action.data(),0x268,a);put(action.data(),kActContext,static_cast<void*>(actionContext.data()));
    put(scope.data(),kScopeAction,static_cast<void*>(nullptr));put(action.data(),kActRootScope,static_cast<void*>(nullptr));
    auto& aw=kcd2vr::activity_input163::witness175;aw.Clear();aw.ready=true;aw.Begin(kcd2vr::activity_input163::Kind175::Alchemy,a);aw.transitionDepth175=0;
    semi46::hooksReady=true;input46={};input46.enabled=true;input46.instance=a;input46.lifetime=aw.lifetime175.load();
    testWorkstationFrame46=workstation_sample46::Frame{};testWorkstationFrame46.allowed=testWorkstationFrame46.headValid=true;
    testWorkstationFrame46.hand[0].valid=testWorkstationFrame46.hand[1].valid=true;
    testWorkstationFrame46.tick=1000;testWorkstationFrame46.timeNs=1000000000;testWorkstationFrame46.sequence=1;testWorkstationFrame46.poseEpoch=8;testWorkstationFrame46.sourceEpoch=9;
    kcd2vr::combat_rules178r10::workstationEnabled=true;kcd2vr::activity_input163::trackedHands59c7f[unsigned(kcd2vr::activity_input163::Kind175::Alchemy)]=true;
    minigame_hands45::publish(minigame_hands45::Alchemy,1000);
    input46.tracked[0]=input46.tracked[1]=true;input46.tick=1000;input46.sequence=1;input46.poseEpoch=8;input46.sourceEpoch=9;semi46::control.publish(input46);
    g_queue_original=NativeQueue46;g_update_original=NativeUpdate46;g_semi_bellows_original46=NativeBellowsTick46;g_semi_book_original46=NativeBookEnable46;
}
static void InstallQueued46(){
    put(controller.data(),0x50,reinterpret_cast<std::uintptr_t>(queue46.data()));
    put(scope.data(),kScopeAction,static_cast<void*>(action.data()));put(action.data(),kActRootScope,static_cast<void*>(scope.data()));put(action.data(),kActStatus,int(2));
    if(Rd<std::uintptr_t>(parent46.data(),0,0)==g_base.load()+0x3F60770)put(parent46.data(),0x70,int(3));
}
static void Sample46(std::uint64_t tick,double total){
    hostTickCount64=tick;input46.tick=tick;++input46.sequence;input46.clockStop=total<=input46.totalSeconds;input46.totalSeconds=total;
    testWorkstationFrame46.tick=tick;testWorkstationFrame46.timeNs=tick*1000000;testWorkstationFrame46.sequence=input46.sequence;
    minigame_hands45::publish(minigame_hands45::Alchemy,tick);
    input46.parent=reinterpret_cast<std::uintptr_t>(parent46.data());input46.child=reinterpret_cast<std::uintptr_t>(action.data());input46.token=55;
    semi46::control.publish(input46);
}

static std::array<unsigned char,0x500> humanParams46{};
static std::array<unsigned char,16> eventList46{};
static std::array<unsigned char,8+8*0x50> eventEntries46{};
static SemiResourceInfo46 eventInfo46{};static unsigned nativeAttach46=0,nativeUse46=0;
static const char emptyFilter46[]="";static const char carafePath46[]="native_carafe_clip";static bool wrongCharacter46=false;
static void NativeResourceEvent46(void*,void*,const void* ev){
    const auto crc=Rd<std::uint32_t>(ev,0x1C,0);
    if(crc==0xDEC97C81u)++nativeAttach46;
    if(crc==0x94B1CC4Bu)++nativeUse46;
}
static void NativeResourceScan46(void*,bool inclusive,float from,float to,const void* source,void*,int segment){
    const auto& info=*static_cast<const SemiResourceInfo46*>(source);
    const auto entries=Rd<std::uintptr_t>(reinterpret_cast<void*>(info.list),8,0);
    const int count=Rd<int>(reinterpret_cast<void*>(entries),-4,0);
    for(int i=0;i<count;++i){const auto record=entries+std::size_t(i)*0x50;const float time=Rd<float>(reinterpret_cast<void*>(record),0,-1.f);
        if((time>from||(inclusive&&time==from))&&time<=to&&Rd<int>(reinterpret_cast<void*>(record),0xC,-1)==segment){
            std::array<unsigned char,0x58> event{};put(event.data(),0,time);put(event.data(),8,info.last-info.first-1);
            put(event.data(),0x10,info.path);put(event.data(),0x18,info.animation);put(event.data(),0x1C,Rd<std::uint32_t>(reinterpret_cast<void*>(record),8,0));
            put(event.data(),0x50,record);SemiResourceEventDetour46(parent46.data(),wrongCharacter46?static_cast<void*>(parent46.data()):static_cast<void*>(character.data()),event.data());
        }
    }
}
static void ResourceFixture46(){
    Prepare46(semi46::Resource,0);NativeQueue46(controller.data(),action.data(),0);InstallQueued46();
    put(action.data(),kActActiveTime,.937f);put(testSkel,0,g_base.load()+kSkelVft);
    humanParams46.fill(0);put(player.data(),0xA50,static_cast<void*>(humanParams46.data()));put(humanParams46.data(),0x90,int(1));
    put(fragmentEntries.data()+8,0x20+0x10,std::uint32_t(0xC5990708u));
    put(parent46.data(),0x68,std::uint32_t(0xDEC97C81u));put(parent46.data(),0x6C,std::uint32_t(0xD3845300u));put(parent46.data(),0x70,std::uint32_t(0x94B1CC4Bu));
    put(alchemy46.data(),0x304,int(13));put(alchemy46.data(),0x428,std::uint8_t(1));
    eventEntries46.fill(0);eventList46.fill(0);put(eventList46.data(),0,g_base.load()+0x3A7D9D0);
    put(eventList46.data(),8,reinterpret_cast<std::uintptr_t>(eventEntries46.data()+8));put(eventEntries46.data(),4,int(4));
    const std::uint32_t names[]{0xDEC97C81u,0xE5D8547Bu,0x94B1CC4Bu,0xD3845300u};
    const float times[]{.151129f,.3945431f,.4208871f,.8392204f}; // actual alchemyHooch native table
    for(unsigned i=0;i<4;++i){auto* rec=eventEntries46.data()+8+i*0x50;put(rec,0,times[i]);put(rec,8,names[i]);put(rec,0xC,int(0));put(rec,0x28,reinterpret_cast<std::uintptr_t>(emptyFilter46));}
    eventInfo46={reinterpret_cast<std::uintptr_t>(eventList46.data()),reinterpret_cast<std::uintptr_t>(carafePath46),0,42,1,1.f};
    nativeAttach46=nativeUse46=0;wrongCharacter46=false;g_semi_scan_original46=NativeResourceScan46;g_semi_resource_original46=NativeResourceEvent46;
}
static void ResourceCases46(){
    ResourceFixture46();SemiResourceScanDetour46(testSkel,false,.14f,.16f,&eventInfo46,nullptr,0);
    ck46(nativeAttach46==1&&nativeUse46==0&&g_semi_pending46&&Rd<float>(action.data(),kActSpeedBias,-1)==0.f&&!g_semi_scan46.valid);
    Sample46(1010,0);UpdateDetour(controller.data(),.01f);ck46(Rd<float>(action.data(),kActSpeedBias,-1)==0.f);
    Sample46(1020,.01);UpdateDetour(controller.data(),.02f);ck46(std::fabs(Rd<float>(action.data(),kActSpeedBias,-1)-.5f)<1e-6f);
    SemiResourceScanDetour46(testSkel,false,.40f,.43f,&eventInfo46,nullptr,0);
    ck46(nativeUse46==1&&!g_semi_pending46&&g_semi_resource46.phase==2&&Rd<float>(action.data(),kActSpeedBias,-1)==1.f);
    UpdateDetour(controller.data(),.02f);ck46(!g_semi_pending46); // automatic return is never acquired again
    // In a stalled frame the native scan may already include Attach AND Use.
    // The actual interval/list prove that burst: leave every callback native.
    // C55 explicit Guided test must retain the REAL native event scan seam
    // even with the saved minigame setting Native. Attach still fires once;
    // only its witnessed working interval is paused/credited, then Use returns.
    ResourceFixture46();kcd2vr::combat_rules178r10::f5Rung=3;
    kcd2vr::workstation178r20c1::setExperiment36(true);minigame_hands45::mode=minigame_hands45::Native;
    input46.hand=1;semi46::control.publish(input46);
    SemiResourceScanDetour46(testSkel,false,.14f,.16f,&eventInfo46,nullptr,0);
    ck46(nativeAttach46==1&&nativeUse46==0&&g_semi_pending46&&Rd<float>(action.data(),kActSpeedBias,-1)==0.f);
    Sample46(1020,.01);UpdateDetour(controller.data(),.02f);ck46(std::fabs(Rd<float>(action.data(),kActSpeedBias,-1)-.5f)<1e-6f);
    SemiResourceScanDetour46(testSkel,false,.40f,.43f,&eventInfo46,nullptr,0);
    ck46(nativeUse46==1&&!g_semi_pending46&&Rd<float>(action.data(),kActSpeedBias,-1)==1.f);
    ResourceFixture46();SemiResourceScanDetour46(testSkel,false,.14f,.43f,&eventInfo46,nullptr,0);
    ck46(nativeAttach46==1&&nativeUse46==1&&!g_semi_pending46&&Rd<float>(action.data(),kActSpeedBias,-1)==1.f);
    for(unsigned fault=0;fault<8;++fault){ResourceFixture46();
        if(fault==0)put(fragmentEntries.data()+8,0x20+0x10,std::uint32_t(0x03E46E25u));
        if(fault==1)put(eventList46.data(),0,g_base.load()+0x3A7D9D8);
        if(fault==2)put(eventEntries46.data()+8+2*0x50,0,.1f); // work before Attach
        if(fault==3)put(eventEntries46.data()+8+3*0x50,8,std::uint32_t(0x94B1CC4Bu)); // ambiguous multiple Use
        if(fault==4){input46.manualUse=true;semi46::control.publish(input46);}
        if(fault==5)put(alchemy46.data(),0x304,int(18));
        if(fault==7)wrongCharacter46=true;
        if(fault==6)put(eventEntries46.data()+8+2*0x50,0x48,std::uintptr_t(1)); // custom marker not ordinary native dispatch
        SemiResourceScanDetour46(testSkel,false,.14f,.16f,&eventInfo46,nullptr,0);
        ck46(!g_semi_pending46&&nativeAttach46==1&&Rd<float>(action.data(),kActSpeedBias,-1)==1.f);
    }
    ResourceFixture46();NativeResourceScan46(testSkel,false,.14f,.16f,&eventInfo46,nullptr,0);
    ck46(!g_semi_pending46&&nativeAttach46==1); // unqualified direct callback has no scan authority
}

static void MortarQueueLedger46(){
    Prepare46(semi46::Mortar,2);QueueDetour(controller.data(),action.data(),0);
    semi46::WorkWitness queued{};ck46(semi46::work.read(queued)&&queued.kind==semi46::Mortar&&queued.phase==2&&queued.working&&!queued.ready&&g_semi_pending46);
    semi46::Producer gesture{};
    const auto sample=[&](std::uint64_t tick,std::uint64_t sequence,float x,const semi46::WorkWitness& w){
        hostTickCount64=tick;testWorkstationFrame46.tick=tick;testWorkstationFrame46.timeNs=tick*1000000;
        testWorkstationFrame46.sequence=sequence;testWorkstationFrame46.hand[0].position[0]=x;
        minigame_hands45::publish(minigame_hands45::Alchemy,tick);
        const auto c=gesture.step(testWorkstationFrame46,true,w.instance,w.lifetime,w);semi46::control.publish(c);return c;
    };
    sample(1000,1,0,queued);sample(1010,2,.0032f,queued);
    auto c=sample(1020,3,.0048f,queued);ck46(std::fabs(c.totalSeconds-.03)<1e-5&&c.hand==0);
    // Real production consumer first consumes the producer's queued-phase
    // credits, while native memory has already completed its phase3 handoff.
    InstallQueued46();UpdateDetour(controller.data(),.02f);
    ck46(std::fabs(Rd<float>(action.data(),kActActiveTime,-1)-.03f)<1e-5f&&std::fabs(g_semi_alchemy46.consumedSeconds-.03)<1e-5);
    semi46::WorkWitness live{};ck46(semi46::work.read(live)&&live.phase==3&&live.working&&semi46::same(queued,live));
    c=sample(1030,4,.0064f,live);
    ck46(std::fabs(c.totalSeconds-.04)<1e-5&&c.hand==0&&gesture.owner.phase==3&&gesture.reference[0][0]==0.f);
    UpdateDetour(controller.data(),.01f);ck46(std::fabs(Rd<float>(action.data(),kActActiveTime,-1)-.04f)<1e-5f);
    c=sample(1030,4,.0064f,live);UpdateDetour(controller.data(),.01f);
    ck46(std::fabs(c.totalSeconds-.04)<1e-5&&std::fabs(Rd<float>(action.data(),kActActiveTime,-1)-.04f)<1e-5f); // duplicate cannot replay
    sample(1040,5,.0064f,live);UpdateDetour(controller.data(),.01f);
    ck46(Rd<float>(action.data(),kActSpeedBias,-1)==0.f&&std::fabs(Rd<float>(action.data(),kActActiveTime,-1)-.04f)<1e-5f); // stopped hand has no deferred work
}

static float duplicateBeforeD47=0,duplicateAfterD47=0;
static void NativeUpdateDuplicateD47(void* ctrl,float dt){
    duplicateBeforeD47=Rd<float>(action.data(),kActSpeedBias,-1.f);
    // Same installed native child, same XR sequence, before any native action
    // time advances. This is the ordering the old sequence-only gate mishandled.
    TickSemiAlchemy46(ctrl,dt,hostTickCount64);
    duplicateAfterD47=Rd<float>(action.data(),kActSpeedBias,-1.f);
    NativeUpdate46(ctrl,dt);
}
static void ClockReadbackD47(){
    Prepare46(semi46::Mortar,2);QueueDetour(controller.data(),action.data(),0);InstallQueued46();
    Sample46(1020,.08);g_update_original=NativeUpdateDuplicateD47;
    UpdateDetour(controller.data(),.02f);
    ck46(duplicateBeforeD47==2.5f&&duplicateAfterD47==2.5f);
    ck46(std::fabs(Rd<float>(action.data(),kActActiveTime,-1.f)-.05f)<1.e-6f);
    UpdateDetour(controller.data(),.02f);
    ck46(std::fabs(duplicateBeforeD47-1.5f)<1.e-5f&&std::fabs(duplicateAfterD47-1.5f)<1.e-5f);
    ck46(std::fabs(Rd<float>(action.data(),kActActiveTime,-1.f)-.08f)<1.e-6f);
    UpdateDetour(controller.data(),.02f);
    ck46(duplicateBeforeD47==0&&duplicateAfterD47==0&&std::fabs(g_semi_alchemy46.clock.advanced-.08)<1.e-6);
    Sample46(1040,.20);UpdateDetour(controller.data(),.01f);
    const float advanced=Rd<float>(action.data(),kActActiveTime,-1.f);
    Sample46(1050,.20);UpdateDetour(controller.data(),.01f);
    ck46(Rd<float>(action.data(),kActActiveTime,-1.f)==advanced&&g_semi_alchemy46.clock.pending==0&&g_semi_alchemy46.clock.discarded>0);
    Sample46(1060,0);UpdateDetour(controller.data(),.01f);
    ck46(g_semi_pending46&&Rd<float>(action.data(),kActActiveTime,-1.f)==advanced&&Rd<float>(action.data(),kActSpeedBias,-1.f)==0);
    Sample46(1070,2.0);UpdateDetour(controller.data(),.01f);
    ck46(g_semi_alchemy46.clock.pending<=.12&&g_semi_alchemy46.clock.discarded>1.8);
}

int SemiScenario46(){
    Prepare46();QueueDetour(controller.data(),action.data(),0);ck46(g_semi_pending46&&Rd<float>(action.data(),kActSpeedBias,-1)==0.f);
    ck46(Rd<int>(action.data(),kActStatus,-1)==1&&Rd<float>(action.data(),kActActiveTime,-1)==0.f);
    InstallQueued46();Sample46(1010,0);UpdateDetour(controller.data(),.01f);ck46(Rd<float>(action.data(),kActActiveTime,-1)==0.f);
    // Two XR samples before one native frame: cumulative credits survive the
    // latest-only mailbox, so no one-third work loss at XR90/native60.
    Sample46(1020,.006);Sample46(1030,.013);UpdateDetour(controller.data(),.02f);
    ck46(std::fabs(Rd<float>(action.data(),kActActiveTime,-1)-.013f)<1e-6f);
    UpdateDetour(controller.data(),.02f);ck46(std::fabs(Rd<float>(action.data(),kActActiveTime,-1)-.013f)<1e-6f); // no replay
    Sample46(1040,.013);UpdateDetour(controller.data(),.01f);ck46(Rd<float>(action.data(),kActSpeedBias,-1)==0.f);
    Sample46(1050,.10);UpdateDetour(controller.data(),.01f);ck46(Rd<float>(action.data(),kActSpeedBias,-1)==2.5f);
    const auto capped=Rd<float>(action.data(),kActActiveTime,-1);Sample46(1060,.10);UpdateDetour(controller.data(),.01f);ck46(Rd<float>(action.data(),kActActiveTime,-1)==capped);
    // Existing controller scale is accounted once; original native dt stays
    // unchanged. Motion during native scale0 never becomes future queued work.
    Prepare46();QueueDetour(controller.data(),action.data(),0);InstallQueued46();put(controller.data(),kCtrlTimeScale,2.f);
    Sample46(1020,.01);UpdateDetour(controller.data(),.02f);
    ck46(originalDt46==.02f&&std::fabs(Rd<float>(action.data(),kActSpeedBias,-1)-.25f)<1e-6f);
    ck46(std::fabs(Rd<float>(action.data(),kActActiveTime,-1)-.01f)<1e-6f);
    put(controller.data(),kCtrlTimeScale,0.f);Sample46(1040,.03);UpdateDetour(controller.data(),.02f);ck46(Rd<float>(action.data(),kActSpeedBias,-1)==0.f);
    put(controller.data(),kCtrlTimeScale,1.f);Sample46(1060,.03);UpdateDetour(controller.data(),.02f);ck46(std::fabs(Rd<float>(action.data(),kActActiveTime,-1)-.01f)<1e-6f);
    // Mode revocation is direct, even before worker publication changes.
    kcd2vr::minigame_hands45::mode=kcd2vr::minigame_hands45::Native;UpdateDetour(controller.data(),.01f);
    ck46(!g_semi_pending46&&Rd<float>(action.data(),kActSpeedBias,-1)==1.f);
    Prepare46();g_owner.state=Playing;QueueDetour(controller.data(),action.data(),0);ck46(!g_semi_pending46&&Rd<float>(action.data(),kActSpeedBias,-1)==1.f);
    Prepare46();QueueDetour(controller.data(),action.data(),0);InstallQueued46();testWorkstationFrame46.hand[0].valid=testWorkstationFrame46.hand[1].valid=false;
    UpdateDetour(controller.data(),.01f);ck46(!g_semi_pending46&&Rd<float>(action.data(),kActSpeedBias,-1)==1.f); // current loss beats still-fresh control packet
    Prepare46();input46.hand=1;semi46::control.publish(input46);QueueDetour(controller.data(),action.data(),0);InstallQueued46();testWorkstationFrame46.hand[1].valid=false;
    UpdateDetour(controller.data(),.01f);ck46(!g_semi_pending46&&Rd<float>(action.data(),kActSpeedBias,-1)==1.f); // selected left loss cannot borrow still-valid right
    // ACTUAL producer plus Queue/Update: a held selection A begins in pickup
    // and stays admission through the first work queue; only a NEW working A
    // press releases the motion clock. No synthetic alternative producer here.
    Prepare46(semi46::Bellows,0);semi46::Producer gesture{};semi46::WorkWitness before{};ck46(SemiAlchemyParent46(controller.data(),before));
    auto control=gesture.step(testWorkstationFrame46,true,before.instance,before.lifetime,before,true);semi46::control.publish(control);ck46(!control.manualUse);
    put(parent46.data(),0x78,int(2));QueueDetour(controller.data(),action.data(),0);InstallQueued46();ck46(g_semi_pending46);
    semi46::WorkWitness working{};ck46(SemiAlchemyParent46(controller.data(),working));
    control=gesture.step(testWorkstationFrame46,true,working.instance,working.lifetime,working,true);semi46::control.publish(control);
    UpdateDetour(controller.data(),.01f);ck46(g_semi_pending46&&!control.manualUse&&Rd<float>(action.data(),kActSpeedBias,-1)==0.f);
    semi46::control.publish(gesture.step(testWorkstationFrame46,true,working.instance,working.lifetime,working,false));
    semi46::control.publish(gesture.step(testWorkstationFrame46,true,working.instance,working.lifetime,working,true));
    UpdateDetour(controller.data(),.01f);ck46(!g_semi_pending46&&Rd<float>(action.data(),kActSpeedBias,-1)==1.f);
    // Manual A restores this clip immediately. Releasing A cannot freeze the
    // same clip after it has resumed; the next native work queue is eligible.
    Prepare46();QueueDetour(controller.data(),action.data(),0);InstallQueued46();input46.manualUse=true;semi46::control.publish(input46);
    UpdateDetour(controller.data(),.01f);ck46(!g_semi_pending46&&Rd<float>(action.data(),kActSpeedBias,-1)==1.f);
    input46.manualUse=false;semi46::control.publish(input46);UpdateDetour(controller.data(),.01f);ck46(!g_semi_pending46);
    // Queued zero-rate children also restore while native queue still owns them.
    Prepare46();QueueDetour(controller.data(),action.data(),0);input46.enabled=false;semi46::control.publish(input46);UpdateDetour(controller.data(),.01f);
    ck46(!g_semi_pending46&&Rd<float>(action.data(),kActSpeedBias,-1)==1.f);
    // Unknown queue membership cannot discard cleanup; worker stop retains it.
    Prepare46();QueueDetour(controller.data(),action.data(),0);g_owner_thread13=99;ck46(!StopSemiAlchemy46()&&g_semi_pending46);
    g_owner_thread13=1;ck46(StopSemiAlchemy46()&&!g_semi_pending46&&Rd<float>(action.data(),kActSpeedBias,-1)==1.f);
    Prepare46();QueueDetour(controller.data(),action.data(),0);InstallQueued46();put(action.data(),kActSpeedBias,.75f);UpdateDetour(controller.data(),.01f);
    ck46(!g_semi_pending46&&Rd<float>(action.data(),kActSpeedBias,-1)==.75f);
    Prepare46();QueueDetour(controller.data(),action.data(),0);InstallQueued46();input46.enabled=false;semi46::control.publish(input46);refusedWrite19=action.data()+kActSpeedBias;
    UpdateDetour(controller.data(),.01f);ck46(g_semi_pending46);refusedWrite19=nullptr;UpdateDetour(controller.data(),.01f);ck46(!g_semi_pending46);
    Prepare46();QueueDetour(controller.data(),action.data(),0);InstallQueued46();input46.poseEpoch++;semi46::control.publish(input46);UpdateDetour(controller.data(),.01f);
    ck46(!g_semi_pending46&&Rd<float>(action.data(),kActSpeedBias,-1)==1.f);
    // Failed variable-rate writes retain BOTH the old and attempted values.
    // Once writes are possible, restoration recognizes our old zero value.
    Prepare46();QueueDetour(controller.data(),action.data(),0);InstallQueued46();Sample46(1020,.01);refusedWrite19=action.data()+kActSpeedBias;
    UpdateDetour(controller.data(),.02f);ck46(g_semi_pending46&&g_semi_alchemy46.writeUncertain&&g_semi_alchemy46.previousWritten==0.f&&g_semi_alchemy46.lastWritten==.5f);
    refusedWrite19=nullptr;UpdateDetour(controller.data(),.02f);ck46(!g_semi_pending46&&Rd<float>(action.data(),kActSpeedBias,-1)==1.f);
    // Unreadable live action memory retains cleanup; it is never interpreted as
    // a dead child. Native queued membership allows a later safe restore.
    Prepare46();QueueDetour(controller.data(),action.data(),0);input46.enabled=false;semi46::control.publish(input46);forbidRetiredRead19c4=true;
    UpdateDetour(controller.data(),.01f);ck46(g_semi_pending46);forbidRetiredRead19c4=false;UpdateDetour(controller.data(),.01f);ck46(!g_semi_pending46);
    // Native pickup and putaway run automatically; working mortar must be
    // qualified in phase2 DURING queue, then becomes phase3 after return.
    for(unsigned kind:{unsigned(semi46::Bellows),unsigned(semi46::Mortar)}){
        for(int phase:{0,1,3,4}){if(kind==semi46::Bellows&&phase==4)continue;if(kind==semi46::Mortar&&phase==3)continue;
            Prepare46(kind,phase);QueueDetour(controller.data(),action.data(),0);ck46(!g_semi_pending46);}
    }
    Prepare46(semi46::Mortar,2);QueueDetour(controller.data(),action.data(),0);ck46(g_semi_pending46);InstallQueued46();Sample46(1020,.01);UpdateDetour(controller.data(),.02f);
    ck46(std::fabs(Rd<float>(action.data(),kActActiveTime,-1)-.01f)<1e-6f);
    // A late enabled run may not freeze an already advancing work child.
    Prepare46();NativeQueue46(controller.data(),action.data(),0);InstallQueued46();put(action.data(),kActActiveTime,.2f);UpdateDetour(controller.data(),.01f);
    ck46(!g_semi_pending46&&Rd<float>(action.data(),kActSpeedBias,-1)==1.f);
    // Exact identity, fresh input and current phase gates precede native writes.
    for(unsigned fault=0;fault<12;++fault){Prepare46();
        if(fault==0)input46.tick=919;if(fault==1)input46.tick=1001;if(fault==2)input46.enabled=false;if(fault==3)input46.instance++;
        if(fault==4)input46.lifetime++;if(fault==5)put(parent46.data(),0x58,std::uintptr_t(1));if(fault==6)put(parent46.data(),0x40,std::uintptr_t(1));
        if(fault==7)put(parent46.data(),0x3C,int(0));if(fault==8)put(action.data(),0x268,std::uintptr_t(1));if(fault==9)put(parent46.data(),0x28,std::uint8_t(0));
        if(fault==10)input46.poseEpoch=0;if(fault==11)kcd2vr::activity_input163::witness175.transitionDepth175=1;
        semi46::control.publish(input46);QueueDetour(controller.data(),action.data(),0);ck46(!g_semi_pending46&&Rd<float>(action.data(),kActSpeedBias,-1)==1.f);
    }
    Prepare46(semi46::Bellows,1);put(parent46.data(),0x90,.2f);SemiBellowsTickDetour46(parent46.data(),.3f);
    ck46(lastBellowsDt46==0.f&&Rd<float>(parent46.data(),0x90,0)==.2f);
    input46.enabled=false;semi46::control.publish(input46);SemiBellowsTickDetour46(parent46.data(),.3f);
    ck46(lastBellowsDt46==.3f&&Rd<float>(parent46.data(),0x90,0)==.5f);
    Prepare46(semi46::Bellows,2);SemiBellowsTickDetour46(parent46.data(),.3f);ck46(lastBellowsDt46==.3f);
    // Book native row enable/disable ownership, immediate generation revocation,
    // and a re-entrant disable never accidentally republished as enabled.
    Prepare46(semi46::Book,0);put(alchemy46.data(),0x300,std::uint8_t(4));SemiBookEnableDetour46(parent46.data(),true);
    semi46::BookEnable e{};ck46(semi46::bookEnable.read(e)&&e.enabled&&e.action==reinterpret_cast<std::uintptr_t>(parent46.data()));
    const auto generation=e.generation;SemiBookEnableDetour46(parent46.data(),false);ck46(semi46::bookEnable.read(e)&&!e.enabled&&e.generation>generation);
    nestedBookDisable46=true;SemiBookEnableDetour46(parent46.data(),true);ck46(semi46::bookEnable.read(e)&&!e.enabled);
    put(alchemy46.data(),0x300,std::uint8_t(0));SemiBookEnableDetour46(parent46.data(),true);ck46(semi46::bookEnable.read(e)&&!e.enabled);
    ResourceCases46();MortarQueueLedger46();ClockReadbackD47();
    semi46::hooksReady=false;kcd2vr::activity_input163::witness175.Clear();
    std::printf("PASS C46 exact alchemy native seams: %u checks; real production Queue/Update/book/tick adapters, mocked native leaves only\n",checks46);
    return 0;
}
}}
#ifndef KCD2VR_ALCHEMY_SEMI46_NO_MAIN
int main(){return kcd2vr::action_time_owner59c11::SemiScenario46();}
#endif
