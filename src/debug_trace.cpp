#include "debug_trace.h"
namespace kcd2vr::debug_trace {
// V61.36 CLEAN: high-frequency fallback trace is compiled to a no-op.
// Normal structured logger output remains available for sparse startup/errors.
void Write(const char*) noexcept {}
void WriteCounter(const char*, std::uint64_t) noexcept {}
}
