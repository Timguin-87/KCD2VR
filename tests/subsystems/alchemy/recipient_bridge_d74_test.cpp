// Exercise the consumer revalidation, not just the input producer flags.
#define main bridge67_original_main
#include "bridge67_test.cpp"
#undef main
static n::Decision recipient(const b::NativeTable& t, int destination, bool ownReturn) {
    n::Decision d{};d.source=n::Source::Right;d.ref=t.refs[destination];d.table=t.refs;
    d.palm={{0,0,0},1002,4,5,6,0,0,true};
    d.proximityCraft1=d.handsOnlyD53=d.palmRayD70=true;
    d.heldSourceH21=t.refs[7];d.selfReturnRecipientD74=ownReturn;d.pourRecipientD72=!ownReturn;
    return d;
}
static void publish(const n::Decision& d){b::nomination.publish({context(),d,1002});}
int main(){
    reset();kcd2vr::minigame_hands45::alchemyMode67.store(kcd2vr::minigame_hands45::SixDofVrik);
    auto t=table(1002);t.handTypesKnown68=true;t.handType68[0]=7;t.handType68[1]=255;
    auto& frame=kcd2vr::frik_pose_probe::observedFrame67;frame.hand[0].grip=0;
    b::finish(t);offer(t,7,0,1002);b::finish(t);
    auto d=recipient(t,7,true);publish(d);std::uint32_t id=0;
    check(b::selectionNear68(t,id,1002)&&id==107);
    // A producer flag cannot override membership, source identity, mode, a
    // regrip, or convert a source-return into an acquisition permission.
    t.handType68[0]=8;b::finish(t);check(!b::selectionNear68(t,id,1002)||id==0);
    t.handType68[0]=7;b::finish(t);frame.hand[0].grip=1;check(!b::selectionNear68(t,id,1002)||id==0);
    frame.hand[0].grip=0;d.palm.grip=1;publish(d);check(!b::selectionNear68(t,id,1002)||id==0);
    d=recipient(t,7,true);d.sourceAcquisitionD39=true;publish(d);check(!b::selectionNear68(t,id,1002)||id==0);
    d=recipient(t,7,true);d.heldSourceH21=t.refs[8];publish(d);check(!b::selectionNear68(t,id,1002)||id==0);
    d=recipient(t,7,true);publish(d);offer(t,7,1,1002);check(!b::selectionNear68(t,id,1002)||id==0);
    offer(t,7,0,1002);kcd2vr::minigame_hands45::alchemyMode67.store(kcd2vr::minigame_hands45::Gesture);
    check(!b::selectionNear68(t,id,1002)||id==0);
    kcd2vr::minigame_hands45::alchemyMode67.store(kcd2vr::minigame_hands45::SixDofVrik);
    // Gravity recipient is independent of tilted palm aim, but still must be
    // below the actual hand, close laterally, and held by the native owner.
    d=recipient(t,6,false);d.palm.point={0,0,.10};publish(d);offer(t,6,0,1002);b::finish(t);
    check(b::selectionNear68(t,id,1002)&&id==106);
    t.handType68[0]=8;b::finish(t);check(!b::selectionNear68(t,id,1002)||id==0);
    t.handType68[0]=7;b::finish(t);offer(t,6,.08,1002);check(!b::selectionNear68(t,id,1002)||id==0);
    offer(t,6,0,1002);d.palm.point.z=.30;publish(d);check(!b::selectionNear68(t,id,1002)||id==0);
    d.palm.point.z=-.10;publish(d);check(!b::selectionNear68(t,id,1002)||id==0);
    d.palm.point.z=.10;publish(d);t.handTypesKnown68=false;b::finish(t);check(!b::selectionNear68(t,id,1002)||id==0);
    std::printf("D74 recipient bridge: %u checks PASS\n",checks);return 0;
}
