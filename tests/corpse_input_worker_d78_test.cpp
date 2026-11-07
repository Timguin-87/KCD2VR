// Both literal D78 mod.cpp CG1 blocks plus the literal returned-press handoff.
// Only XR/native candidate transport is mocked; the CG1, H40 raw-press, ordinary
// pulse and held-input producers are the production implementations.
#include <subsystems/physics/corpse_grip_cg1.h>
#include <subsystems/doors/hand_only_h40.h>
#include <vrik/physical_interaction.h>
#include <vrik/held_interaction114.h>
#include <tracking_sample59c6.h>
#include <cstdio>
using namespace kcd2vr;
namespace c=corpse_grip_cg1;
static std::uint64_t clockD78=1000;
static std::uint64_t GetTickCount64(){return clockD78;}
namespace frik_pose_probe {
static c::Candidate sample[2];static bool qualified[2]{},current[2]{true,true};
bool ReadCorpseGrabCandidateSideCg1(int side,c::Candidate& out,std::uint64_t,bool& fresh){fresh=current[side];out=sample[side];return qualified[side];}
bool ReadCorpseGrabCandidateCg1(c::Candidate& out,std::uint64_t now,bool& fresh){return ReadCorpseGrabCandidateSideCg1(1,out,now,fresh);}
}
template<class C> static C observe(C in,bool near){
 if constexpr(requires(C x){x.pendingOnlyD78;x.requestEpochD78;x.trackingEpochD78;}){
  in.pendingOnlyD78=near;in.requestEpochD78=5;in.trackingEpochD78=6;return in;
 }else return near?C{}:in;
}
static void witness(int side,std::uint64_t now,bool near){
 c::Key key{11,22,33,44,1,55,-1};key.physicsEpoch=8;key.route=c::Route::EntityVelocityD72;key.scriptToken=7;
 frik_pose_probe::sample[side]=observe(c::Candidate{key,now,false},near);frik_pose_probe::qualified[side]=true;
}
struct Fixture {
 struct Controller{float grip=0;bool grip_valid=true;};
 struct XR{Controller hand[2];std::uint64_t tick=0;const Controller& LeftController(){return hand[1];}const Controller& RightController(){return hand[0];}
  bool CopyInputSampleStamp59c6(tracking59c6::Stamp& s){s.tick=tick;s.epoch=tracking59c6::epoch.load();return true;}} xr;
 XR* xr_manager_=&xr;physical_interaction106::State f5Physical{};held_interaction114::State heldUse114{};
 bool neutral=true,menu=false;unsigned uses=0,holds=0;std::uint64_t returned[2]{},nativePress[2]{};
 void frame(std::uint64_t now,int side,float grip){
  clockD78=now;xr.tick=now;xr.hand[0].grip=side==0?grip:0;xr.hand[1].grip=side==1?grip:0;
  physical_interaction106::Input pi{};pi.enabled=true;pi.menu=menu;pi.now_ms=now;
  for(int s=0;s<2;++s){pi.hand[s]={true,true,neutral,xr.hand[s].grip,false};pi.hover_age_ms[s]=200;}
  std::uint64_t corpseReturnedPressD78[2]{};
#include <corpse_input_both_d78.inc>
  std::uint64_t fallbackPressH40[2]{};
  for(int s=0;s<2;++s){
   // Actual H40 spends its raw edge even when CG1 made this hand ineligible.
   (void)hand_only_door_h40::CandidatePress(s,0,now,pi.hand[s].tracked&&!pi.menu,xr.hand[s].grip,0);
   c::Request req{};c::handRequests[s].read(req);nativePress[s]=req.press;returned[s]=corpseReturnedPressD78[s];
  }
#include <corpse_returned_press_d78.inc>
  const auto po=physical_interaction106::Step(f5Physical,pi);
  const auto held114=held_interaction114::update(heldUse114,pi);
#include <corpse_held_press_d78.inc>
  uses+=po.fire_use;holds+=held114.hold;
 }
};
static unsigned checks=0,failed=0;
static void check(bool ok,const char* why){++checks;if(!ok){++failed;std::printf("FAIL %s\n",why);}}
int main(){
 hand_only_door_h40::transportInstalled=false; // raw press protocol runs; no native door query leaf
 for(int side=0;side<2;++side){
  const auto base=std::uint64_t(1000+side*1000);Fixture f;
  frik_pose_probe::qualified[1-side]=false;
  witness(side,base,true);f.frame(base,side,0);witness(side,base+10,true);f.frame(base+10,side,1);
  const auto doorPress=hand_only_door_h40::pressWhy[side].press;
  check(!f.uses&&!f.holds&&!f.nativePress[side]&&c::claims.consumed[side],"both worker blocks reserve before ordinary native input");
  witness(side,base+70,true);f.frame(base+70,side,1);
  witness(side,base+131,true);f.frame(base+131,side,1);
  check(f.returned[side]==base+10&&!f.nativePress[side],"both worker blocks return original timestamp only");
  check(f.heldUse114.pressed==base+10,"literal held-input handoff retains original occurrence time");
  check(hand_only_door_h40::pressWhy[side].press==doorPress,"expiry does not manufacture another door press");
  witness(side,base+151,true);f.frame(base+151,side,1);check(f.uses==1&&f.holds==1,"ordinary Use receives exactly the returned occurrence");
  witness(side,base+160,false);f.frame(base+160,side,1);check(!f.nativePress[side]&&f.uses==1,"late reachable corpse cannot steal returned ordinary press");
  f.frame(base+170,side,0);
 }
 for(int side=0;side<2;++side){
  const auto base=std::uint64_t(4000+side*1000);Fixture f;frik_pose_probe::qualified[1-side]=false;
  witness(side,base,true);f.frame(base,side,0);witness(side,base+10,true);f.frame(base+10,side,1);
  witness(side,base+60,false);f.frame(base+60,side,1);
  check(f.nativePress[side]&&!f.uses&&!f.holds,"same near corpse becomes native request without ordinary replay");
  const auto press=f.nativePress[side];witness(side,base+100,false);f.frame(base+100,side,1);
  check(f.nativePress[side]==press&&!f.returned[side],"both hands retain original consumed native occurrence");
  f.frame(base+110,side,0);
 }
 std::printf("D78 actual dual-hand worker: %u checks, %u failures\n",checks,failed);return failed?1:0;
}
