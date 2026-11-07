#pragma once
// Shape/native leaves for executing the complete RangedSourceXR34 function.
// No game pose is manufactured: retained fixture data come from the actual
// CompletedReloadPoseSourceXR34 used by the pipeline fixture.
namespace kcd2vr::tracking59c6 {static std::atomic<std::uint64_t> epoch{4};}
#include "subsystems/ranged_reload/weapon_kind_xr32.h"
namespace sourceRouteD87 {
static bool retained=true,enabled=true;static unsigned ordinaryCalls=0;
struct PhysFrame6698 {
    void* player=nullptr;void* manager=nullptr;std::uint64_t request=0;
    struct {std::uint64_t sample=0,bodyEpoch=0,tick=0;bool allowed=false,tracked[2]{};double grip[2]{};} input;
    struct {bool active[2]{},owned[2]{};ph::Pose weapons[2]{};} output;
    struct Profile {bool primary=false,nativeSeed=false;int classId=-1;std::uint64_t key=0,generation=0;ph::Pose nativeWeapon{},grip[2]{};} profiles[2];
    struct {pm::Stamp stamp{};} correlation126[2];
};
namespace animation_filter {static std::atomic<bool> nativeBodyOwned112{false};}
namespace diagnostics::lab {static bool NativeReference() noexcept{return false;}}
static std::atomic<bool> g_gameplay_allowed{true},g_head_valid{true},g_left_valid{true},g_right_valid{true};
static bool ReloadContextXR34(const x::Owner& o,std::uint64_t) noexcept{return XR34Current(o);}
static CommittedFrame6695* explicitBodyR93R3=nullptr;
static bool CrossbowReloadEnabledD65C() noexcept{return enabled;}
static bool RangedFrameH21(PhysFrame6698&) noexcept{++ordinaryCalls;return false;}
static bool ReadCommittedPacket6695(CommittedFrame6695& packet) noexcept{
    if(!retained)return false;if(explicitBodyR93R3){packet=*explicitBodyR93R3;return true;}const auto& s=f->source;
    packet.reloadSourceXR34=s;packet.reloadSourceXR34.bodyPublicationR95=packet.publicationR24;
    packet.doorTrackingEpoch66=s.tracking;
    packet.stamp={s.body,s.sample,s.tick,s.character};std::memcpy(packet.world,s.world,sizeof(packet.world));
    for(int hand=0;hand<2;++hand){
        std::memcpy(packet.native[hand].q,s.nativeWrist[hand].q,sizeof(packet.native[hand].q));
        std::memcpy(packet.native[hand].t,s.nativeWrist[hand].p,sizeof(packet.native[hand].t));
        std::memcpy(packet.allowed[hand].q,s.allowedWrist[hand].q,sizeof(packet.allowed[hand].q));
        std::memcpy(packet.allowed[hand].t,s.allowedWrist[hand].p,sizeof(packet.allowed[hand].t));
        std::memcpy(packet.propRequested5916c1[hand].q,s.intentWristD78[hand].q,sizeof(packet.propRequested5916c1[hand].q));
        std::memcpy(packet.propRequested5916c1[hand].t,s.intentWristD78[hand].p,sizeof(packet.propRequested5916c1[hand].t));
    }
    return true;
}
static void* ManualEquipmentPtr59c9(void*) noexcept{return nullptr;}
static bool RangedEquipmentH21(const PhysFrame6698&,int,void*,std::uint64_t) noexcept{return false;}
static bool PhysEmpty6698(std::uint64_t key) noexcept{return !key;}
// Current raw tracking is an explicit input leaf, separate from completed
// body/reload publication. Tests may inject release or invalid tracking here.
static bool rawTrackingAvailableR95=true;
static TrackingSnapshot* rawTrackingOverrideR95=nullptr;
static bool ReadTrackingSnapshot(TrackingSnapshot& out) noexcept{
    if(!rawTrackingAvailableR95)return false;
    if(rawTrackingOverrideR95){out=*rawTrackingOverrideR95;return true;}
    out={};out.sequence=f->source.sample;out.tick=f->now;out.source_epoch=f->source.tracking;
    out.right_grip=float(f->source.grip[0]);out.left_grip=float(f->source.grip[1]);return true;
}
static void SeedReloadEquipmentXR34(const x::Source&) noexcept{}
struct RangedGripRequestD72 {x::Source source{};std::uint64_t tick=0;unsigned kind=0;};
static kcd2vr::core::SnapshotMailbox<RangedGripRequestD72> g_ranged_grip_request_d72;
#include "retained_source_d87_actual.inc"
#include "ranged_source_d87_actual.inc"
}
