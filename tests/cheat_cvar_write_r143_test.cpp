// R143: the real CheatCVarWrite143 / CheatCVarSet*143 (extracted from src/mod.cpp by tools/run_cheat_cvar_r143.py)
// and the real production users (H24 block cooldown lease, R58C2 focus zoom backend, R123 activity faders, V58 dialogue
// mode, D70 dodge window with the production CombatWindowsConsoleH3 IO) against a fake console that behaves like the
// retail one: Set() on a VF_CHEAT cvar is refused unless dev mode. no game, renderer or headset is executed.
#include <atomic>
#include <algorithm>
#include <climits>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <limits>
#include <string>
#include <vector>
#define __try if(true)
#define __except(x) else if(false)
#define EXCEPTION_EXECUTE_HANDLER 1
static constexpr std::uintptr_t kBase=0x180000000ULL;
static bool retail=true,devMode=false;
static void* GetModuleHandleW(const wchar_t*){return reinterpret_cast<void*>(kBase);}
namespace retail_build112 {static bool Ready(std::uintptr_t b){return retail&&b==kBase;}}
namespace kcd2vr {namespace retail_build112 {static bool Ready(std::uintptr_t b){return ::retail_build112::Ready(b);}}}
// fake cvar object, retail layout: +0 vtable, +0x18 flags, +0x20/+0x28 functor list, +0x38 callback, +0x48 value/pointer
enum Kind {FloatRef,IntRef,PlainInt,Other};
struct alignas(16) FakeCVar {
    unsigned char raw[0x60]{};float f=0;int i=0;std::string name;
    FakeCVar(const char* n,Kind k,unsigned flags,float v):name(n){
        const std::uintptr_t vt=kBase+(k==FloatRef?0x3DCFF38:k==IntRef?0x3DD0020:k==PlainInt?0x3A7F0B8:0x3A00000);
        std::memcpy(raw,&vt,8);std::memcpy(raw+0x18,&flags,4);
        void* p=k==FloatRef?static_cast<void*>(&f):static_cast<void*>(&i);
        if(k==PlainInt){i=int(v);std::memcpy(raw+0x48,&i,4);}else std::memcpy(raw+0x48,&p,8);
        if(k==FloatRef)f=v;else i=int(v);
    }
    Kind kind()const{std::uintptr_t vt;std::memcpy(&vt,raw,8);return vt==kBase+0x3DCFF38?FloatRef:vt==kBase+0x3DD0020?IntRef:vt==kBase+0x3A7F0B8?PlainInt:Other;}
    unsigned flags()const{unsigned x;std::memcpy(&x,raw+0x18,4);return x;}
    float get()const{switch(kind()){case FloatRef:return f;case PlainInt:{int x;std::memcpy(&x,raw+0x48,4);return float(x);}default:return float(i);}}
    void store(float v){switch(kind()){case FloatRef:f=v;break;case PlainInt:{int x=int(v);std::memcpy(raw+0x48,&x,4);break;}default:i=int(v);}}
};
static unsigned nativeSets=0,refusedSets=0;
static bool Allowed(const FakeCVar& c){const unsigned fl=c.flags();if(fl&0x40000800u)return false;return devMode||!(fl&0x3800002u);}
static std::vector<FakeCVar*> table;
static int consoleToken=0;static bool consoleReady=true;
static void* SafeGetCryConsole(){return consoleReady?&consoleToken:nullptr;}
static void* SafeConsoleGetCVar(void*,const char* n){for(auto* c:table)if(c->name==n)return c->raw;return nullptr;}
static FakeCVar* Of(void* p){for(auto* c:table)if(c->raw==p)return c;return nullptr;}
static float SafeCVarGetFloat(void* p,float fb=0.f){auto* c=Of(p);return c?c->get():fb;}
static int SafeCVarGetInt(void* p,int fb){auto* c=Of(p);return c?int(c->get()):fb;}
static bool SafeCVarSetFloat(void* p,float v){auto* c=Of(p);if(!c)return false;if(Allowed(*c)){c->store(v);++nativeSets;}else ++refusedSets;return true;}
static bool SafeCVarSetInt(void* p,int v){return SafeCVarSetFloat(p,float(v));}
static std::vector<std::string> lines;
namespace diagnostic_trace {static void Write(const char* s){lines.emplace_back(s);}}
namespace debug_trace {static void Write(const char* s){lines.emplace_back(s);}}
static bool cooldownNotified=false;
namespace frik_pose_probe {static void SetBlockCooldownLease59c17(bool a){cooldownNotified=a;}}
#include "cheat_cvar_r143_production.inc"
#include "subsystems/masterstrike_combos/block_cooldown_lease_h24.inl"
#include "focus_zoom58c2.h"
#include "vrik/dodge_window_d70.h"
struct RagdollConsole59 {
    bool Read(const char* name,float& out) noexcept {void* v=SafeConsoleGetCVar(SafeGetCryConsole(),name);if(!v)return false;out=SafeCVarGetFloat(v,-12345.f);return out!=-12345.f;}
};
#include "combat_windows_console_h3_production.inc"
#define KCD2VR_M2_HOST_SLOT 5013
#include "subsystems/interactions/host.inl"
#undef KCD2VR_M2_HOST_SLOT
#define KCD2VR_M2_HOST_SLOT 5014
#include "subsystems/interactions/host.inl"
#undef KCD2VR_M2_HOST_SLOT
#define KCD2VR_M2_HOST_SLOT 5015
#include "subsystems/dialogues/host.inl"
#undef KCD2VR_M2_HOST_SLOT
static unsigned checks=0,failures=0;
static void ck(bool x,const char* t){++checks;if(!x){++failures;std::printf("FAIL %s\n",t);}}
static bool near(float a,float b){return std::fabs(a-b)<1e-5f;}
int main(){
    // retail registrations (flags from the register calls in WHGame 6A350E20)
    FakeCVar cooldown("wh_cs_BlockCooldown",FloatRef,2,.2f),focus("wh_pl_FocusFOVMaxDecrease",FloatRef,2,30.f),
        dodge("wh_cs_Dodge_WindowMod",FloatRef,2,.65f),noCam("wh_dlg_NoCameras",PlainInt,2,0),pov("wh_dlg_ForcePOV",IntRef,0,0),
        herb("wh_pl_HerbGatheringFadeLength",IntRef,2,1),herbIn("wh_pl_HerbGatheringFadeInLength",FloatRef,2,.5f),
        herbOut("wh_pl_HerbGatheringFadeOutLength",FloatRef,2,.5f),dig("wh_pl_DiggingFaderLength",FloatRef,2,1.f),
        butcher("wh_pl_ButcheringFaderLength",FloatRef,2,1.f),alchemy("wh_pl_AlchemyFinishFaderLength",FloatRef,0,2.f),
        cone("wh_cs_PlayerInputLockAreaWidth",FloatRef,4,.4f);
    table={&cooldown,&focus,&dodge,&noCam,&pov,&herb,&herbIn,&herbOut,&dig,&butcher,&alchemy,&cone};
    char msg[400]{};

    // ---- 1. dev mode (old sessions): Set() works, the R143 write never runs, same values as before
    devMode=true;
    {BlockCooldownLease59c17 l;nativeSets=0;ck(l.Apply(true,msg,sizeof(msg))&&near(cooldown.get(),.001f)&&std::strstr(msg,"written=1")&&nativeSets==1,"dev mode: H24 via Set() only");
     ck(l.Apply(false,msg,sizeof(msg))&&near(cooldown.get(),.2f)&&nativeSets==2,"dev mode: H24 restore via Set() only");}
    {kcd2vr::focus_zoom58c2::Lease z;FocusZoomConsole58c2 io;nativeSets=0;auto r=z.step(true,1000,io);
     ck(r.ok&&near(focus.get(),0.f)&&nativeSets==1,"dev mode: focus zoom via Set()");r=z.step(false,2000,io);ck(r.ok&&near(focus.get(),30.f)&&nativeSets==2,"dev mode: focus zoom restore");}
    {DialogueCVarSnapshot s{};ck(CaptureDialogueCVars(s)&&s.no_cameras==0,"capture dialogue originals");nativeSets=refusedSets=0;
     ck(ApplyDialogueMode(s,2)&&pov.get()==1.f&&noCam.get()==1.f&&nativeSets==2&&refusedSets==0,"dev mode: dialogue mode 2 via Set()");
     ck(ApplyDialogueMode(s,0)&&pov.get()==0.f&&noCam.get()==0.f,"dev mode: dialogue mode 0 restores");}

    // ---- 2. no dev mode (his 7 Nov start): Set() refused on the cheat cvars -> R143 write, features run
    devMode=false;
    {BlockCooldownLease59c17 l;nativeSets=refusedSets=0;ck(l.Apply(true,msg,sizeof(msg))&&near(cooldown.get(),.001f)&&std::strstr(msg,"written=1")&&cooldownNotified&&refusedSets==1&&nativeSets==0,
        "no dev mode: H24 refused Set -> cvar float written, lease written=1 (R141 log had written=0)");
     ck(l.Apply(false,msg,sizeof(msg))&&near(cooldown.get(),.2f)&&std::strstr(msg,"restored=1")&&!cooldownNotified,"no dev mode: H24 restore gives 0.2 back");}
    {kcd2vr::focus_zoom58c2::Lease z;FocusZoomConsole58c2 io;auto r=z.step(true,1000,io);
     ck(r.ok&&near(focus.get(),0.f)&&z.owned,"no dev mode: book focus zoom applied=1 (R141 log applied=0)");
     r=z.step(false,2000,io);ck(r.ok&&near(focus.get(),30.f)&&!z.owned,"no dev mode: focus zoom baseline restored");}
    {DialogueCVarSnapshot s{};CaptureDialogueCVars(s);
     ck(ApplyDialogueMode(s,2)&&pov.get()==1.f&&noCam.get()==1.f,"no dev mode: dialogue mode 2 NoCameras=1 (plain int cvar written)");
     ck(ApplyDialogueMode(s,0)&&noCam.get()==0.f&&pov.get()==0.f,"no dev mode: dialogue OFF writes NoCameras back to 0");}
    {kcd2vr::dodge_window_d70::Lease d;CombatWindowsConsoleH3 io;kcd2vr::dodge_window_d70::SetFactor(1.25f);unsigned applied=0,restored=0;
     auto log=[&](const char*,const char* e,float,float){if(!std::strcmp(e,"APPLIED_GLOBAL"))++applied;if(!std::strcmp(e,"RESTORED"))++restored;};
     d.service(true,1000,io,log);ck(near(dodge.get(),.8125f)&&d.owns()&&applied==1,"no dev mode: D70 dodge window 0.65 -> 0.8125 (R141 F3 fight had no APPLIED row)");
     const unsigned sets=refusedSets;for(int i=1;i<20;++i)d.service(true,1000+i*250,io,log);ck(refusedSets==sets&&applied==1,"no dev mode: D70 settled, no 4/s write loop any more");
     d.service(false,7000,io,log);ck(near(dodge.get(),.65f)&&!d.owns()&&restored==1,"no dev mode: D70 restored when inactive");}
    {ActivityFade123R1 a;ck(a.Apply(true)&&herb.get()==0.f&&near(herbIn.get(),0.f)&&near(herbOut.get(),0.f)&&near(dig.get(),0.f)&&near(butcher.get(),0.f)&&near(alchemy.get(),0.f),
        "no dev mode: herb/digging/butchering faders 0 (int + float cvars), alchemy via Set()");
     ck(a.Apply(false)&&herb.get()==1.f&&near(herbIn.get(),.5f)&&near(dig.get(),1.f)&&near(butcher.get(),1.f)&&near(alchemy.get(),2.f),"no dev mode: faders restored");}

    // ---- 3. the R143 write never touches anything it must not
    {void* v=cone.raw;const float before=cone.get();std::memcpy(cone.raw+0x18,"\x04\0\0\0",4);
     // a normal (flags 4) cvar whose Set() did not take (callback clamp etc.) is never forced
     unsigned fl=0x4;std::memcpy(cone.raw+0x18,&fl,4);fl=0x40000004;std::memcpy(cone.raw+0x18,&fl,4);
     (void)CheatCVarSetFloat143(v,1.f);ck(near(cone.get(),before),"always-refused flag (0x40000000) on a normal cvar: not written");
     fl=0x4;std::memcpy(cone.raw+0x18,&fl,4);ck(!CheatCVarWrite143(v,1.f)&&near(cone.get(),before),"normal cvar (no cheat bit): direct write refused");
     fl=0x802;std::memcpy(cone.raw+0x18,&fl,4);ck(!CheatCVarWrite143(v,1.f)&&near(cone.get(),before),"read-only cheat cvar: not written");
     fl=0x4;std::memcpy(cone.raw+0x18,&fl,4);}
    {const float b=dodge.get();void* cb=&checks;std::memcpy(dodge.raw+0x38,&cb,8);ck(!CheatCVarWrite143(dodge.raw,2.f)&&near(dodge.get(),b),"change callback present: not written");
     cb=nullptr;std::memcpy(dodge.raw+0x38,&cb,8);void* fb=&checks;void* fe=reinterpret_cast<char*>(&checks)+8;std::memcpy(dodge.raw+0x20,&fb,8);std::memcpy(dodge.raw+0x28,&fe,8);
     ck(!CheatCVarWrite143(dodge.raw,2.f)&&near(dodge.get(),b),"functor list not empty: not written");
     fb=fe=nullptr;std::memcpy(dodge.raw+0x20,&fb,8);std::memcpy(dodge.raw+0x28,&fe,8);}
    {FakeCVar other("x_other",Other,2,1.f);table.push_back(&other);ck(!CheatCVarWrite143(other.raw,5.f)&&other.get()==1.f,"unknown cvar class: not written");table.pop_back();}
    {retail=false;const float b=cooldown.get();(void)CheatCVarSetFloat143(cooldown.raw,.5f);ck(near(cooldown.get(),b),"build not verified: not written");retail=true;}
    {const unsigned s=nativeSets;const float b=cooldown.get();ck(CheatCVarSetFloat143(cooldown.raw,b)&&near(cooldown.get(),b)&&nativeSets==s,"asked value already set: nothing changes");}
    {cooldown.f=.3f;const float b=cooldown.get();devMode=false;
     // a refused Set must not be confused with a foreign value: before == after == foreign value is only written when we asked a different value
     BlockCooldownLease59c17 l;(void)l.Apply(true,msg,sizeof(msg));ck(near(cooldown.get(),.001f),"no dev mode: lease takes any start value");(void)l.Apply(false,msg,sizeof(msg));ck(near(cooldown.get(),b),"restores that start value");}
    std::printf("cheat_cvar_write_r143: %u checks, %u failures\n",checks,failures);
    return failures?1:0;
}
