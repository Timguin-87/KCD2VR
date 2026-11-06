// Literal production View() dice-exit block + the actual owner/camera helpers.
// Fixture leaves are scalar native reads, original output and final FOV write.
#include "activity_lens59c10r1.h"
#include "core/snapshot_mailbox.h"
#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <type_traits>
using namespace kcd2vr;
using namespace kcd2vr::activity_lens59c10r1;
static unsigned checksR95=0,writesR95=0;
static void check(bool c,const char* why){++checksR95;if(!c){std::fprintf(stderr,"FAIL R95 DICE: %s\n",why);std::exit(1);}}
static std::uint64_t hostNowR95=1000;
static std::uint64_t GetTickCount64(){return hostNowR95;}
static constexpr std::uintptr_t base=0x180000000,playerR95=0x20000,diceR95=0x30000,gameR95=0x40000,managerR95=0x50000;
static std::map<std::uintptr_t,std::uint64_t> mem;
static float nativeR95=.6f;
static void* outputR95=reinterpret_cast<void*>(0x70000);
static void(*duringOutputReadR95)()=nullptr;
static bool outputReadableR95=true;
template<class T> bool Read(std::uintptr_t a,T& v){
 if(a==reinterpret_cast<std::uintptr_t>(outputR95)+0x30){
  if(duringOutputReadR95)duringOutputReadR95();if(!outputReadableR95)return false;
  v=static_cast<T>(nativeR95);return true;
 }
 auto i=mem.find(a);if(i==mem.end())return false;v=static_cast<T>(i->second);return true;
}
static bool WriteFov(void*,float f){++writesR95;nativeR95=f;return true;}
static core::SnapshotMailbox<Policy> policy;
static ExitHoldR89 exitHoldR89;
static std::atomic<std::uint64_t> exitHoldWritesR89{0};
static struct WitnessR95{std::atomic<bool> ready{true};std::atomic<unsigned> transitionDepth175{0};
 std::atomic<std::uint64_t> lifetime175{10};std::atomic<std::uintptr_t> dice{diceR95};} w;
static activity_eye59c19::State view;
static struct DialogR95{bool active=false;std::uintptr_t player=playerR95,entity=0x61000;} dialog;
static Evidence evidence;
// Diagnostics stay inert, but these are the same copied fields as production.
enum class Result:unsigned{Applied};
static std::atomic<std::uint64_t> observed[1]{};
struct Receipt{std::uint64_t now,tick;float native,written,requested;Evidence evidence;Result result;};
static core::SnapshotMailbox<Receipt> receipts;
static void run(bool readablePolicy=true){
 const auto now=std::uint64_t(GetTickCount64());Policy p{};const bool havePolicy=readablePolicy&&policy.read(p);
 const bool eyePolicy=havePolicy&&p.eye&&dialogue_state59c10r2::age(p.tick,now,80)&&std::isfinite(p.radians)&&p.radians>=1.f&&p.radians<=2.45f;
 const auto dice=w.dice.load();const bool recording=false;void* self=reinterpret_cast<void*>(playerR95+0x40);void* output=outputR95;nativeR95=.6f;
#include "dice_exit_adapter_r95.inc"
}
static void freshPolicy(){policy.publish({hostNowR95,1.65f,true,true});}
static void setup(){
 mem.clear();hostNowR95=1000;writesR95=0;exitHoldR89={};exitHoldWritesR89=0;dialog={};
 duringOutputReadR95=nullptr;outputReadableR95=true;w.ready=true;w.transitionDepth175=0;w.lifetime175=10;w.dice=diceR95;
 mem[playerR95]=base+0x3AA4B98;mem[playerR95+0x40]=base+0x3A81028;
 mem[playerR95+0x278]=0x62000;mem[0x62000]=base+0x3A80610;mem[0x621F8]=playerR95;mem[playerR95+0x38]=dialog.entity;
 mem[gameR95+0x150]=0x63000;mem[0x63000]=base+0x3E64C38;mem[0x63010]=0x64000;
 mem[0x64000]=base+0x3E64C90;mem[0x64090]=0;
 mem[base+0x549D388]=gameR95;mem[gameR95+0x38]=managerR95;mem[managerR95]=base+0x408E560;
 mem[managerR95+0x38]=mem[managerR95+0x3B]=0;mem[managerR95+0x30]=managerR95+0x338;
 mem[managerR95+0x338]=base+0x408EB60;mem[managerR95+0x1E8]=base+0x3A5A0F8;
 mem[managerR95+0x350]=base+0x3A5A3D0;
 view={};view.known=true;view.active=true;view.kindMask=32;view.lifetime=10;
 evidence={Owner::Minigame,32,Refusal::None,diceR95,0x408EB60};freshPolicy();
 run();check(exitHoldR89.armed&&writesR95==0,"qualified current dice visit arms without an exit write");
 w.dice=0;w.lifetime175=11;view.active=false;view.kindMask=0;view.lifetime=11;evidence={};
}

