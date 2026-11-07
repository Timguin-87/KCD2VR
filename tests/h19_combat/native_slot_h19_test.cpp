#define NOMINMAX
extern "C" __declspec(dllimport) void __stdcall RaiseException(unsigned long,unsigned long,unsigned long,const unsigned long long*);
#pragma comment(lib,"kernel32.lib")
#include "subsystems/direct/combat_router_h3.h"
// Connected production query + original-first trigger/manager callbacks. Only
// retail memory, frame publications, tracking and native leaves are fixtures.
#include "subsystems/masterstrike_combos/lineage_policy.h"
#include "subsystems/masterstrike_combos/native_combo_query_f1.h"
#include "vrik/contact_handoff59c3.h"
#include "diagnostics/outcome_chain23.h"
#include "diagnostics/geometry_scope40c1.h"
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
 bool live=true;bool is_enabled()const noexcept{return live;}
 bool disable()noexcept{live=false;return true;}
};}
namespace diagnostic_trace {static std::vector<std::string> lines;static void Write(const char* s){lines.emplace_back(s);}}
static std::uint64_t now=1000,serial=100,deadline=10000;
static unsigned timers=0,queries=0;static float timerSeconds=0;
static std::vector<int> managerCalls,triggerCalls;
static std::map<std::uintptr_t,std::vector<unsigned char>> memory;
static std::vector<std::uintptr_t> reads;
static constexpr std::uintptr_t actor=0x10000,state=0x11000,player=0x12000,henryEntity=0x13000,action=0x14000,
 helper=0x15000,clockOwner=0x16000,actionManager=0x17000,row=0x18000,type=0x19000,gate=0x20000,slots=0x21000,
 itemManager=0x22000,comboManager=0x23000,target=0x24000,targetEntity=0x25000,targetState=0x26000,
 targetB=0x27000,entityB=0x28000;
static bool nativeOpen=true,nativeTimerChanges=true,weaponValid=true,timerFault=false;
static unsigned snapshotReads=0,failSnapshotAt=0;static void (*insideOriginal)()=nullptr;
alignas(16) static std::array<unsigned char,128> trigger;
static void* ptr(std::uintptr_t p){return reinterpret_cast<void*>(p);}
template<class T>static void put(std::uintptr_t p,T value){std::vector<unsigned char> v(sizeof(value));std::memcpy(v.data(),&value,sizeof(value));memory[p]=v;}
static bool readMemory(std::uintptr_t p,void* out,std::size_t size){reads.push_back(p);
 const auto tr=reinterpret_cast<std::uintptr_t>(trigger.data());
 if(p>=tr&&p+size<=tr+trigger.size()){std::memcpy(out,reinterpret_cast<void*>(p),size);return true;}
 auto it=memory.find(p);if(it==memory.end()||it->second.size()!=size)return false;std::memcpy(out,it->second.data(),size);return true;}
