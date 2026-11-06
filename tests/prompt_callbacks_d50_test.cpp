#include "subsystems/hud_ui_controls/runtime/prompt_family_d49.h"
#include "diagnostics/capture168.h"
#include <atomic>
#include <mutex>
#include <cstdio>
#define __fastcall
namespace kcd2vr::prompt_device_lock {
static unsigned reads=0,writes=0,clockReads=0,nativeCalls=0,navigationCalls=0;
static int value=1,current=1;static bool available=true,writeAllowed=true,reenter=false;
static std::uint64_t tick=1000;
static std::atomic<int> g_forced_device{2};
static std::atomic<bool> g_suspended{false};
static std::atomic<std::uint64_t> g_navigation{0},g_forced_calls{0};
static std::uint64_t g_navigation_seen=0;
static std::recursive_mutex g_cursor_mutex;
static thread_local bool g_in_cvar_d49=false;
static std::atomic<std::uint64_t> g_cvar_changed_d49{0},g_cvar_rejected_d49{0},g_cvar_unavailable_d49{0};
static std::uint64_t GetTickCount64(){++clockReads;return tick;}
static void NativeNavigation(){++navigationCalls;}
static bool CurrentDevice(void*,int& device){device=current;return true;}
static void* InputDeviceCVarD49(){return available?&value:nullptr;}
static bool CVarReadD49(void* var,int& out){++reads;if(!var)return false;out=value;return true;}
static void Detour(void*,int);
static bool CVarWriteD49(void* var,int forced){++writes;if(!var)return false;if(writeAllowed)value=forced;if(reenter)Detour(&current,value);return true;}
static void Original(void*,int requested){++nativeCalls;current=requested;}
static void(*g_original)(void*,int)=&Original;
#include "prompt_callbacks_d50.inc"
}
int main(){
    using namespace kcd2vr::prompt_device_lock;
    namespace cap=kcd2vr::diagnostics::capture168;
    unsigned checks=0,fail=0;auto ck=[&](bool b,const char* why){++checks;if(!b){++fail;std::printf("FAIL %s\n",why);}};
    cap::gate.mask=0;Detour(&current,1);
    ck(current==2&&value==2&&nativeCalls==1&&writes==1,"F2/F5 off still corrects manager and CVar");
    ck(!clockReads&&!g_forced_calls.load()&&!g_cvar_changed_d49.load(),"off performs no diagnostic clock/counter work");
    Detour(&current,1);ck(nativeCalls==1&&writes==1,"already-correct value neither replays original nor writes CVar");
    cap::gate.select(cap::Domain::Bug,true,tick);value=1;current=1;reenter=true;const auto before=nativeCalls;
    Detour(&current,1);
    ck(nativeCalls==before+1&&value==2&&current==2,"native CVar callback reentry does not repeat manager change");
    ck(!g_in_cvar_d49&&g_cvar_changed_d49.load()==1&&g_forced_calls.load()==1,"one captured CVar outcome after bounded reentry");
    reenter=false;value=1;writeAllowed=false;Detour(&current,1);
    ck(value==1&&g_cvar_rejected_d49.load()==1,"rejected native write reported rather than accepted");
    writeAllowed=true;available=false;Detour(&current,1);
    ck(g_cvar_unavailable_d49.load()==1,"unavailable native CVar has explicit captured outcome");
    available=true;g_suspended=true;const auto readBefore=reads,writeBefore=writes;Detour(&current,1);
    ck(current==1&&reads==readBefore&&writes==writeBefore,"native navigation suspension preserves requested device");
    g_suspended=false;g_forced_device=4;Detour(&current,1);
    ck(current==2&&value==2,"PS art choice retains Xbox semantic family in both native owners");
    cap::gate.select(cap::Domain::Bug,false,tick);cap::gate.select(cap::Domain::Vrik,true,tick);
    value=1;const auto countBefore=g_cvar_changed_d49.load();const auto clocksBefore=clockReads;Detour(&current,1);
    ck(value==2&&g_cvar_changed_d49.load()==countBefore&&clockReads==clocksBefore,"F7 cannot enable prompt diagnostics or diagnostic clock");
    cap::gate.select(cap::Domain::Interaction,true,tick);value=1;Detour(&current,1);
    ck(g_cvar_changed_d49.load()==countBefore+1,"F5 captures prompt result");
    tick+=cap::Duration;value=1;Detour(&current,1);
    ck(value==2&&g_cvar_changed_d49.load()==countBefore+1,"expired F5 still corrects input but stops diagnostics");
    g_original=nullptr;value=1;const auto finalWrites=writes;Detour(&current,1);
    ck(value==1&&writes==finalWrites,"missing original leaves native state untouched");
    std::printf("D50 actual prompt callbacks %u/%u; native IO and callbacks mocked\n",checks-fail,checks);
    return fail?1:0;
}
