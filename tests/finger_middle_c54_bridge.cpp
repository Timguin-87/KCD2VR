// Executes real frik finger path; retail leaves are explicit existing bridge doubles.
#include "../src/frik_pose_probe.cpp"
#include <cassert>
#include <cstdio>
namespace kcd2vr::diagnostic_trace {void Write(const char*) noexcept {}}
using namespace kcd2vr::frik_pose_probe;
#include <cstring>
#include <limits>
namespace finger40=kcd2vr::finger40;
static void ReferenceTuneFingerBase59(const CoherentState6689& s,int side,int finger,int joint,int id,pc::Quat& open,pc::Quat& closed) noexcept {
    namespace ft=kcd2vr::finger40;const float a=ft::get(side,finger,0),b=ft::get(side,finger,1),spread=ft::get(side,finger,2),proximal=ft::get(side,finger,3);
    if(a==0&&b==1&&spread==0&&proximal==0)return; // preserve default finger pose and contact cost
    pc::Vec3 axis{};
    const int hand=side?s.rig.hand_l:s.rig.hand_r,parent=s.parent[id];
    if(spread!=0&&joint==0&&parent>=0&&s.anatomyHand[side]){
      const auto model=pc::multiply(PQ(QFromQuatT(s.rest[hand])),PQ(s.boneToAnatomy[side]));
      axis=pc::rotate(pc::conjugate(PQ(QFromQuatT(s.rest[parent]))),pc::rotate(model,{0,0,1}));
    }
    const auto e=ft::tune(open,closed,a,b,spread,axis,joint==0,proximal);
    open=e.open;closed=e.closed;
}
// Current authored closed chain in hand space. No live attack pose or cross-hand
// quaternion mirroring: offsets stay anatomical and only local rotations tune.
static bool ReferenceClosedFingerJoint59(const CoherentState6689& s,int side,int finger,int joint,
                                QuatT28& before,QuatT28& result) noexcept {
    if(side<0||side>1||finger<0||finger>4||!s.fingerMap[side]||!s.open[side].ready||!s.closed[side].ready)return false;
    const auto& chain=s.fingers[side][finger];if(joint<0||joint>=chain.count)return false;
    int offset=0;for(int f=0;f<finger;++f)offset+=s.fingers[side][f].count;
    const int hand=side?s.rig.hand_l:s.rig.hand_r,base=chain.id[0];
    if(base<0||base>=int(s.count)||s.parent[base]<0||s.parent[base]>=int(s.count)||hand<0||hand>=int(s.count))return false;
    auto parent=RelativeFromAbsolute(s.rest[hand],s.rest[s.parent[base]]);
    for(int k=0;k<=joint;++k){const int id=chain.id[k];if(id<0||id>=int(s.count)||offset+k>=s.fingerCount[side])return false;
        auto open=PQ(QFromQuatT(s.open[side].local[offset+k])),closed=PQ(QFromQuatT(s.closed[side].local[offset+k]));
        ReferenceTuneFingerBase59(s,side,finger,k,id,open,closed);auto local=s.restLocal[id];QToQuatT(local,QP(closed));
        before=parent;result=AbsoluteFromRelative(parent,local);if(!ValidPose6689(result))return false;parent=result;
    }
    return true;
}
static void ReferenceTuneFinger40(const CoherentState6689& s,int side,int finger,int joint,int id,pc::Quat& open,pc::Quat& closed) noexcept {
    ReferenceTuneFingerBase59(s,side,finger,joint,id,open,closed);
    // C59 headset: middle-finger PIP toward ring by a few degrees, CLOSED only.
    // Derive its axis from this hand's authored closed chain and neighbor. That
    // keeps the two actual hands independent and leaves the accepted open pose.
    if(finger!=2||joint!=1||s.fingers[side][finger].count<3)return;
    QuatT28 parent{},middle{},ringParent{},ring{};
    if(!ReferenceClosedFingerJoint59(s,side,2,1,parent,middle)||!ReferenceClosedFingerJoint59(s,side,3,1,ringParent,ring))return;
    const int child=s.fingers[side][2].id[2];if(child<0||child>=int(s.count))return;
    const auto tuned=kcd2vr::finger40::towardRing59(open,closed,PQ(QFromQuatT(parent)),PQ(QFromQuatT(middle)),
        PV(FromT(s.restLocal[child])),PV(FromT(middle)),PV(FromT(ring)),kcd2vr::finger40::middleTowardRing59.load());
    closed=tuned.closed;
}

