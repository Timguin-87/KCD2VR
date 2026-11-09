// R141 lockpick HUD alpha: host checks of hud_lock_alpha_r141::Fix (the C++ mirror of the pixel shader).
// g++ -std=c++20 -I src tests/hud_lock_alpha_r141_test.cpp
#include "hud_lock_alpha_r141.h"
#include <cmath>
#include <cstdio>
#include <cstring>
static unsigned checks=0,failures=0;
static void check(bool ok,const char* what){++checks;if(!ok){++failures;std::printf("FAIL %s\n",what);}}
namespace la=kcd2vr::hud_lock_alpha_r141;
static float Enc(float l){l=std::fmin(1.f,std::fmax(0.f,l));return l<=.0031308f?l*12.92f:1.055f*std::pow(l,1.f/2.4f)-.055f;}
static int Byte(float v){return int(std::lround(std::fmin(1.f,std::fmax(0.f,v))*255.f));}
// what lands in the swapchain image: an sRGB RTV encodes the shader output, a UNORM RTV stores it
static la::Px Stored(la::Px o,bool srgb){if(!srgb)return o;return {Enc(o.r),Enc(o.g),Enc(o.b),o.a};}
static bool SameBytes(la::Px a,la::Px b){return Byte(a.r)==Byte(b.r)&&Byte(a.g)==Byte(b.g)&&Byte(a.b)==Byte(b.b)&&Byte(a.a)==Byte(b.a);}
int main(){
    // 1. solid and empty pixels: byte-identical to the old CopyResource (both swapchain kinds)
    for(int s=0;s<2;++s){const bool srgb=s==1;
        for(int i=0;i<=255;i+=5){const float g=i/255.f;
            const float h=float(i/2)/255.f; // byte-exact inputs like the capture
            la::Px solid{g,1.f-g,h,1.f},empty{0,0,0,0},glow{g,h,0,0};
            check(SameBytes(Stored(la::Fix(solid,srgb),srgb),solid),"a=1 unchanged");
            check(SameBytes(Stored(la::Fix(empty,srgb),srgb),empty),"a=0 black unchanged");
            check(SameBytes(Stored(la::Fix(glow,srgb),srgb),glow),"a=0 additive light unchanged");
        }
    }
    // 2. one shape drawn with alpha a onto the cleared texture leaves colour c*a, alpha a*a -> alpha back to a
    for(float a=.05f;a<.999f;a+=.05f){const float c=.8f;
        const la::Px cap{c*a,c*a,c*a,a*a};
        const auto u=la::Fix(cap,false);
        check(std::fabs(u.a-a)<1e-4f,"unorm: alpha = drawn alpha");
        check(std::fabs(u.r-c*a)<1e-4f,"unorm: colour stays c*a (premultiplied, gamma blend like the flat screen)");
        const auto l=la::Fix(cap,true);
        check(std::fabs(l.a-a)<1e-4f,"srgb: alpha = drawn alpha");
        check(std::fabs(l.r-a*la::DecodeSrgb(c))<1e-4f,"srgb: linear premultiplied a*decode(c)");
        // coverage of the world behind: flat 1-a, old headset 1-a*a, now 1-a
        check(1.f-l.a<1.f-cap.a||a<.06f,"world shows through less than before");
    }
    // 3. the worked example from the review: lock ring white at 50%
    {const la::Px cap{.5f,.5f,.5f,.25f};const auto o=la::Fix(cap,true);
     check(std::fabs(o.a-.5f)<1e-4f,"50% ring covers 50% (was 25%)");
     check(o.r>.49f&&o.r<.51f,"50% white ring adds 0.5 linear white (old copy: decode(0.5)=0.21)");}
    // 4. gate: both the switch and the open LockPicking movie
    la::enabled=true;la::lockOpen=false;check(!la::Active(),"closed lock: pass off");
    la::lockOpen=true;check(la::Active(),"open lock + ON: pass on");
    la::enabled=false;check(!la::Active(),"switch OFF: pass off");
    la::enabled=true;la::lockOpen=false;check(!la::Active(),"closed again: off (no latch)");
    // 5. shader text and C++ mirror use the same pieces
    const char* s=la::ShaderR141();
    check(std::strstr(s,"sqrt(v.a)")!=nullptr&&std::strstr(s,"v.rgb/a")!=nullptr&&std::strstr(s,"0.04045")!=nullptr,"shader = Fix");
    std::printf("hud_lock_alpha_r141_test checks=%u failures=%u\n",checks,failures);
    return failures?1:0;
}
