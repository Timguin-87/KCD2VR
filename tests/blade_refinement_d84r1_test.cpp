#include "vrik/blade_zone_d84.h"
#include <cassert>
#include <cstdint>
struct Owner {bool attackPrepared=true;std::uint64_t attackKey=1,attackGeneration117=2,epoch=3,request=4,prepareGestureC10R18=5,bladeZoneGestureD84=5;int bladeZoneOriginalD84=0;};
struct Command {bool held57=true;std::uint64_t key=1,generation=2,bodyEpoch=3,epoch=4,gesture59c3=5;int zone117=0;};
int main(){using namespace kcd2vr::blade_zone_d84;
 Owner n;Command c;assert(ownsRefinedRelease(n,c));
 for(int fault=0;fault<9;++fault){auto x=c;auto o=n;switch(fault){
 case 0:x.held57=false;break;case 1:o.attackPrepared=false;break;case 2:++x.key;break;
 case 3:++x.generation;break;case 4:++x.bodyEpoch;break;case 5:++x.epoch;break;
 case 6:++x.gesture59c3;break;case 7:++o.bladeZoneGestureD84;break;case 8:x.zone117=2;break;
 }assert(!ownsRefinedRelease(o,x));}
 assert(!cancelRefusedZone(true,true));assert(!cancelRefusedZone(true,false));
 assert(cancelRefusedZone(false,true));assert(!cancelRefusedZone(false,false));
 assert(!enabled.load()&&!upCutLow.load()&&!stabStart.load());
}
