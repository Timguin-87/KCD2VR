// Portable test of the actual native dispatcher + H36 transaction/retirement.
// Real game calls and graph memory reads are explicit host services. The exact
// selection/stage/retirement code is extracted on every test run, not restated.
#include "vrik/physical_weapon.h"
#include "core/initial_draw_completion_h36r3.h"
#include <atomic>
#include <array>
#include <cstdio>
#include <cstring>
#include <stdexcept>
namespace ph=kcd2vr::physical;
namespace initial=kcd2vr::initial_draw_h36r3;
#define __try try
#define __except(x) catch(...)
#define __fastcall
struct PhysEquipmentItem6698 {ph::Profile profile{};int set=0,slot126=0;void* item=nullptr;void* worldLink54=nullptr;void* entity=nullptr;bool held=false;};
struct PhysEquipment6698 {void* player=nullptr;void* manager=nullptr;std::array<PhysEquipmentItem6698,8>items{};
 int count=0,selected=0,heldIndex[2]{-1,-1};std::uint64_t handKey[2]{},epoch=9,request=10,tick=1000,leftNativeMembershipH36=0;bool handKnown[2]{true,true},valid=true;};
struct PhysFrame6698 {ph::Input input{};void* player=nullptr;void* manager=nullptr;};
#include "selection_structs_d48.inc"
static std::atomic<std::uint64_t>g_pose_epoch6689{9},g_weapon_request_epoch6694{10},g_left_native_membership_h36{0};
static std::atomic<std::uintptr_t>g_player_char37{55};
static std::atomic<bool>g_native_held_equipment_pending_h36{false},g_native_held_release_inhibit_t4{false};
static std::atomic<int>g_weapon_control_mode{6};
namespace kcd2vr::diagnostics::lab {enum class Key{F3};inline int Get(Key){return 0;}}
namespace kcd2vr::diagnostics::capture168 {enum class Domain{Interaction,Bug};struct Gate{std::atomic<std::uint64_t>session[2]{};};inline Gate gate;inline bool active(Domain,std::uint64_t){return false;}}
namespace diagnostics=kcd2vr::diagnostics;
static bool suspended=false,enabled=true,busy=false;static unsigned invalidates=0;
namespace heldT4 {inline bool Enabled(){return enabled;}inline void SetEquipmentAdmissionSuspendedH36(bool on){suspended=on;}}
static bool NativeHeldBusyT4(){return busy;}
static void NativeHeldInvalidateT4(){++invalidates;}
static bool PhysLeftHolsterRetiredH36(const ph::Input&,const PhysEquipment6698&){return true;}
static std::uint64_t now=1000;static std::uint64_t GetTickCount64(){return now;}
static bool PhysHolsterCaptureD48(std::uint64_t){return false;}
namespace diagnostic_trace{inline void Write(const char*){}}
namespace retail_build112{inline bool Ready(std::uintptr_t){return true;}}
static std::uintptr_t WhBase(){return 0x180000000ull;}
static bool FullWeaponMode120(int mode){return mode>=6;}
static bool PhysVft6698(void* p,std::uintptr_t){return p!=nullptr;}
static bool PhysSupplyHeldT2(const PhysEquipment6698&,const PhysEquipmentItem6698&,int){return false;}
static bool PhysEquipmentEntries62(){return true;}
static unsigned faults=0;static void PhysFault6698(const char*){++faults;}
static bool readSelection=true;
static std::array<unsigned char,0x100>manager{},itemMemory{},worldLink{},entityMemory{};
template<class T>static bool Read(const void* p,T& value){
 if(!p||(!readSelection&&p==manager.data()+0x40))return false;std::memcpy(&value,p,sizeof(value));return true;}
template<class T>static void put(std::array<unsigned char,0x100>& mem,std::size_t at,const T& value){std::memcpy(mem.data()+at,&value,sizeof(value));}
static bool bindingKnown=true,initialKnown=false,stowedKnown=true,keysKnown=true,graphKnown=true,destinationsKnown=true;
static LeftBindingH36 live{};static LeftSocketH36 stored{};
static const PhysEquipmentItem6698* LeftItemH36(const PhysEquipment6698&e,const ph::Command&c,bool exact){
 for(int i=0;i<e.count;++i){const auto& item=e.items[i];if(item.profile.key==c.key&&item.profile.side==1&&item.set==c.set&&(!exact||item.profile.generation==c.generation))return &item;}return nullptr;}
