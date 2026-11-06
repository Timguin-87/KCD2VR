#pragma once
// An already admitted lethal contact may survive a short handling-only recovery.
// This carries immutable evidence only: never a pose/pin, damage, native-body map,
// new admission, active ragdoll or permission to drive a corpse.
#include "contact_receipt58c1.h"
#include "impale47.h"
#include "pose_math.h"
namespace kcd2vr::impale_pending59 {
struct Pending {
 contact58c1::Receipt hit{};pose_core::Vec3 contact{},local{};
 std::uint64_t since=0;int pick=-1;double tipDistance=-1,depth=0;bool valid=false;
 template<class S,class I,class P> static Pending capture(const S& s,const I& in,const P& p,int side) noexcept {
  Pending r{};
  if(side<0||side>1||!in.allowed||!in.impaleEnabled178r15||!in.impaleHang32||!in.haveWorld||!in.contact||!p.primary||p.side!=side||!p.melee()||
     !in.tracked[side]||!in.intentTracked[side]||!std::isfinite(in.grip[side])||in.grip[side]<.25||
     !s.ready||s.key!=p.key||s.generation!=p.generation||!s.impale178r15||s.impalePinned178r18||
     // Recovery retains only the original H18 request lifetime; restore never renews it.
     !s.impaleSince178r15||in.tick<s.impaleSince178r15||in.tick-s.impaleSince178r15>impale47::kWeaponWaitH18||
     !s.lastTick||in.tick<s.lastTick||in.tick-s.lastTick>80||
     (in.impalePinValid178r15&&in.impalePinSerial178r19==s.impaleSerial178r15)||
     !s.impaleSerial178r15||s.lastImpaleReceipt58c1!=s.impaleSerial178r15||
     s.impaleHit47.serial!=s.impaleSerial178r15||s.impaleHit47.target!=s.impaleEntity178r15||
     s.impaleHit47.tick>s.impaleSince178r15||s.impaleSince178r15-s.impaleHit47.tick>80||
     !contact58c1::lethalThrust(s.impaleHit47,s.impaleSince178r15,p.key,p.generation,in.bodyEpoch,in.epoch)||
     !pose_core::finite(s.impaleTipWorld178r18)||!pose_core::finite(s.impaleLocal32))return r;
  r.hit=s.impaleHit47;r.contact=s.impaleTipWorld178r18;r.local=s.impaleLocal32;r.since=s.impaleSince178r15;
  r.pick=s.impalePick47;r.tipDistance=s.impaleTipM47;r.depth=s.impaleDepth47;r.valid=true;return r;
 }
 template<class S> void restore(S& s) const noexcept {
  if(!valid)return;
  s.impale178r15=true;s.impalePinned178r18=false;s.impaleSince178r15=since;
  s.impaleSerial178r15=hit.serial;s.impaleEntity178r15=hit.target;s.lastImpaleReceipt58c1=hit.serial;
  s.impaleHit47=hit;s.impaleTipWorld178r18=contact;s.impaleLocal32=local;
  s.impalePick47=pick;s.impaleTipM47=tipDistance;s.impaleDepth47=depth;
 }
 template<class O> void publish(O& out,int side) const noexcept {
  if(!valid||(out.impaleSerial&&out.impaleSerial!=hit.serial))return;
  out.impaleLatchRequest=true;out.impaleSerial=hit.serial;out.impaleEntity=hit.target;
  out.impaleTipWorld=contact;out.impaleSide32=side;out.impaleLocal32=local;
  out.impaleHit47=hit;out.impalePick47=pick;out.impaleTipM47=tipDistance;out.impaleDepth47=depth;
 }
};
}
