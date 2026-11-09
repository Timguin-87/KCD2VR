#pragma once
// R141 (Miki 7 Nov: "lockpicking overlay was nearly not visible (Transparent)").
// the HUD layer (V66.64) catches the game's late Flash draws on a texture cleared to 0,0,0,0. the game blends every
// Flash shape with SRC_ALPHA / INV_SRC_ALPHA on colour AND alpha (CryEngine maps the alpha factors from the colour
// factors), so one shape with alpha a leaves colour c*a but alpha a*a in our texture. on the flat screen the alpha
// channel is never looked at; on the headset the compositor uses it as coverage: a lock drawn at 50% covers only 25%,
// and with the R114 sRGB HUD swapchain its premultiplied colour is also decoded once more (0.5*c -> ~0.22*c). solid
// pixels (a=1) and empty pixels (a=0) are untouched by both, that's why the normal HUD looks fine and the lockpick
// lock (big half-transparent rings over the world) almost disappears.
// fix: ONLY while the LockPicking movie is open the XR thread does not CopyResource the HUD capture into the HUD
// swapchain image, it draws it through Fix() below: alpha = sqrt(captured alpha) (= the a the game drew with),
// colour un-premultiplied by that a and premultiplied again in linear light when the swapchain is sRGB. a=0 and a=1
// pixels come out byte-identical to the copy. every other moment (all other HUD, menus, dialogs, pickpocketing) keeps
// the old CopyResource path. INI VrLockpickOpaqueR141 (default true), Experiments GEAR page 3. no latch: decided every
// XR frame; a failed pass setup is retried after 2 s and the copy is used meanwhile.
#include <algorithm>
#include <atomic>
#include <cmath>
namespace kcd2vr::hud_lock_alpha_r141 {
inline std::atomic<bool> enabled{true};   // VrLockpickOpaqueR141
inline std::atomic<bool> lockOpen{false}; // UI worker: native LockPicking movie is known and open (ui_native_state)

inline bool Active() noexcept {
    return enabled.load(std::memory_order_acquire)&&lockOpen.load(std::memory_order_acquire);
}
// host mirror of the pixel shader below (tests/hud_lock_alpha_r141_test.cpp). in/out 0..1. srgb = the HUD
// swapchain is *_SRGB: the returned rgb is LINEAR (the render target view encodes it); else rgb is stored as is.
struct Px {float r=0,g=0,b=0,a=0;};
inline float DecodeSrgb(float g) noexcept {g=std::clamp(g,0.f,1.f);return g<=.04045f?g/12.92f:std::pow((g+.055f)/1.055f,2.4f);}
inline Px Fix(Px v,bool srgb) noexcept {
    const float A=std::clamp(v.a,0.f,1.f);
    const float a=std::sqrt(A);
    float c[3]{std::clamp(v.r,0.f,1.f),std::clamp(v.g,0.f,1.f),std::clamp(v.b,0.f,1.f)};
    if(a>0.f&&a<1.f)for(float& x:c)x=std::min(1.f,x/a);   // un-premultiply by the drawn alpha
    Px o{};o.a=a;
    for(int i=0;i<3;++i){
        const float k=(a>0.f&&a<1.f)?a:1.f;                    // a=0 (additive light) / a=1 keep their colour
        const float lin=srgb?DecodeSrgb(c[i]):c[i];
        (&o.r)[i]=lin*k;
    }
    return o;
}
inline const char* ShaderR141() noexcept {
    return
    "Texture2D<float4> t:register(t0);cbuffer K:register(b0){uint srgb;};"
    "float4 vs(uint i:SV_VertexID):SV_Position{float2 u=float2((i<<1)&2,i&2);return float4(u.x*2-1,1-u.y*2,0,1);}"
    "float3 Dec(float3 g){g=saturate(g);return g<=0.04045?g/12.92:pow((g+0.055)/1.055,2.4);}"
    "float4 ps(float4 p:SV_Position):SV_Target{float4 v=saturate(t.Load(int3(p.xy,0)));float a=sqrt(v.a);"
    "bool mid=a>0&&a<1;float3 c=mid?min(1,v.rgb/a):v.rgb;float k=mid?a:1;"
    "float3 o=(srgb!=0?Dec(c):c)*k;return float4(o,a);}";
}
}
