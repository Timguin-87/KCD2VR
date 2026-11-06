#include "../src/vrik/workstation_sample46.h"
#include <cassert>
#include <cstdio>
#include <limits>

struct V {float x=0,y=0,z=0;};
struct Q {float x=0,y=0,z=0,w=1;};
struct Tracking {
    std::uint64_t sequence=7,tick=1000,timeNs=1000000000,source_epoch=3;
    bool head_valid=true,head_held=false,left_valid=true,right_valid=true;
    bool left_held=false,right_held=false,allowed=true;
    V head{0,1.7f,0},left{-.2f,1.2f,-.3f},right{.2f,1.1f,-.4f};
    Q hq{},lq{},rq{};
    float left_grip=.3f,right_grip=.7f,left_trigger=.2f,right_trigger=.4f;
};
int main(){
    namespace ws=kcd2vr::workstation_sample46;
    Tracking t{};ws::Frame f{};
    assert(ws::copy(t,9,3,1000,f)&&ws::fresh(f,1000));
    assert(f.sequence==7&&f.poseEpoch==9&&f.sourceEpoch==3&&f.timeNs==t.timeNs);
    assert(f.hand[0].position[0]==t.right.x&&f.hand[1].position[0]==t.left.x);
    assert(f.hand[0].grip==.7f&&f.hand[1].trigger==.2f);
    t.right_held=true;assert(ws::copy(t,9,3,1000,f)&&!f.hand[0].valid&&f.hand[1].valid);
    t.right_held=false;t.left_valid=false;assert(ws::copy(t,9,3,1000,f)&&!f.hand[1].valid&&f.hand[0].valid);
    t.left_valid=true;t.head_held=true;assert(!ws::copy(t,9,3,1000,f)&&!f.sequence);
    t.head_held=false;assert(!ws::copy(t,9,4,1000,f));
    assert(!ws::copy(t,0,3,1000,f)&&!ws::copy(t,9,3,999,f));
    assert(ws::copy(t,9,3,1080,f)&&!ws::copy(t,9,3,1081,f));
    t.rq.w=2;assert(ws::copy(t,9,3,1000,f)&&!f.hand[0].valid);
    t.rq.w=1;t.right.x=std::numeric_limits<float>::quiet_NaN();
    assert(ws::copy(t,9,3,1000,f)&&!f.hand[0].valid);
    t=Tracking{};t.hq.w=0;assert(!ws::copy(t,9,3,1000,f));
    t=Tracking{};t.allowed=false;assert(ws::copy(t,9,3,1000,f)&&!f.allowed);
    // Copying observed input never grants pose, native input or gameplay authority.
    t.timeNs=0;assert(!ws::copy(t,9,3,1000,f));
    std::puts("workstation_sample46: coherent source, side mapping, epochs, strict no-grace motion and freshness PASS");
}
