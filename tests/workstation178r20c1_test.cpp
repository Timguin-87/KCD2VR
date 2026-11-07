#include "vrik/workstation178r20c1.h"
#include <cassert>
#include <limits>
#include <cstdio>
using namespace kcd2vr::workstation178r20c1;
int main(){
    assert(alchemyPassenger42.load());
    {Controller p;std::array<Sample,2> h{};for(auto& a:h){a.valid=true;a.tick=1000;a.grip=1;}
     const auto r=p.update(Kind::Alchemy,10,1,h,1000);assert(!r.use&&!p.ownsGrip(0,1000));
     bool a=true,l=true,rbutton=true;float t=0;p.apply(1000,a,l,rbutton,t);assert(a&&l&&rbutton&&t==0);
     assert(nativeAlchemy42(1000)&&!nativeAlchemy42(1081));}
    alchemyPassenger42=false; // explicit legacy comparison fixtures below

    Controller c;std::array<Sample,2> h{};std::uint64_t now=1000;
    namespace cap=kcd2vr::diagnostics::capture168;
    kcd2vr::combat_rules178r10::f5Rung=2;cap::gate.select(cap::Domain::Interaction,true,now);setExperiment36(true);
    auto sample=[&](float right,float left,float y=1.f){now+=20;for(auto&s:h){s.valid=true;s.tick=now;++s.sequence;s.height=y;}h[0].grip=right;h[1].grip=left;};
    sample(1,1);auto r=c.update(Kind::Alchemy,42,-1,h,now);assert(r.changed&&!r.use);
    sample(1,1);assert(!c.update(Kind::Alchemy,42,-1,h,now).use); // no use from held-on-entry grip
    sample(0,0);assert(!c.update(Kind::Alchemy,42,-1,h,now).use);
    sample(.6f,0);assert(c.update(Kind::Alchemy,42,-1,h,now).use);
    bool a=false,lb=true,rb=true;float rt=.6f;c.apply(now,a,lb,rb,rt);
    assert(a&&lb&&rb&&rt==.6f); // C3: manual pad buttons preserved; owned Grip mapping filtered before Build
    sample(.5f,0);assert(c.update(Kind::Alchemy,42,-1,h,now).use); // hold hysteresis
    sample(.4f,0);assert(!c.update(Kind::Alchemy,42,-1,h,now).use);
    sample(0,.8f);assert(c.update(Kind::Alchemy,42,-1,h,now).use); // either hand
    h[1].valid=false;assert(!c.update(Kind::Alchemy,42,-1,h,now).use);
    sample(0,.8f);assert(!c.update(Kind::Alchemy,42,-1,h,now).use); // recovery requires neutral
    sample(0,0);c.update(Kind::Alchemy,42,-1,h,now);sample(0,.8f);assert(c.update(Kind::Alchemy,42,-1,h,now).use);
    assert(!c.update(Kind::Alchemy,43,-1,h,now).use); // new instance cannot inherit hold
    c.update(Kind::None,0,-1,h,now);a=false;lb=true;rb=true;rt=.7f;c.apply(now,a,lb,rb,rt);
    assert(!a&&lb&&rb&&rt==.7f); // menus, F5 OFF, no XI and ordinary controls unchanged
    sample(0,0,1.3f);c.update(Kind::Anvil,55,11,h,now);unsigned strokes=0;
    for(int i=1;i<=6;++i){sample(0,0,1.3f-.05f*i);strokes+=c.update(Kind::Anvil,55,11,h,now).stroke;}
    // Duplicate XR samples cannot create deceleration/impact.
    assert(!c.update(Kind::Anvil,55,11,h,now).stroke);
    sample(0,0,1.f);strokes+=c.update(Kind::Anvil,55,11,h,now).stroke;assert(strokes==1);
    a=false;lb=true;rb=true;rt=0;c.apply(now,a,lb,rb,rt);assert(rt==1&&!a&&lb&&rb);
    c.update(Kind::SmithingOther,55,8,h,now);rt=0;c.apply(now,a,lb,rb,rt);assert(rt==0); // no forge bellows pulse
    sample(0,0,1.f);c.update(Kind::Anvil,55,11,h,now);sample(0,0,.9f);c.update(Kind::Anvil,55,11,h,now);
    h[0].valid=false;c.update(Kind::Anvil,55,11,h,now);sample(0,0,.9f);
    assert(!c.update(Kind::Anvil,55,11,h,now).stroke); // tracking loss cancels the stroke
    sample(0,0);c.update(Kind::Alchemy,42,-1,h,now);sample(1,0);assert(c.update(Kind::Alchemy,42,-1,h,now).use);
    now+=81;assert(!c.update(Kind::Alchemy,42,-1,h,now).use); // stale poses release, never extend grace
    sample(0,0);c.update(Kind::Alchemy,42,-1,h,now);sample(1,0);assert(c.update(Kind::Alchemy,42,-1,h,now).use);
    setExperiment36(false);a=false;lb=rb=true;rt=0;c.apply(now,a,lb,rb,rt);
    assert(!a&&lb&&rb&&rt==0); // OFF cannot leak a cached hold before the next update
    setExperiment36(true);a=false;c.apply(now,a,lb,rb,rt);assert(!a); // stale generation
    assert(!c.update(Kind::Alchemy,42,-1,h,now).use); // same object requires neutral after re-arm
    sample(0,0);c.update(Kind::Alchemy,42,-1,h,now);sample(1,0);assert(c.update(Kind::Alchemy,42,-1,h,now).use);
    now=cap::gate.until[unsigned(cap::Domain::Interaction)].load();a=false;lb=rb=true;c.apply(now,a,lb,rb,rt);
    assert(a&&lb&&rb); // recorder expiry does not revoke gameplay or manual buttons
    assert(ownsWrist(now));
    setExperiment36(false);a=false;lb=rb=true;c.apply(now,a,lb,rb,rt);
    assert(!a&&lb&&rb); // explicit OFF still revokes immediately
    {   // R59.16C2 alchemy pointing hand: posture rule with hysteresis.
        using kcd2vr::workstation178r20c1::pointingHand59c2;
        double f[2]{.10,.10},d[2]{.40,.40};bool v[2]{true,true};
        assert(pointingHand59c2(-1,f,d,v)==-1);                 // hands lowered/near the body: head keeps the target
        f[0]=.30;assert(pointingHand59c2(-1,f,d,v)==0);          // right hand held out
        f[1]=.40;assert(pointingHand59c2(0,f,d,v)==0);           // current owner kept while still pointing
        f[0]=.20;assert(pointingHand59c2(0,f,d,v)==0);           // hysteresis band (>=.18)
        f[0]=.10;assert(pointingHand59c2(0,f,d,v)==1);           // lowered: the other pointing hand takes over
        d[1]=.90;assert(pointingHand59c2(1,f,d,v)==-1);          // a hand hanging far below the head never points
        d[1]=.40;v[1]=false;assert(pointingHand59c2(1,f,d,v)==-1); // lost tracking
        f[0]=std::numeric_limits<double>::quiet_NaN();v[1]=true;assert(pointingHand59c2(-1,f,d,v)==1);
    }
    std::puts("PASS workstation input: lifecycle, neutral entry/recovery, hysteresis, one native verb, anvil-only stroke, phase/loss cleanup and native control preservation");
}
