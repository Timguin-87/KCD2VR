#include "theater_exit113.h"
#include <cassert>
int main() {
    using kcd2vr::theater_exit113::update;
    bool wait=false; std::uint64_t baseline=0;
    assert(update(true,true,2,10,wait,baseline) && !wait);
    // Supplied V117R1 keeps projection submitted below the Theater layer.
    // Closing Theater must release immediately even without producer progress.
    assert(!update(true,false,0,10,wait,baseline) && !wait && baseline==0);
    for(int i=0;i<100;++i) assert(!update(true,false,0,10,wait,baseline) && !wait && baseline==0);
    assert(!update(true,false,0,11,wait,baseline) && !wait);
    assert(!update(true,false,2,11,wait,baseline) && !wait);
    // Menu reopening while already in mode 0 restarts normal Theater drain.
    assert(!update(true,false,0,11,wait,baseline) && !wait);
    assert(update(true,true,0,11,wait,baseline) && !wait && baseline==0);
    assert(!update(true,false,0,13,wait,baseline) && !wait && baseline==0);
    assert(!update(false,false,0,13,wait,baseline) && !wait);
    assert(!update(true,false,-1,13,wait,baseline));
    // Retired wait state cannot keep a foreground layer alive after close.
    wait=true;baseline=50;
    assert(!update(true,false,0,50,wait,baseline)&&!wait&&baseline==0);
    assert(kcd2vr::theater_exit113::drain(true,false));
    assert(!kcd2vr::theater_exit113::drain(false,false));
    assert(!kcd2vr::theater_exit113::drain(false,true));
}
