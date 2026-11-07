// Complete production native clock adapter. The opt-in tracking leaf publishes
// a newer producer Control between its first and final current-input reads.
// This is deterministic callback interleaving, not a claim about retail timing.
#define KCD2VR_ALCHEMY_INPUT_OBSERVER_D50 1
#define KCD2VR_ALCHEMY_SEMI46_NO_MAIN
#include "alchemy_semi46_native_test.cpp"

namespace kcd2vr::action_time_owner59c11 { namespace {
enum class SupersessionD50 { Moving, BelowQuantum, Stopped, Rebased, Rewound, OtherChild, Manual, Disabled, SameSampleStop };
static SupersessionD50 supersessionD50;
static void PublishDuringReadD50() noexcept {
    testWorkstationFrameObserverD50=nullptr;
    if(supersessionD50!=SupersessionD50::SameSampleStop){
        ++input46.sequence;
        testWorkstationFrame46.sequence=input46.sequence;
        testWorkstationFrame46.tick=input46.tick;
        testWorkstationFrame46.timeNs+=1000000;
        // The native callback and producer are allowed to share the current ms.
        hostTickCount64=input46.tick;
        minigame_hands45::publish(minigame_hands45::Alchemy,hostTickCount64);
    }
    switch(supersessionD50){
    case SupersessionD50::Moving:input46.totalSeconds=.09;break;
    case SupersessionD50::BelowQuantum:break;
    case SupersessionD50::Stopped:case SupersessionD50::SameSampleStop:input46.clockStop=true;break;
    case SupersessionD50::Rebased:input46.totalSeconds=0;break;
    case SupersessionD50::Rewound:input46.sequence-=2;break; // current tracking must not bless a rewound producer
    case SupersessionD50::OtherChild:++input46.child;break;
    case SupersessionD50::Manual:input46.manualUse=true;break;
    case SupersessionD50::Disabled:input46.enabled=false;break;
    }
    semi46::control.publish(input46);
}
static void SupersessionCaseD50(SupersessionD50 kind){
    testWorkstationFrameObserverD50=nullptr;
    Prepare46(semi46::Mortar,2);QueueDetour(controller.data(),action.data(),0);InstallQueued46();
    Sample46(1020,.08);supersessionD50=kind;
    testWorkstationFrameObserverD50=PublishDuringReadD50;
    UpdateDetour(controller.data(),.02f);
    const auto first=Rd<float>(action.data(),kActActiveTime,-1.f);
    const auto discarded=g_semi_alchemy46.clock.discarded;
    std::printf("supersession=%u firstActive=%.6f pending=%.6f discarded=%.6f written=%d\n",
        unsigned(kind),double(first),g_semi_alchemy46.clock.pending,discarded,g_semi_alchemy46.written);
    std::fflush(stdout);
    if(kind==SupersessionD50::Moving||kind==SupersessionD50::BelowQuantum){
        // Both producer snapshots are current, continuous and from the same
        // exact owner. Observing the newer sample must not delete earned work.
        ck46(discarded<1.e-7&&g_semi_alchemy46.written);
        UpdateDetour(controller.data(),.02f);
        UpdateDetour(controller.data(),.02f);
        ck46(std::fabs(Rd<float>(action.data(),kActActiveTime,-1.f)-input46.totalSeconds)<1.e-6);
        UpdateDetour(controller.data(),.02f);
        ck46(std::fabs(Rd<float>(action.data(),kActActiveTime,-1.f)-input46.totalSeconds)<1.e-6);
    }else if(kind==SupersessionD50::Manual||kind==SupersessionD50::Disabled){
        // Native resume is intentional on manual takeover or disabled mode.
        // Do not interpret that authored advance as gesture credit.
        ck46(!g_semi_alchemy46.written&&Rd<float>(action.data(),kActSpeedBias,-1.f)==1.f);
    }else{
        ck46(first==0&&g_semi_alchemy46.clock.pending==0);
        ck46(discarded>=.079999&&Rd<float>(action.data(),kActSpeedBias,-1.f)==0);
    }
}
int SupersessionScenarioD50(){
    for(auto kind:{SupersessionD50::Moving,SupersessionD50::BelowQuantum,SupersessionD50::Stopped,
        SupersessionD50::Rebased,SupersessionD50::Rewound,SupersessionD50::OtherChild,
        SupersessionD50::Manual,SupersessionD50::Disabled,SupersessionD50::SameSampleStop})SupersessionCaseD50(kind);
    std::printf("D50 native input supersession: %u checks PASS; production adapter, explicit native leaves\n",checks46);
    return 0;
}
}}
int main(){return kcd2vr::action_time_owner59c11::SupersessionScenarioD50();}
