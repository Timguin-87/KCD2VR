// Pure XR posture/intent checks. Native action admission is exercised separately
// by native104_bridge_test; no headset or retail execution is claimed here.
#include "../src/vrik/fist_guard175.h"
#include <cstdio>
#include <limits>
namespace fg=kcd2vr::fist_guard175;
namespace fm=kcd2vr::fists;
static unsigned checks=0,failures=0;
static void check(bool value,const char* why){++checks;if(!value){++failures;std::printf("FAIL %s\n",why);}}
static fm::Sample sample(){
    fm::Sample x{};x.epoch=x.sequence=1;x.tick=1000;x.actor=123;
    x.valid=x.empty[0]=x.empty[1]=true;x.head={0,1.7,0};
    x.grip[0]=x.grip[1]=x.trigger[0]=x.trigger[1]=1;
    x.controller[0]=x.allowed[0]={.16,1.57,-.20};
    x.controller[1]=x.allowed[1]={-.16,1.57,-.20};return x;
}
static fg::Decision advance(fg::State& s,fm::Sample& x,unsigned ms=10,bool attack=false){
    ++x.sequence;x.tick+=ms;return fg::update(s,x,attack);
}
static bool settle(fg::State& s,fm::Sample& x){
    fg::update(s,x,false);fg::Decision d{};for(unsigned i=0;i<8;++i)d=advance(s,x);return d.block;
}
static bool contactPunch(bool fresh){
    fm::State s{};auto x=sample();x.grip[0]=x.grip[1]=x.trigger[0]=x.trigger[1]=0;x.contactStop175=true;
    fm::step(s,x);x.grip[0]=x.trigger[0]=1;
    for(unsigned i=0;i<20;++i){++x.sequence;x.tick+=10;fm::step(s,x);}
    for(unsigned i=0;i<2;++i){++x.sequence;x.tick+=10;x.controller[0].z-=.020;x.allowed[0].z-=.020;
        if(fm::step(s,x).valid)return false;}
    ++x.sequence;x.tick+=10;x.controller[0].z-=.020;x.blocked[0]=true;x.freshContact174[0]=fresh;
    const auto punch=fm::step(s,x);
    return punch.valid&&punch.travel<.10&&punch.travel>=.045&&!fm::step(s,x).valid;
}
int main(){
    kcd2vr::punch_travel_d65::minimumMeters.store(.045f); // explicit D64 reference; slider defaults covered separately
    fg::State s{};auto x=sample();
    auto openH11=x;openH11.trigger[0]=openH11.trigger[1]=0;
    check(!settle(s,openH11),"H11 Grip-only pointing cannot acquire boxing guard");s={};
    check(!fg::update(s,x,false).block,"raised hands require dwell before native block intent");
    for(unsigned i=0;i<7;++i)check(!advance(s,x).block,"sub-80ms posture cannot acquire");
    const auto entered=advance(s,x);check(entered.block&&entered.zone==0,"both raised hands select head guard after80ms");
    for(unsigned i=0;i<100;++i)check(fg::update(s,x,false).block,"same committed sample does not invent a new edge");
    check(!advance(s,x,10,true).block&&!s.guarding,"deliberate punch releases posture immediately");
    check(settle(s,x),"fresh settling can reacquire after attack");
    x.controller[0].y=1.05;check(!advance(s,x).block,"lowering either hand drops guard");
    x=sample();s={};x.controller[0].y=x.controller[1].y=1.40;
    check(settle(s,x)&&advance(s,x).zone==5,"both compact sternum fists select body guard, never face guard");
    x.empty[1]=false;check(!advance(s,x).block,"new opposite-hand item revokes fist-only guard");
    x=sample();s={};check(settle(s,x),"fresh owner can settle");
    ++x.epoch;check(!advance(s,x).block,"recenter epoch never borrows previous posture dwell");
    check(settle(s,x),"new epoch requires its own dwell");
    check(!advance(s,x,81).block,"stale tracking resets posture without extending a lease");
    x=sample();s={};check(settle(s,x),"posture baseline for translation test");
    const kcd2vr::pose_core::Vec3 shift{10,3,5};x.head=x.head+shift;
    for(auto& hand:x.controller)hand=hand+shift;
    check(advance(s,x).block,"common root translation retains identical head-relative guard");
    x.controller[0].x=std::numeric_limits<double>::quiet_NaN();
    check(!advance(s,x).block,"nonfinite tracking clears rather than requests native state");
    x=sample();s={};x.headOrientation={0,0,0,0};check(!settle(s,x),"invalid head orientation cannot guard");
    x=sample();s={};x.controller[0].z=x.controller[1].z=.15;
    check(!settle(s,x),"hands behind head cannot guard");
    check(contactPunch(true),"qualified contact stop preserves accepted F10 thresholds and one-punch latch");
    check(!contactPunch(false),"unknown collision cannot turn a stationary allowed fist into attack");
    for(float sensitivity:{.5f,1.f,1.5f}){
        kcd2vr::melee_tuning_c54::blockSensitivityH29.store(sensitivity);
        x=sample();s={};for(unsigned n=0;n<20;++n)advance(s,x);
        check(s.guarding,"both closed face-height fists can acquire at every sensitivity");
        x.controller[0].y=x.head.y-.25;
        check(!advance(s,x).block,"lowering either fist below face releases even existing guard");
    }
    kcd2vr::melee_tuning_c54::blockSensitivityH29.store(1.f);
    x=sample();s={};x.controller[0].y=x.controller[1].y=x.head.y-.19;
    check(settle(s,x)&&advance(s,x).zone==5,"face/chest gap is a body guard after normal dwell");
    x.controller[0].y=x.controller[1].y=x.head.y-.17;
    check(advance(s,x).block&&advance(s,x).zone==0,"moving across shared boundary preserves guard and face sector");
    x.controller[0].y=x.controller[1].y=x.head.y-.19;
    check(advance(s,x).block&&advance(s,x).zone==5,"return across boundary preserves body sector");
    x.controller[0].y=x.controller[1].y=x.head.y-.50;
    check(advance(s,x).block,"existing guard holds lower three-centimetre pad");
    s={};check(!settle(s,x),"lower hold pad cannot acquire a new guard");

    {   // R114: a leftover T24 claim (R113 knife draw) must not take the guard away in a live unarmed fight
        fg::State s{};auto x=sample();x.claimedT24[1]=true;x.responsiveCombatH27=true;
        check(settle(s,x),"R114 raised fists still block in combat while one hand has a leftover T24 claim");
        fg::State o{};auto y=sample();y.claimedT24[1]=true;y.responsiveCombatH27=false;
        check(!settle(o,y),"R114 outside combat a T24 claim still refuses the fist guard");
        fg::State c{};auto z=sample();z.claimedT24[0]=true;z.responsiveCombatH27=true;z.trigger[0]=.5f;
        check(!settle(c,z),"R114 combat claim exemption still needs both analogs above 60 percent");
    }
    std::printf("fist_guard175=%u/%u host_policy=ONLY\n",checks-failures,checks);return failures?1:0;
}
