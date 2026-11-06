// DS-F42: adapter bridge v2 (Codex F35/F36 counterexamples).
#include "../src/vrik/constraint_adapter60.h"
#include <cstdio>
#include <cstring>
namespace ca=kcd2vr::constraint_adapter60;
namespace cs=kcd2vr::constraint60;
namespace co=kcd2vr::constraint_op60;
static int fails=0;
#define CK(c) do{if(!(c)){std::printf("FAIL %s:%d %s\n",__FILE__,__LINE__,#c);++fails;}}while(0)
static constexpr std::uintptr_t WA=0x777;
static cs::Snap present(unsigned id){cs::Snap s{};s.status={cs::Status::Kind::Present,id,WA};s.world=WA;return s;}
static cs::Snap absent(unsigned id){cs::Snap s{};s.status={cs::Status::Kind::Absent,id,WA};s.world=WA;return s;}
int main(){
    co::Op out{};
    // full connected lifecycle (v2): acquire -> Create edge -> ACK -> Drive ->
    // release -> Remove edge -> Absent -> Removed -> Idle
    {ca::Adapter a{};a.sourceBody=0xCAFE;a.sourceGeneration=9;cs::Snap snap{};snap.world=WA;
     CK(a.service.acquire(0xBAD,WA,1000));
     (void)a.feed(snap,1000); // Create edge admitted
     CK(co::take(out)&&out.kind==co::Kind::Create&&out.key.sourceBody==0xCAFE&&out.key.sourceGeneration==9);
     co::settle(out,true);
     snap.acceptedIdValid=true;snap.acceptedId=77;snap.acceptedToken=a.service.acquireToken;snap.acceptedToken=a.service.acquireToken;
     (void)a.feed(snap,1010);
     CK(!co::take(out)); // waiting for the ACK status
     (void)a.feed(present(77),1020);
     CK(co::take(out)&&out.kind==co::Kind::Drive);co::settle(out,true);
     CK(a.service.release(1040));
     (void)a.feed(present(77),1050);
     CK(co::take(out)&&out.kind==co::Kind::Remove);co::settle(out,true);
     (void)a.feed(present(77),1060);
     CK(!co::take(out)); // no second Remove
     (void)a.feed(absent(77),1070);
     CK(a.service.stage==cs::Stage::Removed);
     (void)a.feed(absent(77),1080);
     CK(a.service.stage==cs::Stage::Idle);
     CK(!co::take(out));}
    // Codex counterexample 1 (exact): BUSY Create admission is retried, never lost
    {ca::Adapter a{};a.sourceBody=1;a.sourceGeneration=1;cs::Snap snap{};snap.world=WA;
     CK(a.service.acquire(0xBAD,WA,2000));
     CK(co::gate.test_and_set(std::memory_order_acquire)==false); // hold the gate (simulated busy)
     (void)a.feed(snap,2000); // Create edge: admit busy -> pending retained
     co::gate.clear(std::memory_order_release);
     (void)a.feed(snap,2010); // machine does NOT re-emit Create; the pending retry must admit it
     CK(co::take(out)&&out.kind==co::Kind::Create); // the SAME create, not lost
     co::settle(out,true);
     CK(a.pending.kind==co::Kind::None);}
    // Codex counterexample 2 (exact): a full FIFO with a claimed head never overwrites it
    {for(auto& r:co::ring)r={};for(auto& f:co::inFlight)f.store(false,std::memory_order_release);
     co::head.store(0,std::memory_order_release);co::tail.store(0,std::memory_order_release);
     for(unsigned s=0;s<co::kFifo;++s)CK(co::admit([](std::uint32_t s){co::Op o{};o.kind=co::Kind::Remove;o.key={100+s,0xBAD,0x777,1,1};o.serial=s+1;o.tick=1000+s;return o;}(s)));
     co::Op claimed{};CK(co::take(claimed)&&claimed.serial==1); // claim the head
     co::Op extra{};extra.kind=co::Kind::Remove;extra.key={200,0xBAD,0x777,1,1};extra.serial=99;extra.tick=1100;
     CK(!co::admit(extra)); // full INCLUDING the claimed head: explicit rejection
     CK(co::ring[0].serial==1&&co::inFlight[0].load(std::memory_order_acquire)); // the claimed head is intact
     CK(co::settle(claimed,true));
     CK(co::admit(extra)); // a free slot exists again
     CK(co::take(out)&&out.serial==2); // the head advanced; take never skipped past the claimed head
     CK(co::settle(out,true));
     // drain the rest so later suites start clean
     co::Op t{};while(co::take(t))co::settle(t,true);}
    // settle busy returns false (the drain retries the pending disposition)
    {co::Op o{};o.kind=co::Kind::Remove;o.key={300,0xBAD,0x777,1,1};o.serial=500;o.tick=1200;
     CK(co::admit(o));
     co::Op claimed{};CK(co::take(claimed));
     CK(co::gate.test_and_set(std::memory_order_acquire)==false);
     CK(!co::settle(claimed,true)); // busy: explicit retry result, not a silent give-up
     co::gate.clear(std::memory_order_release);
     CK(co::settle(claimed,true));}
    // Codex F38 case 1 (exact): busy Create pending + worldReplaced -> the stale pending is CANCELLED, never published
    {for(auto& r:co::ring)r={};for(auto& f:co::inFlight)f.store(false,std::memory_order_release);
     ca::Adapter a{};a.sourceBody=1;a.sourceGeneration=1;cs::Snap snap{};snap.world=WA;
     CK(a.service.acquire(0xBAD,WA,3000));
     CK(co::gate.test_and_set(std::memory_order_acquire)==false);
     (void)a.feed(snap,3000); // Create pending (busy)
     co::gate.clear(std::memory_order_release);
     CK(a.pending.kind==co::Kind::Create);
     snap.worldReplaced=true;
     (void)a.feed(snap,3010); // world replaced: the machine quarantines AND the pending is cancelled
     CK(a.pending.kind==co::Kind::None);
     CK(!co::take(out)); // the old-world Create never reached the queue
     CK(a.service.stage==cs::Stage::Quarantined);}
    // Codex F38 case 2 (exact): executed(Create,false) faults the machine in Requested
    {for(auto& r:co::ring)r={};for(auto& f:co::inFlight)f.store(false,std::memory_order_release);
     ca::Adapter a{};a.sourceBody=1;a.sourceGeneration=1;cs::Snap snap{};snap.world=WA;
     CK(a.service.acquire(0xBAD,WA,4000));
     (void)a.feed(snap,4000); // Create admitted
     co::Op op{};CK(co::take(op)&&op.kind==co::Kind::Create);
     auto st=a.executed(op,false,snap,4010); // refused create feeds a faulted status
     CK(a.service.stage==cs::Stage::Quarantined); // the Requested stage now quarantines on a faulted status
     co::settle(op,false);
     // a no-id quarantine preserves possible queued native ownership: acquire stays refused
     CK(!a.service.acquire(0xBE,WA,4030));}
    // Codex F38 case 3 (exact): busy Remove -> pending; the next tick admits the PENDING only, never a second Remove
    {for(auto& r:co::ring)r={};for(auto& f:co::inFlight)f.store(false,std::memory_order_release);
     ca::Adapter a{};a.sourceBody=1;a.sourceGeneration=1;cs::Snap snap{};snap.world=WA;
     CK(a.service.acquire(0xBAD,WA,5000));
     (void)a.feed(snap,5000); // Create
     co::Op op{};CK(co::take(op));co::settle(op,true);
     snap.acceptedIdValid=true;snap.acceptedId=77;snap.acceptedToken=a.service.acquireToken;snap.acceptedToken=a.service.acquireToken;(void)a.feed(snap,5010);
     (void)a.feed(present(77),5020); // Present -> Drive
     CK(co::take(op));co::settle(op,true);
     CK(a.service.release(5030));
     CK(co::gate.test_and_set(std::memory_order_acquire)==false);
     (void)a.feed(present(77),5040); // Remove edge busy -> pending
     co::gate.clear(std::memory_order_release);
     CK(a.pending.kind==co::Kind::Remove);
     (void)a.feed(present(77),5050); // next tick: the PENDING Remove is admitted; NO second Remove is built
     CK(co::take(op)&&op.kind==co::Kind::Remove);co::settle(op,true);
     CK(!co::take(out)); // exactly one Remove reached the queue
     CK(a.pending.kind==co::Kind::None);
     (void)a.feed(absent(77),5060);(void)a.feed(absent(77),5070);
     CK(a.service.stage==cs::Stage::Idle);}
    // Codex 1429 case (exact): a busy CLEANUP in Quarantined is PRESERVED and
    // retried (the teardown must reach the executor); only Create/Drive die
    // there.
    {for(auto& r:co::ring)r={};for(auto& f:co::inFlight)f.store(false,std::memory_order_release);
     ca::Adapter a{};a.sourceBody=1;a.sourceGeneration=1;cs::Snap snap{};snap.world=WA;
     CK(a.service.acquire(0xBAD,WA,6000));
     (void)a.feed(snap,6000); // Create
     co::Op op{};CK(co::take(op));co::settle(op,true);
     snap.acceptedIdValid=true;snap.acceptedId=7;snap.acceptedToken=a.service.acquireToken;snap.acceptedToken=a.service.acquireToken;(void)a.feed(snap,6010); // Created(id7)
     cs::Snap faulted{};
     faulted.world=WA;faulted.status={cs::Status::Kind::Fault,7,WA};
     (void)a.feed(faulted,6020); // Created+statusFault -> Quarantined (id retained)
     CK(a.service.stage==cs::Stage::Quarantined);
     // the machine's Quarantined step emits Cleanup; make its admission busy
     CK(co::gate.test_and_set(std::memory_order_acquire)==false);
     (void)a.feed(faulted,6030); // Cleanup pending (busy)
     co::gate.clear(std::memory_order_release);
     CK(a.pending.kind==co::Kind::Cleanup);
     (void)a.feed(faulted,6040); // next tick: the pending Cleanup is PRESERVED and retried
     CK(a.pending.kind==co::Kind::None); // admitted, not discarded
     CK(co::take(op)&&op.kind==co::Kind::Cleanup); // the teardown reached the queue
     co::settle(op,true);
     CK(!co::take(out));
     // a pending Create in Quarantined IS cancelled (stale work)
     ca::Adapter b{};b.sourceBody=1;b.sourceGeneration=1;cs::Snap snapb{};snapb.world=WA;
     CK(b.service.acquire(0xBAD,WA,6500));
     CK(co::gate.test_and_set(std::memory_order_acquire)==false);
     (void)b.feed(snapb,6500); // Create pending (busy)
     co::gate.clear(std::memory_order_release);
     CK(b.pending.kind==co::Kind::Create);
     snapb.worldReplaced=true;
     (void)b.feed(snapb,6510); // world replaced -> Quarantined; pending Create cancelled
     CK(b.pending.kind==co::Kind::None);
     CK(!co::take(out));}
    // DS-F51 (Codex): a REUSED Adapter emits a fresh Create on the next
    // acquire - the previous lifecycle's Remove edge never leaks across.
    {for(auto& r:co::ring)r={};for(auto& f:co::inFlight)f.store(false,std::memory_order_release);
     ca::Adapter a{};a.sourceBody=1;a.sourceGeneration=1;cs::Snap snap{};snap.world=WA;
     CK(a.service.acquire(0xBAD,WA,7000));
     (void)a.feed(snap,7000); // Create
     co::Op op{};CK(co::take(op)&&op.kind==co::Kind::Create);co::settle(op,true);
     snap.acceptedIdValid=true;snap.acceptedId=77;snap.acceptedToken=a.service.acquireToken;snap.acceptedToken=a.service.acquireToken;(void)a.feed(snap,7010);
     (void)a.feed(present(77),7020); // Drive
     CK(co::take(op));co::settle(op,true);
     CK(a.service.release(7030));
     (void)a.feed(present(77),7040); // Remove edge
     CK(co::take(op)&&op.kind==co::Kind::Remove);co::settle(op,true);
     (void)a.feed(absent(77),7050);(void)a.feed(absent(77),7060); // Removed -> Idle
     CK(a.service.stage==cs::Stage::Idle);
     CK(a.service.acquire(0xBAD,WA,7100)); // REUSE: a new acquire
     (void)a.feed(snap,7110);
     CK(co::take(op)&&op.kind==co::Kind::Create); // the new lifecycle's Create, not the stale Remove
     co::settle(op,true);}
        // Codex F46 repro (exact): a stale accepted id from an OLD acquire must
    // never latch, even after the NEW create was planned/pending
    {for(auto& r:co::ring)r={};for(auto& f:co::inFlight)f.store(false,std::memory_order_release);
     ca::Adapter a{};a.sourceBody=11;a.sourceGeneration=22;cs::Snap snap{};snap.world=WA;
     CK(a.service.acquire(0xBAD,WA,8000)); // token 1
     snap.acceptedIdValid=true;snap.acceptedId=77;snap.acceptedToken=0; // an OLD reply (wrong token)
     CK(co::gate.test_and_set(std::memory_order_acquire)==false);
     (void)a.feed(snap,8000); // first tick: old reply refused; Create planned+pending (busy)
     CK(a.service.stage==cs::Stage::Requested&&a.pending.kind==co::Kind::Create);
     (void)a.feed(snap,8010); // second tick, STILL busy, SAME old reply
     co::gate.clear(std::memory_order_release);
     CK(a.service.stage==cs::Stage::Requested&&a.service.id==0); // the old id never latched
     (void)a.feed(snap,8020); // gate free: the pending Create is admitted; the old reply still refused
     co::Op op{};CK(co::take(op)&&op.kind==co::Kind::Create);co::settle(op,true);
     snap.acceptedToken=a.service.acquireToken; // the reply now carries the CURRENT token
     (void)a.feed(snap,8030);
     CK(a.service.stage==cs::Stage::Created&&a.service.id==77); // only now it latches
     (void)a.feed(absent(77),8040); // typed Absent -> quarantined (retained id)
     CK(a.service.stage==cs::Stage::Quarantined&&a.service.id==77);}
    if(fails)return 1;std::puts("constraint adapter bridge v3 OK");return 0;
}
