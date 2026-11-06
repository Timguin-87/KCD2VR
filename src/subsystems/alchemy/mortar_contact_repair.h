#pragma once
#include "geometry68.h"
#include <algorithm>
#include <cmath>
#include <limits>

namespace kcd2vr::subsystems::alchemy::mortar_contact_repair {
// Current native selection bounds provide a conservative tool proxy, not a
// vessel-mouth mesh. Test the real pestle shaft against the held mortar instead
// of requiring the two controller wrists to be within twelve centimetres.
struct Contact {bool valid=false;double gap=0;double surfaceRadiusD41=0;};
inline Contact contact(const geometry68::Bounds& pestle,const geometry68::Bounds& mortar) noexcept {
 using namespace geometry68;
 if(!valid(pestle)||!valid(mortar))return {};
 double centre[3]{},extent[3]{},worldExtent[3]{};
 int axis=0;
 for(int j=0;j<3;++j){
  centre[j]=(double(pestle.local[j])+pestle.local[j+3])*.5;
  extent[j]=double(pestle.local[j+3])-pestle.local[j];
  const double scale=std::sqrt(double(pestle.world[j])*pestle.world[j]+
      double(pestle.world[j+4])*pestle.world[j+4]+double(pestle.world[j+8])*pestle.world[j+8]);
  worldExtent[j]=extent[j]*scale;if(worldExtent[j]>worldExtent[axis])axis=j;
 }
 if(worldExtent[axis]<.03||worldExtent[axis]>.60)return {};
 double radius=std::numeric_limits<double>::infinity();
 for(int j=0;j<3;++j)if(j!=axis)radius=std::min(radius,worldExtent[j]*.5);
 if(!std::isfinite(radius)||radius<.001||radius>.08)return {};
 double best=std::numeric_limits<double>::infinity();
 for(int i=0;i<5;++i){
  double local[3]{centre[0],centre[1],centre[2]};local[axis]=pestle.local[axis]+extent[axis]*(double(i)/4.);
  const auto p=transform(pestle,{local[0],local[1],local[2]});const auto r=closest(mortar,p);
  if(r.valid)best=std::min(best,std::max(0.,r.distance-radius));
 }
 return std::isfinite(best)?Contact{true,best,radius}:Contact{};
}
}
