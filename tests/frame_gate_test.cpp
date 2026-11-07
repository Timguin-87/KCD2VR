// Actual production predicates + installer. Windows memory/hook operations are
// explicit host replacements; this does NOT execute the retail frame function.
#include <array>
#include <vector>
#include <atomic>
#include <optional>
#include <string>
#include <cstdio>
#include <cstring>
#include <cstdint>
#include <utility>
#define __try try
#define __except(x) catch(...)
#define __fastcall
static std::vector<unsigned char> image(0x5500000);
static bool peOkay=true,execOkay=true,createOkay=true,enableOkay=true,originalOkay=true;
static std::uint32_t timestamp=0x6A350E20;
static int installs=0;static std::atomic<bool> g_supported{false},g_nexus_installed{false};
static std::uintptr_t WhBase(){return reinterpret_cast<std::uintptr_t>(image.data());}
static bool ReadPe(std::uintptr_t,std::uint32_t&t){t=timestamp;return peOkay;}
static bool MatchBytes(std::uintptr_t p,const unsigned char* e,std::size_t n){return p>=WhBase()&&p+n<=WhBase()+image.size()&&std::memcmp(reinterpret_cast<void*>(p),e,n)==0;}
namespace FrameSync {static bool PublishViewCameraHookInstalled(){return true;}}
template<class...T>static void Tracef(const char*,T...){}
static void Trace(const char*){}
#include "generated_frame_gate.inc"
using FrameNexusFn=void(*)(void*,std::uintptr_t,std::uint32_t);
static void FrameNexusDetour(void*,std::uintptr_t,std::uint32_t){}
static void Original(void*,std::uintptr_t,std::uint32_t){}
namespace DetourModKit {
struct Address {std::uintptr_t value;};namespace hook {
struct Target{Address address;};struct Options{};
struct InlineRequest{std::string name;Target target;Options options;};
struct Hook{template<class T>T original(){return originalOkay?static_cast<T>(&Original):nullptr;}
 bool enable(){return enableOkay;}};
template<class F>static std::optional<Hook> inline_at(InlineRequest,F){++installs;return createOkay?std::optional<Hook>(Hook{}):std::nullopt;}
}}
static std::optional<DetourModKit::hook::Hook> g_nexus_hook;
static FrameNexusFn g_nexus_original=nullptr;
static bool Executable(const void*){return execOkay;}
#include "generated_frame_install.inc"
#include "generated_frame_version.h"
static int total=0,pass=0;
static void Check(bool b,const char*n){++total;pass+=b;std::printf("%s %s\n",b?"PASS":"FAIL",n);}
static void Reset(){
 std::fill(image.begin(),image.end(),0);peOkay=execOkay=createOkay=enableOkay=originalOkay=true;
 timestamp=kSupportedTimestamp;g_nexus_hook.reset();g_nexus_original=nullptr;g_nexus_installed=false;installs=0;
 #include "generated_frame_setup.inc"
 auto put=[](std::uintptr_t address,std::uintptr_t value){std::memcpy(image.data()+address,&value,8);};
 put(kCCryActionVtableRva+kCCryActionFrameNexusSlot*sizeof(void*),WhBase()+kFrameNexusRva);
 for(auto p:std::initializer_list<std::pair<unsigned,std::uintptr_t>>{{0x48,kCSystemRenderRva},{0x50,kCSystemRenderBeginRva},{0x58,kCSystemRenderEndRva},{0x430,kCSystemSetViewCameraRva},{0x438,kCSystemGetViewCameraRva}})
   put(kCSystemVtableRva+p.first,WhBase()+p.second);
}
int main(){
 Reset();Check(ExactBuildReady(),"pristine fixture satisfies original broad gate");
 image[kCRenderViewCameraBuildRva]=0xE9;
 Check(!ExactBuildReady(),"original broad gate rejects normal hooked stereo camera entry");
 Check(EnsurePhysicalFramePump6698()&&g_nexus_installed&&installs==1,
       "physical frame installs with camera already hooked, preserving frame signatures");
 if(g_nexus_installed){image[kFrameNexusRva]=0xE9;Check(EnsurePhysicalFramePump6698()&&installs==1,"known installed frame hook is idempotent");}
 for(auto address:{kFrameNexusRva,kPhase7e8CallsiteRva,kCameraHandoffCallsiteRva,kRendererPhaseCallsiteRva,kPhase7f8CallsiteRva}){
   Reset();image[address]^=1;Check(!EnsurePhysicalFramePump6698()&&!g_nexus_installed&&installs==0,"corrupt native frame contract remains rejected");}
 Reset();timestamp^=1;Check(!EnsurePhysicalFramePump6698()&&installs==0,"different PE timestamp rejected");
 Reset();peOkay=false;Check(!EnsurePhysicalFramePump6698()&&installs==0,"invalid PE rejected");
 Reset();image[kCCryActionVtableRva+kCCryActionFrameNexusSlot*sizeof(void*)]^=1;
 Check(!EnsurePhysicalFramePump6698()&&installs==0,"wrong exact frame vtable slot rejected");
 Reset();execOkay=false;Check(!EnsurePhysicalFramePump6698()&&installs==0,"non-executable frame target rejected");
 Reset();createOkay=false;Check(!EnsurePhysicalFramePump6698()&&!g_nexus_installed,"hook creation failure is not arming success");
 Reset();enableOkay=false;Check(!EnsurePhysicalFramePump6698()&&!g_nexus_installed&&!g_nexus_hook&&!g_nexus_original,"hook enable failure clears incomplete installation");
 Reset();originalOkay=false;Check(!EnsurePhysicalFramePump6698()&&!g_nexus_installed&&!g_nexus_hook,"missing original trampoline cannot become an armed frame pump");
 std::printf("frame_gate=%d/%d native_API=MOCKED\n",pass,total);return pass!=total;
}
