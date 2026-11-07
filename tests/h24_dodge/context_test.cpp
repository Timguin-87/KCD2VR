#include <atomic>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <cstdint>
#include <cmath>
#include "../../src/diagnostics/hmd_dodge_context_h24.h"
#include "../../src/vrik/roomscale58.h"
static std::uint64_t tick=1000,epoch=7;
static unsigned char actor[1024]{},entity[64]{},player[1024]{},hs[4096]{},npc[1024]{},ns[4096]{};
static void* currentEntity=entity;static bool originalSeen=false,ownerValid=true;static int fail=0,checks=0;
static constexpr std::uintptr_t g_base=0x100000;
static std::uint64_t GetTickCount64(){return tick;}
static bool SehCopy146(const void* p,void* out,std::size_t n){
 auto a=reinterpret_cast<std::uintptr_t>(p);
 for(auto r:{std::pair<void*,std::size_t>{actor,sizeof(actor)},{entity,sizeof(entity)},{player,sizeof(player)},{hs,sizeof(hs)},{npc,sizeof(npc)},{ns,sizeof(ns)}}){auto b=reinterpret_cast<std::uintptr_t>(r.first);if(a>=b&&a-b<=r.second&&n<=r.second-(a-b)){std::memcpy(out,p,n);return true;}}
 return false;
}
template<class T>static void put(void* p,unsigned off,T v){std::memcpy(static_cast<unsigned char*>(p)+off,&v,sizeof(v));}
static std::uintptr_t ptr(void* p){return reinterpret_cast<std::uintptr_t>(p);}
namespace kcd2vr::frik_pose_probe{std::uint64_t PoseEpochH24()noexcept{return epoch;}void* LastPlayerEntity()noexcept{return currentEntity;}bool BodyFollow128(){return false;}}
namespace frik_pose_probe=kcd2vr::frik_pose_probe;
namespace nhp134{static constexpr auto g_base=::g_base;}
namespace retail_build112{static bool fingerprint=true;static bool Ready(std::uintptr_t){return fingerprint;}}
namespace kcd2vr::frik_pose_probe{bool BlockIndicatorGameplayH24(std::uint64_t) noexcept;}

namespace diagnostic_trace{static void Write(const char*){}}
static bool Allow(int,int){return false;}
static bool CombatActor(const void* a,const void*& e,const void*& s,bool){if(!originalSeen||!ownerValid||a!=actor)return false;e=entity;s=hs;return true;}
static bool IsPlayer(const void* e){return e==currentEntity;}
static constexpr int kInCombat=0;static int combat=1;static int B(const void*,int){return combat;}
namespace originalPublisher{
static std::atomic<std::uint64_t>g_henry_seen178r31{0},g_henry_in_combat_edges178r33{0};static std::atomic<int>g_henry_in_combat178r31{-1};
#include "../../src/diagnostics/henry_combat_context_h20.inl"
}
namespace combat_diag_probe{
#include "actual_cache_getter.inc"
#include "actual_block_getter.inc"
}
namespace native_vr_pad_premap{struct State{float lx=0,ly=0;bool b=false;};struct Context{bool vr_menu=false,theater=false,system_menu=false,dialogue_navigation=false,semantic_minigame=false,apse_ui=false,back_context=false;};}
namespace animation_filter{std::atomic<bool>nativeBodyOwned112{false};}
namespace kcd2vr::frik_pose_probe{
static std::atomic<bool>g_gameplay_allowed{true},g_ctx_menu{false},g_ctx_dialogue{false},g_ctx_cutscene{false},g_ctx_reading{false};
static std::atomic<bool>g_vrik_calibration_requested{false},g_arm_capture_active66100{false};static std::atomic<int>g_guided_cal_pose{0};
static bool trackingFresh=true;static bool TrackingHeadUsable111(std::uint64_t){return trackingFresh;}
#include "../../src/subsystems/vrik/block_indicator_context_h24.inl"
}

