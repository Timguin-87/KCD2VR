#include "native_weapon_clearance_t4.h"
#include "retail_build112.h"
#include <windows.h>
#include <atomic>
#include <cstring>

namespace kcd2vr::native_weapon_clearance_t4 {namespace {
std::atomic<bool> ready{false},fault{false};
std::uintptr_t module=0;
#if defined(KCD2VR_WEAPON_BODY_T4_HOST_TEST)
float HostClearance(native_physics_phase_t4::Context&,Capsule,void*) noexcept;
bool HostClearanceInstall() noexcept;
// Explicit substitute for the exact AF5EF4 TLS getter, not a gameplay gate.
int hostCallerIndexH5=0;
int CurrentCallerH5() noexcept {return hostCallerIndexH5;}
#else
bool Copy(std::uintptr_t p,void* out,std::size_t n) noexcept {
    __try {std::memcpy(out,reinterpret_cast<void*>(p),n);return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
bool Pin(std::uintptr_t rva,const unsigned char* bytes,std::size_t n) noexcept {
    unsigned char actual[32]{};return n<=sizeof actual&&Copy(module+rva,actual,n)&&!std::memcmp(actual,bytes,n);
}
int CurrentCallerH5() noexcept {
    // Exact retail read-only TLS getter: no actor/body lookup or TLS write.
    using Fn=int(__fastcall*)();
    __try {return reinterpret_cast<Fn>(module+0xAF5EF4)();}
    __except(EXCEPTION_EXECUTE_HANDLER){fault.store(true,std::memory_order_release);return -1;}
}
float Call(void* world,Capsule* shape,void** skip) noexcept {
    // Existing retail adapter AD2450 constructs its own0xE8 native request and
    // releases its query lock. Args7,10,11 are ignored by this exact adapter.
    using Fn=float(__fastcall*)(void*,int,void*,const native_physics_phase_t4::V3*,int,void**,
        int,unsigned,void*,void*,int,void**,int,void*,const char*);
    const native_physics_phase_t4::V3 zero{};
    __try {return reinterpret_cast<Fn>(module+0xAD2450)(world,6,shape,&zero,
        0x107,nullptr,0,0x2000Fu,nullptr,nullptr,0,skip,1,nullptr,"KCD2VR_T4_ACQUIRE");}
    __except(EXCEPTION_EXECUTE_HANDLER){fault.store(true,std::memory_order_release);return -1;}
}
#endif
}
bool Install() noexcept {
#if defined(KCD2VR_WEAPON_BODY_T4_HOST_TEST)
    ready.store(HostClearanceInstall());return ready.load();
#else
    if(fault.load())return false;
    module=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll"));
    if(!module||!retail_build112::Ready(module))return false;
    static constexpr unsigned char zero[]{0x41,0xB8,0xE8,0,0,0};
    static constexpr unsigned char dispatch[]{0xFF,0x90,0xC8,1,0,0};
    static constexpr unsigned char release[]{0x8B,0x4D,0xF8,0x48,0x8B,0x45,0,0xF7,0xD9,0xF0,0x01,0x08};
    static constexpr unsigned char overlap[]{0x66,0xC7,0x85,0x42,4,0,0,1,1};
    static constexpr unsigned char capsule[]{0xE8,0x31,0xCA,0xFF,0xFF};
    static constexpr unsigned char callerH5[]{0x48,0x83,0xEC,0x28,0x8B,0x0D,0x92,0x6D,0x6E,0x04};
    static constexpr unsigned char defaultCallerH5[]{0xC7,0x44,0x24,0x30,0x04,0,0,0};
    std::uintptr_t slot=0;
    const bool ok=Copy(module+0x407FC80+0x1C8,&slot,8)&&slot==module+0x8115BC&&
        Pin(0xAD2479,zero,sizeof zero)&&Pin(0xAD2500,dispatch,sizeof dispatch)&&
        Pin(0xAD2506,release,sizeof release)&&Pin(0x812944,overlap,sizeof overlap)&&
        Pin(0x812AD2,capsule,sizeof capsule)&&Pin(0xAF5EF4,callerH5,sizeof callerH5)&&
        Pin(0xAF5EFE,defaultCallerH5,sizeof defaultCallerH5);
    ready.store(ok,std::memory_order_release);return ok;
#endif
}
Result Query(native_physics_phase_t4::Context& context,Capsule shape,void* ownBody) noexcept {
    if(!ready.load(std::memory_order_acquire)||fault.load(std::memory_order_acquire)||
       !context.Current()||context.Kind()!=native_physics_phase_t4::Phase::Step||!Valid(shape)||!ownBody)return Result::Unknown;
#if !defined(KCD2VR_WEAPON_BODY_T4_HOST_TEST)
    if(context.ModuleBase()!=module)return Result::Unknown;
#endif
    const int callerH5=CurrentCallerH5();
    // Native50B638 is exclusive CAS0->10000, not a recursive reader lock.
    // The real Step6F036A still owns world+5FF4. AF5EF4 defaults to4,
    // which selects that exact slot; only existing caller slots0..3 are safe.
    if(callerH5<0||callerH5>=4)return Result::CallerUnavailableH5;
#if defined(KCD2VR_WEAPON_BODY_T4_HOST_TEST)
    return Classify(HostClearance(context,shape,ownBody));
#else
    void* skip[]{ownBody};return Classify(Call(context.World(),&shape,skip));
#endif
}
}
