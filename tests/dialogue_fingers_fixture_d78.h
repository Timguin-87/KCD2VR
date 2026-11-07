#pragma once
// Portable harness for literal production packet/trigger/finger-writer spans.
// Native services, authored curve acquisition and final rendering are explicit
// fixture leaves. The retained full-TU test uses neither this header nor leaves.
#include <windows.h>
#include <array>
#include <atomic>
#include <cmath>
#include <cstring>
#include "core/snapshot_mailbox.h"
#include "tracking_sample59c6.h"
#include "vrik/tracking_grace111.h"
#include "vrik/reading_pose32.h"
#include "vrik/pose_math.h"
#include "vrik/finger_surface.h"
#include "vrik/finger_tuning40.h"
#if __has_include("vrik/dialogue_fingers_d78.h")
#include "vrik/dialogue_fingers_d78.h"
#endif
namespace kcd2vr::haptics101 {inline std::atomic<bool> allowed{false};}
namespace kcd2vr::animation_filter {inline std::atomic<bool> nativeBodyOwned112{false};}
namespace kcd2vr::diagnostics::lab {inline bool NativeReference(){return false;}}
namespace kcd2vr::diagnostics::hands28 {inline bool selected(){return false;}inline bool active(std::uint64_t){return false;}}
namespace kcd2vr::contact176 {inline bool fresh(std::uint64_t tick,std::uint64_t now,std::uint64_t max){return tick&&now>=tick&&now-tick<=max;}}
namespace kcd2vr::frik_pose_probe {
namespace pc=kcd2vr::pose_core;
namespace tracking59c6=kcd2vr::tracking59c6;
namespace tracking_grace111=kcd2vr::tracking_grace111;
#include "generated_dialogue_snapshot_d78.inc"
struct TrackingPose {float px=0,py=0,pz=0,qx=0,qy=0,qz=0,qw=1;};
struct QuatT28 {float q[4]{0,0,0,1},t[3]{};};
inline pc::Quat PQ(Q4 q){return {q.x,q.y,q.z,q.w};}
inline Q4 QP(pc::Quat q){return {float(q.x),float(q.y),float(q.z),float(q.w)};}
inline Q4 QFromQuatT(const QuatT28& q){return {q.q[0],q.q[1],q.q[2],q.q[3]};}
inline void QToQuatT(QuatT28& q,Q4 v){q.q[0]=v.x;q.q[1]=v.y;q.q[2]=v.z;q.q[3]=v.w;}
inline V3 VP(pc::Vec3 p){return {float(p.x),float(p.y),float(p.z)};}
inline void ToT(QuatT28& q,V3 p){q.t[0]=p.x;q.t[1]=p.y;q.t[2]=p.z;}
inline std::atomic<bool> g_ctx_dialogue{false},g_dialogue_body178r9{false},g_gameplay_allowed{false};
inline std::atomic<bool> g_head_valid{false},g_left_valid{false},g_right_valid{false},g_open_bind_enabled105{false};
inline std::atomic<float> g_left_grip{0},g_right_grip{0},g_left_trigger{0},g_right_trigger{0};
inline std::atomic<std::uint64_t> g_track_seq{0},g_tracking_tick6689{0},g_pose_epoch6689{0},g_weapon_request_epoch6694{0},g_fist_epoch101{1};
inline int g_census_mode=0,g_cal_state=0,g_mode=0;
inline kcd2vr::core::SnapshotMailbox<TrackingSnapshot> g_tracking_frames6695;
inline bool ReadTrackingSnapshot(TrackingSnapshot& s){return g_tracking_frames6695.read(s);}
inline void LifecycleTracking36(const TrackingSnapshot&,bool){}
struct FingerChainD78 {int count=0;std::array<int,5> id{};};
struct EndpointD78 {bool ready=false;std::array<QuatT28,32> local{};};
struct CoherentState6689 {
    unsigned count=0;struct {int hand_r=0,hand_l=1;} rig;
    void* character=nullptr;
    std::array<int,50> parent{};std::array<QuatT28,50> restLocal{},local{},rest{};
    std::array<bool,50> dirty{};bool fingerMap[2]{};
    FingerChainD78 fingers[2][5]{};EndpointD78 open[2]{},closed[2]{},nativeCurveClosedH7[2]{};
    bool freeObserved28[2]{},freeClampObserved28[2][5]{};double freeCurlObserved28[2][5]{};
    float hoverW178r14[2]{};
};
struct HandContact6695 {bool fingerContact[5]{};double curl[5]{};};
struct CoherentTransaction6689 {
    std::uint64_t epoch=0;bool traceH29=false,fingerDiagnostic28=false;
    struct {bool fingerCall=false,fingerOk=false;} traceArmH29[2];
    bool fingerTrialH7[2]{};float fingerOpenBiasH7[2]{};
};
inline bool HandClaimBlockedT24(int,std::uintptr_t,std::uint64_t,std::uint64_t){return false;}
inline bool FistSelected101(){return true;}
inline bool FistModePermits178r22c2(){return true;}
struct FistOwner101 {bool empty=true;std::uintptr_t player=1,manager=2;std::uint64_t epoch=7,request=1,tick=10000;};
struct FistOwnerMailboxD78 {bool read(FistOwner101& out){out={};out.tick=GetTickCount64();return true;}};
inline FistOwnerMailboxD78 g_fist_owner101;
// Native endpoint acquisition is outside this input-route test. Supply a
// coherent measured curve; the real writer/blender still produces joint locals.
inline void FreeFingerCurveH7(const CoherentState6689& s,int side,int finger,float,bool,pc::FingerCurve103& out){
    out={};out.count=1;out.open[0]=PQ(QFromQuatT(s.open[side].local[finger]));
    out.closed[0]=PQ(QFromQuatT(s.closed[side].local[finger]));
    const auto& t=s.restLocal[s.fingers[side][finger].id[0]].t;out.offset[0]={t[0],t[1],t[2]};
}
#include "generated_dialogue_packet_d78.inc"
#include "generated_dialogue_functions_d78.inc"
}
