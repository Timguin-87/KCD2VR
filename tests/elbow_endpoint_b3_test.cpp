// B3: execute the production coherent-pose handover, not a duplicate elbow solver.
// Native getter/setter leaves remain the established explicit in-memory fixture.
#define main historical_pose_adapter_b3
#include "pose_adapter_test.cpp"
#undef main


// Search only for a geometric input crossing the original60degree decision.
// The assertions below execute the complete production caller on that input.
static void ContinuousTargetB3() {
    for(int side=0;side<2;++side){
        Init();auto t=Tracking();const int u=side?rig.upper_l:rig.upper_r,f=side?rig.fore_l:rig.fore_r,h=side?rig.hand_l:rig.hand_r;
        const auto shoulder=PV(FromT(fixture.reference[u])),raw=*pc::unit(pc::Vec3{side?-.35:.35,-.25,-1});
        const pc::Vec3 back{0,-1,0},outward{side?-1.:1.,0,0};
        const auto hand=[&](double c){return shoulder+*pc::unit(raw+back*c+outward*.02)*.30;};
        const auto value=[&](double c){const auto target=hand(c),axis=*pc::unit(target-shoulder);
            const auto preferred=pc::torso_elbow_preference(shoulder,target,.25,.25,raw,{0,0,.8},{0,0,1.36},.1152);
            const auto a=pc::unit(pc::perpendicular(raw+back*.6,axis)),b=pc::unit(pc::perpendicular(preferred,axis));
            return a&&b?pc::dot(*a,*b)-.5:0.;};
        double lo=0,hi=0;bool found=false;double previous=.05,pv=value(previous);
        for(int k=1;k<=1000;++k){const double next=.05+double(k)*.00085,v=value(next);
            if(v*pv<0){lo=previous;hi=next;found=true;break;}previous=next;pv=v;}
        Check(found,"fixture locates torso/default anatomical boundary on both sides");if(!found)continue;
        for(int k=0;k<50;++k){const double mid=(lo+hi)*.5;if(value(lo)*value(mid)<=0)hi=mid;else lo=mid;}
        const double root=(lo+hi)*.5;
        Check(std::abs(value(root))<1e-5,"boundary is continuous alignment=.5, not a discrete torso candidate jump");
        pc::Vec3 wrist[2]{},elbow[2]{};bool okAll=true,lengths=true,orientations=true,restored=true;
        for(int phase=0;phase<2;++phase){const auto target=hand(root+(phase?1e-5:-1e-5));
            const auto offset=target-PV(FromT(fixture.reference[rig.head]));(side?t.left:t.right)={float(offset.x),float(offset.z),float(-offset.y)};
            for(int k=0;k<600;++k){++t.sequence;t.tick+=11;t.timeNs=std::uint64_t(t.tick)*1000000;const bool ok=Run(t,2);okAll=okAll&&ok;if(!ok)continue;
                const auto& s=g_pose6689;const auto sh=PV(FromT(s.work[u])),e=PV(FromT(s.work[f])),w=PV(FromT(s.work[h]));
                lengths=lengths&&std::abs(pc::length(e-sh)-s.upper[side])<1e-6&&std::abs(pc::length(w-e)-s.fore[side])<1e-6;
                orientations=orientations&&pc::angle(PQ(QFromQuatT(s.work[h])),PQ(DesiredHand6689(s,side,Q4{},side?t.lq:t.rq)))<1e-5;
                restored=restored&&Same(fixture.live,fixture.original);if(k==599){wrist[phase]=w;elbow[phase]=e;}
            }
        }
        const double wd=pc::length(wrist[1]-wrist[0]),ed=pc::length(elbow[1]-elbow[0]);
        std::printf("B3 continuous target side=%d boundary=%.12f wristDeltaM=%.12f elbowDeltaM=%.12f\n",side,root,wd,ed);
        Check(okAll&&lengths&&orientations&&restored,"continuous target retains native restore, hard bones and exact wrist orientation");
        Check(wd<.00002&&ed<.001,"tiny wrist boundary crossing cannot move settled actual elbow by tens of centimeters");
    }
}

