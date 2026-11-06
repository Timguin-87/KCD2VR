#include "vrik/melee_stroke118.h"
#include "vrik/physical_guard112.h"
#include "diagnostics/capture168.h"
#include "ui_hud_geometry.h"
#include <cstdio>
#include <cstdlib>
using namespace kcd2vr;
static void check(bool b,const char* why){if(!b){std::fprintf(stderr,"FAIL %s\n",why);std::exit(1);}}
static melee118::Result jab(int contactFrame,double speed=3.,bool known=true,unsigned target=42){
    melee118::State s{};s.ready=true;melee118::Result r{};
    for(int i=1;i<=8;++i){melee118::Input in{};in.dt=.01;in.eligible=true;in.known=known;in.outward=.4;
        in.axis=in.modelAxis={0,1,0};in.hand=in.modelHand=in.tip=in.modelTip={0,speed*.01,0};
        if(i==contactFrame){in.blocked=true;in.contactEntity175=target;in.contactFraction175=.5;}
        r=melee118::update(s,in,{true,true});if(r.commit||r.abort)break;
    }return r;
}
struct Replay {
    physical::Input in{};physical::State state{};std::array<physical::Profile,2> profiles{};physical::Output out{};
    Replay(){in.allowed=in.tracked[0]=in.tracked[1]=in.intentTracked[0]=true;in.grip[0]=1;
        in.epoch=in.bodyEpoch=in.sample=1;in.tick=1000;in.reach[0]=1.3;
        in.naturalCombat117=in.coherentMelee119=in.positionGuard176=in.heldMotionAttacks=in.controllerAnchor119=true;
        in.dynamics=in.contact=false;in.guardEnterDwellMs178r29=80;in.guardEnterMaxSpeed178r29=1.2;
        in.shoulders[0]={.15,0,1.3};in.shoulders[1]={-.15,0,1.3};
        in.hands[0].p={-.3,.4,1.5};in.intentHands[0]=in.hands[0];
        auto& p=profiles[0];p.key=p.generation=1;p.side=0;p.classId=4;p.primary=p.geometry=true;p.tip={.6,0,0};p.axis={1,0,0};
        sample();}
    void sample(){out=physical::step(state,in,profiles,[](const auto&,auto,auto,bool){return 1.;});}
    void move(pose_core::Vec3 d){in.tick+=10;++in.sample;in.hands[0].p=in.hands[0].p+d;in.intentHands[0]=in.hands[0];sample();}
};
int main(){
    {using ui_hud_geometry::ManualBorders30c1;
        const auto base=ManualBorders30c1(140,940,1,1),up=ManualBorders30c1(140,940,1.5,1),down=ManualBorders30c1(140,940,1,1.5);
        check(base.top==140&&base.bottom==940&&base.center==540,"manual border defaults preserve previous effective height");
        check(up.top<base.top&&up.bottom==base.bottom&&up.center==base.center,"upper border moves only upper semantic anchors");
        check(down.bottom>base.bottom&&down.top==base.top&&down.center==base.center,"lower border moves only lower semantic anchors");
        const auto asymmetric=ManualBorders30c1(160,960,1.4,.7);
        check(asymmetric.center==560&&asymmetric.center!=(asymmetric.top+asymmetric.bottom)*.5f,"asymmetric borders cannot recenter dialogue prompts");
        check(ManualBorders30c1(0,1080,3,3).top<0&&ManualBorders30c1(0,1080,3,3).bottom>1080,"expanded borders are not silently clamped to 100 percent");
        check(ManualBorders30c1(140,940,NAN,NAN).top==140,"nonfinite boundary falls back to measured surface");}
    auto fast=jab(5);check(fast.commit&&!fast.abort&&fast.observation.age<.065,"13.5 cm / 50 ms checked stab must commit without waiting beyond first contact");
    check(jab(0).commit,"free-space stab retains deliberate travel and age qualification");
    check(jab(3).commit&&!jab(3).abort,"R54: a deliberate 3 m/s stab that reaches the actor at 30 ms commits (the contact is the strike)");
    check(!jab(1).commit||jab(1).observation.kind!=melee118::Kind::None,"R54: a contact on the very first sample still needs an active candidate");
    check(jab(5,3.,true,0).commit,"R54: a prepared stroke stopped by the world still releases (the collision never cancels an attack)");
    check(!jab(5,3.,false).commit,"unknown contact cannot use the actor-contact exception");
    {Replay r;check(!r.out.guard117&&r.out.owned[0],"acquisition keeps weapon ownership without instantaneous block");
        for(int i=0;i<8;++i)r.move({});check(!r.out.guard117,"guard dwell is measured on fresh samples");
        r.move({});check(r.out.guard117&&r.out.guardBySide30c1[0],"stable posture qualifies one shared block decision");
        r.move({.03,0,0});check(r.out.guard117,"already held guard survives deliberate counter-motion");
        r.in.hands[0].p.y=-.4;r.move({});check(!r.out.guard117,"moving guard behind Henry releases it");}
    {Replay r;for(int i=0;i<8;++i){r.move({.03,0,0});check(!r.out.guard117,"moving blade through guard cannot silently cancel the attack candidate");}}
    {Replay r;kcd2vr::guard112::State guard{};auto d=guard112::update(guard,r.in,r.profiles[0],r.out.weapons[0],true);
        check(!d.block,"native guard helper cannot bypass dwell on first/reset sample");}
    {Replay r;r.in.guardEnterMaxSpeed178r29=2.5;r.in.guardEnterDwellMs178r29=30;
        r.move({0,0,.02});r.move({0,0,.02});check(!r.out.guard117,"new preset still requires deliberate entry dwell");
        r.move({0,0,.02});r.move({0,0,.02});check(r.out.guard117,"2 m/s guard raise qualifies after 30 ms in the actual producer");}
    // R59.16C7E-DECAP1 (Miki): combat summaries are F3 material, not quiet-mode lines.
    check(!diagnostics::capture168::essential("V66.178R40C1 GUARD_SUMMARY press=1")&&diagnostics::capture168::domain("V66.178R40C1 GUARD_SUMMARY press=1")==diagnostics::capture168::Domain::Combat,"guard summary is F3 (combat recorder) material");
    {guard_parry176::Entry s{};check(!guard_parry176::enter(s,true,false,1000,NAN,1.2,0),"invalid speed never grants a guard");}
    check(!diagnostics::capture168::essential("V66.178R30C1 BODY_GATE native=1")&&diagnostics::capture168::domain("V66.178R30C1 BODY_GATE native=1")==diagnostics::capture168::Domain::Combat,"handoff cause is F3 material");
    check(diagnostics::capture168::essential("V66.178R30C1 BODY_GATE_FAULT native=1"),"faults still survive recorders OFF");
    check(!diagnostics::capture168::essential("V66.117 MELEE_INPUT edge=PREPARE"),"detailed per-edge logging remains capture gated");
    std::puts("PASS R30C1 actual-header stab/guard/recovery diagnostic policies; no game execution");
}
