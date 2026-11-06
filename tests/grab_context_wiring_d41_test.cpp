// The runner copies GrabSourceCurrentD41 verbatim out of frik_pose_probe.cpp.
// Only its existing atomic/snapshot leaves are substituted; the production
// predicate itself is neither reproduced nor weakened for this fixture.
#include "../src/subsystems/physics/runtime/native_grab_producer60.h"
#include "../src/vrik/physical_weapon.h"
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <initializer_list>
using Intent=kcd2vr::physics::grab_producer60::Intent;
static std::uint64_t clockD41=1000;
static std::uint64_t GetTickCount64() noexcept {return clockD41;}
static std::atomic<std::uint64_t> g_pose_policy_generation57{2},g_pose_epoch6689{3};
static std::atomic<void*> g_last_player_entity{reinterpret_cast<void*>(0x1234)};
static std::atomic<int> g_cal_state{2};
static std::atomic<bool> g_gameplay_allowed{true};
static std::atomic<bool> g_ctx_menu{false},g_ctx_dialogue{false},g_ctx_cutscene{false},g_ctx_reading{false},g_ctx_scripted{false};
namespace tracking59c6 {static std::atomic<std::uint64_t> epoch{4};}
namespace animation_filter {static std::atomic<bool> nativeBodyOwned112{false};}
static bool passenger=false,reference=false,station=false,head=true,hand[2]{true,true};
static bool NativePassengerCurrentA34(std::uint64_t) noexcept {return passenger;}
namespace diagnostics::lab {static bool NativeReference() noexcept {return reference;}}
namespace kcd2vr::workstation178r20c1 {static bool ownsWrist(std::uint64_t) noexcept {return station;}}
static bool TrackingHeadUsable111(std::uint64_t=0) noexcept {return head;}
static bool TrackingHandUsable111(int side,std::uint64_t) noexcept {return hand[side];}
struct CommittedFrame6695 {
    bool valid[2]{true,true};struct Tool {bool valid=false;} tools59c10[2]{};
    std::uintptr_t doorPlayer38=0x1234;std::uint64_t doorTrackingEpoch66=4;
    struct Stamp {std::uint64_t epoch=3,tick=1000;} stamp{};
};
static CommittedFrame6695 published{};
static bool readable=true,policyAbaOnRead=false,nativeOnReservation=false,stationOnPacket=false;
static unsigned reservations=0;
static int modeOnPacket=2;
static std::atomic<int> g_weapon_control_mode{0};
static std::atomic<std::uint64_t> g_weapon_request_epoch6694{2};
static std::atomic<void*> g_henry_item_attach_manager{reinterpret_cast<void*>(0x2000)};
namespace pm=kcd2vr::pose_core;
namespace ph=kcd2vr::physical;
struct TrackingSnapshot {
    bool allowed=true,head_valid=true,right_valid=true,left_valid=true;
    bool head_held=false,right_held=false,left_held=false;
    std::uint64_t tick=1000,source_epoch=4;
    float right_grip=0,right_trigger=0,left_grip=0,left_trigger=0;
};
struct PhysFrame6698 {ph::Input input{};ph::Output output{};ph::Profile profiles[2]{};std::uint64_t request=2;};
struct PhysEquipment6698 {
    bool valid=true,handKnown[2]{true,true};std::uint64_t tick=1000,request=2,handKey[2]{};
    void* manager=reinterpret_cast<void*>(0x2000);
};
template<class T>struct CopiedLeafD41 {T value{};bool available=true;bool read(T& out) const noexcept {out=value;return available;}};
static CopiedLeafD41<TrackingSnapshot> g_tracking_frames6695;
static CopiedLeafD41<PhysFrame6698> g_phys_frame6698;
static CopiedLeafD41<PhysEquipment6698> g_phys_equipment6698;
static bool fistsSelected=false,fistsClosedNative=false;
static unsigned nativeSlotReads=0,lateTrackingReads=0;
static bool FistSelected101() noexcept {return fistsSelected;}
static bool FistHandsClosedH11() noexcept {++lateTrackingReads;return fistsClosedNative;}
static int ExactHandItemState(int) noexcept {++nativeSlotReads;return 0;}
namespace kcd2vr::hands59c7f {static bool active() noexcept {return false;}}
namespace kcd2vr::hand_only_door_h40 {
static unsigned Reserved() noexcept {
    if(nativeOnReservation)animation_filter::nativeBodyOwned112.store(true);
    return reservations;
}}

