// Source-connected camera lease/service. Native CVar IO, clock and published
// context are explicit substitutes; no game, renderer or headset is executed.
#include <atomic>
#include <array>
#include <vector>
#include <string>
#include <cstdio>
#include <cstdint>
#include <climits>
#include <cstring>
#include "subsystems/experimental/free_targeting_d42.h"
#include "subsystems/masterstrike_combos/finisher_diagnostics_d42.h"
#include "diagnostics/capture168.h"
#include "vrik/align_veto_receipts24.h"
namespace diagnostics=kcd2vr::diagnostics;
static std::uint64_t tick=1000;
static std::uint64_t GetTickCount64(){return tick;}
static bool retail=true,consoleReady=true,cvarReady=true,setReady=true,contextReady=true;
static int value=0,reads=0,writes=0,lookups=0,failReadNumber=0;
static std::vector<std::string> lines,keys;
static void* GetModuleHandleW(const wchar_t*){return reinterpret_cast<void*>(0x180000000ULL);}
namespace retail_build112 {static bool Ready(std::uintptr_t){return retail;}}
static void* SafeGetCryConsole(){return consoleReady?&value:nullptr;}
static void* SafeConsoleGetCVar(void*,const char* key){++lookups;keys.emplace_back(key);return cvarReady?&value:nullptr;}
static int SafeCVarGetInt(void*,int fallback){++reads;return reads==failReadNumber?fallback:value;}
static bool SafeCVarSetInt(void*,int v){++writes;if(setReady)value=v;return setReady;}
// R142: the refused-Set route (VF_CHEAT cvar, no dev mode) writes the cvar int. intRoute=false = not the IntRef vtable.
static bool intRoute=false;static int intWrites=0;
static bool CombatCameraIntWrite142(void*,int v){if(!intRoute)return false;++intWrites;value=v;return true;}
namespace diagnostic_trace {static void Write(const char* s){lines.emplace_back(s);}}
struct Log {void warning(const char*,const char*){}};
#ifndef KCD2VR_DIAG_LOG
#define KCD2VR_DIAG_LOG(...) ((void)0) // R133: the service now logs through the diag sink too
#endif
static Log logger;static Log& log(){return logger;}
namespace frik_pose_probe {
static bool CombatFreeViewContextD42(){return contextReady;}
static bool CombatNoTurnReadyD40(){return true;}
static bool CombatNoTurnD40(){return false;}
}
#include "camera_lease50_production.inc"
static void service(int rung=0,bool cameraOnly50=true){
#include "subsystems/experimental/free_targeting_service_d42.inl"
}
static unsigned checks=0,failures=0;
static void ck(bool x,const char* text){++checks;if(!x){++failures;std::printf("FAIL %s\n",text);}}
static unsigned prefix(const char* p){unsigned n=0;for(auto& l:lines)if(l.rfind(p,0)==0)++n;return n;}
static bool contains(const char* p){for(auto& l:lines)if(l.find(p)!=std::string::npos)return true;return false;}
int main(){
 namespace ft=kcd2vr::free_targeting_d42;namespace fd=kcd2vr::finisherdiag_d42;
 auto& gate=diagnostics::capture168::gate;using Domain=diagnostics::capture168::Domain;
 service();ck(writes==0,"disabled preference and F4 off perform no write");
 ft::set(true);service();ck(value==1&&ft::cameraLeased&&writes==1,"F4 OFF explicit mode actually leases camera CVar");
 ck(keys.size()==1&&keys[0]=="wh_cs_DisableCombatCamera","only existing camera switch is touched");
 const auto look=lookups;for(unsigned i=0;i<1000;++i)service();ck(writes==1&&lookups==look,"steady mode does no native CVar polling or writes");
 ck(prefix("D42_F3")==0,"F3 OFF service produces no per-frame combat diagnostics");
 contextReady=false;service();ck(value==0&&!ft::cameraLeased&&writes==2,"protected context restores original CVar");
 contextReady=true;service();ck(value==1&&writes==3,"safe context reacquires camera lease");
 value=3;ft::set(false);service();ck(value==3&&writes==3&&!ft::cameraLeased,"external CVar owner is never overwritten on release");
 value=0;retail=false;ft::set(true);service();ck(writes==3&&!ft::cameraLeased,"retail mismatch makes zero native writes");
 ft::set(false);service();retail=true;cvarReady=false;ft::set(true);service();ck(writes==3&&!ft::cameraLeased,"missing CVar cannot claim successful ownership");
 ft::set(false);service();cvarReady=true;setReady=false;ft::set(true);service();ck(writes==4&&value==0&&!ft::cameraLeased,"failed native write is reported unleased");
 ft::set(false);service();setReady=true;
 service(1);ck(value==1,"OFF new feature retains legacy F4 camera selection");service(0);ck(value==0,"legacy F4 off restores lease");
 ft::set(true);service();gate.select(Domain::Combat,true,tick);service();
 ck(!gate.active(Domain::Bug,tick)&&prefix("D42_F3 ")==1,"F3 starts independently of F2");
 ft::recordRotation(true,0);ft::recordRotation(false,kcd2vr::align24::BodyRequired);
 fd::Budget budget;unsigned admitted=0;for(int i=0;i<12;++i)admitted+=budget.take(gate.session[unsigned(Domain::Combat)],tick);
 ck(admitted==8&&fd::seen==12&&fd::written==8&&fd::suppressed==4,"finisher detail budget reports suppression instead of silently exhausting");
 tick+=1000;service();ck(contains("calls=2 vetoed=1")&&contains("BODY_REQUIRED:1"),"F3 summary reports observed dispatch decisions and refusal reason");
 ck(contains("finisherSeen=12 finisherWritten=8 finisherSuppressed=4"),"F3 summary retains suppressed detailed candidate count");
 gate.select(Domain::Combat,false,tick);service();ck(prefix("D42_F3_END ")==1,"F3 stop emits final session record");
 for(int i=0;i<10;++i)service();ck(prefix("D42_F3_END ")==1,"F3 stop final is not repeated");
 gate.select(Domain::Combat,true,tick);service();gate.select(Domain::Combat,true,tick);service();ck(prefix("D42_F3_END ")==2,"new F3 session retires previous before rebasing");
 tick+=diagnostics::capture168::Duration;service();ck(prefix("D42_F3_END ")==3,"F3 expiry emits final count even after recorder closes");
 ck(budget.take(99,tick),"new session restores finite finisher budget");
 ft::set(false);service();ck(value==0&&!g_combat_camera50.applied,"final explicit OFF releases original camera value");
 // Failed readback cannot erase the restore obligation. These use the
 // exact extracted production lease, with deterministic native IO failures.
 CombatCameraLease50 uncertain;char msg[320]{};value=0;
 failReadNumber=reads+2;uncertain.Apply(true,msg,sizeof(msg));
 ck(value==1&&!uncertain.applied&&uncertain.restoreOwed&&uncertain.pending,"successful setter with missing readback retains original restoration obligation");
 failReadNumber=0;uncertain.Apply(false,msg,sizeof(msg));
 ck(value==0&&!uncertain.restoreOwed,"OFF restores even if acquisition was never readback-verified");
 CombatCameraLease50 delayed;value=0;failReadNumber=reads+2;delayed.Apply(true,msg,sizeof(msg));failReadNumber=0;
 const int w0=writes;tick+=100;delayed.Apply(true,msg,sizeof(msg));
 ck(delayed.applied&&writes==w0&&delayed.original==0,"bounded acquisition retry verifies without rewriting or rebasing original");
 setReady=false;delayed.Apply(false,msg,sizeof(msg));ck(value==1&&delayed.pending&&delayed.restoreOwed,"failed restoration remains pending");
 const int w1=writes;for(int i=0;i<1000;++i)delayed.Apply(false,msg,sizeof(msg));ck(writes==w1,"pending restore does not poll every frame");
 setReady=true;tick+=100;delayed.Apply(false,msg,sizeof(msg));ck(value==0&&!delayed.restoreOwed,"pending restoration succeeds on bounded retry");
 CombatCameraLease50 foreign;value=0;foreign.Apply(true,msg,sizeof(msg));setReady=false;foreign.Apply(false,msg,sizeof(msg));
 value=5;setReady=true;tick+=100;const int w2=writes;foreign.Apply(false,msg,sizeof(msg));
 ck(value==5&&writes==w2&&!foreign.restoreOwed,"foreign writer during pending restore keeps its value");
 CombatCameraLease50 bounded;value=0;bounded.Apply(true,msg,sizeof(msg));setReady=false;bounded.Apply(false,msg,sizeof(msg));const int w3=writes;
 for(int i=0;i<20;++i){tick+=100;bounded.Apply(false,msg,sizeof(msg));}
 ck(writes==w3+3&&bounded.pending&&bounded.retries==3,"failed restoration retries at most three times then reports pending");
 setReady=true;bounded.Restore();ck(value==0&&!bounded.restoreOwed,"explicit shutdown restoration gets a fresh bounded attempt");
 // R142: Set() refused on the VF_CHEAT cvar (no dev mode): the cvar int is written, lease and give-back both;
 // dev mode (Set works) never takes that route; a refusal with no IntRef cvar stays unleased as before.
 ck(intWrites==0,"R142 Set() that works never writes the cvar int");
 CombatCameraLease50 cheat;value=0;setReady=false;intRoute=true;cheat.Apply(true,msg,sizeof(msg));
 ck(value==1&&cheat.applied&&cheat.restoreOwed&&cheat.direct&&intWrites==1&&std::strstr(msg,"write=CVAR_INT")!=nullptr,"R142 refused Set: cvar int written, lease owned and verified");
 cheat.Apply(false,msg,sizeof(msg));ck(value==0&&!cheat.applied&&!cheat.restoreOwed&&intWrites==2,"R142 refused give-back: original written back through the int");
 CombatCameraLease50 cheatForeign;value=0;cheatForeign.Apply(true,msg,sizeof(msg));value=7;tick+=100;cheatForeign.Apply(false,msg,sizeof(msg));
 ck(value==7&&intWrites==3,"R142 a foreign value is never overwritten by the int route");
 intRoute=false;CombatCameraLease50 noRoute;value=0;noRoute.Apply(true,msg,sizeof(msg));ck(value==0&&!noRoute.applied&&!noRoute.restoreOwed&&!noRoute.direct,"R142 refused Set without the IntRef route stays unleased");
 setReady=true;intRoute=false;
 std::printf("D42 actual camera lease/service: %u checks, %u failures; host IO substitutes; native/headset NOT_RUN\n",checks,failures);return failures?1:0;
}