static void ContinuousPathsB3(){
    for(int side=0;side<2;++side){
        Init();auto t=Tracking();const int u=side?rig.upper_l:rig.upper_r,f=side?rig.fore_l:rig.fore_r,h=side?rig.hand_l:rig.hand_r;
        const auto shoulder=PV(FromT(fixture.reference[u]));bool accepted=true,lengths=true,endpoints=true,orientations=true,headSame=true,restored=true;
        QuatT28 firstHead{};bool haveHead=false;pc::Vec3 startElbow{},endElbow{};
        const auto frame=[&](pc::Vec3 direction){const auto target=shoulder+*pc::unit(direction)*.30;
            const auto offset=target-PV(FromT(fixture.reference[rig.head]));(side?t.left:t.right)={float(offset.x),float(offset.z),float(-offset.y)};
            ++t.sequence;t.tick+=11;t.timeNs=std::uint64_t(t.tick)*1000000;const bool ok=Run(t,2);accepted=accepted&&ok;if(!ok)return;
            const auto& state=g_pose6689;const auto sh=PV(FromT(state.work[u])),e=PV(FromT(state.work[f])),w=PV(FromT(state.work[h]));
            lengths=lengths&&std::abs(pc::length(e-sh)-state.upper[side])<1e-6&&std::abs(pc::length(w-e)-state.fore[side])<1e-6;
            endpoints=endpoints&&pc::length(w-target)<1e-5;orientations=orientations&&pc::angle(PQ(QFromQuatT(state.work[h])),PQ(DesiredHand6689(state,side,Q4{},side?t.lq:t.rq)))<1e-5;
            if(haveHead)headSame=headSame&&std::memcmp(&firstHead,&state.work[rig.head],sizeof firstHead)==0;else{firstHead=state.work[rig.head];haveHead=true;}
            restored=restored&&Same(fixture.live,fixture.original);
        };
        // Normal front/lateral, body crossing, overhead, low/back and behind.
        for(auto axis:{pc::Vec3{.2,1,-.2},pc::Vec3{1,.1,-.1},pc::Vec3{-1,.7,-.2},pc::Vec3{.1,.1,1},pc::Vec3{.1,-.6,-1},pc::Vec3{.2,-1,.2}}){
            axis.x*=side?-1.:1.;for(int k=0;k<80;++k)frame(axis);}
        const auto home=pc::Vec3{side?-.25:.25,.8,-.35};for(int k=0;k<500;++k)frame(home);startElbow=PV(FromT(g_pose6689.work[f]));
        for(int k=0;k<=720;++k){const double a=double(k)*2*pc::armPi/720;auto axis=home;axis.x+=.3*std::sin(a);axis.z+=.15*(1-std::cos(a));frame(axis);}
        for(int k=0;k<500;++k)frame(home);endElbow=PV(FromT(g_pose6689.work[f]));
        Check(accepted&&lengths&&endpoints&&orientations&&restored,"front/body-cross/overhead/low/back paths preserve actual wrist and native restore");
        Check(headSame,"elbow target blend leaves the actual head-comfort pose byte-identical through arm paths");
        Check(pc::length(startElbow-endElbow)<1e-5,"closed arm loop returns to stateless anatomical destination after settling");
    }
}
// Both +/-20deg alternatives are collision-clear in this low/back pose.
// The old dot-derived cost could pick the opposite branch on floating-point
// roundoff alone. Exercise the actual coherent caller, including float goals.
void EqualCostTorsoBranchB3() {
    for(int side=0;side<2;++side) {
        Init();auto t=Tracking();
        const int u=side?rig.upper_l:rig.upper_r,f=side?rig.fore_l:rig.fore_r,h=side?rig.hand_l:rig.hand_r;
        const auto shoulder=PV(FromT(fixture.reference[u]));
        const auto base=*pc::unit(pc::Vec3{side?-.55797879:.55797879,-.26018978,-.78801075});
        const auto tangent=*pc::unit(pc::cross(base,pc::Vec3{0,0,1}));
        const auto bitangent=pc::cross(base,tangent);
        pc::Vec3 wrist[2]{},elbow[2]{},previous{},loopEnd{};
        bool accepted=true,lengths=true,endpoints=true,restored=true,headSame=true,havePrevious=false;
        double maximumStep=0;
        const auto frame=[&](pc::Vec3 axis){
            const auto target=shoulder+*pc::unit(axis)*.30;
            const auto offset=target-PV(FromT(fixture.reference[rig.head]));
            (side?t.left:t.right)={float(offset.x),float(offset.z),float(-offset.y)};
            ++t.sequence;t.tick+=11;t.timeNs=std::uint64_t(t.tick)*1000000;
            const bool ok=Run(t,2);accepted=accepted&&ok;if(!ok)return;
            const auto& s=g_pose6689;const auto e=PV(FromT(s.work[f])),w=PV(FromT(s.work[h])),sh=PV(FromT(s.work[u]));
            lengths=lengths&&std::abs(pc::length(e-sh)-s.upper[side])<1e-6&&std::abs(pc::length(w-e)-s.fore[side])<1e-6;
            endpoints=endpoints&&pc::length(w-target)<1e-6;
            restored=restored&&Same(fixture.live,fixture.original);
            headSame=headSame&&std::memcmp(&s.work[rig.head],&fixture.original[rig.head],sizeof(s.work[rig.head]))==0;
            if(havePrevious)maximumStep=std::max(maximumStep,pc::length(e-previous));
            previous=e;havePrevious=true;
        };
        for(int stage=0;stage<2;++stage) {
            for(int k=0;k<600;++k)frame(base+tangent*(stage?1e-5:-1e-5));
            wrist[stage]=PV(FromT(g_pose6689.work[h]));elbow[stage]=PV(FromT(g_pose6689.work[f]));
        }
        for(int k=0;k<=320;++k) {
            const double a=2*pc::armPi*k/320;
            frame(base+tangent*(std::cos(a)*1e-5)+bitangent*(std::sin(a)*1e-5));
        }
        for(int k=0;k<600;++k)frame(base+tangent*(-1e-5));
        loopEnd=PV(FromT(g_pose6689.work[f]));
        std::printf("B3 equalCost side=%d wristDeltaM=%.12f elbowDeltaM=%.12f maximumStepM=%.12f loopReturnM=%.12f\n",
            side,pc::length(wrist[1]-wrist[0]),pc::length(elbow[1]-elbow[0]),maximumStep,pc::length(loopEnd-elbow[0]));
        Check(accepted,"equal-cost torso pair and micro-loop commit every actual pose");
        Check(lengths&&endpoints&&headSame,"equal-cost torso branch retains fixed lengths, controller wrist and native head");
        Check(restored,"equal-cost torso branch restores the native source buffer every frame");
        Check(pc::length(wrist[1]-wrist[0])<.000007&&pc::length(elbow[1]-elbow[0])<.001,
            "6micrometer low-back wrist pair cannot select a distant equal-cost elbow branch");
        Check(maximumStep<.001&&pc::length(loopEnd-elbow[0])<.001,
            "micro-loop through equal-cost torso candidates stays local and returns to its destination");
    }
}

