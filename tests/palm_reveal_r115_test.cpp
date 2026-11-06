// R115: a revealed palm group stays 7 s from the reveal (also after looking away), needs the 1 s dwell first.
#include "../src/ui_hud_experiment36c5.h"
#include <cstdio>
#include <cmath>
using namespace kcd2vr;
static int fails=0;
#define CHECK(x) do{if(!(x)){++fails;std::printf("FAIL %d %s\n",__LINE__,#x);}}while(0)
int main(){
    using I=ui_contextual_hud162r1::Input;
    I in{};in.enabled=in.gameplay=in.headValid=true;in.now=100;in.headOrientation={0,0,0,1};
    const float q=std::sqrt(.5f);
    in.palm[0]={{0,0,-.5f},{0,-q,0,q},true};in.palm[1]={{0,0,-.5f},{0,q,0,q},true};
    std::array<bool,2> open{true,false};
    ui_hud_experiment36c5::PalmReveal r;
    CHECK(!r.Update(in,open).left);                        // first look: dwell starts
    in.now=1099;CHECK(!r.Update(in,open).left);
    in.now=1100;CHECK(r.Update(in,open).left);              // 1 s dwell -> reveal
    in.now=1200;in.headOrientation={0,1,0,0};CHECK(r.Update(in,open).left); // look away: still shown
    in.now=8099;CHECK(r.Update(in,open).left);              // 7 s from the reveal
    in.now=8100;CHECK(!r.Update(in,open).left);             // gone
    in.now=8200;in.headOrientation={0,0,0,1};CHECK(!r.Update(in,open).left); // new glance needs the dwell again
    in.now=9200;CHECK(r.Update(in,open).left);
    in.now=16200;CHECK(!r.Update(in,open).left);            // continuous stare: ends after 7 s
    std::printf(fails?"palm_reveal_r115 FAILED %d\n":"palm_reveal_r115 PASS\n",fails);return fails?1:0;
}
