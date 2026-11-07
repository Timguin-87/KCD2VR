// R111 host test: calibrated = VRIK at start/load (one decision per load episode, retry, his choice kept),
// start at Henry's eyes (no standing-eye shift), menu rows exist. g++ -std=c++20 -I src tests/start_vrik_r111_test.cpp
#include "vrik/start_r111.h"
#include "vrik/calibration_restore43.h"
#include "subsystems/experimental/live_settings_d39.h"
#include <cstdio>
#include <cmath>
using namespace kcd2vr;
static int fails=0;
#define CHECK(c) do{if(!(c)){std::printf("FAIL line %d: %s\n",__LINE__,#c);++fails;}}while(0)
int main(){
    namespace sr=start_r111;
    using sr::Act;
    // game start: main menu (world not ready) does nothing
    sr::Episode e{};std::uint64_t t=1000;
    CHECK(sr::decide(e,1,false,false,t,true,true,true,false,false)==Act::None);
    // save load: loading -> nothing, then world up -> wait 1.5 s, then start VRIK
    CHECK(sr::decide(e,2,true,false,t,true,true,true,false,false)==Act::None);
    CHECK(sr::decide(e,2,false,true,t+=10,true,true,true,false,false)==Act::Wait);
    CHECK(sr::decide(e,2,false,true,t+=1000,true,true,true,false,false)==Act::Wait);
    CHECK(sr::decide(e,2,false,true,t+=600,true,true,true,false,false)==Act::StartVrik);
    // refused: retry only after 2 s (no latch)
    CHECK(sr::decide(e,2,false,true,t+=100,true,true,true,false,false)==Act::Wait);
    CHECK(sr::decide(e,2,false,true,t+=2000,true,true,true,false,false)==Act::StartVrik);
    CHECK(e.tries==2);
    sr::finished(e,2);
    CHECK(sr::decide(e,2,false,true,t+=100,true,true,true,true,false)==Act::None);
    // he switches VRIK off in this episode: kept
    sr::manual(e,2);
    CHECK(sr::decide(e,2,false,true,t+=5000,true,true,true,false,false)==Act::None);
    // next load: on again
    CHECK(sr::decide(e,3,true,false,t+=10,true,true,true,false,false)==Act::None);
    CHECK(sr::decide(e,3,false,true,t+=10,true,true,true,false,false)==Act::Wait);
    CHECK(sr::decide(e,3,false,true,t+=1600,true,true,true,false,false)==Act::StartVrik);
    // VRIK already on after a load: only the Henry-eyes recenter
    sr::Episode f{};
    CHECK(sr::decide(f,4,false,true,t,true,true,true,true,false)==Act::Wait);
    CHECK(sr::decide(f,4,false,true,t+1600,true,true,true,true,false)==Act::Recenter);
    // no calibration, eyes OFF: nothing, episode closed
    sr::Episode g{};(void)sr::decide(g,5,false,true,t,true,false,false,false,false);
    CHECK(sr::decide(g,5,false,true,t+1600,true,false,false,false,false)==Act::None);CHECK(g.done==5);
    // F8 running: wait
    sr::Episode h{};(void)sr::decide(h,6,false,true,t,true,true,true,false,true);
    CHECK(sr::decide(h,6,false,true,t+1600,true,true,true,false,true)==Act::Wait);
    // world drop resets the settle timer
    sr::Episode k{};(void)sr::decide(k,7,false,true,t,true,true,true,false,false);
    CHECK(sr::decide(k,7,false,false,t+1000,true,true,true,false,false)==Act::None);
    CHECK(sr::decide(k,7,false,true,t+1600,true,true,true,false,false)==Act::Wait);
    // the 5 Nov 20:33:21 recenter: seated, currentEye 1.141, standing 1.669 -> C43 shifted the view 0.528 m down.
    const float c43=calibration_restore43::verticalShift(true,true,1.141f,1.669f);
    CHECK(std::fabs(c43+.528f)<1e-3f);
    CHECK(sr::recenterShift(true,c43)==0.f);          // R111 ON: Henry's eyes
    CHECK(std::fabs(sr::recenterShift(false,c43)-c43)<1e-6f); // OFF: old C43 behaviour
    // R112: the automatic recenter waits for a steady head at the runtime center (R111 log 21:36:26 / 21:41:09)
    {
        sr::Episode r{};std::uint64_t u=50000;
        CHECK(sr::decide(r,8,false,true,u,true,true,true,false,false,false)==Act::Wait);
        CHECK(sr::decide(r,8,false,true,u+=1600,true,true,true,false,false,false)==Act::StartVrik); // VRIK on at once
        sr::started(r);
        // VRIK is on, head not at the center: wait, no second start even if the mode reads off for a tick
        CHECK(sr::decide(r,8,false,true,u+=10,true,true,true,true,false,false)==Act::Wait);
        CHECK(sr::decide(r,8,false,true,u+=10,true,true,true,false,false,false)==Act::Wait);
        CHECK(sr::decide(r,8,false,true,u+=500,true,true,true,true,false,true)==Act::Recenter);
        sr::finished(r,8);CHECK(sr::decide(r,8,false,true,u+=10,true,true,true,true,false,true)==Act::None);
        // start recenter also without the eyes setting (same as the menu VRIK button)
        sr::Episode q{};(void)sr::decide(q,9,false,true,u,true,false,true,false,false,true);
        CHECK(sr::decide(q,9,false,true,u+1600,true,false,true,false,false,true)==Act::StartVrik);sr::started(q);
        CHECK(sr::decide(q,9,false,true,u+1610,true,false,true,true,false,true)==Act::Recenter);
        // load with VRIK on, head 18 cm to the side for 10 s: origin kept, episode closed (no jump)
        sr::Episode w{};std::uint64_t v=90000;(void)sr::decide(w,10,false,true,v,true,true,true,true,false,false);
        CHECK(sr::decide(w,10,false,true,v+=1600,true,true,true,true,false,false)==Act::Wait);
        CHECK(sr::decide(w,10,false,true,v+=9000,true,true,true,true,false,false)==Act::Wait);
        // world drops (menu): the 10 s window restarts
        CHECK(sr::decide(w,10,false,false,v+=10,true,true,true,true,false,false)==Act::None);
        CHECK(sr::decide(w,10,false,true,v+=10,true,true,true,true,false,false)==Act::Wait);
        CHECK(sr::decide(w,10,false,true,v+=1600,true,true,true,true,false,false)==Act::Wait);
        CHECK(sr::decide(w,10,false,true,v+=10000,true,true,true,true,false,false)==Act::KeepOrigin);
        sr::finished(w,10);
        // next load: decides again (no latch)
        CHECK(sr::decide(w,11,true,false,v+=10,true,true,true,true,false,true)==Act::None);
        CHECK(sr::decide(w,11,false,true,v+=10,true,true,true,true,false,true)==Act::Wait);
        CHECK(sr::decide(w,11,false,true,v+=1600,true,true,true,true,false,true)==Act::Recenter);
        // a pending start recenter does not leak into the next load: that load can start VRIK again
        sr::Episode z{};(void)sr::decide(z,12,false,true,v,true,true,true,false,false,false);
        CHECK(sr::decide(z,12,false,true,v+1600,true,true,true,false,false,false)==Act::StartVrik);sr::started(z);
        (void)sr::decide(z,13,true,false,v+1700,true,true,true,false,false,false);
        (void)sr::decide(z,13,false,true,v+1800,true,true,true,false,false,false);
        CHECK(sr::decide(z,13,false,true,v+3700,true,true,true,false,false,false)==Act::StartVrik);
        // head gate: the R111 captures
        CHECK(!sr::atRuntimeCenter(-.0248f,.0794f,-.4248f,0.f)); // 21:36:26.009 42 cm forward
        CHECK(!sr::atRuntimeCenter(-.1810f,-.0482f,-.0105f,0.f)); // 21:41:09.803 18 cm sideways
        CHECK(sr::atRuntimeCenter(-.0007f,.0014f,-.0017f,2.f));   // 21:36:27.260 after his Oculus recenter
        CHECK(!sr::atRuntimeCenter(0,0,0,40.f));                  // looking to the side
        CHECK(!sr::atRuntimeCenter(0,.30f,0,0.f));                // stood up after a seated recenter
        CHECK(std::fabs(sr::yawDeg(0,0,0,1))<1e-4f);
        CHECK(std::fabs(sr::yawDeg(0,.7071068f,0,.7071068f)-90.f)<.01f);
        sr::Steady s{};
        CHECK(!sr::steady(s,0,0,0,0,1000));
        CHECK(!sr::steady(s,.005f,0,0,1.f,1300));
        CHECK(sr::steady(s,.01f,0,0,2.f,1500));
        CHECK(!sr::steady(s,.05f,0,0,2.f,1600));                  // moved 4 cm: window restarts
        CHECK(!sr::steady(s,.05f,0,0,2.f,2000));
        CHECK(sr::steady(s,.05f,0,0,2.f,2100));
        CHECK(!sr::steady(s,.05f,0,0,12.f,2200));                 // turned 10 deg
    }
    // menu rows
    bool a=false,b=false;for(const auto& r:live_settings_d39::kBodyTrialsD76){a=a||r.id==live_settings_d39::Id::VrikOnStartR111;b=b||r.id==live_settings_d39::Id::StartHenryEyesR111;}
    CHECK(a&&b);
    live_settings_d39::Settings d{};CHECK(d.vrikOnStartR111&&d.startHenryEyesR111);
    CHECK(sr::vrikOnStart.load()&&sr::henryEyes.load());
    std::printf("start_vrik_r111: %s\n",fails?"FAIL":"PASS");return fails;
}
