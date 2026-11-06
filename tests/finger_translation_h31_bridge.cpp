// Actual retail-adapter translation, quaternion and collision-curve owners.
// Native leaves are the existing complete-TU bridge doubles; no game is run.
#include "../src/frik_pose_probe.cpp"
#include <cassert>
#include <cstdio>
#include <cstring>
#include <limits>
namespace kcd2vr::diagnostic_trace {void Write(const char*) noexcept {}}
using namespace kcd2vr::frik_pose_probe;
namespace ft31=kcd2vr::finger40;
static unsigned checks31=0;
static void ck31(bool v,const char* what){++checks31;if(!v)std::fprintf(stderr,"FAIL %u %s\n",checks31,what);assert(v);}
static bool near31(pc::Vec3 a,pc::Vec3 b,double eps=2e-7){return pc::length(a-b)<eps;}
static QuatT28 joint31(const CoherentState6689& s,int side,int digit,int joint){
    auto p=s.rest[side?s.rig.hand_l:s.rig.hand_r];const auto& c=s.fingers[side][digit];
    for(int k=0;k<=joint;++k)p=AbsoluteFromRelative(p,s.local[c.id[k]]);return p;
}
static void init31(CoherentState6689& s){
    s.count=500;s.rig.hand_r=408;s.rig.hand_l=443;
    for(unsigned i=0;i<s.count;++i){s.parent[i]=-1;s.local[i].q[3]=s.restLocal[i].q[3]=s.rest[i].q[3]=s.original[i].q[3]=1;}
    const int roots[2][5]={{416,415,414,413,412},{451,450,449,448,447}};
    for(int side=0;side<2;++side){const int hand=side?s.rig.hand_l:s.rig.hand_r;const double mirror=side?-1.:1.;
        s.fingerMap[side]=s.anatomyHand[side]=true;s.open[side].ready=s.closed[side].ready=true;s.fingerCount[side]=20;
        s.boneToAnatomy[side]=QP(pc::from_rotation_vector({side?pc::armPi:0.,0,0}));
        QToQuatT(s.rest[hand],QP(pc::from_rotation_vector({.2*mirror,.3,.4*mirror})));ToT(s.rest[hand],{float(side*.4),.2f,.1f});
        s.original[hand]=s.local[hand]=s.rest[hand];
        for(int digit=0;digit<5;++digit){auto& chain=s.fingers[side][digit];chain.count=4;QuatT28 p=s.rest[hand];
            for(int k=0;k<4;++k){const int id=k?100+side*70+digit*4+k:roots[side][digit];chain.id[k]=id;s.parent[id]=k?chain.id[k-1]:hand;
                ToT(s.restLocal[id],k?V3{float(k==1?.045:k==2?.028:.020),0,0}:V3{.05f,float(mirror*(digit-2)*.014),float(digit*.001)});
                s.local[id]=s.restLocal[id];p=AbsoluteFromRelative(p,s.restLocal[id]);s.original[id]=s.rest[id]=p;
                QToQuatT(s.open[side].local[digit*4+k],QP(pc::from_rotation_vector({0,.02,0})));
                QToQuatT(s.closed[side].local[digit*4+k],QP(pc::from_rotation_vector({0,k?.65:.4,0})));
                s.fingerIds[side][digit*4+k]=id;
            }
            for(int parameter=0;parameter<4;++parameter)ft31::set(side,digit,parameter,ft31::defaults59[side][digit][parameter]);
            ft31::setMiddleLateralC54(side,digit,side&&digit==2?7.5f:0.f);
        }
    }
    std::array<bool,kPoseCapacity6689> added{};unsigned count=0;
    while(count<s.count){const auto before=count;for(unsigned id=0;id<s.count;++id)if(!added[id]&&(s.parent[id]<0||added[s.parent[id]])){s.order[count++]=int(id);added[id]=true;}assert(before!=count);}
}
int main(){
    static CoherentState6689 s{};init31(s);const auto anatomy=s.restLocal;
    ck31(ft31::getShiftMmH31(0)==15&&ft31::getShiftMmH31(1)==15,"requested15mm defaults both hands");
    ck31(ft31::getDistalMedialDegH31(0)==10&&ft31::getDistalMedialDegH31(1)==10,"requested10deg defaults both hands");
    for(int side=0;side<2;++side){
        ft31::setShiftMmH31(side,0);ft31::setDistalMedialDegH31(side,0);
        ck31(ApplyFingerLocals6689(s,side,1,1),"actual legacy writer");const auto legacy=s.local;
        const int root=s.fingers[side][2].id[0];const auto originalRoot=PV(FromT(s.local[root]));
        // Independent counterexample: the old PIP rotation (including savedL7.5)
        // moves the MCP/root by ZERO, rather than the requested15mm.
        ck31(near31(originalRoot,PV(FromT(s.restLocal[root])),1e-12),"baseline PIP-only has0mm root displacement");
        std::printf("BASELINE_COUNTEREXAMPLE side=%c rootShiftMm=0 requestedMm=15 legacyPipDeg=%g\n",side?'L':'R',double(ft31::getMiddleLateralC54(side,2)));
        ft31::setShiftMmH31(side,15);ft31::setDistalMedialDegH31(side,10);
        ck31(ApplyFingerLocals6689(s,side,1,1),"actual shifted closed writer");
        const auto frame=MiddleFingerFrameH31(s,side);const auto delta=PV(FromT(s.local[root]))-originalRoot;
        ck31(frame.valid&&std::abs(pc::length(delta)-.015)<2e-8,"actual MCP translates15mm");
        ck31(pc::dot(delta,frame.towardIndex)<-.01499,"MCP moves away fromindex mirrored per hand");
        for(int digit=0;digit<5;++digit)for(int k=0;k<4;++k){const int id=s.fingers[side][digit].id[k];
            if(digit!=2)ck31(std::memcmp(&legacy[id],&s.local[id],sizeof(QuatT28))==0,"other finger byte-identical");
            else if(k)ck31(near31(PV(FromT(s.local[id])),PV(FromT(s.restLocal[id])),1e-12),"all three phalanx offsets identical");
        }
        // The old failed middlePIP-only settings no longer stack onto H31.
        const auto corrected=s.local;const auto medialPip=joint31(s,side,2,1),medialDip=joint31(s,side,2,2),medialTip=joint31(s,side,2,3);
        ft31::setDistalMedialDegH31(side,0);ck31(ApplyFingerLocals6689(s,side,1,1),"translation-only comparison");
        const auto straightPip=joint31(s,side,2,1),straightDip=joint31(s,side,2,2),straightTip=joint31(s,side,2,3);
        for(int k=1;k<=2;++k){const int id=s.fingers[side][2].id[k];
            ck31(std::abs(pc::angle(PQ(QFromQuatT(corrected[id])),PQ(QFromQuatT(s.local[id])))-10*pc::armPi/180.)<2e-6,"PIP and DIP each rotate exactly10deg");}
        const auto indexward=pc::rotate(PQ(QFromQuatT(s.rest[side?s.rig.hand_l:s.rig.hand_r])),frame.towardIndex);
        ck31(pc::dot(PV(FromT(medialDip))-PV(FromT(medialPip)),indexward)>pc::dot(PV(FromT(straightDip))-PV(FromT(straightPip)),indexward),"PIP moves outgoing phalanx towardindex");
        ck31(pc::dot(PV(FromT(medialTip))-PV(FromT(medialDip)),indexward)>pc::dot(PV(FromT(straightTip))-PV(FromT(straightDip)),indexward),"DIP moves final phalanx towardindex");
        ft31::setDistalMedialDegH31(side,10);ft31::setMiddleLateralC54(side,2,-17);
        ck31(ApplyFingerLocals6689(s,side,1,1),"new owner ignores legacyPIP-only");
        for(int k=0;k<4;++k){const int id=s.fingers[side][2].id[k];ck31(std::memcmp(&corrected[id],&s.local[id],sizeof(QuatT28))==0,"middle ignores obsolete lateral rotation");}
        for(float curl:{0.f,.25f,.6f,1.f}){
            ck31(ApplyFingerLocals6689(s,side,curl,curl),"actual partial writer");
            s.deferPassengers137=true;s.essential137.fill(false);
            ck31(Compose6689(s)&&ComposeFull137(s),"actual deferred/full coherent pose composition");
            FingerContactGeometry103 geometry{};V3 points[6]{};const auto hand=s.rest[side?s.rig.hand_l:s.rig.hand_r];
            const auto wrist=QFromQuatT(hand);ck31(PalmGeometry103(s,side,wrist,curl,curl,0,points,geometry),"actual contact geometry");
            const auto renderedTip=PV(FromT(joint31(s,side,2,3)))-PV(FromT(hand));
            ck31(near31(PV(FromT(s.work[s.fingers[side][2].id[3]]))-PV(FromT(hand)),renderedTip),"actual composed skeleton equals independent FK");
            const auto fit=pc::finger_clearance103(geometry.curve[2],curl,PQ(wrist),{},{{0,0,-2},{0,0,1}},0);
            ck31(fit.valid&&near31(fit.tip,renderedTip),"surface curve tip equals actual writer FK");
            ck31(near31(PV(points[3]),renderedTip),"sampled middle fingertip equals writer FK");
            const auto shape=kcd2vr::hand_shape59c7f::make(geometry.curve,geometry.input,PQ(wrist));
            ck31(shape.valid,"actual handshape valid");
            for(unsigned k=0;k<3;++k){const auto a=PV(FromT(joint31(s,side,2,int(k))))-PV(FromT(hand));const auto b=PV(FromT(joint31(s,side,2,int(k+1))))-PV(FromT(hand));
                ck31(near31(shape.part[6+k].a,a)&&near31(shape.part[6+k].b,b),"hand collision phalanx equals actual writer");}
            const auto knuckle=kcd2vr::knuckle174::offset(geometry.curve,geometry.input,PQ(wrist));pc::Vec3 expected{};
            for(int f=1;f<5;++f)expected=expected+(PV(FromT(joint31(s,side,f,1)))-PV(FromT(hand)))*.25;
            ck31(knuckle&&near31(*knuckle,expected),"knuckle punch proxy equals actual writer");
            auto scaled=geometry.curve[2];for(unsigned k=0;k<scaled.count;++k)scaled.offset[k]=scaled.offset[k]*1.75;
            const auto scaledFit=pc::finger_clearance103(scaled,curl,PQ(wrist),{},{{0,0,-2},{0,0,1}},0);
            ck31(scaledFit.valid&&near31(scaledFit.tip,fit.tip*1.75),"root translation scales with model exactly once");
        }
        // Open endpoint stays exactly accepted, even while the legacy storedPIP
        // value differs: all closed-only corrections have zero open contribution.
        ft31::setShiftMmH31(side,0);ft31::setDistalMedialDegH31(side,0);ck31(ApplyFingerLocals6689(s,side,0,0),"legacy open writer");const auto open=s.local;
        ft31::setShiftMmH31(side,15);ft31::setDistalMedialDegH31(side,10);ck31(ApplyFingerLocals6689(s,side,0,0),"new open writer");
        for(int f=0;f<5;++f)for(int k=0;k<4;++k){const int id=s.fingers[side][f].id[k];ck31(std::memcmp(&open[id],&s.local[id],sizeof(QuatT28))==0,"all accepted open locals byte-identical");}
        ck31(ApplyFingerLocals6689(s,side,1,1),"reference hemisphere writer");const auto positiveTip=joint31(s,side,2,3);
        for(int i=0;i<s.fingerCount[side];++i)for(int c=0;c<4;++c)s.closed[side].local[i].q[c]*=-1;
        ck31(ApplyFingerLocals6689(s,side,1,1),"opposite hemisphere writer");const auto negativeTip=joint31(s,side,2,3);
        ck31(near31(PV(FromT(positiveTip)),PV(FromT(negativeTip)))&&pc::angle(PQ(QFromQuatT(positiveTip)),PQ(QFromQuatT(negativeTip)))<1e-6,"quaternion sign cannot flip geometry");
        // Fitted curl, not raw squeeze, must own both root position and rotations.
        HandContact6695 contact{};contact.fingerContact[2]=true;contact.curl[2]=.35;
        ck31(ApplyFingerLocals6689(s,side,1,1,&contact),"actual fitted contact writer");
        ck31(std::abs(pc::length(PV(FromT(s.local[root]))-originalRoot)-.015*.35)<2e-8,"contact fitting scales root translation too");
        // Positively held object owner remains native and does not use free-fist tuning.
        ck31(ApplyObjectFingerLocals6698(s,side,1234,42),"native object finger owner");
        for(int i=0;i<s.fingerCount[side];++i){const int id=s.fingerIds[side][i];ck31(near31(PV(FromT(s.local[id])),PV(FromT(s.restLocal[id])),1e-12),"held object has no new root translation");}
        FingerContactGeometry103 heldGeometry{};V3 heldPoints[6]{};
        ck31(PalmGeometry103(s,side,QFromQuatT(s.rest[side?s.rig.hand_l:s.rig.hand_r]),1,1,1234,heldPoints,heldGeometry),"native object contact geometry");
        ck31(pc::length(heldGeometry.curve[2].rootClosedShiftRatioH31)==0&&!heldGeometry.enabled[2],"native held curve has no new fist shift");
        const int actualParent=s.parent[root];s.parent[root]=0;
        ck31(ApplyFingerLocals6689(s,side,1,1)&&near31(PV(FromT(s.local[root])),originalRoot,1e-12),"unqualified root ancestry refuses translation");s.parent[root]=actualParent;
    }
    ck31(std::memcmp(anatomy.data(),s.restLocal.data(),sizeof(s.restLocal))==0,"immutable anatomical source remains unchanged");
    ft31::setShiftMmH31(0,999);ft31::setDistalMedialDegH31(1,-999);
    ck31(ft31::getShiftMmH31(0)==20&&ft31::getDistalMedialDegH31(1)==-20,"finite parameter bounds");
    ft31::setShiftMmH31(0,std::numeric_limits<float>::quiet_NaN());ft31::setDistalMedialDegH31(1,std::numeric_limits<float>::infinity());
    ck31(ft31::getShiftMmH31(0)==15&&ft31::getDistalMedialDegH31(1)==10,"invalid setting restores requested safe default");
    std::printf("FINGER_TRANSLATION_H31 %u production-connected checks PASS; native/headset NOT_EXECUTED\n",checks31);
    return 0;
}