static bool ReadLeftBindingH36(const PhysEquipment6698&e,const ph::Command&c,LeftBindingH36& out){
 if(!bindingKnown||!LeftItemH36(e,c,true))return false;out=live;out.nativeMembership=g_left_native_membership_h36.load();return true;}
static bool ReadInitialLeftDrawH36r3(const PhysEquipment6698&,const ph::Command&c,LeftBindingH36&out,initial::Watch&watch){
 if(!initialKnown)return false;out=live;watch={0,c.key,c.bodyEpoch,c.epoch,c.sample,c.tick,g_left_native_membership_h36.load(),1,2,55,3};return initial::arm(watch);}
static bool FindOriginalStowedH36(const PhysEquipment6698&,LeftBindingH36&out){if(!stowedKnown)return false;out.stowed=stored;out.stowedIndex=0;out.stowedName[0]='b';return true;}
static bool SocketStillCurrentH36(const LeftSocketH36&s){if(!graphKnown)return false;const auto&current=s.attachment==live.hand.attachment?live.hand:stored;return current.object==s.object&&SameSocketGraphH36(s,current);}
static bool CurrentEquipmentKeysH36(const PhysEquipment6698&){return keysKnown;}
static bool CaptureStowDestinationsH36(const PhysEquipment6698&,const LeftBindingH36&,LeftStowDestinationsH36&out){out.knownEmpty=1;out.sockets[0]=stored;return destinationsKnown;}
static bool ReadLeftHandH36(const PhysEquipment6698&,LeftSocketH36& out){out=live.hand;return graphKnown;}
static bool CurrentOriginalStowedH36(const PhysEquipment6698&,const LeftBindingH36&,LeftSocketH36&out){out=stored;return graphKnown;}
static bool CurrentStowDestinationH36(const PhysEquipment6698&,const LeftBindingH36&,const LeftStowDestinationsH36&,LeftSocketH36&out){out=stored;return graphKnown&&stored.object;}
static PhysEquipment6698* current=nullptr;
static unsigned selects=0,draws=0,stows=0;static bool selectThrows=false,selectAccept=true,drawThrows=false,drawAddsMembership=true,stowReturns=true;
static void HostSelectD48(void* m,int set){++selects;if(selectThrows)throw std::runtime_error("selector fault");if(selectAccept){int actual=set;std::memcpy(static_cast<unsigned char*>(m)+0x40,&actual,4);}}
static void HostDrawD48(void*,std::uint64_t,int){++draws;if(drawThrows)throw std::runtime_error("draw fault");if(drawAddsMembership)++g_left_native_membership_h36;}
static bool HostStowD48(void*,std::uint64_t){++stows;return stowReturns;}
static void HostLegacyDrawD48(void*){++draws;}static void HostLegacyStowD48(void*){++stows;}
#include "selection_actual_d48.inc"
static unsigned checks=0,failed=0;static void ck(bool value,const char* text){++checks;if(!value){++failed;std::printf("FAIL %s\n",text);}}
// Cancellation receipts were added in D48. Baseline runs the common actual
// selection/preparation/native-entry/graph-ACK assertions; it has no cancel API.
static void checkCancel(const PhysEquipment6698& e,const ph::Command& c,bool expected,const char* text){
#ifdef D48_CURRENT
 ck(PhysCancelUnenteredEquipmentH36(e,c)==expected&&g_native_held_equipment_pending_h36.load()!=expected,text);
#else
 (void)e;(void)c;(void)expected;(void)text;
#endif
}
struct Rig {
 PhysEquipment6698 gear{};PhysFrame6698 frame{};ph::Command command{};bool nativeEntered=false;
 Rig(){
  g_left_equipment_obligation_h36={};g_left_equipment_successors_h36={};g_left_equipment_established_h36={};g_left_equipment_overflow_h36=false;
  g_native_held_equipment_pending_h36=false;g_native_held_release_inhibit_t4=false;g_left_native_membership_h36=0;
  now=1000;selects=draws=stows=faults=invalidates=0;suspended=busy=false;enabled=true;g_weapon_control_mode=6;readSelection=true;
  bindingKnown=stowedKnown=keysKnown=graphKnown=destinationsKnown=true;initialKnown=false;
  selectThrows=drawThrows=false;selectAccept=drawAddsMembership=stowReturns=true;manager={};itemMemory={};worldLink={};entityMemory={};
  for(auto&s:initial::slots){s.active=0;s.submission=0;}initial::invoking={};initial::callbackReady=true;
  gear.player=reinterpret_cast<void*>(1);gear.manager=manager.data();gear.count=1;gear.selected=0;gear.items[0].set=1;
  gear.items[0].profile.key=88;gear.items[0].profile.generation=2;gear.items[0].profile.entity=80;gear.items[0].profile.side=1;
  gear.items[0].item=itemMemory.data();gear.items[0].worldLink54=worldLink.data();gear.items[0].entity=entityMemory.data();
  put(manager,0x48+16,std::uint64_t(88));put(manager,0x40,int(0));put(itemMemory,0x30,std::uint64_t(88));put(itemMemory,0x88,static_cast<void*>(worldLink.data()));
  live={};live.valid=true;live.item=itemMemory.data();live.link=worldLink.data();live.entity=entityMemory.data();live.key=88;live.generation=2;
  live.player=gear.player;live.manager=gear.manager;live.body=9;live.request=10;live.set=1;
  live.hand={reinterpret_cast<void*>(3),reinterpret_cast<void*>(4),reinterpret_cast<void*>(5),nullptr,55,0};
  stored={reinterpret_cast<void*>(3),reinterpret_cast<void*>(6),reinterpret_cast<void*>(5),reinterpret_cast<void*>(7),55,80};
  command.kind=ph::CommandKind::Draw;command.side=1;command.set=1;command.key=88;command.generation=2;command.epoch=10;command.bodyEpoch=9;command.sample=100;command.tick=1000;
  frame.player=gear.player;frame.manager=gear.manager;frame.input.allowed=true;frame.input.tracked[1]=frame.input.intentTracked[1]=true;
  frame.input.epoch=10;frame.input.bodyEpoch=9;frame.input.sample=100;frame.input.tick=1000;current=&gear;
 }
 bool dispatch(){ck(PhysLeftEquipmentH36(frame,gear,command)==HolsterNativeH36::Ready,"original left admission is ready");
#ifdef D48_CURRENT
 return PhysEquipmentCommand6698(gear,command,&nativeEntered);
#else
 return PhysEquipmentCommand6698(gear,command);
#endif
 }
 void entry(bool expected,const char* text){
#ifdef D48_CURRENT
 ck(nativeEntered==expected,text);
#else
 (void)expected;(void)text;
#endif
 }
 void bind(){gear.handKey[1]=88;gear.heldIndex[1]=0;gear.items[0].held=true;stored.object=nullptr;live.hand.object=reinterpret_cast<void*>(7);live.hand.entity=80;}
 void refresh(){PhysLeftEquipmentRefreshH36(gear,now);}
};
int main(){
 {Rig r;ck(r.dispatch()&&selects==1&&draws==1,"cross-set selection invokes one same-command native draw");r.entry(true,"consumer receipt records actual draw entry");
  ck(g_left_equipment_obligation_h36.entered&&g_left_equipment_obligation_h36.precise&&g_left_equipment_obligation_h36.returned&&!g_left_equipment_obligation_h36.uncertain,"same command selection does not corrupt precise draw receipt");
  r.refresh();ck(g_native_held_equipment_pending_h36,"selector return and draw wrapper return are not attachment ACKs");
  r.bind();r.refresh();ck(!g_native_held_equipment_pending_h36&&!suspended&&!LeftEquipmentPendingH36(),"exact original left cross-set draw graph completes without permanent debt");}
 {Rig r;bindingKnown=false;ck(!r.dispatch()&&draws==0&&!g_left_equipment_obligation_h36.entered,"failed binding preflight never invokes native draw or marks entered");r.entry(false,"consumer may reject failed preparation as unentered");
  checkCancel(r.gear,r.command,true,"completed selection plus failed preflight is cancellable unentered intent");}
 {Rig r;stowedKnown=false;ck(!r.dispatch()&&draws==0&&!g_left_equipment_obligation_h36.entered,"unproven stowed graph is a preflight denial without a native call");
  checkCancel(r.gear,r.command,true,"releasing unproven stowed candidate clears only local preparation");}
 {Rig r;selectAccept=false;ck(!r.dispatch()&&selects==1&&draws==0,"selector refusing set never falls through to draw");r.entry(false,"normally returned selector refusal leaves no unfinished native call");
  checkCancel(r.gear,r.command,true,"returned synchronous rejected selection does not invent native cleanup debt");}
 {Rig r;readSelection=false;ck(!r.dispatch()&&draws==0,"readback failure never authorizes native draw");readSelection=true;
  checkCancel(r.gear,r.command,true,"normal synchronous selector return remains cancellable when readback unavailable");}
 {Rig r;selectThrows=true;ck(!r.dispatch()&&draws==0&&selects==1&&faults==1,"abnormal selector return cannot invoke draw");r.entry(true,"consumer retains abnormal selector entry without a return receipt");
  now=3000;r.gear.tick=now;r.refresh();checkCancel(r.gear,r.command,false,"unreturned selector entry survives timeout and open hand");}
 {Rig r;drawThrows=true;ck(!r.dispatch()&&draws==1&&g_left_equipment_obligation_h36.entered&&!g_left_equipment_obligation_h36.returned,"draw exception retains actual original entry");r.entry(true,"consumer retains abnormal attachment entry");
  r.bind();now=3000;r.gear.tick=now;r.refresh();checkCancel(r.gear,r.command,false,"plausible later binding cannot acknowledge an abnormal original draw");}
 {Rig r;r.dispatch();const bool precise=g_left_equipment_obligation_h36.precise,uncertain=g_left_equipment_obligation_h36.uncertain;
  ck(!PhysEquipmentCommand6698(r.gear,r.command)&&draws==1&&selects==1&&g_left_equipment_obligation_h36.precise==precise&&g_left_equipment_obligation_h36.uncertain==uncertain,"duplicate same command neither repeats native entry nor corrupts original receipt");}
 {Rig r;r.gear.selected=1;put(manager,0x40,int(1));ck(r.dispatch()&&selects==0&&draws==1,"already-selected loadout bypasses synchronous selector");r.bind();r.refresh();ck(!g_native_held_equipment_pending_h36,"already-selected precise draw still retires from actual graph");}
 {Rig r;r.gear.selected=1;put(manager,0x40,int(1));r.command.kind=ph::CommandKind::Holster;r.gear.handKey[1]=88;r.gear.items[0].held=true;live.hand.object=reinterpret_cast<void*>(7);live.hand.entity=80;stored.object=nullptr;destinationsKnown=false;
  ck(!r.dispatch()&&stows==0&&!g_left_equipment_obligation_h36.entered,"failed stow preparation does not invoke native stow");checkCancel(r.gear,r.command,true,"unentered stow preparation can be released");}
#ifdef D48_CURRENT
 {Rig r;ck(PhysLeftEquipmentH36(r.frame,r.gear,r.command)==HolsterNativeH36::Ready,"initial no-world candidate admits exact record");bindingKnown=false;initialKnown=true;
  ck(PhysLeftEquipmentBeginH36(r.gear,r.command,true)&&g_left_equipment_obligation_h36.preparedD48&&!g_left_equipment_obligation_h36.entered,"initial callback watch preparation is not native entry");
  const auto watch=g_left_equipment_obligation_h36.initialDrawH36r3;ck(watch.serial!=0,"preparation allocated original callback watch");
  ck(PhysCancelUnenteredEquipmentH36(r.gear,r.command)&&!g_native_held_equipment_pending_h36,"open hand cancels prepared initial draw before native invocation");
  bool retained=false;for(auto&s:initial::slots)retained|=s.active.load()==watch.serial;ck(!retained,"unentered cancellation releases only its unused callback watch");}
 {Rig r;g_left_equipment_obligation_h36.active=g_left_equipment_obligation_h36.entered=true;g_left_equipment_obligation_h36.command=r.command;g_left_equipment_obligation_h36.command.key=99;
  for(auto&x:g_left_equipment_successors_h36){x.active=x.entered=true;x.command=r.command;x.command.key=99;}
  ck(PhysLeftEquipmentH36(r.frame,r.gear,r.command)==HolsterNativeH36::Wait&&!draws&&!g_left_equipment_overflow_h36,"ledger capacity waits before native entry without minting permanent overflow debt");}
#endif
 std::printf("D48 actual selection/dispatch/retirement %u/%u; native leaves and graph reads SIMULATED\n",checks-failed,checks);return failed?1:0;
}
