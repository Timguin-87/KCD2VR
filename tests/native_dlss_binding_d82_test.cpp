// Independent review: actual Install body and runtime; controlled OS/hook/SDK
// leaves. The prefix is the existing runtime fixture before its main function.
#include "native_dlss_runtime_host_prefix.inc"
#include <optional>
#include <string>
#include <cwchar>
#include <type_traits>

namespace {
bool modulePresent=true,pinValid=true,exportValid=true,disableFails=false;
int prepareFailure=-1,enableFailure=-1;
unsigned prepareCalls=0,enableCalls=0,disableCalls=0,destroyedHooks=0,pinCalls=0;
std::vector<unsigned> enableOrder;
std::array<bool,5> hookEnabled{};
std::thread racingCreate;
std::atomic<bool> racingStarted{false},racingFinished{false};
bool raceEnabled=false,finishedDuringEnable=false;
void* racingHandle=nullptr;Parameters racingParameters;
std::uint32_t racingResult=0;
using HMODULE=void*;
HMODULE GetModuleHandleW(const wchar_t*) {return modulePresent?reinterpret_cast<void*>(1):nullptr;}
struct ReviewHook {
    unsigned index;
    explicit ReviewHook(unsigned i):index(i){}
    ReviewHook(const ReviewHook&)=delete;ReviewHook& operator=(const ReviewHook&)=delete;
    ~ReviewHook(){++destroyedHooks;}
    bool enable(){
        ++enableCalls;enableOrder.push_back(index);
        CHECK(prepareCalls==5);
        if(int(index)==enableFailure)return false;
        hookEnabled[index]=true;
        if(index==0 && raceEnabled){
            racingCreate=std::thread([]{
                racingStarted=true;
                racingResult=CreateDetour(reinterpret_cast<void*>(0x9000),1,&racingParameters,&racingHandle);
                racingFinished=true;
            });
            while(!racingStarted.load())std::this_thread::yield();
            // The old Install permits completion here and loses that creation.
            // The publication barrier must hold this competing thread until
            // enable returns and the installed state has been published.
            const auto until=std::chrono::steady_clock::now()+std::chrono::milliseconds(80);
            while(!racingFinished.load()&&std::chrono::steady_clock::now()<until)std::this_thread::yield();
            finishedDuringEnable=racingFinished.load();
        }
        return true;
    }
    bool disable(){++disableCalls;if(disableFails)return false;hookEnabled[index]=false;return true;}
};
}
namespace kcd2vr::diagnostic_trace {void Write(const char*) noexcept {}}
namespace kcd2vr::native_dlss_stereo_d82 {namespace {
struct Pin {unsigned index;};
constexpr std::array<Pin,9> kPins{{{0},{1},{2},{3},{4},{5},{6},{7},{8}}};
std::array<std::optional<ReviewHook>,5> g_hooks;
std::mutex g_install_mutex;bool g_attempted=false;
bool ExactModule(HMODULE) noexcept {++pinCalls;return pinValid;}
void* ExactExport(HMODULE,const Pin& p) noexcept {
    if(!exportValid)return nullptr;
    switch(p.index){
    case 5:return reinterpret_cast<void*>(&FakeGetI);
    case 6:return reinterpret_cast<void*>(&FakeSetI);
    case 7:return reinterpret_cast<void*>(&FakeGetF);
    case 8:return reinterpret_cast<void*>(&FakeGetResource);
    default:return reinterpret_cast<void*>(std::uintptr_t(p.index+1));
    }
}
template<class Fn>bool Prepare(unsigned index,void*,Fn,Fn& original){
    ++prepareCalls;if(int(index)==prepareFailure)return false;
    g_hooks[index].emplace(index);
    if constexpr(std::is_same_v<Fn,CreateFn>)original=&FakeCreate;
    else if constexpr(std::is_same_v<Fn,EvaluateFn>)original=index==1?&FakeEvaluate:&FakeEvaluateC;
    else original=index==3?&FakeRelease:&FakeShutdown; // Release and Shutdown have equal ABI.
    return true;
}
}
#include "native_dlss_install_d82_actual.inc"
}

