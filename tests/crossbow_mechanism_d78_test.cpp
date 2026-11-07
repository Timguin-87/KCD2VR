// Executes current raw-source producer, completed-source transport, XR34
// native clock adapter/client and wrist projection. Engine outcomes are leaves.
#define KCD2VR_XR34_NATIVE_NO_MAIN
#include "crossbow_native_xr34_test.cpp"
#include "vrik/physical_weapon.h"
namespace ph=kcd2vr::physical;
namespace pm=kcd2vr::pose_core;
struct QuatT28 {float q[4]{0,0,0,1},t[3]{};};
static ph::Pose PhysPose6698(const QuatT28& w){return {{w.q[0],w.q[1],w.q[2],w.q[3]},{w.t[0],w.t[1],w.t[2]}};}
static bool ValidPose6689(const QuatT28& w){return ph::finite(PhysPose6698(w));}
struct CommittedFrame6695 {pm::Stamp stamp{};QuatT28 native[2]{},allowed[2]{},propRequested5916c1[2]{};
    bool valid[2]{true,true},propRequestedValid5916c1[2]{true,true};float world[8]{0,0,0,1,0,0,0,1};};
struct TrackingSnapshot {std::uint64_t sequence=0,tick=0,source_epoch=0;bool allowed=true,head_valid=true,right_valid=true,left_valid=true;
    bool head_held=false,right_held=false,left_held=false;double right_grip=0,left_grip=0;
    x::Vec head{},right{},left{};struct {float x=0,y=0,z=0,w=1;}hq;};
namespace xcXR34=kcd2vr::crossbow_native_xr34;
#include "raw_source_d78.inc"
static unsigned checksD78=0,failuresD78=0;
#define C78(c) do{++checksD78;if(!(c)){++failuresD78;std::cerr<<"FAIL_D78 line="<<__LINE__<<" " #c "\n";}}while(0)
static bool near78(x::Vec a,x::Vec b,double e=1e-5){return x::length(a-b)<e;}
static x::Vec position78(const x::Wrist& w){return {w.p[0],w.p[1],w.p[2]};}
static CommittedFrame6695 packet78(const x::Source& s,x::Vec raw,x::Vec allowed){
    CommittedFrame6695 b{};b.stamp={s.body,s.sample,s.tick,s.character};
    b.propRequested5916c1[0].t[0]=float(raw.x);b.propRequested5916c1[0].t[1]=float(raw.y);b.propRequested5916c1[0].t[2]=float(raw.z);
    b.allowed[0].t[0]=float(allowed.x);b.allowed[0].t[1]=float(allowed.y);b.allowed[0].t[2]=float(allowed.z);return b;
}
static void input78(Fixture& a,double grip,x::Vec raw={},x::Vec allowed={}){
    a.input(grip,allowed);const auto b=packet78(a.source,raw,allowed);
    x::Passenger p{};p.owner=g_crossbow_xr34.status().owner;p.frame=a.source;p.workingHand=0;
    C78(XR34NativeClock(p.owner,g_crossbow_xr34.clip(),p.clock));
    TrackingSnapshot tracking{};tracking.sequence=a.source.sample;tracking.tick=a.now;tracking.source_epoch=a.source.tracking;tracking.right_grip=grip;
    a.source=CompletedReloadPoseSourceXR34(p,b,{},tracking,a.now);
    C78(a.source.intentKnownD78&&a.source.allowed&&near78(a.source.intentHandD78[0],raw));
    C78(near78(a.source.hand[0],allowed));x::PublishSource(a.source);
    // Explicit native string-pose job leaf; the real reader is exercised by
    // the companion socket suite. A job observes its own current native clock.
    a.spatial.stringMechanismD78=true;a.spatial.stringClockD78=p.clock;a.spatial.stringTickD78=a.now;
    a.spatial.stringCurrentD78={0,-double(g_crossbow_xr34.status().progress)*a.spatial.travel,0};
}
static void init78(Fixture& a,int kind){
    a.source.instantD72=true;a.source.classId=kind;a.source.entryGripD72=kind==10?1:2;a.source.entryGripTickD72=a.now;a.source.grip[0]=.9;
    a.source.chestKnown=true;a.source.handInChest[0]=true;
    const auto b=packet78(a.source,{},{});C78(RangedRawIntentD78(a.source,b));
    if(kind==15){a.spatial.axis={0,1,0};a.spatial.travel=.08;a.spatial.crankQualified=true;}
    setup(a);
    for(int i=0;i<3;++i){input78(a,.9,kind==10?x::Vec{}:x::Vec{.3,0,0},kind==10?x::Vec{}:x::Vec{.3,0,0});nativeFrame(a);}
    C78(g_crossbow_xr34.status().phase==x::Phase::Span&&a.cocks==0);
}
static void witness78(Fixture& a,bool mounted=true){
    x::Clock c{};C78(XR34NativeClock(g_crossbow_xr34.status().owner,g_crossbow_xr34.clip(),c));
    a.spatial.toolMechanismD72=true;a.spatial.toolMountedPoseD78=mounted;a.spatial.mechanismClockD78=c;
    a.spatial.mechanismTickD78=a.now;a.spatial.toolHandleD72={};a.spatial.toolWristD78={};
}
static x::Passenger passenger78(Fixture& a){x::Passenger p{};C78(x::ReadPassenger(p,a.now));return p;}
static void stop78(Fixture& a){XR34BeforeStopNative(a.parent.data());
    C78(!g_crossbow_xr34.pending()&&!g_crossbow_xr34.status().constraintD78.active&&a.get<float>(a.p(a.child)+0x5C)==2.5f);}
