// Executes the actual D72 native owner include. Engine/VM leaves are explicit
// test doubles; the independent Lua5.1 suite executes the actual paired script.
#include "subsystems/physics/saved_corpse_drag_d72.h"
#include "vrik/part_anchor59c7g.h"
#include "subsystems/physics/corpse_grip_cg1.h"
#include "subsystems/physics/corpse_body_owner_d40.h"
#include "vrik/lab58.h"
#include "diagnostics/capture168.h"
#include "native_script_t2.h"
#include "core/observed_load_recovery_r96.h"
#include <atomic>
#include <cassert>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <limits>
static std::uint64_t clock72=1000;
static std::uint64_t GetTickCount64(){return clock72;}
static unsigned GetCurrentThreadId(){return 5;}
static bool empty72[2]{true,true},tracked72[2]{true,true},grip72[2]{false,false},qualified72=true,ui72=true;
static float hand72[2][3]{{.1f,0,0},{10,0,0}};
static bool publishWhileReadingR96=false;
static double rawDelta72=0;static std::uint64_t poseAge72=0;static unsigned displayedReads72=0;static bool cleanupFail72=false;
static unsigned query72=0,drive72=0;static bool reject72=false;static double target72=0;
namespace kcd2vr::collab_h21 {inline std::atomic<std::uint64_t> epoch{1};inline bool current(std::uint64_t e){return e==epoch.load();}}
namespace kcd2vr::tracking59c6 {inline std::atomic<std::uint64_t> epoch{1};}
namespace kcd2vr::hand_only_door_h40 {struct Admission{bool take(unsigned){return true;}};}
namespace diagnostic_trace {inline unsigned threadRefusals=0;inline void Write(const char* text){
 if(std::strstr(text,"SCRIPT_FRAME_OWNER_THREAD_MISMATCH"))++threadRefusals;
}
inline void WriteFor(kcd2vr::diagnostics::capture168::Domain,const char* text){Write(text);}
}
namespace retail_build112 {inline bool Ready(std::uintptr_t){return qualified72;}}
namespace animation_filter {inline std::atomic<bool> nativeBodyOwned112{false};}
namespace gore32n {inline std::atomic<bool> g_drive_fault33{false};
static bool PartVelocity(std::uintptr_t,int,kcd2vr::gore32::V3 v){if(reject72)return false;++drive72;target72=v.x/6;return true;}
}
namespace kcd2vr::native_script_t2 {
bool Context::Current()const noexcept{return true;}
bool Register(Client,Wanted)noexcept{return true;}bool Install()noexcept{return true;}
bool Context::CallPacket(const char*,const char* input,char* output,std::size_t cap)const noexcept {
 char op=0,session[80]{};unsigned side=0,seq=0,id=0;unsigned long long token=0;double x=0,y=0,z=0;
 const auto n=std::sscanf(input,"D72|%c|%79[^|]|%u|%u|%u|%llu|%lf|%lf|%lf",&op,session,&side,&seq,&id,&token,&x,&y,&z);assert(n==9);
 const char* result="CANDIDATE";
 if(op=='Q'||op=='N'||op=='n'){++query72;if(std::fabs(x)>.45){result="NO_NEAR_DEAD_BONE";id=0;token=0;}else{id=42;token=7;result=std::fabs(x)>.25?"NEAR":"CANDIDATE";}}
 else if(op=='A')result="ACQUIRED";
 else if(op=='R'){if(cleanupFail72)return false;result="RELEASED";}
 else if(op=='D'){if(reject72)result="VELOCITY_REFUSED";else{result="VELOCITY_RETURNED";++drive72;target72=x;}}
 else if(op=='V')result=kcd2vr::corpse_grip_cg1::claims.nativeHeld[side].load()?"HELD":std::fabs(x)>.25?"NEAR":"CANDIDATE";
 std::snprintf(output,cap,"D72|%u|%u|%llu|1|0|0|0|%s",seq,id,token,result);return true;
}
}
struct Matrix34V6550{float m[12]{1,0,0,0,0,1,0,0,0,0,1,0};};
struct Entity72{std::uintptr_t vft=123;std::uint32_t pad=0,id=42;unsigned char gap[0x98-16]{};std::uintptr_t parent=0;} entity72;
static char henry72,character72,physics72,otherPhysics72;static void* body72=&physics72;
static std::atomic<void*> g_last_player_entity{&henry72};
static std::atomic<std::uint64_t> g_pose_epoch6689{1};static std::atomic<unsigned> g_stage_thread596{5};
static std::uintptr_t WhBase(){return 1;}static constexpr std::uintptr_t kCEntityVftRva=122;
template<class T>bool Read(const void* p,T& out){if(!p)return false;std::memcpy(&out,p,sizeof(out));return true;}
static bool PhysVft6698(void* p,std::uintptr_t rva){return p==&entity72&&entity72.vft==1+rva;}
static void* FlinchEntityById178r11(std::uint32_t id){return id==42?&entity72:nullptr;}
static void* CharacterForEntity6689(void* e){return e==&entity72?&character72:nullptr;}
static bool legacyRootAvailable95=true;static unsigned legacyRootCalls95=0;
static bool NpcBodyRead172(void* e,void*& out){++legacyRootCalls95;out=legacyRootAvailable95&&e==&entity72?body72:nullptr;return out!=nullptr;}
static bool GetEntityWorldTmV6552(void* e,std::uint32_t id,Matrix34V6550& m){m={};return e==&entity72&&id==42;}
static bool ReadInteractionHandWorld123R1(int side,float& x,float& y,float& z){++displayedReads72;x=hand72[side][0];y=hand72[side][1];z=hand72[side][2];return true;}
static std::atomic<int> g_census_mode{2};static std::atomic<std::uint64_t> g_weapon_request_epoch6694{1};
namespace diagnostics::lab {inline bool NativeReference(){return false;}}
static bool CalibrationReady(){return true;}static bool TrackingHandUsable111(int side,std::uint64_t){return tracked72[side];}
static bool HandClaimBlockedT24(int,std::uintptr_t,std::uint64_t,std::uint64_t){return false;}
struct Point72{float x=0,y=0,z=0;};struct Stamp72{std::uint64_t tick=0,epoch=1;std::uintptr_t actor=55;};
struct CommittedFrame6695{bool valid[2]{true,true},propRequestedValid5916c1[2]{true,true};Stamp72 stamp;Point72 propRequested5916c1[2];float world[8]{};};
static bool ReadCommittedPacket6695(CommittedFrame6695& p){if(publishWhileReadingR96){++clock72;publishWhileReadingR96=false;}p.stamp.tick=clock72-poseAge72;for(int side=0;side<2;++side)p.propRequested5916c1[side]={hand72[side][0]+float(rawDelta72),hand72[side][1],hand72[side][2]};return true;}
static Point72 HandWorld6694(Point72 p,const float*){return p;}static Point72 FromT(Point72 p){return p;}
namespace ph=kcd2vr::physical;
struct SavedCorpsePartR95 {std::uintptr_t body=0,geometry=0,proxyGeometry=0;int part=-1;ph::Pose pose{};double scale=0;};
static bool nativePartReady95=true;static int nativePart95=379;static std::uintptr_t geometry95=1234;
static bool ReadSavedCorpsePartR95(std::uint32_t id,std::uintptr_t identity,std::uintptr_t expected,int part,ph::Vec3,SavedCorpsePartR95& out){
 const auto body=reinterpret_cast<std::uintptr_t>(body72);
 if(!nativePartReady95||id!=42||identity!=reinterpret_cast<std::uintptr_t>(&entity72)||(expected&&expected!=body)||(part>=0&&part!=nativePart95))return false;
 out={body,geometry95,4321,nativePart95,{},1};return true;
}
namespace lab58n {
namespace L=kcd2vr::lab58;namespace corpseMotorD40=kcd2vr::corpse_body_owner_d40;
static std::atomic<bool> g_dragFault{false};struct Legacy{bool held=false,valid=false;};static Legacy g_drag[2],g_dragPendingCg1[2];
static bool DragUiOk(){return ui72;}static bool DragInputCurrentCg1(int side,std::uint64_t){return tracked72[side];}
static bool GripValueR89(int side,std::uint64_t,float& value){value=grip72[side]?.8f:0.f;return tracked72[side];}
static bool HandEmptyW5(int side){return empty72[side];}static bool GripHeldW5(int side){return grip72[side];}
#include "subsystems/physics/saved_corpse_drag_d72.inl"
}
namespace c=kcd2vr::corpse_grip_cg1;namespace l=lab58n;
static void service(){kcd2vr::native_script_t2::Context ctx{1,2,clock72,1};l::SavedServiceD72(ctx);++clock72;}
static c::Gate gates72[2];
static void input(int side,float grip){grip72[side]=grip>=.3f;c::Candidate candidate{};c::handCandidates[side].read(candidate);
 c::updateShared<kcd2vr::hand_only_door_h40::Admission>(gates72[side],side,clock72,true,true,grip,candidate,c::handRequests[side]);++clock72;}
