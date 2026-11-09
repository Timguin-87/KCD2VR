// R152: tests for src/xr_local_roll_r152.h (g++ -std=c++17 -I src tests/local_roll_r152_test.cpp)
#include "xr_local_roll_r152.h"
#include <cstdio>
using namespace kcd2vr::local_roll_r152;
static int fails=0,n=0;
#define CHECK(c) do{++n;if(!(c)){++fails;std::printf("FAIL line %d: %s\n",__LINE__,#c);}}while(0)
int main(){
    // his log: q=(-0.019,-0.006,-1.000,-0.014) -> upside down; recentered (0.008,0.0005,-0.0156,0.9998) -> upright
    CHECK(upY(-0.019f,-0.006f,-1.0f,-0.014f)<-0.9);
    CHECK(upY(0.00832f,0.00046f,-0.0156f,0.99984f)>0.99);
    { Watch w; std::uint64_t t=1000; bool any=false;
      for(int i=0;i<44;++i,t+=11) any|=watch(w,true,-1.0,t); // 0.48 s
      CHECK(!any);
      bool fired=false; for(int i=0;i<5;++i,t+=11) fired|=watch(w,true,-1.0,t);
      CHECK(fired); }
    { Watch w; std::uint64_t t=1000; // one upright frame in between restarts the 0.5 s
      for(int i=0;i<40;++i,t+=11) watch(w,true,-1.0,t);
      CHECK(!watch(w,true,0.9,t)); t+=11; bool f=false;
      for(int i=0;i<40;++i,t+=11) f|=watch(w,true,-1.0,t);
      CHECK(!f); }
    { Watch w; std::uint64_t t=1000; bool f=false; // not tracked: never
      for(int i=0;i<200;++i,t+=11) f|=watch(w,false,-1.0,t);
      CHECK(!f); }
    { Watch w; std::uint64_t t=1000; bool f=false; // lying on its side (upY ~0) is not upside down
      for(int i=0;i<200;++i,t+=11) f|=watch(w,true,-0.3,t);
      CHECK(!f); }
    { Watch w; std::uint64_t t=1000; int swaps=0; // headset upside down on the desk: 2 quick swaps, then every 5 s
      for(int i=0;i<1000;++i,t+=10) swaps+=watch(w,true,-1.0,t)?1:0; // 10 s
      CHECK(swaps>=2&&swaps<=4);
      // worn upright 1 s resets the quick count
      for(int i=0;i<120;++i,t+=10) watch(w,true,0.99,t);
      CHECK(w.quickSwaps==0); }
    { Watch w; CHECK(!watch(w,true,0.0/0.0,1000)); }
    std::printf("local_roll_r152: %d/%d\n",n-fails,n);
    return fails?1:0;
}
