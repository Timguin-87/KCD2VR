// Behavioral contract used by Fresh Pose Uncapped's pose-only acquisition.
// Exercises the production tracking mailbox/composer, not a second implementation.
#include "tracking_sample59c6.h"
#include <cstdio>

namespace tr = kcd2vr::tracking59c6;
struct Position { float x=0,y=0,z=0; };
struct Rotation { float x=0,y=0,z=0,w=1; };
struct Consumer {
    Position head{},left{},right{};
    Rotation hq{},lq{},rq{};
    bool head_valid=false,left_valid=false,right_valid=false;
    bool head_held=true,left_held=true,right_held=true;
    std::uint64_t source_epoch=0,input_tick=0,sequence=0,tick=0,timeNs=0;
    std::uint64_t head_good_tick=0,left_good_tick=0,right_good_tick=0;
    std::int64_t poseTime=0;
    bool allowed=true;
    unsigned actionEdge=0x73;
};
static int failures=0;
static void check(bool pass,const char* label) {
    if(!pass){std::printf("FAIL: %s\n",label);++failures;}
}
static tr::Frame raw(std::uint64_t tick) {
    tr::Frame f{};
    f.epoch=tr::epoch.load();f.tick=tick;f.ns=tick*1000000;
    f.locateTime=std::int64_t(f.ns+9000000000ull);f.controllersSampled=true;
    for(unsigned i=0;i<3;++i){f.poses[i].x=float(i+1);f.valid[i]=f.tracked[i]=true;}
    return f;
}
int main() {
    tr::Configure(true);tr::SetCameraSource(2);
    tr::Frame completed{};
    check(tr::Publish(raw(1000),&completed),"initial complete family publishes");
    check(completed.held[0]&&completed.held[1]&&completed.held[2],"reacquisition cannot start a stroke");
    check(tr::Publish(raw(1001),&completed),"second complete family publishes");
    check(!completed.held[0]&&!completed.held[1]&&!completed.held[2],"continuous tracked family is usable");
    const auto sourceSequence=completed.sequence;

    Consumer c{};c.source_epoch=completed.epoch;c.input_tick=1001;
    check(tr::Compose(c,completed,1001,1001000000)==tr::Choice::Current,"pose family reaches a current consumer");
    check(c.sequence==sourceSequence&&c.poseTime==completed.locateTime&&c.left.x==2&&c.right.x==3,"consumer receives exact family identity and poses");
    check(c.input_tick==1001&&c.allowed&&c.actionEdge==0x73,"pose-only composition does not refresh input or replay an action edge");
    Consumer stale=c;stale.input_tick=900;
    check(tr::Compose(stale,completed,1001,1001000000)==tr::Choice::Invalidated,"fresh poses cannot revive stale input permission");
    check(stale.input_tick==900&&!stale.head_valid&&!stale.left_valid&&!stale.right_valid&&stale.actionEdge==0x73,"stale input stays stale and no action edge is generated");

    auto busy=raw(1002);busy.controllersSampled=false;
    check(!tr::Publish(busy),"unavailable grip pair cannot publish a fabricated complete family");
    tr::Frame retained{};
    check(tr::Read(retained)&&retained.sequence==sourceSequence&&retained.ns==1001000000&&retained.goodTick[1]==1001,"failed family keeps original identity and age");
    check(!tr::Publish(raw(1000)),"older family cannot replace the current one");
    check(tr::Read(retained)&&retained.sequence==sourceSequence&&retained.ns==1001000000,"out-of-order rejection preserves current source age");

    auto partial=raw(1010);partial.valid[1]=partial.tracked[1]=false;
    check(tr::Publish(partial,&completed),"attempted family publishes individual hand invalidity");
    check(completed.valid[1]&&completed.held[1]&&completed.goodTick[1]==1001&&completed.poses[1].x==2,"brief hand loss retains display grace without renewing tracked age");
    check(completed.valid[2]&&!completed.held[2]&&completed.goodTick[2]==1010,"other hand stays independently current");
    partial=raw(1090);partial.valid[1]=partial.tracked[1]=false;
    check(tr::Publish(partial,&completed),"continued loss remains an observed family");
    check(!completed.valid[1]&&completed.held[1]&&completed.goodTick[1]==1001,"repeated failed locates cannot extend hand grace forever");

    const auto prior=completed;
    tr::SetCameraSource(1);
    check(tr::enabled.load()&&!tr::cameraNow.load()&&!tr::Read(retained),"predicted mode change invalidates old NOW family");
    check(!tr::Publish(prior),"old-source publication cannot cross a camera-source change");
    tr::SetCameraSource(0);
    check(tr::enabled.load()&&!tr::cameraNow.load(),"cached worker source keeps its existing coherent-family policy");
    tr::SetCameraSource(3);
    check(!tr::enabled.load(),"offset camera mode retains its existing disabled coherent-family policy");
    std::printf("fresh_pose_family_contract: %s\n",failures?"FAIL":"PASS");
    return failures?1:0;
}
