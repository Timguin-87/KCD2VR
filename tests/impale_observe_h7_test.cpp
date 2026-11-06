#include "subsystems/gore/impale_observe_h7.h"
#include <cassert>
#include <cstdio>
using namespace kcd2vr::impale_observe_h7;
int main(){Identity id{0x100000007ULL,2,3,4,5,6,7};Schedule s;unsigned n=99;
 s.begin(id,1000);assert(s.id.serial==id.serial&&s.take(1000,2,3,4,true,n)&&n==0);
 assert(!s.take(1001,2,3,4,true,n));assert(s.take(1500,2,3,4,true,n)&&n==1);
 assert(s.take(2500,2,3,4,true,n)&&n==2);assert(s.take(4000,2,3,4,true,n)&&n==3);
 assert(s.take(7000,2,3,4,true,n)&&n==4&&!s.active);
 s.begin(id,1000);assert(s.take(4050,2,3,4,true,n)&&n==0&&s.next==4);assert(!s.take(4051,2,3,4,true,n));
 for(int gate=0;gate<6;++gate){s.begin(id,1000);assert(!s.take(gate==0?999:gate==1?7501:1000,gate==2?8:2,gate==3?8:3,gate==4?8:4,gate!=5,n));assert(!s.active);}
 s.begin({},1000);assert(!s.active);std::puts("H7 impale observation schedule: identity, epochs, no rewind, no historical catch-up PASS");
}
