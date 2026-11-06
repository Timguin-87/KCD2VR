#include "subsystems/vrmap/runtime/native_map_live_h28.h"
#include <cassert>
#include <cstdio>
using namespace kcd2vr::native_map_live_h28;
int main(){Lease l{};assert(!Wanted()&&!Current(100,1,l));
Request(1,100,1);Publish(101,0,5,true);assert(!Wanted());Publish(102,0x1234,5,true);assert(Current(102,1,l)&&l.serial==1&&l.reading==0x1234&&l.lifetime==5);
assert(!Current(253,1,l));Publish(254,0x1234,5,true);assert(Current(254,1,l));
Publish(255,0x1234,5,false);assert(until==0&&clear.exchange(false));Publish(500,0x1234,5,true);assert(Current(500,1,l)); // pause retains exact native body
Publish(501,0x1234,6,true);assert(!Current(501,1,l)&&armed==0); // same pointer, new lifecycle cannot inherit
Request(2,600,1);Publish(5601,0,7,true);assert(armed==0); // no native read accepted
Request(3,6000,1);Publish(6001,0x2345,8,true);assert(!Current(6001,2,l)&&armed==0); // replacementVM never replays
Request(4,7000,2);Publish(7001,0x3333,9,true);Finish(3);assert(Current(7001,2,l)&&armed==4); // old retirement cannot spend new request
Publish(7002,0,10,true);assert(armed==0&&!Current(7002,2,l));
std::puts("H28 actual native-book arm/reading lease: no idle service, expiry, pause, exact lifecycle, VM replacement and no-replay PASS");}
