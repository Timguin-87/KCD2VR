#include "temporal_compose_publication_d82.h"
#include <cstdio>
#include <thread>
#include <vector>

using namespace kcd2vr::temporal_compose_d82;
static unsigned checks=0,failed=0;
#define CHECK(x) do{++checks;if(!(x)){++failed;std::printf("FAIL %u: %s\n",__LINE__,#x);}}while(false)

int main(){
    const Scope right{9,601,0xA000};
    Recording recording{};std::atomic<bool> spent{false};
    // The reported failure: an early main-resource barrier arrives before the
    // temporal SDK/final composition. It must leave RIGHT available.
    CHECK(!ClaimRightCopy(spent,true,true,{},601,right.output,false));
    CHECK(!spent.load());
    CHECK(!ClaimRightCopy(spent,true,true,{},601,right.output,true));
    CHECK(!spent.load());
    recording.BindMain(right.output);
    CHECK(!recording.RecordDraw(0x1000,{},3,1));
    CHECK(!recording.RecordDraw(0x1000,right,0,1));
    CHECK(!recording.RecordDraw(0x1000,right,3,0));
    CHECK(!recording.RecordDraw(0,right,3,1));
    CHECK(recording.RecordDraw(0x1000,right,3,1));
    auto stamped=recording.SubmittedCandidate(0x1000,601);
    CHECK(stamped.Valid());
    CHECK(!recording.SubmittedCandidate(0x2000,601).Valid());
    CHECK(!recording.SubmittedCandidate(0x1000,602).Valid());
    CHECK(!ClaimRightCopy(spent,true,false,stamped,601,right.output,false));
    CHECK(!spent.load());
    CHECK(!ClaimRightCopy(spent,true,false,stamped,602,right.output,true));
    CHECK(!spent.load());
    CHECK(!ClaimRightCopy(spent,true,false,stamped,601,0xB000,true));
    CHECK(!spent.load());
    // Actual final compose may be in a later list with no new main barrier.
    CHECK(ClaimRightCopy(spent,true,false,stamped,601,right.output,true));
    CHECK(spent.load());
    CHECK(!ClaimRightCopy(spent,true,true,stamped,601,right.output,true));

    // Reset destroys both binding and completion; pointer equality cannot
    // certify a recording that was canceled or reused after that reset.
    const auto oldGeneration=recording.generation;
    recording.Discard();
    CHECK(recording.generation!=oldGeneration);
    CHECK(!recording.boundOutput);
    CHECK(!recording.SubmittedCandidate(0x1000,601).Valid());
    CHECK(!recording.RecordDraw(0x1000,right,3,1));
    recording.composed=stamped; // even an accidentally retained old stamp fails
    CHECK(!recording.SubmittedCandidate(0x1000,601).Valid());
    recording.BindMain(right.output);
    CHECK(recording.RecordDraw(0x1000,right,3,1));
    stamped=recording.SubmittedCandidate(0x1000,601);
    CHECK(stamped.generation==recording.generation);
    recording.BindMain(0);
    CHECK(!recording.RecordDraw(0x1000,right,3,1));
    recording.Discard();
    CHECK(!recording.SubmittedCandidate(0x1000,601).Valid());

    // ClearState removes bindings but preserves already-recorded pixels. A
    // later genuine main-target Draw outside the verified scope supersedes them.
    recording.BindMain(right.output);
    CHECK(recording.RecordDraw(0x1000,right,3,1));
    recording.ClearBindings();
    CHECK(!recording.boundOutput);
    CHECK(recording.SubmittedCandidate(0x1000,601).Valid());
    CHECK(!recording.RecordDraw(0x1000,right,3,1));
    CHECK(!recording.ObserveUnscopedDraw(0x1000,3,1));
    recording.BindMain(right.output);
    CHECK(!recording.ObserveUnscopedDraw(0x2000,3,1));
    CHECK(!recording.ObserveUnscopedDraw(0x1000,0,1));
    CHECK(!recording.ObserveUnscopedDraw(0x1000,3,0));
    CHECK(recording.ObserveUnscopedDraw(0x1000,3,1));
    CHECK(!recording.SubmittedCandidate(0x1000,601).Valid());

    // Native0 policy remains exactly the established main-writer condition,
    // independent of any temporal receipt, stamp, frame, or output metadata.
    for(bool main:{false,true})for(bool ready:{false,true}){
        std::atomic<bool> nativeSpent{false};
        CHECK(ClaimRightCopy(nativeSpent,false,main,{},0,0,ready)==main);
        CHECK(nativeSpent.load()==main);
    }
    // Simultaneous matching submits still get exactly one copy owner.
    std::atomic<unsigned> wins{0};spent.store(false);
    std::vector<std::thread> threads;
    for(unsigned i=0;i<8;++i)threads.emplace_back([&]{
        if(ClaimRightCopy(spent,true,false,stamped,601,right.output,true))++wins;
    });
    for(auto& t:threads)t.join();
    CHECK(wins.load()==1);
    std::printf("D82 TEMPORAL COMPOSE POLICY: %u checks, %u failures\n",checks,failed);
    return failed?1:0;
}
