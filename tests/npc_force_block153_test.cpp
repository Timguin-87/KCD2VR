// R153 NPC defence roll by skill: pure policy (npc_force_block153.h) and the production wh_cs_ForceBlock lease
// (src/subsystems/direct/host.inl slot 5008, compiled here unchanged against a fake console).
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include "../src/subsystems/direct/npc_force_block153.h"
#include "../src/subsystems/direct/training_vanilla_r163.h" // R163: the slot 5008 lease also yields to "Training fights: vanilla NPCs"
static int checks=0,failed=0;
static void ck(bool ok,const char* what){++checks;if(!ok){++failed;std::printf("FAIL %s\n",what);}}
// ---- fake console / mod.cpp services used by the lease -----------------------------------------------------------
static int cvarValue=0;static bool cvarExists=true,refuseWrites=false;static int writes=0;static std::string lastLine,lastStatus;
static int fakeCvar;
void* SafeGetCryConsole() noexcept {return &fakeCvar;}
void* SafeConsoleGetCVar(void*,const char* n) noexcept {return cvarExists&&std::strcmp(n,"wh_cs_ForceBlock")==0?&fakeCvar:nullptr;}
int SafeCVarGetInt(void*,int) noexcept {return cvarValue;}
static bool CheatCVarSetInt143(void*,int v) noexcept {++writes;if(!refuseWrites)cvarValue=v;return true;}
static bool CheatCVarSetFloat143(void*,float) noexcept {return true;}
namespace diagnostic_trace {void Write(const char* m) noexcept {lastLine=m;} void WriteStatus(const char* m) noexcept {lastStatus=m;}}
namespace npc_defense {inline std::atomic<bool> bySkill{true};}
struct RagdollConsole59 {bool Read(const char*,float&) noexcept {return false;} bool Write(const char*,float) noexcept {return false;}};
namespace retail_build112 {inline bool Ready(std::uintptr_t){return true;}}
inline void* GetModuleHandleW(const wchar_t*){return nullptr;}
namespace kcd2vr::routerH3 {struct Windows{};}
namespace kcd2vr::dodge_window_d70 {struct Lease{};}
#define KCD2VR_M2_HOST_SLOT 5008
#include "../src/subsystems/direct/host.inl"
#undef KCD2VR_M2_HOST_SLOT
namespace fb=kcd2vr::npc_force_block153;
static std::uint64_t now=10000;
static void publish(std::uintptr_t opp,bool recip,int skill,int phase){fb::opponent=opp;fb::reciprocal=recip;fb::skill=skill;fb::henryPhase=phase;fb::publishedMs=now;}
int main(){
 // chance by skill
 ck(fb::chance(0,false,10)==0.f,"mode 0 without BY SKILL = native, no roll");
 ck(fb::chance(0,true,0)==.25f&&fb::chance(0,true,-1)==.25f,"weak / unread enemy: 25 %");
 ck(fb::chance(0,true,30)==.75f&&fb::chance(0,true,40)==.75f,"strong enemy: 75 %");
 ck(fb::chance(0,true,15)>.49f&&fb::chance(0,true,15)<.51f,"skill 15: 50 %");
 ck(fb::chance(1,false,0)==.65f&&fb::chance(2,false,0)==1.f&&fb::chance(3,true,0)==1.f,"VrDirectNpcBlock 1/2/3 keep their C39 meaning");
 ck(fb::forceValue(0)==1&&fb::forceValue(2)==1&&fb::forceValue(3)==2,"normal block, mode 3 perfect block");
 {int s[5]{3,-1,12,41,7};ck(fb::bestSkill(s)==12,"best melee skill ignores out-of-range");int u[5]{-1,-1,-1,-1,-1};ck(fb::bestSkill(u)==-1,"unread skills");}
 // machine: one roll per swing, held during the swing, re-rolled after it
 {fb::Machine m{};
  ck(m.step(true,0,true,0xA0,true,0,1,.1f)==0&&m.rolls==1&&m.value==1,"won roll before the swing: decided, NOT leased yet (R156)");
  ck(m.step(true,0,true,0xA0,true,0,1,.99f)==0&&m.rolls==1,"no new roll while waiting for the swing, still native");
  ck(m.step(true,0,true,0xA0,true,0,4,.99f)==1&&m.step(true,0,true,0xA0,true,0,8,.99f)==1,"held through wind-up and strike");
  ck(m.step(true,0,true,0xA0,true,0,16,.99f)==0&&m.rolls==2,"swing over: next roll (lost) gives the native value");
  ck(m.step(true,0,true,0xA0,true,0,4,.0f)==0&&m.rolls==2,"a swing never changes the value mid-way");
  ck(m.step(true,0,true,0xB0,true,0,1,.0f)==0&&m.rolls==3&&m.value==1,"after the swing, a new opponent is rolled (won, pending)");
  ck(m.step(true,0,true,0xB0,true,0,4,.99f)==1,"the pending win is leased the moment the wind-up starts");
  ck(m.step(true,0,true,0xB0,true,0,32,.99f)==0,"and given back the tick the swing ends");
  ck(m.step(true,0,true,0xC0,true,0,1,.99f)==0&&m.rolls==5,"opponent change between swings re-rolls");
  ck(m.step(false,0,true,0xC0,true,0,1,.0f)==0&&!m.rolled,"inactive: nothing owned, no latch");
  ck(m.step(true,0,true,0xC0,false,0,1,.0f)==0,"not reciprocal: native");
  ck(m.step(true,0,false,0xC0,true,0,1,.0f)==0,"BY SKILL off, mode 0: native");
  ck(m.step(true,0,true,0xC0,true,0,1,.0f)==0&&m.value==1&&m.rolled,"switch back on: rolls again at once (no latch), pending");}
 // R154: first seen mid-swing (F4 switched on during a swing): the roll after it stays for the NEXT swing
 {fb::Machine m{};
  ck(m.step(true,0,true,0xA0,true,0,8,.0f)==0&&m.rolls==0,"mid-swing start: nothing owned yet");
  ck(m.step(true,0,true,0xA0,true,0,16,.0f)==0&&m.rolls==1&&m.value==1,"swing over: first roll (won, pending)");
  ck(m.step(true,0,true,0xA0,true,0,16,.99f)==0&&m.rolls==1,"the same roll holds until the next swing (no second roll)");
  ck(m.step(true,0,true,0xA0,true,0,4,.99f)==1&&m.step(true,0,true,0xA0,true,0,16,.99f)==0&&m.rolls==2,"next swing leases it, ends it, then a new roll");}
 // frequency over many swings follows the skill
 for(int skill:{0,15,30}){fb::Machine m{};std::uint32_t seed=12345u;int won=0;
  for(int i=0;i<4000;++i){m.step(true,0,true,0xA0,true,skill,1,fb::nextRoll(seed));won+=m.value>0;m.step(true,0,true,0xA0,true,skill,4,fb::nextRoll(seed));m.step(true,0,true,0xA0,true,skill,32,fb::nextRoll(seed));}
  const float rate=float(won)/4000.f,want=fb::chance(0,true,skill);char t[80];std::snprintf(t,sizeof(t),"skill %d defence rate %.2f near %.2f",skill,rate,want);
  ck(rate>want-.04f&&rate<want+.04f,t);}
 // R155: the lease exists only in an actual fight (pure gate used by mod.cpp)
 ck(fb::fightActive(true,true,1,false,false,false,false),"F4 on + Henry inCombat=1 + nothing else = fight");
 ck(!fb::fightActive(true,true,0,false,false,false,false)&&!fb::fightActive(true,true,-1,false,false,false,false),"Henry out of combat / unknown or stale flag: no lease");
 ck(!fb::fightActive(true,false,1,false,false,false,false)&&!fb::fightActive(false,true,1,false,false,false,false),"F4 mode off or console not ready: no lease");
 ck(!fb::fightActive(true,true,1,true,false,false,false)&&!fb::fightActive(true,true,1,false,true,false,false),"interactive dialogue / cutscene: no lease");
 ck(!fb::fightActive(true,true,1,false,false,true,false)&&!fb::fightActive(true,true,1,false,false,false,true),"Theater screen / menu: no lease");
 // production lease
 {NpcBlock39 lease{};RagdollConsole59 io;cvarValue=0;
  publish(0xA0,true,30,1);lease.Service(false,0,now,io);ck(cvarValue==0&&writes==0&&!lease.owned,"F4 off: cvar untouched");
  for(int i=0;i<200&&cvarValue==0;++i){now+=10;publish(0xA0,true,30,1);lease.Service(true,0,now,io);
   ck(cvarValue==0,"R156: between swings the cvar is never leased, won roll or not");
   publish(0xA0,true,30,4);lease.Service(true,0,now,io); // the wind-up: a won roll leases now
   if(cvarValue==0){publish(0xA0,true,30,32);lease.Service(true,0,now,io);}}
  ck(cvarValue==1&&lease.owned&&lease.original==0,"won roll leases wh_cs_ForceBlock=1 during the wind-up and remembers the original");
  ck(lastLine.find("R153 NPC_DEFENCE_ROLL")==0,"roll line for the recorder");
  publish(0xA0,true,30,8);lease.Service(true,0,now,io);ck(cvarValue==1,"held while Henry's strike runs");
  publish(0xA0,true,30,32);lease.Service(true,0,now,io);ck(cvarValue==0&&!lease.owned,"swing over: original back the same tick (R156)");
  for(int i=0;i<200&&cvarValue==0;++i){now+=10;publish(0xA0,true,30,1);lease.Service(true,0,now,io);publish(0xA0,true,30,4);lease.Service(true,0,now,io);if(cvarValue==0){publish(0xA0,true,30,32);lease.Service(true,0,now,io);}}
  ck(cvarValue==1,"a later won roll leases again in its wind-up");
  publish(0xA0,true,30,1);fb::publishedMs=now-2000;lease.Service(true,0,now,io);ck(cvarValue==0&&!lease.owned,"stale game data: original back");
  // mode 3 = perfect block, restore to a non-zero original
  cvarValue=5;publish(0xA0,true,0,1);lease.Service(true,3,now,io);publish(0xA0,true,0,4);lease.Service(true,3,now,io);ck(cvarValue==2&&lease.original==5,"mode 3 leases perfect block (in the swing)");
  lease.Service(false,3,now,io);ck(cvarValue==5&&!lease.owned,"F4 off: exact original value back");
  // refused write retries next tick, no latch
  cvarValue=0;refuseWrites=true;writes=0;publish(0xA0,true,0,1);lease.Service(true,2,now,io);publish(0xA0,true,0,4);lease.Service(true,2,now,io);lease.Service(true,2,now,io);
  ck(cvarValue==0&&writes>=2&&lease.written==0,"refused write is retried every tick");
  refuseWrites=false;lease.Service(true,2,now,io);ck(cvarValue==1,"write goes through once allowed");
  // console gone while owned: still owed, given back later
  cvarExists=false;lease.Service(false,2,now,io);ck(lease.owned,"console gone: restore still owed");
  cvarExists=true;lease.Service(false,2,now,io);ck(cvarValue==0&&!lease.owned,"restore done when the console is back");
  cvarExists=false;publish(0xA0,true,0,1);lease.Service(true,2,now,io);publish(0xA0,true,0,4);lease.Service(true,2,now,io);ck(lastStatus.find("wh_cs_ForceBlock not found")!=std::string::npos,"missing cvar: one log reason");
  cvarExists=true;lease.Restore();}
 // R156: practice / quest alias weapon in Henry's hand (prologue sparring with Capon) = no roll, native value, no latch
 ck(fb::practiceWeaponHeld(true,true,true,1),"held melee alias definition without its own weight = practice weapon");
 ck(!fb::practiceWeaponHeld(false,true,true,1)&&!fb::practiceWeaponHeld(true,false,true,1),"stowed item / non-melee item: not a practice weapon");
 ck(!fb::practiceWeaponHeld(true,true,false,1)&&!fb::practiceWeaponHeld(true,true,true,0),"real weight or a real weapon definition: a real weapon");
 {NpcBlock39 lease{};RagdollConsole59 io;cvarValue=0;writes=0;lastLine.clear();
  fb::practiceWeapon=true;
  for(int i=0;i<60;++i){now+=10;publish(0xA0,true,30,1);lease.Service(true,2,now,io);publish(0xA0,true,30,4);lease.Service(true,2,now,io);publish(0xA0,true,30,32);lease.Service(true,2,now,io);}
  ck(cvarValue==0&&writes==0&&!lease.owned&&lease.machine.rolls==0,"practice weapon: no roll, no write, even in mode 2 (always block)");
  ck(lastLine.find("R156 NPC defence roll skipped: practice")==0,"practice weapon: one short reason in the log");
  lastLine.clear();now+=10;publish(0xA0,true,30,1);lease.Service(true,2,now,io);ck(lastLine.empty(),"the reason is written once per edge, not per tick");
  fb::practiceWeapon=false;now+=10;publish(0xA0,true,30,1);lease.Service(true,2,now,io);publish(0xA0,true,30,4);lease.Service(true,2,now,io);
  ck(cvarValue==1&&lease.owned&&lease.machine.rolls==1,"real weapon drawn again: the roll runs on the next tick, leased in the swing (no latch)");
  fb::practiceWeapon=true;now+=10;publish(0xA0,true,30,1);lease.Service(true,2,now,io);ck(cvarValue==0&&!lease.owned,"practice weapon again: original value back at once");
  fb::practiceWeapon=false;lease.Service(false,2,now,io);}
 std::printf("%s %d/%d R153 NPC defence roll checks\n",failed?"FAIL":"PASS",checks-failed,checks);return failed?1:0;
}
