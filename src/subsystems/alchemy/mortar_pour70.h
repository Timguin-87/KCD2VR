#pragma once
#include "../../vrik/alchemy_semi46.h"
#include "geometry68.h"
namespace kcd2vr::subsystems::alchemy::pour70 {
// Gesture authorization of the REAL native placement child. No time seeking,
// event synthesis, ingredient transfer or result fabrication. Irreversible Use
// is observed from the native callback; reverse motion never rewinds it.
struct Home {
 geometry68::Point local{};unsigned pot=0,mortar=0;bool valid=false;
 bool capture(unsigned potId,unsigned mortarId,const geometry68::Bounds& p,const geometry68::Bounds& m) noexcept {
  *this={};if(!potId||!mortarId||!geometry68::valid(p)||!geometry68::valid(m))return false;
  using namespace geometry68;const Point center=transform(m,{(m.local[0]+m.local[3])*.5,(m.local[1]+m.local[4])*.5,(m.local[2]+m.local[5])*.5});
  const Point a{p.world[0],p.world[4],p.world[8]},b{p.world[1],p.world[5],p.world[9]},c{p.world[2],p.world[6],p.world[10]};
  const double det=dot(a,cross(b,c));if(!std::isfinite(det)||std::fabs(det)<1.e-9)return false;
  const Point v=sub(center,{p.world[3],p.world[7],p.world[11]});local={dot(v,cross(b,c))/det,dot(v,cross(c,a))/det,dot(v,cross(a,b))/det};
  pot=potId;mortar=mortarId;valid=finite(local);return valid;
 }
 bool atRest70(unsigned potId,unsigned mortarId,const geometry68::Bounds& p,geometry68::Point palm) const noexcept {
  if(!valid||potId!=pot||mortarId!=mortar||!geometry68::valid(p)||!geometry68::finite(palm))return false;
  const auto d=geometry68::sub(geometry68::transform(p,local),palm);return geometry68::dot(d,d)<=.01;
 }
};
struct Controller {
 std::uintptr_t parent=0,child=0;std::uint64_t life=0,sequence=0,timeNs=0,pose=0,source=0;
 unsigned token=0;double seconds=0;
 template<class Frame> double step(const alchemy_semi46::WorkWitness& w,const Frame& f,bool authorized) noexcept {
  const bool valid=f.allowed&&f.sequence&&f.timeNs&&f.poseEpoch&&f.sourceEpoch&&
   w.parent&&w.child&&w.token&&w.lifetime&&w.kind==alchemy_semi46::MortarPlace70&&w.working;
  if(!valid){*this={};return 0;}
  const bool same=parent==w.parent&&child==w.child&&token==w.token&&life==w.lifetime&&pose==f.poseEpoch&&source==f.sourceEpoch;
  if(!same){*this={};parent=w.parent;child=w.child;token=w.token;life=w.lifetime;pose=f.poseEpoch;source=f.sourceEpoch;sequence=f.sequence;timeNs=f.timeNs;return 0;}
  if(f.sequence==sequence)return seconds;
  if(f.sequence<sequence||f.timeNs<=timeNs||f.timeNs-timeNs>80000000ull){sequence=f.sequence;timeNs=f.timeNs;return seconds;}
  const double dt=double(f.timeNs-timeNs)*1.e-9;sequence=f.sequence;timeNs=f.timeNs;
  if(authorized&&dt>=.001&&dt<=.08)seconds+=dt;
  return seconds;
 }
};
}
