#include <atomic>
#include <array>
#include <optional>
#include <string>
#include <cstring>
#include <cassert>
#include <cstdint>
#include "diagnostics/optional_groups175.h"
namespace diagnostics=kcd2vr::diagnostics;
static unsigned installed=0,reads=0,carry=0;static bool retail=true;static int failAt=-1;
static std::uintptr_t WhBase(){return 0x10000;}
namespace retail_build112 {bool Verify(std::uintptr_t){return retail;}}
namespace diagnostic_trace {void Write(const char*){}}
static bool ArmCarryObserver59c7g(){++carry;return true;}
namespace DetourModKit {struct Address{std::uintptr_t p;}; namespace hook {
struct Options{bool fail_if_already_hooked=false;};struct Target{Address a;};struct MidRequest{std::string n;Target t;Options o;};
struct Handle{unsigned i;bool enable(){++installed;return true;}void disable(){--installed;}};
static std::optional<Handle> mid_at(MidRequest r,void(*)()noexcept){unsigned i=unsigned((r.t.a.p-WhBase())/32);if(int(i)==failAt)return {};return Handle{i};}
}}
struct Site{std::uintptr_t rva;unsigned char bytes[16];void(*callback)()noexcept;};
static std::array<Site,23> kLifecycleSites165{};
static std::array<std::optional<DetourModKit::hook::Handle>,23> g_lifecycle_hooks165{};
static std::array<unsigned,6> g_lifecycle_failure36{};static std::array<std::uintptr_t,6> g_lifecycle_failure_siteH17{};
static std::array<const char*,6> g_lifecycle_failure_stageH17{};
static std::atomic<unsigned> g_lifecycle_coverage175{0};static bool g_lifecycle_ready165=false;
static std::uintptr_t g_lifecycle_attempted175=0;static unsigned g_lifecycle_attemptedGroupsD84=0;
static bool Read(unsigned char* p,unsigned char& out){++reads;auto offset=std::uintptr_t(p)-WhBase();auto i=offset/32;if(i>=23)return false;out=g_lifecycle_hooks165[i]?0xcc:0x90;return true;}
#include "gameplay_hooks_d84r1.inc"
int main(){for(unsigned i=0;i<23;++i){kLifecycleSites165[i].rva=i*32;std::memset(kLifecycleSites165[i].bytes,0x90,16);}
 retail=false;assert(!ArmLifecycle165(1));assert(installed==0);retail=true;
 assert(ArmLifecycle165(1));assert(installed==4&&carry==0&&g_lifecycle_coverage175==1);
 auto before=reads;assert(ArmLifecycle165(1));assert(reads==before&&installed==4);
 // Later F5 can install observers without inspecting/replacing the owned pickup hooks.
 assert(ArmLifecycle165());assert(installed==23&&carry==1&&g_lifecycle_coverage175==63);
 before=reads;assert(ArmLifecycle165());assert(reads==before&&installed==23);
 // Another session with a partial failed pickup group rolls back; no blind retry.
 for(auto& h:g_lifecycle_hooks165)h.reset();installed=0;g_lifecycle_attempted175=0;g_lifecycle_attemptedGroupsD84=0;g_lifecycle_coverage175=0;failAt=2;
 assert(!ArmLifecycle165(1));assert(installed==0);before=reads;assert(!ArmLifecycle165(1));assert(reads==before);
 assert(ArmLifecycle165());assert(installed==19&&g_lifecycle_coverage175==62);
}
