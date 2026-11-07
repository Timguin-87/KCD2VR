// R138 host test: a closed empty left fist is never native LT (vrik/fist_lt_r138.h).
// g++ -std=c++20 -I src tests/fist_lt_r138_test.cpp && ./a.out
#include "vrik/fist_lt_r138.h"
#include <cassert>
#include <cstdio>
using namespace kcd2vr::fist_lt_r138;
static Input base(std::uint64_t t,float raw,float grip){Input i{};i.now=t;i.raw=raw;i.grip=grip;i.consume=true;i.tracked=true;i.inCombat=0;return i;}
int main(){
    // 7 Nov 04:15:29 log: trigger crosses first, grip closes 40 ms later, out of combat -> was native LT (block -> every punch a kick)
    {State s{};assert(step(s,base(1000,.5f,.2f))==Event::None&&!s.pass&&s.pending);
     assert(step(s,base(1040,1.f,1.f))==Event::Fist&&!s.pass);
     for(std::uint64_t t=1050;t<1600;t+=10){step(s,base(t,1.f,1.f));assert(!s.pass);} // whole press stays the mod's
     step(s,base(1610,0.f,1.f));assert(!s.pass&&!s.pending);}
    // fist closed together with the trigger: refused at once
    {State s{};assert(step(s,base(0,1.f,1.f))==Event::Fist&&!s.pass);}
    // open hand (grip open) out of combat: native LT after 100 ms (stealth kill / knock out)
    {State s{};step(s,base(0,1.f,0.f));assert(!s.pass);step(s,base(99,1.f,0.f));assert(!s.pass);
     assert(step(s,base(100,1.f,0.f))==Event::Native&&s.pass);step(s,base(500,1.f,0.f));assert(s.pass);
     // grip closes later in that press: cut for the rest of the press
     assert(step(s,base(600,1.f,.9f))==Event::Cut&&!s.pass);step(s,base(700,1.f,0.f));assert(!s.pass);
     // next press is decided again (no latch)
     step(s,base(800,0.f,0.f));step(s,base(900,1.f,0.f));assert(step(s,base(1000,1.f,0.f))==Event::Native&&s.pass);}
    // weapon in the left hand: the old R122 rule (grip does not make it a fist)
    {State s{};Input i=base(0,1.f,1.f);i.leftHandFree=false;step(s,i);i.now=100;assert(step(s,i)==Event::Native&&s.pass);}
    // in combat / unknown: never native (old rule)
    {State s{};Input i=base(0,1.f,0.f);i.inCombat=1;step(s,i);i.now=200;step(s,i);assert(!s.pass&&!s.pending);
     State u{};Input j=base(0,1.f,0.f);j.inCombat=-1;step(u,j);j.now=200;step(u,j);assert(!u.pass);}
    // mod block/kick took LT: cut (R122 rule kept)
    {State s{};step(s,base(0,1.f,0.f));step(s,base(100,1.f,0.f));assert(s.pass);Input i=base(150,1.f,0.f);i.ltPublished=true;step(s,i);assert(!s.pass);}
    // press that starts in a menu / untracked: not decided
    {State s{};Input i=base(0,1.f,0.f);i.menu=true;step(s,i);assert(!s.pending&&!s.pass);}
    std::puts("fist_lt_r138: all passed");
}
