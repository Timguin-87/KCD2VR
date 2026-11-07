// Executes the production native adapter, clock client, passenger consumer,
// completed-source producer and preparation. Native memory/API, animation
// evaluation and attachment readiness are explicit leaves, not game execution.
#define KCD2VR_BODY_SOURCE_NO_MAIN
#include "crossbow_body_source_r93r3_test.cpp"
#undef KCD2VR_BODY_SOURCE_NO_MAIN
namespace sourceRouteD87 {
static std::uint64_t currentPublicationR95=2;
static void RangedReloadEquipmentXR34() noexcept {}
static std::uint64_t ReadCompletedPublicationT23() noexcept{return currentPublicationR95;}
static bool RangedSpatialXR34(const x::Owner& o,const x::Source& accepted,x::Spatial& s) noexcept{return spatial82(o,accepted,s);}
#include "publish_source_r95_actual.inc"
}
static unsigned checksR95R2=0;
#define L95(c) do{++checksR95R2;if(!(c)){std::cerr<<"FAIL_LIVENESS_R95R2 line="<<__LINE__<<" " #c " reason="<<x::reasonName(g_crossbow_xr34.status().reason)<<"\n";return 1;}}while(0)

namespace contextR95R2 {
static std::atomic<bool> g_ctx_menu{},g_ctx_dialogue{},g_ctx_cutscene{},g_ctx_scripted{},g_ctx_reading{};
static std::atomic<bool> g_gameplay_allowed{true},g_head_valid{true},g_left_valid{true},g_right_valid{true};
static std::atomic<std::uintptr_t> g_player_char37{};
static std::atomic<void*> g_henry_item_attach_manager{};
static std::atomic<std::uint64_t> g_pose_epoch6689{},g_weapon_request_epoch6694{};
static TrackingSnapshot tracking{};static bool publishDuringRead=false,readable=true;
namespace diagnostics::lab {static bool NativeReference() noexcept{return false;}}
static bool NativePassengerCurrentA34(std::uint64_t) noexcept{return false;}
static bool ReloadPlayerCurrentXR34(const x::Source& source) noexcept{return f&&f->current&&x::sameSource(source,f->source);}
static bool ReadTrackingSnapshot(TrackingSnapshot& out) noexcept {
    if(!readable)return false;
    if(publishDuringRead){publishDuringRead=false;tracking.tick=++f->now;++tracking.sequence;}
    out=tracking;return true;
}
#include "reload_context_r95r2_actual.inc"
static void reset(Fixture& a) {
    g_ctx_menu=g_ctx_dialogue=g_ctx_cutscene=g_ctx_scripted=g_ctx_reading=false;
    g_gameplay_allowed=g_head_valid=g_left_valid=g_right_valid=true;
    g_player_char37=a.source.character;g_henry_item_attach_manager=reinterpret_cast<void*>(a.source.manager);
    g_pose_epoch6689=a.source.body;g_weapon_request_epoch6694=a.source.request;
    kcd2vr::tracking59c6::epoch=a.source.tracking;
    tracking={};tracking.sequence=a.source.sample;tracking.tick=a.source.tick;tracking.source_epoch=a.source.tracking;
    publishDuringRead=false;readable=true;
}
}
static bool identityWithContextR95R2(const x::Owner& owner) noexcept {
    return FixtureIdentityXR34(owner)&&contextR95R2::ReloadContextXR34(owner,GetTickCount64());
}
static int contextClockR95R2(int kind) {
    Fixture a;a.source.classId=kind;a.source.chestKnown=true;
    if(kind==15){a.spatial.crankQualified=true;a.spatial.travel=.08;}
    setup(a);a.input(0);
    contextR95R2::reset(a);x::identityReader=&identityWithContextR95R2;
    contextR95R2::publishDuringRead=true;
    XR34PreControllerNative(a.controller.data(),.02f);
    L95(g_crossbow_xr34.pending()&&g_crossbow_xr34.status().clockOwned);
    const auto& owner=g_crossbow_xr34.status().owner;
    L95(contextR95R2::ReloadContextXR34(owner,a.now));
    contextR95R2::tracking.tick=a.now+1;L95(!contextR95R2::ReloadContextXR34(owner,a.now)); // genuine future input still refused
    contextR95R2::tracking.tick=a.now-81;L95(!contextR95R2::ReloadContextXR34(owner,a.now));
    contextR95R2::tracking.tick=a.now;contextR95R2::tracking.right_held=true;L95(!contextR95R2::ReloadContextXR34(owner,a.now));
    contextR95R2::tracking.right_held=false;contextR95R2::g_weapon_request_epoch6694++;
    L95(!contextR95R2::ReloadContextXR34(owner,a.now));contextR95R2::g_weapon_request_epoch6694--;
    contextR95R2::g_ctx_menu=true;XR34PreControllerNative(a.controller.data(),.02f);
    L95(!g_crossbow_xr34.pending()&&a.get<float>(a.p(a.child)+0x5C)==2.5f);
    contextR95R2::g_ctx_menu=false;XR34BeforeStopNative(a.parent.data());x::identityReader=&FixtureIdentityXR34;return 0;
}

