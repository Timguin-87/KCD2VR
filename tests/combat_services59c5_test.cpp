#include "../src/diagnostics/combat_services59c5.h"
#include <cassert>
#include <thread>
#include <vector>
#include <atomic>
using namespace kcd2vr::diagnostics::services59c5;
int main(){
    Journal<4> j;Record in{},out{};in.actor=123;in.kind=Kind::CounterEnd;
    for(int n=0;n<4;++n){in.span=j.span();assert(j.push(in));}
    assert(!j.push(in)&&j.dropped()==1);
    for(int n=1;n<=4;++n){assert(j.pop(out));assert(out.sequence==n&&out.span==n&&out.actor==123);}
    assert(!j.pop(out));
    Journal<128> q;std::atomic<unsigned> sent=0,read=0;std::atomic<bool> done=false;
    std::thread consumer([&]{Record r;std::uint64_t last=0;for(;;){
        if(q.pop(r)){assert(r.sequence>last);last=r.sequence;++read;}
        else if(done){while(q.pop(r)){assert(r.sequence>last);last=r.sequence;++read;}break;}
        else std::this_thread::yield();
    }});
    std::vector<std::thread> producers;
    for(int t=0;t<4;++t)producers.emplace_back([&]{for(int n=0;n<5000;++n){Record r;r.span=q.span();if(q.push(r))++sent;}});
    for(auto& t:producers)t.join();done=true;consumer.join();
    assert(read==sent&&q.dropped()+sent==20000);
}
