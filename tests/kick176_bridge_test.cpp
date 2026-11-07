// Reuse the existing actual coherent-pose fixture; add native-length leg chains.
// The legacy test main is not run. Native ray memory/calls remain replaced leaves.
#include <cstdint>
static std::uint64_t hostTickCount64=10000;
#define KCD2VR_POSE_FIXTURE_CLOCK hostTickCount64
#define KCD2VR_POSE_FIXTURE_COUNT 56
#define main PoseAdapterBaselineUnused176
#include "pose_adapter_test.cpp"
#undef main
static void SetupKick176(){
    Init();hostTickCount64=10000;logs.clear();mockKickQueries176=0;mockKickContact176={};mockKickQueryHook176=nullptr;
    g_kick_busy176=0;g_kick_cancel176=1;g_kick_serial176=0;g_kick_status176=0;
    g_kick_request176.publish({});g_body_follow128=false;
    kcd2vr::diagnostics::capture168::gate.select(kcd2vr::diagnostics::capture168::Domain::Combat,false,10000);
    kcd2vr::frik_pose_probe::mockCombatExperiment175=true;
    for(int side=0;side<2;++side){const int h=48+side*4;
        fixture.parent[h]=0;fixture.local[h].t[0]=side?-.12f:.12f;
        fixture.parent[h+1]=h;fixture.local[h+1].t[1]=.05f;fixture.local[h+1].t[2]=-.38f;
        fixture.parent[h+2]=h+1;fixture.local[h+2].t[1]=-.05f;fixture.local[h+2].t[2]=-.38f;
        fixture.parent[h+3]=h+2;fixture.local[h+3].t[1]=.12f;fixture.local[h+3].t[2]=-.06f;
    }
    for(int j=0;j<Count;++j)fixture.reference[j]=fixture.parent[j]<0?fixture.local[j]:AbsoluteFromRelative(fixture.reference[fixture.parent[j]],fixture.local[j]);
    fixture.original=fixture.live=fixture.reference;
}
static void RequestKickFixture176(std::uint64_t serial,std::uint64_t tick){
    g_kick_busy176=serial;g_kick_request176.publish({serial,tick,g_kick_cancel176.load()});
}
static void RequeueInQuery176(){++g_kick_cancel176;RequestKickFixture176(99,10060);}
int main(){
    SetupKick176();auto t=Tracking();RequestKickFixture176(1,t.tick);
    Check(Run(t,4),"production coherent pose accepts visual probe with body-follow OFF");
    Check(g_pose6689.legsReady128&&g_pose6689.kick176.active,"actual rest topology discovers and owns right leg");
    Check(Same(fixture.live,fixture.original),"transaction restores all native joints after consumer");
    auto next=[&](unsigned dt){t.tick+=dt;hostTickCount64=t.tick;++t.sequence;return Run(t,4);};
    Check(next(60),"second production kick sample commits");
    Check(mockKickQueries176==1,"one contact adapter call per new committed foot segment");
    Check(mockKickTo176.z>mockKickFrom176.z,"contact observes rising committed ankle in current world space");
    Check(std::abs(g_pose6689.work[50].t[2]-fixture.original[50].t[2])>.05,"actual V128 solver raises visible ankle");
    for(int j=52;j<56;++j)Check(std::memcmp(&g_pose6689.work[j],&fixture.original[j],sizeof(QuatT28))==0,"opposite leg remains exactly native with follow OFF");
    const double thigh=Len(Sub(FromT(g_pose6689.work[49]),FromT(g_pose6689.work[48]))),shin=Len(Sub(FromT(g_pose6689.work[50]),FromT(g_pose6689.work[49])));
    Check(std::abs(thigh-g_pose6689.leg[0].thigh)<1e-5&&std::abs(shin-g_pose6689.leg[0].shin)<1e-5,"actual leg solve preserves both bone lengths");
    const auto queries=mockKickQueries176;Check(Run(t,4)&&mockKickQueries176==queries,"second eye reuses sample without another contact query");
    auto duplicate=t;duplicate.tick+=25;const auto beforeDuplicate=g_pose6689.kick176.lastTick;
    const auto beforeAnkle=g_pose6689.work[50];
    Check(Run(duplicate,4)&&g_pose6689.kick176.lastTick==beforeDuplicate&&mockKickQueries176==queries&&
        std::memcmp(&beforeAnkle,&g_pose6689.work[50],sizeof(QuatT28))==0,"changed-tick duplicate sample cannot move kick or query again");
    unsigned kickLogs=0;for(const auto& l:logs)kickLogs+=l.find("KICK_VISUAL_CONTACT_PROBE")!=std::string::npos;
    Check(kickLogs==0,"gameplay visual probe remains quiet with Combat recorder OFF");
    const auto old=g_pose6689.kick176;failAt=49;failOnce=false;
    Check(!next(30),"native pose setter rejection aborts actual transaction");
    Check(g_pose6689.kick176.lastSample==old.lastSample&&!g_kick_busy176,"failed transaction publishes no temporal advance and releases its request");
    Check(Same(fixture.live,fixture.original),"partial failed writes restore native body");
    SetupKick176();t=Tracking();RequestKickFixture176(2,t.tick);Check(Run(t,4),"new independent attempt begins");
    mockKickQueryHook176=RequeueInQuery176;
    Check(!next(60),"cancel during native contact leaf prevents old kick consumption");
    Check(g_kick_busy176==99,"old failed pose cannot clear newly queued serial");
    Check(Same(fixture.live,fixture.original),"cancel/requeue during probe restores old pose");
    SetupKick176();t=Tracking();RequestKickFixture176(3,t.tick);kcd2vr::frik_pose_probe::mockCombatExperiment175=false;
    Check(Run(t,4)&&!g_pose6689.kick176.active&&!g_kick_busy176,"outside F4 rejects pending visual request without leg writes");
    SetupKick176();t=Tracking();RequestKickFixture176(4,t.tick);Check(Run(t,4),"full lifecycle starts");
    for(int n=0;n<11;++n)Check(next(50),"bounded lifecycle pose commits");
    Check(!g_pose6689.kick176.active&&!g_kick_busy176&&g_pose6689.kick176.nextSide==1,"520ms completion releases ownership and selects other leg");
    RequestKickFixture176(5,t.tick);++t.sequence;Check(Run(t,4)&&g_pose6689.kick176.side==1,"actual caller supports left kick on next edge");
    Check(next(60)&&g_pose6689.work[54].t[2]>fixture.original[54].t[2]+.05,"left ankle visibly lifts through production leg solver");
    Check(mockPhysicalOutput.commandCount==0,"visual kick never enqueues native weapon/damage command");
    SetupKick176();t=Tracking();
    kcd2vr::diagnostics::capture168::gate.select(kcd2vr::diagnostics::capture168::Domain::Combat,true,t.tick);
    RequestKickFixture176(6,t.tick);Check(Run(t,4),"recorded visual attempt starts");
    mockKickContact176.kind=kick176::ContactKind::Hit;mockKickContact176.entity=321;mockKickContact176.bodyOwner=true;
    for(int n=0;n<11;++n)Check(next(50),"recorded phase progress commits"); // C34b: 520 ms kick
    auto countKick=[](){unsigned count=0;for(const auto& line:logs)count+=line.find("KICK_VISUAL_CONTACT_PROBE")!=std::string::npos;return count;};
    const auto completeLogs=countKick();Check(completeLogs==5,"one contact plus phase edges bounds formatting under sustained intersection");
    const auto completedQueries=mockKickQueries176;
    for(int n=0;n<30;++n){t.tick+=10;++t.sequence;(void)Run(t,4);}
    Check(countKick()==completeLogs&&mockKickQueries176==completedQueries,"completed kick emits no per-frame terminal or idle queries");
    SetupKick176();t=Tracking();RequestKickFixture176(7,t.tick);Check(Run(t,4)&&next(50),"rollback scenario begins");
    --t.sequence;t.tick+=10;const bool rollbackCommitted=Run(t,4);
    Check(!g_kick_busy176&&Same(fixture.live,fixture.original)&&(!rollbackCommitted||!g_pose6689.kick176.active),
        "actual caller rejects sample rollback and releases owned leg writes");
    SetupKick176();t=Tracking();hostTickCount64=t.tick+1;RequestKickFixture176(8,t.tick+1);
    Check(Run(t,4)&&!g_pose6689.kick176.active&&!g_pose6689.kick176.seenRequest&&g_kick_busy176==8,
        "actual coherent callback waits for request newer than latest tracking sample");
    Check(!mockKickQueries176&&Same(fixture.live,fixture.original),"pending scheduling gap neither probes nor retains pose writes");
    ++t.tick;++t.sequence;Check(Run(t,4)&&g_pose6689.kick176.active&&g_pose6689.kick176.serial==8&&g_pose6689.kick176.started==t.tick,
        "first newer tracking sample starts retained request");
    const auto scheduledStart=g_pose6689.kick176.started;
    Check(Run(t,4)&&g_pose6689.kick176.started==scheduledStart&&!mockKickQueries176,"repeated callback cannot restart retained request");
    SetupKick176();t=Tracking();hostTickCount64=t.tick+1;RequestKickFixture176(9,t.tick+1);
    Check(Run(t,4)&&g_kick_busy176==9,"second pending scheduling case is retained");
    hostTickCount64=t.tick+102;
    Check(Run(t,4)&&!g_kick_busy176&&!g_pose6689.kick176.active&&g_pose6689.kick176.seenRequest==9,
        "actual wall time expires request even while tracking callback remains old");
    t.tick=hostTickCount64;++t.sequence;(void)Run(t,4);
    Check(!g_pose6689.kick176.active&&!mockKickQueries176,"recovered tracking cannot revive expired pending kick");
    std::printf("kick176_bridge=%d/%d actual_coherent=YES native_ray_leaf=REPLACED damage_stamina=DISABLED\n",tests-failures,tests);
    return failures?1:0;
}
