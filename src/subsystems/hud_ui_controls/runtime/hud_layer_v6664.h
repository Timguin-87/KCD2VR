#pragma once
#include <cstddef>
#include <windows.h>
#include <d3d12.h>
#ifndef XR_USE_GRAPHICS_API_D3D12
#define XR_USE_GRAPHICS_API_D3D12
#endif
#ifndef XR_USE_PLATFORM_WIN32
#define XR_USE_PLATFORM_WIN32
#endif
#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>
#include <cstdint>

// V66.64 HUD LAYER.
// KCD2 draws the gameplay HUD Flash movie (incl. fader, vignette, ratio strips, popups) onto the
// main backbuffer AFTER the LEFT/RIGHT eye images were preserved. Instead of replaying draws into
// the eye images, V66.64 REDIRECTS those late backbuffer render-target binds to a private,
// transparent HUD texture (cleared to 0,0,0,0 once per frame), copies it to a small cache ring at
// the PRESENT barrier, and the XR thread shows it as one head-locked quad layer that covers
// exactly the same field of view as the eye images. Eye images and pair timing are untouched.
namespace kcd2vr::hud_layer_v6664 {

void SetDesktopMirror36(bool on) noexcept;
void SetEnabled(bool on) noexcept;       // F4; default ON
bool Enabled() noexcept;
// Manual worker-thread snapshot of atomic counters only; no GPU readback or native calls.
void WriteSnapshot() noexcept;

// ---- render thread (game command lists) ----
void OnLateWindowOpen(std::uint32_t frame) noexcept;
// Returns true when the bind was redirected (caller must NOT call the original again).
using OmSetFn = void(__stdcall*)(ID3D12GraphicsCommandList*, UINT, const D3D12_CPU_DESCRIPTOR_HANDLE*, BOOL, const D3D12_CPU_DESCRIPTOR_HANDLE*);
using MirrorDrawFn36 = void(__stdcall*)(ID3D12GraphicsCommandList*,UINT,UINT,UINT,UINT);
using BarrierFn = void(__stdcall*)(ID3D12GraphicsCommandList*, UINT, const D3D12_RESOURCE_BARRIER*);
bool RedirectMainBind(ID3D12Device* device, ID3D12GraphicsCommandList* list, ID3D12Resource* main,
                      const D3D12_CPU_DESCRIPTOR_HANDLE* dsv, OmSetFn original,const D3D12_CPU_DESCRIPTOR_HANDLE* desktopRtv=nullptr) noexcept;
void OnLateDraw(bool indexed, UINT count, UINT instances, ID3D12GraphicsCommandList* list=nullptr) noexcept;   // only while redirected; R111 counts draws on the redirected list
void OnPresentBarrier(ID3D12GraphicsCommandList* list, BarrierFn original_barrier,ID3D12Resource* main=nullptr,D3D12_RESOURCE_STATES before=D3D12_RESOURCE_STATE_COMMON,OmSetFn original_bind=nullptr,MirrorDrawFn36 original_draw=nullptr) noexcept;
bool RedirectActiveThisFrame() noexcept;
void OnSubmittedBatch(UINT count, ID3D12CommandList* const* lists) noexcept;

// ---- XR thread ----
struct XrHudFrame { bool ok=false; XrSwapchain swapchain=XR_NULL_HANDLE; int32_t width=0, height=0; };
// Acquire/copy/release the HUD swapchain image. Fresh = published within 250 ms.
XrHudFrame XrPrepare(XrSession session, ID3D12Device* device, ID3D12CommandQueue* queue) noexcept;
// R104: one-line HUD swapchain/copy counters for the always-on R104 XR_LAYERS summary.
void StatusR104(char* out,std::size_t n) noexcept;
// R111: counters for the always-on R111 XR_UI_LAYERS row (captures without draws skipped, same capture re-copied).
void StatusR111(std::uint64_t* noDrawSkips,std::uint64_t* recopies,std::uint64_t* published,std::uint64_t* shown) noexcept;
void XrShutdown() noexcept;

// V66.80: production presentation returns to a dedicated OpenXR HUD quad. This compatibility
// accessor remains for archaeology only; the eye blit no longer calls it.
ID3D12Resource* AcquirePublishedForBlit(std::uint64_t max_age_ms) noexcept;

} // namespace kcd2vr::hud_layer_v6664
