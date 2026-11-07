#include "vrik/combo_lineage19.h"
#include "vrik/combo_test_ds.h"
#include "subsystems/direct/combat_router_h3.h"
#include "diagnostics/outcome_chain23.h"
#include "vrik/contact_authority176.h"
#include "diagnostics/geometry_scope40c1.h"
#include "diagnostics/capture168.h"
#include "diagnostics/lab_state.h"
#include <array>
#include <atomic>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <map>
#include <optional>
#include <string>
#include <vector>
#include <thread>
#include "diagnostics/combo_observe.h"
#define EXCEPTION_EXECUTE_HANDLER 1
#ifndef _MSC_VER
#define __fastcall
#define __try try
#define __except(x) catch(...)
#endif
namespace DetourModKit::hook {struct Hook{
 bool live=true,failDisable=false,failEnable=false;unsigned disables=0;
 bool is_enabled()const noexcept{return live;}
 bool enable()noexcept{if(failEnable)return false;live=true;return true;}
 bool disable()noexcept{++disables;if(failDisable)return false;live=false;return true;}
};}
namespace cg=kcd2vr::combo_lineage19;
static std::uintptr_t g_base=0x180000000ull;
static std::uint64_t testReleaseSerial=1000;
static std::uint64_t now=1000;static bool weaponValid=true;static unsigned mode=3,timers=0;
static std::vector<int> managerCalls,triggerCalls;
static unsigned char* triggerMemory=nullptr;
static std::map<std::pair<std::uintptr_t,unsigned>,std::uintptr_t> links;
static const void* actor=reinterpret_cast<void*>(0x10000),*state=reinterpret_cast<void*>(0x11000),*player=reinterpret_cast<void*>(0x12000),*henryEntity=reinterpret_cast<void*>(0x13000);
static const void* manager=reinterpret_cast<void*>(0x14000),*targetA=reinterpret_cast<void*>(0x15000),*entityA=reinterpret_cast<void*>(0x16000),*actionA=reinterpret_cast<void*>(0x17000);
static const void* targetB=reinterpret_cast<void*>(0x18000),*entityB=reinterpret_cast<void*>(0x19000);
static std::uint64_t GetTickCount64(){return now;}
template<class T>static T Rd(const void* p,unsigned offset,T fallback){if(p==triggerMemory&&triggerMemory){T value;std::memcpy(&value,triggerMemory+offset,sizeof(value));return value;}auto it=links.find({reinterpret_cast<std::uintptr_t>(p),offset});if(it==links.end())return fallback;
 if constexpr(std::is_pointer_v<T>)return reinterpret_cast<T>(it->second);else return T(it->second);}
static void set(const void* p,unsigned offset,const void* value){links[{reinterpret_cast<std::uintptr_t>(p),offset}]=reinterpret_cast<std::uintptr_t>(value);}
static bool IsPlayer(const void* p){return p==henryEntity;}
static unsigned EntityId(const void* p){return p==entityA?11:p==entityB?22:0;}
static bool CombatActor(const void* p,const void*& e,const void*& s,bool){if(p==actor){e=henryEntity;s=state;return true;}if(p==targetA||p==targetB){e=p==targetA?entityA:entityB;s=reinterpret_cast<void*>(0x20000);return true;}return false;}
static std::uint64_t inputGenerationH10=1;
namespace kcd2vr::frik_pose_probe {bool ReadNativeComboSlotF1(const void*,subsystems::masterstrike_combos::NativeSlotF1& out) noexcept {out={};return false;}
std::uint64_t NativeComboInputGenerationH10() noexcept {return inputGenerationH10;}}
// This fixture executes the actual grace consumer below. Its upstream frame
// owner is an explicit identity-checked leaf, not a simulated contact or native
// hit. Production now asks for current intent ownership (H30), not the retired
// checked-collision owner. The full frame/recognizer adapter is a separate test.
static bool (*currentIntentFrameH30)(std::uint64_t,std::uint64_t,std::uint64_t,std::uint64_t)=nullptr;
static unsigned (*currentIntentStateFrameH30)(std::uint64_t,std::uint64_t,std::uint64_t,std::uint64_t)=nullptr;
namespace frik_pose_probe {static bool GeometryIntentOwnerH30(std::uint64_t w,std::uint64_t g,std::uint64_t b,std::uint64_t r){
 if(currentIntentFrameH30)return currentIntentFrameH30(w,g,b,r);
 return weaponValid&&w==2&&g==3&&b==4&&r==5;}
static unsigned GeometryIntentOwnerStateH30(std::uint64_t w,std::uint64_t g,std::uint64_t b,std::uint64_t r){
 return currentIntentStateFrameH30?currentIntentStateFrameH30(w,g,b,r):unsigned(GeometryIntentOwnerH30(w,g,b,r));}}
