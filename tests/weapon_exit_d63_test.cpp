// Real WorldRayDistance6692 + PhysSweep6698 + sweep_weapon112 + physical::step.
// Only native memory/query/clock/opponent/recording leaves are host fixtures.
#define KCD2VR_WORLD_ADAPTER_FIXTURE_ONLY_D53 1
#include "world_exit_fixture.inc"
#undef KCD2VR_WORLD_ADAPTER_FIXTURE_ONLY_D53
#include "vrik/weapon_sweep112.h"
#include "diagnostics/bow_contact169r2.h"
namespace ph=kcd2vr::physical;
namespace pm=kcd2vr::pose_core;
static bool PhysContactOpponent175(const npc172::Witness&) noexcept {return false;}
#include "physical_sweep_exit_actual.inc"

struct ExitPlaneD63 {V3 point{},normal{};std::uintptr_t body=0;int part=0;};
static std::vector<ExitPlaneD63> exitPlanesD63;
static bool reverseClearD63=false,reverseWrongPointD63=false;
static int reverseIdentityD63=0;
static int NativeExitLeafD63(const V3& origin,const V3& motion,void* hit){
    float nearest=2;const ExitPlaneD63* chosen=nullptr;
    for(const auto& plane:exitPlanesD63){
        const float denominator=Dot(plane.normal,motion);
        if(std::abs(denominator)<1e-7f)continue;
        const float t=-Dot(plane.normal,Sub(origin,plane.point))/denominator;
        if(t>=0&&t<=1&&t<nearest){nearest=t;chosen=&plane;}
    }
    if(!chosen)return 0;
    if(reverseClearD63&&Dot(chosen->normal,motion)<0)return 0;
    const float distance=nearest*Len(motion);V3 point=Add(origin,Mul(motion,nearest));
    if(reverseWrongPointD63&&Dot(chosen->normal,motion)<0)point.y+=.01f;
    const bool reverse=Dot(chosen->normal,motion)<0;
    const int part=chosen->part+int(reverse&&reverseIdentityD63==1);
    const std::int16_t surface=reverse&&reverseIdentityD63==2?1:0;
    const V3 normal=reverse&&reverseIdentityD63==3?V3{-.999f,.04471018f,0}:chosen->normal;
    std::memcpy(hit,&distance,4);std::memcpy(static_cast<unsigned char*>(hit)+8,&chosen->body,sizeof(chosen->body));
    std::memcpy(static_cast<unsigned char*>(hit)+0x14,&part,4);
    std::memcpy(static_cast<unsigned char*>(hit)+0x18,&surface,2);
    std::memcpy(static_cast<unsigned char*>(hit)+0x24,&point,sizeof(point));
    std::memcpy(static_cast<unsigned char*>(hit)+0x30,&normal,sizeof(normal));return 1;
}
static void ExitBodyD63(std::uintptr_t body,std::uintptr_t vft=0x3a578c0,bool entity=false,bool readable=true){
    Store(body,base+vft);Store(base+vft+0x50,base+0x5d6f44);
    if(readable)Store(body+0x20,entity?std::uintptr_t(0x180006000ull):std::uintptr_t(0));
    if(entity){Store(0x180006000ull,base+0x4072c78);Store(0x18000600Cull,std::uint32_t(933896));}
}
static void ExitInitD63(){
    Init();g_census_mode=4;mockBodies172=mockPrepareNpcH9=true;mockMode=63;
    exitPlanesD63.clear();reverseClearD63=reverseWrongPointD63=false;
    reverseIdentityD63=0;
    ExitBodyD63(0x180004000ull);ExitBodyD63(0x180007000ull);
}
static ph::Profile ExitProfileD63(){
    ph::Profile p{};p.side=0;p.key=p.generation=1;p.classId=4;p.primary=p.geometry=p.nativeSeed=true;
    p.tip={0,0,.2};p.axis={0,0,1};return p;
}
static ph::Contact103 ExitSweepD63(V3 a,V3 b,std::uint64_t& previous,std::uint64_t sample=1,
                                  unsigned budget=18,std::uint64_t tick=1000,bool acquire=false,ph::Profile p=ExitProfileD63()){
    ContactPass6695 pass(true,tick,previous,sample,3);
    if(g_contact_pass6695)g_contact_budget6695.begin_work(budget,1000);
    return PhysSweep6698(reinterpret_cast<void*>(character),p,{{},{a.x,a.y,a.z}},{{},{b.x,b.y,b.z}},acquire);
}
int main(){
    // Captured serials897 and898: original ray origin/motion/native plane. The
    // scene beyond their recorded hit is explicitly modeled, not a game replay.
    struct Recorded {V3 origin,motion,point,normal;};
    const Recorded records[]{
        {{3186.66406f,493.83282f,42.03967f},{-.04443f,.00818f,-.03421f},{3186.660888671875f,493.83343505859375f,42.03717041015625f},{-.6156613826751709f,-.7880107164382935f,0}},
        {{3186.62915f,493.86014f,42.18776f},{.08521f,-.13620f,-.023f},{3186.631591796875f,493.85626220703125f,42.18710708618164f},{-.6156613826751709f,-.7880107164382935f,0}}
    };
    for(const auto& r:records){
        ExitInitD63();exitPlanesD63.push_back({r.point,r.normal,0x180004000ull,0});std::uint64_t previous=0;
        const auto result=ExitSweepD63(r.origin,Add(r.origin,r.motion),previous);
        Check(result.known&&!result.plane&&!result.limited115&&result.fraction==1,
            "recorded shallow outward exit can complete only after its exact reverse tail confirmation");
        Check(mockCalls==10&&g_contact_budget6695.used==10&&g_contact_budget6695.used<=18&&argumentsOk,
            "five real weapon feature rays and five confirmations share the original native budget and ABI");
        const auto calls=mockCalls;const auto repeated=ExitSweepD63(r.origin,Add(r.origin,r.motion),previous,1,18,1000);
        Check(mockCalls==calls&&!repeated.known,
            "same source family cannot manufacture another reverse-tail query or clearance receipt");
        const auto next=ExitSweepD63(r.origin,Add(r.origin,r.motion),previous,2,18,1000);
        Check(next.known&&!next.plane&&next.fraction==1&&mockCalls==calls+10,
            "distinct XR sample with the same Windows tick still gets its own current checked path");
    }
    const V3 origin{.0015f,.2f,1},end{-.10f,.2f,1};
    // Second wall is only2mm thick. Reverse-first-hit proof refuses the entire
    // recovery whether it belongs to another collider OR the same body/part.
    for(bool sameBody:{false,true})for(bool insideDestination:{false,true}){
        ExitInitD63();const auto body=sameBody?0x180004000ull:0x180007000ull;
        exitPlanesD63={{{0,0,0},{-1,0,0},0x180004000ull,0},{{-.050f,0,0},{1,0,0},body,0},{{-.052f,0,0},{-1,0,0},body,0}};
        std::uint64_t previous=0;const auto result=ExitSweepD63(origin,insideDestination?V3{-.051f,.2f,1}:end,previous);
        Check(result.known&&result.plane&&result.fraction==0&&mockCalls<=2,
            "a second2mm wall or destination inside it cannot be skipped by an outward exit");
    }
    for(unsigned budget:{1u,2u,5u,9u}){
        ExitInitD63();exitPlanesD63={{{0,0,0},{-1,0,0},0x180004000ull,0}};
        std::uint64_t previous=0;const auto result=ExitSweepD63(origin,end,previous,1,budget);
        Check(result.fraction==0&&mockCalls<=int(budget)&&g_contact_budget6695.used<=budget,
            "an incomplete exit or feature tail cannot clear the segment or renew native query credits");
    }
    {
        ExitInitD63();mockQueryUs59c1=600;exitPlanesD63={{{0,0,0},{-1,0,0},0x180004000ull,0}};
        std::uint64_t previous=0;const auto result=ExitSweepD63(origin,end,previous);
        Check(result.fraction==0&&mockCalls<=2&&g_contact_budget6695.spent<=1000,
            "reverse confirmation remains inside the existing shared work budget");
    }
    for(int refusal=0;refusal<9;++refusal){
        ExitInitD63();exitPlanesD63={{{0,0,0},{-1,0,0},0x180004000ull,0}};
        V3 from=origin,to=end;auto p=ExitProfileD63();bool acquire=false;
        if(refusal==0)ExitBodyD63(0x180004000ull,0x3a578c0,true);
        if(refusal==1){memory.erase(0x180004020ull);}
        if(refusal==2)ExitBodyD63(0x180004000ull,0x3a48b30);
        if(refusal==3)from.x=.004f;
        if(refusal==4){from.x=-.01f;to.x=.1f;}
        if(refusal==5)p.initialAcquireSweepD44=true;
        if(refusal==6)p.nativeRejoinSweepR28=true;
        if(refusal==7)p.classId=9;
        if(refusal==8)acquire=true;
        std::uint64_t previous=0;const auto result=ExitSweepD63(from,to,previous,1,18,1000,acquire,p);
        Check(result.known&&result.plane&&result.fraction<1&&mockCalls==(refusal==4?5:1),
            "NPC-owned, unreadable, articulated, deep, inward, acquisition, rejoin and nonmelee paths retain their hard hit");
    }
    for(bool malformed:{false,true}){
        ExitInitD63();exitPlanesD63={{{0,0,0},{-1,0,0},0x180004000ull,0}};
        reverseWrongPointD63=malformed;reverseClearD63=!malformed;std::uint64_t previous=0;
        const auto result=ExitSweepD63(origin,end,previous);
        Check(result.known&&result.plane&&result.fraction==0&&mockCalls<=2,
            "reverse CLEAR or off-ray hit cannot substitute for exact same-plane proof");
    }
    for(int identity=1;identity<=3;++identity){
        ExitInitD63();exitPlanesD63={{{0,0,0},{-1,0,0},0x180004000ull,0}};
        reverseIdentityD63=identity;std::uint64_t previous=0;
        const auto result=ExitSweepD63(origin,end,previous);
        Check(result.known&&result.plane&&result.fraction==0&&mockCalls<=2,
            "a changed native part, material surface or plane normal cannot authorize recovery");
    }
    // The actual weapon kernel consumes the recovered sweep as passive motion.
    {
        ExitInitD63();exitPlanesD63={{{0,0,0},{-1,0,0},0x180004000ull,0}};
        ph::Input in{};in.epoch=in.bodyEpoch=1;in.sample=1;in.tick=1000;in.timeNs=1000000000;
        in.allowed=in.controllerAnchor119=in.physicalCombat177=in.contact=in.tracked[0]=in.intentTracked[0]=true;
        in.attacks=in.dynamics=false;in.grip[0]=1;in.reach[0]=1.3;in.shoulders[0]={-.1,.1,1};
        in.hands[0].p={origin.x,origin.y,origin.z};in.intentHands[0]=in.hands[0];
        std::array<ph::Profile,2> profiles{};profiles[0]=ExitProfileD63();profiles[0].nativeWeapon=in.hands[0];profiles[1].side=1;
        ph::State state{};ph::Output output{};std::uint64_t previous=0;
        const auto kernel=[&](){ContactPass6695 pass(true,in.tick,previous,in.sample,3);
            return ph::step(state,in,profiles,[&](const ph::Profile& p,ph::Pose a,ph::Pose b,bool acquire){
                return PhysSweep6698(reinterpret_cast<void*>(character),p,a,b,acquire);});};
        output=kernel();++in.sample;in.tick+=16;in.timeNs+=16000000;in.hands[0].p={end.x,end.y,end.z};in.intentHands[0]=in.hands[0];output=kernel();
        Check(output.active[0]&&output.owned[0]&&output.contactKnown[0]&&!output.blocked[0]&&
            pm::length(output.hands[0].p-in.hands[0].p)<.00001,
            "actual physical weapon kernel regains tracked outward translation through the verified tail");
        Check(output.commandCount==0&&!output.impactRecord178r30[0].valid&&!output.wallBlocked58[0]&&!output.contact119[0].plane,
            "outward recovery emits no strike, impact, NPC damage request or new wall-hit receipt");
        const auto calls=mockCalls;const auto repeated=kernel();
        Check(mockCalls==calls&&pm::length(repeated.hands[0].p-output.hands[0].p)<1e-10,
            "actual physical kernel reuses its same-source output without native queries");
    }
    std::printf("D63 weapon exits %d/%d; production WorldRay+PhysSweep+step=YES; native_geometry=SIMULATED\n",assertions-failures,assertions);
    return failures?1:0;
}
