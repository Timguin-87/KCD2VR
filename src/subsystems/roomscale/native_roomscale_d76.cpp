#include "native_roomscale_d76.h"
#include "receipt_ledger_d76.h"
#include <atomic>
#include <mutex>
#include <cstring>
#include <array>
#include <optional>
#include <cstdio>
#include <string>
#if defined(_WIN32)
#include <Windows.h>
#if !defined(KCD2VR_NATIVE_ROOMSCALE_D76_TEST)
#include <DetourModKit/hook.hpp>
#endif
#include "../../retail_build112.h"
#include "../../diagnostic_trace.h"
#endif

namespace kcd2vr::native_roomscale_d76 {
namespace {
std::mutex mutex;
detail::State state;
std::atomic<std::uintptr_t> enrolledEntity{0},enrolledCharacter{0};
std::atomic<bool> installed{false};
std::uintptr_t base=0;
std::uint64_t occurrence=0;
struct SlotOrigin { std::uintptr_t slot=0; Origin origin{}; bool valid=false; };
SlotOrigin slotOrigin;
struct StepScope { std::uintptr_t body=0,world=0; std::uint64_t number=0; Origin origin{}; bool injected=false,emitted=false; };
struct EntityScope { std::uintptr_t proxy=0,event=0,entity=0; Origin origin{}; bool matched=false,writing=false,unchanged=false; std::uint64_t previousOccurrence=0; };
struct RefreshScope { std::uintptr_t slot=0,entity=0; Origin origin{}; bool valid=false,insideWriter=false; std::uint64_t entitySerial=0; };
struct CharacterScope { std::uintptr_t slot=0,entity=0,character=0,params=0; Origin origin{}; bool valid=false,writing=false; };
thread_local StepScope step;
thread_local EntityScope entity;
thread_local RefreshScope refresh;
thread_local CharacterScope character;

bool copy(std::uintptr_t address,void*out,std::size_t n) noexcept {
#if defined(_MSC_VER)
 __try { if(!address||!out)return false;std::memcpy(out,reinterpret_cast<const void*>(address),n);return true; }
 __except(EXCEPTION_EXECUTE_HANDLER){return false;}
#else
 (void)address;(void)out;(void)n;return false;
#endif
}
bool write(std::uintptr_t address,const void*value,std::size_t n) noexcept {
#if defined(_MSC_VER)
 __try { if(!address||!value)return false;std::memcpy(reinterpret_cast<void*>(address),value,n);return true; }
 __except(EXCEPTION_EXECUTE_HANDLER){return false;}
#else
 (void)address;(void)value;(void)n;return false;
#endif
}
template<class T>bool read(std::uintptr_t address,T&out) noexcept {return copy(address,&out,sizeof(out));}
bool ownerQualified(const Owner&o) noexcept {
 std::uintptr_t vf=0,p=0,animated=0;std::uint32_t id=0;
 return detail::valid(o)&&base&&read(o.player,vf)&&vf==base+0x3AA4B98&&
  read(o.player+0x38,p)&&p==o.entity&&read(o.player+0x30,id)&&id==o.entityId&&
  read(o.entity,vf)&&vf==base+0x4072C78&&read(o.entity+0xC,id)&&id==o.entityId&&
  read(o.player+0x268,animated)&&animated&&read(animated,vf)&&vf==base+0x3A49468&&
  read(animated+0x38,p)&&p==o.entity&&read(o.character,vf)&&vf==base+0x40602F8;
}
bool currentPhysicsBody(std::uintptr_t entityAddress,std::uintptr_t body) noexcept {
 // Exact retail5D70C4 GetProxy(1): entity+A8 is the std::map sentinel;
 // node+20 key, +28 proxy. Bounded, read-only equivalent of that native lookup.
 std::uintptr_t head=0,node=0,candidate=0,proxy=0,back=0,physical=0;
 unsigned char nil=0;std::uint32_t key=0;
 if(!read(entityAddress+0xA8,head)||!head||!read(head+0x19,nil)||!nil||!read(head+8,node))return false;
 candidate=head;
 for(unsigned depth=0;depth<32;++depth){
  if(!node||!read(node+0x19,nil))return false;
  if(nil)break;
  if(!read(node+0x20,key))return false;
  if(key>=1){candidate=node;if(!read(node,node))return false;}
  else if(!read(node+0x10,node))return false;
  if(depth==31)return false;
 }
 if(candidate==head||!read(candidate+0x20,key)||key!=1||!read(candidate+0x28,proxy)||!proxy||
    !read(proxy+0x30,back)||back!=entityAddress||!read(proxy+0x38,physical)||physical!=body)return false;
 std::uintptr_t headAgain=0,physicalAgain=0;
 return read(entityAddress+0xA8,headAgain)&&headAgain==head&&read(proxy+0x38,physicalAgain)&&physicalAgain==body;
}
bool livingQualified(std::uintptr_t body,const Owner&o,std::uintptr_t&world) noexcept {
 std::uintptr_t vf=0,foreign=0,globalWorld=0;std::uint32_t flags=0;std::int16_t foreignType=0;std::int8_t sim=-1;int deleting=-1;
 return body&&ownerQualified(o)&&currentPhysicsBody(o.entity,body)&&read(body,vf)&&vf==base+0x3A82570&&
  read(body+0x20,foreign)&&foreign==o.entity&&read(body+0x28,foreignType)&&foreignType==2&&
  read(body+0x47,sim)&&sim==3&&read(body+0x50,deleting)&&deleting==0&&
  read(body+0x68,world)&&world&&read(base+0x492D830,globalWorld)&&world==globalWorld&&
  read(world,vf)&&vf==base+0x407FC80&&read(body+0x84,flags)&&(flags&0x8080000u)!=0;
}
bool eventKey(std::uintptr_t address,detail::EventKey&out) noexcept {
 std::uint32_t type=0,foreignType=0;
 detail::EventKey k{};k.address=address;
 if(!read(address+8,type)||type!=4||!read(address+0x10,k.body)||!read(address+0x18,k.entity)||
  !read(address+0x20,foreignType)||foreignType!=2||!read(address+0x28,k.payload)||!read(address+0x48,k.step))return false;
 out=k;return true;
}
std::uint64_t nowMs() noexcept {
#if defined(_WIN32)
 return GetTickCount64();
#else
 return 0;
#endif
}
void fault() noexcept {std::lock_guard<std::mutex> lock(mutex);state.fault();}
#if defined(_WIN32)
using namespace DetourModKit::hook;
std::uintptr_t reg(MidContext&c,Gpr r) noexcept{return gpr(c,r);}
// Every callback first compares a directly observed owner. Native pointers are
// read only inside this synchronous engine occurrence; none is retained as an
// independently owned engine object or used to call native methods.
void StepEntry(MidContext&c) noexcept {
 if(!enrolledEntity.load(std::memory_order_acquire))return;
 const auto body=reg(c,Gpr::Rcx);std::uintptr_t foreign=0;
 if(!read(body+0x20,foreign)||foreign!=enrolledEntity.load(std::memory_order_relaxed))return;
 std::lock_guard<std::mutex> lock(mutex);
 std::uintptr_t world=0;if(!livingQualified(body,state.owner,world))return;
 if(step.body){state.fault();return;}
 step={};step.body=body;step.world=world;step.number=++occurrence;step.origin=state.cumulative;step.origin.occurrence=step.number;
}
void StepIdle(MidContext&c) noexcept {
 if(!step.body||reg(c,Gpr::Rdi)!=step.body)return;
 std::lock_guard<std::mutex> lock(mutex);
 if(!detail::same(step.origin.owner,state.owner)||!slotOrigin.valid||!state.hasPending(nowMs()))return;
 // The native active bit test at50BD38 has already passed. Only bypass its
 // zero-velocity early-out; enter the unchanged native capsule collision path.
 instruction_pointer(c)=base+0x50BE56;
}
void StepDelta(MidContext&c) noexcept {
 if(!step.body||step.injected||reg(c,Gpr::Rdi)!=step.body)return;
 std::lock_guard<std::mutex> lock(mutex);std::uintptr_t world=0;
 if(!detail::same(step.origin.owner,state.owner)||!slotOrigin.valid||!state.hasPending(nowMs())||
  !livingQualified(step.body,state.owner,world)||world!=step.world)return;
 Vec3 native{};const auto address=reg(c,Gpr::Rbp);
 if(!read(address,native)||!detail::finite(native)){state.fault();return;}
 const Vec3 combined=detail::add(native,state.pending.world);
 if(!detail::finite(combined)||!write(address,&combined,sizeof(combined))){state.fault();return;}
 step.origin=state.applied(step.number);step.injected=true;
}
void StepEmit(MidContext&c) noexcept {
 if(!step.body||reg(c,Gpr::Rdi)!=step.body||reg(c,Gpr::Rcx)!=step.world)return;
 detail::EventKey key{};if(!eventKey(reg(c,Gpr::R8),key)||key.body!=step.body)return;
 std::lock_guard<std::mutex> lock(mutex);
 if(detail::same(step.origin.owner,state.owner)&&state.record(key,step.origin))step.emitted=true;
}
void StepExit(MidContext&c) noexcept {
 if(!step.body||reg(c,Gpr::Rdi)!=step.body)return;
 if(step.injected&&!step.emitted){std::lock_guard<std::mutex> lock(mutex);if(detail::same(step.origin.owner,state.owner)){++state.counters.lostReceipt;state.fault();}}
 step={};
}
void QueueCopy(MidContext&c) noexcept {
 if(!enrolledEntity.load(std::memory_order_acquire))return;
 detail::EventKey from{},to{};std::uintptr_t foreign=0;
 if(!read(reg(c,Gpr::Rdi)+0x18,foreign)||foreign!=enrolledEntity.load(std::memory_order_relaxed)||
  !eventKey(reg(c,Gpr::Rdi),from)||!eventKey(reg(c,Gpr::Rax),to))return;
 if(from.body!=to.body||from.entity!=to.entity||from.step!=to.step||from.payload!=to.payload)return;
 std::lock_guard<std::mutex> lock(mutex);Origin origin{};
 if(state.lookup(from,origin)&&state.record(to,origin))++state.counters.queued;
}
void EntityEntry(MidContext&c) noexcept {
 if(!enrolledEntity.load(std::memory_order_acquire))return;
 const auto proxy=reg(c,Gpr::Rcx),event=reg(c,Gpr::Rdx);std::uintptr_t owner=0,body=0;
 if(!read(proxy+0x30,owner)||owner!=enrolledEntity.load(std::memory_order_relaxed))return;
 detail::EventKey key{};if(!eventKey(event,key)||key.entity!=owner||!read(proxy+0x38,body)||body!=key.body)return;
 std::lock_guard<std::mutex> lock(mutex);Origin origin{};
 if(!state.lookup(key,origin)){++state.counters.lostReceipt;if(state.lastApplied)state.fault();return;}
 if(entity.matched){state.fault();return;}
 entity={proxy,event,owner,origin,true,false};
}
void EntityWriteBegin(MidContext&c) noexcept {
 if(!entity.matched||entity.writing||reg(c,Gpr::Rcx)!=entity.entity)return;
 std::uintptr_t caller=0;if(!read(stack_pointer(c)+0x28,caller)||caller!=base+0x509EC4)return;
 //509854 is reached only AFTER both native SetPosRotScale vetoes. The no-change
 // branch is still an adopted collision result (essential at a wall).
 std::lock_guard<std::mutex> lock(mutex);
 if(!detail::same(entity.origin.owner,state.owner))return;
 entity.previousOccurrence=state.entity.occurrence;
 entity.writing=state.beginEntity(entity.origin);
 if(!entity.writing)state.fault();
}
void EntityNoChange(MidContext&c) noexcept {
 if(!entity.writing||reg(c,Gpr::Rcx)!=entity.entity)return;
 std::uintptr_t caller=0;if(!read(stack_pointer(c)+0x28,caller)||caller!=base+0x509EC4)return;
 entity.unchanged=reg(c,Gpr::R10)==0; // Native PRS comparison result, not a float heuristic.
}
void EntityWriteEnd(MidContext&c) noexcept {
 if(!entity.writing)return;
 std::uintptr_t caller=0;if(!read(stack_pointer(c)+0x28,caller)||caller!=base+0x509EC4)return;
 Vec3 pos{};float matrix[12]{};
 if(!read(entity.entity+0x58,matrix)){fault();return;}
 pos={matrix[3],matrix[7],matrix[11]};std::lock_guard<std::mutex> lock(mutex);
 if(!detail::finite(pos)){state.fault();return;}
 state.endEntity(pos);entity.writing=false;
 // A collision-shortened step can be a legitimately unchanged native PRS.
 // Native skips XFORM/slot refresh in this exact branch. Carry the new receipt
 // only across caches proven to represent its immediately previous occurrence.
 if(entity.unchanged&&slotOrigin.valid&&detail::same(slotOrigin.origin.owner,state.owner)&&
    slotOrigin.origin.occurrence==entity.previousOccurrence){
  slotOrigin.origin=state.entity;
  if(!state.characterBusy&&state.character.occurrence==entity.previousOccurrence){
   const auto characterPosition=state.character.adoptedPosition;
   if(state.beginCharacter(state.entity))state.endCharacter(state.entity,characterPosition);
  }
 }
}
void EntityExit(MidContext&c) noexcept {
 if(!entity.matched||reg(c,Gpr::Rsi)!=entity.proxy)return;
 if(entity.writing)fault();entity={};
}
void SlotRefreshEntry(MidContext&c) noexcept {
 const auto owner=reg(c,Gpr::Rdx),slot=reg(c,Gpr::Rcx);
 if(!owner||owner!=enrolledEntity.load(std::memory_order_acquire))return;
 std::uintptr_t ch=0,parent=0;
 if(!read(slot+0x80,ch)||ch!=enrolledCharacter.load(std::memory_order_acquire)||!read(slot+0x70,parent)||parent)return;
 std::lock_guard<std::mutex> lock(mutex);Origin origin{};
 if(refresh.valid){state.fault();return;}
 const bool inside=entity.writing&&entity.entity==owner;
 if(state.entityForSlot(origin,inside)){refresh={slot,owner,origin,true,inside,state.entity.serial};slotOrigin.valid=false;}
}
void SlotRefreshEnd(MidContext&c) noexcept {
 if(!refresh.valid||reg(c,Gpr::Rbx)!=refresh.slot)return;
 std::lock_guard<std::mutex> lock(mutex);
 const bool sameGeneration=refresh.insideWriter?
  (entity.writing&&entity.entity==refresh.entity&&detail::same(entity.origin.owner,refresh.origin.owner)&&entity.origin.occurrence==refresh.origin.occurrence):
  (!state.entityBusy&&state.entity.serial==refresh.entitySerial);
 if(detail::same(refresh.origin.owner,state.owner)){
  if(sameGeneration)slotOrigin={refresh.slot,refresh.origin,true};
  else {++state.counters.lostReceipt;state.fault();}
 }
 refresh={};
}
void CharacterEntry(MidContext&c) noexcept {
 const auto owner=reg(c,Gpr::Rdx),slot=reg(c,Gpr::Rcx);
 if(!owner||owner!=enrolledEntity.load(std::memory_order_acquire))return;
 std::uintptr_t ch=0,parent=0;
 if(!read(slot+0x80,ch)||ch!=enrolledCharacter.load(std::memory_order_acquire)||!read(slot+0x70,parent)||parent)return;
 if(character.slot){fault();return;}
 character={};character.slot=slot;character.entity=owner;character.character=ch;
}
void CharacterSlotReady(MidContext&c) noexcept {
 if(!character.slot||reg(c,Gpr::Rbx)!=character.slot)return;
 std::lock_guard<std::mutex> lock(mutex);
 if(slotOrigin.valid&&slotOrigin.slot==character.slot&&detail::same(slotOrigin.origin.owner,state.owner)){
  character.origin=slotOrigin.origin;character.valid=true;return;
 }
 // An already valid cached slot may be observed before its first refresh after
 // enrollment. Only the zero-debt generation is safe to seed without a refresh.
 if(!state.lastApplied&&!state.entityBusy){slotOrigin={character.slot,state.entity,true};character.origin=state.entity;character.valid=true;}
}
void CharacterCall(MidContext&c) noexcept {
 if(!character.valid||reg(c,Gpr::Rcx)!=character.character)return;
 character.params=reg(c,Gpr::Rdx);
}
void CharacterWriteBegin(MidContext&c) noexcept {
 if(!character.valid||character.writing||reg(c,Gpr::Rbx)!=character.character||reg(c,Gpr::Rdi)!=character.params)return;
 std::lock_guard<std::mutex> lock(mutex);
 if(!detail::same(character.origin.owner,state.owner))return;
 character.writing=state.beginCharacter(character.origin);
 if(!character.writing)state.fault();
}
void CharacterWriteEnd(MidContext&c) noexcept {
 if(!character.writing||reg(c,Gpr::Rbx)!=character.character)return;
 Vec3 pos{};if(!read(character.character+0xA18,pos)||!detail::finite(pos)){fault();return;}
 std::lock_guard<std::mutex> lock(mutex);state.endCharacter(character.origin,pos);character.writing=false;
}
void CharacterExit(MidContext&c) noexcept {
 if(!character.slot||reg(c,Gpr::Rbx)!=character.slot)return;
 if(character.writing)fault();character={};
}
#if !defined(KCD2VR_NATIVE_ROOMSCALE_D76_TEST)
struct Pin {std::uintptr_t rva;const char*bytes;std::size_t size;MidHookFn callback;const char*name;};
const Pin pins[]{
 {0x50B870,"\x48\x89\x5c\x24\x18\x55",6,StepEntry,"step_entry"},
 {0x50BD41,"\xf3\x0f\x10\x54\x24\x64",6,StepIdle,"step_idle"},
 {0x50C3C6,"\xf3\x0f\x10\xbf\xb0\x00\x00\x00",8,StepDelta,"step_delta"},
 {0x5141B2,"\xe8\xf5\x5d\x33\x00\x8b\x95\xd0\x07\x00\x00",11,StepEmit,"step_emit"},
 {0x5141DF,"\x48\x8b\x8d\x50\x10\x00\x00",7,StepExit,"step_exit"},
 {0x84A03B,"\x48\xc7\x00\x00\x00\x00\x00",7,QueueCopy,"poststep_copy"},
 {0x509D74,"\x48\x8b\xc4\x48\x89\x58\x18",7,EntityEntry,"entity_poststep"},
 {0x509854,"\x45\x33\xc0\x4c\x8d\x49\x30",7,EntityWriteBegin,"entity_begin"},
 {0x509917,"\x45\x85\xd2\x74\x08\x45\x33\xc0",8,EntityNoChange,"entity_unchanged"},
 {0x509924,"\x48\x8b\x5c\x24\x30\x48\x8b\x74\x24\x38",10,EntityWriteEnd,"entity_commit"},
 {0x509F04,"\x48\x8b\x8d\xd0\x00\x00\x00",7,EntityExit,"entity_poststep_end"},
 {0x5D77F8,"\x48\x8b\xc4\x48\x89\x58\x10",7,SlotRefreshEntry,"slot_refresh"},
 {0x5D78C4,"\x49\x8b\x5b\x18\x49\x8b\xe3",7,SlotRefreshEnd,"slot_commit"},
 {0xD85224,"\x48\x8b\xc4\x48\x89\x58\x08",7,CharacterEntry,"character_scope"},
 {0xD8525F,"\xf3\x0f\x10\x73\x08\x48\x8b\xc7",8,CharacterSlotReady,"character_slot"},
 {0xD85593,"\xff\x90\x98\x00\x00\x00",6,CharacterCall,"character_call"},
 {0x83DF76,"\x0f\x11\x83\x08\x0a\x00\x00",7,CharacterWriteBegin,"character_begin"},
 {0x83DF88,"\x8b\x47\x24\x89\x83\x90\x0a\x00\x00",9,CharacterWriteEnd,"character_commit"},
 {0xD85599,"\x4c\x8d\x9c\x24\xf0\x00\x00\x00",8,CharacterExit,"character_end"}
};
std::array<std::optional<Hook>,std::size(pins)> hooks;
std::mutex installMutex;
void installLog(const char*result,std::uintptr_t rva=0) noexcept {
 char line[160]{};std::snprintf(line,sizeof(line),"D76 ROOMSCALE_NATIVE_INSTALL result=%s rva=%llX",result,static_cast<unsigned long long>(rva));diagnostic_trace::Write(line);
}
bool exactSlot(std::uintptr_t vft,std::uintptr_t offset,std::uintptr_t target) noexcept {std::uintptr_t p=0;return read(base+vft+offset,p)&&p==base+target;}
#endif
#endif
}

bool Ready() noexcept {return installed.load(std::memory_order_acquire);}
bool Enrolled() noexcept {return enrolledEntity.load(std::memory_order_acquire)!=0;}
bool OriginPristine() noexcept {
 if(!enrolledEntity.load(std::memory_order_acquire))return true;
 std::lock_guard<std::mutex> lock(mutex);const auto c=state.entityCompleted.consumedTracking;return state.lastApplied==0&&c.x==0&&c.y==0&&c.z==0;
}
bool Install() noexcept {
#if defined(_WIN32) && !defined(KCD2VR_NATIVE_ROOMSCALE_D76_TEST)
 std::lock_guard<std::mutex> lock(installMutex);if(Ready())return true;
 base=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll"));
 if(!base||!retail_build112::Ready(base)){installLog("RETAIL_REFUSED");return false;}
 std::array<unsigned char,7> proxyPin{};const unsigned char proxyBytes[]{0x4C,0x8B,0x81,0xA8,0,0,0};
 if(!copy(base+0x5D70C4,proxyPin.data(),proxyPin.size())||std::memcmp(proxyPin.data(),proxyBytes,proxyPin.size())){installLog("PROXY_LOOKUP_REFUSED",0x5D70C4);return false;}
 for(const auto&p:pins){std::array<char,16> bytes{};if(p.size>bytes.size()||!copy(base+p.rva,bytes.data(),p.size)||std::memcmp(bytes.data(),p.bytes,p.size)){installLog("PIN_REFUSED",p.rva);return false;}}
 if(!exactSlot(0x3A82570,0xF0,0x50B870)||!exactSlot(0x40602F8,0x98,0x83E2F8)||!exactSlot(0x4072C78,0x328,0x6411AC)||!exactSlot(0x3AA4B98,0x6E8,0x1A81D90)){
  installLog("IDENTITY_SLOT_REFUSED");return false;
 }
 HMODULE self=nullptr;if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_PIN,reinterpret_cast<LPCWSTR>(&Install),&self)){installLog("MODULE_PIN_REFUSED");return false;}
 // Construct the complete inert set before enabling any callback. Callbacks
 // cannot write before explicit Rebase+admission+fresh Submit from the host.
 for(std::size_t i=0;i<std::size(pins);++i){const auto&p=pins[i];Options options{};options.fail_if_already_hooked=true;
  auto h=mid_at(MidRequest{std::string("kcd2vr_d76_roomscale_")+p.name,Target{DetourModKit::Address{base+p.rva}},options},p.callback);
  if(!h){installLog("HOOK_REFUSED",p.rva);for(auto&x:hooks)x.reset();return false;}hooks[i]=std::move(*h);
 }
 for(std::size_t i=0;i<hooks.size();++i)if(!hooks[i]->enable()){
  installLog("ENABLE_REFUSED",pins[i].rva);
  for(auto&h:hooks)if(h){const auto disabled=h->disable();if(disabled)h.reset();else{h->release();h.reset();}}
  return false;
 }
 for(auto&h:hooks){h->release();h.reset();}installed.store(true,std::memory_order_release);installLog("READY");return true;
