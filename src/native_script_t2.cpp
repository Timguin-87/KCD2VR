#include "native_script_t2.h"
#include "retail_build112.h"
#include "diagnostic_trace.h"
#include "core/observed_load_recovery_r96.h"
#include <windows.h>
#include <DetourModKit/hook.hpp>
#include <array>
#include <atomic>
#include <cstring>
#include <optional>
#include <utility>

namespace kcd2vr::native_script_t2 {namespace {
std::uintptr_t base=0;
std::atomic<bool> enabled{false};
std::atomic_flag installing=ATOMIC_FLAG_INIT;
std::atomic_flag servicing=ATOMIC_FLAG_INIT;
std::array<std::atomic<Client>,12> clients{}; // D73: eight retained clients + saved-corpse dragging + Lively NPC; bounded, wanted-only.
// R158: 10 slots were all taken (10 Register sites); +1 Shift+F5 quest recorder, +1 spare so none is ever refused.
std::array<std::atomic<Wanted>,12> wanted{}; // Identical bounded capacity; idle clients do no VM work.
std::optional<DetourModKit::hook::Hook> hook;
using Update=std::uintptr_t(__fastcall*)(void*);
Update original=nullptr;
thread_local const Context* current=nullptr;
thread_local bool inside=false;
thread_local bool callbackRevokedR96=false;
std::uintptr_t lastSystem=0,lastState=0;
std::uint64_t generation=0;
DWORD ownerThread=0;
#if defined(KCD2VR_SCRIPT_T2_HOST_TEST)
std::uintptr_t HostNative(std::uintptr_t) noexcept;
template<class T>T Native(std::uintptr_t rva) noexcept {return reinterpret_cast<T>(HostNative(rva));}
#else
template<class T>T Native(std::uintptr_t rva) noexcept {return reinterpret_cast<T>(base+rva);}
#endif
bool Copy(std::uintptr_t from,void* to,std::size_t n) noexcept {
    if(from<0x10000||!n||from+n<from)return false;
    MEMORY_BASIC_INFORMATION m{};
    if(!VirtualQuery(reinterpret_cast<void*>(from),&m,sizeof(m))||m.State!=MEM_COMMIT||
       (m.Protect&(PAGE_NOACCESS|PAGE_GUARD))||from+n>std::uintptr_t(m.BaseAddress)+m.RegionSize)return false;
    __try{std::memcpy(to,reinterpret_cast<void*>(from),n);return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
bool Pin(std::uintptr_t rva,const unsigned char* expected,std::size_t n) noexcept {
    unsigned char actual[32]{};
    return n<=sizeof(actual)&&Copy(base+rva,actual,n)&&!std::memcmp(actual,expected,n);
}
bool Vm(std::uintptr_t system,std::uintptr_t& state,std::uintptr_t& top,std::uintptr_t& frame) noexcept {
    std::uintptr_t vt=0;
    return Copy(system,&vt,8)&&vt==base+0x3B8C610&&Copy(system+0x10,&state,8)&&state&&
        Copy(state+0x10,&top,8)&&Copy(state+0x18,&frame,8)&&top>=frame&&
        (top-frame)%16==0&&(top-frame)/16<=4096;
}
// Exact retail Lua VM: native interpreter7156AE subtracts current top+10
// from stack-last+38 and compares against required TValue16 bytes, growing on
// <=. Do not call a guessed reserve function or let C API pushes outrun it.
// This read-only guard leaves the native stack unchanged when room is absent.
bool Room(std::uintptr_t state,std::uintptr_t top,unsigned values) noexcept {
    std::uintptr_t last=0;
    return values<=2&&Copy(state+0x38,&last,8)&&last>=top&&
        (last-top)%16==0&&(last-top)/16>values;
}
// Lua's own stack growth relocates top/base while preserving their TValue
// indices (3997318). A moved stack is not a replacement VM or call frame.
struct StackCursor {
    std::uintptr_t first=0,top=0,frame=0,last=0;
};
bool Cursor(std::uintptr_t system,std::uintptr_t expected,StackCursor& c) noexcept {
    std::uintptr_t state=0;
    return Vm(system,state,c.top,c.frame)&&state==expected&&
        Copy(state+0x40,&c.first,8)&&Copy(state+0x38,&c.last,8)&&
        c.first&&c.first<=c.frame&&c.frame<=c.top&&c.top<=c.last&&
        (c.frame-c.first)%16==0&&(c.last-c.first)%16==0;
}
bool SameFrame(const StackCursor& a,const StackCursor& b) noexcept {
    return a.frame-a.first==b.frame-b.first;
}
// Administrative shutdown is distinct from uncertainty in one original-first
// callback. A temporary read or replaced VM cannot disable every script client
// for the rest of the process. True native faults remain quarantined until a
// positively qualified new VM or a completed observed load admits revalidation.
enum class FaultKindR96 : unsigned { None, Observation, Native };
struct TransportFaultR96 {
    FaultKindR96 kind=FaultKindR96::None;
    std::uintptr_t system=0,state=0;
    StackCursor entry{};
    observed_load_recovery_r96::Witness load{};
};
TransportFaultR96 transportFaultR96{};
void QuarantineR96(const Context& c,const StackCursor& entry,
                   observed_load_recovery_r96::Witness load,FaultKindR96 kind,const char* reason) noexcept {
    callbackRevokedR96=true;transportFaultR96={kind,c.system,c.state,entry,load};
    diagnostic_trace::Write(reason);
}
bool QualifiedRecoveryR96(std::uintptr_t system,std::uintptr_t state,const StackCursor& cursor) noexcept {
    auto& fault=transportFaultR96;if(fault.kind==FaultKindR96::None)return true;
    const bool replaced=system!=fault.system||state!=fault.state;
    const bool loaded=observed_load_recovery_r96::completedSince(fault.load);
    // An uncertain read did not justify writing an old top pointer. The native
    // original must itself return this same frame to its original stack height
    // before that VM can resume. No old packet or SetTop is retried here.
    const bool restored=fault.kind==FaultKindR96::Observation&&SameFrame(fault.entry,cursor)&&
        cursor.top-cursor.frame==fault.entry.top-fault.entry.frame;
    if(!replaced&&!loaded&&!restored)return false;
    fault={};
    diagnostic_trace::Write(replaced?"R96 SCRIPT_VM F5_TRANSPORT_RECOVERED proof=QUALIFIED_NEW_VM oldPacket=NOT_REPLAYED":
        loaded?"R96 SCRIPT_VM F5_TRANSPORT_RECOVERED proof=COMPLETED_OBSERVED_LOAD oldPacket=NOT_REPLAYED":
               "R96 SCRIPT_VM F5_TRANSPORT_RECOVERED proof=NATIVE_STACK_RESTORED oldPacket=NOT_REPLAYED");
    return true;
}

bool OwnFunction(const char* name) noexcept {
    if(!name||std::strncmp(name,"kcd2vr_",7))return false;
    for(unsigned i=0;i<96;++i){const unsigned char c=name[i];if(!c)return i>7;
        if(!((c>='a'&&c<='z')||(c>='A'&&c<='Z')||(c>='0'&&c<='9')||c=='_'))return false;}
    return false;
}
bool Invoke(const Context& c,const char* name,const char* input,char* output,std::size_t cap) noexcept {
    if(output&&cap)output[0]=0;
    if(current!=&c||callbackRevokedR96||!enabled.load(std::memory_order_acquire)||GetCurrentThreadId()!=ownerThread||
       !OwnFunction(name)||!input||!output||cap<2||cap>16384||strnlen(input,16385)>16384)return false;
    StackCursor entry{};
    if(!Cursor(c.system,c.state,entry)||!Room(c.state,entry.top,2))return false;
    const auto loadAtEntryR96=observed_load_recovery_r96::observe();
    const auto state=c.state;const int saved=int((entry.top-entry.frame)/16);
    using GetField=void(__fastcall*)(void*,int,const char*);
    using Type=int(__fastcall*)(void*,int);
    using Push=void(__fastcall*)(void*,const char*);
    using PCall=int(__fastcall*)(void*,int,int,int);
    using SetTop=void(__fastcall*)(void*,int);
    bool ok=false,fault=false;void* L=reinterpret_cast<void*>(state);
    __try {
        Native<GetField>(0xAD496C)(L,-10002,name);
        StackCursor field{};
        if(Native<Type>(0x715E00)(L,-1)==6&&
           Cursor(c.system,state,field)&&SameFrame(entry,field)&&
           field.top-field.frame==std::uintptr_t(saved+1)*16&&Room(state,field.top,1)){
            Native<Push>(0x7182C4)(L,input);
            if(Native<PCall>(0x713914)(L,1,1,0)==0&&
               Native<Type>(0x715E00)(L,-1)==4){
                StackCursor after{};std::uintptr_t str=0;std::uint64_t length=0;
                // Own callback must return an actual Lua STRING. The native
                // push-string/type and TString constructor prove TValue16,
                // string length+10/data+18; no guessed lua_tolstring address.
                if(Cursor(c.system,state,after)&&SameFrame(entry,after)&&
                   after.top-after.frame==std::uintptr_t(saved+1)*16&&Copy(after.top-16,&str,8)&&Copy(str+0x10,&length,8)&&
                   length<cap&&(!length||Copy(str+0x18,output,std::size_t(length)))){
                    output[length]=0;ok=std::memchr(output,0,std::size_t(length))==nullptr;
                }
            }
        }
    }__except(EXCEPTION_EXECUTE_HANDLER){fault=true;}
    // Restore only the exact still-current VM/frame. Never apply a stack pointer
    // to a replacement VM or continue native clients after an uncertain fault.
    StackCursor restore{},restored{};bool stackRestored=false;
    if(!fault&&Cursor(c.system,state,restore)&&SameFrame(entry,restore)){
        __try{Native<SetTop>(0x717AEC)(L,saved);}
        __except(EXCEPTION_EXECUTE_HANDLER){fault=true;}
        stackRestored=!fault&&Cursor(c.system,state,restored)&&SameFrame(entry,restored)&&
            restored.top-restored.frame==std::uintptr_t(saved)*16;
    }
    if(fault||!stackRestored)QuarantineR96(c,entry,loadAtEntryR96,fault?FaultKindR96::Native:FaultKindR96::Observation,
        fault?"R96 SCRIPT_VM F5_TRANSPORT_ERROR scope=CURRENT_VM reason=NATIVE_FAULT clients=SUSPENDED":
              "R96 SCRIPT_VM F5_TRANSPORT_ERROR scope=CURRENT_CALLBACK reason=FRAME_UNAVAILABLE_OR_REPLACED clients=SUSPENDED");
    if(!ok||callbackRevokedR96)output[0]=0;return ok&&!callbackRevokedR96;
}
bool CallClient(Client client,const Context& c) noexcept {
    __try{client(c);return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
void ServiceImpl(void* self) noexcept {
    std::uintptr_t state=0,top=0,frame=0;
    if(!enabled.load(std::memory_order_acquire)||!Vm(reinterpret_cast<std::uintptr_t>(self),state,top,frame))return;
    const auto system=reinterpret_cast<std::uintptr_t>(self);const auto thread=GetCurrentThreadId();
    StackCursor cursor{};
    if(!Cursor(system,state,cursor))return;
    const bool changed=system!=lastSystem||state!=lastState;
    if(!changed&&thread!=ownerThread)return;
    if(!QualifiedRecoveryR96(system,state,cursor))return;
    if(changed){lastSystem=system;lastState=state;ownerThread=thread;++generation;}
    const Context c{system,state,std::uint64_t(GetTickCount64()),generation};current=&c;callbackRevokedR96=false;
    for(unsigned i=0;i<clients.size()&&!callbackRevokedR96;++i){const auto client=clients[i].load(std::memory_order_acquire);
        const auto want=wanted[i].load(std::memory_order_acquire);
        if(client&&(!want||want())&&enabled.load(std::memory_order_acquire)){
            StackCursor clientEntry{};if(!Cursor(system,state,clientEntry)){callbackRevokedR96=true;break;}
            const auto loadAtEntry=observed_load_recovery_r96::observe();
            if(!CallClient(client,c))QuarantineR96(c,clientEntry,loadAtEntry,FaultKindR96::Native,
                "R96 SCRIPT_VM F5_TRANSPORT_ERROR scope=CURRENT_VM reason=CLIENT_NATIVE_FAULT clients=SUSPENDED");
        }}
    current=nullptr;
}
void Service(void* self) noexcept {
    if(!enabled.load(std::memory_order_acquire))return;
    bool need=false;
    for(unsigned i=0;i<clients.size();++i)if(clients[i].load(std::memory_order_acquire)){
        const auto want=wanted[i].load(std::memory_order_acquire);if(!want||want()){need=true;break;}}
    if(!need)return; // no VM/VirtualQuery/native reads when all domains are idle.
    if(servicing.test_and_set(std::memory_order_acquire))return;
    ServiceImpl(self);servicing.clear(std::memory_order_release);
}
std::uintptr_t __fastcall Detour(void* self) {
    const auto result=original(self);
    if(!inside){inside=true;Service(self);inside=false;}
    return result;
}
}
bool Context::Current() const noexcept {
    if(current!=this||callbackRevokedR96||!enabled.load(std::memory_order_acquire)||GetCurrentThreadId()!=ownerThread||
       system!=lastSystem||state!=lastState||generation!=kcd2vr::native_script_t2::generation)return false;
    std::uintptr_t nowState=0,top=0,frame=0;
    return Vm(system,nowState,top,frame)&&nowState==state;
}
bool Context::CallPacket(const char* name,const char* input,char* output,std::size_t cap) const noexcept {
    return Invoke(*this,name,input,output,cap);
}
bool Register(Client client,Wanted want) noexcept {
    if(!client)return false;
    for(unsigned i=0;i<clients.size();++i)if(clients[i].load(std::memory_order_acquire)==client){wanted[i].store(want,std::memory_order_release);return true;}
    for(unsigned i=0;i<clients.size();++i){Client empty=nullptr;
        if(clients[i].compare_exchange_strong(empty,client,std::memory_order_acq_rel)){wanted[i].store(want,std::memory_order_release);return true;}}
    return false;
}
bool Install() noexcept {
    if(enabled.load(std::memory_order_acquire))return true;
    if(installing.test_and_set(std::memory_order_acquire))return false;
    bool ok=false;
    if(!hook){
        base=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll"));
        const unsigned char update[]{0x48,0x89,0x5c,0x24,0x18,0x48,0x89,0x74,0x24,0x20,0x57,0x48,0x83,0xec,0x30};
        const unsigned char get[]{0x48,0x89,0x5c,0x24,0x08,0x57,0x48,0x83,0xec,0x30,0x4d,0x8b,0xd0};
        const unsigned char type[]{0x48,0x83,0xec,0x28,0xe8,0xe3,0x12,0,0};
        const unsigned char push[]{0x48,0x89,0x5c,0x24,0x08,0x48,0x89,0x6c,0x24,0x10,0x48,0x89,0x74,0x24,0x18};
        const unsigned char call[]{0x48,0x89,0x5c,0x24,0x08,0x57,0x48,0x83,0xec,0x40,0x33,0xc0};
        const unsigned char set[]{0x85,0xd2,0x78,0x20,0x4c,0x63,0xc2,0x4d,0x03,0xc0,0x48,0x8b,0x51,0x18};
        const unsigned char vm[]{0x48,0x8b,0x4e,0x10};
        const unsigned char value[]{0x48,0x89,0x03,0xc7,0x43,0x08,0x04,0,0,0,0x48,0x83,0x47,0x10,0x10};
        const unsigned char length[]{0x48,0x39,0x7b,0x10};
        const unsigned char chars[]{0x48,0x8d,0x53,0x18};
        const unsigned char vmArg[]{0x48,0x8b,0xf1};
        const unsigned char room[]{0x48,0x8b,0x4e,0x38,0x8b,0xc3,0x48,0x2b,0x4e,0x10,0xc1,0xe0,0x04,0x48,0x98,0x48,0x89,0x6e,0x30,0x48,0x3b,0xc8,0x0f,0x8e,0x24,0x48,0xb1,0x01};
        const unsigned char stackBase[]{0x48,0x89,0x43,0x40};
        const unsigned char rebase[]{0x48,0x8b,0x41,0x18,0x49,0x2b,0xc0,0x48,0x83,0xe0,0xf0,0x48,0x03,0x41,0x40,0x48,0x89,0x41,0x18};
        if(retail_build112::Verify(base)&&Pin(0xA26B94,update,sizeof(update))&&Pin(0xAD496C,get,sizeof(get))&&
           Pin(0x715E00,type,sizeof(type))&&Pin(0x7182C4,push,sizeof(push))&&Pin(0x713914,call,sizeof(call))&&Pin(0x717AEC,set,sizeof(set))&&
           Pin(0xA26C75,vm,sizeof(vm))&&Pin(0x718314,value,sizeof(value))&&Pin(0x717214,length,sizeof(length))&&Pin(0x71721A,chars,sizeof(chars))&&
           Pin(0x714CD5,vmArg,sizeof(vmArg))&&Pin(0x7156AE,room,sizeof(room))&&
           Pin(0x3997523,stackBase,sizeof(stackBase))&&Pin(0x3997399,rebase,sizeof(rebase))){
            DetourModKit::hook::InlineRequest request{"kcd2vr_native_script_t2",DetourModKit::hook::Target{DetourModKit::Address{base+0xA26B94}},DetourModKit::hook::Options{}};
            auto h=DetourModKit::hook::inline_at(std::move(request),&Detour);
            if(h){hook=std::move(*h);original=hook->original<Update>();
                HMODULE own=nullptr;
                const bool retained=GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_PIN,
                    reinterpret_cast<LPCWSTR>(&Detour),&own)!=0;
                ok=retained&&original&&hook->enable();}
        }
    }
    enabled.store(ok,std::memory_order_release);installing.clear(std::memory_order_release);
    diagnostic_trace::Write(ok?"T2 SCRIPT_VM ready exact native Update original-first; bounded own clients only":"T2 SCRIPT_VM unavailable exact retail/native API/hook; no foreign-thread fallback");return ok;
}
void Shutdown() noexcept {enabled.store(false,std::memory_order_release);}
}