static bool ReadCommittedPacket6695(CommittedFrame6695& out) noexcept {
    out=published;if(policyAbaOnRead)g_pose_policy_generation57.fetch_add(2);
    if(stationOnPacket){station=true;g_weapon_control_mode=modeOnPacket;}return readable;
}
#include "grab_source_d41_generated.inl"
static unsigned checks=0,failures=0;
#define CK(x) do{++checks;if(!(x)){std::printf("FAIL wiring line %d: %s\n",__LINE__,#x);++failures;}}while(0)
int main() {
    Intent in{};in.side=0;in.player=0x1234;in.poseEpoch=3;in.trackingEpoch=4;
    CK(GrabSourceCurrentD41(in));in.side=1;CK(GrabSourceCurrentD41(in));in.side=0;
    for(auto* denied:{&g_ctx_menu,&g_ctx_dialogue,&g_ctx_cutscene,&g_ctx_reading,&g_ctx_scripted,&animation_filter::nativeBodyOwned112}) {
        denied->store(true);CK(!GrabSourceCurrentD41(in));denied->store(false);
    }
    for(auto* denied:{&passenger,&reference,&station}){*denied=true;CK(!GrabSourceCurrentD41(in));*denied=false;}
    for(auto* required:{&head,&hand[0],&readable}){*required=false;CK(!GrabSourceCurrentD41(in));*required=true;}
    g_gameplay_allowed=false;CK(!GrabSourceCurrentD41(in));g_gameplay_allowed=true;
    g_cal_state=1;CK(!GrabSourceCurrentD41(in));g_cal_state=2;
    ++in.player;CK(!GrabSourceCurrentD41(in));--in.player;
    ++in.poseEpoch;CK(!GrabSourceCurrentD41(in));--in.poseEpoch;
    ++in.trackingEpoch;CK(!GrabSourceCurrentD41(in));--in.trackingEpoch;
    published.valid[0]=false;CK(!GrabSourceCurrentD41(in));published.valid[0]=true;
    published.tools59c10[0].valid=true;CK(!GrabSourceCurrentD41(in));published.tools59c10[0].valid=false;
    published.tools59c10[1].valid=true;CK(GrabSourceCurrentD41(in));published.tools59c10[1].valid=false;
    reservations=1;CK(!GrabSourceCurrentD41(in));reservations=2;CK(GrabSourceCurrentD41(in));reservations=0;
    ++published.doorPlayer38;CK(!GrabSourceCurrentD41(in));--published.doorPlayer38;
    ++published.stamp.epoch;CK(!GrabSourceCurrentD41(in));--published.stamp.epoch;
    ++published.doorTrackingEpoch66;CK(!GrabSourceCurrentD41(in));--published.doorTrackingEpoch66;
    published.stamp.tick=919;CK(!GrabSourceCurrentD41(in));published.stamp.tick=1001;CK(!GrabSourceCurrentD41(in));published.stamp.tick=1000;
    g_pose_policy_generation57=3;CK(!GrabSourceCurrentD41(in));g_pose_policy_generation57=4;
    policyAbaOnRead=true;CK(!GrabSourceCurrentD41(in));policyAbaOnRead=false;CK(GrabSourceCurrentD41(in));
    nativeOnReservation=true;CK(!GrabSourceCurrentD41(in));nativeOnReservation=false;animation_filter::nativeBodyOwned112=false;
    in.side=-1;CK(!GrabSourceCurrentD41(in));in.side=2;CK(!GrabSourceCurrentD41(in));in.side=0;
    // Workstation/mode can change after the first context check. The actual
    // extracted reservation helper must NEVER reach native equipment slots.
    g_phys_frame6698.value.input.allowed=true;g_phys_frame6698.value.input.tick=clockD41;
    g_phys_frame6698.value.input.bodyEpoch=3;
    stationOnPacket=true;
    for(int mode=0;mode<=7;++mode){station=false;g_weapon_control_mode=7;modeOnPacket=mode;nativeSlotReads=0;
        CK(!GrabSourceCurrentD41(in));CK(nativeSlotReads==0);
    }
    stationOnPacket=false;station=false;g_weapon_control_mode=6;
    // Closed fists on physics consume an already published tracking POD, never
    // the general ReadTrackingSnapshot/HJ1 late-location callback family.
    fistsSelected=true;fistsClosedNative=true;lateTrackingReads=0;
    auto& copied=g_tracking_frames6695.value;copied.right_grip=copied.right_trigger=1;
    CK(frik_pose_probe::PhysicalHandReservationsPhysicsD41()==3);CK(lateTrackingReads==0);
    copied.right_grip=copied.right_trigger=0;
    CK(frik_pose_probe::PhysicalHandReservationsPhysicsD41()==0);CK(lateTrackingReads==0);
    copied.right_grip=copied.right_trigger=1;copied.source_epoch=5;
    CK(!frik_pose_probe::FistHandsClosedPhysicsD41());copied.source_epoch=4;
    copied.tick=919;CK(!frik_pose_probe::FistHandsClosedPhysicsD41());copied.tick=1000;
    // General/game-thread callers retain their original native fallback.
    CK(frik_pose_probe::PhysicalHandReservations114()==3);CK(lateTrackingReads==1);
    fistsSelected=false;station=true;nativeSlotReads=0;g_weapon_control_mode=2;
    CK(frik_pose_probe::PhysicalHandReservations114()==0);CK(nativeSlotReads==2);
    nativeSlotReads=0;CK(frik_pose_probe::PhysicalHandReservationsPhysicsD41()==3);CK(nativeSlotReads==0);
    std::printf("grab_context_wiring_d41: %u checks, %u failures; exact extracted frik predicate + reservation helpers\n",checks,failures);
    return failures?1:0;
}
