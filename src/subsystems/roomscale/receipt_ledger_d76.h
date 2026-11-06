#pragma once
#include "native_roomscale_d76.h"
#include <array>
#include <cmath>
#include <cstddef>

namespace kcd2vr::native_roomscale_d76::detail {
inline bool same(const Owner&a,const Owner&b) noexcept {
 return a.player==b.player&&a.entity==b.entity&&a.character==b.character&&a.entityId==b.entityId&&a.epoch==b.epoch&&a.lifetime==b.lifetime;
}
inline bool valid(const Owner&o) noexcept {return o.player&&o.entity&&o.character&&o.entityId&&o.epoch&&o.lifetime;}
inline bool finite(Vec3 v) noexcept {return std::isfinite(v.x)&&std::isfinite(v.y)&&std::isfinite(v.z);}
inline Vec3 add(Vec3 a,Vec3 b) noexcept {return {a.x+b.x,a.y+b.y,a.z+b.z};}
inline bool bounded(Vec3 v) noexcept {return finite(v)&&(v.x*v.x+v.y*v.y+v.z*v.z)<=1.0f;}
inline bool fresh(std::uint64_t now,std::uint64_t then) noexcept {return then&&now>=then&&now-then<=60;}
struct Pending { Vec3 world{},tracking{}; std::uint64_t firstTick=0,lastTick=0,lastSample=0; bool valid=false; };
struct EventKey {
 std::uintptr_t address=0,body=0,entity=0;
 std::uint32_t step=0;
 // Native event identity, copied verbatim. Position is an anti-reuse signature,
 // never the adoption/generation join. Occurrence comes from the exact Step.
 std::array<std::uint32_t,8> payload{}; // dt + pos + quaternion
};
inline bool sameEvent(const EventKey&a,const EventKey&b) noexcept {
 return a.address==b.address&&a.body==b.body&&a.entity==b.entity&&a.step==b.step&&a.payload==b.payload;
}
struct EventRecord { EventKey key{}; Origin origin{}; bool used=false; };
struct State {
 Owner owner{};
 Origin cumulative{},entity{},character{},entityWriting{},entityCompleted{};
 Pending pending{};
 std::array<EventRecord,256> events{};
 std::size_t nextEvent=0;
 std::uint64_t sequence=0,lastSeenSample=0,lastApplied=0;
 bool admitted=false,faulted=false,entityBusy=false,characterBusy=false;
 Counters counters{};
 bool rebase(const Owner&o) noexcept {
  const bool sameIdentity=o.player==owner.player&&o.entity==owner.entity&&o.character==owner.character&&o.entityId==owner.entityId&&o.lifetime==owner.lifetime;
  if(!valid(o)||(sameIdentity&&(entityBusy||characterBusy||lastApplied>entity.occurrence||lastApplied>character.occurrence)))return false;
  // Raw tracking-origin debt belongs to the room coordinate frame, not the
  // old entity. A new save/player in the SAME origin epoch retains only
  // completed debt; pending old-world physics and its world sum are retired.
  const Vec3 carryTracking=valid(owner)&&owner.epoch==o.epoch?entityCompleted.consumedTracking:Vec3{};
  const auto keep=counters;const auto nextSerial=sequence+2;
  *this=State{};counters=keep;sequence=nextSerial;owner=o;
  cumulative.owner=entity.owner=character.owner=o;
  cumulative.consumedTracking=entity.consumedTracking=character.consumedTracking=carryTracking;
  entity.serial=character.serial=sequence;entityCompleted=entity;return true;
 }
 void admission(bool on) noexcept {admitted=on&&!faulted;if(!on)pending={};}
 void cancel() noexcept {pending={};}
 void fault() noexcept {faulted=true;admitted=false;pending={};++counters.faults;}
 bool submit(const StepRequest&r,std::uint64_t now) noexcept {
  if(!admitted||faulted||!same(owner,r.owner)||!r.sample||!bounded(r.worldDelta)||!bounded(r.trackingDelta)||std::fabs(r.worldDelta.z)>0.00001f){++counters.rejected;return false;}
  if(r.sample<=lastSeenSample){++counters.duplicate;return false;}
  lastSeenSample=r.sample; // A rejected/stale input is retired, never replayed.
  if(!fresh(now,r.tickMs)){++counters.stale;return false;}
  if(pending.valid&&!fresh(now,pending.firstTick)){pending={};++counters.stale;}
  const Vec3 w=add(pending.world,r.worldDelta),t=add(pending.tracking,r.trackingDelta);
  if(!bounded(w)||!bounded(t)){pending={};++counters.rejected;return false;}
  if(!pending.valid)pending.firstTick=r.tickMs;
  pending.world=w;pending.tracking=t;pending.lastTick=r.tickMs;pending.lastSample=r.sample;pending.valid=true;++counters.submitted;return true;
 }
 bool hasPending(std::uint64_t now) noexcept {
  if(!admitted||faulted||!pending.valid)return false;
  if(!fresh(now,pending.firstTick)){pending={};++counters.stale;return false;}
  return true;
 }
 // Called only AFTER the exact native Step local displacement was written.
 Origin applied(std::uint64_t occurrence) noexcept {
  cumulative.owner=owner;cumulative.occurrence=occurrence;cumulative.lastSample=pending.lastSample;
  cumulative.consumedTracking=add(cumulative.consumedTracking,pending.tracking);
  cumulative.consumedWorld=add(cumulative.consumedWorld,pending.world);
  lastApplied=occurrence;pending={};++counters.applied;return cumulative;
 }
 bool record(const EventKey&key,Origin origin) noexcept {
  if(!key.address||key.entity!=owner.entity||!same(origin.owner,owner))return false;
  for(auto&r:events)if(r.used&&r.key.address==key.address){r={key,origin,true};return true;}
  events[nextEvent]={key,origin,true};nextEvent=(nextEvent+1)%events.size();return true;
 }
 bool lookup(const EventKey&key,Origin&out)const noexcept {
  for(const auto&r:events)if(r.used&&sameEvent(r.key,key)&&same(r.origin.owner,owner)){out=r.origin;return true;}return false;
 }
 bool beginEntity(const Origin&o) noexcept {
  if(entityBusy||!same(o.owner,owner))return false;
  entityBusy=true;entityWriting=o;entity.serial=++sequence;if(!(entity.serial&1))entity.serial=++sequence;return true;
 }
 void endEntity(Vec3 pos) noexcept {
  if(!entityBusy)return;
  entity=entityWriting;entity.adoptedPosition=pos;entity.serial=++sequence;if(entity.serial&1)entity.serial=++sequence;
  entityCompleted=entity;entityBusy=false;++counters.entityAdopted;
 }
 bool entityForSlot(Origin&out,bool sameWriterThread)const noexcept {
  if(entityBusy){if(!sameWriterThread)return false;out=entityWriting;return true;}
  out=entity;return !faulted;
 }
 bool beginCharacter(const Origin&o) noexcept {
  if(characterBusy||!same(o.owner,owner))return false;
  characterBusy=true;character.serial=++sequence;if(!(character.serial&1))character.serial=++sequence;return true;
 }
 void endCharacter(const Origin&o,Vec3 pos) noexcept {
  if(!characterBusy)return;
  character=o;character.adoptedPosition=pos;character.serial=++sequence;if(character.serial&1)character.serial=++sequence;
  characterBusy=false;++counters.characterAdopted;
 }
 bool read(Surface s,std::uintptr_t id,Origin&out)const noexcept {
  const auto expected=s==Surface::Entity?owner.entity:owner.character;
  if(!expected||id!=expected){out={};return true;}
  const bool busy=s==Surface::Entity?entityBusy:characterBusy;
  if(busy||faulted)return false;
  out=s==Surface::Entity?entity:character;return true;
 }
 bool serial(Surface s,std::uintptr_t id,std::uint64_t serialValue)const noexcept {
  Origin o{};return read(s,id,o)&&o.serial==serialValue;
 }
};
}
