#pragma once
#include <cstdint>
namespace kcd2vr::retail_build112 {
// Disk I/O is restricted to the explicit control-thread activation request.
bool Verify(std::uintptr_t pinnedModule) noexcept;
bool Ready(std::uintptr_t pinnedModule) noexcept;
}
