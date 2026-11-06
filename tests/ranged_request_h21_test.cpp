#include "subsystems/ranged_reload/crossbow_rifle_reload_drive60.h"
#include "subsystems/ranged_reload/weapon_kind_xr32.h"
#include "vrik/pose_math.h"
#include "vrik/physical_weapon.h"
#include "subsystems/ranged_reload/crossbow_native_xr34.h"
#include "tracking_sample59c6.h"
#include <mutex>
#include <optional>
#include <array>
#include <atomic>
#define EXCEPTION_EXECUTE_HANDLER 1
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <cassert>
#include <functional>
#include <string>
namespace kcd2vr::frik_pose_probe {
namespace pm=kcd2vr::pose_core;
namespace animation_filter {static std::atomic<bool>nativeBodyOwned112{false};}
namespace diagnostics::lab {static bool NativeReference(){return false;}}
namespace action_time_owner59c11 {static bool ready=true;static bool CrossbowNativeReadyXR34(){return ready;}}
static std::atomic<bool> g_reload_pose_ready_xr34{true};
namespace ph=kcd2vr::physical; // production pose transform math, not a translation-only stub
struct Profile {ph::Pose nativeWeapon{};bool nativeSeed=true;int classId=13;bool primary=false;std::uint64_t key=22,generation=1;ph::Pose grip[2];};
struct PhysFrame6698 {void* player=nullptr;void* manager=nullptr;std::uint64_t request=7;
 struct Input {std::uint64_t sample=1,bodyEpoch=8,epoch=7,tick=1000;bool allowed=true,tracked[2]{true,true};ph::Pose hands[2];double grip[2]{};}input;
 struct Output {bool active[2]{true,true},owned[2]{true,true};ph::Pose weapons[2];}output;
 Profile profiles[2];struct Correlation{pm::Stamp stamp{};}correlation126[2];float world[8]{0,0,0,1,0,0,0,1};};
struct V3{float x=0,y=0,z=0;};struct QuatT28{float q[4]{0,0,0,1},t[3]{};};
struct Matrix34V6550 {float m[12]{};};
struct CommittedFrame6695{pm::Stamp stamp{};std::uint64_t doorTrackingEpoch66=0;QuatT28 native[2]{},allowed[2]{};float world[8]{0,0,0,1,0,0,0,1};bool valid[2]{true,true};};
static CommittedFrame6695 body{};static bool bodyRead=true;
static bool ReadCommittedPacket6695(CommittedFrame6695& out){out=body;return bodyRead;}
static ph::Pose PhysPose6698(const QuatT28& q){return {{q.q[0],q.q[1],q.q[2],q.q[3]},{q.t[0],q.t[1],q.t[2]}};}
static QuatT28 Qt(ph::Pose p){return {{float(p.q.x),float(p.q.y),float(p.q.z),float(p.q.w)},{float(p.p.x),float(p.p.y),float(p.p.z)}};}
static QuatT28 AbsoluteFromRelative(const QuatT28&a,const QuatT28&b){return Qt(ph::compose(PhysPose6698(a),PhysPose6698(b)));}
static QuatT28 RelativeFromAbsolute(const QuatT28&a,const QuatT28&b){return Qt(ph::compose(ph::inverse(PhysPose6698(a)),PhysPose6698(b)));}
static V3 FromT(const QuatT28&q){return {q.t[0],q.t[1],q.t[2]};}
static V3 Sub(V3 a,V3 b){return {a.x-b.x,a.y-b.y,a.z-b.z};}
static double Len(V3 a){return std::sqrt(double(a.x)*a.x+double(a.y)*a.y+double(a.z)*a.z);}
static bool ValidPose6689(const QuatT28&q){return ph::finite(PhysPose6698(q));}
static ph::Pose PhysWorld6698(const float* w){return {{w[0],w[1],w[2],w[3]},{w[4],w[5],w[6]}};}

static unsigned char playerMem[0x300]{},equipmentMem[100]{},weaponMem[100]{},ammoMem[100]{},managerMem[100]{};
// Explicit installed-helper capability only; no native helper is executed here.
struct HelperCapabilityH21 {bool enabled=true;bool is_enabled()const noexcept{return enabled;}};
static std::optional<HelperCapabilityH21> g_attachment_helper_transform_hook{HelperCapabilityH21{}};
static void* g_attachment_helper_transform_original=weaponMem;
static std::uint64_t now=1000;static int mode=0;static bool pins=true,frameRead=true,owner=true;static unsigned nativeCalls=0;
static std::function<void()> nativeCallback;
static PhysFrame6698 frame;
struct Mailbox {bool read(PhysFrame6698& o){o=frame;return frameRead;}}g_phys_frame6698;
static std::atomic<std::uint64_t>g_phys_ready_sample6698{1},g_weapon_request_epoch6694{7},g_pose_epoch6689{8};
static std::atomic<void*>g_henry_item_attach_manager{managerMem},g_henry_char_attachment_manager{nullptr};
static std::atomic<bool>g_reload_ready36{true},g_gameplay_allowed{true},g_head_valid{true},g_left_valid{true},g_right_valid{true};
static unsigned g_stage_thread596=10;
static unsigned GetCurrentThreadId(){return 10;}static std::uint64_t GetTickCount64(){return now;}
static std::uintptr_t WhBase(){return 0x180000000ull;}
namespace retail_build112 {static bool Ready(std::uintptr_t){return pins;}}
static bool SehCopy146(const void*,void*,std::size_t);
template<class T>static bool Read(const void* p,T& o){if(reinterpret_cast<std::uintptr_t>(p)==WhBase()+0x2AAA00D)return SehCopy146(p,&o,sizeof(o));if(!p)return false;std::memcpy(&o,p,sizeof(o));return true;}
static bool ManualPlayer596(void* p){return owner&&p==playerMem;}
static bool reenterTrackingXR34=false;
static void*ManualEquipmentPtr59c9(void*){if(reenterTrackingXR34){reenterTrackingXR34=false;kcd2vr::tracking59c6::epoch.fetch_add(1);}return equipmentMem;}
static bool PhysVft6698(void* p,std::uintptr_t){return p==weaponMem||p==ammoMem;}
static bool NativeHandKey6697(void*,int side,std::uint64_t& key){key=frame.profiles[side].key;return true;}
static bool PhysEmpty6698(std::uint64_t k){return !k;}
static bool SehCopy146(const void* p,void* out,std::size_t n){if(!pins)return false;const auto a=reinterpret_cast<std::uintptr_t>(p)-WhBase();
 const unsigned char entry[]{0x48,0x89,0x5C,0x24,0x10,0x48,0x89,0x6C,0x24,0x18,0x48,0x89,0x74,0x24,0x20,0x57};
 const unsigned char caller[]{0xE8,0xAA,0xC8,0xD9,0xFE};if(a==0x18468BC&&n==16){std::memcpy(out,entry,n);return true;}if(a==0x2AAA00D&&n==5){std::memcpy(out,caller,n);return true;}return false;}
static int ReloadMode47C2(){return mode;}static bool ReloadDiagnostics175(){return mode>0;}
namespace diagnostic_trace {static void Write(const char*){}}
static void InvalidateReadCache101R2(){}

// Native leaves required by the additional actual attachment contribution.
// This legacy fixture only exercises source admission and request transport;
// the separate XR34 native/pose fixtures exercise placement and current child.
static bool Safe(void*,std::size_t){return true;}
static void* EntityById6694(unsigned){return nullptr;}
static void* ManualFindHandAttachment19c4r4(void*,const char*){return nullptr;}
static bool ManualBoundEntity19c4r4(void*,unsigned,void*){return false;}
static bool ResolveJointByNameExactV6545(void*,const char*,int&){return false;}
struct ManualItems596{void* weapon=nullptr;void* component=nullptr;bool loaded=false;std::uint64_t weaponKey=0,nockedKey=0,ammoKey=0;};
static bool ManualItemsRead596(void*,ManualItems596&){return false;}
static bool ManualNockAttached19c4r4(const ManualItems596&){return false;}
static bool EntityHiddenFlag59c14(void*,bool&){return false;}
struct ModelWorld6692{pm::Quat q{};V3 t{};float scale=1;};
static bool PhysAttachmentWorld164(void*,std::uintptr_t,ModelWorld6692&){return false;}
static bool ReadAttachmentQs6689(void*,float(&)[8]){return false;}
static ph::Pose PhysCurrentWorld164(ph::Pose p,const ModelWorld6692& w){p.p=p.p*w.scale;return ph::compose({w.q,{w.t.x,w.t.y,w.t.z}},p);}
static bool PlaceBothConsumers6689(void*,void*,const float(&)[8],bool&){return false;}
static bool PlacementReadback6689(void*,void*,unsigned,const float(&)[8],bool&){return false;}
static bool GetEntityWorldTmV6552(void*,unsigned,Matrix34V6550&){return false;}
static bool ManualArrowHelper64(void*,QuatT28&) noexcept{return false;}

#define near
#include "subsystems/ranged_reload/crossbow_rifle_request_h21.inl"
static unsigned checks=0;static void ck(bool x){++checks;if(!x){std::printf("FAIL check%u\n",checks);std::abort();}}
template<class T>static void put(unsigned char* p,T v){std::memcpy(p,&v,sizeof(v));}
static void Native(void* eq,bool a,bool b){++nativeCalls;ck(eq==equipmentMem&&a&&!b);if(nativeCallback){auto fn=std::move(nativeCallback);nativeCallback={};fn();}}
static void reset(int m=2){now=1000;mode=m;pins=frameRead=owner=true;nativeCalls=0;g_ranged_request_h21={};g_ranged_fault_h21=false;
 g_ranged_pass_player_h21=nullptr;g_ranged_pass_key_h21=0;g_ranged_prior_mode_h21=0;g_ranged_replay_h21=false;
 g_ranged_native_h21={WhBase(),&Native};frame={};frame.player=playerMem;frame.manager=managerMem;frame.profiles[1].primary=true;
 frame.profiles[0].primary=false;g_phys_ready_sample6698=frame.input.sample;
 put(equipmentMem+0x40,static_cast<void*>(weaponMem));put(equipmentMem+0x48,static_cast<void*>(ammoMem));
 put(equipmentMem+0x50,reinterpret_cast<void*>(0x123456));put(weaponMem+0x30,std::uint64_t(22));put(ammoMem+0x30,std::uint64_t(33));
 g_head_valid=g_left_valid=g_right_valid=g_gameplay_allowed=true;nativeCallback={};bodyRead=true;
 body={};body.stamp={8,frame.input.sample,now,1234};
 kcd2vr::tracking59c6::epoch=77;body.doorTrackingEpoch66=77;reenterTrackingXR34=false;
 for(int side=0;side<2;++side)frame.correlation126[side].stamp=body.stamp;
 xcXR34::registrationReader=&RangedRegistrationReadyXR34;xcXR34::WithdrawRegistration();}
static void tick(std::uint64_t at,double grip,pm::Vec3 p={}){now=at;frame.input.tick=now;++frame.input.sample;g_phys_ready_sample6698=frame.input.sample;frame.input.grip[0]=grip;frame.input.hands[0].p=p;CrossbowRifleTickH21();}
}
int main(){using namespace kcd2vr::frik_pose_probe;
 ck(RangedRegistrationReadyXR34());g_attachment_helper_transform_hook->enabled=false;
 ck(!RangedRegistrationReadyXR34());g_attachment_helper_transform_hook->enabled=true;
 ck(RangedKindH21(10)==rrH21::Weapon::Crossbow&&RangedKindH21(14)==rrH21::Weapon::Crossbow&&RangedKindH21(15)==rrH21::Weapon::Crossbow);
 ck(RangedKindH21(13)==rrH21::Weapon::Rifle&&RangedKindH21(9)==rrH21::Weapon::None);
 for(int m:{0,1}){reset(m);ck(!CrossbowRifleDeferH21(equipmentMem,1,0));ck(!g_ranged_request_h21.pending&&nativeCalls==0);}
 reset();frame.profiles[1].classId=9;ck(!CrossbowRifleDeferH21(equipmentMem,1,0)); // bow unchanged
 reset();ck(!CrossbowRifleDeferH21(equipmentMem,1,1));ck(!g_ranged_request_h21.pending);
 reset();pins=false;ck(!CrossbowRifleDeferH21(equipmentMem,1,0));
 reset();ck(CrossbowRifleDeferH21(equipmentMem,1,0));auto begin=g_ranged_request_h21.begin;
 ck(CrossbowRifleDeferH21(equipmentMem,1,0)&&g_ranged_request_h21.begin==begin);
 tick(1010,0);ck(g_ranged_request_h21.pending&&nativeCalls==0); // entering rung2 must not replay
 tick(1050,.9);tick(1250,.9,{.12,0,0});tick(1350,0,{.12,0,0});ck(nativeCalls==0&&g_ranged_request_h21.drive.machine.stage==rrH21::Stage::Working);
 tick(1400,.9);tick(1450,0);ck(nativeCalls==1&&!g_ranged_request_h21.pending);
 ck(!CrossbowRifleDeferH21(equipmentMem,1,0)&&nativeCalls==1);tick(1500,0);ck(nativeCalls==1);
 reset();ck(CrossbowRifleDeferH21(equipmentMem,1,0));mode=1;tick(1010,0);ck(nativeCalls==1&&!g_ranged_request_h21.pending);
 reset();ck(CrossbowRifleDeferH21(equipmentMem,1,0));tick(13001,0);ck(nativeCalls==1&&!g_ranged_request_h21.pending);
 reset();ck(CrossbowRifleDeferH21(equipmentMem,1,0));g_left_valid=false;
 ck(!CrossbowRifleDeferH21(equipmentMem,1,0));ck(!g_ranged_request_h21.pending&&nativeCalls==0);tick(1010,0);ck(nativeCalls==0); // naturally passing original already owns fallback
 reset();ck(CrossbowRifleDeferH21(equipmentMem,1,0));put(weaponMem+0x30,std::uint64_t(99));tick(1010,0);ck(nativeCalls==0&&!g_ranged_request_h21.pending);
 reset();ck(CrossbowRifleDeferH21(equipmentMem,1,0));nativeCallback=[&](){ck(!g_ranged_request_h21.pending);ck(!CrossbowRifleDeferH21(equipmentMem,1,0));};mode=0;tick(1010,0);ck(nativeCalls==1);
 // XR32: translating weapon and hand together cannot earn a physical span.
 reset();ck(CrossbowRifleDeferH21(equipmentMem,1,0));tick(1010,0);tick(1050,.9);
 frame.output.weapons[1].p={1,0,0};tick(1250,.9,{1,0,0});tick(1350,0,{1,0,0});
 ck(nativeCalls==0&&g_ranged_request_h21.pending&&g_ranged_request_h21.drive.machine.stage==rrH21::Stage::Empty);
 // An actual relative pull on the same translated weapon still completes.
 tick(1400,.9,{1,0,0});tick(1650,.9,{1.12,0,0});tick(1700,0,{1.12,0,0});
 ck(nativeCalls==0&&g_ranged_request_h21.drive.machine.stage==rrH21::Stage::Working);
 tick(1750,.9,{1,0,0});tick(1800,0,{1,0,0});ck(nativeCalls==1);
 // Crossbows always leave the original request untouched, including repeat
 // requests on the same weapon. They no longer inherit H21's pass marker.
 for(int cls:{10,14,15}){reset();frame.profiles[1].classId=cls;
   ck(!CrossbowRifleDeferH21(equipmentMem,1,0)&&!g_ranged_request_h21.pending&&nativeCalls==0);
   ck(!CrossbowRifleDeferH21(equipmentMem,1,0)&&!g_ranged_request_h21.pending&&nativeCalls==0);}
 reset();frame.profiles[1].classId=14;xcXR34::RefreshRegistration();
 put(playerMem+0x30,unsigned(77));xcXR34::Source source{};
 ck(RangedSourceXR34(source)&&source.weaponItem==reinterpret_cast<std::uintptr_t>(weaponMem)&&source.ammoKey==33);
 ck(source.sample==body.stamp.sample&&source.tick==body.stamp.tick&&source.character==body.stamp.actor);
 std::printf("XR34 initial producer request=%llu inputEpoch=%llu poseTracking=%llu sourceTracking=%llu\n",(unsigned long long)source.request,(unsigned long long)frame.input.epoch,(unsigned long long)body.doorTrackingEpoch66,(unsigned long long)source.tracking);std::fflush(stdout);
 ck(frame.input.epoch==7&&source.request==7&&source.tracking==77); // request and tracking are distinct original identities
 body.doorTrackingEpoch66=0;ck(!RangedSourceXR34(source));body.doorTrackingEpoch66=77;
 kcd2vr::tracking59c6::epoch=78;ck(!RangedSourceXR34(source)); // current epoch cannot restamp old pose
 frame.input.epoch=78;frame.request=78;g_weapon_request_epoch6694=78;ck(!RangedSourceXR34(source)); // accidental request==new tracking grants nothing
 frame.input.epoch=7;frame.request=7;g_weapon_request_epoch6694=7;kcd2vr::tracking59c6::epoch=77;
 body.stamp.tick=999;frame.correlation126[1].stamp.tick=999;ck(!RangedSourceXR34(source));body.stamp.tick=1000;frame.correlation126[1].stamp.tick=1000;
 reenterTrackingXR34=true;ck(!RangedSourceXR34(source)); // actual equipment leaf reenters while assembling source
 kcd2vr::tracking59c6::epoch=77;ck(RangedSourceXR34(source)&&source.tracking==77&&source.request==7);
 frame.input.epoch=8;frame.request=8;g_weapon_request_epoch6694=8;ck(RangedSourceXR34(source)&&source.tracking==77&&source.request==8);
 frame.input.epoch=7;frame.request=7;g_weapon_request_epoch6694=7;
 body.doorTrackingEpoch66=78;kcd2vr::tracking59c6::epoch=78;ck(RangedSourceXR34(source)&&source.tracking==78&&source.request==7);
 body.doorTrackingEpoch66=77;kcd2vr::tracking59c6::epoch=77;
 body.stamp.sample++;ck(!RangedSourceXR34(source));body.stamp.sample--;
 animation_filter::nativeBodyOwned112=true;ck(!RangedSourceXR34(source));animation_filter::nativeBodyOwned112=false;
 bodyRead=false;ck(!RangedSourceXR34(source));
 std::printf("PASS %u actual ranged capture/producer/replay checks; native request leaf mocked; no loaded/ammo claim\n",checks);
}


namespace kcd2vr::frik_pose_probe {
// This retained request fixture has no installed native body/passenger owner.
static bool RangedReloadSourceXR34(kcd2vr::crossbow_native_xr34::Source&) noexcept{return false;}
static void RangedReloadEquipmentXR34() noexcept{}
static void SeedReloadEquipmentXR34(const kcd2vr::crossbow_native_xr34::Source&) noexcept{}
static bool ReloadContextXR34(const kcd2vr::crossbow_native_xr34::Owner&,std::uint64_t) noexcept{return true;}
static bool ReloadNativeOwnerXR34(const kcd2vr::crossbow_native_xr34::Owner&,bool) noexcept{return true;}
static bool ReloadPoseWriterCurrentXR34(const kcd2vr::crossbow_native_xr34::Owner&,std::uint64_t) noexcept{return true;}
}
