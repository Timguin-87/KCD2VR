#if defined(KCD2VR_H40_PARTIAL_REST_LUA51)
// Actual C++ hand admission/service and Lua5.1 original-script composition.
// Native engine leaves are controlled; no injection/headset acceptance.
#include "../src/subsystems/doors/hand_only_h40.h"
extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

namespace h=kcd2vr::hand_only_door_h40;
namespace m=kcd2vr::mutual_admission_t25;
static unsigned checks=0,failures=0;
static void check(bool ok,const char*name){++checks;if(!ok){++failures;std::printf("FAIL %s\n",name);}}
static bool current=true;
static bool reader(const h::Frame&,int,std::uint64_t)noexcept{return current;}
static unsigned restCount(){
#ifdef H40_PARENT
 return 0;
#else
 return h::resting.load();
#endif
}
static void resetRest(){
#ifndef H40_PARENT
 h::resting=0;
#endif
}
static bool isRestProtocol(const h::Ack&a){
#ifdef H40_PARENT
 return false;
#else
 return a.restProtocol;
#endif
}
static unsigned ackRestCount(const h::Ack&a){
#ifdef H40_PARENT
 return 0;
#else
 return a.resting;
#endif
}
struct Rig {
 lua_State*L=nullptr;h::Frame f{};std::uint64_t now=1000;std::string action,last;unsigned replyMode=0;
 explicit Rig(const char*path){
  h::claims[0]=h::claims[1]=0;h::entered[0]=h::entered[1]=0;resetRest();h::admissionBusy=0;
  m::active[0]=m::active[1]=0;m::doorClaimsH41[0]=m::doorClaimsH41[1]=0;
  h::neutral[0]=h::neutral[1]=true;h::pendingInput[0]=h::pendingInput[1]={};h::fallbackToken[0]=h::fallbackToken[1]=0;
  h::nextClaim=1;h::ready=false;h::enabled=true;h::transportInstalled=true;h::nearTick=0;h::vmQuarantine=false;
  h::service={};h::inputIdentityReader=nullptr;h::currentReader=&reader;current=true;h::displayEnabled=false;
  f.tick=now;f.body=20;f.request=30;f.sample=10;f.tracking=1;f.policy=2;
  f.actor=100;f.character=200;f.nativePlayer=300;f.manager=400;f.player=10;f.permitted=true;
  f.available[0]=f.available[1]=true;for(int s=0;s<2;++s){f.point[s][0]=.5f;f.point[s][2]=1;}
  L=luaL_newstate();luaL_openlibs(L);if(luaL_dofile(L,path))error();
  h::PublishCompleted(f);
 }
 ~Rig(){lua_close(L);}
 [[noreturn]]void error(){std::fprintf(stderr,"LUA %s\n",lua_tostring(L,-1));std::exit(8);}
 void exec(const char*s){if(luaL_dostring(L,s))error();}
 double value(const char*s){std::string code="return ";code+=s;if(luaL_loadstring(L,code.c_str())||lua_pcall(L,0,1,0))error();double v=lua_tonumber(L,-1);lua_pop(L,1);return v;}
 bool call(const char*in,char*out,std::size_t cap)noexcept{
  last=in;const char*b=std::strchr(in,'|');const char*e=b?std::strchr(b+1,'|'):nullptr;action=b&&e?std::string(b+1,e):"";
  lua_getglobal(L,h::kCallback);lua_pushstring(L,in);
  if(lua_pcall(L,1,1,0)){std::fprintf(stderr,"CALL_ERROR %s\n",lua_tostring(L,-1));lua_pop(L,1);return false;}
  size_t n=0;const char*r=lua_tolstring(L,-1,&n);if(!r||n>=cap){lua_pop(L,1);return false;}
  std::string reply(r,n);lua_pop(L,1);
  if(replyMode==1&&reply.rfind("H40R1|",0)==0){reply="OK|"+reply.substr(6);reply.resize(reply.rfind('|'));}
  if(replyMode==2)return false; // original Lua call already returned normally
  if(reply.size()>=cap)return false;std::memcpy(out,reply.data(),reply.size());out[reply.size()]=0;return true;
 }
 void step(int side,float grip,float x=.5f,float y=0){
  now+=10;f.tick=now;++f.sample;f.grip[side]=grip;f.point[side][0]=x;f.point[side][1]=y;
  // Both clocks advance independently through their real source consumers.
  std::string clock="now="+std::to_string(double(now)/1000.0);exec(clock.c_str());h::PublishCompleted(f);
  (void)h::CandidatePress(side,101,now,true,grip,0);h::service.tick(*this,now,1);
 }
 void body(const char*door="d",double dt=.001){std::string s="frame(";s+=door;s+=","+std::to_string(dt)+")";exec(s.c_str());}
 void start(int side,bool closing=false,bool nativeJoin=false){
  exec("d=makeDoor(101)");if(closing)exec("d.nDirection=1");if(nativeJoin)exec("AnimDoor.OnUsed(d,player,0)");
  step(side,.9f);body();check(h::Reserved()==(1u<<side)&&value("d.rate")==0,"actual original Grip acquisition and pause");
 }
 void partial(int side,bool closing=false){
  const float sign=closing?1.f:-1.f;
  for(int i=1;i<=30;++i){float a=sign*.35f*float(i)/30.f;step(side,.9f,.5f*std::cos(a),.5f*std::sin(a));body();}
  // H44: these are intentional slow/rest tests. Supply an actual stationary
  // controller sample before release; ordinary moving release now continues.
  step(side,.9f,f.point[side][0],f.point[side][1]);body();
  check(value("d.time")>0&&value("d.time")<.9995,"actual nonterminal phase selected through production curve");
 }
 void park(int side){
  const auto t=value("d.time");step(side,0,f.point[side][0],f.point[side][1]);
  check(action=="feed"&&h::Reserved()==(1u<<side)&&value("d.rate")==0,"physical zero Grip sent distinctly from invalid clear; no early detach");
  body();step(side,0,f.point[side][0],f.point[side][1]);
  check(h::Reserved()==0&&h::entered[side]==0&&restCount()==1&&!m::doorClaimsH41[side],"actual versioned REST reply frees only the hand and shared admission");
  check(h::service.identity&&h::Wanted(now)&&!std::strcmp(h::Name(h::service.stage),"RELEASED_PARTIAL_REST"),"original native debt remains service-owned and wanted");
  check(value("d.time")==t&&value("d.rate")==0&&value("d.inUse")==1&&value("doorReports")==0,"rest is real paused native operation, not endpoint/report");
 }
 void finish(){
  f.permitted=false;
  for(int n=0;n<125;++n){step(0,0);body("d",.02);if(!h::Reserved()&&!restCount())break;}
  check(!h::Reserved()&&!restCount()&&!h::service.identity&&!h::service.fault,"actual native endpoint retires all hand/rest debt");
  check(value("d.inUse")==0&&value("d.bNeedUpdate")==0&&value("doorReports")==1&&value("manipulationStarts")==1,"same original native report/cost once");
 }
};
int main(int argc,char**argv){
 if(argc!=2)return 4;
 for(int side=0;side<2;++side)for(bool closing:{false,true}){
  Rig r(argv[1]);r.start(side,closing);r.partial(side,closing);r.park(side);
  const auto t=r.value("d.time");
  for(int n=0;n<100;++n){r.step(side,0);r.body();}
  check(r.value("d.time")==t&&r.value("d.rate")==0,"slow release remains partial over continuing valid source, no new expiry");
  auto utility=m::TryBegin(side,m::Kind::Throw);check(m::valid(utility),"rest cannot reserve an unrelated utility hand");
  check(m::Finish(utility,[](void*)noexcept{},nullptr),"real transient utility lifecycle unchanged");
  const auto starts=r.value("objectStarts"),cost=r.value("manipulationStarts"),crime=r.value("crimeSounds");
  const int next=1-side;r.step(next,.9f,.35f,.1f);r.body();
  check(h::Reserved()==(1u<<next)&&restCount()==0&&r.value("d.time")==t,"opposite fresh Grip reacquires same actual phase without jump");
  check(r.value("objectStarts")==starts&&r.value("manipulationStarts")==cost&&r.value("crimeSounds")==crime,"reacquisition never replays OnUsed/play/cost/crime");
  r.step(next,.9f,.4f,.05f);r.body();check(r.value("d.time")!=t,"new joined palm displacement drives same original clip");
  r.finish();check(r.value("physicalCalls")==0&&r.value("nativeActorFlags")==0,"object-only rest never acquires or releases Henry native flags");
 }
 // H44: normal moving release hands back the same original clip, not REST.
 for(int side=0;side<2;++side)for(bool closing:{false,true}){
  Rig r(argv[1]);r.start(side,closing);r.partial(side,closing);
  const float a=closing?.55f:-.55f,x=.5f*std::cos(a),y=.5f*std::sin(a);
  r.step(side,.9f,x,y);r.body();const auto t=r.value("d.time");
  r.step(side,0,x,y);r.body();r.step(side,0,x,y);
  check(!restCount()&&h::Reserved()==(1u<<side)&&r.value("d.rate")==1&&r.value("d.time")==t,"normal motion release preserves clip phase and uses original native finish");
  check(r.value("objectStarts")==1&&r.value("manipulationStarts")==1&&r.value("nativeActions")==0,"moving release never invokes a second native use/play/action/start");
  r.finish();
 }
 // H44: reacquire a stopped leaf at its moved prior grab anchor, beyond bind bbox.
 {Rig r(argv[1]);r.start(0);r.partial(0);const float a=-1.2f,x=.5f*std::cos(a),y=.5f*std::sin(a);
  r.step(0,.9f,x,y);r.body();r.step(0,.9f,x,y);r.body();r.park(0);const auto t=r.value("d.time");
  r.step(1,.9f,x,y);r.body();
  check(h::Reserved()==2&&!restCount()&&r.value("d.time")==t&&r.value("objectStarts")==1,"actual C++ claim reacquires moved native rest anchor with no phase jump/restart");r.finish();}
 // Native-player episodes own Henry flags and therefore use original handback.
 {Rig r(argv[1]);r.start(0,false,true);r.partial(0);r.step(0,0);r.body();check(h::Reserved()==1&&!restCount()&&r.value("d.rate")==1,"native-A joined episode resumes rather than parking Henry flags");r.finish();check(r.value("physicalCalls")==2&&r.value("nativeActorFlags")==0,"matching native joined flag cleanup exactly once");}
 // Menu/context withdrawal is native handback, not a physical slow-release rest.
 {Rig r(argv[1]);r.start(0);r.partial(0);r.park(0);r.f.permitted=false;r.step(0,0);r.body();check(r.action=="clear"&&r.value("d.rate")==1&&restCount()==1,"menu/context loss resumes same original while retaining debt until endpoint");r.finish();}
 // VM replacement cannot forgive native REST debt merely because hands are free.
 {Rig r(argv[1]);r.start(0);r.partial(0);r.park(0);auto serial=h::service.serial;h::service.tick(r,r.now,2);check(h::vmQuarantine&&restCount()==1&&h::service.serial==serial,"actual replacement VM retains original rest obligation without foreign ACK");}
 // Slow analogue release crosses the existing .30 release boundary before 0.
 {Rig r(argv[1]);r.start(0);r.partial(0);const auto t=r.value("d.time");
  for(float g:{.55f,.45f,.35f,.30f}){r.step(0,g,r.f.point[0][0],r.f.point[0][1]);r.body();check(h::Reserved()==1&&r.value("d.time")==t,"decreasing Grip remains held through original .30 boundary");}
  r.step(0,.29f,r.f.point[0][0],r.f.point[0][1]);r.body();r.step(0,.29f);check(!h::Reserved()&&restCount()==1&&r.value("d.time")==t,"first genuine sub-.30 physical release rests rather than clearing/finishing");r.finish();}
 // A new close grip before the original update cancels only pending detachment.
 {Rig r(argv[1]);r.start(0);r.partial(0);const auto t=r.value("d.time");r.step(0,0,r.f.point[0][0],r.f.point[0][1]);r.step(0,.9f,r.f.point[0][0],r.f.point[0][1]);r.body();
  check(h::Reserved()==1&&!restCount()&&r.value("d.time")==t&&r.value("objectStarts")==1,"release/regrip before pause observer retains original hand/operation without false REST ACK");r.finish();}
 // Failed pause observation cannot detach an uncertain original operation.
 {Rig r(argv[1]);r.start(0);r.partial(0);r.exec("d.GetAnimationSpeed=function()return nil end");r.step(0,0);r.body();r.step(0,0);
  check(h::Reserved()==1&&!restCount()&&m::doorClaimsH41[0]&&r.value("doorReports")==0,"unknown native pause read retains hand debt and emits no fabricated terminal");}
 // Native leaves may reenter. A replaced scope cannot detach a stale read.
 {Rig r(argv[1]);r.start(0);r.partial(0);r.exec("d.GetAnimationSpeed=function(self)self.curAnim='foreign';return self.rate end");r.step(0,0);r.body();r.step(0,0);
  check(h::Reserved()==1&&!restCount()&&m::doorClaimsH41[0],"clip replacement inside pause getter cannot publish free-hand REST");
  check(r.value("doorReports")==0&&r.value("objectStarts")==1,"reentrant pause uncertainty neither fabricates endpoint nor retries original Use");}
 {Rig r(argv[1]);r.start(0);r.partial(0);r.park(0);r.exec("local original=d.GetBonePos;d.GetBonePos=function(self,name)local p=original(self,name);self.curAnim='foreign';return p end");r.step(1,.9f,.3f,.1f);
  check(!h::Reserved()&&restCount()==1&&r.value("objectStarts")==1,"bone-read replacement cannot reacquire a different native operation");}
 // Replacement is not adoption; successful missing entity can retire its record.
 {Rig r(argv[1]);r.start(0);r.partial(0);r.park(0);r.exec("d.curAnim='foreign'");r.body();r.step(0,0);
  check(restCount()==1&&!h::Reserved()&&r.value("d.rate")==0&&r.value("doorReports")==0,"foreign clip replacement cannot be resumed/adopted or forge endpoint");
  auto t=m::TryBegin(1,m::Kind::Reload);check(m::valid(t),"uncertain detached operation does not reserve unrelated hand");if(m::valid(t))m::Finish(t,[](void*)noexcept{},nullptr);
  r.exec("entities={}");r.body();r.step(0,0);check(!restCount()&&r.value("doorReports")==0,"actual missing door retires original record without invented native report");}
 // Existing short feed-loss handback is preserved; no new long pause timeout.
 {Rig r(argv[1]);r.start(0);r.partial(0);r.park(0);r.exec("now=now+.081;local f=table.remove(timers,1);assert(f);f()");r.body();
  check(r.value("d.rate")==1&&r.value("doorReports")==0,"existing 80ms transport feed loss resumes exact original operation");r.now+=100;r.finish();}
 // Bounded two-rest capacity; exhausting it uses existing native handback.
 {Rig r(argv[1]);r.start(0);r.partial(0);r.park(0);r.exec("first=d;d=makeDoor(102);entities={d}");
  r.step(0,.9f);r.body();r.partial(0);r.step(0,0);r.body();r.step(0,0);check(restCount()==2&&!h::Reserved(),"two native rest slots are independent from hand slots");
  r.exec("second=d;d=makeDoor(103);entities={d}");r.step(1,.9f);r.body();r.partial(1);r.step(1,0);r.body();r.step(1,0);
  check(restCount()==2&&h::Reserved()==2&&r.value("d.rate")==1&&r.value("first.rate")==0&&r.value("second.rate")==0,"capacity exhaustion hands back only the third original clip; never evicts/rest-forgives");
  r.exec("third=d;d=makeDoor(104);entities={d}");r.step(0,.9f);r.body();check(h::Reserved()==3&&restCount()==2&&r.value("objectStarts")==4,"full rest capacity never blocks an unrelated fresh other-hand Grip");}
 // Owner-wide release still waits for real native terminal and one report.
 {Rig r(argv[1]);r.start(1,true);r.partial(1,true);r.park(1);check(r.value("KCD2VRHandDoorsH40.ReleaseAll() and 1 or 0")==0,"ReleaseAll retains paused native obligation until actual endpoint");r.body();check(r.value("d.rate")==1&&r.value("doorReports")==0,"ReleaseAll resumes same selected closing clip, not opposite operation");r.finish();}
 // Lost transport cannot forgive a detached native operation or its hand ACK.
 {Rig r(argv[1]);r.start(0);r.partial(0);r.step(0,0);r.body();r.replyMode=2;r.step(0,0);
  check(h::Reserved()==1&&m::doorClaimsH41[0]&&h::service.fault,"lost REST reply retains original uncertain hand debt");
  r.replyMode=0;r.step(0,0);r.body();check(!h::Reserved()&&restCount()==1&&r.value("d.rate")==1,"later real ACK separates free hand from same native cleanup debt");r.finish();}
 // A version downgrade can never masquerade as a zero-rest terminal receipt.
 {Rig r(argv[1]);r.start(0);r.partial(0);r.park(0);r.replyMode=1;r.step(0,0);
  check(restCount()==1&&h::service.fault&&h::service.identity,"legacy ACK cannot forgive an already versioned native rest");r.replyMode=0;r.finish();}
 {Rig r(argv[1]);r.start(0);r.partial(0);r.step(0,0);r.body();r.replyMode=1;r.step(0,0);
  check(h::Reserved()==1&&h::service.identity&&h::service.fault&&r.value("KCD2VRHandDoorsH40.restsH43[1] and 1 or 0")==1,"FIRST detach cannot downgrade negotiated protocol and lose paused native debt");
  r.replyMode=0;r.finish();}
 // A stale completed frame cannot indefinitely renew the original rest owner.
 {Rig r(argv[1]);r.start(1);r.partial(1);r.park(1);r.now+=81;h::service.tick(r,r.now,1);r.body();
  check(r.action=="clear"&&r.value("d.rate")==1&&restCount()==1,"existing 80ms completed-frame expiry hands back native operation");r.finish();}
 // At an achieved held endpoint there is no partial operation to retain.
 {Rig r(argv[1]);r.start(0);r.step(0,.9f,0,-.5f);r.body();check(r.value("d.time")>=.99949,"actual production nonlinear curve reaches held clamp endpoint");r.step(0,0);r.body();
  check(!restCount()&&h::Reserved()==1&&r.value("d.rate")==1,"actual held endpoint uses existing original native finish, no partial rest");r.finish();}
 // Malformed/legacy packets remain fail-closed; old normal no-rest pairing works.
 {h::Ack a{};check(h::Parse("OK|1|10|20|30|0|0|1|0|0",a)&&!isRestProtocol(a)&&!ackRestCount(a),"legacy terminal ACK retains no-rest compatibility");
  check(h::Parse("H40R1|1|10|20|30|0|0|1|0|0|2",a)&&isRestProtocol(a)&&ackRestCount(a)==2,"bounded rest ACK parsed independently from hand mask");
  for(const char*s:{"H40R1|1|10|20|30|0|0|1|0|0|3","H40R1|1|10|20|30|0|0|1|0|0|-1","H40R1|1|10|20|30|0|0|1|0|0|1|0","H40R1|1|10|20|30|0|0|1|1|0|1","H40R1|1|10|20|30|0|1|1|0|0|1"})check(!h::Parse(s,a),"malformed rest ACK cannot release ownership");}
 std::printf("PARTIAL_REST %u/%u PASS\n",checks-failures,checks);return failures?1:0;
}

