// Compiles the actual complete CG1/W5 native drag adapter and native part
// picker. Only engine/status/input publication leaves are doubled. No copied
// drag state machine or hand-to-part math is implemented in this fixture.
#include <vrik/lab58.h>
#include "subsystems/physics/corpse_search_budget_d85r5r2.h"
#include "core/observed_load_recovery_r96.h"
#include <vrik/part_anchor59c7g.h>
#include <vrik/prop_drive5916c1.h>
#include <core/collab_test_h21.h>
#include <functional>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

namespace ph=kcd2vr::physical;
namespace cg=kcd2vr::corpse_grip_cg1;
namespace pa=kcd2vr::partanchor59c7g;
static std::uint64_t clockD40=1000;
static std::uint64_t GetTickCount64(){return clockD40;}
struct TrackingSnapshot {bool head_valid=true,head_held=false,left_valid=true,left_held=false,right_valid=true,right_held=false;
 std::uint64_t tick=1000,input_tick=1000,source_epoch=1;};
static TrackingSnapshot trackingD40;
static bool publishWhileReadingR96=false;
static bool ReadTrackingSnapshot(TrackingSnapshot& out){if(publishWhileReadingR96){++clockD40;trackingD40.tick=trackingD40.input_tick=clockD40;publishWhileReadingR96=false;}out=trackingD40;return true;}
namespace kcd2vr::tracking59c6 {inline std::atomic<std::uint64_t> epoch{1};}
static std::atomic<bool> g_gameplay_allowed{true},g_ctx_menu{false},g_ctx_dialogue{false},g_ctx_cutscene{false};
static std::atomic<std::uint64_t> g_pose_epoch6689{1};
static std::atomic<void*> g_last_player_entity{reinterpret_cast<void*>(0x9000)};
namespace animation_filter {static std::atomic<bool> nativeBodyOwned112{false};}
struct PhysFrame6698 {struct {std::uint64_t tick=1000;bool tracked[2]{true,true};float grip[2]{};} input;};
static kcd2vr::core::SnapshotMailbox<PhysFrame6698> g_phys_frame6698;
static int equipmentD40[2]{};
static bool heldVisualD40=false;
struct Matrix34V6550 {float m[12]{};};
static bool SmallPickupBoundsR24(void*,void*&,float*){return false;}
static bool GetEntityWorldTmV6552(void*,std::uint32_t,Matrix34V6550&){return false;}
static int heldVisualSideD78=0;
static int ExactHandItemState(int side){return equipmentD40[side];}
static bool HeldInteractionVisual43(){return heldVisualD40;}
static bool HeldInteractionVisual43(int side){return heldVisualD40&&side==heldVisualSideD78;}
static ph::Vec3 handsD40[2]{{.2,0,0},{0,0,.2}};
static bool handValidD40[2]{true,true};
static ph::Vec3 rawOffsetD78[2]{};static unsigned displayedReadsD78=0;
static bool ReadInteractionHandWorld123R1(int side,float& x,float& y,float& z){
 ++displayedReadsD78;x=float(handsD40[side].x);y=float(handsD40[side].y);z=float(handsD40[side].z);return handValidD40[side];}
namespace diagnostics::capture168 {enum class Domain{Combat};static bool active(Domain,std::uint64_t){return false;}}
static std::vector<std::string> linesD40;
namespace diagnostic_trace {static void Write(const char* text){linesD40.emplace_back(text);}
static void WriteFor(kcd2vr::diagnostics::capture168::Domain,const char* text){Write(text);}}
namespace kcd2vr::visible_failure {static bool publish(const char*,std::uint64_t){return true;}}
namespace kcd2vr::hand_only_door_h40 {struct Admission {bool take(unsigned){return true;}};}

struct NativePartD40 {int id=379;ph::Pose pose{};double scale=1;std::uintptr_t geometry=0x4000,proxy=0x5000;};
struct NativeBodyD40 {std::uint32_t id=44;std::uintptr_t entity=0x1000,root=0x2000;bool dead=true,articulated=true,parent=false;
 std::vector<NativePartD40> parts{{}};};
