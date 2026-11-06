// Compile the supplied Claude implementation literally with controlled Win32/SDK
// leaves. These probes confirm its defects; they do not execute vendor shaders.
#include "claude_upscaler_actual.cpp"
#include <array>
#include <iostream>
#include <vector>

using namespace kcd2vr::native_upscaler_stereo_d82;
namespace {
struct Call { void* context; unsigned reset; };
std::vector<Call> calls;
unsigned checks=0;
void* left=reinterpret_cast<void*>(0x110000);
void* right=reinterpret_cast<void*>(0x220000);
int creates=0;
bool failTwin=false;
int evalReturn=0;
struct DlssParams { unsigned char pad[0x38]{}; int reset=0; };
struct FfxParams { FfxHeader header{kFfxDispatchUpscale,nullptr}; unsigned char pad[0x198-sizeof(FfxHeader)]{}; bool reset=false; };
struct XessParams { unsigned char pad[0x3c]{}; unsigned reset=0; };
static_assert(offsetof(DlssParams,reset)==0x38);
static_assert(offsetof(FfxParams,reset)==0x198);
static_assert(offsetof(XessParams,reset)==0x3c);

void check(bool value,const char* why) {
    ++checks;
    if(!value){std::cerr<<"FAILED "<<why<<"\n";std::exit(1);}
}
int dlssCreate(void*,unsigned,unsigned,void** out,void*,void*) {
    ++creates;
    if(creates==2&&failTwin){*out=nullptr;return static_cast<int>(0xBAD00001u);}
    *out=creates==1?left:right;return 1;
}
int dlssEval(void*,void* ctx,void*,void* params) {
    calls.push_back({ctx,static_cast<unsigned>(static_cast<DlssParams*>(params)->reset)});
    return evalReturn;
}
int dlssRelease(void*){return 1;}
std::uint32_t fsrCreate(void** out,FfxHeader*,const void*) {
    ++creates;
    if(creates==2&&failTwin){*out=nullptr;return 1;}
    *out=creates==1?left:right;return 0;
}
std::uint32_t fsrDispatch(void** ctx,const FfxHeader* desc) {
    calls.push_back({*ctx,static_cast<unsigned>(reinterpret_cast<const FfxParams*>(desc)->reset)});
    return static_cast<std::uint32_t>(evalReturn);
}
std::uint32_t fsrDestroy(void** ctx,const void*){*ctx=nullptr;return 0;}
int xessCreate(void*,void** out) {
    ++creates;
    if(creates==2&&failTwin){*out=nullptr;return 1;}
    *out=creates==1?left:right;return 0;
}
int xessInit(void*,const void*){return 0;}
int xessExecute(void* ctx,void*,const void* params) {
    calls.push_back({ctx,static_cast<const XessParams*>(params)->reset});return evalReturn;
}
int xessDestroy(void*){return 0;}

void reset(int tech,bool twinFail=false) {
    for(auto& group:g_pairs)for(auto& p:group)p=Pair{};
    creates=0;failTwin=twinFail;evalReturn=tech==kDlss?1:0;calls.clear();t_eye=-1;
    g_dlssCreate=&dlssCreate;g_dlssEval=&dlssEval;g_ngxRelease=&dlssRelease;
    g_iatOriginal[kFfxCreate]=reinterpret_cast<void*>(&fsrCreate);
    g_iatOriginal[kFfxDispatch]=reinterpret_cast<void*>(&fsrDispatch);
    g_iatOriginal[kFfxDestroy]=reinterpret_cast<void*>(&fsrDestroy);
    g_iatOriginal[kXessCreate]=reinterpret_cast<void*>(&xessCreate);
    g_iatOriginal[kXessInit]=reinterpret_cast<void*>(&xessInit);
    g_iatOriginal[kXessExecute]=reinterpret_cast<void*>(&xessExecute);
    g_iatOriginal[kXessDestroy]=reinterpret_cast<void*>(&xessDestroy);
    void* handle=nullptr;
    if(tech==kDlss)check(DlssCreateDetour(nullptr,0,0,&handle,nullptr,nullptr)==1,"original DLSS create preserved");
    if(tech==kFfx){FfxHeader header{kFfxCreateUpscale,nullptr};check(FfxCreateHook(&handle,&header,nullptr)==0,"original FFX create preserved");}
    if(tech==kXess){check(XessCreateHook(nullptr,&handle)==0,"original XeSS create preserved");check(XessInitHook(handle,nullptr)==0,"original XeSS init preserved");}
    check(handle==left,"native original handle unchanged");
}
void evaluate(int tech,int eye,unsigned resetValue=0) {
    t_eye=eye;
    if(tech==kDlss){DlssParams params{};params.reset=static_cast<int>(resetValue);(void)DlssEvalDetour(nullptr,left,nullptr,&params);check(params.reset==static_cast<int>(resetValue),"DLSS parameter restored");}
    if(tech==kFfx){FfxParams params{};params.reset=resetValue!=0;void* ctx=left;(void)FfxDispatchHook(&ctx,&params.header);check(params.reset==(resetValue!=0),"FFX parameter restored");}
    if(tech==kXess){XessParams params{};params.reset=resetValue;(void)XessExecuteHook(left,nullptr,&params);check(params.reset==resetValue,"XeSS parameter restored");}
}
}

