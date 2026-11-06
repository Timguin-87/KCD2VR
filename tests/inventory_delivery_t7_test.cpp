// Executes the production native actor reader/service. Engine leaves are spies.
#include <atomic>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <map>
#include <string>
#include <type_traits>
#include "../src/subsystems/throw_drop/inventory_lifecycle_t8.h"
#include "../src/vrik/weapon_release_t1.h"
// Feature constants are compile-time leaves matching the production T22 header.
namespace kcd2vr::ammo_service_t22 {inline constexpr bool integrated=true,leftChestKnivesT26=true;}
namespace kcd2vr {
static constexpr std::uintptr_t module=0x10000000,cry=0x20000000,game=0x30000000,player=0x40000000,entity=0x50000000;
static std::map<std::uintptr_t,std::uintptr_t> memory;
static bool fingerprint=true,callOK=true,replaceDuring=false,blockDuringActorRead=false;
static unsigned calls=0,reads=0;static std::uint32_t registeredId=42;static std::string answer="WAIT",lastInput;
static std::uintptr_t WhBase() noexcept {return module;}
template<class T> T Rd(const void* p,std::size_t off,T fallback) noexcept {
 ++reads;if(blockDuringActorRead&&reinterpret_cast<std::uintptr_t>(p)+off==cry+0x88)inventory_lifecycle_t8::Publish(true,true);auto it=memory.find(reinterpret_cast<std::uintptr_t>(p)+off);if(it==memory.end())return fallback;
 if constexpr(std::is_pointer_v<T>)return reinterpret_cast<T>(it->second);else return static_cast<T>(it->second);
}
static bool Vft13(const void* p,std::uintptr_t v) noexcept {return p&&Rd<std::uintptr_t>(p,0,0)==module+v;}
static void* Entity13(std::uint32_t id) noexcept {return id==registeredId?reinterpret_cast<void*>(entity):nullptr;}
namespace retail_build112 {static bool Ready(std::uintptr_t b) noexcept {return fingerprint&&b==module;}}
namespace diagnostic_trace {static unsigned logs=0;static void Write(const char*) noexcept {++logs;}}
namespace native_script_t2 {
struct Context {std::uint64_t now=0,generation=1;
 bool CallPacket(const char* name,const char* input,char* out,std::size_t cap) const noexcept {
  assert(!(inventory_lifecycle_t8::state.load()&1));assert(!std::strcmp(name,"kcd2vr_inventory_native_t8"));++calls;lastInput=input;
  if(!callOK)return false;
  const auto divider=lastInput.find('|');assert(lastInput.substr(divider+1,3)=="42|");
  std::snprintf(out,cap,"%s|%s|5|3",lastInput.substr(0,divider).c_str(),answer.c_str());
  if(replaceDuring)memory[game+0xA40]=0;
  return true;
 }
};
static bool Register(void(*)(const Context&)noexcept,bool(*)()noexcept)noexcept{return true;}
static bool Install()noexcept{return true;}
}
#include "../src/subsystems/throw_drop/runtime/inventory_delivery_t7.inl"
}
int main(){
 using namespace kcd2vr;using namespace inventory_t7;
 memory={{module+0x549D328,cry},{cry,module+0x40472D0},{cry+0x88,game},
  {game,module+0x3AA5988},{game+0xA40,player},{player,module+0x3AA4B98},
  {player+0x38,entity},{player+0x30,42},{entity,module+0x4072C78},{entity+0xC,42}};
 Actor a{};assert(ReadActor(a)&&a.player==player&&a.id==42);
 for(auto field:{cry,game,player,entity,player+0x30,game+0xA40,player+0x38}){
  auto old=memory[field];memory[field]=0;assert(!ReadActor(a));memory[field]=old;
 }
 registeredId=0x1000001;memory[player+0x30]=memory[entity+0xC]=registeredId;
 assert(!ReadActor(a)); // Native numeric branch cannot represent this ID exactly.
 registeredId=0x1000002;memory[player+0x30]=memory[entity+0xC]=registeredId;
 assert(ReadActor(a)); // Representable high IDs remain supported.
 registeredId=42;memory[player+0x30]=memory[entity+0xC]=registeredId;
 inventory_lifecycle_t8::PublishUi(1,0,true,3);assert(!Wanted());
 inventory_lifecycle_t8::PublishUi(2,0,true,3);assert(!Wanted());
 inventory_lifecycle_t8::PublishUi(3,1,true,3);assert(!Wanted());
 inventory_lifecycle_t8::PublishUi(3,0,false,3);assert(!Wanted());
 inventory_lifecycle_t8::PublishUi(3,0,true,3);
 weapon_release_t1::SetMode(0);collab_h21::throwing=false;
 assert(!weapon_release_t1::TestEnabledH21()&&Wanted()); // LEFT chest starter independent of RIGHT release/F4.

 native_script_t2::Context ctx{0,1};Service(ctx);ctx.now=2000;fingerprint=false;Service(ctx);assert(calls==0&&Wanted());
 fingerprint=true;ctx.now=3000;callOK=false;Service(ctx);assert(calls==1&&Wanted());
 ctx.now=3001;Service(ctx);assert(calls==1); // No per-frame native/VM polling.
 ctx.now=4000;callOK=true;answer="UNKNOWN";Service(ctx);assert(calls==2&&Wanted());
 ctx.now=5000;answer="DONE";replaceDuring=true;Service(ctx);assert(Wanted()); // Rejoin required after native callback.
 replaceDuring=false;memory[game+0xA40]=player;
 ctx.now=6000;Service(ctx);assert(!Wanted());
 auto oldCalls=calls,oldReads=reads;ctx.now=7000;Service(ctx);assert(calls==oldCalls&&reads==oldReads);
 for(const char* status:{"ALREADY_CLAIMED","CLAIM_REFUSED","PARTIAL_UNCERTAIN"}){
  pending=true;answer=status;ctx.now+=1000;Service(ctx);assert(!Wanted());
 }
 pending=true;memory[game+0xA40]=0;
 for(int i=0;i<310;++i){ctx.now+=1000;Service(ctx);}assert(!Wanted());
 oldReads=reads;ctx.now+=1000;Service(ctx);assert(reads==oldReads);
 // A terminal startup must rearm for a second save load, including same actor address.
 inventory_lifecycle_t8::Publish(true,true);oldReads=reads;oldCalls=calls;
 for(int i=0;i<12;++i){ctx.now+=1000;Service(ctx);}assert(reads==oldReads&&calls==oldCalls);
 inventory_lifecycle_t8::Publish(true,false);memory[game+0xA40]=player;answer="DONE";
 assert(Wanted());ctx.now+=1000;Service(ctx);assert(calls==oldCalls); // settle only, no native work
 ctx.now+=2000;Service(ctx);assert(calls==oldCalls+1&&!Wanted());
 assert(inventory_lifecycle_t8::daggers==5&&inventory_lifecycle_t8::axes==3);
 inventory_lifecycle_t8::Recheck();ctx.now+=1000;Service(ctx);ctx.now+=2000;
 blockDuringActorRead=true;oldCalls=calls;Service(ctx);assert(calls==oldCalls&&!Wanted());blockDuringActorRead=false;
 inventory_lifecycle_t8::Publish(false,false);assert(!Wanted()); // lost loading knowledge blocks calls
 inventory_lifecycle_t8::Publish(true,false);inventory_lifecycle_t8::Recheck();assert(Wanted());
 inventory_lifecycle_t8::RequestRecovery();assert(inventory_lifecycle_t8::recoveryLease==inventory_lifecycle_t8::state);
 ctx.now+=1000;Service(ctx);ctx.now+=2000;Service(ctx);assert(lastInput.ends_with("|1"));
 inventory_lifecycle_t8::Publish(true,true);inventory_lifecycle_t8::Publish(true,false);
 ctx.now+=1000;Service(ctx);ctx.now+=2000;Service(ctx);assert(lastInput.ends_with("|0"));
 std::puts("PASS T8 actual source: exact current actor, original-first callback packet, bounded retries and terminal idle");
}
