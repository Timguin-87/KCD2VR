// R121 host test: the head look ray starts at the rendered eye only for a first-person eye (vrik/eye_ray_r121.h).
// g++ -std=c++20 -I src tests/eye_ray_r121_test.cpp && ./a.out
#include "vrik/eye_ray_r121.h"
#include <cassert>
#include <cmath>
#include <cstdio>
int main(){
    using namespace kcd2vr::eye_ray_r121;
    const float game[3]{3184.f,490.f,42.65f};
    // R118 log 11:03:41: headset 27 cm forward of the recenter point (+ 4 cm down) -> your eye
    const float lean[3]{3184.f,490.27f,42.61f};
    assert(useRenderedEye(true,game,lean));
    assert(!useRenderedEye(false,game,lean)); // switch OFF = Henry's game eye (R120)
    // the same point = no change, still taken
    assert(useRenderedEye(true,game,game));
    // dialogue / cutscene / station camera 1.5 m away -> not your eye, game eye kept
    const float far[3]{3185.5f,490.f,42.65f};
    assert(!useRenderedEye(true,game,far));
    // just inside / outside the 0.60 m limit
    const float in[3]{3184.f+0.59f,490.f,42.65f},out[3]{3184.f+0.61f,490.f,42.65f};
    assert(useRenderedEye(true,game,in));
    assert(!useRenderedEye(true,game,out));
    // bad numbers or no sample never move the ray
    const float nanEye[3]{NAN,490.f,42.65f};
    assert(!useRenderedEye(true,game,nanEye));
    assert(!useRenderedEye(true,nullptr,lean));
    assert(!useRenderedEye(true,game,nullptr));
    // no latch: a refused call does not change the next decision
    assert(useRenderedEye(true,game,lean));
    assert(enabled.load()); // default ON
    std::puts("eye_ray_r121_test OK");
    return 0;
}
