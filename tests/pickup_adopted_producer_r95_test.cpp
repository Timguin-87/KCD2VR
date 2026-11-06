// Literal production producer/admission/final-owner seams, native leaves modeled.
#include "core/native_grasp13.h"
#include <atomic>
#include <cstdio>
#include <cmath>
#include <limits>
using U=std::uint64_t;
static U tick=1000; static U GetTickCount64(){return tick;}
struct TrackingPose{float px=0,py=0,pz=0,qx=0,qy=0,qz=0,qw=1;};
namespace kcd2vr::tracking59c6{static std::atomic<U> epoch{9}; static U NowNs(){return tick*1000000;}}
namespace tracking59c6=kcd2vr::tracking59c6;
namespace animation_filter{static std::atomic<bool> nativeBodyOwned112{false};static std::atomic<U> combatPoseTick112{0};static bool kick=false;static bool NativeKickActive175(){return kick;}}
namespace diagnostics::lab{static bool reference=false;static bool NativeReference(){return reference;}}
namespace activity_input163{struct Witness{bool active=false;bool Active(){return active;}};static Witness witness175;}
namespace visual_selection59c7f{static bool Selected(bool calibrated,int,int){return calibrated;}}
namespace kcd2vr::reading_pose32{static bool poseAllowed(bool allowed,bool,U){return allowed;}}
static std::atomic<U> g_pose_epoch6689{7},g_pose_policy_generation57{2},g_frame_control51{3},g_weapon_request_epoch6694{4};
static std::atomic<void*> g_committed_character6689{reinterpret_cast<void*>(0x1234)},g_henry_char_attachment_manager{reinterpret_cast<void*>(0x123c)};
static std::atomic<bool> g_ctx_menu{false},g_ctx_dialogue{false},g_ctx_cutscene{false},g_ctx_reading{false},g_ctx_scripted{false},g_gameplay_allowed{false},g_kick_busy176{false},g_pose_restore_fault6689{false},g_dialogue_body178r9{false},g_committed_pose_valid6689{true};
static std::atomic<int> g_cal_state{2},g_census_mode{3},g_mode{0};
static std::atomic<U> g_head_valid_tick{1000},g_left_valid_tick{1000},g_right_valid_tick{1000};
static bool workstation=false,claimed=false,owner=true;
static const char* WorkstationContextRefusal19c4r2(U,bool){return workstation?"station":nullptr;}
static bool HandClaimBlockedT24(int,std::uintptr_t,U,U){return claimed;}
static kcd2vr::grasp13::AcquisitionInput55 raw{};
static kcd2vr::grasp13::FinishingPresentationR94 receipt{};
namespace action_time_owner59c11{
static int reads=0;
static bool ReadAdoptedAttachmentPresentationR95(kcd2vr::grasp13::FinishingPresentationR94& out)noexcept{
 ++reads;out=receipt;const int side=receipt.binding.side;
 return owner&&side>=0&&side<2&&raw.headTracked&&raw.tracked[side]&&raw.grip[side]>.25f&&raw.character==receipt.binding.character&&raw.epoch==receipt.binding.epoch&&raw.trackingEpoch==receipt.trackingEpoch&&raw.tick&&tick>=raw.tick&&tick-raw.tick<=80;
}
static bool AdoptedAttachmentPresentationCurrentR95(const kcd2vr::grasp13::FinishingPresentationR94& old)noexcept{
 kcd2vr::grasp13::FinishingPresentationR94 now{};return ReadAdoptedAttachmentPresentationR95(now)&&kcd2vr::grasp13::sameFinishingPresentationR94(old,now);
}}
static void input(const kcd2vr::grasp13::AcquisitionInput55& r)noexcept{raw=r;}
static auto g_graspInput55=&input;
static bool (*g_graspAcquisition54)(kcd2vr::grasp13::Acquisition54&)noexcept=nullptr;
static bool (*g_graspPendingUse55)(kcd2vr::grasp13::PendingUse55&)noexcept=nullptr;
static bool lastAllowed=false;
#include "pickup_producer_r95_actual.inc"
namespace pc{struct Vec{double x=0,y=0,z=0;};static double length(Vec){return 0;}}
namespace kcd2vr::tracked_frame51{struct Frame{struct {std::uintptr_t actor=0;U epoch=0,tick=0;}stamp;};static bool ordinary_reference_d86(const Frame&){return false;}}
struct State{bool legsReady128=true;kcd2vr::grasp13::PendingUse55 pendingUse55{};kcd2vr::tracked_frame51::Frame pendingReference55{},ordinaryReference54{};};
struct Tracking{U source_epoch=9;bool allowed=false,head_valid=true,left_valid=true,right_valid=true,head_held=false,left_held=false,right_held=false;float left_grip=1,right_grip=1;};
struct Tx{U epoch=7,policyGeneration57=2,control51=3;bool acquisitionRequired57=false,adoptedVisualRequiredR95=false,instantPairHandsD72=false;int setter=0;const char* stage="";Tracking tracking{};kcd2vr::grasp13::Acquisition54 acquisition54{};kcd2vr::grasp13::FinishingPresentationR94 adoptedVisualR95{};};
static bool RejectCoherent6693(Tx&,int){return false;}static bool ValidateBuffer1Setter(void*,int&){return true;}
static struct {void* character=reinterpret_cast<void*>(0x1234);}g_pose6689;
static bool ReadModelWorld6692(void*,int&){return true;}
static bool body(State& s,Tx& tx){void* character=reinterpret_cast<void*>(0x1234);void* pose=character;auto& ts=tx.tracking;
#include "subsystems/interactions/pose_acquisition_admission.inl"
 const bool full=true,readingVisual32=false;int frameWorld51=0;
#include "pickup_foundation_r95_actual.inc"
 return ordinaryFrame51;
}
static bool final(Tx& tx){
#include "pickup_final_r95_actual.inc"
 return false;
}
static int checks=0;
#define CHECK(x) do{++checks;if(!(x)){std::printf("FAIL producer %d: %s\n",__LINE__,#x);return 1;}}while(0)
static void reset(int side){tick=1000;owner=true;workstation=claimed=false;g_ctx_menu=g_ctx_dialogue=g_ctx_cutscene=g_ctx_reading=false;g_ctx_scripted=true;g_gameplay_allowed=false;animation_filter::nativeBodyOwned112=false;animation_filter::kick=false;diagnostics::lab::reference=false;g_kick_busy176=false;g_pose_restore_fault6689=false;g_cal_state=2;g_pose_epoch6689=7;tracking59c6::epoch=9;activity_input163::witness175.active=false;receipt={};receipt.binding.character=0x1234;receipt.binding.epoch=7;receipt.binding.side=side;receipt.binding.token=123;receipt.binding.key=321;receipt.requestedAt=800;receipt.serial=4;receipt.generation=8;receipt.trackingEpoch=9;receipt.sourceTick=tick;}
static void publish(bool allowed=false,bool hv=true,bool lv=true,bool rv=true,float lg=1,float rg=1,U age=0){TrackingPose p{};g_committed_pose_valid6689=true;g_head_valid_tick=g_left_valid_tick=g_right_valid_tick=tick;PublishTrackingSnapshot(p,hv,p,lv,p,rv,lg,rg,0,0,allowed,tick-age,tick*1000000,9,0,false,0,0);}
int main(){
 for(int side=0;side<2;++side){reset(side);publish();CHECK(lastAllowed&&g_committed_pose_valid6689&&raw.tick==1000);State s;Tx tx;tx.tracking.allowed=lastAllowed;CHECK(body(s,tx)&&tx.adoptedVisualRequiredR95&&!tx.acquisitionRequired57&&final(tx));
  // More than 80ms after old cleanup: every NEW source remains current, no frozen pose.
  tick+=300;publish();tx={};tx.tracking.allowed=lastAllowed;CHECK(body(s,tx)&&final(tx)&&raw.tick==1300&&receipt.requestedAt==800);
  owner=false;CHECK(!final(tx));publish();CHECK(!lastAllowed&&!g_committed_pose_valid6689);
  // A normal body does not need this item lease; release must not reject it.
  reset(side);g_gameplay_allowed=true;g_ctx_scripted=false;publish(true);tx={};tx.tracking.allowed=true;CHECK(body(s,tx)&&!tx.adoptedVisualRequiredR95);owner=false;CHECK(final(tx));
  for(int fault=0;fault<16;++fault){reset(side);switch(fault){case 0:g_ctx_menu=true;break;case 1:g_ctx_dialogue=true;break;case 2:g_ctx_cutscene=true;break;case 3:g_ctx_reading=true;break;case 4:workstation=true;break;case 5:animation_filter::nativeBodyOwned112=true;break;case 6:animation_filter::kick=true;break;case 7:diagnostics::lab::reference=true;break;case 8:g_kick_busy176=true;break;case 9:receipt.binding.character++;break;case 10:receipt.binding.epoch++;break;case 11:receipt.trackingEpoch++;break;case 12:owner=false;break;case 13:g_cal_state=0;break;case 14:activity_input163::witness175.active=true;break;default:break;}
   publish(false,true,true,true,side&&fault==15?0:1,!side&&fault==15?0:1);CHECK(!lastAllowed&&!g_committed_pose_valid6689);
  }
  reset(side);publish(false,false);CHECK(!lastAllowed);reset(side);publish(false,true,!side,side);CHECK(!lastAllowed);
  reset(side);publish(false,true,true,true,1,1,61);CHECK(!lastAllowed);reset(side);publish(false,true,true,true,1,1,81);CHECK(!lastAllowed);
  reset(side);publish();tx={};tx.tracking.allowed=lastAllowed;CHECK(body(s,tx));raw.grip[side]=0;CHECK(!final(tx));
 }
 std::printf("PASS adopted current producer/body/exit: %d checks; native owner leaf separately tested\n",checks);return 0;
}
