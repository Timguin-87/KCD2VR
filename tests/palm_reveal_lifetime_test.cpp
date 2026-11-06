#include "../src/ui_hud_experiment36c5.h"
#include <cassert>
#include <cstdio>
#include <limits>
using namespace kcd2vr;
template<class Reveal>static void run(){
    using I=ui_contextual_hud162r1::Input;
    I in{};in.enabled=in.gameplay=in.headValid=true;in.now=100;
    in.headOrientation={0,0,0,1};
    const float q=std::sqrt(.5f);
    in.palm[0]={{0,0,-.5f},{0,-q,0,q},true};
    in.palm[1]={{0,0,-.5f},{0,q,0,q},true};
    std::array<bool,2> open{true,false};
    Reveal gaze;
    auto result=gaze.Update(in,open);assert(result.active&&result.left&&!result.right);
    // Leave the gaze immediately: a valid ten-second reveal survives looking away.
    in.now=101;in.headOrientation={0,1,0,0};assert(gaze.Update(in,open).left);
    in.now=10099;assert(gaze.Update(in,open).left);
    in.now=10100;assert(!gaze.Update(in,open).left);
    in.now=10101;in.headOrientation={0,0,0,1};assert(gaze.Update(in,open).left);
    // Continuing to stare does not renew the deadline or retrigger on expiry.
    in.now=20100;assert(gaze.Update(in,open).left);
    in.now=20101;assert(!gaze.Update(in,open).left);
    in.now=35000;assert(!gaze.Update(in,open).left);
    // A new glance can rearm; the two hands have independent deadlines.
    in.now=35001;in.headOrientation={0,1,0,0};assert(!gaze.Update(in,open).left);
    in.now=35002;in.headOrientation={0,0,0,1};assert(gaze.Update(in,open).left);
    in.now=38000;open[1]=true;result=gaze.Update(in,open);assert(result.left&&result.right);
    in.now=45002;result=gaze.Update(in,open);assert(!result.left&&result.right);
    in.now=48000;assert(!gaze.Update(in,open).right);
    // Closing a hand preserves the earned display timer (existing HUD contract).
    {Reveal closed;I fresh=in;fresh.now=100;open={true,true};assert(closed.Update(fresh,open).left);
     fresh.now=200;open[0]=false;assert(closed.Update(fresh,open).left);
     fresh.now=10100;assert(!closed.Update(fresh,open).left);}
    // Ownership/tracking invalidation releases immediately.
    for(unsigned cause=0;cause<9;++cause){
        Reveal trial;I fresh=in;fresh.now=100;open={true,true};
        assert(trial.Update(fresh,open).left);
        fresh.now=200;
        if(cause==0)fresh.enabled=false;
        if(cause==1)fresh.gameplay=false;
        if(cause==2)fresh.headValid=false;
        if(cause==3)fresh.palm[0].valid=false;
        if(cause==4)fresh.head.x=std::numeric_limits<float>::quiet_NaN();
        if(cause==5)fresh.now=50;
        if(cause==6)fresh.headOrientation={0,0,0,0};
        if(cause==7)fresh.palm[0].orientation={0,0,0,0};
        if(cause==8)fresh.palm[0].position.x=std::numeric_limits<float>::quiet_NaN();
        assert(!trial.Update(fresh,open).left);
    }
    // Valid clock arithmetic does not overflow near uint64 maximum.
    Reveal near;in.now=std::numeric_limits<std::uint64_t>::max()-9;
    open={true,true};assert(near.Update(in,open).left);
    in.now=std::numeric_limits<std::uint64_t>::max();assert(near.Update(in,open).left);
    assert(ui_palm171::Open(.2f,.15f,false,false));
    assert(!ui_palm171::Open(.21f,.1f,false,false));
    assert(!ui_palm171::Open(.1f,.1f,true,false));
    std::puts("palm reveal lifetime: 10s hold, no stare rearm, independent hands, invalidation and overflow PASS");
}
int main(){ui_hud_experiment36c5::PalmReveal::dwellMs=0;run<ui_hud_experiment36c5::PalmReveal>();} // dwell tested in hud_experiment36c5
