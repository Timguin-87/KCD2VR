#include "../src/frik_pose_probe.h"
#include "../src/diagnostics/hook_cost178r22.h" // V66.178R22b: the extracted attachment detour carries a cost scope
using kcd2vr::frik_pose_probe::FullWeaponMode120;
using kcd2vr::frik_pose_probe::BowHandlingMode59c9;
namespace kcd2vr::frik_pose_probe { bool BowGripDrawEnabled596C1() noexcept {return true;} }
// Actual current F12 adapter; only native entity-call leaf and memory/native
// APIs are replaced. No DLL is loaded. No Windows/headset behavior is simulated.
#include "../src/diagnostics/lab_state.h"
#include "../src/diagnostics/capture168.h"
#include "../src/core/haptics.h"
namespace diagnostics=kcd2vr::diagnostics;
#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include "../src/vrik/pose_math.h"
#include "../src/vrik/tracking_grace111.h"
#include "../src/vrik/physical_handoff132.h"
static std::atomic<float> g_right_grip{1.f};
static std::atomic<std::uint64_t> g_phys_waiting_native157{0}; // Production diagnostic counter; fixture storage only.
#include "../src/core/snapshot_mailbox.h"
#define __try try
#define __except(x) catch(...)
#define __fastcall
struct QuatT28{float q[4],t[3];};
#include "generated_adapter_types.h"
using PoseGetFn=const QuatT28*(*)(void*,int);
using EntitySetPosRotScaleFn=void(*)(void*,const float*,const float*,const float*,std::uint32_t);
using AttachmentWorldQuatTsFn=void(*)(void*,float*);
struct Matrix34V6550{float m[12];};
constexpr std::uintptr_t kCEntityVftRva=0x4072C78;
static std::uint64_t clockMs=1000;static std::uint64_t GetTickCount64(){return clockMs;}
static std::vector<std::string> logLines;namespace diagnostic_trace{void Write(const char*m){logLines.emplace_back(m);}}
#include "../src/vrik/fault_pause59.h"
static int callsNative=0,lookupCalls=0,prsCalls=0,childCalls=0,queries=0;static bool failPlacement=false,frameValid=false;
static std::vector<int>order;
struct Entity{std::uintptr_t vf;std::uint32_t padding,id;Matrix34V6550 world;};static Entity entity;
static std::array<unsigned char,0xA40>child{};static std::array<unsigned char,0x100>item{},link{},sys{},attachment{};
static std::array<unsigned char,0x30>self{};
static std::array<unsigned char,0x80>manager{};
static std::array<unsigned char,0xA00>player{};
static std::array<unsigned char,0x300>handSlots{};
static float nativeQS[8]{0,0,0,1,10,20,30,1};
static void ChildUpdate(void*p,const float*q){++childCalls;order.push_back(2);std::memcpy(static_cast<unsigned char*>(p)+0xA08,q,32);}
static std::uintptr_t WhBase(){return reinterpret_cast<std::uintptr_t>(&ChildUpdate)-0x423C30;}
static void PRS(void*e,const float*p,const float*q,const float*scale,std::uint32_t flags){if(failPlacement)throw 1;++prsCalls;order.push_back(1);auto&tm=static_cast<Entity*>(e)->world;using namespace kcd2vr::pose_core;Quat r{q[0],q[1],q[2],q[3]};for(int c=0;c<3;++c){auto v=rotate(r,{double(c==0),double(c==1),double(c==2)});tm.m[c]=float(v.x*scale[c]);tm.m[4+c]=float(v.y*scale[c]);tm.m[8+c]=float(v.z*scale[c]);}tm.m[3]=p[0];tm.m[7]=p[1];tm.m[11]=p[2];if(flags!=0x10)std::abort();}
static void* GetChild(void*,int){return child.data();}
static void GetQs(void*,float*out){++queries;std::memcpy(out,nativeQS,32);}
static void* MockNativeEntityLookup6694(std::uintptr_t fn,void*s,std::uint32_t id){++lookupCalls;return fn==WhBase()+0x4B4F98&&s==sys.data()&&id==entity.id?&entity:nullptr;}
static int readCalls=0,handOwnerReads=0;
template<class T>bool Read(const void*p,T&out){++readCalls;if(p==manager.data())++handOwnerReads;if(!p)return false;const auto a=reinterpret_cast<std::uintptr_t>(p),b=WhBase();std::uintptr_t value=0;bool virtualAddress=true;
 if(a==b+0x492D8A0)value=reinterpret_cast<std::uintptr_t>(sys.data());
 else if(a==b+0x4077048+0x70)value=b+0x4B4F98;
 else if(a==b+0x4072C78+0x168)value=reinterpret_cast<std::uintptr_t>(&PRS);
 else if(a==b+0x4072C78+0x328)value=reinterpret_cast<std::uintptr_t>(&GetChild);
 else if(a==b+0x40602F8+0x190)value=reinterpret_cast<std::uintptr_t>(&ChildUpdate);
 else if(a==b+0x4000000+0x70)value=reinterpret_cast<std::uintptr_t>(&GetQs);
 else virtualAddress=false;
 if(virtualAddress){if(sizeof(T)!=8)return false;std::memcpy(&out,&value,8);return true;}
 auto valid=[&](const void*q,std::size_t n){auto x=reinterpret_cast<std::uintptr_t>(q);return a>=x&&a+sizeof(T)<=x+n;};
 if(!(valid(&entity,sizeof(entity))||valid(item.data(),item.size())||valid(link.data(),link.size())||valid(sys.data(),sys.size())||valid(attachment.data(),attachment.size())||valid(child.data(),child.size())||valid(self.data(),self.size())||valid(manager.data(),manager.size())||valid(player.data(),player.size())||valid(handSlots.data(),handSlots.size())))return false;
 std::memcpy(&out,p,sizeof(T));return true;}
