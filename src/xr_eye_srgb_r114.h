#pragma once
// R114 (Miki 6 Nov: "why does the game look better in steamvr? better colours - the openxr (VDXR) is overblown").
// the game's final image is sRGB-encoded. SteamVR "Meta compatibility mode" only offers sRGB swapchains, so the eyes
// ran on DXGI 29 (R8G8B8A8_UNORM_SRGB) and the compositor decoded the image correctly. VDXR also offers 28 (plain
// UNORM) and the old preference list took 28 first: the compositor treats a UNORM image as LINEAR light and encodes
// it once more -> washed out / overblown (R111 log 21:35:29 "R108 XR_FORMATS eye=28", R113 SteamVR log 00:37:58
// "eye=29"). eyeSrgb ON (default) = the eye swapchain prefers 29/91 when the runtime offers them, exactly the
// SteamVR path. OFF = old order (28/87 first). read at session start: takes effect at the next game start.
// INI VrEyeSrgbR114, Experiments BODY page 2.
#include <atomic>
namespace kcd2vr::xr_eye_srgb_r114 {
inline std::atomic<bool> eyeSrgb{true};
}
