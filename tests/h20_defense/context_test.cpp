#include <atomic>
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cmath>
#include <cstring>
#include "../../src/vrik/roomscale58.h"
#include "../../src/diagnostics/hmd_dodge_context_h24.h"
#define __fastcall
static std::uint64_t tick=1000;static bool geometry=false,originalSeen=false,valid=true,current=true;static int combat=1,reads=0,extra=0,fail=0;
static unsigned char henry[8]{},npc[8]{},entity[8]{},state[8]{};
static std::uint64_t GetTickCount64(){return tick;}
namespace kcd2vr::hookcost178r22{enum{ActorUpdate};struct Scope{Scope(int){}};struct Original{Original(int){}};}
namespace diagnostic_trace{static void Write(const char*){}}
static bool Allow(int,int){return false;}
static constexpr std::uintptr_t g_base=100,kCombatPlayerVft=30;
// H24: this legacy cache fixture intentionally has no reciprocal opponent
// memory. Missing native pair evidence must refuse dodge; the positive actual
// publisher/pad cases live in tests/h24_dodge/context_test.cpp.
static bool SehCopy146(const void*,void*,std::size_t){return false;}
namespace frik_pose_probe{std::uint64_t PoseEpochH24(){return 7;}void* LastPlayerEntity(){return entity;}}

template<class T> T Rd(const void* actor,int,T fallback){return actor==henry?T(g_base+kCombatPlayerVft):fallback;}
static bool CombatActor(const void* actor,const void*& e,const void*& s,bool=false){++reads;if(!originalSeen||actor!=henry||!valid)return false;e=entity;s=state;return true;}
static bool IsPlayer(const void* e){return current&&e==entity;}
static int B(const void*,int){++reads;return combat;}
static constexpr int kInCombat=0;
namespace geom27{
std::atomic<std::uint64_t> g_henry_seen178r31{0},g_henry_in_combat_edges178r33{0};std::atomic<int> g_henry_in_combat178r31{-1};
#include "../../src/diagnostics/henry_combat_context_h20.inl"
#include "actual_combat_cache_getter.inc"
namespace victim5916c3{std::atomic<int>count{0};void Observe(void*,std::uint64_t){++extra;}}
namespace vrstate596{std::atomic<int>g_stage{0};void Tick(const void*,std::uint64_t){++extra;}}
bool Enabled(){return geometry;}void ActorSeen27(void*){++extra;}void ObserveHenry178r31(void*){++extra;}void Drain(void*){++extra;}void KickDrain37(void*){++extra;}
bool TelegraphDirty178r34(){return false;}void TelegraphRestore178r34(void*){++extra;}
}
namespace services59c5{std::atomic<bool>staminaScaling59c17{false};}
static void OriginalUpdate(void*,float){originalSeen=true;}
static auto g_update_original=&OriginalUpdate;
#include "actual_actor_gameplay_prefix.inc"
namespace native_vr_pad_premap{struct State{float lx=0,ly=0;bool b=false;};struct Context{bool vr_menu=false,theater=false,system_menu=false,dialogue_navigation=false,semantic_minigame=false,apse_ui=false,back_context=false;};}
namespace animation_filter{std::atomic<bool>nativeBodyOwned112{false};}
namespace frik_pose_probe{bool BodyFollow128(){return false;}}
static std::atomic<bool>g_roomscale_follow58{false},g_roomscale_dodge58{true};
namespace diagnostics::capture168{bool any(std::uint64_t){return false;}}
namespace combat_diag_probe{
int HenryInCombat178r31(std::uint64_t now){return geom27::HenryInCombat178r31(now);}
bool IncomingHmdDodgeH24(std::uint64_t now){kcd2vr::hmd_dodge_h24::Context c{};return kcd2vr::hmd_dodge_h24::published.read(c)&&kcd2vr::hmd_dodge_h24::current(c,reinterpret_cast<std::uintptr_t>(entity),7,now);}
}
struct CameraHook{
 static inline kcd2vr::roomscale58::State motion{};static inline double x=0,y=0;static inline std::uint64_t poseTick=0;
 static bool RoomscaleRemaining(float&a,float&b,std::uint64_t&t){a=float(x);b=float(y);t=poseTick;return true;}
 static kcd2vr::roomscale58::Drive RoomscaleDrive(std::uint64_t now,bool follow,bool locked,bool enabled){kcd2vr::roomscale58::observeHmd(motion,poseTick,x,y);return kcd2vr::roomscale58::drive(motion,now,x,y,follow,locked,enabled,{});}
 static void RoomscaleCounters(std::uint64_t&a,std::uint64_t&b,std::uint64_t&c,float&d,float&e){a=b=c=0;d=e=0;}
};
#include "actual_roomscale_pad.inc"
static void ck(bool ok,const char*msg){std::printf("%s %s\n",ok?"PASS":"FAIL",msg);fail+=!ok;}
int main(){
 originalSeen=false;ActorUpdateDetour(henry,.01f);ck(originalSeen&&reads==2&&geom27::HenryInCombat178r31(tick)==1,"original-first Henry cache remains functional with geometry and recorder OFF");ck(extra==0,"no attributes/telegraph/physics/census work enabled");
 auto before=reads;ActorUpdateDetour(npc,.01f);ck(reads==before,"NPC Update never traverses Henry semantic publisher");
 native_vr_pad_premap::Context ctx{};native_vr_pad_premap::State pad{};
 for(int i=0;i<6;++i){tick=1100+i*20;CameraHook::poseTick=tick;CameraHook::x=i*.025;pad={};ActorUpdateDetour(henry,.02f);ApplyRoomscalePad58(pad,ctx,true);}
 ck(!pad.b&&CameraHook::motion.dodges==0,"H24 actual pad caller refuses combat-byte-only fixture with no current reciprocal incoming attack");
 for(int i=1;i<=35;++i){tick=1200+i*20;CameraHook::poseTick=tick;pad={};ApplyRoomscalePad58(pad,ctx,true);}
 ck(!pad.b&&CameraHook::motion.dodges==0,"stationary offset and missing attack proof never consume dodge cooldown");
 ctx.theater=true;tick+=20;CameraHook::poseTick=tick;pad={};ApplyRoomscalePad58(pad,ctx,true);ck(!pad.b&&!CameraHook::motion.dodgeUntil,"theater ownership revokes dodge immediately");ctx.theater=false;
 valid=false;tick+=20;ActorUpdateDetour(henry,.01f);ck(geom27::HenryInCombat178r31(tick)==-1,"failed native owner read invalidates cache immediately");valid=true;current=false;ActorUpdateDetour(henry,.01f);ck(geom27::HenryInCombat178r31(tick)==-1,"noncurrent Henry entity cannot publish combat");current=true;combat=255;ActorUpdateDetour(henry,.01f);ck(geom27::HenryInCombat178r31(tick)==-1,"failed combat-property read stays UNKNOWN");combat=0;ActorUpdateDetour(henry,.01f);ck(geom27::HenryInCombat178r31(tick)==0,"native leaving combat publishes zero");tick+=1001;ck(geom27::HenryInCombat178r31(tick)==-1,"stale or stopped native callback expires");
 printf("failures=%d native_leaves=EXPLICIT_TEST_REPLACEMENTS retail_execution=NONE\n",fail);return fail?1:0;
}
