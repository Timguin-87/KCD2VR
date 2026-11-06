// Actual production step + extracted native PhysSweep6698 adapter + real
// shared Budget. Only engine world queries/model/fingerprint leaves are doubles.
// Recorded D43 poses plus inferred proxy radius are not exact retail replay.
#include "vrik/weapon_sweep112.h"
#include "vrik/contact_budget.h"
#include <cstdio>
using namespace kcd2vr::physical;
namespace ph=kcd2vr::physical;namespace pm=kcd2vr::pose_core;
namespace evidence=kcd2vr::sweep169r2;
static unsigned checks=0,failed=0;
static void ck(bool ok,const char* why){++checks;if(!ok){++failed;std::printf("FAIL %s\n",why);}}
using V3=pm::Vec3;using Q4=pm::Quat;
struct ModelWorld6692 {Q4 q{};V3 t{};float scale=1;};
static bool modelValid=true,bindingValid=true;
static bool ReadModelWorld6692(void*,ModelWorld6692& model){model={};return modelValid;}
static bool CollisionBinding6692(std::uintptr_t){return bindingValid;}static std::uintptr_t WhBase(){return 1;}
static V3 ModelToWorld6692(const ModelWorld6692&m,V3 p){return pm::rotate(m.q,p*m.scale)+m.t;}
static V3 WorldToModel6692(const ModelWorld6692&m,V3 p){return pm::rotate(pm::conjugate(m.q),p-m.t)*(1/m.scale);}
static V3 QRotateV(Q4 q,V3 p){return pm::rotate(q,p);}static Q4 QConj(Q4 q){return pm::conjugate(q);}
static V3 Sub(V3 a,V3 b){return a-b;}static float Len(V3 p){return float(pm::length(p));}
static kcd2vr::contact::Budget g_contact_budget6695;
static bool g_contact_budget_active6695=true,g_contact_pass6695=true;
struct Rays {kcd2vr::contact::Budget budget;unsigned calls=0,hitAt=0,unknownAt=0;std::uint64_t clock=0,cost=1;
    explicit Rays(unsigned cap=18){budget.begin_work(cap,1000);}};
static Rays* currentRays=nullptr;
static std::uint64_t ContactMicros6695(){return currentRays?currentRays->clock:0;}
namespace wc {enum class Result {Clear,Hit,Busy};}
struct WorldRayResult6693 {wc::Result state=wc::Result::Clear;bool plane=false;float distance=0;V3 point{},normal{1,0,0};
    std::uint64_t witness169r1=0;kcd2vr::npc_contact172::Witness native172{};evidence::Reason reason169r2=evidence::Reason::FullClear;};
