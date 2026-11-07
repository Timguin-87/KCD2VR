// Actual native Tick and observer bodies; native reads/writes are named leaves.
#include "vrik/lab58.h"
#include "diagnostics/capture168.h"
#include <atomic>
#include <cstdio>
#include <cstdint>
#include <string>
#include <vector>
namespace L=kcd2vr::lab58;
namespace cap=kcd2vr::diagnostics::capture168;
static std::uint64_t now=10000;
static unsigned handReads=0,contextReads=0,searches=0,installs=0,watchdogs=0,drives=0,retirements=0;
static std::vector<std::string> lines;
static std::vector<cap::Domain> domains;
namespace diagnostic_trace {
static void Write(const char* text){lines.emplace_back(text);}
static void WriteFor(cap::Domain d,const char* text){domains.push_back(d);Write(text);}
}
namespace touch43 {static bool g_latched=false;}
namespace gore32n {static std::atomic<bool> g_drive_fault33{false};}
static std::atomic<bool> g_dragFault{false};
static bool g_labServiceDirtyD80=false;
static L::DragState g_drag[2];
static struct Diag {double nearM=-1;unsigned qualified=0,unqualified=0;const char* why="NONE";} g_dragDiag[2];
struct PhysFrame6698{};
static struct Frame {bool read(PhysFrame6698&){return false;}} g_phys_frame6698;
static bool DragUiOk(){++contextReads;return true;}
static bool DragInputCurrentCg1(int,std::uint64_t){++handReads;return true;}
static bool SavedHandIntentD72(int,float& x,float& y,float& z){++handReads;x=y=z=0;return true;}
static bool HandEmptyW5(int){++handReads;return true;}
static bool GripHeldW5(int){++handReads;return false;}
static void DragSearchW5(int,std::uint64_t,const L::DragTuning&,float,float,float){++searches;}
static void RetireUnservicedCg1(std::uint64_t){++retirements;}
static void SavedWatchdogD72(std::uint64_t){++watchdogs;}
static void InstallSavedDragD72(){++installs;}
static void RememberLiving(const PhysFrame6698&){}
static void LivingTick(std::uint64_t){}
static void DragTick(std::uint64_t){++drives;}
static void SavedProviderDiagnosticsD84(std::uint64_t){}
#include "corpse_mode_source.inc"
static int failures=0,checks=0;
static void check(bool ok,const char* name){++checks;if(!ok){++failures;std::printf("FAIL %s\n",name);}}
static void reset(){
 now+=2000;handReads=contextReads=searches=installs=watchdogs=drives=retirements=0;lines.clear();domains.clear();
 L::corpseDrag=false;L::rung=0;L::f9Rung=0;L::livingHitImpact=false;
 cap::gate.select(cap::Domain::Combat,false,now);
#ifdef KCD2VR_TEST_PHYSICS_D84
 cap::gate.select(cap::Domain::Physics,false,now);
#endif
}
static void physicsOn(){
 L::f9Rung=1;
#ifdef KCD2VR_TEST_PHYSICS_D84
 cap::gate.select(cap::Domain::Physics,true,now);
#endif
}
int main(){
 reset();L::corpseDrag=true;Tick(now);
 check(installs==1&&watchdogs==1&&drives==1,"D84 selected corpse serviced with lab and recorders OFF");
 check(lines.empty()&&handReads==0&&contextReads==0&&searches==0,"gameplay selection adds no diagnostic reads with recorders OFF");
 reset();Tick(now);check(!installs&&!watchdogs&&!drives&&!handReads&&!contextReads&&!searches&&lines.empty(),"feature OFF and recorder OFF no hand/native/diagnostic work");
 reset();physicsOn();Tick(now);
 check(lines.size()==1&&handReads>0,"D84 F9 alone produces physics refusal/status without F3");
 check(!installs&&!drives,"F9 recording alone does not enable corpse gameplay");
#ifdef KCD2VR_TEST_PHYSICS_D84
 check(domains.size()==1&&domains[0]==cap::Domain::Physics,"F9 receipt explicitly routed to Physics");
#endif
 reset();cap::gate.select(cap::Domain::Combat,true,now);Tick(now);
 check(lines.size()==1&&handReads>0&&!installs&&!drives,"F3 legacy physics observation independent of feature selection");
 reset();physicsOn();cap::gate.select(cap::Domain::Combat,true,now);cap::gate.select(cap::Domain::Combat,false,now);Tick(now);
 check(lines.size()==1,"stopping F3 preserves independently enabled F9");
 reset();L::corpseDrag=true;physicsOn();Tick(now);check(installs==1&&drives==1&&lines.size()==1,"recording and gameplay can be combined");
 const auto previousDrives=drives;lines.clear();handReads=contextReads=searches=0;now+=cap::Duration;
 Tick(now);check(drives==previousDrives+1&&lines.empty()&&!handReads&&!contextReads&&!searches,"recorder expiry stops observer without disabling corpse gameplay");
 std::printf("corpse mode: %d checks, %d failures\n",checks,failures);return failures?1:0;
}
