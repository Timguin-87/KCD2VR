#include "native_dlss_stereo_d82.h"
#include "native_stereo_preflight_v6602.h"
#include <array>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <mutex>
#include <thread>
#include <vector>
#include <limits>
#define __fastcall

namespace kcd2vr::native_stereo_preflight {
thread_local UpscalerView testView{};
thread_local bool testScoped=false,testValid=true;
bool InUpscalerView() noexcept {return testScoped;}
bool CurrentUpscalerView(UpscalerView& out) noexcept {out=testView;return testScoped&&testValid;}
}
namespace kcd2vr::native_dlss_stereo_d82 { namespace {
unsigned testError=0;
unsigned SaveNativeError() noexcept {return testError;}
void RestoreNativeError(unsigned v) noexcept {testError=v;}
void* ReadHandle(void** p) noexcept {return p?*p:nullptr;}
std::uintptr_t CommandDevice(void* command) noexcept {return reinterpret_cast<std::uintptr_t>(command);}
unsigned reports=0;
void ReportFailure(Fault,const char*) noexcept {++reports;}
struct ResetCutReceipt {std::uint64_t epoch{},cuts{};std::uint32_t frame{},leftFrame{},rightFrame{};
    int leftNative{},rightNative{};bool leftEffective{},rightEffective{};};
ResetCutReceipt lastResetCut{};
void ReportResetCut(std::uint64_t epoch,std::uint32_t frame,std::uint32_t leftFrame,
    std::uint32_t rightFrame,int leftNative,int rightNative,bool leftEffective,
    bool rightEffective,std::uint64_t cuts) noexcept {
    lastResetCut={epoch,cuts,frame,leftFrame,rightFrame,leftNative,rightNative,leftEffective,rightEffective};
}
void ReportPairCreated(const void*,const void*) noexcept {}
void ReportPairEvaluated(std::uint64_t,std::uint32_t,const void*,const void*,std::uint64_t) noexcept {}
void ReportEvaluationInputs(unsigned,std::uint64_t,std::uint32_t,void*,int,bool) noexcept {}
#include "native_dlss_stereo_d82_runtime.inl"
} // namespace
// Extracted byte-for-byte from the production .cpp by the runner. No duplicate
// implementation of readiness, mode transitions, or pair acceptance lives here.
#include "native_dlss_state_d82_actual.inc"
} // namespace

