#include "subsystems/hud_ui_controls/runtime/prompt_family_d49.h"
#include <atomic>
#include <cstdint>
#include <cstdio>
#define __fastcall
namespace kcd2vr::prompt_device_lock {
static std::uintptr_t g_prompt_base_d49=0x10000000,caller=0;
static int value=1;static bool qualified=true,writeOK=true,ready=true;
static unsigned lookups=0,writes=0,reads=0,notes=0,nested=0;
static std::atomic<bool> g_hint_ready_d53{true},g_suspended{false};
static std::atomic<int> g_forced_device{2};static thread_local bool g_in_cvar_d49=false;
static void* _ReturnAddress(){return reinterpret_cast<void*>(caller);}
static void* Lookup(void*,const char*){++lookups;return ready?&value:nullptr;}
static void*(*g_hint_original_d53)(void*,const char*)=&Lookup;
static bool QualifiedInputCVarD53(void* p){return p&&qualified;}
static bool CVarReadD49(void* p,int& n){++reads;if(!p)return false;n=value;return true;}
static void* InputDeviceGetD53(void*,const char*);
static bool recursive=false;
static bool CVarWriteD49(void*,int n){++writes;if(writeOK)value=n;if(recursive){++nested;InputDeviceGetD53(nullptr,"wh_ui_InputDevice");}return true;}
static void ObserveHintFamilyD53(int,int,int,prompt_family_d49::Result){++notes;}
#include "prompt_consumer_d53.inc"
}
int main(){using namespace kcd2vr::prompt_device_lock;unsigned checks=0,fail=0;
auto ck=[&](bool b,const char* s){++checks;if(!b){++fail;std::printf("FAIL %s\n",s);}};
caller=g_prompt_base_d49+0x8E6B1C;
ck(InputDeviceGetD53(nullptr,"wh_ui_InputDevice")==&value&&value==2&&lookups==1&&writes==1,"first actual hint read restores Xbox without manager transition");
InputDeviceGetD53(nullptr,"wh_ui_InputDevice");ck(writes==1,"already correct glyph causes no native write");
value=1;caller++;auto r=reads;InputDeviceGetD53(nullptr,"wh_ui_InputDevice");ck(value==1&&reads==r,"different native caller untouched");
caller--;g_suspended=true;InputDeviceGetD53(nullptr,"wh_ui_InputDevice");ck(value==1&&reads==r,"real mouse/pointer suspension untouched");
g_suspended=false;InputDeviceGetD53(nullptr,"wh_ui_InputDevice");ck(value==2,"first post-pointer hint repairs family without device update");
value=1;qualified=false;r=reads;InputDeviceGetD53(nullptr,"wh_ui_InputDevice");ck(value==1&&reads==r,"unknown CVar class untouched");
qualified=true;g_hint_ready_d53=false;InputDeviceGetD53(nullptr,"wh_ui_InputDevice");ck(value==1&&reads==r,"unqualified native boundary no write");
g_hint_ready_d53=true;ready=false;ck(!InputDeviceGetD53(nullptr,"wh_ui_InputDevice")&&reads==r,"missing original lookup value never dereferenced");
ready=true;recursive=true;InputDeviceGetD53(nullptr,"wh_ui_InputDevice");ck(value==2&&nested==1&&!g_in_cvar_d49,"SetInt callback reentry cannot recurse writes");
recursive=false;g_forced_device=4;InputDeviceGetD53(nullptr,"wh_ui_InputDevice");ck(value==2,"PlayStation art selection retains Xbox action-hint lookup");
value=1;writeOK=false;InputDeviceGetD53(nullptr,"wh_ui_InputDevice");ck(value==1&&!g_in_cvar_d49,"native rejection stays rejection and clears recursion guard");
g_hint_original_d53=nullptr;r=reads;ck(!InputDeviceGetD53(nullptr,"wh_ui_InputDevice")&&reads==r,"absent trampoline cannot fabricate CVar");
std::printf("D53 exact prompt consumer %u/%u; native IO mocked\n",checks-fail,checks);return fail?1:0;}
