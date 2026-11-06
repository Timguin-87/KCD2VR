// Actual PartVelocity adapter; memory and native Action are explicit leaves.
// On Linux a C++ exception models the SEH branch; this is not a Windows SEH test.
#include "subsystems/physics/load_fault_recovery_r96.h"
#include "vrik/gore32.h"
#include <cassert>
#include <cstdio>
#include <cstring>
#include <stdexcept>
#ifdef _WIN32
#include <Windows.h>
#else
#define __fastcall
#define __try try
#define __except(x) catch(...)
#define EXCEPTION_EXECUTE_HANDLER 1
#endif
namespace life=kcd2vr::inventory_lifecycle_t8;
namespace recovery=kcd2vr::load_fault_recovery_r96;
static std::uintptr_t baseR96=0;
static unsigned callsR96=0;
static bool faultR96=false,reloadInCallR96=false,qualifiedR96=true;
static int resultR96=1;
static constexpr std::uintptr_t bodyR96=0x8000;
static int __fastcall actionR96(void* body,void* packet,int threadSafe){
    assert(body==reinterpret_cast<void*>(bodyR96)&&threadSafe==0);
    int type=0,part=0;std::memcpy(&type,packet,4);std::memcpy(&part,static_cast<char*>(packet)+8,4);
    assert(type==10&&part==379);++callsR96;
    if(reloadInCallR96){life::Publish(true,true);life::Publish(true,false);}
    if(faultR96){
#ifdef _WIN32
        RaiseException(EXCEPTION_ACCESS_VIOLATION,0,0,nullptr);
#else
        throw std::runtime_error("explicit native Action leaf fault");
#endif
    }
    return resultR96;
}
static std::uintptr_t WhBase(){return baseR96;}
template<class T> static bool Read(const void* ptr,T& value){
    static_assert(sizeof(T)==sizeof(std::uintptr_t));
    if(!qualifiedR96)return false;
    const auto address=reinterpret_cast<std::uintptr_t>(ptr);
    if(address==bodyR96){value=baseR96+0x3A48B30;return true;}
    if(address==baseR96+0x3A48B30+0x38){value=reinterpret_cast<std::uintptr_t>(&actionR96);return true;}
    return false;
}
namespace diagnostic_trace {static void Write(const char*){}}
namespace gore32n {
namespace g=kcd2vr::gore32;
static bool Pins(){return qualifiedR96;}
#include <corpse_velocity_r96.inc>
}
int main(){
    baseR96=reinterpret_cast<std::uintptr_t>(&actionR96)-0xA99DF0;
    life::Publish(true,false);
    assert(gore32n::PartVelocity(bodyR96,379,{.1,0,0})&&callsR96==1);
    faultR96=true;assert(!gore32n::PartVelocity(bodyR96,379,{.1,0,0})&&callsR96==2);
    faultR96=false;
    // No timer, menu recheck, repeated squeeze or unknown UI can retry a fault.
    for(int i=0;i<20;++i)assert(!gore32n::PartVelocity(bodyR96,379,{.1,0,0}));
    life::Recheck();life::RequestRecovery();assert(gore32n::g_drive_fault33.load());
    life::Publish(false,false);life::Publish(true,false);assert(gore32n::g_drive_fault33.load()&&callsR96==2);
    life::Publish(true,true);assert(gore32n::g_drive_fault33.load());
    life::Publish(false,false);assert(gore32n::g_drive_fault33.load());
    life::Publish(true,false);
    assert(!gore32n::g_drive_fault33.load());
    qualifiedR96=false;assert(!gore32n::PartVelocity(bodyR96,379,{.1,0,0})&&callsR96==2);
    qualifiedR96=true;assert(gore32n::PartVelocity(bodyR96,379,{.1,0,0})&&callsR96==3);
    // An ordinary refusal is not a process-lifetime exception.
    resultR96=0;assert(!gore32n::PartVelocity(bodyR96,379,{.1,0,0}));
    resultR96=1;assert(gore32n::PartVelocity(bodyR96,379,{.1,0,0}));
    // A late exception belongs to the load captured before the native call.
    reloadInCallR96=faultR96=true;
    assert(!gore32n::PartVelocity(bodyR96,379,{.1,0,0}));
    reloadInCallR96=faultR96=false;
    assert(!gore32n::g_drive_fault33.load()&&gore32n::PartVelocity(bodyR96,379,{.1,0,0}));
    std::puts("PASS actual corpse Action: fault stops repeated calls; only completed observed load permits fresh qualification; refusal and late old fault do not poison later work");
}