static bool ReadCompanion(void*,int id,QuatT28&q){q={{0,0,0,1},{0,0,0}};if(id>=412&&id<=415){q.t[0]=.1f;q.t[1]=(id-413.5f)*.015f;}return true;}
static bool ReadQuat(PoseGetFn f,void*p,int id,QuatT28&out){auto*q=f(p,id);if(!q)return false;out=*q;return true;}
static QuatT28 head{{0,0,0,1},{0,0,1.6f}},hand{{0,0,0,1},{0,0,0}};
static const QuatT28* Getter(void*,int id){return id==1?&head:&hand;}
static bool ControllerHeadAnchoredArmGoal(const TrackingSnapshot&,const QuatT28&,bool,V3&){return true;}
static bool GetEntityWorldTmV6552(void*e,std::uint32_t id,Matrix34V6550&out){if(e!=&entity||id!=entity.id)return false;out=entity.world;return true;}
static bool ReadCommittedHandFrame6689(QuatT28&n,QuatT28&v,float(&m)[8],kcd2vr::pose_core::Stamp* stamp=nullptr){if(!frameValid)return false;n=hand;v=hand;v.t[1]=.4f;std::memcpy(m,nativeQS,32);if(stamp)*stamp={1,1,clockMs,reinterpret_cast<std::uintptr_t>(child.data())};return true;}
static void Native(void*,void*){++callsNative;auto fail=failPlacement;failPlacement=false;float sc[3]{1,1,1};PRS(&entity,nativeQS+4,nativeQS,sc,0x10);ChildUpdate(child.data(),nativeQS);failPlacement=fail;}
static auto g_entity_attachment_update_original=&Native;
static std::atomic<int>g_weapon_control_mode{0};
static std::atomic<std::uint64_t>g_weapon_control_generation[2]{{1},{1}},g_native_side_wuid0[2]{{777},{0}},g_pose_epoch6689{1},g_f12_attachment_proof_writes{0};
static std::atomic<std::uint32_t>g_weapon_control_entity_id[2]{{0},{0}};
static std::atomic<void*>g_weapon_control_entity[2]{},g_henry_item_attach_manager{reinterpret_cast<void*>(99)},g_native_side_owner[2]{{reinterpret_cast<void*>(99)},{nullptr}},g_native_side_item[2]{{item.data()},{nullptr}};
static std::atomic<int>g_native_side_item_state[2]{{1},{0}},g_cal_state{0};
static std::atomic<float>g_cal_frame_qx{0},g_cal_frame_qy{0},g_cal_frame_qz{0},g_cal_frame_qw{1};
static std::atomic<float>g_cal_native_head_x{0},g_cal_native_head_y{0},g_cal_native_head_z{1.6f},g_cal_head_x{0},g_cal_head_y{0},g_cal_head_z{0};
static std::atomic<void*> g_committed_character6689{child.data()};
static std::atomic<bool> g_gameplay_allowed{true},g_head_valid{true},g_right_valid{true},g_committed_pose_valid6689{true};
static std::atomic<std::uint64_t> g_head_valid_tick{1000},g_right_valid_tick{1000};
static bool TrackingHeadUsable111(std::uint64_t now=0) noexcept {if(!now)now=GetTickCount64();return kcd2vr::tracking_grace111::KeepLastGood(g_head_valid.load(),now,g_head_valid_tick.load(),60);}
static bool TrackingHandUsable111(int side,std::uint64_t now=0) noexcept {if(side!=0)return false;if(!now)now=GetTickCount64();return kcd2vr::tracking_grace111::KeepLastGood(g_right_valid.load(),now,g_right_valid_tick.load(),80);}
static std::atomic<std::uint64_t> g_weapon_contended6695{0},g_weapon_lost6695{0},g_weapon_restored6695{0};
// Baseline modes 0..3 fixture: new physical adapter deliberately disabled here.
static bool PhysicalAttachmentPending6698(){return false;}
enum class PhysicalAttachmentResult101 {NotMine,Waiting,Placed,Restored,Busy,Fault};
// This retained F12 fixture has no admitted native solver body. The actual
// T4 attachment owner is exercised by native_held_owner_t4_bridge.
namespace {static PhysicalAttachmentResult101 NativeHeldAttachmentT4(void*,void*) noexcept {return PhysicalAttachmentResult101::NotMine;}}
static PhysicalAttachmentResult101 physicalResult101=PhysicalAttachmentResult101::NotMine;
static PhysicalAttachmentResult101 PhysicalAttachmentUpdate101(void*,void*){return physicalResult101;}
// OS page cache is tested in the complete TU bridge, not this native-memory fixture.
struct ReadScope101R2 {};
static void InvalidateReadCache101R2() noexcept {}
// F5 tool ownership is off in this F12 adapter; the actual tool implementation
// and native-leaf failure cases execute in tool_attachment59c10_test.
static bool ToolAttachment59c10(void*,void*) noexcept {return false;}
// The actual arrow identity/socket/readback path executes in manual_arrow64_test.
static bool arrowOwner64=false;
static bool ManualArrowAttachment64(void*,void*) noexcept {return arrowOwner64;}
// Exact native activity hand ownership is exercised by native_activity_hands46_test.
// This F12 fixture has no admitted alchemy/smithing owner.
static bool ObserveNativeActivityHand46(void*,void*) noexcept {return false;}
static bool nativePassengerA34=false;
static bool NativePassengerCurrentA34(std::uint64_t) noexcept {return nativePassengerA34;}
#include "generated_controller_adapter.inc"
// Real mode selection functions, mock only the native arming leaf.
static bool armSuccess=true;static int armCalls=0;
static void StartPredictionCapture120(){}
static bool predictionArmed120=true;
static bool ArmPrediction120(){return predictionArmed120;}
// V121 native camera-hook arming leaf is outside this F12 adapter fixture.
static bool ArmBowZoom121(){return true;}
static bool ArmMotionCombatInput101(){return true;}
static bool ArmPhysicalWeapons6698(){++armCalls;return armSuccess;}
static std::atomic<int> g_census_mode{3};
static std::atomic<std::uint64_t> g_contact_control_epoch6695{1},g_f12_attachment_proof_until_ms{0};
static std::atomic<bool> g_f12_follow_requested6689{false};
static void SetCombatCensusMode(int mode){g_census_mode.store(mode);}
#include "generated_mode_adapter.inc"
static int pass=0,total=0;void Check(bool p,const char*n){++total;pass+=p;std::printf("%s %s\n",p?"PASS":"FAIL",n);}
template<class T>void Store(void*p,T v){std::memcpy(p,&v,sizeof(v));}
static void Reset(){physicalResult101=PhysicalAttachmentResult101::NotMine;callsNative=lookupCalls=prsCalls=childCalls=queries=0;order.clear();logLines.clear();failPlacement=false;frameValid=false;clockMs=1000;g_cal_state=0;g_weapon_control_mode=0;g_pose_epoch6689=1;g_weapon_request_epoch6694=1;g_weapon_applied6694=0;g_weapon_fault6694=false;g_weapon_reason6694=0;g_weapon_control_generation[0]=1;g_henry_item_attach_manager=manager.data();g_native_side_owner[0]=manager.data();g_native_side_item_state[0]=1;g_native_side_item[0]=item.data();g_native_side_wuid0[0]=777;
 diagnostics::lab::state=0;diagnostics::lab::weaponRestorePending=false;g_attachment6689={};g_attachment_lock6689.clear();g_attachment_pending_self6695=nullptr;g_attachment_pending_object6695=nullptr;g_weapon_control_entity_id[0]=0;g_gameplay_allowed=true;g_head_valid=true;g_right_valid=true;g_head_valid_tick=clockMs;g_right_valid_tick=clockMs;g_committed_pose_valid6689=true;
 manager.fill(0);player.fill(0);handSlots.fill(0);
 Store(manager.data(),WhBase()+0x3EC9958);Store(manager.data()+0x38,player.data());
 Store(player.data(),WhBase()+0x3AA4B98);Store(player.data()+0x9D0,manager.data());Store(player.data()+0x990,handSlots.data());
 Store(handSlots.data()+0x260,std::uint64_t(777));
 item.fill(0);link.fill(0);sys.fill(0);self.fill(0);attachment.fill(0);child.fill(0);entity={};entity.vf=WhBase()+kCEntityVftRva;entity.id=42;
 Store(item.data(),WhBase()+0x3A2DA88);Store(item.data()+0x30,std::uint64_t(777));Store(item.data()+0x88,link.data());Store(link.data()+0x30,std::uint32_t(42));Store(sys.data(),WhBase()+0x4077048);Store(child.data(),WhBase()+0x40602F8);Store(attachment.data(),WhBase()+0x4000000);Store(self.data()+8,std::uint32_t(42));for(int c=0;c<3;++c)Store(self.data()+12+c*4,1.f);
 std::memcpy(child.data()+0xA08,nativeQS,32);
 g_weapon_tracking6694={};auto&r=g_weapon_tracking6694;r.valid=true;r.tick=1000;r.epoch=1;r.sample=1;r.character=child.data();r.skeleton=reinterpret_cast<void*>(1);r.nativeHand=hand;r.requestedHand=hand;r.requestedHand.t[0]=.2f;std::memcpy(r.world,nativeQS,32);
}
static void Step(){g_weapon_frames6695.publish(g_weapon_tracking6694);EntityAttachmentUpdateDetourV6688(self.data(),attachment.data());}
int main(){
 Reset();g_weapon_control_mode=2;Step();const auto beforePassengerPrs=prsCalls,beforePassengerNative=callsNative;
 nativePassengerA34=true;Step();
 Check(prsCalls==beforePassengerPrs+1&&callsNative==beforePassengerNative+1&&g_weapon_applied6694==1&&!g_attachment6689.changed,
       "current native passenger restores controller attachment once before renderer mirror");nativePassengerA34=false;
 Reset();arrowOwner64=true;Step();Check(callsNative==0&&prsCalls==0,"claimed native hand ammo skips both original and ordinary weapon placement");arrowOwner64=false;
 Reset();std::uint32_t id=0;Check(NativeHeldItemId6694(0,id)&&id==42,"native C_Item linked ID resolves without AI pointer scan");Check(EntityById6694(id)==&entity,"native entity-system lookup returns matching concrete ID");
 Store(item.data()+0x30,std::uint64_t(778));Check(!NativeHeldItemId6694(0,id),"stale/wrong held key rejected");
 Reset();Step();Check(callsNative==1&&lookupCalls==0&&queries==0,"OFF performs native operation without discovery queries");
 Reset();g_weapon_control_mode=2;Store(self.data()+8,std::uint32_t(99));Step();Check(callsNative==1&&lookupCalls==0&&queries==0,"unrelated attachment receives only native call");
 Reset();g_weapon_control_mode=2;Step();Check(g_weapon_applied6694==1&&g_weapon_control_entity_id[0]==42,"mode2 applies with no VRIK, no finger references and no prior proof");
 Check(std::abs(entity.world.m[3]-10.2f)<.0001f,"raw controller translation drives weapon position");
 float q[8]{};std::memcpy(q,child.data()+0xA08,32);Check(std::abs(q[4]-entity.world.m[3])<.0001f&&order.size()>=2&&order[order.size()-2]==1&&order.back()==2,"entity then child consume the identical requested transform");
 g_weapon_tracking6694.requestedHand.q[2]=.70710678f;g_weapon_tracking6694.requestedHand.q[3]=.70710678f;Step();Check(std::abs(entity.world.m[1]+1)<.0001f,"controller rotation also rotates the weapon");
 g_weapon_control_mode=0;Step();Check(!diagnostics::lab::weaponRestorePending&&std::abs(entity.world.m[3]-10.f)<.0001f,"OFF restores native placement on its own callback");
 Reset();g_weapon_control_mode=2;Step();clockMs=1200;Step();Check(g_weapon_reason6694==4&&!diagnostics::lab::weaponRestorePending&&std::abs(entity.world.m[3]-10)<.0001f,"stale raw tracking restores instead of writing stale transforms");
 Reset();g_weapon_control_mode=2;diagnostics::lab::Set(diagnostics::lab::Key::F3,1);Step();Check(g_weapon_applied6694==0&&g_weapon_reason6694==5,"reference mode suspends new weapon writes");
 Reset();g_weapon_control_mode=3;frameValid=true;Step();Check(std::abs(entity.world.m[7]-20.4f)<.0001f,"combined mode uses committed reachable hand when available");
 Reset();g_weapon_control_mode=3;Step();Check(g_weapon_applied6694==0&&g_weapon_reason6694==9,"combined mode waits for constrained hand, never switches to raw target");
 Reset();g_weapon_control_mode=2;failPlacement=true;Step();const int oldPrs=prsCalls;Step();Check(g_weapon_fault6694&&g_weapon_applied6694==0&&prsCalls==oldPrs+1,"placement exception disables repeats and native fallback remains");
 Reset();TrackingSnapshot ts{};ts.head_valid=ts.right_valid=ts.left_valid=ts.allowed=true;ts.tick=1000;ts.sequence=11;ts.right={.2f,-.8f,-.1f};RigMap rig{};rig.head=1;rig.hand_r=2;
 PublishWeaponTracking6694(child.data(),reinterpret_cast<void*>(1),child.data(),Getter,rig,ts);WeaponTracking6694 raw{};
 Check(ReadWeaponTracking6694(raw)&&std::abs(raw.requestedHand.t[2]-.8f)<.001f,"actual raw packet publisher reaches downward without VRIK calibration");
 const auto previous=raw.requestedHand;ts.hq={.3826834f,0,0,.9238795f};ts.sequence++;PublishWeaponTracking6694(child.data(),reinterpret_cast<void*>(1),child.data(),Getter,rig,ts);ReadWeaponTracking6694(raw);
 Check(Len(Sub(FromT(raw.requestedHand),FromT(previous)))<.0001f,"HMD pitch does not drag the independently requested weapon hand");

 // These replace the V66.94 expected-defect reproductions with passing contracts.
 Reset();g_weapon_control_mode=2;Step();auto position=entity.world.m[3];const int beforeBusy=callsNative;
 g_weapon_tracking_lock6694.test_and_set();Step();g_weapon_tracking_lock6694.clear();
 Check(callsNative==beforeBusy&&std::abs(entity.world.m[3]-position)<.0001f,"busy legacy tracking lock cannot snap published weapon back to native");
 g_attachment_lock6689.test_and_set();const int beforeOwned=callsNative;Step();g_attachment_lock6689.clear();
 Check(callsNative==beforeOwned&&std::abs(entity.world.m[3]-position)<.0001f,"duplicate owned attachment callback never replays native placement on contention");
 g_attachment_lock6689.test_and_set();Store(self.data()+8,std::uint32_t(99));
 // A different object must keep its normal callback even during an owned transaction.
 std::array<unsigned char,0x30> unrelated{};
 Store(self.data()+8,std::uint32_t(42));g_attachment_pending_self6695=nullptr;g_attachment_pending_object6695=nullptr;
 Store(self.data()+8,std::uint32_t(99));Step();g_attachment_lock6689.clear();
 Check(callsNative==beforeOwned+1,"unrelated attachment is filtered before control lock and remains native");
 Reset();g_weapon_control_mode=3;frameValid=true;Step();const float allowedY=entity.world.m[7];frameValid=false;clockMs=1030;Step();
 Check(g_weapon_applied6694==2&&std::abs(entity.world.m[7]-allowedY)<.0001f,"combined mode holds same still-fresh permitted hand, not unconstrained raw controller");
 clockMs=1090;Step();clockMs=1101;Step();
 Check(g_weapon_reason6694==9&&!diagnostics::lab::weaponRestorePending&&std::abs(entity.world.m[7]-20)<.0001f,"replaying a hand never refreshes its timestamp; expiration restores native");
 Reset();g_weapon_control_mode=3;frameValid=true;Step();frameValid=false;g_gameplay_allowed=false;clockMs=1001;Step();
 Check(!diagnostics::lab::weaponRestorePending&&g_weapon_reason6694==9,"gameplay permission loss invalidates even a one-millisecond-old hand");
 Reset();g_weapon_control_mode=2;Step();g_pose_epoch6689=2;Step();
 Check(!diagnostics::lab::weaponRestorePending,"calibration/origin epoch change invalidates old weapon pose");
 Reset();g_weapon_control_mode=2;Step();g_native_side_item_state[0]=0;Step();
 Check(!diagnostics::lab::weaponRestorePending,"unequip restores pending placement without using stale item");
 // Live native role changes with the exact same item ID/WUID and no null-item
 // attach callback. These previously left the sword controller-owned forever.
 for(int mode:{2,3}){
  Reset();g_weapon_control_mode=mode;frameValid=true;Step();const auto applied=g_weapon_applied6694.load();
  const float saved=nativeQS[4];nativeQS[4]=12.f;
  Store(handSlots.data()+0x260,std::uint64_t(0));Step();
  Check(!g_attachment6689.gripValid&&!diagnostics::lab::weaponRestorePending&&g_weapon_applied6694==applied&&std::abs(entity.world.m[3]-12.f)<.001,
        "same-item sheath revokes controller grip and uses current native destination in mode 2/3");
  Step();Check(g_weapon_applied6694==applied,"holstered attachment cannot reacquire from a stale occupied-item cache");
  nativeQS[4]=saved;Store(handSlots.data()+0x260,std::uint64_t(777));Step();
  Check(g_weapon_applied6694==applied+1&&g_attachment6689.gripValid,"positive native return to same hand reacquires without toggling F12");
 }
 Reset();Store(handSlots.data()+0x260,std::uint64_t(888));Check(!NativeHeldItemId6694(0,id),"different current native hand item rejects stale exact entity cache");
 Reset();Store(player.data()+0x9D0,static_cast<void*>(nullptr));Check(!NativeHeldItemId6694(0,id),"broken manager/player back-reference cannot grant hand ownership");
 Reset();Store(player.data(),WhBase()+0x3B71668);Check(!NativeHeldItemId6694(0,id),"dog cannot satisfy the live hand slot owner chain");
 Reset();Store(handSlots.data()+0x268,std::uint64_t(777));Store(handSlots.data()+0x260,std::uint64_t(0));
 Check(!NativeHeldItemId6694(0,id),"left-hand membership cannot authorize right-hand sword placement");
 // Reproduce V66.98 retail failure: failed arming must leave OFF reachable.
 Reset();g_weapon_control_mode=3;armSuccess=false;armCalls=0;
 const auto oldRequest=g_weapon_request_epoch6694.load();CycleWeaponControlModeImpl();
 Check(WeaponControlModeImpl()==0&&g_weapon_request_epoch6694.load()>oldRequest,
       "rejected physical activation transitions through OFF, never traps rung 3");
 CycleWeaponControlModeImpl();Check(WeaponControlModeImpl()==1&&armCalls==1,
       "press after rejected activation advances to reference rather than retries forever");
 Reset();armSuccess=true;bool ladder=true;
 for(int expected:{1,2,3,4,5,6,7,0,1,2,3,4,5,6,7,0}){CycleWeaponControlModeImpl();ladder=ladder&&WeaponControlModeImpl()==expected;}
 Check(ladder,"two complete production F12 cycles include prediction and OFF");
 predictionArmed120=false;SetWeaponControlModeImpl(7);Check(WeaponControlModeImpl()==0,"failed prediction contract returns to OFF");predictionArmed120=true;
 Reset();g_weapon_control_mode=2;Store(self.data()+8,std::uint32_t(99));
 readCalls=handOwnerReads=0;Step();
 std::printf("unrelated_attachment_reads=%d henry_owner_reads=%d\n",readCalls,handOwnerReads);
 Check(handOwnerReads==0&&readCalls<=3&&lookupCalls==0,
       "unrelated attachment uses at most three guarded reads and never walks Henry hand owner");
 Reset();g_weapon_control_mode=2;Store(handSlots.data()+0x260,std::uint64_t(0));Step();
 Check(g_weapon_applied6694==0,"cheap candidate ID never authorizes a sheathed weapon");
 // Actual handover wrapper, with typed physical leaf outcomes.
 Reset();frameValid=true;g_weapon_control_mode=3;Step();const auto gripBefore=g_attachment6689.grip;
 physicalResult101=PhysicalAttachmentResult101::Waiting;SetWeaponControlModeImpl(4);Step();
 Check(g_attachment6689.gripValid&&!g_attachment6689.changed&&g_weapon_applied6694==0,
       "full handling WAITING cannot bypass grip/contact policy through legacy follow");
 Check(!std::memcmp(&gripBefore,&g_attachment6689.grip,sizeof(gripBefore)),"inactive backup grip is preserved without any placement ownership");
 const int nativeBefore=callsNative;physicalResult101=PhysicalAttachmentResult101::Placed;Step();
 Check(g_attachment6689.gripValid&&!g_attachment6689.changed&&callsNative==nativeBefore,
       "explicit PLACED invokes neither native restoration nor competing fallback");
 physicalResult101=PhysicalAttachmentResult101::Busy;Step();Check(callsNative==nativeBefore,"BUSY never invokes a competing fallback writer");
 physicalResult101=PhysicalAttachmentResult101::Waiting;Step();Check(!g_attachment6689.changed&&g_attachment6689.gripValid&&callsNative==nativeBefore+1,"physical-output wait restores native placement exactly once without legacy takeover");
 Store(handSlots.data()+0x260,std::uint64_t(0));Step();Check(!g_attachment6689.changed&&callsNative==nativeBefore+2,"native sheathed placement remains outside legacy ownership");
 Reset();frameValid=true;g_weapon_control_mode=3;Step();g_weapon_control_mode=4;physicalResult101=PhysicalAttachmentResult101::Restored;Step();
 Check(!g_attachment6689.gripValid&&!g_attachment6689.changed,"explicit RESTORED revokes legacy backup instead of announcing takeover");
 std::printf("controller_adapter=%d/%d native_consumers=MOCKED headset=NOT_RUN\n",pass,total);return pass==total?0:1;
}
