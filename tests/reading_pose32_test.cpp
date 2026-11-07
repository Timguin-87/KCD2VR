// The complete production coherent solver is shared with the retained adapter
// fixture. Only OS/native API leaves and synthetic memory are mocked there.
#include <cstdint>
static std::uint64_t readingClock32=10000;
#define KCD2VR_POSE_FIXTURE_CLOCK readingClock32
#define main retained_pose_adapter_main32
#include "pose_adapter_test.cpp"
#undef main
#include "../src/vrik/visual_selection59c7f.h"

static bool readingCalibrated32=true;static int readingCensus32=3,readingSkin32=0;
static unsigned readingBodyCopies32=0;
namespace kcd2vr::frik_pose_probe {
bool CalibrationReady() noexcept {return readingCalibrated32;}
int CombatCensusMode() noexcept {return readingCensus32;}
int FinalSkinTestMode() noexcept {return readingSkin32;}
DialogueVisual28 DialogueVisualSnapshot28(std::uintptr_t,std::uint64_t) noexcept {
    ++readingBodyCopies32;DialogueVisual28 out{};CommittedFrame6695 frame{};
    if(g_committed_frame6695.read(frame)){
        out.frameCharacter=frame.stamp.actor;out.frameTick=frame.stamp.tick;out.sample=frame.stamp.sample;
        out.poseValid=g_committed_pose_valid6689.load();out.right=frame.valid[0];out.left=frame.valid[1];
    }
    return out; // only existing committed-pose POD leaf, not a native object read
}
}
using namespace kcd2vr;
static bool Produce32(ui_menu_router::Decision ui={},bool theater=false,bool matrix=false,
                      bool vrMenu=false,bool dialogue=false,bool cutscene=false) {
    const bool safe_ui_fit=theater,matrix_active=matrix,mis_menu_after_frame=vrMenu;
    const bool dialogue_active=dialogue,cutscene_flat_ui=cutscene;
    const auto& ui89_policy=ui;
#include "../src/vrik/reading_pose32_producer.inl"
    return readingBody32;
}
static void ClearReading32() {
    auto& w=activity_input163::witness175;
    w.ready=true;w.Clear();w.transitionDepth175=0;
    readingCalibrated32=true;readingCensus32=3;readingSkin32=0;
    diagnostics::lab::Set(diagnostics::lab::Key::F3,0);
    (void)Produce32();
}
int main() {
    ClearReading32();
    Check(!Produce32()&&!reading_pose32::active(10000),"hidden HUD/no activity cannot create Reading visual owner");
    auto& w=activity_input163::witness175;
    w.Begin(activity_input163::Kind175::Reading,0x1234,9999);
    // C31 already admitted Reading body independently of this legacy tool flag.
    activity_input163::trackedHands59c7f[2]=false;
    Check(Produce32()&&reading_pose32::poseAllowed(false,false,10000),"actual producer retains Reading body with legacy saved tool preference false");
    Check(!activity_input163::TrackedHands59c7f(),"visual owner does not enable tracked tool preference");
    Check(reading_pose32::active(10080)&&!reading_pose32::active(10081)&&!reading_pose32::active(9999),"lease has bounded 80ms age and rejects future publication");
    reading_pose32::Lease old{};Check(reading_pose32::read(old,10000),"current original Reading instance can be copied");
    w.transitionDepth175=1;Check(!reading_pose32::active(10000)&&!Produce32(),"native transition refuses producer and consumer");
    w.transitionDepth175=0;Check(Produce32(),"settled same activity can re-admit visuals");
    w.End(0x1234);w.Begin(activity_input163::Kind175::Reading,0x1234,10000);
    Check(!reading_pose32::active(10000)&&!reading_pose32::valid(old,10000),"same-address native reuse cannot inherit old visual lease");
    Check(Produce32(),"new Reading lifetime requires actual producer");
    w.Begin(activity_input163::Kind175::Alchemy,0x5678,10000);
    Check(!reading_pose32::active(10000)&&Produce32(),"nested alchemy book obtains new exact two-instance visual owner");
    w.End(0x5678);Check(!reading_pose32::active(10000)&&Produce32(),"leaving nested book parent invalidates previous instance pair");
    w.Begin(activity_input163::Kind175::Smithing,0x9abc,10000);
    Check(!Produce32(),"Reading cannot authorize simultaneous smithing");w.End(0x9abc);
    w.Begin(activity_input163::Kind175::Sharpening,0x9abc,10000);
    Check(!Produce32(),"Reading cannot authorize simultaneous sharpening");w.End(0x9abc);
    for(auto owner:{ui_menu_router::Owner::InventoryMap,ui_menu_router::Owner::Shop,
        ui_menu_router::Owner::Frontend,ui_menu_router::Owner::Pause,
        ui_menu_router::Owner::ContextualMenu,ui_menu_router::Owner::CraftingMenu,
        ui_menu_router::Owner::LoadingScreen,ui_menu_router::Owner::GameOver}) {
        Check(Produce32(),"valid Reading precedes real UI transition");
        ui_menu_router::Decision ui{};ui.owner=owner;
        Check(!Produce32(ui)&&!reading_pose32::poseAllowed(false,false,10000),"real UI revokes old Reading lease immediately");
    }
    ui_menu_router::Decision ui{};ui.owner=ui_menu_router::Owner::NativeContext;ui.input=ui_menu_router::Input::PreserveNative;
    Check(Produce32(ui),"native Reading controls keep visual body without gameplay input");
    Check(Produce32(ui,false,false,false,false,true)&&
        reading_pose32::nativeContext(true,true,10000,ui),
        "actual native Reading context retains immersive body/head despite lingering cinematic bit");
    Check(!reading_pose32::nativeContext(true,true,10000,{}),
        "ownerNone authored cinematic cannot acquire Reading head policy");
    ui.presentation=ui_menu_router::Presentation::ApseTheater;
    Check(!Produce32(ui)&&!reading_pose32::nativeContext(true,true,10000,ui),
        "actual Theater presentation wins even if native-context label remains");
    ui.presentation=ui_menu_router::Presentation::Immersive;
    ui.uncertain=1;Check(!Produce32(ui),"unknown UI denies Reading");
    Check(!Produce32({},true)&&!Produce32({},false,true)&&!Produce32({},false,false,true)&&
        !Produce32({},false,false,false,true)&&!Produce32({},false,false,false,false,true),"Theater matrix VR menu dialogue cinematic all deny Reading");
    diagnostics::lab::Set(diagnostics::lab::Key::F3,1);Check(!Produce32(),"native reference excludes Reading visuals");
    diagnostics::lab::Set(diagnostics::lab::Key::F3,0);
    readingCalibrated32=false;Check(!Produce32(),"uncalibrated Reading cannot write body");readingCalibrated32=true;
    readingCensus32=0;Check(!Produce32(),"VRIK off remains off while Reading");readingCensus32=3;
    w.ready=false;Check(!Produce32(),"native lifecycle installation missing refuses Reading");w.ready=true;
    ui={};ui.owner=ui_menu_router::Owner::NativeContext;
    w.transitionDepth175=1;Check(!reading_pose32::nativeContext(true,true,10000,ui),
        "transitioning Reading cannot revive head policy");w.transitionDepth175=0;

    // Exercise the ACTUAL solver handover and finger consumers. Native book
    // grip is retained; no F5/F10 experiment or gameplay owner is synthesized.
    Init();auto t=Tracking();ClearReading32();
    g_gameplay_allowed=true;g_weapon_control_mode=0;mockPhysical=false;
    Check(Run(t),"ordinary baseline produces calibrated body");
    const V3 controllerR=FromT(g_pose6689.work[rig.hand_r]);
    for(int side=0;side<2;++side){
        auto& s=g_pose6689;s.open[side].ready=s.closed[side].ready=true;
        for(int n=0;n<s.fingerCount[side];++n){
            const auto id=s.fingerIds[side][n];s.open[side].local[n]=s.closed[side].local[n]=s.restLocal[id];
            s.closed[side].local[n].q[2]=.47942555f;s.closed[side].local[n].q[3]=.87758256f;
        }
        s.handoverW178r14[side]=1.f;g_native_use_until107[side]=20000;
    }
    w.Begin(activity_input163::Kind175::Reading,0x1234,9999);Check(Produce32(),"production producer admits actual solver Reading test");
    g_gameplay_allowed=false;g_weapon_control_mode=7;mockPhysical=true;physicalPublications=0;
    t.allowed=reading_pose32::poseAllowed(false,false,10000);t.right_grip=t.right_trigger=t.left_grip=t.left_trigger=1;
    ++t.sequence;
    Check(Run(t,3,true),"actual coherent Reading body commits with gameplay disabled");
    Check(g_pose6689.handoverW178r14[0]==0&&g_pose6689.handoverW178r14[1]==0,
        "native book Use does not steal either tracked wrist");
    Check(Len(Sub(FromT(g_pose6689.work[rig.hand_r]),controllerR))<.004f,
        "Reading wrist stays at same calibrated controller target");
    // A book is held longer than the short native Use window. Test native grip
    // preservation after that window too; the generic interaction cache must
    // not accidentally make the unmodified control pass this assertion.
    g_native_use_until107[0]=g_native_use_until107[1]=0;++t.sequence;
    Check(Run(t,3,true),"Reading grip persists after generic native Use window ends");
    bool nativeFingers=true;
    for(int side=0;side<2;++side)for(int n=0;n<g_pose6689.fingerCount[side];++n){
        const int id=g_pose6689.fingerIds[side][n],p=g_pose6689.parent[id];
        const auto nativeLocal=RelativeFromAbsolute(fixture.original[p],fixture.original[id]);
        nativeFingers=nativeFingers&&pc::angle(PQ(QFromQuatT(g_pose6689.local[id])),PQ(QFromQuatT(nativeLocal)))<.0001;
    }
    Check(nativeFingers,"actual solver preserves evaluated native book finger grip despite full Grip/Trigger");
    Check(!workstation178r20c1::ownsWrist(10000)&&!hands59c7f::activityVisual.load()&&physicalPublications==0,
        "Reading visual lease cannot publish tools weapons holsters or F10 ownership");
    Check(Same(fixture.live,fixture.original),"Reading transaction restores native buffer after callback");
    const V3 nativeHeadBefore=FromT(g_pose6689.work[rig.head]);
    ++t.sequence;t.head.x+=.05f;
    Check(Run(t,3,true)&&Len(Sub(FromT(g_pose6689.work[rig.head]),nativeHeadBefore))>.01f,
        "Reading head and upper body respond to headset movement through actual solver");
    bool fixedArms=true;
    for(int side=0;side<2;++side){const auto& r=g_pose6689.rig;
        const int u=side?r.upper_l:r.upper_r,f=side?r.fore_l:r.fore_r,h=side?r.hand_l:r.hand_r;
        fixedArms=fixedArms&&std::fabs(Len(Sub(FromT(g_pose6689.work[f]),FromT(g_pose6689.work[u])))-g_pose6689.upper[side])<.002&&
            std::fabs(Len(Sub(FromT(g_pose6689.work[h]),FromT(g_pose6689.work[f])))-g_pose6689.fore[side])<.002;
    }
    Check(fixedArms,"Reading motion keeps both anatomical arm lengths fixed");
    w.End(0x1234);t.allowed=reading_pose32::poseAllowed(false,false,10000);writeCalls=0;++t.sequence;
    Check(!Run(t)&&writeCalls==0,"native Reading exit revokes actual writer without waiting for another producer");
    Check(readingBodyCopies32==0,"actual Reading producer does zero diagnostic body copies while F2 OFF");
    auto& capture=kcd2vr::diagnostics::capture168::gate;
    capture.select(kcd2vr::diagnostics::capture168::Domain::Hud,true,9999);
    w.Begin(activity_input163::Kind175::Reading,0x1234,9999);const auto beforeLogs=logs.size();
    Check(Produce32()&&readingBodyCopies32==1&&logs.size()==beforeLogs+1&&logs.back().find("C32 HUD_READING_GATE ")==0,
        "actual F2 Reading producer joins permission with committed solver frame receipt");
    for(unsigned i=0;i<1000;++i)(void)Produce32();
    Check(readingBodyCopies32==1&&logs.size()==beforeLogs+1,"diagnostic POD sampling and formatting stay bounded between frame ticks");
    readingClock32=10500;w.End(0x1234);(void)Produce32();
    char nullPointer32[32]{};std::snprintf(nullPointer32,sizeof(nullPointer32),"%p",static_cast<void*>(nullptr));
    Check(readingBodyCopies32==2&&logs.back().find("allowed=0")!=std::string::npos&&logs.back().find(std::string("reading=")+nullPointer32)!=std::string::npos,
        "actual F2 producer reports native Reading lifetime exit without retaining permission");
    capture.select(kcd2vr::diagnostics::capture168::Domain::Hud,false,10501);const auto offCopies=readingBodyCopies32;
    readingClock32=11000;(void)Produce32();Check(readingBodyCopies32==offCopies,"capture stop immediately removes diagnostic snapshot reads");
    diagnostics::ReadingReceipts32 recorder;unsigned inputs=0;std::vector<std::string> rows;
    auto input=[&]{++inputs;diagnostics::ReadingRecord32 r{};r.flags=inputs&1u;return r;};
    auto sink=[&](const char* line){rows.emplace_back(line);};
    recorder.Observe(11000,input,sink);Check(inputs==0&&rows.empty(),"observer OFF never evaluates input supplier");
    capture.select(kcd2vr::diagnostics::capture168::Domain::Hud,true,11000);
    for(unsigned now=11000;now<100000;now+=10)recorder.Observe(now,input,sink);
    Check(rows.size()==32&&inputs==32,"F2 session budget stops at32 receipts and32 POD samples");
    recorder.Observe(191001,input,sink);Check(rows.size()==32&&inputs==32,"expired F2 capture cannot refresh diagnostic budget");
    capture.select(kcd2vr::diagnostics::capture168::Domain::Hud,true,200000);
    recorder.Observe(200000,input,sink);Check(rows.size()==33&&inputs==33,"explicit new F2 session alone rearms receipt budget");
    capture.select(kcd2vr::diagnostics::capture168::Domain::Hud,false,200001);
    std::printf("reading32=%d/%d actual_producer=YES actual_coherent_writer=YES native_leaves=MOCKS headset=NOT_TESTED\n",tests-failures,tests);
    return failures?1:0;
}
