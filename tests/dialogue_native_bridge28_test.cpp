// Reuse the explicit fake retail memory/native leaves from the helper fixture.
// The bridge itself is extracted from current production source at test setup.
#define DIALOGUE_NATIVE_HEAD28_MEMORY_ONLY
#include "dialogue_native_head28_test.cpp"
#undef DIALOGUE_NATIVE_HEAD28_MEMORY_ONLY
#include "../src/core/snapshot_mailbox.h"
#include <mutex>
#include <thread>
#include <future>

namespace kcd2vr::frik_pose_probe {
namespace {
bool retailReady=true;
unsigned scopes=0;
std::atomic<bool> g_pose_restore_fault6689{false};
bool g_pose_callback_active6689=false;
std::recursive_mutex g_pose_mutex6689;
// Use the same epoch that the explicit fixture native leaves can invalidate.
struct Epoch {std::uint64_t load(std::memory_order) const noexcept{return liveEpoch;}} g_pose_epoch6689;
struct DialogueViewHead28 {ds::NativeHead head{};std::uintptr_t player=0,instance=0;};
kcd2vr::core::SnapshotMailbox<DialogueViewHead28> g_dialogue_view_head28;
struct ReadScope101R2 {ReadScope101R2(){++scopes;}};
std::uintptr_t WhBase() noexcept{return b;}
namespace retail_build112 {bool Ready(std::uintptr_t base) noexcept{return retailReady&&base==b;}}
template<class T>bool Read(const void* a,T& value){return read(reinterpret_cast<std::uintptr_t>(a),value);}
bool NativeDialogueCharacter28(std::uintptr_t fn,std::uintptr_t ent,std::uintptr_t& out) noexcept{return get(fn,ent,out);}
bool NativeDialogueJoint28(std::uintptr_t fn,std::uintptr_t p,int id,ds::V& out) noexcept{return pose(fn,p,id,out);}
}
#include "dialogue_native_bridge28_extracted.inc"
}

int main(){
 using namespace kcd2vr::frik_pose_probe;
 unsigned checks=0;const auto check=[&](bool value){++checks;assert(value);};
 const auto fresh=[&]{reset();retailReady=true;scopes=0;g_pose_restore_fault6689=false;g_pose_callback_active6689=false;};
 ds::NativeHead out{};
 fresh();check(!NativeDialogueViewHead28(out,fixtureState,100));
 ObserveNativeDialogueHead28(fixtureState,100);check(scopes==1&&getCalls==2&&poseCalls==2);
 check(NativeDialogueViewHead28(out,fixtureState,100));check(out.character==fixtureCharacter&&out.tick==100&&out.epoch==7);
 // No g_last_player_entity or skinHint exists in this translation unit.
 auto changed=fixtureState;changed.player+=8;check(!NativeDialogueViewHead28(out,changed,100));
 changed=fixtureState;changed.instance+=8;check(!NativeDialogueViewHead28(out,changed,100));
 changed=fixtureState;changed.entity+=8;check(!NativeDialogueViewHead28(out,changed,100));
 changed=fixtureState;changed.active=false;check(!NativeDialogueViewHead28(out,changed,100));
 changed=fixtureState;changed.camera=ds::Camera::Dialogue;check(!NativeDialogueViewHead28(out,changed,100));
 check(NativeDialogueViewHead28(out,fixtureState,160));check(!NativeDialogueViewHead28(out,fixtureState,161));
 check(!NativeDialogueViewHead28(out,fixtureState,99));
 ++liveEpoch;check(!NativeDialogueViewHead28(out,fixtureState,100));
 // Early native-scope refusal must perform no fixture native reads/calls.
 for(unsigned reason=0;reason<6;++reason){fresh();
   if(reason==0)retailReady=false;
   if(reason==1)fixtureState.active=false;
   if(reason==2)fixtureState.camera=ds::Camera::Dialogue;
   if(reason==3)fixtureState.tick=19;
   if(reason==4)g_pose_restore_fault6689=true;
   if(reason==5)g_pose_callback_active6689=true;
   ObserveNativeDialogueHead28(fixtureState,100);check(reads==0&&getCalls==0&&poseCalls==0&&scopes==0);
 }
 // An actual foreign-thread render lock is refused immediately, no native work.
 fresh();std::promise<void> held,release;auto done=release.get_future();
 std::thread renderer([&]{std::unique_lock<std::recursive_mutex> lock(g_pose_mutex6689);held.set_value();done.wait();});
 held.get_future().wait();ObserveNativeDialogueHead28(fixtureState,100);
 check(reads==0&&getCalls==0&&poseCalls==0&&scopes==0);release.set_value();renderer.join();
 // A refused later copy cannot mint a new timestamp or extend a stale lease.
 for(unsigned reason=0;reason<5;++reason){fresh();ObserveNativeDialogueHead28(fixtureState,100);
   fixtureState.tick=120;
   if(reason==0)mutation=5;
   if(reason==1)mutation=7;
   if(reason==2)failed=fixtureCharacter+0xA08;
   if(reason==3)q(go+0x18,entity+8);
   if(reason==4){getCalls=0;mutation=3;}
   ObserveNativeDialogueHead28(fixtureState,120);
   if(reason==4){check(!NativeDialogueViewHead28(out,fixtureState,120));liveEpoch=7;}
   check(NativeDialogueViewHead28(out,fixtureState,120));check(out.tick==100);
   check(!NativeDialogueViewHead28(out,fixtureState,161));
 }
 fresh();ObserveNativeDialogueHead28(fixtureState,100);fixtureState.tick=120;ObserveNativeDialogueHead28(fixtureState,120);
 check(NativeDialogueViewHead28(out,fixtureState,120)&&out.tick==120);
 const auto before=out;g_pose_restore_fault6689=true;
 check(!NativeDialogueViewHead28(out,fixtureState,120));check(out.tick==before.tick&&out.character==before.character);
 std::printf("dialogue_native_bridge28: %u actual observer/accessor checks PASS\n",checks);
}
