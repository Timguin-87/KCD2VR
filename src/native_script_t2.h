#pragma once
#include <cstddef>
#include <cstdint>
namespace kcd2vr::native_script_t2 {
// Valid only during one registered original-first native script-update callback.
// Neither state nor system is an ownership grant outside that callback.
struct Context {
    std::uintptr_t system=0,state=0;
    std::uint64_t now=0,generation=0;
    bool Current() const noexcept; // exact original-first same-stack capability
    bool CallPacket(const char* ownGlobal,const char* input,char* output,std::size_t capacity) const noexcept;
};
using Client=void(*)(const Context&) noexcept;
using Wanted=bool(*)() noexcept; // atomic intent only; no native/VM calls here.
// At most twelve fixed own clients (R158: was ten, all ten were taken) (D72 saved-body drag and D73 opt-in NPC life). Idempotent; independent domain enable gates.
bool Register(Client,Wanted=nullptr) noexcept;
bool Install() noexcept;
// Disable clients without tearing down an in-flight native trampoline. The
// installed module is process-pinned before enabling this retained hook.
void Shutdown() noexcept;
}
