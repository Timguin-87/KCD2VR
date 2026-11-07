// DS-F55: hand grip proxy owner - receipt-driven lifecycle (pure, host-tested).
#include "../src/subsystems/physics/runtime/native_hand_proxy_t4.h"
#include <cstdio>
#include <cmath>
namespace hp=kcd2vr::native_hand_proxy_t4;
static int fails=0;
#define CK(c) do{if(!(c)){std::printf("FAIL %s:%d %s\n",__FILE__,__LINE__,#c);++fails;}}while(0)
static hp::Input in(int side,std::uint64_t tick){hp::Input i{};i.tick=tick;
    i.source={1,1,1,tick,0xAAA,side};i.target.p={0,0,0};i.target.q={0,0,0,1};return i;}
static hp::NativeReceipt rec(std::uint64_t inc,bool adopted=false,bool ready=false,bool retired=false){hp::NativeReceipt r{};r.incarnation=inc;r.adopted=adopted;r.ready=ready;r.retired=retired;return r;}
int main(){
    // full receipt-driven lifecycle: requested -> adopted -> creating -> ready -> active -> retire -> retired -> off
    {hp::ProxyPolicy p{};hp::NativeReceipt r{};
     CK(p.update(in(0,100),100,r)==hp::State::Requested);
     CK(p.update(in(0,110),110,rec(1,true,false,false))==hp::State::Creating); // adopted for THIS incarnation
     CK(p.update(in(0,120),120,rec(1,true,true,false))==hp::State::Active);    // ready (later Serial)
     CK(p.update(in(0,130),130,r)==hp::State::Active);                          // steady drive
     CK(p.update({},140,r)==hp::State::Retiring);                              // dead input retires
     CK(p.update({},150,rec(1,false,false,true))==hp::State::Off);             // the retire receipt for the old incarnation
    }
    // a delayed receipt of an OLD incarnation never activates a NEW one
    {hp::ProxyPolicy p{};hp::NativeReceipt r{};
     CK(p.update(in(0,100),100,r)==hp::State::Requested);
     CK(p.update(in(0,110),110,rec(1,true,false,false))==hp::State::Creating);
     auto in2=in(1,120);in2.source.sourceGeneration=2;
     CK(p.update(in2,120,r)==hp::State::Retiring); // source change: retire the old first
     CK(p.update(in2,130,rec(1,true,true,false))==hp::State::Retiring); // a LATE ready receipt for incarnation 1: ignored
     CK(p.update(in2,140,rec(1,false,false,true))==hp::State::Off); // the old teardown completes
     CK(p.update(in2,150,r)==hp::State::Requested); // only NOW the new source starts (incarnation 2)
     CK(p.incarnation==2);
     CK(p.update(in2,160,rec(1,true,false,false))==hp::State::Requested); // a stale adopted for incarnation 1: ignored
     CK(p.update(in2,170,rec(2,true,false,false))==hp::State::Creating); // the new incarnation's own receipt
    }
    // input gates: stale/future tick, zero generations, nonfinite pose (validInput direct)
    {CK(hp::ProxyPolicy::validInput(in(0,100),100));
     auto bad=in(0,59);CK(!hp::ProxyPolicy::validInput(bad,140)); // stale tick (140-59=81 > 80)
     auto fut=in(0,300);CK(!hp::ProxyPolicy::validInput(fut,200)); // future tick
     auto zero=in(0,201);zero.source.sourceGeneration=0;CK(!hp::ProxyPolicy::validInput(zero,201));
     auto pt0=in(0,202);pt0.source.poseTick=0;CK(!hp::ProxyPolicy::validInput(pt0,202));
     auto qzero=in(0,203);qzero.target.q={0,0,0,0};CK(!hp::ProxyPolicy::validInput(qzero,203));
     auto nan=in(0,204);nan.target.p.x=std::nanf("");CK(!hp::ProxyPolicy::validInput(nan,204));}
    // DS-F69: invalid input during Requested RETIRES the incarnation (no pending body)
    {hp::ProxyPolicy p{};hp::NativeReceipt r{};
     CK(p.update(in(0,100),100,r)==hp::State::Requested);
     CK(p.update(in(0,59),140,r)==hp::State::Retiring); // stale -> retire
     CK(p.update(in(0,59),150,rec(1,false,false,true))==hp::State::Off); // the retire receipt
    }
    // fault quarantines WITH the live incarnation until the cleanup receipt
    {hp::ProxyPolicy p{};hp::NativeReceipt r{};
     CK(p.update(in(0,100),100,r)==hp::State::Requested);
     CK(p.update(in(0,110),110,rec(1,true,false,false))==hp::State::Creating);
     p.fault=true;
     CK(p.update(in(0,120),120,r)==hp::State::Quarantined);
     CK(p.incarnation==1); // the lease is retained
     CK(p.update(in(0,130),130,r)==hp::State::Quarantined); // a ready receipt does not clear the fault
     CK(p.update(in(0,140),140,rec(1,false,false,false))==hp::State::Quarantined); // cleanedUp=false: still held
     hp::NativeReceipt clean=rec(1,false,false,false);clean.cleanedUp=true;
     CK(p.update(in(0,150),150,clean)==hp::State::Off); // the audited cleanup releases it
    }
    if(fails)return 1;std::puts("hand proxy owner F55 OK");return 0;
}
