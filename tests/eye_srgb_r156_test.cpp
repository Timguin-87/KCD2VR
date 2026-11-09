// R156 eye blit colour space: host checks of xr_eye_srgb_r114::blitDecodeR156 and the shader's decode (mirror).
// g++ -std=c++20 -I src tests/eye_srgb_r156_test.cpp
#include "xr_eye_srgb_r114.h"
#include <cmath>
#include <cstdio>
static unsigned checks=0,failures=0;
static void check(bool ok,const char* what){++checks;if(!ok){++failures;std::printf("FAIL %s\n",what);}}
namespace es=kcd2vr::xr_eye_srgb_r114;
static int Byte(float v){return int(std::lround(std::fmin(1.f,std::fmax(0.f,v))*255.f));}
// what the compositor reads back as display value: an sRGB swapchain is decoded once (linear), then encoded for the
// panel = the stored bytes; a UNORM swapchain is taken as linear and encoded once.
static float Shown(float stored,bool srgbSwapchain){return srgbSwapchain?stored:es::encodeSrgb(stored);}
int main(){
    // 1. the decision: only an sRGB swapchain fed from a non-sRGB (UNORM / typeless->UNORM) view decodes
    check(es::blitDecodeR156(es::kR8G8B8A8_UNORM,es::kR8G8B8A8_UNORM_SRGB),"his PC: source 28 -> swapchain 29 (R114 ON, VDXR / SteamVR) decodes");
    check(es::blitDecodeR156(es::kB8G8R8A8_UNORM,es::kB8G8R8A8_UNORM_SRGB),"BGRA 87 -> 91 decodes");
    check(es::blitDecodeR156(es::kR8G8B8A8_UNORM,es::kB8G8R8A8_UNORM_SRGB),"28 -> 91 decodes");
    check(!es::blitDecodeR156(es::kR8G8B8A8_UNORM,es::kR8G8B8A8_UNORM),"R114 OFF: 28 -> 28 unchanged (old path)");
    check(!es::blitDecodeR156(es::kR8G8B8A8_UNORM_SRGB,es::kR8G8B8A8_UNORM_SRGB),"sRGB view -> sRGB RTV already decodes on sample: no double decode");
    check(!es::blitDecodeR156(es::kR8G8B8A8_UNORM_SRGB,es::kR8G8B8A8_UNORM),"sRGB view -> UNORM RTV: linear stored, unchanged");
    check(!es::blitDecodeR156(0,0)&&!es::blitDecodeR156(28,0),"unknown formats: never decode");
    // 2. byte round trip: game byte g -> shader (raw g, decode) -> RTV encode -> stored == g (what CopyResource stores)
    for(int i=0;i<=255;++i){const float g=i/255.f;
        const float stored=es::encodeSrgb(es::decodeSrgb(g));
        check(std::abs(Byte(stored)-i)<=1,"decode+RTV encode gives the game's byte back (+-1)");
        // before R156: stored=encode(g) -> shown encode(g) > g for every mid tone (the overblown picture)
        const float before=Shown(es::encodeSrgb(g),true),after=Shown(stored,true);
        if(i>0&&i<255)check(before>g+0.01f||i>240,"before: brighter than the game's value");
        check(std::abs(after-g)<0.006f,"after: the game's value");
        // a UNORM swapchain (R114 OFF) shows encode(g) too: the same overblown picture, so R114 alone never fixed the eyes
        check(std::abs(Shown(g,false)-before)<1e-6f,"UNORM swapchain == old sRGB blit picture");
    }
    // 3. the shader formula limits: 0 and 1 are fixed points, monotonic
    check(es::decodeSrgb(0.f)==0.f&&std::abs(es::decodeSrgb(1.f)-1.f)<1e-6f,"0 and 1 fixed");
    for(int i=1;i<=255;++i)check(es::decodeSrgb(i/255.f)>es::decodeSrgb((i-1)/255.f),"monotonic");
    // 4. mid grey numbers for the review: 0.5 was shown as 0.735, now 0.5
    check(std::abs(Shown(es::encodeSrgb(.5f),true)-.735f)<.005f,"old: 0.5 -> 0.735");
    check(std::abs(Shown(es::encodeSrgb(es::decodeSrgb(.5f)),true)-.5f)<.002f,"new: 0.5 -> 0.5");
    std::printf("eye_srgb_r156_test checks=%u failures=%u\n",checks,failures);
    return failures?1:0;
}
