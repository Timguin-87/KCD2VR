#pragma once
#include "npc_contact172.h"
#include "../native_weapon_body_t4.h"
#include <cstdint>
namespace kcd2vr::weapon_parry_h24 {
struct Held { std::uint64_t key=0;std::uintptr_t item=0,entity=0,collider=0;std::uint32_t id=0;int hand=-1; };
// Exact current attacking-hand key -> CItem -> entity -> physics body. Entity
// names, material IDs and the currently selected opponent alone grant nothing.
template<class Read,class Lookup,class Entity>
bool held(std::uintptr_t base,std::uintptr_t state,int hand,const npc_contact172::Witness& hit,
          Read read,Lookup lookup,Entity entity,Held& out) noexcept {
 out={};if(!base||!state||hand<0||hand>1||!hit.ownerRead||!hit.owner||!hit.entityId||!hit.collider)return false;
 std::uintptr_t vf=0,link=0,body=0;std::uint64_t key=0,again=0;std::uint32_t id=0;
 if(!read(state+0x8C0,vf)||vf!=base+0x3B0A5E8||!read(state+0x8C8+8*unsigned(hand),key)||!key)return false;
 const auto item=std::uintptr_t(lookup(key));
 if(!item||!read(item,vf)||vf!=base+0x3A2DA88||!read(item+0x30,again)||again!=key||
    !read(item+0x88,link)||!link||!read(link+0x30,id)||id!=hit.entityId)return false;
 const auto ent=std::uintptr_t(entity(id));
 if(!ent||ent!=hit.owner||!read(ent,vf)||vf!=base+0x4072C78||!read(ent+0xC,id)||id!=hit.entityId||
    !npc_contact172::bodyFromEntity(base,ent,read,body)||body!=hit.collider||
    !read(item+0x30,again)||again!=key||!read(state+0x8C8+8*unsigned(hand),again)||again!=key)return false;
 out={key,item,ent,body,id,hand};return true;
}
struct Pair {std::uintptr_t actor=0,state=0,opponent=0,opponentState=0;std::uint64_t epoch=0,request=0;
 bool operator==(const Pair& b)const noexcept{return actor==b.actor&&state==b.state&&opponent==b.opponent&&opponentState==b.opponentState&&epoch==b.epoch&&request==b.request;}
 bool valid()const noexcept{return actor&&state&&opponent&&opponentState&&epoch&&request;}
};
struct Episode {Pair pair{};std::uint64_t serial=0,sample=0,onsetSample=0;bool incoming=false,uncertain=false;
 bool observe(Pair p,int phase,std::uint64_t xr) noexcept {
  if(!p.valid()||!xr||phase<0){uncertain=true;sample=0;return false;}
  if(!(p==pair)){pair=p;incoming=false;sample=onsetSample=0;}
  const bool attack=phase==4||phase==8;
  if(!attack){incoming=false;uncertain=false;sample=xr;onsetSample=0;return false;}
  if(uncertain)return false;
  if(!incoming){++serial;incoming=true;onsetSample=xr;}
  if(sample==xr)return false;sample=xr;return true;
 }
};
struct Contact {Pair pair{};Held enemy{};std::uint64_t serial=0,tick=0,sample=0,weapon=0,generation=0;int side=-1,part=-1;std::uint64_t nativeEvent=0;native_weapon_body_t4::ContactOwnerH24 physical{};};
inline bool current(const Contact& c,Pair p,std::uint64_t now,std::uint64_t key,std::uint64_t generation,int side) noexcept {
 return c.serial&&c.tick&&now>=c.tick&&now-c.tick<=80&&c.sample&&c.pair==p&&c.weapon==key&&c.generation==generation&&
        c.side==side&&c.enemy.key&&c.enemy.entity&&c.enemy.collider;
}
} // namespace
