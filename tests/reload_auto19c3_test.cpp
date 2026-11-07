#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <string>
#include <vector>
#include "vrik/physical_weapon.h"
namespace ph=kcd2vr::physical;
static unsigned checks=0;
static void ck(bool ok,const char* why){++checks;if(!ok){std::fprintf(stderr,"FAIL %s\n",why);std::exit(1);}}
static int playerMemory=0,otherPlayer=0,managerMemory=0,equipmentMemory=0,otherEquipment=0,componentMemory=0;
struct PhysFrame6698 {void* player=nullptr;void* manager=nullptr;ph::Input input{};ph::Profile profiles[2]{};std::uint64_t request=1;};
static PhysFrame6698 frame;
static bool experiment=true,retail=true,fault=false,haveFrame=true,haveItems=true,handKnown=true;
static std::uint64_t now=1000,liveKey=20;
static unsigned frameReads=0,mutateAt=0,itemsReads=0,keyReads=0,callerReads=0;
struct FrameBox {bool read(PhysFrame6698& f){++frameReads;if(mutateAt==frameReads)++frame.input.sample;f=frame;return haveFrame;}};
static FrameBox g_phys_frame6698;
static std::atomic<std::uint64_t> g_phys_ready_sample6698{9},g_weapon_request_epoch6694{1},g_pose_epoch6689{1};
static std::atomic<bool> g_reload_ready36{true},g_manual_bow_fault596{false};
static std::atomic<std::uint64_t> g_reload_generation36{1};
static std::uint64_t GetTickCount64(){return now;}
static bool StagedBowEnabled59(){return experiment;}
static bool ManualPlayer596(void* p){return retail&&p==&playerMemory;}
static bool NativeHandKey6697(void* m,int side,std::uint64_t& key){++keyReads;key=liveKey;return handKnown&&m==&managerMemory&&side==1;}
struct ManualItems596 {void* equipment=nullptr;void* component=nullptr;std::uint64_t weaponKey=0;};
static ManualItems596 items;
static void (*onItemsRead32)()=nullptr;
static bool ManualItemsRead596(void*,ManualItems596& out){++itemsReads;out=items;if(onItemsRead32)onItemsRead32();return haveItems;}
enum class ReloadReg36 {Rcx,Rbx,Rdi,Rdx,R8};
// rbx/rdi are the CALLER frame's callee-saved registers at 18468BC entry.
struct ReloadContext36 {std::uintptr_t receiver=0,sp=0,ip=0,rbx=0,rdi=0,rdx=1,r8=0;};
namespace DetourModKit::hook {static std::uintptr_t stack_pointer(ReloadContext36& c){return c.sp;}static std::uintptr_t& instruction_pointer(ReloadContext36& c){return c.ip;}}
static std::uintptr_t ReloadRegValue36(ReloadContext36& c,ReloadReg36 r){
 switch(r){case ReloadReg36::Rbx:return c.rbx;case ReloadReg36::Rdi:return c.rdi;case ReloadReg36::Rdx:return c.rdx;case ReloadReg36::R8:return c.r8;default:return c.receiver;}}
static bool callerReadable=true;static std::uintptr_t caller=0x2AAA012;
// receiptReads35 counts every NON-return-address memory read (caller context).
static unsigned receiptReads35=0;
static std::map<std::uintptr_t,std::uint64_t> mem;
static std::uintptr_t WhBase(){return 0;}
template<class T> static bool Read(void* p,T& out){const auto a=reinterpret_cast<std::uintptr_t>(p);
 if(a==8){++callerReads;out=T(caller);return callerReadable;}
 ++receiptReads35;auto it=mem.find(a);if(it==mem.end())return false;out=T(it->second);return true;}
