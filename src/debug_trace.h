#pragma once
#include <cstdint>
namespace kcd2vr::debug_trace {
inline void Write(const char*) noexcept {}
inline void WriteCounter(const char*, std::uint64_t) noexcept {}
}
