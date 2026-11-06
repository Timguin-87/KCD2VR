#pragma once
// Explicit host-only native fault leaf. This does not emulate Windows SEH,
// access violations, game memory or an engine scheduler.
#include <cstdint>
#include <stdexcept>
#define __fastcall
#define __try try
#define __except(x) catch(...)
#define EXCEPTION_EXECUTE_HANDLER 1
constexpr std::uint32_t EXCEPTION_ACCESS_VIOLATION=0xc0000005u;
[[noreturn]] inline void RaiseException(std::uint32_t,unsigned,unsigned,const void*) {
    throw std::runtime_error("explicit native status leaf failure");
}
