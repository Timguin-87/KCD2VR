// DS-F92: throw policy + release-velocity capture (pure, host-tested).
#include "../src/subsystems/physics/throw_policy60.h"
#include <cstdio>
#include <cmath>
namespace t=kcd2vr::throw60;
static int fails=0;
#define CK(c) do{if(!(c)){std::printf("FAIL %s:%d %s\n",__FILE__,__LINE__,#c);++fails;}}while(0)
int main(){
    float v[3]{};
    const float a[3]{0,0,0},b[3]{0.1f,0.2f,0.2f};
    t::velocity(a,b,1000,1100,v); // 100 ms: (1.0,2.0,2.0) m/s
    CK(std::fabs(v[0]-1.0f)<1e-4f&&std::fabs(v[1]-2.0f)<1e-4f&&std::fabs(v[2]-2.0f)<1e-4f);
    CK(t::shouldThrow(v)); // 3 m/s clears 1.5
    float out[3]{};t::throwVelocity(v,out);
    CK(std::fabs(out[0]-1.0f)<1e-4f); // under the cap: unchanged
    const float fast[3]{8,8,8};t::throwVelocity(fast,out);
    CK(out[0]>0&&out[0]<=t::kMaxThrowSpeed+1e-4f); // capped at 10
    CK(!t::shouldThrow(a)); // a release without speed is a drop
    const float stale[3]{0,0,0};t::velocity(a,b,1000,1200,out); // stale (200 ms): no velocity
    CK(out[0]==0&&out[1]==0&&out[2]==0);
    if(fails)return 1;std::puts("throw policy F92 OK");return 0;
}
