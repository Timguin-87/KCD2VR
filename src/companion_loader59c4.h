#pragma once
namespace kcd2vr::companion_loader59c4 {
// Called at the beginning of the ASI worker, before config/XR/device waits.
// Read-only, exact-retail startup observer. Reports through the shared KCD2VR.log sink.
bool Install() noexcept;
void Uninstall() noexcept;
}
