#include "vrik/physical_weapon.h"
#include "vrik/vr_combat_ownership111.h"
#include "vrik/attack_xi178r8.h"
#include <cassert>
using namespace kcd2vr::physical;
int main(){
    // Same-item collision recovery must retain the already-qualified local
    // support grasp. It must not reacquire from the newly animated native pose.
    {
        Input i{};i.allowed=i.contact=i.controllerAnchor119=i.physicalCombat177=true;
        i.tracked[0]=i.tracked[1]=i.intentTracked[0]=i.intentTracked[1]=true;
        i.epoch=i.bodyEpoch=1;i.grip[0]=1;i.reach[0]=i.reach[1]=1;
        i.shoulders[0]={.2,0,0};i.shoulders[1]={-.2,0,0};
        i.hands[0].p={.1,.3,0};i.hands[1].p={-.05,.3,0};
        std::array<Profile,2> ps{};auto& p=ps[0];p.key=p.generation=3;p.classId=4;
        p.primary=p.support=p.twoHanded=p.geometry=true;p.grip[1].p={-.15,0,0};p.tip={0,.6,0};p.axis={0,1,0};
        State st{};bool unknown=false;unsigned acquires=0;
        auto next=[&](){++i.sample;i.tick=1000+i.sample*10;i.timeNs=i.tick*1000000;
            return step(st,i,ps,[&](const Profile&,Pose,Pose,bool ac){acquires+=ac;Contact103 c{};c.known=!unknown;c.fraction=unknown?0:1;return c;});};
        next();i.grip[1]=1;Output o{};for(int j=0;j<8;++j)o=next();
        assert(o.support[0]);const auto binding=st.weapon[0].supportGrip169r2;const auto count=acquires;
        unknown=true;i.contactSuspended59c1=true;
        for(int j=0;j<12;++j){o=next();assert(o.handlingOnly55c1[0]&&o.support[0]&&o.owned[1]);
            assert(!attack_authority55c1(i,o,0)&&!o.commandCount);
            assert(m::length(st.weapon[0].supportGrip169r2.p-binding.p)<1e-9);}
        unknown=false;i.contactSuspended59c1=false;o=next();
        assert(o.support[0]&&!o.handlingOnly55c1[0]&&acquires==count);
        // Explicit release/rejected reach may not auto-regrab through reset.
        i.grip[1]=0;unknown=true;i.contactSuspended59c1=true;o=next();assert(!o.support[0]);
        i.grip[1]=1;for(int j=0;j<8;++j)o=next();assert(!o.support[0]);
        unknown=false;i.contactSuspended59c1=false;for(int j=0;j<8;++j)o=next();assert(o.support[0]);
        i.tracked[1]=i.intentTracked[1]=false;unknown=true;i.contactSuspended59c1=true;o=next();assert(!o.support[0]&&!o.owned[1]);
        i.tracked[1]=i.intentTracked[1]=true;for(int j=0;j<6;++j)o=next();assert(!o.support[0]);
    }
    namespace v=kcd2vr::vr_combat111;
    assert(v::ClinchAlias55c1("combat_clinch2_sz2_clinchguard_swrds_master"));
    assert(!v::ClinchAlias55c1("combat_clinch2_sz2_clinchguard_swrds_slave"));
    assert(!v::ClinchAlias55c1("combat_clinch2_sz2_clinchkill_swrds_master"));
    v::clinchInputTick55c1=1000;
    assert(v::ClinchInput55c1(1080)&&!v::ClinchInput55c1(1081)&&!v::ClinchInput55c1(999));
    namespace ax=kcd2vr::attack_xi178r8;
    ax::reset();float rt=0,lt=0;bool a=false;
    ax::translate(3,1,1000);ax::apply(1000,rt,lt,a);assert(lt==1);
    ax::translate(3,2,1010);ax::translate(3,1,1010);
    for(int i=0;i<10;++i){lt=0;ax::apply(1010+i,rt,lt,a);assert(lt==0);}
    ax::ackLtUp55c1();lt=0;ax::apply(1020,rt,lt,a);assert(lt==1);ax::reset();
    Input in{};in.allowed=in.contact=in.physicalCombat177=in.controllerAnchor119=true;
    in.tracked[0]=in.intentTracked[0]=true;in.grip[0]=1;in.reach[0]=.7;
    in.epoch=in.bodyEpoch=1;in.sample=1;in.tick=1000;in.timeNs=1000000000;
    in.hands[0].p={.2,.3,0};in.intentHands[0]=in.hands[0];
    std::array<Profile,2> p{};p[0].key=p[0].generation=1;p[0].primary=p[0].nativeSeed=p[0].geometry=true;
    p[0].classId=4;p[0].tip={0,.6,0};p[0].axis={0,1,0};p[0].nativeWeapon.p={3,0,0};
    State s{};bool clear=false;
    auto sweep=[&](const Profile&,Pose,Pose,bool){Contact103 c{};c.known=clear;c.fraction=clear?1:0;return c;};
    auto o=step(s,in,p,sweep);
    assert(o.releaseReason121[0]==5&&o.handlingOnly55c1[0]&&o.active[0]&&o.owned[0]);
    in.attacks=true;assert(!attack_authority55c1(in,o,0)); // visible fallback cannot consume an old contact
    assert(m::length(o.hands[0].p-in.hands[0].p)<1e-9&&o.commandCount==0&&!o.impactRecord178r30[0].valid);
    in.sample++;in.tick+=10;in.timeNs+=10000000;in.hands[0].p.x+=.05;
    o=step(s,in,p,sweep);assert(o.handlingOnly55c1[0]&&m::length(o.hands[0].p-in.hands[0].p)<1e-9);
    clear=true;in.sample++;in.tick+=10;in.timeNs+=10000000;
    o=step(s,in,p,sweep);assert(!o.handlingOnly55c1[0]&&o.active[0]&&o.commandCount==0);
    assert(attack_authority55c1(in,o,0));
    // A single ordinary query-budget miss retains checked collision ownership.
    // It must not dispatch a hit; native suspension switches to display-only.
    clear=false;in.sample++;in.tick+=10;in.timeNs+=10000000;
    o=step(s,in,p,sweep);assert(!o.handlingOnly55c1[0]);
    for(unsigned i=0;i<o.commandCount;++i)assert(o.commands[i].kind!=CommandKind::Slash&&o.commands[i].kind!=CommandKind::Thrust);
    in.contactSuspended59c1=true;in.sample++;in.tick+=10;in.timeNs+=10000000;
    o=step(s,in,p,sweep);assert(o.handlingOnly55c1[0]&&!attack_authority55c1(in,o,0));
    in.contactSuspended59c1=false;
    // A reachable native shoulder seed used to remain visible indefinitely.
    State shoulder{};p[0].nativeWeapon.p={-.2,0,.2};clear=false;
    auto held=step(shoulder,in,p,sweep);
    assert(held.handlingOnly55c1[0]&&m::length(held.hands[0].p-in.hands[0].p)<1e-9);
    const auto seed=shoulder.weapon[0].weapon;
    for(int i=0;i<100;++i){in.sample++;in.tick+=10;in.timeNs+=10000000;
        in.hands[0].p.x=.2+.001*i;held=step(shoulder,in,p,sweep);
        assert(held.handlingOnly55c1[0]&&held.commandCount==0&&!attack_authority55c1(in,held,0));
        assert(m::length(held.hands[0].p-in.hands[0].p)<1e-9);
        assert(m::length(shoulder.weapon[0].weapon.p-seed.p)<1e-9);
    }
    clear=true;in.sample++;in.tick+=10;in.timeNs+=10000000;
    held=step(shoulder,in,p,sweep);assert(held.owned[0]&&!held.handlingOnly55c1[0]&&held.contactKnown[0]&&held.commandCount==0);
    in.attacks=false;assert(!attack_authority55c1(in,o,0));in.attacks=true;
    in.tracked[0]=false;in.sample++;in.tick+=10;in.timeNs+=10000000;
    o=step(s,in,p,sweep);assert(!o.active[0]&&!o.handlingOnly55c1[0]&&(o.invalidMask55c1[0]&8));
    in.tracked[0]=true;p[0].primary=false;in.sample++;in.tick+=10;
    o=step(s,in,p,sweep);assert(!o.active[0]&&(o.invalidMask55c1[0]&2));
}
