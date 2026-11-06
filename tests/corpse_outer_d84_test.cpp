// The actual service Tick is included by the local fixture; the outer-flow
// extraction retains the real early-return branch, late service call, and
// nexus/frame dispatch statements. Unrelated combat work is a named leaf.
#define main corpse_local_main_d84
#include "corpse_mode_d84_test.cpp"
#undef main
static std::atomic<unsigned> g_stage_thread596{0};
static std::atomic<std::uint64_t> g_stage_tick596{0},g_native_busy_tick178{0};
static bool physicalCombat=false;
static unsigned tickCalls=0,combatWork=0,otherTail=0;
static bool stageWasCurrent=true;
static std::uint64_t GetTickCount64(){return now;}
static unsigned GetCurrentThreadId(){return 73;}
static bool PhysicalCombat177Enabled(){return physicalCombat;}
static void UpdateSmallPickupReaderR24(bool){}
static void SmallContactTickR24(std::uint64_t){}
namespace gore32n {
static bool ServiceVisualLeases59(std::uint64_t){return true;}
static void TouchFleshFill61(){}
static void Tick(std::uint64_t){++otherTail;}
}
namespace featureProbe59c6 {static void Tick(){}}
static void DrainVisibleFailure(std::uint64_t){}
static void DrainConstraintOps60(std::uint64_t){}
static bool GrabSourceCurrentD41(){return true;}
static void* EntityById6694(unsigned){return nullptr;}
namespace kcd2vr::physics::grab_producer60 {template<class T>static bool SetSourcePredicateD41(T){return true;}}
namespace kcd2vr::physics::grab_input60 {
static void* (*resolveEntity)(unsigned)=nullptr;
static void GameThreadTick(std::uint64_t){}
}
namespace kcd2vr::touch_react_service60 {static void (*fleshEffect)()=nullptr;static void drain(std::uint64_t){}}
static void ConsumeEnvironmentFx178r22c1(){}
static void OuterCombatWorkD84(){++combatWork;}
static void ImpalePartTick59c7g(){}
namespace lab_prop58n {static void Tick(std::uint64_t){++otherTail;}}
namespace blood49n {static void Tick(std::uint64_t){++otherTail;}}
namespace lab58n {
static void Tick(std::uint64_t stamp){
 ++tickCalls;stageWasCurrent=stageWasCurrent&&g_stage_thread596.load()==GetCurrentThreadId()&&g_stage_tick596.load()==stamp;
 ::Tick(stamp);
}
static void RetireUnservicedCg1(std::uint64_t stamp){::RetireUnservicedCg1(stamp);}
}
struct PerfScope101R2 {explicit PerfScope101R2(int){}};
#include "corpse_outer_source.inc"
static void outerReset(bool enabled){
 reset();physicalCombat=enabled;tickCalls=combatWork=otherTail=0;stageWasCurrent=true;
 g_stage_thread596=0;g_stage_tick596=0;g_native_busy_tick178=100;
}
int main(){
 corpse_local_main_d84();
 for(bool combat:{false,true}){
  outerReset(combat);L::corpseDrag=true;NexusFrameD84(0,0);
  check(tickCalls==1&&installs==1&&watchdogs==1&&drives==1,"D84 outer selected corpse serviced exactly once with physical combat either OFF or ON");
  check(stageWasCurrent,"D84 native outer stage owner published before corpse service");
  check(combatWork==(combat?1u:0u)&&otherTail==(combat?3u:0u),"unrelated combat/tail gates and enabled ordering preserved");
  check(lines.empty()&&!handReads&&!contextReads&&!searches,"outer selected gameplay adds no diagnostic work with recording OFF");
  L::corpseDrag=false;now+=2000;NexusFrameD84(0,0);
  check(tickCalls==2&&retirements==1&&drives==1,"D84 outer feature OFF reaches retirement without another drive");
 }
 outerReset(false);physicsOn();NexusFrameD84(0,0);
 check(tickCalls==1&&lines.size()==1&&handReads>0&&!installs&&!drives,"D84 F9-only observation works with physical combat OFF and does not enable gameplay");
 outerReset(false);NexusFrameD84(0,0);
 check(tickCalls==1&&!installs&&!drives&&!handReads&&!contextReads&&!searches&&lines.empty(),"idle OFF outer path performs no corpse reads or observer formatting");
 outerReset(false);L::corpseDrag=true;NexusFrameD84(1,0);NexusFrameD84(0,0x20u);
 check(!tickCalls&&!installs&&!drives&&!g_stage_thread596.load(),"nested and native-preview frames cannot duplicate service");
 std::printf("corpse outer: %d checks, %d failures\n",checks,failures);return failures?1:0;
}
