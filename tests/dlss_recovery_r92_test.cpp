// Runs the real NGX router, aggregate mode guard and complete native eye-control
// completion function. Only OS, native engine and vendor leaves are substituted.
#define main ExistingDlssRoutingChecks
#include "native_dlss_stereo_d82_test.cpp"
#undef main
#include "native_upscaler_stereo_d82.h"
#include "stereo_eye_control_model_v6604.h"
#include "fsr_stereo_runtime_d82.h"
namespace fsr_fixture {
using namespace kcd2vr::fsr_stereo_runtime_d82;
unsigned next=0;ffxContext context=nullptr;std::vector<bool> resets;bool fail=false;
ffxReturnCode_t Create(ffxContext* c,ffxCreateContextDescHeader*,const ffxAllocationCallbacks*){*c=reinterpret_cast<void*>(++next);return 0;}
ffxReturnCode_t Destroy(ffxContext* c,const ffxAllocationCallbacks*){*c=nullptr;return 0;}
ffxReturnCode_t Dispatch(ffxContext*,const ffxDispatchDescHeader* h){
 if(fail)return FFX_API_RETURN_ERROR_RUNTIME_ERROR;
 if(h->type==FFX_API_DISPATCH_DESC_TYPE_UPSCALE)resets.push_back(reinterpret_cast<const ffxDispatchDescUpscale*>(h)->reset);
 return 0;
}
ffxReturnCode_t Configure(ffxContext*,const ffxConfigureDescHeader*){return 0;}
Runtime runtime({Create,Destroy,Dispatch,Configure});
ffxReturnCode_t Eval(unsigned eye,unsigned frame,unsigned epoch,std::uintptr_t pipeline,bool reset){
 ffxDispatchDescUpscale d{};d.header.type=FFX_API_DISPATCH_DESC_TYPE_UPSCALE;
 d.commandList=reinterpret_cast<void*>(1);d.color.resource=reinterpret_cast<void*>(2);
 d.depth.resource=reinterpret_cast<void*>(3);d.motionVectors.resource=reinterpret_cast<void*>(4);
 d.output.resource=reinterpret_cast<void*>(5);d.renderSize={100,100};d.preExposure=1;d.reset=reset;
 return runtime.Dispatch(&context,&d.header,{true,true,eye,epoch,frame,pipeline});
}
void Setup(){
 if(context)CHECK(runtime.Destroy(&context,nullptr)==0);
 runtime.SetEnabled(false);runtime.SetEnabled(true);ffxCreateContextDescUpscale d{};d.header.type=FFX_API_CREATE_CONTEXT_DESC_TYPE_UPSCALE;
 CHECK(runtime.Create(&context,&d.header,nullptr)==0);resets.clear();fail=false;
 CHECK(Eval(0,10,7,0x1234,false)==0);CHECK(Eval(1,10,7,0x1234,false)==0);
 CHECK(Eval(0,11,7,0x1234,false)==0);CHECK(!resets.back());
}
}
namespace kcd2vr::native_fsr_stereo_d82 {
void SetEnabled(bool on) noexcept {fsr_fixture::runtime.SetEnabled(on);}
bool PairReady(std::uint64_t epoch,std::uint32_t frame) noexcept {return fsr_fixture::runtime.PairReady(epoch,frame);}
bool RecoverableResetCut(std::uint64_t epoch,std::uint32_t frame,std::uintptr_t pipeline) noexcept {
 return fsr_fixture::runtime.RecoverableResetCut(epoch,frame,pipeline);
}
}
namespace kcd2vr::native_xess_stereo_d85 {
void SetEnabled(bool) noexcept {}
bool PairReady(std::uint64_t,std::uint32_t) noexcept {return false;}
}
#include "dlss_aggregate_r92_actual.inc"
namespace aggregate=kcd2vr::native_upscaler_stereo_d82;
namespace kcd2vr::native_svo_right_v6654 {void OnRightPassEnd() {}}
namespace kcd2vr::native_stereo_outer_lab {
bool V6638InterEyeRenderBeginReset(){return false;}
bool RunV6638InterEyeRenderBeginReset(){return false;}
}
namespace scheduler {
namespace native_upscaler_stereo_d82=kcd2vr::native_upscaler_stereo_d82;
namespace native_stereo_outer_lab=kcd2vr::native_stereo_outer_lab;
int g_read=0;std::uintptr_t g_wh=0x100000;
bool restoreOK=true,builtOK=true,readOK=true,normalResourceOK=true;
unsigned selectorValue=0,phaseValue=2,scheduleValue=0;
unsigned char resultByte=17;
namespace model {
struct Snapshot {std::uintptr_t crv=0x2000;std::uint32_t frame=11;};
Snapshot snapshot;
Snapshot Capture(int,std::uintptr_t,std::uintptr_t){return snapshot;}
template<class T> bool Read(int,std::uintptr_t,std::uintptr_t offset,T& out){
 if(!readOK)return false;
 if(offset==0xF2)out=static_cast<T>(resultByte);
 else if(offset==0x8EE0)out=static_cast<T>(selectorValue);
 else if(offset==0x1974)out=static_cast<T>(phaseValue);
 else if(offset==0x1A0)out=static_cast<T>(scheduleValue);
 else return false;
 return true;
}
}
namespace rm {
struct Evidence {struct {std::uintptr_t renderer=0x3000;} rt;};
Evidence CaptureEvidence(int,model::Snapshot,std::uintptr_t,std::uint32_t){return {};}
}
namespace ec {
using Work=kcd2vr::eye_control_model::Work;
using CameraSet=kcd2vr::eye_control_model::CameraSet;
using kcd2vr::eye_control_model::NormalComplete;
constexpr unsigned kMinimumCompletedSamples=17;
const char* RejectPersistentResources(model::Snapshot,rm::Evidence,std::uintptr_t,unsigned){return normalResourceOK?nullptr:"fixture-resource-refusal";}
bool CheckBuilt(int,std::uintptr_t,unsigned,std::uint64_t&){return builtOK;}
}
#include "dlss_eye_types_r92_actual.inc"
enum State:int {Idle,Armed,Running,Done,Cancelled,TimedOut,Faulted,Refused,Preparing,Allocating,Witnessing};
std::atomic<int> g_state{Running},g_run_kind{5},g_first_fault{0},g_last_design_stage_passed{0};
std::atomic<bool> g_eye_decision_used{false},g_last_run_passed{false},g_eye1_verified{false},g_velocity_resize_failed{false};
std::atomic<unsigned> g_samples{17},g_pipeline_inflight{1},g_inflight{0},g_eye_control_attempts{0},g_eye_outcome{0},g_f11_outcome{0};
std::atomic<unsigned> g_eye_control_calls{0},g_eye_control_returns{0},g_eye_gbuffer_returns{0},g_eye_opaque_returns{0},g_eye_nonempty_requests{0};
std::atomic<unsigned> g_repeat_completed{0},g_repeat_remaining{30};
std::atomic<std::uint64_t> g_design_serial_counter{0},g_design_pair_serial{0};
constexpr unsigned kStereoBurstFrames=30;
std::atomic<int> g_builder_original{0};
unsigned restoredBuilderCalls=0,rightPipelineCalls=0,discardLogs=0;
EyeFlow* current=nullptr;void* activeHandle=nullptr;Parameters params;
int rightReset=1,changeTechnology=-1;
bool injectFallback=false,injectMissingWork=false,injectFault=false,injectCancelled=false;
unsigned long GetCurrentThreadId(){return 1;}
void PrintWork(const EyeFlow&,const char*,const ec::Work&) {}
const char* EyeOutcomeName(unsigned){return "fixture";}
template<class... A> void Log(const char* label,A...){if(std::strstr(label,"FRAME-DISCARDED"))++discardLogs;}
bool RestoreCameraBanks(EyeFlow&){return restoreOK;}
void StoreSelector(std::uintptr_t,unsigned value){selectorValue=value;}
void StoreRendererResult(std::uintptr_t,unsigned char value){resultByte=value;}
void NativeOne(int,void*,unsigned){++restoredBuilderCalls;}
ec::Work CompleteWork(){ec::Work w;w.builds=w.buildReturns=w.standard=w.standardReturns=w.gbuffer=w.gbufferReturns=w.opaque=w.opaqueReturns=w.nonempty=1;
 w.cameraSourceValid=w.builtPointerValid=w.standardStateValid=true;return w;}
void NativePipeline(void*,void*) {
 ++rightPipelineCalls;current->selected=true;current->eye=CompleteWork();
 if(aggregate::ActualTechnology()==2){View(1,current->frame,current->epoch,current->pipeline);params.reset=rightReset;(void)Eval(activeHandle,params);}
 if(aggregate::ActualTechnology()==1)(void)fsr_fixture::Eval(1,current->frame,unsigned(current->epoch),current->pipeline,rightReset!=0);
 if(injectFallback)current->fallback=true;
 if(injectMissingWork)current->eye.opaqueReturns=0;
 if(injectFault)g_first_fault=1;
 if(injectCancelled)g_state=Cancelled;
 if(changeTechnology>=0)aggregate::technology=changeTechnology;
}
namespace stereo_design_capture {
unsigned recorded=0,aborted=0;
void PrepareStreamingPair(std::uint64_t){} void PrepareCertifiedPair(std::uint64_t){}
bool RecordCurrentMainTarget(std::uint64_t,unsigned){++recorded;return true;}
void SetOutputCensusEye(int,std::uint32_t){}
void AbortSerial(std::uint64_t){++aborted;}
}
#include "dlss_eye_control_r92_actual.inc"
EyeFlow Setup(int stage=4,int tech=2) {
 Reset();params={};aggregate::SetMode(true,tech);aggregate::BeginEpoch(7);
 activeHandle=New(params);PairFrame(activeHandle,params,10);
 EyeFlow f;f.epoch=7;f.frame=11;f.pipeline=0x1234;f.crv=0x2000;f.renderer=0x3000;f.designStage=stage;
 f.normal=CompleteWork();f.leftPrepared=true;f.designIpd=.064f;
 model::snapshot={f.crv,f.frame};
 g_state=Running;g_run_kind=stage+1;g_samples=17;g_pipeline_inflight=1;g_inflight=0;g_first_fault=0;g_velocity_resize_failed=false;
 g_eye_decision_used=false;g_last_run_passed=false;g_repeat_completed=0;g_last_design_stage_passed=0;g_eye_outcome=0;g_repeat_remaining=30;
 restoreOK=builtOK=readOK=normalResourceOK=true;selectorValue=0;phaseValue=2;scheduleValue=0;
 restoredBuilderCalls=rightPipelineCalls=discardLogs=0;stereo_design_capture::recorded=stereo_design_capture::aborted=0;
 rightReset=1;changeTechnology=-1;injectFallback=injectMissingWork=injectFault=injectCancelled=false;
 evalFails=false;params.reset=0;View(0,11);CHECK(Eval(activeHandle,params)==kSuccess);
 if(tech==1)fsr_fixture::Setup();
 return f;
}
void Run(EyeFlow& f){current=&f;model::snapshot={f.crv,f.frame};MaybeRunEyeControl(f);current=nullptr;}
}
int main(){
 if(ExistingDlssRoutingChecks())return 1;
 using namespace scheduler;
 const int before=checks;
 auto f=Setup();Run(f);
 CHECK(g_state==Running);CHECK(!g_eye_decision_used);CHECK(g_eye_outcome==0);CHECK(!g_last_run_passed);
 CHECK(g_repeat_completed==0);CHECK(discardLogs==1);CHECK(restoredBuilderCalls==1&&rightPipelineCalls==1);
 CHECK(!PairReady(7,11));CHECK(!aggregate::PairReady(7,11));CHECK(GetStatus().fault==Fault::None);
 CHECK(aggregate::RecoverableResetCut(7,11,0x1234));CHECK(!aggregate::RecoverableResetCut(8,11,0x1234));
 CHECK(!aggregate::RecoverableResetCut(7,12,0x1234));CHECK(!aggregate::RecoverableResetCut(7,11,0x2222));
 const auto n=evaluations.size();params.reset=0;View(0,12);CHECK(Eval(activeHandle,params)==kSuccess);
 CHECK(!aggregate::RecoverableResetCut(7,11,0x1234));f.frame=12;f.selected=false;f.eye={};rightReset=0;Run(f);
 CHECK(g_state==Running&&g_last_run_passed&&g_repeat_completed==1);CHECK(PairReady(7,12));
 CHECK(evaluations.size()>n+1&&evaluations[n].reset==1&&evaluations[n+1].reset==1);CHECK(g_last_design_stage_passed==4);
 CHECK(stereo_design_capture::recorded==0); // stage4 final-image publication remains separate
 // Independent native failures still stop; a cut receipt must not excuse them.
 f=Setup();injectFallback=true;Run(f);CHECK(g_state==Done&&!g_last_run_passed);
 f=Setup();injectMissingWork=true;Run(f);CHECK(g_state==Done&&!g_last_run_passed);
 f=Setup();restoreOK=false;Run(f);CHECK(g_state==scheduler::Faulted&&!g_last_run_passed);
 f=Setup();builtOK=false;Run(f);CHECK(g_state==scheduler::Faulted&&!g_last_run_passed);
 f=Setup();phaseValue=1;Run(f);CHECK(g_state==scheduler::Faulted&&!g_last_run_passed);
 f=Setup();scheduleValue=1;Run(f);CHECK(g_state==scheduler::Faulted&&!g_last_run_passed);
 f=Setup();injectFault=true;Run(f);CHECK(g_state==Done&&!g_last_run_passed);
 f=Setup();g_velocity_resize_failed=true;Run(f);CHECK(g_state==Done&&!g_last_run_passed);
 f=Setup();injectCancelled=true;Run(f);CHECK(g_state==Cancelled); // retry CAS never resurrects a stopped session
 f=Setup();rightReset=0;evalFails=true;Run(f);CHECK(g_state==Done&&GetStatus().fault==Fault::Evaluate);
 CHECK(!aggregate::RecoverableResetCut(7,11,0x1234));
 f=Setup(3);Run(f);CHECK(g_state==Done&&!g_last_run_passed);CHECK(stereo_design_capture::aborted==1);
 f=Setup();changeTechnology=0;Run(f);CHECK(g_state==Done&&!g_last_run_passed);
 f=Setup();aggregate::EndEpoch(7);Run(f);CHECK(g_state==Done&&!g_last_run_passed);
 f=Setup();aggregate::epochTechnology=1;Run(f);CHECK(g_state==Done&&!g_last_run_passed);
 f=Setup(4,0);Run(f);CHECK(g_state==Running&&g_last_run_passed&&g_repeat_completed==1);
 f=Setup();Run(f);SetEnabled(false);CHECK(!aggregate::RecoverableResetCut(7,11,0x1234));
 SetEnabled(true);CHECK(!aggregate::RecoverableResetCut(7,11,0x1234));
 f=Setup();Run(f);g_fault=Fault::Evaluate;CHECK(!aggregate::RecoverableResetCut(7,11,0x1234));
 f=Setup();Run(f);CHECK(ReleaseDetour(activeHandle)==kSuccess);CHECK(!aggregate::RecoverableResetCut(7,11,0x1234));
 // FSR uses the same native completion path and must retain every guard.
 f=Setup(4,1);Run(f);CHECK(g_state==Running&&!g_last_run_passed&&g_repeat_completed==0);
 CHECK(discardLogs==1&&restoredBuilderCalls==1&&rightPipelineCalls==1);
 CHECK(!aggregate::PairReady(7,11)&&aggregate::RecoverableResetCut(7,11,0x1234));
 CHECK(!aggregate::RecoverableResetCut(8,11,0x1234));CHECK(!aggregate::RecoverableResetCut(7,12,0x1234));
 CHECK(!aggregate::RecoverableResetCut(7,11,0x2222));CHECK(stereo_design_capture::recorded==0);
 const auto fsrBefore=fsr_fixture::resets.size();CHECK(fsr_fixture::Eval(0,12,7,0x1234,false)==0);
 CHECK(!aggregate::RecoverableResetCut(7,11,0x1234));f.frame=12;f.selected=false;f.eye={};rightReset=0;Run(f);
 CHECK(g_state==Running&&g_last_run_passed&&g_repeat_completed==1&&aggregate::PairReady(7,12));
 CHECK(fsr_fixture::resets[fsrBefore]&&fsr_fixture::resets[fsrBefore+1]);
 f=Setup(4,1);injectFallback=true;Run(f);CHECK(g_state==Done&&!g_last_run_passed);
 f=Setup(4,1);injectMissingWork=true;Run(f);CHECK(g_state==Done&&!g_last_run_passed);
 f=Setup(4,1);restoreOK=false;Run(f);CHECK(g_state==scheduler::Faulted&&!g_last_run_passed);
 f=Setup(4,1);g_velocity_resize_failed=true;Run(f);CHECK(g_state==Done&&!g_last_run_passed);
 f=Setup(4,1);injectFault=true;Run(f);CHECK(g_state==Done&&!g_last_run_passed);
 f=Setup(4,1);injectCancelled=true;Run(f);CHECK(g_state==Cancelled);
 f=Setup(4,1);changeTechnology=0;Run(f);CHECK(g_state==Done&&!g_last_run_passed);
 f=Setup(4,1);aggregate::EndEpoch(7);Run(f);CHECK(g_state==Done&&!g_last_run_passed);
 f=Setup(3,1);Run(f);CHECK(g_state==Done&&!g_last_run_passed);
 f=Setup(4,1);rightReset=0;fsr_fixture::fail=true;Run(f);CHECK(g_state==Done&&!g_last_run_passed);
 CHECK(!aggregate::RecoverableResetCut(7,11,0x1234));
 std::printf("R92 actual reset-cut -> aggregate -> native eye completion: checks=%d failures=%d (total=%d)\n",checks-before,failures,checks);
 return failures?1:0;
}
