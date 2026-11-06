#pragma once
#include "native_weapon_parry_join_h24.h"
namespace kcd2vr::native_weapon_actor_join_h28 {
namespace ev=native_weapon_contact_h24;namespace body=native_weapon_body_t4;namespace nj=native_weapon_parry_join_h24;
struct Contact {npc_contact172::Witness witness{};ev::Event occurrence{};};
// Game-thread copied occurrence -> CURRENT entity's exact physical proxy. No
// ray query, SDK part stride, event-pointer lifetime, material-domain conversion,
// native damage call, or new gesture is introduced here.
template<class Read,class Entity>
bool join(std::uintptr_t base,const ev::Event& e,const body::ContactOwnerH24& live,const nj::Frame& frame,
          std::uint64_t onsetSample,std::uint64_t now,Read read,Entity entity,Contact& out) noexcept {
 out={};if(!base||!onsetSample||!e.serial||!ev::active(live,now)||!ev::fresh(e.tick,now)||
  !ev::sameIncarnation(e.source,live)||!nj::frame(live,frame)||
  e.source.sourceSample<onsetSample||e.source.sourceSample>frame.sourceSample||e.source.sourceTick>frame.sourceTick||
  e.otherForeignType!=2||!e.otherForeign||!e.otherBody||e.otherPart<0||
  !ev::finite(e.point)||!ev::finite(e.normal)||!ev::finite(e.selfVelocity)||!ev::finite(e.otherVelocity))return false;
 std::uintptr_t vf=0,foreign=0,currentBody=0;std::uint32_t id=0;std::int16_t type=-1;
 if(!read(e.otherForeign,vf)||vf!=base+0x4072C78||!read(e.otherForeign+0xC,id)||!id||
    id==live.owner.entityId||std::uintptr_t(entity(id))!=e.otherForeign||
    !read(e.otherBody,vf)||!read(e.otherBody+0x20,foreign)||foreign!=e.otherForeign||
    !read(e.otherBody+0x28,type)||type!=2||
    !npc_contact172::bodyFromEntity(base,e.otherForeign,read,currentBody)||currentBody!=e.otherBody)return false;
 const auto kind=npc_contact172::classify(vf-base);
 if(kind!=npc_contact172::Body::Living&&kind!=npc_contact172::Body::Articulated)return false;
 // Original event part is passed to native validation. It is NOT reinterpreted
 // as an index in an unproven multipart layout. Event idmat and ray surface/body
 // material have distinct unproven domains; leave both ray fields unknown.
 out.witness.mask=npc_contact172::BodiesAndWorld;out.witness.body=kind;out.witness.collider=e.otherBody;
 out.witness.vft=vf-base;out.witness.owner=e.otherForeign;out.witness.entityId=id;
 out.witness.part=e.otherPart;out.witness.ownerRead=true;out.occurrence=e;
 // Native resolver/read callbacks may reenter: recheck the current entity/body
 // after all traversals before publishing even a copied witness.
 std::uintptr_t after=0;std::uint32_t afterId=0;
 if(std::uintptr_t(entity(id))!=e.otherForeign||!read(e.otherForeign+0xC,afterId)||afterId!=id||
    !npc_contact172::bodyFromEntity(base,e.otherForeign,read,after)||after!=e.otherBody){out={};return false;}
 return true;
}
}
