// Executes the actual Mod lock producer and native projection acceptance gate.
// Native selection is a copied mailbox; no game code or worker native reads.
#include "../src/activity_input163.h"
#include "../src/vrik/alchemy_palm_lock27.h"
#include "../src/vrik/workstation178r20c1.h"
#include "../src/vrik/palm_selection59c9.h"
#include <cassert>
#include <cstdio>
using namespace kcd2vr;
static unsigned checks27=0;
static void ck(bool b){++checks27;assert(b);}
int main(){
    auto& w=activity_input163::witness175;w.ready=true;
    w.Begin(activity_input163::Kind175::Alchemy,123,1000);
    workstation178r20c1::setExperiment36(true);
    bool alchemyPointer59c2=true;unsigned f5Mode=2;std::uint64_t ladder_now_ms=1000;
    const auto GetTickCount64=[&](){return ladder_now_ms;}; // deterministic native-clock leaf
    struct Candidate {bool object=true;int owner=1;std::uint32_t entity=777;std::uint64_t tick=1000;} candidate59c7f;
    palm59c9::Pair palms59c9{};
    alchemy_input19c4::PalmLock27 alchemyPalmLock27;
    auto run=[&](int side,std::uint32_t selected,std::uint64_t age=0){
        ++ladder_now_ms;candidate59c7f.tick=ladder_now_ms;
        alchemy_input19c4::selected.publish({123,w.lifetime175.load(),ladder_now_ms-age,selected});
        palms59c9.locked=side;
#ifdef C27_OLD_LOCK_CONTROL
#include "old_palm_lock27.inc"
#else
#include "../src/vrik/interaction_palm_lock27.inl"
#endif
        return palms59c9.lockedEntity;
    };
    ck(run(-1,42)==0);ck(run(1,42)==42); // offered777 must not veto native-selected42
    ck(palm59c9::accepts(palms59c9,42,1)&&!palm59c9::accepts(palms59c9,777,1));
    ck(!palm59c9::accepts(palms59c9,42,-1));
    ck(run(1,43)==42); // changing projection/native hover cannot replace held target
    ck(run(-1,43)==0);ck(run(1,43)==43); // physical release then next selection
    // Native selection is independent of an unavailable native hold/release hint.
    alchemy_input19c4::Observation hint{};
    ck(!alchemy_input19c4::lookupSelected(123,w.lifetime175.load(),ladder_now_ms,hint));
    ck(run(-1,42)==0);ck(run(1,42)==42);
    // Same-address native restart invalidates the grip, without a held re-acquire.
    w.End(123);w.Begin(activity_input163::Kind175::Alchemy,123,ladder_now_ms);
    ck(run(1,43)==0);ck(run(1,43)==0);ck(run(-1,43)==0);ck(run(1,43)==43);
    f5Mode=0;workstation178r20c1::setExperiment36(false);ck(run(1,42)==0);
    // C29: ordinary alchemy (F5 rung 2 experiment NOT armed) keeps the palm lock after a release/regrip
    ck(run(-1,42)==0);ck(run(1,42)==42);ck(run(1,43)==42);ck(run(-1,43)==0);
    ck(run(1,42)==42);f5Mode=1;ck(run(1,43)==0);ck(run(1,43)==0);
    f5Mode=0;ck(run(1,43)==0);ck(run(-1,43)==0);ck(run(1,43)==43);
    f5Mode=2;workstation178r20c1::setExperiment36(true);ck(run(1,42)==0);ck(run(-1,42)==0);ck(run(1,42)==42);
    w.transitionDepth175=1;ck(run(1,43)==0);w.transitionDepth175=0;ck(run(1,43)==0);
    ck(run(-1,43)==0);ck(run(1,43)==43);
    ck(run(-1,42)==0);ck(run(1,42,81)==0);ck(run(1,42)==0); // stale hint cannot latch later mid-Grip
    ck(run(-1,42)==0);ck(run(1,0)==0);ck(palm59c9::accepts(palms59c9,42,1)); // UNKNOWN never falls back to offered777
    f5Mode=1;ck(run(1,42)==0);f5Mode=2;ck(run(1,42)==0);ck(run(-1,42)==0);ck(run(1,42)==42);
    alchemyPointer59c2=false;ck(run(-1,42)==0);ck(run(1,42)==0); // ordinary Grip owns a hand, not an unsent hover target
    ck(run(-1,42)==0);alchemyPointer59c2=true;ck(run(-1,42)==0);ck(run(1,42)==42);
    // selection snapshot rejects time reversal/old activity identity explicitly.
    alchemy_input19c4::Selection s{};
    ck(alchemy_input19c4::selectionSnapshot(123,w.lifetime175.load(),ladder_now_ms,s)&&s.entity==42);
    ck(!alchemy_input19c4::selectionSnapshot(124,w.lifetime175.load(),ladder_now_ms,s));
    ck(!alchemy_input19c4::selectionSnapshot(123,w.lifetime175.load()+1,ladder_now_ms,s));
    ck(!alchemy_input19c4::selectionSnapshot(123,w.lifetime175.load(),ladder_now_ms-1,s));
    ck(!alchemy_input19c4::selectionSnapshot(123,w.lifetime175.load(),ladder_now_ms+81,s));
    std::printf("PASS %u actual palm lock / native-selected candidate checks\n",checks27);
}
