#include "diagnostics/lab_state.h"
#include "subsystems/masterstrike_combos/native_kill_anim_nf.h" // C29: coordinator stand-down
#include "vrik/impale_hold60.h"
#include "subsystems/gore/impale_constraint_h21.h"
#include "vrik/visible_failure.h"
// Full production impale coordinator; only native status/owner/action leaves
// are doubled. Exact successor owner/status adapter is tested separately.
#include "vrik/part_anchor59c7g.h"
#include "vrik/blood49.h"
#include "vrik/kick_style26.h"
#include "vrik/contact_contract176.h"
#include "vrik/contact_authority176.h"
#include "subsystems/gore/impale_observe_h7.h"
#include <cassert>
#include <cstdio>
#include <string>
#include <vector>
namespace ph=kcd2vr::physical;namespace pm=kcd2vr::pose_core;
static std::uint64_t nowH18=1000;static std::uint64_t GetTickCount64(){return nowH18;}
static std::atomic<std::uint64_t> g_weapon_request_epoch6694{3},g_pose_epoch6689{2},g_pose_policy_generation57{2},g_phys_ready_sample6698{1};
static std::atomic<bool> g_gameplay_allowed{true},g_ctx_menu{false},g_ctx_dialogue{false},g_ctx_cutscene{false};
namespace kcd2vr::tracking59c6{static std::atomic<std::uint64_t> epoch{1};}
namespace animation_filter{static std::atomic<bool> nativeBodyOwned112{false};} // C29: native kill stand-down + corpse stab entity lookup reached from the coordinator
namespace animation_filter{inline void* PlayerSkeletonAnim() noexcept{return nullptr;}}
static void* FlinchEntityById178r11(unsigned) noexcept{return nullptr;}

namespace diagnostics::capture168{enum class Domain{Combat};static bool active(Domain,std::uint64_t){return true;}}
namespace diagnostic_trace{static std::vector<std::string> lines;static void Write(const char* s){lines.push_back(s);}}
struct PhysFrame6698{ph::Input input{};ph::Output output{};std::array<ph::Profile,2> profiles{};
 std::uint64_t request=0;float world[8]{};double publishWorldDeltaM122=0,publishWorldDeltaRad122=0;};
static kcd2vr::core::SnapshotMailbox<PhysFrame6698> g_phys_frame6698;
struct ImpalePin178r15{bool valid=false;std::uint64_t serial=0,tick=0;float x=0,y=0,z=0;int mode40=0;float depth40=0;};
struct ImpaleSkip40{std::uintptr_t entity=0,body=0;std::uint64_t tick=0;};
static kcd2vr::core::SnapshotMailbox<ImpalePin178r15> g_impale_pin178r15;
static kcd2vr::core::SnapshotMailbox<ImpaleSkip40> g_impale_skip40;
static int F4Rung178r10(){return 3;}
static ph::Pose PhysWorld6698(const float* w){return {{w[0],w[1],w[2],w[3]},{w[4],w[5],w[6]}};}
static unsigned rootsH18=0,newContactsH18=0,statusH18=0,oldStatusH18=0,drivesH18=0;
static bool ownerH18=true,driveH18=true,reenterH18=false;
static bool legacyReadyH18=false,legacyPartH18=false;static unsigned statusAdvanceH18=0,rootAdvanceH18=0;
static unsigned revokePhysicsAfterStatusH21=0,revokeImpaleAfterStatusR93=0,referenceAfterStatusR93=0;
static ph::Vec3 corpsePointH18{0,.7,0};
static bool equippedH18=true;static bool NativeImpaleWeaponCurrentH18(const PhysFrame6698&,int side){return equippedH18&&side==0;}
namespace kcd2vr::impale_constraint_h21 {
static unsigned cancelCallsH21=0;
void Cancel(std::uint64_t)noexcept{++cancelCallsH21;}
bool MotorBlocked()noexcept{return false;}
const char* Failure()noexcept{return "NO_CURRENT_ACHIEVED_RIGID_BUDDY_PHYSICAL_IMPALE_ENTRY_NOT_CONNECTED";}
}
static bool StageImpaleConstraintH21(const PhysFrame6698&,const kcd2vr::partanchor59c7g::Identity&,int,ph::Vec3,ph::Vec3,ph::Vec3,std::uint64_t)noexcept{return false;}
static void ObserveImpaleOwnersH7(const kcd2vr::impale_observe_h7::Identity&,std::uint64_t,unsigned){}
static kcd2vr::impale47::Poll ImpaleRoot47(const kcd2vr::partanchor59c7g::Identity& id,std::uintptr_t& root,const char*& why){
 ++rootsH18;assert(id.body==0x440&&id.part==12);root=legacyReadyH18?0x440:0;why="ROOT_STILL_LIVING";nowH18+=rootAdvanceH18;
 return legacyReadyH18?kcd2vr::impale47::Poll::Ready:kcd2vr::impale47::Poll::NotYet;}
