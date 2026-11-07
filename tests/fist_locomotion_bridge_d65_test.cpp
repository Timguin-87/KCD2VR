#define main ExistingFistBridgeLocomotionD65
#include "fist_bridge_test.cpp"
#undef main
// Unrelated subsystem leaves are explicit refusals/no-op diagnostics only.
namespace kcd2vr::native_script_t2 {bool Context::Current()const noexcept{return false;}bool Context::CallPacket(const char*,const char*,char*,std::size_t)const noexcept{return false;}bool Register(Client,Wanted)noexcept{return false;}bool Install()noexcept{return false;}}
namespace kcd2vr::physics::grab_producer60 {bool SetSourcePredicateD41(bool(*)(const Intent&)noexcept)noexcept{return false;}bool Enabled()noexcept{return false;}}
namespace kcd2vr::diagnostic_trace {void WriteFor(diagnostics::capture168::Domain,const char*)noexcept{}void Drain(bool)noexcept{}}
namespace kcd2vr::action_time_owner59c11 {bool ReadAcquisitionFoundationD56(grasp13::Acquisition54&out)noexcept{out={};return false;}}
namespace kcd2vr::door_signal19c4r4 {bool SetSmallPickupReaderR24(SmallPickupReaderR24)noexcept{return false;}}
namespace kcd2vr::combat_diag_probe {unsigned ComboCandidatesCZ(const void*,kcd2vr::combo_zone_assist_cz::NativeCandidate*,unsigned)noexcept{return 0;}}
namespace kcd2vr::animation_filter {void* PlayerSkeletonAnim()noexcept{return nullptr;}}
struct LocomotionRigD65 {
 std::vector<unsigned char> image;std::array<unsigned char,0x100> entity{},manager{};std::array<unsigned char,0x500> actor{};
 std::array<unsigned char,0xE00> state{},player{};std::array<unsigned char,0x300> slots{};std::array<unsigned char,0x30> input{};
 TrackingSnapshot t{};CommittedFrame6695 body{};bool blocked[2]{};
 LocomotionRigD65():image(0x5400000){
  kcd2vr::retail_build112::hostRetailVerified112=true;hostModuleHandleW=image.data();g_wh_pinned104=0;const auto base=WhBase();hostTickCount64=10000;
  put(manager.data(),base+0x3EC9958);put(manager.data()+0x38,static_cast<void*>(player.data()));put(player.data(),base+0x3AA4B98);
  put(player.data()+0x9D0,static_cast<void*>(manager.data()));put(player.data()+0x990,static_cast<void*>(slots.data()));put(player.data()+0xB88,static_cast<void*>(input.data()));
  put(player.data()+0x38,entity.data());put(entity.data(),base+0x4072C78);g_last_player_entity=entity.data();
  put(player.data()+0x278,actor.data());put(actor.data(),base+0x3A80610);put(actor.data()+0x1F8,player.data());put(actor.data()+0x210,state.data());put(state.data()+0xCE0,actor.data());
  state104=state.data();put(image.data()+0x5321010,17);put(image.data()+0x5321014,18);put(image.data()+0x5321018,19);
  g_native_contract104=base;g_native_request104=&NativeRequest104Test;g_native_zone104=&NativeZone104Test;
  g_input_contract117=base;g_input_begin117=g_input_release117=&PunchLeafR19;put(punchControlR19.data()+0x18,actor.data());
  put(input.data(),base+0x3EC99A8);put(input.data()+0x10,static_cast<void*>(player.data()));g_henry_item_attach_manager=manager.data();
  g_combat_contract101=base;g_combat_input_original101=&NativeInput;g_combat_bypass101=&NativeInput;
  const char*names[]{"prepare_to_attack","attack_primary","attack_abort"};for(int i=0;i<3;++i)g_fist_action_names101[i]=reinterpret_cast<std::uintptr_t>(names[i]);
  g_census_mode=5;g_weapon_control_mode=0;g_gameplay_allowed=g_head_valid=g_right_valid=g_left_valid=true;g_pose_epoch6689=1;g_fist_epoch101=2;g_player_char37=0x1234;
  g_fist_native101={};g_fist_pending101={};g_fist_motion101={};g_fist_motion_request101=0;g_fist_seen101=0;g_fist_fault101=false;
  g_fist_kick_serial_h35=g_fist_kick_consumed_serial_h35=g_fist_kick_seen_h35=0;g_fist_kick_request_h35.publish({});SetFistKickEnabledH35(false);
  g_ctx_menu=g_ctx_dialogue=g_ctx_cutscene=g_ctx_reading=g_ctx_scripted=false;kcd2vr::animation_filter::nativeBodyOwned112=false;
  calls.clear();punchClassesR19.clear();latch119=accepted119=0;FistNativeTick101();calls.clear();
  t.allowed=t.head_valid=t.right_valid=t.left_valid=true;t.head={0,1.6f,0};t.hq={0,0,0,1};t.right={.2f,1.25f,-.2f};t.left={-.2f,1.25f,-.2f};t.sequence=50;t.tick=10000;
  body.stamp.epoch=1;body.stamp.actor=0x1234;body.stamp.tick=10000;body.stamp.sample=50;body.valid[0]=body.valid[1]=true;body.world[3]=body.world[7]=1;
  body.allowed[0]={{0,0,0,1},{.2f,.2f,1.25f}};body.allowed[1]={{0,0,0,1},{-.2f,.2f,1.25f}};
  FistPublish101(t,body,blocked);
 }
 void publish(std::uint64_t tick,bool older=false){hostTickCount64=tick;if(!older){t.tick=tick;++t.sequence;body.stamp.tick=tick;body.stamp.sample=t.sequence;}FistOwner101 o{};FistReadOwner101(o);g_fist_owner101.publish(o);FistPublish101(t,body,blocked);}
 void native(std::uint64_t tick){hostTickCount64=tick;NativeFrameBegin104();FistNativeTick101();NativeFrameEnd104();}
};

