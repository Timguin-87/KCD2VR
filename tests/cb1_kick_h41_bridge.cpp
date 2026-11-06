#define main ExistingFistBridgeH41
#include "fist_bridge_test.cpp"
#undef main
struct KickRigH41 {
 std::vector<unsigned char> image;std::array<unsigned char,0x100> entity{},manager{};std::array<unsigned char,0x500> actor{};
 std::array<unsigned char,0xE00> state{},player{};std::array<unsigned char,0x300> slots{};std::array<unsigned char,0x30> input{};
 TrackingSnapshot t{};CommittedFrame6695 body{};bool blocked[2]{};
 KickRigH41():image(0x5400000){
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
int main(){failures=total=0;
 using R=kcd2vr::frik_pose_probe::FistKickResultH35;
 {KickRigH41 r;Check(!FistKickEnabledH35()&&RequestFistKickStatusH35(0)==R::NotThisLane,"new native-kick experiment is OFF and preserves old route");}
 {KickRigH41 r;SetFistKickEnabledH35(true);hostTickCount64+=101;Check(RequestFistKickStatusH35(0)==R::Refused,"stale completed frame cannot be promoted by a fresh button timestamp");}
 {KickRigH41 r;SetFistKickEnabledH35(true);++g_pose_epoch6689;Check(RequestFistKickStatusH35(0)==R::Refused,"changed body refuses selected empty-hand lane without alternate fallback");}
 {KickRigH41 r;SetFistKickEnabledH35(true);g_census_mode=0;Check(RequestFistKickStatusH35(0)==R::Refused,"empty but unselected lane refuses explicitly");}
 {KickRigH41 r;SetFistKickEnabledH35(true);put(r.slots.data()+0x260,std::uint64_t(77));Check(RequestFistKickStatusH35(0)==R::NotThisLane,"positively armed owner preserves existing native special route");}
 {KickRigH41 r;SetFistKickEnabledH35(true);Check(RequestFistKickStatusH35(0)==R::Queued,"real native kick button queues without physical foot contact or closed fists");FistKickRequestH35 k{};g_fist_kick_request_h35.read(k);Check(k.sequence==50&&k.tick==10000,"original R3 occurrence and original source sample remain separate");r.publish(10010);FistFrame101 f{};g_fist_frame101.read(f);Check(f.punch.valid&&f.punch.kickH35&&f.punch.kickSerialH35==k.serial,"actual completed publisher admits original native-kick serial");r.native(10010);int zone=-1;std::memcpy(&zone,r.state.data()+0xF8,4);Check(zone==5&&Count("prepare_to_attack:1")==1&&Count("attack_primary:2")==0,"actual native desired-zone setter chooses5 and prepare occurs alone");r.native(10011);Check(Count("attack_primary:2")==1&&accepted119==1,"actual authored native prepare/release reaches original heavy input once");auto once=calls.size();r.native(10012);r.native(10013);Check(calls.size()==once,"repeated current frames cannot repeat original native kick");Check(g_fist_seen101==0,"kick identity does not consume a physical punch sample");}
 {KickRigH41 r;SetFistKickEnabledH35(true);++hostTickCount64;Check(RequestFistKickStatusH35(0)==R::Queued,"R3 after latest XR sample is still original input");r.publish(10001,true);Check(g_fist_kick_consumed_serial_h35==0,"older repeated eye callback cannot discard newly pressed native kick");r.publish(10010);FistFrame101 f{};g_fist_frame101.read(f);Check(f.punch.valid&&f.punch.kickH35&&f.punch.tick==10001,"next completed sample admits same original R3 without restamp");}
 {KickRigH41 r;SetFistKickEnabledH35(true);g_fist_seen101=50;g_fist_native101.lastSample=50;Check(RequestFistKickStatusH35(0)==R::Queued,"new R3 can share source sample already used by prior punch");r.publish(10010);r.native(10010);r.native(10011);Check(accepted119==1&&g_fist_seen101==50,"separate native-kick serial survives old punch seen marker");g_fist_native101.owned=false;r.native(10012);Check(accepted119==1,"native owner rejoin cannot replay already-seen original kick");}
 {KickRigH41 r;SetFistKickEnabledH35(true);RequestFistKickStatusH35(0);SetFistKickEnabledH35(false);r.publish(10010);r.native(10010);Check(!Count("prepare_to_attack:1")&&!accepted119,"OFF before prepare cannot activate old queued experiment");}
 {KickRigH41 r;SetFistKickEnabledH35(true);RequestFistKickStatusH35(0);r.publish(10010);r.native(10010);SetFistKickEnabledH35(false);r.native(10011);Check(accepted119==1&&Count("attack_primary:2")==1,"OFF never blocks release/cleanup of already prepared native request");}
 {KickRigH41 r;SetFistKickEnabledH35(true);RequestFistKickStatusH35(0);r.publish(10151,true);Check(g_fist_kick_consumed_serial_h35==1&&!g_fist_pending101.valid,"old tracking cannot retain original kick beyond real150ms wall-clock expiry");}
 std::printf("CB1_KICK_H41 %d/%d PASS native_GAME_calls=EXPLICIT_HOST_LEAVES\n",total-failures,total);return failures?1:0;
}
