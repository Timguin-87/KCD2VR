// DS-F108: the impale feel layer (pure, host-tested).
#include "../src/subsystems/physics/impale_feel60.h"
#include <cstdio>
#include <cmath>
namespace ifeel=kcd2vr::impale_feel60;
static int fails=0;
#define CK(c) do{if(!(c)){std::printf("FAIL %s:%d %s\n",__FILE__,__LINE__,#c);++fails;}}while(0)
static ifeel::Blade blade(float speed,std::uint64_t tick){ifeel::Blade b{};b.motion[0]=speed;b.tick=tick;b.serial=1;b.ragdoll=true;return b;}
int main(){
    ifeel::Params p{};
    {ifeel::State s{};
     auto o=ifeel::plan(p,blade(1.f,1000),s,1000);
     CK(o.slideFactor<1.f&&o.slideFactor>0.2f); // 70% resistance damps the slide
     CK(o.dragScale>.3f&&o.dragScale<.4f);} // 35% ragdoll drag
    {ifeel::Params free{};free.slideResistance=0;
     ifeel::State s{};auto o=ifeel::plan(free,blade(1.f,2000),s,2000);
     CK(o.slideFactor==1.f);} // frictionless: the blade follows 1:1
    {ifeel::Params full{};full.slideResistance=100;
     ifeel::State s{};auto o=ifeel::plan(full,blade(1.f,3000),s,3000);
     CK(o.slideFactor<.3f);} // full resistance: hard damp
    {ifeel::State s{};s.serial=1;s.exitDepth=1.f;
     auto o=ifeel::plan(p,blade(1.f,4000),s,4000);
     CK(o.exitTug>.5f);} // the final tug at the exit
    {ifeel::State s{};auto o=ifeel::plan(p,blade(.0f,5000),s,5000);
     CK(!o.freefall);} // just stopped: still pinned
    {ifeel::State s{};ifeel::plan(p,blade(.0f,6000),s,6000);
     auto o=ifeel::plan(p,blade(.0f,7000),s,7000);
     CK(!o.freefall);} // 1000ms stationary: gravity owns the corpse
    {ifeel::State s{};ifeel::plan(p,blade(.0f,8000),s,8000);
     auto o=ifeel::plan(p,blade(1.f,8100),s,8100);
     CK(!o.freefall);} // any motion rearms at once
    {ifeel::Blade b=blade(1.f,9000);b.ragdoll=false;
     ifeel::State s{};auto o=ifeel::plan(p,b,s,9000);
     CK(o.dragScale==1.f);} // a living victim is dragged at full strength
    {ifeel::State s{};auto b=blade(0.f,10000);b.gripHeld=false;CK(ifeel::plan(p,b,s,10000).freefall);}
    {ifeel::State s{};CK(!ifeel::plan(p,blade(1.f,10000),s,10081).valid);}
    {ifeel::State s{};auto b=blade(NAN,10000);CK(!ifeel::plan(p,b,s,10000).valid);}
    if(fails)return 1;std::puts("impale feel F108 OK");return 0;
}
