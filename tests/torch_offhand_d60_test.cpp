#include "vrik/torch_offhand_d60.h"
#include <cassert>
#include <cstdio>
int main(){
    namespace t=kcd2vr::torch_offhand_d60;
    t::State s{};t::Frame f{};f.owner=1;f.key=2;f.generation=3;f.body=4;f.request=5;
    f.sample=1;f.tick=1000;f.classId=11;f.side=1;f.flags=t::Active|t::Owned;
    assert(!s.offer(f,1,false,1000)&&!s.session&&!s.emitted);
    assert(!s.offer(f,0,true,1000));
    auto stale=f;stale.tick=919;assert(!s.offer(stale,1,true,1000));
    assert(s.offer(f,1,true,1000)&&s.emitted==1);
    f.flags|=t::LocalBlocked;assert(!s.offer(f,1,true,1000)); // reused frame cannot invent a change
    ++f.sample;++f.tick;assert(s.offer(f,1,true,1001));
    ++f.sample;++f.tick;assert(!s.offer(f,1,true,1002)); // steady state is quiet
    f.classId=8;++f.sample;++f.tick;assert(!s.offer(f,1,true,1003));
    f.classId=11;f.side=0;++f.sample;++f.tick;assert(!s.offer(f,1,true,1004));
    f.side=1;f.guardSide=0;f.flags|=t::GuardHeld;++f.sample;++f.tick;assert(s.offer(f,1,true,1005));
    for(int i=0;i<100;++i){++f.sample;++f.tick;f.phase=i;s.offer(f,1,true,f.tick);}
    assert(s.emitted==64); // item/generation churn cannot reset this capture's budget
    ++f.key;++f.generation;++f.sample;++f.tick;assert(!s.offer(f,1,true,f.tick));
    assert(s.offer(f,2,true,f.tick)&&s.emitted==1);
    f.owner=0;++f.sample;++f.tick;assert(!s.offer(f,2,true,f.tick));
    std::puts("TORCH_OFFHAND_D60 PASS: finite copied-frame observation; no native calls/writes");
}
