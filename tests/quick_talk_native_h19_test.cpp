// Production observer callbacks, native original/patching leaves replaced.
#define KCD2VR_TEST_HOOK_SPY115
#include <windows.h>
#include <DetourModKit/hook.hpp>
#include <atomic>
#include <optional>
#include <cstring>
#include <utility>
#include <cstdio>
#include <vector>
#include <cassert>
#include "subsystems/hud_ui_controls/runtime/quick_talk_native_h19.inl"
namespace kcd2vr::retail_build112 {bool Verify(std::uintptr_t) noexcept{return true;}bool Ready(std::uintptr_t) noexcept{return true;}}
using namespace kcd2vr::quick_talk_native_h19;
static unsigned calls=0;static bool nested=false;
static void NativeToggle(void*,bool enabled){++calls;assert(!Read().known);if(nested&&enabled){nested=false;Toggle(nullptr,false);}}
static void NativeInput(void* p,bool enabled){++calls;static_cast<unsigned char*>(p)[0]=enabled;}
int main(){
    std::vector<unsigned char> image(0x3B00000);hostModuleHandleW=image.data();
    const auto base=reinterpret_cast<std::uintptr_t>(image.data());
    const unsigned char toggle[]{0x40,0x53,0x48,0x83,0xEC,0x20,0x8A,0xDA,0xE8,0xCF,0xF4,0x2F,0x00};
    const unsigned char input[]{0x48,0x89,0x5C,0x24,0x08,0x48,0x89,0x74,0x24,0x10,0x57,0x48,0x83,0xEC,0x20,0x40,0x8A,0xFA};
    const unsigned char selector[]{0x80,0x7E,0x35,0};
    std::memcpy(image.data()+0x6160EC,toggle,sizeof(toggle));std::memcpy(image.data()+0x616128,input,sizeof(input));
    std::memcpy(image.data()+0x61615A,selector,sizeof(selector));std::memcpy(image.data()+0x3A42518,"chat_interaction",17);
    DetourModKit::hook::hostOriginal115=[](const auto& request)->std::uintptr_t{
        return reinterpret_cast<std::uintptr_t>(request.name=="h19_chat_map_toggle"?&NativeToggle:&NativeInput);};
    assert(!Open()&&!Read().known);assert(Install()&&!Open()&&Read().ready&&!Read().known);
    Toggle(nullptr,true);assert(Open()&&calls==1);const auto first=Read().revision;
    Toggle(nullptr,false);assert(!Open()&&Read().known&&Read().revision>first&&calls==2);
    nested=true;Toggle(nullptr,true);assert(!Open()&&Read().known&&calls==4); // inner close wins
    unsigned char state[0x38]{};state[0x35]=1;
    Input(state,true);assert(Open()&&state[0]==1&&calls==5);
    Input(state,false);assert(!Open()&&Read().known&&state[0]==0&&calls==6);
    // Full dialogue's same native wrapper must not publish quick-talk state.
    const auto before=Read().revision;state[0x35]=0;Input(state,true);
    assert(!Open()&&Read().revision==before&&calls==7);
    // If the scoped native mode is no longer classifiable, never retain open.
    state[0x35]=1;Input(state,true);assert(Open());
    Uninstall();assert(!Open()&&!Read().ready&&!Read().known);
    assert(image[0x6160EC]==toggle[0]&&image[0x616128]==input[0]);
    image[0x6160EC]^=1;assert(!Install()&&!Open());
    std::puts("PASS production native-chat callbacks: original first, open/close, nested close, dialogue exclusion, teardown, exact pins");
}