static std::vector<NativeBodyD40> bodiesD40;
static std::function<void()> statusCallbackD40;
static int statusCallsD40=0;
static NativeBodyD40* byIdD40(std::uint32_t id){for(auto& b:bodiesD40)if(b.id==id)return &b;return nullptr;}
static void* FlinchEntityById178r11(std::uint32_t id){auto* b=byIdD40(id);return b?reinterpret_cast<void*>(b->entity):nullptr;}
namespace combat_diag_probe {
static unsigned LabDeathCandidates58(std::uint32_t* ids,unsigned cap,std::uint64_t){
 unsigned n=0;for(const auto& b:bodiesD40)if(b.dead&&n<cap)ids[n++]=b.id;return n;}
static bool LabCorpseQualified58(std::uint32_t id,std::uintptr_t identity,std::uint64_t epoch,std::uint64_t,const char*& why){
 const auto* b=byIdD40(id);const bool ok=b&&b->dead&&!b->parent&&b->entity==identity&&epoch==1;
 why=ok?"":"NATIVE_DEATH_OR_OWNER_UNKNOWN";return ok;}
}
static kcd2vr::impale47::Poll ImpaleRoot47(const pa::Identity& id,std::uintptr_t& root,const char*& why,bool){
 auto* b=byIdD40(id.target);if(!b||b->entity!=id.entity||!b->articulated){root=0;why="WRONG_ROOT";return kcd2vr::impale47::Poll::Fatal;}
 root=b->root;return kcd2vr::impale47::Poll::Ready;}