static void nativeDialogueR95(bool readableMap){
 // Existing verified dialogue membership and input-map adapters execute over
 // controlled native bytes; no synthetic "dialogue active" decision hook.
 mem[0x64090]=0x65000;mem[0x65000]=base+0x3E63A40;mem[0x65018]=base+0x3E63B60;
 mem[playerR95+0x668]=0x66000;mem[0x66000]=base+0x3F44F28;mem[0x66030]=77;
 mem[0x65278]=1;mem[0x65158]=0x67000;mem[0x670BC]=0;mem[0x67000]=0x68000;mem[0x67008]=0x68020;
 mem[0x68000]=77;mem[0x68018]=1;mem[0x640C0]=0x69000;mem[0x640C8]=0x69010;mem[0x69000]=0x65000;
 if(!readableMap)return;
 mem[base+0x549D328]=0x6A000;mem[0x6A000]=base+0x40472D0;mem[0x6A530]=0x6B000;mem[0x6B000]=base+0x4048018;
 mem[base+0x4048018+0x98]=base+0xA10AF4;mem[base+0x4048530+0xD0]=base+0x1A88910;mem[base+0x4048530+0xC8]=base+0x85ADF4;
 mem[0x6B050]=0x6C000;mem[0x6B058]=1;mem[0x6C008]=0x6D000;mem[0x6C019]=1;
 mem[0x6D019]=0;mem[0x6D008]=0x6C000;mem[0x6D020]=0x6E000;mem[0x6D028]=0x6F000;
 mem[0x6F000]=base+0x4048530;mem[0x6F010]=0x6B000;mem[0x6F058]=0x6E000;mem[0x6F009]=1;
 const char name[]="dialog";for(unsigned i=0;i<sizeof(name);++i)mem[0x6E000+i]=name[i];
}
int main(){
 setup();run();check(writesR95==1&&nativeR95==1.65f,"same dice exit with lingering native minigame camera restores FOV");
 hostNowR95=1081;run();check(writesR95==1&&exitHoldR89.armed&&nativeR95==.6f,"stale policy suspends writes without forgetting visit");
 freshPolicy();run();check(writesR95==2&&exitHoldR89.armed,"fresh enabled policy resumes the same exit restoration");
 run(false);check(writesR95==2&&exitHoldR89.armed,"unavailable policy cannot write or erase identity");
 run();check(writesR95==3,"available policy recovers after missed observation");
 view.known=false;run();check(writesR95==3&&exitHoldR89.armed,"unknown witness suspends writes and preserves identity");
 view.known=true;run();check(writesR95==4,"same witness can resume after an unavailable sample");
 mem.erase(managerR95+0x30);run();check(writesR95==4&&exitHoldR89.armed,"unreadable camera cannot authorize or erase debt");
 mem[managerR95+0x30]=managerR95+0x338;run();check(writesR95==5,"readable same native camera restores again");
 policy.publish({hostNowR95,1.65f,true,false});run();check(writesR95==5&&!exitHoldR89.armed,"explicit user off retires visit");
 freshPolicy();run();check(writesR95==5&&!exitHoldR89.armed,"reenable never resurrects retired visit");
 for(int bad=0;bad<9;++bad){setup();
  switch(bad){
   case 0:dialog.player++;break;
   case 1:view.kindMask=1;break;
   case 2:view.lifetime=w.lifetime175=13;break;
   case 3:w.dice=diceR95+8;view.kindMask=32;view.active=true;break;
   case 4:dialog.active=true;break;
   case 5:mem[managerR95+0x30]=managerR95+0x1E8;break;
   case 6:mem[managerR95+0x30]=managerR95+0x350;break;
   case 7:mem[managerR95+0x38]=1;break;
   case 8:mem[managerR95+0x3B]=1;break;
  }
  run();check(!writesR95&&!exitHoldR89.armed,"positive replacement/off/completion/protected camera revokes retained visit");
 }
 // Invalid policy values are unavailable authority, not explicit user-off.
 for(int bad=0;bad<3;++bad){setup();Policy p{hostNowR95,1.65f,true,true};
  if(bad==0)p.tick=hostNowR95+1;if(bad==1)p.radians=0;if(bad==2)p.radians=3;
  policy.publish(p);run();check(!writesR95&&exitHoldR89.armed,"future/invalid policy never grants a scalar write");
  freshPolicy();run();check(writesR95==1,"valid policy resumes preserved identity after invalid sample");
 }
 for(int bad=0;bad<13;++bad){setup();
  switch(bad){
   case 0:duringOutputReadR95=[](){++w.lifetime175;};break;
   case 1:duringOutputReadR95=[](){w.dice=diceR95+8;};break;
   case 2:duringOutputReadR95=[](){w.ready=false;};break;
   case 3:duringOutputReadR95=[](){w.transitionDepth175=1;};break;
   case 4:duringOutputReadR95=[](){policy.publish({hostNowR95,1.65f,true,false});};break;
   case 5:duringOutputReadR95=[](){mem[managerR95+0x30]=managerR95+0x350;};break;
   case 6:duringOutputReadR95=[](){mem[managerR95+0x38]=1;};break;
   case 7:duringOutputReadR95=[](){mem[managerR95+0x3B]=1;};break;
   case 8:duringOutputReadR95=[](){mem[0x64090]=0x65000;};break;
   case 9:duringOutputReadR95=[](){mem[playerR95+0x38]=0x61008;};break;
   case 10:duringOutputReadR95=[](){mem.erase(managerR95+0x30);};break;
   case 11:duringOutputReadR95=[](){nativeDialogueR95(true);};break;
   case 12:duringOutputReadR95=[](){nativeDialogueR95(false);};break;
  }
  run();check(!writesR95,"final writer rechecks lifecycle, policy, dialogue, player and camera after native output read");
  if(bad==4||bad==5||bad==6||bad==7||bad==9||bad==11)check(!exitHoldR89.armed,"positive final takeover/off retires visit immediately");
  if(bad==8||bad==10||bad==12)check(exitHoldR89.armed,"unavailable final native witness suspends without erasing visit");
 }
 std::printf("R95 dice exit actual adapter: %u checks PASS; no head/camera/stereo writes exercised\n",checksR95);
}
