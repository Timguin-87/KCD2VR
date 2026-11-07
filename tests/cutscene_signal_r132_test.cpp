#include "core/cutscene_signal_r132.h"
#include <cassert>
#include <cstring>
int main(){
    using namespace kcd2vr::cutscene_signal_r132;
    Gate g;
    // no sequence: never a cutscene
    assert(!g.update(false,1000));
    // a cut-scene sequence starts: flat only after the 150 ms hold, then stays flat
    assert(!g.update(true,1000)); assert(!g.update(true,1149)); assert(g.update(true,1150)); assert(g.update(true,5000));
    // it ends: immersive the same tick
    assert(!g.update(false,5001));
    // a one-frame camera sequence never flashes Theater
    assert(!g.update(true,6000)); assert(!g.update(false,6011)); assert(!g.update(true,6020)); assert(!g.update(false,6030));
    // next real cutscene works again (no latch), also at tick 0
    Gate z; assert(!z.update(true,0)); assert(z.update(true,200));
    assert(!std::strcmp(Source(false,true),"VIEW_SYSTEM")); assert(!std::strcmp(Source(true,false),"CVAR"));
    assert(!std::strcmp(Source(true,true),"CVAR+VIEW")); assert(!std::strcmp(Source(false,false),"NONE"));
    return 0;
}