static bool henryActor=true;
static ReloadContext36 ctxNext;static bool useCtx=false;
static bool ReloadHenryActor37(std::uintptr_t actor) noexcept {return henryActor&&actor==reinterpret_cast<std::uintptr_t>(&playerMemory);}
static void DeferredReload596(void*,void*) noexcept {}
static bool recorder=false;static bool ReloadActive36(){return recorder;}
namespace diagnostic_trace {static std::vector<std::string> notes;static void (*onWrite32)()=nullptr;
static void Write(const char* s){notes.emplace_back(s);if(onWrite32)onWrite32();}}
static bool ManualAutomaticFault32() noexcept {return g_manual_bow_fault596.load();}
#include "generated_reload_auto19c3.inc"
static void reset(){
 experiment=retail=haveFrame=haveItems=handKnown=callerReadable=true;recorder=false;g_manual_bow_fault596=false;
 g_reload_ready36=true;g_phys_ready_sample6698=9;g_weapon_request_epoch6694=1;g_pose_epoch6689=1;
 now=1000;liveKey=20;frameReads=mutateAt=itemsReads=keyReads=callerReads=receiptReads35=0;mem.clear();useCtx=false;ctxNext={};henryActor=true;g_reload_callers37=true;diagnostic_trace::notes.clear();diagnostic_trace::onWrite32=nullptr;++g_reload_generation36;onItemsRead32=nullptr;caller=0x2AAA012;frame=PhysFrame6698{};frame.player=&playerMemory;frame.manager=&managerMemory;
 frame.request=1;frame.input.sample=9;frame.input.bodyEpoch=1;frame.input.tick=1000;frame.input.allowed=true;
 frame.profiles[1].key=20;frame.profiles[1].generation=1;frame.profiles[1].classId=9;items={&equipmentMemory,&componentMemory,20};
}
static bool invoke(void* p=&equipmentMemory){ReloadContext36 c{reinterpret_cast<std::uintptr_t>(p),8,0};if(useCtx){c=ctxNext;c.receiver=reinterpret_cast<std::uintptr_t>(p);c.sp=8;c.ip=0;}
 const auto before=c;const auto memBefore=mem;
 ReloadAutomaticRequest19c3(c);ck(c.receiver==before.receiver&&c.sp==before.sp&&c.rbx==before.rbx&&c.rdi==before.rdi&&c.rdx==before.rdx&&c.r8==before.r8,"callback preserves native arguments/registers/stack");
 ck(mem==memBefore,"callback never writes native memory (no +20 latch or stage invention)");
 return c.ip==reinterpret_cast<std::uintptr_t>(&DeferredReload596);}
// ---- C37 exact caller expansion fixtures --------------------------------
static int ctlMemory[16],closureMemory[8];
static const std::uintptr_t CTL=reinterpret_cast<std::uintptr_t>(&ctlMemory),CLO=reinterpret_cast<std::uintptr_t>(&closureMemory);
static const std::uintptr_t EQ=reinterpret_cast<std::uintptr_t>(&equipmentMemory),PL=reinterpret_cast<std::uintptr_t>(&playerMemory);
static void lambdaCaller(std::uintptr_t ret,std::uint64_t vft){caller=ret;useCtx=true;ctxNext={};ctxNext.rbx=CLO;ctxNext.rdi=0xD1;
 mem[CLO]=vft;mem[CLO+8]=CTL;mem[CLO+0x10]=EQ;mem[CTL+0x18]=PL;mem[CTL+0x20]=1;}
