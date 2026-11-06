#include "subsystems/physics/runtime/dialogue_hand_presence_h24.h"
#include "subsystems/physics/runtime/native_hand_presence_pose_h24.h"
#include "core/snapshot_mailbox.h"
#include <atomic>
#include <cstdio>
namespace dh=kcd2vr::physics::dialogue_hand_h24;
namespace hp=kcd2vr::physics::hand_presence_h24;
namespace ds=kcd2vr::dialogue_state59c10r2;
static std::uint64_t testNow=100;
static std::uint64_t TestNow() noexcept{return testNow;}
// Compile and exercise the ACTUAL producer, not a duplicate implementation.
namespace kcd2vr::frik_pose_probe {
static std::atomic<bool> g_dialogue_body178r9{false};
static std::atomic<std::uint64_t> g_pose_policy_generation57{2};
static core::SnapshotMailbox<dh::Witness> g_dialogue_hand_presence_h24;
static std::atomic<bool> g_dialogue_hand_presence_enabled_h24{false};
#define GetTickCount64 TestNow
#include "subsystems/dialogues/body_control.inl"
#undef GetTickCount64
}
namespace producer=kcd2vr::frik_pose_probe;
static int fails=0,checks=0;
#define CK(c) do{++checks;if(!(c)){std::printf("FAIL %d %s\n",__LINE__,#c);++fails;}}while(0)
int main(){
 kcd2vr::native_hand_proxy_t4::Input in{};in.tick=100;in.source={1,1,2,100,0x111,0,true,0x222};
 in.playerEntityH24=0x333;in.playerBodyH24=0x444;in.playerBodyIdH24=7;in.emptyBothHandsH24=true;
 hp::Context c{};c.calibrated=c.tracked=c.dialogue=true;c.epoch=1;c.cancel=2;c.trackingEpoch=1;
 c.character=0x222;c.entity=0x333;
 ds::State state{};state.tick=100;state.known=state.active=true;state.entity=0x333;
 state.player=0x111;state.instance=0x555;state.camera=ds::Camera::FirstPerson;
 auto admitted=[&](){dh::Witness w{};c.dialoguePresence=producer::g_dialogue_hand_presence_enabled_h24.load()&&
   producer::g_dialogue_hand_presence_h24.read(w)&&dh::current(w,in,c.cancel,testNow);
   return hp::current(in,testNow,c);};
 CK(!admitted());
 // Composite visual-only body flag cannot enable physical dialogue hands.
 producer::SetDialogueBody178r9(true);CK(!admitted());
 producer::PublishDialogueHandPresenceH24(state,true);CK(admitted());CK(!c.gameplay);
 c.menu=true;CK(!admitted());c.menu=false;
 c.cutscene=true;CK(!admitted());c.cutscene=false;
 c.scripted=true;CK(!admitted());c.scripted=false;
 c.nativeBody=true;CK(!admitted());c.nativeBody=false;
 c.reading=true;CK(!admitted());c.reading=false;
 c.nativeReference=true;CK(!admitted());c.nativeReference=false;
 c.tracked=false;CK(!admitted());c.tracked=true;
 c.grabBusy=true;CK(!admitted());c.grabBusy=false;
 c.nativeUseUntil=101;CK(!admitted());c.nativeUseUntil=0;
 c.enabled=false;CK(!admitted());c.enabled=true;
 c.character++;CK(!admitted());c.character--;
 c.entity++;CK(!admitted());c.entity--;
 c.trackingEpoch++;CK(!admitted());c.trackingEpoch--;
 auto wrong=in;wrong.source.player++;dh::Witness w{};producer::g_dialogue_hand_presence_h24.read(w);
 CK(!dh::current(w,wrong,2,100));wrong=in;wrong.playerEntityH24++;CK(!dh::current(w,wrong,2,100));
 CK(!dh::current(w,in,4,100));CK(!dh::current(w,in,3,100));
 testNow=181;in.tick=in.source.poseTick=181;CK(!admitted()); // native witness NOT restamped
 testNow=100;in.tick=in.source.poseTick=100;CK(admitted());
 producer::PublishDialogueHandPresenceH24(state,false);CK(!admitted()); // Theater/presentation revocation
 producer::PublishDialogueHandPresenceH24(state,true);CK(admitted());
 state.camera=ds::Camera::Unknown;producer::PublishDialogueHandPresenceH24(state,true);CK(!admitted());
 state.camera=ds::Camera::Dialogue;producer::PublishDialogueHandPresenceH24(state,true);CK(admitted());
 state.active=false;producer::PublishDialogueHandPresenceH24(state,true);CK(!admitted());
 state.active=true;state.instance=0;producer::PublishDialogueHandPresenceH24(state,true);CK(!admitted());
 state.instance=0x555;producer::PublishDialogueHandPresenceH24(state,true);c.dialogue=false;CK(!admitted());
 c.gameplay=true;CK(admitted()); // ordinary H24 world presence remains unchanged
 std::printf("dialogue hand presence actual producer checks=%d failures=%d\n",checks,fails);return fails?1:0;
}
