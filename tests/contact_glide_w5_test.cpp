// W5: expired free-hand contact glides back instead of a one-frame jump.
#include "vrik/contact_glide_w5.h"
#include <cstdio>
#include <cmath>
struct V {float x=0,y=0,z=0;};
static int total=0,failed=0;static void check(bool v,const char* n){++total;if(!v)++failed;std::printf("%s %s\n",v?"PASS":"FAIL",n);}
int main(){
    namespace g=kcd2vr::contact_glide_w5;
    V a{0,0,0},goal{.075f,0,0};int calls=0;
    while(g::step(a,goal,1.f)&&calls<100)++calls;
    check(calls>=6&&calls<=7,"75 mm (the 10:32:11 jump) closes in 6-7 calls, not 1");
    check(std::fabs(goal.x-a.x)<=.002f+1e-6f,"ends within 2 mm, then the old reset runs");
    V b{0,0,0},far{1,0,0};check(!g::step(b,far,1.f)&&b.x==0,"a 1 m gap is not a contact: no glide (old reset)");
    V c{0,0,0},near{.001f,0,0};check(!g::step(c,near,1.f),"within 2 mm: done");
    V d{0,0,0},z{0,0,.05f};check(g::step(d,z,1.f)&&std::fabs(d.z-1.f/90.f)<1e-5f,"one 90 Hz step is 11.1 mm (1 m/s)");
    for(float rate:{45.f,90.f,180.f}){V p{0,0,0},q{.075f,0,0};float ms=0;int n2=0;while(g::step(p,q,1.f,1000.f/rate)&&n2<400){++n2;ms+=1000.f/rate;}
        check(ms>=66.f&&ms<=90.f,"R8b: 75 mm release takes ~75 ms at 45/90/180 calls per second (time based)");}
    V h{0,0,0},hg{.05f,0,0};check(g::step(h,hg,1.f,0.f)&&h.x==0,"R8b: a repeated callback in the same pass (dt 0) holds, no extra step");
    V e{0,0,0},w{.05f,0,0};check(!g::step(e,w,0.f)&&!g::step(e,w,NAN),"bad scale refuses");
    V f{0,0,0},s2{.05f,0,0};check(g::step(f,s2,2.f)&&std::fabs(f.x-2.f/90.f)<1e-5f,"model scale scales the step");
    // J26 fast release (F2 rung 2/3): only an unconstrained leftover (no plane, no hit) qualifies.
    check(g::fastRelease(true,0,false)&&!g::fastRelease(false,0,false)&&!g::fastRelease(true,1,false)&&!g::fastRelease(true,0,true),"J26: fast only when enabled, no plane, no hit");
    for(float rate:{45.f,62.5f,90.f}){V p{0,0,0},q{.40f,0,0};float ms=0;int n3=0;while(g::stepFast(p,q,1.f,1000.f/rate)&&n3<400){++n3;ms+=1000.f/rate;}
        check(ms<=200.f&&std::fabs(q.x-p.x)<=.002f+1e-6f,"J26: a 40 cm leftover is gone in < 0.2 s at 45/62/90 passes per s");}
    {V p{0,0,0},q{.40f,0,0};float ms=0;while(ms<75.f&&g::stepFast(p,q,1.f,16.f))ms+=16.f;check(q.x-p.x<.40f*.06f,"J26: ~95% of the offset gone after ~75 ms of 16 ms passes");}
    {V p{0,0,0},q{.10f,0,0};V old=p;g::step(old,q,1.f,16.f);g::stepFast(p,q,1.f,16.f);check(p.x>old.x,"J26: faster than the 1 m/s glide (16 mm per 16 ms pass)");}
    {V p{0,0,0},q{.05f,0,0};check(g::stepFast(p,q,1.f,0.f)&&p.x==0,"J26: same pass (dt 0) holds the point");}
    {V p{0,0,0},q{.05f,0,0};g::stepFast(p,q,1.f,16.f);check(p.x>0&&p.x<q.x,"J26: never overshoots or pushes away");}
    {V p{0,0,0},q{1.f,0,0};check(!g::stepFast(p,q,1.f,16.f),"J26: 1 m gap is not a contact (old reset)");}
    std::printf("contact_glide_w5 %d/%d\n",total-failed,total);return failed?1:0;
}
