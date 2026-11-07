// D37B camera-owner regression. The runner extracts the unmodified production
// admission, camera core, gap wrapper and settings code. Native packet/world
// reads and telemetry sinks are explicit synthetic leaves. Two real host
// threads force camera-reader contention without timing sleeps or reentrant
// std::mutex assumptions. No game, XR runtime or headset is executed.
#include "vrik/tracked_frame51.h"
#include "activity_eye59c19.h"
#include "vrik/workstation178r20c1.h"
#include "vrik/presentation_context55.h"
#include "tracking_sample59c6.h"
#include "core/native_grasp13.h"
#include "core/snapshot_mailbox.h"
#include "diagnostics/capture168.h"
#include <atomic>
#include <mutex>
#include <cstdio>
#include <cstring>
#include <thread>
#include <condition_variable>
#include <chrono>

namespace kcd2vr::visible_failure { static bool publish(const char*,std::uint64_t){return true;} }
namespace kcd2vr::frik_pose_probe {
namespace f=kcd2vr::tracked_frame51;namespace m=kcd2vr::pose_core;
struct CommittedFrame6695 {m::Stamp stamp{};float world[8]{};bool headEvidence122=false;};
static int census51=3;static std::uint64_t now51=1000;
static std::uint64_t GetTickCount64(){return now51;}
static int CombatCensusMode(){return census51;}
static std::atomic<int> g_cal_state{2};
static std::atomic<bool> g_gameplay_allowed{true},g_head_valid{true},g_ctx_dialogue{false},g_ctx_menu{false},g_ctx_cutscene{false},g_ctx_scripted{false},g_ctx_reading{false};
static kcd2vr::grasp13::Acquisition54 pickupCamera54{};
static bool pickupCameraReady54=false;
static bool ReadPickupCamera54(kcd2vr::grasp13::Acquisition54& p) noexcept {p=pickupCamera54;return pickupCameraReady54;}
static bool (*g_graspAcquisition54)(kcd2vr::grasp13::Acquisition54&) noexcept=nullptr;
static constexpr std::uint64_t kNativeBodyHeightBit54=std::uint64_t(1)<<63;
static std::atomic<std::uint64_t> g_frame_control51{2},g_frame_publication51{0},g_pose_epoch6689{1};
static std::atomic<void*> g_henry_char_attachment_manager{reinterpret_cast<void*>(0x108)};
namespace diagnostics::lab {static bool reference=false;static bool NativeReference(){return reference;}}
namespace animation_filter {static std::atomic<bool> nativeBodyOwned112{false};static bool kick=false;static bool NativeKickActive175(){return kick;}}
namespace diagnostic_trace {static unsigned bowReceipts55=0;static void Write(const char* s){if(std::strstr(s,"C55 BOW_CAMERA "))++bowReceipts55;}}
static bool packetReady51=true;static CommittedFrame6695 packet51{};
// The mailbox is the synthetic leaf. The production packet admission/freshness
// reader is extracted below so it cannot silently bypass gameplay ownership.
static std::atomic<bool> g_committed_pose_valid6689{true},g_pose_restore_fault6689{false},g_dialogue_body178r9{false};
static std::atomic<std::uint64_t> g_tool_context19c4r2{1000<<8};
static std::atomic<void*> g_committed_character6689{reinterpret_cast<void*>(0x100)};
static bool TrackingHeadUsable111(){return g_head_valid.load();}

static std::atomic<float> g_cal_native_head_x{0},g_cal_native_head_y{0},g_cal_native_head_z{1.7f};
static std::uint64_t GetCurrentThreadId(){return 1;}
namespace diagnostics::ownership48 {
enum {CameraBase=1};
struct Event {std::uint64_t stage=0,tick=0,thread=0,code=0,epoch=0,publication=0,flags=0,actor=0,sample=0;float v[32]{};char label[128]{};};
static std::uint64_t cameraSample48=0,cameraPublication48=0,cameraEpoch48=0,cameraActor48=0;
static std::atomic<bool> capture{false};static std::atomic<unsigned> busyReceipts{0};
static bool active(std::uint64_t){return capture.load();}
static void name(Event& e,const char* label){std::snprintf(e.label,sizeof(e.label),"%s",label);}
static bool push(const Event& e){if(std::strcmp(e.label,"TRACKED_CAMERA_CORE_BUSY")==0)++busyReceipts;return true;}
}
static std::mutex nativeLeafMutex;
static std::condition_variable nativeLeafCv;
static bool leafEntered=false,leafRelease=false;
static std::atomic<bool> blockOneWorldRead{false},menuOnOneWorldRead{false};
// Current committed-packet native leaf: exact production reader is extracted below.
static struct PacketMailbox55 {bool read(CommittedFrame6695& p){p=packet51;return packetReady51;}} g_committed_frame6695;
static kcd2vr::core::SnapshotMailbox<f::Frame> g_tracked_frame51;
struct ModelWorld6692 {m::Quat q{};m::Vec3 t{};double scale=1;};
static ModelWorld6692 world51{};static bool worldReady51=true;static std::atomic<unsigned> worldReads51{0};
static std::atomic<std::uintptr_t> worldCharacter51{0};
static bool ReadModelWorld6692(void* character,ModelWorld6692& out){
    ++worldReads51;worldCharacter51=reinterpret_cast<std::uintptr_t>(character);out=world51;
    if(blockOneWorldRead.exchange(false)){
        std::unique_lock<std::mutex> lk(nativeLeafMutex);leafEntered=true;nativeLeafCv.notify_all();
        nativeLeafCv.wait(lk,[]{return leafRelease;});
    }else if(menuOnOneWorldRead.exchange(false))g_ctx_menu=true;

    return worldReady51;
}

#include "generated_bow_camera_d37b.inc"
static unsigned checks=0,failures=0;
static void ck(bool yes,const char* name){++checks;if(!yes)++failures;std::printf("%s %s\n",yes?"PASS":"FAIL",name);}
static bool near(float a,float b){return std::fabs(a-b)<1e-5f;}
static f::Frame frame{};
static void publishFrame(){
    const auto previous=g_frame_publication51.load();g_frame_publication51=previous+1;frame.publication=previous+2;
    packet51.stamp=frame.stamp;packet51.headEvidence122=true;
    packet51.world[0]=float(frame.rotation.x);packet51.world[1]=float(frame.rotation.y);packet51.world[2]=float(frame.rotation.z);packet51.world[3]=float(frame.rotation.w);
    packet51.world[4]=float(frame.root.x);packet51.world[5]=float(frame.root.y);packet51.world[6]=float(frame.root.z);packet51.world[7]=float(frame.scale);
    g_tracked_frame51.publish(frame);g_frame_publication51=frame.publication;
    world51={frame.rotation,frame.root,frame.scale};g_tool_context19c4r2=now51<<8;
}
static bool call(float& x,float& y,float& z,float& hx,float& hy){return TrackedFrameCamera55(10.10f,20.10f,6.50f,.12f,.9928f,.02f,x,y,z,hx,hy);}
static bool cacheValid(){kcd2vr::camera_hold_e1::Sample s{};return g_camera_hold_e1.read(s)&&s.valid;}
static void seed(){
 g_cal_state=2;census51=3;g_head_valid=true;g_gameplay_allowed=true;g_committed_pose_valid6689=true;
 g_ctx_menu=g_ctx_dialogue=g_ctx_cutscene=g_ctx_scripted=g_ctx_reading=false;animation_filter::nativeBodyOwned112=false;animation_filter::kick=false;
 diagnostics::lab::reference=false;worldReady51=packetReady51=true;g_graspAcquisition54=nullptr;menuOnOneWorldRead=false;
 ++now51;frame={};frame.stamp={1,now51,now51,0x100};frame.control=2;frame.valid=frame.neutral=true;
 frame.root={10,20,5};frame.nativeHead=frame.anchor=frame.rawAnchor=frame.calibratedHead={0,0,1.7};frame.restOffset={0,0,.8};publishFrame();
 float x,y,z,hx,hy;ck(TrackedFrameCamera55(10,20,6.756f,0,1,0,x,y,z,hx,hy),"seed neutral anatomy and heading");
 frame.bow=true;frame.neutral=false;++frame.stamp.sample;frame.stamp.tick=++now51;publishFrame();
 ck(call(x,y,z,hx,hy)&&near(x,10)&&near(y,20)&&near(z,6.756f)&&near(hx,0)&&near(hy,1),"seed accepted bow excludes native aim sway");
 ck(cacheValid(),"seed hold cache valid");
}
static void contention(int protectedGate,bool raceAfterAdmission=false){
 seed();
 {std::lock_guard<std::mutex> lk(nativeLeafMutex);leafEntered=false;leafRelease=false;}
 blockOneWorldRead=true;
 bool first=false;
 std::thread t([&]{float x,y,z,hx,hy;first=call(x,y,z,hx,hy);});
 {std::unique_lock<std::mutex> lk(nativeLeafMutex);if(!nativeLeafCv.wait_for(lk,std::chrono::seconds(5),[]{return leafEntered;}))std::abort();}
 switch(protectedGate){case 1:g_ctx_menu=true;break;case 2:g_ctx_dialogue=true;break;case 3:g_ctx_cutscene=true;break;
 case 4:g_ctx_scripted=true;break;case 5:g_ctx_reading=true;break;case 6:animation_filter::nativeBodyOwned112=true;break;
 case 7:animation_filter::kick=true;break;case 8:g_gameplay_allowed=false;break;case 9:g_head_valid=false;break;}
 if(raceAfterAdmission)menuOnOneWorldRead=true;
 diagnostics::ownership48::busyReceipts=0;diagnostics::ownership48::capture=true;
 float x=0,y=0,z=0,hx=0,hy=0;const bool second=call(x,y,z,hx,hy);const bool retained=cacheValid();
 diagnostics::ownership48::capture=false;const auto busyReceipts=diagnostics::ownership48::busyReceipts.load();
 if(!protectedGate&&!raceAfterAdmission){
  std::printf("REPRO busyResult=%d cacheAfterBusy=%d consumerBase=(%.3f,%.3f,%.3f) nativeEye=(10.100,20.100,6.500)\n",second,retained,second?x:10.1,second?y:20.1,second?z:6.5);
  ck(second&&near(x,10)&&near(y,20)&&near(z,6.756f)&&near(hx,0)&&near(hy,1),"valid gameplay busy reader retains accepted bow eye and heading");
  ck(retained&&busyReceipts==1,"valid gameplay busy reader preserves cache and emits specific busy reason only under capture");
 }else{
  ck(!second,"protected owner or takeover during hold refuses held base");
  ck(!retained,"protected owner clears held camera cache");
 }
 {std::lock_guard<std::mutex> lk(nativeLeafMutex);leafRelease=true;nativeLeafCv.notify_all();}t.join();
 if(!protectedGate&&!raceAfterAdmission)ck(first,"first owner completes after contention");
 else ck((!g_ctx_menu&&!protectedGate)||!first,"first callback cannot publish after an observed current context takeover");
 menuOnOneWorldRead=false;
}
static void expiredGap(){
 seed();g_committed_pose_valid6689=false;
 const auto start=now51;float x,y,z,hx,hy;
 now51=start+80;ck(call(x,y,z,hx,hy)&&near(z,6.756f),"fresh HMD camera survives no completed hand/body packet for80ms");
 now51=start+1500;ck(call(x,y,z,hx,hy)&&near(z,6.756f),"same-body hold covers inclusive1500ms limit");
 now51=start+1501;ck(!call(x,y,z,hx,hy),"head base returns unavailable after1500ms body gap despite valid head and gameplay");
 g_committed_pose_valid6689=true;
}
}
int main(){using namespace kcd2vr::frik_pose_probe;
 contention(0);for(int gate=1;gate<=9;++gate)contention(gate);contention(0,true);expiredGap();
 std::printf("CAMERA_REPRO %u/%u checks passed; source=exact_service synthetic_leaves only native/headset=NOT_EXECUTED\n",checks-failures,checks);
 return failures?1:0;
}
