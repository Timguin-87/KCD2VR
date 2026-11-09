// R148 host test: start/load recenter when the kept camera forward points away from you (SteamVR start behind you).
// g++ -std=c++20 -I src -I src/vrik tests/start_facing_r148_test.cpp
#include "vrik/start_facing_r148.h"
#include "vrik/tracking_origin125.h"
#include "subsystems/experimental/live_settings_d39.h"
#include <cstdio>
#include <cmath>
#include <string_view>
using namespace kcd2vr;
static int fails=0;
#define CHECK(c) do{if(!(c)){std::printf("FAIL line %d: %s\n",__LINE__,#c);++fails;}}while(0)
struct Q{float x,y,z,w;};
static Q yawQ(float deg){const float a=deg/57.2957795f;return {0,std::sin(a*.5f),0,std::cos(a*.5f)};}
int main(){
    namespace sf=start_facing_r148;namespace sr=start_r111;using sr::Act;
    // origin yaw (yaw-only quaternion from tracking125::recenter) and head yaw use the same axis and sign
    for(float deg:{-170.f,-90.f,-30.f,0.f,45.f,120.f,179.f}){
        const Q h=yawQ(deg);
        const auto o=tracking125::recenter({0,0,0},{h.x,h.y,h.z,h.w});
        const float oy=sf::originYawDeg(float(o.yaw.x),float(o.yaw.y),float(o.yaw.z),float(o.yaw.w));
        CHECK(sr::yawDelta(oy,sr::yawDeg(h.x,h.y,h.z,h.w))<.5f);
    }
    // a pitched head (looking down 30 deg) keeps its yaw for the check
    {const float p=-30.f/57.2957795f;const Q pitch{std::sin(p*.5f),0,0,std::cos(p*.5f)};const Q y=yawQ(170.f);
     const Q q{y.w*pitch.x+y.x*pitch.w+y.y*pitch.z-y.z*pitch.y,y.w*pitch.y-y.x*pitch.z+y.y*pitch.w+y.z*pitch.x,
               y.w*pitch.z+y.x*pitch.y-y.y*pitch.x+y.z*pitch.w,y.w*pitch.w-y.x*pitch.x-y.y*pitch.y-y.z*pitch.z};
     CHECK(sr::yawDelta(sr::yawDeg(q.x,q.y,q.z,q.w),170.f)<1.f);}
    // VDXR / origin already right: neck turns never trigger (<= 90 deg)
    CHECK(!sf::facingAway(true,true,0.f,0.f));
    CHECK(!sf::facingAway(true,true,60.f,0.f));
    CHECK(!sf::facingAway(true,true,-89.f,0.f));
    CHECK(!sf::facingAway(true,true,-170.f,170.f)); // 20 deg across the wrap
    // SteamVR: start forward behind or beside-behind you
    CHECK(sf::facingAway(true,true,180.f,0.f));
    CHECK(sf::facingAway(true,true,-170.f,10.f));
    CHECK(sf::facingAway(true,true,100.f,0.f));
    // untracked / not steady / NaN never recenter
    CHECK(!sf::facingAway(false,true,180.f,0.f));
    CHECK(!sf::facingAway(true,false,180.f,0.f));
    CHECK(!sf::facingAway(true,true,NAN,0.f));
    CHECK(!sf::facingAway(true,true,180.f,NAN));
    // with the R111/R112 episode: head away = Recenter at once (no 10 s wait), still only after the 1.5 s settle
    {sr::Episode e{};std::uint64_t t=5000;
     CHECK(sr::decide(e,3,false,true,t,false,true,true,true,false,true)==Act::Wait);
     CHECK(sr::decide(e,3,false,true,t+1000,false,true,true,true,false,true)==Act::Wait);
     CHECK(sr::decide(e,3,false,true,t+1600,false,true,true,true,false,true)==Act::Recenter);}
    // F8 calibration running: wait, even with the head away
    {sr::Episode e{};std::uint64_t t=5000;(void)sr::decide(e,4,false,true,t,false,true,true,true,true,true);
     CHECK(sr::decide(e,4,false,true,t+3000,false,true,true,true,true,true)==Act::Wait);}
    // not away and not at center: unchanged R112 = keep origin after 10 s; the next load decides again (no latch)
    {sr::Episode e{};std::uint64_t t=5000;(void)sr::decide(e,5,false,true,t,false,true,true,true,false,false);
     CHECK(sr::decide(e,5,false,true,t+1600,false,true,true,true,false,false)==Act::Wait);
     CHECK(sr::decide(e,5,false,true,t+11700,false,true,true,true,false,false)==Act::KeepOrigin);sr::finished(e,5);
     CHECK(sr::decide(e,5,false,true,t+12000,false,true,true,true,false,true)==Act::None);
     (void)sr::decide(e,6,false,true,t+20000,false,true,true,true,false,true);
     CHECK(sr::decide(e,6,false,true,t+21600,false,true,true,true,false,true)==Act::Recenter);}
    // switch OFF (VrStartAtHenryEyesR111=false): no load recenter, as before
    {sr::Episode e{};std::uint64_t t=5000;(void)sr::decide(e,7,false,true,t,false,false,true,true,false,true);
     CHECK(sr::decide(e,7,false,true,t+1600,false,false,true,true,false,true)==Act::None);}
    // part 2: start / menu screen (Theater quad) re-placed when you look away from it
    {const Q z=yawQ(0.f),back=yawQ(180.f),side=yawQ(70.f),neck=yawQ(50.f);
     auto F=[](Q q){return sf::forwardOf(q.x,q.y,q.z,q.w);};
     CHECK(std::fabs(sf::angleDeg(F(z),F(z)))<.5f);CHECK(std::fabs(sf::angleDeg(F(z),F(back))-180.f)<.5f);
     CHECK(std::fabs(sf::pitchDeg(F(z)))<.5f);
     {const float p=-60.f/57.2957795f;const Q down{std::sin(p*.5f),0,0,std::cos(p*.5f)};CHECK(std::fabs(sf::pitchDeg(F(down))+60.f)<.5f);}
     std::uint64_t since=0,t=1000;
     // screen behind you (SteamVR start): replaced after 1.5 s of a tracked level head
     const float a=sf::angleDeg(F(back),F(z));
     CHECK(!sf::uiLookAway(since,true,false,true,a,0.f,t));CHECK(!sf::uiLookAway(since,true,false,true,a,0.f,t+1400));
     CHECK(sf::uiLookAway(since,true,false,true,a,0.f,t+1500));
     // looking back at it resets the timer
     since=0;CHECK(!sf::uiLookAway(since,true,false,true,a,0.f,t));CHECK(!sf::uiLookAway(since,true,false,true,10.f,0.f,t+800));
     CHECK(since==0);CHECK(!sf::uiLookAway(since,true,false,true,a,0.f,t+900));CHECK(!sf::uiLookAway(since,true,false,true,a,0.f,t+2000));
     CHECK(sf::uiLookAway(since,true,false,true,a,0.f,t+2400));
     // a neck turn of 50 deg never moves it, 70 deg does
     since=0;for(std::uint64_t u=0;u<=3000;u+=100)CHECK(!sf::uiLookAway(since,true,false,true,sf::angleDeg(F(neck),F(z)),0.f,t+u));
     since=0;bool moved=false;for(std::uint64_t u=0;u<=3000;u+=100)moved|=sf::uiLookAway(since,true,false,true,sf::angleDeg(F(side),F(z)),0.f,t+u);CHECK(moved);
     // looking down at controllers / lap (pitch -50) never moves it
     since=0;for(std::uint64_t u=0;u<=3000;u+=100)CHECK(!sf::uiLookAway(since,true,false,true,a,-50.f,t+u));
     // untracked, switch off, or your own runtime recenter anchor: never
     since=0;for(std::uint64_t u=0;u<=3000;u+=100){CHECK(!sf::uiLookAway(since,true,false,false,a,0.f,t+u));}
     since=0;for(std::uint64_t u=0;u<=3000;u+=100){CHECK(!sf::uiLookAway(since,false,false,true,a,0.f,t+u));}
     since=0;for(std::uint64_t u=0;u<=3000;u+=100){CHECK(!sf::uiLookAway(since,true,true,true,a,0.f,t+u));}
     since=0;CHECK(!sf::uiLookAway(since,true,false,true,NAN,0.f,t));CHECK(!sf::uiLookAway(since,true,false,true,a,NAN,t));
     CHECK(!std::isfinite(sf::angleDeg(sf::forwardOf(NAN,0,0,1),F(z))));}
    // runtime name for the log row
    CHECK(std::string_view(sf::runtime())=="<unknown>");sf::setRuntime("SteamVR/OpenXR");CHECK(std::string_view(sf::runtime())=="SteamVR/OpenXR");
    sf::setRuntime(nullptr);CHECK(std::string_view(sf::runtime())=="<unknown>");
    // the Experiments row that owns it exists (BODY page 2) and is ON by default
    {live_settings_d39::Settings d{};CHECK(d.startHenryEyesR111);bool row=false;
     for(const auto& r:live_settings_d39::kBodyTrialsD76)if(r.id==live_settings_d39::Id::StartHenryEyesR111)row=true;CHECK(row);}
    if(fails){std::printf("start_facing_r148_test: %d FAIL\n",fails);return 1;}
    std::printf("start_facing_r148_test: OK\n");return 0;
}
