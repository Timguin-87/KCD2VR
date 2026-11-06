#include "vrik/staged_hold596c2.h"
#include <cassert>
#include <limits>
#include <cstdio>
using namespace kcd2vr::staged596c2;
int main(){
    Hold h;h.begin(1,1000);
    // Near the chest on pickup never immediately consumes the item.
    for(unsigned t=1020;t<1500;t+=20)assert(h.update(1,t,true,1,.1)==Decision::Hold);
    assert(h.update(1,1500,true,1,.4)==Decision::Hold);
    for(unsigned t=1520;t<1820;t+=20)assert(h.update(1,t,true,1,.1)==Decision::Hold);
    assert(h.update(1,1820,true,1,.1)==Decision::Stow);
    h.begin(1,2000);assert(h.update(1,2020,true,0,.1)==Decision::Drop);
    h.begin(1,2000);assert(h.update(2,2020,true,1,.1)==Decision::Drop);
    h.begin(1,2000);assert(h.update(1,2081,true,1,.1)==Decision::Drop);
    h.begin(1,2000);assert(h.update(1,2020,false,1,.1)==Decision::Drop);
    h.begin(1,2000);assert(h.update(1,2020,true,1,std::numeric_limits<double>::quiet_NaN())==Decision::Drop);
    Loader l;
    assert(l.update(1000,true,false,false,1,true,.1)==Command::None); // held on entry
    assert(l.update(1020,true,false,false,0,true,.1)==Command::None);
    assert(l.update(1040,true,false,false,1,true,.1)==Command::Retrieve);
    assert(l.update(1060,true,false,true,1,true,.1)==Command::None); // cannot nock in quiver
    assert(l.update(1080,true,false,true,1,false,.5)==Command::None);
    for(unsigned t=1100;t<1180;t+=20)assert(l.update(t,true,false,true,1,false,.1)==Command::None);
    assert(l.update(1180,true,false,true,1,false,.1)==Command::Nock);
    assert(l.state==Reload::AwaitNock); // submitted once; command is NOT a receipt
    assert(l.update(1200,true,true,false,1,false,.1)==Command::None);
    assert(l.state==Reload::Ready);
    assert(l.update(1220,true,false,false,1,true,.1)==Command::None); // shot, no auto-reload
    assert(l.state==Reload::Empty);
    assert(l.update(1240,true,false,false,0,true,.5)==Command::None);
    assert(l.update(1260,true,false,false,1,true,.5)==Command::Retrieve);
    assert(l.update(1280,true,false,true,0,false,.5)==Command::RemoveHand);
    assert(l.update(1300,false,false,true,1,false,.1)==Command::RemoveHand);
    l={};l.update(2000,true,false,true,1,false,.5);
    assert(l.update(2081,true,false,true,1,false,.1)==Command::RemoveHand);
    std::puts("PASS: staged hold/stow, loss/epoch cancellation, quiver edge, nock receipt, no automatic reload");
}
