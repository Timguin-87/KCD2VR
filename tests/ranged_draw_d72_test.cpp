// D76 review: current callback/cleanup behavior is exercised by first_draw_d75_test.cpp.
#include "../src/subsystems/ranged_reload/ranged_draw_policy_d72.h"
#include <cassert>
#include <cstdio>
// D75 moved the actual native visibility adapter/callback coverage to
// tools/run_weapon_draw_d75.py. That fixture includes the real binding readers,
// first metadata-free callback, physical dispatch and T4/crossbow writers.
// Keep the original pure identity/readback contract check here. The former
// 750 ms reveal expectations were the regression, not placement readiness.
namespace draw=kcd2vr::ranged_draw_d72;
int main(){
    const draw::Identity a{1,2,3,4,5,6,7,8,1};
    assert(draw::same(a,a));
    for(int field=0;field<10;++field){auto b=a;switch(field){
        case 0:b.entity=9;break;case 1:b.self=9;break;case 2:b.attachment=9;break;
        case 3:b.key=9;break;case 4:b.generation=9;break;case 5:b.body=9;break;
        case 6:b.request=9;break;case 7:b.id=9;break;case 8:b.side=0;break;case 9:b.generation=0;break;}
        assert(!draw::same(a,b));}
    auto check=draw::verification(true,true,100,0);assert(check.read&&check.periodic);
    check=draw::verification(false,true,120,100);assert(check.read&&!check.periodic);
    assert(!draw::mayReveal(false,true));assert(!draw::mayReveal(true,false));
    assert(draw::mayReveal(check.read,true));
    check=draw::verification(false,false,120,100);assert(!check.read&&!check.periodic);
    check=draw::verification(false,false,600,100);assert(check.read&&check.periodic);
    assert(!draw::mayReveal(check.read,false));
    std::puts("D72 exact identity/readback contract PASS; D75 actual callback suite owns lifecycle regression coverage");
}
