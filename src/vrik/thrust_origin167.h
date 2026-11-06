#pragma once
#include "pose_math.h"
#include <cmath>
namespace kcd2vr::thrust_origin167 {
// Chest-relative origin, not the direction the blade happens to point at release.
// Table zones: 0 high, 1 left, 2 right, 5 low; disabled zones 3/4 never emitted.
// A central start remains unknown, with the established native zone-5 fallback.
inline int zone(pose_core::Vec3 origin){
    if(!pose_core::finite(origin)||std::hypot(origin.x,origin.z)<.09)return -1;
    if(std::abs(origin.z)>std::abs(origin.x))return origin.z>0?0:5;
    return origin.x<0?1:2;
}
inline const char* name(int z){return z==0?"UP":z==1?"LEFT":z==2?"RIGHT":z==5?"DOWN":"CENTRE_UNKNOWN";}
inline int opposite(int z){return z==0?5:z==5?0:z==1?2:z==2?1:-1;}
}
