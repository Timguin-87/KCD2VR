#pragma once
#include "../../vrik/physical_weapon.h"
#include "../../vrik/impale47.h"

namespace kcd2vr::impale_successor_h18 {
namespace p=physical;namespace m=pose_core;
// An admitted native lethal receipt owns this bounded wait. A recognizer end
// cannot renew it; no old contact/body/part is dereferenced or remapped here.
struct Source {
 std::uint64_t key=0,generation=0,epoch=0,request=0,tracking=0,recovery=0,policy=0;
 bool operator==(const Source& b)const noexcept{return key==b.key&&generation==b.generation&&epoch==b.epoch&&request==b.request&&
  tracking==b.tracking&&recovery==b.recovery&&policy==b.policy;}
};
struct Wait {
 Source source{};std::uint64_t serial=0,began=0;int side=-1;
 p::Vec3 tip{},axis{};bool active=false;
 bool begin(std::uint64_t s,std::uint64_t originalTick,int hand,Source owner,p::Vec3 tipWorld,p::Vec3 axisWorld) noexcept {
  *this={};const auto a=m::unit(axisWorld);
  if(!s||!originalTick||hand<0||hand>1||!owner.key||!owner.generation||!owner.epoch||!owner.request||!owner.tracking||
     !owner.policy||(owner.policy&1)||!m::finite(tipWorld)||!a)return false;
  serial=s;began=originalTick;side=hand;source=owner;tip=tipWorld;axis=*a;active=true;return true;
 }
 const char* current(std::uint64_t now,Source owner,p::Vec3 currentTip,bool held,bool tracked)const noexcept {
  if(!active)return "NO_VICTIM_WAIT";
  if(now<began||now-began>=impale47::kCorpseWaitH18)return "ORIGINAL_VICTIM_WAIT_EXPIRED";
  if(!(source==owner))return "VICTIM_SOURCE_CHANGED";
  if(!held||!tracked)return "WEAPON_RELEASED_OR_TRACKING_LOST";
  if(!m::finite(currentTip))return "CURRENT_BLADE_INVALID";
  // Real blade withdrawal, independent of the victim's authored death motion.
  if(m::dot(tip-currentTip,axis)>.25)return "BLADE_WITHDRAWN";
  return nullptr;
 }
};
// A NEW synchronous query contact on the currently allowed blade, never a
// remembered impact record or an old receipt projected onto a replacement root.
inline bool currentBladeContact(const p::Input& in,const p::Output& out,const p::Profile& profile,int side) noexcept {
 if(side<0||side>1||!in.allowed||!in.contact||!in.tracked[side]||!in.haveWorld||!(in.worldScale>=.1&&in.worldScale<=10)||
    !out.active[side]||!out.owned[side]||out.solverOwnedT4[side]||out.handlingOnly55c1[side]||!out.contactKnown[side]||
    out.sweepDeferred115[side]||out.ownShieldBlocked60[side]||!p::finite(out.weapons[side])||!profile.geometry)return false;
 const auto& c=out.contact119[side];const auto& w=c.native172;
 if(!c.known||!c.plane||c.limited115||!(c.fraction>=0&&c.fraction<1)||!m::finite(c.point)||
    w.mask!=npc_contact172::BodiesAndWorld||w.body!=npc_contact172::Body::Articulated||!w.ownerRead||!w.owner||!w.entityId||!w.collider||w.part<0)return false;
 const auto grip=p::point(out.weapons[side],profile.grip[side].p),tip=p::point(out.weapons[side],profile.tip),d=tip-grip;
 const double length=m::dot(d,d);if(!std::isfinite(length)||length<.0025)return false;
 const double t=m::dot(c.point-grip,d)/length;
 // Two and a half centimetres covers the checked ray margin; this is not a
 // permission to teleport a wound to a blade that has already moved away.
 return std::isfinite(t)&&t>=0&&t<=1&&m::length(c.point-(grip+d*t))*in.worldScale<=.025;
}
}
