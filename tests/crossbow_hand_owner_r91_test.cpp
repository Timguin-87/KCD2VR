#include "subsystems/ranged_reload/crossbow_hand_d89.h"
#include <cstdio>
namespace x=kcd2vr::crossbow_hand_d89;
namespace o=kcd2vr::crossbow_hand_r91;
static int fail=0,pass=0;
#define CHECK(v) do {if(v)++pass;else {++fail;std::printf("FAIL line %d: %s\n",__LINE__,#v);}}while(0)
int main(){
    const o::Owner owner{1,2,3,4,5,6,7,1};
    o::Admission ready{owner,100,true};
    auto claim=[&](const o::Admission& a,std::uint64_t now=101,bool enabled=true,bool fault=false,unsigned char A=0,unsigned char B=0,std::uintptr_t retained=0){
        return o::intercept(a,now,2,5,6,enabled,fault,A,B,retained);
    };
    CHECK(claim(ready));CHECK(claim(ready,180));CHECK(!claim(ready,181));CHECK(!claim(ready,99));
    CHECK(!claim({},101));CHECK(!claim(ready,101,false));CHECK(!claim(ready,101,true,true));
    CHECK(!claim(ready,101,true,false,2));CHECK(!claim(ready,101,true,false,0,1));
    auto withdrawn=ready;withdrawn.runnable=false;CHECK(!claim(withdrawn)); // menu/tracking withdrawal, no grace
    CHECK(!o::intercept(ready,101,2,50,6,true,false,0,0,0));
    CHECK(!o::intercept(ready,101,2,5,60,true,false,0,0,0));
    CHECK(!o::intercept(ready,101,22,5,6,true,false,0,0,0));
    CHECK(claim({},400,false,true,0,0,2)); // mode OFF/fault cannot reopen a partially changed transaction
    CHECK(!claim({},400,false,true,0,1,2)); // native immediate path remains native
    CHECK(!claim({},400,false,true,0,0,9)); // never quarantine another equipment

    x::Machine machine;o::Owner previous{};
    CHECK(x::resetForOwnerR91(machine,previous,owner));
    x::Input in{};in.context=true;in.leftHand=true;in.classId=10;in.cocked=0;in.handFree=true;
    in.geometry=true;in.rest={0,.23,.06};in.rail={0,.14,.06};in.hand=in.rest;in.grip=0;
    machine.update(in);in.grip=.9;machine.update(in);CHECK(machine.stage()==x::Stage::Span);
    CHECK(!x::resetForOwnerR91(machine,previous,owner));CHECK(machine.stage()==x::Stage::Span);
    auto changed=owner;changed.key=33;
    CHECK(x::resetForOwnerR91(machine,previous,changed));CHECK(machine.stage()==x::Stage::Idle);
    // The old pull cannot cock the replacement crossbow while Grip stays held.
    in.hand=in.rail;CHECK(machine.update(in).command==x::Command::None);
    const auto resetAndCheck=[&](o::Owner next){
        machine.reset();in.grip=0;in.hand=in.rest;machine.update(in);in.grip=.9;machine.update(in);
        CHECK(machine.stage()==x::Stage::Span);CHECK(x::resetForOwnerR91(machine,previous,next));CHECK(machine.stage()==x::Stage::Idle);
    };
    changed.epoch++;resetAndCheck(changed);changed.request++;resetAndCheck(changed);
    changed.generation++;resetAndCheck(changed);changed.ammo++;resetAndCheck(changed);
    changed.player++;resetAndCheck(changed);changed.side=0;resetAndCheck(changed);
    resetAndCheck({});CHECK(!previous.valid());
    machine.reset();in.handFree=false;in.grip=0;in.hand=in.rest;machine.update(in);in.grip=.9;
    CHECK(machine.update(in).note==x::Note::HandBusy);CHECK(machine.stage()==x::Stage::Idle);
    std::printf("crossbow hand owner: %d passed, %d failed\n",pass,fail);return fail?1:0;
}