namespace geom27 {static std::atomic<bool> g_enabled{true},g_sync59c10r9{true};static unsigned ComboMode19(){return mode;}
 namespace vrstate596{static std::atomic<const void*> g_henry_actor{actor};}
 struct Result{bool seen=false,ok=false,fields=false;int process=-1;unsigned target=0;};static Result g_inject_result178r35;
}
static std::vector<std::string> traceH15;
static unsigned captureMutationH15=0;
namespace diagnostic_trace {static void Write(const char* line){traceH15.emplace_back(line);}}
static float expectedTimerH3=1.5f;
static void HostTimerStart(void*,float wanted,float){assert(std::fabs(wanted-expectedTimerH3)<.001f);++timers;}
static void __fastcall managerOriginal(void*,void*,void*,int);
static void __fastcall triggerOriginal(void*,void*,void*,int);
static bool SehCopy146(const void*,void*,std::size_t);
static bool (*extraSehCopyH10)(const void*,void*,std::size_t)=nullptr;
static unsigned installCalls=0,failInstallAt=0;static bool failNewManagerDisable=false;
template<class Fn,class Detour,std::size_t N>static bool Hook(std::optional<DetourModKit::hook::Hook>& h,Fn& original,std::uintptr_t,const unsigned char(&)[N],Detour,const char*){
 ++installCalls;if(installCalls==failInstallAt)return false;
 h.emplace();h->failDisable=installCalls==1&&failNewManagerDisable;
 original=installCalls==1?&managerOriginal:&triggerOriginal;return true;
}
#ifndef KCD2VR_COMBO_MID_HOST
namespace combo59c17 {static bool ArmUpdateContinuationH10() noexcept {return true;}
static bool DropUpdateContinuationH10() noexcept {return true;}}
#endif
#include "combo_lineage19.inc"
static bool SehCopy146(const void* p,void* out,std::size_t n){
 if(extraSehCopyH10&&extraSehCopyH10(p,out,n))return true;
 if(p==triggerMemory&&triggerMemory&&n==0x40){std::memcpy(out,p,n);return true;}
 const auto a=reinterpret_cast<std::uintptr_t>(p);
 if(a==g_base+0x911224&&n==sizeof(combo59c17::kTimerCallSite)){std::memcpy(out,combo59c17::kTimerCallSite,n);return true;}
 if(a==g_base+combo59c17::kTimerStart&&n==sizeof(combo59c17::kTimerStartEntry)){std::memcpy(out,combo59c17::kTimerStartEntry,n);return true;}return false;
}
static void __fastcall managerOriginal(void*,void*,void*,int phase){managerCalls.push_back(phase);}
static void __fastcall triggerOriginal(void*,void*,void*,int phase){triggerCalls.push_back(phase);
 // Explicit native leaf: eligible phase16 starts its own timer before returning.
 if(phase==16&&triggerMemory){std::uint64_t deadline=0;std::memcpy(&deadline,triggerMemory+0x28,8);++deadline;std::memcpy(triggerMemory+0x28,&deadline,8);}
 namespace capture=kcd2vr::diagnostics::capture168;
 if(captureMutationH15==1||captureMutationH15==2)capture::gate.select(capture::Domain::Combat,false,now);
 if(captureMutationH15==2)capture::gate.select(capture::Domain::Combat,true,now);
 if(captureMutationH15==3)kcd2vr::diagnostics::lab::Set(kcd2vr::diagnostics::lab::Key::F5,1);
 if(captureMutationH15==4)now=capture::gate.until[unsigned(capture::Domain::Combat)].load();
}
static cg::Owner owner(){cg::Owner r{};r.key={1,2,3,4,5,6,reinterpret_cast<std::uintptr_t>(player)};
 r.actor=reinterpret_cast<std::uintptr_t>(actor);r.state=reinterpret_cast<std::uintptr_t>(state);r.manager=reinterpret_cast<std::uintptr_t>(manager);
 r.target=reinterpret_cast<std::uintptr_t>(targetA);r.targetEntity=reinterpret_cast<std::uintptr_t>(entityA);r.targetId=11;r.action=reinterpret_cast<std::uintptr_t>(actionA);r.release=testReleaseSerial;
 r.modeGeneration=cg::grace.generation();r.mode=mode;r.tick=now;return r;}
