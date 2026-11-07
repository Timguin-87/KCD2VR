// Actual door Controller transport -> raw-only gap -> point return -> native
// acceptance join. A successful transport is the only supplied native leaf;
// no original unavailable pulse or native accepted action is fabricated.
#ifndef DOOR_GAP_CORE_HEADER
#define DOOR_GAP_CORE_HEADER "../src/core/door_use46.h"
#endif
#include DOOR_GAP_CORE_HEADER
#include <cstdio>
#include <cmath>
using namespace kcd2vr;
namespace d=door_use46;
static unsigned checks=0,failures=0;
static void check(bool ok,const char* text,int line){++checks;if(!ok){++failures;std::fprintf(stderr,"FAIL %d: %s\n",line,text);}}
#define CHECK(x) check(bool(x),#x,__LINE__)
static void reset(){
    d::enabled=true;d::settingsEpoch=1;d::nextPhysical62=0;d::claimedPhysical62=0;d::retiredPhysical62=0;
    d::nextToken=0;d::consumed=0;
    (void)d::intent.publish({});(void)d::acceptedIntent62.publish({});
    (void)d::physicalUse62.publish({});(void)d::currentInput62.publish({});
}
static d::Input input(unsigned mode=0,int side=0){
    d::Input i{};i.available=i.held=i.scoped54=true;i.side=side;i.now=i.poseTick=1000;i.press=900;
    i.poseEpoch=7;i.sourceGeneration=3;i.trackingEpoch=11;i.player=0x1234;
    i.experimentEpoch64=4;i.interactionMode64=mode;i.motion={.1f,1.2f,.3f};return i;
}
static d::Selection native(const d::Input& i,std::uint32_t target=42){
    return {true,target,i.side,i.now,i.poseTick,i.poseEpoch,i.sourceGeneration,i.player,{0,0,.08f},.08f};
}
static d::PhysicalUse62 transport(d::Controller& c,const d::Input& i,bool held=false){
    // A non-door native hover permits ordinary input transport. It does not
    // preselect the door that the native collector may accept later.
    auto hover=native(i,99);hover.ordinary=false;
    const auto p=c.update(i,hover);CHECK(!p.owns&&!p.pulse);
    c.transport62(i,!held,held);
    d::PhysicalUse62 original{};CHECK(d::physicalUse62.read(original));
    CHECK(original.serial&&original.input.available&&original.input.now==1000&&original.input.poseTick==1000);
    CHECK(original.input.press==i.press&&original.target==0);return original;
}
static d::Input gap(d::Input i,std::uint64_t now){
    i.now=i.poseTick=now;i.available=false;i.waitingForPoint65=true;return i;
}
int main(){
    for(unsigned mode:{0u,2u})for(int side=0;side<2;++side)for(bool held:{false,true}){
        reset();d::Controller c;auto i=input(mode,side);const auto original=transport(c,i,held);
        i=gap(i,1020);auto p=c.update(i,native(i,777));
        CHECK(p.owns&&!p.pulse&&!c.cancelled&&!c.transportRetired64);
        CHECK(d::retiredPhysical62.load()<original.serial&&d::nextPhysical62.load()==original.serial);
        d::AcceptedUse62 accepted{};CHECK(!d::captureAccepted62(native(i),i.now,accepted));
        d::Input unavailable{};CHECK(d::currentInput62.read(unavailable)&&!unavailable.available);
        c.transport62(i,false,false); // actual producer releases any generic held input on the gap
        d::PhysicalUse62 retained{};CHECK(d::physicalUse62.read(retained)&&retained.serial==original.serial);
        CHECK(retained.input.now==original.input.now&&retained.input.poseTick==original.input.poseTick&&
              retained.input.press==original.input.press&&retained.input.available);
        p=c.update(i,native(i,777));c.transport62(i,false,false); // re-entrant gap cannot mint another edge
        CHECK(p.owns&&!p.pulse&&d::nextPhysical62.load()==original.serial);
        i.now=i.poseTick=1040;i.available=true;i.waitingForPoint65=false;
        p=c.update(i,{});CHECK(p.owns&&!p.pulse); // suppress generic held/po.fire_use until native acceptance
        CHECK(d::captureAccepted62(native(i),i.now,accepted));
        d::Intent result{};CHECK(d::publishAccepted62(accepted,i.now,result));
        CHECK(result.target==42&&result.physicalUseSerial62==original.serial&&result.press==original.input.press);
        CHECK(!d::publishAccepted62(accepted,i.now,result));
        CHECK(d::nextPhysical62.load()==original.serial);
    }
    // Transaction age is not pose age: real complete points may refresh the
    // current source while the ORIGINAL native pulse remains immutable.
    {reset();d::Controller c;auto i=input();const auto original=transport(c,i);
     for(auto now:{1070u,1140u,1210u,1280u}){i.now=i.poseTick=now;c.update(i,{});}
     i=gap(i,1300);CHECK(c.update(i,{}).owns&&d::retiredPhysical62.load()<original.serial);
     i.now=i.poseTick=1320;i.available=true;i.waitingForPoint65=false;c.update(i,{});
     d::AcceptedUse62 a{};CHECK(d::captureAccepted62(native(i),i.now,a));
     CHECK(a.pulse.input.now==1000&&a.pulse.input.poseTick==1000&&a.pulse.serial==original.serial);}
    // Fresh raw-only frames cannot renew the last COMPLETE point's 80ms lease.
    {reset();d::Controller c;auto i=input();const auto original=transport(c,i);
     for(auto now:{1020u,1040u,1060u,1080u}){i=gap(i,now);auto p=c.update(i,{});
       CHECK(p.owns&&!p.pulse&&d::retiredPhysical62.load()<original.serial);}
     i=gap(i,1081);c.update(i,{});CHECK(d::retiredPhysical62.load()>=original.serial);
     i.now=i.poseTick=1090;i.available=true;i.waitingForPoint65=false;c.update(i,{});
     d::AcceptedUse62 a{};CHECK(!d::captureAccepted62(native(i),i.now,a));}
    // Reacquiring a complete point must not erase the elapsed gap first.
    for(auto restored:{1080u,1081u}){reset();d::Controller c;auto i=input();const auto original=transport(c,i);
     for(auto now:{1020u,1040u,1060u}){i=gap(i,now);CHECK(c.update(i,{}).owns);}
     i.now=i.poseTick=restored;i.available=true;i.waitingForPoint65=false;auto p=c.update(i,{});
     CHECK(p.owns&&!p.pulse);d::AcceptedUse62 a{};
     CHECK(d::captureAccepted62(native(i),i.now,a)==(restored==1080));
     CHECK((d::retiredPhysical62.load()>=original.serial)==(restored==1081));}
    for(int loss=0;loss<24;++loss){
        reset();d::Controller c;auto originalInput=input();const auto original=transport(c,originalInput);
        auto i=gap(originalInput,1020);
        if(loss==0)i.held=false;if(loss==1)i.scoped54=false;
        if(loss==2)i.trackingEpoch=0;if(loss==3)++i.trackingEpoch;
        if(loss==4)++i.player;if(loss==5)++i.poseEpoch;if(loss==6)++i.sourceGeneration;
        if(loss==7)++i.experimentEpoch64;if(loss==8)i.interactionMode64=3;
        if(loss==9)i.poseTick=939;if(loss==10)i.now=999;if(loss==11)i.now=i.poseTick=1081;
        if(loss==12)i.poseTick=1021;if(loss==13)++d::settingsEpoch;
        if(loss==14)d::enabled=false;if(loss==15)d::retirePhysical62(original.serial);
        if(loss==16)d::claimedPhysical62=original.serial;if(loss==17)i.motion.x=std::nanf("");
        if(loss==18)i.waitingForPoint65=false;if(loss==19)(void)d::physicalUse62.publish({});
        if(loss==20)i.side=1;if(loss==21)++i.press;if(loss==22)i.now=0;
        if(loss==23)i.poseTick=999; // current raw clock can be fresh while last full point already expired below
        if(loss==23)i.now=1080; // 81ms-old raw pose must fail even at exact complete-point boundary
        auto p=c.update(i,{});CHECK(!p.pulse&&d::retiredPhysical62.load()>=original.serial);
        d::enabled=true;i=originalInput;i.now=i.poseTick=1040;i.available=true;c.update(i,{});
        d::AcceptedUse62 a{};CHECK(!d::captureAccepted62(native(i),i.now,a));
    }
    // No valid initial transport, no last complete point, or an already
    // accepted/claimed pulse cannot acquire the transient-gap exception.
    {reset();d::Controller c;auto i=gap(input(),1020);auto p=c.update(i,{});c.transport62(i,true,false);
     CHECK(!p.pulse&&!c.transportedSerial62);d::PhysicalUse62 pulse{};
     CHECK(d::physicalUse62.read(pulse)&&!pulse.serial);}
    {reset();d::Controller c;auto i=input();c.transport62(i,true,false);const auto serial=c.transportedSerial62;
     i=gap(i,1020);c.update(i,{});CHECK(serial&&d::retiredPhysical62.load()>=serial);}
    // Normal direct-door tokens have physicalUseSerial62=0. Withdrawing only
    // the acceptance mailbox would leave read/current/consume accidentally live.
    {reset();d::Controller c;auto i=input();auto s=native(i);auto p=c.update(i,s);CHECK(p.pulse);
     c.complete(p,true);c.transport62(i,true,false);d::Intent before{};CHECK(d::read(before,1000));
     CHECK(!before.physicalUseSerial62);i=gap(i,1020);p=c.update(i,{});
     d::Intent unavailable{};CHECK(p.owns&&!p.pulse&&!d::read(unavailable,i.now));
     CHECK(!d::current(before,i.now)&&!d::consume(before,i.now));
     CHECK(c.chosen.token==before.token&&c.chosen.tick==1000&&c.chosen.poseTick==1000);
     i.now=i.poseTick=1040;i.available=true;i.waitingForPoint65=false;p=c.update(i,{});
     CHECK(p.owns&&!p.pulse&&d::read(unavailable,i.now)&&unavailable.token==before.token);
     CHECK(d::nextPhysical62.load()==c.transportedSerial62);
     (void)d::intent.publish({});d::AcceptedUse62 a{};
     CHECK(!d::captureAccepted62(native(i,43),i.now,a));
     CHECK(d::captureAccepted62(native(i,42),i.now,a));}
    // Withdrawing this controller's pending token must not clear another owner.
    {reset();d::Controller c;auto i=input();auto p=c.update(i,native(i));c.complete(p,true);c.transport62(i,true,false);
     auto other=c.chosen;other.token+=99;other.target=99;(void)d::intent.publish(other);
     i=gap(i,1020);c.update(i,{});d::Intent retained{};
     CHECK(d::intent.read(retained)&&retained.token==other.token&&retained.target==99);}
    {reset();d::Controller c;auto i=input();auto p=c.update(i,native(i));c.complete(p,true);c.transport62(i,true,false);
     CHECK(d::consume(c.chosen,1000));i=gap(i,1020);c.update(i,{});
     CHECK(c.cancelled&&d::retiredPhysical62.load()>=c.transportedSerial62);}
    std::printf("door_gap_h31: %u/%u PASS; actual Controller transport, immutable pulse, bounded point-gap, native acceptance join\n",checks-failures,checks);
    return failures?1:0;
}
