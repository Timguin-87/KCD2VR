#pragma once
#include <cstdint>

namespace kcd2vr::action_map_probe {

bool Install() noexcept;
void Uninstall() noexcept;

// True after at least one exact CActionMap::Enable transition was observed for
// any Shop map.  This lets callers distinguish UNKNOWN from a proven CLOSED.

// V66.69: native APSE/Inventory ownership. Root `apse` is the strongest close
// acknowledgement; high-level pages are tracked to survive tab transitions.
bool ApseRoot() noexcept;
bool ApseMap() noexcept;

bool Alchemy() noexcept;
bool BackContext() noexcept;
bool ChatOpen59c5() noexcept; // R59.16C5: the native quick-talk (chat_interaction) map is enabled
bool NativeMinigame() noexcept;
bool WeaponQamOpen171() noexcept;
bool FoodQamOpen171() noexcept;
std::uint64_t Transitions() noexcept;

} // namespace kcd2vr::action_map_probe
