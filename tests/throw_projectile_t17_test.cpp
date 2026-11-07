#include "subsystems/throw_drop/projectile_flight_t17.h"
#include <cassert>
#include <limits>
namespace t=kcd2vr::throw_projectile_t17;
int main() {
    t::Flight f{};t::Vec3 out{};
    assert(f.begin({100,200,2}));
    assert(f.observe({120,200,2}));
    assert(!f.tailVelocity({20,0,-3},.01f,out));
    assert(f.observe({121,200,2}));
    assert(f.tailVelocity({20,0,-3},.05f,out));
    assert(out.x<16.38f && out.x>16.37f && out.z==-3); // integrated20->21m ramp, not whole-step endpoint drag
    assert(f.observe({119,200,2})); // no budget reset after a rebound
    assert(f.tailVelocity({0,-20,3},.05f,out));
    assert(out.y<0 && out.y>-14 && out.z>0 && out.z<3);
    assert(!f.tailVelocity({20,0,-3},.06f,out)); // never replay a stalled frame
    assert(!f.observe({std::numeric_limits<float>::quiet_NaN(),0,0}));
    assert(!t::launchVelocity({1,0,0},2,out)); // release is a drop
    assert(t::launchVelocity({-8,0,0},3,out) && out.x==-24); // left/right symmetry
    assert(t::launchVelocity({0,8,0},3,out) && out.y==24);
    // End-to-end numerical experiment: native-like gravity applied exactly
    // once by the simulated owner. Compare free and assisted flight at the
    // same instant, not a mirror of the drag formula.
    t::Flight tail{};assert(tail.begin({0,0,2}));
    t::Vec3 p{0,0,2},v{24,0,4},freeP=p,freeV=v;
    const float dt=.005f;
    for(int n=0;n<250;++n) {
        v.z-=9.81f*dt;freeV.z-=9.81f*dt;
        p.x+=v.x*dt;p.y+=v.y*dt;p.z+=v.z*dt;
        freeP.x+=freeV.x*dt;freeP.z+=freeV.z*dt;
        assert(tail.observe(p));
        if(tail.tailVelocity(v,dt,out))v=out;
    }
    assert(p.x>20 && p.x<25); // effective 20 m, smooth tail; no invisible wall
    assert(freeP.x>29 && v.x<freeV.x*.1f);
    assert(std::abs(p.z-freeP.z)<.0001f); // downward gravity never counteracted
}
