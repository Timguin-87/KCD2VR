#include "vrik/physical_interaction.h"
#include "vrik/hands_mode59c7f.h"
#include "vrik/hand_point123r1.h"
#include "vrik/physical_hand_owner55.h"
#include <array>
#include <cassert>
#include <cstdio>
// In-memory publications are leaves; each decision below executes the actual
// production policy header. There is no simulated native action acceptance.
struct Profile {std::uint64_t key=0,generation=0,entity=0;int side=0;bool twoHanded=false;bool bow()const{return false;}};
struct Frame {
    std::uint64_t request=3;
    struct Input {std::uint64_t sample=10,epoch=3,bodyEpoch=7,tick=1000;bool allowed=true,tracked[2]{true,true};double grip[2]{};} input;
    struct Output {bool active[2]{true,false},owned[2]{true,false},support[2]{};bool bowHand169r2=false;} output;
    struct Correlation {struct Socket{bool valid=false;std::uint64_t key=0,generation=0;void* character=nullptr;} socket;
        struct Stamp{std::uint64_t epoch=0,sample=0,tick=0;std::uintptr_t actor=0;} stamp;} correlation126[2];
    Profile profiles[2]{};bool retainedSupport55[2]{};
    void* player=reinterpret_cast<void*>(2);void* manager=reinterpret_cast<void*>(3);
};
struct Equipment {
    bool valid=true;std::uint64_t request=3,epoch=7,tick=1000;int count=1;
    void* player=reinterpret_cast<void*>(2);void* manager=reinterpret_cast<void*>(3);
    bool handKnown[2]{true,true};std::uint64_t handKey[2]{77,0};int heldIndex[2]{0,-1};
    struct Item{bool held=true;Profile profile{};};std::array<Item,2> items{};
};
int main(){
    namespace p=kcd2vr::physical_interaction106;
    p::State st{};p::Input in{};in.enabled=true;in.now_ms=1000;
    in.hand[0]={true,true,true,0,false};in.hand[1]={true,true,true,0,false};
    p::Step(st,in);in.now_ms=1010;in.hand[0].grip=1;
    assert(!p::Step(st,in).fire_use&&st.use_pending);
    in.now_ms=1020;in.hand[0].grip=0;assert(!p::Step(st,in).fire_use&&!st.use_pending);
    in.now_ms=1200;assert(!p::Step(st,in).fire_use); // old delayed pulse cannot fire after release
    in.now_ms=1210;in.hand[0].grip=1;p::Step(st,in);
    in.now_ms=1330;auto out=p::Step(st,in);assert(out.fire_use);
    p::CompleteUse116(st,out,false);in.now_ms=1340;in.hand[0].grip=0;
    assert(!p::Step(st,in).fire_use&&!st.use_pending); // rejected transport retry cancels too
    namespace h=kcd2vr::hands59c7f;
    h::Arbitration a{};h::Hand hands[2]{{true,true,0,0},{true,true,0,0}};
    assert(a.update(true,1,1000,hands,{})==0);
    hands[0].grip=1;assert(a.update(true,1,1010,hands,{})==0);
    assert(a.update(true,1,1020,hands,{true,0,1020,.08f})==1); // late selector, same original squeeze
    hands[0].grip=0;a.update(true,1,1030,hands,{});
    assert(a.update(true,1,1090,hands,{true,0,1090,.08f})==0);
    a={};hands[0].grip=hands[1].grip=0;a.update(true,1,2000,hands,{});
    hands[0].grip=hands[1].grip=1;assert(a.update(true,1,2010,hands,{true,0,2010,.08f})==0);
    assert(a.update(true,1,2020,hands,{true,0,2020,.08f})==0); // simultaneous holds do not choose arbitrarily
    a={};hands[1].grip=0;hands[0].grip=0;a.update(true,1,3000,hands,{});
    hands[0].grip=1;a.update(true,1,3010,hands,{});hands[0].grip=0;a.update(true,1,3020,hands,{});
    assert(a.update(true,1,3030,hands,{true,0,3030,.08f})==0); // released before candidate
    namespace point=kcd2vr::hand_point123r1;
    point::State ps{};point::Input pi{};pi.enabled=true;pi.now=4000;pi.hand[0]={true,0,0};
    point::Update(ps,pi);pi.now=4010;pi.hand[0]={false,1,0};point::Update(ps,pi); // H40 temporarily reserves hand
    pi.now=4030;pi.hand[0]={true,1,0,false,false,4010};
    assert(point::Update(ps,pi).owner==0); // original validated fallback reclaims palm, no resqueeze
    pi.hand[0].eligible=false;pi.now=4040;point::Update(ps,pi);
    pi.hand[0].eligible=true;pi.now=4050;assert(point::Update(ps,pi).owner==-1); // capability is not replayed
    ps={};pi.now=4500;pi.hand[0].resumePressD48=4010;assert(point::Update(ps,pi).owner==-1);
    Frame f{};Equipment e{};f.profiles[0]={77,2,99,0,false};e.items[0].profile=f.profiles[0];
    // Right item's correlation is missing. It cannot poison an independent,
    // known-empty left hand, but cannot certify the right or support hand.
    auto read=[&](int side){return kcd2vr::physical_hand_owner55::read(f,e,side,1000,10,3,7,0x1234);};
    assert(read(0)==-1&&read(1)==0);
    f.output.support[0]=true;f.output.owned[1]=true;assert(read(1)==-1);
    f.correlation126[0].socket={true,77,2,reinterpret_cast<void*>(0x1234)};
    f.correlation126[0].stamp={7,10,1000,0x1234};assert(read(0)==1&&read(1)==1);
    std::puts("PASS D48 delayed-use release, late selection, original door fallback, simultaneous rejection and per-hand ownership");
}
