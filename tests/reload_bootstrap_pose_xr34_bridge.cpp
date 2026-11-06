// A separate TU runs the actual full coherent transaction on its retained
// anatomical fixture, preserving the native executor's ORIGINAL actor/body.
#include <cstddef>
#include <cstdint>
#include <cstring>
static std::uintptr_t bootstrapActorXR34=0;
static bool BootstrapPoseReadXR34(const void* p,void* out,std::size_t n){
    std::uintptr_t vft=0;if(!bootstrapActorXR34)return false;
    std::memcpy(&vft,reinterpret_cast<void*>(bootstrapActorXR34),sizeof(vft));
    if(reinterpret_cast<std::uintptr_t>(p)==vft+0x190&&n==sizeof(void*)){
        const std::uintptr_t update=0x423C30;std::memcpy(out,&update,n);return true;}
    return false;
}
#define KCD2VR_POSE_FIXTURE_READ_LEAF BootstrapPoseReadXR34
#define KCD2VR_XR34_WORKING_NO_MAIN
#include "reload_working_hand_xr34_test.cpp"
namespace xcXR34=kcd2vr::crossbow_native_xr34;
static ph::Pose PhysPose6698(const QuatT28& a){return {{a.q[0],a.q[1],a.q[2],a.q[3]},{a.t[0],a.t[1],a.t[2]}};}
#include "reload_completed_source_xr34_actual.inc"
using BootstrapCurrentXR34=bool(*)(const xr::Passenger&) noexcept;
static BootstrapCurrentXR34 nativeBootstrapCurrentXR34=nullptr;
static xr::Source completedBootstrapSourceXR34{};
static xr::Passenger originalBootstrapWitnessXR34{};
static bool BootstrapCurrentPoseXR34(const xr::Passenger& p,std::uintptr_t actor,std::uint64_t epoch,std::uint64_t) noexcept {
    return actor==p.frame.character&&epoch==p.frame.body&&nativeBootstrapCurrentXR34&&nativeBootstrapCurrentXR34(p);
}
static bool BootstrapReadPoseXR34(xr::Passenger& p,std::uintptr_t actor,std::uint64_t epoch,std::uint64_t now) noexcept {
    p=originalBootstrapWitnessXR34;return BootstrapCurrentPoseXR34(p,actor,epoch,now);
}
static bool BootstrapPublishPoseXR34(xr::Passenger& p,CommittedFrame6695& packet,
    const QuatT28*,std::size_t,const TrackingSnapshot& t) noexcept {
    if(!BootstrapCurrentPoseXR34(p,packet.stamp.actor,packet.stamp.epoch,reloadClockXR34))return false;
    // Explicit native attachment leaf in this fixture: the ordinary primary
    // socket is identity relative to its ORIGINAL current native wrist.
    completedBootstrapSourceXR34=CompletedReloadPoseSourceXR34(p,packet,packet.native[p.frame.primary],t,reloadClockXR34);
    packet.reloadSourceXR34=completedBootstrapSourceXR34;return completedBootstrapSourceXR34.allowed;
}
extern "C" bool ActualBootstrapPoseXR34(const xr::Passenger& p,std::uint64_t tick,std::uint64_t sample,
    double grip,double pull,BootstrapCurrentXR34 current,xr::Source& result){
    Init();bootstrapActorXR34=p.frame.character;reloadClockXR34=tick;
    g_pose_epoch6689=p.frame.body;g_player_char37=p.frame.character;g_last_player_entity=reinterpret_cast<void*>(p.frame.player);
    g_player_lease37=tick;g_player_publication37=2;kcd2vr::tracking59c6::epoch=p.frame.tracking;
    animation_filter::nativeBodyOwned112=true;originalBootstrapWitnessXR34=p;nativeBootstrapCurrentXR34=current;
    g_readReloadPassengerXR34=&BootstrapReadPoseXR34;g_currentReloadPassengerXR34=&BootstrapCurrentPoseXR34;g_prepareReloadPoseXR34=&BootstrapPublishPoseXR34;
    completedBootstrapSourceXR34={};auto t=Tracking();t.tick=tick;t.sequence=sample;t.source_epoch=p.frame.tracking;
    t.right_grip=float(grip);t.right.z+=float(pull);
    CoherentTransaction6689 tx;const bool ok=BeginCoherent6689(tx,&fixture,&fixture,reinterpret_cast<void*>(p.frame.character),nullptr,Count,rig,MockGet,4,t);
    tx.Restore();if(ok)PublishCommitted6689(tx);
    result=completedBootstrapSourceXR34;
    return ok&&Same(fixture.live,fixture.original)&&result.allowed&&result.tick==tick&&result.sample==sample&&
        result.completedLease==p.owner.serial&&physicalPublications==0;
}
