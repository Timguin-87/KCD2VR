// Live final-wrist caller + complete A2 adapter + actual J27 contact. Native
// identity, bounds, world ray and root reads are explicit host leaves.
#define KCD2VR_SMITH_CONTACT_FIXTURE_ONLY_D52
#include "smith_tool_contact_repair_test.cpp"
#include "vrik/pose_math.h"
#include "subsystems/smithing/anvil_a2.h"
namespace pc=kcd2vr::pose_core;
namespace av=kcd2vr::subsystems::smithing::anvil_a2;
static pc::Quat PQ(Q4 q){return {q.x,q.y,q.z,q.w};}
static Q4 QP(pc::Quat q){return {float(q.x),float(q.y),float(q.z),float(q.w)};}
struct HostPoseD52 {pc::Quat q{};pc::Vec3 p{};};
namespace kcd2vr::physics::hand_presence_h24 {
inline pc::Quat wristRotation(bool owned,const HostPoseD52& measured,pc::Quat q){return owned?measured.q:q;}
}
namespace kcd2vr::visible_failure {static bool publish(const char*,std::uint64_t){return true;}}
static bool Read(void* at,std::uintptr_t& value){if(at!=reinterpret_cast<void*>(2+0x38))return false;value=0x2000;return true;}
static bool Read(void* at,float (&value)[12]){if(at!=reinterpret_cast<void*>(0x2000+0x58))return false;std::fill_n(value,12,0.f);value[0]=value[5]=value[10]=1.f;return true;}
struct StateD52 {QuatT28 original[2];};
static Q4 DesiredHand6689(const StateD52&,int,Q4 frame,Q4 xr){return QMul(frame,XrToCryQ(xr));}
#include "smith_anvil_adapter_d52.inc"
// Independent D56 floor has its full production adapter fixture. This retained
// regression exercises D52 A2/J27 while that separate owner is inactive.
template<class S>static bool SmithNativeFloorD56(void*,std::uint64_t,int,const S&,std::uint64_t,std::uint64_t,std::uint64_t,V3&,const Q4&,SmithContactHistoryR38&){return false;}
static void AlchemyToolContactD53(void*,std::uint64_t,int,V3&,Q4&){} // non-alchemy fixture
struct TransactionD52 {
 std::uint64_t epoch=4;SmithContactHistoryR38 smithContactR38{};bool physicalActive=false;
 struct {bool owned[2]{},displayLatched[2]{},displayRejoinR13[2]{};HostPoseD52 displayHands[2]{},hands[2]{};}physicalOutput;
};
struct FinalD52 {V3 p;Q4 q,xr;};
static FinalD52 finalWrist(TransactionD52& tx,int side,V3 requested,V3 guided,Q4 requestedQ,Q4 guidedQ,float weight,
                         bool freeHand=false,bool nativeCapture=false,int owner=0){
 StateD52 s{};const int h=side;QToQuatT(s.original[h],guidedQ);ToT(s.original[h],guided);
 void* character=reinterpret_cast<void*>(0x1000);Q4 frame{};Q4 visQH50[2]{};
 visQH50[side]=CryToXrQA2(requestedQ);V3 goals[2]{};goals[side]=requested;
 const bool nativeFingerCapture=nativeCapture,freeContact164R1[2]{freeHand,freeHand};
 const bool nativeHeatingHandD56[2]{};TrackingSnapshot ts{};ts.sequence=1;ts.source_epoch=2;ts.tick=nowRepair;
 bool presenceOwnedH24[2]{},propHeld5916c1[2]{},doorPose66[2]{};
 HostPoseD52 presenceWristH24[2]{},propWrist5916c1[2]{};pc::LocalPose doorWrist66[2]{};
 if(owner==1){tx.physicalActive=true;tx.physicalOutput.owned[side]=true;tx.physicalOutput.hands[side].q=PQ(requestedQ);}
 if(owner==2){presenceOwnedH24[side]=true;presenceWristH24[side].q=PQ(requestedQ);}
 if(owner==3){propHeld5916c1[side]=true;propWrist5916c1[side].q=PQ(requestedQ);}
 if(owner==4){doorPose66[side]=true;doorWrist66[side].rotation=PQ(requestedQ);}
 const float hw=weight;const float stagedHandoverD47[2]{weight,weight};
 #include "smith_guidance_position_d52.inc"
 #include "subsystems/vrik/pose_final_wrist_d52.inl"
 return {goals[side],handDesired,visQH50[side]};
}
static double qgap(Q4 a,Q4 b){return av::qangle({a.x,a.y,a.z,a.w},{b.x,b.y,b.z,b.w});}
static void onSide(int side){
 auto sock=g_tool_socket59c10[0].value;sock.side=sock.smith36.side=side;g_tool_socket59c10[side].value=sock;
 // One real attachment cannot be owned by both hands. The D60 native reread
 // must observe this side, rather than an artificial stale duplicate in slot0.
 g_tool_socket59c10[1-side].value=ToolSocket59c10{};
 auto& p=g_committed_frame6695.value;p.valid[side]=true;p.tools59c10[side]=p.tools59c10[0];p.tools59c10[side].socket=sock;
}
static void advanceSide(int side){advanceRepair();g_tool_socket59c10[side].value.tick=nowRepair;g_committed_frame6695.value.tools59c10[side].socket=g_tool_socket59c10[side].value;}
static void resetD52(int side,int tool=0){resetRepair(tool);onSide(side);av::setMode(0);g_anvil_a2=av::Anvil{};}
int main(){
 const Q4 tilted{0,.5f,0,.8660254f};
 // Original failure: after contact stopped at x=0, the old fixture blend
 // pulled the display to x=.1 and rotated it 30 degrees. Current code is tested
 // for both hands and several fixture weights, including no guidance.
 for(int side:{0,1})for(float weight:{0.f,.25f,.5f,1.f}){
  resetD52(side,side?1:0);TransactionD52 tx{};tx.smithContactR38=ReadSmithContactHistoryR38();
  (void)finalWrist(tx,side,{},{},{},{},0);CommitSmithContactHistoryR38(tx.smithContactR38);
  advanceSide(side);queryStateRepair=wc::Result::Hit;tx.smithContactR38=ReadSmithContactHistoryR38();
  const auto r=finalWrist(tx,side,{.2f,0,0},{.3f,0,0},tilted,tilted,weight);
  const auto& c=tx.smithContactR38.world[side];
  checkRepair(std::fabs(r.p.x)<1e-6&&qgap(r.q,{})<.001,"guidance cannot overwrite the blocked position or rotation");
  checkRepair(Len(Sub(r.p,c.lastGoal))<1e-6&&qgap(r.q,c.lastQ)<.001,"display candidate and contact history are the same accepted pose");
  checkRepair(qgap(r.xr,CryToXrQA2(tilted))<.001,"contact never rewrites raw/controller visual orientation input");
  const auto grip=RelativeFromAbsolute(g_committed_frame6695.value.allowed[side],g_committed_frame6695.value.tools59c10[side].model);
  QuatT28 wrist{};QToQuatT(wrist,r.q);ToT(wrist,r.p);const auto tool=AbsoluteFromRelative(wrist,grip);
  checkRepair(Len(Sub(FromT(tool),c.lastGoal))<1e-6&&qgap(QFromQuatT(tool),c.lastQ)<.001,"tool attachment uses the same allowed wrist");
  checkRepair(g_smith_tool_contact_j27[side].lastGoal.x==0,"staged correction is not a premature history publication");
  CommitSmithContactHistoryR38(tx.smithContactR38);advanceSide(side);queryStateRepair=wc::Result::Clear;
  tx.smithContactR38=ReadSmithContactHistoryR38();const auto released=finalWrist(tx,side,{.2f,0,0},{.3f,0,0},tilted,tilted,weight);
  checkRepair(std::fabs(released.p.x-(.2f+.1f*weight))<1e-6&&qgap(released.q,tilted)<.001,"clear movement follows guided intent without stale contact drag");
 }
 // Existing owner output remains exact when no smith owner exists.
 for(int owner=0;owner<5;++owner){resetD52(0);smithLiveRepair=false;TransactionD52 tx{};
  const auto r=finalWrist(tx,0,{.2f,0,0},{.3f,0,0},tilted,{},0,false,false,owner);
  checkRepair(std::fabs(r.p.x-.2f)<1e-6&&qgap(r.q,tilted)<.001,"non-smith wrist owner preserves its requested pose");}
 for(bool capture:{false,true})for(bool free:{false,true}){resetD52(0);TransactionD52 tx{};
  (void)finalWrist(tx,0,{},{},{},{},0,free,capture);
  checkRepair(tx.smithContactR38.world[0].have==(!free&&!capture),"only held, non-capture wrist dispatches exact smith owner");}
 // Native owner loss retires the previous contact state, even with fresh copied packets.
 resetD52(0);TransactionD52 tx{};(void)finalWrist(tx,0,{},{},{},{},0);advanceSide(0);
 ++kcd2vr::workstation178r20c1::smithHoldEpoch36;queryStateRepair=wc::Result::Hit;
 auto r=finalWrist(tx,0,{.2f,0,0},{.4f,0,0},tilted,{},.5f);
 checkRepair(std::fabs(r.p.x-.3f)<1e-6&&!tx.smithContactR38.world[0].have,"revoked owner cannot constrain a new guided wrist");
 // Execute the actual A2 branch against explicitly supplied current REAL box.
 resetD52(0);av::setMode(1);g_anvil_a2=av::Anvil{};g_anvil_a2.sourceModeD39=1;g_anvil_a2.modeRevisionD39=av::modeRevisionD39.load();
 g_anvil_a2.id={1,0x2000,7,3};g_anvil_a2.poseEpoch=4;g_anvil_a2.rootFwd={0,1,0};g_anvil_a2.blows=1;
 g_anvil_a2.locked=g_anvil_a2.box=true;g_anvil_a2.boxMin={-.5,-.5,-.5};g_anvil_a2.boxMax={.5,.5,0};g_anvil_a2.top=0;
 tx=TransactionD52{};(void)finalWrist(tx,0,{0,0,.25f},{0,0,.25f},{},{},0);advanceSide(0);
 r=finalWrist(tx,0,{0,0,.05f},{0,0,-.3f},{},tilted,.8f);
 checkRepair(tx.smithContactR38.anvil[0].corrected,"A2 real-box branch constrains fixture-guided requested pose");
 checkRepair(Len(Sub(r.p,tx.smithContactR38.anvil[0].lastGoal))<1e-6&&qgap(r.q,tx.smithContactR38.anvil[0].lastQ)<.001,"A2 stores exactly the accepted displayed position and rotation");
 checkRepair(qgap(r.xr,{})<.001,"A2 model rotation output leaves controller input unchanged");
 // Detaching while turning A2 OFF must retire a formerly corrected tongs
 // wrist. A bounded visual recovery still needs the same native held owner.
 resetD52(1,1);TransactionD52 detachedTx{};auto& prior=detachedTx.smithContactR38.anvil[1];
 prior.have=prior.corrected=true;prior.owner=g_tool_socket59c10[1].value;
 prior.id=prior.owner.id;prior.epoch=4;prior.smithEpoch=3;prior.lastTick=nowRepair;
 prior.lastGoal={};prior.lastQ={};ironAttachedRepair=false;
 r=finalWrist(detachedTx,1,{.2f,0,0},{.3f,0,0},tilted,tilted,.5f);
 checkRepair(std::fabs(r.p.x-.25f)<1e-6&&qgap(r.q,tilted)<.001&&
   !detachedTx.smithContactR38.anvil[1].have&&!detachedTx.smithContactR38.world[1].have,
   "detached tongs cannot recover a previous A2 correction after collision mode turns off");
 std::printf("smith_final_wrist_d52: %u checks PASS\n",checksRepair);
}