static void toolBindingR95R2(Fixture& a,int kind) {
    a.spatial.toolItem=a.p(a.main)+0x100;a.spatial.toolKey=710;
    a.spatial.toolId=711;a.spatial.toolEntity=a.p(a.item)+0x140;
    a.spatial.toolAttachment=a.p(a.equipment)+0x100;
    a.spatial.toolBound=true;a.spatial.toolPendingD72=false;
    a.spatial.toolWeaponBoundD72=kind==15;
    a.put(a.p(a.main)+0x80,a.spatial.toolItem);
    a.put(a.spatial.toolItem+0x30,a.spatial.toolKey);
}
static int boundTransitionR95R2(int kind) {
    Fixture a;a.source.classId=kind;a.source.physicalD81=true;a.source.intentKnownD78=true;
    a.source.entryGripD72=2;a.source.grip[0]=.9;a.spatial.toolBound=false;
    RetailScopeD82 profile(a,kind);start82(a,false,true);completeFirstPose82(a,admission82(a));
    toolBindingR95R2(a,kind);XR34PostControllerNative(a.controller.data());
    preparationR93R3::setup(a);auto complete=bodyFor(a.source);QuatT28 original[1]{};
    sourceRouteD87::explicitBodyR93R3=&complete;sourceRouteD87::retained=sourceRouteD87::enabled=true;
    sourceRouteD87::currentPublicationR95=complete.publicationR24;
    const auto serial=g_crossbow_xr34.status().owner.serial;
    for(int i=0;i<8;++i) {
        x::Passenger p{};L95(ReadReloadPassengerXR34(p,a.source.character,a.source.body,a.now)&&p.binding.toolBound);
        a.put(a.entry()+0x44,p.clock.normalized+.01f);
        a.put(a.p(a.child)+0x10,p.clock.activeTime+.05f);
        L95(!ReloadNativeClockCurrentD86(p,a.source.character)); // no mechanism time relaxation
        L95(ReloadNativeClockCurrentD86(p,a.source.character,true));
        auto effectful=p;effectful.clip.nativeTransitionD80=false;
        L95(!ReloadNativeClockCurrentD86(effectful,a.source.character,true));
        auto next=complete;TrackingSnapshot tracking{};a.now+=20;
        tracking.sequence=complete.stamp.sample+1;tracking.tick=a.now;tracking.source_epoch=a.source.tracking;
        tracking.right_grip=.9;tracking.left_grip=1;next.stamp.sample=tracking.sequence;next.stamp.tick=a.now;
        next.allowed[0].t[0]+=.01f;next.propRequested5916c1[0].t[0]+=.01f;
        preparationR93R3::relativeReads=0;
        L95(preparationR93R3::PrepareReloadPoseXR34(p,next,original,1,tracking));
        next.publicationR24=complete.publicationR24+2;next.reloadSourceXR34.bodyPublicationR95=next.publicationR24;
        complete=next;sourceRouteD87::currentPublicationR95=complete.publicationR24;
        sourceRouteD87::RangedPublishXR34();L95(x::ReadSource(a.source)&&!a.source.auxiliaryUnavailableR95);
        XR34PreControllerNative(a.controller.data(),.02f);XR34PostControllerNative(a.controller.data());
        L95(g_crossbow_xr34.pending()&&g_crossbow_xr34.status().owner.serial==serial);
        L95(a.get<float>(a.p(a.child)+0x5C)==2.5f&&a.originalEvents==0&&a.cocks==0&&a.loads==0);
    }
    x::Passenger p{};L95(ReadReloadPassengerXR34(p,a.source.character,a.source.body,a.now));
    a.put(a.spatial.toolItem+0x30,a.spatial.toolKey+1);
    L95(!ReloadPassengerCurrentXR34(p,a.source.character,a.source.body,a.now)); // actual binding change revokes
    sourceRouteD87::explicitBodyR93R3=nullptr;XR34BeforeStopNative(a.parent.data());return 0;
}
static void beginToolR95R2(Fixture& a,int kind,int missing) {
    a.source.classId=kind;a.source.physicalD81=true;a.source.intentKnownD78=true;
    a.source.entryGripD72=2;a.source.chestKnown=true;a.source.handInChest[0]=true;a.source.grip[0]=.9;
    toolBindingR95R2(a,kind);a.spatial.toolPoseReady=false;
    if(missing){a.spatial.toolBound=false;a.spatial.toolPendingD72=missing==1;
        a.spatial.toolId=0;a.spatial.toolEntity=a.spatial.toolAttachment=0;}
    if(kind==15){a.spatial.axis={0,1,0};a.spatial.travel=.08;a.spatial.crankQualified=true;}
    setup(a);x::SetSpatialReader(&spatial82);input82(a,.9);nativeFrame(a);
}
static int toolWaitR95R2(int kind,int missing,bool recover) {
    Fixture a;beginToolR95R2(a,kind,missing);const auto serial=g_crossbow_xr34.status().owner.serial;
    const auto before=a.get<float>(a.entry()+0x44);L95(before>.19f&&before<.2f&&a.spawns==0);
    for(int i=0;i<10;++i){input82(a,.9);nativeFrame(a);
        L95(g_crossbow_xr34.pending()&&a.spawns==0&&a.cocks==0&&a.loads==0&&a.get<float>(a.p(a.child)+0x5C)==0);}
    if(recover){
        toolBindingR95R2(a,kind);a.spatial.toolPoseReady=true;
        for(int i=0;i<3;++i){input82(a,.9);nativeFrame(a);}
        L95(g_crossbow_xr34.pending()&&g_crossbow_xr34.status().owner.serial==serial&&a.spawns==1);
        L95(g_crossbow_xr34.status().toolEngagedD72&&a.cocks==0);
        input82(a,.9,{0,kind==15?.09:-.15,0});nativeFrame(a);
        L95(a.cocks==1&&a.loads==0&&g_crossbow_xr34.status().phase==x::Phase::Tail);
    }else{
        for(int i=0;i<25&&g_crossbow_xr34.pending();++i){input82(a,.9);nativeFrame(a);}
        L95(!g_crossbow_xr34.pending()&&!g_crossbow_xr34.status().clockOwned);
        L95(a.get<float>(a.p(a.child)+0x5C)==2.5f); // native escape, no blanket zero remains
        const auto oldSerial=g_crossbow_xr34.status().owner.serial;
        a.source.entryGripTickD72=++a.now;a.source.tick=a.now;x::PublishSource(a.source);
        XR34PostStartNative(a.parent.data(),true);L95(!g_crossbow_xr34.pending());
        XR34BeforeStopNative(a.parent.data());
        // The next real action may reuse all addresses after native Stop.
        a.put(a.p(a.controller)+0x50,a.p(a.queued)+8);a.put(a.p(a.scope)+0x148,std::uintptr_t(0));
        a.put(a.p(a.child)+0x28,1);a.put(a.p(a.child)+0x10,0.f);a.put(a.p(a.child)+0x30,std::uintptr_t(0));
        a.put(a.p(a.child)+0x54,18u);a.put(a.entry()+0x44,0.f);a.remaining=5;
        a.source.entryGripTickD72=++a.now;a.source.tick=a.now;++a.source.sample;
        a.source.completedLease=0;toolBindingR95R2(a,kind);a.spatial.toolPoseReady=true;a.spatial.toolVisible=false;
        setup(a,false,false);x::SetSpatialReader(&spatial82);
        for(int i=0;i<3;++i){input82(a,.9);nativeFrame(a);}
        L95(g_crossbow_xr34.pending()&&g_crossbow_xr34.status().owner.serial>oldSerial&&g_crossbow_xr34.status().toolEngagedD72);
    }
    XR34BeforeStopNative(a.parent.data());return 0;
}
static int noToolIntentR95R2(int kind) {
    Fixture a;beginToolR95R2(a,kind,0);a.source.handInChest[0]=false;
    for(int i=0;i<35;++i){input82(a,0);nativeFrame(a);}
    L95(g_crossbow_xr34.pending()&&a.spawns==0&&a.cocks==0); // waiting for a deliberate gesture is not a join failure
    a.current=false;XR34PreControllerNative(a.controller.data(),.02f);
    L95(!g_crossbow_xr34.pending()&&a.get<float>(a.p(a.child)+0x5C)==2.5f);
    XR34BeforeStopNative(a.parent.data());return 0;
}
static int boltWaitR95R2(int kind,bool recover,bool release) {
    Fixture a;a.source.classId=kind;a.source.physicalD81=true;a.source.quiverKnownD72=true;
    a.source.handInQuiverD72[0]=true;a.source.grip[0]=.9;a.source.entryGripD72=3;
    a.spatial.toolBound=false;a.spatial.boltId=321;a.spatial.boltKey=a.source.ammoKey;
    a.spatial.boltEntity=456;a.spatial.boltAttachment=789;
    setup(a,true);a.input(.9);nativeFrame(a);L95(a.originalEvents==1&&a.loads==0);
    // Native acquisition event ran once; its real attachment/readback is late.
    a.spatial.handBoltBound=false;a.spatial.boltPoseReady=false;
    a.source.handInQuiverD72[0]=false;
    for(int i=0;i<10;++i){a.input(release?0:.9);nativeFrame(a);
        L95(g_crossbow_xr34.pending()&&a.originalEvents==1&&a.loads==0);}
    if(recover){
        a.spatial.handBoltBound=true;a.spatial.boltPoseReady=true;a.spatial.handBoltPlaced=true;
        a.input(release?0:.9);nativeFrame(a);
        L95(g_crossbow_xr34.pending()&&a.loads==0&&a.originalEvents==1);
        if(release){a.input(.9);nativeFrame(a);L95(a.loads==0);}
        a.input(0);nativeFrame(a);L95(a.loads==1&&a.originalEvents==2);
    }else{
        for(int i=0;i<25&&g_crossbow_xr34.pending();++i){a.input(.9);nativeFrame(a);}
        L95(!g_crossbow_xr34.pending()&&a.get<float>(a.p(a.child)+0x5C)==2.5f&&a.loads==0);
    }
    XR34BeforeStopNative(a.parent.data());return 0;
}
static x::QuiverInputR95 lateReleaseR95R2{};
static bool rawReleaseR95R2(const x::Owner&,x::QuiverInputR95& out) noexcept {out=lateReleaseR95R2;return true;}
static int newerPendingReleaseR95R2(int kind) {
    Fixture a;a.source.classId=kind;a.source.physicalD81=true;a.source.quiverKnownD72=true;
    a.source.handInQuiverD72[0]=true;a.source.grip[0]=.9;a.source.entryGripD72=3;
    a.spatial.toolBound=false;a.spatial.boltId=321;a.spatial.boltKey=a.source.ammoKey;
    a.spatial.boltEntity=456;a.spatial.boltAttachment=789;
    setup(a,true);a.input(.9);nativeFrame(a);L95(a.originalEvents==1);
    a.spatial.handBoltBound=false;a.spatial.boltPoseReady=false;a.source.handInQuiverD72[0]=false;
    const auto saved=x::quiverReaderR95.load();x::quiverReaderR95=&rawReleaseR95R2;
    lateReleaseR95R2={a.source.sample+1,++a.now,0,false};nativeFrame(a);
    L95(g_crossbow_xr34.pending()&&a.loads==0);
    // Completed input still predates this observed release when the real
    // attachment finally reports placement. It cannot silently regrip.
    a.spatial.handBoltBound=a.spatial.boltPoseReady=a.spatial.handBoltPlaced=true;
    nativeFrame(a);L95(a.loads==0&&g_crossbow_xr34.status().phase==x::Phase::PlaceBolt);
    a.input(0);nativeFrame(a);L95(a.loads==0&&a.originalEvents==1);
    x::quiverReaderR95=saved;a.input(.9);nativeFrame(a);a.input(0);nativeFrame(a);
    L95(a.loads==1&&a.originalEvents==2);
    XR34BeforeStopNative(a.parent.data());return 0;
}
int main(int argc,char** argv) {
    const char* selected=argc>1?argv[1]:"all";
    const auto run=[&](const char* name){return !std::strcmp(selected,"all")||!std::strcmp(selected,name);};
    if(run("transition"))for(int kind:{14,15})if(boundTransitionR95R2(kind))return 1;
    if(run("tool_wait"))for(int kind:{14,15}){for(int missing:{0,1,2})for(bool recover:{true,false})if(toolWaitR95R2(kind,missing,recover))return 1;if(noToolIntentR95R2(kind))return 1;}
    if(run("bolt_wait"))for(int kind:{10,14,15})if(boltWaitR95R2(kind,true,false)||boltWaitR95R2(kind,false,false))return 1;
    if(run("pending_release"))for(int kind:{10,14,15})if(boltWaitR95R2(kind,true,true)||newerPendingReleaseR95R2(kind))return 1;
    if(run("context_clock"))for(int kind:{10,14,15})if(contextClockR95R2(kind))return 1;
    std::cout<<"PASS_LIVENESS_R95R2 checks="<<checksR95R2<<" case="<<selected<<" classes=10,14,15; actual adapter/passenger/preparation, explicit native leaves\n";
    return 0;
}
