#include "vrik/physical_weapon.h"
#include "vrik/weapon_sweep112.h"
#include <array>
#include <cstdio>
#include <cmath>
#include <vector>
#include "gesture_recovery_h30_fixture.h"
namespace ph=kcd2vr::physical;namespace m=kcd2vr::pose_core;
static unsigned checks=0,failures=0;
static void check(bool yes,const char* note){++checks;failures+=!yes;std::printf("%s %s\n",yes?"PASS":"FAIL",note);}
template<class T> static void target(T& in,std::int64_t time,std::uint64_t epoch=9){
    if constexpr(requires{in.poseTime[0];in.poseTimeEpoch;}){in.poseTime[0]=in.poseTime[1]=time;in.poseTimeEpoch=epoch;}}
struct Scene {
    ph::Input in{};ph::State state{};std::array<ph::Profile,2> profiles{};ph::Output out{};
    bool plane=false;unsigned actualPlanes=0;
    explicit Scene(int cls=7){auto&p=profiles[0];p.key=p.generation=1;p.entity=42;p.side=0;
        p.classId=cls;p.primary=p.geometry=true;p.weight=cls==7?9.6:cls==5?3.4:1.3;
        p.extent=cls==7?1.7:cls==5?1.1:.8;p.tip={0,p.extent,0};p.axis={0,1,0};profiles[1].side=1;
        in.allowed=true;in.epoch=in.bodyEpoch=1;in.tick=1000;in.timeNs=1000000000;in.sample=1;
        in.tracked[0]=in.tracked[1]=true;in.hands[0].p={.2,0,1.3};in.hands[1].p={.3,.5,1.3};
        in.shoulders[0]={.2,0,1.3};in.shoulders[1]={0,0,1.3};in.reach[0]=in.reach[1]=.65;
        in.grip[0]=in.grip[1]=1;in.dynamics=in.physicalCombat177=in.contact=true;in.attacks=false;
        in.controllerAnchor119=in.constraintDrive129=in.haveWorld=true;in.worldScale=1;
    }
    ph::Contact103 sweep(const ph::Profile&p,ph::Pose a,ph::Pose b,bool acquire){
        return ph::sweep_weapon112(p,a,b,acquire,[&](ph::Vec3 x,ph::Vec3 y,double margin){
            ph::Contact103 c{};const auto d=y-x;
            if(plane&&x.x>-.25&&y.x<=-.25){c.fraction=std::clamp((-.25-x.x+margin)/d.x,0.,1.);
                c.point=x+d*c.fraction;c.normal={1,0,0};c.plane=true;++actualPlanes;}
            return c;});
    }
    void frame(double angle,std::uint64_t ns,std::int64_t poseTime,std::uint64_t epoch=9){
        in.tick+=ns/1000000;in.timeNs+=ns;++in.sample;target(in,poseTime,epoch);
        in.hands[0].q=m::from_rotation_vector({0,0,angle});
        out=ph::step(state,in,profiles,[&](const ph::Profile&p,ph::Pose a,ph::Pose b,bool acquire){return sweep(p,a,b,acquire);});
    }
};
static bool equal(const ph::Output&a,const ph::Output&b){
    bool same=a.commandCount==b.commandCount;
    for(unsigned i=0;i<a.commandCount&&same;++i){const auto&x=a.commands[i];const auto&y=b.commands[i];
        same=x.kind==y.kind&&x.key==y.key&&x.generation==y.generation&&x.sample==y.sample&&x.tick==y.tick;}
    for(int n=0;n<2;++n)same=same&&a.owned[n]==b.owned[n]&&a.active[n]==b.active[n]&&
        m::length(a.weapons[n].p-b.weapons[n].p)<1e-10&&m::angle(a.weapons[n].q,b.weapons[n].q)<1e-7&&
        m::length(a.hands[n].p-b.hands[n].p)<1e-10&&a.support[n]==b.support[n]&&
        a.blocked[n]==b.blocked[n]&&a.sweepDeferred115[n]==b.sweepDeferred115[n]&&a.resetH29[n].reason==b.resetH29[n].reason;
    return same;
}
static void cadence(int cls,bool withPlane){Scene fixed(cls),vary(cls),old(cls);fixed.plane=vary.plane=old.plane=withPlane;
    bool equivalent=true,finite=true,originalClocks=true;double oldSpanLo=1000,oldSpanHi=-1000,newSpanLo=1000,newSpanHi=-1000;ph::Quat qo{},qn{};
    for(unsigned k=0;k<240;++k){const double angle=k<50?0:std::min(double(k-50)*.17453292519943295,withPlane?.8:100.);
        auto time=1000000000ll+std::int64_t(k)*11000000ll;const std::uint64_t ns=k%2?18000000:4000000;
        fixed.frame(angle,11000000,time);vary.frame(angle,ns,time);old.frame(angle,ns,0);
        if(k>60){equivalent=equivalent&&m::angle(fixed.out.weapons[0].q,vary.out.weapons[0].q)<1e-7&&m::length(fixed.out.weapons[0].p-vary.out.weapons[0].p)<1e-9;
            const double so=m::rotation_vector(m::multiply(old.out.weapons[0].q,m::conjugate(qo))).z;
            const double sn=m::rotation_vector(m::multiply(vary.out.weapons[0].q,m::conjugate(qn))).z;
            oldSpanLo=std::min(oldSpanLo,so);oldSpanHi=std::max(oldSpanHi,so);newSpanLo=std::min(newSpanLo,sn);newSpanHi=std::max(newSpanHi,sn);}
        qo=old.out.weapons[0].q;qn=vary.out.weapons[0].q;
        finite=finite&&ph::finite(vary.out.weapons[0])&&m::finite(vary.state.weapon[0].angularVelocity);
        originalClocks=originalClocks&&vary.state.weapon[0].lastTick==vary.in.tick&&vary.state.weapon[0].timeNs==vary.in.timeNs;
    }
    std::printf("CADENCE class=%d contactPlane=%d oldSpanDeg=%.9f newSpanDeg=%.9f actualPlaneQueries=%u\n",cls,withPlane,(oldSpanHi-oldSpanLo)*57.295779513,(newSpanHi-newSpanLo)*57.295779513,vary.actualPlanes);
    if(!withPlane)check(equivalent,"same pose-target sequence retains equal visual results across unequal arrival clocks");
    check(originalClocks,"physical state's occurrence/freshness clocks remain the original delivery clocks");
    check(finite,"visual dynamics and contact output remain finite");
    if(!withPlane)check(oldSpanHi-oldSpanLo>.05,"original unavailable-target-clock path reproduces nonuniform angular delivery");
    else check(vary.actualPlanes>0&&vary.out.contact119[0].plane,"real positive plane queries remain authoritative during target-time drive");
}
int main(){
    for(int cls:{4,5,7})for(bool plane:{false,true})cadence(cls,plane);
    for(int cls:{4,7}){Scene s(cls);const auto&p=s.profiles[0];
        const auto body=kcd2vr::dynamics177::body(p.classId,p.weight,p.extent);
        // A unit-radian/s rotation about a fixed grip with blade perpendicular
        // to the rotation axis has this closed-form kinetic energy. No prior
        // delivery-clock result is used as the expected value.
        const double energy=.5*(body.mass*body.com*body.com+body.transverse);
        const double reference=.5*body.gripInertia*12.*12.;
        const double expectedScale=std::sqrt(energy/reference)*kcd2vr::melee_tuning_c54::read(p.classId).sensitivityH29;
        double maxError=0,mean=0;unsigned samples=0;
        for(unsigned k=0;k<400;++k){const double theta=k<60?0:double(k-60)*.011;
            s.frame(theta,k%2?18000000:4000000,1000000000ll+std::int64_t(k)*11000000);
            if(k>200){ph::ImpactRecord178r30 impact{};impact.gesture59c3=1;impact.kind=0;
                ph::impact_effort19(impact,p,0,s.out.weapons[0],s.state.weapon[0].linearVelocity,s.state.weapon[0].angularVelocity,1.);
                maxError=std::max(maxError,std::abs(impact.effortScale19/expectedScale-1.));mean+=impact.effortScale19;++samples;}}
        std::printf("ANALYTIC_ENERGY class=%d omegaRadPerSec=1 expectedScale=%.17g actualMeanScale=%.17g maxRelativeError=%.9g\n",cls,expectedScale,mean/samples,maxError);
        check(maxError<.02,"settled source-target motion agrees with analytic rigid-body energy within2percent integration error");
    }
#ifndef KCD2VR_TARGET_CLOCK_PARENT
    namespace mt=kcd2vr::motion_time;
    check(mt::target_elapsed(100,100,9,9)<0,"repeated target time uses the unchanged delivery-time path");
    check(mt::target_elapsed(101,100,9,9)<0,"backward target time uses the unchanged delivery-time path");
    check(mt::target_elapsed(100,200,9,10)<0,"tracking epoch change cannot join two target clocks");
    check(mt::target_elapsed(0,200,9,9)<0&&mt::target_elapsed(100,200,0,0)<0,"missing target time or owner never invents an interval");
    check(mt::target_elapsed(1,100000003,9,9)<0,"discontinuous target clock preserves delivery timing");
#endif
    for(unsigned policy=0;policy<5;++policy){Scene a,b;bool same=true;
        for(unsigned k=0;k<110;++k){std::int64_t time=policy==0?0:policy==1?1000000000:policy==2?2000000000ll-std::int64_t(k)*11000000:policy==3?1000000000ll+std::int64_t(k)*200000000:1000000000ll+std::int64_t(k)*11000000;
            const auto ns=k%2?18000000ull:4000000ull;const double q=.2*std::sin(k*.2);
            a.frame(q,ns,0);b.frame(q,ns,time,policy==4?k+1:9);same=same&&equal(a.out,b.out);}
        check(same,"missing/repeated/backward/discontinuous/changed-owner clock preserves parent output and command clocks");}
    for(unsigned mixed=0;mixed<3;++mixed){Scene a,b;bool stable=true;
        for(unsigned k=0;k<200;++k){const auto ns=k%2?18000000ull:4000000ull;const double seconds=double(a.in.timeNs-1000000000)*1e-9;
            if(mixed==0){a.in.world.p=b.in.world.p={seconds*.2,0,0};}
            if(mixed==1){a.in.world.q=b.in.world.q=m::from_rotation_vector({0,0,seconds*.4});}
            if(mixed==2){a.in.shoulders[0].x=b.in.shoulders[0].x=.2+.02*std::sin(seconds*3);}
            const auto time=1000000000ll+std::int64_t(k)*11000000;a.frame(0,ns,0);b.frame(0,ns,time);
            stable=stable&&m::angle(a.out.weapons[0].q,b.out.weapons[0].q)<1e-7&&m::length(a.out.weapons[0].p-b.out.weapons[0].p)<1e-9;}
        check(stable,"stationary relative hand gains no artificial visual motion from current native root/shoulder changes");}
    {Scene s;auto&p=s.profiles[0];p.support=p.twoHanded=true;p.supportSource37=1;p.grip[1].p={0,.5,0};
        s.in.supportAxis59c16=s.in.supportConstraint130=true;bool held=true;
        for(unsigned k=0;k<230;++k){s.frame(0,k%2?18000000:4000000,1000000000ll+std::int64_t(k)*11000000);
            if(k>50)held=held&&s.state.weapon[0].support&&s.out.support[0]&&s.out.owned[1];}
        check(held,"real support grasp acquisition and retained secondary publication survive target-clock drive");
        auto cached=s.state;const auto result=ph::step(s.state,s.in,s.profiles,[](const ph::Profile&,ph::Pose,ph::Pose,bool){return 1.;});
        check(s.state.sample==cached.sample&&s.state.weapon[0].timeNs==cached.weapon[0].timeNs&&result.commandCount==0,"duplicate input sample keeps once-only command and state semantics");
    }
    {Scene s;s.in.requirePrimaryGrip114=s.in.releasePresentation45=true;s.in.intentTracked[0]=true;
        for(unsigned k=0;k<40;++k)s.frame(0,11000000,1000000000ll+std::int64_t(k)*11000000);
        Scene expected=s;const auto before=s.state.weapon[0].lastDesired;
#ifndef KCD2VR_TARGET_CLOCK_PARENT
        const auto beforeTarget=s.state.weapon[0].poseTime;
#endif
        bool retained=true;
        for(unsigned k=40;k<43;++k){s.in.grip[0]=0;s.frame(.1*double(k-39),11000000,1000000000ll+std::int64_t(k)*11000000);
            retained=retained&&m::angle(s.state.weapon[0].lastDesired.q,before.q)<1e-8;
#ifndef KCD2VR_TARGET_CLOCK_PARENT
            retained=retained&&s.state.weapon[0].poseTime==beforeTarget;
#endif
        }
        s.in.grip[0]=1;s.frame(.4,11000000,1000000000ll+43ll*11000000);
        expected.out=ph::step(expected.state,s.in,expected.profiles,[&](const ph::Profile&p,ph::Pose a,ph::Pose b,bool acquire){return expected.sweep(p,a,b,acquire);});
        check(retained,"released visual bridge does not advance the retained desired endpoint or its target clock");
        check(equal(s.out,expected.out),"regrip uses the full original per-weapon desired endpoint interval, not an unrelated intervening hand sample");
    }
    for(bool direct:{false,true}){RigH30 old(direct),current(direct);old.in.dynamics=current.in.dynamics=true;
        old.in.constraintDrive129=current.in.constraintDrive129=true;
        bool intent=true,commands=true,achievedEffectChanged=false,achievedEffectMapped=true;
        for(unsigned k=0;k<40;++k){target(current.in,1000000000ll+std::int64_t(k)*11000000);
            const ph::Vec3 delta=k<14?ph::Vec3{0,.025,0}:k<20?ph::Vec3{}:ph::Vec3{0,-.008,0};old.step(delta);current.step(delta);
            const auto&a=old.out;const auto&b=current.out;intent=intent&&a.guard117==b.guard117&&old.state.weapon[0].lastTick==current.state.weapon[0].lastTick&&
                old.state.weapon[0].timeNs==current.state.weapon[0].timeNs&&old.state.weapon[0].committedGestureH26.tick==current.state.weapon[0].committedGestureH26.tick&&
                old.state.weapon[0].gripRearm==current.state.weapon[0].gripRearm&&old.state.weapon[0].holdRearmH26==current.state.weapon[0].holdRearmH26&&old.state.weapon[0].quietSince==current.state.weapon[0].quietSince;
            commands=commands&&a.commandCount==b.commandCount;
            for(unsigned n=0;n<a.commandCount&&commands;++n){const auto&x=a.commands[n];const auto&y=b.commands[n];
                if(x.timingSpeed178r22c2!=y.timingSpeed178r22c2||x.effortScale19!=y.effortScale19){achievedEffectChanged=true;
                    std::printf("ENERGY_FIELD_DIFF direct=%d frame=%u kind=%u clocks=(%llu,%llu)/(%llu,%llu) gesture=%llu/%llu fraction=%.17g/%.17g timingSpeed=%.17g/%.17g effortScale=%.17g/%.17g effortMass=%.17g/%.17g support=%d/%d known=%d/%d\n",direct,k,unsigned(x.kind),(unsigned long long)x.tick,(unsigned long long)x.sample,(unsigned long long)y.tick,(unsigned long long)y.sample,(unsigned long long)x.gesture59c3,(unsigned long long)y.gesture59c3,x.fraction,y.fraction,x.timingSpeed178r22c2,y.timingSpeed178r22c2,x.effortScale19,y.effortScale19,x.effortMass19,y.effortMass19,x.effortSupport19,y.effortSupport19,x.effortKnown19,y.effortKnown19);
                }
                if(x.effortKnown19&&x.kind==ph::CommandKind::Slash)achievedEffectMapped=achievedEffectMapped&&x.effortScale19==a.strike178.scale&&y.effortScale19==b.strike178.scale&&a.strike178.accepted&&b.strike178.accepted;
                commands=x.kind==y.kind&&x.key==y.key&&x.generation==y.generation&&x.epoch==y.epoch&&x.bodyEpoch==y.bodyEpoch&&x.tick==y.tick&&x.sample==y.sample&&x.zone117==y.zone117&&x.gesture59c3==y.gesture59c3&&
                    x.fraction==y.fraction&&x.controlledHoldH26==y.controlledHoldH26&&x.effortMass19==y.effortMass19&&x.effortSupport19==y.effortSupport19&&x.effortKnown19==y.effortKnown19;}}
        check(old.hits()>0&&current.hits()>0&&old.count(ph::CommandKind::Prepare)>0&&current.count(ph::CommandKind::Prepare)>0,"actual raw Hybrid/Direct recognizer still emits native Prepare and release requests");
        check(intent&&commands,"raw guard/rearm/committed intent and native command identities keep original clocks");
#ifndef KCD2VR_TARGET_CLOCK_PARENT
        check(achievedEffectChanged&&achievedEffectMapped,"existing achieved-body energy producer reflects changed integration without changing native action/cost identity");
#endif
    }
    std::printf("physical-target-clock=%u/%u\n",checks-failures,checks);return failures?1:0;
}
