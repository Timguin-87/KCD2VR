// Actual retail actor/entity reader + applied-result adapter; native SubmitHit is an explicit leaf.
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <array>
#include <functional>
#include <vector>
static std::uintptr_t g_base=0x10000000;
namespace retail_build112 {inline bool ready=true;static bool Ready(std::uintptr_t b){return ready&&b==g_base;}}
static std::uint64_t GetTickCount64(){return 7000;}
static const void* playerEntity=nullptr;
static unsigned calls=0,notes=0,checks=0,failures=0;static bool result=true,argsOkay=true;
static std::uint32_t notedId=0;static std::uintptr_t notedEntity=0;static std::function<void()> during;
struct Block{std::array<unsigned char,0x1000> b{};void* ptr(){return b.data();}template<class T>void set(unsigned o,T v){std::memcpy(b.data()+o,&v,sizeof(v));}};
static Block attacker,target,as,ts,player,npc,ae,te,rpg,request,hit,other;
template<class T>T Rd(const void* p,std::uintptr_t o,T fallback){T v{};if(!p)return fallback;std::memcpy(&v,static_cast<const unsigned char*>(p)+o,sizeof(v));return v;}
static constexpr std::uintptr_t kCombatActorVft=0x3E3FAC8,kCombatPlayerVft=0x3A80610,kPlayerVft=0x3AA4B98,kEntityVft=0x4072C78;
#include <atomic>
static std::atomic<unsigned> g_counts[11];
#define Allow(a,b) false
namespace diagnostic_trace{static void Write(const char*){}}
#include "actual_combat_actor_r94.inc"
static bool IsPlayer(const void* p){return p&&p==playerEntity;}
namespace kcd2vr::gore32{static void NoteDead(std::uint32_t id,std::uintptr_t e,std::uint64_t tick){++notes;notedId=id;notedEntity=e;argsOkay&=tick==7000;}}
static float strength=3;static bool post=false;static int detail=0;
using SubmitHitFn=bool(*)(void*,void*,const float*,bool*,const void*,bool,int,void*);
static bool original(void* a,void* h,const float* s,bool* p,const void* q,bool apply,int kind,void* d){
 ++calls;argsOkay&=a==rpg.ptr()&&h==hit.ptr()&&s==&strength&&p==&post&&q==request.ptr()&&kind==5&&d==&detail;
 if(during)during();return result;
}
static SubmitHitFn g_submit_original=&original;
#include "subsystems/gore/native_kill_observer_r94.inl"
static void ck(bool b,const char* what){++checks;if(!b){++failures;std::printf("FAIL %s\n",what);}}
static void setup(){
 for(auto* p:{&attacker,&target,&as,&ts,&player,&npc,&ae,&te,&rpg,&request,&hit,&other})p->b.fill(0);
 attacker.set(0,g_base+kCombatPlayerVft);attacker.set(0x210,as.ptr());as.set(0xCE0,attacker.ptr());attacker.set(0x1F8,player.ptr());
 player.set(0,g_base+kPlayerVft);player.set(0x278,attacker.ptr());player.set(0x38,ae.ptr());ae.set(0,g_base+kEntityVft);ae.set(0xC,std::uint32_t(11));
 target.set(0,g_base+kCombatActorVft);target.set(0x210,ts.ptr());ts.set(0xCE0,target.ptr());target.set(0x1F8,npc.ptr());npc.set(0x38,te.ptr());
 te.set(0,g_base+kEntityVft);te.set(0xC,std::uint32_t(22));rpg.set(8,attacker.ptr());request.set(0x10,target.ptr());
 hit.set(0,std::uint32_t(11));hit.set(0x28,std::uint32_t(22));hit.set(0x40,std::uint8_t(1));
 calls=notes=notedId=0;notedEntity=0;argsOkay=result=retail_build112::ready=true;during={};playerEntity=ae.ptr();g_submit_original=&original;
}
static bool submit(bool apply=true){return SubmitObservedNativeKillR94(rpg.ptr(),hit.ptr(),&strength,&post,request.ptr(),apply,5,&detail);}
static void no(const char* what,bool apply=true){ck(submit(apply)==result,"original result preserved");ck(calls==1&&argsOkay,"one original with unchanged arguments");ck(notes==0,what);}
int main(){
 setup();ck(submit()&&calls==1&&argsOkay,"applied native call once");ck(notes==1&&notedId==22&&notedEntity==reinterpret_cast<std::uintptr_t>(te.ptr()),"exact target death recorded");
 setup();no("evaluation never proves death",false);
 setup();result=false;no("native refusal never proves death");
 setup();hit.set(0x40,std::uint8_t(0));hit.set(0x41,std::uint8_t(1));no("KO alone is not death");
 setup();hit.set(0x40,std::uint8_t(2));no("invalid death byte rejected");
 setup();retail_build112::ready=false;no("unsupported retail refused");
 setup();playerEntity=other.ptr();no("non-Henry refused");
 setup();target.set(0,std::uintptr_t(123));no("unknown target actor refused");
 setup();request.set(0x10,static_cast<void*>(nullptr));no("missing target refused");
 setup();ts.set(0xCE0,other.ptr());no("broken actor backlink refused");
 setup();te.set(0xC,std::uint32_t(33));no("hit target ID must agree");
 setup();hit.set(0,std::uint32_t(44));no("hit attacker ID must agree");
 setup();during=[](){request.set(0x10,other.ptr());};no("target actor changed inside native call");
 setup();during=[](){rpg.set(8,other.ptr());};no("attacker changed inside native call");
 setup();during=[](){other.set(0,g_base+kEntityVft);other.set(0xC,std::uint32_t(22));npc.set(0x38,other.ptr());};no("same target ID replacement entity refused");
 setup();during=[](){other.set(0xCE0,target.ptr());target.set(0x210,other.ptr());};no("same actor changed state refused");
 setup();during=[](){te.set(0xC,std::uint32_t(33));hit.set(0x28,std::uint32_t(33));};no("both result and target ID replaced inside callback refused");
 setup();during=[](){playerEntity=other.ptr();};no("current Henry replaced during callback");
 setup();during=[](){retail_build112::ready=false;};no("retail contract revoked during callback");
 setup();hit.set(0x40,std::uint8_t(0));during=[](){hit.set(0x40,std::uint8_t(1));};ck(submit()&&notes==1,"native call may create death result");
 setup();g_submit_original=nullptr;ck(!submit()&&calls==0&&notes==0,"missing original cannot create proof");
 std::printf("native_kill_observer_r94 %u/%u\n",checks-failures,checks);return failures?1:0;
}