int main(){
    {Fixture a;a.source.instantD72=true;auto b=packet78(a.source,{.03,.04,.05},{8,9,10});
        C78(RangedRawIntentD78(a.source,b));C78(near78(a.source.intentHandD78[0],{.03,.04,.05}));
        b.propRequestedValid5916c1[0]=false;C78(!RangedRawIntentD78(a.source,b)&&!a.source.intentKnownD78);}
    for(int kind:{10,14}){
        Fixture a;init78(a,kind);input78(a,.9);nativeFrame(a);
        C78(g_crossbow_xr34.status().toolEngagedD72&&g_crossbow_xr34.status().constraintD78.active);
        // Causal feedback trap: a destination already at the cock endpoint is
        // not user input. Only the raw hand's later movement earns progress.
        input78(a,.9,{}, {0,-.15,0});nativeFrame(a);
        C78(a.cocks==0&&g_crossbow_xr34.status().progress==0);
        input78(a,.9,{.04,-.06575,.02},{0,-.15,0});nativeFrame(a);
        C78(a.cocks==0&&std::abs(g_crossbow_xr34.status().progress-.5f)<.001f);
        auto p=passenger78(a);p.binding.stringClockD78=p.clock;p.binding.stringTickD78=a.now;
        p.binding.stringCurrentD78={0,-.06575,0};
        x::Wrist raw[2]{},out{};raw[0].p[0]=.04f;raw[0].p[1]=-.06575f;raw[0].p[2]=.02f;
        const bool valid[2]{true,true};C78(x::projectReloadWristD78(p,raw,valid,a.source.sample,a.now,a.source.tracking,.9,out));
        C78(near78(position78(out),{0,-.06575,0}));
        // Nonlinear authored animation: the hand follows the actually observed
        // string, not the linear controller fraction or a previously written wrist.
        p.binding.stringCurrentD78.y=-.04;
        C78(x::projectReloadWristD78(p,raw,valid,a.source.sample,a.now,a.source.tracking,.9,out)&&near78(position78(out),{0,-.04,0}));
        p.binding.stringCurrentD78.y=-.06575;C78(x::projectReloadWristD78(p,raw,valid,a.source.sample,a.now,a.source.tracking,.9,out));
        ++p.binding.stringClockD78.entry;
        C78(!x::projectReloadWristD78(p,raw,valid,a.source.sample,a.now,a.source.tracking,.9,out));
        p.binding.stringClockD78=p.clock;C78(x::projectReloadWristD78(p,raw,valid,a.source.sample,a.now,a.source.tracking,.9,out));
        const auto rawBefore=raw[0];const auto expected=out;
        C78(x::projectReloadWristD78(p,raw,valid,a.source.sample,a.now,a.source.tracking,.9,out)&&!std::memcmp(&out,&expected,sizeof(out)));
        C78(!std::memcmp(&rawBefore,&raw[0],sizeof(raw[0])));
        C78(!x::projectReloadWristD78(p,raw,valid,a.source.sample,a.now,a.source.tracking,0,out));
        C78(!x::projectReloadWristD78(p,raw,valid,a.source.sample,a.now+81,a.source.tracking,.9,out));
        // Equivariance: translating/rotating both current controllers carries
        // the contact with the weapon; world motion never supplies pull credit.
        const float q=float(std::sqrt(.5));x::Wrist transform{{0,0,q,q},{3,4,5}};
        raw[0]=x::composeWristD78(transform,raw[0]);raw[1]=transform;
        C78(x::projectReloadWristD78(p,raw,valid,a.source.sample,a.now,a.source.tracking,.9,out));
        C78(near78(position78(out),position78(x::composeWristD78(transform,expected))));
        input78(a,0,{0,-.06575,0},{0,-.06575,0});nativeFrame(a);
        C78(!g_crossbow_xr34.status().constraintD78.active&&!g_crossbow_xr34.status().toolEngagedD72&&a.cocks==0);stop78(a);
    }
    for(int kind:{10,14}){Fixture a;init78(a,kind);input78(a,.9);nativeFrame(a);
        input78(a,.9,{0,-.15,0},{});nativeFrame(a);
        C78(a.cocks==1&&a.loads==0);stop78(a);}
    {Fixture a;a.source.instantD72=true;a.source.classId=10;a.source.entryGripD72=1;a.source.entryGripTickD72=a.now;a.source.grip[0]=.9;
        C78(RangedRawIntentD78(a.source,packet78(a.source,{},{})));setup(a);
        // The real originating contact survives native admission while the
        // player already starts the bounded stroke, preserving D76 intent.
        input78(a,.9,{0,-.15,0},{0,-.15,0});nativeFrame(a);C78(a.cocks==1);stop78(a);}
    {Fixture a;init78(a,15);input78(a,.9);witness78(a,false);nativeFrame(a);
        C78(g_crossbow_xr34.status().toolMountedD78&&g_crossbow_xr34.status().toolEngagedD72&&a.cocks==0);
        input78(a,.9,{0,.09,0},{});witness78(a,false);nativeFrame(a);
        C78(g_crossbow_xr34.status().progress==0&&a.cocks==0&&g_crossbow_xr34.status().reason==x::Reason::WaitingMechanismD78);
        input78(a,.9,{0,.04,0},{});witness78(a);nativeFrame(a);
        C78(std::abs(g_crossbow_xr34.status().progress-.5f)<.001f&&a.cocks==0);
        auto p=passenger78(a);p.binding.mechanismClockD78=p.clock;p.binding.mechanismTickD78=a.now;
        p.binding.toolWristD78={{0,0,.38268343f,.92387953f},{.015f,.035f,.02f}};
        x::Wrist raw[2]{},out{};raw[0].p[1]=.04f;const bool valid[2]{true,true};
        C78(x::projectReloadWristD78(p,raw,valid,a.source.sample,a.now,a.source.tracking,.9,out));
        C78(near78(position78(out),{.015,.035,.02})&&std::abs(out.q[2]-.38268343f)<1e-6f);
        ++p.binding.mechanismClockD78.entry;C78(!x::projectReloadWristD78(p,raw,valid,a.source.sample,a.now,a.source.tracking,.9,out));
        p.binding.mechanismClockD78=p.clock;p.binding.toolMountedPoseD78=false;
        C78(!x::projectReloadWristD78(p,raw,valid,a.source.sample,a.now,a.source.tracking,.9,out));
        input78(a,0,{0,.04,0},{});witness78(a);nativeFrame(a);
        C78(g_crossbow_xr34.status().toolMountedD78&&!g_crossbow_xr34.status().toolEngagedD72&&!g_crossbow_xr34.status().constraintD78.active);
        input78(a,.9,{.3,0,0},{.3,0,0});witness78(a);nativeFrame(a);C78(!g_crossbow_xr34.status().toolEngagedD72&&a.cocks==0);
        input78(a,.9);witness78(a);nativeFrame(a);C78(g_crossbow_xr34.status().toolEngagedD72);
        input78(a,.9,{0,.09,0},{});witness78(a);nativeFrame(a);C78(a.cocks==1&&a.spawns==1);stop78(a);
    }
    // An unjoined native pose cannot keep the clock leased indefinitely.
    {Fixture a;init78(a,15);input78(a,.9);witness78(a);nativeFrame(a);
        for(int i=0;i<6&&g_crossbow_xr34.pending();++i){input78(a,.9,{0,.06,0},{});a.spatial.toolMechanismD72=false;nativeFrame(a);}
        C78(!g_crossbow_xr34.pending()&&!g_crossbow_xr34.status().toolMountedD78&&!g_crossbow_xr34.status().constraintD78.active&&a.cocks==0);
        C78(g_crossbow_xr34.status().reason==x::Reason::GeometryUnavailable);stop78(a);}
    {Fixture a;init78(a,15);
        for(int i=0;i<6&&g_crossbow_xr34.pending();++i){input78(a,.9);a.spatial.toolMechanismD72=false;nativeFrame(a);}
        C78(!g_crossbow_xr34.pending()&&!g_crossbow_xr34.status().toolMountedD78&&a.cocks==0);stop78(a);}
    for(int kind:{10,14}){Fixture a;init78(a,kind);input78(a,.9);nativeFrame(a);
        for(int i=0;i<6&&g_crossbow_xr34.pending();++i){input78(a,.9,{0,-.10,0},{});a.spatial.stringMechanismD78=false;nativeFrame(a);}
        C78(!g_crossbow_xr34.pending()&&!g_crossbow_xr34.status().constraintD78.active&&a.cocks==0);stop78(a);}
    for(int loss=0;loss<3;++loss){Fixture a;init78(a,15);input78(a,.9);witness78(a);nativeFrame(a);
        input78(a,.9);witness78(a);
        if(loss==0){++a.source.request;x::PublishSource(a.source);}else if(loss==1)a.spatial.toolBound=false;else a.current=false;
        tick(a);C78(!g_crossbow_xr34.status().constraintD78.active&&!g_crossbow_xr34.status().toolMountedD78&&a.cocks==0);
        C78(a.get<float>(a.p(a.child)+0x5C)==2.5f);a.current=true;stop78(a);}
    for(int kind:{10,14,15})for(int loss=0;loss<2;++loss){Fixture a;init78(a,kind);input78(a,.9);if(kind==15)witness78(a);nativeFrame(a);
        const x::Vec remote=loss?x::Vec{.13,0,0}:x::Vec{0,kind==15?.30:-.40,0};
        input78(a,.9,remote,{});if(kind==15)witness78(a);nativeFrame(a);
        C78(a.cocks==0&&g_crossbow_xr34.status().progress==0&&!g_crossbow_xr34.status().toolEngagedD72&&!g_crossbow_xr34.status().constraintD78.active);
        input78(a,.9);if(kind==15)witness78(a);nativeFrame(a);C78(!g_crossbow_xr34.status().toolEngagedD72);
        input78(a,0);if(kind==15)witness78(a);nativeFrame(a);
        input78(a,.9);if(kind==15)witness78(a);nativeFrame(a);C78(g_crossbow_xr34.status().toolEngagedD72&&a.cocks==0);stop78(a);}
    {Fixture a;init78(a,15);input78(a,.9);witness78(a);nativeFrame(a);
        input78(a,.9,{0,.09,0},{});witness78(a);a.spatial.toolWristD78.p[0]=.36f;nativeFrame(a);
        C78(a.cocks==0&&g_crossbow_xr34.status().toolMountedD78&&!g_crossbow_xr34.status().toolEngagedD72&&!g_crossbow_xr34.status().constraintD78.active);stop78(a);}
    {x::Vec native[3]{{1,0,0},{0,1,0},{0,0,1}},actual[3]{{0,1,0},{-1,0,0},{0,0,1}};bool aligned=false;
        C78(x::boltFrameAlignedD78(native,native,aligned)&&aligned);
        C78(x::boltFrameAlignedD78(actual,native,aligned)&&!aligned);
        actual[0]={-1,0,0};actual[1]={0,1,0};C78(!x::boltFrameAlignedD78(actual,native,aligned));}
    std::cout<<"D78 reload intent / mechanism / presentation: "<<(checksD78-failuresD78)<<"/"<<checksD78<<" PASS; native engine outcomes mocked\n";
    return failuresD78?1:0;
}
