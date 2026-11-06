// Actual extracted source/mode/prelude + real guard header. Native resolver,
// hardware tracking provider and native guard call tail remain explicit leaves.
#include "../src/vrik/fist_guard175.h"
#include "../src/vrik/user_modes175.h"
#include "../src/core/snapshot_mailbox.h"
#include <atomic>
#include <cstdio>
#include <mutex>
#include <array>
#include "hand_claim_fixture_t24.h"
namespace fm=kcd2vr::fists;
namespace p=kcd2vr::pose_core;
struct TrackingSnapshot {bool allowed=true,head_valid=true,right_valid=true,left_valid=true,head_held=false,right_held=false,left_held=false;
 std::uint64_t sequence=1,tick=1000,timeNs=1000000000; p::Vec3 head{0,1.7,0},right{.16,1.57,-.2},left{-.16,1.57,-.2};p::Quat hq{};
 float right_grip=.8f,left_grip=.8f,right_trigger=.8f,left_trigger=.8f;};
struct CommittedFrame6695 {struct Stamp {std::uint64_t epoch=1,sample=1;std::uintptr_t actor=42;}stamp;
 p::Vec3 allowed[2]{{90,80,70},{-90,-80,-70}},itemHelper[2]{{-40,-30,-20},{40,30,20}};float avatarScale=3;};
struct FistContactFrameH11 {bool valid[2]{};}; // Nonauthorizing contact metadata sink.
struct FistOwner101 {void* player=nullptr;void* manager=nullptr;std::uint64_t tick=0,epoch=0,request=0;bool empty=false,responsiveCombatH27=false;};
struct FistFrame101 {fm::Sample sample{};fm::Punch punch{};void* player=nullptr;std::uint64_t request=0;};
static kcd2vr::core::SnapshotMailbox<FistOwner101> g_fist_owner101;
static kcd2vr::core::SnapshotMailbox<FistFrame101> g_fist_frame101;
static std::uint64_t nowH6=1000;static std::uint64_t GetTickCount64(){return nowH6;}
static std::atomic<int> g_weapon_control_mode{0},g_census_mode{0};
static std::atomic<bool> g_combat_experiment175{false},g_normal_fists44{false};
static std::atomic<std::uint64_t> g_f4_fists_tick178r22c2{0},g_f4_fists_epoch178r22c2{0},g_pose_epoch6689{1},g_fist_epoch101{1};
static std::atomic<std::uint64_t> g_weapon_request_epoch6694{1};
static std::array<unsigned char,0x100> claimPlayerH6{};
static std::atomic<bool> g_fist_fault101{false},g_gameplay_allowed{true},g_head_valid{true},g_right_valid{true},g_left_valid{true};
static bool FullWeaponMode120(int mode){return mode==7;} // existing mode-number contract leaf, not a native call
static int FocusedTestMode167(){return 0;}
namespace kcd2vr::workstation178r20c1 {static bool owned=false;static bool ownsWrist(std::uint64_t){return owned;}}
namespace diagnostics::lab {static bool reference=false;static bool NativeReference(){return reference;}}
namespace animation_filter {static std::atomic<bool> nativeBodyOwned112{false};}
static std::recursive_mutex g_fist_native_mutex101;
struct GuardState {kcd2vr::fist_guard175::State fists175;};static GuardState g_native_guard112{};
struct NativeOwner104 {};
static bool NativeResolve104(void*,NativeOwner104&){return false;} // no engine object or native instruction
static bool NativeHmdGuardYieldH24(const NativeOwner104&,std::uint64_t){return false;}
static bool NativeKickGuardFenceR19(void*,std::uint64_t){return false;}
static struct {bool prepared=false,waitingGuardU16=false;}g_fist_native101;
static unsigned releasesH6=0;
static void NativeGuardRelease112(const char* ="OWNER_CLEANUP"){++releasesH6;g_native_guard112={};}
static kcd2vr::fist_guard175::Decision lastDecisionH6{};
#include <face_guard_h6_actual.inc>
static unsigned checks=0,failures=0;
static void ck(bool b,const char* why){++checks;if(!b){++failures;std::printf("FAIL %s\n",why);}}
static void publicMode(kcd2vr::user175::Mode mode){const auto c=kcd2vr::user175::configuration(mode);
 g_census_mode=c.pose;g_weapon_control_mode=c.weapon;g_combat_experiment175=c.experiment;
 // Exact empty-hand selection source is a native-pump leaf. Test stamps it
 // current, never a manually force-enabled alternate public mode.
 g_f4_fists_tick178r22c2=nowH6;g_f4_fists_epoch178r22c2=g_pose_epoch6689.load();}
