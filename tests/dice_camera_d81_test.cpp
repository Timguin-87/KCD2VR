// Literal camera adapter and real shared eye/body lease. Native state/head,
// station selection, clock and diagnostic sink are the explicit host leaves.
#include "../src/activity_lens59c10r1.h"
#include "../src/vrik/dice_body_d81.h"
#include "../src/subsystems/roomscale/coordinate_d76.h"
#include <cstdio>
#include <cmath>
#include <cstring>
static std::uint64_t clockD81=100;
static std::uint64_t GetTickCount64() noexcept {return clockD81;}
namespace kcd2vr {
namespace ds=dialogue_state59c10r2;
namespace ae19=activity_eye59c19;
static ae19::State stateD81{};static ds::NativeHead headD81{};
static bool haveHeadD81=true,stationD81=false;
namespace activity_lens59c10r1 {
activity_eye59c19::State EyeState(std::uint64_t) noexcept {return stateD81;}
void NoteEye59c19(const EyeReceipt59c19&) noexcept {}
}
namespace frik_pose_probe {
bool NativeDialogueHead59c10r2(ds::NativeHead& h,std::uint64_t) noexcept {h=headD81;return haveHeadD81;}
}
namespace workstation178r20c1 {
struct WorkstationOwnerH51 {};
bool workstationOwnerH51(std::uint64_t,WorkstationOwnerH51&) noexcept {return stationD81;}
}
struct Matrix34f {float m[3][4];};
#include "generated_activity_eye_camera43.inc"
}
using namespace kcd2vr;
namespace db=dice_body_d81;
namespace ai=activity_input163;
static unsigned checks=0,failures=0;
static void check(bool good,const char* message){++checks;if(!good){++failures;std::printf("FAIL %s\n",message);}}
static Matrix34f matrix(float y,float z){return {{{1,0,0,10},{0,1,0,y},{0,0,1,z}}};}
static void reset(){
 auto& w=ai::witness175;w.Clear();w.ready=true;clockD81=100;stationD81=false;haveHeadD81=true;
 stateD81={};stateD81.tick=100;stateD81.lifetime=w.lifetime175.load();stateD81.entity=0x100;
 stateD81.known=stateD81.neutral=true;
 headD81={100,7,0x100,0x200,{10,20,1.7f},{},1,{10,20,0},true};
 (void)g_activity_neutral59c19.publish(ae19::Neutral{});(void)g_activity_anchor59c19.publish(ae19::Anchor{});db::retire();
 auto native=matrix(20.05f,1.756f);
 check(!ActivityEyeBase59c19(native,false),"actual cull learns neutral without changing gameplay camera");
 w.Begin(ai::Kind175::Dice,0x400,110);clockD81=110;
 stateD81.tick=110;stateD81.enteredAt=110;stateD81.lifetime=w.lifetime175.load();
 stateD81.kindMask=32;stateD81.active=true;stateD81.neutral=false;stateD81.eligible=ae19::eligibleKindsD81(32);
 headD81.tick=110;headD81.world={10,20.8f,1.2f};
}
int main(){
 reset();auto native=matrix(20.85f,1.256f);
 const auto witnessLife=ai::witness175.lifetime175.load();
 check(ActivityEyeBase59c19(native,false),"actual dice inspection cull uses accepted neutral eye");
 check(std::fabs(native.m[1][3]-20.05f)<.00001f&&std::fabs(native.m[2][3]-1.756f)<.00001f,
       "actual adapter removes authored inspection forward/down translation");
 db::Lease body{};check(db::read(body,110),"actual successful camera publication admits shared dice body");
 check(std::fabs(body.headLocal.y)<.00001f&&std::fabs(body.headLocal.z-1.7f)<.00001f,
       "body consumes same pre-entry anatomical head rather than leaned native head");
 check(std::fabs(native.m[1][3]-(headD81.modelOrigin.y+body.headLocal.y)-.05f)<.00001f,
       "camera and body retain measured eye-to-head offset");
 check(ai::witness175.dice.load()==0x400&&ai::witness175.lifetime175.load()==witnessLife,
       "camera/body presentation does not alter dice action lifetime");
 const auto captured=body;stateD81.eligible=false;native=matrix(20.85f,1.256f);
 check(!ActivityEyeBase59c19(native,false)&&!db::current(captured,110),
       "camera owner refusal retires prior body lease immediately");
 reset();native=matrix(20.85f,1.256f);check(ActivityEyeBase59c19(native,false),"new positive camera fixture");
 native_roomscale_d76::Origin other{};other.owner.entity=0x999;
 native=matrix(20.85f,1.256f);
 check(!ActivityEyeBase59c19(native,false,&other,9)&&!db::active(110),
       "incompatible current tracking origin cannot leave anchored body behind refused camera");
 reset();native=matrix(20.85f,1.256f);check(ActivityEyeBase59c19(native,false),"positive body epoch fixture");
 check(db::read(body,110),"captured old body epoch");headD81.epoch=8;
 native=matrix(20.85f,1.256f);
 check(ActivityEyeBase59c19(native,false)&&!db::current(body,110),"new current body epoch revokes a staged old transaction");
 headD81.scale=1.1f;native=matrix(20.85f,1.256f);
 check(!ActivityEyeBase59c19(native,false)&&!db::active(110),"body scale replacement refuses old neutral anatomy");
 reset();native=matrix(20.85f,1.256f);check(ActivityEyeBase59c19(native,false),"positive station precedence fixture");
 stationD81=true;
 check(!ActivityEyeBase59c19(native,false)&&!db::active(110),"exact workstation owner takes precedence and retires dice visual lease");
 reset();native=matrix(20.85f,1.256f);check(ActivityEyeBase59c19(native,false),"positive missing-head fixture");
 haveHeadD81=false;
 check(!ActivityEyeBase59c19(native,false)&&!db::active(110),"missing current native body refuses both anchored outputs");
 reset();native=matrix(20.85f,1.256f);check(ActivityEyeBase59c19(native,false),"positive native exit fixture");
 ai::witness175.End(0x400);check(!db::active(110),"native exit revokes lease before next camera callback");
 std::printf("D81 actual dice camera/body %u/%u; native leaves synthetic, headset NOT_RUN\n",checks-failures,checks);
 return failures?1:0;
}