template<class T>static T Rd(const void* p,unsigned offset,T fallback){T value{};return readMemory(reinterpret_cast<std::uintptr_t>(p)+offset,&value,sizeof(value))?value:fallback;}
static std::uint64_t GetTickCount64(){return now;}
static void HostTimerStart(void* timer,float wanted,float jitter){assert(timer==trigger.data()+0x28&&jitter==0);++timers;timerSeconds=wanted;if(timerFault)RaiseException(0xE000D5F6,0,0,nullptr);}
static std::uintptr_t g_base=0;
namespace animation_filter{static std::atomic<bool> nativeBodyOwned112{false};}
namespace kcd2vr::frik_pose_probe {
namespace ac50=kcd2vr::attackclock50;
static std::atomic<bool> selected{true},g_gameplay_allowed{true},g_ctx_menu{false},g_ctx_dialogue{false},g_ctx_cutscene{false},g_ctx_scripted{false};
static std::atomic<bool> g_head_valid{true},g_right_valid{true},g_left_valid{true};
static std::atomic<std::uint64_t> g_attack_timing_policy22{1},g_pose_epoch6689{4},g_weapon_request_epoch6694{5},g_phys_ready_sample6698{9};
static std::atomic<std::uintptr_t> g_motion_clock_actor50{actor};
static std::atomic<void*> g_last_player_entity{ptr(henryEntity)};
static kcd2vr::core::SnapshotMailbox<ac50::Binding> g_motion_clock_binding50;
struct PhysFrame6698 {
 struct Input{std::uint64_t sample=9,tick=1000,bodyEpoch=4;bool allowed=true,attacks=true,coherentMelee119=true,tracked[2]{true,true};}input;
 struct Output{bool active[2]{true,true},owned[2]{true,true};}output;
 struct Profile{std::uint64_t key=2,generation=3;bool isMelee=true;bool melee()const{return isMelee;}}profiles[2];
 void* player=ptr(::player);void* manager=ptr(itemManager);std::uint64_t request=5;
};
static kcd2vr::core::SnapshotMailbox<PhysFrame6698> g_phys_frame6698;
static PhysFrame6698 frame{};
struct TrackingSnapshot{bool allowed=true,head_valid=true,right_valid=true,left_valid=true,head_held=false,right_held=false,left_held=false;std::uint64_t tick=1000;};
static TrackingSnapshot tracking{};static bool trackingRead=true,handRead=true;static std::uint64_t handKey=2;
static bool ReadTrackingSnapshot(TrackingSnapshot& out){out=tracking;return trackingRead;}
static bool CombatSynchronized59c10r9(){return selected.load();}
static bool MotionClockSelected50(){return selected.load();}
static std::uintptr_t WhBase(){return g_base;}
static bool ClockRead50(std::uintptr_t p,void* out,std::size_t n){return readMemory(p,out,n);}
static bool NativeHandKey6697(void* p,int side,std::uint64_t& out){out=handKey;return handRead&&p==ptr(itemManager)&&side==0;}
static bool GeometryOwnerValid178r30(std::uint64_t key,std::uint64_t generation,std::uint64_t epoch,std::uint64_t request){
 return weaponValid&&key==frame.profiles[0].key&&generation==frame.profiles[0].generation&&epoch==g_pose_epoch6689&&request==g_weapon_request_epoch6694&&
 frame.input.allowed&&g_gameplay_allowed&&!g_ctx_menu&&!g_ctx_dialogue&&!animation_filter::nativeBodyOwned112&&
 contact176::fresh(frame.input.tick,now,80);}
static std::atomic<float> g_native_hit_time59c17{.3f},g_weapon_stat_effect59c17{1};static std::atomic<bool> g_ms_keep_stance59c17{false};
#include "subsystems/masterstrike_combos/native_flow_controls.inl"
}
namespace frik_pose_probe=kcd2vr::frik_pose_probe;
namespace fp=kcd2vr::frik_pose_probe;
namespace cg=kcd2vr::combo_lineage19;
namespace geom27 {static std::atomic<bool> g_enabled{true},g_sync59c10r9{true};static unsigned mode=3;
 static unsigned ComboMode19(){return mode;}
 namespace vrstate596{static std::atomic<const void*> g_henry_actor{ptr(actor)};}
 struct Result{bool seen=false,ok=false,fields=false;int process=-1;unsigned target=0;};static Result g_inject_result178r35;
 static std::atomic<std::uint64_t> g_combo_flags59c17{0};
}
static bool IsPlayer(const void* p){return p==ptr(henryEntity);}
static unsigned EntityId(const void* p){return p==ptr(targetEntity)?11:p==ptr(entityB)?22:0;}
static bool CombatActor(const void* p,const void*& e,const void*& s,bool){
 if(p==ptr(actor)){e=ptr(henryEntity);s=ptr(state);return true;}
 if(p==ptr(target)||p==ptr(targetB)){e=p==ptr(target)?ptr(targetEntity):ptr(entityB);s=ptr(targetState);return true;}return false;}
static void __fastcall managerOriginal(void*,void*,void*,int phase){managerCalls.push_back(phase);}
static void __fastcall triggerOriginal(void*,void*,void*,int phase){triggerCalls.push_back(phase);
 if(phase==16&&insideOriginal){auto leaf=insideOriginal;insideOriginal=nullptr;leaf();}
 if(phase==16&&nativeOpen){trigger[0x18]=trigger[0x1B]=1;float seconds=.3f;std::memcpy(trigger.data()+0x1C,&seconds,4);
  if(nativeTimerChanges){++deadline;std::memcpy(trigger.data()+0x28,&deadline,8);}}
 if(phase==1||phase==4||phase==64){trigger[0x18]=trigger[0x19]=trigger[0x1B]=0;}}
