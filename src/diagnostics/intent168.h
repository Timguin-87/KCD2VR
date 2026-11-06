#pragma once
#include <cstdint>
namespace kcd2vr::diagnostics::intent168 {
enum class Source : unsigned {None, R3, Slash, Thrust, Pommel};
struct Context {std::uint64_t serial=0;std::uintptr_t actor=0;Source source=Source::None;};
inline thread_local Context current{};
struct Scope {
    Context old;explicit Scope(Context c) noexcept:old(current){current=c;}
    ~Scope(){current=old;}
    Scope(const Scope&)=delete;Scope& operator=(const Scope&)=delete;
};
// This is synchronous diagnostic provenance, NEVER a damage/admission ticket.
}
