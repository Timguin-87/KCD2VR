// Literal current gameplay adapter and actual native pad reducer. Native target,
// item, tracking and chat-open observations are explicit fixture leaves.
#include <string>
#define main retained_quick_talk_h37_main
#include "quick_talk_combat_h37_test.cpp"
#undef main
static unsigned checksR93R1=0,failedR93R1=0;
static void ckR93R1(bool ok,const char* label){++checksR93R1;if(!ok)++failedR93R1;std::printf("%s %s\n",ok?"PASS":"FAIL",label);}
int main(int argc,char** argv){
 if(argc!=2)return 2;const std::string which=argv[1];
 auto& f=g_gesture_frame59c5;f.valid=true;f.head.qw=1;point(f.hands[1]);f.hands[0].active=f.hands[0].aim_active=true;
 f.hands[0].grip=.9f;f.hands[0].trigger=.9f; // opposite equipped weapon retains its input
 native_vr_pad_premap::Context c{};native_vr_pad_premap::Reset();auto& hand=f.hands[1];
 if(which=="dog-low")hand.aim_pose.pos_y=-.75f;
 if(which=="index-on")hand.trigger_touch=true;
 if(which=="setting-off")g_gesture_talk59c5=false;
 if(which=="no-target")quick_talk_target_d78::suitable=false;
 if(which=="held")frik_pose_probe::item[1]=7;
 if(which=="support")frik_pose_probe::physical[1]=1;
 if(which=="unknown")frik_pose_probe::item[1]=-1;
 if(which=="holster")frik_pose_probe::holsterR93R1[1]=true;
 if(which=="pickup")action_time_owner59c11::pendingSide=1;
 if(which=="corpse")corpse_grip_cg1::claims.consumed[1]=true;
 if(which=="door")hand_only_door_h40::claims[1]=91;
 if(which=="near-grab")grab_over_talk_ds::PublishLivingContact(1,1000);
 if(which=="trigger")hand.trigger=.11f;
 if(which=="inward"){hand.aim_pose.qy=1;hand.aim_pose.qw=0;}
 if(which=="behind")hand.aim_pose.pos_z=.45f;
 if(which=="too-close")hand.aim_pose.pos_z=-.2f;
 if(which=="too-low")hand.aim_pose.pos_y=-1.1f;
 if(which=="nonfinite")hand.trigger=std::numeric_limits<float>::quiet_NaN();
 if(which=="raise"){hand.aim_pose.pos_z=-.1f;hand.aim_pose.pos_y=.4f;}
 if(which=="self-pat"){hand.grip=.1f;frik_pose_probe::selfPatR93R1=true;}
 const bool refused=which=="index-on"||which=="setting-off"||which=="no-target"||which=="held"||which=="support"||which=="unknown"||which=="holster"||which=="pickup"||which=="corpse"||which=="door"||which=="near-grab"||which=="trigger"||which=="inward"||which=="behind"||which=="too-close"||which=="too-low"||which=="nonfinite"||which=="raise"||which=="self-pat";
 auto first=step(1000,c);
 if(refused){
  ckR93R1(!first.openChat&&!first.claimMaskH4&&!first.option,"invalid or occupied point emits no opener or automatic command");
  if(which=="near-grab")grab_over_talk_ds::PublishLivingContact(1,1401); // current contact, not an expired 250 ms witness
  auto later=step(1401,c);ckR93R1(!later.openChat&&!later.option,"dwell cannot turn refused point or raised hand into a command");
  if(which=="self-pat")ckR93R1(frik_pose_probe::selfPatReadsR93R1==0,"optional pointing does not consume self-pat or request battle cry");
 }else if(which=="x"){
  // Explicit X keeps the current short-press route with optional gesture OFF.
  g_gesture_talk59c5=false;c={};native_vr_pad_premap::Reset();f.hands[1].x_down=true;hostTickCount64=2000;
  native_vr_pad_premap::Build(f.hands[1],f.hands[0],c);f.hands[1].x_down=false;hostTickCount64=2050;
  const auto pad=native_vr_pad_premap::Build(f.hands[1],f.hands[0],c);ckR93R1(pad.x,"controller X Quick Talk stays available independently of pointing preference");
 }else{
  ckR93R1(first.openChat&&first.fired==gt::Kind::Talk&&!first.option&&first.claimMaskH4==2,"first qualified index point opens native chooser immediately with no automatic option");
  auto context=c;context.quick_talk_claim_h4=first.claimMaskH4||first.openChat;
  auto pad=native_vr_pad_premap::Build(f.hands[1],f.hands[0],context);gt::apply54(pad,first);
  ckR93R1(pad.rb&&!pad.x&&!pad.b&&pad.rt>.8f,"RB opener leaves native choices and opposite weapon Trigger intact");
  c.quick_talk_open54=true;
  if(which=="rearm"){
   ckR93R1(step(1010,c).openChat,"native chooser observes original point owner");c.quick_talk_open54=false;
   ckR93R1(!step(1020,c).openChat&&!step(2000,c).openChat,"one held point never reopens a natively closed menu");
   hand.trigger_touch=true;step(2100,c);hand.trigger_touch=false;
   ckR93R1(!step(2200,c).openChat,"neutral then new point preserves original 1500 ms cooldown");
   ckR93R1(step(2500,c).openChat,"deliberate rearmed point opens when original cooldown ends");
   std::printf("R93R1 point case=%s checks=%u failures=%u\n",which.c_str(),checksR93R1,failedR93R1);return failedR93R1?1:0;
  }
  for(std::uint64_t now=1010;now<=3010;now+=100){const auto held=step(now,c);ckR93R1(held.openChat&&!held.option&&held.claimMaskH4==2,"keep pointing retains original native chooser without automatic command");}
  if(which.rfind("choice",0)==0){
   const int chosen=which.back()-'1';if(chosen<0||chosen>3)return 2;
   if(chosen==0)f.hands[0].a_down=true;if(chosen==1)f.hands[0].b_down=true;if(chosen==2)f.hands[1].x_down=true;if(chosen==3)f.hands[1].y_down=true;
   const auto out=step(3020,c);pad=native_vr_pad_premap::Build(f.hands[1],f.hands[0],c);gt::apply54(pad,out);
   const bool options[4]{pad.a,pad.b,pad.x,pad.y};for(int i=0;i<4;++i)ckR93R1(options[i]==(i==chosen),"manual native choice preserved without extra command");
   f.hands[0].a_down=f.hands[0].b_down=f.hands[1].x_down=f.hands[1].y_down=false;hand.trigger_touch=true;hand.grip=.1f;
   ckR93R1(step(3040,c).openChat,"bounded manual selection delivery may finish after lowering the pointing hand");
   ckR93R1(!step(3141,c).openChat,"selection delivery does not become a frozen pointing owner");
  }else{
   if(which=="grip-release")hand.grip=.1f;
   else if(which=="tracking-loss")hand.active=false;
   else if(which=="new-held")frik_pose_probe::item[1]=7;
   else if(which=="menu")c.vr_menu=true;
   else hand.trigger_touch=true;
   ckR93R1(!step(3020,c).openChat,"index curl, Grip release, ownership/context or tracking loss closes chooser immediately");
  }
 }
 std::printf("R93R1 point case=%s checks=%u failures=%u\n",which.c_str(),checksR93R1,failedR93R1);return failedR93R1?1:0;
}
