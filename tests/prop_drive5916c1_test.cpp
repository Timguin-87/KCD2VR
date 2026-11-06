#include "vrik/prop_drive5916c1.h"
#include "diagnostics/capture168.h"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <limits>
using namespace kcd2vr::prop5916c1;
int main(){
    Controller s;Dynamics d{};d.mass=2;
    p::Pose actual{},target{};target.p={.10,0,0};s.seed(actual,1,1000);
    auto c=s.update(target,actual,d,2,1020,1020);
    // R59.16C2: the force budget applies on top of weight support (g*dt upward).
    assert(c.decision==Decision::Drive&&m::length(c.linear-m::Vec3{0,0,9.81*.02})<=.4000001&&c.linear.x>0);
    assert(s.update(target,actual,d,2,1020,1040).decision==Decision::Duplicate);
    assert(s.update(target,actual,d,3,1020,1040).decision==Decision::Duplicate); // new sample, same coarse clock tick
    assert(s.update(target,actual,d,3,1101,1101).decision==Decision::End);
    s.seed(actual,1,1000);assert(s.update(target,actual,d,2,1020,1101).decision==Decision::End);
    s.seed(actual,1,1000);d.mass=0;assert(s.update(target,actual,d,2,1020,1020).decision==Decision::End);
    d.mass=2;target.p.x=.351;assert(s.update(target,actual,d,2,1020,1020).decision==Decision::End);
    target.p.x=.1;d.linear.x=std::numeric_limits<double>::quiet_NaN();assert(s.update(target,actual,d,2,1020,1020).decision==Decision::End);
    d={};d.mass=1;target={};target.q=m::from_rotation_vector({0,0,.3});s.seed(actual,1,1000);
    c=s.update(target,actual,d,2,1020,1020);assert(c.decision==Decision::Drive&&c.angular.z>0&&m::length(c.angular)<=1.600001);
    // COM velocity from an offset body is not visual pivot velocity.
    d.centre={.2,0,0};s.seed(actual,1,1000);c=s.update(target,actual,d,2,1020,1020);assert(c.linear.y>0);
    // Same servo, explicitly substituted free body vs wall-constrained physics.
    // No native collision implementation is claimed by this model.
    for(bool wall:{false,true}){
        actual={};target={};target.p={.12,0,0};d={};d.mass=1;s.seed({},1,1000);
        for(unsigned n=1;n<=40;++n){c=s.update(target,actual,d,n+1,1000+10*n,1000+10*n);assert(c.decision==Decision::Drive);
            d.linear=c.linear;d.angular=c.angular;auto next=actual.p+d.linear*.01;
            if(wall&&next.x>.04){next.x=.04;d.linear.x=0;}actual.p=next;d.centre=actual.p;}
        if(wall)assert(actual.p.x<=.040001);else assert(actual.p.x>.10);
    }
    // R59.16C2: a body under native gravity stays in the hand at every accepted mass
    // (the 40 N budget alone could not support anything above ~4.1 kg).
    for(double mass:{.3,5.,15.}){
        actual={};target={};d={};d.mass=mass;s.seed({},1,1000);
        for(unsigned n=1;n<=200;++n){c=s.update(target,actual,d,n+1,1000+11*n,1000+11*n);assert(c.decision==Decision::Drive);
            d.linear=c.linear;for(int k=0;k<11;++k){d.linear.z-=9.81*.001;actual.p=actual.p+d.linear*.001;}d.centre=actual.p;}
        assert(std::fabs(actual.p.z)<.02);
    }
    // R59.16C2: a light object follows a 6 m/s throwing hand (release keeps body velocity).
    {
        actual={};target={};d={};d.mass=.3;s.seed({},1,1000);
        for(unsigned n=1;n<=12;++n){target.p.x=.066*n;c=s.update(target,actual,d,n+1,1000+11*n,1000+11*n);assert(c.decision==Decision::Drive);
            d.linear=c.linear;actual.p=actual.p+d.linear*.011;d.centre=actual.p;}
        assert(d.linear.x>5.&&d.linear.x<=8.0000001);
    }
    // R116 firm grip: Henry turns 180 deg/s with the hand 0.5 m out (1.57 m/s), game 110 fps, pose samples at
    // 100 Hz (some frames see no new sample), physics integrates the command one frame later with gravity.
    // The firm hold keeps the gap < 1.5 cm and the spin error small; the R103 servo (coarse 16 ms tick) lags
    // several cm - that is his "lagging behind my hand when I turn".
    {
        auto run=[&](bool firm,double mass,double& worstCm,double& endCm){
            FirmHoldR116 fh;Controller old;Limits ol{};ol.force=80;ol.angularAcceleration=300;ol.angularSpeed=20;ol.rotationGain=14;
            Dynamics dd{};dd.mass=mass;p::Pose body{};body.p={.5,0,0};dd.centre=body.p;
            m::Vec3 pending{},pendingW{};worstCm=0;endCm=0;
            auto hand=[&](double t){p::Pose h{};const double a=3.14159265*t;h.p={.5*std::cos(a),.5*std::sin(a),0};h.q=m::from_rotation_vector({0,0,a});return h;};
            fh.seed(hand(0),1,0,1e-9);old.seed(hand(0),1,1);
            std::uint64_t sample=1;double lastSampleT=0;p::Pose seen=hand(0);
            for(int n=1;n<=220;++n){
                const double t=n*(1./110);
                // physics step of the PREVIOUS command (one frame delay), gravity on
                dd.linear=pending;dd.angular=pendingW;dd.linear.z-=9.81/110;body.p=body.p+dd.linear*(1./110);
                if(const auto q=m::unit(m::multiply(m::from_rotation_vector(dd.angular*(1./110)),body.q)))body.q=*q;
                dd.centre=body.p;
                if(t-lastSampleT>=.0099){++sample;lastSampleT=t;seen=hand(t);}
                Command cc{};
                if(firm)cc=fh.update(seen,body,dd,sample,t,lastSampleT+1e-9);
                else cc=old.update(seen,body,dd,sample,std::uint64_t(1+std::floor(lastSampleT*1000/15.6)*15.6),std::uint64_t(1+std::floor(t*1000/15.6)*15.6),ol);
                assert(cc.decision!=Decision::End);
                if(cc.decision==Decision::Drive){pending=cc.linear;pendingW=cc.angular;}
                const double gap=m::length(hand(t).p-body.p)*100;
                if(n>40)worstCm=std::max(worstCm,gap);endCm=gap;
            }
        };
        double fw=0,fe=0,ow=0,oe=0;
        run(true,1.5,fw,fe);run(false,1.5,ow,oe);
        std::printf("R116 turn 180deg/s 1.5 kg: firm worst %.2f cm end %.2f cm | R103 servo worst %.2f cm end %.2f cm\n",fw,fe,ow,oe);
        assert(fw<2.0&&fe<2.0&&ow>fw);
        run(true,.2,fw,fe);assert(fw<2.0);
        // release = the hand velocity (throw), capped at 8 m/s
        FirmHoldR116 th;p::Pose hp{};th.seed(hp,1,0,1);Dynamics td{};td.mass=.5;
        for(int n=1;n<=10;++n){hp.p.x=.05*n;(void)th.update(hp,hp,td,std::uint64_t(n+1),n*.01,1+n*.01);}
        m::Vec3 lv{},av{};th.release(lv,av);assert(lv.x>4.5&&lv.x<=8.0000001);
        // leash and bad input still end the hold
        FirmHoldR116 le;le.seed({},1,0,1);p::Pose far{};far.p={.36,0,0};assert(le.update(far,{},td,2,.01,1.01).decision==Decision::End);
        td.mass=0;assert(le.update({},{},td,2,.01,1.01).decision==Decision::End);
    }
    // R118 jitter: hand moving steadily at 1 m/s, poses located every 11.1 ms (90 Hz headset), game frames
    // 7-12 ms random, physics one frame behind. R116 timed the velocity by when the game thread SAW the sample
    // (here: frame time) -> uneven prop steps; R118 uses the pose's own time -> steps as even as the hand's.
    {
        auto jerk=[&](bool poseClock){
            std::uint32_t seed=7;auto rnd=[&]{seed=seed*1664525u+1013904223u;return (seed>>8)/16777216.0;};
            FirmHoldR116 fh;Dynamics dd{};dd.mass=1;p::Pose body{};dd.centre=body.p;m::Vec3 pending{};
            fh.seed({},1,0,1e-9);std::uint64_t sample=1;double t=0,poseT=0,prevStep=-1,worst=0;
            for(int n=0;n<300;++n){const double fdt=.007+.005*rnd();t+=fdt;
                dd.linear=pending;dd.linear.z-=9.81*fdt;const auto before=body.p;body.p=body.p+dd.linear*fdt;dd.centre=body.p;
                // step length per second of game time: a smooth carry keeps it at ~1 m/s every frame
                const double speed=m::length(body.p-before)/fdt;if(n>60&&prevStep>=0)worst=std::max(worst,std::fabs(speed-1.0));prevStep=speed;
                const double newest=std::floor(t/.0111)*.0111;
                if(newest>poseT+1e-6){poseT=newest;++sample;}
                p::Pose hand{};hand.p={poseT*1.0,0,0};
                const auto c=fh.update(hand,body,dd,sample,t,poseClock?poseT+1e-9:t);
                assert(c.decision==Decision::Drive);pending=c.linear;}
            return worst;
        };
        const double r116=jerk(false),r118=jerk(true);
        std::printf("R118 jitter 1 m/s carry: worst frame speed error game-thread timing (R116) %.2f m/s, pose clock (R118) %.2f m/s\n",r116,r118);
        assert(r118<r116&&r118<.6);
    }
    // Original source age and immutable identity bound display; reads do not extend it.
    Presentation snap{};snap.character=1;snap.body=2;snap.bodyEpoch=3;snap.experimentEpoch=4;snap.key=5;
    snap.observationTick=1020;snap.sourceTick=1000;snap.sequence=6;snap.side=0;snap.wristWorld.p={1,2,3};presentation.publish(snap);
    p::Pose wrist{};assert(select(1,3,4,1050,0,wrist)&&wrist.p.x==1);
    assert(!select(1,3,4,1081,0,wrist)&&!select(1,3,5,1050,0,wrist)&&!select(1,4,4,1050,0,wrist)&&!select(1,3,4,1050,1,wrist));
    assert(!select(1,3,4,1050,0,wrist,17)); // legacy presentation cannot borrow instant ownership
    snap.instantTokenD78=17;presentation.publish(snap);
    assert(select(1,3,99,1050,0,wrist,17)); // F5 experiment epoch does not own this occurrence
    assert(!select(1,3,4,1050,0,wrist)&&!select(1,3,4,1050,0,wrist,18));
    assert(!select(1,4,4,1050,0,wrist,17)&&!select(1,3,4,1081,0,wrist,17));
    clearPresentation();assert(!select(1,3,4,1050,0,wrist));
    using namespace kcd2vr::diagnostics::capture168;
    gate.select(Domain::Combat,false,1000);gate.select(Domain::Interaction,false,1000);
    assert(!accept("R59.16C1 F5_PROP_BODY entity=1",1010));
    gate.select(Domain::Interaction,true,1000);assert(accept("R59.16C1 F5_PROP_BODY entity=1",1010));
    // Current capture contract keeps faults quiet too unless a recorder owns
    // the family. A fault is not an implicit permission to begin capture.
    gate.select(Domain::Interaction,false,1010);assert(!accept("R59.16C1 F5_PROP stage=BODY_DRIVE_FAULT",1010));
    std::puts("PASS prop servo: bounded motion/COM, freshness, duplicate, native-physics model, display ownership, F5 gate");
}
