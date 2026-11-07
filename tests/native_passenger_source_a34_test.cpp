// Execute the existing production T4 worker and its mock native lifetime seam.
// The retained fixture's broad main is not executed by this focused test.
#define main RetainedNativeWeaponFixtureMainA34
#include "native_weapon_body_t4_test.cpp"
#undef main
#include "../src/subsystems/interactions/native_passenger_a34.h"
#include "../src/subsystems/physics/runtime/native_hand_presence_pose_h24.h"
#include "../src/subsystems/physics/runtime/dialogue_hand_presence_h24.h"
namespace kcd2vr::tracking59c6 {inline std::atomic<std::uint64_t> epoch{9};}
namespace kcd2vr::animation_filter {
inline std::atomic<bool>nativeBodyOwned112{false};inline bool NativeKickActive175(){return false;}
}
namespace kcd2vr::diagnostics::lab {inline bool NativeReference(){return false;}}
namespace kcd2vr::activity_input163 {struct Witness{bool Active(){return false;}};inline Witness witness175;}
namespace sourceA34 {
using namespace kcd2vr;namespace heldT4=native_weapon_body_t4;
namespace handPolicyH24=native_hand_proxy_t4;
struct NativeHeldSourceT4 {heldT4::Input input{};std::uint64_t tick=0;bool valid=false;};
inline core::SnapshotMailbox<NativeHeldSourceT4>g_native_held_source_t4;
inline std::atomic<bool>g_native_held_valid_t4{true},g_native_hand_physics_h24{true},
    g_phys_fault6698{false},g_gameplay_allowed{true},g_ctx_menu{false},g_ctx_dialogue{false},
    g_ctx_cutscene{false},g_ctx_reading{false},g_ctx_scripted{false},g_head_valid{true},g_right_valid{true};
inline std::atomic<std::uint64_t>g_native_held_lease_t4{10},g_pose_epoch6689{4},
    g_weapon_request_epoch6694{3},g_pose_policy_generation57{8},g_native_use_until107[2]{};
inline std::atomic<std::uintptr_t>g_player_char37{0x300};inline std::atomic<void*>g_last_player_entity{reinterpret_cast<void*>(0xa00)};
inline std::atomic<int>g_weapon_control_mode{4},g_cal_state{2};
inline std::atomic<float>g_right_grip{1},g_left_grip{0};
inline core::SnapshotMailbox<physics::dialogue_hand_h24::Witness>g_dialogue_hand_presence_h24;
inline std::atomic<bool>g_dialogue_hand_presence_enabled_h24{false};
bool TrackingHeadUsable111(std::uint64_t){return true;}bool TrackingHandUsable111(int,std::uint64_t){return true;}
bool LegacyPhysBodiesRetiredT4(){return true;}const char* WorkstationContextRefusal19c4r2(std::uint64_t,bool){return nullptr;}
std::uint64_t NowA34(){return fixture::now;}
#define GetTickCount64 NowA34
#include <generated_passenger_source_a34.inl>
#undef GetTickCount64
void sync(const heldT4::Input& in){g_native_held_source_t4.publish({in,fixture::now,true});}
native_passenger_a34::Root passenger(){native_passenger_a34::Root r{};
 r.controller=1;r.character=0x300;r.action=2;r.scope=3;r.context=4;r.epoch=4;
 r.sourceTick=r.tick=fixture::now;r.serial=1;r.token=9;r.crc=0xeed2e729;r.fragment=1;r.status=2;r.layer=0;return r;}
int checks=0;
void check(bool pass,const char* text){++checks;if(!pass){std::printf("FAIL A34 %s\n",text);std::exit(2);}}
}
int main(){using namespace fixture;using namespace sourceA34;namespace a=kcd2vr::native_passenger_a34;
 check(w::SetSourcePredicate(&NativeHeldSourceCurrentT4),"register actual source predicate");
 check(w::Install(),"actual T4 worker installs through existing native seam");w::SetEnabled(true);
 auto in=Input();sync(in);Active(in);sync(in);w::Achieved out{};
 check(w::Read(out)&&out.poseValid,"actual achieved body active before passenger");
 auto r=passenger();a::witness.publish(r);
 check(!animation_filter::nativeBodyOwned112.load(),"renderer mirror has not yet updated");
 const auto nativeBefore=creates+adds+enables+removes+destroys+releases+queries+velocities+livingCalls;
 check(!NativeHeldSourceCurrentT4(in),"current passenger revokes source before next skinning");
 check(nativeBefore==creates+adds+enables+removes+destroys+releases+queries+velocities+livingCalls,"source check makes no native calls");
 check(w::Read(out)&&!out.poseValid&&!out.colliderRetired,"achieved authority revoked while collider remains retained");
 const auto oldVelocities=velocities;Advance(in);sync(in);
 check(velocities==oldVelocities,"post-observation protected phase never drives old target");
 Tick();Tick();check(!alive,"native retirement proceeds after presentation source revoked");
 w::SetEnabled(false);check(w::Read(out)&&out.colliderRetired&&out.safeNativeRestore,"retirement acknowledgment remains truthful");
 // The predicate's pre-existing current body/request/UI guards stay mandatory.
 a::witness.publish({});Restart();in=Input();sync(in);
 check(NativeHeldSourceCurrentT4(in),"new ordinary source allowed after native terminal");
 ++g_player_char37;check(!NativeHeldSourceCurrentT4(in),"current character replaced without relabeling original source");--g_player_char37;
 ++g_pose_epoch6689;check(!NativeHeldSourceCurrentT4(in),"body epoch revoked");--g_pose_epoch6689;
 ++g_weapon_request_epoch6694;check(!NativeHeldSourceCurrentT4(in),"request epoch revoked");--g_weapon_request_epoch6694;
 g_ctx_menu=true;check(!NativeHeldSourceCurrentT4(in),"menu still revokes");g_ctx_menu=false;
 g_ctx_cutscene=true;check(!NativeHeldSourceCurrentT4(in),"real cutscene still revokes");g_ctx_cutscene=false;
 ++tracking59c6::epoch;check(!NativeHeldSourceCurrentT4(in),"tracking epoch revoked");--tracking59c6::epoch;
 g_pose_policy_generation57=9;check(!NativeHeldSourceCurrentT4(in),"odd cancellation generation revoked");g_pose_policy_generation57=8;
 auto wrong=r;wrong.character++;wrong.sourceTick=wrong.tick=now;a::witness.publish(wrong);
 check(NativeHeldSourceCurrentT4(in),"unrelated character witness does not own this physical source");
 wrong=r;wrong.epoch++;wrong.sourceTick=wrong.tick=now;a::witness.publish(wrong);
 check(NativeHeldSourceCurrentT4(in),"different body witness does not own this physical source");
 now+=81;check(!NativeHeldSourceCurrentT4(in),"original source clock never refreshed by witness");
 a::witness.publish({});Cleanup();
 // Same copied native owner must revoke the independent hand-presence phase.
 using HP=native_hand_proxy_t4::ProxyPolicy;native_hand_proxy_t4::Input hand{};
 hand.source={9,4,8,now,0x100,0,true,0x300};hand.tick=now;
 hand.playerBodyH24=0x900;hand.playerEntityH24=0xa00;hand.playerBodyIdH24=12;hand.emptyBothHandsH24=true;
 check(HandPresenceContextH24(hand),"ordinary current hand presence allowed");
 HP policy{};native_hand_proxy_t4::NativeReceipt receipt{};policy.update(hand,now,receipt);
 receipt.incarnation=policy.incarnation;receipt.adopted=true;policy.update(hand,now,receipt);
 receipt.ready=true;policy.update(hand,now,receipt);
 check(policy.state==native_hand_proxy_t4::State::Active,"actual hand policy active before passenger");
 r=passenger();a::witness.publish(r);
 check(!HandPresenceContextH24(hand)&&!animation_filter::nativeBodyOwned112.load(),"hand context revoked independently of renderer mirror");
 // Existing real presenceSource clears this input before ProxyPolicy::update.
 auto revoked=HandPresenceContextH24(hand)?hand:native_hand_proxy_t4::Input{};
 policy.update(revoked,now,receipt);
 check(policy.state==native_hand_proxy_t4::State::Retiring&&policy.retireRequested,"hand policy retires instead of driving/replacing incarnation");
 receipt.retired=true;policy.update(revoked,now,receipt);
 check(policy.state==native_hand_proxy_t4::State::Off,"hand retirement remains available after source revoked");
 a::witness.publish({});check(HandPresenceContextH24(hand),"fresh hand can resume after actual native terminal");
 std::printf("A34 actual protected-source/weapon-worker/hand-policy %d checks PASS; native leaves substituted; game/headset NOT_RUN\n",checks);
}
