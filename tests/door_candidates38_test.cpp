#include <cassert>
#include <cstring>
#include <cstdio>
#include "interaction_alignment.h"
#include "core/door_candidates38.h"
using kcd2vr::interaction_alignment::HandCandidate;
using kcd2vr::door_candidates38::Records;
static HandCandidate observation(unsigned id,int side,std::uint64_t tick=1000) {
    HandCandidate c{};c.entity=id;c.owner=side;c.tick=tick;c.object=true;c.distance=.2f;
    c.x=1.f;c.y=2.f;c.z=3.f;std::strcpy(c.cls,"AnimDoor");return c;
}
int main() {
    Records<HandCandidate,4> r;HandCandidate out{};
    auto door=observation(42,0);r.record(door);
    auto pickup=observation(99,1);std::strcpy(pickup.cls,"PickableItem");r.record(pickup);
    assert(r.find(42,0,1000,out)&&out.entity==42);
    assert(!r.find(42,1,1000,out));assert(!r.find(99,1,1000,out));
    assert(r.find(42,0,1080,out));assert(!r.find(42,0,1081,out));assert(!r.find(42,0,999,out));
    r.record(observation(42,1));assert(r.find(42,0,1000,out));assert(r.find(42,1,1000,out));
    r.withdraw(42);assert(!r.find(42,0,1000,out));assert(!r.find(42,1,1000,out));
    r.record(door);auto changed=door;std::strcpy(changed.cls,"BasicEntity");r.record(changed);
    assert(!r.find(42,0,1000,out));
    r.record(door);changed=door;changed.x=std::nanf("");r.record(changed);assert(!r.find(42,0,1000,out));
    r.clear();r.record(door);r.record(observation(43,1));r.record(observation(44,0));r.record(observation(45,1));
    r.record(observation(46,0));assert(!r.find(42,0,1000,out));assert(r.find(46,0,1000,out));
    r.clear();assert(!r.find(46,0,1000,out));
    // Producing player is paired to the committed character and epoch, with a
    // stable even ledger publication around packet construction.
    using kcd2vr::door_candidates38::framePlayer;
    assert(framePlayer(2,2,7,7,0x100,0x100,0x200,1000,1000)==0x200);
    assert(framePlayer(3,3,7,7,0x100,0x100,0x200,1000,1000)==0);
    assert(framePlayer(2,4,7,7,0x100,0x100,0x200,1000,1000)==0);
    assert(framePlayer(2,2,6,7,0x100,0x100,0x200,1000,1000)==0);
    assert(framePlayer(2,2,7,7,0x100,0x300,0x200,1000,1000)==0);
    assert(framePlayer(2,2,7,7,0x100,0x100,0,1000,1000)==0);
    assert(framePlayer(2,2,7,7,0x100,0x100,0x200,1000,4001)==0);
    std::puts("door_candidates38: exact target/hand, overwrite, withdrawal, freshness and bounded eviction PASS");
}
