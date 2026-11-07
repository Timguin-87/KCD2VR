// Actual production coherent-pose helper, not a restatement of its decisions.
#include "../src/frik_pose_probe.cpp"
#include <cassert>
#include <cstdio>
namespace kcd2vr::diagnostic_trace {void Write(const char*) noexcept {}}
using namespace kcd2vr::frik_pose_probe;
static kcd2vr::grasp13::Acquisition54 proof54{};
static bool proofLive54=true;
static bool proofRead54(kcd2vr::grasp13::Acquisition54& out) noexcept {out=proof54;return proofLive54;}
static kcd2vr::grasp13::Binding binding55{};static bool bindingLive55=true;
static bool bindingRead55(kcd2vr::grasp13::Binding& out) noexcept {out=binding55;return bindingLive55;}
int main(){
    namespace g=kcd2vr::grasp13;
    CoherentState6689 s{};s.character=reinterpret_cast<void*>(0x1234);s.owner=reinterpret_cast<void*>(0x4321);s.ready=true;
    s.graspMappingGeneration55=1;s.rig.hand_r=1;s.count=4;s.fingerMap[0]=true;s.fingerCount[0]=2;
    for(unsigned i=0;i<4;++i){s.original[i].q[3]=1;s.restLocal[i].q[3]=1;s.local[i].q[3]=1;}
    s.fingerIds[0][0]=2;s.fingerIds[0][1]=3;s.parent[2]=1;s.parent[3]=2;
    s.restLocal[2].t[0]=s.restLocal[3].t[0]=.02f;
    QuatT28 close{};close.q[0]=.1f;close.q[3]=std::sqrt(.99f);close.t[0]=.02f;
    s.original[2]=AbsoluteFromRelative(s.original[1],close);s.original[3]=AbsoluteFromRelative(s.original[2],close);
    g::Binding b{};b.character=0x1234;b.manager=0x2345;b.item=0x3456;b.logic=0x4567;
    b.token=13;b.key=14;b.epoch=7;b.entity=72;b.actor=42;b.side=0;b.socketOnly28=true;
    proof54.character=b.character;proof54.action=0x5678;proof54.scope=0x6789;proof54.serial=99;
    proof54.epoch=7;proof54.startedAt=900;proof54.tick=1000;proof54.side=0;proof54.active=true;
    proof54.graspToken=b.token;proof54.graspKey=b.key;proof54.graspItem=b.item;proof54.graspEntity=b.entity;
    g_graspAcquisition54=&proofRead54;g_graspBinding13=&bindingRead55;binding55=b;
    // A resource match is necessary but not enough: first attached frame A may
    // precede the native socket transaction B. Never freeze A independently.
    s.local[2].q[0]=.7f; // an earlier physical-item finger cache must not win
    assert(ApplyPublishedGraspFingers55(s,0,7,1000)&&!s.graspFingers54[0].ready);
    assert(std::fabs(s.local[2].q[0]-.1f)<1e-6f&&s.local[2].t[0]==.02f);
    g::FingerReference55 first{};assert(OriginalGraspFingers55(s,0,b,7,100,1000,1000,first,&proof54));
    close.q[0]=.3f;close.q[3]=std::sqrt(.91f);
    s.original[2]=AbsoluteFromRelative(s.original[1],close);s.original[3]=AbsoluteFromRelative(s.original[2],close);
    // Native evaluation can reuse both XR clocks. The payload must come from
    // transaction B itself, not a history lookup by XR sample/tick.
    g::FingerReference55 canonical{};assert(OriginalGraspFingers55(s,0,b,7,100,1000,1001,canonical,&proof54));
    assert(first.sample==canonical.sample&&first.tick==canonical.tick&&first.rotations[0][0]!=canonical.rotations[0][0]);
    b.gripReady55=true;b.grip55.p[0]=.07f;b.fingers55=canonical;binding55=b;
    assert(ApplyPublishedGraspFingers55(s,0,7,1001)&&s.graspFingers54[0].ready);
    assert(std::fabs(s.local[2].q[0]-.3f)<1e-6f&&s.local[2].t[0]==.02f);
    // Actual cleanup quarantine: no public binding, therefore NO finger write.
    // The inert sample must survive until the same native item is revalidated.
    bindingLive55=false;s.local[2]=s.restLocal[2];s.local[3]=s.restLocal[3];
    assert(!ApplyPublishedGraspFingers55(s,0,7,1010)&&s.graspFingers54[0].ready);
    assert(s.local[2].q[0]==0&&s.local[3].q[0]==0);
    // Native Picking finishes and its logic retires; the native skeleton opens.
    proofLive54=false;b.logic=0;b.socketOnly28=false;b.attached=b.place=true;
    bindingLive55=true;binding55=b;
    s.original[2]=AbsoluteFromRelative(s.original[1],s.restLocal[2]);
    s.original[3]=AbsoluteFromRelative(s.original[2],s.restLocal[3]);
    assert(ApplyPublishedGraspFingers55(s,0,7,1020)&&std::fabs(s.local[3].q[0]-.3f)<1e-6f);
    // A missing first pair is a cosmetic limitation, never a Finish/drop veto.
    // Once the native action has ended, fallback follows its native idle/open
    // locals instead of inventing a stable grip from this new animation phase.
    auto noPair=b;noPair.fingers55={};binding55=noPair;s.graspFingers54[0]={};
    assert(ApplyPublishedGraspFingers55(s,0,7,1020)&&!s.graspFingers54[0].ready&&s.local[3].q[0]==0);
    binding55=b;assert(ApplyPublishedGraspFingers55(s,0,7,1020)&&std::fabs(s.local[3].q[0]-.3f)<1e-6f);
    // A different item or epoch must never inherit a previous native grip.
    auto other=b;++other.key;binding55=other;s.local[3]=s.restLocal[3];
    assert(ApplyPublishedGraspFingers55(s,0,7,1020)&&!s.graspFingers54[0].ready&&s.local[3].q[0]==0);
    proofLive54=true;proof54.tick=900;g::FingerReference55 rejected{};
    assert(!OriginalGraspFingers55(s,0,b,7,101,1000,1000,rejected)); // stale witness
    proof54.tick=1000;proof54.active=false;assert(!OriginalGraspFingers55(s,0,b,7,101,1000,1000,rejected)); // queued is not a native hand grip
    proof54.active=true;assert(!ApplyGraspFingers54(s,0,b,8,1000)); // body epoch changed
    auto replacedAction=proof54;++replacedAction.action;
    assert(!OriginalGraspFingers55(s,0,b,7,101,1000,1000,rejected,&replacedAction));
    auto wrong=b;++wrong.fingers55.acquisition.graspKey;
    assert(!ApplyGraspFingers54(s,0,wrong,7,1000)&&!s.graspFingers54[0].ready);
    auto missing=b;missing.fingers55={};assert(!ApplyGraspFingers54(s,0,missing,7,1000));
    auto bad=b;bad.fingers55.rotations[0][0]=std::nanf("");assert(!ApplyGraspFingers54(s,0,bad,7,1000));
    const int savedId=s.fingerIds[0][0];s.fingerIds[0][0]=s.count;
    assert(!OriginalGraspFingers55(s,0,b,7,101,1000,1000,rejected));s.fingerIds[0][0]=savedId;
    assert(ApplyGraspFingers54(s,0,b,7,1000));
    bindingLive55=false;assert(!ApplyPublishedGraspFingers55(s,0,8,1001)&&!s.graspFingers54[0].ready);
    assert(ApplyGraspFingers54(s,0,b,7,1000));
    assert(!ApplyGraspFingers54(s,0,{},7,1000)&&!s.graspFingers54[0].ready); // release
    std::puts("PASS H34 same ORIGINAL-transaction native grip/fingers: later phase paired, repeated XR clocks safe, current action/resource/map validated, cleanup quarantine and Finish retained, no changed item/body reuse or bone translation");
}