static bool exactFinger(pc::Quat a,pc::Quat b){return std::memcmp(&a,&b,sizeof(a))==0;}
int main(){
 for(int h=0;h<2;++h){finger40::setShiftMmH31(h,0);finger40::setDistalMedialDegH31(h,0);} // explicit historical C54 contract
 unsigned checks=0;
 auto check=[&](bool v){++checks;if(!v)std::fprintf(stderr,"FAIL check %u\n",checks);assert(v);};
 static CoherentState6689 s{};s.count=40;s.rig.hand_r=0;s.rig.hand_l=1;
 for(unsigned i=0;i<s.count;++i){s.rest[i].q[3]=s.restLocal[i].q[3]=1;s.parent[i]=0;}
 for(int side=0;side<2;++side){s.anatomyHand[side]=s.fingerMap[side]=true;s.open[side].ready=s.closed[side].ready=true;s.fingerCount[side]=15;
 s.boneToAnatomy[side]=QP(pc::from_rotation_vector({side?3.141592653589793:0.,0,0}));
 for(int finger=0;finger<5;++finger){auto& chain=s.fingers[side][finger];chain.count=3;
 for(int joint=0;joint<3;++joint){int id=2+side*15+finger*3+joint;chain.id[joint]=id;s.parent[id]=joint?id-1:side;
 s.restLocal[id].t[0]=.03f;s.rest[id].t[0]=.03f*(joint+1);s.rest[id].t[1]=.012f*finger;
 QToQuatT(s.open[side].local[finger*3+joint],QP(pc::from_rotation_vector({.05,0,0})));
 QToQuatT(s.closed[side].local[finger*3+joint],QP(pc::from_rotation_vector({.6,.02*finger,.01*joint})));
 }
 }}
 for(int side=0;side<2;++side){s.anatomyHand[side]=s.fingerMap[side]=true;s.open[side].ready=s.closed[side].ready=true;s.fingerCount[side]=15;
 s.boneToAnatomy[side]=QP(pc::from_rotation_vector({side?3.141592653589793:0.,0,0}));
 for(int finger=0;finger<5;++finger){auto& chain=s.fingers[side][finger];chain.count=3;
 for(int joint=0;joint<3;++joint){int id=2+side*15+finger*3+joint;chain.id[joint]=id;s.parent[id]=joint?id-1:side;
 s.restLocal[id].t[0]=.03f;s.rest[id].t[0]=.03f*(joint+1);s.rest[id].t[1]=.012f*finger;
 QToQuatT(s.open[side].local[finger*3+joint],QP(pc::from_rotation_vector({.05,0,0})));
 QToQuatT(s.closed[side].local[finger*3+joint],QP(pc::from_rotation_vector({.6,.02*finger,.01*joint})));
 }
 for(int parameter=0;parameter<4;++parameter)finger40::set(side,finger,parameter,finger40::defaults59[side][finger][parameter]);
 check(finger40::getMiddleLateralC54(side,finger)==0);
 for(int joint=0;joint<3;++joint){const int id=chain.id[joint];
 auto open=PQ(QFromQuatT(s.open[side].local[finger*3+joint])),closed=PQ(QFromQuatT(s.closed[side].local[finger*3+joint]));
 auto oldOpen=open,oldClosed=closed;ReferenceTuneFinger40(s,side,finger,joint,id,oldOpen,oldClosed);TuneFinger40(s,side,finger,joint,id,open,closed);
 check(exactFinger(open,oldOpen));check(exactFinger(closed,oldClosed));
 finger40::setMiddleLateralC54(side,finger,10);open=PQ(QFromQuatT(s.open[side].local[finger*3+joint]));closed=PQ(QFromQuatT(s.closed[side].local[finger*3+joint]));
 const auto translations=s.restLocal;TuneFinger40(s,side,finger,joint,id,open,closed);
 check(exactFinger(open,oldOpen));check(joint==1?pc::angle(closed,oldClosed)>.17:exactFinger(closed,oldClosed));
 check(std::memcmp(translations.data(),s.restLocal.data(),sizeof(s.restLocal))==0);
 finger40::setMiddleLateralC54(side,finger,0);
 }
 }}
 finger40::setMiddleLateralC54(0,3,99);check(finger40::getMiddleLateralC54(0,3)==20);check(finger40::getMiddleLateralC54(1,3)==0);
 finger40::setMiddleLateralC54(0,3,std::numeric_limits<float>::quiet_NaN());check(finger40::getMiddleLateralC54(0,3)==0);
 const pc::Quat notNormalized{.1,.2,.3,.8};check(exactFinger(finger40::middleLateralRotationC54(notNormalized,{0,0,1},0),notNormalized));
 check(exactFinger(finger40::middleLateralRotationC54(notNormalized,{0,0,0},10),notNormalized));
 std::printf("FINGER_CURRENT_MIDDLE_C54 %u checks PASS\n",checks);
}
