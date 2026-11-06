// Compiled with the exact ApplyUtilityGestures54 production function extracted
// by tools/test_live_experiments_d39.py. Only XR/native boundary leaves are fake.
#include "vrik/utility_gestures54.h"
#include <atomic>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdio>
static std::uint64_t fakeNow=1000;
static std::uint64_t GetTickCount64() noexcept {return fakeNow;}
namespace kcd2vr {
struct XrPose {float qw=1,qx=0,qy=0,qz=0,pos_x=0,pos_y=0,pos_z=0;};
struct ControllerState {float trigger=0,grip=0;bool active=true;XrPose pose{};};
namespace native_vr_pad_premap {
struct State {float lt=0;};
struct Context {bool vr_menu=false,theater=false,system_menu=false,dialogue_navigation=false,semantic_minigame=false,apse_ui=false,back_context=false;};
}
namespace animation_filter {std::atomic<bool> nativeBodyOwned112{false};}
namespace diagnostics::capture168 {enum class Domain {Combat,Interaction};bool active(Domain,std::uint64_t){return false;}}
namespace diagnostic_trace {void Write(const char*){}}
namespace frik_pose_probe {
bool calibrated=true;int leftItem=0,physicalItem=0;unsigned torchRequests=0,visorRequests=0;bool visorClosed=false;std::uint64_t requestTick=0;
bool CalibrationReady(){return calibrated;}
int UtilityLeftItem54(){return leftItem;}
int UtilityLeftItemR114(std::uint64_t){return leftItem;}
int ReadPhysicalHandHeld55(int,std::uint64_t){return physicalItem;}
bool RequestTorch62(bool gesture,std::uint64_t tick){assert(gesture);++torchRequests;requestTick=tick;return true;}
bool RequestVisor54(bool close,std::uint64_t tick){++visorRequests;visorClosed=close;requestTick=tick;return true;}
}
#include "utility_dispatch_d39_production.inc"
}
using namespace kcd2vr;
namespace ug=kcd2vr::utility54;
namespace native=kcd2vr::native_vr_pad_premap;
static native::Context context{};
static float step(bool tracked=true){fakeNow+=20;native::State pad{};pad.lt=.9f;ApplyUtilityGestures54(pad,context,tracked);return pad.lt;}
static void release(){g_gesture_frame59c5.hands[1].trigger=0;g_gesture_frame59c5.hands[1].grip=0;step();}
static void shoulder(){auto& p=g_gesture_frame59c5.hands[1].pose;p.pos_x=-.30f;p.pos_y=-.22f;p.pos_z=.23f;}
static void temple(){auto& p=g_gesture_frame59c5.hands[1].pose;p.pos_x=-.16f;p.pos_y=0;p.pos_z=-.04f;}
int main(){
    namespace receipt=frik_pose_probe;
    g_gesture_frame59c5.valid=true;auto& left=g_gesture_frame59c5.hands[1];
    // D38 torch fix must survive the settings addition: holding a native torch
    // with Grip is not a fist. The actual production dispatcher queues once.
    shoulder();receipt::leftItem=11;receipt::physicalItem=11;release();left.grip=1;left.trigger=.9f;
    assert(step()==0);for(int n=0;n<8;++n)step();
    assert(receipt::torchRequests==1&&receipt::requestTick==fakeNow);
    for(int n=0;n<10;++n){step();}
    assert(receipt::torchRequests==1);
    // A torch toggle OFF in a live hold consumes the retired trigger until
    // release. Re-enabling the same hold cannot replay or leak native LT.
    release();left.grip=1;left.trigger=.9f;assert(step()==0);
    ug::SetTorchGestureD39(false);assert(step()==0);ug::SetTorchGestureD39(true);
    for(int n=0;n<12;++n){assert(step()==0);}
    assert(receipt::torchRequests==1);
    release();left.grip=1;left.trigger=.9f;step();for(int n=0;n<8;++n)step();assert(receipt::torchRequests==2);
    // Visor owns an empty left temple gesture independently of torch.
    receipt::leftItem=receipt::physicalItem=0;temple();release();ug::SetTorchGestureD39(false);
    left.trigger=.9f;assert(step()==0);left.pose.pos_y=-.12f;step();assert(receipt::visorRequests==1&&receipt::visorClosed);
    // Disabled visor cannot intercept gameplay trigger or arm after re-enable
    // without a known release. Grip+Trigger retains the native fist route.
    release();temple();ug::SetVisorGestureD39(false);left.trigger=.9f;assert(step()==.9f);
    ug::SetVisorGestureD39(true);left.pose.pos_y=.12f;assert(step()==.9f);assert(receipt::visorRequests==1);
    release();temple();left.grip=1;left.trigger=.9f;assert(step()==.9f);left.pose.pos_y=-.12f;step();assert(receipt::visorRequests==1);
    // Held unknown/weapon identity, menu ownership, native body actions,
    // calibration loss and tracking loss cannot dispatch utility requests.
    release();temple();receipt::leftItem=-1;left.trigger=.9f;step();left.pose.pos_y=-.12f;step();assert(receipt::visorRequests==1);
    receipt::leftItem=0;
    for(unsigned gate=0;gate<4;++gate){release();temple();context.vr_menu=gate==0;animation_filter::nativeBodyOwned112=gate==1;receipt::calibrated=gate!=2;
        left.trigger=.9f;step(gate!=3);left.pose.pos_y=-.12f;step(gate!=3);assert(receipt::visorRequests==1);
        context={};animation_filter::nativeBodyOwned112=false;receipt::calibrated=true;
    }
    release();temple();left.trigger=.9f;step();left.pose.pos_y=.12f;step();assert(receipt::visorRequests==2&&!receipt::visorClosed);
    std::puts("D39 actual ApplyUtilityGestures54: held-torch Grip, independent toggles, cancellation/rearm, native eligibility gates PASS");
}
