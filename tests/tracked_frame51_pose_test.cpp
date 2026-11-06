// Actual coherent_pose.inl and physical kernel, retaining the existing native
// getter/setter leaves; this fixture adds real two-leg topology and direct
// endpoint expectations instead of the retired shoulder-ratio contract.
#include <cstdint>
static std::uint64_t frameClock51=10000;
#define KCD2VR_POSE_FIXTURE_CLOCK frameClock51
#define KCD2VR_POSE_FIXTURE_COUNT 64
#define main historical_pose_adapter51
#include "pose_adapter_test.cpp"
#undef main

namespace tf=kcd2vr::tracked_frame51;
static kcd2vr::frik_pose_probe::GameStateContext poseContext55{};
static bool refreshPoseContext55=true;
static void InitFrame51(bool smooth=false){
    Init();g_frame_control51=smooth?3:2;g_body_follow128=false;
    animation_filter::nativeBodyOwned112=false;g_ctx_menu=g_ctx_dialogue=g_ctx_cutscene=g_ctx_scripted=g_ctx_reading=false;
    poseContext55={};refreshPoseContext55=true;g_tool_context19c4r2=0;mockModelWorldHook55=nullptr;
    g_eye_forward115=g_eye_up115=0;g_head_raise_max59c9=.40;frameClock51=10000;
    for(int side=0;side<2;++side){const int hip=47+side*4;const float sign=side?-1.f:1.f;
        fixture.parent[hip]=rig.pelvis;fixture.local[hip].t[0]=sign*.09f;fixture.local[hip].t[1]=0;fixture.local[hip].t[2]=-.04f;
        for(int n=1;n<=3;++n){fixture.parent[hip+n]=hip+n-1;fixture.local[hip+n].t[0]=0;
            fixture.local[hip+n].t[1]=n==1?.025f:n==2?-.025f:.06f;fixture.local[hip+n].t[2]=n==3?-.07f:-.36f;}
    }
    for(int j=0;j<Count;++j)fixture.reference[j]=fixture.parent[j]<0?fixture.local[j]:AbsoluteFromRelative(fixture.reference[fixture.parent[j]],fixture.local[j]);
    fixture.original=fixture.live=fixture.reference;
    g_cal_native_head_x=fixture.reference[rig.head].t[0];g_cal_native_head_y=fixture.reference[rig.head].t[1];g_cal_native_head_z=fixture.reference[rig.head].t[2];
    g_pose6689.ready=false;g_graspAcquisition54=nullptr;kcd2vr::grasp13::foundationReaderD56=nullptr;g_graspPendingUse55=nullptr;(void)g_tracked_frame51.publish({});
}
static void NativePelvis51(V3 offset){
    fixture.original=fixture.reference;
    for(int j=0;j<Count;++j){int p=j;while(p>=0&&p!=rig.pelvis)p=fixture.parent[p];
        if(p==rig.pelvis){for(int c=0;c<3;++c)fixture.original[j].t[c]+=c==0?offset.x:c==1?offset.y:offset.z;}}
    fixture.live=fixture.original;
}
static bool TickFrame51(TrackingSnapshot& t,bool publish=true){++t.sequence;t.tick=frameClock51+=10;t.timeNs=t.tick*1000000;
    if(refreshPoseContext55)PublishPoseContext55(poseContext55);
    return Run(t,3,publish);}