using namespace kcd2vr::native_dlss_stereo_d82;
namespace scope=kcd2vr::native_stereo_preflight;
namespace {
int failures=0,checks=0;
void Check(bool value,const char* expression,int line) {
    ++checks;if(!value){++failures;std::fprintf(stderr,"FAIL line %d: %s\n",line,expression);}
}
#define CHECK(x) Check(bool(x),#x,__LINE__)
struct Parameters {
    int reset=0;
    std::array<void*,4> resources{reinterpret_cast<void*>(11),reinterpret_cast<void*>(12),reinterpret_cast<void*>(13),reinterpret_cast<void*>(14)};
    std::array<float,4> values{.125f,-.25f,1280.f,720.f};
    int userToken=91823; // untouched input/dictionary marker
    bool badRead=false,denyResetWrite=false,denyResetRestore=false;
};
struct Evaluation {void* command;const void* handle;void* parameters;void* callback;int reset;int api;};
std::vector<Evaluation> evaluations;
std::vector<void*> releases;
std::vector<void*> creationParameters,creationCommands;
std::array<int,64> handles{};
unsigned creates=0;
bool aliasTwin=false,failTwin=false,failFirst=false,evalFails=false;
std::uint32_t FakeCreate(void* c,int,void* p,void** out) {
    ++creates;creationParameters.push_back(p);creationCommands.push_back(c);testError=1000+creates;
    *out=&handles.at(creates-1);
    if(creates==2&&aliasTwin)*out=&handles[0];
    if((creates==2&&failTwin)||(creates==1&&failFirst))return 0xBAD00002;
    return kSuccess;
}
std::uint32_t FakeEvaluate(void* c,const void* h,void* p,void* cb) {
    auto& params=*static_cast<Parameters*>(p);evaluations.push_back({c,h,p,cb,params.reset,0});testError=7001;
    return evalFails?0xBAD00009:kSuccess;
}
std::uint32_t FakeEvaluateC(void* c,const void* h,void* p,void* cb) {
    const auto result=FakeEvaluate(c,h,p,cb);evaluations.back().api=1;return result;
}
std::uint32_t FakeRelease(void* h) {releases.push_back(h);return kSuccess;}
std::uint32_t FakeShutdown(void*) {return kSuccess;}
std::uint32_t FakeGetI(void* ptr,const char* key,int* out) {
    auto& p=*static_cast<Parameters*>(ptr);if(p.badRead||std::strcmp(key,"Reset"))return kFailure;
    *out=p.reset;return kSuccess;
}
void FakeSetI(void* ptr,const char*,int value) {
    auto& p=*static_cast<Parameters*>(ptr);
    if((value==1&&p.denyResetWrite)||(value==0&&p.denyResetRestore))return;
    p.reset=value;
}
std::uint32_t FakeGetF(void* ptr,const char* key,float* out) {
    auto& p=*static_cast<Parameters*>(ptr);const char* names[]{"Jitter.Offset.X","Jitter.Offset.Y","MV.Scale.X","MV.Scale.Y"};
    for(unsigned i=0;i<4;++i)if(!std::strcmp(names[i],key)){*out=p.values[i];return kSuccess;}
    return kFailure;
}
std::uint32_t FakeGetResource(void* ptr,const char* key,void** out) {
    auto& p=*static_cast<Parameters*>(ptr);const char* names[]{"Color","Output","Depth","MotionVectors"};
    for(unsigned i=0;i<4;++i)if(!std::strcmp(names[i],key)){*out=p.resources[i];return kSuccess;}
    return kFailure;
}
void Reset() {
    g_pairs={};for(auto& r:g_receipts)ClearReceipt(r);g_receipt_next=0;g_receipt_serial=0;g_next_identity=0;
    g_stats={};g_fault=Fault::None;g_enabled=false;g_installed=true;g_live_pairs=0;g_generation=1;g_native_depth=0;
    g_create=&FakeCreate;g_evaluate=&FakeEvaluate;g_evaluate_c=&FakeEvaluateC;g_release=&FakeRelease;g_shutdown=&FakeShutdown;
    g_get_i=&FakeGetI;g_set_i=&FakeSetI;g_get_f=&FakeGetF;g_get_resource=&FakeGetResource;
    evaluations.clear();releases.clear();creationParameters.clear();creationCommands.clear();creates=0;
    aliasTwin=failTwin=failFirst=evalFails=false;reports=0;testError=0;scope::testScoped=false;scope::testValid=true;scope::testView={};
}
void View(unsigned eye,std::uint32_t frame,std::uint64_t epoch=7,std::uintptr_t pipeline=0x1234) {
    scope::testScoped=true;scope::testValid=true;scope::testView={epoch,frame,eye,pipeline};
}
void* New(Parameters& p,int feature=1,void* command=reinterpret_cast<void*>(0x9000)) {
    void* h=nullptr;CHECK(CreateDetour(command,feature,&p,&h)==kSuccess);return h;
}
std::uint32_t Eval(void* h,Parameters& p,bool c=false) {
    return c?EvaluateCDetour(reinterpret_cast<void*>(0x9000),h,&p,reinterpret_cast<void*>(0x9001)):
        EvaluateDetour(reinterpret_cast<void*>(0x9000),h,&p,reinterpret_cast<void*>(0x9001));
}
void PairFrame(void* h,Parameters& p,std::uint32_t frame,std::uint64_t epoch=7) {
    View(0,frame,epoch);CHECK(Eval(h,p)==kSuccess);View(1,frame,epoch);CHECK(Eval(h,p,true)==kSuccess);
}
}
int main() {
    Reset();Parameters p;void* h=New(p);View(1,10);CHECK(Eval(h,p)==kSuccess);CHECK(creates==1);CHECK(evaluations.back().handle==h);CHECK(!ReadyForStereo());
    Reset();SetEnabled(true);p={};h=New(p,9);View(9,10);scope::testValid=false;CHECK(Eval(h,p)==kSuccess);CHECK(!Faulted());CHECK(creates==1);

    Reset();SetEnabled(true);p={};h=New(p);CHECK(h==&handles[0]);CHECK(creates==2);CHECK(testError==1001);
    CHECK(creationParameters[0]==&p&&creationParameters[1]==&p);CHECK(creationCommands[0]==creationCommands[1]);CHECK(ReadyForStereo());
    PairFrame(h,p,100);CHECK(evaluations.size()==2);CHECK(evaluations[0].handle==h);CHECK(evaluations[1].handle==&handles[1]);
    CHECK(evaluations[0].reset==1&&evaluations[1].reset==1);CHECK(evaluations[0].api==0&&evaluations[1].api==1);
    CHECK(evaluations[0].parameters==&p&&evaluations[1].parameters==&p);CHECK(evaluations[1].callback==reinterpret_cast<void*>(0x9001));
    CHECK(p.reset==0&&p.userToken==91823&&p.values[0]==.125f&&p.resources[2]==reinterpret_cast<void*>(13));CHECK(testError==7001);
    CHECK(PairReady(7,100));CHECK(!PairReady(7,99));CHECK(!PairReady(8,100));
    PairFrame(h,p,101);CHECK(evaluations[2].reset==0&&evaluations[3].reset==0);CHECK(PairReady(7,100));CHECK(PairReady(7,101));
    p.reset=1;View(0,102);CHECK(Eval(h,p)==kSuccess);p.reset=0;View(1,102);CHECK(Eval(h,p)==kSuccess);
    CHECK(evaluations[4].reset==1&&evaluations[5].reset==1);CHECK(p.reset==0);
    PairFrame(h,p,108);CHECK(evaluations[6].reset==1&&evaluations[7].reset==1);
    PairFrame(h,p,1,8);CHECK(evaluations[8].reset==1&&evaluations[9].reset==1);
    scope::testScoped=false;CHECK(Eval(h,p)==kSuccess);CHECK(evaluations.back().handle==h);PairFrame(h,p,2,8);
    CHECK(evaluations[evaluations.size()-2].reset==1&&evaluations.back().reset==1);
    SetEnabled(false);CHECK(ReadyForStereo());CHECK(!PairReady(8,2));CHECK(releases.empty());SetEnabled(true);PairFrame(h,p,3,8);CHECK(PairReady(8,3));
    CHECK(ReleaseDetour(h)==kSuccess);CHECK(releases.size()==2);CHECK(releases[0]==&handles[1]&&releases[1]==&handles[0]);CHECK(!ReadyForStereo());CHECK(!PairReady(8,3));

    Reset();SetEnabled(true);p={};View(0,10);CHECK(Eval(&handles[40],p)==kFailure);CHECK(evaluations.empty());CHECK(GetStatus().fault==Fault::MissingCreate);
    Reset();SetEnabled(true);p={};aliasTwin=true;h=New(p);CHECK(GetStatus().fault==Fault::AliasedHandle);CHECK(releases.empty());CHECK(!ReadyForStereo());CHECK(h==&handles[0]);
    Reset();SetEnabled(true);p={};failTwin=true;h=New(p);CHECK(GetStatus().fault==Fault::TwinCreate);CHECK(releases.size()==1&&releases[0]==&handles[1]);CHECK(h==&handles[0]);
    Reset();SetEnabled(true);p={};failFirst=true;void* failed=nullptr;CHECK(CreateDetour(reinterpret_cast<void*>(0x9000),1,&p,&failed)==0xBAD00002);CHECK(creates==1);CHECK(!ReadyForStereo());

    Reset();SetEnabled(true);p={};h=New(p);View(1,10);CHECK(Eval(h,p)==kFailure);CHECK(evaluations.empty());CHECK(GetStatus().fault==Fault::MissingLeft);
    Reset();SetEnabled(true);p={};h=New(p);View(0,10);CHECK(Eval(h,p)==kSuccess);CHECK(Eval(h,p)==kFailure);CHECK(evaluations.size()==1);CHECK(GetStatus().fault==Fault::DuplicateEye);
    Reset();SetEnabled(true);p={};h=New(p);PairFrame(h,p,10);CHECK(Eval(h,p)==kFailure);CHECK(!PairReady(7,10));CHECK(evaluations.size()==2);
    Reset();SetEnabled(true);p={};h=New(p);View(0,10);scope::testValid=false;CHECK(Eval(h,p)==kFailure);CHECK(evaluations.empty());CHECK(GetStatus().fault==Fault::ViewIdentity);
    Reset();SetEnabled(true);p={};h=New(p);p.badRead=true;View(0,10);CHECK(Eval(h,p)==kFailure);CHECK(evaluations.empty());CHECK(GetStatus().fault==Fault::Parameters);
    Reset();SetEnabled(true);p={};h=New(p);p.resources[2]=nullptr;View(0,10);CHECK(Eval(h,p)==kFailure);CHECK(evaluations.empty());
    Reset();SetEnabled(true);p={};h=New(p);p.values[0]=std::numeric_limits<float>::quiet_NaN();View(0,10);CHECK(Eval(h,p)==kFailure);CHECK(evaluations.empty());
    Reset();SetEnabled(true);p={};h=New(p);p.denyResetWrite=true;View(0,10);CHECK(Eval(h,p)==kFailure);CHECK(evaluations.empty());CHECK(p.reset==0);
    Reset();SetEnabled(true);p={};h=New(p);p.denyResetRestore=true;View(0,10);CHECK(Eval(h,p)==kFailure);CHECK(evaluations.size()==1);CHECK(GetStatus().fault==Fault::RestoreReset);
    Reset();SetEnabled(true);p={};h=New(p);View(0,10);CHECK(Eval(h,p)==kSuccess);View(1,10);evalFails=true;CHECK(Eval(h,p)==0xBAD00009);CHECK(p.reset==0);CHECK(!PairReady(7,10));CHECK(GetStatus().fault==Fault::Evaluate);

    // A new pipeline may reuse the same native feature handle, but it owns a
    // different camera history. Reset both eyes when LEFT opens that view.
    Reset();SetEnabled(true);p={};h=New(p);PairFrame(h,p,10);
    View(0,11,7,0x5678);CHECK(Eval(h,p)==kSuccess);View(1,11,7,0x5678);CHECK(Eval(h,p)==kSuccess);
    CHECK(evaluations[2].reset==1&&evaluations[3].reset==1);CHECK(PairReady(7,11));
    View(0,12,7,0x5678);CHECK(Eval(h,p)==kSuccess);View(1,12,7,0x5678);CHECK(Eval(h,p)==kSuccess);
    CHECK(evaluations[4].reset==0&&evaluations[5].reset==0);
    Reset();SetEnabled(true);p={};h=New(p);View(0,10);CHECK(Eval(h,p)==kSuccess);
    View(1,10,7,0x5678);CHECK(Eval(h,p)==kFailure);CHECK(evaluations.size()==1);
    CHECK(GetStatus().fault==Fault::ViewIdentity);CHECK(!PairReady(7,10));

    // A native reset arriving first at RIGHT must withhold that pair instead
    // of mixing a continuous LEFT history with a freshly reset RIGHT history.
    Reset();SetEnabled(true);p={};h=New(p);PairFrame(h,p,10);
    View(0,11);CHECK(Eval(h,p)==kSuccess);p.reset=1;View(1,11);CHECK(Eval(h,p)==kFailure);
    CHECK(evaluations.size()==3);CHECK(evaluations.back().reset==0);CHECK(p.reset==1);
    CHECK(GetStatus().fault==Fault::None);CHECK(GetStatus().resetCuts==1);CHECK(!PairReady(7,11));
    CHECK(lastResetCut.epoch==7&&lastResetCut.frame==11&&lastResetCut.leftFrame==11&&lastResetCut.rightFrame==10);
    CHECK(lastResetCut.leftNative==0&&lastResetCut.rightNative==1);
    CHECK(!lastResetCut.leftEffective&&lastResetCut.rightEffective&&lastResetCut.cuts==1);
    CHECK(!PairReady(7,10));CHECK(ReadyForStereo());
    // A right-only camera cut must NOT latch a permanent SDK fault. The next
    // pair resets both actual SDK calls, restores native Reset, then resumes
    // continuous history on the following pair. Do not "fix" this by accepting
    // the asymmetrically reset current pair.
    p.reset=0;PairFrame(h,p,12);
    CHECK(PairReady(7,12));CHECK(!PairReady(7,11));CHECK(!Faulted());
    CHECK(evaluations.size()==5);CHECK(evaluations[3].reset==1&&evaluations[4].reset==1);
    CHECK(evaluations[3].handle==h&&evaluations[4].handle==&handles[1]);CHECK(p.reset==0);
    PairFrame(h,p,13);CHECK(evaluations[5].reset==0&&evaluations[6].reset==0);
    CHECK(PairReady(7,13));CHECK(GetStatus().resetCuts==1);
    // Both eyes being reset is valid, including a native RIGHT reset on the
    // initial frame where the bridge already reset LEFT.
    Reset();SetEnabled(true);p={};h=New(p);View(0,10);CHECK(Eval(h,p)==kSuccess);
    p.reset=1;View(1,10);CHECK(Eval(h,p)==kSuccess);CHECK(PairReady(7,10));
    CHECK(evaluations[0].reset==1&&evaluations[1].reset==1);CHECK(p.reset==1);

    // If RIGHT never executes, LEFT must inspect both eyes' previous frames
    // before the next pair. The following frame recovers with two resets.
    Reset();SetEnabled(true);p={};h=New(p);PairFrame(h,p,10);
    View(0,11);CHECK(Eval(h,p)==kSuccess);CHECK(!PairReady(7,11));
    PairFrame(h,p,12);CHECK(evaluations[3].reset==1&&evaluations[4].reset==1);CHECK(PairReady(7,12));
    CHECK(!Faulted());PairFrame(h,p,13);CHECK(evaluations[5].reset==0&&evaluations[6].reset==0);
    Reset();SetEnabled(true);p={};h=New(p);View(0,10);CHECK(Eval(h,p)==kSuccess);
    PairFrame(h,p,11);CHECK(evaluations[1].reset==1&&evaluations[2].reset==1);CHECK(PairReady(7,11));

    Reset();SetEnabled(true);p={};h=New(p);PairFrame(h,p,0xffffffffu);PairFrame(h,p,0);CHECK(evaluations[2].reset==0&&evaluations[3].reset==0);CHECK(PairReady(7,0));
    Reset();SetEnabled(true);p={};h=New(p);for(unsigned frame=0;frame<24;++frame)PairFrame(h,p,frame);
    CHECK(!PairReady(7,7));for(unsigned frame=8;frame<24;++frame)CHECK(PairReady(7,frame));
    // Delayed queue publication must read receipts without acquiring the SDK
    // registry mutex. This exercises the real public PairReady implementation.
    std::atomic<bool> held=false,readerDone=false,lockfree=false;
    std::thread holder([&]{std::lock_guard<std::recursive_mutex> lock(g_mutex);held=true;
        const auto end=std::chrono::steady_clock::now()+std::chrono::seconds(2);
        while(!readerDone&&std::chrono::steady_clock::now()<end)std::this_thread::yield();
        lockfree=readerDone.load();});
    while(!held)std::this_thread::yield();
    CHECK(PairReady(7,23));readerDone=true;holder.join();CHECK(lockfree);
    CHECK(ShutdownDetour(reinterpret_cast<void*>(0x9000))==kSuccess);CHECK(!ReadyForStereo());CHECK(releases.empty());CHECK(!PairReady(7,23));
    Reset();SetEnabled(true);p={};New(p,1,reinterpret_cast<void*>(0x9000));New(p,1,reinterpret_cast<void*>(0x9002));CHECK(GetStatus().livePairs==2);
    CHECK(ShutdownDetour(reinterpret_cast<void*>(0x9000))==kSuccess);CHECK(GetStatus().livePairs==1);CHECK(ShutdownDetour(nullptr)==kSuccess);CHECK(GetStatus().livePairs==0);
    Reset();SetEnabled(true);p={};for(int i=0;i<9;++i)New(p);CHECK(creates==17);CHECK(GetStatus().livePairs==8);CHECK(GetStatus().fault==Fault::Capacity);
    std::printf("DLSS production routing checks=%d failures=%d\n",checks,failures);return failures?1:0;
}
