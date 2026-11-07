#include <atomic>
#include <cstdint>
#include <cstdio>
#define __fastcall
static std::atomic<std::uint64_t> g_nexus_calls{},g_last_nexus_serial{},g_nexus_present_advances{},g_last_nexus_log_ms{};
static std::atomic<std::uint32_t> g_last_flags{};
static std::atomic<int> g_mode{};
static thread_local std::uint64_t t_nexus_serial=0;
static thread_local unsigned t_nexus_depth=0;
static unsigned nativeCalls=0,physicalCalls=0,postCalls=0,logCalls=0;
static std::uint64_t presents=0;
static void (*g_nexus_original)(void*,std::uintptr_t,std::uint32_t)=nullptr;
static bool beforeNative=true,afterNative=true;
struct DeviceHook {static std::uint64_t PresentCaptureCount(){return presents;}};
static std::uint64_t GetTickCount64(){return 10000;}
static unsigned GetCurrentThreadId(){return 1;}
template<class... A>static void Tracef(const char*,A...){++logCalls;}
namespace frik_pose_probe {static void PhysicalNativeFrameEnd104(){++postCalls;afterNative=afterNative&&nativeCalls==2;} static void PhysicalNativeFrameTick6698(){beforeNative=beforeNative&&nativeCalls==0;++physicalCalls;}}
#include "generated_physical_frame.inc"
static void Original(void*,std::uintptr_t,std::uint32_t){++nativeCalls;++presents;if(t_nexus_depth==1)FrameNexusDetour(nullptr,0,0);}
int main(){unsigned n=0,failed=0;auto check=[&](bool b,const char* s){++n;if(!b)++failed;std::printf("%s %s\n",b?"PASS":"FAIL",s);};
 g_nexus_original=&Original;FrameNexusDetour(nullptr,0,0);
 check(physicalCalls==1&&nativeCalls==2,"actual outer native frame invokes physical pump once despite recursion");
 check(beforeNative,"native command pump executes before native frame body");
 check(postCalls==1&&afterNative,"actual post-frame state observer runs once after original including recursive work");
 check(t_nexus_depth==0&&t_nexus_serial==0,"native nesting state restored");
 check(logCalls==0,"physical-only frame hook does not enable nexus diagnostic output");
 nativeCalls=physicalCalls=postCalls=0;FrameNexusDetour(nullptr,0,0x20);
 check(physicalCalls==0&&postCalls==0&&nativeCalls==2,"non-gameplay frame flag rejects physical pump, native frame still executes");
 g_mode=1;FrameNexusDetour(nullptr,0,0x20);check(logCalls>0,"explicit nexus diagnostics remain available");
 std::printf("physical_frame=%u/%u full_outer_translation_unit=NOT_COMPILED\n",n-failed,n);return failed?1:0;
}
