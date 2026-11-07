#include "../src/subsystems/doors/hand_only_h40.h"
#include "../src/subsystems/physics/grab_over_talk_ds.h"
// Actual extracted gameplay adapter + real gamepad arbitration; native item,
// tracking, combat and chat observations are explicit host leaves.
#include <windows.h>
#include <atomic>
#include "../src/core/native_grasp13.h"
#include "../src/subsystems/physics/corpse_grip_cg1.h"
#include <cstdio>
#include <cmath>
#include <cstring>
#include <limits>
#ifndef QUICK_TALK_HEADER_H37
#define QUICK_TALK_HEADER_H37 "../src/vrik/gesture_talk59c5.h"
#endif
#include QUICK_TALK_HEADER_H37
#include "../src/vrik/self_contact54.h"
#include "../src/native_vr_pad_premap.cpp"
namespace kcd2vr::frik_pose_probe {
static unsigned claimed=0,selfPatReadsR93R1=0;static int item[2]{1,0},physical[2]{};static bool holsterR93R1[2]{},selfPatR93R1=false;
int ExactHandItemState(int s) noexcept{return item[s];}
int ReadPhysicalHandHeld55(int s,std::uint64_t) noexcept{return physical[s];}
bool ReadHolsterGripOwnerR15(int side,std::uint64_t) noexcept{return holsterR93R1[side];}
bool ReadSelfPat54(kcd2vr::self_contact54::Event& out,std::uint64_t now) noexcept{++selfPatReadsR93R1;out={};out.serial=1;out.tick=now;return selfPatR93R1;}
void PublishQuickTalkOwnerH4(unsigned mask,std::uint64_t) noexcept{claimed=mask;}
}
namespace kcd2vr::combat_diag_probe {static bool combat=true;
int HenryInCombat178r31(std::uint64_t) noexcept{return combat?1:0;}}
namespace kcd2vr::diagnostic_trace {void Write(const char*) noexcept{}}
namespace kcd2vr::quick_talk_target_d78 {static bool suitable=true,requested=false;static bool Prepare(bool on,std::uint64_t){requested=on;return on&&suitable;}}
namespace kcd2vr::misc_gestures_d59 {static unsigned installs=0;static bool Ensure(std::uint64_t){++installs;return true;}}
namespace kcd2vr::action_time_owner59c11 {
static int pendingSide=-1;
static bool ReadPendingUse55(grasp13::PendingUse55& out){out={};if(pendingSide<0)return false;out={11,12,13,1000,pendingSide};return true;}
}
namespace kcd2vr {
static std::atomic<bool> g_gesture_talk59c5{true};
#ifndef QUICK_TALK_HOST_INC_H37
#define QUICK_TALK_HOST_INC_H37 "generated_quick_talk_h4.inc"
#endif
#include QUICK_TALK_HOST_INC_H37
}
using namespace kcd2vr;
namespace gt=kcd2vr::gesture_talk59c5;
static unsigned checks=0;
static bool check(bool value,const char* text){++checks;if(!value)std::printf("FAIL %s\n",text);return value;}
static void point(ControllerState& c){c.active=c.aim_active=true;c.grip=.8f;c.trigger=0;
 c.trigger_touch=false;c.aim_pose.pos_z=-.45f;c.aim_pose.pos_y=-.2f;}
