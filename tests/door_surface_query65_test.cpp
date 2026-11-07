// Actual post-commit surface publisher; only native world/body leaves and
// already completed pose publication are doubles. No live engine claim.
#include "../src/core/door_surface65.h"
#include "../src/core/door_use46.h"
#include "../src/vrik/contact_budget.h"
#include "../src/vrik/pose_math.h"
#include <atomic>
#include <cassert>
#include <cstdio>
namespace fixturePose65=kcd2vr::pose_core;
namespace door_use46=kcd2vr::door_use46;
struct V3 {float x=0,y=0,z=0;};using Q4=fixturePose65::Quat;
static V3 QRotateV(Q4 q,V3 p){const auto v=fixturePose65::rotate(q,fixturePose65::Vec3{p.x,p.y,p.z});return {float(v.x),float(v.y),float(v.z)};}
static V3 Mul(V3 p,float s){return {p.x*s,p.y*s,p.z*s};}
static float Len(V3 p){return float(fixturePose65::length(fixturePose65::Vec3{p.x,p.y,p.z}));}
struct CommittedFrame6695 {fixturePose65::Stamp stamp{};float world[8]{0,0,0,1,10,20,30,1};
 bool valid[2]{true,true},palmValid59c9[2]{true,true};V3 palm59c9[2]{},palmDirection59c9[2]{{0,1,0},{0,1,0}};std::uintptr_t doorPlayer38=0x1234;};
struct ModelWorld6692 {Q4 q{};V3 t{10,20,30};float scale=1;};
static ModelWorld6692 nativeWorld;static CommittedFrame6695 published;static unsigned modelReads65=0,packetReads65=0;
static bool ReadModelWorld6692(void*,ModelWorld6692& p){++modelReads65;p=nativeWorld;return true;}
static V3 ModelToWorld6692(const ModelWorld6692& w,V3 p){p=QRotateV(w.q,Mul(p,w.scale));return {p.x+w.t.x,p.y+w.t.y,p.z+w.t.z};}
static bool ReadCommittedPacket6695(CommittedFrame6695& p){++packetReads65;p=published;return true;}
static std::uint64_t now65=1000;static std::uint64_t GetTickCount64(){return now65;}
namespace tracking59c6 {static std::atomic<std::uint64_t> epoch{9};}
static std::atomic<std::uint64_t> g_pose_epoch6689{7};static std::atomic<void*> g_last_player_entity{reinterpret_cast<void*>(0x1234)};
static std::atomic<bool> g_gameplay_allowed{true},g_ctx_menu{false},g_ctx_dialogue{false},g_ctx_cutscene{false},g_ctx_reading{false};
static std::atomic<float> g_right_grip{1},g_left_grip{1};
namespace animation_filter {static std::atomic<bool> nativeBodyOwned112{false};}
namespace diagnostics::lab {static bool NativeReference(){return false;}}
static bool CalibrationReady(){return true;}static bool TrackingHeadUsable111(std::uint64_t){return true;}
static bool TrackingHandUsable111(int,std::uint64_t){return true;}static int ExactHandItemState(int){return 0;}
namespace interaction_alignment {
 static constexpr std::uint64_t acquisitionTag65=std::uint64_t(1)<<63;
 static std::uint64_t generation=acquisitionTag65|1;static bool legacyRay65=true;static unsigned legacyReads65=0;
 static std::uint64_t DoorSourceGeneration38(){++legacyReads65;return legacyRay65?1:0;}
 static std::uint64_t DoorAcquisitionGeneration65(){return generation;}
}
namespace workstation178r20c1 {static std::atomic<std::uint64_t> experimentEpoch36{2};}
struct NpcQuery172 {bool requested=false;int marker=7;};static NpcQuery172 g_npc_query172;
static kcd2vr::contact::Budget g_contact_budget6695;
static bool g_contact_budget_active6695=false,g_contact_pass6695=false;
static std::uint64_t g_contact_budget_sample65=0;
namespace wc {enum class Result {Hit,Clear,Busy};}
struct WorldRayResult6693 {wc::Result state=wc::Result::Hit;bool plane=true;float distance=.1f;V3 point{},normal{0,-1,0};
 struct {bool ownerRead=true;unsigned entityId=42;std::uintptr_t owner=0xABCD,collider=0xEF01;}native172;};
