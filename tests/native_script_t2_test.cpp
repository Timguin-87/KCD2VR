// Executes the actual production VM scheduler/packet adapter. Only native C API
// leaves and game memory are test-owned; no real game function/DLL executes.
#define KCD2VR_SCRIPT_T2_HOST_TEST 1
#include "../src/native_script_t2.cpp"
#include <cassert>
#include <string>
#include <vector>
#include "../src/subsystems/doors/hand_only_h40.h"
#include "../src/diagnostics/capture168.h"
namespace kcd2vr::retail_build112 {bool Verify(std::uintptr_t) noexcept{return true;}}
static std::vector<std::string> trace;
namespace kcd2vr::diagnostic_trace {void Write(const char*s) noexcept{trace.emplace_back(s);}}
namespace {
std::array<unsigned char,256> scriptSystem{},vm{},otherVm{},stack{},movedStack{},resultString{};
int relocateAt=0;bool expectedPacket=true,changeFrame=false,corruptFrame=false,badString=false,nativeReenter=false,raiseFault=false,doorReplies=false;
unsigned doorCalls=0,displayCalls=0;std::size_t outputCap=64;

int originalCalls=0,clientCalls=0,pcalls=0,restores=0,getFields=0,pushes=0;
bool missing=false,pcallError=false,wrongType=false,replace=false,reenter=false,domainWanted=true,expectRoom=true,shrinkAfterField=false;
bool Want() noexcept{return domainWanted;}
std::string result="ACK|1",lastInput,lastName,requestR96="DRAW|1";
template<class T>void put(void* p,std::size_t off,T value){std::memcpy(static_cast<unsigned char*>(p)+off,&value,sizeof(value));}
template<class T>T get(void* p,std::size_t off){T value{};std::memcpy(&value,static_cast<unsigned char*>(p)+off,sizeof(value));return value;}
void Relocate(void* L){
 const auto before=get<std::uintptr_t>(L,0x40);
 const auto after=reinterpret_cast<std::uintptr_t>(before==reinterpret_cast<std::uintptr_t>(stack.data())?movedStack.data():stack.data());
 std::memcpy(reinterpret_cast<void*>(after),reinterpret_cast<void*>(before),stack.size());
 put(L,0x40,after);put(L,0x18,after+(get<std::uintptr_t>(L,0x18)-before));
 put(L,0x10,after+(get<std::uintptr_t>(L,0x10)-before));put(L,0x38,after+240);
}

void GetField(void* L,int index,const char* name){++getFields;assert(index==-10002);lastName=name;
 auto top=get<std::uintptr_t>(L,0x10);put(reinterpret_cast<void*>(top),8,missing?0:6);put(L,0x10,top+16);if(shrinkAfterField)put(L,0x38,top+32);if(relocateAt==1)Relocate(L);}
int Type(void* L,int index){assert(index==-1);return get<int>(reinterpret_cast<void*>(get<std::uintptr_t>(L,0x10)-16),8);}
void Push(void* L,const char* input){++pushes;lastInput=input;put(L,0x10,get<std::uintptr_t>(L,0x10)+16);if(relocateAt==2)Relocate(L);}
int PCall(void* L,int args,int results,int errors){assert(args==1&&results==1&&errors==0);++pcalls;
 if(raiseFault){
#ifdef _WINDOWS_
  RaiseException(0xE04B4344,0,0,nullptr);
#else
  throw 7; // syntax-shim __except branch; real SDK runner also exercises native SEH.
#endif
 }
 if(nativeReenter){nativeReenter=false;kcd2vr::native_script_t2::Detour(scriptSystem.data());}
 if(doorReplies){
  unsigned long long serial=0,body=0,request=0;unsigned player=0;char action[12]{};
  assert(std::sscanf(lastInput.c_str(),"%llu|%11[^|]|%u|%llu|%llu",&serial,action,&player,&body,&request)==5);
  char out[220]{};
  if(lastName=="kcd2vr_door_display_h40_t1"){
   ++displayCalls;std::snprintf(out,sizeof(out),"H40D1|%llu|%u|%llu|%llu|1|55|101|1|2|3|0.5|0|0|0|0|0|0",serial,player,body,request);
  }else{++doorCalls;const bool probe=std::strcmp(action,"probe")==0;
   std::snprintf(out,sizeof(out),"OK|%llu|%u|%llu|%llu|%u|0|%llu|%u|0",serial,player,body,request,probe?0:1,probe?serial:0,probe?0:101);}
  result=out;
 }
 auto top=get<std::uintptr_t>(L,0x10)-16;put(L,0x10,top);
 put(resultString.data(),0x10,std::uint64_t(result.size()));std::memcpy(resultString.data()+0x18,result.data(),result.size());
 put(reinterpret_cast<void*>(top-16),0,reinterpret_cast<std::uintptr_t>(resultString.data()));put(reinterpret_cast<void*>(top-16),8,wrongType?3:4);
 if(badString)put(reinterpret_cast<void*>(top-16),0,std::uintptr_t(1));
 if(relocateAt==3)Relocate(L);
 if(changeFrame)put(L,0x18,get<std::uintptr_t>(L,0x18)+16);
 if(corruptFrame)put(L,0x18,get<std::uintptr_t>(L,0x18)+1);
 if(replace)put(scriptSystem.data(),0x10,reinterpret_cast<std::uintptr_t>(otherVm.data()));return pcallError?1:0;
}
void SetTop(void* L,int count){assert(count==2);++restores;put(L,0x10,get<std::uintptr_t>(L,0x18)+std::uintptr_t(count)*16);}
std::uintptr_t Original(void*){++originalCalls;return 47;}
void reset(){scriptSystem.fill(0);vm.fill(0);otherVm.fill(0);stack.fill(0);resultString.fill(0);
 trace.clear();
 put(scriptSystem.data(),0,std::uintptr_t(0x180000000ull+0x3B8C610));put(scriptSystem.data(),0x10,reinterpret_cast<std::uintptr_t>(vm.data()));
 put(vm.data(),0x40,reinterpret_cast<std::uintptr_t>(stack.data()));put(vm.data(),0x18,reinterpret_cast<std::uintptr_t>(stack.data()));put(vm.data(),0x10,reinterpret_cast<std::uintptr_t>(stack.data()+32));put(vm.data(),0x38,reinterpret_cast<std::uintptr_t>(stack.data()+240));
 put(otherVm.data(),0x18,reinterpret_cast<std::uintptr_t>(stack.data()));put(otherVm.data(),0x10,reinterpret_cast<std::uintptr_t>(stack.data()+32));
 relocateAt=0;expectedPacket=true;changeFrame=corruptFrame=badString=nativeReenter=raiseFault=doorReplies=false;doorCalls=displayCalls=0;outputCap=64;missing=pcallError=wrongType=replace=reenter=shrinkAfterField=false;domainWanted=expectRoom=true;result="ACK|1";
 using namespace kcd2vr::native_script_t2;base=0x180000000ull;enabled=true;original=&Original;current=nullptr;
 for(auto& c:clients)c=nullptr;lastSystem=lastState=0;ownerThread=0;generation=0;inside=false;servicing.clear();transportFaultR96={};callbackRevokedR96=false;
 kcd2vr::inventory_lifecycle_t8::loadEpisode18=1;kcd2vr::inventory_lifecycle_t8::loadingObserved18=false;kcd2vr::inventory_lifecycle_t8::state=2;
 requestR96="DRAW|1";originalCalls=clientCalls=pcalls=restores=getFields=pushes=0;
}
void OwnClient(const kcd2vr::native_script_t2::Context& c) noexcept {
 assert(originalCalls>clientCalls);++clientCalls;char out[64]{};
 const bool ok=c.CallPacket("kcd2vr_supply_native_t2",requestR96.c_str(),out,outputCap);
 assert(ok==(expectedPacket&&expectRoom&&!missing&&!pcallError&&!wrongType&&!replace));if(ok)assert(std::string(out)==result);else assert(out[0]==0);
 if(reenter){reenter=false;kcd2vr::native_script_t2::Detour(scriptSystem.data());}
}
}
namespace kcd2vr::native_script_t2 {namespace {
std::uintptr_t HostNative(std::uintptr_t rva) noexcept {
 if(rva==0xAD496C)return reinterpret_cast<std::uintptr_t>(&GetField);
 if(rva==0x715E00)return reinterpret_cast<std::uintptr_t>(&Type);
 if(rva==0x7182C4)return reinterpret_cast<std::uintptr_t>(&Push);
 if(rva==0x713914)return reinterpret_cast<std::uintptr_t>(&PCall);
 if(rva==0x717AEC)return reinterpret_cast<std::uintptr_t>(&SetTop);
 std::abort();
}
}}
namespace hd=kcd2vr::hand_only_door_h40;
bool DoorCurrent(const hd::Frame&,int,std::uint64_t)noexcept{return true;}
#ifdef _WINDOWS_
void FaultClient(const kcd2vr::native_script_t2::Context&)noexcept {RaiseException(0xE04B4344,0,0,nullptr);}
#endif
void DoorClient(const kcd2vr::native_script_t2::Context&c)noexcept{
 struct IO{const kcd2vr::native_script_t2::Context&c;
  bool call(const char*in,char*out,size_t n)noexcept{return c.CallPacket(hd::kCallback,in,out,n);}
  bool display(const char*in,char*out,size_t n)noexcept{return c.CallPacket("kcd2vr_door_display_h40_t1",in,out,n);}
 }io{c};hd::service.tick(io,c.now,c.generation);
}
int main(){using namespace kcd2vr::native_script_t2;
 reset();assert(Register(&OwnClient)&&Register(&OwnClient));assert(Detour(scriptSystem.data())==47);
 assert(originalCalls==1&&clientCalls==1&&pcalls==1&&restores==1&&lastInput=="DRAW|1");
 assert(get<std::uintptr_t>(vm.data(),0x10)==reinterpret_cast<std::uintptr_t>(stack.data()+32));
 Context forged{reinterpret_cast<std::uintptr_t>(scriptSystem.data()),reinterpret_cast<std::uintptr_t>(vm.data()),100,1};char out[64]{};
 assert(!forged.CallPacket("kcd2vr_supply_native_t2","DRAW|2",out,sizeof(out))&&pcalls==1);
 reset();missing=true;Register(&OwnClient);Detour(scriptSystem.data());assert(pcalls==0&&restores==1&&enabled);
 reset();pcallError=true;Register(&OwnClient);Detour(scriptSystem.data());assert(pcalls==1&&restores==1&&enabled);
 reset();wrongType=true;Register(&OwnClient);Detour(scriptSystem.data());assert(pcalls==1&&restores==1&&enabled);
 reset();replace=true;Register(&OwnClient);Detour(scriptSystem.data());assert(enabled&&transportFaultR96.kind!=FaultKindR96::None&&restores==0);Detour(scriptSystem.data());assert(clientCalls==1);
 // Actual production adapter: no native C API push when free capacity is
 // exhausted, exactly at its native guard boundary, reversed or unaligned.
 for(int bytes:{-16,0,16,32,33}){
  reset();expectRoom=false;const auto top=get<std::uintptr_t>(vm.data(),0x10);
  put(vm.data(),0x38,std::uintptr_t(std::intptr_t(top)+bytes));Register(&OwnClient);Detour(scriptSystem.data());
  assert(getFields==0&&pushes==0&&pcalls==0&&restores==0&&enabled&&get<std::uintptr_t>(vm.data(),0x10)==top);
 }
 reset();expectRoom=false;shrinkAfterField=true;Register(&OwnClient);Detour(scriptSystem.data());
 assert(getFields==1&&pushes==0&&pcalls==0&&restores==1&&enabled&&
    get<std::uintptr_t>(vm.data(),0x10)==reinterpret_cast<std::uintptr_t>(stack.data()+32));
 // Just beyond the native strict guard supports two TValue16 pushes; the
 // original call/frame restoration and result checks still execute normally.
 reset();put(vm.data(),0x38,reinterpret_cast<std::uintptr_t>(stack.data()+80));Register(&OwnClient);Detour(scriptSystem.data());
 assert(getFields==1&&pushes==1&&pcalls==1&&restores==1&&enabled);
 reset();Register(&OwnClient);reenter=true;Detour(scriptSystem.data());assert(originalCalls==2&&clientCalls==1&&restores==1);
 reset();domainWanted=false;Register(&OwnClient,&Want);Detour(scriptSystem.data());
 assert(originalCalls==1&&clientCalls==0&&lastSystem==0&&generation==0); // idle exits before Vm/VirtualQuery.
 domainWanted=true;Detour(scriptSystem.data());assert(originalCalls==2&&clientCalls==1&&generation==1);
 Shutdown();Detour(scriptSystem.data());assert(originalCalls==3&&clientCalls==1&&!enabled); // retained original remains live.
 std::puts("PASS actual T2 original-first scheduler, exact pinned stack capacity/no-push and post-field exhaustion, protected result/frame restoration, no forged/reentrant/replacement VM use");
 unsigned checks=0;const auto ck=[&](bool v,const char*name){++checks;if(!v){std::printf("FAIL %s\n",name);std::abort();}};
 for(int phase:{1,2,3}){
  reset();relocateAt=phase;Register(&OwnClient);Detour(scriptSystem.data());
  ck(enabled&&pcalls==1&&restores==1,"same VM stack rebase keeps transport live");
  ck(get<std::uintptr_t>(vm.data(),0x10)-get<std::uintptr_t>(vm.data(),0x18)==32,"restore original relative top");
  Detour(scriptSystem.data());ck(enabled&&pcalls==2&&clientCalls==2&&restores==2,"next client still executes once");
 }
 for(auto cap:{std::size_t(1),std::size_t(16385)}){
  reset();outputCap=cap;expectedPacket=false;Register(&OwnClient);Detour(scriptSystem.data());
  ck(enabled&&getFields==0&&pcalls==0&&restores==0,"invalid output capacity does not enter native Lua");
 }
 reset();outputCap=2;relocateAt=3;result="X";Register(&OwnClient);Detour(scriptSystem.data());ck(enabled&&restores==1,"one-byte exact-fit output survives rebase");
 reset();relocateAt=3;result=std::string(63,'x');Register(&OwnClient);Detour(scriptSystem.data());ck(enabled&&restores==1,"maximum exact-fit output survives rebase");
 reset();relocateAt=3;pcallError=true;Register(&OwnClient);Detour(scriptSystem.data());
 ck(enabled&&pcalls==1&&restores==1,"Lua error after relocation restores without disabling all clients");
 reset();relocateAt=3;wrongType=true;Register(&OwnClient);Detour(scriptSystem.data());ck(enabled&&restores==1,"nonstring after relocation stays failed but live");
 reset();relocateAt=3;result=std::string(100,'x');expectedPacket=false;Register(&OwnClient);Detour(scriptSystem.data());ck(enabled&&restores==1,"oversized reply never copied and stack restored");
 reset();relocateAt=3;result=std::string("A\0B",3);expectedPacket=false;Register(&OwnClient);Detour(scriptSystem.data());ck(enabled&&restores==1,"embedded NUL reply remains refused");
 reset();relocateAt=3;badString=true;expectedPacket=false;Register(&OwnClient);Detour(scriptSystem.data());ck(enabled&&restores==1,"unreadable result pointer remains refused");
 for(int which:{0,1,2}){
  reset();expectedPacket=false;relocateAt=3;if(which==0)changeFrame=true;if(which==1)corruptFrame=true;if(which==2)replace=true;
  Register(&OwnClient);Detour(scriptSystem.data());ck(enabled&&transportFaultR96.kind!=FaultKindR96::None&&restores==0,"different frame/corrupt cursor/replaced VM suspends this occurrence");
  Detour(scriptSystem.data());ck(originalCalls==2&&clientCalls==1,"true unknown never resumes own clients");
 }
 reset();nativeReenter=true;relocateAt=3;Register(&OwnClient);Detour(scriptSystem.data());ck(enabled&&originalCalls==2&&clientCalls==1&&pcalls==1&&restores==1,"nested native update original once no duplicate own callback");
 reset();raiseFault=true;expectedPacket=false;Register(&OwnClient);Detour(scriptSystem.data());ck(enabled&&transportFaultR96.kind==FaultKindR96::Native&&pcalls==1&&restores==0,"caught native SEH quarantines the current VM without administrative shutdown");
 namespace capture=kcd2vr::diagnostics::capture168;
 ck(trace.size()==1&&trace[0]=="R96 SCRIPT_VM F5_TRANSPORT_ERROR scope=CURRENT_VM reason=NATIVE_FAULT clients=SUSPENDED","actual fault edge carries the interaction capture family");
 ck(!capture::essential(trace[0].c_str())&&!capture::accept(trace[0].c_str(),100),"fault family remains capture gated while OFF");
 capture::gate.select(capture::Domain::Combat,true,100);
 ck(!capture::accept(trace[0].c_str(),100),"F3 alone does not misfile script-transport fault");
 capture::gate.select(capture::Domain::Combat,false,100);capture::gate.select(capture::Domain::Interaction,true,100);
 ck(capture::accept(trace[0].c_str(),100),"F5-only captures the actual transport fault");
 capture::gate.select(capture::Domain::Interaction,false,100);
#ifdef _WINDOWS_
 reset();Register(&FaultClient);Detour(scriptSystem.data());
 ck(enabled&&transportFaultR96.kind==FaultKindR96::Native&&trace.size()==1&&trace[0]=="R96 SCRIPT_VM F5_TRANSPORT_ERROR scope=CURRENT_VM reason=CLIENT_NATIVE_FAULT clients=SUSPENDED","actual outer client SEH quarantines the current VM and labels its edge");
 capture::gate.select(capture::Domain::Interaction,true,100);
 ck(capture::accept(trace[0].c_str(),100)&&!capture::essential(trace[0].c_str()),"F5 captures outer client fault without always-on logging");
 capture::gate.select(capture::Domain::Interaction,false,100);
#endif
 reset();doorReplies=true;relocateAt=3;hd::claims[0]=55;hd::entered[0]=0;hd::claims[1]=hd::entered[1]=0;
 hd::admissionBusy=0;hd::ready=false;hd::vmQuarantine=false;hd::enabled=true;hd::displayEnabled=true;hd::currentReader=&DoorCurrent;hd::service={};
 hd::Frame f{};f.tick=GetTickCount64();f.body=7;f.request=8;f.sample=20922;f.tracking=4;f.policy=2;f.player=30583;f.actor=13;f.character=14;f.permitted=true;f.available[0]=f.available[1]=true;f.grip[0]=1;
 hd::completed.publish(f);hd::candidates[0].publish({f,55,f.tick,101});Register(&DoorClient);Detour(scriptSystem.data());
 ck(enabled&&pcalls==3&&restores==3&&doorCalls==2&&displayCalls==1,"real door probe/feed/display calls each execute once across valid rebase");
 ck(hd::service.serial==2&&!hd::service.fault&&hd::Reserved()==1&&hd::entered[0]==55&&hd::service.doors[0]==101,"actual H40 ACK decode retains exact original native operation");
 hd::Display shown{};ck(hd::display.read(shown)&&shown.mask==1&&shown.token[0]==55&&shown.door[0]==101&&shown.point[0][0]==1&&shown.phase[0]==.5f,"actual separate query result decoded once");
 ck(get<std::uintptr_t>(vm.data(),0x10)-get<std::uintptr_t>(vm.data(),0x18)==32,"door service leaves original Lua stack height");
 std::printf("SCRIPT_STACK_REBASE %u/%u supplemental checks; retained original assertions PASS\n",checks,checks);
 return 0;
}
