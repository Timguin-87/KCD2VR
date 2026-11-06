#pragma once
#include <cstdint>
namespace kcd2vr::quick_talk_native_h19 {
struct Snapshot { bool ready=false,known=false,enabled=false;std::uint64_t revision=0; };
// Exact native chat_interaction map transitions, never GetActionMap polling or
// an inference from the opener button/gesture. Unknown is deliberately closed.
bool Install() noexcept;
void Uninstall() noexcept;
Snapshot Read() noexcept;
inline bool Open() noexcept {const auto s=Read();return s.ready&&s.known&&s.enabled;}
}
