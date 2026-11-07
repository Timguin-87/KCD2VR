// R120: pickup Use fallback decision (game interactor did not run the item Use).
#include "../src/vrik/pickup_use_fallback_r120.h"
#include <cstdio>
namespace pf=kcd2vr::pickup_use_fallback_r120;
static int fails=0;
#define CHECK(c) do{if(!(c)){std::printf("FAIL %s:%d %s\n",__FILE__,__LINE__,#c);++fails;}}while(0)
int main(){
    CHECK(pf::read().serial==0);
    pf::note(525233,0,1000);auto p=pf::read();CHECK(p.serial==1&&p.target==525233&&p.side==0&&p.tick==1000);
    using S=pf::Step;
    CHECK(pf::decide(p,0,1300,true,true,false,true)==S::Wait);                 // the game still has time
    CHECK(pf::decide(p,0,1700,true,true,false,true)==S::Act);                  // 700 ms, nothing took it
    CHECK(pf::decide(p,0,1700,false,true,false,true)==S::Drop);                // switch OFF: never
    CHECK(pf::decide(p,0,1300,true,true,true,true)==S::Drop);                  // native Picking / held / gone
    CHECK(pf::decide(p,0,1700,true,true,false,false)==S::Drop);                // grip opened
    CHECK(pf::decide(p,0,1700,true,false,false,true)==S::Drop);                // menu / dialogue
    CHECK(pf::decide(p,0,1000+pf::kMaxMs+1,true,true,false,true)==S::Drop);    // too old
    CHECK(pf::decide(p,p.serial,1700,true,true,false,true)==S::Wait);          // already decided: once per squeeze
    pf::note(0,0,2000);CHECK(pf::read().serial==1);                            // no target: ignored
    pf::note(525285,1,3000);p=pf::read();CHECK(p.serial==2&&p.side==1);        // new squeeze = new attempt (no latch)
    CHECK(pf::decide(p,1,3700,true,true,false,true)==S::Act);
    if(fails)return 1;std::puts("pickup_use_fallback_r120 OK");return 0;
}
