#include "../src/vrik/finisher_observation_c2.h"
#include "../src/vrik/contact_receipt58c1.h"
#include <cassert>
#include <cstdio>
#include <limits>
int main(){
    namespace f=kcd2vr::finisher_observation_c2;
    namespace c=kcd2vr::contact58c1;
    f::Input i{true,true,true,true,true,1,0,38,116,0,0,0,.8f,.6f,8,64};
    auto e=f::evaluate(i);
    assert(e.candidate&&e.postPhase32OrHigher&&!e.bodyMaterialDomainProven&&!e.severBoundaryProven);
    // A successful native death entering an owned/death phase is NOT silently
    // rejected by the old phase>=32 condition. This remains a log-only candidate.
    for(int phase:{-1,1,8,16,32,64,128,256}){i.victimPhaseAfter=phase;assert(f::evaluate(i).candidate);}
    i.death=0;i.kind=1;i.bodyMaterial=3;i.surface=144;i.normalizedEnergy=.2f;
    e=f::evaluate(i);
    assert((e.failures&f::NotLethal)&&(e.failures&f::NotCut)&&(e.failures&f::NotNeckCandidate)&&
        (e.failures&f::NotFleshSurfaceCandidate)&&(e.failures&f::EnergyInsufficient));
    // Seven VICTIM_ALIVE lines cannot hide every other reason any more.
    i={false,false,false,false,false,-1,-1,-1,-1,-1,-1,-1,0,.6f,-1,-1};
    e=f::evaluate(i);assert(e.failures==2047&&!e.candidate);
    i={true,true,true,true,true,1,0,38,116,0,0,0,.8f,.6f,8,64};
    i.normalizedEnergy=std::numeric_limits<float>::quiet_NaN();assert(f::evaluate(i).failures&f::EnergyInsufficient);
    i.normalizedEnergy=.8f;i.blocked=1;assert(f::evaluate(i).failures&f::DefendedOrUnknown);
    c::Receipt r{};r.serial=3;r.attempt=10;r.target=123;r.targetIdentity=0x1234;r.collider=0x5678;r.rawPart=4;
    r.seen=r.accepted=r.applied=r.fields=r.bodyContact=r.continuation=true;r.kind=1;
    r.blocked=r.perfect=r.parry=0;r.death=1;r.damage=8;r.tick=1000;r.impactTick=950;
    r.feature=int(kcd2vr::strike_feature_d41::Feature::Tip);r.weaponClass=4; // explicit proven piercing tip, not Unknown
    r.key=2;r.generation=4;r.epoch=6;r.request=8;
    const auto reason=[](const c::Receipt& x){return c::lethalThrustReasonC2(x,1010,2,4,6,8);};
    assert(reason(r)==c::LethalThrustRefusalC2::None&&c::lethalThrust(r,1010,2,4,6,8));
    // Keep the production eligibility predicate untouched. The new diagnostic
    // reason must agree with it for every old receipt-refusal fixture variation.
    for(int k=0;k<26;++k){auto x=r;
        switch(k){case 0:x.seen=false;break;case 1:x.accepted=false;break;case 2:x.applied=false;break;
        case 3:x.fields=false;break;case 4:x.target=0;break;case 5:x.kind=0;break;case 6:x.death=0;break;
        case 7:x.blocked=1;break;case 8:x.perfect=1;break;case 9:x.parry=1;break;case 10:x.damage=0;break;
        case 11:x.key=3;break;case 12:x.generation=5;break;case 13:x.epoch=7;break;case 14:x.request=9;break;
        case 15:x.tick=900;break;case 16:x.tick=1100;break;case 17:x.point[1]=std::numeric_limits<float>::quiet_NaN();break;
        case 18:x.bodyContact=false;break;case 19:x.continuation=false;break;
        case 20:x.attempt=0;break;case 21:x.targetIdentity=0;break;case 22:x.collider=0;break;case 23:x.rawPart=-1;break;case 24:x.feature=-1;break;case 25:x.weaponClass=5;break;}
        assert(reason(x)!=c::LethalThrustRefusalC2::None&&!c::lethalThrust(x,1010,2,4,6,8));
    }
    assert(c::lethalThrustReasonC2(r,1080,2,4,6,8)==c::LethalThrustRefusalC2::None);
    assert(c::lethalThrustReasonC2(r,1081,2,4,6,8)==c::LethalThrustRefusalC2::ReceiptAge);
    std::puts("finisher_observation_c2: multiple reasons, posthit phase annotation, no sever authority; old lethal predicate agreement PASS");
}
