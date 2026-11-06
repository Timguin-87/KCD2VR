#include "subsystems/alchemy/presentation_speed_d81.h"
#include "subsystems/alchemy/carafe_pour_d85.h"
#include "subsystems/alchemy/instant_native_d72.h"
#include <cstdio>
#include <limits>
namespace p=kcd2vr::subsystems::alchemy::presentation_d81;
namespace c=kcd2vr::subsystems::alchemy::carafe_d85;
namespace i=kcd2vr::subsystems::alchemy::instant_d72;
static int checks=0,failures=0;
static void check(bool ok,const char* why){++checks;if(!ok){++failures;std::printf("FAIL %s\n",why);}}
int main(){
    check(p::allowed(p::Kind::Transfer,true,false,false),"current verified pickup can accelerate");
    check(!p::allowed(p::Kind::Transfer,false,false,false),"stale hand cannot accelerate");
    check(!p::allowed(p::Kind::Transfer,true,true,false),"physical clock keeps ownership");
    check(!p::allowed(p::Kind::Book,true,false,false),"book and station transitions stay native");
    check(!p::allowed(p::Kind::Resource,true,false,false),"generic resource work stays native");
#ifndef LEGACY_SCOPE
    for(auto v:{0x3A5C308u,0x3A5C4C0u,0x3B87A38u,0x3A5CCD0u,0x3C23010u})
        check(p::transferKind(v)==p::Kind::Transfer,"exact transfer whitelist");
    for(auto v:{0x3F60940u,0x3A5C150u,0x3A5BB90u,0x3A5BD40u,0u,1u})
        check(p::transferKind(v)==p::Kind::Work,"bowl pot mortar book unknown excluded");
    check(!i::semanticOnlyAllowed(i::Action::PlaceBowl),"bowl authored child and FX cannot be omitted");
    check(i::semanticOnlyAllowed(i::Action::PourMortarD75),"existing mortar held-pair path preserved");
    check(i::semanticOnlyAllowed(i::Action::TakeObject),"instant object pickup preserved");
    check(i::semanticOnlyAllowed(i::Action::PlaceIngredient),"ingredient deposit preserved");
#endif
    check(std::fabs(p::rate(8.f,.02f,1.f)-4.f)<.0001f,"native update stays within80ms");
    check(p::rate(8.f,.2f,1.f)==1.f,"slow frame cannot skip native timeline");
    check(p::rate(std::numeric_limits<float>::quiet_NaN(),.01f,1.f)==1.f,"invalid multiplier");
    c::Observation o{};o.key={1,2,3,4,5};c::Controller ctrl;
    check(ctrl.step(o,8,100).rate==1.f,"unqualified pre-attach resource not accelerated");
    o.attached=true;check(ctrl.step(o,8,101).rate==1.f,"unqualified attached resource never paused");
    o.used=true;check(ctrl.step(o,8,102).rate==1.f,"unqualified work never paused");
    o={};o.key={1,2,3,4,5};o.tracked=o.control=true;o.bits=c::Valid|c::Grip|c::PotKnown|c::HomeKnown;
    ctrl.reset();auto d=ctrl.step(o,8,200);check(d.stage==c::Stage::Reach&&d.rate==8.f,"qualified carafe reach fast");
    o.attached=true;d=ctrl.step(o,8,201);check(d.stage==c::Stage::Hold&&d.rate==0.f,"held upright waits for actual tilt");
    o.bits|=c::Pour;d=ctrl.step(o,8,202);check(d.stage==c::Stage::Pour&&d.rate==1.f,"actual pour and FX at native speed");
    o.used=true;d=ctrl.step(o,8,203);check(d.stage==c::Stage::Carry&&d.rate==0.f,"used vessel waits for return");
    o.bits|=c::Home;d=ctrl.step(o,8,204);check(d.stage==c::Stage::Finish&&d.rate==1.f&&!o.detached,"post-Use home cannot accelerate an unqualified FX tail");
    o.bits&=~c::Home;d=ctrl.step(o,8,205);check(d.stage==c::Stage::Finish&&d.rate==1.f,"latched Finish cannot retain8x after home is lost");
    o.detached=true;d=ctrl.step(o,8,206);check(d.stage==c::Stage::Finish&&d.rate==1.f,"native detach finishes at1x without unproved FX acceleration");
    ctrl.reset();o.used=o.detached=false;o.bits=c::Valid|c::Grip|c::PotKnown|c::HomeKnown;
    (void)ctrl.step(o,8,300);o.bits&=~c::Grip;d=ctrl.step(o,8,301);check(d.rate==0.f,"short release grace preserves owner");
    d=ctrl.step(o,8,501);check(d.stage==c::Stage::Native&&d.rate==1.f,"release gives native1x not8x");
    d=ctrl.step(o,8,502);check(d.rate==1.f&&!d.announce,"loss is not repeatedly announced or accelerated");
    o.key.child=9;o.bits=c::Valid|c::Grip|c::PotKnown|c::HomeKnown;o.attached=false;
    d=ctrl.step(o,8,503);check(d.stage==c::Stage::Reach&&d.rate==8.f,"new child has independent state");
    o.key={};d=ctrl.step(o,8,504);check(d.rate==1.f&&d.stage==c::Stage::Off,"missing child clears state");
    std::printf("alchemy_scope_d85r5r1: %d/%d passed\n",checks-failures,checks);return failures?1:0;
}
