#pragma once
#include "physical_weapon.h"
#include <cstddef>
#include <limits>
namespace kcd2vr::partanchor59c7g {
namespace p=physical;namespace m=pose_core;
// Exact retail GetStatus(type=1) buffer, derived from +8A627C and its callers.
// +4 is PART ID, +8 is array index. They are not interchangeable with a bone.
struct Status {
 int type=1,part=-1,index=-1,flags=0,flagsOr=0,flagsAnd=0;
 float position[3]{},boundsMin[3]{},boundsMax[3]{},rotation[4]{},scale=0;
 int simulationClass=0,padding=0;
 std::uintptr_t matrix33=0,matrix34=0,geometry=0,proxyGeometry=0;
 // CRigidEntity's successful type-1 continuation reads +0x78 at +8A6E2D.
 // Zero requests the current pose and avoids the historical-pose branch.
 float timeBack=0;
 explicit Status(int id=-1) noexcept:part(id) {
  for(auto& f:position)f=std::numeric_limits<float>::quiet_NaN();
  for(auto& f:rotation)f=std::numeric_limits<float>::quiet_NaN();
 }
};
static_assert(sizeof(Status)==0x80&&offsetof(Status,position)==0x18&&offsetof(Status,rotation)==0x3c&&
 offsetof(Status,scale)==0x4c&&offsetof(Status,matrix33)==0x58&&offsetof(Status,timeBack)==0x78);
inline bool decode(const Status& s,int expected,p::Pose& pose,double& scale) noexcept {
 if(s.type!=1||s.part!=expected||expected<0||!std::isfinite(s.scale)||s.scale<.01||s.scale>10)return false;
 pose={{s.rotation[0],s.rotation[1],s.rotation[2],s.rotation[3]},
       {s.position[0],s.position[1],s.position[2]}};
 scale=s.scale;return p::finite(pose);
}
struct Identity {
 std::uint64_t serial=0,epoch=0,request=0;std::uint32_t target=0;
 std::uintptr_t entity=0,body=0;int part=-1;
 bool operator==(const Identity& b)const noexcept{return serial==b.serial&&epoch==b.epoch&&request==b.request&&
  target==b.target&&entity==b.entity&&body==b.body&&part==b.part;}
};
struct Anchor {
 Identity owner{};p::Vec3 local{},previous{};double scale=0;bool active=false;
 void clear() noexcept {*this={};}
 bool acquire(Identity id,p::Pose part,double partScale,p::Vec3 hit) noexcept {
  clear();if(!id.serial||!id.target||!id.entity||!id.body||id.part<0||!p::finite(part)||
   !m::finite(hit)||!std::isfinite(partScale)||partScale<.01||partScale>10)return false;
  const auto point=p::point(p::inverse(part),hit)*(1./partScale);
  // Near-surface attachment, not an unlimited offset into a different body.
  if(m::length(point)*partScale>1.)return false;
  owner=id;local=point;scale=partScale;previous=hit;active=true;return true;
 }
 bool update(Identity id,p::Pose part,double partScale,p::Vec3& hit) noexcept {
  if(!active||!(owner==id)||!p::finite(part)||!std::isfinite(partScale)||std::abs(scale-partScale)>.0001){clear();return false;}
  const auto next=p::point(part,local*scale);
  if(!m::finite(next)||m::length(next-previous)>.35){clear();return false;}
  previous=next;hit=next;return true;
 }
};
}
#include "../subsystems/gore/impale_successor_h18.h"
