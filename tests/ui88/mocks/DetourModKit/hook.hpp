#pragma once
// Syntax-only stand-in. No detour is installed or validated by this mock.
#include <cstdint>
#include <optional>
namespace DetourModKit {
struct Address{std::uintptr_t value;};
namespace hook {
struct Target{Address value;};struct Options{bool fail_if_already_hooked=false;};
struct InlineRequest{const char* name;Target target;Options options;};
struct Hook{template<class F>F original(){return nullptr;}bool enable(){return false;}bool disable(){return true;}};
template<class F>std::optional<Hook> inline_at(InlineRequest,F){return std::nullopt;}
}}

namespace DetourModKit::hook {
// Host-only synthetic register bank, NOT the opaque production DMK layout.
enum class Gpr {Rdi,R12};
struct MidContext{std::uintptr_t registers[2]{};};
inline std::uintptr_t& gpr(MidContext& c,Gpr r) noexcept{return c.registers[static_cast<unsigned>(r)];}
using MidRequest=InlineRequest;
template<class F>std::optional<Hook> mid_at(MidRequest r,F f){return inline_at(r,f);}
}