struct Rig {
 TrackingSnapshot t{};CommittedFrame6695 packet{};FistOwner101 owner{};
 Rig(){nowH6=1000;g_pose_epoch6689=1;g_fist_epoch101=1;g_gameplay_allowed=g_head_valid=g_right_valid=g_left_valid=true;
  g_fist_fault101=false;g_normal_fists44=false;animation_filter::nativeBodyOwned112=false;
  kcd2vr::workstation178r20c1::owned=false;diagnostics::lab::reference=false;
  hand_claim_fixture_t24::Reset(claimPlayerH6.data(),42);g_weapon_request_epoch6694=1;
  g_fist_native101={};g_native_guard112={};owner.player=claimPlayerH6.data();owner.empty=true;owner.epoch=owner.request=1;owner.tick=nowH6;
  publicMode(kcd2vr::user175::Mode::CombatSynchronized);g_fist_owner101.publish(owner);}
 void step(unsigned dt=10,bool refreshOwner=true,bool refreshPose=true){nowH6+=dt;
  if(refreshPose){++t.sequence;t.tick=nowH6;t.timeNs+=dt*1000000ull;packet.stamp.sample=t.sequence;}
  if(refreshOwner){owner.tick=nowH6;g_fist_owner101.publish(owner);}
  g_f4_fists_tick178r22c2=nowH6;ActualGuardSourceH6(t,packet);lastDecisionH6={};ActualGuardPreludeH6();}
 bool settle(){for(unsigned n=0;n<22;++n)step();return lastDecisionH6.block;}
};
int main(){
 for(float sensitivity:{.5f,1.f,1.5f}){
  kcd2vr::melee_tuning_c54::blockSensitivityH29=sensitivity;
  {Rig r;ck(g_census_mode==4&&g_weapon_control_mode==7&&FistSelected101()&&FistModePermits178r22c2(),"actual public SYNC selects unarmed policy through current owner");
   ck(r.settle(),"actual producer projection and consumer prelude enter face guard");FistFrame101 f{};g_fist_frame101.read(f);
   ck(f.sample.head.y==r.t.head.y&&f.sample.controller[0].y==r.t.right.y&&f.sample.controller[1].y==r.t.left.y&&f.sample.tick==r.t.tick&&f.sample.sequence==r.t.sequence,"source is exact original XR head/grip positions and clocks");
   ck(f.sample.controller[0].x!=r.packet.allowed[0].x&&f.sample.controller[1].x!=r.packet.itemHelper[1].x,"avatar/allowed/item-helper transforms never replace guard source");
   r.t.right.y=r.t.left.y=r.t.head.y-.55;r.step();ck(!lastDecisionH6.block,"actual current consumer rejects low hands at all sensitivities");
   r.t.right.y=r.t.left.y=r.t.head.y-.60;r.step();ck(!lastDecisionH6.block,"actual current consumer rejects60cm retention");}
  for(int side=0;side<2;++side)for(int analog=0;analog<2;++analog){Rig r;ck(r.settle(),"bilateral analog loss starts from actual held guard");
   float& value=side?(analog?r.t.left_trigger:r.t.left_grip):(analog?r.t.right_trigger:r.t.right_grip);value=.60f;r.step();
   ck(!lastDecisionH6.block&&!g_native_guard112.fists175.guarding,"actual strict0.60 either hand/analog revokes");}
  for(unsigned failure=0;failure<15;++failure){Rig r;ck(r.settle(),"source/mode negative starts held");
   switch(failure){
    case 0:r.t.allowed=false;break;case 1:r.t.head_valid=false;break;case 2:r.t.right_valid=false;break;case 3:r.t.left_valid=false;break;
    case 4:r.t.head_held=true;break;case 5:r.t.right_held=true;break;case 6:r.t.left_held=true;break;
    case 7:r.owner.empty=false;break;case 8:r.owner.request=2;break;case 9:r.owner.epoch=2;break;
    case 10:diagnostics::lab::reference=true;break;case 11:animation_filter::nativeBodyOwned112=true;break;
    case 12:g_fist_fault101=true;break;case 13:g_gameplay_allowed=false;break;case 14:kcd2vr::workstation178r20c1::owned=true;break;
   }
   const auto released=releasesH6;r.step();ck(!lastDecisionH6.block&&releasesH6>released,"actual producer/consumer fail closed on owner/tracking/context/native mode loss");}
  {Rig r;ck(r.settle(),"mode switch starts held");publicMode(kcd2vr::user175::Mode::Off);const auto n=releasesH6;r.step();
   ck(!lastDecisionH6.block&&releasesH6>n,"actual public OFF cannot inherit old guard");
   publicMode(kcd2vr::user175::Mode::CombatSynchronized);r.step();ck(!lastDecisionH6.block,"actual public reentry requires new dwell");ck(r.settle(),"public new dwell restores guard");}
  {Rig r;ck(r.settle(),"stale raw source starts held");r.step(81,true,false);ck(!lastDecisionH6.block,"current owner cannot relabel original stale XR pose");}
  {Rig r;ck(r.settle(),"stale owner starts held");r.step(101,false,true);ck(!lastDecisionH6.block,"fresh XR cannot revive stale native empty-hand owner");}
  {Rig r;ck(r.settle(),"future pose starts held");r.t.tick=nowH6+100;r.step(1,true,false);ck(!lastDecisionH6.block,"future original source clock refuses");}
  {Rig r;ck(r.settle(),"actor/pose epoch starts held");g_pose_epoch6689=2;r.packet.stamp.epoch=2;r.owner.epoch=2;r.step();ck(!lastDecisionH6.block,"new calibrated body epoch needs fresh dwell");}
  {Rig r;ck(r.settle(),"punch cancellation starts held");g_fist_native101.prepared=true;r.step();ck(!lastDecisionH6.block,"qualified original punch owns input over guard");g_fist_native101.prepared=false;r.step();ck(!lastDecisionH6.block,"punch return does not borrow pre-punch dwell");}
 }

 for(int side=0;side<2;++side){Rig r;ck(r.settle(),"T24 starts from actual clean guard");const auto reads=hand_claim_fixture_t24::nativeReads;
  ck(!HandClaimBlockedT24(side,42,1,1)&&hand_claim_fixture_t24::nativeReads==reads,"T24 FREE path has no native identity reads");
  hand_claim_fixture_t24::Publish(side);ck(HandClaimBlockedT24(side,42,1,1)&&!HandClaimBlockedT24(1-side,42,1,1),"T24 actual helper excludes only its own hand");
  r.step();FistFrame101 f{};g_fist_frame101.read(f);ck(f.sample.claimedT24[side]&&!f.sample.claimedT24[1-side],"T24 actual producer copies per-side claim");
  ck(!lastDecisionH6.block,"T24 actual guard consumer rejects the claimed hand");
  ck(HandClaimBlockedT24(side,42,2,2),"T24 same actor cleanup survives changed body request");}
 {Rig r;ck(r.settle(),"T24 partial publication starts guarded");kcd2vr::ammo_carry_t18::hands=1;r.step();ck(!lastDecisionH6.block,"T24 unknown bit-only claim is not FREE");}
 kcd2vr::melee_tuning_c54::blockSensitivityH29=1;
 std::printf("face_guard_native_h6 %u/%u actual_public_selection_projection_guard_prelude; native_tail_NOT_EXECUTED\n",checks-failures,checks);return failures?1:0;
}
