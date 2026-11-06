// Actual production J27 adapter with explicit native socket/bounds/query leaves.
// There is no retail execution or assertion of recipe/strike completion here.
#include "vrik/smith_tool36.h"
#include "vrik/station_ws_j27.h"
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <vector>
// Current native lifecycle/request witnesses are explicit leaves, independent
// of the two copied tool packets. Revocation must win over fresh old packets.
namespace kcd2vr::activity_input163 {
enum class Kind175 : unsigned {Alchemy,Smithing};
struct WitnessRepair {
 std::atomic<bool> ready{true};std::atomic<unsigned> transitionDepth175{0};
 std::atomic<std::uint64_t> lifetime175{7},smithEpochC1{3};std::atomic<std::uintptr_t> active[2]{};
};
static WitnessRepair witness175;
}
namespace kcd2vr::workstation178r20c1 {static std::atomic<std::uint64_t> smithHoldEpoch36{5};}
namespace kcd2vr::minigame_hands45 {static std::atomic<std::uint64_t> modeEpoch67{6};}
struct QuatT28 {float q[4]{0,0,0,1};float t[3]{};};
#include "generated_adapter_types.h"
struct ToolSocket59c10 {
 kcd2vr::smith_tool36::Binding smith36{};int side=-1;void* character=nullptr;void* entity=nullptr;
 std::uint32_t id=0;std::uint64_t epoch=0,tick=0,request=0,modeEpoch67=0;
};
struct CommittedFrame6695 {
 struct Stamp {std::uintptr_t actor=0;std::uint64_t epoch=0,tick=0;}stamp;
 struct Tool {bool valid=false;ToolSocket59c10 socket{};QuatT28 model{};}tools59c10[2];
 bool valid[2]{};QuatT28 allowed[2]{};
};
template<class T>struct Copy {T value{};bool live=true;bool read(T& out)const{out=value;return live;}};
static Copy<ToolSocket59c10> g_tool_socket59c10[2];
static Copy<CommittedFrame6695> g_committed_frame6695;
struct ModelWorld6692 {Q4 q{};float scale=1;V3 p{};};
static bool ReadModelWorld6692(void*,ModelWorld6692& x){x=ModelWorld6692{};return true;}
static V3 ModelToWorld6692(const ModelWorld6692& x,V3 p){return Add(x.p,Mul(QRotateV(x.q,p),x.scale));}
static V3 WorldToModel6692(const ModelWorld6692& x,V3 p){return Mul(QRotateV(QConj(x.q),Sub(p,x.p)),1/x.scale);}
static unsigned logWritesRepair=0;
static void(*onBoundsRepair)()=nullptr;static void(*onRayRepair)()=nullptr;
static bool nativeOwnerRepair=true;static unsigned nativeOwnerReadsRepair=0;
namespace diagnostic_trace {static void Write(const char*){++logWritesRepair;}}
static std::uint64_t nowRepair=1000;
static std::uint64_t GetTickCount64(){return nowRepair;}
static bool smithLiveRepair=true,boundsLiveRepair=true,ironLiveRepair=false,ironAttachedRepair=true;
static std::uint32_t ironEntityRepair=77;
static unsigned boundsReadsRepair=0,ironBoundsReadsRepair=0;
static const char* SmithAutoHoldReason36(std::uint64_t){return smithLiveRepair?nullptr:"INACTIVE";}
static void* EntityById6694(std::uint32_t id){return reinterpret_cast<void*>(std::uintptr_t(id));}
static bool PhysBounds6698(void* entity,float (&bounds)[6]){
 ++boundsReadsRepair;if(onBoundsRepair)onBoundsRepair();if(!boundsLiveRepair)return false;
 const float hammer[6]{-.02f,-.02f,0,.02f,.02f,.30f};
 const float iron[6]{-.25f,-.01f,-.01f,.25f,.01f,.01f};
 if(entity==EntityById6694(77)||entity==EntityById6694(78)){++ironBoundsReadsRepair;std::copy_n(iron,6,bounds);}
 else std::copy_n(hammer,6,bounds);return true;
}
namespace wc {enum class Result {Clear,Hit,Unknown};}
struct RayResult {wc::Result state=wc::Result::Clear;float distance=0;struct{int surfaceIdx=2;}native172;V3 normal{},point{},nativeNormalD63{};bool plane=false;};
static wc::Result queryStateRepair=wc::Result::Clear;
static std::vector<V3> originsRepair;
static bool planeRepair=true,autoNormalRepair=true;static V3 normalRepair{},pointErrorRepair{};
static float distanceRepair=.01f;
static RayResult WorldRayDistance6692(V3 origin,V3 delta){
 originsRepair.push_back(origin);if(onRayRepair)onRayRepair();RayResult r{};
 r.state=queryStateRepair;r.distance=distanceRepair;r.plane=planeRepair;
 const float length=float(Len(delta));const auto direction=Mul(delta,1.f/length);
 r.normal=autoNormalRepair?Mul(direction,-1):normalRepair;
 r.nativeNormalD63=r.normal;
 // Match the production leaf: preserve native normal then face the query normal toward origin.
 if(r.normal.x*delta.x+r.normal.y*delta.y+r.normal.z*delta.z>0)r.normal=Mul(r.normal,-1);
 r.point=Add(Add(origin,Mul(direction,r.distance)),pointErrorRepair);return r;
}
static bool ReadSmithTool36(void*,std::uint32_t id,std::uintptr_t character,kcd2vr::smith_tool36::Binding& out) noexcept {
 ++nativeOwnerReadsRepair;out={};if(!nativeOwnerRepair)return false;
 for(const auto& copy:g_tool_socket59c10)if(copy.value.id==id&&copy.value.smith36.character==character){out=copy.value.smith36;return true;}
 return false;
}
#ifndef KCD2VR_SMITH_CONTACT_HEADER_D60
#define KCD2VR_SMITH_CONTACT_HEADER_D60 "../src/vrik/smith_tool_contact_j27.inl"
#endif
#include KCD2VR_SMITH_CONTACT_HEADER_D60
// Native current-workpiece read is an explicit leaf. The actual read/pins are
// retained unchanged in smith_anvil_contact_a2.inl and separately documented.
static bool SmithIronA2(const kcd2vr::smith_tool36::Binding&,std::uint32_t& id,float (&world)[12]) noexcept {
 if(!ironLiveRepair)return false;id=ironEntityRepair;world[0]=world[5]=world[10]=1;world[3]=1;return true;
}
// Qualified held-workpiece read is an explicit leaf here. D78's connected
// native reader fixture executes the real D56 nested attachment proof.
static bool SmithHeldIronA2(const kcd2vr::smith_tool36::Binding& b,std::uint32_t& id,float (&world)[12]) noexcept {
 return ironAttachedRepair&&SmithIronA2(b,id,world);
}
static unsigned checksRepair=0;
static void checkRepair(bool ok,const char* why){++checksRepair;if(!ok){std::fprintf(stderr,"FAIL: %s\n",why);std::exit(1);}}
static void currentOwnerRepair(){
 using namespace kcd2vr::activity_input163;
 witness175.ready=true;witness175.transitionDepth175=0;witness175.lifetime175=7;
 witness175.active[unsigned(Kind175::Smithing)]=1;
 kcd2vr::workstation178r20c1::smithHoldEpoch36=5;kcd2vr::minigame_hands45::modeEpoch67=6;
}
static void resetRepair(int tool=0){
 g_smith_contact_history_r38=SmithContactHistoryR38{};
 for(auto& a:g_smith_world_attempts_r38)a=SmithContactAttemptsR38{};
 for(auto& a:g_smith_anvil_attempts_r38)a=SmithContactAttemptsR38{};
 currentOwnerRepair();
 nowRepair=1000;smithLiveRepair=boundsLiveRepair=ironAttachedRepair=true;ironLiveRepair=tool==1;boundsReadsRepair=ironBoundsReadsRepair=0;
 planeRepair=autoNormalRepair=true;normalRepair=pointErrorRepair={};distanceRepair=.01f;kcd2vr::diagnostics::capture168::gate.select(kcd2vr::diagnostics::capture168::Domain::Interaction,false,nowRepair);
 ironEntityRepair=77;
 originsRepair.clear();logWritesRepair=0;onBoundsRepair=onRayRepair=nullptr;nativeOwnerRepair=true;nativeOwnerReadsRepair=0;queryStateRepair=wc::Result::Clear;kcd2vr::station_ws_j27::toolCollision=true;
 g_committed_frame6695.value=CommittedFrame6695{};
 for(auto& copy:g_tool_socket59c10)copy.value=ToolSocket59c10{};
 auto& socket=g_tool_socket59c10[0].value;socket=ToolSocket59c10{};
 socket.character=reinterpret_cast<void*>(0x1000);socket.entity=EntityById6694(42);socket.id=42;socket.side=0;
 socket.epoch=4;socket.tick=nowRepair;socket.request=5;socket.modeEpoch67=6;
 auto& b=socket.smith36;b.instance=1;b.player=2;b.character=0x1000;b.holder=3;b.model=4;b.attachment=5;b.object=6;
 b.entity=42;b.sourceEntity=42;b.side=0;b.tool=tool;b.lifetime=7;b.key=8;b.host=9;
 auto& prev=g_committed_frame6695.value;prev.stamp={0x1000,4,nowRepair};prev.valid[0]=true;prev.tools59c10[0].valid=true;
 prev.tools59c10[0].socket=socket;
}
static void advanceRepair(unsigned ms=16){
 nowRepair+=ms;auto& socket=g_tool_socket59c10[0].value;socket.tick=nowRepair;
 auto& prev=g_committed_frame6695.value;prev.stamp.tick=nowRepair;prev.tools59c10[0].socket=socket;
}
static void solveRepair(V3& goal,Q4& q){SmithToolContactJ27(reinterpret_cast<void*>(0x1000),4,0,q,goal,&q);}
#ifndef KCD2VR_SMITH_CONTACT_FIXTURE_ONLY_D52
int main(){
 resetRepair();boundsLiveRepair=false;V3 goal{};Q4 q{};solveRepair(goal,q);
 checkRepair(boundsReadsRepair==1&&!g_smith_tool_contact_j27[0].boundsOk,"first missing attachment bounds are observed");
 for(int i=0;i<4;++i){advanceRepair(20);solveRepair(goal,q);}
 checkRepair(boundsReadsRepair==1,"missing bounds do not spin native reads each frame");
 boundsLiveRepair=true;advanceRepair(20);solveRepair(goal,q);
 checkRepair(boundsReadsRepair==2&&g_smith_tool_contact_j27[0].boundsOk,"same tool retries failed bounds after 100ms");
 advanceRepair();goal.x=.2f;queryStateRepair=wc::Result::Hit;q={0,.5f,0,.8660254f};solveRepair(goal,q);
 checkRepair(goal.x>=.18f-1e-6f&&goal.x<.2f&&std::fabs(q.y-.5f)<1e-6&&std::fabs(q.w-.8660254f)<1e-6,"current inward contact applies at most 20mm correction and retains controller rotation");
 checkRepair(originsRepair.size()==3,"three real-bound tool probes use the supplied world query");
 checkRepair(logWritesRepair==0,"F5 off performs contact without diagnostic formatting or writes");
 advanceRepair();goal.x=.2f;q={};distanceRepair=0;solveRepair(goal,q);
 checkRepair(std::fabs(goal.x-.2f)<1e-6,"zero-distance overlap cannot retain a full-wrist pin on withdrawal");
 advanceRepair();goal.x=.1f;distanceRepair=.01f;solveRepair(goal,q);
 checkRepair(std::fabs(goal.x-.12f)<1e-6,"positive current inward contact keeps a bounded normal response");
 advanceRepair();queryStateRepair=wc::Result::Unknown;goal.x=.2f;q={};solveRepair(goal,q);
 checkRepair(std::fabs(goal.x-.2f)<1e-6,"unknown query follows controller without sticking tool to an old result");
 resetRepair(1);goal={};q={};solveRepair(goal,q);advanceRepair();goal.y=.1f;solveRepair(goal,q);
 checkRepair(ironBoundsReadsRepair==1&&originsRepair.size()==3,"tongs contact follows the current workpiece bounds");
 checkRepair(std::fabs(originsRepair[0].x-.75f)<1e-6&&std::fabs(originsRepair[2].x-1.25f)<1e-6,
   "swept probe endpoints are on native iron, not the tongs or wrist");
 originsRepair.clear();advanceRepair();g_committed_frame6695.value.stamp.tick=nowRepair-81;goal.y=.2f;solveRepair(goal,q);
 checkRepair(originsRepair.empty()&&!g_smith_tool_contact_j27[0].have,"stale committed tool identity cannot authorize contact");
 resetRepair();goal={};solveRepair(goal,q);advanceRepair();++g_tool_socket59c10[0].value.request;advanceRepair();
 ++kcd2vr::workstation178r20c1::smithHoldEpoch36;
 goal.x=.2f;queryStateRepair=wc::Result::Hit;solveRepair(goal,q);
 checkRepair(originsRepair.empty()&&std::fabs(goal.x-.2f)<1e-6&&g_smith_tool_contact_j27[0].have,
   "new current native tool request rebases instead of reusing the old stopped wrist");
 advanceRepair();goal.x=std::numeric_limits<float>::quiet_NaN();solveRepair(goal,q);
 checkRepair(!g_smith_tool_contact_j27[0].have,"invalid controller goal retires the contact pose");
 for(int revoked=0;revoked<7;++revoked){
  resetRepair();goal={};q={};solveRepair(goal,q);advanceRepair();
  // Leave socket and committed frame fresh and mutually identical. Only the
  // authoritative current witness changes between the two controller poses.
  using namespace kcd2vr::activity_input163;
  switch(revoked){
   case 0:++kcd2vr::workstation178r20c1::smithHoldEpoch36;break;
   case 1:++kcd2vr::minigame_hands45::modeEpoch67;break;
   case 2:++witness175.lifetime175;break;
   case 3:witness175.active[unsigned(Kind175::Smithing)]=2;break;
   case 4:witness175.active[unsigned(Kind175::Smithing)]=0;break;
   case 5:witness175.ready=false;break;
   case 6:witness175.transitionDepth175=1;break;
  }
  goal.x=.2f;q={0,.5f,0,.8660254f};queryStateRepair=wc::Result::Hit;solveRepair(goal,q);
  checkRepair(originsRepair.empty()&&!g_smith_tool_contact_j27[0].have&&std::fabs(goal.x-.2f)<1e-6&&
    std::fabs(q.y-.5f)<1e-6,"fresh old packets cannot constrain the wrist after independent current-owner revocation");
 }
 currentOwnerRepair();solveRepair(goal,q);
 checkRepair(originsRepair.empty()&&g_smith_tool_contact_j27[0].have&&std::fabs(goal.x-.2f)<1e-6,
   "restored current owner starts at the tracked pose rather than recovering a revoked stopped pose");
 // Execute the actual world adapter on transaction-owned history, then reject
 // the pose after contact. Its unpublished movement must not seed the replay.
 resetRepair();goal={};q={};solveRepair(goal,q);
 {
  auto rejected=ReadSmithContactHistoryR38();advanceRepair();goal.x=.2f;
  SmithToolContactJ27(reinterpret_cast<void*>(0x1000),4,0,q,goal,&q,rejected.world);
  checkRepair(std::fabs(rejected.world[0].lastGoal.x-.2f)<1e-6&&g_smith_tool_contact_j27[0].lastGoal.x==0,
    "solved uncommitted world contact changes only the transaction copy");
  // The anvil solver uses the same exact production history bundle. A native
  // anvil query/solver is not fabricated in this fixture.
  rejected.anvil[0].have=rejected.anvil[0].corrected=true;
  rejected.anvil[0].lastGoal={.2f,0,0};rejected.anvil[0].lastQ={0,.5f,0,.8660254f};
 }
 checkRepair(!g_smith_anvil_contact_a2[0].have&&!g_smith_anvil_contact_a2[0].corrected,
   "rejected staged anvil recovery history never enters the published cache");
 originsRepair.clear();advanceRepair();auto replay=ReadSmithContactHistoryR38();goal.x=.3f;queryStateRepair=wc::Result::Hit;
 SmithToolContactJ27(reinterpret_cast<void*>(0x1000),4,0,q,goal,&q,replay.world);
 checkRepair(originsRepair.size()==3&&std::fabs(originsRepair[0].x)<1e-6&&std::fabs(goal.x-.28f)<1e-6,
   "next collision replay starts at the last published wrist, not the rejected solved wrist");
 originsRepair.clear();advanceRepair();auto published=ReadSmithContactHistoryR38();goal.x=.15f;queryStateRepair=wc::Result::Clear;
 SmithToolContactJ27(reinterpret_cast<void*>(0x1000),4,0,q,goal,&q,published.world);
 published.anvil[0].have=published.anvil[0].corrected=true;
 published.anvil[0].lastGoal={.15f,0,0};published.anvil[0].lastQ={0,.5f,0,.8660254f};
 CommitSmithContactHistoryR38(published);const auto next=ReadSmithContactHistoryR38();
 checkRepair(std::fabs(next.world[0].lastGoal.x-.15f)<1e-6&&next.anvil[0].corrected&&
   std::fabs(next.anvil[0].lastGoal.x-.15f)<1e-6&&std::fabs(next.anvil[0].lastQ.y-.5f)<1e-6,
   "successful publication commits the same world and anvil position/orientation histories together");
 resetRepair();boundsLiveRepair=false;goal={};q={};
 for(int i=0;i<5;++i){
  auto rejected=ReadSmithContactHistoryR38();
  SmithToolContactJ27(reinterpret_cast<void*>(0x1000),4,0,q,goal,&q,rejected.world);
  advanceRepair(20);
 }
 checkRepair(boundsReadsRepair==1&&!g_smith_tool_contact_j27[0].have,
   "five rejected poses preserve the failed native-bounds attempt throttle");
 boundsLiveRepair=true;auto retried=ReadSmithContactHistoryR38();
 SmithToolContactJ27(reinterpret_cast<void*>(0x1000),4,0,q,goal,&q,retried.world);
 checkRepair(boundsReadsRepair==2&&retried.world[0].boundsOk&&retried.world[0].have,
   "rejected-pose failed bounds retry becomes eligible after 100ms");
 resetRepair();goal={};q={};solveRepair(goal,q);const auto blocks=g_smith_tool_blocks_j27.load();
 kcd2vr::diagnostics::capture168::gate.select(kcd2vr::diagnostics::capture168::Domain::Interaction,true,nowRepair);
 for(int i=0;i<2;++i){
  advanceRepair();auto rejected=ReadSmithContactHistoryR38();goal.x=.2f;queryStateRepair=wc::Result::Hit;
  SmithToolContactJ27(reinterpret_cast<void*>(0x1000),4,0,q,goal,&q,rejected.world);
 }
 checkRepair(logWritesRepair==1&&originsRepair.size()==6&&g_smith_tool_blocks_j27.load()==blocks+2,
   "rejected poses preserve diagnostic rate limits and do not undo actual query/block attempts");
 resetRepair(1);goal={};q={};solveRepair(goal,q);advanceRepair();ironEntityRepair=78;boundsLiveRepair=false;
 {
  auto rejected=ReadSmithContactHistoryR38();goal.y=.1f;
  SmithToolContactJ27(reinterpret_cast<void*>(0x1000),4,0,q,goal,&q,rejected.world);
  checkRepair(!rejected.world[0].ironOk&&g_smith_tool_contact_j27[0].ironOk&&g_smith_tool_contact_j27[0].ironId==77,
    "replacement iron bounds failure stays uncommitted while recording its failed attempt");
 }
 const auto readsAfterReplacement=boundsReadsRepair;advanceRepair();auto throttled=ReadSmithContactHistoryR38();goal.y=.2f;
 SmithToolContactJ27(reinterpret_cast<void*>(0x1000),4,0,q,goal,&q,throttled.world);
 checkRepair(boundsReadsRepair==readsAfterReplacement&&originsRepair.empty()&&!throttled.world[0].ironOk&&
   throttled.world[0].ironId==78&&std::fabs(goal.y-.2f)<1e-6,
   "throttled replacement iron never borrows old committed workpiece bounds after rejection");
 resetRepair(1);goal={};q={};solveRepair(goal,q);advanceRepair();ironAttachedRepair=false;
 goal.y=.2f;queryStateRepair=wc::Result::Hit;solveRepair(goal,q);
 checkRepair(originsRepair.empty()&&!g_smith_tool_contact_j27[0].have&&std::fabs(goal.y-.2f)<1e-6,
   "native detached workpiece immediately retires held-tongs world contact without a clamp");
 std::printf("smith_tool_contact_repair: %u checks PASS\n",checksRepair);
 return 0;
}
#endif
