// Exact production actor view, native lookup adapter/cache and original-first
// ActorUpdate callback. Only native memory, method implementation and effects
// outside this targeted resolution seam are explicit host leaves.
#include "diagnostics/actor_lookup_h7.h"
#include <array>
#include <vector>
#include <atomic>
#include <mutex>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#define __try try
#define __except(x) catch(...)
#ifndef _MSC_VER
#define __fastcall
#endif
namespace carrier_cb {static void Service(void*){} } // unrelated native carrier service leaf
static std::uint64_t tick=1000;static bool retail=true,enabled=true;static unsigned originalCalls=0,nativeCalls=0,drains=0,failures=0;
static unsigned checks=0;
static std::uint32_t keyH7=45102;static const void* resultH7=nullptr;static const void* resolvedH7=nullptr;
struct Region{std::uintptr_t at;std::size_t size;unsigned char* p;};static std::vector<Region> regions;
static std::vector<std::vector<unsigned char>> code;
static const void* __fastcall NativeMapH7(void*,std::uint32_t key){++nativeCalls;return key==keyH7?resultH7:nullptr;}
static std::uintptr_t g_base=0;
static constexpr std::uintptr_t kCombatActorVft=0x3E3FAC8,kCombatPlayerVft=0x3A80610,kPlayerVft=0x3AA4B98,kEntityVft=0x4072C78;
static std::atomic<std::uint64_t> g_counts[11]{};
static std::uint64_t GetTickCount64(){return tick;}
static bool SehCopy146(const void* p,void* out,std::size_t n){auto a=reinterpret_cast<std::uintptr_t>(p);
 for(const auto& r:regions)if(a>=r.at&&a-r.at<=r.size&&n<=r.size-(a-r.at)){std::memcpy(out,r.p+(a-r.at),n);return true;}return false;}