int main() {
    tests=failures=0;
    bool allLengths=true,allWrists=true,allPlanes=true,fixtureExposesBug=true;
    double maximumError=0,oldMinimumError=100;
    for(int side=0;side<2;++side) {
        Init();g_native_handover178r14=true;
        auto t=Tracking();const float sign=side?-1.f:1.f;
        const int u=side?rig.upper_l:rig.upper_r;
        const int f=side?rig.fore_l:rig.fore_r;
        const int h=side?rig.hand_l:rig.hand_r;
        const V3 shoulder=FromT(fixture.reference[u]);
        // The tracked hand is forward/outward. The native action holds its wrist
        // near the chest on the opposite side: a normal passenger handover case.
        const V3 tracked{sign*.37546827f,.30388961f,shoulder.z+.02236548f};
        const V3 native{sign*-.03235641f,.03025198f,shoulder.z+.06056027f};
        const V3 fromHead=Sub(tracked,FromT(fixture.reference[rig.head]));
        (side?t.left:t.right)={fromHead.x,fromHead.z,-fromHead.y};
        fixture.original[h].t[0]=native.x;fixture.original[h].t[1]=native.y;fixture.original[h].t[2]=native.z;
        fixture.live=fixture.original;
        NotifyNativeHandUse(side,4000);g_pose6689.handoverW178r14[side]=1.f;
        Check(Run(t,2),"actual native-passenger wrist handover commits on both sides");
        const auto& s=g_pose6689;
        const V3 finalShoulder=FromT(s.work[u]);
        const V3 down=QRotateV(QFromQuatT(s.work[rig.spine3]),s.sideDownLocal[side]);
        const double radius=std::clamp(double(Len(Sub(FromT(s.work[rig.upper_r]),FromT(s.work[rig.upper_l]))))*.32,.08,.16);
        const auto preference=pc::torso_elbow_preference(PV(finalShoulder),PV(native),s.upper[side],s.fore[side],
            PV(down),PV(FromT(s.work[rig.pelvis])),PV(FromT(s.work[rig.spine3])),radius);
        const auto expected=pc::solve_arm(PV(finalShoulder),PV(native),s.upper[side],s.fore[side],preference,preference);
        const auto oldPreference=pc::torso_elbow_preference(PV(finalShoulder),PV(tracked),s.upper[side],s.fore[side],
            PV(down),PV(FromT(s.work[rig.pelvis])),PV(FromT(s.work[rig.spine3])),radius);
        const auto oldArm=pc::solve_arm(PV(finalShoulder),PV(native),s.upper[side],s.fore[side],oldPreference,oldPreference);
        const double actualError=expected?pc::length(PV(FromT(s.work[f]))-expected->elbow):100;
        const double oldError=expected&&oldArm?pc::length(oldArm->elbow-expected->elbow):0;
        maximumError=std::max(maximumError,actualError);oldMinimumError=std::min(oldMinimumError,oldError);
        allPlanes=allPlanes&&expected&&actualError<.0001;
        fixtureExposesBug=fixtureExposesBug&&oldError>.05;
        allWrists=allWrists&&Len(Sub(FromT(s.work[h]),native))<.0001f;
        allLengths=allLengths&&std::fabs(Len(Sub(FromT(s.work[f]),finalShoulder))-s.upper[side])<.0001&&
            std::fabs(Len(Sub(FromT(s.work[h]),FromT(s.work[f])))-s.fore[side])<.0001;
        Check(Same(fixture.live,fixture.original),"native source pose is restored after each passenger solve");
    }
    std::printf("B3 passenger elbow finalGoalErrorM=%.9f oldGoalCounterexampleMinM=%.9f\n",maximumError,oldMinimumError);
    Check(fixtureExposesBug,"both mirrored fixtures distinguish old-endpoint pole by more than 5 cm");
    Check(allPlanes,"passenger elbow plane is selected using final native-handover endpoint");
    Check(allWrists&&allLengths,"passenger correction preserves exact wrist endpoints and both fixed arm lengths");
    ContinuousTargetB3();
    ContinuousPathsB3();
    EqualCostTorsoBranchB3();
    std::printf("B3 elbow endpoint ordering=%d/%d\n",tests-failures,tests);
    return failures!=0;
}