#else
 return false;
#endif
}
bool Rebase(const Owner&o) noexcept {
 if(!Ready()||!ownerQualified(o))return false;
 std::lock_guard<std::mutex> lock(mutex);if(!state.rebase(o))return false;
 slotOrigin={};enrolledCharacter.store(o.character,std::memory_order_release);enrolledEntity.store(o.entity,std::memory_order_release);return true;
}
bool TryRecenterOrigin(std::uint64_t epoch) noexcept {
 if(!enrolledEntity.load(std::memory_order_acquire))return true;
 std::lock_guard<std::mutex> lock(mutex);state.admission(false);
 if(!epoch)return false;auto next=state.owner;next.epoch=epoch;
 if(!state.rebase(next))return false;
 slotOrigin={};return true;
}
void SetAdmission(bool enabled) noexcept {if(!enrolledEntity.load(std::memory_order_acquire))return;std::lock_guard<std::mutex> lock(mutex);state.admission(enabled&&Ready());}
void CancelPending() noexcept {if(!enrolledEntity.load(std::memory_order_acquire))return;std::lock_guard<std::mutex> lock(mutex);state.cancel();}
bool Submit(const StepRequest&r) noexcept {std::lock_guard<std::mutex> lock(mutex);return Ready()&&state.submit(r,nowMs());}
bool ReadEntityOrigin(std::uintptr_t id,Origin&out) noexcept {
 if(!id||id!=enrolledEntity.load(std::memory_order_acquire)){out={};return true;}
 std::lock_guard<std::mutex> lock(mutex);return state.read(Surface::Entity,id,out);
}
bool ReadCharacterOrigin(std::uintptr_t id,Origin&out) noexcept {
 if(!id||id!=enrolledCharacter.load(std::memory_order_acquire)){out={};return true;}
 std::lock_guard<std::mutex> lock(mutex);return state.read(Surface::Character,id,out);
}
bool ReadAdoptedEntityOrigin(const Owner&o,Origin&out) noexcept {
 if(!enrolledEntity.load(std::memory_order_acquire)){out={};return true;}
 std::lock_guard<std::mutex> lock(mutex);
 if(state.faulted||!detail::same(o,state.owner))return false;
 out=state.entityCompleted;return true;
}
bool ReadAdoptedOrigin(Origin&out) noexcept {
 if(!enrolledEntity.load(std::memory_order_acquire)){out={};return true;}
 std::lock_guard<std::mutex> lock(mutex);if(state.faulted)return false;
 out=state.entityCompleted;return true;
}
bool ReadTrackingOriginD76(Origin&out) noexcept {
 if(!enrolledEntity.load(std::memory_order_acquire)){out={};return true;}
 std::lock_guard<std::mutex> lock(mutex);out=state.entityCompleted;return true;
}
bool SameSerial(Surface s,std::uintptr_t id,std::uint64_t serial) noexcept {
 const auto enrolled=s==Surface::Entity?enrolledEntity.load(std::memory_order_acquire):enrolledCharacter.load(std::memory_order_acquire);
 if(!id||id!=enrolled)return serial==0;
 std::lock_guard<std::mutex> lock(mutex);return state.serial(s,id,serial);
}
bool CurrentOwner(const Owner&o) noexcept {
 if(!o.entity||o.entity!=enrolledEntity.load(std::memory_order_acquire))return false;
 std::lock_guard<std::mutex> lock(mutex);return detail::same(o,state.owner);
}
Counters ReadCounters() noexcept {std::lock_guard<std::mutex> lock(mutex);return state.counters;}
}
