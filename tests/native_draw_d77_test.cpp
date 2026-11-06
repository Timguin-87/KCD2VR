#include <windows.h>
#include <array>
#include <cassert>
#include <cstring>
#include <optional>
#include <cstdio>
#include "../src/diagnostics/capture168.h"
#include "../src/diagnostics/draw_native_d77.h"
namespace DetourModKit::hook {
enum class Gpr {Rcx,Rdx,Rbx,R14,Rsi,Rdi};
struct MidContext {std::array<std::uintptr_t,6> r{};std::uintptr_t capturedsp=0;};
inline std::uintptr_t gpr(MidContext& c,Gpr r){return c.r[unsigned(r)];}
inline std::uintptr_t stack_pointer(const MidContext& c){return c.capturedsp;}
struct Hook {};
}
static constexpr std::uintptr_t kCEntityVftRva=0x4072C78;
static std::uintptr_t WhBase(){return 0x10000000;}
namespace retail_build112 {inline bool Ready(std::uintptr_t){return true;}}
static unsigned reads=0;
template<class T> bool Read(const void* p,T& value){++reads;if(reinterpret_cast<std::uintptr_t>(p)<0x10000000)return false;std::memcpy(&value,p,sizeof(T));return true;}
static bool PhysVft6698(void* p,std::uintptr_t rva){std::uintptr_t v=0;return Read(p,v)&&v==WhBase()+rva;}
enum class InitialPendingH36r3 {Unknown,Absent,Present};
static std::uintptr_t pendingItem=0;
static InitialPendingH36r3 ReadInitialPendingH36r3(std::uintptr_t,std::uint64_t,std::uintptr_t& item){item=pendingItem;return item?InitialPendingH36r3::Present:InitialPendingH36r3::Absent;}
#define D77_NATIVE_ADAPTER_TEST
#include "../src/diagnostics/draw_native_d77.inl"
struct ProfileFixture {std::uint64_t key=0,generation=0;int classId=0;bool bow()const{return classId==5;}bool crossbowD72()const{return classId==6;}};
struct PhysEquipmentItem6698 {ProfileFixture profile;void* item=nullptr;};
struct PhysEquipment6698 {std::array<PhysEquipmentItem6698,2> items;int count=0;void* player=nullptr;void* manager=nullptr;};
namespace ph {struct Command {std::uint64_t key=0,generation=0,tick=0,bodyEpoch=0,epoch=0,sample=0;int side=0;};}
static std::uint64_t ReadCompletedPublicationT23(){return 12;}
namespace tracking59c6 {inline std::atomic<std::uint64_t> epoch{4};}
static std::atomic<std::uintptr_t> g_player_char37{0xabc};
#include "../src/subsystems/throw_drop/draw_observer_host_d77.inl"
static std::array<unsigned char,64> allocation{},copyAllocation{};
static void* __fastcall allocStub(void*){return allocation.data();}
static void* __fastcall cloneStub(void*){return copyAllocation.data();}
static std::uint64_t invoked=0;static unsigned originals=0;
static void __fastcall invokeStub(void*){invoked=dn77::invoking;++originals;}
static bool scopedStub(void*){assert(dn77::invoking);return true;}
static void* __fastcall ctorStub(void* storage,void*,void*,unsigned char){return storage;}
static void __fastcall throwingOriginal(void*){++originals;g_d77_spawn_params=12345;g_d77_spawn_draw=67890;RaiseException(0xE0000077,0,0,nullptr);}
static bool checkSeh(void* self){__try{DrawInvokeWorldOriginalD77(self,throwingOriginal);}__except(EXCEPTION_EXECUTE_HANDLER){return true;}return false;}
static void put(void* p,std::size_t offset,std::uintptr_t value){std::memcpy(static_cast<unsigned char*>(p)+offset,&value,8);}
int main(){
 using namespace kcd2vr::diagnostics;using R=DetourModKit::hook::Gpr;using C=DetourModKit::hook::MidContext;
 const auto now=GetTickCount64();capture168::gate.select(capture168::Domain::Bug,true,now);const auto session=capture168::gate.session[7].load();
 g_draw_native_ready_d77=true;dn77::recorder.enable(session);dn77::recorder.clock=DrawStampD77;
 dn77::Draw d{};d.session=session;d.item=0x1234;d.key=55;d.tick=now;d.side=0;d.body=2;d.request=3;
 auto id=dn77::recorder.arm(d);assert(id);assert(dn77::invokeScoped(id,scopedStub,nullptr));assert(!dn77::invoking);
 auto scope=dn77::begin(id);C c{};c.r[unsigned(R::Rcx)]=0x100;c.r[unsigned(R::Rdx)]=d.item;c.r[unsigned(R::Rbx)]=99;DrawTokenSubmittedD77(c);
 g_d77_alloc=allocStub;g_d77_clone=cloneStub;g_d77_invoke=invokeStub;
 auto* first=DrawAllocateD77(nullptr);auto receipt=dn77::recorder.findCommand(reinterpret_cast<std::uintptr_t>(first),now);assert(receipt.draw==id);
 auto* clone=DrawCloneD77(first);dn77::end(scope);DrawCommandInvokeD77(static_cast<unsigned char*>(clone)+8);assert(invoked==id&&!dn77::invoking&&originals==1);
 // Actual parent host wrapper allocates only an accepted item/class occurrence;
 // unmatched and OFF paths call original without manufacturing observer identity.
 PhysEquipment6698 equipment{};equipment.count=1;equipment.items[0].item=reinterpret_cast<void*>(0x7654);equipment.items[0].profile={77,4,5};
 ph::Command command{};command.key=77;command.generation=4;command.side=1;
 unsigned hostCalls=0;std::uint64_t hostScope=0;auto host=[&](){++hostCalls;hostScope=dn77::invoking;return true;};
 assert(PhysObserveDrawInvokeD77(equipment,command,host)&&hostCalls==1&&hostScope&&!dn77::invoking);
 command.generation=5;assert(PhysObserveDrawInvokeD77(equipment,command,host)&&hostCalls==2&&!hostScope);
 // Actual Windows SEH restores both causal draw and borrowed spawn parameters.
 std::array<unsigned char,80> outer{};put(outer.data(),0,WhBase()+0x3EBC600);put(outer.data(),8,0x100);put(outer.data(),16,99);pendingItem=0x1234;
 g_d77_spawn_params=10;g_d77_spawn_draw=11;auto previousScope=dn77::begin(0); // replaced below
 dn77::end(previousScope);
 assert(checkSeh(outer.data()+8));assert(!dn77::invoking&&g_d77_spawn_params==10&&g_d77_spawn_draw==11);assert(originals==2);
 // Both hands independent; no F9 gate is consulted anywhere in adapters.
 d.side=1;d.key=56;d.item=0x2345;const auto left=dn77::recorder.arm(d);assert(left&&left!=id);
 auto stale=receipt;C retireCommand{};retireCommand.r[unsigned(R::Rcx)]=reinterpret_cast<std::uintptr_t>(first);put(first,0,WhBase()+0x123);DrawCommandRetireD77(retireCommand);assert(dn77::recorder.findCommand(reinterpret_cast<std::uintptr_t>(first),now).draw==id);put(first,0,WhBase()+0x3E74FE8);DrawCommandRetireD77(retireCommand);assert(!dn77::recorder.findCommand(reinterpret_cast<std::uintptr_t>(first),now).draw);dn77::recorder.create(left,reinterpret_cast<std::uintptr_t>(first),now);dn77::recorder.clone(stale,0x9000,now);assert(!dn77::recorder.findCommand(0x9000,now).draw);
 // Actual callback scope restored, exact token->original item only.
 assert(dn77::recorder.completion(0x100,99,0x1234,now)==id);assert(!dn77::recorder.completion(0x100,99,0x2345,now));
 dn77::recorder.entity(id,0x5555,42,now);assert(dn77::recorder.byEntity(0x5555,42,now).occurrence==id);
 // Execute actual binding, visibility and registration observers with owned fixtures.
 std::array<unsigned char,256> itemMemory{},linkMemory{},entityMemory{},bindingMemory{};
 std::array<unsigned char,320> proxyMemory{};std::array<unsigned char,16> payloadMemory{};
 auto ptr=[](auto& value){return reinterpret_cast<std::uintptr_t>(value.data());};
 put(entityMemory.data(),0,WhBase()+kCEntityVftRva);std::uint32_t eid=88;std::memcpy(entityMemory.data()+0xC,&eid,4);
 put(itemMemory.data(),0x88,ptr(linkMemory));put(linkMemory.data(),0,WhBase()+0x3EC85E0);
 put(linkMemory.data(),0x58,ptr(itemMemory));put(linkMemory.data(),0x38,ptr(entityMemory));std::memcpy(linkMemory.data()+0x30,&eid,4);
 d.side=0;d.item=ptr(itemMemory);d.key=80;put(itemMemory.data(),0,WhBase()+0x3A2DA88);put(itemMemory.data(),0x30,d.key);auto observed=dn77::recorder.arm(d);DrawObserveExistingD77(observed);assert(dn77::recorder.byEntity(ptr(entityMemory),eid,now).occurrence==observed);auto nativeScope=dn77::begin(observed);g_d77_ctor=ctorStub;
 put(bindingMemory.data(),0,WhBase()+0x3A68F68);
 assert(DrawBindingCtorD77(bindingMemory.data(),entityMemory.data(),nullptr,3)==bindingMemory.data());dn77::end(nativeScope);
 C stored{};stored.r[unsigned(R::Rbx)]=ptr(bindingMemory);stored.r[unsigned(R::Rdi)]=0x8888;DrawBindingStoredD77(stored);
 std::uintptr_t caller=WhBase()+0x9DFDAC;C hide{};hide.r[unsigned(R::Rcx)]=ptr(entityMemory);hide.capturedsp=reinterpret_cast<std::uintptr_t>(&caller);DrawVisibilityD77(hide);
 put(proxyMemory.data(),0,WhBase()+0x3A49FD8);put(proxyMemory.data(),0x80,ptr(entityMemory));put(payloadMemory.data(),8,ptr(proxyMemory));payloadMemory[0]=1;
 C registration{};registration.r[unsigned(R::Rcx)]=ptr(payloadMemory);DrawProxyRegistrationD77(registration);
 C retire{};retire.r[unsigned(R::Rcx)]=ptr(bindingMemory);DrawBindingRetireD77(retire);retire.r[unsigned(R::Rcx)]=ptr(proxyMemory);DrawProxyRetireD77(retire);
 dn77::Event actual{};bool ctor=false,store=false,visibility=false,registrationOnly=false,retired=false,existing=false;
 while(dn77::recorder.pop(actual))if(actual.draw.occurrence==observed){existing|=actual.kind==dn77::Kind::ExistingEntity;ctor|=actual.kind==dn77::Kind::BindingCreate;store|=actual.kind==dn77::Kind::BindingStore;visibility|=actual.kind==dn77::Kind::Hide&&actual.caller==caller;registrationOnly|=actual.kind==dn77::Kind::ProxyRegister&&!actual.transformValid;retired|=actual.kind==dn77::Kind::BindingRetire;}
 assert(existing&&ctor&&store&&visibility&&registrationOnly&&retired);
 // New-world native spawn publication requires same exact SpawnParams and draw.
 d.key=81;d.item=0x8765;auto fresh=dn77::recorder.arm(d);auto spawnScope=dn77::begin(fresh);
 C spawnRequest{};spawnRequest.r[unsigned(R::R14)]=d.item;spawnRequest.r[unsigned(R::Rdx)]=0xabc0;DrawSpawnRequestD77(spawnRequest);
 C spawnPublish{};spawnPublish.r[unsigned(R::Rbx)]=ptr(entityMemory);spawnPublish.r[unsigned(R::Rsi)]=0xdef0;DrawSpawnPublishD77(spawnPublish);
 bool foreignSpawn=false;while(dn77::recorder.pop(actual))foreignSpawn|=actual.draw.occurrence==fresh&&actual.kind==dn77::Kind::Spawn;assert(!foreignSpawn);
 spawnPublish.r[unsigned(R::Rsi)]=0xabc0;DrawSpawnPublishD77(spawnPublish);bool exactSpawn=false;while(dn77::recorder.pop(actual))exactSpawn|=actual.draw.occurrence==fresh&&actual.kind==dn77::Kind::Spawn;assert(exactSpawn);
 DrawSpawnReturnedD77(spawnPublish);dn77::end(spawnScope);
 // New F2 session cannot admit the previous world's token, entity or allocation.
 capture168::gate.select(capture168::Domain::Bug,true,now+1);dn77::recorder.enable(session+1);
 assert(!dn77::recorder.completion(0x100,99,0x1234,now+1));assert(!dn77::recorder.byEntity(0x5555,42,now+1).occurrence);assert(!dn77::recorder.findCommand(reinterpret_cast<std::uintptr_t>(clone),now+1).draw);
 // OFF retains original calls but records no pointer reads: invalid input safe.
 capture168::gate.select(capture168::Domain::Bug,false,now+2);dn77::recorder.enable(0);const auto offReads=reads;C invalid{};invalid.r.fill(1);DrawVisibilityD77(invalid);DrawProxyRegistrationD77(invalid);DrawTokenSubmittedD77(invalid);DrawCommandInvokeD77(reinterpret_cast<void*>(1));assert(originals==3&&!invoked&&reads==offReads);
 // Stale scopes and budget/expiry cannot generate apparently complete evidence.
 dn77::Recorder bounded;bounded.enable(9);d.session=9;d.tick=now;auto b=bounded.arm(d);assert(b);for(unsigned i=0;i<150;++i)bounded.mark(b,dn77::Kind::Hide,now);assert(bounded.lost.load()>0);assert(!bounded.lookup(b,now+1501).occurrence);
 dn77::Event event{};unsigned total=0;std::uint64_t previous=0;while(bounded.pop(event)){assert(event.sequence>previous);previous=event.sequence;++total;}assert(total==96);
 std::puts("PASS: actual host/native adapters; both hands; existing/redraw and exact new-world spawn; clone ABA/lifetime; token mismatch; F2 restart/OFF zero reads; genuine SEH original/scope preservation; bounded events");
}
