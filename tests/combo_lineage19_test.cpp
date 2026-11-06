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
int main(){
 assert(!kcd2vr::combo_test_ds::force.load()); // real production default OFF
 // Legacy lineage expiry cases below explicitly exercise the configured 1.5 s profile.
 setup();auto r=owner();assert(cg::grace.publish(r));combo59c17::ManagerPhaseDetour(const_cast<void*>(manager),nullptr,nullptr,1);assert(managerCalls.empty());
 // Target A -> B -> A inside the old window cannot reuse A's grace.
 set(state,0xCF8,targetB);combo59c17::ManagerPhaseDetour(const_cast<void*>(manager),nullptr,nullptr,1);assert(managerCalls==std::vector<int>({1}));
 set(state,0xCF8,targetA);combo59c17::ManagerPhaseDetour(const_cast<void*>(manager),nullptr,nullptr,1);assert(managerCalls.size()==2);
 setup();r=owner();assert(cg::grace.publish(r));
 set(state,0xCF8,targetB);combo59c17::ManagerPhaseDetour(const_cast<void*>(manager),nullptr,nullptr,4);
 set(state,0xCF8,targetA);combo59c17::ManagerPhaseDetour(const_cast<void*>(manager),nullptr,nullptr,1);
 assert(managerCalls==std::vector<int>({4,1})); // observing B at phase4 retires A even with no B idle callback
 setup();r=owner();assert(cg::grace.publish(r));combo59c17::ClearLineage19();geom27::g_sync59c10r9=false;geom27::g_sync59c10r9=true;
 combo59c17::ManagerPhaseDetour(const_cast<void*>(manager),nullptr,nullptr,1);assert(managerCalls.size()==1&&!cg::grace.publish(r));
 for(unsigned failure=0;failure<6;++failure){setup();r=owner();assert(cg::grace.publish(r));
  if(failure==0)weaponValid=false;
  if(failure==1)kcd2vr::contact176::recoveryEpoch55c1=7;
  if(failure==2)mode=1;
  if(failure==3)set(actor,0x3B0,reinterpret_cast<void*>(0x30000));
  if(failure==4)set(actor,0x210,reinterpret_cast<void*>(0x31000));
  if(failure==5)set(actor,0x1F8,reinterpret_cast<void*>(0x32000));
  combo59c17::ManagerPhaseDetour(const_cast<void*>(manager),nullptr,nullptr,2);assert(managerCalls==std::vector<int>({2}));
 }
 // Changed upstream intent identity cannot retain a native reset suppression.
 for(unsigned failure=0;failure<4;++failure){setup();r=owner();
  if(failure==0)++r.key.weapon;
  if(failure==1)++r.key.generation;
  if(failure==2)++r.key.body;
  if(failure==3)++r.key.request;
  assert(cg::grace.publish(r));
  combo59c17::ManagerPhaseDetour(const_cast<void*>(manager),nullptr,nullptr,2);
  assert(managerCalls==std::vector<int>({2}));cg::Owner withdrawn{};assert(!cg::grace.read(withdrawn));
 }
 setup();r=owner();assert(cg::grace.publish(r));combo59c17::ManagerPhaseDetour(const_cast<void*>(manager),nullptr,nullptr,1);now=2501;
 combo59c17::ManagerPhaseDetour(const_cast<void*>(manager),nullptr,nullptr,4);assert(managerCalls==std::vector<int>({1,4})); // deferred reset for same lineage at expiry
 setup();r=owner();auto next=r;next.key.gesture=2;assert(cg::sameLineage(r,next)); // next legitimate combo stroke is allowed
 // Real adapter trigger uses provisional grace only after the matching same-stack applied native hit.
 alignas(16) std::array<unsigned char,128> trigger{};triggerMemory=trigger.data();std::memcpy(trigger.data()+8,&actor,sizeof(actor));trigger[0x18]=trigger[0x19]=trigger[0x1B]=1;float native=.3f;std::memcpy(trigger.data()+0x1C,&native,4);
 {cg::OfferScope offer(&r);kcd2vr::geometry40c1::Scope nativeStack(actor,reinterpret_cast<void*>(1),targetA);
  combo59c17::TriggerPhaseDetour(trigger.data(),nullptr,nullptr,1);assert(triggerCalls.size()==1); // no observed native hit yet
  geom27::g_inject_result178r35={true,true,true,1,11};
  combo59c17::TriggerPhaseDetour(trigger.data(),nullptr,nullptr,16);assert(timers==1&&triggerCalls.size()==2);
  combo59c17::TriggerPhaseDetour(trigger.data(),nullptr,nullptr,1);assert(triggerCalls.size()==2);
 }
 combo59c17::TriggerPhaseDetour(trigger.data(),nullptr,nullptr,1);assert(triggerCalls.size()==2); // F1: the actual native-open/timed slot retains its finite anchor after the applied same-stack offer ends
 assert(cg::grace.publish(r));std::memcpy(trigger.data()+8,&targetB,sizeof(targetB));combo59c17::TriggerPhaseDetour(trigger.data(),nullptr,nullptr,1);
 cg::Owner saved{};assert(cg::grace.read(saved)); // an unrelated NPC callback cannot erase Henry's grace
 combo59c17::TriggerPhaseDetour(trigger.data(),nullptr,nullptr,64);assert(cg::grace.read(saved)); // NPC hit reaction likewise cannot clear Henry's lineage
 std::memcpy(trigger.data()+8,&actor,sizeof(actor));combo59c17::TriggerPhaseDetour(trigger.data(),nullptr,nullptr,64);
 assert(!cg::grace.read(saved)&&triggerCalls.back()==64); // actual native interruption remains native
 // H3 extends only an already-open native slot, with the same target/action
 // lineage and one native timer call. It does not create a recipe or hit flag.
 setup();r=owner();kcd2vr::routerH3::mode=2;expectedTimerH3=3.f;
 trigger={};triggerMemory=trigger.data();std::memcpy(trigger.data()+8,&actor,sizeof(actor));
 trigger[0x18]=trigger[0x19]=trigger[0x1B]=1;std::memcpy(trigger.data()+0x1C,&native,4);
 {cg::OfferScope offer(&r);kcd2vr::geometry40c1::Scope stack(actor,reinterpret_cast<void*>(1),targetA);
  geom27::g_inject_result178r35={true,true,true,1,11};
  combo59c17::TriggerPhaseDetour(trigger.data(),nullptr,nullptr,16);assert(timers==1);}
 now=3500;combo59c17::ManagerPhaseDetour(const_cast<void*>(manager),nullptr,nullptr,1);assert(managerCalls.empty());
 now=4001;combo59c17::ManagerPhaseDetour(const_cast<void*>(manager),nullptr,nullptr,1);assert(managerCalls==std::vector<int>({1}));
 kcd2vr::routerH3::mode=0;expectedTimerH3=1.5f;
 // Actual install rollback / teardown: a failed disable retains live handles and originals.
 setup();installCalls=0;failInstallAt=2;failNewManagerDisable=true;
 assert(!combo59c17::Install());assert(combo59c17::g_teardown_pending19&&combo59c17::g_manager_hook&&combo59c17::g_manager_original);
 assert(combo59c17::g_manager_hook->is_enabled()&&!combo59c17::g_ready);
 const auto callsBefore=installCalls;assert(!combo59c17::Install()&&installCalls==callsBefore);
 r=owner();assert(cg::grace.publish(r));combo59c17::ManagerPhaseDetour(const_cast<void*>(manager),nullptr,nullptr,1);
 assert(managerCalls==std::vector<int>({1})); // retained detour cannot suppress a native reset
 combo59c17::g_manager_hook->failDisable=false;combo59c17::Uninstall();
 assert(!combo59c17::g_teardown_pending19&&!combo59c17::g_manager_hook&&!combo59c17::g_manager_original&&!cg::grace.read(saved));
 installCalls=0;failInstallAt=0;failNewManagerDisable=false;assert(combo59c17::Install());
 combo59c17::g_trigger_hook->failDisable=true;combo59c17::Uninstall();
 assert(combo59c17::g_teardown_pending19&&combo59c17::g_trigger_hook&&combo59c17::g_trigger_original&&!combo59c17::g_manager_hook);
 r=owner();assert(cg::grace.publish(r));triggerCalls.clear();timers=0;
 combo59c17::TriggerPhaseDetour(trigger.data(),nullptr,nullptr,1);combo59c17::TriggerPhaseDetour(trigger.data(),nullptr,nullptr,16);
 assert(triggerCalls==std::vector<int>({1,16})&&timers==0);
 combo59c17::g_trigger_hook->failDisable=false;combo59c17::Uninstall();
 assert(!combo59c17::g_teardown_pending19&&!combo59c17::g_trigger_hook&&!combo59c17::g_trigger_original);
 // An already-disabled hook does not need another (possibly failing) disarm.
 combo59c17::g_manager_hook.emplace();combo59c17::g_manager_hook->live=false;combo59c17::g_manager_hook->failDisable=true;
 combo59c17::g_manager_original=&managerOriginal;combo59c17::Uninstall();assert(!combo59c17::g_manager_hook&&!combo59c17::g_manager_original);
 // Actual production phase callbacks under the independent F3 recorder.
 // Recorder edges do not change any native/lease/timer decision.
 namespace capture=kcd2vr::diagnostics::capture168;
 const auto containsH15=[](const char* text){unsigned n=0;for(const auto& line:traceH15)n+=line.find(text)!=std::string::npos;return n;};
 setup();r=owner();assert(cg::grace.publish(r));traceH15.clear();
 capture::gate.select(capture::Domain::Combat,false,now);
 combo59c17::ManagerPhaseDetour(const_cast<void*>(manager),nullptr,nullptr,1);
 assert(traceH15.empty()&&managerCalls.empty()); // existing gameplay retain, no diagnostic work
 capture::gate.select(capture::Domain::Combat,true,now);
 combo59c17::ManagerPhaseDetour(const_cast<void*>(manager),nullptr,nullptr,1);
 assert(containsH15("RETAIN_EXISTING_LINEAGE_SKIP_NATIVE_PHASE")==1&&managerCalls.empty());
 assert(containsH15("gesture=1 key=0000000000000002 generation=3 body=4 request=5 recovery=6")==1);
 assert(containsH15("target=11")==1&&containsH15("nativeAcceptance=UNKNOWN")==1);
 for(unsigned i=0;i<200;++i)combo59c17::ManagerPhaseDetour(const_cast<void*>(manager),nullptr,nullptr,1);
 assert(traceH15.size()==1&&managerCalls.empty()); // identical callback is not a per-frame log
 ++now;combo59c17::ManagerPhaseDetour(const_cast<void*>(manager),nullptr,nullptr,2);
 assert(containsH15("RETAIN_EXISTING_LINEAGE_SKIP_NATIVE_PHASE")==2&&managerCalls.empty());
 now=r.tick+1501;combo59c17::ManagerPhaseDetour(const_cast<void*>(manager),nullptr,nullptr,4);
 assert(containsH15("EXPIRED_DEFERRED_RESET_FORWARD_PLANNED")==1&&managerCalls==std::vector<int>({1,4}));
 // New capture must record its first edge even with the same owner/phase.
 capture::gate.select(capture::Domain::Combat,false,now);capture::gate.select(capture::Domain::Combat,true,now);
 combo59c17::ManagerPhaseDetour(const_cast<void*>(manager),nullptr,nullptr,4);
 assert(containsH15("NATIVE_PHASE_FORWARD_PLANNED")==2);
 setup();r=owner();trigger={};triggerMemory=trigger.data();std::memcpy(trigger.data()+8,&actor,sizeof(actor));
 capture::gate.select(capture::Domain::Combat,true,now);
 trigger[0x18]=trigger[0x19]=trigger[0x1B]=1;std::memcpy(trigger.data()+0x1C,&native,4);traceH15.clear();
 {cg::OfferScope offer(&r);kcd2vr::geometry40c1::Scope stack(actor,reinterpret_cast<void*>(1),targetA);
  geom27::g_inject_result178r35={true,true,true,1,11};
  combo59c17::TriggerPhaseDetour(trigger.data(),nullptr,nullptr,16);}
 assert(timers==1&&triggerCalls==std::vector<int>({16}));
 assert(containsH15("NATIVE_RETURN_TIMED_OPEN_OBSERVED")==1&&containsH15("EXISTING_TIMER_EXTENSION_RETURNED")==1);
 assert(containsH15("slotBefore{known=1 requested=1 timed=1")==2&&containsH15("slotSnapshots=BEFORE_OPTIONAL_EXTENSION")==2);
 combo59c17::TriggerPhaseDetour(trigger.data(),nullptr,nullptr,1);
 assert(containsH15("KEEP_EXISTING_SLOT_SKIP_NATIVE_PHASE")==1&&triggerCalls==std::vector<int>({16}));
 combo59c17::TriggerPhaseDetour(trigger.data(),nullptr,nullptr,64);
 assert(containsH15("NATIVE_PHASE_RETURNED")==1&&triggerCalls==std::vector<int>({16,64})&&!cg::grace.read(saved));
 for(const auto& line:traceH15)if(line.find("H15 F3_COMBO_PHASE ")!=std::string::npos){assert(line.size()<2047&&capture::domain(line.c_str())==capture::Domain::Combat);}
 for(unsigned mutation:{1u,2u,3u,4u}){
  setup();r=owner();capture::gate.select(capture::Domain::Combat,true,now);traceH15.clear();captureMutationH15=mutation;
  trigger={};std::memcpy(trigger.data()+8,&actor,sizeof(actor));trigger[0x18]=trigger[0x19]=trigger[0x1B]=1;std::memcpy(trigger.data()+0x1C,&native,4);
  {cg::OfferScope offer(&r);kcd2vr::geometry40c1::Scope stack(actor,reinterpret_cast<void*>(1),targetA);
   geom27::g_inject_result178r35={true,true,true,1,11};combo59c17::TriggerPhaseDetour(trigger.data(),nullptr,nullptr,16);}
  assert(triggerCalls==std::vector<int>({16})&&containsH15("H15 F3_COMBO_PHASE ")==0);
  captureMutationH15=0;kcd2vr::diagnostics::lab::Set(kcd2vr::diagnostics::lab::Key::F5,0);
 }
 capture::gate.select(capture::Domain::Combat,false,now);traceH15.clear();
 std::puts("PASS actual install rollback and teardown retry retain live hook/trampoline and native forwarding; actual combo grace consumers: target/manager/player/state/weapon/mode/recovery isolation, OFF-ON, expiry, same-stack native result, no forced advancement");
 std::puts("F3_COMBO_PHASE actualcallbacks: OFF quiet, unchanged native call/timer counts, exact existing owner IDs, repeated edge collapse, phase1/2/4/16/64, new-session first edge, current native-open/extension/terminal records PASS");
 return 0;
}
