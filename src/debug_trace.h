#pragma once

#include <cstdint>

namespace kcd2vr::debug_trace {

// Crash/debug-friendly fallback trace. Each call opens the file in append mode,
// writes one line, flushes the OS file buffers, and closes the handle. This is
// intentionally heavier than the normal logger but is only used for sparse
// milestones/counters while diagnosing startup and XR tracking.
void Write(const char* message) noexcept;
void WriteCounter(const char* label, std::uint64_t value) noexcept;

} // namespace kcd2vr::debug_trace
