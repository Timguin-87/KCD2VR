#include "../src/subsystems/doors/hand_only_h40.h"
#include "../src/subsystems/physics/grab_over_talk_ds.h"
#include <windows.h>
#include <atomic>
#include "../src/core/native_grasp13.h"
#include "../src/subsystems/physics/corpse_grip_cg1.h"
#include <cstdio>
#include <cmath>
#include "../src/vrik/gesture_talk59c5.h"
#include "../src/vrik/self_contact54.h"
#include "../src/native_vr_pad_premap.cpp"
namespace kcd2vr::frik_pose_probe {
static unsigned claimed=0;static int empty[2]{0,0};
int ExactHandItemState(int s)noexcept{return empty[s];}int ReadPhysicalHandHeld55(int,std::uint64_t)noexcept{return 0;}
bool ReadHolsterGripOwnerR15(int,std::uint64_t) noexcept{return false;}
bool ReadSelfPat54(kcd2vr::self_contact54::Event&,std::uint64_t)noexcept{return false;}
void PublishQuickTalkOwnerH4(unsigned m,std::uint64_t)noexcept{claimed=m;}
}
namespace kcd2vr::combat_diag_probe {int HenryInCombat178r31(std::uint64_t)noexcept{return 0;}}
namespace kcd2vr::diagnostic_trace {void Write(const char*)noexcept{}}
namespace kcd2vr::quick_talk_target_d78 {static bool suitable=true;static bool Prepare(bool on,std::uint64_t){return on&&suitable;}}
namespace kcd2vr::misc_gestures_d59 {static bool Ensure(std::uint64_t){return true;}}
namespace kcd2vr::action_time_owner59c11 {
static int pendingSide=-1;
static bool ReadPendingUse55(grasp13::PendingUse55& out){out={};if(pendingSide<0)return false;out={11,12,13,1000,pendingSide};return true;}
}
namespace kcd2vr {
static std::atomic<bool> g_gesture_talk59c5{true};
#include "generated_quick_talk_h4.inc"
}
int main(){using namespace kcd2vr;ControllerState l{},r{};l.active=r.active=l.aim_active=r.aim_active=true;l.grip=.8;r.quick_access_touch=true;l.thumb_x=-1;l.trigger_touch=false;l.aim_pose.pos_z=-.45;l.aim_pose.pos_y=-.2;
 g_gesture_frame59c5.valid=true;g_gesture_frame59c5.head.qw=1;g_gesture_frame59c5.hands[0]=r;g_gesture_frame59c5.hands[1]=l;native_vr_pad_premap::Context c{};hostTickCount64=1000;
 auto pending=PrepareGestureTalkH4(c,true);auto pad=native_vr_pad_premap::Build(l,r,c);gesture_talk59c5::apply54(pad,pending);if(frik_pose_probe::claimed!=2||!pending.openChat)return 1; // immediate qualified index point owns the opener
 hostTickCount64=1300;c.quick_talk_open54=true;g_gesture_frame59c5.hands[1].aim_pose.pos_z=-.31;auto opened=PrepareGestureTalkH4(c,true);if(!opened.openChat)return 2;
 c.quick_talk_open54=true;hostTickCount64=1310;g_gesture_frame59c5.hands[1].trigger_touch=true;g_gesture_frame59c5.hands[1].aim_pose.pos_z=-.31;if(PrepareGestureTalkH4(c,true).openChat)return 3; // index curl ends retention
 hostTickCount64=1320;c.vr_menu=true;auto off=PrepareGestureTalkH4(c,true);if(off.openChat||frik_pose_probe::claimed)return 4;
 std::puts("quick_talk_host_h4=4/4 actualAdapter=EXTRACTED_CURRENT emptyItem/nativeObserverLeaves=EXPLICIT");return 0;
}
