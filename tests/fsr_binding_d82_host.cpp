// Literal production Install and wrappers; only native pin/module/hook leaves
// are replaced. Runtime and public FFX types are the actual production code.
#include "fsr_stereo_runtime_d82.h"
#include "native_stereo_preflight_v6602.h"
#include "native_fsr_stereo_d82.h"
#include "diagnostic_trace.h"
#include <optional>
#include <vector>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cwchar>
#include <type_traits>
static unsigned checks=0;
#define CHECK(x) do{++checks;if(!(x)){std::fprintf(stderr,"FAIL %u line%d: %s\n",checks,__LINE__,#x);std::exit(1);}}while(0)
static bool loaderLoaded=false,gameLoaded=false,pinned=true,disableFails=false;
static int makeFailure=-1,enableFailure=-1;
static unsigned makeCalls=0,enableCalls=0,disableCalls=0,hookDestructions=0,pinCalls=0;
static unsigned creates=0,destroys=0,dispatches=0,configures=0,activeReads=0,viewReads=0;
static bool activeView=false,validView=false,apiPublished=false;
static kcd2vr::native_stereo_preflight::UpscalerView currentView{};
static std::vector<std::string> events,logs;static std::vector<ffxContext> dispatched;
static std::vector<unsigned> enableOrder;
static std::array<bool,4> hookEnabled{};
static std::array<int,32> contexts{};
using HMODULE=void*;
using DWORD=std::uint32_t;
static DWORD lastError=0;
static DWORD GetLastError(){return lastError;}
static void SetLastError(DWORD value){lastError=value;}
static HMODULE GetModuleHandleW(const wchar_t* name){
 if(!std::wcscmp(name,L"amd_fidelityfx_loader_dx12.dll"))return loaderLoaded?reinterpret_cast<void*>(1):nullptr;
 if(!std::wcscmp(name,L"WHGame.dll"))return gameLoaded?reinterpret_cast<void*>(2):nullptr;
 return nullptr;
}
namespace kcd2vr::diagnostic_trace {void Write(const char* s) noexcept {logs.emplace_back(s?s:"");lastError=0xbadf00du;}}
namespace kcd2vr::native_stereo_preflight {
bool InUpscalerView() noexcept {++activeReads;return activeView;}
bool CurrentUpscalerView(UpscalerView& v) noexcept {++viewReads;v=currentView;return validView;}
}
static ffxReturnCode_t sdkCreate(ffxContext* c,ffxCreateContextDescHeader*,const ffxAllocationCallbacks*){CHECK(c&&!*c);CHECK(creates<contexts.size());*c=&contexts[creates++];return FFX_API_RETURN_OK;}
static ffxReturnCode_t sdkDestroy(ffxContext* c,const ffxAllocationCallbacks*){++destroys;if(c)*c=nullptr;return FFX_API_RETURN_OK;}
static ffxReturnCode_t sdkDispatch(ffxContext* c,const ffxDispatchDescHeader*){++dispatches;CHECK(c&&*c);dispatched.push_back(*c);lastError=0x1234u;return FFX_API_RETURN_OK;}
static ffxReturnCode_t sdkConfigure(ffxContext* c,const ffxConfigureDescHeader*){++configures;CHECK(c&&*c);return FFX_API_RETURN_OK;}
struct Hook {
 unsigned index=0;explicit Hook(unsigned i):index(i){}
 Hook(const Hook&)=delete;Hook& operator=(const Hook&)=delete;
 ~Hook(){++hookDestructions;}
 bool enable(){CHECK(makeCalls==4&&apiPublished);++enableCalls;enableOrder.push_back(index);events.emplace_back("enable"+std::to_string(index));
  if(index==3)CHECK(hookEnabled[0]&&hookEnabled[1]&&hookEnabled[2]);
  if(int(index)==enableFailure)return false;
  hookEnabled[index]=true;return true;}
 bool disable(){++disableCalls;events.emplace_back("disable"+std::to_string(index));if(disableFails)return false;hookEnabled[index]=false;return true;}
};
namespace kcd2vr::native_fsr_stereo_d82 {namespace {
namespace rt=fsr_stereo_runtime_d82;
// Observe SetApi ordering while executing the real Runtime setter and all
// real Runtime methods. This does not replace runtime behavior.
struct RuntimeProbe:rt::Runtime {void SetApi(rt::Api api) noexcept {
 apiPublished=api.create&&api.destroy&&api.dispatch&&api.configure;rt::Runtime::SetApi(api);
}} runtime;
std::mutex installMutex;
std::atomic<bool> installed{false};bool attempted=false;
std::atomic<std::uint64_t> completedPairs{0};
std::array<std::optional<Hook>,4> hooks;
rt::Api originals{};
static bool ExactGame() noexcept {++pinCalls;return pinned;}
template<class Fn>static bool Make(unsigned i,HMODULE module,const char* name,Fn,Fn& original) noexcept {
 CHECK(module==reinterpret_cast<void*>(1));++makeCalls;events.emplace_back(name);
 const char* expected[]{"ffxDestroyContext","ffxDispatch","ffxConfigure","ffxCreateContext"};CHECK(i<4&&std::string(name)==expected[i]);
 if(int(i)==makeFailure)return false;
 hooks[i].emplace(i);
 if constexpr(std::is_same_v<Fn,PfnFfxCreateContext>)original=sdkCreate;
 else if constexpr(std::is_same_v<Fn,PfnFfxDestroyContext>)original=sdkDestroy;
 else if constexpr(std::is_same_v<Fn,PfnFfxDispatch>)original=i==1?sdkDispatch:sdkConfigure; // dispatch/configure share the public header signature
 return true;
}
#include "fsr_wrappers_extracted.inc"
}
#include "fsr_install_extracted.inc"
}
static ffxDispatchDescUpscale Input(){ffxDispatchDescUpscale d{};d.header.type=FFX_API_DISPATCH_DESC_TYPE_UPSCALE;
 d.commandList=reinterpret_cast<void*>(10);d.color.resource=reinterpret_cast<void*>(11);d.depth.resource=reinterpret_cast<void*>(12);d.motionVectors.resource=reinterpret_cast<void*>(13);d.output.resource=reinterpret_cast<void*>(14);
 d.renderSize={1706,960};d.upscaleSize={2560,1440};d.jitterOffset={.25f,-.125f};d.motionVectorScale={1706,960};d.preExposure=1;return d;}
