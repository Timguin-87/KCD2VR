// Actual extracted C43T declaration and mod-thread service block. Native CVar
// IO and external feature flags are explicit fixture leaves.
#include "ragdoll_lease59.h"
#include "vrik/lab58.h"
#include "diagnostics/capture168.h"
#include <atomic>
#include <cmath>
#include <cstdio>
#include <string>
#include <unordered_map>
namespace diagnostic_fixture{
static unsigned optionalReads=0,searches=0,emitted=0,accepted=0;
static std::uint64_t now=0;
static std::string last;
static void reset(){optionalReads=searches=emitted=accepted=0;last.clear();}
}
namespace diagnostic_trace{static void Write(const char* text){
 ++diagnostic_fixture::emitted;diagnostic_fixture::last=text;
 if(kcd2vr::diagnostics::capture168::accept(text,diagnostic_fixture::now))++diagnostic_fixture::accepted;
}}
namespace kcd2vr::collab_h21{static bool npcPhysics(){return true;}}
namespace frik_pose_probe{static bool CombatExperiment175Enabled(){return true;}}
static unsigned reads=0,writes=0,checks=0,failed=0;
static std::unordered_map<std::string,float> values;
struct RagdollConsole59{
 bool Read(const char* n,float& v)noexcept{++reads;auto p=values.find(n);if(p==values.end())return false;v=p->second;return true;}
 bool Write(const char* n,float v)noexcept{++writes;values[n]=v;return true;}
};
#include "living_reaction_declarations_actual_d51.inc"
static void Service(bool consoleReady){
#include "living_reaction_service_actual_d51.inc"
}
// Actual optional observer body, with hand/native-candidate leaves counted.
// Functional DragTick and owner release are outside this observer in production.
namespace touch43{static bool g_latched=false;}
namespace gore32n{static std::atomic<bool> g_drive_fault33{false};}
namespace lab58n{
namespace L=kcd2vr::lab58;
static std::atomic<bool> g_dragFault{false};
static L::DragState g_drag[2]{};
struct DragDiag{double nearM=0;unsigned qualified=0,unqualified=0;const char* why="FIXTURE";};
static DragDiag g_dragDiag[2]{};
static bool DragUiOk(){++diagnostic_fixture::optionalReads;return true;}
static bool DragInputCurrentCg1(int,std::uint64_t){++diagnostic_fixture::optionalReads;return true;}
static bool ReadInteractionHandWorld123R1(int,float& x,float& y,float& z){++diagnostic_fixture::optionalReads;x=y=z=0;return true;}
static void DragSearchW5(int,std::uint64_t,const L::DragTuning&,float,float,float){++diagnostic_fixture::searches;}
static bool HandEmptyW5(int){++diagnostic_fixture::optionalReads;return true;}
static bool GripHeldW5(int){++diagnostic_fixture::optionalReads;return false;}
#include "physics_diagnostics_actual_d51.inc"
}
static void ck(bool b,const char* why){++checks;if(!b){++failed;std::printf("FAIL %s\n",why);}}
int main(){
 namespace lab=kcd2vr::lab58;
 values={{"ca_physicsProcessImpact",0.f},{"ca_UseRecoil",0.f},{"g_fallAndPlayThreshold",50.f}};
 const auto original=values;lab::rung=3;lab::livingHitImpact=lab::livingTouch=true;
 g_touch43=g_fallplay43=true;g_fallplay_threshold43=15.f;lab::impactLeaseOk=true;
 Service(true);
 ck(values==original&&writes==0,"actual mod service cannot change global native arrow/hit reaction settings");
 ck(reads==0,"actual service with no owned restore performs no native CVar lookup");
 ck(!lab::impactLeaseOk,"actual host publishes living impulse actuation unavailable");
 for(int i=0;i<100;++i)Service(true);
 ck(reads==0&&writes==0,"held F9/saved preferences never reassert global living settings");
 // Explicit historical ownership state, not a fresh guess of retail defaults.
 g_touch43=g_fallplay43=false;lab::livingHitImpact=lab::livingTouch=false;lab::rung=0;
 g_lease_impact43.owned=true;g_lease_impact43.original=0.f;g_lease_impact43.wrote=1.f;values["ca_physicsProcessImpact"]=1.f;
 g_lease_recoil43.owned=true;g_lease_recoil43.original=0.f;g_lease_recoil43.wrote=1.f;values["ca_UseRecoil"]=2.f;
 g_lease_fall43.owned=true;g_lease_fall43.original=50.f;g_lease_fall43.wrote=15.f;values["g_fallAndPlayThreshold"]=15.f;
 const auto before=writes;Service(true);
 ck(values["ca_physicsProcessImpact"]==0.f&&values["g_fallAndPlayThreshold"]==50.f&&writes==before+2,
    "actual host restores its exact owned impact and fall values");
 ck(values["ca_UseRecoil"]==2.f&&!g_lease_recoil43.owned,"foreign recoil value is preserved and ownership retired");
 const auto afterReads=reads,afterWrites=writes;Service(true);Service(false);
 ck(reads==afterReads&&writes==afterWrites,"settled retirement is quiet and cannot fight another writer");

 namespace cap=kcd2vr::diagnostics::capture168;
 namespace df=diagnostic_fixture;
 lab::rung=0;lab::f9Rung=1;df::reset();df::now=1000;
 lab58n::PhysicsDiagnosticsW5(df::now,1);
 ck(df::optionalReads==0&&df::searches==0&&df::emitted==0,"F9 observe without F3 performs no optional native reads or formatting");
 cap::gate.select(cap::Domain::Bug,true,2000);df::reset();df::now=2000;
 lab58n::PhysicsDiagnosticsW5(df::now,1);
 ck(df::optionalReads==0&&df::searches==0&&df::emitted==0,"F2 does not start the F3 physics observer");
 cap::gate.select(cap::Domain::Bug,false,2001);cap::gate.select(cap::Domain::Combat,true,3000);
 df::reset();df::now=3000+cap::Duration;
 lab58n::PhysicsDiagnosticsW5(df::now,1);
 ck(df::optionalReads==0&&df::searches==0&&df::emitted==0,"expired unpolled F3 performs no optional native reads");
 df::now+=1000;cap::gate.select(cap::Domain::Combat,true,df::now);df::reset();
 lab58n::PhysicsDiagnosticsW5(df::now,1);
 ck(df::optionalReads>0&&df::searches==2&&df::emitted==1&&df::accepted==1,
    "live F3 observes both candidate hands and reaches the actual F3 writer router");
 ck(df::last.find("livingActuation=PAUSED")!=std::string::npos,"observer does not advertise saved living selections as active");
 df::now+=100;df::reset();lab58n::PhysicsDiagnosticsW5(df::now,1);
 ck(df::optionalReads==0&&df::searches==0&&df::emitted==0,"live F3 preserves once-per-second observer cadence");
 df::now+=1000;cap::gate.select(cap::Domain::Combat,false,df::now);df::reset();
 lab58n::PhysicsDiagnosticsW5(df::now,3);
 ck(df::optionalReads==0&&df::searches==0&&df::emitted==0,"F3 stop also quiets the rung3 observer without touching drag ownership");
 std::printf("D51 actual living-reaction mod service and F3 observer %u/%u; native IO mocked\n",checks-failed,checks);
 return failed?1:0;
}
