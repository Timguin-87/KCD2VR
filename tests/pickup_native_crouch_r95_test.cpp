// Actual game-thread native stance adapter and exact query, controlled native memory.
#include "pickup_crouch_memory_r95_actual.inc"
#include "core/native_grasp13.h"
static std::uint64_t clockR95=1000;static auto GetTickCount64(){return clockR95;}static unsigned GetCurrentThreadId(){return 77;}
static std::uintptr_t WhBase(){return id().base;}
template<class T>static bool Read(const void* p,T& value){return rd(reinterpret_cast<std::uintptr_t>(p),value);}
static std::atomic<std::uintptr_t> g_player_char37{0x30000};
static std::atomic<void*>g_player_module_d76{reinterpret_cast<void*>(0x10000)},g_last_player_entity{reinterpret_cast<void*>(0x20000)};
static std::atomic<std::uint64_t>g_pose_epoch6689{1},g_frame_control51{3},g_player_lifetime_d44{4};static std::atomic<unsigned>g_stage_thread596{77};static std::atomic<int>g_cal_state{2};
static std::atomic<bool>g_gameplay_allowed{true},g_ctx_menu{false},g_ctx_dialogue{false},g_ctx_cutscene{false},g_ctx_scripted{false},g_ctx_reading{false};
static constexpr std::uint64_t kNativeBodyHeightBit54=4;
namespace kcd2vr::tracking59c6{static std::atomic<std::uint64_t>epoch{2};}
namespace kcd2vr::activity_input163{struct W{bool active=false;bool Active(){return active;}};static W witness175;}
namespace diagnostics::lab{static bool reference=false;static bool NativeReference(){return reference;}}
namespace animation_filter{static std::atomic<bool> nativeBodyOwned112{false};static bool kick=false;static bool NativeKickActive175(){return kick;}}
namespace retail_build112{static bool ready=true;static bool Ready(std::uintptr_t){return ready;}}
static bool passenger=false,identityGood=true,held=true;static float grip=1;static std::uint64_t inputTick=1000;
static bool NativePassengerCurrentA34(std::uint64_t){return passenger;}
static bool PlayerIdentityAccepts37(void*,std::uint64_t){return identityGood;}
static kcd2vr::grasp13::FinishingPresentationR94 heldReceipt{};
namespace action_time_owner59c11{
static bool ReadAdoptedAttachmentPresentationR95(kcd2vr::grasp13::FinishingPresentationR94& out)noexcept{out=heldReceipt;return held&&grip>.25f&&clockR95>=inputTick&&clockR95-inputTick<=60;}
static bool AdoptedAttachmentPresentationCurrentR95(const kcd2vr::grasp13::FinishingPresentationR94& expected)noexcept{kcd2vr::grasp13::FinishingPresentationR94 now{};return ReadAdoptedAttachmentPresentationR95(now)&&kcd2vr::grasp13::sameFinishingPresentationR94(now,expected);}
}
static void(*duringNativeR95)()=nullptr;
static bool CallNativeCrouchR93(std::uintptr_t function,std::uintptr_t player,bool& result)noexcept{const bool ok=call(function,player,result);if(duringNativeR95)duringNativeR95();return ok;}
#include "pickup_crouch_publisher_r95_actual.inc"
static void resetR95(){setup();clockR95=inputTick=1000;c::enabled=true;c::invalidate();c::receipt.publish({});g_gameplay_allowed=true;g_ctx_scripted=g_ctx_menu=g_ctx_dialogue=g_ctx_cutscene=g_ctx_reading=false;diagnostics::lab::reference=false;animation_filter::nativeBodyOwned112=false;animation_filter::kick=false;passenger=false;identityGood=held=true;grip=1;g_stage_thread596=77;g_frame_control51=3;retail_build112::ready=true;kcd2vr::activity_input163::witness175.active=false;duringNativeR95=nullptr;heldReceipt={};heldReceipt.binding.character=id().character;heldReceipt.binding.epoch=id().body;heldReceipt.binding.side=0;heldReceipt.binding.token=8;heldReceipt.serial=9;heldReceipt.generation=10;heldReceipt.trackingEpoch=id().tracking;}
static bool currentR95(){c::Receipt r{};return c::receipt.read(r)&&c::current(r,id(),c::generation.load());}
int main(){
 resetR95();held=false;PublishNativeCrouchR93();ck(currentR95()&&calls==1,"ordinary native crouch is independent of holding an item");
 for(int side=0;side<2;++side){resetR95();g_gameplay_allowed=false;g_ctx_scripted=true;heldReceipt.binding.side=side;PublishNativeCrouchR93();ck(currentR95()&&calls==1,"same adopted hand keeps current crouch predicate through policy gap");c::Receipt r{};c::receipt.read(r);
  f::Input i{};i.stamp={1,1,1000,id().character};i.control=3;i.timeNs=1000000000;i.allowed=i.hmdHeight=i.nativeStanceR93=true;i.nativeCrouchedR93=r.crouched;i.scale=1;i.restHead=i.calibratedHead=i.nativeHead={0,0,1.7};i.restPelvis=i.nativePelvis={0,0,.9};f::State st{};f::Frame frame{};ck(f::advance(st,i,frame)&&frame.nativeCrouchOffsetR93==-.5,"actual frame retains authored half-meter stance under current predicate");
 }
 resetR95();g_gameplay_allowed=false;g_ctx_scripted=true;
 duringNativeR95=[](){g_gameplay_allowed=true;g_ctx_scripted=false;held=false;};
 PublishNativeCrouchR93();ck(currentR95()&&calls==1,"ordinary gameplay recovery does not need a retired held-item permission");
 for(int fault=0;fault<17;++fault){resetR95();g_gameplay_allowed=false;g_ctx_scripted=true;switch(fault){case 0:c::enabled=false;break;case 1:held=false;break;case 2:grip=0;break;case 3:inputTick=939;break;case 4:heldReceipt.binding.character++;break;case 5:heldReceipt.binding.epoch++;break;case 6:heldReceipt.trackingEpoch++;break;case 7:g_ctx_menu=true;break;case 8:g_ctx_dialogue=true;break;case 9:g_ctx_cutscene=true;break;case 10:g_ctx_reading=true;break;case 11:kcd2vr::activity_input163::witness175.active=true;break;case 12:passenger=true;break;case 13:animation_filter::nativeBodyOwned112=true;break;case 14:diagnostics::lab::reference=true;break;case 15:g_stage_thread596=99;break;case 16:retail_build112::ready=false;break;}
  PublishNativeCrouchR93();ck(!currentR95()&&calls==0,"invalid current held/context/native option prevents query and publication");
 }
 for(int fault=0;fault<6;++fault){resetR95();g_gameplay_allowed=false;g_ctx_scripted=true;switch(fault){case 0:duringNativeR95=[](){held=false;};break;case 1:duringNativeR95=[](){grip=0;};break;case 2:duringNativeR95=[](){heldReceipt.binding.token++;};break;case 3:duringNativeR95=[](){g_ctx_menu=true;};break;case 4:duringNativeR95=[](){c::enabled=false;};break;case 5:duringNativeR95=[](){clockR95+=81;};break;}
  PublishNativeCrouchR93();ck(calls==1&&!currentR95(),"release replacement stale source or protected entry during native read prevents receipt");
 }
 std::printf("pickup native crouch %d/%d; actual query/frame adapter native leaves modeled\n",tests-failed,tests);return failed?1:0;
}
