#include "vrik/carry_snapshot59c7g.h"
#include <cassert>
#include <cstring>
using namespace kcd2vr::carry59c7g;
int main(){
    assert(!std::strcmp(phase(5),"CARRYING"));assert(!std::strcmp(phase(0),"IDLE"));
    assert(!std::strcmp(phase(8),"PUTDOWN_PHASE"));assert(!std::strcmp(phase(9),"UNKNOWN"));
    Budget b;assert(!b.accept(0,100,1,2,5));assert(!b.accept(1,100,0,2,5));
    assert(b.accept(1,100,1,2,5));assert(!b.accept(1,101,1,2,5));
    assert(!b.accept(1,99,1,2,5));assert(b.accept(1,101,1,2,6));
    assert(b.accept(1,102,1,0,0));assert(!b.accept(1,103,1,0,9));
    assert(b.accept(1,602,1,0,0));assert(b.accept(1,603,2,3,5));
    for(unsigned i=0;i<100;++i)(void)b.accept(1,604+i,2,3,i%9);
    assert(b.emitted==64);assert(!b.accept(1,900,3,4,5));
    assert(b.accept(2,901,3,4,5)&&b.emitted==1);
}
