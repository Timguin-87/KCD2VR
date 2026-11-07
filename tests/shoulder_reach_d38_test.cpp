// D39: legacy threshold observes shoulder travel but never bounds the authoritative wrist.
#include "vrik/shoulder_reach_d38.h"
#include "vrik/upper_body.h"
#include <cstdio>
#include <cmath>
namespace m=kcd2vr::pose_core;namespace sr=kcd2vr::shoulder_reach_d38;
static int n=0,f=0;static void ck(bool b,const char* w){++n;if(!b){++f;std::printf("FAIL %s\n",w);}}
int main(){
 const double up=.2663,fo=.2615;const m::Vec3 sh{.249,.0436,1.3413};
 // D35B 00:37:54 R: 7.8 cm over-reach -> fit moves the root ~7.8 cm, inside the 15 cm bound: unchanged, wrist exact
 const m::Vec3 req{.4391,.0896,.7683};
 const auto fit=m::controller_shoulder103(sh,req,up,fo,m::Vec3{0,0,-1});ck(bool(fit),"fit exists");
 const auto b=sr::observe(sh,fit->shoulder,.15);ck(m::length(b-fit->shoulder)<1e-12,"inside the bound: R38 result untouched");
 ck(std::fabs(m::length(req-b)-fit->distance)<1e-9,"inside the bound: wrist distance = R38 fit");
 // 1.48 x arm (the worst D35B sample): fit remains exact; excess travel is observable
 const m::Vec3 far=sh+m::Vec3{0,.78,0};const auto ff=m::controller_shoulder103(sh,far,up,fo,m::Vec3{0,0,-1});
 const auto c=sr::observe(sh,ff->shoulder,.15);
 ck(m::length(c-ff->shoulder)<1e-12,"far: warning cannot alter fitted root");
 ck(sr::capped.load()>0,"far: threshold still records excessive travel");
 ck(m::length(m::cross(c-sh,ff->shoulder-sh))<1e-9,"far: same direction as R38");
 // close target: fitted shoulder retains a viable fixed-length triangle
 const auto cl=m::controller_shoulder103(sh,sh+m::Vec3{.01,0,0},up,fo,m::Vec3{0,0,-1});
 ck(m::length(sr::observe(sh,cl->shoulder,.02)-cl->shoulder)<1e-12,"close: warning never changes fit");
 ck(m::length(sr::observe(m::Vec3{NAN,0,0},fit->shoulder,.15)-fit->shoulder)==0,"non-finite natural: no change");
 std::printf("shoulder_reach_d38 %d checks, %d failures\n",n,f);return f?1:0;}
