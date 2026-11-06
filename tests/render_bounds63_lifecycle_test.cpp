// Actual generated production decision AND install/shutdown. Native memory,
// module pinning and patch handles are explicit host leaves, not game injection.
#define KCD2VR_TEST_HOOK_SPY115
#define KCD2VR_RENDER_BOUNDS_TEST63
#include <DetourModKit/hook.hpp>
#define main BoundsDecisionChecks63
#include "render_bounds63_adapter_test.cpp"
#undef main
using HMODULE=void*;using LPCWSTR=const wchar_t*;
static constexpr unsigned GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS=4,GET_MODULE_HANDLE_EX_FLAG_PIN=1;
static bool retailAllowed63=true,pinAllowed63=true;
static unsigned pinCalls63=0,installCalls63=0;
static LPCWSTR pinAddress63=nullptr;
static std::optional<DetourModKit::hook::Hook> g_henry_bounds_hook63;
static bool SupportedWhBuild(std::uintptr_t b){return b==base63&&retailAllowed63;}
namespace retail_build112 {static bool Ready(std::uintptr_t){return retailAllowed63;}static bool Verify(std::uintptr_t){return retailAllowed63;}}
static bool Safe(const void* p,std::size_t n){const auto a=reinterpret_cast<std::uintptr_t>(p);return a>=base63&&a+n<=base63+moduleMemory63.size();}
static int GetModuleHandleExW(unsigned flags,LPCWSTR address,HMODULE* out){
    ++pinCalls63;pinAddress63=address;
    *out=pinAllowed63&&flags==(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_PIN)?moduleMemory63.data():nullptr;
    return *out!=nullptr;
}
#include "generated_render_bounds63_lifecycle.inc"
int main(int argc,char** argv){
    const bool failEnable=argc>1&&std::strcmp(argv[1],"fail-enable")==0;
    const bool failDisable=argc>1&&std::strcmp(argv[1],"fail-disable")==0;
    moduleMemory63.resize(0x4060400);base63=reinterpret_cast<std::uintptr_t>(moduleMemory63.data());
    const unsigned char getter[]{0x48,0x8D,0x81,0xE0,0x09,0,0,0xC3};
    const unsigned char caller[]{0x48,0x8B,0x02,0x48,0x8B,0xCA,0xFF,0x90,0xA8,0,0,0,
        0xF3,0x0F,0x10,0,0xF3,0x0F,0x5D,3,0xF3,0x0F,0x11,3};
    std::memcpy(moduleMemory63.data()+rb63::kGetter,getter,sizeof(getter));
    std::memcpy(moduleMemory63.data()+0x63F902,caller,sizeof(caller));
    const auto entry=base63+rb63::kGetter;std::memcpy(moduleMemory63.data()+rb63::kCharacterVft+0xA8,&entry,sizeof(entry));
    DetourModKit::hook::hostOriginal115=[](const auto& r)->std::uintptr_t{++installCalls63;return r.name=="kcd2vr_c63_henry_render_bounds"?reinterpret_cast<std::uintptr_t>(&Original63):0;};
    g_henry_bounds_original63=nullptr;g_henry_bounds_enabled63=false;originalCalls63=1;
    retailAllowed63=false;check63(!InstallHenryBounds63(base63)&&!installCalls63,"unsupported fingerprint never patches");retailAllowed63=true;
    moduleMemory63[rb63::kGetter]^=1;check63(!InstallHenryBounds63(base63)&&!installCalls63,"wrong getter pin never patches");moduleMemory63[rb63::kGetter]^=1;
    pinAllowed63=false;check63(!InstallHenryBounds63(base63)&&!g_henry_bounds_original63&&!installCalls63,"failed self-module pin never patches");pinAllowed63=true;
    DetourModKit::hook::hostFailEnableBounds63=failEnable;
    const bool installed=InstallHenryBounds63(base63);
    check63(installed!=failEnable&&installCalls63==1&&pinCalls63==2,"one startup install attempt after exact pins and module pin");
    check63(pinAddress63==reinterpret_cast<LPCWSTR>(&HenryBoundsDetour63),"module pinned by exact detour address");
    check63(g_henry_bounds_hook63.has_value()&&g_henry_bounds_original63==&Original63,"original/handle exist before enable result is acted on");
    if(failEnable){
        check63(g_henry_bounds_retired63&&!g_henry_bounds_enabled63&&g_henry_bounds_hook63->live,"potentially reachable failed-enable handle retained");
        const auto before=originalCalls63;
        check63(call63()==native63()&&originalCalls63==before+1,"failed-enable callback forwards immutable original exactly once");
        check63(!InstallHenryBounds63(base63)&&installCalls63==1,"failed enable forbids replacement installation");
    }else{
        reset63();check63(call63()!=native63(),"installed native caller can receive current render union");
        DetourModKit::hook::hostFailDisableBounds63=failDisable;
        if(failDisable)onOriginal63=&ShutdownHenryBounds63;else onRead63=&ShutdownHenryBounds63;
        const auto before=originalCalls63;
        check63(call63()==native63()&&originalCalls63==before+1,"shutdown during original or checked read retires permission before result");
        onOriginal63=nullptr;
        check63(g_henry_bounds_retired63&&!g_henry_bounds_enabled63&&g_henry_bounds_hook63.has_value()&&
            g_henry_bounds_original63==&Original63,"disarm retains pinned handle/trampoline even after successful disable");
        check63(g_henry_bounds_hook63->live==failDisable,"failed disarm remains reachable and safe");
        const auto again=originalCalls63;check63(call63()==native63()&&originalCalls63==again+1,"retained inflight callback remains original-only");
        check63(!InstallHenryBounds63(base63)&&installCalls63==1,"no second installation after retirement");
    }
    std::printf("render_bounds63_lifecycle mode=%s: %u checks %u failures; native injection not executed\n",failEnable?"failed-enable":failDisable?"failed-disarm":"successful-disarm",checks63,failures63);
    return failures63?1:0;
}
