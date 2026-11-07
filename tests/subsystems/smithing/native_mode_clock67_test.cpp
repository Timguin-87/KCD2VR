#define KCD2VR_SMITH_NATIVE_NO_MAIN_G1 1
#include "../../semi_smith46_native_test.cpp"
#undef KCD2VR_SMITH_NATIVE_NO_MAIN_G1
int main(){
    namespace mh=kcd2vr::minigame_hands45;
    namespace ao=kcd2vr::action_time_owner59c11;
    ao::SmithNativeScenario46();
    mh::alchemyMode67=-1;mh::smithingMode67=-1;
    ao::PrepareSmith46();ao::SmithBuilder46();ao::sc46(ao::g_semi_smith_pending46);
    const auto before=mh::modeEpoch67.load();mh::setMode(mh::Smithing,mh::Gesture);
    ao::sc46(mh::modeEpoch67.load()!=before);
    // No worker packet is republished: a stale paused native child must yield.
    ao::UpdateDetour(ao::controller.data(),.01f);
    ao::sc46(!ao::g_semi_smith_pending46&&ao::Rd<float>(ao::action.data(),ao::kActSpeedBias,-1)==1.f);
    mh::smithingMode67=-1;ao::PrepareSmith46();++ao::smithInput46.modeEpoch67;
    ao::smith46::clockControl.publish(ao::smithInput46);ao::SmithBuilder46();
    ao::sc46(!ao::g_semi_smith_pending46&&ao::smithQueueCalls46==1);
    std::printf("PASS G1 native smith mode clock: %u checks; actual Queue/Update cleanup, mocked native leaves\n",ao::smithChecks46);
}
