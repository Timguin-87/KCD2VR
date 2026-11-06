#include "vrik/weapon_sweep112.h"
#include "vrik/contact_budget.h"
#include <cstdio>

using namespace kcd2vr::physical;
namespace ev=kcd2vr::sweep169r2;
static unsigned checks=0,failed=0;
static void ck(bool x,const char* why){++checks;if(!x){++failed;std::printf("FAIL %s\n",why);}}
struct Rays {
    kcd2vr::contact::Budget budget;unsigned calls=0,hitAt=0,unknownAt=0;std::uint64_t clock=0,cost=1;
    bool allWalls=false;
    explicit Rays(unsigned cap=18,std::uint64_t limit=1000){budget.begin_work(cap,limit);}
    Contact103 operator()(Vec3,Vec3,double){
        Contact103 c;
        if(!budget.take(clock)){c.known=false;c.fraction=0;c.coverage169r2.reason=budget.remaining?ev::Reason::TimeBudget:ev::Reason::RayCap;return c;}
        ++calls;clock+=cost;budget.finish(clock);
        if(calls==hitAt||allWalls){c.fraction=allWalls?0:.5;c.plane=true;c.normal={0,1,0};}
        if(calls==unknownAt){c.known=false;c.fraction=0;c.coverage169r2.reason=ev::Reason::QueryBusy;}
        return c;
    }
};
struct Fixture {
    Input in{};std::array<Profile,2> profiles{};State state{};
    Pose oldWorld{},oldWeapon{},newHand{};
    bool correctStart=false;double queryRootAngle=0;int side=1;
    Fixture(int selectedSide=1,int kind=9,int combatPolicy=0):side(selectedSide){
        // F2 samples221038/221042 at21:21:32.879/891: same bow/gen,
        // unchanged root position,53.946deg native root yaw. The root cause of
        // that native turn is NOT identified by this fixture or the log.
        // Grip reconstructed from recorded same-sample hand/weapon transforms;
        // radius inferred from recorded34.687258deg checked prefix. No native
        // obstacle query result or exact blade-tip location was captured.
        auto& p=profiles[side];p.key=0x02000000000195A2ULL;p.generation=2;p.entity=42;
        p.side=side;p.classId=kind;p.primary=p.geometry=p.nativeSeed=true;
        p.grip[side]={*m::unit(Quat{.2731664480,.0080332122,.6597885477,.6999961654}),
                   {-.0357543635,-.0623628371,-.0148330012}};
        p.tip=p.grip[side].p+Vec3{0,.7864412998833463,0};
        oldWeapon={*m::unit(Quat{-.1512001,.01180379,-.07752265,.985388}),{.1402384,.4160007,1.289218}};
        p.nativeWeapon=oldWeapon;
        oldWorld={*m::unit(Quat{0,0,.7068586,.7073549}),{3186.596436,490.251801,41.057587}};
        in.world=oldWorld;in.haveWorld=true;in.worldScale=1;
        in.allowed=in.contact=in.controllerAnchor119=in.physicalCombat177=true;
        in.tracked[side]=in.intentTracked[side]=true;in.grip[side]=1;in.reach[side]=reach_max(.26628,.26153);
        in.shoulders[side]={.00639,.15556,1.42689};in.hands[side]=compose(oldWeapon,p.grip[side]);in.intentHands[side]=in.hands[side];
        if(combatPolicy){
            in.naturalCombat117=in.coherentMelee119=in.forwardOnly19=in.heldMotionAttacks=in.hitTraceC54R3=true;
            in.motionOwned59c5=combatPolicy==2;
            in.modelIntentSourceH30[side]=in.modelIntentTrackedH30[side]=true;in.modelIntentHandsH30[side]=in.hands[side];
        }
        in.epoch=in.bodyEpoch=8;in.sample=221038;in.tick=1271361515;in.timeNs=1271361533576100;
        newHand={*m::unit(Quat{.1818165,.09310453,.5872417,.7832122}),{.09593326,.3586,1.295328}};
        g_sweep_coverage_scale178r6=1.;Rays rays;run(rays);
        in.sample=221042;in.tick=1271361531;in.timeNs=1271361544593300;
        in.world.q=*m::unit(Quat{0,0,.3091314,.9510193});
        in.hands[side]=newHand;in.intentHands[side]=newHand;
    }
    Output run(Rays& rays){return step(state,in,profiles,[&](const Profile& p,Pose a,Pose b,bool acquire){
        const auto expected=compose(inverse(in.world),compose(state.world,oldWeapon));
        correctStart=!state.haveWorld||(m::length(a.p-expected.p)<1e-7&&m::angle(a.q,expected.q)<1e-6);
        queryRootAngle=p.rootTurnAngleD44;
        kcd2vr::contact::AcquisitionBurstD45 burst(rays.budget,p.initialAcquireSweepD44&&!acquire);
        return sweep_weapon112(p,a,b,acquire,rays);});}
};
int main(){
    {Fixture f;Rays rays;const auto out=f.run(rays);
     const auto oldHand=compose(f.oldWeapon,f.profiles[1].grip[1]);
     const double worldMotion=m::length(point(f.in.world,f.newHand.p)-point(f.oldWorld,oldHand.p));
     const double gap=m::length(out.hands[1].p-f.newHand.p)*1000;
     std::printf("witnessedRootDeg=%.6f requestedWorldMotionMm=%.6f handGapMm=%.6f rays=%u reason=%s\n",
        m::angle(f.oldWorld.q,f.in.world.q)*180/3.141592653589793,worldMotion*1000,gap,rays.calls,ev::name(out.contact119[1].coverage169r2.reason));
     ck(f.correctStart&&worldMotion>.33,"same item sweep starts at actual preceding WORLD pose, not live transported display");
     ck(out.contactKnown[1]&&!out.sweepDeferred115[1]&&gap<.00001&&rays.calls<=18,
        "54degree root arc completes real bounded queries without pulling wrist through a clear-prefix pose");
     ck(!out.displayRejoinR13[1]&&!out.displayLatched[1]&&!out.handlingOnly55c1[1]&&out.commandCount==0,
        "world-turn repair grants no display exemption, action, or second pose owner");
     const auto calls=rays.calls;f.run(rays);ck(rays.calls==calls,"repeated source cannot spend the query budget again");}
    {Fixture f;Rays rays;rays.allWalls=true;const auto out=f.run(rays);
     ck(f.correctStart&&out.contact119[1].plane&&out.blocked[1]&&!out.displayRejoinR13[1]&&
        m::length(out.hands[1].p-f.newHand.p)>.3&&out.commandCount==0,
        "real wall blocks root arc and retains checked pose");}
    for(int mode=0;mode<3;++mode){
        Fixture f;Rays rays(mode==0?4:18,mode==2?5:1000);if(mode==1)rays.unknownAt=5;
        const auto out=f.run(rays);
        // D67R1: all five spatial features must be queried before any common
        // prefix exists. A first-prefix failure is unknown/zero (not a known
        // limited prefix), and must keep the complete preceding WORLD pose.
        const bool zeroPrefix=!out.contact119[1].known&&out.contact119[1].fraction==0&&
            m::length(out.hands[1].p-compose(compose(inverse(f.in.world),compose(f.oldWorld,f.oldWeapon)),f.profiles[1].grip[1]).p)<1e-7;
        ck(f.correctStart&&!out.contactKnown[1]&&(out.sweepDeferred115[1]||zeroPrefix)&&!out.displayRejoinR13[1]&&
           m::length(out.hands[1].p-f.newHand.p)>.05&&out.commandCount==0,
           "spent budget UNKNOWN and timeout preserve only completed segments without display bypass");
        ck(rays.calls<=unsigned(mode==0?4:mode==1?5:5),"root arc cannot replenish original shared budget");
    }
    {Fixture f;f.in.world.q=m::multiply(f.oldWorld.q,m::from_rotation_vector({0,0,.01}));Rays rays;
     const auto out=f.run(rays);ck(out.contactKnown[1]&&rays.calls==5,"small ordinary turn keeps original single segment five probes");}
    for(int replacement=0;replacement<3;++replacement){
        Fixture f;
        if(replacement==0)++f.profiles[1].generation;
        if(replacement==1)++f.in.epoch;
        if(replacement==2)++f.in.bodyEpoch;
        // New owner has its own known native pose. Old root metadata must not
        // extend this owner's acquisition coverage.
        f.profiles[1].nativeWeapon=compose(inverse(f.in.world),compose(f.oldWorld,f.oldWeapon));
        Rays rays;const auto out=f.run(rays);
        ck(f.queryRootAngle==0&&out.initialAcquireSweepD44[1]&&out.contactKnown[1]&&rays.budget.acquisitionBurstUsedD45,
           "generation request and body replacements get their own checked acquisition, never prior-owner root coverage");
    }
    for(int policy:{1,2}){
        // Same root turn, but a real right-hand melee profile, with native
        // HYBRID/DIRECT recognition enabled and stationary tracked intention.
        Fixture f(0,4,policy);const auto hand=compose(f.oldWeapon,f.profiles[0].grip[0]);
        f.in.hands[0]=f.in.intentHands[0]=f.in.modelIntentHandsH30[0]=hand;
        Rays rays;auto out=f.run(rays);
        ck(out.contactKnown[0]&&rays.calls<=18&&m::length(out.hands[0].p-hand.p)<1e-7,
           "right sword completes checked root arc while staying at stationary model grip");
        ck(out.hitTraceC54R3.observed&&out.hitTraceC54R3.attackIntent&&out.hitTraceC54R3.continuous&&
           out.hitTraceC54R3.modelTipSpeed<1e-7&&out.hitTraceC54R3.relativeTipSpeed<1e-7&&
           out.hitTraceC54R3.modelHandSpeed<1e-7&&out.hitTraceC54R3.relativeHandSpeed<1e-7,
           "enabled melee recognizer measures actual model and tracking intention rather than world root sweep speed");
        ck(out.commandCount==0&&!out.strokeActive119&&!f.state.weapon[0].committedGestureH26.gesture&&
           !out.impactRecord178r30[0].valid&&!out.impaleLatchRequest,
           "native root rotation cannot mint a HYBRID or DIRECT strike impact or impale");
        ++f.in.sample;f.in.tick+=11;f.in.timeNs+=11000000;Rays next;out=f.run(next);
        ck(out.commandCount==0&&!out.strokeActive119&&!f.state.weapon[0].committedGestureH26.gesture,
           "no delayed gesture appears on the next stationary sample after root turn");
    }
    std::printf("RESULT %u/%u passed; production sweep/kernel, not native-turn or headset validation\n",checks-failed,checks);
    return failed?1:0;
}
