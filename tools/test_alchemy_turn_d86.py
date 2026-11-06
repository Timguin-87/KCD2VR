#!/usr/bin/env python3
"""Execute the current C46 owner/clock/restore functions with bounded native leaves.

The retail memory layout is modelled; no engine or DLL is executed. The native
Update/scanner leaf advances the actual admitted FIFO and counts authored events
once. Production functions, rather than a reimplementation of their decisions,
consume current input, ownership, native clock and the D86 before-event policy.
"""
from pathlib import Path
import argparse,hashlib,json,os,re,subprocess
from test_crossbow_mechanism_d78 import function
ROOT=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser(description=__doc__)
p.add_argument('--out',type=Path,required=True)
p.add_argument('--baseline',type=Path,help='Optional original source tree; runs the historical owner-clock negative control when supplied')
a=p.parse_args();out=a.out.resolve();out.mkdir(parents=True,exist_ok=True)
semi=(ROOT/'src/alchemy_semi_native46.inl').read_text()
resource=(ROOT/'src/alchemy_resource46.inl').read_text()
shared=(ROOT/'src/vrik/alchemy_semi46.h').read_text()
cpp=(ROOT/'src/action_time_owner59c11.cpp').read_text()
lease=semi[semi.index('struct SemiAlchemyLease46 {'):semi.index('semi46::WorkWitness g_semi_retired46')]
state=semi[semi.index('struct SemiResourceState46 {'):semi.index('// D67R1: WHGame')]
structs=shared[shared.index('enum Kind :'):shared.index('struct BookEnable')]
same=function(shared,'inline bool same(')
constants='\n'.join(line for line in cpp.splitlines() if line.startswith('constexpr ') and any(x in line for x in ['kCtrlContext=','kScopeStride=','kScopeLayer=','kActContext=','kSkelChar=']))+'\nconstexpr unsigned kMaxScopes=24;\n'
pending=resource[resource.index('struct SemiResourcePending19 {'):resource.index('// Before Attach, an admitted resource')]
extracted='\n'.join([function(semi,'static int SemiAlchemyMembership46('),function(semi,'static bool SemiAlchemyRestore46('),function(semi,'static bool SemiAlchemyAcquire46('),function(resource,'static bool SemiResourceInterval46('),function(resource,'static bool SemiResourceMarkersCurrent19('),function(resource,'static bool SemiResourceClockBeforeWork19('),function(resource,'static bool SemiResourceDirectClockD86(void* ctrl,void* character,const void* event,SemiResourceState46& interval) noexcept {'),function(resource,'static void SemiResourceDetachedD86('),function(resource,'static float SemiResourceTurnRateD86('),function(resource,'static float SemiResourceTailRateD86('),function(semi,'static void TickSemiAlchemy46(')])
host=r'''
#include "subsystems/alchemy/resource_turn_d86.h"
#include "subsystems/alchemy/gesture_work67.h"
#include "vrik/alchemy_work_clock_d47.h"
#include "vrik/alchemy_resource_scope_d67r1.h"
#include <array>
#include <atomic>
#include <cstring>
#include <string>
#include <vector>
#include <cstdio>
#include <cstdlib>
#define __fastcall
namespace semi46 {
__STRUCTS__
__SAME__
struct WorkBox { bool publish(const WorkWitness&){return true;} } work;
}
__CONSTANTS__
__LEASE__
__STATE__
namespace kcd2vr::subsystems::alchemy::bridge67 {struct Control {}; }
__PENDING__
static semi46::WorkWitness g_semi_retired46{};
static std::atomic<bool> g_semi_pending46{false};
static std::array<unsigned char,0x20000> mem{};
static std::uint64_t now=1000;
static bool owner=true,inputValid=true,tracking=true,pins=true,tags=true,fullVr=true,parentValid=true;
static bool newerStop=false,restoreReadsFail=false;
static unsigned inputReads=0,writeCount=0;
static int events=0,quantities=0,fx=0,proceduralFx=0,detachEvents=0;
static double ordinaryWorldSeconds=0,fastestEventRate=0;
static float requested=8.f;
static std::vector<std::string> receipts;
static constexpr std::uintptr_t ctrl=0x1000,root=0x2000,child=0x3000,ctx=0x4000,character=0x5000,entries=0x7000,header=0x8000,eventList=0x9000,instance=0xA000;
static std::atomic<std::uintptr_t> g_base{0x1000000};
static std::atomic<void*> g_henry_ctrl{reinterpret_cast<void*>(ctrl)};
static semi46::WorkWitness live{};
static semi46::Control physical{};
struct Owner {int state=0;} g_owner;
static constexpr int Idle=0;
static std::uint64_t GetTickCount64(){return now;}
static bool OnAlchemyUpdateOwnerR27(){return owner;}
static bool IsHenryController(void* p){return p==reinterpret_cast<void*>(ctrl);}
static bool SehCopy(const void* p,void* out,std::size_t n){auto at=reinterpret_cast<std::uintptr_t>(p);if(!restoreReadsFail&&at==g_base.load()+0x406BB38+0xF0&&n==sizeof(std::uintptr_t)){const auto value=g_base.load()+0x955DF4;std::memcpy(out,&value,n);return true;}if(restoreReadsFail||!at||at+n>mem.size())return false;std::memcpy(out,mem.data()+at,n);return true;}
template<class T> static T Rd(const void* p,std::ptrdiff_t off,T fallback){T v{};return SehCopy(reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(p)+off),&v,sizeof(v))?v:fallback;}
template<class T> static bool Wr(void* p,std::ptrdiff_t off,T v){auto at=reinterpret_cast<std::uintptr_t>(p)+off;if(!at||at+sizeof(v)>mem.size())return false;std::memcpy(mem.data()+at,&v,sizeof(v));++writeCount;return true;}
template<class T> static void put(std::uintptr_t at,T v){std::memcpy(mem.data()+at,&v,sizeof(v));}
static std::uintptr_t lookupHeader(void*,int animation){return animation==54?header:0;}
template<class T> static T Native13(std::uintptr_t rva){return rva==0x955DF4?reinterpret_cast<T>(&lookupHeader):nullptr;}
static void SemiResourceJoinRefusedD64(const semi46::WorkWitness&,const char*,std::uint64_t=0,std::uint64_t=0){}
static float nativeSpeed(){return Rd<float>(reinterpret_cast<void*>(child),kActSpeedBias,-1.f);}
static float AlchemyTransferSpeedD65(){return requested;}
static bool SemiAlchemyPins46(){return pins;}
static bool SemiResourceTurnPinsD86(){return pins;}
static bool SemiResourceTagsCurrent19(const SemiResourceState46&){return tags;}
namespace kcd2vr::workstation178r20c1 {static bool fullVrAlchemy63(std::uint64_t){return fullVr;}}
static bool SemiAlchemyParent46(void*,semi46::WorkWitness& w,std::uintptr_t expected=0,bool=false,bool=false){w=live;return parentValid&&(!expected||expected==live.parent);}
static bool SemiAlchemyInput46(const semi46::WorkWitness& w,std::uint64_t tick,semi46::Control& c){
 c=physical;++inputReads;if(newerStop&&inputReads>=2)c.clockStop=true;
 return inputValid&&tracking&&c.enabled&&c.tick&&tick>=c.tick&&tick-c.tick<=80&&c.instance==w.instance&&c.lifetime==w.lifetime&&!c.manualUse;
}
static void SemiAlchemyReceipt46(const char* s,const semi46::WorkWitness&,float=-1.f){receipts.emplace_back(s);}
static bool AlchemyPresentationRateD81(const semi46::WorkWitness&,float&){return false;}
static bool HandoffAlchemyPresentationD81(const semi46::WorkWitness&){return true;}
static kcd2vr::alchemy_resource_scope_d67r1::Result SemiResourceRoot3D67R1(const SemiResourceState46&,void*,const kcd2vr::alchemy_resource_scope_d67r1::Proof* =nullptr){return {kcd2vr::alchemy_resource_scope_d67r1::Reason::Ok,{}};}
static void SemiResourceConsume19(void*,std::uint64_t){}
static constexpr std::uintptr_t kSkelVft=0x3A711E0;
namespace animation_filter {static void* PlayerSkeletonAnim(){return reinterpret_cast<void*>(character+0x110);}}
static bool FragmentCrc19c4(void*,int,std::uint32_t& crc){crc=0xC5990708;return true;}
struct SemiResourceScan46 {void* skel=nullptr;const void* info=nullptr;float from=0,to=0;int segment=0;bool valid=false;};
static SemiResourceScan46 g_semi_scan46{};
struct SemiResourceInfo46 {std::uintptr_t list=0,path=0;int first=0,animation=-1,last=0;float weight=0;};
struct SemiResourceIntervalTraceR25 {const semi46::WorkWitness& owner;const char* stage=nullptr;float from=0,to=0;int animation=-1;};

__EXTRACTED__
static unsigned checks=0;
static void check(bool ok,const char* why){++checks;if(!ok){std::fprintf(stderr,"FAIL_ALCHEMY_TURN %s speed=%g phase=%d norm=%g\n",why,nativeSpeed(),live.phase,Rd<float>(reinterpret_cast<void*>(entries),0x44,-1.f));std::exit(1);}}

static void gestureTests(){
 namespace g=kcd2vr::subsystems::alchemy::gesture67;
 struct Hand {bool valid=true;float position[3]{},rotation[4]{0,0,0,1},grip=.8f;};
 struct Frame {bool allowed=true,headValid=true;std::uint64_t sequence=1,timeNs=1000000000,tick=1000,poseEpoch=18,sourceEpoch=15;float head[3]{};Hand hand[2]{};} f;
 g::Input in{};in.enabled=true;in.referenceRotationValid=true;in.work={1,2,3,4,1000,5,g::Resource,1,true,false};
 in.grip[1]={true,1,2,4,1000,18,15,6,1,13};in.pot={true,1,4,1000,18,15,7,6,{0,0,0},.20f};
 in.palm[1]={true,{0,0,0},1000,18,15};in.referenceRotation[3]=1;
 auto tilt=[&](float angle){f.hand[1].rotation[0]=std::sin(angle*.5f);f.hand[1].rotation[3]=std::cos(angle*.5f);};
 auto fresh=[&](){++f.sequence;f.timeNs+=10000000;f.tick+=10;in.work.tick=in.grip[1].tick=in.pot.tick=in.palm[1].tick=f.tick;};
 g::Controller c;tilt(.9f);(void)c.step(in,f,f.tick);fresh();auto o=c.step(in,f,f.tick);
 check(!o.clockStop&&o.totalSeconds>0,"real gesture producer supplies phase1 tilt credit");const double before=o.totalSeconds;
 in.work.phase=2;in.resourceTailD86=true;fresh();o=c.step(in,f,f.tick);
 check(o.completedHeld&&!o.clockStop&&o.totalSeconds>before&&!o.use,"same native Use phase retains tilt credit for its original FX tail, no new Use");
 const auto once=o.totalSeconds;o=c.step(in,f,f.tick);check(o.totalSeconds==once,"duplicate XR sample cannot replay tail credit");
 tilt(0);fresh();o=c.step(in,f,f.tick);check(o.clockStop&&o.totalSeconds==once,"upright away from home stops actual gesture tail");
 tilt(.9f);fresh();o=c.step(in,f,f.tick);check(!o.clockStop&&o.totalSeconds>once,"fresh actual tilt resumes original tail");
 tilt(0);in.resourceHomeD86=true;fresh();o=c.step(in,f,f.tick);check(!o.clockStop&&!o.use,"current original home gesture earns ordinary return time");
 f.hand[1].grip=0;fresh();o=c.step(in,f,f.tick);check(o.clockStop&&o.releaseRequested&&o.status==g::Status::NativeReturn,"release takes original native return once");
 g::Controller unqualified;f.hand[1].grip=.8f;in.resourceTailD86=in.resourceHomeD86=false;fresh();o=unqualified.step(in,f,f.tick);
 check(o.clockStop&&o.completedHeld,"unqualified completed resource retains legacy pause");
 in.resourceTailD86=true;in.grip[1].entity=0;fresh();o=unqualified.step(in,f,f.tick);check(o.clockStop&&!o.enabled,"lost native held identity cannot run tail");
}
static void setup(unsigned kind=semi46::Resource,float boundary=.420895f){
 mem.fill(0);g_semi_alchemy46={};g_semi_retired46={};g_semi_resource46={};g_semi_pending46=false;
 owner=inputValid=tracking=pins=tags=fullVr=parentValid=true;newerStop=restoreReadsFail=false;
 inputReads=writeCount=0;events=quantities=fx=proceduralFx=detachEvents=0;ordinaryWorldSeconds=fastestEventRate=0;requested=8;now=1000;receipts.clear();
 live={instance,0xB000,child,11,now,55432,kind,1,3,true,false};
 physical={};physical.instance=instance;physical.lifetime=11;physical.parent=live.parent;physical.child=child;physical.token=live.token;
 physical.enabled=true;physical.tick=now;physical.sequence=1;physical.poseEpoch=18;physical.sourceEpoch=15;physical.hand=1;physical.tracked[1]=true;physical.clockStop=true;
 put(ctrl+kCtrlContext,ctx);put(ctrl+kCtrlScopeCount,1u);put(ctrl+kCtrlScopes,root);put(ctrl+kCtrlTimeScale,1.f);
 put(root,g_base.load()+0x3AEA100);put(root+0x20,ctx);put(root+kScopeAction,child);put(root+0x30,0u);put(root+0x34,1u);put(ctx+0x10,character);
 put(root+0x40,std::uintptr_t(0xC000));put(root+0x48,std::uintptr_t(0xC0C0));
 put(0xC039,static_cast<unsigned char>(1));put(0xC01C,2.733333f-.166128f*6.34f);
 put(0xC068,std::uintptr_t(0xD000));put(0xD000,g_base.load()+0x3AB5680);
 put(0xC0A8,std::uintptr_t(0xD100));put(0xD100,g_base.load()+0x3AB60A8);
 put(child,g_base.load()+0x3A47948);put(child+kActContext,ctx);put(child+kActRootScope,root);put(child+0x268,instance);put(child+kActToken,live.token);
 put(child+kActStatus,2);put(child+kActSpeedBias,1.f);put(child+kActActiveTime,0.f);
 put(character+0x110+0x48,entries);put(entries-4,1u);put(entries,g_base.load()+0x3A70A60);put(entries+0x10,54u);
 put(entries+0x34,6.34f);put(entries+0x44,.166128f);put(header+0x34,6.34f);put(header+0xA4,static_cast<unsigned char>(1));
 put(character,g_base.load()+0x40602F8);put(character+0x110+kSkelChar,character);put(character+0xA70,std::uintptr_t(0x16000));put(0x16218,std::uintptr_t(0x17000));put(0x17000,g_base.load()+0x406BB38);
 put(header+0x60,0.f);put(header+0x64,1.f);put(header+0x2C,0.f);put(header+0x30,6.34f);
 put(header+0x48,g_base.load()+0x3A7D9D0);put(header+0x50,eventList);put(eventList-4,4u);
 put(eventList+0x50,boundary);put(eventList+0x58,0x94B1CC4Bu);put(eventList+0x5C,0);
 put(eventList+0xA0,.866421f);put(eventList+0xA8,0xD3845300u);put(eventList+0xAC,0);
 g_semi_resource46.owner=live;g_semi_resource46.phase=1;g_semi_resource46.firstWork=.420895f;g_semi_resource46.useTime=.420895f;g_semi_resource46.detachTime67=.866421f;g_semi_resource46.animationId=54;
 auto& p=g_semi_resource46.turnD86;p.root=root;p.context=ctx;p.character=character;p.entries=entries;p.header=header;p.eventEntries=eventList;p.boundaryEvent=eventList+0x50;
 p.procBegin=0xC000;p.procEnd=0xC0C0;p.layer=0;p.layers=1;p.eventCount=4;p.detachEvent=eventList+0xA0;p.detach=.866421f;p.boundaryCrc=0x94B1CC4B;p.animation=54;p.duration=6.34f;p.headerDuration=6.34f;p.afterAttach=.166128f;p.boundary=boundary;
 check(SemiAlchemyAcquire46(reinterpret_cast<void*>(ctrl),live,false,kind==semi46::Resource),"real production acquisition pauses current child");
 check(nativeSpeed()==0,"new acquisition is paused");
}

static void intervalTests(){
 setup();
 put(character+0x110,g_base.load()+kSkelVft);put(instance+0x18,std::uintptr_t(0xE000));put(0xEA50,std::uintptr_t(0xF000));put(0xF090,42);put(child+kActFragId,42);
 put(live.parent+0x68,0xDEC97C81u);put(live.parent+0x6C,0xD3845300u);put(live.parent+0x70,0x94B1CC4Bu);put(instance+0x304,13);put(instance+0x428,static_cast<unsigned char>(0));
 SemiResourceInfo46 info{header+0x48,0x12000,0,54,1,1.f};put(0x11000,info);
 for(unsigned i=0;i<4;++i){put(eventList+i*0x50+0x28,std::uintptr_t(0x13000));put(eventList+i*0x50+0xC,0);}
 put(eventList,.165368f);put(eventList+8,0xDEC97C81u);
 put(eventList+0x50,.420895f);put(eventList+0x58,0x94B1CC4Bu);
 put(eventList+0xA0,.866421f);put(eventList+0xA8,0xD3845300u);
 put(eventList+0xF0,.3f);put(eventList+0xF8,0x12345678u);
 put(0x14000,.165368f);put(0x14008,0);put(0x14010,std::uintptr_t(0x12000));put(0x14018,54);put(0x1401C,0xDEC97C81u);put(0x14050,eventList);
 g_semi_scan46={reinterpret_cast<void*>(character+0x110),reinterpret_cast<void*>(0x11000),.15f,.166128f,0,true};
 SemiResourceState46 qualified;
 check(SemiResourceInterval46(live,reinterpret_cast<void*>(0x14000),qualified),"production Attach scanner qualifies actual native event list");
 check(qualified.turnD86.boundary==.3f&&qualified.turnD86.boundaryEvent==eventList+0xF0,"unknown authored FX closes acceleration before Use");
 check(!qualified.turnD86.root&&!qualified.turnD86.entries,"scanner-only direct Attach begins without preseeded live clock proof");
 const auto beforeClockWrites=writeCount;
 check(SemiResourceDirectClockD86(reinterpret_cast<void*>(ctrl),reinterpret_cast<void*>(character),reinterpret_cast<void*>(0x14000),qualified),"actual direct Attach runs actual CAF lookup and BeforeWork qualifier");
 check(writeCount==beforeClockWrites&&qualified.turnD86.root==root&&qualified.turnD86.entries==entries&&qualified.turnD86.duration==6.34f,"direct clock proof is read-only and populates current FIFO/duration");
 g_semi_resource46=qualified;live.phase=2;g_semi_resource46.phase=2;
 check(SemiResourceTailRateD86(live,1.f,.01,false)>0.f,"direct Attach admission permits ordinary phase2 gesture tail");live.phase=1;
 put(header+0x34,6.3405f);
 check(SemiResourceDirectClockD86(reinterpret_cast<void*>(ctrl),reinterpret_cast<void*>(character),reinterpret_cast<void*>(0x14000),qualified)&&qualified.turnD86.headerDuration==6.3405f,"original CAF admission tolerates its actual sub-millisecond header/FIFO difference");
 g_semi_resource46=qualified;live.phase=2;g_semi_resource46.phase=2;
 check(SemiResourceTailRateD86(live,1.f,.01,false)>0.f,"ordinary tail accepts the unchanged separately stored header duration");
 put(header+0x34,6.3406f);check(SemiResourceTailRateD86(live,1.f,.01,false)==0.f,"later header duration mutation cannot borrow the admitted tolerance");
 put(header+0x34,6.34f);live.phase=1;


 put(eventList+0xF0,.5f);check(SemiResourceInterval46(live,reinterpret_cast<void*>(0x14000),qualified)&&qualified.turnD86.boundary==.420895f,"production first-event boundary uses earliest actual marker");
 put(eventList+0xF0,std::nanf(""));check(SemiResourceInterval46(live,reinterpret_cast<void*>(0x14000),qualified)&&qualified.turnD86.boundaryEvent==0,"unreadable unknown marker disables only new fast interval, not existing work");
 check(SemiResourceDirectClockD86(reinterpret_cast<void*>(ctrl),reinterpret_cast<void*>(character),reinterpret_cast<void*>(0x14000),qualified),"unknown optional marker does not revoke direct CAF admission");
 g_semi_resource46=qualified;
 check(SemiResourceTurnRateD86(reinterpret_cast<void*>(ctrl),live,.01f,.01,true)==0.f,"missing optional fast marker forbids acceleration");
 live.phase=2;g_semi_resource46.phase=2;
 check(SemiResourceTailRateD86(live,1.f,.01,false)>0.f,"optional fast boundary disabled still allows ordinary gestured FX tail");
 g_semi_resource46.turnD86.procBegin=g_semi_resource46.turnD86.procEnd=0;
 check(SemiResourceTailRateD86(live,1.f,.01,false)>0.f,"missing optional procedural fast proof does not suppress ordinary tail");
 put(eventList+0xA0,.9f);check(SemiResourceTailRateD86(live,1.f,.01,false)==0.f,"changed mandatory Detach marker closes unsafe ordinary tail");put(eventList+0xA0,.866421f);
 live.phase=1;g_semi_resource46.phase=1;
 put(entries+0x10,55u);check(!SemiResourceDirectClockD86(reinterpret_cast<void*>(ctrl),reinterpret_cast<void*>(character),reinterpret_cast<void*>(0x14000),qualified),"direct Attach with a different live CAF remains native");put(entries+0x10,54u);

 put(eventList+0xF0,.3f);g_semi_scan46.to=.43f;check(!SemiResourceInterval46(live,reinterpret_cast<void*>(0x14000),qualified),"same native scan crossing Attach and Use cannot claim gesture clock");
 g_semi_scan46.to=.17f;put(instance+0x304,18);check(!SemiResourceInterval46(live,reinterpret_cast<void*>(0x14000),qualified),"unsupported non-carafe native verb cannot borrow asset profile");
}
static void input(bool stop=false,double credit=.01){++physical.sequence;physical.tick=now;physical.clockStop=stop;if(!stop)physical.totalSeconds+=credit;inputReads=0;}
static void tick(float dt=.01f){TickSemiAlchemy46(reinterpret_cast<void*>(ctrl),dt,now);}
// Controlled native Update/scanner leaf. The production adapter never calls
// these effects. Monotone native time crosses each authored marker only once.
static void native(float dt=.01f){
 const float speed=nativeSpeed(),scale=Rd<float>(reinterpret_cast<void*>(ctrl),kCtrlTimeScale,1.f);
 const float before=Rd<float>(reinterpret_cast<void*>(entries),0x44,0.f),delta=dt*scale*speed;
 const float after=before+delta/6.34f;put(entries+0x44,after);
 put(child+kActActiveTime,Rd<float>(reinterpret_cast<void*>(child),kActActiveTime,0.f)+delta);
 ordinaryWorldSeconds+=dt;
 if(Rd<unsigned char>(reinterpret_cast<void*>(0xC000),0x39,0)&1u){
   const float remaining=Rd<float>(reinterpret_cast<void*>(0xC000),0x1C,0.f);
   if(delta>=remaining){++proceduralFx;put(0xC039,static_cast<unsigned char>(0));put(0xC028,std::uintptr_t(0xD200));put(0xD200,g_base.load()+0x3BB3EE8);}
   else put(0xC01C,remaining-delta);
 }
 if(before<g_semi_resource46.turnD86.boundary&&after>=g_semi_resource46.turnD86.boundary){++fx;fastestEventRate=std::max(fastestEventRate,double(speed));}
 if(before<.420895f&&after>=.420895f){++events;++quantities;live.phase=2;g_semi_resource46.phase=2;}
 if(before<.866421f&&after>=.866421f){++detachEvents;SemiResourceDetachedD86(reinterpret_cast<void*>(ctrl),live);live.phase=3;live.working=false;}
 now+=unsigned(dt*1000.f);
}
int main(){
 using namespace kcd2vr::subsystems::alchemy::resource_turn_d86;
 gestureTests();intervalTests();
 check(rate(8,.01,.01,.2,6.34,.42,2.f,false)==0,"no current credit grants no fast turn");
 check(rate(8,.09,.09,.2,6.34,.42,2.f,true)==0,"long frame cannot gain accelerated step");
 check(rate(8,.01,0,.2,6.34,.42,2.f,true)==0,"paused native controller cannot gain accelerated step");
 check(rate(8,.01,.02,.2,6.34,.42,2.f,true)==4,"controller scale included in80ms bound");
 check(rate(8,.01,.01,.41,6.34,.42,2.f,true)==0,"80ms before-event guard excludes fast rate");
 setup();input();tick();check(nativeSpeed()>7.9,"captured Attach-to-Use delay uses fast Henry-only clock");native();
 input(true);tick();check(nativeSpeed()==0,"stopping gesture pauses immediately despite earlier credit");
 const auto held=Rd<float>(reinterpret_cast<void*>(entries),0x44,0.f);native();input(true);tick();native();
 check(Rd<float>(reinterpret_cast<void*>(entries),0x44,0.f)==held&&events==0,"stopped gesture cannot advance or use");
 input();tick();check(nativeSpeed()>7.9,"fresh current gesture resumes pre-event turn");native();
 for(int n=0;n<150&&events==0;++n){input();tick();const float before=Rd<float>(reinterpret_cast<void*>(entries),0x44,0.f),speed=nativeSpeed();native();
   if(speed>2.5f)check((g_semi_resource46.turnD86.boundary-Rd<float>(reinterpret_cast<void*>(entries),0x44,0.f))*6.34f>=.07999f,"fast update cannot enter event guard");
   check(Rd<float>(reinterpret_cast<void*>(entries),0x44,0.f)>=before,"never seek or rewind timeline");}
 check(events==1&&quantities==1&&fx==1,"original native work/FX occurs once after gesture crosses guard");
 check(fastestEventRate<=2.5&&ordinaryWorldSeconds<.7,"native event crosses on ordinary gesture credit; Henry wait removed");
 for(int n=0;n<20;++n){input(true);tick();native();}
 check(events==1&&quantities==1&&fx==1&&nativeSpeed()==0,"completed held resource stays paused without repeat effects");
 check(proceduralFx==0,"Use can precede procedural particle start; initial held stop must not force it");
 for(int n=0;n<15&&proceduralFx==0;++n){input();tick();check(nativeSpeed()<=2.5f,"post-Use FX tail never inherits fast slider");native();}
 check(proceduralFx==1&&events==1&&quantities==1,"continued tilt reaches the original procedural FX once without repeating recipe quantity");
 input(true);tick();const auto stoppedTail=Rd<float>(reinterpret_cast<void*>(entries),0x44,0.f);native();
 check(nativeSpeed()==0&&Rd<float>(reinterpret_cast<void*>(entries),0x44,0.f)==stoppedTail,"stop pauses post-Use native timeline immediately");
 for(int n=0;n<400;++n){input();tick();native();}
 check(detachEvents==0&&nativeSpeed()==0,"pouring over pot cannot run native Detach before return home");
 physical.resourceReturnHomeD86=true;
 for(int n=0;n<20&&detachEvents==0;++n){input();tick();native();}
 check(detachEvents==1&&events==1&&quantities==1&&proceduralFx==1,"current home gesture permits original Detach once, no repeated work");
 check(!g_semi_alchemy46.written&&nativeSpeed()==1,"actual original Detach gives back the clock so child retirement cannot hang");
 setup();input();tick();native();physical.manualUse=true;tick();check(!g_semi_alchemy46.written&&nativeSpeed()==1,"explicit native return releases only owned clock at1x");

 setup();input();newerStop=true;tick();check(nativeSpeed()==0,"newer stop arriving before write revokes fast turn");
 setup();input();tick();native();now+=81;tick();check(!g_semi_alchemy46.written&&nativeSpeed()==1,"stale input restores exact saved native1x");
 input();tick();check(!g_semi_alchemy46.written&&nativeSpeed()==1,"late recovery cannot steal retired child");
 setup();input();tick();tracking=false;tick();check(!g_semi_alchemy46.written&&nativeSpeed()==1,"tracking loss restores lease at native speed");
 setup();input();tick();put(child+kActSpeedBias,1.25f);tick();check(!g_semi_alchemy46.written&&nativeSpeed()==1.25f,"foreign clock writer wins compare restore");
 setup();input();tick();const auto writes=writeCount;put(root+kScopeAction,std::uintptr_t(0));parentValid=false;tick();
 check(!g_semi_alchemy46.written&&writeCount==writes,"ended child receives no stale restore write");
 setup();input();put(entries+0x10,55u);tick();check(nativeSpeed()<=2.5f,"different CAF cannot borrow presentation proof");
 setup();input();put(eventList+0x50,.2f);tick();check(nativeSpeed()<=2.5f,"changed boundary closes only acceleration");
 setup();input();put(0xC01C,.07f);tick();check(nativeSpeed()<=2.5f,"actual procedural FX starts sooner than CAF marker: no fast step in80ms guard");
 setup();input();put(0xC039,static_cast<unsigned char>(0));put(0xC028,std::uintptr_t(0xD200));put(0xD200,g_base.load()+0x3BB3EE8);tick();check(nativeSpeed()<=2.5f,"active native particle clip is never accelerated");
 setup();input();put(0xD000,g_base.load()+0x12345);tick();check(nativeSpeed()<=2.5f,"unknown active procedural class is never accelerated");
 setup();input();put(0xC060,.01f);tick();check(nativeSpeed()<=2.5f,"deferred procedural time blocks added acceleration");
 setup();input();put(root+0x40,std::uintptr_t(0xC040));tick();check(nativeSpeed()<=2.5f,"procedural array migration cannot borrow proof");
 setup();input();put(root+0x60,.01f);tick();check(nativeSpeed()<=2.5f,"pending native time forbids boundary acceleration");
 setup();input();physical.sourceEpoch++;tick();check(!g_semi_alchemy46.written&&nativeSpeed()==1,"source epoch replacement restores rather than migrate");
 setup();input();requested=1;tick();check(nativeSpeed()<=2.5f,"slider1 keeps gesture clock");
 setup();input();put(ctrl+kCtrlTimeScale,2.f);tick();check(nativeSpeed()<=4.f&&nativeSpeed()>2.5f,"real adapter includes native scale");
 setup();input();tick(.1f);check(nativeSpeed()==0,"long native update remains paused");
 for(auto kind:{semi46::Mortar,semi46::Bellows,semi46::MortarPlace70,semi46::Book}){setup(kind);input();tick();check(nativeSpeed()<=2.5f,"resource pre-event acceleration cannot affect other work");}
 setup(semi46::Resource,.25f);for(int n=0;n<150&&events==0;++n){input();tick();native();}
 check(fx==1&&events==1&&quantities==1&&fastestEventRate<=2.5,"earlier unrecognised FX event is protected before Use");
 std::printf("alchemy_turn_d86: %u/%u passed\n",checks,checks);
}
'''
for key,value in {'STRUCTS':structs,'SAME':same,'CONSTANTS':constants,'LEASE':lease,'STATE':state,'PENDING':pending,'EXTRACTED':extracted}.items():host=host.replace('__'+key+'__',value)
(out/'production_adapter.inc').write_text(extracted)
(out/'alchemy_turn_d86.cpp').write_text(host)
cmd=['g++','-std=c++20','-O1','-g','-fsanitize=address,undefined','-fno-omit-frame-pointer','-I',str(ROOT/'src'),str(out/'alchemy_turn_d86.cpp'),'-o',str(out/'alchemy_turn_d86')]
r=subprocess.run(cmd,text=True,capture_output=True);(out/'compile.txt').write_text(r.stdout+r.stderr)
if r.returncode:print(r.stderr);raise SystemExit(r.returncode)
env={**os.environ,'ASAN_OPTIONS':'detect_leaks=0'}
r=subprocess.run([str(out/'alchemy_turn_d86')],text=True,capture_output=True,env=env);(out/'run.txt').write_text(r.stdout+r.stderr);print(r.stdout+r.stderr)
result={'passed':r.returncode==0,'returncode':r.returncode,'command':cmd,'production_functions':['SemiAlchemyMembership46','SemiAlchemyRestore46','SemiAlchemyAcquire46','SemiResourceInterval46','SemiResourceMarkersCurrent19','SemiResourceClockBeforeWork19','SemiResourceDirectClockD86','SemiResourceDetachedD86','SemiResourceTurnRateD86','SemiResourceTailRateD86','TickSemiAlchemy46'],'controlled_native_leaves':'current native parent, physical admission, immutable pin validity, CAF-header lookup leaf, root3 verifier, native Update/scanner; no retail execution','native_event_original_call_sites':{name:function(resource,name).count('g_semi_resource_original46(self,character,event);') for name in ['static void __fastcall SemiResourceEventDetour46(','static void SemiResourceObserve19(void* self,void* character,const void* event) {']},'source_sha256':{str(p.relative_to(ROOT)):hashlib.sha256(p.read_bytes()).hexdigest() for p in [ROOT/'src/alchemy_semi_native46.inl',ROOT/'src/alchemy_resource46.inl',ROOT/'src/subsystems/alchemy/resource_turn_d86.h',ROOT/'src/subsystems/alchemy/gesture_work67.h',ROOT/'src/subsystems/alchemy/input67.h',ROOT/'src/vrik/alchemy_semi46.h',ROOT/'src/action_time_owner59c11.cpp']}}
if r.returncode==0:
 if a.baseline:
  baseline=(a.baseline/'src/alchemy_semi_native46.inl').read_text()
  negative=host.replace(function(semi,'static void TickSemiAlchemy46('),function(baseline,'static void TickSemiAlchemy46('))
  (out/'negative_baseline.cpp').write_text(negative)
  negcmd=cmd[:-3]+[str(out/'negative_baseline.cpp'),'-o',str(out/'negative_baseline')]
  nr=subprocess.run(negcmd,text=True,capture_output=True);(out/'negative_compile.txt').write_text(nr.stdout+nr.stderr)
  if nr.returncode:print(nr.stderr);raise SystemExit(nr.returncode)
  nr=subprocess.run([str(out/'negative_baseline')],text=True,capture_output=True,env=env);(out/'negative_run.txt').write_text(nr.stdout+nr.stderr)
  result['negative_baseline_detected']=nr.returncode!=0 and 'captured Attach-to-Use delay' in nr.stderr
  if not result['negative_baseline_detected']:print(nr.stdout+nr.stderr);raise SystemExit('baseline negative control did not fail at the recorded delay')
  print('Baseline actual TickSemiAlchemy46 negative control: detected')
 else:
  result['negative_baseline_detected']=None
  result['baseline_control']='not run: optional --baseline not supplied'
 producer=ROOT/'src/subsystems/alchemy/gesture_work67.h'
 old_condition='if(completed&&!(w.kind==Resource&&w.phase==2&&w.working&&in.resourceTailD86))'
 assert old_condition in producer.read_text()
 negative_header=out/'negative_completed_gesture.h'
 negative_header.write_text(producer.read_text().replace(old_condition,'if(completed)',1))
 negative=host.replace('#include "subsystems/alchemy/gesture_work67.h"','#include "'+negative_header.as_posix()+'"')
 (out/'negative_completed.cpp').write_text(negative)
 nr=subprocess.run(cmd[:-3]+[str(out/'negative_completed.cpp'),'-o',str(out/'negative_completed')],text=True,capture_output=True)
 (out/'negative_completed_compile.txt').write_text(nr.stdout+nr.stderr)
 if nr.returncode:print(nr.stderr);raise SystemExit(nr.returncode)
 nr=subprocess.run([str(out/'negative_completed')],text=True,capture_output=True,env=env)
 (out/'negative_completed_run.txt').write_text(nr.stdout+nr.stderr)
 result['negative_post_use_stop_detected']=nr.returncode!=0 and 'same native Use phase retains tilt credit' in nr.stderr
 if not result['negative_post_use_stop_detected']:print(nr.stdout+nr.stderr);raise SystemExit('post-Use negative control did not fail')
 input_source=ROOT/'tests/alchemy_resource_tail_d86.cpp'
 input_cmd=cmd[:-3]+['-I',str(ROOT/'tests/syntax_shims'),str(input_source),'-o',str(out/'alchemy_resource_tail_d86')]
 ir=subprocess.run(input_cmd,text=True,capture_output=True);(out/'input_compile.txt').write_text(ir.stdout+ir.stderr)
 if ir.returncode:print(ir.stderr);raise SystemExit(ir.returncode)
 ir=subprocess.run([str(out/'alchemy_resource_tail_d86')],text=True,capture_output=True,env=env);(out/'input_run.txt').write_text(ir.stdout+ir.stderr)
 if ir.returncode:print(ir.stdout+ir.stderr);raise SystemExit(ir.returncode)
 result['input_production_adapter']={'passed':True,'command':input_cmd,'output':ir.stdout.strip()}
 print('Prior completed-Use stop negative control: detected')
 print(ir.stdout)

(out/'RESULT.json').write_text(json.dumps(result,indent=2)+'\n')
raise SystemExit(r.returncode)