static bool ReadImpalePart59c7g(const kcd2vr::partanchor59c7g::Identity&,ph::Pose& part,double& scale){
 ++oldStatusH18;part={};part.p=corpsePointH18;scale=1;nowH18+=statusAdvanceH18;return legacyPartH18;}
static bool ReadImpaleCurrentCorpseH18(const kcd2vr::partanchor59c7g::Identity& id,ph::Pose& part,double& scale,const char*& why){
 ++statusH18;assert(id.body==0x990&&id.part==379);part={};part.p=corpsePointH18;scale=1;why="MOCK_CURRENT_OWNER";nowH18+=statusAdvanceH18;
 if(referenceAfterStatusR93&&!--referenceAfterStatusR93)kcd2vr::diagnostics::lab::Set(kcd2vr::diagnostics::lab::Key::F3,1);
 if(revokeImpaleAfterStatusR93&&!--revokeImpaleAfterStatusR93)kcd2vr::gore32::impaleHang=false;
 if(revokePhysicsAfterStatusH21&&!--revokePhysicsAfterStatusH21){kcd2vr::collab_h21::setPhysicsMode(0);kcd2vr::collab_h21::setPhysicsMode(3);}
 if(reenterH18)g_pose_policy_generation57.fetch_add(2);return ownerH18;}
static bool ReadImpaleSuccessorH18(const kcd2vr::partanchor59c7g::Identity& old,const ph::Contact103& c,bool dead,
 kcd2vr::partanchor59c7g::Identity& fresh,ph::Pose& part,double& scale,const char*& why){
 ++newContactsH18;assert(old.body==0x440&&old.part==12&&dead);
 if(c.native172.owner!=0x420||c.native172.entityId!=42||c.native172.collider!=0x990){why="MOCK_FOREIGN_OWNER";return false;}
 fresh={old.serial,old.epoch,old.request,old.target,old.entity,0x990,379};return ReadImpaleCurrentCorpseH18(fresh,part,scale,why);}
static bool bloodReenterH21=false;static double lastDriveSpeedH21=0;static unsigned rawAddsH21=0,rawRemovesH21=0;
namespace gore32n{
static void BloodV(kcd2vr::gore32::V3,kcd2vr::gore32::V3,std::uint32_t,const char*,float){if(bloodReenterH21){bloodReenterH21=false;g_pose_policy_generation57.fetch_add(2);}}
static bool PartVelocity(std::uintptr_t body,int part,kcd2vr::gore32::V3 v){lastDriveSpeedH21=kcd2vr::gore32::len(v);assert(body==0x990&&part==379);++drivesH18;return driveH18;}
static unsigned PartHoldAdd60(std::uintptr_t,const kcd2vr::constraint_packet60::Packet&){++rawAddsH21;return 42;}
static bool PartHoldRemove60(std::uintptr_t,unsigned){++rawRemovesH21;return true;}
}
// The existing lethal/successor fixture never requests C40 display ownership;
// its evaluated-anchor path is covered by living_impale_c40_test instead.
struct LivingImpaleC40 {int joint=-1;};
static bool AcquireLivingImpaleC40(const kcd2vr::partanchor59c7g::Identity&,ph::Vec3,LivingImpaleC40&,ph::Pose&,double&){return false;}
static bool ReadLivingImpaleC40(const kcd2vr::partanchor59c7g::Identity&,const LivingImpaleC40&,ph::Pose&,double&){return false;}
#include "../src/vrik/impale_death_speed_r106.h" // R106
#include "../src/subsystems/gore/native_impale_part.inl"
static PhysFrame6698 frameH18;
static void tick(std::uint64_t t,bool newSample=true){nowH18=t;frameH18.input.tick=t;if(newSample)++frameH18.input.sample;
 g_phys_frame6698.publish(frameH18);g_phys_ready_sample6698=frameH18.input.sample;ImpalePartTick59c7g();}
