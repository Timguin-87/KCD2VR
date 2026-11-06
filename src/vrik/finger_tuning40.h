#pragma once
#include "pose_math.h"
#include <atomic>
namespace kcd2vr::finger40 {
struct Cell {std::atomic<float> open{0},closed{1},lateral{0},proximal{0};};
inline Cell values[2][5];
// C59: separate headset-approved 2026-10-18 defaults; saved keys still win.
inline constexpr float defaults59[2][5][4]={{{0.f,0.58f,0.f,-18.f},{0.f,1.f,-20.f,11.f},{0.f,0.88f,16.f,-0.5f},{0.f,1.f,0.f,0.f},{0.f,1.f,-9.f,3.f}},{{0.f,0.79f,0.f,-20.f},{0.f,1.f,-15.5f,12.f},{0.f,0.94f,17.5f,4.5f},{0.f,1.f,0.f,0.f},{0.f,1.f,-18.5f,0.f}}};
inline std::atomic<float> middleTowardRing59{3.f};
// H31 anatomical middle finger (digit2), independently adjustable on each hand.
// A root translation relocates the knuckle; it never lengthens a phalanx.
inline std::atomic<float> middleBaseShiftMmH31[2]{{15.f},{15.f}};
inline std::atomic<float> middleDistalMedialDegH31[2]{{10.f},{10.f}};
inline float boundedShiftMmH31(float v) noexcept {return std::isfinite(v)?std::clamp(v,0.f,20.f):15.f;}
inline float boundedDistalMedialDegH31(float v) noexcept {return std::isfinite(v)?std::clamp(v,-20.f,20.f):10.f;}
inline float getShiftMmH31(int side) noexcept {return side>=0&&side<2?boundedShiftMmH31(middleBaseShiftMmH31[side].load()):0.f;}
inline void setShiftMmH31(int side,float v) noexcept {if(side>=0&&side<2)middleBaseShiftMmH31[side].store(boundedShiftMmH31(v));}
inline float getDistalMedialDegH31(int side) noexcept {return side>=0&&side<2?boundedDistalMedialDegH31(middleDistalMedialDegH31[side].load()):0.f;}
inline void setDistalMedialDegH31(int side,float v) noexcept {if(side>=0&&side<2)middleDistalMedialDegH31[side].store(boundedDistalMedialDegH31(v));}
inline bool middleCorrectionH31(int side) noexcept {return getShiftMmH31(side)>0.f||getDistalMedialDegH31(side)!=0.f;}
// Optional middle-joint lateral offset: independent from all accepted H28 values.
inline std::atomic<float> middleLateralC54[2][5];
inline float boundedMiddleLateralC54(float v) noexcept {return std::isfinite(v)?std::clamp(v,-20.f,20.f):0.f;}
inline float getMiddleLateralC54(int side,int finger) noexcept {return middleLateralC54[side][finger].load();}
inline void setMiddleLateralC54(int side,int finger,float v) noexcept {middleLateralC54[side][finger].store(boundedMiddleLateralC54(v));}
inline pose_core::Quat middleLateralRotationC54(pose_core::Quat closed,pose_core::Vec3 parentSpreadAxis,float degrees) noexcept {
 namespace m=pose_core;degrees=boundedMiddleLateralC54(degrees);
 if(degrees==0)return closed; // bit-identical H28 endpoint; no renormalization
 const auto axis=m::unit(parentSpreadAxis);if(!axis||!m::unit(closed))return closed;
 const auto q=m::unit(m::multiply(m::from_rotation_vector(*axis*(degrees*3.141592653589793/180.)),closed));
 return q?*q:closed;
}
inline float bounded(float v,int parameter) noexcept {
 if(!std::isfinite(v))return parameter==1?1.f:0.f;
 return std::clamp(v,parameter==3?-30.f:parameter==2?-20.f:0.f,parameter==3?30.f:parameter==2?20.f:1.f);
}
inline float get(int side,int finger,int parameter) noexcept {
 auto& c=values[side][finger];return parameter==0?c.open.load():parameter==1?c.closed.load():parameter==2?c.lateral.load():c.proximal.load();
}
inline void set(int side,int finger,int parameter,float v) noexcept {
 v=bounded(v,parameter);auto& c=values[side][finger];
 if(parameter==0)c.open.store(v);else if(parameter==1)c.closed.store(v);else if(parameter==2)c.lateral.store(v);else c.proximal.store(v);
}
inline void copyRightClosedToLeft47() noexcept {
 for(int f=0;f<5;++f)for(int k=1;k<3;++k)set(1,f,k,get(0,f,k));
}
struct Endpoints {pose_core::Quat open,closed;};
inline Endpoints towardRing59(pose_core::Quat open,pose_core::Quat closed,pose_core::Quat parent,
 pose_core::Quat joint,pose_core::Vec3 phalanx,pose_core::Vec3 position,pose_core::Vec3 ring,float degrees) noexcept {
 namespace m=pose_core;Endpoints out{open,closed};
 if(!std::isfinite(degrees)||degrees==0||!m::unit(parent)||!m::unit(joint)||!m::unit(closed))return out;
 const auto direction=m::rotate(joint,phalanx),target=ring-position;
 const auto axis=m::unit(m::cross(direction,target));if(!axis)return out;
 const auto local=m::rotate(m::conjugate(parent),*axis);
 const auto adjusted=m::unit(m::multiply(m::from_rotation_vector(local*(std::clamp(double(degrees),-8.,8.)*3.141592653589793/180.)),closed));
 if(adjusted)out.closed=*adjusted;return out;
}

inline Endpoints tune(pose_core::Quat open,pose_core::Quat closed,float a,float b,float lateral,pose_core::Vec3 axis,bool proximal,float proximalDegrees=0.f) noexcept {
 namespace m=pose_core;
 a=bounded(a,0);b=std::max(a,bounded(b,1));lateral=bounded(lateral,2);
 if(a==0&&b==1&&lateral==0&&proximalDegrees==0)return {open,closed}; // exact default parity, no extra interpolation
 const auto o=m::slerp(open,closed,a),c=m::slerp(open,closed,b);
 if(!o||!c)return {open,closed}; // downstream pose validation rejects invalid native endpoints
 Endpoints e{*o,*c};
 // C55: independent MCP/base-joint flexion follows this side's authored curl
 // axis in the joint PARENT frame. No raw right-hand bone mirroring, donor
 // axis, translation or distal-joint change. Open is never changed by it.
 if(proximal&&proximalDegrees!=0){
   auto delta=m::multiply(closed,m::conjugate(open));
   if(delta.w<0)delta={-delta.x,-delta.y,-delta.z,-delta.w};
   const auto n=m::unit(m::Vec3{delta.x,delta.y,delta.z});
   if(n){const auto q=m::from_rotation_vector(*n*(bounded(proximalDegrees,3)*3.141592653589793/180.));
     const auto adjusted=m::unit(m::multiply(q,e.closed));if(adjusted)e.closed=*adjusted;}
 }
 if(proximal&&lateral!=0){auto n=m::unit(axis);if(n){const double h=bounded(lateral,2)*3.141592653589793/360.;
 const m::Quat q{n->x*std::sin(h),n->y*std::sin(h),n->z*std::sin(h),std::cos(h)};
 e.closed=*m::unit(m::multiply(q,e.closed));}}
 return e;
}
}