static unsigned calls65=0,fault65=0;static V3 lastOrigin65,lastSweep65;
static WorldRayResult6693 WorldRayDistance6692(V3 origin,V3 sweep){
 WorldRayResult6693 h{};h.state=wc::Result::Busy;
 assert(g_contact_budget_active6695&&g_contact_pass6695&&g_npc_query172.requested);
 if(!g_contact_budget6695.take(10))return h;
 ++calls65;lastOrigin65=origin;lastSweep65=sweep;g_contact_budget6695.finish(11);h.state=wc::Result::Hit;
 h.point={origin.x+sweep.x*.5f,origin.y+sweep.y*.5f,origin.z+sweep.z*.5f};h.normal=Mul(sweep,-5.f);
 if(fault65>=20){
  // Actual plane x=origin.x+distance. Tangential axes are CLEAR, so a
  // successful fallback must really cross the plane within its20cm segment.
  const float d=fault65==22?.21f:.10f;
  if(sweep.x<=0||sweep.x<d){h.state=wc::Result::Clear;return h;}
  const float t=d/sweep.x;h.point={origin.x+d,origin.y+sweep.y*t,origin.z+sweep.z*t};
  h.normal={-1,0,0};h.distance=Len({sweep.x*t,sweep.y*t,sweep.z*t});
  if(fault65==21)h.native172.entityId=99;
  return h;
 }
 if(fault65==1)h.native172.entityId=99;if(fault65==2)h.plane=false;if(fault65==3)h.native172.ownerRead=false;
 if(fault65==4){h.distance=.21f;h.point={origin.x,origin.y+.21f,origin.z};}
 if(fault65==5)++interaction_alignment::generation;if(fault65==6)g_left_grip=g_right_grip=0;
 if(fault65==7)nativeWorld.t.x+=.1f;if(fault65==8)tracking59c6::epoch=10;
 if(fault65==9)++workstation178r20c1::experimentEpoch36;if(fault65==10)door_use46::SetEnabled(false);
 if(fault65==11)g_ctx_menu=true;if(fault65==12)g_pose_epoch6689=8;
 if(fault65==13)h.state=wc::Result::Clear;if(fault65==14)h.state=wc::Result::Busy;
 return h;
}
#include "../src/vrik/door_surface_budget65.inl"
#include "../src/vrik/door_surface_query65.inl"
static unsigned checks=0;static void checkAt(bool b,unsigned line){++checks;if(!b)std::fprintf(stderr,"door surface check%u line%u failed\n",checks,line);assert(b);}
#define check(b) checkAt(bool(b),__LINE__)
static kcd2vr::door_surface65::Request request65(int side=0){
 using namespace kcd2vr::door_surface65;Request r{};r.target=42;r.side=side;r.press=980;r.tick=r.poseTick=now65;
 r.poseEpoch=7;r.trackingEpoch=9;r.sourceGeneration=interaction_alignment::DoorAcquisitionGeneration65();r.settingsEpoch=door_use46::settingsEpoch.load();r.experimentEpoch=2;
 r.player=0x1234;r.palm={10,20,30};r.nativePoint={10,20,30}; return r; // degenerate centre retains the anatomical-palm direction
}
static void reset65(unsigned sample){
 using namespace kcd2vr::door_surface65;requestMailbox.publish({});witnessMailbox.publish({});calls65=fault65=modelReads65=packetReads65=0;
 door_use46::SetEnabled(true);g_pose_epoch6689=7;tracking59c6::epoch=9;interaction_alignment::generation=interaction_alignment::acquisitionTag65|1;interaction_alignment::legacyRay65=true;interaction_alignment::legacyReads65=0;
 g_gameplay_allowed=true;g_ctx_menu=g_ctx_dialogue=g_ctx_cutscene=g_ctx_reading=false;g_left_grip=g_right_grip=1;
 workstation178r20c1::experimentEpoch36=2;nativeWorld={};published={};published.stamp={7,sample,now65,0x5678};
 g_contact_budget_sample65=0;g_contact_budget_active6695=g_contact_pass6695=false;g_npc_query172={};g_door_surface_credits65={};
 kcd2vr::door_hold65::Frame f{};
 for(int side=0;side<2;++side){const auto r=request65(side);auto& h=f.hand[side];
  h.available=true;h.side=side;h.grip=1;h.tick=h.poseTick=now65;h.press=r.press;h.player=r.player;
  h.poseEpoch=r.poseEpoch;h.trackingEpoch=r.trackingEpoch;h.sourceGeneration=r.sourceGeneration;
  h.settingsEpoch=r.settingsEpoch;h.experimentEpoch=r.experimentEpoch;h.point=r.palm;}
 kcd2vr::door_hold65::frame.publish(f);
}
int main(){using namespace kcd2vr::door_surface65;
 reset65(1);PublishDoorSurface65(published,9);check(calls65==0&&modelReads65==0&&packetReads65==0); // no candidate: no new native/model reads
 check(offer(request65(),now65));check(offer(request65(1),now65));PublishDoorSurface65(published,9);
 check(calls65==2&&g_contact_budget6695.used==2&&g_contact_budget6695.remaining==16);
 Witnesses table{};check(witnessMailbox.read(table));unsigned count=0;for(const auto& w:table.value)count+=w.valid;
 check(count==2);check(lastOrigin65.x==10&&lastOrigin65.y==20&&std::fabs(lastSweep65.y-.2f)<1e-6);
 check(!g_contact_budget_active6695&&!g_contact_pass6695&&!g_npc_query172.requested&&g_npc_query172.marker==7);
 PublishDoorSurface65(published,9);check(calls65==2); // repeated eyes/skin callback cannot repeat a query
 for(unsigned fault=1;fault<=12;++fault){reset65(10+fault);fault65=fault;check(offer(request65(),now65));
  PublishDoorSurface65(published,9);check(calls65==(fault<=4?7u:1u));table={};witnessMailbox.read(table);
  count=0;for(const auto& w:table.value)count+=w.valid;check(count==0);
 }
 reset65(40);check(offer(request65(),now65));check(offer(request65(1),now65));
 g_contact_budget_sample65=40;g_contact_budget6695.begin_work();g_contact_budget6695.remaining=1;g_contact_budget6695.used=17;g_contact_budget6695.spent=900;
 PublishDoorSurface65(published,9);check(calls65==1&&g_contact_budget6695.used==18&&g_contact_budget6695.spent==901);
 reset65(41);check(offer(request65(),now65));g_contact_budget_sample65=41;g_contact_budget6695.begin_work();g_contact_budget6695.spent=1000;
 PublishDoorSurface65(published,9);check(calls65==0&&g_contact_budget6695.spent==1000);
 reset65(42);auto stale=request65();stale.trackingEpoch=2;check(offer(stale,now65));PublishDoorSurface65(published,9);check(calls65==0);
 reset65(43);check(offer(request65(),now65));auto old=published;published.world[4]+=.1f;PublishDoorSurface65(old,9);check(calls65==0);
 reset65(44);check(offer(request65(),now65));
 nativeWorld.q={0,0,std::sqrt(.5),std::sqrt(.5)};nativeWorld.scale=2;
 published.world[2]=published.world[3]=float(std::sqrt(.5));published.world[7]=2;published.palm59c9[0]={.1f,0,0};
 {auto r=request65();r.nativePoint={10,20.2f,30};check(offer(r,now65));}
 PublishDoorSurface65(published,9);check(calls65==1);
 check(std::fabs(lastOrigin65.x-10)<1e-5&&std::fabs(lastOrigin65.y-20.2f)<1e-5);
 check(std::fabs(lastSweep65.x+.2f)<1e-5&&std::fabs(lastSweep65.y)<1e-5); // same world rotation, ray remains20cm
 // A known new miss must withdraw the successful previous sample even
 // while that old point remains within20cm and its80ms lifetime.
 for(unsigned fault: {1u,13u,14u}){reset65(50+fault*2);check(offer(request65(),now65));
  PublishDoorSurface65(published,9);table={};check(witnessMailbox.read(table));
  count=0;for(const auto& w:table.value)count+=w.valid;check(count==1);
  ++published.stamp.sample;fault65=fault;PublishDoorSurface65(published,9);
  table={};check(witnessMailbox.read(table));count=0;for(const auto& w:table.value)count+=w.valid;
  check(count==0&&calls65==(fault==14?2u:8u));
 }
 // Generic Trigger/ray withdrawal cannot deadlock raw held-door acquisition.
 // Real acquisition-policy retirement still rejects the request before reads.
 reset65(90);interaction_alignment::legacyRay65=false;check(offer(request65(),now65));
 PublishDoorSurface65(published,9);check(calls65==1&&interaction_alignment::legacyReads65==0);
 table={};check(witnessMailbox.read(table));count=0;for(const auto& w:table.value)count+=w.valid;check(count==1);
 reset65(91);check(offer(request65(),now65));++interaction_alignment::generation;
 PublishDoorSurface65(published,9);check(calls65==0&&modelReads65==0&&packetReads65==0);
 // Existing ContactPass begin: reserve copied-only credits, let optional
 // hand/arm work consume every unreserved ray, then final same-sample query.
 reset65(100);BeginContactBudget65(100,now65);check(g_contact_budget6695.remaining==18&&g_door_surface_credits65.count==0);
 check(modelReads65==0&&packetReads65==0&&calls65==0);
 for(unsigned n=0;n<18;++n){check(g_contact_budget6695.take(10));g_contact_budget6695.finish(11);}
 check(!g_contact_budget6695.take(10)&&g_contact_budget6695.used==18); // old quiet cap unchanged
 reset65(101);check(offer(request65(),now65));check(offer(request65(1),now65));
 BeginContactBudget65(101,now65);
 check(g_contact_budget6695.remaining==4&&g_door_surface_credits65.count==14);
 for(unsigned n=0;n<4;++n){check(g_contact_budget6695.take(10));g_contact_budget6695.finish(11);}
 check(!g_contact_budget6695.take(10)&&g_contact_budget6695.used==4);
 const auto beforeSpent65=g_contact_budget6695.spent;BeginContactBudget65(101,now65);
 check(g_contact_budget6695.used==4&&g_contact_budget6695.remaining==0&&g_contact_budget6695.spent==beforeSpent65&&g_door_surface_credits65.count==14);
 PublishDoorSurface65(published,9);check(calls65==2&&g_contact_budget6695.used==6&&g_contact_budget6695.remaining==12);
 BeginContactBudget65(101,now65);check(g_contact_budget6695.used==6&&g_contact_budget6695.remaining==12);
 ReturnDoorSurfaceCredits65(101);check(g_contact_budget6695.remaining==12); // exact credits returned once only
 table={};witnessMailbox.read(table);count=0;for(const auto& w:table.value)count+=w.valid;check(count==2);
 reset65(102);check(offer(request65(),now65));BeginContactBudget65(102,now65);check(g_contact_budget6695.remaining==11&&g_door_surface_credits65.count==7);
 g_contact_budget6695.spent=1000;PublishDoorSurface65(published,9);
 check(calls65==0&&g_contact_budget6695.spent==1000&&g_contact_budget6695.used==0&&g_contact_budget6695.remaining==18);
 reset65(103);check(offer(request65(),now65));++interaction_alignment::generation;BeginContactBudget65(103,now65);
 check(g_contact_budget6695.remaining==18&&g_door_surface_credits65.count==0&&modelReads65==0&&packetReads65==0);
 reset65(104);check(offer(request65(),now65));BeginContactBudget65(104,now65);check(g_door_surface_credits65.count==7);
 g_contact_budget_sample65=999; // unrelated/new budget cannot inherit the old pass's credit
 PublishDoorSurface65(published,9);check(calls65==1&&g_contact_budget6695.used==1&&g_contact_budget6695.remaining==17);
 reset65(105);check(offer(request65(),now65));BeginContactBudget65(105,now65);check(g_door_surface_credits65.count==7);
 ++interaction_alignment::generation;PublishDoorSurface65(published,9);
 check(calls65==0&&modelReads65==0&&packetReads65==0); // policy retires after credit reservation
 BeginContactBudget65(106,now65);check(g_door_surface_credits65.count==0&&g_contact_budget6695.remaining==18);
 reset65(107);BeginContactBudget65(107,now65);g_door_surface_credits65={107,1};
 ReturnDoorSurfaceCredits65(107);check(g_contact_budget6695.remaining==18); // injected credit cannot mint19th ray
 // H23 functional orientation coverage: a palm pointing parallel to the
 // door must acquire its actual nearest physical plane on a bounded fallback.
 reset65(200);fault65=20;check(offer(request65(),now65));
 PublishDoorSurface65(published,9);check(calls65==2&&g_contact_budget6695.used==2);
 table={};check(witnessMailbox.read(table));count=0;for(const auto& w:table.value)if(w.valid){
  ++count;check(w.entityId==42&&std::fabs(w.point.x-10.1f)<1e-5f&&w.distance<=.20f);
 }
 check(count==1);PublishDoorSurface65(published,9);check(calls65==2);
 // The same occluded native physics query returning another entity must not
 // grant a door witness; a plane beyond20cm must not become reach assistance.
 for(unsigned fault:{21u,22u}){reset65(201+fault);fault65=fault;check(offer(request65(),now65));
  PublishDoorSurface65(published,9);check(calls65==7&&g_contact_budget6695.used==7);
  table={};witnessMailbox.read(table);count=0;for(const auto& w:table.value)count+=w.valid;check(count==0);
 }
 // One query set serves every current native door nominee. A first unrelated
 // candidate cannot consume all probes before the physically hit door is joined.
 reset65(240);auto other=request65();other.target=99;check(offer(other,now65));check(offer(request65(),now65));
 PublishDoorSurface65(published,9);check(calls65==1);
 table={};witnessMailbox.read(table);count=0;for(const auto& w:table.value)if(w.valid){++count;check(w.entityId==42);}
 check(count==1);
 reset65(250);fault65=20;check(offer(request65(),now65));g_contact_budget_sample65=250;
 g_contact_budget6695.begin_work();g_contact_budget6695.remaining=1;g_contact_budget6695.used=17;
 PublishDoorSurface65(published,9);check(calls65==1&&g_contact_budget6695.used==18);
 table={};witnessMailbox.read(table);count=0;for(const auto& w:table.value)count+=w.valid;check(count==0);
 // H24 native centre is WORLD. Under translated/rotated/scaled Henry it
 // steers the actual physical query without a second ModelToWorld transform.
 reset65(300);nativeWorld.q={0,0,std::sqrt(.5),std::sqrt(.5)};nativeWorld.scale=2;
 published.world[2]=published.world[3]=float(std::sqrt(.5));published.world[7]=2;published.palm59c9[0]={.1f,0,0};
 auto worldHint=request65();worldHint.nativePoint={10.10f,20.30f,30.0f};check(offer(worldHint,now65));
 PublishDoorSurface65(published,9);check(calls65==1);
 check(std::fabs(lastOrigin65.x-10)<1e-5&&std::fabs(lastOrigin65.y-20.2f)<1e-5&&lastOrigin65.z==30);
 check(std::fabs(lastSweep65.x-.14142136f)<2e-5&&std::fabs(lastSweep65.y-.14142136f)<2e-5&&std::fabs(lastSweep65.z)<1e-6);
 check(std::fabs(Len(lastSweep65)-.20f)<1e-6);
 // Descriptor hints do not grant proximity/ownership: every foreign or CLEAR
 // physical query remains refused even if the descriptor point touches palm.
 for(unsigned fault:{1u,13u}){reset65(301+fault);fault65=fault;auto hint=request65();hint.nativePoint={10.05f,20.05f,30};check(offer(hint,now65));
  PublishDoorSurface65(published,9);check(calls65==7);table={};witnessMailbox.read(table);count=0;for(const auto& w:table.value)count+=w.valid;check(count==0);}
 // D45: a first-draw burst cannot spend withheld door credits or renew time.
 reset65(400);check(offer(request65(),now65));BeginContactBudget65(400,now65);
 check(g_contact_budget6695.remaining==11&&g_door_surface_credits65.count==7);
 {kcd2vr::contact::AcquisitionBurstD45 burst(g_contact_budget6695,true);check(burst.granted);
  for(unsigned n=0;n<36;++n){check(g_contact_budget6695.take(10));g_contact_budget6695.finish(11);}}
 check(g_contact_budget6695.used==36&&g_contact_budget6695.remaining==0&&g_contact_budget6695.capacity==43);
 {kcd2vr::contact::AcquisitionBurstD45 again(g_contact_budget6695,true);check(!again.granted);}
 PublishDoorSurface65(published,9);check(calls65==1&&g_contact_budget6695.used==37&&g_contact_budget6695.remaining==6&&g_contact_budget6695.spent==37);
 ReturnDoorSurfaceCredits65(400);check(g_contact_budget6695.remaining==6);
 reset65(401);BeginContactBudget65(401,now65);
 {kcd2vr::contact::AcquisitionBurstD45 burst(g_contact_budget6695,true);
  check(g_contact_budget6695.take(10));g_contact_budget6695.finish(11);}
 check(g_contact_budget6695.used==1&&g_contact_budget6695.remaining==17&&g_contact_budget6695.capacity==18);
 reset65(402);BeginContactBudget65(402,now65);g_contact_budget6695.spent=1000;
 {kcd2vr::contact::AcquisitionBurstD45 burst(g_contact_budget6695,true);check(!burst.granted&&!g_contact_budget6695.take(10));}
 std::printf("door_surface_query65: %u checks PASS; actual post-commit publisher, native query leaves doubled\n",checks);
}

