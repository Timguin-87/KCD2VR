// Actual production ray/candidate functions + actual HUD cleanup statements.
// Native selection is observed/copied, never fabricated into a completed door action.
#include "generated_interactor5916c3.inc"
#include <atomic>
#include <thread>
namespace kcd2vr::frik_pose_probe {
inline int modeH5 = 0;
inline int MiscInteractionDiagMode() noexcept { return modeH5; }
}
using namespace kcd2vr;
#include "generated_door_ray_owner_h5.inc"
using namespace kcd2vr::interaction_alignment;
static unsigned checksH5=0, failuresH5=0;
static void checkH5(bool value,const char* label) {
    ++checksH5;
    if(!value){++failuresH5;std::printf("FAIL %s\n",label);}
}
static void ordinaryH5(int side) {
    SetHandRayWorld(true,0,0,0,0,1,0,2,.2f,side);
}
static void witnessH5(int side) {
    Candidate57c1 candidate{0x55,hostTickCount64,interaction57c1::Kind::Object,42};
    std::strcpy(candidate.cls,"AnimDoor");
    RaySample ray{};ray.hand=true;ray.owner=side;ray.tick=hostTickCount64;
    ray.y=1;ray.limit=2;ray.radius=.2f;ray.sourceEpoch=7;ray.sourcePlayer=0x1234;
    const float point[]{0,.2f,0,.2f};
    DoorCandidateObservation38(candidate,point,ray,hostTickCount64,true,DoorSourceGeneration38());
}
static bool currentWitnessH5(int side) {
    HandCandidate out{};
    return ExactDoorCandidate38(42,side,hostTickCount64,out)&&out.entity==42&&out.owner==side;
}
int main() {
    hostTickCount64=20000;g_split_ready57c1=true;
    SetVisualFallbackEnabled(false);SetEnabled(true);
    const auto acquisition=g_door_acquisition_generation65;
    // Missing mailbox data is not evidence that the legacy publisher owns it.
    g_hand_ray_active=true;const auto unread=g_door_generation38;
    LegacyCleanupH5_0();
    checkH5(g_hand_ray_active&&g_door_generation38==unread,"failed mailbox read cannot authorize legacy cleanup");
    checkH5(!HandRayActive(),"missing mailbox is still unusable as an interaction ray");
    g_hand_ray_active=false;
    for(int side=0;side<2;++side)for(int mode: {0,1,2})for(int branch=0;branch<2;++branch){
        frik_pose_probe::modeH5=mode;ordinaryH5(side);witnessH5(side);
        const auto generation=g_door_generation38;
        checkH5(currentWitnessH5(side),"real copied candidate established");
        if(branch==0)LegacyCleanupH5_0();else LegacyCleanupH5_1();
        checkH5(HandRayActive(),"inactive legacy cleanup preserves ordinary right/left ray");
        checkH5(g_door_generation38==generation,"inactive legacy cleanup preserves ordinary source generation");
        checkH5(currentWitnessH5(side),"inactive legacy cleanup preserves exact ordinary door witness");
        checkH5(g_door_acquisition_generation65==acquisition,"legacy cleanup does not change native acquisition generation");
    }
    // An actual legacy publication is still cleaned by its own inactive path.
    for(int branch=0;branch<2;++branch){
        frik_pose_probe::modeH5=0;SetHandRayWorld(true,0,0,0,0,1,0);
        const auto generation=g_door_generation38;
        if(branch==0)LegacyCleanupH5_0();else LegacyCleanupH5_1();
        checkH5(!HandRayActive(),"legacy owner still releases its own ray");
        checkH5(g_door_generation38==generation+1,"legacy release invalidates its projection source once");
        if(branch==0)LegacyCleanupH5_0();else LegacyCleanupH5_1();
        checkH5(g_door_generation38==generation+1,"repeated inactive legacy cleanup does not churn generations");
    }
    // Real Grip/context release remains unconditional and invalidates old observations.
    ordinaryH5(0);witnessH5(0);const auto released=g_door_generation38;
    SetHandRayWorld(false,0,0,0,0,1,0);
    checkH5(!HandRayActive()&&!currentWitnessH5(0),"ordinary explicit release clears ray and candidate");
    checkH5(g_door_generation38==released+1,"ordinary explicit release advances projection generation");
    ordinaryH5(0);checkH5(!currentWitnessH5(0),"republish cannot revive a released candidate");
    ordinaryH5(0);witnessH5(0);SetEnabled(false);
    checkH5(!HandRayActive()&&!currentWitnessH5(0),"real backend/context loss still revokes ordinary ray");
    SetEnabled(true);ordinaryH5(0);witnessH5(0);SetVisualFallbackEnabled(true);
    checkH5(!currentWitnessH5(0),"real visual ownership change still revokes candidate");
    SetVisualFallbackEnabled(false);ordinaryH5(0);witnessH5(0);
    SetHandRayWorld(true,0,0,0,0,0,0,2,.2f,0);
    checkH5(!HandRayActive()&&!currentWitnessH5(0),"invalid ordinary direction still fails closed");
    const auto currentAcquisition=g_door_acquisition_generation65;
    // Cleanup never refreshes the ray timestamp or extends the 80ms tracking lifetime.
    ordinaryH5(1);witnessH5(1);hostTickCount64+=80;LegacyCleanupH5_0();
    checkH5(HandRayActive(),"ordinary original timestamp valid at exactly 80ms");
    ++hostTickCount64;LegacyCleanupH5_1();
    checkH5(!HandRayActive()&&!currentWitnessH5(1),"legacy cleanup cannot extend ordinary ray beyond 80ms");
    // Both serialized publication orders, then concurrent native mailbox publication/cleanup.
    frik_pose_probe::modeH5=0;SetHandRayWorld(true,0,0,0,0,1,0);
    LegacyCleanupH5_0();ordinaryH5(0);
    checkH5(HandRayActive(),"cleanup before new ordinary publication preserves new ray");
    ordinaryH5(1);LegacyCleanupH5_1();
    checkH5(HandRayActive(),"cleanup after new ordinary publication preserves new ray");
    bool interleavings=true;
    for(unsigned run=0;run<512;++run){
        SetHandRayWorld(true,0,0,0,0,1,0);
        std::atomic<unsigned> ready{0};std::atomic<bool> go{false};
        std::thread publish([&]{++ready;while(!go.load(std::memory_order_acquire))std::this_thread::yield();ordinaryH5(run&1);});
        std::thread cleanup([&]{++ready;while(!go.load(std::memory_order_acquire))std::this_thread::yield();LegacyCleanupH5_0();});
        while(ready.load(std::memory_order_acquire)!=2)std::this_thread::yield();
        go.store(true,std::memory_order_release);publish.join();cleanup.join();
        RaySample ray{};
        interleavings=interleavings&&HandRayActive()&&g_hand_packets112.read(ray)&&ray.owner==int(run&1);
    }
    checkH5(interleavings,"concurrent cleanup never revokes a newly published different owner");
    checkH5(g_door_acquisition_generation65==currentAcquisition,"ray-only ownership cases preserve acquisition generation65");
    // The recorder remained OFF throughout the actual ordinary-mode checks.
    checkH5(diagnostic_trace::receipts.empty(),"correction does not depend on F5 recording");
    std::printf("%u/%u checks passed\n",checksH5-failuresH5,checksH5);
    return failuresH5?1:0;
}
