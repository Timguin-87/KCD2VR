#include "diagnostics/interaction_lifecycle165.h"
#include <cassert>
#include <thread>
#include <cstdio>
using namespace kcd2vr::diagnostics::lifecycle165;
int main(){
    {using namespace kcd2vr::diagnostics::lifecycle165;WatchIndex index;Event e{};
     e.session=1;e.entity=0x1000;e.target=7;e.key=12;e.tick=1000;assert(index.access(e,true));
     std::array<Event,4> out{};assert(index.snapshot59c1(2,1100,out)==0);
     assert(index.snapshot59c1(1,1100,out)==1&&out[0].key==12);
     assert(index.snapshot59c1(1,17000,out)==0); // no indefinite polling of an old object
     e.tick=18000;assert(index.access(e,true));assert(index.snapshot59c1(1,18100,out)==1);
    }

    Ledger l;
    Event a{};a.session=1;a.target=100;a.entity=0x1000;a.key=77;a.kind=Kind::PickupRequest;
    auto* apple=l.accept(a);assert(apple&&apple->requested&&!apple->held&&!apple->accepted);
    const auto appleId=apple->id;
    Event b=a;b.target=101;b.entity=0x2000;b.key=78;
    assert(l.accept(b)->id!=appleId); // interleaved apples, never 'last item'
    a.kind=Kind::PickupReturn;a.result=1;
    assert(l.accept(a)->accepted&&!apple->held&&!apple->attached); // accepted use is not a physical hold
    Event hand{};hand.session=1;hand.key=77;hand.tick=100;hand.kind=Kind::HandEnter;
    assert(l.accept(hand)->id==appleId&&apple->held);
    hand.tick=350;hand.kind=Kind::HandLeave;assert(l.accept(hand)->released&&!apple->held);
    assert(apple->heldMs==250&&apple->handVisits==1);
    hand.kind=Kind::HandSample;l.accept(hand);l.accept(hand);
    hand.kind=Kind::BodySample;hand.body=0x7000;l.accept(hand);
    hand.kind=Kind::ConstraintReturn;hand.result=0;hand.constraintId=17;l.accept(hand);
    assert(apple->handSamples58c2==2&&apple->bodySamples58c2==1&&apple->observedBody58c2==0x7000);
    assert(apple->lastConstraintResult58c2==0&&apple->lastConstraintId58c2==17);
    assert((apple->eventMask58c2&(1ull<<unsigned(Kind::BodySample)))!=0);
    assert(apple->constraintReturn&&!apple->constraintRequest&&!apple->attached&&!apple->held);
    // These are raw observations. They cannot manufacture a successful constraint/hold.
    auto* second=l.accept(b);assert(second->handSamples58c2==0&&second->observedBody58c2==0);
    assert(second->lastConstraintId58c2==-1&&second->lastConstraintResult58c2==-1);
    a.kind=Kind::RemoveAcceptedPath;l.accept(a);
    assert(apple->removeAccepted&&!apple->constraintRequest); // removal is not constraint cleanup
    a.key=99;assert(!l.accept(a)&&l.conflicts==1); // recycled/conflicting instance, fail closed
    a.session=2;a.key=77;assert(l.accept(a)->id!=appleId); // never joins across capture generations
    Event failed=b;failed.kind=Kind::PickupReturn;failed.result=0;
    auto* failure=l.accept(failed);assert(failure->returned&&!failure->accepted&&!failure->held);
    Event stage{};stage.session=1;stage.controller=0x3000;stage.kind=Kind::AlchemyState;
    assert(!l.accept(stage)); // workstation controller is not an invented prop identity
    Scopes<2> scope;b.session=2;scope.put(0x100,a);scope.put(0x80,b);
    assert(scope.find(0x100,2)&&scope.find(0x80,2));
    assert(!scope.find(0x100,1));scope.erase(0x80);assert(!scope.find(0x80,2));
    WatchIndex watch;Event watched=b;watched.session=3;
    assert(!watch.possible(watched.entity));assert(watch.access(watched,true)&&watch.possible(watched.entity));watched.key=0;assert(watch.access(watched,false)&&watched.key==78);
    watched.entity=0x3000;assert(!watch.access(watched,false)); // same target, wrong instance
    watched=b;watched.session=4;assert(!watch.access(watched,false)); // old capture unavailable
    watched.session=3;watched.key=79;assert(!watch.access(watched,true)&&watch.lost==1);
    watch.lock.test_and_set();assert(!watch.access(watched,false)&&watch.lost==2);watch.lock.clear();
    Queue q;for(int i=0;i<511;++i){a.call=i;assert(q.push(a));}
    assert(!q.push(a)&&q.dropped==1);
    Event e;for(int i=0;i<511;++i){assert(q.pop(e)&&e.call==static_cast<unsigned>(i));}assert(!q.pop(e));
    // Real producer contention cannot corrupt a record or disappear from accounting.
    q.dropped=0;
    auto producer=[&](unsigned id){for(unsigned i=0;i<10000;++i){Event ev{};ev.actor=id;ev.call=i;ev.key=id*10000+i;q.push(ev);}};
    std::thread t1(producer,1),t2(producer,2);t1.join();t2.join();
    unsigned got=0;while(q.pop(e)){assert(e.actor==1||e.actor==2);assert(e.key==e.actor*10000+e.call);++got;}
    assert(got+q.dropped==20000);
    std::puts("PASS lifecycle: exact joins, interleaving, rejection, scope/session isolation, bounded queue accounting");
}
