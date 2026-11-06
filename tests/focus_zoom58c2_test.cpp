#include "../src/focus_zoom58c2.h"
#include <cassert>
struct IO {float value=15;unsigned gets=0,sets=0;bool available=true,write=true;
 bool get(float& v){++gets;v=value;return available;}bool set(float v){++sets;if(write)value=v;return write;}};
int main(){using kcd2vr::focus_zoom58c2::Lease;Lease l;IO io;
 assert(!l.step(false,1,io).attempted&&io.gets==0);
 assert(l.step(true,2,io).ok&&io.value==0&&l.owned);auto calls=io.gets;
 for(unsigned i=3;i<10000;++i)assert(!l.step(true,i,io).attempted);
 assert(io.gets==calls);assert(l.step(false,10000,io).ok&&io.value==15&&!l.owned);
 l.step(true,10001,io);io.value=9;assert(l.step(false,10002,io).ok&&io.value==9&&!l.owned);
 l={};io={};io.available=false;
 assert(!l.step(true,1,io).ok);assert(!l.step(true,100,io).attempted);
 l.step(true,501,io);l.step(true,1001,io);assert(!l.step(true,1501,io).attempted&&io.gets==3);
 l={};io={};io.write=false;assert(!l.step(true,1,io).ok&&l.owned);
 io.write=true;assert(l.step(false,2,io).ok&&io.value==15&&!l.owned);
 l={};io={};io.value=0;assert(l.step(true,1,io).ok&&io.sets==0);
 assert(l.step(false,2,io).ok&&io.sets==0);
}
