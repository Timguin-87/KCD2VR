// R9 lane B: why a squeeze did not become a hand door claim (door_press_why_r9.h + CandidatePress codes).
// harness copied from door_throw_admission_h41_test.cpp (same actual H40 header and current reader).
// Native leaves below are copied context/storage only; no game is executed.
#ifndef KCD2VR_HAND_DOOR_HEADER_R96
#define KCD2VR_HAND_DOOR_HEADER_R96 "../src/subsystems/doors/hand_only_h40.h"
#endif
#include KCD2VR_HAND_DOOR_HEADER_R96
#include "../src/subsystems/ranged_reload/reload_throw_claim_t25.h"
#include "../src/vrik/quick_talk_owner_h4.h"
#include "../src/core/native_grasp13.h"
#include <cassert>
namespace kcd2vr::door_use46 {inline std::atomic<bool> enabled{true};}
namespace kcd2vr::tracking59c6 {inline std::atomic<std::uint64_t> epoch{1};}
namespace kcd2vr::animation_filter {inline std::atomic<bool> nativeBodyOwned112{false};inline bool NativeKickActive175(){return false;}}
static bool boundaryBusy=false;
namespace kcd2vr::action_time_owner59c11 {bool CrossbowNativeAdmissionBusyXR34() noexcept{return boundaryBusy;}}
using namespace kcd2vr;
namespace h=hand_only_door_h40;namespace m=mutual_admission_t25;namespace ac=ammo_carry_t18;namespace xr=crossbow_native_xr34;
static std::atomic<bool> g_gameplay_allowed{true},g_pose_restore_fault6689{false},g_ctx_menu{false},g_ctx_dialogue{false},g_ctx_cutscene{false},g_ctx_reading{false},g_ctx_scripted{false},g_committed_pose_valid6689{true},g_native_held_equipment_pending_h36{false};
static std::atomic<unsigned> g_cal_state{2};
static std::atomic<std::uint64_t> g_pose_epoch6689{9},g_weapon_request_epoch6694{10},g_pose_policy_generation57{2};
static std::atomic<void*> g_last_player_entity{reinterpret_cast<void*>(0x5000)},g_committed_character6689{reinterpret_cast<void*>(0x3000)},g_henry_item_attach_manager{reinterpret_cast<void*>(0x4000)};
static std::atomic<std::uintptr_t> g_player_char37{0x3000};
static struct {std::atomic<unsigned> head{0},tail{0};} g_phys_commands6698;
static bool NativeHeldBusyT4(int){return false;}
struct TrackingSnapshot {std::uint64_t tick=1000,source_epoch=1,sequence=20;bool allowed=true,head_valid=true,head_held=false,left_valid=true,right_valid=true,left_held=false,right_held=false;float left_grip=.8f,right_grip=.8f,left_trigger=0,right_trigger=0;};
static TrackingSnapshot tracking;
static bool ReadTrackingSnapshot(TrackingSnapshot&t){t=tracking;return true;}
struct PhysEquipment6698 {bool valid=true,handKnown[2]{true,true};std::uint64_t tick=1000,epoch=9,request=10,handKey[2]{};void* manager=reinterpret_cast<void*>(0x4000);void* player=reinterpret_cast<void*>(0x2000);};
static core::SnapshotMailbox<PhysEquipment6698> g_phys_equipment6698;
static bool(*g_graspPendingUse55)(grasp13::PendingUse55&)=nullptr;
static bool(*g_doorActive66)(int,std::uint64_t,std::uint64_t,std::uint64_t&)=nullptr;
template<class T>static bool Read(const void*,T&){return false;}
#include "../src/subsystems/doors/hand_only_h40_current.inl"
static unsigned checks=0;
#define CK(x) do{++checks;assert(x);}while(0)
static void nothing(void*)noexcept{}
static h::Frame frame(){h::Frame f{};f.tick=1000;f.body=9;f.request=10;f.sample=20;f.tracking=1;f.policy=2;f.actor=0x5000;f.character=0x3000;f.nativePlayer=0x2000;f.manager=0x4000;f.player=42;f.permitted=true;f.available[0]=f.available[1]=true;f.grip[0]=f.grip[1]=.8f;return f;}
static void reset(){
 for(int side=0;side<2;++side){m::active[side]=m::doorClaimsH41[side]=0;h::claims[side]=h::entered[side]=0;h::neutral[side]=true;h::pendingInput[side]={};h::fallbackToken[side]=0;ac::claims[side].publish({});}
 ac::hands=0;boundaryBusy=false;xr::activeNativeLease=0;xr::cleanupInterested=false;xr::statusMailbox.publish({});
 h::vmQuarantine=false;h::admissionBusy=0;h::transportInstalled=true;h::enabled=true;h::ready=true;h::service={};h::currentReader=&HandOnlyDoorCurrentH40;h::PublishCompleted(frame());
 g_phys_equipment6698.publish({});tracking={};quick_talk_owner_h4::mailbox.publish({1000,9,10,0x2000,0});
}
static ac::Claim claim(){ac::Claim c{};c.serial=71;c.player=0x2000;c.manager=0x4000;c.character=0x3000;c.body=9;c.request=10;c.stage=ac::Stage::Pending;return c;}
namespace pw=door_press_why_r9;
static unsigned lastCode(int s){return pw::last[s].load();}
static void resetR9(){reset();for(int s=0;s<2;++s){h::pressWhy[s]={};h::lastBusyR9[s]=0;pw::last[s]=0;pw::refused[s]=0;pw::claimedCount[s]=0;}visible_failure::mailbox.publish({});visible_failure::logMailboxD80.publish({});visible_failure::lastLogPublishD80=0;}
// R90 makes this diagnostic log-only without an explicit F2 screen lease.
static bool receiptSaysR96(const char* text){visible_failure::Message m{},screen{};return visible_failure::logMailboxD80.read(m)&&std::strcmp(m.text,text)==0&&
 visible_failure::mailbox.read(screen)&&!screen.text[0];}
