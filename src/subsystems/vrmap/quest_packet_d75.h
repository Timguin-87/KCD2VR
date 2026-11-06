#pragma once
#include "held_map_snapshot_t2.h"
#include <cstdio>
namespace kcd2vr::held_map_feed_t2 {
// Public packet contains value copies only; native pointers never cross to Lua.
// At most 16 current compass objectives, plus an independent current-level pin.
inline bool EncodeQuestD75(const NativeSnapshot& native,bool valid,bool waypoint,
    const float (&xy)[2],std::uint64_t activation,std::uint64_t epoch,
    std::uint32_t actor,std::uint32_t helper,char (&out)[4096]) noexcept {
 if(native.count>native.markers.size()||!activation||!epoch||!actor||!helper)return false;
 int used=std::snprintf(out,sizeof(out),"Q75|%llu|%llu|%u|%u|%u|",
  (unsigned long long)activation,(unsigned long long)epoch,actor,helper,
  unsigned(valid&&native.compassKnown));
 if(waypoint&&Coordinate(xy[0])&&Coordinate(xy[1])){
  const int n=std::snprintf(out+used,sizeof(out)-used,"%.3f,%.3f",xy[0],xy[1]);
  if(n<0||std::size_t(n)>=sizeof(out)-used)return false;used+=n;
 }
 out[used++]='|';out[used]=0;unsigned count=0;
 if(valid&&native.compassKnown)for(unsigned i=0;i<native.count&&count<16;++i){const auto& m=native.markers[i];
  if(m.kind!=Kind::Quest||!Coordinate(m.x)||!Coordinate(m.y))continue;
  const int n=std::snprintf(out+used,sizeof(out)-used,"%d,%d,%d,%.3f,%.3f;",m.nativeType,m.tracking,m.objective,m.x,m.y);
  if(n<0||std::size_t(n)>=sizeof(out)-used)return false;used+=n;++count;
 }
 return true;
}
}