namespace {
std::uint32_t NestedC(void* c,const void* h,void* p,void* cb){return EvaluateDetour(c,h,p,cb);}
std::uint32_t StopDuringEvaluation(void* c,const void* h,void* p,void* cb){
    const auto r=FakeEvaluate(c,h,p,cb);SetEnabled(false);SetEnabled(true);return r;
}
}
int main(int argc,char** argv){
    const std::string scenario=argc>1?argv[1]:"success";
    Reset();g_installed=false;SetEnabled(true);
    if(scenario=="late_module"){
        modulePresent=false;CHECK(!Install()&&!g_attempted&&prepareCalls==0);
        modulePresent=true;CHECK(Install()&&Installed()&&prepareCalls==5);
    }else if(scenario=="wrong_pin"){
        pinValid=false;CHECK(!Install()&&g_attempted&&prepareCalls==0);
        pinValid=true;CHECK(!Install()&&pinCalls==1);
    }else if(scenario=="wrong_export"){
        exportValid=false;CHECK(!Install()&&g_attempted&&prepareCalls==0);
        exportValid=true;CHECK(!Install()&&prepareCalls==0);
    }else if(scenario=="construction"){
        prepareFailure=2;CHECK(!Install()&&g_attempted&&prepareCalls==3&&enableCalls==0);
        CHECK(destroyedHooks==0&&g_hooks[0]&&g_hooks[1]);
        prepareFailure=-1;CHECK(!Install()&&prepareCalls==3);
    }else if(scenario.rfind("enable",0)==0){
        enableFailure=std::stoi(scenario.substr(6));disableFails=true;
        CHECK(!Install()&&g_attempted&&!Installed()&&prepareCalls==5);
        CHECK(disableCalls==5&&destroyedHooks==0);
        for(const auto& hook:g_hooks)CHECK(hook.has_value());
        CHECK(g_create&&g_evaluate&&g_evaluate_c&&g_release&&g_shutdown&&g_get_i&&g_set_i&&g_get_f&&g_get_resource);
        Parameters p;void* h=nullptr;CHECK(CreateDetour(reinterpret_cast<void*>(0x9000),1,&p,&h)==kSuccess&&creates==1);
        CHECK(EvaluateDetour(reinterpret_cast<void*>(0x9000),h,&p,nullptr)==kSuccess&&evaluations.size()==1);
        CHECK(ReleaseDetour(h)==kSuccess&&releases.size()==1);
        CHECK(ShutdownDetour(nullptr)==kSuccess&&!ReadyForStereo());
        CHECK(!Install()&&prepareCalls==5&&destroyedHooks==0);
    }else if(scenario=="race"){
        raceEnabled=true;CHECK(Install());if(racingCreate.joinable())racingCreate.join();
        CHECK(racingFinished&&racingResult==kSuccess&&racingHandle);
        CHECK(!finishedDuringEnable); // CREATION_PUBLICATION_BARRIER
        CHECK(creates==2&&ReadyForStereo()&&GetStatus().livePairs==1);
        PairFrame(racingHandle,racingParameters,55);CHECK(PairReady(7,55));
        CHECK(ReleaseDetour(racingHandle)==kSuccess&&releases.size()==2);
    }else if(scenario=="reentrancy"){
        CHECK(Install());Parameters p;void* h=New(p);
        g_evaluate_c=&NestedC;View(0,10);CHECK(Eval(h,p,true)==kSuccess);
        View(1,10);CHECK(Eval(h,p,true)==kSuccess);CHECK(evaluations.size()==2);
        CHECK(evaluations[0].handle==h&&evaluations[1].handle!=h);
        CHECK(GetStatus().leftEvaluations==1&&GetStatus().rightEvaluations==1&&GetStatus().pairs==1&&g_native_depth==0);
        CHECK(PairReady(7,10));
        View(0,11);CHECK(Eval(h,p)==kSuccess);View(1,11);g_evaluate=&StopDuringEvaluation;
        CHECK(Eval(h,p)==kSuccess);CHECK(!PairReady(7,11)&&!PairReady(7,10)&&g_native_depth==0);
        g_evaluate=&FakeEvaluate;PairFrame(h,p,12);CHECK(PairReady(7,12));
        CHECK(evaluations[evaluations.size()-2].reset==1&&evaluations.back().reset==1);
    }else{
        CHECK(Install()&&Installed());CHECK((enableOrder==std::vector<unsigned>{3,4,1,2,0}));
        CHECK(Install()&&prepareCalls==5&&enableCalls==5&&pinCalls==1);
        Parameters p;void* h=New(p);CHECK(creates==2&&ReadyForStereo());PairFrame(h,p,10);CHECK(PairReady(7,10));
        CHECK(ReleaseDetour(h)==kSuccess&&!ReadyForStereo()&&releases.size()==2);
    }
    std::printf("D82 actual DLSS binding %s checks=%d failures=%d OS/hooks/SDK leaves controlled GPU/headset NOT_RUN\n",scenario.c_str(),checks,failures);
    return failures?1:0;
}