static kcd2vr::grasp13::Acquisition54 pickupWitness54{};
static bool pickupWitnessReady54=false,pickupWitnessStale54=false;
static bool PickupWitness54(kcd2vr::grasp13::Acquisition54& out) noexcept {
    out=pickupWitness54;if(!pickupWitnessStale54)out.tick=frameClock51;return pickupWitnessReady54;
}
static kcd2vr::grasp13::PendingUse55 pendingWitness55{};
static bool pendingReady55=false;
static bool PendingWitness55(kcd2vr::grasp13::PendingUse55& out) noexcept {out=pendingWitness55;return pendingReady55;}
static kcd2vr::grasp13::HandFrame publishedGraspHands55{};
static unsigned graspHandsPublications55=0,resetWorldReads55=0;
static void CaptureGraspHands55(const kcd2vr::grasp13::HandFrame& hands) noexcept {publishedGraspHands55=hands;++graspHandsPublications55;}
static void RotateNativePelvis54(V3 axis,double degrees){
    fixture.original=fixture.reference;auto locals=fixture.local;
    const double half=degrees*3.14159265358979323846/360;
    locals[rig.pelvis].q[0]=float(axis.x*std::sin(half));
    locals[rig.pelvis].q[1]=float(axis.y*std::sin(half));
    locals[rig.pelvis].q[2]=float(axis.z*std::sin(half));
    locals[rig.pelvis].q[3]=float(std::cos(half));
    for(int j=0;j<Count;++j)fixture.original[j]=fixture.parent[j]<0?locals[j]:AbsoluteFromRelative(fixture.original[fixture.parent[j]],locals[j]);
    fixture.live=fixture.original;
}
int main(){
    tests=failures=0;
    for(bool smooth:{false,true}){
        InitFrame51(smooth);auto t=Tracking();
        Check(TickFrame51(t)&&g_pose6689.legsReady128,"actual whole-body reference discovers two fixed-length legs");
        tf::Frame frame{};Check(g_tracked_frame51.read(frame)&&frame.valid&&frame.publication&&
              frame.publication==g_frame_publication51.load()&&!(frame.publication&1),
              "actual successful pose publishes its own even native-pose publication serial");
        const auto beforeNative=fixture.original;const auto beforeRight=FromT(g_pose6689.work[rig.hand_r]);const auto beforeHead=FromT(g_pose6689.work[rig.head]);
        // Native upper-body pose deliberately changes while controllers stay still.
        fixture.original[rig.head].t[1]+=.18f;fixture.original[rig.head].t[2]-=.24f;
        fixture.original[rig.upper_r].t[2]-=.10f;fixture.live=fixture.original;
        Check(TickFrame51(t),"native attack/bow upper-body slump still permits one completed pose");
        Check(Len(Sub(FromT(g_pose6689.work[rig.head]),beforeHead))<.002&&Len(Sub(FromT(g_pose6689.work[rig.hand_r]),beforeRight))<.002,
              "native head/shoulder animation cannot move tracked head or hand with comfort ON or OFF");
        Check(Same(fixture.live,fixture.original),"shared frame path restores the untouched native bone buffer");
        // Calibrated user arm geometry must not scale a reachable controller goal.
        fixture.original=fixture.live=beforeNative;g_arm_profile66100.publish({1.55,.77,.79,.24});
        t.right.x+=.025f;const auto raw=Add(FromT(fixture.reference[rig.head]),XrToCryV(t.right));
        Check(TickFrame51(t)&&Len(Sub(FromT(g_pose6689.work[rig.hand_r]),raw))<.002,
              "current saved arm profile does not remap reachable controller position");
        Check(g_tracked_frame51.read(frame),"read frame for HMD-relative endpoint check");
        Check(pc::length(PV(FromT(g_pose6689.work[rig.hand_r]))-frame.anchor-PV(XrToCryV(t.right)))<.002,
              "hand endpoint remains in the same frame as tracked headset");
        NativePelvis51({.02f,.03f,-.12f});
        Check(TickFrame51(t)&&g_tracked_frame51.read(frame)&&frame.valid,"native gait/crouch pose publishes one corrected frame");
        const auto actualPelvis=PV(FromT(g_pose6689.work[rig.pelvis]));const auto nativePelvis=PV(FromT(fixture.original[rig.pelvis]));
        Check(pc::length(actualPelvis-nativePelvis-frame.bodyOffset)<.0001,
              "the actual pelvis receives the identical correction used by head and hands");
        Check(pc::length(PV(FromT(g_pose6689.work[rig.hand_r]))-frame.anchor-PV(XrToCryV(t.right)))<.002,
              "walking does not slide an unmodified animated hand under the camera");
        for(int side=0;side<2;++side){const auto& leg=g_pose6689.leg[side];
            Check(std::fabs(Len(Sub(FromT(g_pose6689.work[leg.knee]),FromT(g_pose6689.work[leg.hip])))-leg.thigh)<.0001&&
                  std::fabs(Len(Sub(FromT(g_pose6689.work[leg.ankle]),FromT(g_pose6689.work[leg.knee])))-leg.shin)<.0001,
                  "pelvis compensation preserves both actual leg lengths");}
        const auto stateBefore=g_pose6689.trackedFrame51;const auto publicationBefore=g_frame_publication51.load();
        Check(Run(t,3,true)&&g_pose6689.trackedFrame51.timeNs==stateBefore.timeNs&&
              pc::length(g_pose6689.trackedFrame51.filtered-stateBefore.filtered)<1e-9,
              "duplicate pose consumer does not integrate shared smoothing again");
        Check(g_tracked_frame51.read(frame)&&frame.publication>publicationBefore&&frame.publication==g_frame_publication51.load(),
              "same XR sample creates a distinct completed native-pose publication");
        fixture.original[rig.head].t[1]+=.02f;fixture.live=fixture.original;
        Check(Run(t,3,true)&&pc::length(g_pose6689.trackedFrame51.filtered-stateBefore.filtered)<1e-9,
              "actual repeated callback with changed native head retains zero-dt shared smoothing");
        failAt=rig.hand_r;failOnce=false;++t.sequence;t.tick=frameClock51+=10;
        Check(!Run(t,3,true)&&!g_pose6689.trackedFrame51.ready&&g_tracked_frame51.read(frame)&&!frame.valid&&
              g_frame_publication51.load()>publicationBefore&&!(g_frame_publication51.load()&1),
              "failed native write retires the shared frame and temporal state");
        Check(Same(fixture.live,fixture.original),"failed shared-frame write still restores all attempted native joints");
    }
    // Production regression: native pelvis pitch/roll used to alter the
    // supposedly neutral neck and spine while the completed frame stayed still.
    // Include yaw, both comfort modes and actual fixed-length leg preservation.
    for(bool smooth:{false,true})for(auto axis:{V3{1,0,0},V3{0,1,0},V3{0,0,1}}){
        InitFrame51(smooth);auto t=Tracking();Check(TickFrame51(t),"pelvis foundation baseline");
        const auto head=FromT(g_pose6689.work[rig.head]),chest=FromT(g_pose6689.work[rig.spine3]),wrist=FromT(g_pose6689.work[rig.hand_r]);
        RotateNativePelvis54(axis,15);
        Check(TickFrame51(t),"ordinary tracked frame accepts native pelvis tilt");
        Check(Len(Sub(FromT(g_pose6689.work[rig.head]),head))<.0001&&
              Len(Sub(FromT(g_pose6689.work[rig.hand_r]),wrist))<.0001,
              "native pelvis tilt cannot leak into tracked head or hand");
        if(axis.z==0)Check(Len(Sub(FromT(g_pose6689.work[rig.spine3]),chest))<.0001,
              "ordinary chest foundation has no native pitch-roll oscillation");
        else Check(pc::angle(PQ(QFromQuatT(g_pose6689.work[rig.pelvis])),PQ(QFromQuatT(fixture.reference[rig.pelvis])))<1e-5&&
                   Len(Sub(FromT(g_pose6689.work[rig.spine3]),chest))<.0001,
              "D56 authored local pelvis yaw cannot add a second shoulder basis; world heading stays separate");
        for(int side=0;side<2;++side){const auto& leg=g_pose6689.leg[side];
            Check(std::fabs(Len(Sub(FromT(g_pose6689.work[leg.knee]),FromT(g_pose6689.work[leg.hip])))-leg.thigh)<.0001&&
                  std::fabs(Len(Sub(FromT(g_pose6689.work[leg.ankle]),FromT(g_pose6689.work[leg.knee])))-leg.shin)<.0001,
                  "pelvis angular correction preserves fixed thigh/shin lengths");}
        Check(Same(fixture.live,fixture.original),"angular correction restores all native joints after publication");
    }
    // C55 regression request: compare the existing elbow/pole solver through a
    // complete moving-hand sweep with and without animated native pelvis tilt
    // and gait. Everything, including skin roll, must share the same model-space
    // frame; the elbow history is NOT reset to hide a discontinuity.
    for(bool smooth:{false,true}){
        struct Arm55 {V3 elbow[2]{},hand[2]{};Q4 upper[2]{},fore[2]{};};
        std::array<Arm55,120> reference55{};bool all55=true;double worstElbow55=0,worstRoll55=0,worstWrist55=0;
        for(unsigned pass55=0;pass55<2;++pass55){
            InitFrame51(smooth);auto move55=Tracking();
            for(unsigned n55=0;n55<reference55.size();++n55){
                const float phase55=float(n55)*.035f;
                move55.right.x=.33f+.04f*std::sin(phase55);move55.left.x=-move55.right.x;
                move55.right.y=move55.left.y=1.22f-fixture.reference[rig.head].t[2]+.04f*std::sin(phase55*.7f);
                move55.right.z=move55.left.z=-.25f-.03f*std::cos(phase55);
                if(pass55){
                    RotateNativePelvis54({1,0,0},15*std::sin(phase55));
                    const float gait55=-.035f*(1+std::sin(phase55*2));
                    for(int j55=0;j55<Count;++j55){int parent55=j55;while(parent55>=0&&parent55!=rig.pelvis)parent55=fixture.parent[parent55];
                        if(parent55==rig.pelvis)fixture.original[j55].t[2]+=gait55;}
                    fixture.live=fixture.original;
                }
                all55=TickFrame51(move55)&&all55;
                for(int side55=0;side55<2;++side55){
                    const int upper55=side55?rig.upper_l:rig.upper_r,fore55=side55?rig.fore_l:rig.fore_r,hand55=side55?rig.hand_l:rig.hand_r;
                    const auto elbow55=FromT(g_pose6689.work[fore55]),wrist55=FromT(g_pose6689.work[hand55]);
                    const auto uq55=QFromQuatT(g_pose6689.work[upper55]),fq55=QFromQuatT(g_pose6689.work[fore55]);
                    if(!pass55){auto& r55=reference55[n55];r55.elbow[side55]=elbow55;r55.hand[side55]=wrist55;r55.upper[side55]=uq55;r55.fore[side55]=fq55;}
                    else {const auto& r55=reference55[n55];worstElbow55=std::max(worstElbow55,double(Len(Sub(elbow55,r55.elbow[side55]))));
                        worstWrist55=std::max(worstWrist55,double(Len(Sub(wrist55,r55.hand[side55]))));
                        worstRoll55=std::max({worstRoll55,pc::angle(PQ(uq55),PQ(r55.upper[side55])),pc::angle(PQ(fq55),PQ(r55.fore[side55]))});}
                    all55=all55&&std::fabs(Len(Sub(elbow55,FromT(g_pose6689.work[upper55])))-g_pose6689.upper[side55])<.0001&&
                        std::fabs(Len(Sub(wrist55,elbow55))-g_pose6689.fore[side55])<.0001;
                }
            }
        }
        std::printf("C55_ELBOW_CONTINUITY smooth=%d complete=%d elbowDelta=%.9f wristDelta=%.9f skinAngleDelta=%.9f\n",smooth,all55,worstElbow55,worstWrist55,worstRoll55);
        Check(all55&&worstElbow55<.0001&&worstWrist55<.0001&&worstRoll55<.001,
              "continuous native pelvis tilt/gait cannot spin or move elbows relative to the same moving tracked targets");
    }
    // The supplied review reproduced a 9.64 cm head sink after one second of
    // a 10 cm authored pelvis drop, and full reseeding on native-owner exit.
    // HMD height ON corrects the actual body and frame together, not camera only.
    for(bool smooth:{false,true}){
        InitFrame51(smooth);auto t=Tracking();Check(TickFrame51(t),"owned height baseline");
        const auto head=FromT(g_pose6689.work[rig.head]),chest=FromT(g_pose6689.work[rig.spine3]),wrist=FromT(g_pose6689.work[rig.hand_r]);
        NativePelvis51({0,0,-.10f});bool complete=true;
        for(unsigned n=0;n<100;++n)complete=TickFrame51(t)&&complete;
        Check(complete&&Len(Sub(FromT(g_pose6689.work[rig.head]),head))<.0001&&
              Len(Sub(FromT(g_pose6689.work[rig.spine3]),chest))<.0001&&
              Len(Sub(FromT(g_pose6689.work[rig.hand_r]),wrist))<.0001,
              "sustained native hip depression cannot sink shared head chest or wrist");
        animation_filter::nativeBodyOwned112=true;Check(TickFrame51(t),"protected native reaction still runs with HMD height enabled");
        tf::Frame native{};Check(g_tracked_frame51.read(native)&&!native.valid,"protected reaction keeps its explicit camera owner");
        animation_filter::nativeBodyOwned112=false;
        Check(TickFrame51(t)&&Len(Sub(FromT(g_pose6689.work[rig.head]),head))<.0001&&
              Len(Sub(FromT(g_pose6689.work[rig.spine3]),chest))<.0001,
              "return from native owner cannot reseed depressed standing anatomy");
        Check(Same(fixture.live,fixture.original),"owned-height path restores untouched native buffer");
        g_frame_control51.fetch_or(kNativeBodyHeightBit54);
        Check(TickFrame51(t)&&std::fabs(FromT(g_pose6689.work[rig.head]).z-(head.z-.10f))<.0001,
              "explicit legacy native-height option retains button-only native stance lowering");
    }
    // Accepted native acquisition keeps its pre-Queue presentation foundation;
    // it never skips the real Queue/Attach or lends gameplay/attack permission.
    for(bool smooth:{false,true}){
        InitFrame51(smooth);auto t=Tracking();Check(TickFrame51(t),"pickup pre-Queue completed body reference");
        tf::Frame before{};g_tracked_frame51.read(before);
        const auto head=FromT(g_pose6689.work[rig.head]),chest=FromT(g_pose6689.work[rig.spine3]);
        pickupWitness54={before.stamp.actor,0x2000,0x3000,23,before.stamp.epoch,frameClock51+1,frameClock51+1,17,0,true};
        pickupWitness54.trackingEpoch55=t.source_epoch=kcd2vr::tracking59c6::epoch.load(std::memory_order_acquire);
        pickupWitnessReady54=true;pickupWitnessStale54=false;g_graspAcquisition54=PickupWitness54;
        g_gameplay_allowed=false;g_ctx_scripted=true;t.allowed=false;t.right_grip=1;
        NativePelvis51({.05f,.20f,-.03f});
        Check(TickFrame51(t),"exact accepted Picking allows presentation-only body frame");
        tf::Frame held{};g_tracked_frame51.read(held);
        Check(held.valid&&held.acquisitionSerial==23&&held.acquisitionAction==0x2000&&
              Len(Sub(FromT(g_pose6689.work[rig.head]),head))<.0001&&
              Len(Sub(FromT(g_pose6689.work[rig.spine3]),chest))<.0001,
              "native reach pelvis lunge cannot move tracked upper body before Attach");
        Check(!g_gameplay_allowed&&Same(fixture.live,fixture.original),"acquisition foundation leaves native permission/buffer unchanged");
        for(int n=0;n<110;++n)TickFrame51(t);
        g_tracked_frame51.read(held);
        Check(held.valid&&Len(Sub(FromT(g_pose6689.work[rig.head]),head))<.0001,
              "same current action retains anatomy beyond80ms without replaying tracking");
        pickupWitnessReady54=false;
        Check(!TickFrame51(t)&&g_tracked_frame51.read(held)&&!held.valid,
              "released acquisition cannot borrow old presentation ownership");
        g_gameplay_allowed=true;g_ctx_scripted=false;t.allowed=true;
        InitFrame51(smooth);t=Tracking();TickFrame51(t);g_tracked_frame51.read(before);
        pickupWitness54={before.stamp.actor,0x2000,0x3000,24,before.stamp.epoch,frameClock51+200,frameClock51+200,17,0,true};
        pickupWitness54.trackingEpoch55=t.source_epoch=kcd2vr::tracking59c6::epoch.load(std::memory_order_acquire);
        pickupWitnessReady54=true;g_graspAcquisition54=PickupWitness54;
        g_gameplay_allowed=false;g_ctx_scripted=true;t.allowed=false;t.right_grip=1;frameClock51+=200;
        Check(TickFrame51(t)&&g_tracked_frame51.read(held)&&!held.valid,
              "acquisition without current pre-Queue reference cannot invent a foundation");
        g_gameplay_allowed=true;g_ctx_scripted=false;
    }
    // The recorded native action can bypass the observed Queue and only become
    // visible after ordinary pose frames have already begun the reach. Retain
    // pre-Use anatomy, then require the later exact installed action to use it.
    for(bool smooth:{false,true}){
        InitFrame51(smooth);auto t=Tracking();t.right_grip=1;TickFrame51(t);
        tf::Frame before{};g_tracked_frame51.read(before);
        const auto head=FromT(g_pose6689.work[rig.head]),chest=FromT(g_pose6689.work[rig.spine3]);
        pendingWitness55={before.stamp.actor,before.stamp.epoch,55,frameClock51+1,0};
        pendingReady55=true;g_graspPendingUse55=PendingWitness55;
        for(unsigned n=0;n<12;++n){NativePelvis51({.03f,.12f,-.02f});Check(TickFrame51(t),"pending Use keeps ordinary native authority before installed Picking");}
        Check(g_pose6689.ordinaryReference54.stamp.tick>pendingWitness55.requestedAt&&
              g_pose6689.pendingReference55.stamp.tick==before.stamp.tick,
              "pre-Use reference survives later ordinary reach frames without relearning their lean");
        pickupWitness54={before.stamp.actor,0x2000,0x3000,55,before.stamp.epoch,pendingWitness55.requestedAt,frameClock51,17,0,true};
        pickupWitness54.trackingEpoch55=t.source_epoch=kcd2vr::tracking59c6::epoch.load(std::memory_order_acquire);
        pickupWitnessReady54=true;pickupWitnessStale54=false;g_graspAcquisition54=PickupWitness54;
        const auto savedEnabled=g_graspEnabled13;const auto savedHands=g_graspHands13;
        g_graspEnabled13=+[]() noexcept {return true;};g_graspHands13=CaptureGraspHands55;
        g_gameplay_allowed=false;g_ctx_scripted=true;t.allowed=false;
        Check(TickFrame51(t),"late installed Picking can use only its matching pre-Use reference");
        tf::Frame acquired{};g_tracked_frame51.read(acquired);
        Check(acquired.valid&&acquired.acquisitionSerial==55&&
              Len(Sub(FromT(g_pose6689.work[rig.head]),head))<.0001&&
              Len(Sub(FromT(g_pose6689.work[rig.spine3]),chest))<.0001,
              "late native owner restores same tracked head and chest foundation");
        Check(!g_gameplay_allowed&&publishedGraspHands55.tracked[0]&&!publishedGraspHands55.tracked[1]&&
              publishedGraspHands55.tick==t.tick,
              "current acquisition hand retains real tracking without lending gameplay or off-hand authority");
        ++pickupWitness54.serial;
        Check(TickFrame51(t)&&g_tracked_frame51.read(acquired)&&!acquired.valid,
              "a different native Use serial cannot borrow retained pre-Use anatomy");
        g_graspEnabled13=savedEnabled;g_graspHands13=savedHands;
        g_gameplay_allowed=true;g_ctx_scripted=false;pendingReady55=false;
    }
    // The packed real-UI publication precedes generic gameplay flags in Mod.
    // Use its actual publisher/decoder even when g_ctx_menu is still false.
    // A current native owner cannot lend the pose another source session.
    for(bool smooth:{false,true})for(unsigned refusal55=0;refusal55<6;++refusal55){
        InitFrame51(smooth);auto t=Tracking();t.right_grip=1;TickFrame51(t);
        tf::Frame before{};g_tracked_frame51.read(before);
        pickupWitness54={before.stamp.actor,0x2000,0x3000,61,before.stamp.epoch,frameClock51+1,frameClock51+1,17,0,true};
        pickupWitness54.trackingEpoch55=t.source_epoch=kcd2vr::tracking59c6::epoch.load(std::memory_order_acquire);
        pickupWitnessReady54=true;pickupWitnessStale54=false;g_graspAcquisition54=PickupWitness54;
        g_gameplay_allowed=false;g_ctx_scripted=true;t.allowed=false;NativePelvis51({0,.12f,-.02f});
        if(refusal55==0)poseContext55.tool_pose_ui_blocked=true;
        else if(refusal55==1){refreshPoseContext55=false;frameClock51+=71;}
        else if(refusal55==2){refreshPoseContext55=false;g_tool_context19c4r2=(frameClock51+11)<<8;}
        else if(refusal55==3){refreshPoseContext55=false;g_tool_context19c4r2=0;}
        else if(refusal55==4)++t.source_epoch;
        else t.source_epoch=0;
        static constexpr const char* reasons55[]{"packed safe-UI/matrix flag refuses Picking before generic menu flag",
            "81ms old packed context refuses Picking", "future packed context refuses Picking",
            "missing packed context refuses Picking", "different composed tracking epoch refuses Picking",
            "missing composed tracking epoch refuses Picking"};
        const int priorWrites=writeCalls;
        Check(!TickFrame51(t)&&!g_ctx_menu&&writeCalls==priorWrites&&Same(fixture.live,fixture.original),reasons55[refusal55]);
        tf::Frame denied{};Check(g_tracked_frame51.read(denied)&&!denied.valid&&!g_gameplay_allowed,
            "refused Picking cannot retain a shared camera frame or gameplay authority");
        g_gameplay_allowed=true;g_ctx_scripted=false;
    }
    // Recenter/source selection increments the real tracking-family epoch
    // before a newer raw input can retire the old native acquisition mailbox.
    // Exercise the actual pose writer and publication at all three intervals.
    for(bool smooth:{false,true})for(unsigned resetAt55=0;resetAt55<3;++resetAt55){
        InitFrame51(smooth);auto t=Tracking();t.right_grip=1;TickFrame51(t);
        tf::Frame before{};g_tracked_frame51.read(before);
        pickupWitness54={before.stamp.actor,0x2000,0x3000,71,before.stamp.epoch,frameClock51+1,frameClock51+1,17,0,true};
        pickupWitness54.trackingEpoch55=t.source_epoch=kcd2vr::tracking59c6::epoch.load(std::memory_order_acquire);
        pickupWitnessReady54=true;pickupWitnessStale54=false;g_graspAcquisition54=PickupWitness54;
        g_gameplay_allowed=false;g_ctx_scripted=true;t.allowed=false;NativePelvis51({0,.12f,-.02f});
        const auto savedEnabled=g_graspEnabled13;const auto savedHands=g_graspHands13;
        g_graspEnabled13=+[]() noexcept {return true;};g_graspHands13=CaptureGraspHands55;
        publishedGraspHands55={};graspHandsPublications55=resetWorldReads55=0;
        ++t.sequence;t.tick=frameClock51+=10;t.timeNs=t.tick*1000000;PublishPoseContext55(poseContext55);
        const int priorWrites=writeCalls;
        if(resetAt55==0)kcd2vr::tracking59c6::Reset();
        else if(resetAt55==1)mockModelWorldHook55=+[](){
            ++resetWorldReads55;mockModelWorldHook55=nullptr;kcd2vr::tracking59c6::Reset();};
        CoherentTransaction6689 tx;
        const bool solved=BeginCoherent6689(tx,&fixture,&fixture,character.data(),nullptr,Count,rig,MockGet,3,t);
        tx.Restore();
        if(resetAt55==2){
            Check(solved&&tx.committed&&tx.frame51.valid&&tx.frame51.acquisitionSerial==71,
                "same-source Picking can commit and restore before a later source reset");
            kcd2vr::tracking59c6::Reset();
        }
        PublishCommitted6689(tx);
        if(resetAt55<2)Check(!solved&&writeCalls==priorWrites&&resetWorldReads55==resetAt55,
            resetAt55==0?"source reset before Begin refuses old Picking without native writes":
                         "source reset during native world read refuses old Picking before native writes");
        tf::Frame denied{};
        Check(Same(fixture.live,fixture.original)&&g_tracked_frame51.read(denied)&&!denied.valid&&
              !g_gameplay_allowed&&!publishedGraspHands55.tracked[0]&&!publishedGraspHands55.tracked[1]&&
              graspHandsPublications55==0u,
              "reset acquisition cannot publish a usable shared frame or tracked native hand after restoration");
        g_graspEnabled13=savedEnabled;g_graspHands13=savedHands;mockModelWorldHook55=nullptr;
        g_gameplay_allowed=true;g_ctx_scripted=false;
    }
    // C57: revoke the original acquisition while the ACTUAL production solver
    // is reading its native model transform. No held/composed pose may substitute
    // for a released Grip or a changed semantic policy/owner.
    for(bool smooth:{false,true})for(unsigned revoke57=0;revoke57<15;++revoke57){
        InitFrame51(smooth);auto t=Tracking();t.right_grip=1;TickFrame51(t);
        tf::Frame before{};g_tracked_frame51.read(before);
        pickupWitness54={before.stamp.actor,0x2000,0x3000,91,before.stamp.epoch,frameClock51+1,frameClock51+1,17,0,true};
        pickupWitness54.generation55=7;
        pickupWitness54.trackingEpoch55=t.source_epoch=kcd2vr::tracking59c6::epoch.load(std::memory_order_acquire);
        pickupWitnessReady54=true;pickupWitnessStale54=false;g_graspAcquisition54=PickupWitness54;
        g_gameplay_allowed=false;g_ctx_scripted=true;t.allowed=false;NativePelvis51({0,.12f,-.02f});
        static unsigned cause57=0;cause57=revoke57;
        mockModelWorldHook55=+[](){
            mockModelWorldHook55=nullptr;
            switch(cause57){
             case 0:pickupWitnessReady54=false;break; // real getter false after raw Grip/source retirement
             case 1:++pickupWitness54.serial;break;
             case 2:++pickupWitness54.action;break;
             case 3:++pickupWitness54.scope;break;
             case 4:++pickupWitness54.nativeToken;break;
             case 5:pickupWitness54.side=1;break;
             case 6:++pickupWitness54.epoch;break;
             case 7:++pickupWitness54.generation55;break;
             case 8:++pickupWitness54.startedAt;break;
             case 9:g_ctx_menu=true;break;
             case 10:poseContext55.tool_pose_ui_blocked=true;PublishPoseContext55(poseContext55);break;
             case 11:g_pose_policy_generation57.fetch_add(1);break; // native policy publication in progress
             case 12:g_pose_policy_generation57.fetch_add(4);break; // menu opened AND closed during solve
             case 13:animation_filter::nativeBodyOwned112=true;break;
             case 14:g_ctx_reading=true;break;
            }
        };
        const int priorWrites=writeCalls;const bool solved=TickFrame51(t);
        tf::Frame denied{};
        Check(!solved&&writeCalls==priorWrites&&Same(fixture.live,fixture.original)&&
              g_tracked_frame51.read(denied)&&!denied.valid&&!g_committed_pose_valid6689,
              "original acquisition/semantic revocation during solve refuses ALL bone writes and pose publication");
        g_gameplay_allowed=true;g_ctx_scripted=false;mockModelWorldHook55=nullptr;
    }
    // Check the exact seam used by the native skinning callback, as well as
    // publication after successful restoration. Neither stage may reuse a
    // completed but subsequently revoked scoped pose.
    for(bool smooth:{false,true})for(unsigned stage57=0;stage57<3;++stage57){
        InitFrame51(smooth);auto t=Tracking();t.right_grip=1;TickFrame51(t);
        tf::Frame before{};g_tracked_frame51.read(before);
        pickupWitness54={before.stamp.actor,0x2000,0x3000,92,before.stamp.epoch,frameClock51+1,frameClock51+1,17,0,true};
        pickupWitness54.generation55=7;
        pickupWitness54.trackingEpoch55=t.source_epoch=kcd2vr::tracking59c6::epoch.load(std::memory_order_acquire);
        pickupWitnessReady54=true;pickupWitnessStale54=false;g_graspAcquisition54=PickupWitness54;
        g_gameplay_allowed=false;g_ctx_scripted=true;t.allowed=false;NativePelvis51({0,.12f,-.02f});
        ++t.sequence;t.tick=frameClock51+=10;t.timeNs=t.tick*1000000;PublishPoseContext55(poseContext55);
        CoherentTransaction6689 tx;bool committed=BeginCoherent6689(tx,&fixture,&fixture,character.data(),nullptr,Count,rig,MockGet,3,t);
        Check(committed&&CoherentOwnerCurrent57(tx),"current raw-backed acquisition may reach scoped native skinning");
        if(stage57==0)pickupWitnessReady54=false;
        auto& transaction=tx;
#include "../src/vrik/native_skinning_acquisition57.inl"
        unsigned nativeSkinningCalls57=0;bool skinningSawNative57=false;
        const auto originalSkinning57=[&](){++nativeSkinningCalls57;skinningSawNative57=Same(fixture.live,fixture.original);};
        originalSkinning57(); // explicit native leaf, invoked once after the actual source seam
        Check(nativeSkinningCalls57==1&&(stage57?committed:!committed&&skinningSawNative57),
              "actual pre-skinning seam calls native once and revoked acquisition restores bones BEFORE that call");
        tx.Restore();
        if(stage57==1){g_ctx_menu=true;g_pose_policy_generation57.fetch_add(2);}
        if(stage57==2){g_pose_policy_generation57.fetch_add(4);} // real menu ABA with same final flags
        PublishCommitted6689(tx);tf::Frame denied{};
        Check(g_tracked_frame51.read(denied)&&!denied.valid&&!g_committed_pose_valid6689&&Same(fixture.live,fixture.original),
              "revocation before skinning/publication cannot publish usable body or hand frame");
        g_gameplay_allowed=true;g_ctx_scripted=false;
    }
    // A transaction beginning during an unfinished semantic publication must
    // refuse even if the visible flags have not been stored yet.
    {
        InitFrame51();auto t=Tracking();t.right_grip=1;TickFrame51(t);tf::Frame before{};g_tracked_frame51.read(before);
        pickupWitness54={before.stamp.actor,0x2000,0x3000,93,before.stamp.epoch,frameClock51+1,frameClock51+1,17,0,true};
        pickupWitness54.trackingEpoch55=t.source_epoch=kcd2vr::tracking59c6::epoch.load(std::memory_order_acquire);
        pickupWitnessReady54=true;pickupWitnessStale54=false;g_graspAcquisition54=PickupWitness54;
        g_gameplay_allowed=false;t.allowed=false;g_pose_policy_generation57.fetch_add(1);
        const int priorWrites=writeCalls;Check(!TickFrame51(t)&&writeCalls==priorWrites&&Same(fixture.live,fixture.original),
            "odd semantic publication revision cannot start acquisition bone ownership");g_gameplay_allowed=true;
    }
    // The user's actual headset crouch is not the game's native stance and
    // must work when optional horizontal roomscale walking is OFF.
    for(bool smooth:{false,true})for(bool follow:{false,true}){
        InitFrame51(smooth);g_body_follow128=follow;auto t=Tracking();
        Check(TickFrame51(t),"physical crouch baseline");
        const auto before=FromT(g_pose6689.work[rig.head]);t.head={0,-.30f,0};
        Check(TickFrame51(t),"physical crouch publishes with horizontal follow ON or OFF");
        tf::Frame frame{};g_tracked_frame51.read(frame);
        { const auto got=FromT(g_pose6689.work[rig.head]),want=Add(VP(frame.anchor),XrToCryV(t.head));std::printf("CROUCH54 smooth=%d follow=%d got=(%.6f,%.6f,%.6f) want=(%.6f,%.6f,%.6f) err=%.6f\n",smooth,follow,got.x,got.y,got.z,want.x,want.y,want.z,Len(Sub(got,want))); }
        Check(Len(Sub(FromT(g_pose6689.work[rig.head]),Add(VP(frame.anchor),XrToCryV(t.head))))<.003,
              "actual head follows full physical crouch at fixed spine lengths");
        Check(FromT(g_pose6689.work[rig.pelvis]).z<fixture.original[rig.pelvis].t[2]-.25f,
              "pelvis participates in tracked crouch independently of horizontal follow");
        for(int side=0;side<2;++side){const auto& leg=g_pose6689.leg[side];
            Check(std::fabs(Len(Sub(FromT(g_pose6689.work[leg.knee]),FromT(g_pose6689.work[leg.hip])))-leg.thigh)<.0001&&
                  std::fabs(Len(Sub(FromT(g_pose6689.work[leg.ankle]),FromT(g_pose6689.work[leg.knee])))-leg.shin)<.0001,
                  "physical crouch retains fixed thigh and shin lengths");}
        t.head={};Check(TickFrame51(t)&&Len(Sub(FromT(g_pose6689.work[rig.head]),before))<.0001,
              "standing physical headset restores the existing head anchor without residual crouch");
    }
    {InitFrame51();g_frame_neutral51=[](std::uint64_t) noexcept {return g_weapon_control_mode.load()==0;};
     auto first55=Tracking();Check(TickFrame51(first55),"same-sample first-draw baseline with explicit native nonbow neutral witness");
     tf::Frame idle55{};g_tracked_frame51.read(idle55);
     mockPhysical=true;g_weapon_control_mode=6;auto& profile55=mockProfiles[1];
     profile55.key=7;profile55.generation=8;profile55.side=1;profile55.classId=9;profile55.primary=true;profile55.support=true;
     first55.right_grip=first55.right_trigger=1;NativePelvis51({.08f,.11f,-.13f});
     Check(Run(first55,3,true),"actual native callback may equip and start draw inside the existing XR sample");
     tf::Frame draw55{};g_tracked_frame51.read(draw55);
     Check(draw55.bow&&draw55.valid&&tf::same_stamp(idle55.stamp,draw55.stamp)&&
           pc::length(draw55.anchor-idle55.anchor)<.0001&&g_pose6689.trackedFrame51.bowReady,
           "actual same-stamp first draw pins prior positively neutral ordinary foundation");g_frame_neutral51=nullptr;}
    // Grip and Trigger share direct controller intent even with a saved profile.
    // Only the existing physical bow/string constraint may choose the allowed
    // endpoint; a native torso animation cannot secretly retarget the input.
    for(bool smooth:{false,true})for(bool grip:{false,true}){
        InitFrame51(smooth);mockGripDraw596c1=grip;
        g_bow_manual_reload146=false;g_bow_native_grab146=false;g_bow_string_trigger160=false;
        g_experiments169=true;g_combat_integrated178r22c2=true;g_chest_natural128=true;
        auto bowTracking=Tracking();bowTracking.left_grip=1;
        const V3 bowHand{-.15f,.35f,1.42f},startHand{-.15f,.25f,1.49f},endHand{.15f,-.12f,1.54f};
        const auto trackingHand=[](V3 p){const V3 d=Sub(p,FromT(fixture.reference[rig.head]));return V3{d.x,d.z,-d.y};};
        bowTracking.left=trackingHand(bowHand);bowTracking.right=trackingHand(startHand);
        Check(TickFrame51(bowTracking),"direct bow fixture establishes one complete anatomical body");
        Check(g_arm_profile66100.publish({1.50,.75,.75,.20}),"direct bow fixture retains a different saved arm profile");
        mockPhysical=true;g_weapon_control_mode=6;
        auto& bow=mockProfiles[1];bow.key=7;bow.generation=3;bow.side=1;bow.classId=9;bow.primary=bow.support=true;
        bow.grip[1].q=PQ(DesiredHand6689(g_pose6689,1,{0,0,0,1},bowTracking.lq));bow.grip[0].p={0,-.10,0};bow.axis={0,-1,0};
        bow.bowNockQualified40=true;
        bool completed=TickFrame51(bowTracking)&&TickFrame51(bowTracking);
        if(grip)bowTracking.right_grip=1;else bowTracking.right_trigger=1;
        completed=TickFrame51(bowTracking)&&completed;
        Check(mockPhysicalOutput.bowHeld,"actual Grip/Trigger kernel acquires the string from direct unscaled controller coordinates");
        const auto headBefore=PV(FromT(g_pose6689.work[rig.head]));bool rawExact=true,allowedExact=true,owned=true;
        for(unsigned j=1;j<=30;++j){const auto goal=Add(startHand,Mul(Sub(endHand,startHand),float(j)/30));
            bowTracking.right=trackingHand(goal);completed=TickFrame51(bowTracking)&&completed;
            rawExact=rawExact&&pc::length(mockPhysicalInput175.hands[0].p-PV(goal))<.004;
            allowedExact=allowedExact&&pc::length(PV(FromT(g_pose6689.work[rig.hand_r]))-mockPhysicalOutput.hands[0].p)<.004;
            owned=owned&&mockPhysicalOutput.owned[0]&&mockPhysicalOutput.bowHeld;
        }
        Check(completed&&owned,"both comfort modes keep every actual Grip/Trigger draw transaction and string ownership");
        Check(rawExact,"actual bow kernel receives the direct controller target without a shoulder/profile remap");
        Check(allowedExact,"one completed drawing hand follows the actual allowed string/weapon endpoint");
        Check(pc::length(PV(FromT(g_pose6689.work[rig.head]))-headBefore)<.002&&Same(fixture.live,fixture.original),
              "actual bow torso participation preserves tracked head and restores the native bone buffer");
    }
    mockGripDraw596c1=false;g_combat_integrated178r22c2=false;
    InitFrame51();g_frame_control51.fetch_or(kNativeBodyHeightBit54);auto t=Tracking();NativePelvis51({0,0,-.35f});
    Check(TickFrame51(t),"first pose may be a real native crouch");tf::Frame frame{};g_tracked_frame51.read(frame);
    Check(frame.valid&&std::fabs(frame.anchor.z-(fixture.reference[rig.head].t[2]-.35))<.0001,
          "crouched first native pose does not overwrite standing anatomy");
    for(int owner=0;owner<5;++owner){InitFrame51(true);t=Tracking();TickFrame51(t);
        switch(owner){case 0:g_gameplay_allowed=false;break;case 1:g_ctx_dialogue=true;break;case 2:g_ctx_scripted=true;break;
        case 3:g_ctx_reading=true;break;case 4:animation_filter::nativeBodyOwned112=true;break;}
        Check(TickFrame51(t)&&g_tracked_frame51.read(frame)&&!frame.valid,"activity/dialogue/reading/native reaction retains its explicit camera owner");}
    std::printf("tracked_frame51_pose=%d/%d; actual coherent pose; native leaves=IN_MEMORY; headset=NOT_EXECUTED\n",tests-failures,tests);
    return failures?1:0;
}