static void reset(){
 for(int side=0;side<2;++side){l::SavedRetireD72(side,"FIXTURE",clock72);l::g_savedDragD72[side]={};l::g_drag[side]={};l::g_dragPendingCg1[side]={};
  c::claims.consumed[side]=false;c::claims.nativeHeld[side]=false;c::claims.fault[side]=false;c::claims.livePress[side]=0;c::claims.generation[side]=1;
  c::handRequests[side].publish({});gates72[side]={};empty72[side]=tracked72[side]=true;grip72[side]=false;
 }
 legacyRootAvailable95=true;legacyRootCalls95=0;nativePartReady95=true;nativePart95=379;geometry95=1234;clock72+=1000;query72=drive72=0;rawDelta72=0;poseAge72=0;displayedReads72=0;cleanupFail72=false;body72=&physics72;g_pose_epoch6689=1;g_stage_thread596=5;kcd2vr::collab_h21::epoch=1;qualified72=ui72=true;reject72=false;
 hand72[0][0]=.1f;hand72[1][0]=10;entity72.parent=0;l::L::rung=3;l::L::f9Rung=3;l::L::corpseDrag=true;
}
static void acquire(int side=0){service();assert(l::SavedDragOwnsD72(side));input(side,0);input(side,.8f);service();assert(l::g_savedDragD72[side].held);assert(drive72==0);}
int main(){
 // D84: selected corpse gameplay must acquire, move and release with ALL
 // diagnostic keys OFF and the former lab mode OFF, for each hand.
 for(int side=0;side<2;++side){
  reset();l::L::rung=0;l::L::f9Rung=0;hand72[side][0]=.1f;hand72[1-side][0]=10;
  service();
  if(!l::SavedDragOwnsD72(side)){std::puts("FAIL D84 selected corpse candidate blocked by recorder/lab OFF");return 1;}
  input(side,0);input(side,.8f);service();
  if(!l::g_savedDragD72[side].held){std::puts("FAIL D84 selected corpse acquisition blocked by recorder/lab OFF");return 1;}
  input(side,.8f);hand72[side][0]=.4f;service();assert(drive72==1&&std::fabs(target72-.3)<1e-5);
  input(side,0);service();assert(!l::SavedDragOwnsD72(side)&&drive72==1);
 }
 // A real near-body nomination precedes the final reach. The original squeeze
 // is reserved without A/velocity, then the SAME script token becomes reachable.
 for(int side=0;side<2;++side){
  reset();hand72[side][0]=.4f;hand72[1-side][0]=10;service();
  if(!l::SavedDragOwnsD72(side)){std::puts("FAIL D78 native near-corpse witness before final reach");return 1;}
  input(side,0);input(side,.8f);service();
  assert(!l::g_savedDragD72[side].held&&drive72==0&&c::claims.consumed[side]);
  const auto original=gates72[side].press;clock72+=60;hand72[side][0]=.2f;
  service();input(side,.8f);service();
  assert(l::g_savedDragD72[side].held&&l::g_savedDragD72[side].press==original&&drive72==0);
  input(side,0);service();assert(!l::SavedDragOwnsD72(side));
 }
 // D78 actual adapter counterexample: contact wrist can be blocked far from a
 // corpse while the raw controller reaches it. Both hands must acquire there.
 for(int side=0;side<2;++side){
  reset();hand72[side][0]=.6f;hand72[1-side][0]=10;rawDelta72=-.5;
  service();if(!l::SavedDragOwnsD72(side)){std::puts("FAIL D78 raw acquisition while contact wrist is blocked");return 1;}
  input(side,0);input(side,.8f);service();assert(l::g_savedDragD72[side].held);
  const auto owner=l::g_savedDragD72[side].key;
  const auto queries=query72;rawDelta72=-.2;input(side,.8f);service();
  assert(drive72==1&&std::fabs(target72-.3)<1e-5);
  assert(c::same(owner,l::g_savedDragD72[side].key)&&query72==queries&&displayedReads72==0);
  input(side,0);service();assert(!l::SavedDragOwnsD72(side)&&drive72==1);
 }
 reset();poseAge72=81;service();assert(query72==0&&drive72==0&&displayedReads72==0);
 reset();rawDelta72=std::numeric_limits<double>::quiet_NaN();service();assert(query72==0&&drive72==0&&displayedReads72==0);

 reset();acquire();input(0,.8f);hand72[0][0]=.59f;service();assert(drive72==1&&std::fabs(target72-.49)<1e-5); // retained .1 contact offset
 input(0,0);service();assert(!l::SavedDragOwnsD72(0)&&!c::claims.nativeHeld[0]&&drive72==1);
 reset();acquire();auto before=query72;for(int i=0;i<20;++i){input(0,.8f);service();}assert(query72-before<=1); // other empty hand may query once
 reset();acquire();tracked72[0]=false;service();assert(!l::SavedDragOwnsD72(0)&&drive72==0);
 reset();acquire();body72=&otherPhysics72;input(0,.8f);service();assert(!l::SavedDragOwnsD72(0)&&drive72==0);
 reset();acquire();g_pose_epoch6689=2;input(0,.8f);service();assert(!l::SavedDragOwnsD72(0)&&drive72==0);
 reset();acquire();l::L::f9Rung=0;l::L::rung=0;input(0,.8f);service();assert(l::SavedDragOwnsD72(0)&&drive72==1);
 reset();acquire();l::L::corpseDrag=false;service();assert(!l::SavedDragOwnsD72(0)&&drive72==0);
 reset();acquire();empty72[0]=false;service();assert(!l::SavedDragOwnsD72(0)&&drive72==0);
 reset();acquire();g_stage_thread596=6;service();assert(!l::SavedDragOwnsD72(0)&&drive72==0);
 reset();acquire();reject72=true;input(0,.8f);service();assert(!l::SavedDragOwnsD72(0)&&drive72==0);
 reset();l::g_dragPendingCg1[0].valid=true;service();assert(!l::SavedDragOwnsD72(0));
 reset();l::L::corpseDrag=false;service();assert(query72==0&&drive72==0);
 reset();hand72[0][0]=10;hand72[1][0]=.1f;acquire(1);input(1,.8f);hand72[1][0]=.4f;service();assert(drive72==1&&std::fabs(target72-.3)<1e-5);
 reset();acquire();std::uintptr_t parent[4]{0,0,0,1};entity72.parent=reinterpret_cast<std::uintptr_t>(parent);input(0,.8f);service();assert(!l::SavedDragOwnsD72(0)&&drive72==0);
 reset();acquire();clock72+=81;l::SavedWatchdogD72(clock72);assert(!l::SavedDragOwnsD72(0)&&!c::claims.nativeHeld[0]&&drive72==0&&l::g_savedClearD72[0]);
 service();assert(!l::g_savedClearD72[0]);
 reset();acquire();rawDelta72=.4;input(0,.8f);service();assert(drive72==1&&std::fabs(target72-.4)<1e-5); // achieved wrist stayed at .1, controller .5 still drives body
 reset();acquire();cleanupFail72=true;tracked72[0]=false;service();assert(l::g_savedClearD72[0]);clock72+=501;service();assert(l::g_savedClearD72[0]);cleanupFail72=false;clock72+=501;service();assert(!l::g_savedClearD72[0]);
 reset();qualified72=false;service();assert(query72==0&&drive72==0);
 // A mismatched native callback owner never enters the query/drive leaves and
 // records at most one refusal per hand in each two-second diagnostic window.
 reset();g_stage_thread596=6;l::g_savedSearchReportD72[0]=l::g_savedSearchReportD72[1]=0;
 namespace cap=kcd2vr::diagnostics::capture168;
#ifdef KCD2VR_TEST_PHYSICS_D84
 cap::gate.select(cap::Domain::Physics,true,clock72);
#else
 cap::gate.select(cap::Domain::Combat,true,clock72);
#endif
 diagnostic_trace::threadRefusals=0;service();assert(diagnostic_trace::threadRefusals==2&&query72==0&&drive72==0);
 for(int i=0;i<100;++i)service();assert(diagnostic_trace::threadRefusals==2&&query72==0&&drive72==0);
 clock72+=2000;service();assert(diagnostic_trace::threadRefusals==4&&query72==0&&drive72==0);
#ifdef KCD2VR_TEST_PHYSICS_D84
 cap::gate.select(cap::Domain::Physics,false,clock72);
#else
 cap::gate.select(cap::Domain::Combat,false,clock72);
#endif
 l::L::f9Rung=0;clock72+=2000;service();assert(diagnostic_trace::threadRefusals==4&&query72==0&&drive72==0);
 // A failed/unavailable legacy entity-root query does not veto the current
 // native main-body/part join. This fixture does not claim a null ns::Binding root.
 for(int side=0;side<2;++side){
  reset();legacyRootAvailable95=false;hand72[side][0]=.1f;hand72[1-side][0]=10;acquire(side);
  input(side,.8f);hand72[side][0]=.3f;service();assert(drive72==1&&legacyRootCalls95==0);
  input(side,0);service();assert(!l::SavedDragOwnsD72(side)&&legacyRootCalls95==0);
 }
 // R95: every hand retains an exact part. Transient body-status refusal
 // releases, then a new valid body and a new squeeze recover without a latch.
 for(int side=0;side<2;++side){
  reset();hand72[side][0]=.1f;hand72[1-side][0]=10;acquire(side);
  assert(l::g_savedDragD72[side].key.route==c::Route::SavedArticulatedPartR95&&l::g_savedDragD72[side].key.part==379);
  nativePartReady95=false;input(side,.8f);service();assert(!l::SavedDragOwnsD72(side)&&drive72==0);
  nativePartReady95=true;clock72+=501;input(side,0);service();input(side,.8f);service();
  assert(l::g_savedDragD72[side].held);input(side,.8f);hand72[side][0]=.3f;service();assert(drive72==1);
 }
 reset();acquire();geometry95++;input(0,.8f);service();assert(!l::SavedDragOwnsD72(0)&&drive72==0);
 reset();acquire();nativePart95=380;input(0,.8f);service();assert(!l::SavedDragOwnsD72(0)&&drive72==0);
 reset();acquire();input(0,.8f);hand72[0][0]=.7f;service();assert(!l::SavedDragOwnsD72(0)&&drive72==0); // existing part-servo leash
 // A concurrent publisher can finish after the caller's old clock sample.
 // Its fresh packet must not release an otherwise valid held corpse.
 reset();acquire();input(0,.8f);publishWhileReadingR96=true;service();
 if(!l::g_savedDragD72[0].held||drive72!=1){std::puts("FAIL R96 fresh corpse hand packet treated as future tracking loss");return 1;}
 // R96 regression: a completed save load can retain every native pointer,
 // pose epoch and VM identity. It still cannot replay the old held squeeze.
 for(int side=0;side<2;++side){
  reset();kcd2vr::inventory_lifecycle_t8::Publish(true,false);
  hand72[side][0]=.1f;hand72[1-side][0]=10;acquire(side);
  kcd2vr::inventory_lifecycle_t8::Publish(true,true);
  kcd2vr::inventory_lifecycle_t8::Publish(true,false);
  input(side,.8f);service();
  if(l::SavedDragOwnsD72(side)||drive72){std::puts("FAIL R96 same-address load replays saved-corpse grip");return 1;}
  clock72+=501;input(side,0);service();input(side,.8f);service();
  if(!l::g_savedDragD72[side].held){std::puts("FAIL R96 new squeeze cannot reacquire after load");return 1;}
  input(side,.8f);hand72[side][0]=.3f;service();assert(drive72==1);
 }
 std::puts("PASS actual D72 service: loaded-corpse provider, CG1 acquire/release, zero acquire write, hand-relative servo, retained target, native part priority, either hand, recorder-independent gameplay, feature/fingerprint/tracking/item/VM-thread/body/parent/epoch retirement");
}
