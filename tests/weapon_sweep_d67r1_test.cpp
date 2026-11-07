// Actual production sweep, synthetic geometry/native-query leaves. This proves
// the D67 lost-quarter-probe regression and resource policy, not headset meshes.
#include "vrik/weapon_sweep112.h"
#include "vrik/contact_budget.h"
#include <cstdio>
using namespace kcd2vr::physical;
namespace reason=kcd2vr::sweep169r2;
static unsigned checks=0,failed=0;
static void check(bool value,const char* label){++checks;if(!value)++failed;std::printf("%s %s\n",value?"PASS":"FAIL",label);}
static constexpr double deg=3.14159265358979323846/180.;
static Profile sword(){Profile p{};p.side=0;p.classId=4;p.primary=p.geometry=true;p.tip={0,.915,0};return p;}
static Pose turn(double degrees){Pose p{};p.q=m::from_rotation_vector({0,0,degrees*deg});return p;}
int main(){
    kcd2vr::held_motion_d67::setCurrentArcCoverage(true);g_sweep_coverage_scale178r6=1.;
    auto p=sword();const auto to=turn(42.8077);
    const Vec3 center=m::rotate(turn(10).q,{0,.915*.25,0});unsigned calls=0,hits=0;
    const auto obstruction=sweep_weapon112(p,Pose{},to,false,[&](Vec3 from,Vec3 end,double margin){
        ++calls;Contact103 h{};const auto d=end-from,f=from-center;
        // Zero-width native ray first, then exactly the adapter's along-ray
        // stop margin. No inflated sphere/conservative-volume claim.
        const double aa=m::dot(d,d),bb=2*m::dot(f,d),cc=m::dot(f,f)-.005*.005;
        if(aa<=1e-18)return h;const double disc=bb*bb-4*aa*cc;if(disc<0)return h;
        const double t=(-bb-std::sqrt(disc))/(2*aa);
        if(t>=0&&t<=1){++hits;h.fraction=std::clamp(t-margin/std::sqrt(aa),0.,1.);h.plane=true;
            h.point=from+d*t;h.normal=m::unit(h.point-center).value_or(Vec3{1,0,0});}return h;
    });
    check(obstruction.known&&obstruction.plane&&obstruction.fraction<1&&hits>0,
          "D67 regression: current fast arc retains the quarter-shaft10mm obstacle hit");
    std::printf("COUNTEREXAMPLE fraction=%.9f rays=%u hits=%u reason=%u\n",obstruction.fraction,calls,hits,unsigned(obstruction.coverage169r2.reason));
    std::array<unsigned,5> features{};calls=0;
    const auto clear=sweep_weapon112(p,Pose{},to,false,[&](Vec3 from,Vec3,double){
        ++calls;const auto feature=unsigned(std::lround(m::length(from)/.915*4));
        if(feature<5)++features[feature];return Contact103{};});
    check(clear.known&&!clear.limited115&&clear.fraction==1&&clear.coverage169r2.reason==reason::Reason::FullClear,
          "recorded42.8077deg old-physical-to-desired arc completes immediately in clear space");
    check(calls==14&&features==std::array<unsigned,5>{1,2,3,4,4},
          "own-radius subdivisions retain grip quarter mid three-quarter and tip");
    for(double angle:{64.,80.}){calls=0;
        const auto nativeReturn=sweep_weapon112(p,Pose{},turn(angle),false,[&](Vec3,Vec3,double){++calls;return Contact103{};});
        check(nativeReturn.known&&!nativeReturn.limited115&&nativeReturn.fraction==1&&calls<=18,
              "large clear native-return arc retains all features under18rays");
    }
    for(double angle:{100.,145.,180.}){calls=0;
        const auto high=sweep_weapon112(p,Pose{},turn(angle),false,[&](Vec3,Vec3,double){++calls;return Contact103{};});
        check(high.known&&high.limited115&&high.fraction>0&&high.fraction<1&&calls<=18&&
              high.coverage169r2.reason==reason::Reason::AngularPrefix&&high.coverage169r2.checkedAngle<100*deg,
              "high angle retains bounded checked prefix instead of dropping spatial features");
    }
    for(auto failure:{reason::Reason::QueryBusy,reason::Reason::CallFault,reason::Reason::TimeBudget,reason::Reason::RayCap}){
        calls=0;const auto incomplete=sweep_weapon112(p,Pose{},to,false,[&](Vec3,Vec3,double){Contact103 h{};
            if(++calls==7){h.known=false;h.fraction=0;h.coverage169r2.reason=failure;}return h;});
        check(incomplete.known&&incomplete.limited115&&std::abs(incomplete.fraction-.25)<1e-9&&
              incomplete.coverage169r2.completed==1&&incomplete.coverage169r2.completedRays==6&&
              incomplete.coverage169r2.reason==failure,"incomplete segment admits only fully checked earlier segment");
    }
    {kcd2vr::contact::Budget budget;budget.begin_work();budget.remaining=13;unsigned attempted=0;
        const auto capped=sweep_weapon112(p,Pose{},to,false,[&](Vec3,Vec3,double){Contact103 h{};++attempted;
            if(!budget.take(attempted)){h.known=false;h.fraction=0;h.coverage169r2.reason=reason::Reason::RayCap;}
            else budget.finish(attempted);return h;});
        check(budget.used==13&&attempted==14&&capped.fraction<1&&capped.limited115,
              "shared remaining credits cannot fund an incomplete last segment");}
    {kcd2vr::contact::Budget budget;budget.begin_work();std::uint64_t us=0;unsigned executed=0;
        const auto timed=sweep_weapon112(p,Pose{},to,false,[&](Vec3,Vec3,double){Contact103 h{};
            if(!budget.take(us)){h.known=false;h.fraction=0;h.coverage169r2.reason=reason::Reason::TimeBudget;}
            else{++executed;us+=250;budget.finish(us);}return h;});
        check(executed==4&&budget.spent==1000&&!timed.known&&timed.fraction==0,
              "same1000us work budget stops before an incomplete first segment can move");}
    {auto initial=p;initial.initialAcquireSweepD44=true;kcd2vr::contact::Budget budget;budget.begin_work();
        kcd2vr::contact::AcquisitionBurstD45 burst(budget,true);calls=0;
        const auto c=sweep_weapon112(initial,Pose{},turn(180),false,[&](Vec3,Vec3,double){Contact103 h{};
            if(!budget.take(calls)){h.known=false;h.fraction=0;h.coverage169r2.reason=reason::Reason::RayCap;}
            else{++calls;budget.finish(calls);}return h;});
        check(c.known&&calls<=72&&budget.used==calls&&c.fraction==1&&!c.limited115,
              "initial binding retains fine chord and all five features within unchanged72ray burst");}
    {auto longInitial=p;longInitial.initialAcquireSweepD44=true;longInitial.tip={0,3,0};calls=0;
        const auto c=sweep_weapon112(longInitial,Pose{},turn(180),false,[&](Vec3,Vec3,double){++calls;return Contact103{};});
        check(c.known&&c.limited115&&c.fraction<1&&calls<=72,
              "very long initial shape keeps a checked prefix when all five features cannot fit72rays");}
    for(unsigned failureAt=1;failureAt<=14;++failureAt){calls=0;std::array<double,5> covered{};
        const auto limited=sweep_weapon112(p,Pose{},to,false,[&](Vec3 from,Vec3 end,double){
            Contact103 h{};const auto feature=unsigned(std::lround(m::length(from)/.915*4));
            if(++calls==failureAt){h.known=false;h.fraction=0;h.coverage169r2.reason=reason::Reason::QueryBusy;return h;}
            if(feature<5)covered[feature]=feature?std::atan2(-end.x,end.y)/(42.8077*deg):1.;return h;});
        const auto common=*std::min_element(covered.begin(),covered.end());
        check(limited.fraction<=common+1e-10&&limited.fraction<1,
              "unknown at every query index admits only common coverage across ALL five features");
    }
    {calls=0;const auto uncertainHit=sweep_weapon112(p,Pose{},to,false,[&](Vec3,Vec3,double){Contact103 h{};
        if(++calls==2){h.plane=true;h.fraction=.5;h.normal={1,0,0};}
        if(calls==5){h.known=false;h.fraction=0;h.coverage169r2.reason=reason::Reason::QueryBusy;}return h;});
        check(!uncertainHit.known&&!uncertainHit.plane&&uncertainHit.fraction==0,
              "a shaft hit ahead cannot authorize unqueried tip travel after a later unknown");}
    {auto shield=p;shield.classId=8;shield.diagnosticBounds59c6=true;
        shield.diagnosticBoundsMin59c6={-.35,-.025,-.45};shield.diagnosticBoundsMax59c6={.35,.025,.45};
        std::array<Vec3,5> seen{};calls=0;
        const auto c=sweep_weapon112(shield,Pose{},turn(5),false,[&](Vec3 from,Vec3,double){if(calls<5)seen[calls]=from;++calls;return Contact103{};});
        check(c.known&&calls==5&&m::length(seen[1]-seen[2])>.89&&m::length(seen[3]-seen[4])>.69,
              "shield retains centre and all four lateral rim features");}
    {auto support=p;support.grip[1].p={0,.3,0};calls=0;
        const auto c=sweep_weapon112(support,Pose{},turn(5),true,[&](Vec3 from,Vec3,double){++calls;check(std::abs(m::length(from)-.3)<1e-9,"support query follows its exact opposite-hand grip");return Contact103{};});
        check(c.known&&calls==1,"support acquisition keeps its independent one-feature policy");}
    {auto invalid=p;invalid.tip.x=std::numeric_limits<double>::quiet_NaN();calls=0;
        const auto c=sweep_weapon112(invalid,Pose{},to,false,[&](Vec3,Vec3,double){++calls;return Contact103{};});
        check(!c.known&&c.fraction==0&&!calls,"invalid shape remains unknown before query");}
    kcd2vr::held_motion_d67::setCurrentArcCoverage(false);calls=0;
    const auto legacy=sweep_weapon112(p,Pose{},to,false,[&](Vec3,Vec3,double){++calls;return Contact103{};});
    check(legacy.known&&legacy.limited115&&legacy.fraction<.8&&calls==15,
          "current-arc option off retains the fine three-segment comparison path");
    std::printf("D67R1 weapon sweep %u/%u\n",checks-failed,checks);return failed?1:0;
}