static bool PartStatus47(std::uintptr_t root,pa::Status& out){
 ++statusCallsD40;
 for(auto& b:bodiesD40)if(b.root==root){
  const NativePartD40* part=nullptr;
  if(out.index>=0){if(unsigned(out.index)<b.parts.size())part=&b.parts[unsigned(out.index)];}
  else for(const auto& p:b.parts)if(p.id==out.part){part=&p;break;}
  if(!part)return false;
  out.part=part->id;out.position[0]=float(part->pose.p.x);out.position[1]=float(part->pose.p.y);out.position[2]=float(part->pose.p.z);
  out.rotation[0]=float(part->pose.q.x);out.rotation[1]=float(part->pose.q.y);out.rotation[2]=float(part->pose.q.z);out.rotation[3]=float(part->pose.q.w);
  out.scale=float(part->scale);out.geometry=part->geometry;out.proxyGeometry=part->proxy;
  if(statusCallbackD40){auto callback=std::move(statusCallbackD40);statusCallbackD40={};callback();}
  return true;
 }
 return false;
}
struct DriveD40 {std::uintptr_t body;int part;kcd2vr::gore32::V3 velocity;};
static std::vector<DriveD40> drivesD40;
static bool acceptDriveD40=true,throwDriveD40=false;
namespace gore32n {
static std::atomic<bool> g_drive_fault33{false};
static bool PartVelocity(std::uintptr_t body,int part,kcd2vr::gore32::V3 velocity){
 drivesD40.push_back({body,part,velocity});if(throwDriveD40)g_drive_fault33=true;return acceptDriveD40&&!throwDriveD40;}
}
#include <corpse_drag_part_picker_d40.inc>
namespace lab58n {
namespace L=kcd2vr::lab58;
// Only the saved-provider presence and raw committed-pose transport are leaves
// here; the companion saved-service fixture executes its actual raw reader.
static bool SavedDragOwnsD72(int) noexcept {return false;}
static void SavedWatchdogD72(std::uint64_t) noexcept {}
static bool SavedHandIntentD72(int side,float& x,float& y,float& z) noexcept {
 const auto raw=handsD40[side]+rawOffsetD78[side];x=float(raw.x);y=float(raw.y);z=float(raw.z);return handValidD40[side];
}
#include <corpse_drag_native_d40.inc>
}
static cg::Gate gatesD40[2];
static unsigned checksD40=0,failedD40=0;
static void checkD40(bool ok,const char* text){++checksD40;if(!ok){++failedD40;std::printf("FAIL %s\n",text);}}
static double speedD40(const DriveD40& d){return kcd2vr::gore32::len(d.velocity);}
static void resetD40(){
 lab58n::RetireUnservicedCg1(clockD40);
 clockD40=1000;trackingD40={};g_pose_epoch6689=1;kcd2vr::tracking59c6::epoch=1;
 g_gameplay_allowed=true;g_ctx_menu=g_ctx_dialogue=g_ctx_cutscene=false;animation_filter::nativeBodyOwned112=false;
 kcd2vr::lab58::rung=3;kcd2vr::lab58::corpseDrag=true;kcd2vr::lab58::f9Rung=3;kcd2vr::lab58::uiBlocked=false;
 kcd2vr::collab_h21::setPhysicsMode(3);
 bodiesD40={NativeBodyD40{}};statusCallbackD40={};statusCallsD40=0;drivesD40.clear();linesD40.clear();
 acceptDriveD40=true;throwDriveD40=false;gore32n::g_drive_fault33=false;lab58n::g_dragFault=false;
 rawOffsetD78[0]=rawOffsetD78[1]={};displayedReadsD78=0;
 handsD40[0]={.2,0,0};handsD40[1]={0,0,.2};heldVisualD40=false;heldVisualSideD78=0;
 for(int s=0;s<2;++s){
  equipmentD40[s]=0;handValidD40[s]=true;gatesD40[s]={};lab58n::g_dragIdleTickD85R5R2[s]=0;
  lab58n::g_drag[s]={};lab58n::g_dragRoot[s]=0;lab58n::g_dragPressCg1[s]=0;lab58n::g_dragSearch[s]=0;lab58n::g_dragDiag[s]={};
  lab58n::g_dragPendingCg1[s]={};lab58n::g_dragPendingRootCg1[s]=0;lab58n::g_dragInputCg1[s].side=s;lab58n::g_dragInputCg1[s].clear();
  cg::claims.generation[s]=1;cg::claims.livePress[s]=0;cg::claims.consumed[s]=false;cg::claims.nativeHeld[s]=false;cg::claims.fault[s]=false;
  cg::handRequests[s].publish({});
 }
}
static void frameD40(int side,float grip,std::uint64_t elapsed=16){
 clockD40+=elapsed;trackingD40.tick=trackingD40.input_tick=clockD40;
 PhysFrame6698 f{};f.input.tick=clockD40;f.input.grip[side]=grip;g_phys_frame6698.publish(f);
 cg::Candidate candidate{};cg::handCandidates[side].read(candidate);
 cg::updateShared<kcd2vr::hand_only_door_h40::Admission>(gatesD40[side],side,clockD40,true,true,grip,candidate,cg::handRequests[side]);
 lab58n::DragTickSide(side,clockD40);
}
static void grabD40(int side=0){frameD40(side,0);frameD40(side,1);checkD40(lab58n::g_drag[side].held,"real worker squeeze enters native corpse owner");}
int main(){
 // The other hand's exact native grasp does not occupy this hand. The same
 // hand and unknown native item state still refuse before any body write.
 for(int side=0;side<2;++side){
  resetD40();heldVisualD40=true;heldVisualSideD78=1-side;grabD40(side);
  checkD40(lab58n::g_drag[side].held,"other-hand native grasp preserves corpse hand");
  resetD40();heldVisualD40=true;heldVisualSideD78=side;frameD40(side,0);frameD40(side,1);
  checkD40(!lab58n::g_drag[side].held&&drivesD40.empty(),"same-hand native grasp refuses corpse acquisition");
  resetD40();equipmentD40[side]=-1;frameD40(side,0);frameD40(side,1);
  checkD40(!lab58n::g_drag[side].held&&drivesD40.empty(),"unknown hand item state refuses corpse acquisition");
 }
 // D78: engine contact wrist does not move; only raw controller intent moves.
 // Execute the actual native articulated adapter for either hand, retain the
 // selected entity/part, and release without a final write.
 for(int side=0;side<2;++side){
  resetD40();handsD40[side]={.6,0,0};rawOffsetD78[side]={-.5,0,0};
  frameD40(side,0);frameD40(side,1);
  if(!lab58n::g_drag[side].held){std::puts("FAIL D78 articulated raw acquisition while contact wrist is blocked");return 1;}
  const auto entity=lab58n::g_drag[side].entity;const auto part=lab58n::g_drag[side].part;
  rawOffsetD78[side]={-.4,0,0};frameD40(side,1);
  checkD40(drivesD40.size()==1&&std::abs(drivesD40.back().velocity.x-.6)<1e-5,"raw hand drives retained part while collision wrist remains fixed");
  checkD40(lab58n::g_drag[side].entity==entity&&lab58n::g_drag[side].part==part&&displayedReadsD78==0,"same corpse and part retained with no post-contact wrist feedback");
  const auto writes=drivesD40.size();frameD40(side,0);
  checkD40(!lab58n::g_drag[side].held&&drivesD40.size()==writes,"either-hand raw controller release has no final velocity write");
 }

 resetD40();grabD40();frameD40(0,1);
 checkD40(drivesD40.size()==1&&speedD40(drivesD40.back())<1e-6,"stationary off-center grip does not pull the part origin into the hand");
 frameD40(0,.4f);checkD40(lab58n::g_drag[0].held,"relaxed .4 grip retains the worker's .30 release hysteresis");
 resetD40();grabD40();const double h=std::sqrt(.5);bodiesD40[0].parts[0].pose.q={0,0,h,h};handsD40[0]={0,.2,0};frameD40(0,1);
 checkD40(lab58n::g_drag[0].held&&drivesD40.size()==1&&speedD40(drivesD40.back())<1e-6,"retained point rotates with the native part, without origin snap");
 resetD40();grabD40();handsD40[0].x+=.1;frameD40(0,1);
 checkD40(drivesD40.size()==1&&std::abs(drivesD40.back().velocity.x-.6)<1e-6,"hand translation drives error from retained grip point");
 const auto calls=drivesD40.size();frameD40(0,0);
 checkD40(!lab58n::g_drag[0].held&&!cg::claims.nativeHeld[0]&&drivesD40.size()==calls,"real release retires hand ownership without changing body momentum");
 resetD40();handsD40[0]={0,0,0};grabD40();for(int i=0;i<360;++i)frameD40(0,1,60);
 checkD40(lab58n::g_drag[0].held,"current native hold lasts beyond the old arbitrary twenty seconds");
 for(int change=0;change<5;++change){
  resetD40();grabD40();
  if(change==0)bodiesD40[0].root+=0x100;
  if(change==1)bodiesD40[0].parts[0].id++;
  if(change==2)bodiesD40[0].parts[0].geometry++;
  if(change==3)bodiesD40[0].parts[0].scale=1.1;
  if(change==4)bodiesD40[0].parent=true;
  frameD40(0,1);checkD40(!lab58n::g_drag[0].held&&drivesD40.empty(),"changed root/part/geometry/scale/native parent releases before a write");
 }
 resetD40();grabD40();statusCallbackD40=[](){bodiesD40[0].entity+=0x100;};frameD40(0,1);
 checkD40(!lab58n::g_drag[0].held&&drivesD40.empty(),"entity mutation inside native GetStatus cannot receive a subsequent velocity write");
 resetD40();grabD40();statusCallbackD40=[](){kcd2vr::collab_h21::setPhysicsMode(0);kcd2vr::collab_h21::setPhysicsMode(3);};frameD40(0,1);
 checkD40(!lab58n::g_drag[0].held&&drivesD40.empty(),"OFF/ON during native status retires the original physics generation");
 resetD40();grabD40();acceptDriveD40=false;frameD40(0,1);
 checkD40(!lab58n::g_drag[0].held&&!cg::claims.nativeHeld[0],"native action refusal retires same-callback ownership");
 resetD40();grabD40();throwDriveD40=true;frameD40(0,1);
 checkD40(!lab58n::g_drag[0].held&&!cg::claims.nativeHeld[0]&&lab58n::g_dragFault,"native action fault retires same-callback ownership and latches further driving");
 resetD40();grabD40();handValidD40[0]=false;frameD40(0,1);
 checkD40(!lab58n::g_drag[0].held&&drivesD40.empty(),"tracking loss releases without injecting throw velocity");
 resetD40();bodiesD40[0].dead=false;frameD40(0,0);frameD40(0,1);
 checkD40(!lab58n::g_drag[0].held&&drivesD40.empty(),"unproven unconscious/pre-existing body is not promoted to a native-dead corpse");
#ifndef KCD2VR_D40_ADAPTER_BASELINE
 namespace owner=kcd2vr::corpse_body_owner_d40;
 resetD40();owner::Lease impale{};const owner::Key impaleKey{owner::Slot::ImpaleServo,0x1000,0x2000,99,1,379};
 checkD40(owner::claims.acquire(impale,impaleKey),"impale reserves its original body");frameD40(0,0);frameD40(0,1);
 checkD40(!lab58n::g_drag[0].held&&drivesD40.empty(),"actual native corpse acquisition refuses an impale-owned victim");owner::claims.release(impale);
 resetD40();grabD40();checkD40(!owner::claims.acquire(impale,impaleKey),"existing hand hold prevents later impale motor takeover");
 lab58n::RetireUnservicedCg1(clockD40);
 checkD40(owner::claims.acquire(impale,impaleKey),"native service retirement releases original body reservation");owner::claims.release(impale);
 resetD40();bodiesD40[0].parts.push_back({380,{{},{0,0,.4}},1,0x4001,0x5001});handsD40[1]={0,0,.4};grabD40(0);grabD40(1);
 checkD40(lab58n::g_drag[0].held&&lab58n::g_drag[1].held,"two actual hand owners may hold different parts of one corpse");
 checkD40(!owner::claims.acquire(impale,impaleKey),"either retained hand excludes impale on the whole victim");
 frameD40(0,0);checkD40(!owner::claims.acquire(impale,impaleKey),"releasing one hand does not erase the other hand's body ownership");
 frameD40(1,0);checkD40(owner::claims.acquire(impale,impaleKey),"last hand release makes the body available");owner::claims.release(impale);
#endif
 resetD40();kcd2vr::inventory_lifecycle_t8::Publish(true,false);grabD40();
 publishWhileReadingR96=true;frameD40(0,1);
 checkD40(lab58n::g_drag[0].held&&drivesD40.size()==1,"fresh tracking published during read does not release corpse grip");
 // R96: no native identities need to change for an observed load to retire
 // the previous squeeze. This is policy retirement, not native body cleanup.
 for(int side=0;side<2;++side){
  resetD40();kcd2vr::inventory_lifecycle_t8::Publish(true,false);grabD40(side);
  kcd2vr::inventory_lifecycle_t8::Publish(true,true);
  kcd2vr::inventory_lifecycle_t8::Publish(true,false);frameD40(side,1);
  checkD40(!lab58n::g_drag[side].held&&drivesD40.empty(),"same-address save load retires old corpse grip before write");
  frameD40(side,0,501);frameD40(side,0,501);frameD40(side,1);
  checkD40(lab58n::g_drag[side].held,"fresh squeeze reacquires after save load");
 }
 lab58n::RetireUnservicedCg1(clockD40);
 std::printf("D40 actual corpse adapter: %u checks, %u failures\n",checksD40,failedD40);
 return failedD40?1:0;
}
