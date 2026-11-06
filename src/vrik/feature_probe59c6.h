#pragma once
// Read-only proxy geometry and temporal qualification. Not a strike detector,
// no native attack input, no damage, no effect, no pose/physics writes.
#include "physical_weapon.h"
#include <array>
#include <limits>
#include <cstdint>
namespace kcd2vr::feature59c6 {
namespace p=kcd2vr::physical;namespace m=kcd2vr::pose_core;
enum class Kind {None,Pommel,Shield};
inline const char* name(Kind k) noexcept{return k==Kind::Pommel?"POMMEL_BOUNDS_PROXY":k==Kind::Shield?"SHIELD_BOUNDS_PROXY":"NONE";}
struct Shape {Kind kind=Kind::None;std::array<p::Vec3,3> points{};unsigned count=0;};
inline Shape shape(const p::Profile& profile) noexcept {
 Shape s;if(!profile.primary||!profile.geometry||!profile.diagnosticBounds59c6||profile.side<0||profile.side>1)return s;
 const auto lo=profile.diagnosticBoundsMin59c6,hi=profile.diagnosticBoundsMax59c6;
 if(!m::finite(lo)||!m::finite(hi)||lo.x>hi.x||lo.y>hi.y||lo.z>hi.z)return s;
 const double a[3]{lo.x,lo.y,lo.z},b[3]{hi.x,hi.y,hi.z};
 const double lengths[3]{hi.x-lo.x,hi.y-lo.y,hi.z-lo.z};
 if(profile.shield176()){
  unsigned x=0;for(unsigned i=1;i<3;++i)if(lengths[i]>lengths[x])x=i;
  unsigned y=(x+1)%3;for(unsigned i=0;i<3;++i)if(i!=x&&lengths[i]>lengths[y])y=i;
  if(lengths[x]<.1||lengths[x]>2.5||lengths[y]<.1)return s;
  s.kind=Kind::Shield;s.count=3;s.points[0]=(lo+hi)*.5;s.points[1]=s.points[2]=s.points[0];
  double* q[3]{&s.points[1].x,&s.points[1].y,&s.points[1].z};*q[x]+=lengths[x]*.4;
  double* z[3]{&s.points[2].x,&s.points[2].y,&s.points[2].z};*z[y]-=lengths[y]*.4;
 }else if(profile.melee()){
  const auto axis=m::unit(profile.axis);const auto grip=profile.grip[profile.side].p;
  if(!axis||!m::finite(grip))return s;
  const double g[3]{grip.x,grip.y,grip.z},d[3]{-axis->x,-axis->y,-axis->z};
  // C26 (independent review 16 Oct, finding B): the native wrist/grip ORIGIN is not a mesh point - a correct
  // longsword wrist sits ~6.6 cm outside the 2 cm handle AABB. The proxy is now EXPLICITLY APPROXIMATE: the
  // handle-side end of the AABB centreline along the weapon axis, independent of where the wrist is. The wrist
  // only has to be near the item (<=15 cm from the bounds, a sanity check), never inside it.
  // This is NOT an authored pommel socket or proof every class has a pommel.
  for(unsigned i=0;i<3;++i)if(g[i]<a[i]-.15||g[i]>b[i]+.15)return s;
  const double c[3]{(a[0]+b[0])*.5,(a[1]+b[1])*.5,(a[2]+b[2])*.5};
  double travel=std::numeric_limits<double>::infinity();
  for(unsigned i=0;i<3;++i){
   if(d[i]>1e-8)travel=std::min(travel,(b[i]-c[i])/d[i]);
   else if(d[i]<-1e-8)travel=std::min(travel,(a[i]-c[i])/d[i]);}
  if(!std::isfinite(travel)||travel<.05)return s; // no handle length along the axis
  const p::Vec3 centre{c[0],c[1],c[2]};
  s.kind=Kind::Pommel;s.count=1;s.points[0]=centre-(*axis)*(travel-.005);
 }
 return s;
}
struct Owner {
 std::uintptr_t player=0;std::uint64_t key=0,generation=0,epoch=0,request=0,recovery=0;
 std::uint32_t entity=0;int side=-1;
 bool operator==(const Owner& b)const noexcept{return player&&player==b.player&&key&&key==b.key&&generation==b.generation&&epoch==b.epoch&&request==b.request&&recovery==b.recovery&&entity==b.entity&&side==b.side;}
};
struct Sample {Owner owner{};Shape local{};p::Pose weapon{},world{};double scale=1;std::uint64_t sample=0,tick=0,ns=0;bool eligible=false,guard=false;};
enum class Stage {Unavailable,Seed,Duplicate,NoMotion,Query,Jump};
inline const char* name(Stage s)noexcept{switch(s){case Stage::Unavailable:return "UNAVAILABLE";case Stage::Seed:return "SEED";case Stage::Duplicate:return "SAME_SAMPLE";case Stage::NoMotion:return "NO_MOTION";case Stage::Query:return "SWEEP";case Stage::Jump:return "DISCONTINUITY";}return "UNKNOWN";}
struct Segment {p::Vec3 from{},to{};double speed=0;unsigned feature=0;};
struct Batch {Stage stage=Stage::Unavailable;Kind kind=Kind::None;Owner owner{};std::array<Segment,3> segments{};unsigned count=0;std::uint64_t sample=0,tick=0;bool guardFrom=false,guardNow=false;};
struct Tracker {
 Sample previous{};std::array<p::Vec3,3> points{};bool have=false;
 void clear()noexcept{previous=Sample{};points={};have=false;}
 Batch step(const Sample& v,std::uint64_t now,std::uint64_t nowNs)noexcept{
  Batch b;b.kind=v.local.kind;b.owner=v.owner;b.sample=v.sample;b.tick=v.tick;b.guardNow=v.guard;
  if(!v.eligible||!v.sample||!v.tick||now<v.tick||now-v.tick>80||!v.ns||nowNs<v.ns||nowNs-v.ns>80000000||
     !v.local.count||v.local.count>3||!p::finite(v.weapon)||!p::finite(v.world)||!std::isfinite(v.scale)||v.scale<.1||v.scale>10){clear();return b;}
  std::array<p::Vec3,3> current{};for(unsigned i=0;i<v.local.count;++i){current[i]=p::point(v.world,p::point(v.weapon,v.local.points[i])*v.scale);if(!m::finite(current[i])){clear();return b;}}
  const bool same=have&&previous.owner==v.owner&&previous.local.kind==v.local.kind&&previous.local.count==v.local.count&&std::abs(previous.scale-v.scale)<1e-6;
  if(same&&v.sample==previous.sample){b.stage=Stage::Duplicate;return b;}
  const bool timed=same&&v.sample>previous.sample&&v.ns>previous.ns&&v.ns-previous.ns<=80000000&&v.tick>=previous.tick;
  bool topology=timed;for(unsigned i=0;topology&&i<v.local.count;++i)topology=m::length(v.local.points[i]-previous.local.points[i])<1e-6;
  if(topology){b.guardFrom=previous.guard;const double dt=double(v.ns-previous.ns)*1e-9;double longest=0;
   for(unsigned i=0;i<v.local.count;++i)longest=std::max(longest,m::length(current[i]-points[i]));
   if(longest>.6)b.stage=Stage::Jump;
   else if(longest<.002)b.stage=Stage::NoMotion;
   else{b.stage=Stage::Query;for(unsigned i=0;i<v.local.count;++i){const double d=m::length(current[i]-points[i]);if(d>=.002)b.segments[b.count++]={points[i],current[i],d/dt,i};}}
  }else b.stage=Stage::Seed;
  previous=v;points=current;have=true;return b;
 }
};
} // namespace
