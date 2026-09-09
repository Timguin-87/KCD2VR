#pragma once
#include <cstdint>

namespace kcd2vr {
class NativeStereoProbe {
public:
    static bool Install();
    static void Uninstall();
    // Arms exact 4-byte read/write hardware watchpoints for the three retained
    // CryEngine stereo backing floats on every process thread for a short window.
    // Diagnostic only: values are never modified.
    static void StartWindow(std::uint32_t duration_ms = 10000);
    static bool Active();
};
} // namespace kcd2vr
