#include "../src/vrik/melee_stroke118.h"
#include <cassert>
#include <cstdio>
#include <limits>
using namespace kcd2vr;
using namespace melee118;
Input sample(double speed,double modelSpeed) {
    Input i{};i.dt=.01;i.eligible=true;i.modelTip=i.modelHand={0,modelSpeed*.01,0};
    i.tip=i.hand={0,speed*.01,0};i.axis=i.modelAxis=i.forward=i.modelForward={0,1,0};i.outward=1;
    return i;
}
Policy legacy() {Policy p{};p.deliberate119=p.handRearm178=p.responsive44c2=p.forwardOnly19=true;p.stabSensitivityH29=.55;p.cutSensitivityH29=.55;p.intentSensitivityH29=.9;return p;}
bool commits(Policy p,Input i,int count=40) {State s{};s.ready=true;for(int n=0;n<count;++n)if(update(s,i,p).commit)return true;return false;}
int main() {
    auto old=legacy(),p=old;thrust_tuning_d67::apply(4,p);
    assert(!commits(old,sample(1.5,1.5))); // D66 recorded common sensitivity raises onset.
    assert(commits(p,sample(1.5,1.5)));
    assert(!commits(p,sample(1.2,1.2)));
    assert(!commits(p,sample(0,1.5))); // Whole-body forward translation is not a hand stroke.
    assert(!commits(p,sample(-2,-2))); // Pullback cannot become a forward thrust.
    auto slow=p;slow.thrustOnsetD67=2.5;assert(!commits(slow,sample(2,2)));
    auto shorter=p;shorter.thrustTravelD67=.10;assert(commits(shorter,sample(1.5,1.5),9));
    auto longer=p;longer.thrustTravelD67=.25;assert(!commits(longer,sample(1.5,1.5),9));
    for(int kind=0;kind<2;++kind) { // Cuts and reverse axial classification retain exact old decisions.
        State a{},b{};a.ready=b.ready=true;
        for(int n=0;n<80;++n){auto i=sample(kind?-3:0,kind?-3:0);if(!kind)i.hand=i.modelHand=i.tip=i.modelTip={.06,0,0};
            const auto x=update(a,i,old),y=update(b,i,p);
            assert(x.prepare==y.prepare&&x.commit==y.commit&&x.abort==y.abort&&x.kind==y.kind&&x.observation.reason==y.observation.reason);
        }
    }
    State changing{};changing.ready=true;for(int n=0;n<5;++n)update(changing,sample(1.5,1.5),p);
    p.thrustTravelD67=.06;auto changed=update(changing,sample(1.5,1.5),p);assert(!changed.commit&&!changing.active);
    auto v=thrust_tuning_d67::bounded({std::numeric_limits<float>::quiet_NaN(),-1});assert(v.speed==1.25f&&v.travel==.06f);
    thrust_tuning_d67::enabled.store(false);auto off=legacy();thrust_tuning_d67::apply(4,off);assert(!off.independentThrustD67&&!commits(off,sample(1.5,1.5)));
    std::puts("D67 thrust tuning: independent speed/travel, legacy OFF, cuts/pommel, forward-body and retraction checks PASS");
}
