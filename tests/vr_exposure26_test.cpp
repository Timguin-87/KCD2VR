// C26d head-steady eye adaptation: pure level logic.
#include "vr_exposure26.h"
#include <cstdio>
#include <cmath>
namespace e=kcd2vr::exposure26;
static int checks=0,fails=0;
static void ck(bool b,const char* m){++checks;if(!b)++fails;std::printf("%s %s\n",b?"PASS":"FAIL",m);}
static e::Q yawPitch(double yawDeg,double pitchDeg){ // rotation: yaw about Y then pitch about X
    const double y=yawDeg*3.14159265358979/360,p=pitchDeg*3.14159265358979/360;
    const double cy=std::cos(y),sy=std::sin(y),cp=std::cos(p),sp=std::sin(p);
    return {float(cy*sp),float(sy*cp),float(-sy*sp),float(cy*cp)};
}
int main(){
    e::State s{};std::uint64_t t=1000;
    for(int i=0;i<40;++i){e::Step(s,true,yawPitch(0,0),t);t+=11;}
    ck(s.level==e::Level::Native,"steady level head = native adaptation");
    for(int i=0;i<10;++i){e::Step(s,true,yawPitch(i*2.0,0),t);t+=11;} // ~180 deg/s
    ck(s.level==e::Level::Paused,"fast head turn pauses adaptation");
    for(int i=0;i<10;++i){e::Step(s,true,yawPitch(20,0),t);t+=11;}
    ck(s.level==e::Level::Paused,"stays paused until calm for 300 ms");
    for(int i=0;i<40;++i){e::Step(s,true,yawPitch(20,0),t);t+=11;}
    ck(s.level==e::Level::Native,"calm 300 ms -> native again");
    for(int i=0;i<60;++i){e::Step(s,true,yawPitch(20,-50),t);t+=11;}
    ck(s.level==e::Level::Slow,"looking far down with a steady head = slow (x0.25)");
    for(int i=0;i<60;++i){e::Step(s,true,yawPitch(20,-10),t);t+=11;}
    ck(s.level==e::Level::Native,"back to level = native");
    ck(std::fabs(e::SpeedFor(e::Level::Slow,3.f)-.75f)<1e-6&&e::SpeedFor(e::Level::Paused,3.f)<.02f&&e::SpeedFor(e::Level::Native,3.f)==3.f,"speeds");
    const auto before=s.level;e::Step(s,true,yawPitch(90,0),t+500);ck(s.level==before,"a stale gap never counts as a turn");
    std::printf("%d/%d\n",checks-fails,checks);return fails?1:0;
}