int main(){
    for(int tech=0;tech<kTechCount;++tech){
        const char* name=kTechName[tech];
        reset(tech,true);evaluate(tech,1);
        check(calls.size()==1&&calls[0].context==left,"demonstrated RIGHT falls through to LEFT after twin failure");
        std::cout<<name<<" missing_twin right_uses_left=1\n";

        reset(tech);evaluate(tech,0,1);evaluate(tech,1,1);evaluate(tech,0,0);evaluate(tech,1,0);
        check(calls.size()==4&&calls[2].reset==1&&calls[3].reset==0,"demonstrated next-pair asymmetric reset after bilateral native reset");
        std::cout<<name<<" native_reset_both next_pair_resets="<<calls[2].reset<<","<<calls[3].reset<<"\n";

        reset(tech);evaluate(tech,0);evaluate(tech,1);calls.clear();
        evaluate(tech,0); // RIGHT never evaluates for this logical frame.
        evaluate(tech,0);evaluate(tech,1);
        check(calls.size()==3&&calls[1].reset==0&&calls[2].reset==0,"demonstrated missing RIGHT does not reset next pair");
        std::cout<<name<<" skipped_right next_pair_resets=0,0\n";

        reset(tech);evaluate(tech,0);evaluate(tech,1);calls.clear();
        evaluate(tech,-1);evaluate(tech,0);evaluate(tech,1);
        check(calls.size()==3&&calls[0].context==left&&calls[1].reset==0&&calls[2].reset==0,"demonstrated out-of-eye evaluation advances LEFT without invalidating pair");
        std::cout<<name<<" unscoped_eval uses_left=1 next_pair_resets=0,0\n";

        reset(tech);evaluate(tech,0);evaluate(tech,1);calls.clear();evaluate(tech,1);evaluate(tech,1);
        check(calls.size()==2&&calls[0].context==right&&calls[1].context==right,"demonstrated duplicate RIGHT advances twice without a frame check");
        std::cout<<name<<" duplicate_right sdk_evaluations=2\n";

        reset(tech);evaluate(tech,0);evaluate(tech,1);calls.clear();
        evalReturn=tech==kDlss?static_cast<int>(0xBAD00001u):1;
        evaluate(tech,0);evalReturn=tech==kDlss?1:0;evaluate(tech,1);evaluate(tech,0);evaluate(tech,1);
        check(calls.size()==4&&calls[2].reset==0&&calls[3].reset==0,"demonstrated failed evaluation leaves next pair unreset");
        std::cout<<name<<" failed_eval next_pair_resets=0,0\n";
    }
    std::cout<<"PASS "<<checks<<" checks; 18 literal-source defect probes; Windows/SDK leaves controlled; GPU/headset NOT_RUN\n";
}
