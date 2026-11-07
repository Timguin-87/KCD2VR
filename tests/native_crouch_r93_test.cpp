#include "vrik/native_crouch_r93.h"
#include "vrik/tracked_frame51.h"
#include "vrik/fist_motion.h"
#include <map>
#include <cstdio>
#include <cstring>
#include <cwchar>
namespace c=kcd2vr::native_crouch_r93;namespace f=kcd2vr::tracked_frame51;namespace m=kcd2vr::pose_core;
static int tests=0,failed=0;static void ck(bool ok,const char* text){++tests;if(!ok)++failed;std::printf("%s %s\n",ok?"PASS":"FAIL",text);}
static std::map<std::uintptr_t,unsigned char> mem;static int calls=0;static bool value=true,throwCall=false,replaceState=false;
template<class T> static void put(std::uintptr_t p,const T& v){auto b=reinterpret_cast<const unsigned char*>(&v);for(unsigned n=0;n<sizeof(T);++n)mem[p+n]=b[n];}
template<class T> static bool rd(std::uintptr_t p,T& v){auto b=reinterpret_cast<unsigned char*>(&v);for(unsigned n=0;n<sizeof(T);++n){auto at=mem.find(p+n);if(at==mem.end())return false;b[n]=at->second;}return true;}
static c::Identity id(){return {0x180000000,0x10000,0x20000,0x30000,1,2,3,4,1000};}
static void setup(){mem.clear();calls=0;value=true;throwCall=replaceState=false;auto i=id();
 put(i.base+c::getterRva,c::getterBytes);put(i.base+0x3AA4B98+0x758,i.base+c::getterRva);
 put(i.player,i.base+0x3AA4B98);put(i.player+0x38,i.entity);put(i.player+0x30,std::uint32_t(47));put(i.entity,i.base+0x4072C78);put(i.entity+0x0c,std::uint32_t(47));
 put(i.player+0x9d0,std::uintptr_t(0x40000));put(0x40000,i.base+0x3EC9958);put(0x40038,i.player);
 put(i.player+0x990,std::uintptr_t(0x50000));put(0x50000,std::uintptr_t(0x60000));put(0x60010,std::uintptr_t(0x70000));}
