#pragma once
#include "native_weapon_body_t4.h"
#include <array>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstring>
namespace kcd2vr::native_weapon_contact_h24 {
namespace body=native_weapon_body_t4;
// Exact retail EventPhysCollision. Addresses below remain opaque copied equality
// identities; no consumer may dereference the event or its borrowed body pointers.
struct Raw {std::array<unsigned char,0xA8> bytes{};};
struct V {float x=0,y=0,z=0;};
struct Event {
 body::ContactOwnerH24 source{};std::uint64_t serial=0,tick=0;
 std::uintptr_t otherBody=0,otherForeign=0;int otherForeignType=-1,otherPhysicsId=-1;
 int selfSide=-1,selfPart=-1,otherPart=-1;short selfMaterial=-1,otherMaterial=-1;
 V point{},normal{},selfVelocity{},otherVelocity{};
 float selfMass=0,otherMass=0,penetration=0,normalImpulse=0,radius=0;
 bool otherPhysicsIdKnown=false;
};
template<class T> inline T at(const Raw& r,unsigned offset) noexcept {T v{};std::memcpy(&v,r.bytes.data()+offset,sizeof(v));return v;}
inline bool finite(V v) noexcept {return std::isfinite(v.x)&&std::isfinite(v.y)&&std::isfinite(v.z);}
inline bool fresh(std::uint64_t tick,std::uint64_t now) noexcept {return tick&&now>=tick&&now-tick<=80;}
inline bool active(const body::ContactOwnerH24& s,std::uint64_t now) noexcept {
 return s.active&&s.body&&s.foreign&&s.foreignType==0x5634&&s.bodyId>=0&&s.owner.key&&s.owner.generation&&
  s.owner.player&&s.owner.epoch&&s.owner.request&&s.worldGeneration&&s.bodyGeneration&&s.colliderLease&&
  s.trackingEpoch&&s.sourceSample&&fresh(s.sourceTick,now)&&fresh(s.observedTick,now);
}
inline bool sameIncarnation(const body::ContactOwnerH24& a,const body::ContactOwnerH24& b) noexcept {
 return a.active&&b.active&&a.owner==b.owner&&a.body==b.body&&a.foreign==b.foreign&&a.foreignType==b.foreignType&&
  a.bodyId==b.bodyId&&a.worldGeneration==b.worldGeneration&&a.bodyGeneration==b.bodyGeneration&&
  a.colliderLease==b.colliderLease&&a.trackingEpoch==b.trackingEpoch&&a.cancelGeneration==b.cancelGeneration;
}
inline bool sameSource(const body::ContactOwnerH24& a,const body::ContactOwnerH24& b) noexcept {
 return sameIncarnation(a,b)&&a.sourceSample==b.sourceSample&&a.sourceTick==b.sourceTick&&a.observedTick==b.observedTick;
}
inline bool decode(const Raw& r,const body::ContactOwnerH24& s,std::uint64_t now,Event& e) noexcept {
 e={};if(!active(s,now)||at<int>(r,8)!=2)return false;
 bool own[2]{};
 for(unsigned i=0;i<2;++i)own[i]=at<std::uintptr_t>(r,0x10+8*i)==s.body&&
  at<std::uintptr_t>(r,0x20+8*i)==s.foreign&&at<int>(r,0x30+4*i)==s.foreignType;
 if(own[0]==own[1])return false;
 const int self=own[0]?0:1,other=1-self;e.source=s;e.tick=now;e.selfSide=self;
 e.otherBody=at<std::uintptr_t>(r,0x10+8*other);e.otherForeign=at<std::uintptr_t>(r,0x20+8*other);
 e.otherForeignType=at<int>(r,0x30+4*other);
 // Native world+A8 ->52950C returns the ID of pEntity[1]. When our proxy is
 // side1, +38 names OUR ID; it is never re-labelled as the other body ID.
 e.otherPhysicsId=self==0?at<int>(r,0x38):-1;e.otherPhysicsIdKnown=self==0&&e.otherPhysicsId>=0;
 if(self==1&&at<int>(r,0x38)!=s.bodyId)return false;
 e.point=at<V>(r,0x3C);e.normal=at<V>(r,0x48);
 e.selfVelocity=at<V>(r,0x54+12*self);e.otherVelocity=at<V>(r,0x54+12*other);
 e.selfMass=at<float>(r,0x6C+4*self);e.otherMass=at<float>(r,0x6C+4*other);
 e.selfPart=at<int>(r,0x74+4*self);e.otherPart=at<int>(r,0x74+4*other);
 e.selfMaterial=at<short>(r,0x7C+2*self);e.otherMaterial=at<short>(r,0x7C+2*other);
 e.penetration=at<float>(r,0x84);e.normalImpulse=at<float>(r,0x88);e.radius=at<float>(r,0x8C);
 const float n=e.normal.x*e.normal.x+e.normal.y*e.normal.y+e.normal.z*e.normal.z;
 return e.otherBody&&e.otherBody!=s.body&&e.selfPart==0&&e.otherPart>=-1&&
  finite(e.point)&&finite(e.normal)&&n>.64f&&n<1.44f&&
  finite(e.selfVelocity)&&finite(e.otherVelocity)&&std::isfinite(e.selfMass)&&e.selfMass>0&&
  std::isfinite(e.otherMass)&&e.otherMass>=0&&std::isfinite(e.penetration)&&e.penetration>=0&&
  std::isfinite(e.normalImpulse)&&e.normalImpulse>=0&&std::isfinite(e.radius)&&e.radius>=0;
}
// Functional fixed POD queue, independently of diagnostic capture/rate limits.
class Queue {
 std::array<Event,32> data{};unsigned head=0,count=0;std::atomic_flag busy=ATOMIC_FLAG_INIT;
 std::atomic<std::uint64_t> dropped{0};
public:
 bool push(Event e) noexcept {if(busy.test_and_set(std::memory_order_acquire)){++dropped;return false;}
  if(count==data.size()){busy.clear(std::memory_order_release);++dropped;return false;}
  data[(head+count)%data.size()]=e;++count;busy.clear(std::memory_order_release);return true;}
 bool pop(Event& e) noexcept {if(busy.test_and_set(std::memory_order_acquire))return false;
  if(!count){busy.clear(std::memory_order_release);return false;}e=data[head];head=(head+1)%data.size();--count;busy.clear(std::memory_order_release);return true;}
 std::uint64_t lost()const noexcept{return dropped.load(std::memory_order_relaxed);}
};
inline Queue events,parryEvents;
inline std::atomic<std::uint64_t> occurrenceSerial{0};
// The actual collision producer, BA7168, supplies occurrence time and source.
// The later logged callback must not re-label an old native event as a new swing.
// No event/body dereference occurs after the original producer returns.
template<class Read,class Now,class CurrentWorld,class Original> int observeProducer(void* world,unsigned flags,const void* ev,void* queuedOut,bool contract,
 Read&& read,Now&& now,CurrentWorld&& currentWorld,Original&& original) noexcept {
 body::ContactOwnerH24 before{};Raw raw{};Event candidate{};
 const auto begin=now();const bool copied=contract&&(flags&body::kLogCollisionsH24)&&world&&
  reinterpret_cast<std::uintptr_t>(world)==currentWorld()&&ev&&read(ev,raw.bytes.data(),0x38)&&at<int>(raw,8)==2&&
  (at<int>(raw,0x30)==0x5634||at<int>(raw,0x34)==0x5634)&&
  body::ReadContactOwnerH24(before)&&active(before,begin)&&read(ev,raw.bytes.data(),raw.bytes.size())&&decode(raw,before,begin,candidate);
 const int result=original(world,flags,ev,queuedOut);
 body::ContactOwnerH24 after{};const auto done=now();
 if(copied&&reinterpret_cast<std::uintptr_t>(world)==currentWorld()&&body::ReadContactOwnerH24(after)&&active(after,done)&&
  sameSource(before,after)&&fresh(candidate.tick,done)){
  candidate.serial=occurrenceSerial.fetch_add(1,std::memory_order_relaxed)+1;
  (void)events.push(candidate);(void)parryEvents.push(candidate);
 }
 return result;
}
// A copied contact is evidence only. Existing game-thread RPG admission must
// still join current opponent/entity/part/gesture/native rules; never damage here.
inline bool readFrom(Queue& queue,Event& e,std::uint64_t now) noexcept {
 body::ContactOwnerH24 current{};if(!body::ReadContactOwnerH24(current)||!active(current,now))return false;
 for(unsigned i=0;i<32&&queue.pop(e);++i)if(fresh(e.tick,now)&&sameIncarnation(e.source,current)&&
  e.source.sourceSample<=current.sourceSample&&e.source.sourceTick<=current.sourceTick)return true;
 e={};return false;
}
inline bool ReadContactEventH24(Event& e,std::uint64_t now) noexcept {return readFrom(events,e,now);}
inline bool ReadParryEventH24(Event& e,std::uint64_t now) noexcept {return readFrom(parryEvents,e,now);}
template<class Read> bool contract(Read&& read) noexcept {
 const struct Pin{std::uintptr_t rva;unsigned char bytes[10];unsigned size;} pins[]{
  {0xAF5F80,{0xC7,0x43,0x08,0x02,0,0,0},7}, // constructor event2
  {0xBA7191,{0x0F,0xBA,0xE6,0x19},4}, // log collision bit25
  {0xBA71C0,{0xBA,2,0,0,0,0x41,0xB8,0xA8,0,0},10}, // event2, sizeA8
  {0x38FC4CF,{0x48,0x89,0x7D,0x10,0x48,0x89,0x55,0x18},8}, // both physical bodies
  {0x38FC4C4,{0x0F,0xBF,0x47,0x28},4}, // signed foreign kind
  {0xBA6E87,{0xFF,0x90,0xA8,0,0,0},6}, // current world physical ID getter
  {0xBA6EA0,{0x89,0x45,0x98},3}, // event base rbp-A0; ID at+38
 };
 for(const auto& p:pins){unsigned char bytes[10]{};if(!read(p.rva,bytes,p.size)||std::memcmp(bytes,p.bytes,p.size))return false;}return true;
}
}
