// Current production frame, pelvis/foot callsite and fixed-length leg solver.
// Native stance receipt and a small valid skeleton are explicit fixture inputs.
#include "vrik/tracked_frame51.h"
#include "vrik/body_follow_r38.h"
#include <array>
#include <atomic>
#include <cstdio>
#include <cmath>
namespace pc=kcd2vr::pose_core;namespace tf=kcd2vr::tracked_frame51;
struct V3 {float x{},y{},z{};};struct Q4 {float x{},y{},z{},w{1};};
struct QuatT28 {float q[4]{0,0,0,1};float t[3]{};};
#include "native_crouch_math_r93.inc"
struct CoherentState6689 {
 struct Leg128 {int hip=-1,knee=-1,ankle=-1;double thigh=0,shin=0;bool ok=false;}leg[2]{};
 std::uint32_t count=10;std::array<int,10> order{},parent{};
 std::array<QuatT28,10> local{},work{},original{};
 std::array<bool,10> dirty{},essential137{},pending137{},pelvisTree128{};
 bool dirtyAll=true,deferPassengers137=false,legsReady128=true;unsigned composedJoints=0;
};
#include "native_crouch_solve_r93.inc"
struct Tx {tf::Frame frame51{};kcd2vr::body_follow_r38::State bodyFollowR38{};bool diceBodyD81=false;};
struct Rig {int pelvis=1;};
struct Snapshot {std::uint64_t sequence=1,tick=1000;};
struct Axes {pc::Vec3 forward{0,1,0};};
namespace animation_filter {static bool NativeKickActive175(){return false;}}
static std::atomic<bool> g_body_follow128{false};
static std::atomic<float> g_follow_crouch128{1},g_step_trigger128{.16f};
static bool RejectCoherent6693(Tx&,int){return false;}
static Q4 yawQ128(double x){return QP(kcd2vr::body_follow_r38::yaw(x));}
static bool solveBody(CoherentState6689& s,Tx& tx,const Snapshot& ts,bool followOn){
 const bool full=true,station6DofH41=false,stationTorsoJ27=false,framePelvis54=false;
 g_body_follow128=followOn;const V3 headAnchor123=VP(tx.frame51.anchor),headTarget=headAnchor123;
 const Rig rig{};const Axes axes{};const Axes* neutralAxes=&axes;
 const auto headingR38=kcd2vr::body_follow_r38::Heading{0,0,true,true,true};const double bodyYawR38=0;
 #include "native_crouch_follow_gate_r93.inc"
 V3 follow128Offset{};float follow128Reach[2]{};
 if(follow128||frameFollow51||stationTorsoJ27){
 #include "native_crouch_drop_r93.inc"
 #include "native_crouch_torso_r93.inc"
 }
 return true;
}
static CoherentState6689 skeleton(float pelvisZ){
 CoherentState6689 s{};s.parent={-1,0,1,2,1,4,5,1,7,8};
 for(int j=0;j<10;++j){s.order[j]=j;s.essential137[j]=s.pelvisTree128[j]=true;}
 s.leg[0]={4,5,6,.5,.5,true};s.leg[1]={7,8,9,.5,.5,true};
 ToT(s.work[1],{0,0,pelvisZ});ToT(s.work[2],{0,0,pelvisZ+.4f});ToT(s.work[3],{0,0,pelvisZ+.8f});
 const float bend=std::sqrt(.25f-pelvisZ*pelvisZ*.25f);
 for(int side=0;side<2;++side){const auto& leg=s.leg[side];const float x=side?.12f:-.12f;
  ToT(s.work[leg.hip],{x,0,pelvisZ});ToT(s.work[leg.knee],{x,bend,pelvisZ*.5f});ToT(s.work[leg.ankle],{x,0,0});}
 s.original=s.work;for(int j=0;j<10;++j)s.local[j]=s.parent[j]<0?s.work[j]:RelativeFromAbsolute(s.work[s.parent[j]],s.work[j]);
 Compose6689(s);return s;
}
static unsigned checks=0,failures=0;
static void ck(bool ok,const char* what){++checks;if(!ok){++failures;std::printf("FAIL %s\n",what);}}
static bool near(double a,double b,double tolerance=2e-5){return std::abs(a-b)<tolerance;}
int main(){
 for(bool follow:{false,true})for(double scale:{.8,1.,1.25}){
  tf::State state{};tf::Input in{};in.stamp={1,1,1000,0x100};in.control=1;in.timeNs=1000000000;in.scale=scale;
  in.allowed=in.eyeNeutral=in.hmdHeight=in.nativeStanceR93=true;
  in.restHead=in.calibratedHead={0,0,1.75};in.restPelvis={0,0,.95};
  const bool crouched[]{false,true,true,true,false,false,false};const double nativeDrop[]{0,0,.25,.5,.5,.25,0};
  Tx tx{};
  for(int stage=0;stage<7;++stage){
   ++in.stamp.sample;in.stamp.tick+=16;in.timeNs+=16000000;in.nativeCrouchedR93=crouched[stage];
   in.nativePelvis={0,0,.95-nativeDrop[stage]};in.nativeHead={0,0,1.75-nativeDrop[stage]};
   ck(tf::advance(state,in,tx.frame51),"actual frame accepts native stance while authored pelvis transitions");
   auto s=skeleton(float(in.nativePelvis.z));const auto nativeLocal=s.local;
   const double wanted=.95-(crouched[stage]?.5/scale:0);
   ck(solveBody(s,tx,{in.stamp.sample,in.stamp.tick},follow),"actual pelvis and two-leg solve succeeds");
   ck(near(FromT(s.work[1]).z,wanted),"pelvis receives shared native crouch exactly once");
   ck(near(FromT(s.work[3]).z,wanted+.8)&&near(FromT(s.work[3]).z,tx.frame51.anchor.z),"fixed spine and head remain on same shared crouch frame");
   ck(near((FromT(s.work[1]).z-.95)*scale,crouched[stage]?-.5:0),"world crouch depth is exactly half metre independent of scale");
   for(int side=0;side<2;++side){const auto& l=s.leg[side];
    ck(Len(Sub(FromT(s.work[l.ankle]),FromT(s.original[l.ankle])))<3e-5,"native grounded ankle remains planted");
    ck(near(Len(Sub(FromT(s.work[l.knee]),FromT(s.work[l.hip]))),l.thigh)&&near(Len(Sub(FromT(s.work[l.ankle]),FromT(s.work[l.knee]))),l.shin),"thigh and shin keep hard anatomical lengths");
    ck(Len(Sub(FromT(s.local[l.knee]),FromT(nativeLocal[l.knee])))<1e-7&&Len(Sub(FromT(s.local[l.ankle]),FromT(nativeLocal[l.ankle])))<1e-7,"leg solve rotates bones without stretching their local offsets");
    ck(pc::angle(PQ(QFromQuatT(s.work[l.ankle])),PQ(QFromQuatT(s.original[l.ankle])))<1e-5,"grounded foot orientation remains native");
   }
  }
 }
 std::printf("native_crouch_body_r93 %u/%u checks pass; actual frame/pelvis/foot/leg code; synthetic skeleton, no engine\n",checks-failures,checks);
 return failures?1:0;
}
