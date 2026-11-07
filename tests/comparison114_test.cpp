#include "vrik/physical_weapon.h"
#include "vrik/held_interaction114.h"
#include "vrik/interaction_ownership111.h"
#include "vrik/vr_combat_ownership111.h"
#include "diagnostics/lab_state.h"
#include <cassert>
#include <cstdio>
#include <limits>
using namespace kcd2vr;
int main(){
    using equipment114::Request;
    equipment114::Input i{};i.eligible=i.mountKnown=true;i.key=1;i.generation=i.epoch=1;i.tick=1000;
    equipment114::State s{};
    assert(equipment114::update(s,i)==Request::None);
    i.grip=1;i.tick+=10;assert(equipment114::update(s,i)==Request::None);
    i.hand.x=.30;i.tick+=80;assert(equipment114::update(s,i)==Request::Draw);
    assert(equipment114::update(s,i)==Request::None); // duplicate producer sample
    i.grip=0;i.tick+=20;assert(equipment114::update(s,i)==Request::None); // no native acknowledgement yet
    i.drawn=true;i.tick+=20;assert(equipment114::update(s,i)==Request::None); // acknowledgement does not auto-holster
    i.tick+=20;assert(equipment114::update(s,i)==Request::None);
    i.grip=1;i.hand.x=1;i.tick+=20;assert(equipment114::update(s,i)==Request::None);
    i.grip=0;i.tick+=20;assert(equipment114::update(s,i)==Request::None); // release away keeps item drawn
    i.mountKnown=false;i.grip=1;i.tick+=20;assert(equipment114::update(s,i)==Request::None);
    i.grip=0;i.tick+=20;assert(equipment114::update(s,i)==Request::None); // missing mount cannot invent a holster
    i.mountKnown=true;i.hand=i.mount;i.grip=1;i.tick+=20;assert(equipment114::update(s,i)==Request::None);
    i.tick+=60;assert(equipment114::update(s,i)==Request::None);
    i.grip=0;i.tick+=20;assert(equipment114::update(s,i)==Request::Holster); // deliberate return+dwell+release
    for(int n=0;n<101;++n){i.tick+=20;assert(equipment114::update(s,i)==Request::None);} // failed native request never storms
    i.tick+=20;assert(equipment114::update(s,i)==Request::None);
    i.hand=i.mount;i.mountKnown=true;i.grip=1;i.tick+=20;assert(equipment114::update(s,i)==Request::None);
    i.tick+=60;assert(equipment114::update(s,i)==Request::None);
    i.grip=0;i.tick+=20;assert(equipment114::update(s,i)==Request::Holster); // deliberate retry at mount
    i.eligible=false;i.tick+=20;assert(equipment114::update(s,i)==Request::None&&!s.pending);
    i.eligible=true;i.grip=std::numeric_limits<double>::quiet_NaN();i.tick+=20;
    assert(equipment114::update(s,i)==Request::None);

    // A timed-out DRAW may retry on the first deliberate release/press cycle.
    s={};i={};i.key=i.generation=i.epoch=1;i.tick=5000;i.eligible=i.mountKnown=true;
    equipment114::update(s,i);i.grip=1;i.tick+=10;equipment114::update(s,i);
    i.hand.x=.30;i.tick+=80;assert(equipment114::update(s,i)==Request::Draw);
    for(int n=0;n<101;++n){i.tick+=20;assert(equipment114::update(s,i)==Request::None);}
    i.hand.x=0;i.grip=0;i.tick+=10;equipment114::update(s,i);
    i.grip=1;i.tick+=10;equipment114::update(s,i);
    i.hand.x=.30;i.tick+=80;assert(equipment114::update(s,i)==Request::Draw);

    quiver114::State q{};std::uint64_t t=1000;
    assert(!quiver114::update(q,true,1,.05,t)); // entering mode already squeezing must not acquire
    assert(!quiver114::update(q,true,0,.05,t+=10));
    assert(!quiver114::update(q,true,1,.4,t+=10));
    assert(!quiver114::update(q,true,1,.05,t+=50)); // moving a held grip into the zone is not an edge
    assert(!quiver114::update(q,true,0,.05,t+=10));
    assert(!quiver114::update(q,true,1,.05,t+=10));
    assert(quiver114::update(q,true,1,.05,t+=40));
    assert(quiver114::update(q,true,1,.7,t+=10));
    quiver114::consume(q);
    assert(!quiver114::update(q,true,1,.05,t+=10)); // one ticket, one nock request
    assert(!quiver114::update(q,true,0,.05,t+=10));
    assert(!quiver114::update(q,true,1,.05,t+=10));
    assert(!quiver114::update(q,false,1,.05,t+=40)); // real tracking loss clears ticket
    assert(quiver114::chargeTarget(1,1)==1); // native charge-complete branch remains reachable
    assert(quiver114::chargeTarget(1,.4f)==.4f); // physical speed cannot skip native draw time

    physical::Input p{};p.allowed=p.tracked[0]=p.tracked[1]=true;p.sample=1;p.tick=1000;p.epoch=p.bodyEpoch=1;
    p.hands[0].p={.2,.3,1.3};p.shoulders[0]={.2,0,1.3};p.shoulders[1]={-.2,0,1.3};p.grip[0]=1;
    p.requirePrimaryGrip114=true;p.heldMotionAttacks=true;p.contact=false;p.dynamics=false;
    std::array<physical::Profile,2> profiles{};profiles[0].key=3;profiles[0].generation=1;profiles[0].classId=4;
    profiles[0].primary=profiles[0].geometry=true;profiles[0].tip={0,.8,0};profiles[1].side=1;
    physical::State st{};auto clear=[](auto&,auto,auto,bool){return 1.;};
    assert(physical::step(st,p,profiles,clear).active[0]);
    st.weapon[0].prepared=true;p.grip[0]=0;++p.sample;p.tick+=11;
    auto o=physical::step(st,p,profiles,clear);
    assert(!o.active[0]&&o.commandCount==1&&o.commands[0].kind==physical::CommandKind::Abort);

    physical_interaction106::Input h{};h.enabled=true;h.now_ms=1000;h.hand[0]={true,true,true,0,false};
    held_interaction114::State hs{};
    assert(!held_interaction114::update(hs,h).hold);
    h.hand[0].grip=1;h.now_ms+=10;assert(held_interaction114::update(hs,h).ray);
    h.now_ms+=20;assert(held_interaction114::update(hs,h).hold);
    for(int n=0;n<50;++n){h.now_ms+=20;assert(held_interaction114::update(hs,h).hold);} // beyond old 350ms pulse
    h.hand[0].tracked=false;h.now_ms+=10;assert(!held_interaction114::update(hs,h).hold);
    h.hand[0].tracked=true;h.now_ms+=10;assert(!held_interaction114::update(hs,h).hold); // no ghost repress
    h.hand[0].grip=0;h.now_ms+=10;held_interaction114::update(hs,h);
    h.hand[0].grip=1;h.now_ms+=10;held_interaction114::update(hs,h);
    h.menu=true;h.now_ms+=30;assert(!held_interaction114::update(hs,h).hold);
    assert(interaction_ownership111::Comparison114(3,3,false,true));
    assert(interaction_ownership111::Comparison114(1,3,false,true)); // F3/F5 domains are independent
    auto modes=diagnostics::lab::Change(0,diagnostics::lab::Key::F3,3);
    modes=diagnostics::lab::Change(modes,diagnostics::lab::Key::F5,3);
    assert(diagnostics::lab::Get(modes,diagnostics::lab::Key::F3)==3&&diagnostics::lab::Get(modes,diagnostics::lab::Key::F5)==3);
    assert(vr_combat111::SuppressEnqueue114(2)&&!vr_combat111::SuppressEnqueue114(3));
    std::puts("V114 gesture/ownership regressions passed; native calls and headset NOT executed");
}
