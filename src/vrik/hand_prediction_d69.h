#pragma once
// Extra prediction beyond the runtime target, NOT a measured display-latency correction.
// Whole-pose velocity extrapolation; any invalid component selects native future locate.
#include "../tracking_sample59c6.h"
#include "claude_motion_d69.h"
#include <limits>
namespace kcd2vr::hand_prediction_d69 {
using Pose=tracking59c6::Pose;
struct Velocity {float linear[3]{},angular[3]{};bool linearValid=false,angularValid=false;};
inline bool ValidPose(const Pose& p) noexcept {
 const double n=double(p.qx)*p.qx+double(p.qy)*p.qy+double(p.qz)*p.qz+double(p.qw)*p.qw;
 return std::isfinite(p.x)&&std::isfinite(p.y)&&std::isfinite(p.z)&&std::isfinite(n)&&n>.9&&n<1.1;
}
inline bool Extrapolate(Pose& p,const Velocity& v,std::int64_t extra) noexcept {
 if(extra<=0||extra>33000000||!ValidPose(p)||!v.linearValid||!v.angularValid)return false;
 double lv=0,av=0;for(unsigned i=0;i<3;++i){if(!std::isfinite(v.linear[i])||!std::isfinite(v.angular[i]))return false;lv+=double(v.linear[i])*v.linear[i];av+=double(v.angular[i])*v.angular[i];}
 if(lv>400||av>3600)return false;
 const double dt=double(extra)*1e-9,rate=std::sqrt(av),half=.5*rate*dt;
 const double sn=rate>1e-12?std::sin(half)/rate:.5*dt,c=std::cos(half);
 const double x=v.angular[0]*sn,y=v.angular[1]*sn,z=v.angular[2]*sn;
 Pose q=p;q.x=float(double(p.x)+v.linear[0]*dt);q.y=float(double(p.y)+v.linear[1]*dt);q.z=float(double(p.z)+v.linear[2]*dt);
 q.qx=float(c*p.qx+x*p.qw+y*p.qz-z*p.qy);q.qy=float(c*p.qy-x*p.qz+y*p.qw+z*p.qx);
 q.qz=float(c*p.qz+x*p.qy-y*p.qx+z*p.qw);q.qw=float(c*p.qw-x*p.qx-y*p.qy-z*p.qz);
 const double n=std::sqrt(double(q.qx)*q.qx+double(q.qy)*q.qy+double(q.qz)*q.qz+double(q.qw)*q.qw);
 if(!std::isfinite(n)||n<1e-6)return false;q.qx=float(q.qx/n);q.qy=float(q.qy/n);q.qz=float(q.qz/n);q.qw=float(q.qw/n);
 if(!ValidPose(q))return false;p=q;return true;
}
// Locate is called at most once for the pair, only for fully tracked current
// poses without usable velocity. No source/last-good timestamp is changed.
template<class Locate> inline void Apply(tracking59c6::Frame& f,const Velocity (&v)[2],std::int64_t extra,Locate&& locate) {
 if(extra<=0||extra>33000000||f.locateTime<=0||f.locateTime>std::numeric_limits<std::int64_t>::max()-extra||
    !f.valid[0]||!f.tracked[0])return;
 bool need[2]{},any=false;const auto target=f.locateTime+extra;
 for(unsigned h=0;h<2;++h){const unsigned i=h+1;if(!f.valid[i]||!f.tracked[i]||!ValidPose(f.poses[i]))continue;
  if(Extrapolate(f.poses[i],v[h],extra)){f.handTargetD69[h]=target;f.handLeadSourceD69[h]=1;}
  else need[h]=any=true;
 }
 if(!any)return;Pose future[2]{};bool tracked[2]{};
 if(!locate(target,future,tracked))return;
 for(unsigned h=0;h<2;++h)if(need[h]&&tracked[h]&&ValidPose(future[h])){f.poses[h+1]=future[h];f.handTargetD69[h]=target;f.handLeadSourceD69[h]=2;}
}

inline std::uint64_t EffectivePolicy(const tracking59c6::Frame& f,std::int64_t extra) noexcept {
 const unsigned mask=unsigned(f.handTargetD69[0]!=0)|(unsigned(f.handTargetD69[1]!=0)<<1);
 return mask&&extra>0?(std::uint64_t(extra)<<2)|mask:0;
}
}