static void FrameD65(LocomotionRigD65& r){r.publish(hostTickCount64+10);r.native(hostTickCount64);}
static void ReadyD65(LocomotionRigD65& r){r.t.right_grip=r.t.right_trigger=1;for(int i=0;i<20;++i)FrameD65(r);calls.clear();latch119=accepted119=0;}
int main(){failures=total=0;
 for(float base:{.045f,.10f,.20f})for(bool trial:{false,true}){
  kcd2vr::punch_travel_d65::minimumMeters.store(base);kcd2vr::fist_stroke_d49::enabled.store(trial);
  for(int mode=0;mode<7;++mode){
   LocomotionRigD65 r;ReadyD65(r);
   for(int i=0;i<40;++i){
    // Cry world/root locomotion changes no raw XR controller position.
    r.body.world[5]+=.012f;
    if(mode==0){r.body.allowed[0].t[2]+=i%2?.005f:-.005f;}
    if(mode==1)r.t.head.z-=.012f;
    if(mode>=2){r.t.head.z-=.012f;r.t.right.z-=.012f;r.t.left.z-=.012f;r.body.allowed[0].t[1]+=.012f;r.body.allowed[1].t[1]+=.012f;}
    if(mode==3||mode==5){const double a=-double(i+1)*.018;r.t.hq={float(std::sin(a*.5)),0,0,float(std::cos(a*.5))};}
    if((mode==4||mode==5)&&i<18){r.t.right.z-=.018f;r.body.allowed[0].t[1]+=.018f;}
    if((mode==4||mode==5)&&i>=18&&i<36){r.t.right.z+=.018f;r.body.allowed[0].t[1]-=.018f;}
    if(mode==6){r.t.right.z+=.018f;r.body.allowed[0].t[1]-=.018f;}
    FrameD65(r);
   }
   // Consume any one-frame delayed publisher without introducing movement.
   FrameD65(r);FrameD65(r);
   const unsigned expected=(mode==4||mode==5)?1u:0u;
   std::printf("mode=%d base=%.3f trial=%d prepare=%d release=%d accepted=%u\n",mode,base,trial?1:0,Count("prepare_to_attack:1"),Count("attack_primary:2"),accepted119);
   Check(Count("prepare_to_attack:1")==int(expected)&&Count("attack_primary:2")==int(expected)&&accepted119==expected,"actual publisher/native bridge emits only deliberate moving punch, never locomotion or return");
  }
 }
 std::printf("fist_locomotion_bridge_d65 %d/%d host native input leaves; no game damage executed\n",total-failures,total);return failures?1:0;
}