int main(int argc,char** argv){namespace n=kcd2vr::native_fsr_stereo_d82;const std::string scenario=argc>1?argv[1]:"success";
 n::SetEnabled(true);
 if(scenario=="late_module"){
  CHECK(!n::Install()&&!n::attempted&&makeCalls==0);loaderLoaded=true;CHECK(!n::Install()&&!n::attempted&&makeCalls==0);
  gameLoaded=true;CHECK(n::Install()&&n::Installed());CHECK(makeCalls==4&&enableCalls==4&&pinCalls==1);
 }else if(scenario=="wrong_pin"){
  loaderLoaded=gameLoaded=true;pinned=false;CHECK(!n::Install()&&n::attempted&&pinCalls==1&&makeCalls==0);
  pinned=true;CHECK(!n::Install()&&pinCalls==1&&makeCalls==0);
 }else if(scenario=="construction"){
  loaderLoaded=gameLoaded=true;makeFailure=2;CHECK(!n::Install()&&n::attempted&&makeCalls==3&&enableCalls==0);makeFailure=-1;CHECK(!n::Install()&&makeCalls==3);
 }else if(scenario.rfind("enable",0)==0){
  loaderLoaded=gameLoaded=true;enableFailure=std::stoi(scenario.substr(6));disableFails=true;
  CHECK(!n::Install()&&!n::Installed()&&n::attempted);CHECK(makeCalls==4&&enableCalls==unsigned(enableFailure+1)&&disableCalls==4);
  CHECK(hookDestructions==0);for(const auto& h:n::hooks)CHECK(h.has_value());
  CHECK(apiPublished&&n::originals.create&&n::originals.destroy&&n::originals.configure&&n::originals.dispatch);
  // Retained API must remain usable by an already-entered pass-through even
  // after an uncertain disable. No new stereo twin is admitted by the binding.
  n::SetEnabled(false);ffxContext c=reinterpret_cast<void*>(99);ffxConfigureDescHeader conf{};
  CHECK(n::Configure(&c,&conf)==FFX_API_RETURN_OK&&configures==1);
  activeView=false;ffxDispatchDescHeader desc{};CHECK(n::Dispatch(&c,&desc)==FFX_API_RETURN_OK&&dispatches==1);
  CHECK(n::Destroy(&c,nullptr)==FFX_API_RETURN_OK&&destroys==1);
  CHECK(!n::Install()&&makeCalls==4&&enableCalls==unsigned(enableFailure+1)&&hookDestructions==0);
 }else{
  loaderLoaded=gameLoaded=true;CHECK(n::Install());CHECK((enableOrder==std::vector<unsigned>{0,1,2,3}));
  CHECK(makeCalls==4&&enableCalls==4&&pinCalls==1);CHECK(n::Install()&&makeCalls==4&&enableCalls==4&&pinCalls==1);
  CHECK(!n::ReadyForStereo());ffxCreateContextDescUpscale desc{};desc.header.type=FFX_API_CREATE_CONTEXT_DESC_TYPE_UPSCALE;
  desc.maxRenderSize={1706,960};desc.maxUpscaleSize={2560,1440};ffxContext c=nullptr;
  CHECK(n::Create(&c,&desc.header,nullptr)==FFX_API_RETURN_OK&&creates==2&&n::ReadyForStereo());
  activeView=true;validView=true;currentView={7,100,0,0xabc};auto d=Input();
  CHECK(n::Dispatch(&c,&d.header)==FFX_API_RETURN_OK&&!n::PairReady(7,100));
  currentView.eye=1;CHECK(n::Dispatch(&c,&d.header)==FFX_API_RETURN_OK&&n::PairReady(7,100));
  CHECK(lastError==0x1234u&&n::completedPairs==1);
  CHECK(!logs.empty()&&logs.back().find("D82 FSR_STEREO evaluated epoch=7 frame=100 pairs=1")!=std::string::npos);
  CHECK(dispatched[0]==c&&dispatched[1]!=c&&activeReads==2&&viewReads==2);CHECK(!n::PairReady(6,100)&&!n::PairReady(7,99));
  const auto before=dispatches;validView=false;currentView.frame=101;CHECK(n::Dispatch(&c,&d.header)!=FFX_API_RETURN_OK&&dispatches==before&&!n::PairReady(7,101));
  activeView=false;const auto beforeViews=viewReads;CHECK(n::Dispatch(&c,&d.header)==FFX_API_RETURN_OK&&viewReads==beforeViews&&dispatched.back()==c);
  n::Stop();CHECK(n::ReadyForStereo()&&!n::PairReady(7,100));n::SetEnabled(true);CHECK(n::ReadyForStereo()&&!n::PairReady(7,100));
  activeView=validView=true;currentView={8,102,0,0xdef};CHECK(n::Dispatch(&c,&d.header)==FFX_API_RETURN_OK);currentView.eye=1;CHECK(n::Dispatch(&c,&d.header)==FFX_API_RETURN_OK&&n::PairReady(8,102));
  const auto callsBefore=creates+destroys+dispatches+configures;for(int i=0;i<100;++i)CHECK(n::PairReady(8,102));CHECK(creates+destroys+dispatches+configures==callsBefore);
  ffxConfigureDescHeader conf{};CHECK(n::Configure(&c,&conf)==FFX_API_RETURN_OK&&configures==2);
  CHECK(n::Destroy(&c,nullptr)==FFX_API_RETURN_OK&&destroys==2&&!n::ReadyForStereo());
 }
 std::printf("D82 literal FSR binding %s: %u checks; native pin/module/hook leaves controlled; GPU/headset NOT_RUN\n",scenario.c_str(),checks);
}
