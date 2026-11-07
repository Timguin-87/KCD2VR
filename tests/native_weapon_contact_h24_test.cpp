#include "subsystems/physics/runtime/native_weapon_contact_event_h24.h"
#include <cstdio>
#include <cstdlib>
#include <limits>
#ifndef _MSC_VER
#define __fastcall
#endif
namespace body=kcd2vr::native_weapon_body_t4;namespace ct=kcd2vr::native_weapon_contact_h24;
static body::ContactOwnerH24 owner;static bool ownerReadable=true;
namespace kcd2vr::native_weapon_body_t4 {bool ReadContactOwnerH24(ContactOwnerH24& out) noexcept {out=owner;return ownerReadable;}}
static std::uint64_t clockH24=1000;static unsigned reads=0,originals=0;static int mutate=0;static const void* originalArgument=nullptr;
static std::uintptr_t currentWorldH24=0x3333;static std::uintptr_t g_base=reinterpret_cast<std::uintptr_t>(&currentWorldH24)-0x492D830;
static bool SehCopy146(const void* p,void* out,std::size_t n){++reads;std::memcpy(out,p,n);return true;}
static std::uint64_t GetTickCount64(){return clockH24;}
static int original(void* w,unsigned f,const void* p,void* out){++originals;originalArgument=p;if(w!=reinterpret_cast<void*>(0x3333)||f!=0x02000000u||out!=reinterpret_cast<void*>(0x4444))std::abort();
 if(mutate==1)owner.active=false;if(mutate==2)++owner.bodyGeneration;if(mutate==3)++owner.sourceSample;
 if(mutate==4)owner.foreign+=8;if(mutate==5)++clockH24;if(mutate==6)std::memset(const_cast<void*>(p),0,0xA8);
 return 73;}
static int(*g_phys_produce_original_h24)(void*,unsigned,const void*,void*)=original;
static unsigned loggedOriginals=0;static int loggedOriginal(const void*){++loggedOriginals;return 91;}static int(*g_phys_original)(const void*)=loggedOriginal;static std::atomic<bool>g_collision_contract_h24{true};
namespace kcd2vr::hookcost178r22 {enum K{PhysCollision};struct Scope{Scope(K){}};struct Original{Original(K){}};}
static bool Active(){return false;}static bool Allow(int,int){return false;}static bool IsPlayer(const void*){return false;}
static unsigned EntityId(const void*){return 0;}static std::atomic<std::uint64_t>g_counts[11]{};
template<class T>static T Rd(const void* p,std::uintptr_t off,T fallback){if(!p)return fallback;T v;std::memcpy(&v,static_cast<const char*>(p)+off,sizeof(v));return v;}
namespace diagnostic_trace {static void Write(const char*){}}
#include "h24_collision/actual_detour_h24.inc"
static int callProduction(const void* e){return PhysCollisionProduceDetourH24(reinterpret_cast<void*>(0x3333),0x02000000u,e,reinterpret_cast<void*>(0x4444));}
static unsigned checks=0;static void ck(bool b){++checks;if(!b){std::printf("FAIL check%u\n",checks);std::abort();}}
template<class T>void set(ct::Raw& r,unsigned off,T v){std::memcpy(r.bytes.data()+off,&v,sizeof(v));}
static ct::Raw sample(int side=0){ct::Raw r{};set(r,8,2);set(r,0x10+8*side,owner.body);set(r,0x20+8*side,owner.foreign);set(r,0x30+4*side,owner.foreignType);
 set(r,0x10+8*(1-side),std::uintptr_t(0x9999));set(r,0x20+8*(1-side),std::uintptr_t(0x8888));set(r,0x30+4*(1-side),2);
 set(r,0x38,side?owner.bodyId:88);set(r,0x3C,ct::V{1,2,3});set(r,0x48,ct::V{0,1,0});
 set(r,0x54,ct::V{3,0,0});set(r,0x60,ct::V{0,0,0});set(r,0x6C,1.f);set(r,0x70,2.f);
 set(r,0x74+4*side,0);set(r,0x74+4*(1-side),12);set(r,0x88,3.f);return r;}