static bool SehCopy146(const void* p,void* out,std::size_t n){if(p!=trigger.data()||n!=0x40)return false;
 ++snapshotReads;if(snapshotReads==failSnapshotAt)return false;std::memcpy(out,p,n);return true;}
template<class Fn,class Detour,std::size_t N>static bool Hook(std::optional<DetourModKit::hook::Hook>&,Fn&,std::uintptr_t,const unsigned char(&)[N],Detour,const char*){return false;}
#include "subsystems/masterstrike_combos/native_combo_grace.inl"
static kcd2vr::attackclock50::Binding binding;
static kcd2vr::contact176::Ticket ticket;
static void currentFrame(){fp::frame.input.tick=now;fp::tracking.tick=now;assert(fp::g_phys_frame6698.publish(fp::frame));}
static void setup(){
 g_base=reinterpret_cast<std::uintptr_t>(&HostTimerStart);g_base-=0x9D6A34ull;
 ++serial;now=1000;timers=0;timerSeconds=0;managerCalls.clear();triggerCalls.clear();memory.clear();reads.clear();trigger={};nativeOpen=nativeTimerChanges=weaponValid=true;
 snapshotReads=failSnapshotAt=0;insideOriginal=nullptr;timerFault=false;combo59c17::g_timer_fault_h19=false;kcd2vr::routerH3::mode=0;
 cg::grace.clear();cg::nativeSlot.retire();combo59c17::g_deferred_owner19={};combo59c17::g_window_ms=1500;
 combo59c17::g_teardown_pending19=false;combo59c17::g_manager_original=&managerOriginal;combo59c17::g_trigger_original=&triggerOriginal;
 geom27::g_enabled=geom27::g_sync59c10r9=true;geom27::mode=3;geom27::g_inject_result178r35={};geom27::vrstate596::g_henry_actor=ptr(actor);
 fp::selected=fp::g_gameplay_allowed=fp::g_head_valid=fp::g_right_valid=fp::g_left_valid=true;
 kcd2vr::motion59c4::selected=false;
 fp::g_ctx_menu=fp::g_ctx_dialogue=fp::g_ctx_cutscene=fp::g_ctx_scripted=false;animation_filter::nativeBodyOwned112=false;
 fp::g_attack_timing_policy22=1;fp::g_pose_epoch6689=4;fp::g_weapon_request_epoch6694=5;fp::g_phys_ready_sample6698=9;
 fp::g_motion_clock_actor50=actor;fp::g_last_player_entity=ptr(henryEntity);fp::trackingRead=fp::handRead=true;fp::handKey=2;
 fp::frame={};fp::tracking={};currentFrame();kcd2vr::contact176::recoveryEpoch55c1=6;
 binding={};auto& i=binding.intent;i.player=player;i.actor=actor;i.state=state;i.manager=itemManager;i.serial=serial+1000;
 i.policy=1;i.epoch=4;i.request=5;i.sample=7;i.tick=1000;i.key=2;i.generation=3;i.kind=kcd2vr::attack_timing178r22c2::Kind::Slash;i.side=0;i.speed=4;i.gesture59c3=serial+500;i.recovery54=6;
 binding.action=action;binding.helper=helper;binding.clock=clockOwner;binding.manager=actionManager;binding.row=row;binding.entity=henryEntity;
 assert(fp::g_motion_clock_binding50.publish(binding));
 ticket={};ticket.player=player;ticket.entity=henryEntity;ticket.serial=serial;ticket.tick=1000;ticket.sample=7;ticket.epoch=4;ticket.request=5;ticket.key=2;ticket.generation=3;ticket.policy=1;ticket.gesture59c3=i.gesture59c3;ticket.recovery59c3=6;
 assert(kcd2vr::contact59c3::releases.publish(ticket));
 put(actor,g_base+0x3A80610);put(actor+0x1F8,player);put(player,g_base+0x3AA4B98);put(player+0x38,henryEntity);
 put(actor+0x210,state);put(state+0xCE0,actor);put(actor+0x200,actionManager);put(actor+0x3C0,clockOwner);put(clockOwner+8,actor);
 put(actionManager+0x38,slots);put(slots,action);put(action,g_base+0x3A61C58);put(action+0x3C,int(0));put(action+0x40,actionManager);
 put(action+0x28,std::uint8_t(1));put(action+0x70,actor);put(action+0xA0,helper);put(action+0x58,row);
 put(helper,g_base+0x3A61040);put(helper+8,actor);put(helper+0x20,action+0x88);put(action+0x88,g_base+0x3A61440);put(clockOwner+0x14,std::uint8_t(1));
 put(row+0x148,type);put(type+8,int(0));put(actor+0x220,gate);put(gate+0x28,std::uint8_t(0));put(state+0x38,int(16));
 put(actor+0x3B0,comboManager);put(comboManager+8,actor);put(state+0xCF8,target);
 std::memcpy(trigger.data()+8,&actor,8);
}
static cg::Owner acceptedOwner(){cg::Owner o{};o.key={ticket.gesture59c3,2,3,4,5,6,player};o.actor=actor;o.state=state;o.manager=comboManager;
 o.target=target;o.targetEntity=targetEntity;o.targetId=11;o.action=action;o.release=ticket.serial;o.modeGeneration=cg::grace.generation();o.mode=geom27::mode;o.tick=now;return o;}
