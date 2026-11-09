#pragma once
// R114 (Miki 6 Nov: "why does the game look better in steamvr? better colours - the openxr (VDXR) is overblown").
// the game's final image is sRGB-encoded. SteamVR "Meta compatibility mode" only offers sRGB swapchains, so the eyes
// ran on DXGI 29 (R8G8B8A8_UNORM_SRGB) and the compositor decoded the image correctly. VDXR also offers 28 (plain
// UNORM) and the old preference list took 28 first: the compositor treats a UNORM image as LINEAR light and encodes
// it once more -> washed out / overblown (R111 log 21:35:29 "R108 XR_FORMATS eye=28", R113 SteamVR log 00:37:58
// "eye=29"). eyeSrgb ON (default) = the eye swapchain prefers 29/91 when the runtime offers them, exactly the
// SteamVR path. OFF = old order (28/87 first). read at session start: takes effect at the next game start.
// INI VrEyeSrgbR114, Experiments BODY page 2.
//
// R156 (Miki 8 Nov R155 VDXR log: "I lost sRGB (not working)" - colours overblown again, eye swapchain still 29):
// the swapchain format was never the whole story. the eye image is not a CopyResource: PresentBridgePair /
// BlitAfrStereoToEyes / BlitGameImageToEyes DRAW the game's backbuffer (DXGI 28, display-encoded bytes) into the
// swapchain image: sampled through a UNORM view (raw bytes, no decode) and written through a render target view of
// the swapchain format. an *_SRGB RTV encodes linear->sRGB on write, so the already encoded bytes were encoded a
// second time; the compositor decodes once and shows the same overblown picture a UNORM swapchain gives. the HUD and
// the VR menu copy their bytes (CopyResource / CopyTextureRegion) and were right. blitDecodeR156 says when the pixel
// shader has to decode its final colour to linear (sRGB swapchain, non-sRGB source view): the RTV encode then gives
// the game's bytes back, exactly what the copies deliver. gamma/brightness/contrast (VrGamma...) still act on the
// display-encoded values as before, so the sliders keep their meaning. decodeSrgb = the shader's formula (host test).
#include <atomic>
#include <algorithm>
#include <cmath>
namespace kcd2vr::xr_eye_srgb_r114 {
inline std::atomic<bool> eyeSrgb{true};
constexpr int kR8G8B8A8_UNORM=28,kR8G8B8A8_UNORM_SRGB=29,kB8G8R8A8_UNORM=87,kB8G8R8A8_UNORM_SRGB=91; // DXGI_FORMAT values
inline bool isSrgb(int dxgi) noexcept {return dxgi==kR8G8B8A8_UNORM_SRGB||dxgi==kB8G8R8A8_UNORM_SRGB;}
// sourceView = the format the blit samples the game image with (Kcd2VrSrvFormat: typeless -> UNORM), swapchain = the
// eye swapchain / RTV format. true = decode the final colour to linear before the sRGB render target encodes it.
inline bool blitDecodeR156(int sourceView,int swapchain) noexcept {return isSrgb(swapchain)&&!isSrgb(sourceView);}
inline float decodeSrgb(float g) noexcept {g=std::clamp(g,0.f,1.f);return g<=.04045f?g/12.92f:std::pow((g+.055f)/1.055f,2.4f);}
inline float encodeSrgb(float l) noexcept {l=std::clamp(l,0.f,1.f);return l<=.0031308f?l*12.92f:1.055f*std::pow(l,1.f/2.4f)-.055f;} // what the RTV does
}