static void retire(std::uint64_t t){g_gameplay_allowed=false;tick(t);g_gameplay_allowed=true;}
static bool pin(){ImpalePin178r15 p{};return g_impale_pin178r15.read(p)&&p.valid;}
static void request(std::uint64_t serial,std::uint64_t t,bool transformed=false){retire(t);frameH18={};frameH18.request=3;
 frameH18.input.epoch=3;frameH18.input.bodyEpoch=2;frameH18.input.sample=serial*100;
 frameH18.input.allowed=frameH18.input.tracked[0]=frameH18.input.contact=frameH18.input.haveWorld=true;
 frameH18.input.worldScale=1;frameH18.input.grip[0]=1;frameH18.input.impaleEnabled178r15=frameH18.input.impaleHang32=true;
 frameH18.input.hands[1].p={-1,0,0};frameH18.world[3]=frameH18.world[7]=1;corpsePointH18={0,.7,0};
 if(transformed){frameH18.world[2]=frameH18.world[3]=float(std::sqrt(.5));frameH18.world[4]=2;frameH18.world[5]=3;frameH18.world[6]=4;
  frameH18.world[7]=2;frameH18.input.worldScale=2;frameH18.input.world=PhysWorld6698(frameH18.world);
  corpsePointH18=ph::point(frameH18.input.world,ph::Vec3{0,1.4,0});}
 auto& o=frameH18.output;o.impaleLatchRequest=true;o.impaleSerial=serial;o.impaleEntity=42;o.impaleSide32=0;
 o.impaleTipWorld=corpsePointH18;o.impaleLocal32={0,.7,0};o.active[0]=o.owned[0]=o.contactKnown[0]=true;
 auto& p=frameH18.profiles[0];p.key=p.generation=1;p.side=0;p.primary=p.geometry=true;p.classId=4;p.tip={0,.9,0};
 auto& h=o.impaleHit47;h.serial=serial;h.tick=h.impactTick=t;h.attempt=77;h.key=h.generation=1;h.epoch=2;h.request=3;
 h.target=42;h.targetIdentity=0x420;h.collider=0x440;h.rawPart=12;h.kind=1;h.feature=int(kcd2vr::bodycombo59c7g::Feature::Tip);h.weaponClass=4;h.death=1;h.damage=25;h.energy=1;
 h.seen=h.accepted=h.applied=h.fields=h.bodyContact=h.continuation=true;h.blocked=h.perfect=h.parry=0;
 h.point[0]=float(corpsePointH18.x);h.point[1]=float(corpsePointH18.y);h.point[2]=float(corpsePointH18.z);
 ownerH18=driveH18=equippedH18=true;reenterH18=legacyReadyH18=legacyPartH18=false;statusAdvanceH18=rootAdvanceH18=0;tick(t);assert(!pin());}
static void contact(){auto& c=frameH18.output.contact119[0];c={};c.known=c.plane=true;c.fraction=.5;c.point={0,.7,0};
 auto& w=c.native172;w.mask=kcd2vr::npc_contact172::BodiesAndWorld;w.body=kcd2vr::npc_contact172::Body::Articulated;
 w.ownerRead=true;w.owner=0x420;w.entityId=42;w.collider=0x990;w.part=379;}
