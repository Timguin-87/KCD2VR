// Actual H40 input admission/current reader and T24/XR persistent-debt readers.
// Native leaves below are copied context/storage only; no game is executed.
#include "../src/subsystems/doors/hand_only_h40.h"
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
int main(){
 reset();const auto f=frame();CK(HandOnlyDoorCurrentH40(f,0,1000)&&HandOnlyDoorCurrentH40(f,1,1000));
 // Completed pose is older than a newly published T24 owner; actual current
 // reader catches it without relying on stale pose.available or native slots.
 ac::claims[1].publish(claim());ac::hands=2;
 CK(!HandOnlyDoorCurrentH40(f,1,1000)&&HandOnlyDoorCurrentH40(f,0,1000));
 CK(!h::CandidatePress(1,0,1000,true,.8f,0)&&h::Pending(1));
 CK(h::CandidatePress(0,0,1000,true,.8f,0));
 auto c=claim();c.body=99;c.request=200;ac::claims[1].publish(c);CK(!HandOnlyDoorCurrentH40(f,1,1000));
 c.player=0x9000;c.manager=0xA000;ac::claims[1].publish(c);CK(HandOnlyDoorCurrentH40(f,1,1000));
 ac::claims[1].publish({});CK(!HandOnlyDoorCurrentH40(f,1,1000)); // partial busy publication is unknown
 reset();xr::Status debt{};debt.owner.serial=77;auto&s=debt.owner.source;s.player=0x2000;s.manager=0x4000;s.character=0x3000;s.body=8;s.request=7;s.primary=0;
 debt.phase=xr::Phase::Retiring;debt.clockOwned=false;xr::cleanupInterested=true;xr::statusMailbox.publish(debt);
 CK(!HandOnlyDoorCurrentH40(f,1,1000)&&HandOnlyDoorCurrentH40(f,0,1000));
 CK(!h::CandidatePress(1,0,1000,true,.8f,0)&&h::CandidatePress(0,0,1000,true,.8f,0));
 reset();auto t=m::TryBegin(1,m::Kind::Throw);CK(m::valid(t));
 CK(!h::CandidatePress(1,0,1000,true,.8f,0)&&h::Pending(1));
 ac::claims[1].publish(claim());ac::hands=2;CK(m::Finish(t,&nothing,nullptr));
 CK(!h::CandidatePress(1,0,1001,true,.8f,0)&&!h::Reserved());
 ac::claims[1].publish({});ac::hands=0;CK(h::CandidatePress(1,0,1002,true,.8f,0));
 const auto original=h::claims[1].load();CK(original&&m::doorClaimsH41[1].load()==original&&!m::active[1].load());
 CK(!h::RetireClaimH41(1,original+1)&&m::doorClaimsH41[1].load()==original);
 t=m::TryBegin(1,m::Kind::Throw);CK(!m::valid(t));auto other=m::TryBegin(0,m::Kind::Reload);CK(m::valid(other));CK(m::Finish(other,&nothing,nullptr));
 CK(h::RetireClaimH41(1,original)&&!m::doorClaimsH41[1].load());
 h::CandidatePress(1,0,1003,true,0,0);CK(h::CandidatePress(1,0,1004,true,.8f,0));const auto successor=h::claims[1].load();
 CK(successor!=original&&!h::RetireClaimH41(1,original)&&m::doorClaimsH41[1].load()==successor);
 CK(h::RetireClaimH41(1,successor));t=m::TryBegin(1,m::Kind::Throw);CK(m::valid(t));CK(m::Finish(t,&nothing,nullptr));
 reset();t=m::TryBegin(1,m::Kind::Reload);CK(m::valid(t));CK(!m::RetainDoorH41(t,91));CK(m::Finish(t,&nothing,nullptr));
 t=m::TryBegin(1,m::Kind::Door);CK(m::valid(t)&&m::RetainDoorH41(t,91));CK(!m::RetainDoorH41(t,92));CK(m::Finish(t,&nothing,nullptr));
 CK(!m::RetireDoorH41(1,92)&&m::doorClaimsH41[1].load()==91);CK(m::RetireDoorH41(1,91));
 std::printf("H41 actual door/T24/XR admission: %u checks PASS; native NOT_RUN\n",checks);
}
