// H15 executes the production coherent-pose caller. Native memory leaves are
// the existing explicit in-memory fixture; arm/torso/continuity math is real.
#define main retained_pose_adapter_main_h15
#include "pose_adapter_test.cpp"
#undef main

static void TargetH15(int side,pc::Vec3 target,TrackingSnapshot& t) {
    const auto offset=target-PV(FromT(fixture.reference[rig.head]));
    (side?t.left:t.right)={float(offset.x),float(offset.z),float(-offset.y)};
}
static pc::Vec3 DownH15(int side) {
    return *pc::unit(pc::Vec3{side?-.35:.35,-.25,-1});
}
int main() {
    tests=failures=0;
    bool allCommitted=true,allLengths=true,allWrists=true,allOrientations=true,allRestored=true;
    double maximumElbowDelta=0,maximumWristDelta=0,maximumColdElbowDelta=0,maximumCancellationDelta=0,oldMinimumElbowDelta=1;
    double maximumHistoryPlaneStep=0;
    pc::Vec3 mirroredCold[2]{};
    for(int side=0;side<2;++side) {
        Init();auto t=Tracking();
        const int u=side?rig.upper_l:rig.upper_r,f=side?rig.fore_l:rig.fore_r,h=side?rig.hand_l:rig.hand_r;
        const auto shoulder=PV(FromT(fixture.reference[u])),down=DownH15(side);
        const auto tangent=*pc::unit(pc::perpendicular(pc::Vec3{side?-1.:1.,0,0},down));
        const auto a0=*pc::unit(down-tangent*.001),a1=*pc::unit(down+tangent*.001);
        const auto p0=shoulder+a0*.30,p1=shoulder+a1*.30;
        pc::Vec3 elbow[2]{},wrist[2]{};
        pc::ElbowContinuity old{};pc::Vec3 oldElbow[2]{};
        for(int stage=0;stage<2;++stage) {
            const auto target=stage?p1:p0,axis=stage?a1:a0;
            TargetH15(side,target,t);
            // The unchanged pre-H15 argument chain is the counterexample.
            const auto oldPreference=pc::torso_elbow_preference(shoulder,target,.25,.25,down,{0,0,.8},{0,0,1.36},.1152);
            for(int n=0;n<100;++n) {
                ++t.sequence;t.tick+=11;
                const bool ok=Run(t,2);allCommitted=allCommitted&&ok;
                const auto oldBend=pc::continuous_elbow(old,axis,oldPreference,down,.6,1,t.sequence,t.tick);
                const auto oldArm=oldBend?pc::solve_arm(shoulder,target,.25,.25,*oldBend,*oldBend):std::optional<pc::ArmSolution>{};
                if(oldArm&&n==99)oldElbow[stage]=oldArm->elbow;
                if(!ok)continue;
                const auto& s=g_pose6689;
                const auto e=PV(FromT(s.work[f])),w=PV(FromT(s.work[h])),sh=PV(FromT(s.work[u]));
                allLengths=allLengths&&std::abs(pc::length(e-sh)-s.upper[side])<1e-6&&std::abs(pc::length(w-e)-s.fore[side])<1e-6;
                allWrists=allWrists&&pc::length(w-target)<1e-6;
                allOrientations=allOrientations&&pc::angle(PQ(QFromQuatT(s.work[h])),PQ(DesiredHand6689(s,side,Q4{},side?t.lq:t.rq)))<1e-5;
                allRestored=allRestored&&Same(fixture.live,fixture.original);
                if(n==99){elbow[stage]=e;wrist[stage]=w;}
            }
        }
        maximumElbowDelta=std::max(maximumElbowDelta,pc::length(elbow[1]-elbow[0]));
        maximumWristDelta=std::max(maximumWristDelta,pc::length(wrist[1]-wrist[0]));
        oldMinimumElbowDelta=std::min(oldMinimumElbowDelta,pc::length(oldElbow[1]-oldElbow[0]));
        pc::Vec3 coldElbow[2]{};bool coldCommitted=true;
        for(int stage=0;stage<2;++stage){
            Init();t=Tracking();TargetH15(side,stage?p1:p0,t);
            const bool ok=Run(t,2);coldCommitted=coldCommitted&&ok;
            if(ok)coldElbow[stage]=PV(FromT(g_pose6689.work[f]));
        }
        maximumColdElbowDelta=std::max(maximumColdElbowDelta,pc::length(coldElbow[1]-coldElbow[0]));
        Check(coldCommitted&&pc::length(coldElbow[1]-coldElbow[0])<.001,
            "independent cold starts across 0.6 mm choose the same anatomical elbow branch");
        Init();t=Tracking();TargetH15(side,shoulder+down*.30,t);
        Check(Run(t,2),"cold start on body-pole axis commits on both mirrored arms");
        mirroredCold[side]=g_pose6689.elbow[side].bend;
        const auto saved=g_pose6689.elbow[side];const auto savedElbow=FromT(g_pose6689.work[f]);
        Check(Run(t,2)&&g_pose6689.elbow[side].updates==saved.updates&&Len(Sub(FromT(g_pose6689.work[f]),savedElbow))<1e-6f,
            "duplicate actual pose consumption cannot advance near-singular elbow history");
        ++g_pose_epoch6689;++t.sequence;t.tick+=11;
        Check(Run(t,2)&&g_pose6689.elbow[side].epoch==g_pose_epoch6689&&pc::length(g_pose6689.elbow[side].bend-mirroredCold[side])<1e-5,
            "new origin uses calibrated chest fallback without carrying old elbow branch");
        const auto current=g_pose6689.elbow[side];--t.sequence;t.tick-=11;
        Check(!Run(t,2)&&g_pose6689.elbow[side].sample==current.sample&&g_pose6689.elbow[side].updates==current.updates&&Same(fixture.live,fixture.original),
            "out-of-order actual pose fails closed without rewinding geometric history");
    }
    Check(allCommitted,"both actual arm callers accept 200 samples crossing the body-pole axis");
    Check(oldMinimumElbowDelta>.35,"fixture exposes pre-fix elbow reversal above 35 cm for a 0.6 mm wrist move");
    Check(maximumWristDelta<.00061&&maximumElbowDelta<.001,"0.6 mm actual wrist crossing stays below 1 mm elbow movement after settling");
    Check(allLengths&&allWrists&&allOrientations,"conditioning preserves both fixed lengths and exact controller wrist position and orientation");
    Check(allRestored,"every successful corrected pose restores the byte-identical native source buffer");
    Check(std::abs(mirroredCold[0].x+mirroredCold[1].x)<1e-5&&std::abs(mirroredCold[0].y-mirroredCold[1].y)<1e-5&&std::abs(mirroredCold[0].z-mirroredCold[1].z)<1e-5,
        "calibrated backward fallback has exact bilateral parity");

    bool cancellationAccepted=true,historyAccepted=true,historyWrists=true,historyDuplicates=true;
    for(int side=0;side<2;++side) {
        const auto down=DownH15(side),backTangent=*pc::unit(pc::perpendicular(pc::Vec3{0,-1,0},down));
        const pc::Vec3 shoulder{side?-.18:.18,0,double(fixture.reference[rig.upper_r].t[2])};
        pc::Vec3 coldElbow[2]{};
        // At conditioning .205 the former cold linear blend had exactly equal
        // opposite unit inputs. This sweep executes the production caller.
        for(int stage=0;stage<2;++stage){
            Init();auto t=Tracking();const double a=std::asin(.205)+(stage?.00001:-.00001);
            const auto axis=*pc::unit(down*std::cos(a)+backTangent*std::sin(a));
            TargetH15(side,shoulder+axis*.30,t);const bool ok=Run(t,2);
            cancellationAccepted=cancellationAccepted&&ok;
            if(ok)coldElbow[stage]=PV(FromT(g_pose6689.work[side?rig.fore_l:rig.fore_r]));
        }
        maximumCancellationDelta=std::max(maximumCancellationDelta,pc::length(coldElbow[1]-coldElbow[0]));
        Init();auto t=Tracking();pc::Vec3 previousNormal{};
        for(int n=0;n<900;++n){
            // Move out through the .33 cone edge, reverse, then settle just
            // beyond the former antipodal cancellation point.
            const double a=n<350?.15+.001*n:n<700?.5-.001*(n-350):std::asin(.205)+.00001;
            const auto axis=*pc::unit(down*std::cos(a)+backTangent*std::sin(a)),target=shoulder+axis*.30;
            TargetH15(side,target,t);++t.sequence;t.tick+=11;const bool ok=Run(t,2);
            historyAccepted=historyAccepted&&ok;if(!ok)continue;
            const auto& state=g_pose6689.elbow[side];const auto normal=*pc::unit(pc::cross(state.axis,state.bend));
            if(n)maximumHistoryPlaneStep=std::max(maximumHistoryPlaneStep,std::acos(std::clamp(pc::dot(previousNormal,normal),-1.,1.)));
            previousNormal=normal;const int h=side?rig.hand_l:rig.hand_r;
            historyWrists=historyWrists&&pc::length(PV(FromT(g_pose6689.work[h]))-target)<1e-6;
            const auto saved=state;const auto savedElbow=FromT(g_pose6689.work[side?rig.fore_l:rig.fore_r]);
            historyDuplicates=historyDuplicates&&Run(t,2)&&g_pose6689.elbow[side].updates==saved.updates&&
                Len(Sub(FromT(g_pose6689.work[side?rig.fore_l:rig.fore_r]),savedElbow))<1e-6f;
        }
    }
    Check(cancellationAccepted&&maximumCancellationDelta<.001,"both cold arm callers avoid opposite-vector cancellation at conditioning midpoint");
    Check(historyAccepted&&historyWrists,"valid history crosses cone edges and settles with exact wrist targets");
    Check(maximumHistoryPlaneStep<.0451,"cone-edge and antipodal settling changes remain within existing 4 rad/s plus reach transport bound");
    Check(historyDuplicates,"duplicate consumption throughout cone-edge crossing and settling never advances history");

    bool ordinaryUnchanged=true,bodyFrameIndependent=true,invalidRejected=true,staleUsesCold=true;
    for(int side=0;side<2;++side) {
        const auto down=DownH15(side);
        for(pc::Vec3 axis:{pc::Vec3{0,1,0},pc::Vec3{0,0,1},pc::Vec3{side?-1.:1.,0,0},pc::Vec3{.5,.6,-.1}}) {
            axis=*pc::unit(axis);const auto preference=*pc::unit(pc::perpendicular(down,axis));
            pc::ElbowContinuity a{},b{};
            const auto input=pc::condition_elbow_preference(b,axis,preference,down,{0,-1,0},1,1,1000);
            const auto before=pc::continuous_elbow(a,axis,preference,down,.6,1,1,1000);
            const auto after=input?pc::continuous_elbow(b,axis,input->anatomical,input->fallback,.6,1,1,1000):std::optional<pc::Vec3>{};
            ordinaryUnchanged=ordinaryUnchanged&&before&&after&&pc::length(*before-*after)<1e-12;
        }
        pc::ElbowContinuity state{};const auto q=pc::from_rotation_vector({.3,.5,.8});
        const auto input=pc::condition_elbow_preference(state,down,down,down,{0,-1,0},1,1,1000);
        const auto rotated=pc::condition_elbow_preference(state,pc::rotate(q,down),pc::rotate(q,down),pc::rotate(q,down),pc::rotate(q,{0,-1,0}),1,1,1000);
        bodyFrameIndependent=bodyFrameIndependent&&input&&rotated&&pc::length(pc::rotate(q,input->fallback)-rotated->fallback)<1e-12;
        state.ready=true;state.epoch=1;state.sample=2;state.tick=1000;state.axis=down;state.bend=*pc::unit(pc::perpendicular(pc::Vec3{side?-1.:1.,0,0},down));
        invalidRejected=invalidRejected&&!pc::condition_elbow_preference(state,down,down,down,{0,-1,0},1,1,1011)&&
            !pc::condition_elbow_preference(state,down,down,down,{0,-1,0},1,3,999)&&
            !pc::condition_elbow_preference(state,{},down,down,{0,-1,0},1,3,1011);
        const auto stale=pc::condition_elbow_preference(state,down,down,down,{0,-1,0},1,3,1501);
        staleUsesCold=staleUsesCold&&stale&&pc::length(stale->fallback-pc::Vec3{0,-1,0})<1e-12;
    }
    Check(ordinaryUnchanged,"ordinary forward lateral and overhead pole destinations outside the cone stay identical");
    Check(bodyFrameIndependent,"fallback rotates with calibrated body axes instead of a world-fixed axis");
    Check(invalidRejected,"invalid or out-of-order conditioning inputs fail closed");
    Check(staleUsesCold,"500 ms retention remains geometric memory only and expired history uses cold anatomy");

    // A prepared bow profile must bypass the change on BOTH arm loop sides,
    // including the non-string arm and bow contexts without an archer blend.
    bool bowUnchanged=true;
    for(int side=0;side<2;++side) {
        Init();auto t=Tracking();g_weapon_control_mode=4;mockPhysical=true;
        mockProfiles[1].key=7;mockProfiles[1].generation=3;mockProfiles[1].side=1;mockProfiles[1].classId=9;
        const int u=side?rig.upper_l:rig.upper_r,f=side?rig.fore_l:rig.fore_r;
        const auto shoulder=PV(FromT(fixture.reference[u])),down=DownH15(side);
        const auto tangent=*pc::unit(pc::perpendicular(pc::Vec3{side?-1.:1.,0,0},down));
        const auto axis=*pc::unit(down+tangent*.001),target=shoulder+axis*.30;
        TargetH15(side,target,t);
        const auto preference=pc::torso_elbow_preference(shoulder,target,.25,.25,down,{0,0,.8},{0,0,1.36},.1152);
        pc::ElbowContinuity old{};const auto bend=pc::continuous_elbow(old,axis,preference,down,.6,1,t.sequence,t.tick);
        const auto expected=bend?pc::solve_arm(shoulder,target,.25,.25,*bend,*bend):std::optional<pc::ArmSolution>{};
        bowUnchanged=bowUnchanged&&Run(t,2)&&expected&&pc::length(PV(FromT(g_pose6689.work[f]))-expected->elbow)<1e-5;
    }
    Check(bowUnchanged,"both actual prepared-bow arm callers retain their original pole arguments");
    std::printf("H15 raw-pole conditioning wristDeltaM=%.9f elbowDeltaM=%.9f coldElbowDeltaM=%.9f cancellationElbowDeltaM=%.9f historyPlaneStepRad=%.9f oldElbowDeltaMinM=%.9f checks=%d/%d\n",
        maximumWristDelta,maximumElbowDelta,maximumColdElbowDelta,maximumCancellationDelta,maximumHistoryPlaneStep,oldMinimumElbowDelta,tests-failures,tests);
    return failures!=0;
}
