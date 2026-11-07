#include "../src/vrik/melee_stroke118.h"
#include <cassert>
#include <limits>
using namespace kcd2vr::melee118;
Input hit(bool stab=true){
    Input i{};i.dt=.04;i.eligible=i.known=i.blocked=true;i.contactEntity175=42;i.contactFraction175=.5;
    i.axis=i.modelAxis=i.modelForward={0,1,0};
    i.hand=i.modelHand=stab?Vec3{0,.16,0}:Vec3{.08,0,0};
    i.tip=i.modelTip=stab?Vec3{0,.16,0}:Vec3{.24,0,0};return i;
}
int main(){
    const Policy preview{true,true,true,true},comparison{true,true,true,false};
    for(bool stab:{false,true}){State s{};s.ready=true;const auto r=update(s,hit(stab),preview);
        assert(r.prepare&&r.commit&&!r.abort);assert(r.kind==(stab?Kind::Thrust:Kind::Cut));
        const auto pressure=update(s,hit(stab),preview);assert(!pressure.commit&&!pressure.prepare);
        State old{};old.ready=true;assert(!update(old,hit(stab),comparison).commit);
    }
    // World, unknown owner, zero travel, withdrawal, re-centre-sized jump and
    // non-finite fraction never gain contact commitment.
    for(int n=0;n<7;++n){State s{};s.ready=true;auto i=hit();
        if(n==0)i.contactEntity175=0;
        if(n==1)i.known=false;
        if(n==2)i.contactFraction175=0;
        if(n==3)i.hand=i.modelHand=i.tip=i.modelTip={0,-.16,0};
        if(n==4)i.hand=i.modelHand=i.tip=i.modelTip={0,4,0};
        if(n==5)i.contactFraction175=std::numeric_limits<double>::quiet_NaN();
        if(n==6)i.dt=.15;
        const auto r=update(s,i,preview);assert(!r.commit);
    }
    // One frame of tiny fast contact is insufficient, even when speed is high.
    State s{};s.ready=true;auto i=hit();i.dt=.01;i.hand=i.modelHand=i.tip=i.modelTip={0,.02,0};
    assert(!update(s,i,preview).commit);
    // Completed free-space thrust still commits without needing an NPC.
    s={};s.ready=true;i=hit();i.blocked=false;i.contactEntity175=0;
    assert(update(s,i,preview).commit);
}
