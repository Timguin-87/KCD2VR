#include "subsystems/vrmap/map_fold_m39.h"
#include "held_map_pose_m1.h"
#include <cassert>
#include <random>
#include <limits>
#include <cstdio>
int main(){
 using kcd2vr::map_fold_m39::Gesture;
 Gesture g;
 assert(g.next(1,100,true,true,1,.01f,.3f).fraction<0); // carried grip cannot acquire
 g.next(1,101,true,true,0,.01f,.3f);
 assert(g.next(1,102,true,true,1,.3f,.3f).fraction<0); // not at paper edge
 assert(g.next(1,103,true,true,1,.01f,.3f).fraction==1);
 assert(g.next(1,104,true,true,1,.01f,.18f).fraction>0);
 assert(!g.next(1,105,true,true,1,.01f,.065f).close);
 assert(!g.next(1,184,true,true,1,.01f,.065f).close);
 assert(g.next(1,185,true,true,1,.01f,.065f).close);
 assert(g.next(2,186,true,true,1,.01f,.065f).fraction<0); // new intent clears ownership
 g.next(2,187,true,true,0,.01f,.3f);g.next(2,188,true,true,1,.01f,.3f);
 assert(g.next(2,189,true,false,1,.01f,.065f).fraction<0); // lost RIGHT ownership/tracking
 assert(!g.next(2,300,true,true,1,.01f,.065f).close);
 g.next(2,301,true,true,0,.01f,.3f);g.next(2,302,true,true,1,.01f,.3f);
 assert(g.next(2,303,true,true,0,.01f,.1f).fraction<0); // release resumes native opening
 assert(!g.next(2,400,false,true,1,.01f,.065f).close);
 assert(g.next(2,401,true,true,1,std::numeric_limits<float>::quiet_NaN(),.3f).fraction<0);
 std::mt19937 rng(39);std::uniform_real_distribution<float> d(-1,1);
 constexpr float grip[]{-.00367412553f,-.002206292f,.1560563594f};
 for(int n=0;n<10000;++n){
  float h[8]{d(rng),d(rng),d(rng),d(rng),100*d(rng),100*d(rng),100*d(rng),1};
  double length=0;for(int k=0;k<4;++k)length+=h[k]*h[k];if(length<.001){--n;continue;}
  for(int k=0;k<4;++k)h[k]/=float(std::sqrt(length));
  float p[8]{},v[3]{};assert(kcd2vr::held_map_pose_m1::Place(h,p));
  kcd2vr::held_map_pose_m1::Point(p,grip,v);
  for(int k=0;k<3;++k)assert(std::fabs(v[k]-h[k+4])<.00002f);
  // model * native grip rotation = socket rotation (q and -q equivalent).
  constexpr float a=.7071067811865476f;
  float q[]{a*(p[3]+p[2]),a*(-p[3]+p[2]),-a*(p[0]+p[1]),a*(-p[0]+p[1])};
  float dot=0;for(int k=0;k<4;++k)dot+=q[k]*h[k];assert(std::fabs(std::fabs(dot)-1)<1e-5);
 }
 std::puts("PASS deliberate free-RIGHT fold, close dwell, ownership loss, release, 10000 native grip transforms");
}