static void setup(){++testReleaseSerial;links.clear();cg::grace.clear();cg::nativeSlot.retire();combo59c17::g_deferred_owner19={};managerCalls.clear();triggerCalls.clear();timers=0;now=1000;mode=3;weaponValid=true;
 geom27::g_enabled=geom27::g_sync59c10r9=true;geom27::g_inject_result178r35={};kcd2vr::contact176::recoveryEpoch55c1=6;
 set(actor,0x210,state);set(actor,0x1F8,player);set(actor,0x3B0,manager);set(state,0xCE0,actor);set(state,0xCF8,targetA);set(manager,8,actor);
 combo59c17::g_manager_original=&managerOriginal;combo59c17::g_trigger_original=&triggerOriginal;}
// CB2: a REAL native combo step of Henry keeps the VR combo window (fixture = combo_lineage19_test.cpp, actual
// production grace consumer extracted to combo_lineage19.inc). Reproduces Miki's 24 Oct 09:52:13 C06 chain:
// the continuation hit landed after its native phase16 (no slot), the old lease expired, idle reset the combo.
static std::uintptr_t managerStates[2]{0,0};
static bool ManagerStateCopy(const void* p,void* out,std::size_t n){
 if(reinterpret_cast<std::uintptr_t>(p)==reinterpret_cast<std::uintptr_t>(manager)+0xC0&&n==16){std::memcpy(out,managerStates,16);return true;}return false;}
static void __fastcall noOpenLeaf(void*,void*,void*,int phase){triggerCalls.push_back(phase);} // 909A90 false: 911195 returns
static void __fastcall nativeOpenLeaf(void* t,void*,void*,int phase){triggerCalls.push_back(phase);
 if(phase==16){auto* b=static_cast<unsigned char*>(t);b[0x18]=1;b[0x1B]=1;float d=.3f;std::memcpy(b+0x1C,&d,4);}}

namespace diagnostics=kcd2vr::diagnostics;
namespace nhp134 {namespace combo59c17=::combo59c17;}
namespace kcd2vr::hookcost178r22 {enum {ComboStep};struct Scope{Scope(int){}};struct Original{Original(int){}};}
namespace kcd2vr::attack_xi178r8 {struct HookTimer178r22{};struct OriginalTimer178r22{};}
static std::map<std::uint64_t,unsigned char> nativeMemory;
template<class T>static void nativePut(std::uint64_t address,const T& value){const auto* b=reinterpret_cast<const unsigned char*>(&value);for(unsigned i=0;i<sizeof(T);++i)nativeMemory[address+i]=b[i];}
static unsigned nativeCursor=1,nativeMutation=1,nativeOriginalCalls=0,diagnosticEmits=0;
static bool diagnosticRecord=false,clearDuringOriginal=false,clearDuringOpen=false;
static bool ComboRead6691(std::uint64_t address,void* out,std::size_t n)noexcept{
 const auto m=reinterpret_cast<std::uintptr_t>(manager);
 if(address==m+0xC0&&n==sizeof(diagnostics::combo::Vector)){
  diagnostics::combo::Vector v{managerStates[0],managerStates[1],0x50028};std::memcpy(out,&v,n);return true;}
 auto* bytes=static_cast<unsigned char*>(out);for(std::size_t i=0;i<n;++i){auto it=nativeMemory.find(address+i);if(it==nativeMemory.end())return false;bytes[i]=it->second;}return true;}
