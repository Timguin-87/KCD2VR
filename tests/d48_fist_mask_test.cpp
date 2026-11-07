// Verbatim FistHandMaskD48 from the production source; the actual copied-input
// mailbox remains in use. Clock/current epoch are explicit host leaves.
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include "core/snapshot_mailbox.h"
struct QuatT28{float q[4]{0,0,0,1};float t[3]{};};
#include "arm_types_d47.inc"
static std::uint64_t now=1000;
static std::uint64_t GetTickCount64(){return now;}
namespace tracking59c6 {inline std::atomic<std::uint64_t> epoch{5};}
static kcd2vr::core::SnapshotMailbox<TrackingSnapshot> g_tracking_frames6695;
#include "fist_mask_d48.inc"
static int checks=0,failures=0;
static void ck(bool ok,const char* message){++checks;if(!ok){++failures;std::printf("FAIL %s\n",message);}}
static TrackingSnapshot fresh(){TrackingSnapshot t{};t.tick=t.input_tick=now;t.sequence=3;t.source_epoch=5;t.allowed=t.head_valid=t.right_valid=t.left_valid=true;return t;}
static unsigned mask(const TrackingSnapshot& t){ck(g_tracking_frames6695.publish(t),"fixture publishes one coherent copied input");return FistHandMaskD48();}
int main(){
 ck(FistHandMaskD48()==0,"missing copied input reserves neither hand");
 auto t=fresh();ck(mask(t)==0,"open hands reserve neither hand");
 t.right_grip=t.right_trigger=1;ck(mask(t)==1,"right fist reserves only right hand");
 t.left_grip=t.left_trigger=1;ck(mask(t)==3,"two fists reserve both hands");
 t.right_grip=0;ck(mask(t)==2,"opening right grip cancels right reservation immediately");
 t.right_grip=1;t.right_trigger=0;ck(mask(t)==2,"opening right trigger cancels right reservation immediately");
 t=fresh();t.left_grip=t.left_trigger=1;ck(mask(t)==2,"left fist reserves only left hand");
 t.left_trigger=.60f;ck(mask(t)==0,"threshold exactly .60 does not silently close a fist");
 t=fresh();t.right_grip=t.right_trigger=1;t.left_valid=false;ck(mask(t)==1,"invalid opposite hand cannot revoke current right fist reservation");
 t.left_valid=true;t.left_held=true;ck(mask(t)==1,"held opposite hand cannot revoke current right fist reservation");
 t=fresh();t.left_grip=t.left_trigger=1;t.right_valid=false;ck(mask(t)==2,"invalid opposite hand cannot revoke current left fist reservation");
 t.right_valid=true;t.right_held=true;ck(mask(t)==2,"held opposite hand cannot revoke current left fist reservation");
 t=fresh();t.right_grip=t.right_trigger=t.left_grip=t.left_trigger=1;t.right_held=true;ck(mask(t)==2,"held right controller contributes no right fist permission");
 t=fresh();t.right_grip=t.right_trigger=t.left_grip=t.left_trigger=1;t.left_held=true;ck(mask(t)==1,"held left controller contributes no left fist permission");
 t=fresh();t.right_grip=t.right_trigger=1;t.tick=t.input_tick=now-80;ck(mask(t)==1,"current copied input at inclusive age limit remains usable");
 t.tick=t.input_tick=now-81;ck(mask(t)==0,"stale copied buttons cannot retain a fist reservation");
 t.tick=t.input_tick=0;ck(mask(t)==0,"unstamped input has no fist reservation");
 t.tick=t.input_tick=now+1;ck(mask(t)==0,"future input has no fist reservation");
 t=fresh();t.right_grip=t.right_trigger=1;t.source_epoch=6;ck(mask(t)==0,"obsolete tracking source has no fist reservation");
 t=fresh();t.right_grip=t.right_trigger=1;t.allowed=false;ck(mask(t)==0,"disallowed copied input contributes no fist reservation");
 t.allowed=true;t.head_valid=false;ck(mask(t)==0,"invalid head contributes no hand permission");
 t.head_valid=true;t.head_held=true;ck(mask(t)==0,"held head contributes no hand permission");
 t=fresh();t.left_grip=t.left_trigger=1;t.tick=t.input_tick=now-20;ck(mask(t)==2,"current input recovers immediately after an invalid/stale sample");
 std::printf("D48 extracted fist reservation: %d/%d PASS; no native/headset execution.\n",checks-failures,checks);return failures?1:0;
}
