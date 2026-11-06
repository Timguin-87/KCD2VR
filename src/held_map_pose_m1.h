#pragma once
#include <cmath>
namespace kcd2vr::held_map_pose_m1 {
// Native parchment_folded.chr slt_1, sampled from native open.caf. The
// producer supplies the actual committed LEFT held-item socket (LeftHandSlot).
// modelWorld = socketWorld * inverse(nativePaperGrip), with no Henry writes.
inline bool Place(const float* hand,float* out) noexcept {
 if(!hand||!out)return false;
 double norm=0;for(int i=0;i<7;++i)if(!std::isfinite(hand[i]))return false;
 for(int i=0;i<4;++i)norm+=double(hand[i])*hand[i];if(norm<.8||norm>1.2)return false;
 const double inv=1/std::sqrt(norm);
 const double x=hand[0]*inv,y=hand[1]*inv,z=hand[2]*inv,w=hand[3]*inv;
 constexpr double a=.7071067811865476;
 // inverse(slt_1.q) = (-a,+a,0,0); source quantization residual <3e-5.
 out[0]=float(-w*a-z*a);out[1]=float(w*a-z*a);
 out[2]=float(x*a+y*a);out[3]=float(x*a-y*a);
 const double qx=out[0],qy=out[1],qz=out[2],qw=out[3];
 constexpr double vx=-.0036741255316883326,vy=-.0022062920033931732,vz=.15605635941028595;
 const double tx=2*(qy*vz-qz*vy),ty=2*(qz*vx-qx*vz),tz=2*(qx*vy-qy*vx);
 out[4]=float(hand[4]-(vx+qw*tx+qy*tz-qz*ty));
 out[5]=float(hand[5]-(vy+qw*ty+qz*tx-qx*tz));
 out[6]=float(hand[6]-(vz+qw*tz+qx*ty-qy*tx));out[7]=1.f;
 return true;
}
inline void Point(const float* pose,const float* p,float* out) noexcept {
 const float x=pose[0],y=pose[1],z=pose[2],w=pose[3];
 const float tx=2*(y*p[2]-z*p[1]),ty=2*(z*p[0]-x*p[2]),tz=2*(x*p[1]-y*p[0]);
 out[0]=pose[4]+p[0]+w*tx+y*tz-z*ty;
 out[1]=pose[5]+p[1]+w*ty+z*tx-x*tz;
 out[2]=pose[6]+p[2]+w*tz+x*ty-y*tx;
}
}