#else
#define H36_MAIN_NAME H36PreservedMainH40
#include "holster_completion_h36_bridge.cpp"
#include "../src/vrik/physical_interaction.h"
#include "../src/vrik/held_interaction114.h"
#include <string>
#include <vector>
#include <sstream>
#include <thread>
namespace h40=kcd2vr::hand_only_door_h40;
using namespace kcd2vr;
static bool currentH40=true;
static bool HostCurrentH40(const h40::Frame&,int,std::uint64_t) noexcept {return currentH40;}
static h40::Frame frameH40(){h40::Frame f{};f.tick=1000;f.body=9;f.request=10;f.sample=20;f.tracking=1;f.policy=2;
 f.actor=0x5000;f.character=0x3000;f.nativePlayer=0x2000;f.manager=0x4000;f.player=42;f.permitted=true;
 f.available[0]=f.available[1]=true;f.grip[0]=f.grip[1]=.8f;f.point[0][0]=1;f.point[1][0]=-1;return f;}
static void resetH40(){h40::claims[0]=h40::claims[1]=0;h40::entered[0]=h40::entered[1]=0;h40::admissionBusy=0;
 kcd2vr::mutual_admission_t25::doorClaimsH41[0]=kcd2vr::mutual_admission_t25::doorClaimsH41[1]=0;
 kcd2vr::mutual_admission_t25::active[0]=kcd2vr::mutual_admission_t25::active[1]=0;
 h40::neutral[0]=h40::neutral[1]=true;h40::pendingInput[0]=h40::pendingInput[1]={};h40::nativeFallback[0]=h40::nativeFallback[1]={};h40::inputIdentityReader=nullptr;h40::fallbackToken[0]=h40::fallbackToken[1]=0;h40::ready=false;h40::enabled=true;h40::transportInstalled=true;
 h40::nearTick=0;h40::nextClaim=1;h40::vmQuarantine=false;h40::service={};currentH40=true;h40::currentReader=&HostCurrentH40;h40::PublishCompleted(frameH40());
 kcd2vr::door_use46::enabled=true;diagnostics::lab::state=0;}