namespace frik_pose_probe {static const void* LastPlayerEntity(){return henryEntity;}}
static std::atomic<std::uintptr_t> g_combo_base6691{g_base};static unsigned g_combo_depth6691=0;
using ComboStepFn6691=void(__fastcall*)(void*,const std::int32_t*,const std::int32_t*);
static ComboStepFn6691 g_combo_step_original6691=nullptr;
struct ComboRequest6691{bool record=false;std::uint64_t callerRva=0;};
static ComboRequest6691 BeginCombo6691(void*){return {diagnosticRecord,0};}
static bool ComboLease176(const ComboRequest6691& q){return q.record;}
static void ClearReadableCache178r23(){}
static void EmitComboEpisode6691(const ComboRequest6691&,const char*,const diagnostics::combo::Episode&,bool,std::int32_t,bool,std::int32_t){++diagnosticEmits;}
static bool ReadCopy6689(const void* p,void* out,std::size_t n){return ComboRead6691(reinterpret_cast<std::uintptr_t>(p),out,n);}
#define KCD2VR_COMBO_CALLER() std::uintptr_t(0)
static void __fastcall nativeStep(void*,const std::int32_t*,const std::int32_t*){
 ++nativeOriginalCalls;
 if(nativeMutation==1){++nativeCursor;nativePut(0x50008,nativeCursor);}
 if(nativeMutation==2){managerStates[1]=managerStates[0];}
 if(nativeMutation==3){nativeCursor=0;nativePut(0x50008,nativeCursor);}
 if(clearDuringOriginal)combo59c17::ClearLineage19();
}
static void ResetNative(){nativeMemory.clear();nativeCursor=1;nativeMutation=1;nativeOriginalCalls=0;diagnosticEmits=0;diagnosticRecord=false;clearDuringOriginal=false;clearDuringOpen=false;
 const auto m=reinterpret_cast<std::uintptr_t>(manager),a=reinterpret_cast<std::uintptr_t>(actor),p=reinterpret_cast<std::uintptr_t>(player),e=reinterpret_cast<std::uintptr_t>(henryEntity);
 nativePut(m,g_base+0x3B0A9A0);nativePut(m+8,a);nativePut(a,g_base+0x3A80610);nativePut(a+0x1F8,p);nativePut(p,g_base+0x3AA4B98);nativePut(p+0x38,e);nativePut(e,g_base+0x4072C78);nativePut(e+0xC,std::uint32_t(77));
 nativePut(m+0xA8,diagnostics::combo::Vector{});nativePut(m+0xD8,std::uint32_t(0));nativePut(m+0xDC,std::uint32_t(0));nativePut(m+0xE8,std::uint8_t(0));
 nativePut(0x50008,nativeCursor);nativePut(0x50010,diagnostics::combo::Vector{0x60000,0x60018,0x60018});
 nativePut(0x60000,std::uint64_t(0x70000));nativePut(0x60008,std::uint32_t(3));nativePut(0x6000C,std::uint8_t(0));nativePut(0x60010,std::uint32_t(0));
 nativePut(0x70008,std::uint32_t(1));nativePut(0x70010,std::uint64_t(0));nativePut(0x70018,std::array<std::int32_t,4>{});nativePut(0x70028,std::array<std::uint8_t,16>{});nativePut(0x70040,diagnostics::combo::Vector{});
 set(player,0x38,henryEntity);g_combo_step_original6691=&nativeStep;cg::grace.publish(owner());
}