static void phase(int p){combo59c17::TriggerPhaseDetour(trigger.data(),nullptr,nullptr,p);}
static void managerPhase(int p){combo59c17::ManagerPhaseDetour(ptr(comboManager),nullptr,nullptr,p);}
int main(){
 // A real native timer fault is session-sticky even after a settings reload.
 setup();timerFault=true;phase(16);
 assert(timers==1&&combo59c17::g_timer_fault_h19&&combo59c17::WindowMsH3()==0);
 cg::SlotLeaseF1 faultLease{};std::uint64_t faultConsumed=0;assert(!cg::nativeSlot.read(faultLease,faultConsumed));
 timerFault=false;combo59c17::g_window_ms=1500;kcd2vr::routerH3::mode=2;
 phase(16);assert(timers==1&&combo59c17::WindowMsH3()==0);
 managerPhase(1);assert(managerCalls==std::vector<int>({1}));
 puts("PASS actual native timer fault: setting reload/router cannot rearm; native manager forwards");
 setup();kcd2vr::subsystems::masterstrike_combos::NativeSlotF1 observed{};
 assert(fp::ReadNativeComboSlotF1(ptr(actor),observed)&&observed.release.serial==serial&&observed.action==action);
 phase(16);assert(timers==1&&timerSeconds>1.499f&&timerSeconds<1.501f&&triggerCalls==std::vector<int>({16}));
 managerPhase(1);phase(1);assert(managerCalls.empty()&&triggerCalls.size()==1); // slot before accepted hit
 now=2000;currentFrame();auto hit=acceptedOwner();assert(cg::grace.publish(hit));phase(16);assert(timers==1); // hit/duplicate cannot refresh
 now=2500;currentFrame();assert(combo59c17::Active(ptr(actor),now));
 now=2501;currentFrame();assert(!combo59c17::Active(ptr(actor),now));phase(16);assert(timers==1);
 managerPhase(4);assert(managerCalls==std::vector<int>({1,4})); // expired deferred native reset, then original phase4
 phase(4);assert(triggerCalls.back()==4&&trigger[0x18]==0);
 // Minimum requested 500ms and the exact inclusive expiry boundary.
 setup();combo59c17::g_window_ms=500;phase(16);assert(timers==1&&timerSeconds>.499f&&timerSeconds<.501f);
 now=1500;currentFrame();assert(combo59c17::Active(ptr(actor),now));now=1501;currentFrame();assert(!combo59c17::Active(ptr(actor),now));
 // Original refused slot, stale flags, NPC and OFF never borrow a release or timer.
 setup();nativeOpen=false;phase(16);assert(timers==0&&triggerCalls.size()==1);
 trigger[0x18]=trigger[0x1B]=1;float old=.3f;std::memcpy(trigger.data()+0x1C,&old,4);phase(16);assert(timers==0); // unchanged flags/deadline are not this open
 setup();std::memcpy(trigger.data()+8,&targetB,8);phase(16);assert(timers==0&&triggerCalls.size()==1);
 setup();fp::selected=false;phase(16);assert(timers==0); // DIRECT/no admitted native clock: honest native passthrough
 setup();kcd2vr::motion59c4::selected=true;phase(16);assert(timers==0&&combo59c17::g_last_query_f1==2);
 setup();geom27::g_enabled=false;phase(16);assert(timers==0);
 setup();combo59c17::g_window_ms=0;phase(16);assert(timers==0);
 // Native phase4 and phase64 must always close; interruption spends this release.
 setup();phase(16);phase(4);assert(triggerCalls==std::vector<int>({16,4})&&trigger[0x18]==0);
 phase(64);assert(triggerCalls.back()==64&&!combo59c17::Active(ptr(actor),now));phase(16);assert(timers==1);
 // Target excursion or explicit lifecycle clear cannot resurrect old release.
 setup();phase(16);put(state+0xCF8,targetB);managerPhase(1);put(state+0xCF8,target);phase(16);assert(timers==1&&!combo59c17::Active(ptr(actor),now));
 setup();phase(16);combo59c17::ClearLineage19();phase(16);assert(timers==1);
 // Fresh next release is allowed; NPC callbacks cannot retire Henry's lease.
 setup();phase(16);std::memcpy(trigger.data()+8,&targetB,8);phase(64);std::memcpy(trigger.data()+8,&actor,8);assert(combo59c17::Active(ptr(actor),now));
 ++serial;++binding.intent.serial;binding.intent.gesture59c3++;ticket.serial=serial;ticket.gesture59c3=binding.intent.gesture59c3;
 assert(fp::g_motion_clock_binding50.publish(binding)&&kcd2vr::contact59c3::releases.publish(ticket));phase(16);assert(timers==2);
 // The independent observer cannot accidentally relax the shared phase4/8 clock writer.
 kcd2vr::attackclock50::View view{};assert(!kcd2vr::attackclock50::inspect(binding,g_base,readMemory,view));
 // Unknown preimages cannot convert a refused old flags/deadline into an open.
 setup();failSnapshotAt=1;trigger[0x18]=trigger[0x1B]=1;nativeOpen=false;phase(16);assert(timers==0&&triggerCalls.size()==1);
 setup();failSnapshotAt=2;phase(16);assert(timers==0&&triggerCalls.size()==1);
 // Native original can call other native work: never attribute its opening
 // across nested cancellation, teardown, a replaced admission or a new body.
 setup();insideOriginal=[](){phase(64);};phase(16);assert(timers==0&&triggerCalls==std::vector<int>({16,64}));
 setup();insideOriginal=[](){combo59c17::g_teardown_pending19=true;};phase(16);assert(timers==0);
 setup();insideOriginal=[](){++binding.intent.serial;assert(fp::g_motion_clock_binding50.publish(binding));};phase(16);
 assert(fp::ReadNativeComboSlotF1(ptr(actor),observed)&&timers==0); // still a valid NEW admission, wrong outer opening
 setup();insideOriginal=[](){
  const auto old=action,newAction=action+0x500;
  for(auto offset:{0u,0x3Cu,0x40u,0x28u,0x70u,0xA0u,0x58u,0x88u})memory[newAction+offset]=memory[old+offset];
  put(helper+0x20,newAction+0x88);put(slots,newAction);binding.action=newAction;++binding.intent.serial;
  ++serial;ticket.serial=serial;ticket.gesture59c3=++binding.intent.gesture59c3;
  assert(fp::g_motion_clock_binding50.publish(binding)&&kcd2vr::contact59c3::releases.publish(ticket));};
 phase(16);assert(fp::ReadNativeComboSlotF1(ptr(actor),observed)&&observed.action==action+0x500&&timers==0);
 setup();insideOriginal=[](){
  fp::g_pose_epoch6689=5;fp::frame.input.bodyEpoch=5;binding.intent.epoch=5;ticket.epoch=5;
  ++serial;ticket.serial=serial;ticket.gesture59c3=++binding.intent.gesture59c3;++binding.intent.serial;currentFrame();
  assert(fp::g_motion_clock_binding50.publish(binding)&&kcd2vr::contact59c3::releases.publish(ticket));};
 phase(16);assert(fp::ReadNativeComboSlotF1(ptr(actor),observed)&&timers==0);
 // Original eligibility can use trigger+38 override. It must match the same
 // current opponent both before and after, even if original clears/changes it.
 setup();std::memcpy(trigger.data()+0x38,&targetB,8);insideOriginal=[](){std::memset(trigger.data()+0x38,0,8);};phase(16);assert(timers==0);
 setup();insideOriginal=[](){std::memcpy(trigger.data()+0x38,&targetB,8);};phase(16);assert(timers==0);
 // Full current-native lifetime gates and fresh frame/tracking evidence.
 for(unsigned fail=0;fail<33;++fail){setup();switch(fail){
  case 0:put(slots,action+0x100);break;case 1:put(action,g_base+1);break;case 2:put(action+0x3C,1);break;
  case 3:put(action+0x28,std::uint8_t(0));break;case 4:put(action+0x40,actionManager+1);break;
  case 5:put(action+0x70,actor+1);break;case 6:put(action+0xA0,helper+1);break;case 7:put(helper+0x20,action+1);break;
  case 8:put(action+0x88,g_base+1);break;case 9:put(type+8,2);break;case 10:put(gate+0x28,std::uint8_t(1));break;
  case 11:put(state+0x38,8);break;case 12:fp::g_attack_timing_policy22=2;break;case 13:fp::g_pose_epoch6689=5;break;
  case 14:fp::g_weapon_request_epoch6694=6;break;case 15:kcd2vr::contact176::recoveryEpoch55c1=7;break;
  case 16:now=2501;currentFrame();break;case 17:fp::frame.input.tick=919;assert(fp::g_phys_frame6698.publish(fp::frame));break;
  case 18:fp::tracking.tick=919;break;case 19:fp::frame.input.allowed=false;currentFrame();break;
  case 20:fp::frame.input.coherentMelee119=false;currentFrame();break;case 21:fp::frame.output.owned[0]=false;currentFrame();break;
  case 22:fp::frame.profiles[0].key=9;currentFrame();break;case 23:fp::handKey=9;break;case 24:fp::g_ctx_menu=true;break;
  case 25:fp::g_ctx_dialogue=true;break;case 26:fp::g_ctx_cutscene=true;break;case 27:animation_filter::nativeBodyOwned112=true;break;
  case 28:fp::tracking.right_held=true;break;case 29:weaponValid=false;break;case 30:fp::g_motion_clock_actor50=actor+1;break;
  case 31:ticket.gesture59c3++;assert(kcd2vr::contact59c3::releases.publish(ticket));binding.intent.gesture59c3+=100;assert(fp::g_motion_clock_binding50.publish(binding));break;
  case 32:put(actor+0x210,state+1);break;}
  reads.clear();assert(!fp::ReadNativeComboSlotF1(ptr(actor),observed)&&observed.action==0);phase(16);assert(timers==0);
  if(fail==0){for(auto p:reads)assert(p<action||p>=action+0x100);} // membership before borrowed action reads
 }
 // Revoke established lease with every current owner/lifetime change, then
 // restore the original identity: its release remains terminal.
 for(unsigned fail=0;fail<7;++fail){setup();phase(16);switch(fail){
  case 0:weaponValid=false;break;case 1:geom27::mode=1;break;case 2:kcd2vr::contact176::recoveryEpoch55c1=7;break;
  case 3:put(actor+0x3B0,comboManager+1);break;case 4:put(actor+0x210,state+1);break;case 5:fp::g_ctx_menu=true;break;
  case 6:fp::frame.profiles[0].generation=4;currentFrame();break;}
  assert(!combo59c17::Active(ptr(actor),now));weaponValid=true;geom27::mode=3;kcd2vr::contact176::recoveryEpoch55c1=6;
  put(actor+0x3B0,comboManager);put(actor+0x210,state);fp::g_ctx_menu=false;fp::frame.profiles[0].generation=3;currentFrame();
  phase(16);assert(timers==1&&!combo59c17::Active(ptr(actor),now));
 }
 std::puts("PASS actual F1 native query/trigger/manager: original-first native-open before hit, finite500/1500ms, no refresh/rearm, native close/interruption, NPC/OFF and33 stale gates,7 revocation returns; ac50 phase4/8 unchanged");
}
