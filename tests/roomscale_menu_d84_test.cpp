// Real menu reducers, Mod getter/apply/F1 statements, D76 settings and host slots.
// Native installation/admission and camera publication are the controlled leaves.
#include "subsystems/experimental/live_settings_d39.h"
#include "subsystems/roomscale/settings_d76.h"
#include "vrik/roomscale58.h"
#include "combat_menu49.h"
#include <atomic>
#include <cstdio>
#include <cwchar>

using namespace kcd2vr;
namespace live=kcd2vr::live_settings_d39;
namespace kcd2vr::native_roomscale_d76 {
static int installs=0,cancels=0;static bool admitted=false;
bool Install() noexcept {++installs;return true;}
void CancelPending() noexcept {++cancels;}
void SetAdmission(bool on) noexcept {admitted=on;}
}
namespace CameraHook {static bool tracking=false;static void SetRoomscaleFollow(bool on){tracking=on;}}
namespace frik_pose_probe {
static bool body=false,combat=false;
static void SetBodyFollow128(bool on){body=on;}
static bool CombatExperiment175Enabled(){return combat;}
static void SetLegacyHolsterLayout147(bool){}
}
namespace diagnostic_trace {static void Write(const char*){}}
static std::atomic<bool> vrik_body_follow_{false},user_shoulder_holsters_{false};
static std::atomic<bool> g_roomscale_follow58{false},g_roomscale_dodge58{false};
static bool PtIn(int,int,int){return true;}
static void RequestMenuSave(bool){}
static void LabLadderCue(int){}
static void save(){}
static void MarkDirty(){}
#include "roomscale_menu_source.inc"
static int failures=0,checks=0;
static void check(bool ok,const char* reason){++checks;if(!ok){++failures;std::printf("FAIL D84 %s\n",reason);}}
static void reset(bool native=false,bool legacy=false,bool dodge=false,bool body=true){
 roomscale_d76::nativeSteps=native;roomscale_d76::movementContext=false;roomscale_d76::recenterPending=false;
 roomscale_d76::selectionRevision=1;native_roomscale_d76::installs=native_roomscale_d76::cancels=0;
 g_roomscale_follow58=legacy;g_roomscale_dodge58=dodge;vrik_body_follow_=body;
 user_shoulder_holsters_=false;frik_pose_probe::body=body;frik_pose_probe::combat=false;CameraHook::tracking=false;
 Hotkey(false,false); // release the actual F1 edge before another scenario
}
static void selected(bool native,bool legacy,bool dodge,bool body,const char* reason){
 check(roomscale_d76::Enabled()==native&&g_roomscale_follow58.load()==legacy&&g_roomscale_dodge58.load()==dodge&&vrik_body_follow_.load()==body,reason);
}
int main(){
 for(bool native:{false,true})for(bool legacy:{false,true})for(bool dodge:{false,true})for(bool body:{false,true}){
  reset(native,legacy,dodge,body);auto s=Fill();
  const bool walking=native||legacy;check(s.roomscale_mode==(walking?(dodge?2:1):(dodge?3:0)),"getter joins native and legacy walking");
  Apply(s);selected(native,legacy&&!native,dodge,body,"unrelated apply preserves explicit OFF or existing backend");
  check(!native_roomscale_d76::installs&&!native_roomscale_d76::cancels,"unrelated apply creates no native transition");
  check(CameraHook::tracking==((walking&&body)||dodge),"tracking selection includes native physical walking");
  s=Fill();live::advance(s,live::Id::BodyFollow);Apply(s);
  selected(native,legacy&&!native,dodge,!body,"torso choice cannot enable or disable physical walking/dodge");
 }
 // The three existing physical-walking controls all reach the same owner.
 for(int control=0;control<3;++control)for(bool dodge:{false,true}){
  reset(false,false,dodge);auto s=Fill();
  if(control==0)ClickWalking(s);
  else if(control==1){live::advance(s,live::Id::NativeRoomscaleD76);Apply(s);}
  else {s.roomscale_mode=0;g_roomscale_dodge58=false;ClickCycle(s);}
  selected(true,false,control==2?false:dodge,true,"walking ON selects finite D76, no offset servo");
  check(native_roomscale_d76::installs==1&&native_roomscale_d76::cancels==1,"one native selection/install per explicit ON");
  s=Fill();check(s.live_d39.nativeRoomscaleD76,"all controls re-read canonical native preference");
  if(control==0)ClickWalking(s);else {live::advance(s,live::Id::NativeRoomscaleD76);Apply(s);}
  selected(false,false,control==2?false:dodge,true,"walking OFF retires finite owner without legacy fallback");
 }
 // Live four-state cycle is explicit selection; both it and the native toggle
 // keep the duplicate row coherent before D40 LIVE APPLY runs.
 reset();for(int mode:{1,2,3,0}){auto s=Fill();live::advance(s,live::Id::Roomscale);Apply(s);
  check(Fill().roomscale_mode==mode,"live four-state mode round trip");
  selected(mode==1||mode==2,false,mode>=2,true,"live mode selects finite walking and independent dodge");}
 // A saved legacy configuration remains legacy until the user makes a choice.
 reset(false,true,true);auto s=Fill();live::advance(s,live::Id::NativeRoomscaleD76);Apply(s);
 selected(true,false,true,true,"explicit native ON migrates legacy walking once and preserves dodge");
 s=Fill();ClickDodge(s);selected(true,false,false,true,"old dodge-only button preserves selected finite walking");
 s=Fill();ClickDodge(s);selected(true,false,true,true,"old dodge-only button can re-enable dodge independently");
 // F1 is torso-only; an explicit physical-walking OFF must survive it.
 reset(false,false,true,false);Hotkey(false,true);selected(false,false,true,true,"F1 torso ON does not restore continuous FOLLOW");
 Hotkey(false,false);Hotkey(false,true);selected(false,false,true,false,"F1 torso OFF preserves dodge-only");
 reset(true,false,false,true);Hotkey(false,true);selected(true,false,false,false,"F1 preserves native walking preference while body paused");
 check(!CameraHook::tracking,"body OFF pauses walking-only tracking");
 Hotkey(false,false);Hotkey(false,true);check(CameraHook::tracking,"body ON resumes selected native walking tracking");
 // Body and physical-walking OFF stay off after unrelated interaction settings.
 reset(false,false,false,false);s=Fill();live::advance(s,live::Id::PickupCameraBridgeD55);Apply(s);
 selected(false,false,false,false,"unrelated preference cannot migrate explicit OFF");
 std::printf("roomscale menu: %d checks, %d failures\n",checks,failures);return failures?1:0;
}