static WorldRayResult6693 WorldRayDistance6692(V3 origin,V3 motion){
    auto&r=*currentRays;WorldRayResult6693 h{};
    if(!g_contact_budget_active6695||!g_contact_pass6695||!g_contact_budget6695.take(r.clock)){
        h.state=wc::Result::Busy;h.reason169r2=g_contact_budget6695.spent>=g_contact_budget6695.limit?evidence::Reason::TimeBudget:evidence::Reason::RayCap;return h;}
    ++r.calls;r.clock+=r.cost;g_contact_budget6695.finish(r.clock);
    if(r.calls==r.hitAt){h.state=wc::Result::Hit;h.plane=true;h.distance=Len(motion)*.5f;h.point=origin+motion*.5;}
    if(r.calls==r.unknownAt){h.state=wc::Result::Busy;h.reason169r2=evidence::Reason::QueryBusy;}
    return h;
}
static bool PhysContactOpponent175(const kcd2vr::npc_contact172::Witness&){return false;}
namespace kcd2vr::diagnostics::passive169r1 {
struct Sample {kcd2vr::passive169r1::Source source{};int side=0;std::uintptr_t actor=0;std::uint64_t key=0,generation=0,tick=0,sequence=0,epoch=0,request=0;};
static struct {Sample sample;}rayContext;
struct RayScope {explicit RayScope(Sample){}};
}
namespace kcd2vr::diagnostics::bow169r2 {
static bool recording(std::uint64_t){return false;}
struct Record {int kind=0,side=0;bool acquire=false,known=false,limited=false;double fraction=0;
    std::uint64_t tick=0,sample=0,epoch=0,request=0,actor=0,key=0,generation=0,workUs=0,totalUs=0;unsigned raysUsed=0,raysRemaining=0;evidence::Evidence sweep{};};
static void push(const Record&){}
}
#include "generated_draw_native_d45.inc"
struct Fixture {
    Input in{};
    std::array<Profile,2> profiles{};
    State state{};
    int side=0;
    explicit Fixture(int s=0,int kind=4):side(s){
        g_sweep_coverage_scale178r6=1.; // independent fixture, same initial query policy
        // D43 F2/F7 sample205662, 2026-10-28 21:20:49.584:
        // requested hand, allowed hand and allowed weapon are recorded model
        // transforms. The concurrent native attachment is converted by the
        // recorded actor world transform. Grip is reconstructed from the two
        // allowed transforms. Its exact blade tip is absent from this capture;
        // a 1.00147m proxy radius reconstructs the observed ~30.74deg prefix.
        // Therefore this is a witnessed-pose production-kernel counterexample,
        // not an exact replay of the game's native query results.
        auto& p=profiles[side];p.key=0x0200000000018DDBULL+side;p.generation=1;p.entity=42;
        p.side=side;p.classId=kind;p.primary=p.geometry=p.nativeSeed=true;
        p.grip[side]={*m::unit(Quat{-.6955634978,.7175128608,.0089310518,.0358741052}),
                      {.0208202091,-.0661980550,.0122388618}};
        p.tip=p.grip[side].p+Vec3{0,1.0014743331427713,0};
        p.nativeWeapon={*m::unit(Quat{.5990976809,.2437859012,-.1687048390,-.7437668186}),
                        {.2384974597,.0246359308,.780442}};
        in.allowed=in.contact=in.controllerAnchor119=in.physicalCombat177=true;
        in.attacks=in.naturalCombat117=in.coherentMelee119=true;
        in.tracked[side]=in.intentTracked[side]=true;in.grip[side]=1;
        in.hands[side]={*m::unit(Quat{.5166928,.05994649,.5118814,-.6836758}),
                        {-.1237072,.2190489,1.217}};
        in.intentHands[side]=in.hands[side];
        in.shoulders[side]={.12219,.11650,1.37426};in.reach[side]=reach_max(.26627,.26153);
        in.epoch=in.bodyEpoch=8;in.sample=205662;in.tick=1271318218;in.timeNs=in.tick*1000000;
    }
    void next(){++in.sample;in.tick+=11;in.timeNs+=11000000;}
    Output run(Rays& rays){g_contact_budget6695=rays.budget;currentRays=&rays;
        const auto out=step(state,in,profiles,[&](const Profile& p,Pose a,Pose b,bool acquire){return PhysSweep6698(reinterpret_cast<void*>(1),p,a,b,acquire);});
        rays.budget=g_contact_budget6695;currentRays=nullptr;return out;}
};