static bool call(std::uintptr_t address,std::uintptr_t player,bool& out){++calls;ck(address==id().base+c::getterRva&&player==id().player,"exact verified getter and current player only");if(replaceState)put(player+0x990,std::uintptr_t(0x50080));out=value;return !throwCall;}
static f::Input input(){f::Input i{};i.stamp={1,1,1000,0x30000};i.control=3;i.timeNs=1000000000;i.allowed=i.eyeNeutral=i.smooth=i.hmdHeight=i.nativeStanceR93=true;i.scale=1;i.restHead=i.calibratedHead=i.nativeHead={0,0,1.7};i.restPelvis=i.nativePelvis={0,0,.9};return i;}
int main(){const auto read=[](std::uintptr_t p,auto& v){return rd(p,v);};bool result=false;
 setup();ck(c::query(id(),true,read,call,result)&&result&&calls==1,"accepted native crouch read");value=false;ck(c::query(id(),true,read,call,result)&&!result,"accepted native stand read");
 setup();ck(!c::query(id(),false,read,call,result)&&calls==0,"wrong retail fingerprint never calls native");
 setup();mem[id().base+c::getterRva]^=1;ck(!c::query(id(),true,read,call,result)&&calls==0,"modified getter rejected before call");
 setup();put(id().base+0x3AA4B98+0x758,std::uintptr_t(17));ck(!c::query(id(),true,read,call,result)&&calls==0,"modified native virtual mapping rejected");
 setup();put(id().player+0x38,std::uintptr_t(42));ck(!c::query(id(),true,read,call,result)&&calls==0,"foreign entity rejected");
 setup();put(0x40038,std::uintptr_t(42));ck(!c::query(id(),true,read,call,result)&&calls==0,"foreign manager owner rejected");
 setup();replaceState=true;ck(!c::query(id(),true,read,call,result)&&!result,"state replacement during query withdraws result");
 setup();throwCall=true;ck(!c::query(id(),true,read,call,result)&&!result,"faulting native leaf withdraws result");
 c::Receipt r{id(),5,true};auto now=id();now.tick+=80;ck(c::current(r,now,5),"80ms receipt valid");++now.tick;ck(!c::current(r,now,5),"81ms receipt rejected");now=id();--now.tick;ck(!c::current(r,now,5),"future receipt rejected");
 for(int field=0;field<8;++field){now=id();switch(field){case 0:++now.player;break;case 1:++now.entity;break;case 2:++now.character;break;case 3:++now.body;break;case 4:++now.tracking;break;case 5:++now.control;break;case 6:++now.lifetime;break;case 7:++now.base;break;}ck(!c::current(r,now,5),"replacement receipt owner rejected");}
 ck(!c::current(r,id(),6),"explicit invalidation revokes unchanged packet");
 struct Menu {bool native_crouch_r93=false,hmd_body_height54=false;} menu;
 ck(std::wcscmp(c::label(menu),L"NATIVE (LEGACY HEIGHT)")==0,"legacy loaded height is honestly labeled");c::select(menu);ck(menu.native_crouch_r93&&menu.hmd_body_height54,"native choice preserves HMD walking");c::select(menu);ck(!menu.native_crouch_r93&&menu.hmd_body_height54,"physical choice preserves HMD walking");
 for(bool bow:{false,true})for(bool smooth:{false,true}){
  auto i=input();i.smooth=smooth;i.bow=bow;i.drawing=bow;i.bowKey=1;i.bowGeneration=2;i.bowRequest=3;f::State state{};f::Frame a{};
  ck(f::advance(state,i,a),"standing foundation qualified");const auto standing=a.anchor;
  i.nativeCrouchedR93=true; ck(f::advance(state,i,a)&&a.nativeCrouchOffsetR93==0,"same XR source cannot move body ahead of cached weapon");
  ++i.stamp.sample;i.stamp.tick+=10;i.timeNs+=10000000;
  for(double lowering:{0.,.25,.5}){i.nativePelvis.z=.9-lowering;ck(f::advance(state,i,a)&&std::fabs(a.anchor.z-1.2)<1e-8,"native crouch immediate during native-pelvis transition");ck(std::fabs(a.bodyOffset.z-(-.5+lowering))<1e-8,"settled native pelvis is not lowered twice");ck(std::fabs(state.filtered.z-standing.z)<1e-8,"stance never enters native filter or bow history");}
  i.nativeCrouchedR93=false;++i.stamp.sample;i.stamp.tick+=10;i.timeNs+=10000000;for(double lowering:{.5,.25,0.}){i.nativePelvis.z=.9-lowering;ck(f::advance(state,i,a)&&std::fabs(a.anchor.z-standing.z)<1e-8,"native stand immediate during native-pelvis transition");}
 }
 {auto i=input();i.nativeStanceR93=false;f::State state{};f::Frame a{};f::advance(state,i,a);i.nativeStanceR93=i.nativeCrouchedR93=true;
  ck(f::advance(state,i,a)&&!a.nativeStanceKnownR93&&a.nativeCrouchOffsetR93==0,"new positive receipt on duplicate source waits for coherent next source");
  ++i.stamp.sample;i.stamp.tick+=10;i.timeNs+=10000000;ck(f::advance(state,i,a)&&a.nativeStanceKnownR93&&a.nativeCrouchOffsetR93==-.5,"next fresh source admits first accepted native crouch");
  i.nativeStanceR93=false;ck(f::advance(state,i,a)&&!a.nativeStanceKnownR93&&a.nativeCrouchOffsetR93==0,"lost native receipt never borrows duplicate retention");
 }
 {auto i=input();f::State state{};f::Frame a{};i.nativeCrouchedR93=true;i.nativePelvis.z=-.051;ck(!f::advance(state,i,a),"residual over45cm beyond authored50cm still rejected");}
 {auto i=input();i.nativePelvis.z=.4;f::State state{};f::Frame a{};ck(!f::advance(state,i,a),"unrelated standing predicate never widens the45cm guard");}
 {auto i=input();i.nativeCrouchedR93=true;i.nativePelvis.z=.4;f::State state{};f::Frame a{};ck(f::advance(state,i,a),"qualified crouch starts release test");i.nativeCrouchedR93=false;++i.stamp.sample;i.stamp.tick+=10;i.timeNs+=10000000;ck(f::advance(state,i,a),"same-owner stand edge retains native return allowance");i.nativePelvis.z=.6;ck(f::advance(state,i,a)&&!state.nativeStandReleaseR93,"return allowance retires when original guard admits");i.nativePelvis.z=.4;ck(!f::advance(state,i,a),"later unrelated pelvis slump cannot reuse retired stance allowance");}
 for(auto domain:{f::ReferenceDomainD86::Dice,f::ReferenceDomainD86::Mounted,f::ReferenceDomainD86::Workstation,f::ReferenceDomainD86::Other}){auto i=input();i.referenceDomainD86=domain;i.nativeCrouchedR93=true;f::State state{};f::Frame a{};ck(f::advance(state,i,a)&&a.nativeCrouchOffsetR93==0,"protected activity cannot consume ordinary crouch receipt");}
 {auto i=input();i.hmdHeight=false;i.nativeCrouchedR93=true;i.nativePelvis.z=.6;f::State state{};f::Frame a{};ck(f::advance(state,i,a)&&std::fabs(a.anchor.z-1.4)<1e-8&&a.nativeCrouchOffsetR93==0,"legacy native-pelvis height remains unchanged");}
 {auto i=input();i.nativeCrouchedR93=true;f::State state{};f::Frame a{};ck(f::advance(state,i,a)&&f::ordinary_reference_d86(a),"crouched ordinary frame still seeds exact native pickup");i.acquisitionSerial=2;i.acquisitionAction=3;i.acquisitionScope=4;i.acquisitionAnchor=a.anchor;i.acquisitionCrouchOffsetR93=a.nativeCrouchOffsetR93;i.nativeStanceR93=false;i.nativePelvis.z=.4;ck(f::advance(state,i,a)&&std::fabs(a.anchor.z-1.2)<1e-8,"exact pickup keeps pre-queue crouch foundation without querying native state");}
 {auto i=input();i.scale=2;i.nativeCrouchedR93=true;f::State state{};f::Frame a{};ck(f::advance(state,i,a)&&std::fabs(a.nativeCrouchOffsetR93*i.scale+.5)<1e-8,"authored half-meter height is world-scale independent");}
 {namespace fs=kcd2vr::fists;struct {fs::Sample sample{};} f;auto& in=f.sample;in.epoch=in.sequence=1;in.tick=1000;in.actor=3;in.valid=in.empty[0]=in.empty[1]=true;in.head={0,1.7,0};in.controller[0]={.2,1.3,-.2};in.controller[1]={-.2,1.3,-.2};
  struct {struct {double t[3];} allowed[2];double nativeCrouchOffsetR93=0;} packet{};
  auto publish=[&](){for(int i=0;i<2;++i){packet.allowed[i].t[0]=in.controller[i].x;packet.allowed[i].t[1]=in.controller[i].y;packet.allowed[i].t[2]=in.controller[i].z+packet.nativeCrouchOffsetR93;
#include "generated_fist_allowed_r93.inc"
  }};
  fs::State state{};publish();fs::step(state,in);in.grip[0]=in.trigger[0]=1;bool punch=false;
  for(int n=0;n<20;++n){++in.sequence;in.tick+=10;publish();punch|=fs::step(state,in).valid;}
  ck(state.hand[0].phase==fs::Phase::Ready&&state.hand[0].released,"fist is actually ready before native crouch");
  packet.nativeCrouchOffsetR93=-.5;++in.sequence;in.tick+=10;publish();fs::Trace trace{};punch|=fs::step(state,in,&trace).valid;
  ck(!punch&&state.hand[0].phase==fs::Phase::Ready&&state.hand[0].released&&!(trace.hand[0].reject&fs::Jump),"actual producer removes only stance and preserves ready fist");
  int strikes=0;for(int n=0;n<12;++n){in.controller[0].z-=.018;++in.sequence;in.tick+=10;publish();strikes+=fs::step(state,in).valid;}
  ck(strikes==1,"first deliberate punch after crouch needs no release or new settle");
  in.allowed[0].x+=.30;++in.sequence;in.tick+=10;fs::step(state,in,&trace);ck((trace.hand[0].reject&fs::Jump)!=0,"real collision or tracking endpoint jump remains rejected");
 }

 std::printf("native_crouch_r93=%d/%d; native leaf synthetic; no headset\n",tests-failed,tests);return failed?1:0;
}
