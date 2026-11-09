#pragma once
#include <d3d12.h>
#include <cstdint>
#include <string>

// R149 alternate-eye frame dump, XR composition side. The DLSS side records the
// live eye frames inside NGX evaluation (native_dlss_aer_dump_d82.inl); XrManager
// adds the post-DLSS image it caches for each eye and the two warped eye images it
// submits, using its own synchronous command list. Nothing here runs unless a dump
// was requested (Ctrl+F12 or the VR menu DUMP button).
namespace kcd2vr::native_dlss_stereo_d82 {
struct AerDumpXrCapture;
// Cheap atomic check: true only while the DLSS side has started capturing and
// the XR quota for this dump is not yet filled.
bool AerDumpXrWanted() noexcept;
AerDumpXrCapture* AerDumpXrBegin(ID3D12Device* device) noexcept;
// Records barrier(state -> COPY_SOURCE), copy into a readback buffer and the
// restoring barrier on the caller's open command list.
bool AerDumpXrCopy(AerDumpXrCapture* capture, ID3D12GraphicsCommandList* list,
                   ID3D12Resource* resource, D3D12_RESOURCE_STATES state, const char* name) noexcept;
// Metadata object body for this blit (JSON object text, without braces).
void AerDumpXrSetJson(AerDumpXrCapture* capture, std::string json) noexcept;
// Call exactly once per Begin. executed=true only after the command list that
// carries the copies has completed on the GPU (XrManager waits its fence).
void AerDumpXrCommit(AerDumpXrCapture* capture, bool executed, bool newFrame) noexcept;
}