#include "combo_cb2_step_observer.inc"
static void DoStep(){ComboStepDetour6691(const_cast<void*>(manager),nullptr,nullptr);}
static void __fastcall invalidateOpen(void* t,void* a,void* b,int phase){nativeOpenLeaf(t,a,b,phase);if(clearDuringOpen)combo59c17::ClearLineage19();}
static int checks=0;static void ok(bool b,const char* m){++checks;if(!b){std::printf("FAIL %s\n",m);std::exit(1);}std::printf("PASS %s\n",m);}
int main(){
 extraSehCopyH10=&ManagerStateCopy;
 alignas(16) std::array<unsigned char,128> trigger{};
 const auto fresh=[&](){setup();trigger={};triggerMemory=trigger.data();std::memcpy(trigger.data()+8,&actor,sizeof(actor));
  combo59c17::g_trigger_original=&nativeOpenLeaf;managerStates[0]=0x50000;managerStates[1]=0x50028; // one live native combo state
  set(state,0x38,reinterpret_cast<void*>(std::uintptr_t(16)));combo59c17::g_cb2={};combo59c17::g_ready=true;combo59c17::g_teardown_pending19=false;ResetNative();
  combo59c17::TriggerPhaseDetour(trigger.data(),nullptr,nullptr,32);triggerCalls.clear(); // Henry's trigger is seen by the owner
 };
 fresh();now=5000;DoStep();
 ok(triggerCalls==std::vector<int>({16})&&trigger[0x18]==1&&timers==1,"late VR hit after phase16: the native phase16 open runs once, its timer gets the 1.5 s window");
 now=5340;combo59c17::ManagerPhaseDetour(const_cast<void*>(manager),nullptr,nullptr,2);
 ok(managerCalls.empty()&&combo59c17::g_cb2.deferred,"idle phase2 inside the step window: native reset held (was C5C9C7 at +343 ms)");
 combo59c17::TriggerPhaseDetour(trigger.data(),nullptr,nullptr,1);
 ok(triggerCalls==std::vector<int>({16}),"phase1 keeps the open slot inside the window");
 ok(combo59c17::KeepReadCB2(trigger.data(),actor,g_base+0x90874E,now),"slot update keeps the cleared hit read (no early C5B418 close)");
 ok(!combo59c17::KeepReadCB2(trigger.data(),actor,g_base+0x911200,now),"any other 909A90 caller stays native");
 set(state,0x38,reinterpret_cast<void*>(std::uintptr_t(4)));ok(!combo59c17::KeepReadCB2(trigger.data(),actor,g_base+0x90874E,now),"a new own attack keeps its native close");
 set(state,0x38,reinterpret_cast<void*>(std::uintptr_t(2)));
 now=5000+1501;combo59c17::ManagerPhaseDetour(const_cast<void*>(manager),nullptr,nullptr,4);
 ok(managerCalls==std::vector<int>({1,4}),"window expired: the held reset runs at the next attack start, then native phase4");
 // the next real step inside the window re-arms it (a chain of steps, each its own window)
 fresh();now=8000;DoStep();now=9400;DoStep();
 now=10800;combo59c17::ManagerPhaseDetour(const_cast<void*>(manager),nullptr,nullptr,2);
 ok(managerCalls.empty()&&triggerCalls==std::vector<int>({16})&&timers==2,"second real step 1.4 s later re-arms the same slot (no second open)");
 // refusals
 fresh();managerStates[1]=managerStates[0];now=5000;DoStep();
 ok(!combo59c17::g_cb2.until&&triggerCalls.empty()&&timers==0,"no live native combo state -> nothing armed");
 fresh();now=5000;DoStep();set(state,0xCF8,targetB);
 combo59c17::ManagerPhaseDetour(const_cast<void*>(manager),nullptr,nullptr,2);ok(managerCalls==std::vector<int>({2}),"opponent changed -> native reset");
 fresh();now=5000;DoStep();managerStates[1]=managerStates[0];
 combo59c17::ManagerPhaseDetour(const_cast<void*>(manager),nullptr,nullptr,2);ok(managerCalls==std::vector<int>({2}),"native states gone (finisher taken / reset) -> native");
 // CB3: the H50 10:37:28 chain. the native step is the hit-flag delegate, so in strike phase 8 the hit flag is real:
 // open the slot now (was: no reopen in phase 8 -> reopenedByNativePhase16=0 x18, next press 0.72 s later NO_SLOT).
 fresh();now=5000;set(state,0x38,reinterpret_cast<void*>(std::uintptr_t(8)));DoStep();
 ok(triggerCalls==std::vector<int>({16})&&trigger[0x18]==1&&timers==1&&combo59c17::g_cb2.until,"CB3 hit in strike phase8: native phase16 open runs at the hit-flag step, VR window armed");
 triggerCalls.clear();combo59c17::g_trigger_original=&noOpenLeaf;now=5200;set(state,0x38,reinterpret_cast<void*>(std::uintptr_t(16)));
 combo59c17::TriggerPhaseDetour(trigger.data(),nullptr,nullptr,16);
 ok(trigger[0x18]==1&&timers==1,"CB3 a later native phase16 with 909A90 false (911195 return) leaves the opened slot alone");
 combo59c17::g_trigger_original=&nativeOpenLeaf;combo59c17::TriggerPhaseDetour(trigger.data(),nullptr,nullptr,16);
 ok(trigger[0x18]==1&&timers==2,"CB3 a later native phase16 that re-opens with its 0.3 s native timer is re-armed to the VR window");
 now=5000+720;set(state,0x38,reinterpret_cast<void*>(std::uintptr_t(2)));combo59c17::ManagerPhaseDetour(const_cast<void*>(manager),nullptr,nullptr,2);
 ok(managerCalls.empty()&&combo59c17::KeepReadCB2(trigger.data(),actor,g_base+0x90874E,now)&&trigger[0x18]==1,"CB3 next press 0.72 s later: slot still open, idle reset held, cleared hit read kept");
 fresh();now=5000;set(state,0x38,reinterpret_cast<void*>(std::uintptr_t(4)));DoStep();
 ok(triggerCalls.empty()&&combo59c17::g_cb2.until,"CB3 hit flag during a NEW attack start (phase4): no replay, native phase16 of that attack decides");
 fresh();set(manager,8,targetA);now=5000;DoStep();
 ok(!combo59c17::g_cb2.until&&triggerCalls.empty(),"an NPC's combo manager is never touched");set(manager,8,actor);
 fresh();combo59c17::g_window_ms=0;now=5000;DoStep();
 ok(!combo59c17::g_cb2.until,"window 0 = off");combo59c17::g_window_ms=1500;
 fresh();now=5000;DoStep();combo59c17::TriggerPhaseDetour(trigger.data(),nullptr,nullptr,64);
 ok(!combo59c17::g_cb2.until,"Henry hit by the enemy (phase64) clears the window");

 // The original native callback always runs, even with diagnostics OFF.
 fresh();nativeMutation=0;now=5000;DoStep();
 ok(nativeOriginalCalls==1&&!combo59c17::g_cb2.until&&timers==0,"unchanged native cursor/candidates: no CB2 window earned; native original once without F3");
 fresh();nativeMutation=2;now=5000;DoStep();ok(nativeOriginalCalls==1&&!combo59c17::g_cb2.until,"native reset/pruning to empty never earns a step");
 fresh();nativeMutation=3;now=5000;DoStep();ok(!combo59c17::g_cb2.until,"native cursor moved backwards: no fresh step");
 fresh();diagnosticRecord=true;now=5000;DoStep();ok(nativeOriginalCalls==1&&diagnosticEmits==1&&timers==1,"F3 enabled still invokes native once and proves current progress");
 fresh();geom27::g_enabled=false;now=5000;DoStep();ok(nativeOriginalCalls==1&&!combo59c17::g_cb2.until,"disabled VR timing leaves native step unchanged");
 fresh();geom27::g_sync59c10r9=false;now=5000;DoStep();ok(nativeOriginalCalls==1&&!combo59c17::g_cb2.until,"unsynchronized route leaves native step unchanged");
 fresh();kcd2vr::contact176::recoveryEpoch55c1=7;now=5000;DoStep();ok(nativeOriginalCalls==1&&!combo59c17::g_cb2.until,"old recovery owner cannot extend native timing");
 fresh();weaponValid=false;now=5000;DoStep();ok(nativeOriginalCalls==1&&!combo59c17::g_cb2.until,"stale current physical intent owner cannot extend timing");
 fresh();mode=2;now=5000;DoStep();ok(nativeOriginalCalls==1&&!combo59c17::g_cb2.until,"changed native combat mode cannot inherit old owner");
 fresh();clearDuringOriginal=true;now=5000;DoStep();ok(nativeOriginalCalls==1&&!combo59c17::g_cb2.until&&timers==0,"control invalidation during native original retires old proof");
 fresh();now=5000;DoStep();const auto oldUntil=combo59c17::g_cb2.until,oldTrigger=combo59c17::g_cb2_trigger_seen;
 std::thread control([]{combo59c17::ClearLineage19();});control.join();
 ok(oldUntil&&combo59c17::g_cb2.until==oldUntil&&combo59c17::g_cb2_trigger_seen==oldTrigger,"control thread publishes atomic invalidation without touching native plain receipt or trigger");
 ok(!combo59c17::ActiveCB2(actor,now)&&!combo59c17::g_cb2.until&&!combo59c17::g_cb2_trigger_seen,"native callback retires receipt AND trigger after generation change");
 cg::grace.publish(owner());const auto oldTimers=timers;DoStep();
 ok(combo59c17::g_cb2.trigger==0&&timers==oldTimers,"new real step cannot reuse old trigger after control invalidation");
 fresh();now=5000;DoStep();kcd2vr::contact176::recoveryEpoch55c1=7;
 ok(!combo59c17::ActiveCB2(actor,now),"armed window rejects a subsequent recovery replacement");
 fresh();now=5000;DoStep();cg::grace.clear();ok(!combo59c17::ActiveCB2(actor,now),"mode generation retirement invalidates an armed CB2 owner");
 fresh();now=5000;clearDuringOpen=true;combo59c17::g_trigger_original=&invalidateOpen;DoStep();
 ok(nativeOriginalCalls==1&&timers==0&&!combo59c17::g_cb2.until,"control invalidation inside native phase16 open prevents timer continuation");
 // Compaction alone must not make a lower duplicate cursor look advanced.
 diagnostics::combo::Snapshot b{},a{};b.status=a.status=diagnostics::combo::Status::Ok;b.identity=a.identity={1,2,3,4,5};
 b.stateCount=b.statesCopied=2;a.stateCount=a.statesCopied=1;b.candidatesCopied=2;a.candidatesCopied=1;
 b.states[0].cursor=1;b.states[1].cursor=3;a.states[0].cursor=3;b.candidates[0].recipe=b.candidates[1].recipe=a.candidates[0].recipe=99;b.candidates[1].state=1;
 ok(!combo59c17::AdvancedStepCB2(b,a),"native prune/compaction with unchanged recipe maximum is not cursor advancement");
 a.states[0].cursor=4;ok(combo59c17::AdvancedStepCB2(b,a),"same exact recipe maximum advances across native compaction");
 a=b;a.stateCount=a.statesCopied=3;ok(combo59c17::AdvancedStepCB2(b,a),"native added live state is bounded positive progression");
 a.truncated=true;ok(!combo59c17::AdvancedStepCB2(b,a),"truncated state evidence cannot manufacture a progression receipt");
 std::printf("combo_step_window_cb2 %d/%d\n",checks,checks);return 0;
}
