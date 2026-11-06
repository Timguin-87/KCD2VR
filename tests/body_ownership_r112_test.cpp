// Literal production chest-bias and completed stowed packet writer. The fixture
// provides copied anatomy/pose values; native memory and rendering are not run.
#include <array>
#include <atomic>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <limits>
#include "vrik/body_follow_r38.h"
#include "vrik/upper_body.h"
#include "vrik/stowed_render_pose_d63.h"
namespace pc=kcd2vr::pose_core;
#include "body_math_r112.inc"
static pc::Vec3 PV(V3 v){return {v.x,v.y,v.z};}
static V3 VP(pc::Vec3 v){return {float(v.x),float(v.y),float(v.z)};}
static Q4 QP(pc::Quat q){return {float(q.x),float(q.y),float(q.z),float(q.w)};}
constexpr std::size_t kPoseCapacity6689=32;
struct RigMap {int pelvis=0,spine=1,spine1=2,spine2=3,spine3=4,neck=5,head=6;};
struct CoherentState6689 {
    std::uint32_t count=32;
    std::array<int,10> stowedJointsD63{},stowedAnchorsR112{};
    std::array<QuatT28,10> stowedReferenceR112{};
    std::array<int,kPoseCapacity6689> parent{};
    std::array<bool,kPoseCapacity6689> trunk{},armR{},armL{};
    std::array<QuatT28,kPoseCapacity6689> rest{},work{},neutral{};
};
struct Transaction {bool stowedFullD63=true;std::array<bool,kPoseCapacity6689> selected{};};
struct StowedPoseD63 {QuatT28 model{};int joint=-1;bool valid=false;};
struct Packet {std::array<StowedPoseD63,10> stowedD63{};};
static constexpr int kStowedSlotsD63[10]{};
#include "body_stowed_r112.inc"
static void Initialize(CoherentState6689& s,const RigMap& rig){
#include "body_stowed_init_r112.inc"
}
static Packet Publish(const CoherentState6689& s,const Transaction& tx){Packet packet{};
#include "body_stowed_publish_r112.inc"
    return packet;
}
static int checks=0,failures=0;
static void ck(bool ok,const char* name){++checks;if(!ok)++failures;std::printf("%s %s\n",ok?"PASS":"FAIL",name);}
static QuatT28 pose(V3 t,Q4 q={}){QuatT28 result{};ToT(result,t);QToQuatT(result,q);return result;}
static bool near(const QuatT28& a,const QuatT28& b){
    const auto qa=QFromQuatT(a),qb=QFromQuatT(b);
    return Len(Sub(FromT(a),FromT(b)))<2e-5f&&std::abs(qa.x*qb.x+qa.y*qb.y+qa.z*qb.z+qa.w*qb.w)>1.f-2e-6f;
}
static CoherentState6689 anatomy(){
    CoherentState6689 s{};s.parent.fill(-1);s.stowedJointsD63.fill(-1);s.stowedAnchorsR112.fill(-1);
    for(unsigned i=0;i<s.count;++i)s.rest[i]=s.work[i]=s.neutral[i]=pose({});
    for(int i=0;i<=6;++i){s.parent[i]=i-1;s.trunk[i]=true;s.rest[i]=pose({0,0,.9f+.1f*i});}
    // A back slot behind the chest with a separate animated equipment helper.
    s.parent[7]=4;s.parent[8]=7;s.rest[7]=pose({0,-.1f,1.35f});s.rest[8]=pose({.08f,-.2f,1.48f});
    s.stowedJointsD63[0]=8;
    // Hip holster, direct chest socket, duplicate quiver, and unsupported arm/head.
    s.parent[9]=0;s.rest[9]=pose({-.2f,0,.85f});s.stowedJointsD63[1]=9;
    s.parent[10]=4;s.rest[10]=pose({-.1f,-.18f,1.4f});s.stowedJointsD63[2]=10;s.stowedJointsD63[3]=8;
    s.parent[11]=4;s.parent[12]=11;s.armR[11]=s.armR[12]=true;s.stowedJointsD63[4]=12;
    s.parent[13]=6;s.stowedJointsD63[5]=13;
    s.parent[14]=31;s.parent[31]=14;s.stowedJointsD63[6]=14; // malformed unsupported cycle is bounded
    s.work=s.neutral=s.rest;return s;
}
static void Stowed(){
    auto s=anatomy();RigMap rig{};Initialize(s,rig);Transaction tx{};tx.selected.fill(true);
    const auto original=s;const auto neutral=Publish(s,tx);
    ck(neutral.stowedD63[0].valid&&near(neutral.stowedD63[0].model,s.rest[8]),"rest stowed socket keeps its authored placement");
    const float half=1.1f*.5f;s.work[7]=pose({.2f,.3f,1.65f},{std::sin(half),0,0,std::cos(half)});
    s.work[8]=pose({.3f,.45f,1.7f},{std::sin(half),0,0,std::cos(half)});
    s.work[9]=pose({-.1f,.2f,.95f});s.work[10]=pose({-.2f,.3f,1.8f});
    const auto animated=Publish(s,tx);
    ck(near(animated.stowedD63[0].model,neutral.stowedD63[0].model),"R112_FIXED: native attack helper/slot cannot swing the back weapon over the head");
    ck(near(animated.stowedD63[1].model,neutral.stowedD63[1].model),"R112_FIXED: hip holster follows solved pelvis rather than native attack slot");
    ck(near(animated.stowedD63[2].model,neutral.stowedD63[2].model),"R112_FIXED: direct chest socket does not retain native local lean");
    ck(near(animated.stowedD63[0].model,animated.stowedD63[3].model),"duplicate quiver sockets publish the same solved location");
    ck(std::memcmp(s.rest.data(),original.rest.data(),sizeof(s.rest))==0,"packet preparation cannot change companion anatomy");
    const auto before=s.work;const float yaw=.7f;s.work[4]=pose({.15f,.1f,1.2f},{0,0,std::sin(yaw*.5f),std::cos(yaw*.5f)});
    const auto following=Publish(s,tx);
    // Independent expected XYZ for a rest offset (.08,-.2,.18) in chest coordinates.
    const auto expected=pose({.15f+.08f*std::cos(yaw)+.2f*std::sin(yaw),.1f+.08f*std::sin(yaw)-.2f*std::cos(yaw),1.38f},QFromQuatT(s.work[4]));
    ck(near(following.stowedD63[0].model,expected),"R112_FIXED: tracked chest translation/rotation carries the back socket once");
    ck(near(s.work[8],before[8]),"stowed publication leaves native/solver socket buffer unchanged");
    tx.stowedFullD63=false;ck(!Publish(s,tx).stowedD63[0].valid,"arms-only/native passenger transaction cannot publish stowed ownership");
    tx.stowedFullD63=true;tx.selected[8]=false;ck(!Publish(s,tx).stowedD63[0].valid,"unselected socket cannot borrow full-body publication");
    tx.selected[8]=true;tx.selected[4]=false;ck(near(Publish(s,tx).stowedD63[0].model,s.work[8]),"unowned trunk leaves the existing selected socket route intact");
    tx.selected[4]=true;
    s.work[12]=pose({.7f,.4f,.8f});s.work[13]=pose({0,.5f,1.9f});
    auto unsupported=Publish(s,tx);ck(near(unsupported.stowedD63[4].model,s.work[12]),"arm-owned socket is never reassigned to the chest");
    ck(near(unsupported.stowedD63[5].model,s.work[13]),"head/neck socket is never reassigned to an earlier trunk ancestor");
    ck(near(unsupported.stowedD63[6].model,s.work[14]),"unsupported cyclic chain remains existing route without an unbounded traversal");
    s.work[4].q[0]=std::numeric_limits<float>::quiet_NaN();ck(near(Publish(s,tx).stowedD63[0].model,s.work[8]),"invalid current trunk cannot change a valid fallback socket");
    s=anatomy();s.rest[8].t[0]=std::numeric_limits<float>::infinity();Initialize(s,rig);
    s.work[8]=pose({.4f,.3f,1.6f});ck(near(Publish(s,tx).stowedD63[0].model,s.work[8]),"invalid companion socket cannot seed a reference");
    // The current skeleton/rig init overwrites cached anchors; no old load owner remains.
    s=anatomy();Initialize(s,rig);s.stowedJointsD63[0]=-1;Initialize(s,rig);
    ck(!Publish(s,tx).stowedD63[0].valid,"new skeleton without the named socket cannot inherit old stowed ownership");
}
static std::atomic<bool> g_chest_natural128{true},g_chest_hand_bias128{true};
static std::atomic<float> g_chest_max_deg128{45.f},g_chest_gain128{.55f},g_chest_hand_weight128{.35f};
static float Chest(V3 nativeShift,bool station,bool shared,bool validLeft=true,int pelvisOwner=2){
    CoherentState6689 s{};RigMap rig{};s.neutral[rig.spine3]=pose(Add({0,0,1.2f},nativeShift));
    struct {struct {bool valid;pc::Vec3 bodyOffset;} frame51;} tx{{shared,PV(Mul(nativeShift,-1.f))}};
    const bool freeStationFoundationR13=station;const bool valid[2]{true,validLeft};
    const bool follow128=pelvisOwner==1,frameFollow51=pelvisOwner==2,stationTorsoJ27=pelvisOwner==3;
    V3 requestedGoalsRawH50[2]{{.35f,.34f,1.12f},{-.12f,.28f,1.10f}};
    const auto neutralAxes=pc::chest_axes103({1,0,0},{0,0,1});
    const double d2r=3.14159265358979323846/180.,headRelativeR38=.05,bodyYawR38=0;
    float chestYaw128=0,handBias128=0;auto yawQ128=[](double v){return QP(kcd2vr::body_follow_r38::yaw(v));};
#include "body_chest_r112.inc"
    return handBias128;
}
static void ChestReference(){
    const auto neutral=Chest({},false,true);
    for(V3 lunge: {V3{.2f,.18f,0},V3{-.12f,.09f,-.15f},V3{.08f,-.17f,.09f}}){
        ck(std::abs(Chest(lunge,false,true)-neutral)<1e-6f,"R112_FIXED: corrected ordinary chest bias ignores animated pelvis displacement");
        ck(std::abs(Chest(lunge,true,true)-neutral)<1e-6f,"existing free-station shared chest reference remains stable");
    }
    ck(std::abs(Chest({.2f,.18f,0},false,false)-neutral)>.005f,"unqualified shared frame retains existing native chest basis");
    ck(std::abs(Chest({.2f,.18f,0},false,true,true,0)-Chest({.2f,.18f,0},false,false))<1e-6f,"valid frame without owned pelvis never offsets the chest basis");
    ck(std::abs(Chest({.2f,.18f,0},false,true,true,1)-neutral)<1e-6f,"R112_FIXED: explicit body-follow pelvis uses the same corrected basis");
    ck(std::abs(Chest({.2f,.18f,0},true,true,true,3)-neutral)<1e-6f,"station torso owner preserves its existing corrected chest basis");
    ck(Chest({.2f,.18f,0},false,true,false)==0,"one missing hand cannot manufacture two-hand chest bias");
    g_chest_hand_bias128=false;ck(Chest({},false,true)==0,"saved hand-bias OFF remains honored");g_chest_hand_bias128=true;
}
int main(){Stowed();ChestReference();std::printf("body_ownership_r112 %d/%d passed; native_renderer_and_HMD=NOT_RUN\n",checks-failures,checks);return failures?1:0;}