static void reset(){ct::Event e;while(ct::events.pop(e)){};while(ct::parryEvents.pop(e)){};owner={};owner.active=true;owner.owner.key=12;owner.owner.generation=2;owner.owner.player=1;owner.owner.epoch=3;owner.owner.request=4;
 owner.body=0x1111;owner.foreign=0x2222;owner.foreignType=0x5634;owner.bodyId=42;owner.worldGeneration=5;owner.bodyGeneration=6;owner.colliderLease=7;owner.trackingEpoch=8;owner.sourceSample=9;owner.sourceTick=owner.observedTick=clockH24=1000;
 ownerReadable=true;reads=originals=mutate=0;g_collision_contract_h24=true;}
int main(){
 reset();auto r=sample();ck(callProduction(&r)==73&&originals==1&&originalArgument==&r);ct::Event e{};
 ck(ct::ReadContactEventH24(e,1000)&&e.selfSide==0&&e.otherPhysicsIdKnown&&e.otherPhysicsId==88&&e.otherBody==0x9999&&e.otherPart==12&&e.normalImpulse==3&&e.serial);
 const auto sharedSerial=e.serial;ck(ct::ReadParryEventH24(e,1000)&&e.serial==sharedSerial);
 ck(reads==4&&!ct::ReadContactEventH24(e,1000)); // diagnostic Active FALSE does not gate functional contact.
 reset();r=sample(1);ck(callProduction(&r)==73&&originals==1);ck(ct::ReadContactEventH24(e,1000)&&e.selfSide==1&&!e.otherPhysicsIdKnown&&e.otherPhysicsId==-1);
 for(int m=1;m<=4;++m){reset();r=sample();mutate=m;ck(callProduction(&r)==73&&originals==1);ck(!ct::ReadContactEventH24(e,1000));}
 reset();r=sample();mutate=6;ck(callProduction(&r)==73);ck(ct::ReadContactEventH24(e,1000)&&e.point.x==1&&e.otherPart==12&&reads==4); // no post-original event reads.
 for(int bad=0;bad<10;++bad){reset();r=sample();if(bad==0)g_collision_contract_h24=false;if(bad==1)owner.active=false;if(bad==2)set(r,0x20,owner.foreign+1);if(bad==3)set(r,0x30,2);if(bad==4)set(r,0x10,owner.body+1);if(bad==5)set(r,8,1);if(bad==6)set(r,0x48,ct::V{});if(bad==7)set(r,0x88,std::numeric_limits<float>::quiet_NaN());if(bad==8)owner.sourceTick=900;if(bad==9)set(r,0x74,1);
  ck(callProduction(&r)==73&&originals==1);ck(!ct::ReadContactEventH24(e,1000));}
 reset();r=sample();owner.foreign+=8;owner.bodyGeneration++;ck(callProduction(&r)==73);ck(!ct::ReadContactEventH24(e,1000)); // delayed oldevent same body address/ID, distinct incarnationcookie
 reset();r=sample();ck(callProduction(&r)==73);owner.bodyGeneration++;ck(!ct::ReadContactEventH24(e,1000));
 reset();r=sample();ck(callProduction(&r)==73);owner.sourceSample++;owner.sourceTick=owner.observedTick=1001;ck(ct::ReadContactEventH24(e,1001)); // next-frame consumption same incarnation.
 reset();r=sample();for(int i=0;i<40;++i)ck(callProduction(&r)==73);unsigned n=0;while(ct::ReadContactEventH24(e,1000))++n;ck(n==32&&ct::events.lost()>=8);
 reset();r=sample();ck(callProduction(&r)==73);owner.sourceSample=10;owner.sourceTick=owner.observedTick=clockH24=1020;
 ck(ct::ReadContactEventH24(e,1020)&&e.tick==1000&&e.source.sourceSample==9&&e.source.sourceTick==1000);
 const auto loggedBefore=loggedOriginals;ck(PhysCollisionDetour(&r)==91&&loggedOriginals==loggedBefore+1);ck(!ct::ReadContactEventH24(e,1020)); // delayed callback cannot mint another newer-swing contact.
 std::printf("PASS %u actual native collision detour checks; no engine/headset execution\n",checks);
}