struct IOH40 {
 enum Result {NoDoor,Hold,Retire,Failure,Bad,WrongSerial,UnexpectedSide} result=Hold;
 unsigned calls=0,feeds=0,clears=0,probes=0;std::uint64_t lastBody=0,lastRequest=0,lastTick=0,lastToken[2]{};std::string action;
 bool call(const char*in,char*out,std::size_t cap) noexcept {
  ++calls;std::vector<std::string> v;std::stringstream ss(in);std::string p;while(std::getline(ss,p,'|'))v.push_back(p);
  if(v.size()!=16&&v.size()!=18)return false;auto n=[&](int i){return std::stoull(v[i]);};auto serial=n(0);action=v[1];lastBody=n(3);lastRequest=n(4);lastTick=n(5);lastToken[0]=n(14);lastToken[1]=n(15);
  if(action=="probe")++probes;else if(action=="feed")++feeds;else ++clears;
  if(result==Failure)return false;if(result==Bad){std::snprintf(out,cap,"OK|truncated");return true;}
  unsigned mask=0,retiring=0;if(action!="probe"&&result!=NoDoor){for(int s=0;s<2;++s)if(lastToken[s])mask|=1u<<s;if(result==Retire)retiring=mask;}
  if(result==UnexpectedSide&&action!="probe")mask=2;if(result==WrongSerial&&action!="probe")++serial;
  std::snprintf(out,cap,"OK|%llu|%llu|%llu|%llu|%u|%u|%llu|%u|%u",serial,n(2),lastBody,lastRequest,mask,retiring,mask?0ull:serial,(mask&1)?77:0,(mask&2)?88:0);return true;
 }
};
struct PadH40 {bool active=true;float grip=0,trigger=0;};
struct ManagerH40 {PadH40 right,left;const PadH40&RightController()const{return right;}const PadH40&LeftController()const{return left;}};
struct InputRigH40 {
 ManagerH40 manager;physical_interaction106::State pulse{};held_interaction114::State held{};interaction_alignment::HandCandidate candidate{};bool blocked=false,empty[2]{true,true};
 std::uint64_t original=0;unsigned nativePulses=0;bool tracked[2]{},ray=false;
 void run(std::uint64_t now){hostTickCount64=now;auto*xr_manager_=&manager;const auto ladder_now_ms=now;const unsigned f5Mode=0;bool alchemyPointer59c2=false;
  physical_interaction106::Input pi{};pi.enabled=true;pi.menu=blocked;pi.now_ms=now;
  pi.hand[0]={true,empty[0],true,manager.right.grip,false};pi.hand[1]={true,empty[1],true,manager.left.grip,false};
  const auto&candidate59c7f=candidate;auto&f5Physical=pulse;auto&heldUse114=held;
  #include "../src/subsystems/doors/hand_only_h40_input.inl"
  // Same actual native-input FSMs and original-stamp restoration as mod caller.
  auto po=physical_interaction106::Step(f5Physical,pi);const auto held114=held_interaction114::update(heldUse114,pi);
  if(held114.owner>=0&&fallbackPressH40[held114.owner])heldUse114.pressed=fallbackPressH40[held114.owner];
  if(po.fire_use)++nativePulses;original=heldUse114.pressed;ray=po.ray_active;tracked[0]=pi.hand[0].tracked;tracked[1]=pi.hand[1].tracked;
 }
};
static void stepH40(IOH40&io,std::uint64_t now=1000,std::uint64_t vm=1){h40::service.tick(io,now,vm);}
static void publishH40(std::uint64_t now,float r=.8f,float l=.8f){auto f=frameH40();f.tick=now;f.sample+=now-1000;f.grip[0]=r;f.grip[1]=l;h40::PublishCompleted(f);}
static void sourceTestsH40(){
 RigH36 r;r.gear.handKey[0]=r.gear.handKey[1]=0;r.gear.heldIndex[0]=r.gear.heldIndex[1]=-1;r.gear.count=0;
 g_henry_item_attach_manager=r.gear.manager;g_phys_equipment6698.publish(r.gear);g_weapon_control_mode=4;g_phys_armed6698=true;g_phys_fault6698=false;
 g_cal_state=2;g_pose_policy_generation57=2;g_committed_character6689=reinterpret_cast<void*>(0x3000);g_committed_pose_valid6689=true;g_pose_restore_fault6689=false;
 g_last_player_entity=reinterpret_cast<void*>(0x5000);g_ctx_reading=false;g_graspPendingUse55=nullptr;g_doorActive66=nullptr;animation_filter::nativeBodyOwned112=false;
 TrackingSnapshot t{};t.tick=t.head_good_tick=t.left_good_tick=t.right_good_tick=1000;t.sequence=20;t.source_epoch=tracking59c6::epoch.load();
 t.allowed=t.head_valid=t.left_valid=t.right_valid=true;t.right_grip=t.left_grip=.8f;g_tracking_frames6695.publish(t);tracking59c6::enabled=false;
 auto f=frameH40();f.tracking=t.source_epoch;f.manager=reinterpret_cast<std::uintptr_t>(r.gear.manager);h40::currentReader=&HandOnlyDoorCurrentH40;
 ck(HandOnlyDoorCurrentH40(f,0,1000),"actual current-reader joins original body/request/player/manager and raw tracking");
 g_ctx_menu=true;ck(!HandOnlyDoorCurrentH40(f,0,1000),"actual menu immediately revokes same hand");g_ctx_menu=false;
 ++g_weapon_request_epoch6694;ck(!HandOnlyDoorCurrentH40(f,0,1000),"actual changed request cannot restamp old pose");--g_weapon_request_epoch6694;
 t.right_trigger=.9f;g_tracking_frames6695.publish(t);ck(!HandOnlyDoorCurrentH40(f,0,1000)&&HandOnlyDoorCurrentH40(f,1,1000),"actual qualified fist excludes only that hand");t.right_trigger=0;g_tracking_frames6695.publish(t);
 r.gear.handKey[0]=9;g_phys_equipment6698.publish(r.gear);ck(!HandOnlyDoorCurrentH40(f,0,1000)&&HandOnlyDoorCurrentH40(f,1,1000),"actual current occupied hand preserves opposite empty hand");r.gear.handKey[0]=0;g_phys_equipment6698.publish(r.gear);
 quick_talk_owner_h4::mailbox.publish({1000,9,10,0x2000,1});ck(!HandOnlyDoorCurrentH40(f,0,1000)&&HandOnlyDoorCurrentH40(f,1,1000),"actual quicktalk claim excludes only original hand");quick_talk_owner_h4::mailbox.publish({1000,9,10,0x2000,0});
 g_native_held_equipment_pending_h36=true;ck(!HandOnlyDoorCurrentH40(f,1,1000)&&HandOnlyDoorCurrentH40(f,0,1000),"actual retained native equipment debt is not empty-slot completion");g_native_held_equipment_pending_h36=false;
 t.head_held=true;g_tracking_frames6695.publish(t);ck(!HandOnlyDoorCurrentH40(f,0,1000),"actual held HMD never authorizes fresh door motion");t.head_held=false;g_tracking_frames6695.publish(t);
 // Actual completed pose producer, with native getters backed by known fixture memory.
 std::array<unsigned char,0x30> actor{};std::uint32_t id=42;std::memcpy(actor.data()+0x0C,&id,4);g_last_player_entity=actor.data();
 PhysFrame6698 physical{};physical.input.allowed=physical.input.tracked[0]=physical.input.tracked[1]=true;physical.input.sample=20;physical.input.tick=1000;physical.input.epoch=10;physical.input.bodyEpoch=9;
 physical.player=r.gear.player;physical.manager=r.gear.manager;physical.request=10;g_phys_frame6698.publish(physical);g_phys_ready_sample6698=20;
 CoherentTransaction6689 tx;tx.epoch=9;tx.doorRequestH40=10;tx.policyGeneration57=2;tx.committed=tx.restored=true;tx.tracking=t;tx.mapPresentationOwnerM34[0]=tx.mapPresentationOwnerM34[1]=0;
 CommittedFrame6695 p{};p.doorPlayer38=reinterpret_cast<std::uintptr_t>(actor.data());p.stamp.actor=0x3000;p.stamp.epoch=9;p.stamp.sample=20;p.stamp.tick=1000;
 p.world[3]=1;p.world[4]=10;p.world[5]=20;p.world[6]=30;p.world[7]=2;p.valid[0]=p.valid[1]=true;p.palmValid59c9[0]=p.palmValid59c9[1]=true;p.palm59c9[0]={.2f,.3f,.4f};p.palm59c9[1]={-.2f,.1f,.5f};
 PublishHandOnlyDoorH40(tx,p);h40::Frame produced{};ck(h40::completed.read(produced)&&produced.permitted&&produced.available[0]&&produced.available[1],"actual completed pose qualifies empty tracked hands");
 ck(std::fabs(produced.point[0][0]-10.4f)<1e-5&&std::fabs(produced.point[1][0]-9.6f)<1e-5&&std::fabs(produced.point[0][2]-30.8f)<1e-5,"actual two world palms preserve distinct hand offsets and model scale");
 // The raw requested wrist and achieved/contact-corrected wrist share the
 // same completed body/request/sample. Door motion must not feed back the
 // achieved point of the panel it is meant to move.
 for(int side=0;side<2;++side){p.allowed[side].q[3]=1;p.propRequested5916c1[side]=p.allowed[side];p.propRequestedValid5916c1[side]=true;}
 PublishHandOnlyDoorH40(tx,p);h40::completed.read(produced);const auto baseline=produced;
 ToT(p.propRequested5916c1[0],{0,-.2f,0});
 PublishHandOnlyDoorH40(tx,p);h40::completed.read(produced);
 ck(std::fabs(produced.point[0][1]-(baseline.point[0][1]-.4f))<1e-5,"same completed raw controller displacement moves door intent despite stationary achieved palm");
 ck(std::memcmp(produced.point[1],baseline.point[1],sizeof(produced.point[1]))==0,"right raw motion preserves independently completed opposite-hand geometry");
 ToT(p.allowed[0],{0,.4f,0});p.palm59c9[0].y+=.4f;
 const auto beforeContact=p;
 PublishHandOnlyDoorH40(tx,p);h40::completed.read(produced);
 ck(std::fabs(produced.point[0][1]-(baseline.point[0][1]-.4f))<1e-5,"achieved contact translation cannot become new door drive input");
 ck(std::memcmp(&beforeContact,&p,sizeof(p))==0&&produced.body==9&&produced.request==10&&produced.sample==20&&produced.tick==1000,"door intent leaves achieved/native packet and original completed identity untouched");
 const float h=std::sqrt(.5f);p.propRequested5916c1[0].q[2]=p.propRequested5916c1[0].q[3]=h;
 PublishHandOnlyDoorH40(tx,p);h40::completed.read(produced);
 ck(std::fabs(produced.point[0][0]-9.4f)<1e-5&&std::fabs(produced.point[0][1]-20.f)<1e-5&&std::fabs(produced.point[0][2]-30.8f)<1e-5,"current raw wrist rotation carries the same fixed anatomical palm-local offset");
 p.world[2]=p.world[3]=h;
 PublishHandOnlyDoorH40(tx,p);h40::completed.read(produced);
 ck(std::fabs(produced.point[0][0]-10.f)<1e-5&&std::fabs(produced.point[0][1]-19.4f)<1e-5,"existing model-world rotation and scale apply once to raw palm geometry");
 p.palmValid59c9[0]=false;PublishHandOnlyDoorH40(tx,p);h40::completed.read(produced);
 ck(std::fabs(produced.point[0][0]-10.4f)<1e-5&&std::fabs(produced.point[0][1]-20.f)<1e-5&&produced.available[0],"missing palm geometry uses current requested wrist without adding a hand-availability veto");
 p.propRequestedValid5916c1[0]=false;PublishHandOnlyDoorH40(tx,p);h40::completed.read(produced);
 ck(std::fabs(produced.point[0][0]-9.2f)<1e-5&&std::fabs(produced.point[0][1]-20.f)<1e-5&&produced.available[0],"missing raw annotation preserves previous achieved-wrist behavior and eligibility");
 // Export the actual producer->Packet result for the composed retail Lua
 // causal fixture: a closed panel keeps achieved palm(.5,0,1) fixed while
 // the same current requested hand moves to(.5,-.25,1).
 p.world[0]=p.world[1]=p.world[2]=0;p.world[3]=p.world[7]=1;p.world[4]=p.world[5]=p.world[6]=0;
 p.allowed[0]={};p.allowed[0].q[3]=1;ToT(p.allowed[0],{.48f,0,1});p.palmValid59c9[0]=true;p.palm59c9[0]={.5f,0,1};
 p.propRequested5916c1[0]=p.allowed[0];p.propRequestedValid5916c1[0]=true;
 const float grip[2]{.8f,-1.f};const std::uint64_t claim[2]{1,0};char wire[512]{};
 PublishHandOnlyDoorH40(tx,p);h40::completed.read(produced);
 ck(h40::Packet(produced,1,"feed",grip,wire,sizeof(wire),claim),"actual native-script packet serializes original current producer");std::printf("H40_WIRE_BEGIN %s\n",wire);
 hostTickCount64=1010;t.tick=t.head_good_tick=t.left_good_tick=t.right_good_tick=1010;t.sequence=21;tx.tracking=t;g_tracking_frames6695.publish(t);
 physical.input.tick=1010;physical.input.sample=21;g_phys_frame6698.publish(physical);g_phys_ready_sample6698=21;
 p.stamp.sample=21;p.stamp.tick=1010;ToT(p.propRequested5916c1[0],{.48f,-.25f,1});PublishHandOnlyDoorH40(tx,p);h40::completed.read(produced);
 ck(std::fabs(produced.point[0][0]-.5f)<1e-5&&std::fabs(produced.point[0][1]+.25f)<1e-5&&p.palm59c9[0].y==0,"raw door point escapes a stationary achieved collision plane without changing that plane");
 ck(produced.sample==21&&produced.tick==1010&&produced.body==9&&produced.request==10,"new hand motion uses its fresh completed sample without extending old tracking or native ownership");
 ck(h40::Packet(produced,2,"feed",grip,wire,sizeof(wire),claim),"moved packet retains original claim/body/request and only new transport serial");std::printf("H40_WIRE_MOVE %s\n",wire);
 ++g_weapon_request_epoch6694;PublishHandOnlyDoorH40(tx,p);h40::completed.read(produced);ck(!produced.body,"request changed during solve revokes instead of retagging producer");--g_weapon_request_epoch6694;
}
int main(){tests=failed=0;
 {resetH40();InputRigH40 r;r.run(990);r.manager.right.grip=.8f;r.run(1000);ck(h40::Reserved()==1&&!r.tracked[0]&&r.tracked[1]&&!r.nativePulses,"actual first unknown Grip is reserved before old native Use and opposite side preserved");IOH40 io;stepH40(io);ck(io.probes==1&&io.feeds==1&&h40::Reserved()==1&&io.lastToken[0]!=0,"actual first press probes then feeds original claim token exactly once");ck(!h40::Wanted(1201)||h40::Reserved(),"bounded service wanted has no permanent near-only owner");}
 {resetH40();InputRigH40 r;r.run(990);r.manager.right.grip=.31f;r.run(1000);r.manager.right.grip=.45f;r.run(1005);ck(!h40::Reserved(),"analog ramp below real press cannot prematurely consume no-door");r.manager.right.grip=.65f;r.run(1010);ck(h40::Reserved()==1,"gradual analog ramp acquires at actual >55percent Grip");}
 {resetH40();InputRigH40 r;r.run(990);r.manager.right.grip=.8f;r.run(1000);IOH40 io;io.result=IOH40::NoDoor;stepH40(io);ck(!h40::Reserved()&&h40::fallbackToken[0]!=0,"exact initial no-door reply releases to retained original input");r.run(1020);ck(r.original==1000,"actual held input FSM receives original press time without restamp");r.run(1140);r.run(1150);ck(r.nativePulses==1,"actual same held no-door press emits existing native Use once");}
 {resetH40();InputRigH40 r;r.candidate.entity=7;r.candidate.owner=0;r.candidate.tick=1000;std::strcpy(r.candidate.cls,"PickableItem");r.run(990);r.manager.right.grip=.8f;r.run(1000);ck(!h40::Reserved()&&r.tracked[0],"actual known non-door firstpress stays on existing pickup route");}
 for(unsigned refusal=0;refusal<3;++refusal){resetH40();InputRigH40 r;r.run(990);r.manager.right.grip=.8f;if(refusal==0)r.manager.right.trigger=.9f;if(refusal==1)r.empty[0]=false;if(refusal==2)r.blocked=true;r.run(1000);ck(!h40::Reserved()&&!h40::Wanted(1000),"fist/occupied/menu never reserves or wakes door probe");}
 {resetH40();h40::Admission owner;ck(owner.take(1),"same-hand other owner enters");ck(!h40::CandidatePress(0,0,1000,true,.8f,0)&&h40::CandidatePress(1,0,1000,true,.8f,0),"shared actual admission rejects same hand and permits opposite hand");}
 {resetH40();h40::CandidatePress(0,0,1000,true,.8f,0);IOH40 io;stepH40(io);publishH40(1020,0,.8f);io.result=IOH40::Retire;stepH40(io,1020);ck(h40::Reserved()==1&&io.action=="clear"&&!h40::fallbackToken[0],"release pending retains native may-own and cannot fall back");io.result=IOH40::NoDoor;stepH40(io,1021);ck(!h40::Reserved(),"exact original owned handback alone retires claim");}
 for(auto result:{IOH40::Failure,IOH40::Bad,IOH40::WrongSerial,IOH40::UnexpectedSide}){resetH40();h40::CandidatePress(0,0,1000,true,.8f,0);IOH40 io;stepH40(io);io.result=result;stepH40(io,1001);ck(h40::Reserved()==1&&h40::service.fault&&!h40::fallbackToken[0],"unknown malformed stale foreign reply retains original native debt");}
 {resetH40();h40::CandidatePress(0,0,1000,true,.8f,0);IOH40 io;stepH40(io);auto f=frameH40();f.body=11;f.request=12;f.tick=1020;h40::PublishCompleted(f);stepH40(io,1020);ck(io.action=="clear"&&io.lastBody==9&&io.lastRequest==10,"body/request change cleans original identity instead of restamping");}
 {resetH40();h40::CandidatePress(0,0,1000,true,.8f,0);IOH40 io;stepH40(io);auto calls=io.calls;stepH40(io,1001,2);ck(h40::vmQuarantine&&h40::Reserved()==1&&io.calls==calls&&!h40::Wanted(1001),"replacement VM cannot forgive old paused clip or reacquire held Grip");}
 {resetH40();h40::ObserveNear(1000);IOH40 io;stepH40(io);ck(h40::ready&&!h40::Wanted(1000)&&io.probes==1,"ready idle owner stops VM inspection even while near");}
 {resetH40();InputRigH40 r;r.run(990);r.manager.right.grip=.8f;
  {h40::Admission busy;busy.take(1);r.run(1000);ck(h40::Pending(0)&&!h40::Reserved()&&!r.tracked[0],"actual same-side admission contention parks original press before old Use");}
  publishH40(1020);r.run(1020);h40::Candidate c{};h40::candidates[0].read(c);ck(h40::Reserved()==1&&c.tick==1000,"same original press survives transient admission contention without restamp");}
 {resetH40();h40::inputIdentityReader=+[](h40::Frame&out,std::uint64_t) noexcept {out=frameH40();return true;};h40::RevokeCompleted();InputRigH40 r;r.run(990);r.manager.right.grip=.8f;r.run(1000);
  ck(h40::Pending(0)&&!r.tracked[0],"firstpress can retain original input identity while completed pose is unavailable");publishH40(1020);r.run(1020);h40::Candidate c{};h40::candidates[0].read(c);ck(h40::Reserved()==1&&c.tick==1000,"matching later completed pose joins original pending press");}
 {resetH40();h40::CandidatePress(0,0,1000,true,.8f,0);IOH40 io;io.result=IOH40::NoDoor;stepH40(io);h40::CandidatePress(0,0,1001,true,0,0);h40::Candidate out{};ck(!h40::TakeFallback(0,frameH40(),1001,out),"physical neutral cancels old no-door fallback before any new press");}
 {resetH40();auto old=frameH40();old.grip[0]=.1f;h40::PublishCompleted(old);InputRigH40 r;r.run(990);r.manager.right.grip=.8f;r.run(1000);IOH40 io;stepH40(io);
  ck(h40::Reserved()==1&&io.feeds==0&&io.clears==0&&!h40::entered[0],"old completed neutral cannot clear a newly reserved raw press before qualified pose");
  publishH40(1020);stepH40(io,1020);ck(io.feeds==1&&h40::entered[0],"next qualified completed pose drives retained original press once");}
 {resetH40();InputRigH40 r;r.run(990);r.manager.right.grip=.56f;r.manager.right.trigger=.9f;r.run(1000);ck(!h40::Reserved()&&!h40::Wanted(1000),"Trigger-first fist ramp cannot become a Grip-door query before fist threshold");}
 {resetH40();h40::CandidatePress(0,0,1000,true,.8f,0);IOH40 io;{h40::Admission busy;busy.take(1,true);stepH40(io);ck(!io.calls&&h40::Reserved()==1,"script admission contention cannot consume already reserved firstpress");}stepH40(io);ck(io.feeds==1,"reserved firstpress retries actual script callback after contention");}
 {resetH40();InputRigH40 r;r.run(990);r.manager.right.grip=.56f;publishH40(1000,.56f);r.run(1000);IOH40 io;io.result=IOH40::NoDoor;stepH40(io);r.run(1020);r.manager.right.grip=.8f;r.run(1040);ck(r.original==1000,"no-door analog ramp crossing old native58percent retains original55percent press clock");r.run(1160);r.run(1170);ck(r.nativePulses==1,"delayed native threshold crosses once after no-door handoff");}
 {resetH40();h40::CandidatePress(0,0,1000,true,.8f,0);IOH40 io;io.result=IOH40::Failure;stepH40(io);ck(h40::service.fault&&!h40::entered[0],"failed readiness-only probe records no native door entry");stepH40(io,1001);io.result=IOH40::NoDoor;stepH40(io,1002);ck(h40::ready&&!h40::service.fault&&!h40::Reserved(),"exact empty readiness reply recovers only local never-entered probe fault");h40::CandidatePress(0,0,1003,true,0,0);h40::CandidatePress(0,0,1004,true,.8f,0);stepH40(io,1004);ck(io.feeds==1,"later real press feeds after verified local probe recovery");}
 {resetH40();h40::CandidatePress(0,0,1000,true,.8f,0);IOH40 io;io.result=IOH40::Failure;stepH40(io);stepH40(io,1001);ck(io.probes==1&&!h40::Reserved(),"failed original claim probes once instead of per-frame retry");h40::CandidatePress(0,0,1002,true,0,0);h40::CandidatePress(0,0,1003,true,.8f,0);io.result=IOH40::NoDoor;stepH40(io,1003);ck(io.probes==2&&io.feeds==1&&!h40::service.fault,"new original recovery press probes and feeds without requiring idle callback or second press");}
 {resetH40();RigH36 r;g_native_held_release_inhibit_t4=false;
  {CoherentTransaction6689 tx;tx.doorHolsterAdmissionH40[0].take(1);tx.doorHolsterAdmissionH40[1].take(2);
   ph::Output noDraw{};PhysHolsterGestures6698(r.in,r.gear,noDraw,r.states,3);h40::ReleaseUnstagedAdmissions(noDraw,ph::CommandKind::Draw,tx.doorHolsterAdmissionH40);
   h40::Admission utility;ck(noDraw.commandCount==0&&!h40::admissionBusy&&utility.take(1),"actual no-DRAW holster producer releases leases before unrelated utility/skinning");}
  r.out={};ck(r.draw(),"control actual native-side left DRAW staged by production holster producer");
  {CoherentTransaction6689 tx;tx.doorHolsterAdmissionH40[0].take(1);tx.doorHolsterAdmissionH40[1].take(2);h40::ReleaseUnstagedAdmissions(r.out,ph::CommandKind::Draw,tx.doorHolsterAdmissionH40);
   ck(h40::admissionBusy==3&&r.out.commands[0].side==1,"actual DRAW retains both native and possible cross-body gesture-hand exclusions through publication");
   g_phys_commands6698.push(r.out.commands[0]);ck(g_phys_commands6698.head!=g_phys_commands6698.tail&&h40::admissionBusy==3,"actual command queue publication occurs while both original staging leases still held");}
  ck(!h40::admissionBusy,"completed transaction releases staging admissions after queue publication");}
 {resetH40();sourceTestsH40();}
 std::printf("H40 %u/%u PASS\n",tests-failed,tests);return failed?1:0;
}

#endif // KCD2VR_H40_PARTIAL_REST_LUA51
