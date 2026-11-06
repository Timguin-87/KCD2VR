// Actual current PhysPrepare6698 and frame foundation are extracted by the runner.
// Native calls are not executed; mailbox failure and profile-fill are explicit fixture leaves.
#include "vrik/physical_weapon.h"
#include "subsystems/masterstrike_combos/draw_signal_d63.h"
#include "vrik/tracked_frame51.h"
#include <atomic>
#include <cstdio>
#include <array>
namespace ph=kcd2vr::physical;
namespace tf=kcd2vr::tracked_frame51;
namespace pm=kcd2vr::pose_core;
static std::uint64_t now=1000;
static std::uint64_t GetTickCount64(){return now;}
struct PhysEquipmentItem6698{ph::Profile profile{};bool held=false;};
struct PhysEquipment6698{bool valid=false;std::uint64_t request=0,epoch=0,tick=0;bool handKnown[2]{};std::uint64_t handKey[2]{};int heldIndex[2]{-1,-1};std::array<PhysEquipmentItem6698,4> items{};void* player=nullptr;void* manager=nullptr;int count=0;};
struct Grip{std::uint64_t key=0,generation=0;unsigned nativeStamp=0,nativeSocket126=0;bool ready[2]{};};
struct PhysGripBank6698{std::array<Grip,4> grip{};};
struct PhysCorrelation126{unsigned stamp=0,socket=0;};
struct BowGeometry166{};
template<class T>struct Mailbox {T value{};bool readable=true;bool read(T& x){if(!readable)return false;x=value;return true;}};
static Mailbox<PhysEquipment6698>g_phys_equipment6698;
static Mailbox<PhysGripBank6698>g_phys_grips6698;
static Mailbox<BowGeometry166>g_bow_geometry166;
static std::atomic<bool>g_phys_armed6698{true},g_phys_fault6698{false},g_live_attributes_all_rungs59c14{false};
static std::atomic<int>g_weapon_control_mode{6};
static std::atomic<std::uint64_t>g_weapon_request_epoch6694{7},g_pose_epoch6689{1};
static std::atomic<void*>g_last_player_entity{nullptr},g_henry_item_attach_manager{reinterpret_cast<void*>(0x400)};
static std::atomic<unsigned>g_physical_gates101R2[2]{};
namespace diagnostics::lab {bool NativeReference(){return false;}}
namespace diagnostic_trace {void Write(const char*){}}
namespace kcd2vr::live5912 {struct Snapshot{double strength=0,agility=0,stamina=1,strengthEffective=0,agilityEffective=0;int skills26[5]{};};static Mailbox<Snapshot> latest;template<class... A>bool readBounded(A...){return false;}}
namespace kcd2vr::kick26 {inline std::atomic<std::uint64_t> exhaustedSeenCLE{0};inline std::atomic<bool>henryExhaustedCLE{false};}
// Explicit fixture leaf: metadata propagation is identical to the first line of production PhysFillProfile6698;
// native weapon geometry is outside this camera-identity reproduction.
static void PhysFillProfile6698(const PhysEquipmentItem6698&i,const Grip&g,ph::Profile&p){p=i.profile;p.primary=g.ready[p.side];}
static bool PhysApplyBowGeometry166(const BowGeometry166&,const PhysEquipment6698&,ph::Profile&,std::uint64_t){return false;}
#include "actual_prepare.inc"
#include "vrik/bow_camera_identity_d37b.h"
static auto BowCameraEquipmentD37B(const PhysEquipment6698& e,bool prepared,std::uint64_t request,std::uint64_t epoch,std::uint64_t at){return kcd2vr::bow_camera_identity_d37b::qualify(e,{g_henry_item_attach_manager.load(),g_weapon_request_epoch6694.load(),g_pose_epoch6689.load(),request,epoch,at,prepared});}
static bool BowGripDrawEnabled596C1(){return true;}
static tf::Input ProductionFoundationD37B(const PhysEquipment6698& equipment,bool prepared169r2,std::uint64_t preparedRequest169r2,const std::array<ph::Profile,2>& profiles){
 struct {PhysEquipment6698 physicalEquipment;std::uint64_t epoch;}tx{equipment,1};
 struct {double right_grip=1,right_trigger=0;}ts;
 struct {bool bowHeld=false;}priorBow169r2;
 const auto& bowProfile169r2=profiles[1];const bool sameBow169r2=false;
 tf::Input fi51{};
 #include "actual_foundation.inc"
 return fi51;
}
static unsigned checks=0,failures=0;
static void ck(bool ok,const char* msg){++checks;failures+=!ok;std::printf("%s %s\n",ok?"PASS":"FAIL",msg);}
int main(){
 auto& e=g_phys_equipment6698.value;e.valid=true;e.request=7;e.epoch=1;e.tick=now;e.handKnown[0]=e.handKnown[1]=true;e.handKey[1]=0xB0;e.heldIndex[1]=0;
 e.player=reinterpret_cast<void*>(0x300);e.manager=reinterpret_cast<void*>(0x400);e.count=1;e.items[0].held=true;
 auto& p=e.items[0].profile;p.key=0xB0;p.generation=3;p.side=1;p.classId=9;
 auto& g=g_phys_grips6698.value.grip[0];g.key=p.key;g.generation=p.generation;g.ready[1]=true;
 ph::Input input{};std::array<ph::Profile,2> profiles{};PhysEquipment6698 copied{};
 tf::EyeState state{};tf::Frame frame{};frame.valid=frame.neutral=true;frame.stamp={1,10,now,0x100};frame.control=2;frame.root={10,20,0};frame.anchor=frame.nativeHead=frame.calibratedHead={0,0,1.7};frame.restOffset={0,0,.8};
 pm::Vec3 eye{},heading{};ck(tf::eye(state,frame,frame.stamp,2,now,{10,20,1.756},eye,{0,1,0},&heading)&&state.headingReady,"prime positively neutral eye and heading");
 auto run=[&](const char*label){bool prepared=PhysPrepare6698(input,profiles,copied);const auto fi=ProductionFoundationD37B(copied,prepared,input.epoch,profiles);frame.neutral=false;frame.bow=fi.bow;++frame.stamp.sample;frame.stamp.tick=++now;e.tick=now;
 bool camera=tf::eye(state,frame,frame.stamp,2,now,{10.05,20.04,1.68},eye,{.15,.988686,0},&heading);
 std::printf("%s prepared=%d knownEquipmentBow=%d producedProfileClass=%d frameBow=%d cameraAccepted=%d heading=(%.6f,%.6f)\n",label,prepared,copied.items[0].profile.bow(),profiles[1].classId,frame.bow,camera,heading.x,heading.y);return camera&&pm::length(heading)>.5;};
 ck(run("normal"),"matching current grip profile retains bow camera heading");
 g_phys_grips6698.readable=false;
 ck(run("mailbox_unavailable"),"same fresh exact native bow retains camera heading through grip mailbox gap");
 ck(copied.handKey[1]==0xB0&&copied.items[0].profile.classId==9,"equipment still positively identifies same bow on failed grip read");
 ck(profiles[1].classId==-1&&!profiles[1].primary,"camera identity grants no physical profile or primary-grip permission");
 g_phys_grips6698.readable=true;g.generation=2;
 ck(run("grip_generation_gap"),"same fresh exact native bow retains camera heading through current grip generation gap");
 g.generation=3;g.ready[1]=false;
 ck(run("matching_unready_grip"),"grip readiness alone does not drop bow camera classification");
 input.bowActions=false;
 ck(run("ammo_authority_false"),"later ammunition readiness alone does not drop bow camera classification");
 const auto exact=e;
 auto reject=[&](auto change,const char* label){e=exact;change(e);ck(!BowCameraEquipmentD37B(e,true,7,1,now),label);};
 reject([](auto&v){v.valid=false;},"reject invalid equipment");
 reject([](auto&v){v.player=nullptr;},"reject absent validated native player");
 reject([](auto&v){v.manager=nullptr;},"reject absent native manager");
 reject([](auto&v){v.manager=reinterpret_cast<void*>(0x900);},"reject foreign manager");
 reject([](auto&v){v.request=6;},"reject request mismatch");
 reject([](auto&v){v.epoch=2;},"reject body epoch mismatch");
 reject([](auto&v){v.tick=0;},"reject missing source timestamp");
 reject([](auto&v){v.tick=now+1;},"reject future equipment");
 reject([](auto&v){v.tick=now-101;},"reject stale equipment");
 reject([](auto&v){v.handKnown[1]=false;},"reject unknown native left hand");
 reject([](auto&v){v.handKey[1]=0;},"reject native empty hand");
 reject([](auto&v){v.count=0;},"reject no described items");
 reject([](auto&v){v.count=int(v.items.size())+1;},"reject overflowed item count");
 reject([](auto&v){v.heldIndex[1]=-1;},"reject no current held item");
 reject([](auto&v){v.heldIndex[1]=v.count;},"reject index beyond described items");
 reject([](auto&v){v.heldIndex[1]=int(v.items.size());},"reject array overflow");
 reject([](auto&v){v.items[0].held=false;},"reject stowed bow");
 reject([](auto&v){v.items[0].profile.side=0;},"reject wrong native side");
 reject([](auto&v){v.items[0].profile.classId=4;},"reject melee profile");
 reject([](auto&v){v.items[0].profile.key=0;},"reject absent item key");
 reject([](auto&v){v.items[0].profile.key=0xB1;},"reject mismatched held key");
 reject([](auto&v){v.items[0].profile.generation=0;},"reject unknown item generation");
 e=exact;
 ck(!BowCameraEquipmentD37B(e,false,7,1,now),"reject unavailable preparation; no old equipment fallback");
 ck(!BowCameraEquipmentD37B(e,true,0,1,now),"reject missing prepared request");
 ck(!BowCameraEquipmentD37B(e,true,7,0,now),"reject missing body epoch");
 g_weapon_request_epoch6694=8;ck(!BowCameraEquipmentD37B(e,true,7,1,now),"reject current request changed after preparation");g_weapon_request_epoch6694=7;
 g_pose_epoch6689=2;ck(!BowCameraEquipmentD37B(e,true,7,1,now),"reject current body changed after preparation");g_pose_epoch6689=1;
 const auto id=BowCameraEquipmentD37B(e,true,7,1,now);
 ck(bool(id)&&id.key==e.handKey[1]&&id.generation==3&&id.request==7,"qualification returns exact identity with no geometry facts");
 std::printf("checks=%u failures=%u\n",checks,failures);return failures?1:0;
}