static void tickCaller(){caller=0x224A38C;useCtx=true;ctxNext={};ctxNext.rbx=EQ;ctxNext.rdi=CTL;mem[CTL+0x18]=PL;mem[CTL+0x20]=1;}
static bool has(const char* t){return !diagnostic_trace::notes.empty()&&diagnostic_trace::notes.back().find(t)!=std::string::npos;}
static void c37(){
 struct Allowed {const char* name;void(*setup)();const char* kind;unsigned reads;};
 const Allowed allowed[]{
  {"bow_primary press 1F24831->1F24836",+[](){lambdaCaller(0x1F24836,0x3EC9610);},"callerKind=BOW_PRIMARY_1F24836",5},
  {"bow_secondary press 1F24699->1F2469E",+[](){lambdaCaller(0x1F2469E,0x3EC97D0);},"callerKind=BOW_SECONDARY_1F2469E",5},
  {"per-tick retry 224A387->224A38C",+[](){tickCaller();},"callerKind=TICK_RETRY_224A38C",2}};
 for(const auto& a:allowed){
  reset();a.setup();ck(invoke(),a.name);ck(receiptReads35==a.reads&&itemsReads==1,"allowed caller reads only its pinned context, then one owner admission");
  reset();recorder=true;a.setup();ck(invoke(),a.name);
  ck(has("stage=AUTO_RELOAD_DEFERRED")&&has("reason=ACCEPT")&&has(a.kind)&&has("context=1")&&has("pendingLatch=1")&&has("latchWrites=NONE"),"deferred receipt names exact caller/controller/latch");
  // released trigger: native +20 latch already cleared by release/context leave
  reset();recorder=true;a.setup();mem[CTL+0x20]=0;ck(!invoke(),"released trigger (+20==0) keeps native reload");
  ck(has("reason=TRIGGER_RELEASED")&&has("pendingLatch=0")&&itemsReads==0,"released trigger refused before owner admission");
  reset();recorder=true;a.setup();mem.erase(CTL+0x20);ck(!invoke()&&has("reason=CONTROLLER_OWNER"),"unreadable latch keeps native");
  // controller owned by another actor (NPC bow controller with same args)
  reset();recorder=true;a.setup();henryActor=false;ck(!invoke()&&has("reason=CONTROLLER_OWNER"),"non-Henry controller keeps native");
  reset();recorder=true;a.setup();mem[CTL+0x18]=0x1234;ck(!invoke()&&has("reason=CONTROLLER_OWNER"),"controller actor mismatch keeps native");
  // arguments must be the automatic-shot request dl=1,r8b=0 from 2AAA4FB
  reset();recorder=true;a.setup();ctxNext.rdx=0;ck(!invoke()&&has("reason=CALLER_ARGS"),"dl=0 request keeps native");
  reset();recorder=true;a.setup();ctxNext.r8=1;ck(!invoke()&&has("reason=CALLER_ARGS"),"r8b=1 explicit request keeps native");
  reset();a.setup();ctxNext.rdx=0x7700000001;ck(invoke(),"only dl/r8b bytes are argument bits");
  // exact pin failure disables only the expansion
  reset();recorder=true;a.setup();g_reload_callers37=false;ck(!invoke()&&has("reason=CALLER_PINS")&&has("callerPins=0")&&receiptReads35==0,"pin mismatch keeps native with no context reads");
  // manual mode off: native default, no caller-context read at all
  reset();recorder=true;a.setup();experiment=false;ck(!invoke()&&has("reason=MANUAL_MODE_OFF")&&receiptReads35==0&&itemsReads==0,"manual mode off keeps native auto reload and reads nothing");
  // equipment/context changes after the press
  reset();recorder=true;a.setup();items.equipment=&otherEquipment;ck(!invoke()&&has("reason=EQUIPMENT_OWNER"),"equipment replaced keeps native");
  reset();recorder=true;a.setup();items.weaponKey=21;ck(!invoke()&&has("reason=WEAPON_KEY"),"different bow keeps native");
  reset();recorder=true;a.setup();frame.request=2;ck(!invoke(),"stale request/context keeps native");
  reset();recorder=true;a.setup();frame.profiles[1].classId=10;ck(!invoke(),"non-bow (crossbow/gun profile) keeps native even at a bow site");
  reset();recorder=true;a.setup();onItemsRead32=+[](){++frame.profiles[1].generation;};ck(!invoke()&&has("reason=OWNER_CHANGED"),"owner change during admission keeps native");
  reset();recorder=true;a.setup();g_manual_bow_fault596=true;ck(!invoke()&&has("reason=MANUAL_FAULT"),"manual fault keeps native");
  reset();a.setup();ck(!invoke(&otherEquipment),"request for another equipment keeps native");
 }
 // closure relation details (bow press lambdas)
 reset();recorder=true;lambdaCaller(0x1F2469E,0x3EC9610);ck(!invoke()&&has("reason=CALLER_CONTEXT"),"secondary site with primary functor vtable keeps native");
 reset();recorder=true;lambdaCaller(0x1F24836,0x3EC9760);ck(!invoke()&&has("reason=CALLER_CONTEXT"),"crossbow functor at bow return keeps native");
 reset();recorder=true;lambdaCaller(0x1F24836,0x3EC9610);mem[CLO+0x10]=reinterpret_cast<std::uintptr_t>(&otherEquipment);ck(!invoke()&&has("reason=CALLER_CONTEXT"),"captured equipment != request equipment keeps native");
 reset();recorder=true;lambdaCaller(0x1F24836,0x3EC9610);ctxNext.rbx=0;ck(!invoke()&&has("reason=CALLER_CONTEXT"),"missing closure keeps native");
 reset();recorder=true;lambdaCaller(0x1F24836,0x3EC9610);mem.erase(CLO+8);ck(!invoke()&&has("reason=CALLER_CONTEXT"),"unreadable closure keeps native");
 reset();recorder=true;tickCaller();ctxNext.rbx=reinterpret_cast<std::uintptr_t>(&otherEquipment);ck(!invoke()&&has("reason=CALLER_CONTEXT"),"tick retry with rbx!=equipment keeps native");
 // 2AAA012 keeps its C32 admission: return address + owner only, no context read
 reset();caller=0x2AAA012;useCtx=true;ctxNext={};ctxNext.rdi=CTL;mem[CTL+0x20]=0;ck(invoke()&&receiptReads35==0,"2AAA012 behaviour unchanged (no new latch/context gate)");
 reset();caller=0x2AAA012;g_reload_callers37=false;ck(invoke(),"2AAA012 unaffected by C37 pin state");
 reset();caller=0x2AAA012;experiment=false;ck(!invoke(),"2AAA012 manual mode off native");
 // every excluded caller stays native even with a perfectly valid-looking bow context
 const std::uintptr_t excluded[]{0x737A74,0x222CB41,0x1F28766,0x33A144B,0x33A2B6E,0x33A33A4,0x2AAA50A,0x2AAA505,0x1F245FD,0x1F24831,0x224A387,0x2AAA00D,0x1846A54,0x5550123};
 for(auto ret:excluded){reset();recorder=true;lambdaCaller(ret,0x3EC9610);ck(!invoke(),"excluded caller keeps native");
  ck(has("reason=CALLER_NOT_MATCHED")&&has("callerMatch=0")&&receiptReads35==0&&itemsReads==0,"excluded caller: no context or owner read");}
 // crossbow/gun prepare: 1F245F0 tail-jumps 2AAA4C0 so the return address is the GENERIC dispatcher's; never matched
 reset();recorder=true;lambdaCaller(0x2B1F0E3,0x3EC9760);ck(!invoke()&&has("reason=CALLER_NOT_MATCHED"),"crossbow_prepare generic dispatcher keeps native");
 reset();recorder=true;lambdaCaller(0x2B1F0E3,0x3EC9728);ck(!invoke()&&has("reason=CALLER_NOT_MATCHED"),"gun_prepare generic dispatcher keeps native");
 // edge-only receipts: press, held-trigger retries, release+press again
 reset();recorder=true;lambdaCaller(0x1F24836,0x3EC9610);ck(invoke(),"press defers");
 useCtx=true;caller=0x2AAA012;ctxNext={};ctxNext.rdi=CTL;for(int i=0;i<30;++i)ck(invoke(),"held trigger retries defer");
 ck(diagnostic_trace::notes.size()==2&&has("callerKind=UPDATE_RETRY_2AAA012")&&has("repeatsBefore=0"),"held retry prints one edge");
 lambdaCaller(0x1F24836,0x3EC9610);ck(invoke(),"second press defers");
 ck(diagnostic_trace::notes.size()==2,"second identical press is a repeat of a recent edge");
 tickCaller();for(int i=0;i<10;++i){ck(invoke(),"tick retry defers");caller=0x2AAA012;ctxNext={};ctxNext.rdi=CTL;ck(invoke(),"update retry defers");tickCaller();}
 ck(diagnostic_trace::notes.size()==3&&has("callerKind=TICK_RETRY_224A38C")&&has("repeatsBefore=30"),"interleaved 224A38C/2AAA012 retries print one new edge");
 caller=0x1846A54;ck(!invoke()&&diagnostic_trace::notes.size()==4&&has("repeatsBefore=19"),"new refusal edge reports suppressed retries");
}
static void pending40(){caller=0x222CB40;useCtx=true;ctxNext={};ctxNext.rbx=EQ;ctxNext.rdx=0;}
static void c40(){
 reset();recorder=true;pending40();ck(invoke(),"equipment pending reload defers for current fresh Henry bow");
 ck(has("callerKind=EQUIPMENT_PENDING_222CB40")&&has("stage=AUTO_RELOAD_DEFERRED")&&has("pendingLatch=-1"),"receipt distinguishes equipment request from controller latch");
 ck(receiptReads35==0&&itemsReads==1,"equipment request adds no unproved controller/latch read");
 for(unsigned i=0;i<14;++i){reset();recorder=true;pending40();
  switch(i){case 0:experiment=false;break;case 1:g_reload_callers37=false;break;
   case 2:ctxNext.rbx=PL;break;case 3:ctxNext.rdx=1;break;case 4:ctxNext.r8=1;break;
   case 5:retail=false;break;case 6:frame.player=&otherPlayer;break;case 7:frame.input.tick=919;break;
   case 8:items.equipment=&otherEquipment;break;case 9:frame.profiles[1].classId=10;break;
   case 10:liveKey=21;break;case 11:g_manual_bow_fault596=true;break;
   case 12:caller=0x222CB41;break;case 13:g_reload_ready36=false;break;}
  ck(!invoke(),"equipment pending: off/pin/context/args/foreign/stale/nonbow/fault remain native");
 }
 reset();pending40();ctxNext.rdx=0x7700000000;ctxNext.r8=0x8800000000;ck(invoke(),"equipment pending checks only native byte arguments");
}
static bool nestedDeferred32=false;
static void actionAdmission64(){
 const auto action=std::uintptr_t(0x5000);
 const auto invokeAction=[&](std::uintptr_t receiver=0x5000){
  ReloadContext36 c{receiver,8,0};const auto memoryBefore=mem;const auto before=c;
  ReloadActionAdmission64(c);
  ck(mem==memoryBefore&&c.receiver==before.receiver&&c.sp==before.sp&&c.r8==before.r8,"allocation gate changes no native memory or arguments");
  return c.ip==reinterpret_cast<std::uintptr_t>(&DeferredReload596);
 };
 reset();caller=0x1846A54;mem[EQ+0x50]=action;ck(invokeAction(),"common action-allocation branch defers current exact manual bow owner");
 reset();caller=0x1846A54;mem[EQ+0x50]=action;ck(!invokeAction(action+1),"foreign shooting action keeps native allocator");
 reset();caller=0x1846A54;mem[EQ+0x50]=0;ck(!invokeAction(),"no-action pending path never qualifies allocation owner");
 reset();caller=0x1846A54;mem[EQ+0x50]=action;experiment=false;ck(!invokeAction(),"explicit automatic loading leaves authored allocation native");
 reset();caller=0x1846A54;mem[EQ+0x50]=action;g_manual_bow_fault596=true;ck(!invokeAction(),"ambiguous manual fault never denies native allocator");
 reset();caller=0x1846A54;mem[EQ+0x50]=action;now=1081;ck(!invokeAction(),"old frame cannot suppress a new native allocation");
 reset();caller=0x33A144B;mem[EQ+0x50]=action;ck(!invokeAction(),"immediate load postcondition caller cannot enter the allocation deferral contract");
 reset();caller=0x1846A54;mem[EQ+0x50]=action;g_reload_callers37=false;ck(!invokeAction(),"retail caller/receiver pins required for common admission");
 reset();caller=0x1846A54;mem[EQ+0x50]=action;frame.profiles[1].classId=10;ck(!invokeAction(),"crossbow does not borrow manual bow action ownership");
 reset();caller=0x1846A54;mem[EQ+0x50]=action;frame.profiles[1].classId=11;ck(!invokeAction(),"firearm does not borrow manual bow action ownership");
}
int main(){
 actionAdmission64();
 reset();ck(invoke(),"same fresh Henry bow automatic request defers before allocation");
 ck(diagnostic_trace::notes.empty(),"recorder off does not format a receipt");
 ck(receiptReads35==0,"recorder off: 2AAA012 admission adds no caller-context read");
 ck(itemsReads==1&&keyReads==2&&callerReads==1,"accepted callback performs one admission and no diagnostic native reads");
 reset();recorder=true;nestedDeferred32=false;
 diagnostic_trace::onWrite32=+[](){nestedDeferred32=invoke();};
 ck(invoke()&&nestedDeferred32,"reentrant recorder preserves both native reload admission decisions");
 ck(diagnostic_trace::notes.size()==1,"reentrant diagnostic formatting skips without waiting or recursing");
 diagnostic_trace::onWrite32=nullptr;
 caller=0x1846A54;ck(!invoke()&&diagnostic_trace::notes.size()==2,"receipt lock releases after formatting and does not quarantine later callbacks");caller=0x2AAA012;
 reset();caller=0x1846A54;ck(!invoke(),"capture OFF unknown caller stays native");
 ck(diagnostic_trace::notes.empty()&&itemsReads==0&&keyReads==0&&callerReads==1,"OFF caller refusal performs only original caller query and no logging");
 reset();recorder=true;caller=0x1846A54;ck(!invoke(),"recorded caller refusal stays native");
 ck(diagnostic_trace::notes.size()==1&&diagnostic_trace::notes.back().find("reason=CALLER_NOT_MATCHED")!=std::string::npos&&
    diagnostic_trace::notes.back().find("callerChecked=1 callerMatch=0")!=std::string::npos,"caller refusal reports exact checked outcome");
 ck(itemsReads==0&&keyReads==0&&callerReads==1,"refusal diagnostic adds no native queries");
 ck(receiptReads35==0&&diagnostic_trace::notes.back().find("callerReadable=1 callerRaw=")!=std::string::npos&&
    diagnostic_trace::notes.back().find("callerMinusWH=1846A54")!=std::string::npos,"bounded receipt reports actual unknown caller without admitting it");
 reset();recorder=true;g_reload_ready36=false;ck(!invoke(),"not ready callback stays native");
 ck(callerReads==0&&itemsReads==0&&diagnostic_trace::notes.back().find("reason=NOT_READY")!=std::string::npos&&
    diagnostic_trace::notes.back().find("callerChecked=0")!=std::string::npos,"not-ready receipt does not invent a caller check");
 reset();recorder=true;experiment=false;ck(!invoke(),"manual mode OFF stays native");
 ck(diagnostic_trace::notes.back().find("reason=MANUAL_MODE_OFF")!=std::string::npos&&itemsReads==0&&keyReads==0,"manual mode refusal has no owner queries");
 reset();recorder=true;g_manual_bow_fault596=true;ck(!invoke(),"faulted manual path stays native");
 ck(diagnostic_trace::notes.back().find("reason=MANUAL_FAULT")!=std::string::npos,"manual fault is visible");
 reset();recorder=true;haveItems=false;ck(!invoke(),"failed item read stays native");
 ck(diagnostic_trace::notes.back().find("reason=ITEMS_READ")!=std::string::npos&&itemsReads==1&&keyReads==1,"item reason uses single existing query");
 reset();recorder=true;onItemsRead32=+[](){++frame.profiles[1].generation;};ck(!invoke(),"owner change stays native");
 ck(diagnostic_trace::notes.back().find("reason=OWNER_CHANGED")!=std::string::npos,"owner mismatch explicitly reported");
 reset();recorder=true;for(unsigned i=0;i<30;++i){caller=0x1846A54+i;ck(!invoke(),"repeated refusal stays native");}
 ck(diagnostic_trace::notes.size()==16,"distinct refusals share bounded sixteen receipts per capture session");
 ck(receiptReads35==0&&callerReads==30,"receipts reuse the single admission return-address read");
 reset();recorder=true;for(unsigned i=0;i<30;++i)ck(invoke(),"repeated automatic attempts stay deferred");
 ck(diagnostic_trace::notes.size()==1,"edge-only receipt: identical held-trigger retries are counted, not printed");
 for(unsigned which=0;which<21;++which){reset();
  switch(which){case 0:experiment=false;break;case 1:retail=false;break;case 2:g_reload_ready36=false;break;
   case 3:g_manual_bow_fault596=true;break;case 4:haveFrame=false;break;case 5:frame.player=&otherPlayer;break;
   case 6:g_phys_ready_sample6698=0;break;case 7:frame.input.sample=8;break;case 8:frame.request=2;break;
   case 9:frame.input.bodyEpoch=2;break;case 10:frame.input.tick=919;break;case 11:frame.input.tick=1001;break;
   case 12:frame.input.allowed=false;break;case 13:frame.profiles[1].classId=10;break;case 14:handKnown=false;break;
   case 15:liveKey=21;break;case 16:haveItems=false;break;case 17:items.component=nullptr;break;
   case 18:items.weaponKey=21;break;case 19:caller=0x1846A54;break;case 20:callerReadable=false;break;}
  ck(!invoke(),"unknown/foreign/stale/default/nonbow request preserves original path");
 }
 c37();
 c40();
 reset();ck(!invoke(nullptr),"null equipment preserves native path");
 reset();ck(!invoke(&componentMemory),"missile item component is NOT the native automatic-request equipment receiver");
 reset();items.equipment=&otherEquipment;ck(!invoke(),"different current equipment preserves native path even with the same bow component");
 reset();mutateAt=3;ck(!invoke(),"new frame publication during native item lookup refuses old sample");
 reset();frame.input.tick=990;onItemsRead32=+[](){++frame.input.sample;g_phys_ready_sample6698=frame.input.sample;frame.input.tick=1000;};
 ck(invoke(),"newer fresh current-ready frame of the exact same owner still defers automatic allocation");
 reset();onItemsRead32=+[](){++frame.input.sample;g_phys_ready_sample6698=frame.input.sample;frame.input.tick=999;};
 ck(!invoke(),"newer sample with reversed source time cannot defer native allocation");
 reset();onItemsRead32=+[](){++frame.input.sample;g_phys_ready_sample6698=frame.input.sample;++frame.profiles[1].generation;};
 ck(!invoke(),"new publication with replaced bow generation keeps native path");
 reset();onItemsRead32=+[](){++frame.input.sample;g_phys_ready_sample6698=frame.input.sample;frame.manager=&componentMemory;};
 ck(!invoke(),"new publication with replaced manager keeps native path");
 reset();frame.input.tick=920;ck(invoke(),"original 80ms endpoint accepted without refreshing source time");
 std::printf("%u actual automatic-reload callback/owner checks passed; native lookups substituted\n",checks);
}
