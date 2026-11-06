#include "native_stereo_preflight_v6602.h"
#include <cstdint>
#include <cstdio>
#include <thread>
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
    std::printf("Native TLS view checks=%d failures=%d\n",checks,failures);return failures?1:0;
}