int main(){
 g_sweep_coverage_scale178r6=1;
 for(int kind:{4,9,11}){
    Fixture f(kind==4?0:1,kind);Rays rays;const auto out=f.run(rays);const int s=f.side;
    printf("class=%d controllerGapMm=%.9f queries=%u workUs=%llu completed=%u reason=%u\n",kind,
        m::length(out.hands[s].p-f.in.hands[s].p)*1000,rays.calls,(unsigned long long)rays.budget.spent,
        out.contact119[s].coverage169r2.completed,unsigned(out.contact119[s].coverage169r2.reason));
    ck(out.initialAcquireSweepD44[s]&&out.owned[s]&&out.contactKnown[s]&&!out.sweepDeferred115[s],"checked first acquisition covers full clear native-to-grip arc");
    ck(m::length(out.hands[s].p-f.in.hands[s].p)<1e-7,"sword bow and torch first checked grip reaches controller");
    ck(!out.displayLatched[s]&&!out.displayRejoinR13[s]&&!out.handlingOnly55c1[s],"no unchecked visual bridge used");
    ck(rays.calls>18&&rays.calls<=72&&rays.budget.spent==rays.calls&&rays.budget.remaining==0,"actual native adapter receives extra rays without resetting work clock");
    ck(out.commandCount==0&&!out.impactRecord178r30[s].valid,"acquisition fabricates no attack or hit");
    const auto calls=rays.calls;auto duplicate=f.run(rays);
    ck(rays.calls==calls&&duplicate.commandCount==0,"same sample does not repeat acquisition");
    f.next();Rays later;const auto normal=f.run(later);
    ck(!normal.initialAcquireSweepD44[s]&&later.calls<=18&&!later.budget.acquisitionBurstUsedD45,"following owned motion returns to normal budget");
 }
 for(int failure=0;failure<5;++failure){
    Fixture f;Rays r(failure==4?0:18);if(failure==0)r.hitAt=30;if(failure==1)r.unknownAt=23;if(failure==2)r.cost=200;if(failure==3)r.budget.spent=1000;
    const auto out=f.run(r);
    ck(!out.contactKnown[0]||out.contact119[0].plane,"real wall unknown or spent budget cannot be clear");
    ck(!out.displayLatched[0]&&!out.displayRejoinR13[0]&&out.commandCount==0,"failed query cannot produce unchecked display or attack");
    if(failure==0)ck(out.contact119[0].plane&&r.calls>=30&&r.calls<=72&&out.contact119[0].fraction<1,
        "wall beyond old18ray prefix constrains pose after all features cover its hit prefix");
    if(failure==1){const auto& e=out.contact119[0].coverage169r2;
        ck(r.calls==23&&e.completedRays==22&&e.completed>0&&e.checkedAngle>0&&e.checkedAngle<e.requestedAngle&&
           e.reason==evidence::Reason::QueryBusy,"unknown feature admits only common checked coverage with original cause");}
    if(failure==2)ck(r.calls==5&&r.budget.spent==1000,"unchanged1000us time cap stops burst");
    if(failure>=3)ck(r.calls==0,"already spent or disabled budget performs no query");
    f.next();f.state.weapon[0]={};Rays again;auto retry=f.run(again);
    ck(!retry.initialAcquireSweepD44[0]&&!again.budget.acquisitionBurstUsedD45&&again.calls<=18,"recoverable state reset cannot buy another burst for same binding");
 }
 {Fixture f;Rays r;r.cost=25;r.budget.spent=900;auto out=f.run(r);
  ck(r.calls==4&&r.budget.spent==1000&&!out.contactKnown[0],"work from earlier hands remains charged to acquisition");}
 {Fixture f;Rays r;f.run(r);f.next();++f.profiles[0].generation;Rays fresh;auto out=f.run(fresh);
  ck(out.initialAcquireSweepD44[0]&&fresh.budget.acquisitionBurstUsedD45,"new generation gets one new binding attempt");}
 {Fixture f;Rays r;f.run(r);f.next();auto saved=f.profiles[0];f.profiles[0]={};Rays empty;f.run(empty);
  f.next();f.profiles[0]=saved;Rays fresh;auto out=f.run(fresh);
  ck(out.initialAcquireSweepD44[0]&&fresh.budget.acquisitionBurstUsedD45,"actual no-item interval permits subsequent draw");}
 for(int bad=0;bad<5;++bad){Fixture f;Rays r;
  if(bad==0)bindingValid=false;if(bad==1)modelValid=false;if(bad==2)g_contact_budget_active6695=false;if(bad==3)g_contact_pass6695=false;if(bad==4)f.in.contactFaultH30=true;
  f.run(r);ck(!r.budget.acquisitionBurstUsedD45&&(bad==4?r.calls<=18:r.calls==0),"unavailable native binding or fault cannot grant query burst");
  bindingValid=modelValid=g_contact_budget_active6695=g_contact_pass6695=true;
 }
 {Fixture f;f.in.nativeRejoinR28[0]=true;Rays r;const auto out=f.run(r);
  ck(out.nativeRejoinSweepR28[0]&&!out.initialAcquireSweepD44[0]&&!r.budget.acquisitionBurstUsedD45&&r.calls<=18,"native return remains its separate ordinary bounded path");}
 {Fixture f;auto p=f.profiles[0];p.initialAcquireSweepD44=true;p.grip[1].p=p.grip[0].p+Vec3{0,.3,0};
  Rays r;currentRays=&r;g_contact_budget6695=r.budget;const auto target=compose(f.in.hands[0],inverse(p.grip[0]));
  PhysSweep6698(reinterpret_cast<void*>(1),p,p.nativeWeapon,target,true);
  ck(!g_contact_budget6695.acquisitionBurstUsedD45&&r.calls<=18,"support acquisition cannot inherit primary burst");currentRays=nullptr;}
 printf("initial_acquire_burst_d45 %u/%u passed; native leaves doubled, not headset evidence\n",checks-failed,checks);return failed?1:0;
}
