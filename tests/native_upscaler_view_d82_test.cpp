#include "native_stereo_preflight_v6602.h"
#include <cstdint>
#include <cstdio>
#include <thread>
// R149: alternate-eye identity owner. Production FrameSync resolves it from the
// exact publication-ring match inside the Normal pipeline execute.
namespace kcd2vr {
struct FrameSync {
    static inline bool aerActive=false;
    static inline std::uint64_t aerEpoch=5;static inline std::uint32_t aerFrame=77,aerEye=1;
    static inline std::uintptr_t aerPipeline=0x3000;
    static bool AlternateEyeUpscalerIdentity(std::uint64_t* epoch,std::uint32_t* frame,std::uint32_t* eye,std::uintptr_t* pipeline) noexcept {
        if(!aerActive)return false;
        if(epoch)*epoch=aerEpoch;if(frame)*frame=aerFrame;if(eye)*eye=aerEye;if(pipeline)*pipeline=aerPipeline;
        return true;
    }
};
}
namespace kcd2vr::native_stereo_preflight { namespace {
enum class ControlPass {Normal,Eye1,Restore};
struct Work {unsigned buildReturns=1;bool builtPointerValid=true,cameraSourceValid=true,standardStateValid=true;};
struct EyeFlow {
    std::uintptr_t crv=0x1000,pipeline=0x2000;
    std::uint64_t epoch=42;std::uint32_t frame=9;
    ControlPass pass=ControlPass::Normal;Work normal{},eye{};
    bool leftPrepared=true,selected=true,fallback=false;int designStage=4;
};
thread_local EyeFlow* t_eye_flow=nullptr;
bool IsStereoDesign(const EyeFlow& f) noexcept {return f.designStage>=1&&f.designStage<=4;}
unsigned g_read=0;std::uint32_t testFrame=9,testSelector=0;std::uintptr_t testPipeline=0x2000;bool testRead=true;
namespace model {
template<class T> bool Read(unsigned,std::uintptr_t base,std::uintptr_t offset,T& out) noexcept {
    if(!testRead||base!=0x1000)return false;
    if(offset==0x2c)out=static_cast<T>(testFrame);
    else if(offset==0x8ee0)out=static_cast<T>(testSelector);
    else if(offset==0x8d60)out=static_cast<T>(testPipeline);
    else return false;
    return true;
}
}
} // namespace
#include "native_upscaler_view_d82_actual.inc"
}
using namespace kcd2vr::native_stereo_preflight;
int main() {
    int checks=0,failures=0;
    auto check=[&](bool v){++checks;if(!v){++failures;std::fprintf(stderr,"view check %d FAILED\n",checks);}};
    UpscalerView out{};check(!InUpscalerView());check(!CurrentUpscalerView(out));
    EyeFlow f;t_eye_flow=&f;check(InUpscalerView());check(CurrentUpscalerView(out));check(out.eye==0&&out.epoch==42&&out.frame==9&&out.pipeline==0x2000);
    f.pass=ControlPass::Eye1;testSelector=1;check(CurrentUpscalerView(out));check(out.eye==1);
    f.selected=false;check(!CurrentUpscalerView(out));f.selected=true;
    f.fallback=true;check(!CurrentUpscalerView(out));f.fallback=false;
    testFrame=10;check(!CurrentUpscalerView(out));testFrame=9;
    testSelector=0;check(!CurrentUpscalerView(out));testSelector=1;
    testPipeline=0x2222;check(!CurrentUpscalerView(out));testPipeline=0x2000;
    f.eye.standardStateValid=false;check(!CurrentUpscalerView(out));f.eye.standardStateValid=true;
    f.eye.buildReturns=2;check(!CurrentUpscalerView(out));f.eye.buildReturns=1;
    f.eye.cameraSourceValid=false;check(!CurrentUpscalerView(out));f.eye.cameraSourceValid=true;
    f.eye.builtPointerValid=false;check(!CurrentUpscalerView(out));f.eye.builtPointerValid=true;
    testRead=false;check(!CurrentUpscalerView(out));testRead=true;
    f.pass=ControlPass::Restore;check(!InUpscalerView());check(!CurrentUpscalerView(out));
    f.pass=ControlPass::Normal;testSelector=0;f.leftPrepared=false;check(!CurrentUpscalerView(out));f.leftPrepared=true;
    f.designStage=0;check(!InUpscalerView());f.designStage=4;
    bool foreign=false;std::thread other([&]{UpscalerView v;foreign=InUpscalerView()||CurrentUpscalerView(v);});other.join();check(!foreign);check(CurrentUpscalerView(out));
    // R149 AFR: only FrameSync's exact render identity opens the scope, also
    // while a non-design eye flow object is present on the thread.
    using kcd2vr::FrameSync;
    t_eye_flow=nullptr;check(!InUpscalerView());check(!CurrentUpscalerView(out));
    FrameSync::aerActive=true;check(InUpscalerView());check(CurrentUpscalerView(out));
    check(out.epoch==5&&out.frame==77&&out.eye==1&&out.pipeline==0x3000);
    FrameSync::aerEye=0;check(CurrentUpscalerView(out)&&out.eye==0);
    f.designStage=0;t_eye_flow=&f;check(InUpscalerView());check(CurrentUpscalerView(out)&&out.frame==77&&out.pipeline==0x3000);
    FrameSync::aerFrame=0;check(!CurrentUpscalerView(out));FrameSync::aerFrame=77;
    FrameSync::aerActive=false;check(!InUpscalerView());check(!CurrentUpscalerView(out));
    f.designStage=4;check(CurrentUpscalerView(out)&&out.eye==0&&out.frame==9);
    std::printf("Native TLS view checks=%d failures=%d\n",checks,failures);return failures?1:0;
}
