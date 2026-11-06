#pragma once
#include "self_contact54.h"

// Cosmetic own-sword/shield contact edge, NOT a native block/hit/collider.
// The caller supplies the already committed StageShieldPat54 native-local
// bounds proxy. No invented disc, mesh normal or world-tip velocity is used.
namespace kcd2vr::own_shield_contact58 {
namespace sc=self_contact54;namespace m=pose_core;
struct Guard {std::uint64_t now=0,sourceEpoch=0,currentSourceEpoch=0;bool current=false;};
struct Event {std::uint64_t tick=0,sample=0;double speed=0,approach=0;int weapon=-1,shield=-1;};
struct State {sc::ShieldSample previous{};std::uint64_t sourceEpoch=0,lastClang=0;bool have=false,touching=false;};
inline void retire(State& s) noexcept {s={};}
inline bool same(const sc::ShieldSample& a,const sc::ShieldSample& b) noexcept {
 return sc::same(a.owner,b.owner)&&a.shieldKey==b.shieldKey&&a.shieldGeneration==b.shieldGeneration&&
  a.shieldEntity==b.shieldEntity&&std::abs(a.scale-b.scale)<1e-6&&
  m::length(a.box.lo-b.box.lo)<1e-6&&m::length(a.box.hi-b.box.hi)<1e-6&&
  std::abs(m::length(a.blade.b-a.blade.a)-m::length(b.blade.b-b.blade.a))<1e-4&&
  std::abs(a.blade.radius-b.blade.radius)<1e-6;
}
inline Event step(State& s,const sc::ShieldSample& x,const Guard& g) noexcept {
 Event out{};
 if(!g.current||!g.sourceEpoch||g.sourceEpoch!=g.currentSourceEpoch||!g.now||!x.tick||!x.sample||!x.timeNs||
    g.now<x.tick||g.now-x.tick>80||!x.eligible||!sc::valid(x.owner)||!x.shieldKey||!x.shieldGeneration||
    !x.shieldEntity||!sc::valid(x.blade)||!sc::valid(x.box)||!std::isfinite(x.scale)||x.scale<.1||x.scale>10){retire(s);return out;}
 const auto c=sc::nearest(x.blade,x.box);if(!c.valid||!m::unit(c.normal)){retire(s);return out;}
 const bool owner=s.have&&s.sourceEpoch==g.sourceEpoch&&same(x,s.previous);
 if(owner&&x.sample==s.previous.sample)return out;
 const double dt=owner?motion_time::elapsed(s.previous.tick,x.tick,s.previous.timeNs,x.timeNs):0;
 const bool continuous=owner&&x.sample>s.previous.sample&&x.tick>=s.previous.tick&&
  x.tick-s.previous.tick<=80&&dt>=.001&&dt<=.080;
 if(!owner){s={};s.sourceEpoch=g.sourceEpoch;}
 if(continuous&&c.gap<=.004&&!s.touching){
  // Corresponding points on BOTH rigid proxies. Common body translation or
  // rotation cannot masquerade as the donor's fast absolute world tip.
  const auto oldBlade=s.previous.blade.a+(s.previous.blade.b-s.previous.blade.a)*c.bladeU;
  const auto oldShield=s.previous.box.position+m::rotate(*m::unit(s.previous.box.rotation),c.targetLocal);
  const auto relative=((c.blade-c.target)-(oldBlade-oldShield))*(x.scale/dt);
  const double speed=m::length(relative),approach=-m::dot(relative,c.normal);
  if(std::isfinite(speed)&&std::isfinite(approach)&&speed>=1.2&&speed<=20&&approach>=.35&&
     (!s.lastClang||(x.tick>=s.lastClang&&x.tick-s.lastClang>=250))){
   out={x.tick,x.sample,speed,approach,x.owner.weapon,x.owner.hand};s.lastClang=x.tick;
  }
 }
 s.touching=continuous?(s.touching?c.gap<=.012:c.gap<=.004):c.gap<=.004;
 s.previous=x;s.sourceEpoch=g.sourceEpoch;s.have=true;return out;
}
} // namespace kcd2vr::own_shield_contact58
