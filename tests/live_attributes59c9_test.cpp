#include "vrik/live_attributes5912.h"
#include <cassert>
#include <cstdio>
int main(){
    using namespace kcd2vr::live5912;
    Snapshot exhausted{},cache{},miss{};
    exhausted.player=1;exhausted.entity=2;exhausted.soul=3;exhausted.recovery=4;
    exhausted.tick=1000;exhausted.valid=true;exhausted.maximum=100;exhausted.current=0;exhausted.stamina=0;
    assert(readBounded(cache,true,exhausted,1,2,1000,4)&&cache.stamina==0);
    assert(readBounded(cache,false,miss,1,2,1050,4)&&cache.stamina==0&&cache.tick==1000);
    assert(!readBounded(cache,false,miss,1,2,1101,4)); // never extends freshness
    assert(readBounded(cache,true,exhausted,1,2,1000,4));
    assert(!readBounded(cache,true,miss,1,2,1001,4)); // explicit failure revokes
    assert(readBounded(cache,true,exhausted,1,2,1000,4));
    assert(!readBounded(cache,false,miss,1,2,1001,5)); // new recovery cannot inherit
    assert(readBounded(cache,true,exhausted,1,2,1000,4));
    assert(!readBounded(cache,false,miss,8,2,1001,4)); // changed player cannot inherit
    std::puts("PASS bounded attribute reuse: no freshening, invalid/identity/epoch revoke");
}
