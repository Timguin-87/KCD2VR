// R89 corpse: search-on-squeeze fixture (harness copied from saved_corpse_native_d72_test.cpp).
// R90: runs on top of ChatGPT R2 (idle budget, bounds, shared discovery). The R89
// route is the Experiments option 'Corpse search' = ONLY ON SQUEEZE
// (lab58::corpseSqueezeSearchR90); cases 7-8 check the R2 default ALWAYS route.
// Executes the actual D72 native owner include. Engine/VM leaves are explicit
// test doubles; the independent Lua5.1 suite executes the actual paired script.
#include "subsystems/physics/saved_corpse_drag_d72.h"
#include "subsystems/physics/corpse_grip_cg1.h"
#include "subsystems/physics/corpse_body_owner_d40.h"
#include "vrik/lab58.h"
#include "diagnostics/capture168.h"
#include "native_script_t2.h"
#include <atomic>
#include <cassert>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <limits>
#include <chrono>
#include <cmath>
static std::uint64_t clock72=1000;
static std::uint64_t GetTickCount64(){return clock72;}
static unsigned GetCurrentThreadId(){return 5;}
static bool empty72[2]{true,true},tracked72[2]{true,true},grip72[2]{false,false},qualified72=true,ui72=true;
static float hand72[2][3]{{.1f,0,0},{10,0,0}};
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
namespace gore32n {inline std::atomic<bool> g_drive_fault33{false};}
namespace kcd2vr::native_script_t2 {
bool Context::Current()const noexcept{return true;}
bool Register(Client,Wanted)noexcept{return true;}bool Install()noexcept{return true;}
bool Context::CallPacket(const char*,const char* input,char* output,std::size_t cap)const noexcept {
 char op=0,session[80]{};unsigned side=0,seq=0,id=0;unsigned long long token=0;double x=0,y=0,z=0;
 const auto n=std::sscanf(input,"D72|%c|%79[^|]|%u|%u|%u|%llu|%lf|%lf|%lf",&op,session,&side,&seq,&id,&token,&x,&y,&z);assert(n==9);
 const char* result="CANDIDATE";
 if(op=='Q'){++query72;if(std::fabs(x)>.45){result="NO_NEAR_DEAD_BONE";id=0;token=0;}else{id=42;token=7;result=std::fabs(x)>.25?"NEAR":"CANDIDATE";}}
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
static bool NpcBodyRead172(void* e,void*& out){out=e==&entity72?body72:nullptr;return out!=nullptr;}
static bool GetEntityWorldTmV6552(void* e,std::uint32_t id,Matrix34V6550& m){m={};return e==&entity72&&id==42;}
static bool ReadInteractionHandWorld123R1(int side,float& x,float& y,float& z){++displayedReads72;x=hand72[side][0];y=hand72[side][1];z=hand72[side][2];return true;}
static std::atomic<int> g_census_mode{2};static std::atomic<std::uint64_t> g_weapon_request_epoch6694{1};
namespace diagnostics::lab {inline bool NativeReference(){return false;}}
static bool CalibrationReady(){return true;}static bool TrackingHandUsable111(int side,std::uint64_t){return tracked72[side];}
static bool HandClaimBlockedT24(int,std::uintptr_t,std::uint64_t,std::uint64_t){return false;}
struct Point72{float x=0,y=0,z=0;};struct Stamp72{std::uint64_t tick=0,epoch=1;std::uintptr_t actor=55;};
struct CommittedFrame6695{bool valid[2]{true,true},propRequestedValid5916c1[2]{true,true};Stamp72 stamp;Point72 propRequested5916c1[2];float world[8]{};};
static bool ReadCommittedPacket6695(CommittedFrame6695& p){p.stamp.tick=clock72-poseAge72;for(int side=0;side<2;++side)p.propRequested5916c1[side]={hand72[side][0]+float(rawDelta72),hand72[side][1],hand72[side][2]};return true;}
static Point72 HandWorld6694(Point72 p,const float*){return p;}static Point72 FromT(Point72 p){return p;}
namespace lab58n {
namespace L=kcd2vr::lab58;namespace corpseMotorD40=kcd2vr::corpse_body_owner_d40;
static std::atomic<bool> g_dragFault{false};struct Legacy{bool held=false,valid=false;};static Legacy g_drag[2],g_dragPendingCg1[2];
static bool DragUiOk(){return ui72;}static bool DragInputCurrentCg1(int side,std::uint64_t){return tracked72[side];}
static float gripValue72[2]{0,0};
static bool HandEmptyW5(int side){return empty72[side];}static bool GripHeldW5(int side){return gripValue72[side]>=.3f;}
#ifndef R89_BASELINE
static bool GripValueR89(int side,std::uint64_t,float& g){g=gripValue72[side];return tracked72[side];}
#endif
#include "subsystems/physics/saved_corpse_drag_d72.inl"
}

namespace c=kcd2vr::corpse_grip_cg1;namespace l=lab58n;
static void frame(){kcd2vr::native_script_t2::Context ctx{1,2,clock72,1};if(l::SavedWantedD72())l::SavedServiceD72(ctx);clock72+=11;}
static c::Gate gates72[2];static bool squeeze72=true;
static void grip(int side,float g){l::gripValue72[side]=g;c::Candidate candidate{};c::handCandidates[side].read(candidate);
 c::updateShared<kcd2vr::hand_only_door_h40::Admission>(gates72[side],side,clock72,true,true,g,candidate,c::handRequests[side]);}
static void reset(){
 for(int side=0;side<2;++side){l::SavedRetireD72(side,"FIXTURE",clock72);l::g_savedDragD72[side]={};l::g_drag[side]={};l::g_dragPendingCg1[side]={};
  c::claims.consumed[side]=false;c::claims.nativeHeld[side]=false;c::claims.fault[side]=false;c::claims.livePress[side]=0;c::claims.generation[side]=1;
  c::handRequests[side].publish({});gates72[side]={};empty72[side]=tracked72[side]=true;l::gripValue72[side]=0;}
 clock72+=1000;query72=drive72=0;rawDelta72=0;poseAge72=0;body72=&physics72;g_pose_epoch6689=1;g_stage_thread596=5;kcd2vr::collab_h21::epoch=1;qualified72=ui72=true;reject72=false;
 hand72[0][0]=.1f;hand72[1][0]=10;entity72.parent=0;l::L::rung=0;l::L::f9Rung=0;l::L::corpseDrag=true;l::L::corpseSqueezeSearchR90=squeeze72;
 for(int i=0;i<3;++i){grip(0,0);grip(1,0);frame();} // open hands arm the CG1 gates
 query72=0;
}
#define CHECK(x,msg) do{if(!(x)){std::printf("FAIL %s\n",msg);return 1;}}while(0)
int main(){
 // 1. Feature ON, both hands empty and OPEN, a corpse right at the hand for 10 s of frames: no Lua query at all.
 reset();for(int i=0;i<900;++i){grip(0,0);grip(1,0);frame();}
 CHECK(query72==0,"open empty hands near a corpse still run the dead-body query every 100 ms (perf tank)");
 // 2. Grip held closed for 10 s in empty space: bounded queries (squeeze window only), then none.
 reset();hand72[0][0]=5;for(int i=0;i<900;++i){grip(0,1);frame();}
 CHECK(query72>=1&&query72<=8,"closed empty hand keeps querying after the squeeze window");
 // 3. Real squeeze near a corpse still grabs and drives (analog Quest grip ramps .1 -> .2 -> .4 -> .8).
 for(int side=0;side<2;++side){
  reset();hand72[side][0]=.1f;hand72[1-side][0]=10;
  const float ramp[]{.1f,.2f,.4f,.8f};for(float g:ramp){grip(side,g);frame();}
  CHECK(l::g_savedDragD72[side].held,"squeeze near a corpse no longer grabs");
  CHECK(drive72==0,"acquisition wrote a velocity");
  hand72[side][0]=.4f;grip(side,.8f);frame();CHECK(drive72==1&&std::fabs(target72-.3)<1e-5,"held corpse not driven to the hand");
  for(int i=0;i<200;++i){grip(side,.8f);frame();}CHECK(l::g_savedDragD72[side].held,"long hold dropped by the search window");
  grip(side,0);frame();CHECK(!l::SavedDragOwnsD72(side),"release did not retire");
 }
 // 4. A near nomination ends when the hand opens again: no per-frame V calls on an open hand.
 reset();hand72[0][0]=.4f;grip(0,.2f);frame();CHECK(l::SavedDragOwnsD72(0),"near nomination missing during squeeze start");
 grip(0,0);frame();CHECK(!l::SavedDragOwnsD72(0),"nomination kept on an open hand");
 const auto q=query72;for(int i=0;i<300;++i){grip(0,0);frame();}CHECK(query72==q,"open hand queried after nomination ended");
 // 5. Opening and squeezing again reopens the window.
 reset();hand72[0][0]=5;for(int i=0;i<200;++i){grip(0,1);frame();}const auto a=query72;
 for(int i=0;i<3;++i){grip(0,0);frame();}hand72[0][0]=.1f;
 const float ramp[]{.2f,.4f,.8f,.8f};for(float g:ramp){grip(0,g);frame();}
 CHECK(query72>a&&l::g_savedDragD72[0].held,"second squeeze did not search/grab");
 // 6. Pure window policy.
#ifndef R89_BASELINE
 // 7. R90 default ALWAYS (R2): open hands near a corpse ARE searched (R2 10 Hz budget),
 // so a squeeze that jumps from open to fully closed inside one frame still grabs.
 squeeze72=false;reset();for(int i=0;i<91;++i){grip(0,0);grip(1,0);frame();}
 CHECK(query72>=8&&query72<=20,"R2 always route: open hand not searched at its 10 Hz budget");
 grip(0,1);frame();grip(0,1);frame();CHECK(l::g_savedDragD72[0].held,"R2 always route: one-frame squeeze missed");
 // 8. The same one-frame squeeze on ONLY ON SQUEEZE: the search starts in the frame the
 // grip already crossed the acquire threshold, so this press misses (documented trade-off;
 // D85R5 log: 4 of 32 real presses went 0 -> >.58 between two ~11 ms samples).
 squeeze72=true;reset();grip(0,1);frame();grip(0,1);frame();
 CHECK(!l::g_savedDragD72[0].held,"one-frame squeeze case changed: update the R90 option text");
 squeeze72=true;
 c::SearchWindowR89 w{};CHECK(!w.update(true,.1f,100),"below threshold searched");CHECK(w.update(true,.15f,100),"threshold");
 CHECK(w.update(true,1,800)&&!w.update(true,1,801),"700 ms window");CHECK(!w.update(true,1,900),"searching after window");
 CHECK(!w.update(false,1,900)&&w.update(true,.5f,1000),"stale input resets / new squeeze reopens");
 CHECK(!c::closingR89(true,std::numeric_limits<float>::quiet_NaN()),"NaN grip");
#endif
 std::puts("PASS R89 corpse search (R90 option on R2): zero queries on open hands, bounded per squeeze, grab/drive/release kept for both hands, nomination retired on open; R2 always route searches idle hands and catches a one-frame squeeze, squeeze route misses it");
}
