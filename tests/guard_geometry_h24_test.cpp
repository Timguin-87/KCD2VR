// Actual shared producer/native-consumer posture function. No native result mocks.
#include "vrik/physical_weapon.h"
#include <cstdio>
namespace ph=kcd2vr::physical;namespace m=kcd2vr::pose_core;
static int n=0,bad=0;static void ck(bool x,const char* s){++n;bad+=!x;std::printf("%s %s\n",x?"PASS":"FAIL",s);}
int main(){
 ph::Input in{};in.allowed=in.tracked[0]=in.tracked[1]=in.positionGuard176=true;
 in.shoulders[0]={.15,0,1.3};in.shoulders[1]={-.15,0,1.3};in.grip[0]=1;
 ph::Profile p{};p.key=77;p.generation=4;p.side=0;p.classId=4;p.primary=p.geometry=true;
 p.tip={0,0,1};p.axis={0,0,1};p.diagnosticBounds59c6=true;
 p.diagnosticBoundsMin59c6={-.08,-.01,-.05};p.diagnosticBoundsMax59c6={.08,.01,1.0};
 ph::Pose w{{},{0,.35,1.1}};ph::Vec3 normal{};
 ck(ph::planar_face24(p,normal)&&normal.y==1,"measured planar model uses its thin transverse dimension, not blade axis");
 ck(ph::natural_guard117(in,p,w,false),"vertical measured broad face in front of torso qualifies");
 const auto beforeGrip=ph::point(w,p.grip[0].p),beforeTip=ph::point(w,p.tip);
 w.q={0,0,.7071067811865475,.7071067811865475};
 ck(m::length(ph::point(w,p.grip[0].p)-beforeGrip)<1.e-8&&m::length(ph::point(w,p.tip)-beforeTip)<1.e-8,
    "ninety degree blade roll leaves grip and tip positions unchanged");
 ck(!ph::natural_guard117(in,p,w,false)&&!ph::natural_guard117(in,p,w,true),"edge facing torso releases auto block despite identical blade segment");
 w.q={};w.p={0,-.05,1.1};ck(!ph::natural_guard117(in,p,w,true),"blade behind chest cannot retain automatic block");
 w.p={.65,.35,1.1};ck(!ph::natural_guard117(in,p,w,true),"blade carried beside torso releases auto block");
 w.p={0,.35,.25};ck(!ph::natural_guard117(in,p,w,true),"lowered blade releases auto block");
 w.p={0,.35,1.8};ck(!ph::natural_guard117(in,p,w,true),"blade above head releases auto block");
 w.p={0,.35,1.1};w.q={0,.7071067811865475,0,.7071067811865475};w.p.x=-.5;w.p.z=1.5;
 ck(ph::natural_guard117(in,p,w,false),"horizontal measured broad face still covers frontal torso");
 w.q={0,.3826834323650898,0,.9238795325112867};w.p.x=-.3;w.p.z=1.2;
 ck(ph::natural_guard117(in,p,w,false),"diagonal measured broad face still covers frontal torso");
 auto unknown=p;unknown.diagnosticBoundsMin59c6={-.02,-.02,-.19};unknown.diagnosticBoundsMax59c6={.02,.02,1.06};
 ck(!ph::planar_face24(unknown,normal)&&!ph::natural_guard117(in,unknown,w,false),"actual square-proxy dimensions do not invent a flat normal");
 auto visual=unknown;visual.guardFaceReadyH24=true;visual.guardFaceNormalH24={1,0,0};
 ph::Pose visualPose{{0,0,.7071067811865475,.7071067811865475},{0,.35,1.1}};
 ck(ph::planar_face24(visual,normal)&&normal.x==1&&ph::natural_guard117(in,visual,visualPose,false),
    "qualified visual sharpening face restores real longsword block despite round collision bounds");
 visualPose.q={};ck(!ph::natural_guard117(in,visual,visualPose,true),
    "real longsword visual face rejects edge-on held guard independently of proxy bounds");
 visual.guardFaceNormalH24={0,0,1};ck(!ph::planar_face24(visual,normal),
    "longitudinal or corrupt helper cannot masquerade as a blade face");
 unknown=p;unknown.diagnosticBounds59c6=false;ck(!ph::natural_guard117(in,unknown,w,false),"missing measured plane cannot silently restore permissive automatic block");
 auto shield=p;shield.classId=8;shield.side=1;shield.diagnosticBounds59c6=false;shield.grip[1].p={};
 kcd2vr::shield_block_r120::enabled=false; // R120 switch OFF = the legacy wide shield rule
 w={};w.p={-.42,.35,1.4};ck(ph::natural_guard117(in,shield,w,false),"shield keeps its separate owned frontal geometry without fake blade normal");
 kcd2vr::shield_block_r120::enabled=true; // R120 shield block box (default ON)
 ck(!ph::natural_guard117(in,shield,w,false),"R120 shield 42 cm out to the left is outside the shield box");
 w.p={-.10,.35,1.25};ck(ph::natural_guard117(in,shield,w,false),"R120 shield raised in front of the chest is inside the shield box");
 w.p={-.20,.25,.88};ck(!ph::natural_guard117(in,shield,w,false),"R120 shield lowered to the belly is not a block");
 // Exercise the actual pose producer as well as its shared native gate.
 in.naturalCombat117=in.coherentMelee119=in.attacks=in.heldMotionAttacks=true;
 in.controllerAnchor119=true;in.dynamics=false;in.contact=false;in.epoch=in.bodyEpoch=1;in.tick=1000;in.sample=1;
 in.guardEnterDwellMs178r29=0;in.guardEnterMaxSpeed178r29=2.5;
 auto produce=[&](ph::Pose pose){ph::State state{};ph::Output out{};std::array<ph::Profile,2> profiles{};profiles[0]=p;
   in.hands[0]=in.intentHands[0]=pose;in.intentTracked[0]=true;
   for(int i=0;i<24;++i){in.tick+=10;++in.sample;out=ph::step(state,in,profiles,[](const ph::Profile&,ph::Pose,ph::Pose,bool){return 1.;});}
   return out;
 };
 w={{},{0,.35,1.1}};ck(produce(w).guardBySide30c1[0],"actual pose producer publishes frontal broad-face block owner");
 w.q={0,0,.7071067811865475,.7071067811865475};ck(!produce(w).guardBySide30c1[0],"actual pose producer publishes READY rather than hidden block after edge-on roll");
 std::printf("%d checks %d failures\n",n,bad);return bad?1:0;
}
