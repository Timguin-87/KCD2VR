#pragma once
#include "weapon_parry_contact_h24.h"
#include "../subsystems/physics/runtime/native_weapon_contact_event_h24.h"
namespace kcd2vr::native_weapon_parry_join_h24 {
namespace ev=native_weapon_contact_h24;namespace wp=weapon_parry_h24;
struct Frame {
 std::uintptr_t player=0,manager=0,character=0;std::uint32_t entity=0;
 std::uint64_t key=0,generation=0,request=0,epoch=0,sample=0,sourceSample=0,sourceTick=0,
  lease=0,world=0,body=0,tracking=0,cancel=0;
};
inline bool frame(const native_weapon_body_t4::ContactOwnerH24& s,const Frame& f) noexcept {
 return f.player&&f.manager&&f.character&&f.entity&&f.key&&f.generation&&f.request&&f.epoch&&f.sample&&
  f.sourceSample&&f.sourceTick&&f.lease&&f.world&&f.body&&f.tracking&&s.active&&s.owner.side==0&&s.owner.player==f.player&&s.owner.manager==f.manager&&s.owner.character==f.character&&
  s.owner.entityId==f.entity&&s.owner.key==f.key&&s.owner.generation==f.generation&&s.owner.request==f.request&&
  s.owner.epoch==f.epoch&&s.colliderLease==f.lease&&s.worldGeneration==f.world&&s.bodyGeneration==f.body&&
  s.trackingEpoch==f.tracking&&s.cancelGeneration==f.cancel&&s.sourceSample>=f.sourceSample&&s.sourceTick>=f.sourceTick;
}
template<class Read,class Lookup,class Entity>
bool join(std::uintptr_t base,const ev::Event& e,const native_weapon_body_t4::ContactOwnerH24& live,const Frame& f,
 wp::Pair pair,std::uint64_t episode,std::uint64_t onsetSample,int hand,std::uint64_t now,
 Read read,Lookup lookup,Entity entity,wp::Contact& out) noexcept {
 out={};if(!episode||!onsetSample||!pair.valid()||pair.epoch!=f.epoch||pair.request!=f.request||!e.serial||!ev::fresh(e.tick,now)||
  !ev::sameIncarnation(e.source,live)||!frame(live,f)||e.source.sourceSample<onsetSample||
  e.source.sourceSample>f.sourceSample||e.source.sourceSample>f.sample||e.source.sourceTick>f.sourceTick||
  e.otherForeignType!=2||!e.otherForeign||!e.otherBody||e.otherPart<0)return false;
 // Compare current registered item entity and exact current physical proxy;
 // do not interpret a body material or cached name as an enemy weapon.
 std::uintptr_t vf=0,foreign=0,parts=0;std::uint32_t id=0;std::int16_t kind=-1;int count=-1,part=-1;
 if(!read(e.otherForeign,vf)||vf!=base+0x4072C78||!read(e.otherForeign+0xC,id)||!id||
  std::uintptr_t(entity(id))!=e.otherForeign||!read(e.otherBody,vf)||
  (vf!=base+0x3A48DC0&&vf!=base+0x3A578C0)||!read(e.otherBody+0x20,foreign)||foreign!=e.otherForeign||
  !read(e.otherBody+0x28,kind)||kind!=2||
  // Only the proven single-part native GetParams layout. Multiple parts need
  // their exact retail stride/mapping; no guessed index or borrowed SDK slot.
  !read(e.otherBody+0x120,count)||count!=1||!read(e.otherBody+0x118,parts)||!parts||
  !read(parts+0x48,part)||part!=e.otherPart)return false;
 npc_contact172::Witness hit{};hit.ownerRead=true;hit.owner=e.otherForeign;hit.entityId=id;hit.collider=e.otherBody;hit.part=e.otherPart;
 wp::Held held{};if(!wp::held(base,pair.opponentState,hand,hit,read,lookup,entity,held))return false;
 out={pair,held,episode,e.tick,e.source.sourceSample,f.key,f.generation,0,e.otherPart};
 out.nativeEvent=e.serial;out.physical=e.source;return true;
}
}