template<class T>static T Rd(const void* p,std::uintptr_t off,T fallback){T value{};return p&&SehCopy146(static_cast<const unsigned char*>(p)+off,&value,sizeof(value))?value:fallback;}
namespace frik_pose_probe {static const void* henry=nullptr;static const void* LastPlayerEntity(){return henry;}}
namespace retail_build112 {static bool Ready(std::uintptr_t base){return retail&&base==g_base;}}
namespace diagnostic_trace {void Write(const char*){}}
static bool Allow(int,int){return false;}
#include "generated_actor_native_view_h8.inc"
namespace counter36 {static unsigned observations=0;static void ActorUpdated(const void*,std::uint64_t){++observations;}}
namespace geom27 {
struct ActorSeen {const void* actor=nullptr;std::uint32_t entityId=0;std::uint64_t seen=0,resolved=0;};
static std::array<ActorSeen,32> g_actors{};static std::mutex g_actors_mutex;
#include "generated_actor_lookup_h8.inc"
static bool Enabled(){return enabled;}static void PublishHenryCombatContextH20(void*){}
namespace victim5916c3 {static std::atomic<unsigned> count{0};static void Observe(void*,std::uint64_t){}}
static void ObserveHenry178r31(void*){}
static void Drain(void*){++drains;resolvedH7=ActorForEntity(keyH7,tick);}
static void KickDrain37(void*){}
static bool TelegraphDirty178r34(){return false;}static void TelegraphRestore178r34(void*){}
namespace vrstate596 {static std::atomic<int>g_stage{0};static void Tick(const void*,std::uint64_t){}}
}
namespace services59c5 {static std::atomic<bool>staminaScaling59c17{false};}
namespace kcd2vr::hookcost178r22 {enum{ActorUpdate};struct Scope{Scope(int){}};struct Original{Original(int){}};}
static void NativeUpdateH7(void*,float){++originalCalls;}
static auto g_update_original=&NativeUpdateH7;
#include "generated_actor_callback_h8.inc"
template<std::size_t N>static void region(std::array<unsigned char,N>& a){regions.push_back({reinterpret_cast<std::uintptr_t>(a.data()),N,a.data()});}
template<class T,std::size_t N>static void put(std::array<unsigned char,N>& a,std::size_t at,T value){std::memcpy(a.data()+at,&value,sizeof(value));}
static void ck(bool ok,const char* msg){++checks;std::printf("%s %s\n",ok?"PASS":"FAIL",msg);failures+=!ok;}
int main(){
 std::setvbuf(stdout,nullptr,_IONBF,0);
 g_base=reinterpret_cast<std::uintptr_t>(&NativeMapH7);g_base-=kcd2vr::actor_lookup_h7::methodRva;
 namespace lookup=kcd2vr::actor_lookup_h7;
 std::array<unsigned char,0x200> game{},service{},entity{},henryEntity{};
 std::array<unsigned char,0x300> nativeActor{},combat{},henryActor{},player{};
 std::array<unsigned char,0xE20> state{},henryState{};
 region(game);region(service);region(entity);region(henryEntity);region(nativeActor);region(combat);region(henryActor);region(player);region(state);region(henryState);
 std::uintptr_t gamePointer=reinterpret_cast<std::uintptr_t>(game.data());regions.push_back({g_base+lookup::gameGlobal,sizeof(gamePointer),reinterpret_cast<unsigned char*>(&gamePointer)});
 std::uintptr_t method=g_base+lookup::methodRva;regions.push_back({g_base+lookup::serviceVft+0x18,sizeof(method),reinterpret_cast<unsigned char*>(&method)});
 for(const auto& pin:lookup::pins){code.emplace_back(pin.bytes,pin.bytes+pin.size);regions.push_back({g_base+pin.rva,pin.size,code.back().data()});}
 put(game,0x180,service.data());put(service,0,g_base+lookup::serviceVft);
 put(entity,0,g_base+kEntityVft);put(entity,0xC,keyH7);put(nativeActor,0x38,entity.data());put(nativeActor,0x278,combat.data());
 put(combat,0,g_base+kCombatActorVft);put(combat,0x1F8,nativeActor.data());put(combat,0x210,state.data());put(state,0xCE0,combat.data());
 put(henryEntity,0,g_base+kEntityVft);put(henryEntity,0xC,std::uint32_t{1});put(henryActor,0,g_base+kCombatPlayerVft);
 put(henryActor,0x1F8,player.data());put(henryActor,0x210,henryState.data());put(henryState,0xCE0,henryActor.data());
 put(player,0,g_base+kPlayerVft);put(player,0x38,henryEntity.data());put(player,0x278,henryActor.data());frik_pose_probe::henry=henryEntity.data();
 resultH7=combat.data(); // intentionally wrong: method returns C_Actor, not combat actor
 ck(!geom27::ActorForEntity(keyH7,tick)&&nativeCalls==0,"physical producer outside callback performs no native actor lookup");
 resultH7=nativeActor.data();

 ck(Rd<const void*>(henryState.data(),0xCF8,nullptr)==nullptr,"neutral NPC fixture has no native opponent");
 ActorUpdateDetour(henryActor.data(),.01f);
 ck(originalCalls==1&&nativeCalls==1&&drains==1&&resolvedH7==combat.data(),"original-first real Drain scope resolves neutral45102 absent Update cache/opponent");
 ck(lookup::callbackDepth==0,"borrowed callback scope expires after native Update returns");
 ck(counter36::observations==1,"native lookup never fabricates NPC ActorUpdated observation");
 ck(geom27::ActorForEntity(keyH7,tick)==combat.data()&&nativeCalls==1,"current native lookup seeds producer observation without another native call");
 auto invoke=[&](){resolvedH7=reinterpret_cast<void*>(1);ActorUpdateDetour(henryActor.data(),.01f);return resolvedH7;};
 auto savedCalls=nativeCalls;resultH7=nullptr;ck(!invoke()&&nativeCalls==savedCalls+1,"removed current map member cannot fall back to old cached actor");resultH7=nativeActor.data();
 put(entity,0xC,keyH7+1);ck(!invoke(),"wrong entity id from current native map refuses");put(entity,0xC,keyH7);
 put(entity,0,g_base+kEntityVft+8);ck(!invoke(),"wrong current entity vft refuses");put(entity,0,g_base+kEntityVft);
 put(combat,0,g_base+kCombatActorVft+8);ck(!invoke(),"wrong combat actor vft refuses");put(combat,0,g_base+kCombatActorVft);
 put(combat,0x1F8,player.data());ck(!invoke(),"combat actor belonging to another C_Actor refuses");put(combat,0x1F8,nativeActor.data());
 put(state,0xCE0,henryActor.data());ck(!invoke(),"combat-state backlink mismatch refuses");put(state,0xCE0,combat.data());
 put(nativeActor,0x278,henryActor.data());ck(!invoke(),"another actor cannot substitute for struck native actor");put(nativeActor,0x278,combat.data());
 auto before=nativeCalls;put(service,0,g_base+lookup::serviceVft+8);ck(!invoke()&&nativeCalls==before,"wrong actor-service vft refuses before method call");put(service,0,g_base+lookup::serviceVft);
 method++;ck(!invoke()&&nativeCalls==before,"wrong native method binding refuses before call");method--;
 retail=false;ck(!invoke()&&nativeCalls==before,"exact retail mismatch refuses before native work");retail=true;
 code[1][0]^=1;ck(!invoke()&&nativeCalls==before,"lookup code mismatch refuses before native work");code[1][0]^=1;
 ck(invoke()==combat.data(),"valid current entity still resolves after negative fixtures");
 put(combat,0x1F8,entity.data());ck(invoke()==combat.data(),"existing direct-entity NPC owner remains accepted by current native actor lookup");put(combat,0x1F8,nativeActor.data());
 auto other=keyH7+1;const void* wrong=nullptr;{lookup::CallbackScope scope;wrong=geom27::ActorForEntity(other,tick);}ck(!wrong,"native lookup does not overwrite or redirect another NPC key");
 enabled=false;before=nativeCalls;invoke();ck(nativeCalls==before&&lookup::callbackDepth==0,"geometry OFF performs no new native lookup and no lingering scope");enabled=true;
 tick=1500;ck(!geom27::ActorForEntity(keyH7,tick),"outside callback cannot renew500ms-old observed actor");
 std::printf("actor_lookup_h8 checks=%u failures=%u nativeCalls=%u nativeGameExecuted=0\n",checks,failures,nativeCalls);return failures?1:0;
}