static std::atomic<bool>g_roomscale_follow58{false},g_roomscale_dodge58{true};
namespace diagnostics::capture168{bool any(std::uint64_t){return false;}}
struct CameraHook{
 static inline kcd2vr::roomscale58::State motion{};static inline double x=0,y=0;static inline std::uint64_t poseTick=0;
 static bool RoomscaleRemaining(float&a,float&b,std::uint64_t&t){a=float(x);b=float(y);t=poseTick;return true;}
 static kcd2vr::roomscale58::Drive RoomscaleDrive(std::uint64_t now,bool follow,bool locked,bool enabled){kcd2vr::roomscale58::observeHmd(motion,poseTick,x,y);return kcd2vr::roomscale58::drive(motion,now,x,y,follow,locked,enabled,{});}
 static void RoomscaleCounters(std::uint64_t&a,std::uint64_t&b,std::uint64_t&c,float&d,float&e){a=b=c=0;d=e=0;}
};
#include "actual_roomscale_pad.inc"
static void ck(bool ok,const char* msg){++checks;std::printf("%s %s\n",ok?"PASS":"FAIL",msg);fail+=!ok;}
static void publish(){originalSeen=true;originalPublisher::PublishHenryCombatContextH20(actor);}
int main(){
 put(actor,0,g_base+0x3A80610);put(actor,0x210,ptr(hs));put(actor,0x1F8,ptr(player));put(hs,0xCE0,ptr(actor));put(hs,0xCF8,ptr(npc));
 put(player,0,g_base+0x3AA4B98);put(player,0x278,ptr(actor));put(player,0x38,ptr(entity));put(entity,0,g_base+0x4072C78);
 put(npc,0,g_base+0x3E3FAC8);put(npc,0x210,ptr(ns));put(ns,0xCE0,ptr(npc));put(ns,0xCF8,ptr(actor));put(hs,0x38,128);put(hs,0x218,-1);put(ns,0x38,4);put(ns,0x218,1);
 originalPublisher::PublishHenryCombatContextH20(actor);ck(!combat_diag_probe::IncomingHmdDodgeH24(tick),"before original-first owner validation cannot publish");
 publish();ck(combat_diag_probe::IncomingHmdDodgeH24(tick),"F3-independent original-first current reciprocal incoming attack qualifies");

 put(hs,0x630,g_base+0x3B0A968);put(hs,0x64D,static_cast<unsigned char>(1));publish();
 ck(combat_diag_probe::NativeBlockIndicatorH24(tick),"actual native MAX1 shows indicator without F3 or physical posture authority");
 put(hs,0x64D,static_cast<unsigned char>(0));publish();ck(!combat_diag_probe::NativeBlockIndicatorH24(tick),"actual native MAX0 clears indicator immediately");
 put(hs,0x64D,static_cast<unsigned char>(2));publish();ck(combat_diag_probe::NativeBlockIndicatorH24(tick),"native MAX2 remains an active blocking context");
 ck(!combat_diag_probe::NativeBlockIndicatorH24(tick+61),"stale native block cache hides indicator");
 ++epoch;ck(!combat_diag_probe::NativeBlockIndicatorH24(tick),"wrong pose epoch hides native block indicator");--epoch;
 currentEntity=npc;ck(!combat_diag_probe::NativeBlockIndicatorH24(tick),"wrong current Henry hides native block indicator");currentEntity=entity;
 retail_build112::fingerprint=false;ck(!combat_diag_probe::NativeBlockIndicatorH24(tick),"fingerprint mismatch hides cached native block indicator");retail_build112::fingerprint=true;
 for(auto flag:{&frik_pose_probe::g_ctx_menu,&frik_pose_probe::g_ctx_dialogue,&frik_pose_probe::g_ctx_cutscene,&frik_pose_probe::g_ctx_reading,&frik_pose_probe::g_vrik_calibration_requested,&frik_pose_probe::g_arm_capture_active66100}){
  flag->store(true);ck(!combat_diag_probe::NativeBlockIndicatorH24(tick),"actual semantic menu/dialogue/cutscene/reading/calibration owner hides indicator");flag->store(false);
 }
 frik_pose_probe::g_gameplay_allowed=false;ck(!combat_diag_probe::NativeBlockIndicatorH24(tick),"gameplay disallow hides native block indicator");frik_pose_probe::g_gameplay_allowed=true;
 frik_pose_probe::trackingFresh=false;ck(!combat_diag_probe::NativeBlockIndicatorH24(tick),"tracking loss hides native block indicator");frik_pose_probe::trackingFresh=true;
 put(hs,0x64D,static_cast<unsigned char>(3));publish();ck(!combat_diag_probe::NativeBlockIndicatorH24(tick),"unknown MAX payload hides instead of treating corruption as block");
 put(hs,0x64D,static_cast<unsigned char>(1));put(hs,0x630,std::uintptr_t(0));publish();ck(!combat_diag_probe::NativeBlockIndicatorH24(tick),"unqualified native MAX property class hides indicator");put(hs,0x630,g_base+0x3B0A968);
 combat=0;publish();ck(combat_diag_probe::IncomingHmdDodgeH24(tick),"transient combat flag zero does not erase mutual current attack");
 ck(!combat_diag_probe::IncomingHmdDodgeH24(tick+61),"native attack publication expires after60ms");
 ++epoch;ck(!combat_diag_probe::IncomingHmdDodgeH24(tick),"reanchor epoch invalidates old context immediately");publish();
 put(ns,0xCF8,std::uintptr_t(1));publish();ck(!combat_diag_probe::IncomingHmdDodgeH24(tick),"swapped opponent backlink invalidates cache");put(ns,0xCF8,ptr(actor));
 put(hs,0x38,512);publish();ck(!combat_diag_probe::IncomingHmdDodgeH24(tick),"protected native player phase cannot authorize dodge");put(hs,0x38,128);
 put(ns,0x38,16);publish();ck(!combat_diag_probe::IncomingHmdDodgeH24(tick),"spent attack cannot consume next dodge window");
 native_vr_pad_premap::Context ctx{};native_vr_pad_premap::State pad{};
 for(int i=0;i<7;++i){tick=1100+i*20;CameraHook::poseTick=tick;CameraHook::x=i*.025;pad={};publish();ApplyRoomscalePad58(pad,ctx,true);}
 ck(pad.b&&CameraHook::motion.dodges==1,"pre-emptive side step in exact ordinary reciprocal combat requests dodge without waiting for an attack");
 ck(combat_diag_probe::EngagedHmdDodgeD74(tick)&&!combat_diag_probe::IncomingHmdDodgeH24(tick),"engaged input authority remains distinct from incoming-strike authority");
 retail_build112::fingerprint=false;ck(!combat_diag_probe::EngagedHmdDodgeD74(tick),"mismatched retail cannot authorize an engaged dodge");retail_build112::fingerprint=true;
 put(ns,0x38,512);publish();ck(!combat_diag_probe::EngagedHmdDodgeD74(tick),"paired/special opponent cannot authorize pre-emptive dodge");put(ns,0x38,16);
 put(ns,0xCF8,std::uintptr_t(0));publish();ck(!combat_diag_probe::EngagedHmdDodgeD74(tick),"no reciprocal opponent cannot turn jump into dodge");put(ns,0xCF8,ptr(actor));
 ctx.theater=true;pad={};ApplyRoomscalePad58(pad,ctx,true);ctx.theater=false;CameraHook::motion={};
 put(ns,0x38,8);for(int i=0;i<7;++i){tick=1300+i*20;CameraHook::poseTick=tick;CameraHook::x=.15+i*.025;pad={};publish();ApplyRoomscalePad58(pad,ctx,true);}
 ck(pad.b&&pad.lx>.99f&&CameraHook::motion.dodges==1,"actual caller sends native right dodge during current enemy attack");
 ctx.theater=true;tick+=20;CameraHook::poseTick=tick;pad={};ApplyRoomscalePad58(pad,ctx,true);ck(!pad.b&&!CameraHook::motion.dodgeUntil,"theater revokes held input immediately");ctx.theater=false;
 CameraHook::motion={};++epoch;tick+=20;CameraHook::poseTick=tick;pad={};publish();ApplyRoomscalePad58(pad,ctx,true);ck(!pad.b&&CameraHook::motion.histCount<=1,"pose epoch cannot join pre-reanchor head travel");
 CameraHook::poseTick=tick-61;pad={};ApplyRoomscalePad58(pad,ctx,true);ck(!pad.b&&!CameraHook::motion.dodgeUntil,"stale HMD fails closed");

 // The actual pad caller must not race its own LT owner into a dodge press.
 ctx.theater=true;pad={};ApplyRoomscalePad58(pad,ctx,true);ctx.theater=false;CameraHook::motion={};
 kcd2vr::hmd_dodge_h24::publishGuard(ptr(entity),ptr(actor),ptr(hs),epoch,tick,true);
 for(int i=0;i<6;++i){tick=3000+i*20;CameraHook::poseTick=tick;CameraHook::x=i*.04;pad={};publish();ApplyRoomscalePad58(pad,ctx,true);}
 ck(!pad.b,"actual caller queues dodge but withholds B while exact owned LT awaits release");
 kcd2vr::hmd_dodge_h24::Request req{};
 ck(kcd2vr::hmd_dodge_h24::currentRequest(ptr(entity),ptr(actor),ptr(hs),epoch,tick,req)&&req.needsRelease,"native consumer receives bounded exact-owner request before dodge transport");
 kcd2vr::hmd_dodge_h24::publishGuard(ptr(entity),ptr(actor),ptr(hs),epoch,tick,false);
 kcd2vr::hmd_dodge_h24::acknowledgeRelease(req.serial+1);
 tick+=10;CameraHook::poseTick=tick;pad={};publish();ApplyRoomscalePad58(pad,ctx,true);
 ck(!pad.b,"wrong serial and merely published held=false cannot bypass latched release ACK");
 kcd2vr::hmd_dodge_h24::acknowledgeRelease(req.serial);
 tick+=10;CameraHook::poseTick=tick;pad={};publish();ApplyRoomscalePad58(pad,ctx,true);
 ck(pad.b,"same request dispatches native B only after matching owned-release ACK");
 ck(!kcd2vr::hmd_dodge_h24::currentRequest(ptr(entity),ptr(npc),ptr(hs),epoch,tick,req),"foreign actor cannot consume request or release its own unrelated LT");
 ++epoch;ck(!kcd2vr::hmd_dodge_h24::currentRequest(ptr(entity),ptr(actor),ptr(hs),epoch,tick,req),"pose epoch retires active handoff token");
 currentEntity=nullptr;ck(!combat_diag_probe::IncomingHmdDodgeH24(tick),"current Henry replacement invalidates snapshot without native polling");
 ck(CameraHook::motion.anchorX==0&&CameraHook::motion.anchorY==0,"requests never warp camera or calibration anchors");
 std::printf("%dchecks %dfailures; native leaves replaced; perfect dodge remains UNPROVEN\n",checks,fail);return fail?1:0;
}
