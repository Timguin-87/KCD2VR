// Complete production door-signal TU. Only native module lookup/fingerprint and
// hook backend are substituted. No game/Lua function or executable bytes run.
#define KCD2VR_TEST_HOOK_SPY115 1
#include <windows.h>
#include <vector>
#include <cassert>
#include <cstdio>
static std::vector<unsigned char> doorImage32(0x2AC0000);
#include "../src/door_signal19c4r4.cpp"
namespace kcd2vr::retail_build112 {
bool Verify(std::uintptr_t b) noexcept{return b==reinterpret_cast<std::uintptr_t>(doorImage32.data());}
bool Ready(std::uintptr_t b) noexcept{return Verify(b);}
}
namespace kcd2vr::diagnostic_trace {void Write(const char*) noexcept{}}
static int __fastcall OriginalDoor32(void*,void*){return 73;}
int main(){
 using namespace kcd2vr::door_signal19c4r4;
 hostModuleHandleW=doorImage32.data(); // existing Windows native-leaf shim
 auto pin=[](std::uintptr_t at,std::initializer_list<unsigned char> b){std::copy(b.begin(),b.end(),doorImage32.begin()+at);};
 pin(0x41C388,{0x48,0x89,0x5C,0x24,0x08,0x57,0x48,0x83,0xEC,0x30,0x48,0x8B,0x02,0x48,0x8B,0xCA});
 pin(0x41DC20,{0x48,0x89,0x5C,0x24,0x08,0x57,0x48,0x83,0xEC,0x40,0x48,0x8B,0x01,0x49,0x8B,0xF8});
 pin(0x2AB4C2C,{0x40,0x53,0x48,0x83,0xEC,0x40,0x8B,0x02,0x0F,0x57,0xC0});
 pin(0x144BFD2,{0x4C,0x8D,0x05,0xAF,0x03,0xFD,0xFE});
 unsigned installs=0;
 static unsigned* counter=nullptr;counter=&installs;
 DetourModKit::hook::hostOriginal115=+[](const DetourModKit::hook::InlineRequest& r){
  assert(r.name=="kcd2vr_c4r4_private_door_getter");++*counter;
  return reinterpret_cast<std::uintptr_t>(&OriginalDoor32);
 };
 // frik Install unconditionally starts with Uninstall -> door Shutdown.
 assert(Shutdown()&&Shutdown()&&SetEnabled(false)&&!Ready());
 assert(SetEnabled(true)&&Ready()&&installs==1);
 assert(doorImage32[0x41C388]==0xCC);
 assert(SetEnabled(false)&&!Ready()&&SetEnabled(true)&&Ready()&&installs==1);
 // After a REAL published hook, shutdown retains its callable original and
 // permanently refuses hot reinstall. The cold-start fix cannot weaken this.
 assert(Shutdown()&&!Ready()&&doorImage32[0x41C388]==0x48);
 assert(!SetEnabled(true)&&installs==1&&Shutdown());
 assert(DoorGetCVarDetour19c4r4(nullptr,nullptr)==73);
 std::puts("PASS actual door signal: cold Shutdown permits FIRST install, OFF/ON reuses live hook, real retirement retains original and refuses reinstall");
}
