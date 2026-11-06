#pragma once
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include "player_identity37.h"

namespace kcd2vr::door_candidates38 {
// Qualify the CEntity identity captured before this committed packet was built.
// A late current-player sample cannot relabel a packet for another character.
inline std::uintptr_t framePlayer(std::uint64_t before,std::uint64_t after,
    std::uint64_t epoch,std::uint64_t currentEpoch,std::uintptr_t frameCharacter,
    std::uintptr_t qualifiedCharacter,std::uintptr_t entity,std::uint64_t leaseTick,
    std::uint64_t now) noexcept {
    return !(before&1u)&&before==after&&epoch&&epoch==currentEpoch&&entity&&
        player_identity37::accepts(qualifiedCharacter,leaseTick,frameCharacter,now)?entity:0;
}
// Copies of the native projection observations, keyed by exact target AND hand.
// These are geometry witnesses only. A successful native ordinary-door action
// must still admit the request. The general nearest-hover candidate is separate.
template<class Candidate, std::size_t Capacity = 16>
struct Records {
    static_assert(Capacity > 0, "A door witness cache needs at least one slot");
    std::array<Candidate, Capacity> entries{};
    std::size_t next = 0;
    void clear() noexcept { entries = {}; next = 0; }
    void withdraw(std::uint32_t target) noexcept {
        if (!target) return;
        for (auto& entry : entries) if (entry.entity == target) entry = {};
    }
    void record(const Candidate& value) noexcept {
        if (!value.entity) return;
        if (!value.tick || !value.object || value.owner < 0 || value.owner > 1 ||
            std::strcmp(value.cls, "AnimDoor") || !std::isfinite(value.distance) ||
            value.distance < 0 || !std::isfinite(value.x) ||
            !std::isfinite(value.y) || !std::isfinite(value.z)) {
            withdraw(value.entity); return;
        }
        for (auto& entry : entries) {
            if (entry.entity == value.entity && entry.owner == value.owner) {
                entry = value; return;
            }
        }
        entries[next] = value;
        next = (next + 1) % Capacity;
    }
    bool find(std::uint32_t target, int side, std::uint64_t now, Candidate& out) const noexcept {
        out = {};
        if (!target || side < 0 || side > 1 || !now) return false;
        for (const auto& entry : entries) {
            if (entry.entity == target && entry.owner == side && entry.tick &&
                now >= entry.tick && now - entry.tick <= 80) {
                out = entry; return true;
            }
        }
        return false;
    }
};
}
