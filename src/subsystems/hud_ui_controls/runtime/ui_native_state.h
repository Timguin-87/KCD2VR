#pragma once
#include <cstdint>
#include <array>

namespace kcd2vr::ui_native_state {
// Published observations, NOT GetElement existence or cursor guesses.
enum Kind : unsigned {
    Inventory, Map, Menu, ItemSelection, ItemTransfer, AlchemyBook, Modal, Overlay, GameOver, Loading, Progress, SkipTime, CraftList, CraftContent, Shop, Crafting, Character, CodexList, InventoryInfo, MapLegend, PlayerInfo, PlayerList, QuestDiary, QuestList, ForgePlan, GeneralBook, HorseInspect, Hud, LockPicking, Pickpocketing, Count
};
static_assert(Count <= 32, "UI state masks must cover every catalogue kind");
constexpr std::uint32_t Bit(Kind k) noexcept { return k < Count ? (std::uint32_t{1} << unsigned(k)) : 0; }
struct Snapshot {
    std::uint32_t known=0, visible=0, epoch=0, installed=0;
    std::uint64_t losses=0, observations=0;
    std::array<std::uint64_t,Count> samples{};
    bool Known(Kind k) const noexcept { return (known & Bit(k)) != 0; }
    bool Open(Kind k) const noexcept { return (visible & Bit(k)) != 0; }
};
bool Install() noexcept;
void Uninstall() noexcept;
Snapshot Read() noexcept;
// Called by the existing GetElement detour after its original returns. It performs
// only validated field reads, no new native lookup or manager call. Registration
// alone is never promoted to visibility.
void ObserveElement(void* element) noexcept;
}