static gt::Output step(std::uint64_t tick,native_vr_pad_premap::Context& c){
 hostTickCount64=tick;return PrepareGestureTalkH4(c,true);
}
int main(int argc,char** argv){
 if(argc!=2)return 2;const auto test=argv[1];bool ok=true;
 native_vr_pad_premap::Reset();
 auto& f=g_gesture_frame59c5;f.valid=true;f.head.qw=1;
 point(f.hands[1]);f.hands[0].active=f.hands[0].aim_active=true;
 f.hands[0].grip=.8f;f.hands[0].quick_access_touch=true;
 f.hands[1].thumb_x=-1; // competing old quick-weapon chord
 native_vr_pad_premap::Context c{};
 const bool leftWeapon=!std::strcmp(test,"left-weapon");
 if(leftWeapon){frik_pose_probe::item[0]=0;frik_pose_probe::item[1]=1;
  point(f.hands[0]);f.hands[1]={};f.hands[1].active=f.hands[1].aim_active=true;
  f.hands[1].grip=.9f;f.hands[1].trigger=.9f;}
 const int side=leftWeapon?0:1;auto& utility=f.hands[side];
 if(!std::strcmp(test,"weapon-charge"))f.hands[0].trigger=.9f;
 if(!std::strcmp(test,"capacitive-grip-only"))utility.trigger_touch=true;
 if(!std::strcmp(test,"upper-point-delayed")){utility.aim_pose.pos_z=-.4f;utility.aim_pose.pos_y=.3f;}
 if(!std::strcmp(test,"raise-only")){utility.aim_pose.pos_z=-.1f;utility.aim_pose.pos_y=.3f;}
 if(!std::strcmp(test,"support-hand"))frik_pose_probe::physical[1]=1;
 if(!std::strcmp(test,"occupied-hand"))frik_pose_probe::item[1]=1;
 if(!std::strcmp(test,"unknown-hand"))frik_pose_probe::item[1]=-1;
 if(!std::strcmp(test,"fist")||!std::strcmp(test,"partial-trigger"))utility.trigger=!std::strcmp(test,"fist")?.8f:.15f;
 if(!std::strcmp(test,"setting-off"))g_gesture_talk59c5=false;
 if(!std::strcmp(test,"no-suitable-npc"))quick_talk_target_d78::suitable=false;
 if(!std::strcmp(test,"pending-same-hand"))action_time_owner59c11::pendingSide=side;
 if(!std::strcmp(test,"pending-other-hand"))action_time_owner59c11::pendingSide=1-side;
 if(!std::strcmp(test,"corpse-same-hand"))corpse_grip_cg1::claims.consumed[side]=true;
 if(!std::strcmp(test,"corpse-other-hand"))corpse_grip_cg1::claims.consumed[1-side]=true;
 if(!std::strcmp(test,"no-point-idle"))utility.grip=.1f;
 if(!std::strcmp(test,"nonfinite-trigger"))utility.trigger=std::numeric_limits<float>::quiet_NaN();
 const bool refused=!std::strcmp(test,"support-hand")||!std::strcmp(test,"occupied-hand")||
  !std::strcmp(test,"unknown-hand")||!std::strcmp(test,"fist")||!std::strcmp(test,"partial-trigger")||
  !std::strcmp(test,"setting-off")||!std::strcmp(test,"nonfinite-trigger")||
  !std::strcmp(test,"no-suitable-npc")||!std::strcmp(test,"pending-same-hand")||!std::strcmp(test,"no-point-idle")||
  !std::strcmp(test,"corpse-same-hand")||!std::strcmp(test,"capacitive-grip-only");
 const bool later=!std::strcmp(test,"upper-point-delayed")||!std::strcmp(test,"raise-only");
 auto pending=step(1000,c);auto opened=step(later?1400:1300,c);
 if(!std::strcmp(test,"raise-only")){
  ok&=check(!pending.claimMaskH4&&!opened.openChat&&!opened.option,
   "optional pointing does not make a raised hand request battle cry");
  c.quick_talk_open54=true;const auto native=step(1430,c);
  ok&=check(!native.openChat&&!native.option,"raised hand cannot auto-select a native command");
  std::printf("quick_talk_combat_h37 case=%s checks=%u %s actualAdapter=EXTRACTED actualPad=PRODUCTION nativeExecution=NONE\n",test,checks,ok?"PASS":"FAIL");
  return ok?0:1;
 }
 if(refused){
  ok&=check(!pending.claimMaskH4&&!opened.claimMaskH4&&!opened.openChat&&!opened.option,
   "occupied/support/unknown/fist/Trigger/disabled input does not own quick talk");
  if(!std::strcmp(test,"pending-same-hand")||!std::strcmp(test,"no-point-idle")||!std::strcmp(test,"corpse-same-hand"))
   ok&=check(!quick_talk_target_d78::requested&&!misc_gestures_d59::installs,
    "idle or pickup-owned hand cannot install/query the chat descriptor bridge");
 }else{
  ok&=check(pending.openChat&&pending.claimMaskH4==(1u<<side),"qualified index point owns its native opener immediately");
  ok&=check(opened.openChat&&pending.fired==gt::Kind::Talk&&!opened.option,
   "actual combat adapter opens chooser without auto-taunt or blind face button");
  auto padCtx=c;padCtx.quick_talk_claim_h4=pending.claimMaskH4!=0;
  auto pad=native_vr_pad_premap::Build(f.hands[1],f.hands[0],padCtx);gt::apply54(pad,opened);
  ok&=check(pad.rb&&!pad.dpad_left&&!pad.dpad_right,"native opener owns old quick-weapon chord");
  if(!std::strcmp(test,"weapon-charge"))ok&=check(pad.rt>.8f,"weapon Trigger charge input retained");
  c.quick_talk_open54=true;
  if(!std::strncmp(test,"choice-owner-",13)){
   const auto held=step(1330,c);
   ok&=check(held.openChat&&held.claimMaskH4==(1u<<side),"native-open chooser has an original current hand before choice");
   const bool leased=!std::strncmp(test,"choice-owner-lease-",19);
   if(leased){
    f.hands[0].a_down=true;const auto first=step(1340,c);
    ok&=check(first.openChat&&first.claimMaskH4==(1u<<side),"a valid first choice starts native delivery");
    f.hands[0].a_down=false;const auto delivery=step(1345,c);
    ok&=check(delivery.openChat&&delivery.claimMaskH4==(1u<<side),"native delivery is active before current owner changes");
   }
   const bool relaxed=!std::strcmp(test,"choice-owner-relaxed");
   if(!std::strcmp(test,"choice-owner-tracking"))utility.active=false;
   else if(!std::strcmp(test,"choice-owner-occupied"))frik_pose_probe::item[side]=1;
   else if(!std::strcmp(test,"choice-owner-near"))grab_over_talk_ds::PublishLivingContact(side,1340);
   else if(!std::strcmp(test,"choice-owner-grip-nan"))utility.grip=std::numeric_limits<float>::quiet_NaN();
   else if(!std::strcmp(test,"choice-owner-trigger-nan"))utility.trigger=std::numeric_limits<float>::quiet_NaN();
   else if(!std::strcmp(test,"choice-owner-forward-nan"))utility.aim_pose.pos_z=std::numeric_limits<float>::quiet_NaN();
   else if(!std::strcmp(test,"choice-owner-up-inf"))utility.aim_pose.pos_y=std::numeric_limits<float>::infinity();
   else if(!std::strcmp(test,"choice-owner-lease-grip-nan"))utility.grip=std::numeric_limits<float>::quiet_NaN();
   else if(!std::strcmp(test,"choice-owner-lease-trigger-nan"))utility.trigger=std::numeric_limits<float>::quiet_NaN();
   else if(!std::strcmp(test,"choice-owner-lease-forward-nan"))utility.aim_pose.pos_z=std::numeric_limits<float>::quiet_NaN();
   else if(!std::strcmp(test,"choice-owner-lease-up-inf"))utility.aim_pose.pos_y=std::numeric_limits<float>::infinity();
   else if(relaxed){utility.grip=.1f;utility.aim_pose.pos_z=-.10f;}
   else return 2;
   if(leased)f.hands[0].b_down=true;else f.hands[0].a_down=true;
   const auto selected=step(leased?1350:1340,c);
   ok&=check(selected.openChat==relaxed&&selected.claimMaskH4==(relaxed?(1u<<side):0u),
     "explicit choice keeps a relaxed current owner and revokes an invalid or occupied owner");
   auto chosen=native_vr_pad_premap::Build(f.hands[1],f.hands[0],c);gt::apply54(chosen,selected);
   ok&=check((leased?chosen.b:chosen.a)&&!selected.option,"original native face-button choice is retained without invented extra option");
   f.hands[0].a_down=f.hands[0].b_down=false;c.quick_talk_open54=false;
   const auto closed=step(leased?1360:1350,c);
   ok&=check(!closed.openChat&&!closed.claimMaskH4&&!closed.option,"closed chooser cannot retain or replay the prior choice");
   std::printf("quick_talk_combat_h37 case=%s checks=%u %s actualAdapter=EXTRACTED actualPad=PRODUCTION nativeExecution=NONE\n",test,checks,ok?"PASS":"FAIL");
   return ok?0:1;
  }
  if(!std::strcmp(test,"partial-trigger-cancels"))utility.trigger=.15f;
  if(!std::strcmp(test,"fist-cancels"))utility.trigger=.8f;
  if(!std::strcmp(test,"tracking-loss"))utility.active=false;
  if(!std::strcmp(test,"context-revoke"))c.vr_menu=true;
  if(!std::strcmp(test,"grip-release"))utility.grip=.1f;
  const bool cancelled=!std::strcmp(test,"partial-trigger-cancels")||!std::strcmp(test,"fist-cancels")||
   !std::strcmp(test,"tracking-loss")||!std::strcmp(test,"context-revoke")||!std::strcmp(test,"grip-release");
  if(!std::strcmp(test,"held-jitter")){utility.grip=.5f;utility.trigger_touch=false;
   utility.aim_pose.pos_z=-.31f;}
  auto held=step(!std::strcmp(test,"upper-point-delayed")?1430:1330,c);
  if(cancelled)ok&=check(!held.openChat&&!held.option&&!held.claimMaskH4,
   "original utility Trigger/fist/tracking/context/Grip loss cancels held chooser");
  else ok&=check(held.openChat&&!held.option&&held.claimMaskH4==(1u<<side),
   "native-open chooser retains only original hand without automatic option");
  if(!std::strncmp(test,"choice-",7)){
   const int choice=test[7]-'1';
   if(choice==0)f.hands[0].a_down=true;else if(choice==1)f.hands[0].b_down=true;
   else if(choice==2)f.hands[1].x_down=true;else if(choice==3)f.hands[1].y_down=true;else return 2;
   const auto selected=step(1340,c);pad=native_vr_pad_premap::Build(f.hands[1],f.hands[0],c);gt::apply54(pad,selected);
   const bool actual[4]{pad.a,pad.b,pad.x,pad.y};
   for(int i=0;i<4;++i)ok&=check(actual[i]==(i==choice),"native chosen face button survives, no forced extra option");
   ok&=check(selected.openChat&&!selected.option,"selection frame remains owned by actual native chat");
   // A real selection must remain held beyond one render/pose frame. The
   // user may lower the pointing hand while physically pressing the option.
   const auto priorForward=utility.aim_pose.pos_z;utility.aim_pose.pos_z=-.10f;
   f.hands[0].a_down=f.hands[0].b_down=f.hands[1].x_down=f.hands[1].y_down=false;
   const auto delivery=step(1345,c);
   ok&=check(delivery.openChat&&delivery.claimMaskH4==(1u<<side)&&!delivery.option,
     "native-open choice retains opener through native input delivery without synthesizing another option");
   utility.aim_pose.pos_z=priorForward;
   f.hands[0].a_down=f.hands[0].b_down=f.hands[1].x_down=f.hands[1].y_down=false;
   c.quick_talk_open54=false;utility.grip=.1f;const auto after=step(1350,c);
   ok&=check(!after.openChat&&!after.option,"closed chooser cannot replay previous native option");
  }
 }
 std::printf("quick_talk_combat_h37 case=%s checks=%u %s actualAdapter=EXTRACTED actualPad=PRODUCTION nativeExecution=NONE\n",test,checks,ok?"PASS":"FAIL");
 return ok?0:1;
}
