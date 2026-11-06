#define main OriginalSemiSmithG1
#include "../../semi_smith46_test.cpp"
#undef main
int main(){
    namespace mh=kcd2vr::minigame_hands45;
    producer();nativeReader();
    Driver d;d.baseline();d.next(20);d.frame.hand[0].position[1]+=.04f;auto out=d.step();
    ck(out.requestRT&&out.clock.modeEpoch67==mh::modeEpoch67.load());const auto request=out.clock.request;
    mh::setMode(mh::Smithing,mh::Gesture);d.next(20);d.frame.hand[0].position[1]+=.04f;out=d.step();
    ck(out.reason==s::Reason::Rebased&&!out.requestRT&&!out.clock.enabled);
    d.next();out=d.step();ck(!out.requestRT&&!out.clock.enabled);d.next();d.step();
    d.next(20);d.frame.hand[0].position[1]+=.04f;out=d.step();
    ck(out.requestRT&&out.clock.request!=request&&out.clock.modeEpoch67==mh::modeEpoch67.load());
    std::printf("PASS G1 smith mode clock: %u checks; actual producer/rebase, no native execution\n",checks);
}