int main(){
 {pw::Press p{};CK(!p.note(pw::LeaseBusy));p.begin(1);CK(p.note(pw::LeaseBusy)&&!p.note(pw::PoseMissing)&&p.code==pw::LeaseBusy);
  p.claimed();CK(p.code==pw::None&&!p.note(pw::LeaseBusy));
  CK(!pw::visible(pw::HandBusy)&&!pw::visible(pw::OtherTarget)&&!pw::visible(pw::Trigger)&&pw::visible(pw::SharedBusy)&&pw::visible(pw::PoseNotPermitted));
  for(unsigned c=0;c<pw::Count;++c){CK(std::strcmp(pw::name(c),"UNKNOWN")!=0);if(const char* t=pw::say(c))CK(visible_failure::safe(t));}}
 // a working squeeze claims and clears the code
 resetR9();CK(h::CandidatePress(0,0,1000,true,.8f,0));CK(lastCode(0)==pw::None&&pw::claimedCount[0]==1&&pw::refused[0]==0);
 // a knife/reload transaction holds the hand for the whole 100 ms: the press expires with SHARED_BUSY + one bounded log receipt; no unsolicited screen line
 resetR9();auto t=m::TryBegin(1,m::Kind::Throw);CK(m::valid(t));
 CK(!h::CandidatePress(1,0,1000,true,.8f,0)&&lastCode(1)==pw::None); // one frame of contention is not the reason yet
 for(std::uint64_t now=1011;now<=1100;now+=11){tracking.tick=now;auto e=PhysEquipment6698{};e.tick=now;g_phys_equipment6698.publish(e);auto f=frame();f.tick=now;h::PublishCompleted(f);quick_talk_owner_h4::mailbox.publish({now,9,10,0x2000,0});CK(!h::CandidatePress(1,0,now,true,.8f,0));}
 CK(lastCode(1)==pw::None);
 const bool expiredR96=h::CandidatePress(1,0,1102,true,.8f,0);

 CK(!expiredR96&&lastCode(1)==pw::SharedBusy&&pw::refused[1]==1&&receiptSaysR96(pw::say(pw::SharedBusy)));
 CK(!h::CandidatePress(1,0,1103,true,.8f,0)&&pw::refused[1]==1); // first code of the press only
 CK(m::Finish(t,&nothing,nullptr));
 // completed pose not permitted (menu/dialogue/native body): released early -> the busy code is still reported
 resetR9();{auto f=frame();f.permitted=false;h::PublishCompleted(f);}
 CK(!h::CandidatePress(0,0,1000,true,.8f,0)&&lastCode(0)==pw::None);
 CK(!h::CandidatePress(0,0,1010,true,.1f,0)&&lastCode(0)==pw::PoseNotPermitted&&receiptSaysR96(pw::say(pw::PoseNotPermitted)));
 // the hand holds/supports something in the completed pose
 resetR9();{auto f=frame();f.available[0]=false;h::PublishCompleted(f);}
 CK(!h::CandidatePress(0,0,1000,true,.8f,0));CK(!h::CandidatePress(0,0,1010,true,.1f,0)&&lastCode(0)==pw::HandNotAvailable);
 // the hand's own state: no diagnostic line, but the code says it
 resetR9();CK(!h::CandidatePress(0,0,1000,false,.8f,0,pw::OtherTarget)&&lastCode(0)==pw::OtherTarget&&!receiptSaysR96(pw::say(pw::SharedBusy)));
 visible_failure::Message none{};CK(!visible_failure::mailbox.read(none)||!none.text[0]);
 resetR9();CK(!h::CandidatePress(1,0,1000,false,.8f,0)&&lastCode(1)==pw::HandBusy);
 resetR9();CK(!h::CandidatePress(0,0,1000,true,.8f,.5f)&&lastCode(0)==pw::Trigger);
 // no edge (hand still closed from before) never writes a code
 resetR9();h::neutral[0]=false;CK(!h::CandidatePress(0,0,1000,false,.8f,0)&&lastCode(0)==pw::None&&pw::refused[0]==0);
 // the lease (holster/draw) held over the press
 resetR9();{h::Admission busy;CK(busy.take(1u));CK(!h::CandidatePress(0,0,1000,true,.8f,0));CK(!h::CandidatePress(0,0,1010,true,.1f,0)&&lastCode(0)==pw::LeaseBusy);}
 // after a refusal the next squeeze claims normally and clears it
 CK(h::CandidatePress(0,0,1020,true,.8f,0)&&lastCode(0)==pw::None&&pw::claimedCount[0]==1);
 std::printf("door_press_why_r9 %u/%u checks PASS\n",checks,checks);return 0;
}
