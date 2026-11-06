#pragma once
#include "../core/door_origin_h12.h"
#include <algorithm>
namespace kcd2vr::door_near_area_c54 {
inline bool closest(const float* inverse,const float* box,door19c4::Point hand,door19c4::Point& point,float& distance) noexcept {
 point={};distance=0;float local[3]{};const float input[]{hand.x,hand.y,hand.z};
 if(!door_origin_h12::localPoint(inverse,input,local)||!box)return false;
 double norm[3]{};for(unsigned i=0;i<3;++i){
  for(unsigned j=0;j<3;++j)norm[i]+=double(inverse[4*i+j])*inverse[4*i+j];
  if(!std::isfinite(norm[i])||norm[i]<1e-8||norm[i]>1e8||!std::isfinite(box[i])||!std::isfinite(box[i+3])||box[i]>box[i+3])return false;
 }
 for(unsigned i=0;i<3;++i)for(unsigned j=i+1;j<3;++j){double dot=0;for(unsigned k=0;k<3;++k)dot+=double(inverse[4*i+k])*inverse[4*j+k];
  if(std::fabs(dot)>1e-5*std::sqrt(norm[i]*norm[j]))return false;}
 double world[3]{};for(unsigned i=0;i<3;++i){const double v=std::clamp(local[i],box[i],box[i+3])-inverse[4*i+3];
  for(unsigned j=0;j<3;++j)world[j]+=double(inverse[4*i+j])*v/norm[i];}
 point={float(world[0]),float(world[1]),float(world[2])};if(!door19c4::finite(point))return false;
 const double dx=world[0]-hand.x,dy=world[1]-hand.y,dz=world[2]-hand.z;
 distance=float(std::sqrt(dx*dx+dy*dy+dz*dz));return std::isfinite(distance)&&distance<=.20000001f;
}
}
