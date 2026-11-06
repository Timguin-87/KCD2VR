#include "../src/subsystems/interactions/native_passenger_a34.h"
#include <cassert>
#include <cstdio>
int main(){namespace n=kcd2vr::native_passenger_a34;
 n::Root r{1,2,3,4,5,6,100,100,1,7,0xeed2e729,8,2,0};
 assert(n::witness.publish(r));assert(n::current(2,6,100));assert(n::bodyCurrent(2,6,100));
 n::trackedWashD72=true;assert(n::current(2,6,100));assert(!n::bodyCurrent(2,6,100));
 assert(n::trackedWashAlias("player_washing_watertube",0,2,6,100));
 assert(!n::trackedWashAlias("player_washing_watertube",1,2,6,100));
 assert(!n::trackedWashAlias("player_washing_other",0,2,6,100));
 assert(!n::trackedWash(2,6,181));assert(!n::trackedWash(2,7,100));assert(!n::trackedWash(3,6,100));
 r.crc=0x591f4107;assert(n::witness.publish(r));assert(n::bodyCurrent(2,6,100));assert(!n::trackedWash(2,6,100));
 r.crc=0xeed2e729;r.status=3;assert(n::witness.publish(r));assert(!n::trackedWash(2,6,100));
 n::trackedWashD72=false;r.status=2;assert(n::witness.publish(r));assert(n::bodyCurrent(2,6,100));
 std::puts("D72 tracked wash exact-owner cases PASS");
}
