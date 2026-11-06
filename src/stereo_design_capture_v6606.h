#pragma once
#include <cstdint>

// Lightweight boundary between the CPU-native stereo controller and DeviceHook's
// D3D12 implementation. Keeping D3D12 types out of the preflight translation unit
// preserves its small compile/test surface.
namespace kcd2vr::stereo_design_capture {
void PrepareCertifiedPair(std::uint64_t serial) noexcept;
// Repeated live stereo: start a new working serial without withdrawing the last
// already-certified pair from XR while the new game command list is still recording.
void PrepareStreamingPair(std::uint64_t serial) noexcept;
bool RecordCurrentMainTarget(std::uint64_t serial, int eye) noexcept;
void AbortSerial(std::uint64_t serial) noexcept;
// V66.12 F9 read-only output-owner census controls. These are no-ops unless
// the diagnostic has been explicitly armed by the main-thread key handler.
void SetOutputCensus(bool enabled) noexcept;
void SetOutputCensusEye(int eye, std::uint32_t frame) noexcept;
// V66.12: semantic KCD2 renderer owner observed at the exact native final-compose
// boundary. Primitive fields only; no D3D12 types cross into the preflight TU.
void RecordSemanticOutput(int eye, std::uint32_t frame, std::uintptr_t render_output,
                          std::uintptr_t texture_object, std::uintptr_t device_backing,
                          std::uintptr_t native_rtv_view, std::uint64_t native_rtv_key,
                          std::uint16_t width, std::uint16_t height,
                          std::uint8_t source_format, std::uint8_t device_format,
                          std::uint32_t flags, const char* name) noexcept;
// V66.12: exact final-composition helper scope.  The engine controller supplies
// the two CTexture arguments exactly as KCD2 passes them to WHGame+0x2543A6C.
// DeviceHook tags the synchronous D3D12 work issued inside that helper only.
void BeginCompositionTrace(int eye, std::uint32_t frame,
                           std::uintptr_t input_texture, std::uintptr_t input_backing, std::uintptr_t input_rtv_view,
                           std::uintptr_t output_texture, std::uintptr_t output_backing, std::uintptr_t output_rtv_view) noexcept;
void EndCompositionTrace() noexcept;
// D82: the verified RIGHT final-compose call opens this lightweight TLS scope.
// A real Draw into the observed main RTV, on its current list generation, is
// required before the temporal queue publication can consume RIGHT.
bool BeginTemporalCompositionD82(std::uint64_t epoch,std::uint32_t frame) noexcept;
void EndTemporalCompositionD82() noexcept;
void DumpOutputCensus() noexcept;
}
