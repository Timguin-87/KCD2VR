// Executes the actual current free-hand/smith caller, including its outer
// collision/performance gates. World queries and native tool solver are leaves;
// this test verifies ownership dispatch, not a fabricated physical world.
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <initializer_list>
#include "vrik/pose_math.h"
namespace pc=kcd2vr::pose_core;
struct V3 {float x=0,y=0,z=0;};
struct Q4 {float x=0,y=0,z=0,w=1;};
static pc::Quat PQ(Q4 q){return {q.x,q.y,q.z,q.w};}
static Q4 QP(pc::Quat q){return {float(q.x),float(q.y),float(q.z),float(q.w)};}
static Q4 QFromQuatT(V3){return {};}
struct HostPose {pc::Quat q{};pc::Vec3 p{};};
namespace kcd2vr::physics::hand_presence_h24 {
static pc::Quat wristRotation(bool owned,const HostPose& pose,pc::Quat q){return owned?pose.q:q;}
}
static V3 FromT(V3 v){return v;}
namespace kcd2vr::passive169r1 {
enum class Source {HandWorld};
struct Sample {Source source{};int side=0;std::uintptr_t actor=0;std::uint64_t key=0,epoch=0,request=0,sequence=0,tick=0;};
}
namespace kcd2vr::diagnostics::passive169r1 {
struct RayScope {explicit RayScope(const kcd2vr::passive169r1::Sample&) {}};
}
struct FingerContactGeometry103 {
 struct Spring {std::uintptr_t character=0;std::uint64_t epoch=0,source=0,control=0,sequence=0,tick=0;bool enabled=false;};
 Spring springSourceH31{};
};
struct HandContactPending6695 {bool ready=false;V3 requested{};};
struct HandContact6695 {std::uint64_t lastCompleteTickH18=0;bool blocked=false;};
struct SmithContactHistoryR38 {};
struct State {V3 original[2]{},allowedGoal[2]{};bool allowedReady[2]{},handBlocked[2]{},armBlocked[2]{},armSaturated[2]{};};
static std::atomic<std::uint64_t> g_contact_control_epoch6695{1};
static unsigned smithCalls[2]{};
static Q4 DesiredHand6689(const State&,int,int,Q4 q){return q;}
static bool PalmGeometry103(const State&,int,Q4,float,float,std::uint64_t,V3*,FingerContactGeometry103&,float,bool){return true;}
static bool SimplePalmGeometryR24(const State&,int,Q4,V3*){return true;}
static bool ContactHand6695(HandContact6695&,void*,std::uint64_t,V3,V3,V3&,V3*,FingerContactGeometry103*,HandContactPending6695*,bool=false){return false;}
static bool CompleteHandContact6695(HandContact6695&,HandContactPending6695&,V3&){return false;}
static void CollisionStatus6692(std::uint64_t,const char*){}
static void SmithAnvilContactA2(void*,std::uint64_t,int side,const State&,int,Q4&,V3&,SmithContactHistoryR38* staged,Q4*){if(staged)++smithCalls[side];}
#include "generated_free_hand_caller.inc"
static void caller(bool collisionMode,bool perfContactModeDs2,bool nativeFingerCapture,const bool (&valid)[2],const bool (&freeContact164R1)[2]){
 const bool preciseContactModeR24=true,stationContactD75=false;State s{};HandContact6695 stagedContact[2]{};V3 goals[2]{};
 const struct {std::uint64_t tick=1000,sequence=1;float left_grip=0,right_grip=0,left_trigger=0,right_trigger=0;}ts;
 struct {std::uint64_t epoch=1;bool fingerTrialH7[2]{},fingerSpringH31=false;float fingerOpenBiasH7[2]{};SmithContactHistoryR38 smithContactR38{};
  bool physicalActive=false;
  struct {bool owned[2]{},displayLatched[2]{},displayRejoinR13[2]{};HostPose displayHands[2]{},hands[2]{};}physicalOutput;
 }tx;
 const struct {int hand_l=1,hand_r=0;}rig;
 void* character=reinterpret_cast<void*>(0x1000);const int frame=0;Q4 visQH50[2]{};
 const std::uint64_t heldFingerKey[2]{},contactControl=1;
 WORLD_ADAPTER_FREE_HAND_CALLER_H24
 const bool presenceOwnedH24[2]{},propHeld5916c1[2]{},doorPose66[2]{};
 const HostPose presenceWristH24[2]{},propWrist5916c1[2]{};const pc::LocalPose doorWrist66[2]{};
 const float stagedHandoverD47[2]{};
 for(int side=0;side<2;++side)if(valid[side]){
  const int h=side;
  #include "subsystems/vrik/pose_final_wrist_d52.inl"
 }
}
static unsigned checks=0;
static void check(bool ok,const char* why){++checks;if(!ok){std::fprintf(stderr,"FAIL: %s\n",why);std::exit(1);}}
int main(){
 for(bool collision:{false,true})for(bool perf:{false,true}){
  const bool valid[2]{true,true},held[2]{false,false};smithCalls[0]=smithCalls[1]=0;
  caller(collision,perf,false,valid,held);
  check(smithCalls[0]==1&&smithCalls[1]==1,"smith collision survives free-hand off and performance mode");
  caller(collision,perf,true,valid,held);
  check(smithCalls[0]==1&&smithCalls[1]==1,"native finger capture keeps ownership");
 }
 const bool valid[2]{true,false},held[2]{false,false};smithCalls[0]=smithCalls[1]=0;
 caller(false,true,false,valid,held);check(smithCalls[0]==1&&smithCalls[1]==0,"invalid hand is never dispatched");
 const bool both[2]{true,true},freeHand[2]{true,true};smithCalls[0]=smithCalls[1]=0;
 caller(true,false,false,both,freeHand);check(!smithCalls[0]&&!smithCalls[1],"free hands retain their own collision owner");
 std::printf("smith_contact_dispatch_repair: %u checks PASS\n",checks);
}
