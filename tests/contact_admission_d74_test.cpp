// Actual extracted production contact query/completion; native geometry is a
// synthetic test leaf. This is not a headset or engine-physics result.
#define KCD2VR_WORLD_ADAPTER_FIXTURE_ONLY_D53 1
#include "world_adapter_test.cpp"
#undef KCD2VR_WORLD_ADAPTER_FIXTURE_ONLY_D53
#include "../src/vrik/contact_admission_d74.h"
#include "../src/vrik/elbow_shoulder_d45.h"
#include "../src/vrik/shoulder_reach_d38.h"
namespace pc=kcd2vr::pose_core;
static pc::Vec3 PV(V3 v){return {v.x,v.y,v.z};}
static V3 VP(pc::Vec3 v){return {float(v.x),float(v.y),float(v.z)};}
static pc::Quat PQ(Q4 q){return {q.x,q.y,q.z,q.w};}
namespace ph {
#include "generated_reach_max_d74.inc"
}
struct ShoulderFixtureD74 {
 QuatT28 work[5]{};double upper[2]{.3,.3},fore[2]{.27,.27};
 bool allowedReady[2]{true,true},handBlocked[2]{true,true},armBlocked[2]{true,true},armSaturated[2]{true,true};
};
static void SetWorldPosition128(ShoulderFixtureD74& s,int i,V3 p){s.work[i].t[0]=p.x;s.work[i].t[1]=p.y;s.work[i].t[2]=p.z;}
static bool Compose6689(ShoulderFixtureD74&){return true;} // hierarchy-write leaf only
static void ActualShoulderCallerD74(){
 for(bool experiment:{false,true})for(bool lab:{false,true}){
  ShoulderFixtureD74 s{};for(auto& p:s.work)p.q[3]=1;SetWorldPosition128(s,2,{.4f,0,0});
  const struct {int upper_r=0,upper_l=1,hand_r=2,hand_l=3,spine3=4;} rig;
  struct{std::uint64_t epoch=1;bool traceH29=false;} tx;
  struct{std::uint64_t sequence=10,tick=1000,timeNs=1000000000;} ts;
  pc::ElbowContinuity stagedElbow[2]{};kcd2vr::elbow_shoulder_d45::ShoulderState stagedShoulderFitD45[2]{};
  HandContact6695 stagedContact[2]{};stagedContact[0].have=stagedContact[0].blocked=stagedContact[0].fresh169r1=true;
  const std::uint64_t elbowPolicyD45=1;V3 gestureShouldersR38[2]{};
#include "generated_shoulder_caller_d74.inc"
  const V3 raw{.4f,0,0};V3 endpoint{0,4,0};
  Check(accommodateShoulderR38(0,endpoint,lab,false,experiment,&raw)&&endpoint.x==raw.x&&endpoint.y==raw.y&&endpoint.z==raw.z,
        "actual caller retires remote collision and preserves exact controller endpoint");
  Check(!stagedContact[0].have&&!stagedContact[0].fresh169r1&&!s.allowedReady[0]&&!s.handBlocked[0],
        "actual caller clears collision authority instead of rejecting whole pose");
  auto arm=pc::solve_arm(PV(FromT(s.work[0])),PV(endpoint),.3,.27,{0,0,-1},{0,0,-1},
       experiment?kcd2vr::elbow_shoulder_d45::reserve(.3,.27):-1.);
  Check(arm&&pc::length(arm->hand-PV(raw))<1e-5&&std::abs(pc::length(arm->elbow-PV(FromT(s.work[0])))-.3)<1e-5&&
        std::abs(pc::length(arm->hand-arm->elbow)-.27)<1e-5,"repaired actual caller keeps both arm lengths and controller wrist");
  endpoint={.36f,0,0};
  Check(accommodateShoulderR38(0,endpoint,lab,false,experiment,&raw)&&endpoint.x==.36f,
        "nearby valid contact endpoint retains its authority without hand filtering");
  endpoint={0,4,0};
  Check(accommodateShoulderR38(0,endpoint,lab,false,experiment,nullptr)&&endpoint.y==4,
        "caller without free-world collision admission preserves existing owner behavior");
 }
}

