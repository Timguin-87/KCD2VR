#include "vrik/attack_timing178r22c2.h"
#include "vrik/user_modes175.h"
#include <cassert>
#include <limits>
using namespace kcd2vr::attack_timing178r22c2;
int main(){
    assert(multiplier(Kind::Slash,4.5)==1.f);
    assert(multiplier(Kind::Thrust,2.5)==1.f);
    assert(multiplier(Kind::Punch,.35)==.75f);
    assert(multiplier(Kind::Slash,30)==1.6f);
    for(auto kind:{Kind::Slash,Kind::Thrust,Kind::Punch}){
        float previous=0;
        for(double v=.35;v<=30;v+=.01){const auto rate=multiplier(kind,v);assert(rate>=previous&&rate>=.75f&&rate<=1.6f);previous=rate;}
        for(double invalid:{-1.,0.,31.,std::numeric_limits<double>::infinity(),std::numeric_limits<double>::quiet_NaN()})assert(multiplier(kind,invalid)==1.f);
    }
    assert(apply(.8f,1.5f)==1.2f); // native RPG factor survives
    assert(apply(.8f,20.f)==.8f);
    assert(apply(-1.f,1.5f)==-1.f);
    assert(std::isnan(apply(std::numeric_limits<float>::quiet_NaN(),1.2f)));
    Intent i{};i.player=1;i.actor=2;i.state=3;i.manager=4;i.serial=7;i.policy=2;i.epoch=3;i.sample=8;i.tick=1000;i.kind=Kind::Slash;i.speed=4.5;
    assert(eligible(i,1100,2,3,6));
    assert(!eligible(i,1101,2,3,6)); // short-lived, never refreshed by matching
    assert(!eligible(i,999,2,3,6));
    assert(!eligible(i,1000,3,3,6)); // cancellation
    assert(!eligible(i,1000,2,4,6)); // recenter / owner change
    assert(!eligible(i,1000,2,3,7)); // one native action only
    assert(!eligible(i,1000,2,3,8));
    i.side=2;assert(!eligible(i,1000,2,3,0));i.side=0;
    i.manager=0;assert(!eligible(i,1000,2,3,0));
    namespace u=kcd2vr::user175;
    const auto integrated=u::configuration(u::integrated(false));
    assert(integrated.pose==4&&integrated.weapon==7&&integrated.interaction==2&&integrated.combat==3);
    assert(integrated.experiment&&integrated.physical177&&integrated.full178r10&&integrated.rung178r15==1);
    const auto off=u::configuration(u::integrated(true));
    assert(!off.experiment&&!off.physical177&&!off.full178r10); // repeat F4 cancels experiment
}