int main(){
 kcd2vr::collab_h21::setPhysicsMode(3);
 // The actual pending victim survives recognizer completion; no second hit or
 // native original is invoked while waiting for the delayed successor.
 request(1,1000);tick(2600);assert(!pin());contact();tick(4000);assert(pin()&&drivesH18>0&&newContactsH18==1&&oldStatusH18==0);
 const auto before=drivesH18;frameH18.input.tracked[0]=false;tick(4010);assert(!pin()&&drivesH18==before);
 // Original deadline, not the most recent publication/request timestamp.
 request(2,5000);const auto n0=newContactsH18;contact();tick(9500);assert(!pin()&&newContactsH18==n0&&drivesH18==before);
 tick(9510);assert(!pin()&&newContactsH18==n0); // same old serial cannot restart.
 // No fresh body contact -> no pin/driver, even after the corpse can exist.
 request(3,10000);tick(13000);assert(!pin()&&newContactsH18==n0);
 // A borrowed original part id is never substituted for the new contact.
 contact();frameH18.output.contact119[0].native172.collider=0x440;tick(13050);assert(!pin());
 contact();frameH18.output.contact119[0].point={.1,.7,0};const auto off=newContactsH18;tick(13100);assert(!pin()&&newContactsH18==off);
 contact();frameH18.output.contact119[0].limited115=true;tick(13150);assert(!pin()&&newContactsH18==off);
 contact();frameH18.output.contactKnown[0]=false;tick(13200);assert(!pin()&&newContactsH18==off);
 frameH18.output.contactKnown[0]=true;frameH18.publishWorldDeltaM122=.02;tick(13250);assert(!pin()&&newContactsH18==off);
 frameH18.publishWorldDeltaM122=0;contact();tick(13300);assert(pin()&&drivesH18>before);
 // Source loss is permanent for that original victim, even after recovery.
 request(4,15000);const auto d4=drivesH18;g_pose_policy_generation57.fetch_add(2);contact();tick(15100);assert(!pin()&&drivesH18==d4);
 tick(15200);assert(!pin()&&drivesH18==d4);
 request(5,17000);kcd2vr::tracking59c6::epoch.fetch_add(1);contact();tick(17100);assert(!pin()&&drivesH18==d4);
 request(6,19000);frameH18.profiles[0].generation=2;contact();tick(19100);assert(!pin()&&drivesH18==d4);
 request(7,21000);frameH18.input.grip[0]=0;contact();tick(21100);assert(!pin()&&drivesH18==d4);
 request(8,23000);frameH18.output.weapons[0].p={0,-.26,0};contact();tick(23100);assert(!pin()&&drivesH18==d4);
 // Actual native status callback changing source cannot authorize a first pin.
 request(9,25000);contact();reenterH18=true;tick(25100);assert(!pin()&&drivesH18==d4);reenterH18=false;
 // Refused/faulted existing Action ends the attachment and cannot retry it.
 request(10,27000);contact();driveH18=false;tick(27100);assert(!pin()&&drivesH18==d4+1);
 driveH18=true;tick(27200);assert(!pin()&&drivesH18==d4+1);
 // A victim-owned wait can outlive a missing recognizer packet, but it cannot
 // attach/drive without that original weapon accepting the first-pin serial.
 request(11,29000);frameH18.output.impaleLatchRequest=false;frameH18.output.impaleSerial=0;contact();const auto nc=newContactsH18;
 tick(32000);assert(!pin()&&newContactsH18==nc);tick(33500);assert(!pin()&&newContactsH18==nc);
 request(12,35000);equippedH18=false;contact();const auto d12=drivesH18;tick(35100);assert(!pin()&&drivesH18==d12);
 equippedH18=true;tick(35200);assert(!pin()&&drivesH18==d12);
 assert(oldStatusH18==0);
 // Actual native callback clocks can cross the original deadline despite an
 // otherwise fresh frame. Neither successor nor old-body status may acquire.
 request(13,37000);contact();statusAdvanceH18=30;const auto d13=drivesH18;
 tick(41490);assert(!pin()&&drivesH18==d13&&nowH18==41520);statusAdvanceH18=0;
 request(14,43000);legacyReadyH18=legacyPartH18=true;statusAdvanceH18=30;
 tick(47490);assert(!pin()&&drivesH18==d13&&nowH18==47520&&oldStatusH18==1);statusAdvanceH18=0;
 request(15,49000);legacyReadyH18=legacyPartH18=true;rootAdvanceH18=30;
 tick(53490);assert(!pin()&&drivesH18==d13&&nowH18==53520&&oldStatusH18==1);rootAdvanceH18=0;
 // Different scale packets must refuse BEFORE contact status/anchor creation.
 request(16,55000);contact();frameH18.world[7]=2;const auto n16=newContactsH18;
 tick(55100);assert(!pin()&&newContactsH18==n16&&drivesH18==d13);
 // Exact same geometry under world rotation, translation AND scale: the new
 // anchor/pin must be the transformed contact, never a model-space point.
 request(17,57000,true);contact();tick(60000);assert(pin()&&drivesH18>d13);
 ImpalePin178r15 transformedPin{};assert(g_impale_pin178r15.read(transformedPin));
 assert(std::abs(transformedPin.x-.6)<1e-5&&std::abs(transformedPin.y-3)<1e-5&&std::abs(transformedPin.z-4)<1e-5);
 // Already published attachments retain existing hang limits, not the first
 // admission deadline. Here the established pin survives beyond4500ms.
 const auto d17=drivesH18;tick(61600);assert(pin()&&drivesH18>d17);
 // H21: actual production coordinator, not just the feel header. Default
 // VrImpaleHang drives the same exact corpse-part servo with F9 OFF or ON.
 kcd2vr::collab_h21::setPhysicsMode(0);const auto offDrivesH21=drivesH18;request(18,63000);contact();tick(63100);
 frameH18.output.weapons[0].p.x=.02;tick(63120);assert(pin()&&drivesH18>offDrivesH21);
 kcd2vr::collab_h21::setPhysicsMode(3);request(19,65000);contact();tick(65100);
 frameH18.output.weapons[0].p.x=.02;tick(65120);assert(pin()&&drivesH18>offDrivesH21&&lastDriveSpeedH21>0);
 // A raw buddy-provider pointer is deliberately insufficient to enqueue an Add.
 kcd2vr::impale_hold60::weaponBodyProvider.store(+[](int) noexcept -> std::uintptr_t{return 0x123456789ULL;});
 for(unsigned n=1;n<=55;++n){frameH18.input.hands[1].p.x=-1.0-.002*n;tick(65120+n*20);assert(pin());}
 assert(rawAddsH21==0&&rawRemovesH21==0); // moving offhand cannot own/remove/re-add blade
 bool missingTypedH21=false;for(const auto& line:diagnostic_trace::lines)if(line.find("NO_CURRENT_ACHIEVED_RIGID_BUDDY_PHYSICAL_IMPALE_ENTRY_NOT_CONNECTED")!=std::string::npos)missingTypedH21=true;
 assert(missingTypedH21);const auto beforeGripH21=drivesH18;frameH18.input.grip[0]=0;tick(66240);assert(!pin()&&drivesH18==beforeGripH21);
 kcd2vr::collab_h21::setPhysicsMode(0);kcd2vr::impale_hold60::weaponBodyProvider.store(nullptr);
 request(20,68000);contact();const auto beforeBloodH21=drivesH18;bloodReenterH21=true;tick(68100);assert(!pin()&&drivesH18==beforeBloodH21);
 kcd2vr::collab_h21::setPhysicsMode(3);request(21,70000);contact();tick(70100);assert(pin());
 const auto beforeRevokeH21=drivesH18;revokePhysicsAfterStatusH21=2;frameH18.output.weapons[0].p.x=.02;tick(70120);
 assert(pin()&&drivesH18>beforeRevokeH21); // unrelated F9 change never revokes the independent servo
 const auto beforeOptionR93=drivesH18;revokeImpaleAfterStatusR93=2;tick(70140);
 assert(!pin()&&drivesH18==beforeOptionR93); // option OFF during native readback cannot write after callback
 kcd2vr::gore32::impaleHang=true;
 request(121,71000);contact();tick(71100);assert(pin());
 const auto beforeReferenceR93=drivesH18;referenceAfterStatusR93=2;tick(71120);
 assert(!pin()&&drivesH18==beforeReferenceR93); // native callback may switch to native reference
 kcd2vr::diagnostics::lab::Set(kcd2vr::diagnostics::lab::Key::F3,0);
 kcd2vr::collab_h21::setPhysicsMode(0); // exclusive corpse ownership also holds without F9
 // D40: the actual producer cannot take over a corpse already held by a hand.
 namespace ownerD40=kcd2vr::corpse_body_owner_d40;
 ownerD40::Lease handD40;
 const ownerD40::Key handKeyD40{ownerD40::Slot::LeftHand,0x420,0x990,71,2,380};
 request(22,72000);assert(ownerD40::claims.acquire(handD40,handKeyD40));contact();const auto blockedDrivesD40=drivesH18;
 tick(72100);assert(!pin()&&drivesH18==blockedDrivesD40);ownerD40::claims.release(handD40);
 tick(72120);assert(!pin()&&drivesH18==blockedDrivesD40); // original refused request cannot restart itself
 request(23,74000);contact();tick(74100);assert(pin()&&drivesH18>blockedDrivesD40);
 assert(!ownerD40::claims.acquire(handD40,handKeyD40)); // live servo reserves the whole victim, not only its part
 retire(74120);assert(ownerD40::claims.acquire(handD40,handKeyD40));ownerD40::claims.release(handD40);
 const ownerD40::Key unrelatedD40{ownerD40::Slot::LeftHand,0x1420,0x1990,72,2,380};
 assert(ownerD40::claims.acquire(handD40,unrelatedD40));request(24,76000);contact();tick(76100);assert(pin());
 ownerD40::claims.release(handD40);retire(76120);
 std::puts("PASS H18 actual coordinator: original lifetime/current source checks, per-victim hand exclusion, no cross-victim global block, no old-part transfer or repeated hit");
}
