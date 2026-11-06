#include "theater_toggle_r124.h"
#include <cassert>
int main(){
    using namespace kcd2vr::theater_toggle_r124;
    State s;
    // gameplay (key 0, no theater): F6 -> flat Theater, no panic
    assert(!Press(s,false,true)); assert(Apply(s,0,false)); assert(Apply(s,0,false));
    // F6 again -> immersive
    assert(!Press(s,true,true)); assert(!Apply(s,0,false)); assert(s.force==Force::None);
    // forced Theater ends when a native menu opens (key change); the menu keeps its own policy
    Press(s,false,true); assert(Apply(s,0,false)); assert(Apply(s,1,true)); assert(s.force==Force::None); assert(!Apply(s,0,false));
    // cutscene Theater (key 100): F6 -> immersive + panic, stays immersive while the cutscene lasts, ends after it
    assert(Press(s,true,true)); assert(!Apply(s,100,true)); assert(!Apply(s,100,true)); assert(Apply(s,0,false)==false); assert(s.force==Force::None);
    assert(Apply(s,100,true)); // next cutscene is Theater again (no latch)
    // menu Theater: panic drops the owner the same tick -> captured key 0; the menu coming back ends the override
    assert(Press(s,true,true)); assert(!Apply(s,0,false)); assert(Apply(s,1,true));
    // L3+R3 click never forces Theater
    State l; assert(!Press(l,false,false)); assert(l.force==Force::None); assert(Press(l,true,false)); assert(!Apply(l,100,true));
    return 0;
}