static HandContactPending6695 Impossible(HandContact6695 before,std::uint64_t tick){
 HandContactPending6695 p{};p.ready=true;p.before=p.queried=before;p.worldRotation={0,0,0,1};p.worldScale=1;
 p.requested=p.rawDesiredD45=p.desired={.10f,0,0};p.skin=.008f;
 p.queried.planeCount=2;p.queried.blocked=true;p.queried.featureCount=5;
 // Current tangent set asks x>=8mm AND x<=-8mm. It cannot be a
 // simultaneously feasible solid-hand endpoint; never renew the old answer.
 p.queried.planes[0]={{0,0,0},{1,0,0},0};p.queried.planes[1]={{0,0,0},{-1,0,0},0};
 for(auto& f:p.rawFeatures)f=p.rawDesiredD45;g_contact_query_tick_h18=tick;return p;
}
int main(){
 ActualShoulderCallerD74();
 Init();HandContact6695 c{};c.have=c.blocked=true;c.worldAllowed={0,0,0};c.lastCompleteTickH18=1000;
 c.planeCount=1;c.featureCount=5;c.planes[0]={{0,0,0},{-1,0,0},0};V3 out{};
 for(auto tick:{1011ull,1022ull,1044ull,1079ull}){
  auto p=Impossible(c,tick);Check(CompleteHandContact6695(c,p,out),"infeasible corner keeps original short conservative hold");
  Check(!c.fresh169r1&&c.lastCompleteTickH18==1000,"infeasible corner does not fabricate freshness or renew deadline");
 }
 auto expired=Impossible(c,1081);Check(CompleteHandContact6695(c,expired,out),"expired incomplete endpoint uses existing release policy");
 Check(!c.fresh169r1&&c.lastCompleteTickH18==1000&&!c.planeCount&&out.x>0,"repeated failed corner cannot glue the wrist forever");
 // Current feasible geometry still establishes a fresh full stop; this is not
 // treating failed queries or genuine obstacles as clear.
 c={};c.have=true;c.worldAllowed={};c.featureCount=5;
 auto fresh=Impossible(c,1200);fresh.queried.planeCount=1;fresh.queried.planes[0]={{.048f,0,0},{-1,0,0},0};
 Check(CompleteHandContact6695(c,fresh,out)&&c.fresh169r1&&c.lastCompleteTickH18==1200&&std::abs(out.x-.04f)<.00001f,
       "valid current plane retains exact positive skin and new query receipt");
 // Existing and new feature layouts do not share digit/palm array identities.
 Init();c={};c.have=c.rawFeatureReady55=true;c.featureCount=6;c.worldAllowed={};c.lastCompleteTickH18=1000;
 c.character=reinterpret_cast<void*>(character);
 for(auto& v:c.offset)v={-.6f,0,0};for(auto& v:c.rawFeatureWorld55)v={-.6f,0,0};
 c.springH31={};V3 features[5]{};for(auto& v:features)v={.04f,0,0};std::uint64_t previous=0;
 {ContactPass6695 pass(true,1011,previous,1,3);
  Check(ContactHand6695(c,reinterpret_cast<void*>(character),0,{0,0,0},{.001f,0,0},out,features),"layout change queries current geometry");
 }
 Check(mockOrigins55.size()==5&&std::abs(mockOrigins55.front().x-.04f)<.00001f,"precise-to-coarse uses new feature offsets, not old unrelated digit origins");
 Check(out.x==.001f&&out.y==0&&out.z==0,"clear free-hand endpoint remains exact, no added hand lag");
 namespace ca=kcd2vr::contact_admission_d74;namespace pc=kcd2vr::pose_core;
 const pc::Vec3 natural{},raw{.4,0,0},axis{1,0,0};
 const auto remote=pc::controller_shoulder103(natural,{0,4,0},.3,.27,axis);
 Check(remote&&!ca::shoulderAllowed(natural,raw,remote->shoulder,.3,.27,axis),"remote collision cannot pull shoulder metres away while upper and forearm lengths stay valid");
 Check(ca::shoulderAllowed(natural,raw,{.1,0,0},.3,.27,axis),"bounded scapular participation remains allowed");
 const pc::Vec3 extended{.95,0,0};const auto extendedFit=pc::controller_shoulder103(natural,extended,.3,.27,axis);
 Check(extendedFit&&ca::shoulderAllowed(natural,extended,extendedFit->shoulder,.3,.27,axis),"existing legitimate controller reach is not globally capped");
 Check(ca::shoulderAllowed(natural,extended,natural,.3,.27,axis),"contact returning shoulder toward body is not rejected");
 Check(!ca::shoulderAllowed(natural,raw,{NAN,0,0},.3,.27,axis),"invalid endpoint has no anatomical authority");
 std::printf("D74_CONTACT_ADMISSION %d/%d; native/headset=NOT_EXECUTED\n",assertions-failures,assertions);return failures?1:0;
}
